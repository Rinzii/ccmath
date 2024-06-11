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

#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <cerrno>
#include <cfenv>
#include <cstdint>

namespace ccm::support::fenv::internal
{
	inline int clear_except(int err_code)
	{
		return std::feclearexcept(err_code);
	}

	inline int test_except(int err_code)
	{
		return std::fetestexcept(err_code);
	}

	inline int get_except()
	{
		// Only GNU-based compilers support this function.
#ifdef __USE_GNU
		return ::fegetexcept();
#else
		return 0;
#endif
	}

	inline int set_except(int err_code)
	{
		// Only GNU-based compilers support this function.
#ifdef __USE_GNU
		return ::fesetexcept(err_code);
#else
		return 0;
#endif
	}

	inline int raise_except(int err_code)
	{
		return std::feraiseexcept(err_code);
	}

	inline int enable_except(int err_code)
	{
		// Only GNU-based compilers support this function.
#ifdef __USE_GNU
		return ::feenableexcept(err_code);
#else
		return 0;
#endif
	}
	inline int disable_except(int err_code)
	{
// Only GNU-based compilers support this function.
#ifdef __USE_GNU
		return ::fedisableexcept(err_code);
#else
		return 0;
#endif
	}

	inline int get_round()
	{
		return ccm::support::get_rounding_mode();
	}

	inline int set_round(int rounding_mode)
	{
		return std::fesetround(rounding_mode);
	}

	inline int get_env(std::fenv_t * envp)
	{
		return std::fegetenv(envp);
	}

	inline int set_env(const std::fenv_t * envp)
	{
		return std::fesetenv(envp);
	}
} // namespace ccm::support::fenv::internal

namespace ccm::support::fenv
{
	enum class ccm_math_err_mode : std::uint8_t
	{
		eErrno		 = 1,
		eErrnoExcept = 2,
	};

	// Helper function to convert the enum class to an integer to enable bitwise operations.
	constexpr int get_mode(ccm_math_err_mode mode)
	{
		return static_cast<int>(mode);
	}

	constexpr int ccm_math_err_handling()
	{
#if defined(__FAST_MATH__) || defined(CCM_CONFIG_DISABLE_ERRNO)
		return 0;
#elif defined(__NO_MATH_ERRNO__)
		return get_mode(ccm_math_err_mode::eErrnoExcept);
#elif defined(__NVPTX__) || defined(__AMDGPU__)
		return get_mode(ccm_math_err_mode::eErrno);
#else
		return get_mode(ccm_math_err_mode::eErrno) | get_mode(ccm_math_err_mode::eErrnoExcept);
#endif
	}

	constexpr int set_except_if_required(int excepts)
	{
		if constexpr (is_constant_evaluated()) { return 0; } // We cannot raise fenv exceptions in a constexpr context. So we return.
		else
		{
			if ((ccm_math_err_handling() & get_mode(ccm_math_err_mode::eErrnoExcept)) != 0) { return internal::set_except(excepts); }
			return 0;
		}
	}

	constexpr int raise_except_if_required(int excepts)
	{
		if constexpr (is_constant_evaluated()) { return 0; } // We cannot raise fenv exceptions in a constexpr context. So we return.
		else
		{
			if ((ccm_math_err_handling() & get_mode(ccm_math_err_mode::eErrnoExcept)) != 0) { return internal::raise_except(excepts); }
			return 0;
		}
	}

	constexpr void set_errno_if_required(int err)
	{
		// NOLINTNEXTLINE(bugprone-branch-clone)
		if constexpr (is_constant_evaluated()) // We cannot raise fenv exceptions in a constexpr context. So we return.
		{
			// Do nothing
		}
		else
		{
			if ((ccm_math_err_handling() & get_mode(ccm_math_err_mode::eErrnoExcept)) != 0) { errno = err; }
		}
	}
} // namespace ccm::support::fenv