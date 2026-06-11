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

#include "ccmath/internal/config/powl_policy.hpp"
#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/math/generic/func/power/pow_impl/pow_impl.hpp"
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
	#include "ccmath/internal/math/generic/func/power/pow_impl/powl_ld80_kernel.hpp"
#endif
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <cfloat>
#include <cstdint>
#include <limits>

namespace ccm::gen
{
	namespace internal
	{
		namespace powl_bits
		{
			using PowlFPBits_t = support::fp::FPBits<long double>;

			template <typename Storage>
			constexpr bool storage_is_zero(Storage value) noexcept
			{ return value == Storage{}; }

			template <typename Storage>
			constexpr int storage_countr_zero(Storage value) noexcept
			{
				if (storage_is_zero(value)) { return std::numeric_limits<Storage>::digits; }
				return support::countr_zero(value);
			}

			template <typename Storage>
			constexpr bool storage_fits_shifted_int64(Storage unit, int scale) noexcept
			{
				if (scale < 0 || scale > 62) { return false; }
				if (scale == 0) { return unit <= static_cast<Storage>(std::numeric_limits<std::int64_t>::max()); }

				const Storage limit = static_cast<Storage>(1) << static_cast<unsigned>(62 - scale);
				return unit <= limit;
			}

			template <typename Storage>
			constexpr std::int64_t storage_to_int64_magnitude(Storage unit, int scale) noexcept
			{
				if (scale == 0) { return static_cast<std::int64_t>(unit); }
				return static_cast<std::int64_t>(unit) << static_cast<unsigned>(scale);
			}

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
			constexpr typename PowlFPBits_t::storage_type float80_unit_mantissa(const PowlFPBits_t & bits) noexcept
			{
				typename PowlFPBits_t::storage_type mantissa = bits.get_explicit_mantissa();
				if (bits.get_implicit_bit()) { mantissa |= PowlFPBits_t::EXPLICIT_BIT_MASK; }
				return mantissa;
			}

			constexpr bool is_integer_float80_bits(const PowlFPBits_t & bits) noexcept
			{
				if (bits.is_nan() || bits.is_inf()) { return false; }
				if (bits.is_zero()) { return true; }

				const typename PowlFPBits_t::storage_type mantissa = float80_unit_mantissa(bits);
				if (storage_is_zero(mantissa)) { return true; }

				const int exponent		 = bits.get_explicit_exponent();
				const int trailing_zeros = storage_countr_zero(mantissa);
				return exponent + trailing_zeros >= static_cast<int>(PowlFPBits_t::fraction_length);
			}

			constexpr bool is_odd_integer_float80_bits(const PowlFPBits_t & bits) noexcept
			{
				if (!is_integer_float80_bits(bits)) { return false; }
				if (bits.is_zero()) { return false; }

				const typename PowlFPBits_t::storage_type mantissa = float80_unit_mantissa(bits);
				const int exponent								   = bits.get_explicit_exponent();
				const int trailing_zeros						   = storage_countr_zero(mantissa);
				return exponent + trailing_zeros == static_cast<int>(PowlFPBits_t::fraction_length);
			}

			constexpr bool try_extract_int64(const PowlFPBits_t & bits, std::int64_t & out) noexcept;
#endif

			constexpr bool is_integer(const PowlFPBits_t & bits) noexcept
			{
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
				return is_integer_float80_bits(bits);
#else
				if (bits.is_nan() || bits.is_inf()) { return false; }
				if (bits.is_zero()) { return true; }

				const int exponent								   = bits.get_explicit_exponent();
				const typename PowlFPBits_t::storage_type mantissa = bits.get_explicit_mantissa();
				if (storage_is_zero(mantissa)) { return true; }

				const int trailing_zeros = storage_countr_zero(mantissa);
				return exponent + trailing_zeros >= static_cast<int>(PowlFPBits_t::fraction_length);
#endif
			}

			constexpr bool is_odd_integer(const PowlFPBits_t & bits) noexcept
			{
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
				std::int64_t magnitude = 0;
				if (try_extract_int64(bits, magnitude)) { return (magnitude & 1) != 0; }
				return is_odd_integer_float80_bits(bits);
#else
				if (!is_integer(bits)) { return false; }
				if (bits.is_zero()) { return false; }

				const int exponent								   = bits.get_explicit_exponent();
				const typename PowlFPBits_t::storage_type mantissa = bits.get_explicit_mantissa();
				const int trailing_zeros						   = storage_countr_zero(mantissa);
				return exponent + trailing_zeros == static_cast<int>(PowlFPBits_t::fraction_length);
#endif
			}

			inline constexpr std::int64_t kBoundedExponentMax = (std::int64_t{ 1 } << 62) - 1;

			constexpr bool try_extract_int64(const PowlFPBits_t & bits, std::int64_t & out) noexcept
			{
				if (bits.is_nan() || bits.is_inf()) { return false; }
				if (bits.is_zero())
				{
					out = 0;
					return true;
				}
				if (!is_integer(bits)) { return false; }

				const int exponent = bits.get_explicit_exponent();
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
				typename PowlFPBits_t::storage_type mantissa = bits.get_explicit_mantissa();
				if (bits.get_implicit_bit()) { mantissa |= PowlFPBits_t::EXPLICIT_BIT_MASK; }
				const int trailing_zeros = storage_countr_zero(mantissa);
				const int scale			 = exponent + trailing_zeros - static_cast<int>(PowlFPBits_t::fraction_length);
#else
				typename PowlFPBits_t::storage_type mantissa = bits.get_explicit_mantissa();
				const int trailing_zeros					 = storage_countr_zero(mantissa);
				const int scale								 = exponent + trailing_zeros - static_cast<int>(PowlFPBits_t::fraction_length);
#endif
				if (scale < 0) { return false; }

				mantissa >>= static_cast<unsigned>(trailing_zeros);
				if (!storage_fits_shifted_int64(mantissa, scale)) { return false; }

				const std::int64_t magnitude = storage_to_int64_magnitude(mantissa, scale);
				if (magnitude > kBoundedExponentMax) { return false; }

				out = bits.is_neg() ? -magnitude : magnitude;
				return true;
			}

			constexpr bool is_integer(long double val) noexcept
			{ return is_integer(PowlFPBits_t(val)); }

			constexpr bool is_odd_integer(long double val) noexcept
			{ return is_odd_integer(PowlFPBits_t(val)); }

			constexpr bool try_extract_int64(long double val, std::int64_t & out) noexcept
			{ return try_extract_int64(PowlFPBits_t(val), out); }

		} // namespace powl_bits

		namespace impl
		{
			using PowlFPBits_t = support::fp::FPBits<long double>;
			using Sign		   = types::Sign;

			constexpr long double powl_unsupported_result() noexcept
			{ return std::numeric_limits<long double>::quiet_NaN(); }

			// Incomplete powl tiers use double pow_impl and cast back. Not native long double yet.
			constexpr long double powl_reduced_precision_double_fallback(long double base, long double exp) noexcept
			{ return static_cast<long double>(::ccm::gen::impl::pow_impl(static_cast<double>(base), static_cast<double>(exp))); }

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
			// Double-double (compensated) arithmetic for integer powers. Naive exponentiation by
			// squaring loses the bits past the 64-bit significand on every multiply, which for large
			// |exp| (e.g. 10^1000) drifts well past the 4 ULP budget. Accumulating the squaring in a
			// long double pair keeps a ~128-bit running product and rounds once at the end.
			namespace powl_dd
			{
				using Pair = types::NumberPair<long double>;

				// The Veltkamp split inside exact_mult multiplies its operand by 2^33, which overflows
				// to infinity once the operand approaches LDBL_MAX. Above this magnitude the running
				// product is already saturating toward overflow/underflow where the compensation tail is
				// irrelevant, so we fall back to a plain product to avoid NaN contamination.
				inline constexpr long double kSplitSafe = 0x1.0p16300L;

				constexpr long double abs_ld(long double x) noexcept
				{ return x < 0.0L ? -x : x; }

				constexpr Pair mul(const Pair & a, const Pair & b) noexcept
				{
					const long double hi = a.hi * b.hi;
					// Once the product overflows (or contaminates with NaN) the exact-mult split would
					// produce inf - inf = NaN, so propagate a clean infinity instead.
					if (ccm::isinf(hi) || ccm::isnan(hi)) { return Pair{ hi, 0.0L }; }
					if (abs_ld(a.hi) >= kSplitSafe || abs_ld(b.hi) >= kSplitSafe) { return Pair{ hi, 0.0L }; }
					Pair p = bit80::powl_ld80_detail::exact_mult(a.hi, b.hi);
					p.lo += a.hi * b.lo + a.lo * b.hi;
					return bit80::powl_ld80_detail::exact_add(p.hi, p.lo);
				}

				// base^e for e > 0, accumulated as a double-double.
				constexpr Pair ipow(long double base, std::uint64_t e) noexcept
				{
					Pair result{ 1.0L, 0.0L };
					Pair factor{ base, 0.0L };
					while (e > 0U)
					{
						if ((e & 1U) != 0U) { result = mul(result, factor); }
						e >>= 1U;
						if (e > 0U) { factor = mul(factor, factor); }
					}
					return result;
				}

				// 1 / (p.hi + p.lo) refined to long double precision via one Newton step.
				constexpr long double reciprocal(const Pair & p) noexcept
				{
					const long double r0 = 1.0L / p.hi;
					if (r0 == 0.0L || ccm::isinf(r0) || ccm::isnan(r0)) { return r0; }
					// p.hi near LDBL_MAX would overflow the split. r0 alone is already within an ULP.
					if (abs_ld(p.hi) >= kSplitSafe) { return r0; }
					const Pair pr		  = bit80::powl_ld80_detail::exact_mult(p.hi, r0);
					const long double res = ((1.0L - pr.hi) - pr.lo) - p.lo * r0;
					return r0 + r0 * res;
				}
			} // namespace powl_dd
#endif

			constexpr long double powl_bounded_integer(long double base, std::int64_t exp) noexcept
			{
				if (exp == 0) { return 1.0L; }

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
				const std::uint64_t magnitude = exp < 0 ? static_cast<std::uint64_t>(-(exp + 1)) + 1U : static_cast<std::uint64_t>(exp);
				const powl_dd::Pair p		  = powl_dd::ipow(base, magnitude);
				if (exp < 0) { return powl_dd::reciprocal(p); }
				return p.hi + p.lo;
#else
				if (exp < 0) { return 1.0L / powl_bounded_integer(base, -exp); }

				long double result = 1.0L;
				long double factor = base;
				std::uint64_t e	   = static_cast<std::uint64_t>(exp);
				while (e > 0)
				{
					if ((e & 1U) != 0U) { result *= factor; }
					if (e > 1U) { factor *= factor; }
					e >>= 1U;
				}
				return result;
#endif
			}

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
			inline constexpr long double kPowlHugeExponentThreshold =
				2.0L * static_cast<long double>(-LDBL_MIN_EXP + LDBL_MANT_DIG + 1) / static_cast<long double>(LDBL_EPSILON);

			namespace bit80
			{
				constexpr long double powl_calc_80bits(long double base, long double exp) noexcept
				{
					if (exp == 0.0L) { return 1.0L; }
					if (base == 1.0L) { return 1.0L; }

					if (ccm::isnan(base) || ccm::isnan(exp)) { return std::numeric_limits<long double>::quiet_NaN(); }

					if (support::is_constant_evaluated() && base > 0.0L && !ccm::isinf(base))
					{
						const long double rounded_exp = static_cast<long double>(static_cast<std::int64_t>(exp));
						if (exp == rounded_exp)
						{
							const std::int64_t int_exp = static_cast<std::int64_t>(exp);
							if (int_exp >= -powl_bits::kBoundedExponentMax && int_exp <= powl_bits::kBoundedExponentMax)
							{
								return powl_bounded_integer(base, int_exp);
							}
						}
					}

					const PowlFPBits_t base_bits(base);
					const PowlFPBits_t exp_bits(exp);

					if (base_bits.is_zero())
					{
						const bool out_is_neg = base_bits.is_neg() && powl_bits::is_odd_integer(exp);

						if (exp < 0.0L)
						{
							support::fenv::set_errno_if_required(EDOM);
							support::fenv::raise_except_if_required(FE_DIVBYZERO);
							return PowlFPBits_t::inf(out_is_neg ? Sign::NEG : Sign::POS).get_val();
						}

						return out_is_neg ? -0.0L : 0.0L;
					}

					if (exp_bits.is_inf())
					{
						const long double base_abs = base_bits.abs().get_val();
						const long double one	   = 1.0L;

						if (base_abs == one) { return 1.0L; }

						if (exp_bits.is_neg()) { return base_abs < one ? std::numeric_limits<long double>::infinity() : 0.0L; }

						return base_abs < one ? 0.0L : std::numeric_limits<long double>::infinity();
					}

					if (base_bits.is_inf())
					{
						if (base_bits.is_neg() && !powl_bits::is_integer(exp))
						{
							// glibc libm: |−inf|^y for non-integer y is +inf when y > 0 and +0 when y < 0.
							if (exp > 0.0L) { return PowlFPBits_t::inf(Sign::POS).get_val(); }
							if (exp < 0.0L) { return 0.0L; }
							return std::numeric_limits<long double>::quiet_NaN();
						}

						const bool out_is_neg = base_bits.is_neg() && powl_bits::is_odd_integer(exp);
						if (exp < 0.0L) { return out_is_neg ? -0.0L : 0.0L; }
						return PowlFPBits_t::inf(out_is_neg ? Sign::NEG : Sign::POS).get_val();
					}

					if (base_bits.is_neg() && !powl_bits::is_integer(exp))
					{
						support::fenv::set_errno_if_required(EDOM);
						support::fenv::raise_except_if_required(FE_INVALID);
						return std::numeric_limits<long double>::quiet_NaN();
					}

					long double working_base = base;
					long double working_exp	 = exp;
					bool result_is_neg		 = false;

					if (base_bits.is_neg())
					{
						if (powl_bits::is_odd_integer(exp)) { result_is_neg = true; }
						working_base = base_bits.abs().get_val();
					}

					std::int64_t int_exp = 0;
					if (powl_bits::try_extract_int64(exp, int_exp))
					{
						long double result = powl_bounded_integer(working_base, int_exp);
						// Only trust the exact integer round trip when it lands on a normal value.
						// An overflowing or underflowing intermediate (e.g. max^-3 squares max first)
						// cannot saturate per rounding mode, so defer such cases to the general kernel,
						// which over/underflows through internal_ldexp and is correct in every mode.
						if (const PowlFPBits_t r_bits(result); r_bits.is_finite() && !r_bits.is_zero() && !r_bits.is_subnormal())
						{
							if (result_is_neg) { result = -result; }
							return result;
						}
					}

					const PowlFPBits_t exp_abs_bits(exp);
					const long double exp_abs = exp_abs_bits.abs().get_val();
					if (exp_abs > kPowlHugeExponentThreshold) { working_exp = exp_abs_bits.is_neg() ? -0x1.0p100L : 0x1.0p100L; }

					long double result = bit80::powl_ld80_general_finite(working_base, working_exp);
					if (result_is_neg && result != 0.0L && !ccm::isinf(result) && !ccm::isnan(result)) { result = -result; }
					return result;
				}
			} // namespace bit80
#endif

			constexpr long double powl_impl_non_native(long double base, long double exp) noexcept
			{
				if (!config::reduced_precision_powl_fallback_enabled()) { return powl_unsupported_result(); }
				return powl_reduced_precision_double_fallback(base, exp);
			}

			constexpr long double powl_impl(long double base, long double exp) noexcept
			{
#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT64)
				// long double is a double alias. Same kernel as pow, not extended precision.
				return static_cast<long double>(::ccm::gen::impl::pow_impl(static_cast<double>(base), static_cast<double>(exp)));
#elif defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
				return bit80::powl_calc_80bits(base, exp);
#elif defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT128)
				return powl_impl_non_native(base, exp);
#else
				return powl_impl_non_native(base, exp);
#endif
			}

		} // namespace impl
	} // namespace internal

	// Dispatch for long double pow. Native precision only on binary80. See powl_policy.hpp.
	constexpr long double powl_gen(long double base, long double exp) noexcept
	{ return internal::impl::powl_impl(base, exp); }

} // namespace ccm::gen
