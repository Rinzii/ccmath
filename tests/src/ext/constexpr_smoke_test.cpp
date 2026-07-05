/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <gtest/gtest.h>

#include <ccmath/ext/abs_diff.hpp>
#include <ccmath/ext/align.hpp>
#include <ccmath/ext/approximately.hpp>
#include <ccmath/ext/ceil_div.hpp>
#include <ccmath/ext/clamp.hpp>
#include <ccmath/ext/cubic.hpp>
#include <ccmath/ext/degrees.hpp>
#include <ccmath/ext/delta_angle.hpp>
#include <ccmath/ext/fract.hpp>
#include <ccmath/ext/inverse_lerp.hpp>
#include <ccmath/ext/is_power_of_two.hpp>
#include <ccmath/ext/lerp_angle.hpp>
#include <ccmath/ext/lerp_smooth.hpp>
#include <ccmath/ext/move_towards.hpp>
#include <ccmath/ext/move_towards_angle.hpp>
#include <ccmath/ext/normalize.hpp>
#include <ccmath/ext/ping_pong.hpp>
#include <ccmath/ext/radians.hpp>
#include <ccmath/ext/rcp.hpp>
#include <ccmath/ext/remap.hpp>
#include <ccmath/ext/repeat.hpp>
#include <ccmath/ext/saturate.hpp>
#include <ccmath/ext/sign.hpp>
#include <ccmath/ext/smoothstep.hpp>
#include <ccmath/ext/step.hpp>
#include <ccmath/ext/unlerp.hpp>
#include <ccmath/math/numbers.hpp>

#include <cstddef>

TEST(CcmathExtConstexprSmokeTest, AllFunctions)
{
	static_assert(ccm::ext::abs_diff(5, 2) == 3);
	static_assert(ccm::ext::align_up<unsigned, 8>(9) == 16);
	static_assert(ccm::ext::align_down<unsigned, 8>(9) == 8);
	static_assert((ccm::ext::align<unsigned, 8, ccm::ext::AR::Direction::eUP>(9) == 16));
	static_assert(ccm::ext::approximately(1.0, 1.0, 0.001));
	static_assert(ccm::ext::ceil_div(7, 3) == 3);
	static_assert(ccm::ext::clamp(5.0, 0.0, 10.0) == 5.0);
	static_assert(ccm::ext::cubic(0.0, 1.0, 2.0, 3.0, 0.5) == 1.5);
	static_assert(ccm::ext::degrees(ccm::numbers::pi_v<double>) == 180.0);
	static_assert(ccm::ext::delta_angle(0.0, ccm::numbers::pi_v<double>) == ccm::numbers::pi_v<double>);
	static_assert(ccm::ext::fract(3.25) == 0.25);
	static_assert(ccm::ext::inverse_lerp(0.0, 10.0, 5.0) == 0.5);
	static_assert(ccm::ext::is_power_of_two(8U));
	static_assert(ccm::ext::ispow2(16));
	static_assert(ccm::ext::lerp_angle(0.0, ccm::numbers::pi_v<double> / 2, 0.5) == ccm::numbers::pi_v<double> / 4);
	static_assert(ccm::ext::lerp_smooth(0.0, 10.0, 0.0, 1.0) == 0.0);
	static_assert(ccm::ext::move_towards(0.0, 10.0, 5.0) == 5.0);
	static_assert(ccm::ext::move_towards_angle(0.0, ccm::numbers::pi_v<double> / 2, ccm::numbers::pi_v<double> / 4) == ccm::numbers::pi_v<double> / 4);
	static_assert(ccm::ext::normalize(5.0, 0.0, 10.0) == 0.5);
	static_assert(ccm::ext::ping_pong(3.0, 4.0) == 3.0);
	static_assert(ccm::ext::radians(180.0) > 3.0);
	static_assert(ccm::ext::rcp(4.0) == 0.25);
	static_assert(ccm::ext::remap(0.0, 10.0, 0.0, 100.0, 5.0) == 50.0);
	static_assert(ccm::ext::repeat(5.0, 4.0) == 1.0);
	static_assert(ccm::ext::saturate(2.0) == 1.0);
	static_assert(ccm::ext::sign(-3.0) == -1.0);
	static_assert(ccm::ext::smoothstep(0.0, 1.0, 0.5) == 0.5);
	static_assert(ccm::ext::step(1.0, 0.5) == 0.0);
	static_assert(ccm::ext::unlerp(0.0, 10.0, 5.0) == 0.5);

	// Default helpers guard the degenerate input.
	static_assert(ccm::ext::ceil_div(7, 0) == 0);
	static_assert(ccm::ext::inverse_lerp(5.0, 5.0, 7.0) == 0.0);
	static_assert(ccm::ext::normalize(5.0, 5.0, 5.0) == 0.0);
	static_assert(ccm::ext::remap(5.0, 5.0, 1.0, 2.0, 7.0) == 1.0);
	static_assert(ccm::ext::repeat(5.0, 0.0) == 0.0);
	static_assert(ccm::ext::unlerp(5.0, 5.0, 7.0) == 0.0);

	// The unsafe variants skip the guard.
	static_assert(ccm::ext::unsafe::normalize(5.0, 0.0, 10.0) == 0.5);
	static_assert(ccm::ext::unsafe::remap(0.0, 10.0, 0.0, 100.0, 5.0) == 50.0);
	static_assert(ccm::ext::unsafe::repeat(5.0, 4.0) == 1.0);
	static_assert(ccm::ext::unsafe::unlerp(0.0, 10.0, 5.0) == 0.5);
}
