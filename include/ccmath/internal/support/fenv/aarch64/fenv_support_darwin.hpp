/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// Code borrowed from LLVM with heavy modifications done for ccmath to allow for both cross-platform and cross-compiler support.
// https://github.com/llvm/llvm-project/

#pragma once

#include "ccmath/internal/config/arch/targets/aarch64.hpp" // CCM_TARGET_ARCH_AARCH64

#if defined(CCM_TARGET_ARCH_AARCH64) && defined(__APPLE__)


#include "ccmath/internal/support/fp_bits.hpp"

#include <arm_acle.h>
#include <cfenv>
#include <cstdint>

namespace ccm::support::fenv::internal
{

	struct FEnv
	{
		struct FPState
		{
			std::uint64_t StatusWord;
			std::uint64_t ControlWord;
		};

		static_assert(sizeof(std::fenv_t) == sizeof(FPState), "Internal floating point state does not match the public std::fenv_t type.");

		static constexpr std::uint32_t TONEAREST  = 0x0;
		static constexpr std::uint32_t UPWARD	  = 0x1;
		static constexpr std::uint32_t DOWNWARD	  = 0x2;
		static constexpr std::uint32_t TOWARDZERO = 0x3;

		// These will be the exception flags we use for exception values normalized
		// from both status word and control word.
		// We add EX_ prefix to the names since macOS <math.h> defines OVERFLOW and
		// UNDERFLOW macros.
		static constexpr std::uint32_t EX_INVALID	= 0x1;
		static constexpr std::uint32_t EX_DIVBYZERO = 0x2;
		static constexpr std::uint32_t EX_OVERFLOW	= 0x4;
		static constexpr std::uint32_t EX_UNDERFLOW = 0x8;
		static constexpr std::uint32_t EX_INEXACT	= 0x10;
		// __APPLE__ ARM64 has an extra flag that is raised when a denormal is flushed
		// to zero.
		static constexpr std::uint32_t EX_FLUSHTOZERO = 0x20;

		// Zero-th bit is the first bit.
		static constexpr std::uint32_t ROUNDING_CONTROL_BIT_POSITION = 22;

		// In addition to the 5 floating point exceptions, macOS on arm64 defines
		// another floating point exception: FE_FLUSHTOZERO, which is controlled by
		// __fpcr_flush_to_zero bit in the FPCR register.  This control bit is
		// located in a different place from FE_FLUSHTOZERO status bit relative to
		// the other exceptions.
		inline static std::uint32_t exception_value_from_status(int status)
		{
			return (((status & FE_INVALID) != 0) ? EX_INVALID : 0) | (((status & FE_DIVBYZERO) != 0) ? EX_DIVBYZERO : 0) |
				   (((status & FE_OVERFLOW) != 0) ? EX_OVERFLOW : 0) | (((status & FE_UNDERFLOW) != 0) ? EX_UNDERFLOW : 0) |
				   (((status & FE_INEXACT) != 0) ? EX_INEXACT : 0) | (((status & FE_FLUSHTOZERO) != 0) ? EX_FLUSHTOZERO : 0);
		}

		inline static std::uint32_t exception_value_from_control(int control)
		{
			return ((control & __fpcr_trap_invalid) ? EX_INVALID : 0) | ((control & __fpcr_trap_divbyzero) ? EX_DIVBYZERO : 0) |
				   ((control & __fpcr_trap_overflow) ? EX_OVERFLOW : 0) | ((control & __fpcr_trap_underflow) ? EX_UNDERFLOW : 0) |
				   ((control & __fpcr_trap_inexact) ? EX_INEXACT : 0) | ((control & __fpcr_flush_to_zero) ? EX_FLUSHTOZERO : 0);
		}

		inline static int exception_value_to_status(std::uint32_t excepts)
		{
			return ((excepts & EX_INVALID) != 0U ? FE_INVALID : 0) | ((excepts & EX_DIVBYZERO) != 0U ? FE_DIVBYZERO : 0) |
				   ((excepts & EX_OVERFLOW) != 0U ? FE_OVERFLOW : 0) | ((excepts & EX_UNDERFLOW) != 0U ? FE_UNDERFLOW : 0) |
				   ((excepts & EX_INEXACT) != 0U ? FE_INEXACT : 0) | ((excepts & EX_FLUSHTOZERO) != 0U ? FE_FLUSHTOZERO : 0);
		}

		inline static int exception_value_to_control(std::uint32_t excepts)
		{
			return (((excepts & EX_INVALID) != 0U) ? __fpcr_trap_invalid : 0) | ((excepts & EX_DIVBYZERO) != 0U ? __fpcr_trap_divbyzero : 0) |
				   ((excepts & EX_OVERFLOW) != 0U ? __fpcr_trap_overflow : 0) | ((excepts & EX_UNDERFLOW) != 0U ? __fpcr_trap_underflow : 0) |
				   ((excepts & EX_INEXACT) != 0U ? __fpcr_trap_inexact : 0) | ((excepts & EX_FLUSHTOZERO) != 0U ? __fpcr_flush_to_zero : 0);
		}

		inline static std::uint32_t get_control_word() { return __arm_rsr("fpcr"); }

		inline static void set_control_word(std::uint32_t fpcr) { __arm_wsr("fpcr", fpcr); }

		inline static std::uint32_t get_status_word() { return __arm_rsr("fpsr"); }

		inline static void set_status_word(std::uint32_t fpsr) { __arm_wsr("fpsr", fpsr); }
	};

	inline int enable_except(int excepts)
	{
		std::uint32_t const new_excepts = FEnv::exception_value_from_status(excepts);
		std::uint32_t control_word		= FEnv::get_control_word();
		std::uint32_t const old_excepts = FEnv::exception_value_from_control(static_cast<int>(control_word));
		if (new_excepts != old_excepts)
		{
			control_word |= static_cast<std::uint32_t>(FEnv::exception_value_to_control(new_excepts));
			FEnv::set_control_word(control_word);
		}
		return FEnv::exception_value_to_status(old_excepts);
	}

	inline int disable_except(int excepts)
	{
		std::uint32_t const disabled_excepts = FEnv::exception_value_from_status(excepts);
		std::uint32_t control_word			 = FEnv::get_control_word();
		std::uint32_t const old_excepts		 = FEnv::exception_value_from_control(static_cast<int>(control_word));
		control_word &= static_cast<std::uint32_t>(~FEnv::exception_value_to_control(disabled_excepts));
		FEnv::set_control_word(control_word);
		return FEnv::exception_value_to_status(old_excepts);
	}

	inline int get_except()
	{
		std::uint32_t const control_word	= FEnv::get_control_word();
		std::uint32_t const enabled_excepts = FEnv::exception_value_from_control(static_cast<int>(control_word));
		return FEnv::exception_value_to_status(enabled_excepts);
	}

	inline int clear_except(int excepts)
	{
		std::uint32_t status_word		 = FEnv::get_status_word();
		std::uint32_t const except_value = FEnv::exception_value_from_status(excepts);
		status_word &= static_cast<std::uint32_t>(~FEnv::exception_value_to_status(except_value));
		FEnv::set_status_word(status_word);
		return 0;
	}

	inline int test_except(int excepts)
	{
		std::uint32_t const statusWord = FEnv::get_status_word();
		std::uint32_t const ex_value   = FEnv::exception_value_from_status(excepts);
		return static_cast<int>(statusWord & static_cast<std::uint32_t>(FEnv::exception_value_to_status(ex_value)));
	}

	inline int set_except(int excepts)
	{
		std::uint32_t status_word		   = FEnv::get_status_word();
		std::uint32_t const new_exceptions = FEnv::exception_value_from_status(excepts);
		status_word |= static_cast<std::uint32_t>(FEnv::exception_value_to_status(new_exceptions));
		FEnv::set_status_word(status_word);
		return 0;
	}

	inline int raise_except(int excepts)
	{
		float const zero		= 0.0F;
		float const one			= 1.0F;
		float const large_value = FPBits<float>::max_normal().get_val();
		float const small_value = FPBits<float>::min_normal().get_val();
		auto divfunc			= [](float a, float b)
		{
			__asm__ __volatile__("ldr  s0, %0\n\t"
								 "ldr  s1, %1\n\t"
								 "fdiv s0, s0, s1\n\t"
								 : // No outputs
								 : "m"(a), "m"(b)
								 : "s0", "s1" /* s0 and s1 are clobbered */);
		};

		std::uint32_t const to_raise = FEnv::exception_value_from_status(excepts);
		int result					 = 0;

		if ((to_raise & FEnv::EX_INVALID) != 0U)
		{
			divfunc(zero, zero);
			std::uint32_t const status_word = FEnv::get_status_word();
			if ((FEnv::exception_value_from_status(static_cast<int>(status_word)) & FEnv::EX_INVALID) == 0U) { result = -1; }
		}

		if ((to_raise & FEnv::EX_DIVBYZERO) != 0U)
		{
			divfunc(one, zero);
			std::uint32_t const status_word = FEnv::get_status_word();
			if ((FEnv::exception_value_from_status(static_cast<int>(status_word)) & FEnv::EX_DIVBYZERO) == 0u) { result = -1; }
		}
		if ((to_raise & FEnv::EX_OVERFLOW) != 0U)
		{
			divfunc(large_value, small_value);
			std::uint32_t const status_word = FEnv::get_status_word();
			if ((FEnv::exception_value_from_status(static_cast<int>(status_word)) & FEnv::EX_OVERFLOW) == 0U) { result = -1; }
		}
		if ((to_raise & FEnv::EX_UNDERFLOW) != 0U)
		{
			divfunc(small_value, large_value);
			std::uint32_t const status_word = FEnv::get_status_word();
			if ((FEnv::exception_value_from_status(static_cast<int>(status_word)) & FEnv::EX_UNDERFLOW) == 0U) { result = -1; }
		}
		if ((to_raise & FEnv::EX_INEXACT) != 0U)
		{
			float const two	  = 2.0F;
			float const three = 3.0F;
			// 2.0 / 3.0 cannot be represented exactly in any radix 2 floating point
			// format.
			divfunc(two, three);
			std::uint32_t const status_word = FEnv::get_status_word();
			if ((FEnv::exception_value_from_status(static_cast<int>(status_word)) & FEnv::EX_INEXACT) == 0U) result = -1;
		}
		if ((to_raise & FEnv::EX_FLUSHTOZERO) != 0U)
		{
			// TODO: raise the flush to zero floating point exception.
			result = -1;
		}
		return result;
	}

	inline int get_round()
	{
		std::uint32_t const rounding_mode = (FEnv::get_control_word() >> FEnv::ROUNDING_CONTROL_BIT_POSITION) & 0x3;
		switch (rounding_mode)
		{
		case FEnv::TONEAREST: return FE_TONEAREST;
		case FEnv::DOWNWARD: return FE_DOWNWARD;
		case FEnv::UPWARD: return FE_UPWARD;
		case FEnv::TOWARDZERO: return FE_TOWARDZERO;
		default: return -1; // Error value
		}
	}

	inline int set_round(int mode)
	{
		std::uint16_t bit_value = 1;
		switch (mode)
		{
		case FE_TONEAREST: bit_value = FEnv::TONEAREST; break;
		case FE_DOWNWARD: bit_value = FEnv::DOWNWARD; break;
		case FE_UPWARD: bit_value = FEnv::UPWARD; break;
		case FE_TOWARDZERO: bit_value = FEnv::TOWARDZERO; break;
		default: return bit_value; // Returns 1 to indicate failure if value was never reset
		}

		std::uint32_t control_word = FEnv::get_control_word();
		control_word &= static_cast<std::uint32_t>(~(0x3 << FEnv::ROUNDING_CONTROL_BIT_POSITION));
		control_word |= (static_cast<std::uint32_t>(bit_value << FEnv::ROUNDING_CONTROL_BIT_POSITION));
		FEnv::set_control_word(control_word);

		return 0;
	}

	inline int get_env(fenv_t * envp)
	{
		auto * state	   = reinterpret_cast<FEnv::FPState *>(envp);
		state->ControlWord = FEnv::get_control_word();
		state->StatusWord  = FEnv::get_status_word();
		return 0;
	}

	inline int set_env(const fenv_t * envp)
	{
		if (envp == FE_DFL_ENV)
		{
			// Default status and control words bits are all zeros, so we just write zeros.
			FEnv::set_status_word(0);
			FEnv::set_control_word(0);
			return 0;
		}
		const auto * state = reinterpret_cast<const FEnv::FPState *>(envp);
		FEnv::set_control_word(static_cast<uint32_t>(state->ControlWord));
		FEnv::set_status_word(static_cast<uint32_t>(state->StatusWord));
		return 0;
	}
} // namespace ccm::support::fenv::internal

#endif