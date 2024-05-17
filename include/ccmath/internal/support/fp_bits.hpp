/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// Large parts of this file is directly copied from LLVM and modified to fit the needs of ccmath.

#pragma once

#include "ccmath/internal/types/sign.hpp"
#include "ccmath/internal/predef/likely.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/always_false.hpp"
#include "ccmath/internal/support/bits.hpp"

#include <climits>
#include <cstdint>
#include <type_traits>

namespace ccm::fputil
{

	// Create a bitmask with the count right-most bits set to 1, and all other bits
	// set to 0.  Only unsigned types are allowed.
	template <typename T, size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T> mask_trailing_ones()
	{
		constexpr unsigned T_BITS = CHAR_BIT * sizeof(T);
		static_assert(count <= T_BITS && "Invalid bit index");
		return count == 0 ? 0 : (T(-1) >> (T_BITS - count));
	}

	// Create a bitmask with the count left-most bits set to 1, and all other bits
	// set to 0.  Only unsigned types are allowed.
	template <typename T, size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T> mask_leading_ones()
	{
		return T(~mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>());
	}

	// Create a bitmask with the count right-most bits set to 0, and all other bits
	// set to 1.  Only unsigned types are allowed.
	template <typename T, size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T> mask_trailing_zeros()
	{
		return mask_leading_ones<T, CHAR_BIT * sizeof(T) - count>();
	}

	// Create a bitmask with the count left-most bits set to 0, and all other bits
	// set to 1.  Only unsigned types are allowed.
	template <typename T, size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T> mask_leading_zeros()
	{
		return mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>();
	}

	// The supported floating point types.
	enum class FPType : std::uint8_t
	{
		IEEE754_Binary32,
		IEEE754_Binary64,

		// These values are not yet implemented.
		// IEEE754_Binary128,
		// X86_Binary80,
	};

	// The classes hierarchy is as follows:
	//
	//             ┌───────────────────┐
	//             │ FPLayout<FPType>  │
	//             └─────────▲─────────┘
	//                       │
	//             ┌─────────┴─────────┐
	//             │ FPStorage<FPType> │
	//             └─────────▲─────────┘
	//                       │
	//          ┌────────────┴─────────────┐
	//          │                          │
	// ┌────────┴─────────┐ ┌──────────────┴──────────────────┐
	// │ FPRepSem<FPType> │ │  FPRepSem<FPType::X86_Binary80  │
	// └────────▲─────────┘ └──────────────▲──────────────────┘
	//          │                          │
	//          └────────────┬─────────────┘
	//                       │
	//               ┌───────┴───────┐
	//               │  FPRepImpl<T> │
	//               └───────▲───────┘
	//                       │
	//              ┌────────┴────────┐
	//        ┌─────┴─────┐     ┌─────┴─────┐
	//        │  FPRep<T> │     │ FPBits<T> │
	//        └───────────┘     └───────────┘
	//
	// - 'FPLayout' defines only a few constants, namely the 'StorageType' and
	//   length of the sign, the exponent, fraction and significand parts.
	// - 'FPStorage' builds more constants on top of those from 'FPLayout' like
	//   exponent bias and masks. It also holds the bit representation of the
	//   floating point as a 'StorageType' type and defines tools to assemble or
	//   test these parts.
	// - 'FPRepSem' defines functions to interact semantically with the floating
	//   point representation. The default implementation is the one for 'IEEE754',
	//   a specialization is provided for X86 Extended Precision.
	// - 'FPRepImpl' derives from 'FPRepSem' and adds functions that are common to
	//   all implementations or build on the ones in 'FPRepSem'.
	// - 'FPRep' exposes all functions from 'FPRepImpl' and returns 'FPRep'
	//   instances when using Builders (static functions to create values).
	// - 'FPBits' exposes all the functions from 'FPRepImpl' but operates on the
	//   native C++ floating point type instead of 'FPType'. An additional 'get_val'
	//   function allows getting the C++ floating point type value back. Builders
	//   called from 'FPBits' return 'FPBits' instances.

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
		struct FPLayout<FPType::IEEE754_Binary32>
		{
			using StorageType				  = std::uint32_t;
			static constexpr int SIGN_LEN	  = 1;
			static constexpr int EXP_LEN	  = 8;
			static constexpr int SIG_LEN	  = 23;
			static constexpr int FRACTION_LEN = SIG_LEN;
		};

		template <>
		struct FPLayout<FPType::IEEE754_Binary64>
		{
			using StorageType				  = std::uint64_t;
			static constexpr int SIGN_LEN	  = 1;
			static constexpr int EXP_LEN	  = 11;
			static constexpr int SIG_LEN	  = 52;
			static constexpr int FRACTION_LEN = SIG_LEN;
		};

		/* Currently we don't support these types, but may in the future.
		template <> struct FPLayout<FPType::IEEE754_Binary128> {
			using StorageType = UInt128;
			 static constexpr int SIGN_LEN = 1;
			 static constexpr int EXP_LEN = 15;
			 static constexpr int SIG_LEN = 112;
			 static constexpr int FRACTION_LEN = SIG_LEN;
		};

		template <> struct FPLayout<FPType::X86_Binary80> {
			using StorageType = UInt128;
			 static constexpr int SIGN_LEN = 1;
			 static constexpr int EXP_LEN = 15;
			 static constexpr int SIG_LEN = 64;
			 static constexpr int FRACTION_LEN = SIG_LEN - 1;
		};
		*/

		// FPStorage derives useful constants from the FPLayout above.
		template <FPType fp_type>
		struct FPStorage : public FPLayout<fp_type>
		{
			using UP = FPLayout<fp_type>;

			using UP::EXP_LEN;	// The number of bits for the *exponent* part
			using UP::SIG_LEN;	// The number of bits for the *significand* part
			using UP::SIGN_LEN; // The number of bits for the *sign* part
								// For convenience, the sum of `SIG_LEN`, `EXP_LEN`, and `SIGN_LEN`.
			static constexpr int TOTAL_LEN = SIGN_LEN + EXP_LEN + SIG_LEN;

			// The number of bits after the decimal dot when the number is in normal form.
			using UP::FRACTION_LEN;

			// An unsigned integer that is wide enough to contain all of the floating
			// point bits.
			using StorageType = typename UP::StorageType;

			// The number of bits in StorageType.
			static constexpr int STORAGE_LEN = sizeof(StorageType) * CHAR_BIT;
			static_assert(STORAGE_LEN >= TOTAL_LEN);

			// The exponent bias. Always positive.
			static constexpr int32_t EXP_BIAS = (1U << (EXP_LEN - 1U)) - 1U;
			static_assert(EXP_BIAS > 0);

			// The bit pattern that keeps only the *significand* part.
			static constexpr StorageType SIG_MASK = mask_trailing_ones<StorageType, SIG_LEN>();
			// The bit pattern that keeps only the *exponent* part.
			static constexpr StorageType EXP_MASK = mask_trailing_ones<StorageType, EXP_LEN>() << SIG_LEN;
			// The bit pattern that keeps only the *sign* part.
			static constexpr StorageType SIGN_MASK = mask_trailing_ones<StorageType, SIGN_LEN>() << (EXP_LEN + SIG_LEN);
			// The bit pattern that keeps only the *exponent + significand* part.
			static constexpr StorageType EXP_SIG_MASK = mask_trailing_ones<StorageType, EXP_LEN + SIG_LEN>();
			// The bit pattern that keeps only the *sign + exponent + significand* part.
			static constexpr StorageType FP_MASK = mask_trailing_ones<StorageType, TOTAL_LEN>();
			// The bit pattern that keeps only the *fraction* part.
			// i.e., the *significand* without the leading one.
			static constexpr StorageType FRACTION_MASK = mask_trailing_ones<StorageType, FRACTION_LEN>();

			static_assert((SIG_MASK & EXP_MASK & SIGN_MASK) == 0, "masks disjoint");
			static_assert((SIG_MASK | EXP_MASK | SIGN_MASK) == FP_MASK, "masks cover");

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

			protected:
				T value;
			};

			// An opaque type to store a floating point exponent.
			// We define special values but it is valid to create arbitrary values as long
			// as they are in the range [min, max].
			struct Exponent : public TypedInt<int32_t>
			{
				using UP = TypedInt<int32_t>;
				using UP::UP;
				static constexpr auto subnormal() { return Exponent(-EXP_BIAS); }
				static constexpr auto min() { return Exponent(1 - EXP_BIAS); }
				static constexpr auto zero() { return Exponent(0); }
				static constexpr auto max() { return Exponent(EXP_BIAS); }
				static constexpr auto inf() { return Exponent(EXP_BIAS + 1); }
			};

			// An opaque type to store a floating point biased exponent.
			// We define special values but it is valid to create arbitrary values as long
			// as they are in the range [zero, bits_all_ones].
			// Values greater than bits_all_ones are truncated.
			struct BiasedExponent : public TypedInt<uint32_t>
			{
				using UP = TypedInt<uint32_t>;
				using UP::UP;

				constexpr BiasedExponent(Exponent exp) : UP(static_cast<int32_t>(exp) + EXP_BIAS) {}

				// Cast operator to get convert from BiasedExponent to Exponent.
				constexpr operator Exponent() const { return Exponent(UP::value - EXP_BIAS); }

				constexpr BiasedExponent & operator++()
				{
					LIBC_ASSERT(*this != BiasedExponent(Exponent::inf()));
					++UP::value;
					return *this;
				}

				constexpr BiasedExponent & operator--()
				{
					LIBC_ASSERT(*this != BiasedExponent(Exponent::subnormal()));
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
				static constexpr auto msb() { return Significand(StorageType(1) << (SIG_LEN - 1)); }
				static constexpr auto bits_all_ones() { return Significand(SIG_MASK); }
			};

			static constexpr StorageType encode(BiasedExponent exp) { return (exp.to_storage_type() << SIG_LEN) & EXP_MASK; }

			static constexpr StorageType encode(Significand value) { return value.to_storage_type() & SIG_MASK; }

			static constexpr StorageType encode(BiasedExponent exp, Significand sig) { return encode(exp) | encode(sig); }

			static constexpr StorageType encode(Sign sign, BiasedExponent exp, Significand sig)
			{
				if (sign.is_neg()) return SIGN_MASK | encode(exp, sig);
				return encode(exp, sig);
			}

			// The floating point number representation as an unsigned integer.
			StorageType bits{};

			constexpr FPStorage() : bits(0) {}
			constexpr FPStorage(StorageType value) : bits(value) {}

			// Observers
			constexpr StorageType exp_bits() const { return bits & EXP_MASK; }
			constexpr StorageType sig_bits() const { return bits & SIG_MASK; }
			constexpr StorageType exp_sig_bits() const { return bits & EXP_SIG_MASK; }

			// Parts
			constexpr BiasedExponent biased_exponent() const { return BiasedExponent(static_cast<uint32_t>(exp_bits() >> SIG_LEN)); }
			constexpr void set_biased_exponent(BiasedExponent biased) { bits = merge(bits, encode(biased), EXP_MASK); }

		public:
			constexpr Sign sign() const { return (bits & SIGN_MASK) ? Sign::negative : Sign::positive; }
			constexpr void set_sign(Sign signVal)
			{
				if (sign() != signVal) bits ^= SIGN_MASK;
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
			constexpr bool is_zero() const { return exp_sig_bits() == 0; }
			constexpr bool is_nan() const { return exp_sig_bits() > encode(Exponent::inf(), Significand::zero()); }
			constexpr bool is_quiet_nan() const { return exp_sig_bits() >= encode(Exponent::inf(), Significand::msb()); }
			constexpr bool is_signaling_nan() const { return is_nan() && !is_quiet_nan(); }
			constexpr bool is_inf() const { return exp_sig_bits() == encode(Exponent::inf(), Significand::zero()); }
			constexpr bool is_finite() const { return exp_bits() != encode(Exponent::inf()); }

			constexpr bool is_subnormal() const { return exp_bits() == encode(Exponent::subnormal()); }
			constexpr bool is_normal() const { return is_finite() && !is_subnormal(); }
			constexpr RetT next_toward_inf() const
			{
				if (is_finite()) return RetT(bits + StorageType(1));
				return RetT(bits);
			}

			// Returns the mantissa with the implicit bit set iff the current
			// value is a valid normal number.
			constexpr StorageType get_explicit_mantissa() const
			{
				if (is_subnormal()) return sig_bits();
				return (StorageType(1) << UP::SIG_LEN) | sig_bits();
			}
		};

		/* TODO: Currently we do not support x86 extended precision, but may in the future.
		// Specialization for the X86 Extended Precision type.
		template <typename RetT>
		struct FPRepSem<FPType::X86_Binary80, RetT> : public FPStorage<FPType::X86_Binary80>
		{
			using UP = FPStorage<FPType::X86_Binary80>;
			using typename UP::StorageType;
			using UP::FRACTION_LEN;
			using UP::FRACTION_MASK;

			// The x86 80 bit float represents the leading digit of the mantissa
			// explicitly. This is the mask for that bit.
			static constexpr StorageType EXPLICIT_BIT_MASK = StorageType(1) << FRACTION_LEN;
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
			static constexpr RetT zero(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::subnormal(), Significand::zero())); }
			static constexpr RetT one(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::zero(), Significand::msb())); }
			static constexpr RetT min_subnormal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::subnormal(), Significand::lsb())); }
			static constexpr RetT max_subnormal(Sign sign = Sign::positive)
			{
				return RetT(encode(sign, Exponent::subnormal(), Significand::bits_all_ones() ^ Significand::msb()));
			}
			static constexpr RetT min_normal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::min(), Significand::msb())); }
			static constexpr RetT max_normal(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::max(), Significand::bits_all_ones())); }
			static constexpr RetT inf(Sign sign = Sign::positive) { return RetT(encode(sign, Exponent::inf(), Significand::msb())); }
			static constexpr RetT signaling_nan(Sign sign = Sign::positive, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | (v ? Significand(v) : (Significand::msb() >> 2))));
			}
			static constexpr RetT quiet_nan(Sign sign = Sign::positive, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | (Significand::msb() >> 1) | Significand(v)));
			}

			// Observers
			constexpr bool is_zero() const { return exp_sig_bits() == 0; }
			constexpr bool is_nan() const
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
				if (exp_bits() == encode(Exponent::inf())) return !is_inf();
				if (exp_bits() != encode(Exponent::subnormal())) return (sig_bits() & encode(Significand::msb())) == 0;
				return false;
			}
			constexpr bool is_quiet_nan() const { return exp_sig_bits() >= encode(Exponent::inf(), Significand::msb() | (Significand::msb() >> 1)); }
			constexpr bool is_signaling_nan() const { return is_nan() && !is_quiet_nan(); }
			constexpr bool is_inf() const { return exp_sig_bits() == encode(Exponent::inf(), Significand::msb()); }
			constexpr bool is_finite() const { return !is_inf() && !is_nan(); }

			constexpr bool is_subnormal() const { return exp_bits() == encode(Exponent::subnormal()); }
			constexpr bool is_normal() const
			{
				const auto exp = exp_bits();
				if (exp == encode(Exponent::subnormal()) || exp == encode(Exponent::inf())) return false;
				return get_implicit_bit();
			}
			constexpr RetT next_toward_inf() const
			{
				if (is_finite())
				{
					if (exp_sig_bits() == max_normal().uintval()) { return inf(sign()); }
					else if (exp_sig_bits() == max_subnormal().uintval()) { return min_normal(sign()); }
					else if (sig_bits() == SIG_MASK) { return RetT(encode(sign(), ++biased_exponent(), Significand::zero())); }
					else { return RetT(bits + StorageType(1)); }
				}
				return RetT(bits);
			}

			constexpr StorageType get_explicit_mantissa() const { return sig_bits(); }

			// This functions is specific to FPRepSem<FPType::X86_Binary80>.
			constexpr bool get_implicit_bit() const { return static_cast<bool>(bits & EXPLICIT_BIT_MASK); }

			// This functions is specific to FPRepSem<FPType::X86_Binary80>.
			constexpr void set_implicit_bit(bool implicitVal)
			{
				if (get_implicit_bit() != implicitVal) bits ^= EXPLICIT_BIT_MASK;
			}
		};
		*/

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
		// FPRep<FPType::IEEE754_Binary32:>::zero() // returns an FPRep<>
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
			static constexpr int MAX_BIASED_EXPONENT = (1 << UP::EXP_LEN) - 1;

			// CTors
			constexpr FPRepImpl() = default;
			constexpr explicit FPRepImpl(StorageType x) : UP(x) {}

			// Comparison
			constexpr friend bool operator==(FPRepImpl a, FPRepImpl b) { return a.uintval() == b.uintval(); }
			constexpr friend bool operator!=(FPRepImpl a, FPRepImpl b) { return a.uintval() != b.uintval(); }

			// Representation
			constexpr StorageType uintval() const { return bits & FP_MASK; }
			constexpr void set_uintval(StorageType value) { bits = (value & FP_MASK); }

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
			constexpr RetT abs() const { return RetT(bits & UP::EXP_SIG_MASK); }

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
			constexpr bool is_inf_or_nan() const { return !is_finite(); }
			constexpr bool is_neg() const { return sign().is_neg(); }
			constexpr bool is_pos() const { return sign().is_pos(); }

			constexpr uint16_t get_biased_exponent() const { return static_cast<uint16_t>(static_cast<uint32_t>(UP::biased_exponent())); }

			constexpr void set_biased_exponent(StorageType biased) { UP::set_biased_exponent(BiasedExponent((int32_t)biased)); }

			constexpr int get_exponent() const { return static_cast<int32_t>(Exponent(UP::biased_exponent())); }

			// If the number is subnormal, the exponent is treated as if it were the
			// minimum exponent for a normal number. This is to keep continuity between
			// the normal and subnormal ranges, but it causes problems for functions where
			// values are calculated from the exponent, since just subtracting the bias
			// will give a slightly incorrect result. Additionally, zero has an exponent
			// of zero, and that should actually be treated as zero.
			constexpr int get_explicit_exponent() const
			{
				Exponent exponent(UP::biased_exponent());
				if (is_zero()) exponent = Exponent::zero();
				if (exponent == Exponent::subnormal()) exponent = Exponent::min();
				return static_cast<int32_t>(exponent);
			}

			constexpr StorageType get_mantissa() const { return bits & FRACTION_MASK; }

			constexpr void set_mantissa(StorageType mantVal) { bits = UP::merge(bits, mantVal, FRACTION_MASK); }

			constexpr void set_significand(StorageType sigVal) { bits = UP::merge(bits, sigVal, SIG_MASK); }
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
			static constexpr RetT make_value(StorageType number, int ep)
			{
				FPRepImpl result(0);
				int lz = UP::FRACTION_LEN + 1 - (UP::STORAGE_LEN - support::countl_zero(number));

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

			constexpr explicit operator StorageType() const { return UP::uintval(); }
		};

	} // namespace internal

	// Returns the FPType corresponding to C++ type T on the host.
	template <typename T>
	static constexpr FPType get_fp_type()
	{
		using UnqualT = std::remove_cv_t<T>;
		if constexpr (std::is_same_v<UnqualT, float> && __FLT_MANT_DIG__ == 24)
			return FPType::IEEE754_Binary32;
		else if constexpr (std::is_same_v<UnqualT, double> && __DBL_MANT_DIG__ == 53)
			return FPType::IEEE754_Binary64;
		else if constexpr (std::is_same_v<UnqualT, long double>)
		{
			return FPType::IEEE754_Binary64;
			/* TODO: Currently we do not support long double, but may in the future.
			if constexpr (__LDBL_MANT_DIG__ == 53)
				return FPType::IEEE754_Binary64;
			else if constexpr (__LDBL_MANT_DIG__ == 64)
				return FPType::X86_Binary80;
			else if constexpr (__LDBL_MANT_DIG__ == 113)
				return FPType::IEEE754_Binary128;
			*/
		}
#if defined(CCM_TYPES_HAS_FLOAT128)
		else if constexpr (std::is_same_v<UnqualT, float128>)
			return FPType::IEEE754_Binary128;
#endif
		else
			static_assert(ccm::support::always_false<UnqualT>, "Unsupported type");
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
   constexpr FPBits() = default;

  template <typename XType>  constexpr explicit FPBits(XType x) {
    using Unqual = typename std::remove_cv_t<XType>;
    if constexpr (std::is_same_v<Unqual, T>) {
      UP::bits = support::bit_cast<StorageType>(x);
    } else if constexpr (std::is_same_v<Unqual, StorageType>) {
      UP::bits = x;
    } else {
      // We don't want accidental type promotions/conversions, so we require
      // exact type match.
      static_assert(ccm::support::always_false<XType>);
    }
  }

  // Floating-point conversions.
   constexpr T get_val() const { return support::bit_cast<T>(UP::bits); }
};

} // namespace ccm::fputil


