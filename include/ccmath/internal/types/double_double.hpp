/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/multiply_add.hpp"
#include "ccmath/internal/types/number_pair.hpp"

namespace ccm
{
	namespace type
	{
		using DoubleDouble = NumberPair<double>;

		// The output of Dekker's FastTwoSum algorithm is correct, i.e.:
		//   r.hi + r.lo = a + b exactly
		//   and |r.lo| < eps(r.lo)
		// if assumption: |a| >= |b|, or a = 0.
		constexpr DoubleDouble exact_add(double a, double b)
		{
			DoubleDouble r{0.0, 0.0};
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
			DoubleDouble r{0.0, 0.0};
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
			DoubleDouble r{0.0, 0.0};

			// If we have builtin FMA, we can use it to get the exact product.
			#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
			r.hi = a * b;
			r.lo = support::multiply_add(a, b, -r.hi);
			#else
			// Dekker's Product.
			const DoubleDouble as = split(a);
			const DoubleDouble bs = split(b);
			r.hi				  = a * b;
			const double t1		  = as.hi * bs.hi - r.hi;
			const double t2		  = as.hi * bs.lo + t1;
			const double t3		  = as.lo * bs.hi + t2;
			r.lo				  = as.lo * bs.lo + t3;
			#endif

			return r;
		}

		constexpr DoubleDouble quick_mult(double a, const DoubleDouble & b)
		{
			DoubleDouble r = exact_mult(a, b.hi);
			r.lo		   = support::multiply_add(a, b.lo, r.lo);
			return r;
		}

		constexpr DoubleDouble quick_mult(const DoubleDouble & a, const DoubleDouble & b)
		{
			DoubleDouble r	= exact_mult(a.hi, b.hi);
			const double t1 = support::multiply_add(a.hi, b.lo, r.lo);
			const double t2 = support::multiply_add(a.lo, b.hi, t1);
			r.lo			= t2;
			return r;
		}

	} // namespace type

	// Specialization for DoubleDouble FMA.
	namespace support
	{
		// Assuming |c| >= |a * b|.
		template <>
		constexpr type::DoubleDouble multiply_add<type::DoubleDouble>(const type::DoubleDouble & x, const type::DoubleDouble & y, const type::DoubleDouble & z)
		{
			return add(z, quick_mult(x, y));
		}
	} // namespace support
} // namespace ccm