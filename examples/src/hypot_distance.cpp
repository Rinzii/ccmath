/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "io.hpp"

#include <ccmath/ccmath.hpp>

namespace
{
	struct Vec3
	{
		double x;
		double y;
		double z;
	};

	Vec3 const kSensor{ 12.0, -4.5, 1.25 };
	Vec3 const kTarget{ 27.0, 18.0, 6.0 };

	double planar_distance(Vec3 const & a, Vec3 const & b)
	{
		return ccm::hypot(b.x - a.x, b.y - a.y);
	}

	double spatial_distance(Vec3 const & a, Vec3 const & b)
	{
		double const dx = b.x - a.x;
		double const dy = b.y - a.y;
		double const dz = b.z - a.z;
		return ccm::sqrt(ccm::fma(dx, dx, ccm::fma(dy, dy, dz * dz)));
	}
} // namespace

int main()
{
	ccm::examples::banner("sensor geometry");

	ccm::examples::print_row("planar distance (hypot)", planar_distance(kSensor, kTarget));
	ccm::examples::print_row("spatial distance (sqrt+fma)", spatial_distance(kSensor, kTarget));

	return 0;
}
