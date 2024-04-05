/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/likely.hpp"
#include "ccmath/internal/support/always_false.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/math_support.hpp"
#include "ccmath/internal/types/float128.hpp"
#include "ccmath/internal/types/sign.hpp"
#include "ccmath/internal/types/uint128.hpp"
#include <cstdint>

namespace ccm::support
{

	// The supported floating point types.
	enum class FPType : uint8_t
	{
		eBinary32,
		eBinary64,
		eBinary128,
		eBinary80_x86,
	};

	namespace internal
	{

		template <FPType>
		struct FPLayout
		{
		};

		template <>
		struct FPLayout<FPType::eBinary32>
		{
			using StorageType						 = uint32_t;
			inline static constexpr int SIGN_LEN	 = 1;
			inline static constexpr int EXP_LEN		 = 8;
			inline static constexpr int SIG_LEN		 = 23;
			inline static constexpr int FRACTION_LEN = SIG_LEN;
		};

		template <>
		struct FPLayout<FPType::eBinary64>
		{
			using StorageType						 = uint64_t;
			inline static constexpr int SIGN_LEN	 = 1;
			inline static constexpr int EXP_LEN		 = 11;
			inline static constexpr int SIG_LEN		 = 52;
			inline static constexpr int FRACTION_LEN = SIG_LEN;
		};

		template <>
		struct FPLayout<FPType::eBinary128>
		{
			using StorageType						 = ccm::uint128;
			inline static constexpr int SIGN_LEN	 = 1;
			inline static constexpr int EXP_LEN		 = 15;
			inline static constexpr int SIG_LEN		 = 112;
			inline static constexpr int FRACTION_LEN = SIG_LEN;
		};

		template <>
		struct FPLayout<FPType::eBinary80_x86>
		{
			using StorageType						 = ccm::uint128;
			inline static constexpr int SIGN_LEN	 = 1;
			inline static constexpr int EXP_LEN		 = 15;
			inline static constexpr int SIG_LEN		 = 64;
			inline static constexpr int FRACTION_LEN = SIG_LEN - 1;
		};

		// FPStorage derives useful constants from the FPLayout above.
		template <FPType fp_type>
		struct FPStorage : public FPLayout<fp_type>
		{
			using UP = FPLayout<fp_type>;

			using UP::EXP_LEN;	// The number of bits for the *exponent* part
			using UP::SIG_LEN;	// The number of bits for the *significand* part
			using UP::SIGN_LEN; // The number of bits for the *sign* part
			// For convenience, the sum of `SIG_LEN`, `EXP_LEN`, and `SIGN_LEN`.
			inline static constexpr int TOTAL_LEN = SIGN_LEN + EXP_LEN + SIG_LEN;

			// The number of bits after the decimal dot when the number is in normal form.
			using UP::FRACTION_LEN;

			// An unsigned integer that is wide enough to contain all of the floating
			// point bits.
			using StorageType = typename UP::StorageType;

			// The number of bits in StorageType.
			inline static constexpr int STORAGE_LEN = sizeof(StorageType) * CHAR_BIT;
			static_assert(STORAGE_LEN >= TOTAL_LEN);

			// The exponent bias. Always positive.
			inline static constexpr int32_t EXP_BIAS = (1U << (EXP_LEN - 1U)) - 1U;
			static_assert(EXP_BIAS > 0);

			// The bit pattern that keeps only the *significand* part.
			inline static constexpr StorageType SIG_MASK = mask_trailing_ones<StorageType, SIG_LEN>();
			// The bit pattern that keeps only the *exponent* part.
			inline static constexpr StorageType EXP_MASK = mask_trailing_ones<StorageType, EXP_LEN>() << SIG_LEN;
			// The bit pattern that keeps only the *sign* part.
			inline static constexpr StorageType SIGN_MASK = mask_trailing_ones<StorageType, SIGN_LEN>() << (EXP_LEN + SIG_LEN);
			// The bit pattern that keeps only the *exponent + significand* part.
			inline static constexpr StorageType EXP_SIG_MASK = mask_trailing_ones<StorageType, EXP_LEN + SIG_LEN>();
			// The bit pattern that keeps only the *sign + exponent + significand* part.
			inline static constexpr StorageType FP_MASK = mask_trailing_ones<StorageType, TOTAL_LEN>();
			// The bit pattern that keeps only the *fraction* part.
			// i.e., the *significand* without the leading one.
			inline static constexpr StorageType FRACTION_MASK = mask_trailing_ones<StorageType, FRACTION_LEN>();

			static_assert((SIG_MASK & EXP_MASK & SIGN_MASK) == 0, "masks disjoint");
			static_assert((SIG_MASK | EXP_MASK | SIGN_MASK) == FP_MASK, "masks cover");

		protected:
			// Merge bits from 'a' and 'b' values according to 'mask'.
			// Use 'a' bits when corresponding 'mask' bits are zeroes and 'b' bits when
			// corresponding bits are ones.
			inline static constexpr StorageType merge(StorageType a, StorageType b, StorageType mask)
			{
				// https://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
				return a ^ ((a ^ b) & mask);
			}

			// A stongly typed integer that prevents mixing and matching integers with
			// different semantics.
			template <typename T>
			struct TypedInt // NOLINT(cppcoreguidelines-special-member-functions)
			{
				using value_type = T;
				inline constexpr explicit TypedInt(T value) : value(value) {}
				inline constexpr TypedInt(const TypedInt & value)			  = default;
				inline constexpr TypedInt & operator=(const TypedInt & value) = default;

				inline constexpr explicit operator T() const { return value; }

				[[nodiscard]] inline constexpr StorageType to_storage_type() const { return static_cast<StorageType>(value); }

				inline friend constexpr bool operator==(TypedInt a, TypedInt b) { return a.value == b.value; }
				inline friend constexpr bool operator!=(TypedInt a, TypedInt b) { return a.value != b.value; }

			protected:
				T value; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
			};

			// An opaque type to store a floating point exponent.
			// We define special values but it is valid to create arbitrary values as long
			// as they are in the range [min, max].
			struct Exponent : public TypedInt<int32_t>
			{
				using UP = TypedInt<int32_t>;
				using UP::UP;
				inline static constexpr auto subnormal() { return Exponent(-EXP_BIAS); }
				inline static constexpr auto min() { return Exponent(1 - EXP_BIAS); }
				inline static constexpr auto zero() { return Exponent(0); }
				inline static constexpr auto max() { return Exponent(EXP_BIAS); }
				inline static constexpr auto inf() { return Exponent(EXP_BIAS + 1); }
			};

			// An opaque type to store a floating point biased exponent.
			// We define special values but it is valid to create arbitrary values as long
			// as they are in the range [zero, bits_all_ones].
			// Values greater than bits_all_ones are truncated.
			struct BiasedExponent : public TypedInt<uint32_t>
			{
				using UP = TypedInt<uint32_t>;
				using UP::UP;

				inline constexpr explicit BiasedExponent(Exponent exp) : UP(static_cast<int32_t>(exp) + EXP_BIAS) {}

				// Cast operator to get convert from BiasedExponent to Exponent.
				inline constexpr explicit operator Exponent() const { return Exponent(UP::value - EXP_BIAS); }

				inline constexpr BiasedExponent & operator++()
				{
					assert(*this != BiasedExponent(Exponent::inf()));
					++UP::value;
					return *this;
				}

				inline constexpr BiasedExponent & operator--()
				{
					assert(*this != BiasedExponent(Exponent::subnormal()));
					--UP::value;
					return *this;
				}
			};

			// An opaque type to store a floating point significand.
			// We define special values but it is valid to create arbitrary values as long
			// as they are in the range [zero, bits_all_ones].
			// Note that the semantics of the Significand are implementation dependent.
			// Values greater than bits_all_ones are truncated.
			struct Significand : public TypedInt<StorageType>
			{
				using UP = TypedInt<StorageType>;
				using UP::UP;

				inline friend constexpr Significand operator|(const Significand a, const Significand b)
				{
					return Significand((a.to_storage_type() | b.to_storage_type()));
				}
				inline friend constexpr Significand operator^(const Significand a, const Significand b)
				{
					return Significand((a.to_storage_type() ^ b.to_storage_type()));
				}
				inline friend constexpr Significand operator>>(const Significand a, int shift) { return Significand((a.to_storage_type() >> shift)); }

				inline static constexpr auto zero() { return Significand(static_cast<StorageType>(0)); }
				inline static constexpr auto lsb() { return Significand(static_cast<StorageType>(1)); }
				inline static constexpr auto msb() { return Significand(static_cast<StorageType>(1) << (SIG_LEN - 1)); }
				inline static constexpr auto bits_all_ones() { return Significand(SIG_MASK); }
			};

			inline static constexpr StorageType encode(BiasedExponent exp) { return (exp.to_storage_type() << SIG_LEN) & EXP_MASK; }

			inline static constexpr StorageType encode(Significand value) { return value.to_storage_type() & SIG_MASK; }

			inline static constexpr StorageType encode(BiasedExponent exp, Significand sig) { return encode(exp) | encode(sig); }

			inline static constexpr StorageType encode(Sign sign, BiasedExponent exp, Significand sig)
			{
				if (sign.is_neg()) { return SIGN_MASK | encode(exp, sig); }
				return encode(exp, sig);
			}

			// The floating point number representation as an unsigned integer.
			StorageType bits{}; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

			inline constexpr FPStorage() : bits(0) {}
			inline constexpr FPStorage(StorageType value) : bits(value) {}

			// Observers
			inline constexpr StorageType exp_bits() const { return bits & EXP_MASK; }
			inline constexpr StorageType sig_bits() const { return bits & SIG_MASK; }
			inline constexpr StorageType exp_sig_bits() const { return bits & EXP_SIG_MASK; }

			// Parts
			inline constexpr BiasedExponent biased_exponent() const { return BiasedExponent(static_cast<uint32_t>(exp_bits() >> SIG_LEN)); }
			inline constexpr void set_biased_exponent(BiasedExponent biased) { bits = merge(bits, encode(biased), EXP_MASK); }

		public:
			inline constexpr Sign sign() const { return (bits & SIGN_MASK) ? Sign::negative : Sign::positive; }
			inline constexpr void set_sign(Sign signVal)
			{
				if (sign() != signVal) { bits ^= SIGN_MASK; }
			}
		};

		// This layer defines all functions that are specific to how the the floating
		// point type is encoded. It enables constructions, modification and observation
		// of values manipulated as 'StorageType'.
		template <FPType fp_type, typename RetT>
		struct FPRepSem : public FPStorage<fp_type>
		{
			using UP = FPStorage<fp_type>;
			using typename UP::StorageType;
			using UP::FRACTION_LEN;
			using UP::FRACTION_MASK;

		protected:
			using typename UP::Exponent;
			using typename UP::Significand;
			using UP::bits;
			using UP::encode;
			using UP::exp_bits;
			using UP::exp_sig_bits;
			using UP::sig_bits;
			using UP::UP;

		public:
			// Builders
			inline static constexpr RetT zero(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::subnormal(), Significand::zero())); }
			inline static constexpr RetT one(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::zero(), Significand::zero())); }
			inline static constexpr RetT min_subnormal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::subnormal(), Significand::lsb())); }
			inline static constexpr RetT max_subnormal(Sign sign = Sign::positive)
			{
				return RetT(encode(sign, Exponent::subnormal(), Significand::bits_all_ones()));
			}
			inline static constexpr RetT min_normal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::min(), Significand::zero())); }
			inline static constexpr RetT max_normal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::max(), Significand::bits_all_ones())); }
			inline static constexpr RetT inf(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::inf(), Significand::zero())); }
			inline static constexpr RetT signaling_nan(Sign sign = Sign::positive, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), (v ? Significand(v) : (Significand::msb() >> 1))));
			}
			inline static constexpr RetT quiet_nan(Sign sign = Sign::positive, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | Significand(v)));
			}

			// Observers
			[[nodiscard]] inline constexpr bool is_zero() const { return exp_sig_bits() == 0; }
			[[nodiscard]] inline constexpr bool is_nan() const { return exp_sig_bits() > encode(Exponent::inf(), Significand::zero()); }
			[[nodiscard]] inline constexpr bool is_quiet_nan() const { return exp_sig_bits() >= encode(Exponent::inf(), Significand::msb()); }
			[[nodiscard]] inline constexpr bool is_signaling_nan() const { return is_nan() && !is_quiet_nan(); }
			[[nodiscard]] inline constexpr bool is_inf() const { return exp_sig_bits() == encode(Exponent::inf(), Significand::zero()); }
			[[nodiscard]] inline constexpr bool is_finite() const { return exp_bits() != encode(Exponent::inf()); }
			[[nodiscard]] inline constexpr bool is_subnormal() const { return exp_bits() == encode(Exponent::subnormal()); }
			[[nodiscard]] inline constexpr bool is_normal() const { return is_finite() && !is_subnormal(); }
			[[nodiscard]] inline constexpr RetT next_toward_inf() const
			{
				if (is_finite()) { return RetT(bits + StorageType(1)); }
				return RetT(bits);
			}

			// Returns the mantissa with the implicit bit set iff the current
			// value is a valid normal number.
			inline constexpr StorageType get_explicit_mantissa() const
			{
				if (is_subnormal()) { return sig_bits(); }
				return (StorageType(1) << UP::SIG_LEN) | sig_bits();
			}
		};

		// Specialization for the X86 Extended Precision type.
		template <typename RetT>
		struct FPRepSem<FPType::eBinary80_x86, RetT> : public FPStorage<FPType::eBinary80_x86>
		{
			using UP = FPStorage<FPType::eBinary80_x86>;
			using typename UP::StorageType;
			using UP::FRACTION_LEN;
			using UP::FRACTION_MASK;

			// The x86 80 bit float represents the leading digit of the mantissa
			// explicitly. This is the mask for that bit.
			static constexpr StorageType EXPLICIT_BIT_MASK = static_cast<StorageType>(1) << FRACTION_LEN;
			// The X80 significand is made of an explicit bit and the fractional part.
			static_assert((EXPLICIT_BIT_MASK & FRACTION_MASK) == 0, "the explicit bit and the fractional part should not overlap");
			static_assert((EXPLICIT_BIT_MASK | FRACTION_MASK) == SIG_MASK, "the explicit bit and the fractional part should cover the "
																		   "whole significand");

		protected:
			using typename UP::Exponent;
			using typename UP::Significand;
			using UP::encode;
			using UP::UP;

		public:
			// Builders
			inline static constexpr RetT zero(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::subnormal(), Significand::zero())); }
			inline static constexpr RetT one(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::zero(), Significand::msb())); }
			inline static constexpr RetT min_subnormal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::subnormal(), Significand::lsb())); }
			inline static constexpr RetT max_subnormal(Sign sign = Sign::positive)
			{
				return RetT(encode(sign, Exponent::subnormal(), Significand::bits_all_ones() ^ Significand::msb()));
			}
			inline static constexpr RetT min_normal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::min(), Significand::msb())); }
			inline static constexpr RetT max_normal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::max(), Significand::bits_all_ones())); }
			inline static constexpr RetT inf(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::inf(), Significand::msb())); }
			inline static constexpr RetT signaling_nan(Sign sign = Sign::positive, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | (v ? Significand(v) : (Significand::msb() >> 2))));
			}
			inline static constexpr RetT quiet_nan(Sign sign = Sign::positive, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | (Significand::msb() >> 1) | Significand(v)));
			}

			// Observers
			[[nodiscard]] inline constexpr bool is_zero() const { return exp_sig_bits() == 0; }
			inline constexpr bool is_nan() const
			{
				// Most encoding forms from the table found in
				// https://en.wikipedia.org/wiki/Extended_precision#x86_extended_precision_format
				// are interpreted as NaN.
				// More precisely :
				// - Pseudo-Infinity
				// - Pseudo Not a Number
				// - Signalling Not a Number
				// - Floating-point Indefinite
				// - Quiet Not a Number
				// - Unnormal
				// This can be reduced to the following logic:
				if (exp_bits() == encode(Exponent::inf())) { return !is_inf(); }
				if (exp_bits() != encode(Exponent::subnormal())) { return (sig_bits() & encode(Significand::msb())) == 0; }
				return false;
			}
			[[nodiscard]] inline constexpr bool is_quiet_nan() const
			{
				return exp_sig_bits() >= encode(Exponent::inf(), Significand::msb() | (Significand::msb() >> 1));
			}
			[[nodiscard]] inline constexpr bool is_signaling_nan() const { return is_nan() && !is_quiet_nan(); }
			[[nodiscard]] inline constexpr bool is_inf() const { return exp_sig_bits() == encode(Exponent::inf(), Significand::msb()); }
			[[nodiscard]] inline constexpr bool is_finite() const { return !is_inf() && !is_nan(); }
			[[nodiscard]] inline constexpr bool is_subnormal() const { return exp_bits() == encode(Exponent::subnormal()); }
			[[nodiscard]] inline constexpr bool is_normal() const
			{
				const auto exp = exp_bits();
				if (exp == encode(Exponent::subnormal()) || exp == encode(Exponent::inf())) { return false; }
				return get_implicit_bit();
			}
			inline constexpr RetT next_toward_inf() const
			{
				if (is_finite())
				{
					if (exp_sig_bits() == max_normal().uintval()) { return inf(sign()); }
					if (exp_sig_bits() == max_subnormal().uintval()) { return min_normal(sign()); }
					if (sig_bits() == SIG_MASK) { return RetT(encode(sign(), ++biased_exponent(), Significand::zero())); }
					return RetT(bits + static_cast<StorageType>(1));
				}
				return RetT(bits);
			}

			[[nodiscard]] inline constexpr StorageType get_explicit_mantissa() const { return sig_bits(); }

			// This functions is specific to FPRepSem<FPType::eBinary80_x86>.
			inline constexpr bool get_implicit_bit() const { return static_cast<bool>(bits & EXPLICIT_BIT_MASK); }

			// This functions is specific to FPRepSem<FPType::eBinary80_x86>.
			inline constexpr void set_implicit_bit(bool implicitVal)
			{
				if (get_implicit_bit() != implicitVal) { bits ^= EXPLICIT_BIT_MASK; }
			}
		};

		// 'FPRepImpl' is the bottom of the class hierarchy that only deals with
		// 'FPType'. The operations dealing with specific float semantics are
		// implemented by 'FPRepSem' above and specialized when needed.
		//
		// The 'RetT' type is being propagated up to 'FPRepSem' so that the functions
		// creating new values (Builders) can return the appropriate type. That is, when
		// creating a value through 'FPBits' below the builder will return an 'FPBits'
		// value.
		// FPBits<float>::zero(); // returns an FPBits<>
		//
		// When we don't care about specific C++ floating point type we can use
		// 'FPRep' and specify the 'FPType' directly.
		// FPRep<FPType::eBinary32:>::zero() // returns an FPRep<>
		template <FPType fp_type, typename RetT>
		struct FPRepImpl : public FPRepSem<fp_type, RetT>
		{
			using UP		  = FPRepSem<fp_type, RetT>;
			using StorageType = typename UP::StorageType;

		protected:
			using UP::bits;
			using UP::encode;
			using UP::exp_bits;
			using UP::exp_sig_bits;

			using typename UP::BiasedExponent;
			using typename UP::Exponent;
			using typename UP::Significand;

			using UP::FP_MASK;

		public:
			// Constants.
			using UP::EXP_BIAS;
			using UP::EXP_MASK;
			using UP::FRACTION_MASK;
			using UP::SIG_LEN;
			using UP::SIG_MASK;
			using UP::SIGN_MASK;
			inline static constexpr int MAX_BIASED_EXPONENT = (1 << UP::EXP_LEN) - 1;

			// CTors
			inline constexpr FPRepImpl() = default;
			inline constexpr explicit FPRepImpl(StorageType x) : UP(x) {}

			// Comparison
			inline constexpr friend bool operator==(FPRepImpl a, FPRepImpl b) { return a.uintval() == b.uintval(); }
			inline constexpr friend bool operator!=(FPRepImpl a, FPRepImpl b) { return a.uintval() != b.uintval(); }

			// Representation
			inline constexpr StorageType uintval() const { return bits & FP_MASK; }
			inline constexpr void set_uintval(StorageType value) { bits = (value & FP_MASK); }

			// Builders
			using UP::inf;
			using UP::max_normal;
			using UP::max_subnormal;
			using UP::min_normal;
			using UP::min_subnormal;
			using UP::one;
			using UP::quiet_nan;
			using UP::signaling_nan;
			using UP::zero;

			// Modifiers
			inline constexpr RetT abs() const { return RetT(bits & UP::EXP_SIG_MASK); }

			// Observers
			using UP::get_explicit_mantissa;
			using UP::is_finite;
			using UP::is_inf;
			using UP::is_nan;
			using UP::is_normal;
			using UP::is_quiet_nan;
			using UP::is_signaling_nan;
			using UP::is_subnormal;
			using UP::is_zero;
			using UP::next_toward_inf;
			using UP::sign;
			inline constexpr bool is_inf_or_nan() const { return !is_finite(); }
			inline constexpr bool is_neg() const { return sign().is_neg(); }
			inline constexpr bool is_pos() const { return sign().is_pos(); }

			inline constexpr uint16_t get_biased_exponent() const { return static_cast<uint16_t>(static_cast<uint32_t>(UP::biased_exponent())); }

			inline constexpr void set_biased_exponent(StorageType biased) { UP::set_biased_exponent(BiasedExponent((int32_t)biased)); }

			inline constexpr int get_exponent() const { return static_cast<int32_t>(Exponent(UP::biased_exponent())); }

			// If the number is subnormal, the exponent is treated as if it were the
			// minimum exponent for a normal number. This is to keep continuity between
			// the normal and subnormal ranges, but it causes problems for functions where
			// values are calculated from the exponent, since just subtracting the bias
			// will give a slightly incorrect result. Additionally, zero has an exponent
			// of zero, and that should actually be treated as zero.
			inline constexpr int get_explicit_exponent() const
			{
				Exponent exponent(UP::biased_exponent());
				if (is_zero()) exponent = Exponent::zero();
				if (exponent == Exponent::subnormal()) exponent = Exponent::min();
				return static_cast<int32_t>(exponent);
			}

			inline constexpr StorageType get_mantissa() const { return bits & FRACTION_MASK; }

			inline constexpr void set_mantissa(StorageType mantVal) { bits = UP::merge(bits, mantVal, FRACTION_MASK); }

			inline constexpr void set_significand(StorageType sigVal) { bits = UP::merge(bits, sigVal, SIG_MASK); }
			// Unsafe function to create a floating point representation.
			// It simply packs the sign, biased exponent and mantissa values without
			// checking bound nor normalization.
			//
			// WARNING: For X86 Extended Precision, implicit bit needs to be set correctly
			// in the 'mantissa' by the caller.  This function will not check for its
			// validity.
			//
			// FIXME: Use an uint32_t for 'biased_exp'.
			inline static constexpr RetT create_value(Sign sign, StorageType biased_exp, StorageType mantissa)
			{
				return RetT(encode(sign, BiasedExponent(static_cast<uint32_t>(biased_exp)), Significand(mantissa)));
			}

			// The function converts integer number and unbiased exponent to proper
			// float T type:
			//   Result = number * 2^(ep+1 - exponent_bias)
			// Be careful!
			//   1) "ep" is the raw exponent value.
			//   2) The function adds +1 to ep for seamless normalized to denormalized
			//      transition.
			//   3) The function does not check exponent high limit.
			//   4) "number" zero value is not processed correctly.
			//   5) Number is unsigned, so the result can be only positive.
			inline static constexpr RetT make_value(StorageType number, int ep)
			{
				FPRepImpl result(0);
				int lz = UP::FRACTION_LEN + 1 - (UP::STORAGE_LEN - ccm::support::countl_zero(number));

				number <<= lz;
				ep -= lz;

				if (CCM_LIKELY(ep >= 0))
				{
					// Implicit number bit will be removed by mask
					result.set_significand(number);
					result.set_biased_exponent(ep + 1);
				}
				else { result.set_significand(number >> -ep); }
				return RetT(result.uintval());
			}
		};

		// A generic class to manipulate floating point formats.
		// It derives its functionality to FPRepImpl above.
		template <FPType fp_type>
		struct FPRep : public FPRepImpl<fp_type, FPRep<fp_type>>
		{
			using UP		  = FPRepImpl<fp_type, FPRep<fp_type>>;
			using StorageType = typename UP::StorageType;
			using UP::UP;

			inline constexpr explicit operator StorageType() const { return UP::uintval(); }
		};

	} // namespace internal

	// Returns the FPType corresponding to C++ type T on the host.
	template <typename T>
	inline static constexpr FPType get_fp_type()
	{
		using UnqualT = std::remove_cv_t<T>;
		if constexpr (std::is_same_v<UnqualT, float> && __FLT_MANT_DIG__ == 24) { return FPType::eBinary32; }
		else if constexpr (std::is_same_v<UnqualT, double> && __DBL_MANT_DIG__ == 53) { return FPType::eBinary64; }
		else if constexpr (std::is_same_v<UnqualT, long double>)
		{
			if constexpr (__LDBL_MANT_DIG__ == 53) { return FPType::eBinary64; }
			else if constexpr (__LDBL_MANT_DIG__ == 64) { return FPType::eBinary80_x86; }
			else if constexpr (__LDBL_MANT_DIG__ == 113) { return FPType::eBinary128; }
		}

#if defined(CCM_HAS_FLOAT128)
		else if constexpr (std::is_same_v<UnqualT, float128>)
		{
			return FPType::eBinary128;
#endif
		}
		else { static_assert(ccm::support::always_false<UnqualT>, "Unsupported type"); }
	}

	// A generic class to manipulate C++ floating point formats.
	// It derives its functionality to FPRepImpl above.
	template <typename T>
	struct FPBits final : public internal::FPRepImpl<get_fp_type<T>(), FPBits<T>>
	{
		static_assert(std::is_floating_point_v<T>, "FPBits instantiated with invalid type.");
		using UP		  = internal::FPRepImpl<get_fp_type<T>(), FPBits<T>>;
		using StorageType = typename UP::StorageType;

		// Constructors.
		inline constexpr FPBits() = default;

		template <typename XType>
		inline constexpr explicit FPBits(XType x)
		{
			using Unqual = typename std::remove_cv_t<XType>;
			if constexpr (std::is_same_v<Unqual, T>) { UP::bits = ccm::support::bit_cast<StorageType>(x); }
			else if constexpr (std::is_same_v<Unqual, StorageType>) { UP::bits = x; }
			else
			{
				// We don't want accidental type promotions/conversions, so we require
				// exact type match.
				static_assert(ccm::support::always_false<XType>);
			}
		}

		// Floating-point conversions.
		inline constexpr T get_val() const { return ccm::support::bit_cast<T>(UP::bits); }
	};

} // namespace ccm::support
