/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/fp/fma.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/internal/support/multiply_add.hpp"
#include "ccmath/internal/types/number_pair.hpp"

namespace ccm
{
	namespace types
	{
		using DoubleDouble = NumberPair<double>;

		// True fused multiply-add for the error-free transforms below. support::multiply_add only
		// fuses when the target advertises a hardware FMA, so on a generic build without one it
		// degrades to (x * y) + z and a residual such as fma(a, b, -a*b) collapses to zero. At
		// runtime __builtin_fma is a correct fused operation on every target that provides it; where
		// it is absent (e.g. MSVC) the correctly-rounded software support::fp::generic_fma keeps the
		// residual exact in every rounding mode without falling back to libm. During constant
		// evaluation generic_fma is used as well, so the transforms stay exact and follow
		// CCM_CONSTEXPR_ROUNDING_MODE at compile time too.
		constexpr double exact_fma(double x, double y, double z) noexcept
		{
			if (support::is_constant_evaluated()) { return support::fp::generic_fma(x, y, z); }
#if CCM_HAS_BUILTIN(__builtin_fma)
			return __builtin_fma(x, y, z);
#else
			return support::fp::generic_fma(x, y, z);
#endif
		}

		// The output of Dekker's FastTwoSum algorithm is correct, i.e.:
		//   r.hi + r.lo = a + b exactly
		//   and |r.lo| < eps(r.lo)
		// if assumption: |a| >= |b|, or a = 0.
		constexpr DoubleDouble exact_add(double a, double b)
		{
			DoubleDouble r{ 0.0, 0.0 };
			r.hi		   = a + b;
			const double t = r.hi - a;
			r.lo		   = b - t;
			return r;
		}

		// Assumption: |a.hi| >= |b.hi|
		constexpr DoubleDouble add(const DoubleDouble & a, const DoubleDouble & b)
		{
			const DoubleDouble r = exact_add(a.hi, b.hi);
			const double lo		 = a.lo + b.lo;
			return exact_add(r.hi, r.lo + lo);
		}

		// Assumption: |a.hi| >= |b|
		constexpr DoubleDouble add(const DoubleDouble & a, double b)
		{
			const DoubleDouble r = exact_add(a.hi, b);
			return exact_add(r.hi, r.lo + a.lo);
		}

		// Velkamp's Splitting for double precision.
		constexpr DoubleDouble split(double a)
		{
			DoubleDouble r{ 0.0, 0.0 };
			// Splitting constant = 2^ceil(prec(double)/2) + 1 = 2^27 + 1.
			constexpr double C = 0x1.0p27 + 1.0;
			const double t1	   = C * a;
			const double t2	   = a - t1;
			r.hi			   = t1 + t2;
			r.lo			   = a - r.hi;
			return r;
		}

		constexpr DoubleDouble exact_mult(double a, double b)
		{
			DoubleDouble r{ 0.0, 0.0 };
			// Dekker's Product.
			const DoubleDouble as = split(a);
			const DoubleDouble bs = split(b);
			r.hi				  = a * b;
			const double t1		  = as.hi * bs.hi - r.hi;
			const double t2		  = as.hi * bs.lo + t1;
			const double t3		  = as.lo * bs.hi + t2;
			r.lo				  = as.lo * bs.lo + t3;
			return r;
		}

		constexpr DoubleDouble quick_mult(double a, const DoubleDouble & b)
		{
			DoubleDouble r = exact_mult(a, b.hi);
			r.lo		   = exact_fma(a, b.lo, r.lo);
			return r;
		}

		constexpr DoubleDouble quick_mult(const DoubleDouble & a, const DoubleDouble & b)
		{
			DoubleDouble r	= exact_mult(a.hi, b.hi);
			const double t1 = exact_fma(a.hi, b.lo, r.lo);
			const double t2 = exact_fma(a.lo, b.hi, t1);
			r.lo			= t2;
			return r;
		}

	} // namespace types

	// Specialization for DoubleDouble FMA.
	namespace support
	{
		// Assuming |c| >= |a * b|.
		template <>
		constexpr types::DoubleDouble
		multiply_add<types::DoubleDouble>(const types::DoubleDouble & x, const types::DoubleDouble & y, const types::DoubleDouble & z)
		{
			return add(z, quick_mult(x, y));
		}
	} // namespace support
} // namespace ccm
