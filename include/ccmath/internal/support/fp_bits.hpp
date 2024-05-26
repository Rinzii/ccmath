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

#include "ccmath/internal/support/always_false.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/math_support.hpp"
#include "ccmath/internal/support/type_traits.hpp"
#include "ccmath/internal/types/int128_types.hpp"
#include "ccmath/internal/types/sign.hpp"

#include <cfloat>
#include <climits>
#include <cstdint>

#include "ccmath/internal/predef/likely.hpp"

namespace ccm::support
{

	// All supported floating point types
	enum class FPType : std::uint8_t
	{
		eBinary32,
		eBinary64,
		eBinary80,
		eBinary128,
	};

	namespace internal
	{
		/**
		 * @brief Defines the layout of a floating point in memory and all of its parts along with the associated storage type.
		 */
		template <FPType>
		struct FPLayout
		{
		};

		template <>
		struct FPLayout<FPType::eBinary32>
		{
			using StorageType				  = std::uint32_t;
			static constexpr int SIGN_LEN	  = 1;
			static constexpr int EXP_LEN	  = 8;
			static constexpr int SIG_LEN	  = 23;
			static constexpr int FRACTION_LEN = SIG_LEN;
		};

		template <>
		struct FPLayout<FPType::eBinary64>
		{
			using StorageType				  = std::uint64_t;
			static constexpr int SIGN_LEN	  = 1;
			static constexpr int EXP_LEN	  = 11;
			static constexpr int SIG_LEN	  = 52;
			static constexpr int FRACTION_LEN = SIG_LEN;
		};

		template <>
		struct FPLayout<FPType::eBinary80>
		{
			using StorageType				  = types::uint128_t;
			static constexpr int SIGN_LEN	  = 1;
			static constexpr int EXP_LEN	  = 15;
			static constexpr int SIG_LEN	  = 64;
			static constexpr int FRACTION_LEN = SIG_LEN - 1;
		};

		template <>
		struct FPLayout<FPType::eBinary128>
		{
			using StorageType				  = types::uint128_t;
			static constexpr int SIGN_LEN	  = 1;
			static constexpr int EXP_LEN	  = 15;
			static constexpr int SIG_LEN	  = 112;
			static constexpr int FRACTION_LEN = SIG_LEN;
		};

		template <FPType fp_type>
		struct FPStorage : FPLayout<fp_type>
		{
			using UP = FPLayout<fp_type>;

			/**
			 * @brief The number of bits in the exponent part.
			 */
			using UP::EXP_LEN;

			/**
			 * @brief The number of bits in the significand part.
			 */
			using UP::SIG_LEN;

			/**
			 * @brief The number of bits in the sign part.
			 */
			using UP::SIGN_LEN;

			/**
			 * @brief The total number of bits in the floating point number. Provided as a convenience.
			 */
			static constexpr int TOTAL_LEN = SIGN_LEN + EXP_LEN + SIG_LEN;

			/**
			 * @brief The number of bits after the decimal dot when the number is in normal form.
			 */
			using UP::FRACTION_LEN;

			/**
			 * @brief The storage type for the floating point number repersented as an unsigned integer wide enough to contain all of the floating point bits.
			 */
			using StorageType = typename UP::StorageType;

			/**
			 * @brief The number of bits in the storage type.
			 */
			static constexpr int STORAGE_LEN = sizeof(StorageType) * CHAR_BIT;
			static_assert(STORAGE_LEN >= TOTAL_LEN,
						  "ccmath - FPBits: Some how the storage is not able to hold the entire floating point number. This should never happen!");

			/**
			 * @brief The exponent bias for the floating point type that should always be positive.
			 */
			static constexpr std::int32_t EXP_BIAS = (1U << EXP_LEN - 1U) - 1U;
			static_assert(EXP_BIAS > 0, "ccmath - FPBits: The exponent bias should always be positive. This should never happen!");

			/**
			 * @brief The bit mask that keeps only the significand part.
			 */
			static constexpr StorageType SIG_MASK = support::mask_trailing_ones<StorageType, SIG_LEN>();

			/**
			 * @brief The bit mask that keeps only the exponent part.
			 */
			static constexpr StorageType EXP_MASK = support::mask_trailing_ones<StorageType, EXP_LEN>() << SIG_LEN;

			/**
			 * @brief The bit mask that keeps only the sign part.
			 */
			static constexpr StorageType SIGN_MASK = support::mask_trailing_ones<StorageType, SIGN_LEN>() << (EXP_LEN + SIG_LEN);

			/**
			 * @brief The bit mask that keeps only the exponent and significand parts.
			 */
			static constexpr StorageType EXP_SIG_MASK = support::mask_trailing_ones<StorageType, EXP_LEN + SIG_LEN>();

			/**
			 * @brief The bit mask that keeps the entire bit pattern. (sign + exponent + significand)
			 */
			static constexpr StorageType FP_MASK = ccm::support::mask_trailing_ones<StorageType, TOTAL_LEN>();

			/**
			 * @brief The bit mask that keeps only the fraction part. (i.e., the significand without the leading one)
			 */
			static constexpr StorageType FRACTION_MASK = ccm::support::mask_trailing_ones<StorageType, FRACTION_LEN>();

			static_assert((SIG_MASK & EXP_MASK & SIGN_MASK) == 0, "Bitmasks overlap! SIG_MASK, EXP_MASK, and SIGN_MASK must be disjoint");
			static_assert((SIG_MASK | EXP_MASK | SIGN_MASK) == FP_MASK, "Bitmasks do not fully cover the floating-point mask");

		protected:
			/**
			 * @brief Merges the bits from 'a' and 'b' values according to 'mask'.
			 * @param a Value to merge in non-masked bits
			 * @param b Value to merge in masked bits
			 * @param mask Mask that uses 'a' bits when corresponding 'mask' bits are zeroes and 'b' bits when corresponding bits are ones.
			 * @return Merged bits from 'a' and 'b' according to 'mask'.
			 */
			static constexpr StorageType merge(StorageType a, StorageType b, StorageType mask)
			{
				// Algorithm taken from here. All algorithms from here are public domain.
				// https://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
				return a ^ ((a ^ b) & mask);
			}

			/**
			 * @brief A strongly typed integer that prevents mixing and matching integers with different semantics.
			 * @tparam T The type of the integer.
			 */
			template <typename T>
			struct TypedInt
			{
				using value_type = T;
				constexpr explicit TypedInt(T value) : value(value) {}
				constexpr TypedInt(const TypedInt & value)			   = default;
				constexpr TypedInt & operator=(const TypedInt & value) = default;

				constexpr explicit operator T() const { return value; }

				[[nodiscard]] constexpr StorageType to_storage_type() const { return StorageType(value); }

				friend constexpr bool operator==(TypedInt a, TypedInt b) { return a.value == b.value; }
				friend constexpr bool operator!=(TypedInt a, TypedInt b) { return a.value != b.value; }

			protected:
				T value;
			};

			/**
			 * @brief An opaque type for storing floating point exponents.
			 *
			 * Inherits from TypedInt<std::int32_t> and defines special values like subnormal, min, zero, max, and inf.
			 * Arbitrary values are valid within the range [min, max].
			 */
			struct Exponent : TypedInt<std::int32_t>
			{
				using UP = TypedInt<std::int32_t>;
				using UP::UP;
				static constexpr auto subnormal() { return Exponent(-EXP_BIAS); }
				static constexpr auto min() { return Exponent(1 - EXP_BIAS); }
				static constexpr auto zero() { return Exponent(0); }
				static constexpr auto max() { return Exponent(EXP_BIAS); }
				static constexpr auto inf() { return Exponent(EXP_BIAS + 1); }
			};

			/**
			 * @brief An opaque type for storing a floating point biased exponent.
			 *
			 * Inherits from TypedInt<std::uint32_t> and allows storage of biased exponents.
			 * Provides conversion to and from the Exponent type and includes increment and decrement operators.
			 * Arbitrary values are valid within the range [zero, bits_all_ones]. Values exceeding this range are truncated.
			 */
			struct BiasedExponent : TypedInt<std::uint32_t>
			{
				using UP = TypedInt<std::uint32_t>;
				using UP::UP;

				// NOLINTNEXTLINE(google-explicit-constructor)
				constexpr BiasedExponent(Exponent exp) : UP(static_cast<std::uint32_t>(static_cast<std::int32_t>(exp) + EXP_BIAS)) {}

				/**
				 * @brief Cast operator to convert from BiasedExponent to Exponent.
				 */
				explicit constexpr operator Exponent() const { return Exponent(static_cast<std::int32_t>(UP::value) - EXP_BIAS); }

				constexpr BiasedExponent & operator++()
				{
					assert(*this != BiasedExponent(Exponent::inf())); // Legal in C++14 and above
					++UP::value;
					return *this;
				}

				constexpr BiasedExponent & operator--()
				{
					assert(*this != BiasedExponent(Exponent::subnormal())); // Legal in C++14 and above
					--UP::value;
					return *this;
				}
			};

			/**
			 * @brief An opaque type for storing a floating point significand.
			 *
			 * Inherits from TypedInt<StorageType> and allows storage of significand values.
			 * Provides operators for bitwise OR, XOR, and right shift.
			 * Defines special values like zero, lsb (least significant bit), msb (most significant bit),
			 * and all bits set to one. Arbitrary values are valid within the range [zero, bits_all_ones].
			 * Values exceeding this range are truncated.
			 *
			 * @attention The semantics of the Significand are implementation dependent.
			 */
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
				static constexpr auto msb() { return Significand(StorageType(1) << (SIG_LEN - 1)); }
				static constexpr auto bits_all_ones() { return Significand(SIG_MASK); }
			};

			static constexpr StorageType encode(BiasedExponent exp) { return (exp.to_storage_type() << SIG_LEN) & EXP_MASK; }

			static constexpr StorageType encode(Significand value) { return value.to_storage_type() & SIG_MASK; }

			static constexpr StorageType encode(BiasedExponent exp, Significand sig) { return encode(exp) | encode(sig); }

			static constexpr StorageType encode(types::Sign sign, BiasedExponent exp, Significand sig)
			{
				if (sign.is_neg()) { return SIGN_MASK | encode(exp, sig); }
				return encode(exp, sig);
			}

			constexpr FPStorage() : bits(0) {}

			constexpr FPStorage(StorageType value) : bits(value) {}

			/// Observers

			[[nodiscard]] constexpr StorageType exp_bits() const { return bits & EXP_MASK; }
			[[nodiscard]] constexpr StorageType sig_bits() const { return bits & SIG_MASK; }
			[[nodiscard]] constexpr StorageType exp_sig_bits() const { return bits & EXP_SIG_MASK; }

			/// Parts

			[[nodiscard]] constexpr BiasedExponent biased_exponent() const { return BiasedExponent(static_cast<std::uint32_t>(exp_bits() >> SIG_LEN)); }
			constexpr void set_biased_exponent(BiasedExponent biased) { bits = merge(bits, encode(biased), EXP_MASK); }

			/**
			 * @brief The bits of the floating point number reperesented as an unsigned integer.
			 */
			StorageType bits{};

		public:
			[[nodiscard]] constexpr types::Sign sign() const { return (bits & SIGN_MASK) ? types::Sign::NEG : types::Sign::POS; }
			constexpr void set_sign(types::Sign signVal)
			{
				if (sign() != signVal) { bits ^= SIGN_MASK; }
			}
		};

		/**
		 * @brief This layer defines all functions related to the encoding of the floating point type.
		 *
		 * It facilitates the construction, modification, and observation of values manipulated as 'StorageType'.
		 */
		template <FPType fp_type, typename RetT>
		struct FPRepSem : FPStorage<fp_type>
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
			/// Builders

			static constexpr RetT zero(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::subnormal(), Significand::zero())); }
			static constexpr RetT one(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::zero(), Significand::zero())); }
			static constexpr RetT min_subnormal(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::subnormal(), Significand::lsb())); }
			static constexpr RetT max_subnormal(types::Sign sign = types::Sign::POS)
			{
				return RetT(encode(sign, Exponent::subnormal(), Significand::bits_all_ones()));
			}
			static constexpr RetT min_normal(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::min(), Significand::zero())); }
			static constexpr RetT max_normal(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::max(), Significand::bits_all_ones())); }
			static constexpr RetT inf(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::inf(), Significand::zero())); }
			static constexpr RetT signaling_nan(types::Sign sign = types::Sign::POS, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), (v ? Significand(v) : (Significand::msb() >> 1))));
			}
			static constexpr RetT quiet_nan(types::Sign sign = types::Sign::POS, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | Significand(v)));
			}

			/// Observers

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

			/**
			 * @brief Returns the mantissa with the implicit bit set if the current value is a valid normal number.
			 * @return The explicit mantissa as a StorageType.
			 *
			 * This function checks if the current value is subnormal. If it is, it returns the significand bits.
			 * Otherwise, it returns the mantissa with the implicit bit set.
			 */
			constexpr StorageType get_explicit_mantissa() const
			{
				if (is_subnormal()) { return sig_bits(); }
				return (StorageType(1) << UP::SIG_LEN) | sig_bits();
			}
		};

		/**
		 * @brief Template specialization for the X86 Extended Precision floating point type.
		 */
		template <typename RetT>
		struct FPRepSem<FPType::eBinary80, RetT> : FPStorage<FPType::eBinary80>
		{
			using UP = FPStorage;
			using UP::FRACTION_LEN;
			using UP::FRACTION_MASK;
			using UP::StorageType;

			/**
			 * @brief Mask for the leading digit of the mantissa in the x86 80-bit float.
			 *
			 * The x86 80-bit float represents the leading digit of the mantissa explicitly.
			 * This constant defines the mask for that bit.
			 */
			static constexpr StorageType EXPLICIT_BIT_MASK = static_cast<StorageType>(1) << FRACTION_LEN;

			// The X80 significand includes an explicit bit and the fractional part.
			static_assert((EXPLICIT_BIT_MASK & FRACTION_MASK) == 0, "Explicit bit and fractional part must not overlap.");
			static_assert((EXPLICIT_BIT_MASK | FRACTION_MASK) == SIG_MASK, "Explicit bit and fractional part must cover the entire significand.");

		protected:
			using UP::encode;
			using UP::Exponent;
			using UP::Significand;
			using UP::UP;

		public:
			/// Builders

			static constexpr RetT zero(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::subnormal(), Significand::zero())); }
			static constexpr RetT one(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::zero(), Significand::msb())); }
			static constexpr RetT min_subnormal(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::subnormal(), Significand::lsb())); }
			static constexpr RetT max_subnormal(types::Sign sign = types::Sign::POS)
			{
				return RetT(encode(sign, Exponent::subnormal(), Significand::bits_all_ones() ^ Significand::msb()));
			}
			static constexpr RetT min_normal(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::min(), Significand::msb())); }
			static constexpr RetT max_normal(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::max(), Significand::bits_all_ones())); }
			static constexpr RetT inf(types::Sign sign = types::Sign::POS) { return RetT(encode(sign, Exponent::inf(), Significand::msb())); }
			static constexpr RetT signaling_nan(types::Sign sign = types::Sign::POS, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | (v ? Significand(v) : (Significand::msb() >> 2))));
			}
			static constexpr RetT quiet_nan(types::Sign sign = types::Sign::POS, StorageType v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | (Significand::msb() >> 1) | Significand(v)));
			}

			/// Observers

			[[nodiscard]] constexpr bool is_zero() const { return exp_sig_bits() == 0; }
			[[nodiscard]] constexpr bool is_nan() const
			{
				/**
				 * Most encoding forms from the table found in
				 * https://en.wikipedia.org/wiki/Extended_precision#x86_extended_precision_format
				 * are interpreted as NaN.
				 *
				 * Specifically, these forms include:
				 * - Pseudo-Infinity				- The sign bit gives the sign of the infinity. The 8087 and 80287 treat this as Infinity. The 80387 and
				 *										later treat this as an invalid operand.
				 * - Pseudo Not a Number			- The sign bit is meaningless. The 8087 and 80287 treat this as a Signaling Not a Number. The 80387 and
				 *										later treat this as an invalid operand.
				 * - Signaling Not a Number			- The sign bit is meaningless.
				 * - Floating-point Indefinite		- the result of invalid calculations such as square root of a negative number, logarithm of a negative
				 *										number, 0/0, Inf/Inf, Infinty times zero, and others, when the processor has been configured to not
				 *										generate exceptions for invalid operands. The sign bit is meaningless. This is a special case of
				 *										a Quiet Not a Number.
				 * - Quiet Not a Number				- The sign bit is meaningless. The 8087 and 80287 treat this as a Signaling Not a Number.
				 * - Unnormal						- Only generated on the 8087 and 80287. The 80387 and later treat this as an invalid operand.
				 *										The value is (-1)^sign * m * 2^(e-16383), where "m" is the significand as a positive binary number
				 *										and "e" is the value of the exponent field interpreted as a positive integer.
				 *
				 * This can be simplified to the following logic:
				 */
				if (exp_bits() == encode(Exponent::inf())) { return !is_inf(); }
				if (exp_bits() != encode(Exponent::subnormal())) { return (sig_bits() & encode(Significand::msb())) == 0; }
				return false;
			}
			[[nodiscard]] constexpr bool is_quiet_nan() const
			{
				return exp_sig_bits() >= encode(Exponent::inf(), Significand::msb() | (Significand::msb() >> 1));
			}
			[[nodiscard]] constexpr bool is_signaling_nan() const { return is_nan() && !is_quiet_nan(); }
			[[nodiscard]] constexpr bool is_inf() const { return exp_sig_bits() == encode(Exponent::inf(), Significand::msb()); }
			[[nodiscard]] constexpr bool is_finite() const { return !is_inf() && !is_nan(); }
			[[nodiscard]] constexpr bool is_subnormal() const { return exp_bits() == encode(Exponent::subnormal()); }
			[[nodiscard]] constexpr bool is_normal() const
			{
				if (const auto exp = exp_bits(); exp == encode(Exponent::subnormal()) || exp == encode(Exponent::inf())) { return false; }
				return get_implicit_bit();
			}
			constexpr RetT next_toward_inf() const
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

			[[nodiscard]] constexpr StorageType get_explicit_mantissa() const { return sig_bits(); }

			/**
			 * @brief Checks if the implicit bit is set.
			 *
			 * @return True if the implicit bit is set, false otherwise.
			 *
			 * This function examines the bits and returns whether the implicit bit, as defined by EXPLICIT_BIT_MASK, is set.
			 *
			 * @attention This function is specific to FPRepSem<FPType::X86_Binary80>.
			 */
			[[nodiscard]] constexpr bool get_implicit_bit() const { return static_cast<bool>(bits & EXPLICIT_BIT_MASK); }

			/**
			 * @brief Sets the implicit bit to the specified value.
			 *
			 * @param implicitVal The value to set the implicit bit to (true or false).
			 *
			 * This function sets the implicit bit, defined by EXPLICIT_BIT_MASK, to the given value.
			 * If the current state of the implicit bit differs from the specified value, it toggles the bit.
			 *
			 * @attention This function is specific to FPRepSem<FPType::X86_Binary80>.
			 */
			constexpr void set_implicit_bit(bool implicitVal)
			{
				if (get_implicit_bit() != implicitVal) { bits ^= EXPLICIT_BIT_MASK; }
			}
		};

		/**
		 * @brief 'FPRepImpl' is the base of the class hierarchy that handles 'FPType'.
		 *
		 * Specific float semantics are implemented by 'FPRepSem' above and specialized as needed.
		 *
		 * The 'RetT' type is propagated to 'FPRepSem' to ensure that functions creating new values (Builders) return the appropriate type.
		 * For example, when creating a value through 'FPBits', the builder returns an 'FPBits' value.
		 * Example: FPBits<float>::zero(); // returns an FPBits<>
		 *
		 * When the specific C++ floating point type is not a concern, 'FPRep' can be used by specifying the 'FPType' directly.
		 * Example: FPRep<FPType::eBinary32>::zero() // returns an FPRep<>
		 */
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

			using UP::FP_MASK;

		public:
			/// Constants

			using UP::EXP_BIAS;
			using UP::EXP_MASK;
			using UP::FRACTION_MASK;
			using UP::SIG_LEN;
			using UP::SIG_MASK;
			using UP::SIGN_MASK;
			static constexpr int MAX_BIASED_EXPONENT = (1 << UP::EXP_LEN) - 1;

			/// Constructors

			constexpr FPRepImpl() = default;
			constexpr explicit FPRepImpl(StorageType x) : UP(x) {}

			/// Comparison Operators

			constexpr friend bool operator==(FPRepImpl a, FPRepImpl b) { return a.uintval() == b.uintval(); }
			constexpr friend bool operator!=(FPRepImpl a, FPRepImpl b) { return a.uintval() != b.uintval(); }

			// Representation
			constexpr StorageType uintval() const { return bits & FP_MASK; }
			constexpr void set_uintval(StorageType value) { bits = (value & FP_MASK); }

			/// Builders

			using UP::inf;
			using UP::max_normal;
			using UP::max_subnormal;
			using UP::min_normal;
			using UP::min_subnormal;
			using UP::one;
			using UP::quiet_nan;
			using UP::signaling_nan;
			using UP::zero;

			/// Modifiers

			constexpr RetT abs() const { return RetT(bits & UP::EXP_SIG_MASK); }

			/// Observers

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

			constexpr void set_biased_exponent(StorageType biased)
			{
				UP::set_biased_exponent(BiasedExponent(static_cast<std::uint32_t>(static_cast<std::int32_t>(biased))));
			}

			[[nodiscard]] constexpr int get_exponent() const { return static_cast<std::int32_t>(Exponent(UP::biased_exponent())); }

			/**
			 * @brief Retrieves the explicit exponent value, adjusted for subnormal and zero values.
			 *
			 * @return The explicit exponent as an integer.
			 *
			 * For subnormal numbers, the exponent is treated as the minimum exponent for a normal number
			 * to maintain continuity between normal and subnormal ranges. This adjustment prevents slight
			 * inaccuracies when calculating values directly from the exponent by simply subtracting the bias.
			 * Additionally, zero has an exponent of zero, which should be treated as zero.
			 */
			[[nodiscard]] constexpr int get_explicit_exponent() const
			{
				Exponent exponent(UP::biased_exponent());
				if (is_zero()) { exponent = Exponent::zero(); }
				if (exponent == Exponent::subnormal()) { exponent = Exponent::min(); }
				return static_cast<std::int32_t>(exponent);
			}

			constexpr StorageType get_mantissa() const { return bits & FRACTION_MASK; }

			/**
			 * @brief Sets the mantissa value of the internal bits.
			 * @param mantVal The mantissa value to set.
			 */
			constexpr void set_mantissa(StorageType mantVal) { bits = UP::merge(bits, mantVal, FRACTION_MASK); }

			/**
			 * @brief Sets the significand value of the inernal bits.
			 * @param sigVal The significand value to set.
			 */
			constexpr void set_significand(StorageType sigVal) { bits = UP::merge(bits, sigVal, SIG_MASK); }

			/**
			 * @brief Unsafe function that creates a floating point representation that simply packs the sign, biased exponent, and mantissa values without
			 *checking bounds nor normalization.
			 * @param sign Sign of the number.
			 * @param biased_exp Biased exponent.
			 * @param mantissa Mantissa value.
			 * @return The created floating point representation.
			 *
			 * @warning For Binary80 Extended Precision, implicit bit needs to be set correctly in the 'mantissa' by the caller.
			 *			This function will not check for its validity and assumes the caller has set it correctly.
			 */
			static constexpr RetT create_value(types::Sign sign, StorageType biased_exp, StorageType mantissa)
			{
				return RetT(encode(sign, BiasedExponent(static_cast<std::uint32_t>(biased_exp)), Significand(mantissa)));
			}

			/**
			 * @brief Converts an integer number and unbiased exponent to a proper float type.
			 * @param number The number to convert.
			 * @param expo The raw exponent value.
			 * @return The converted number.
			 *
			 * @attention Pay close attention to the following items about this function:
			 *				1) "ep" is the raw exponent value.
			 *				2) The function adds +1 to ep for seamless normalized to denormalized transition.
			 *				3) The function does not check exponent high limit.
			 *				4) "number" zero value is not processed correctly.
			 *				5) Number is unsigned, so the result can be only positive.
			 */
			static constexpr RetT make_value(StorageType number, int expo)
			{
				// Result = number * 2^(expo + 1 - exponent_bias)
				FPRepImpl result(0);
				int lz = UP::FRACTION_LEN + 1 - (UP::STORAGE_LEN - support::countl_zero(number));

				number <<= lz;
				expo -= lz;

				if (CCM_LIKELY(expo >= 0))
				{
					// Implicit number bit will be removed by mask
					result.set_significand(number);
					result.set_biased_exponent(expo + 1);
				}
				else { result.set_significand(number >> -expo); }
				return RetT(result.uintval());
			}
		};

		/**
		 * @brief A generic class to manipulate floating point formats.
		 * @tparam fp_type The floating point type to manipulate.
		 */
		template <FPType fp_type>
		struct FPRep : FPRepImpl<fp_type, FPRep<fp_type>>
		{
			using UP		  = FPRepImpl<fp_type, FPRep<fp_type>>;
			using StorageType = typename UP::StorageType;
			using UP::UP;

			constexpr explicit operator StorageType() const { return UP::uintval(); }
		};

	} // namespace internal

	/**
	 * @brief Identifies what the FPType corresponding to C++ type T is on the host.
	 * @tparam T The type to identify the FPType for.
	 * @return The FPType corresponding to the C++ type T.
	 */
	template <typename T>
	static constexpr FPType get_fp_type()
	{
		using UnqualT = std::remove_cv_t<T>;
		if constexpr (std::is_same_v<UnqualT, float> && FLT_MANT_DIG == 24) { return FPType::eBinary32; }
		else if constexpr (std::is_same_v<UnqualT, double> && DBL_MANT_DIG == 53) { return FPType::eBinary64; }
		else if constexpr (std::is_same_v<UnqualT, long double>)
		{
			if constexpr (LDBL_MANT_DIG == 53) { return FPType::eBinary64; }		// long double is same as double
			else if constexpr (LDBL_MANT_DIG == 64) { return FPType::eBinary80; }	// long double is 80-bits
			else if constexpr (LDBL_MANT_DIG == 113) { return FPType::eBinary128; } // long double is 128-bits
		}
#if defined(CCM_TYPES_HAS_FLOAT128)
		else if constexpr (std::is_same_v<UnqualT, types::float128>) { return FPType::eBinary128; }
#endif
		else { static_assert(support::always_false<UnqualT>, "Unsupported type"); }
		return FPType::eBinary32; // This will never be reached due to assert. Only here to appease the compiler.
	}

	/**
	 * @brief A generic class to manipulate C++ floating point formats.
	 * @tparam T The floating point type to manipulate.
	 */
	template <typename T>
	struct FPBits final : internal::FPRepImpl<get_fp_type<T>(), FPBits<T>>
	{
		static_assert(std::is_floating_point_v<T>, "FPBits instantiated with invalid type.");
		using UP		  = internal::FPRepImpl<get_fp_type<T>(), FPBits<T>>;
		using StorageType = typename UP::StorageType;

		constexpr FPBits() = default;

		template <typename XType>
		constexpr explicit FPBits(XType x)
		{
			using Unqual = std::remove_cv_t<XType>;
			if constexpr (std::is_same_v<Unqual, T>) { UP::bits = support::bit_cast<StorageType>(x); }
			else if constexpr (std::is_same_v<Unqual, StorageType>) { UP::bits = x; }
			else
			{
				// Prevent accidental type promotion or conversion form being able to happen. Exact types only.
				static_assert(support::always_false<XType>);
			}
		}

		/**
		 * @brief Converts the stored bits to a floating-point value.
		 * @return The floating-point value of type T.
		 */
		constexpr T get_val() const { return support::bit_cast<T>(UP::bits); }
	};

} // namespace ccm::support
