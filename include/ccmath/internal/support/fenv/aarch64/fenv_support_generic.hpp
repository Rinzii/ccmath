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

#if defined(CCM_TARGET_ARCH_AARCH64) && !defined(__APPLE__)
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
			std::uint32_t ControlWord;
			std::uint32_t StatusWord;
		};

		static_assert(sizeof(fenv_t) == sizeof(FPState), "Internal floating point state does not match the public fenv_t type.");

		static constexpr std::uint32_t TONEAREST  = 0x0;
		static constexpr std::uint32_t UPWARD	  = 0x1;
		static constexpr std::uint32_t DOWNWARD	  = 0x2;
		static constexpr std::uint32_t TOWARDZERO = 0x3;

		static constexpr std::uint32_t INVALID	 = 0x1;
		static constexpr std::uint32_t DIVBYZERO = 0x2;
		static constexpr std::uint32_t OVERFLOW	 = 0x4;
		static constexpr std::uint32_t UNDERFLOW = 0x8;
		static constexpr std::uint32_t INEXACT	 = 0x10;

		// Zero-th bit is the first bit.
		static constexpr std::uint32_t RoundingControlBitPosition		= 22;
		static constexpr std::uint32_t ExceptionStatusFlagsBitPosition	= 0;
		static constexpr std::uint32_t ExceptionControlFlagsBitPosition = 8;

		inline static std::uint32_t getStatusValueForExcept(int excepts)
		{
			return (((excepts & FE_INVALID) != 0) ? INVALID : 0) | ((excepts & FE_DIVBYZERO) != 0 ? DIVBYZERO : 0) |
				   ((excepts & FE_OVERFLOW) != 0 ? OVERFLOW : 0) | ((excepts & FE_UNDERFLOW) != 0 ? UNDERFLOW : 0) |
				   ((excepts & FE_INEXACT) != 0 ? INEXACT : 0);
		}

		inline static int exceptionStatusToMacro(std::uint32_t status)
		{
			return ((status & INVALID) != 0 ? FE_INVALID : 0) | ((status & DIVBYZERO) != 0 ? FE_DIVBYZERO : 0) | ((status & OVERFLOW) != 0 ? FE_OVERFLOW : 0) |
				   ((status & UNDERFLOW) != 0 ? FE_UNDERFLOW : 0) | ((status & INEXACT) != 0 ? FE_INEXACT : 0);
		}

		static std::uint32_t getControlWord()
		{
#ifdef __clang__
			// GCC does not currently support __arm_rsr.
			return __arm_rsr("fpcr");
#else
			return __builtin_aarch64_get_fpcr();
#endif
		}

		static void writeControlWord(std::uint32_t fpcr)
		{
#ifdef __clang__
			// GCC does not currently support __arm_wsr.
			__arm_wsr("fpcr", fpcr);
#else
			__builtin_aarch64_set_fpcr(fpcr);
#endif
		}

		static std::uint32_t getStatusWord()
		{
#ifdef __clang__
			return __arm_rsr("fpsr");
#else
			return __builtin_aarch64_get_fpsr();
#endif
		}

		static void writeStatusWord(std::uint32_t fpsr)
		{
#ifdef __clang__
			__arm_wsr("fpsr", fpsr);
#else
			__builtin_aarch64_set_fpsr(fpsr);
#endif
		}
	};

	inline int enable_except(int excepts)
	{
		std::uint32_t const newExcepts = FEnv::getStatusValueForExcept(excepts);
		std::uint32_t controlWord	   = FEnv::getControlWord();
		int const oldExcepts		   = (controlWord >> FEnv::ExceptionControlFlagsBitPosition) & 0x1F;
		controlWord |= (newExcepts << FEnv::ExceptionControlFlagsBitPosition);
		FEnv::writeControlWord(controlWord);
		return FEnv::exceptionStatusToMacro(static_cast<std::uint32_t>(oldExcepts));
	}

	inline int disable_except(int excepts)
	{
		std::uint32_t const disabledExcepts = FEnv::getStatusValueForExcept(excepts);
		std::uint32_t controlWord			= FEnv::getControlWord();
		int const oldExcepts				= (controlWord >> FEnv::ExceptionControlFlagsBitPosition) & 0x1F;
		controlWord &= ~(disabledExcepts << FEnv::ExceptionControlFlagsBitPosition);
		FEnv::writeControlWord(controlWord);
		return FEnv::exceptionStatusToMacro(static_cast<std::uint32_t>(oldExcepts));
	}

	inline int get_except()
	{
		std::uint32_t const controlWord = FEnv::getControlWord();
		int const enabledExcepts		= (controlWord >> FEnv::ExceptionControlFlagsBitPosition) & 0x1F;
		return FEnv::exceptionStatusToMacro(static_cast<std::uint32_t>(enabledExcepts));
	}

	inline int clear_except(int excepts)
	{
		std::uint32_t statusWord	= FEnv::getStatusWord();
		std::uint32_t const toClear = FEnv::getStatusValueForExcept(excepts);
		statusWord &= ~(toClear << FEnv::ExceptionStatusFlagsBitPosition);
		FEnv::writeStatusWord(statusWord);
		return 0;
	}

	inline int test_except(int excepts)
	{
		std::uint32_t const toTest	   = FEnv::getStatusValueForExcept(excepts);
		std::uint32_t const statusWord = FEnv::getStatusWord();
		return FEnv::exceptionStatusToMacro((statusWord >> FEnv::ExceptionStatusFlagsBitPosition) & toTest);
	}

	inline int set_except(int excepts)
	{
		std::uint32_t statusWord		= FEnv::getStatusWord();
		std::uint32_t const statusValue = FEnv::getStatusValueForExcept(excepts);
		statusWord |= (statusValue << FEnv::ExceptionStatusFlagsBitPosition);
		FEnv::writeStatusWord(statusWord);
		return 0;
	}

	inline int raise_except(int excepts)
	{
		float const zero	   = 0.0F;
		float const one		   = 1.0F;
		float const largeValue = FPBits<float>::max_normal().get_val();
		float const smallValue = FPBits<float>::min_normal().get_val();
		auto divfunc		   = [](float a, float b)
		{
			__asm__ __volatile__("ldr  s0, %0\n\t"
								 "ldr  s1, %1\n\t"
								 "fdiv s0, s0, s1\n\t"
								 : // No outputs
								 : "m"(a), "m"(b)
								 : "s0", "s1" /* s0 and s1 are clobbered */);
		};

		std::uint32_t const toRaise = FEnv::getStatusValueForExcept(excepts);
		int result					= 0;

		if ((toRaise & FEnv::INVALID) != 0U)
		{
			divfunc(zero, zero);
			std::uint32_t const statusWord = FEnv::getStatusWord();
			if (((statusWord >> FEnv::ExceptionStatusFlagsBitPosition) & FEnv::INVALID) == 0U) { result = -1; }
		}

		if ((toRaise & FEnv::DIVBYZERO) != 0U)
		{
			divfunc(one, zero);
			std::uint32_t const statusWord = FEnv::getStatusWord();
			if (((statusWord >> FEnv::ExceptionStatusFlagsBitPosition) & FEnv::DIVBYZERO) == 0U) { result = -1; }
		}
		if ((toRaise & FEnv::OVERFLOW) != 0U)
		{
			divfunc(largeValue, smallValue);
			std::uint32_t const statusWord = FEnv::getStatusWord();
			if (((statusWord >> FEnv::ExceptionStatusFlagsBitPosition) & FEnv::OVERFLOW) == 0U) { result = -1; }
		}
		if ((toRaise & FEnv::UNDERFLOW) != 0U)
		{
			divfunc(smallValue, largeValue);
			std::uint32_t const statusWord = FEnv::getStatusWord();
			if (((statusWord >> FEnv::ExceptionStatusFlagsBitPosition) & FEnv::UNDERFLOW) == 0U) { result = -1; }
		}
		if ((toRaise & FEnv::INEXACT) != 0U)
		{
			float const two	  = 2.0F;
			float const three = 3.0F;
			// 2.0 / 3.0 cannot be represented exactly in any radix 2 floating point
			// format.
			divfunc(two, three);
			std::uint32_t const statusWord = FEnv::getStatusWord();
			if (((statusWord >> FEnv::ExceptionStatusFlagsBitPosition) & FEnv::INEXACT) == 0U) { result = -1; }
		}
		return result;
	}

	inline int get_round()
	{
		std::uint32_t const roundingMode = (FEnv::getControlWord() >> FEnv::RoundingControlBitPosition) & 0x3;
		switch (roundingMode)
		{
		case FEnv::TONEAREST: return FE_TONEAREST;
		case FEnv::DOWNWARD: return FE_DOWNWARD;
		case FEnv::UPWARD: return FE_UPWARD;
		case FEnv::TOWARDZERO: return FE_TOWARDZERO;
		default: return -1; // Error value.
		}
	}

	inline int set_round(int mode)
	{
		std::uint16_t bitValue = 1;
		switch (mode)
		{
		case FE_TONEAREST: bitValue = FEnv::TONEAREST; break;
		case FE_DOWNWARD: bitValue = FEnv::DOWNWARD; break;
		case FE_UPWARD: bitValue = FEnv::UPWARD; break;
		case FE_TOWARDZERO: bitValue = FEnv::TOWARDZERO; break;
		default: return bitValue; // If bitValue value is not reset, then 1 indicates failure
		}

		std::uint32_t controlWord = FEnv::getControlWord();
		controlWord &= static_cast<std::uint32_t>(~(0x3 << FEnv::RoundingControlBitPosition));
		controlWord |= (static_cast<std::uint32_t>(bitValue << FEnv::RoundingControlBitPosition));
		FEnv::writeControlWord(controlWord);

		return 0;
	}

	inline int get_env(fenv_t * envp)
	{
		auto * state	   = reinterpret_cast<FEnv::FPState *>(envp);
		state->ControlWord = FEnv::getControlWord();
		state->StatusWord  = FEnv::getStatusWord();
		return 0;
	}

	inline int set_env(const fenv_t * envp)
	{
		if (envp == FE_DFL_ENV)
		{
			// Default status and control words bits are all zeros so we just
			// write zeros.
			FEnv::writeStatusWord(0);
			FEnv::writeControlWord(0);
			return 0;
		}
		const auto * state = reinterpret_cast<const FEnv::FPState *>(envp);
		FEnv::writeControlWord(state->ControlWord);
		FEnv::writeStatusWord(state->StatusWord);
		return 0;
	}
} // namespace ccm::support::fenv::internal
#endif // defined(CCM_TARGET_ARCH_AARCH64) && !defined(__APPLE__)