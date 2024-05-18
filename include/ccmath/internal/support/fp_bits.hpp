/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// Large parts of this file is directly copied from LLVM and modified to fit the needs of ccmath.

#pragma once

#include "ccmath/internal/predef/likely.hpp"
#include "ccmath/internal/support/always_false.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/types/sign.hpp"

#include <cfloat>
#include <climits>
#include <cstdint>
#include <type_traits>

namespace ccm::fputil
{

	// Create a bitmask with the count right-most bits set to 1, and all other bits
	// set to 0.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T> mask_trailing_ones()
	{
		constexpr unsigned T_BITS = CHAR_BIT * sizeof(T);
		static_assert(count <= T_BITS && "Invalid bit index");
		return count == 0 ? 0 : (T(-1) >> (T_BITS - count));
	}

	// Create a bitmask with the count left-most bits set to 1, and all other bits
	// set to 0.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T> mask_leading_ones()
	{
		return T(~mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>());
	}

	// Create a bitmask with the count right-most bits set to 0, and all other bits
	// set to 1.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T> mask_trailing_zeros()
	{
		return mask_leading_ones<T, CHAR_BIT * sizeof(T) - count>();
	}

	// Create a bitmask with the count left-most bits set to 0, and all other bits
	// set to 1.  Only unsigned types are allowed.
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T> mask_leading_zeros()
	{
		return mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>();
	}

	// The supported floating point types.
	enum class FPType : std::uint8_t
	{
		eBinary32,
		eBinary64,
		eUnknown
	};

	namespace internal
	{

		// Defines the layout (sign, exponent, significand) of a floating point type in
		// memory. It also defines its associated StorageType, i.e., the unsigned
		// integer type used to manipulate its representation.
		// Additionally we provide the fractional part length, i.e., the number of bits
		// after the decimal dot when the number is in normal form.
		template <FPType>
		struct FPLayout
		{
		};

		template <>
		struct FPLayout<FPType::eBinary32>
		{
			using StorageType						= std::uint32_t;
			static constexpr int sign_length		= 1;
			static constexpr int exponent_length	= 8;
			static constexpr int significand_length = 23;
			static constexpr int fraction_length	= significand_length;
		};

		template <>
		struct FPLayout<FPType::eBinary64>
		{
			using StorageType						= std::uint64_t;
			static constexpr int sign_length		= 1;
			static constexpr int exponent_length	= 11;
			static constexpr int significand_length = 52;
			static constexpr int fraction_length	= significand_length;
		};

		// FPStorage derives useful constants from the FPLayout above.
		template <FPType fp_type>
		struct FPStorage : FPLayout<fp_type>
		{
			using UP = FPLayout<fp_type>;

			using UP::exponent_length;	  // The number of bits for the *exponent* part
			using UP::sign_length;		  // The number of bits for the *sign* part
										  // For convenience, the sum of `significand_length`, `exponent_length`, and `sign_length`.
			using UP::significand_length; // The number of bits for the *significand* part
			static constexpr int total_length = sign_length + exponent_length + significand_length;

			// The number of bits after the decimal dot when the number is in normal form.
			using UP::fraction_length;

			// An unsigned integer that is wide enough to contain all of the floating
			// point bits.
			using StorageType = typename UP::StorageType;

			// The number of bits in StorageType.
			static constexpr int storage_length = sizeof(StorageType) * CHAR_BIT;
			static_assert(storage_length >= total_length);

			// The exponent bias. Always positive.
			static constexpr std::int32_t exponent_bias = (1U << (exponent_length - 1U)) - 1U;
			static_assert(exponent_bias > 0);

			// The bit pattern that keeps only the *significand* part.
			static constexpr StorageType significand_mask = mask_trailing_ones<StorageType, significand_length>();
			// The bit pattern that keeps only the *exponent* part.
			static constexpr StorageType exponent_mask = mask_trailing_ones<StorageType, exponent_length>() << significand_length;
			// The bit pattern that keeps only the *sign* part.
			static constexpr StorageType sign_mask = mask_trailing_ones<StorageType, sign_length>() << (exponent_length + significand_length);
			// The bit pattern that keeps only the *exponent + significand* part.
			static constexpr StorageType exponent_significand_mask = mask_trailing_ones<StorageType, exponent_length + significand_length>();
			// The bit pattern that keeps only the *sign + exponent + significand* part.
			static constexpr StorageType fp_mask = mask_trailing_ones<StorageType, total_length>();
			// The bit pattern that keeps only the *fraction* part.
			// i.e., the *significand* without the leading one.
			static constexpr StorageType fraction_mask = mask_trailing_ones<StorageType, fraction_length>();

			static_assert((significand_mask & exponent_mask & sign_mask) == 0, "masks disjoint");
			static_assert((significand_mask | exponent_mask | sign_mask) == fp_mask, "masks cover");

		protected:
			// Merge bits from 'a' and 'b' values according to 'mask'.
			// Use 'a' bits when corresponding 'mask' bits are zeroes and 'b' bits when
			// corresponding bits are ones.
			static constexpr StorageType merge(StorageType a, StorageType b, StorageType mask)
			{
				// https://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
				return a ^ ((a ^ b) & mask);
			}

			// A stongly typed integer that prevents mixing and matching integers with
			// different semantics.
			template <typename T>
			// NOLINTNEXTLINE(hicpp-special-member-functions,cppcoreguidelines-special-member-functions)
			struct TypedInt
			{
				using value_type = T;
				constexpr explicit TypedInt(T value) : value(value) {}
				constexpr TypedInt(const TypedInt & value)			   = default;
				constexpr TypedInt & operator=(const TypedInt & value) = default;

				constexpr explicit operator T() const { return value; }

				constexpr StorageType to_storage_type() const { return StorageType(value); }

				friend constexpr bool operator==(TypedInt a, TypedInt b) { return a.value == b.value; }
				friend constexpr bool operator!=(TypedInt a, TypedInt b) { return a.value != b.value; }

			private:
				T value;
			};

			// An opaque type to store a floating point exponent.
			// We define special values but it is valid to create arbitrary values as long
			// as they are in the range [min, max].
			struct Exponent : TypedInt<std::int32_t>
			{
				using UP = TypedInt<std::int32_t>;
				using UP::UP;
				static constexpr auto subnormal() { return Exponent(-exponent_bias); }
				static constexpr auto min() { return Exponent(1 - exponent_bias); }
				static constexpr auto zero() { return Exponent(0); }
				static constexpr auto max() { return Exponent(exponent_bias); }
				static constexpr auto inf() { return Exponent(exponent_bias + 1); }
			};

			// An opaque type to store a floating point biased exponent.
			// We define special values but it is valid to create arbitrary values as long
			// as they are in the range [zero, bits_all_ones].
			// Values greater than bits_all_ones are truncated.
			struct BiasedExponent : TypedInt<std::uint32_t>
			{
				using UP = TypedInt<std::uint32_t>;
				using UP::UP;

				constexpr BiasedExponent(Exponent exp) : UP(static_cast<std::int32_t>(exp) + exponent_bias) {}

				// Cast operator to get convert from BiasedExponent to Exponent.
				constexpr operator Exponent() const { return Exponent(UP::value - exponent_bias); }

				constexpr BiasedExponent & operator++()
				{
					assert(*this != BiasedExponent(Exponent::inf()));
					++UP::value;
					return *this;
				}

				constexpr BiasedExponent & operator--()
				{
					assert(*this != BiasedExponent(Exponent::subnormal()));
					--UP::value;
					return *this;
				}
			};

			struct Significand : TypedInt<StorageType>
			{
				using UP = TypedInt<StorageType>;
				using UP::UP;

				friend constexpr Significand operator|(const Significand a, const Significand b)
				{
					return Significand(StorageType(a.to_storage_type() | b.to_storage_type()));
				}
				friend constexpr Significand operator^(const Significand a, const Significand b)
				{
					return Significand(StorageType(a.to_storage_type() ^ b.to_storage_type()));
				}
				friend constexpr Significand operator>>(const Significand a, int shift) { return Significand(StorageType(a.to_storage_type() >> shift)); }

				static constexpr auto zero() { return Significand(StorageType(0)); }
				static constexpr auto lsb() { return Significand(StorageType(1)); }
				static constexpr auto msb() { return Significand(StorageType(1) << (significand_length - 1)); }
				static constexpr auto bits_all_ones() { return Significand(significand_mask); }
			};

			static constexpr StorageType encode(BiasedExponent exp) { return (exp.to_storage_type() << significand_length) & exponent_mask; }

			static constexpr StorageType encode(Significand value) { return value.to_storage_type() & significand_mask; }

			static constexpr StorageType encode(BiasedExponent exp, Significand sig) { return encode(exp) | encode(sig); }

			static constexpr StorageType encode(Sign sign, BiasedExponent exp, Significand sig)
			{
				if (sign.is_neg()) { return sign_mask | encode(exp, sig); }
				return encode(exp, sig);
			}

			// The floating point number representation as an unsigned integer.
			StorageType bits{};

			constexpr FPStorage() : bits(0) {}
			constexpr FPStorage(StorageType value) : bits(value) {}

			// Observers
			constexpr StorageType exp_bits() const { return bits & exponent_mask; }
			constexpr StorageType sig_bits() const { return bits & significand_mask; }
			constexpr StorageType exp_sig_bits() const { return bits & exponent_significand_mask; }

			// Parts
			constexpr BiasedExponent biased_exponent() const { return BiasedExponent(static_cast<std::uint32_t>(exp_bits() >> significand_length)); }
			constexpr void set_biased_exponent(const BiasedExponent biased) { bits = merge(bits, encode(biased), exponent_mask); }

		public:
			[[nodiscard]] constexpr Sign sign() const { return (bits & sign_mask) ? Sign::negative : Sign::positive; }
			constexpr void set_sign(const Sign signVal)
			{
				if (sign() != signVal) { bits ^= sign_mask; }
			}
		};

		// This layer defines all functions that are specific to how the the floating
		// point type is encoded. It enables constructions, modification and observation
		// of values manipulated as 'StorageType'.
		template <FPType fp_type, typename RetT>
		struct FPRepSem : FPStorage<fp_type>
		{
			using UP = FPStorage<fp_type>;
			using typename UP::StorageType;
			using UP::fraction_length;
			using UP::fraction_mask;

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
			static constexpr RetT zero(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::subnormal(), Significand::zero())); }
			static constexpr RetT one(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::zero(), Significand::zero())); }
			static constexpr RetT min_subnormal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::subnormal(), Significand::lsb())); }
			static constexpr RetT max_subnormal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::subnormal(), Significand::bits_all_ones())); }
			static constexpr RetT min_normal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::min(), Significand::zero())); }
			static constexpr RetT max_normal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::max(), Significand::bits_all_ones())); }
			static constexpr RetT inf(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::inf(), Significand::zero())); }
			static constexpr RetT signaling_nan(Sign sign = Sign::positive, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), (v ? Significand(v) : (Significand::msb() >> 1))));
			}
			static constexpr RetT quiet_nan(Sign sign = Sign::positive, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | Significand(v)));
			}

			// Observers
			[[nodiscard]] constexpr bool is_zero() const { return exp_sig_bits() == 0; }
			[[nodiscard]] constexpr bool is_nan() const { return exp_sig_bits() > encode(Exponent::inf(), Significand::zero()); }
			[[nodiscard]] constexpr bool is_quiet_nan() const { return exp_sig_bits() >= encode(Exponent::inf(), Significand::msb()); }
			[[nodiscard]] constexpr bool is_signaling_nan() const { return is_nan() && !is_quiet_nan(); }
			[[nodiscard]] constexpr bool is_inf() const { return exp_sig_bits() == encode(Exponent::inf(), Significand::zero()); }
			[[nodiscard]] constexpr bool is_finite() const { return exp_bits() != encode(Exponent::inf()); }

			[[nodiscard]] constexpr bool is_subnormal() const { return exp_bits() == encode(Exponent::subnormal()); }
			[[nodiscard]] constexpr bool is_normal() const { return is_finite() && !is_subnormal(); }
			constexpr RetT next_toward_inf() const
			{
				if (is_finite()) { return RetT(bits + StorageType(1)); }
				return RetT(bits);
			}

			// Returns the mantissa with the implicit bit set iff the current
			// value is a valid normal number.
			constexpr StorageType get_explicit_mantissa() const
			{
				if (is_subnormal()) { return sig_bits(); }
				return (StorageType(1) << UP::significand_length) | sig_bits();
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
		struct FPRepImpl : FPRepSem<fp_type, RetT>
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

			using UP::fp_mask;

		public:
			// Constants.
			using UP::exponent_bias;
			using UP::exponent_mask;
			using UP::fraction_mask;
			using UP::sign_mask;
			using UP::significand_length;
			using UP::significand_mask;
			static constexpr int max_biased_exponent = (1 << UP::exponent_length) - 1;

			// CTors
			constexpr FPRepImpl() = default;
			constexpr explicit FPRepImpl(StorageType x) : UP(x) {}

			// Comparison
			constexpr friend bool operator==(FPRepImpl a, FPRepImpl b) { return a.uintval() == b.uintval(); }
			constexpr friend bool operator!=(FPRepImpl a, FPRepImpl b) { return a.uintval() != b.uintval(); }

			// Representation
			constexpr StorageType uintval() const { return bits & fp_mask; }
			constexpr void set_uintval(StorageType value) { bits = (value & fp_mask); }

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
			constexpr RetT abs() const { return RetT(bits & UP::exponent_significand_mask); }

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
			[[nodiscard]] constexpr bool is_inf_or_nan() const { return !is_finite(); }
			[[nodiscard]] constexpr bool is_neg() const { return sign().is_neg(); }
			[[nodiscard]] constexpr bool is_pos() const { return sign().is_pos(); }

			[[nodiscard]] constexpr std::uint16_t get_biased_exponent() const
			{
				return static_cast<std::uint16_t>(static_cast<std::uint32_t>(UP::biased_exponent()));
			}

			constexpr void set_biased_exponent(StorageType biased) { UP::set_biased_exponent(BiasedExponent(static_cast<std::int32_t>(biased))); }

			[[nodiscard]] constexpr int get_exponent() const { return static_cast<std::int32_t>(Exponent(UP::biased_exponent())); }

			// If the number is subnormal, the exponent is treated as if it were the
			// minimum exponent for a normal number. This is to keep continuity between
			// the normal and subnormal ranges, but it causes problems for functions where
			// values are calculated from the exponent, since just subtracting the bias
			// will give a slightly incorrect result. Additionally, zero has an exponent
			// of zero, and that should actually be treated as zero.
			[[nodiscard]] constexpr int get_explicit_exponent() const
			{
				Exponent exponent(UP::biased_exponent());
				if (is_zero()) { exponent = Exponent::zero(); }
				if (exponent == Exponent::subnormal()) { exponent = Exponent::min(); }
				return static_cast<std::int32_t>(exponent);
			}

			constexpr StorageType get_mantissa() const { return bits & fraction_mask; }

			constexpr void set_mantissa(StorageType mantVal) { bits = UP::merge(bits, mantVal, fraction_mask); }

			constexpr void set_significand(StorageType sigVal) { bits = UP::merge(bits, sigVal, significand_mask); }
			// Unsafe function to create a floating point representation.
			// It simply packs the sign, biased exponent and mantissa values without
			// checking bound nor normalization.
			//
			// WARNING: For X86 Extended Precision, implicit bit needs to be set correctly
			// in the 'mantissa' by the caller.  This function will not check for its
			// validity.
			//
			// FIXME: Use an uint32_t for 'biased_exp'.
			static constexpr RetT create_value(Sign sign, StorageType biased_exp, StorageType mantissa)
			{
				return RetT(encode(sign, BiasedExponent(static_cast<std::uint32_t>(biased_exp)), Significand(mantissa)));
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
			static constexpr RetT make_value(StorageType number, int ep)
			{
				FPRepImpl result(0);
				int lz = UP::fraction_length + 1 - (UP::storage_length - support::countl_zero(number));

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
		struct FPRep : FPRepImpl<fp_type, FPRep<fp_type>>
		{
			using UP		  = FPRepImpl<fp_type, FPRep<fp_type>>;
			using StorageType = typename UP::StorageType;
			using UP::UP;

			constexpr explicit operator StorageType() const { return UP::uintval(); }
		};

	} // namespace internal

	// Returns the FPType corresponding to C++ type T on the host.
	template <typename T>
	static constexpr FPType get_fp_type()
	{
		using UnqualT = std::remove_cv_t<T>;
		// NOLINTBEGIN(bugprone-branch-clone)
		if constexpr (std::is_same_v<UnqualT, float> && FLT_MANT_DIG == 24) { return FPType::eBinary32; }
		else if constexpr (std::is_same_v<UnqualT, double> && DBL_MANT_DIG == 53) { return FPType::eBinary64; }
		else if constexpr (std::is_same_v<UnqualT, long double>)
		{
			return FPType::eBinary64; // Remove this when the below code is uncommented.
									  /* TODO: Currently we do not support long double, but may in the future.
									  if constexpr (LDBL_MANT_DIG == 53)
										  return FPType::eBinary64;
									  else if constexpr (LDBL_MANT_DIG == 64)
										  return FPType::eBinary80;
									  else if constexpr (LDBL_MANT_DIG == 113)
										  return FPType::eBinary128;
									  */
		}
#if defined(CCM_TYPES_HAS_FLOAT128)
		else if constexpr (std::is_same_v<UnqualT, float128>) { return FPType::IEEE754_Binary128; }
#endif
		else
		{
			static_assert(ccm::support::always_false<UnqualT>, "Unsupported type");
			return FPType::eUnknown;
		}
		// NOLINTEND(bugprone-branch-clone)
	}

	// A generic class to manipulate C++ floating point formats.
	// It derives its functionality to FPRepImpl above.
	template <typename T>
	struct FPBits final : internal::FPRepImpl<get_fp_type<T>(), FPBits<T>>
	{
		static_assert(std::is_floating_point_v<T>, "FPBits instantiated with invalid type.");
		using UP		  = internal::FPRepImpl<get_fp_type<T>(), FPBits<T>>;
		using StorageType = typename UP::StorageType;

		// Constructors.
		constexpr FPBits() = default;

		template <typename XType>
		constexpr explicit FPBits(XType x)
		{
			using Unqual = std::remove_cv_t<XType>;
			if constexpr (std::is_same_v<Unqual, T>) { UP::bits = support::bit_cast<StorageType>(x); }
			else if constexpr (std::is_same_v<Unqual, StorageType>) { UP::bits = x; }
			else
			{
				// We don't want accidental type promotions/conversions, so we require
				// exact type match.
				static_assert(ccm::support::always_false<XType>);
			}
		}

		// Floating-point conversions.
		constexpr T get_val() const { return support::bit_cast<T>(UP::bits); }
	};

} // namespace ccm::fputil
