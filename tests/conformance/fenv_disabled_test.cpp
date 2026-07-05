/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Built with CCM_CONFIG_DISABLE_FENV so that host_fenv.hpp resolves to its no-fenv tier, the same
// state a host with no <cfenv> and no <fenv.h> (such as the PS5 toolchain) ends up in. The suite
// confirms that the headers compile without the host fenv and that the documented fallback contract
// holds: runtime rounding pinned to round-to-nearest, fp-exception signaling reduced to a no-op, and
// errno handling still compilable.

#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/host_fenv.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/math/nearest/rint.hpp"

#include <gtest/gtest.h>

#if defined(CCM_CONFIG_HAS_FENV)
	#error "fenv_disabled_test must be built with CCM_CONFIG_DISABLE_FENV so no host fenv is selected"
#endif

namespace fenv = ccm::support::fenv;

TEST(CcmathFenvDisabledTests, RoundingModeIsPinnedToNearest)
{
	// Without a host fenv the runtime read cannot observe the hardware mode, so every query helper
	// reports the round-to-nearest default. The volatile sink keeps the call on the runtime read
	// rather than letting the constant evaluator fold it.
	volatile int rm = 0;
	rm				= fenv::get_rounding_mode();
	EXPECT_EQ(static_cast<int>(rm), FE_TONEAREST);

	EXPECT_TRUE(fenv::rounding_mode_is_round_to_nearest());
	EXPECT_FALSE(fenv::rounding_mode_is_round_up());
	EXPECT_FALSE(fenv::rounding_mode_is_round_down());
	EXPECT_FALSE(fenv::rounding_mode_is_round_to_zero());
}

TEST(CcmathFenvDisabledTests, ExceptionSignalingIsNoOp)
{
	// The fp-exception entry points still exist and compile, but they do nothing, and the error
	// handling mask does not carry the exception bit when the host fenv is unavailable.
	EXPECT_EQ(fenv::raise_except_if_required(FE_OVERFLOW | FE_INVALID), 0);
	EXPECT_EQ(fenv::set_except_if_required(FE_INEXACT), 0);
	EXPECT_EQ(fenv::ccm_math_err_handling() & fenv::get_mode(fenv::ccm_math_err_mode::eErrnoExcept), 0);
}

TEST(CcmathFenvDisabledTests, RuntimeMathEvaluatesUnderNearest)
{
	// A function that reads the rounding mode must still evaluate and produce the round-to-nearest,
	// ties-to-even result.
	const volatile double x = 2.5;
	EXPECT_EQ(ccm::rint(static_cast<double>(x)), 2.0);
}
