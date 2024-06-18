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

#include "ccmath/internal/predef/likely.hpp"
#include "ccmath/internal/support/always_false.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/math_support.hpp"
#include "ccmath/internal/support/type_traits.hpp"
#include "ccmath/internal/types/int128_types.hpp"
#include "ccmath/internal/types/sign.hpp"

#include <cfloat>
#include <climits>
#include <cstdint>

namespace ccm::support::fp
{
	/// All supported floating point types
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
			using storage_type						= std::uint32_t;
			static constexpr std::int_fast32_t sign_length		= 1;
			static constexpr std::int_fast32_t exponent_length	= 8;
			static constexpr std::int_fast32_t significand_length = 23;
			static constexpr std::int_fast32_t fraction_length	= significand_length;
		};

		template <>
		struct FPLayout<FPType::eBinary64>
		{
			using storage_type						= std::uint64_t;
			static constexpr std::int_fast32_t sign_length		= 1;
			static constexpr std::int_fast32_t exponent_length	= 11;
			static constexpr std::int_fast32_t significand_length = 52;
			static constexpr std::int_fast32_t fraction_length	= significand_length;
		};

		template <>
		struct FPLayout<FPType::eBinary80>
		{
			using storage_type						= types::uint128_t;
			static constexpr std::int_fast32_t sign_length		= 1;
			static constexpr std::int_fast32_t exponent_length	= 15;
			static constexpr std::int_fast32_t significand_length = 64;
			static constexpr std::int_fast32_t fraction_length	= significand_length - 1;
		};

		template <>
		struct FPLayout<FPType::eBinary128>
		{
			using storage_type						= types::uint128_t;
			static constexpr std::int_fast32_t sign_length		= 1;
			static constexpr std::int_fast32_t exponent_length	= 15;
			static constexpr std::int_fast32_t significand_length = 112;
			static constexpr std::int_fast32_t fraction_length	= significand_length;
		};

		template <FPType fp_type>
		struct FPStorage : FPLayout<fp_type>
		{
			using BASE = FPLayout<fp_type>;

			/**
			 * @brief The number of bits in the exponent part.
			 */
			using BASE::exponent_length;

			/**
			 * @brief The number of bits in the significand part.
			 */
			using BASE::significand_length;

			/**
			 * @brief The number of bits in the sign part.
			 */
			using BASE::sign_length;

			/**
			 * @brief The total number of bits in the floating point number. Provided as a convenience.
			 */
			static constexpr int total_length = sign_length + exponent_length + significand_length;

			/**
			 * @brief The number of bits after the decimal dot when the number is in normal form.
			 */
			using BASE::fraction_length;

			/**
			 * @brief The storage type for the floating point number represented as an unsigned integer wide enough to contain all of the floating point bits.
			 */
			using storage_type = typename BASE::storage_type;

			/**
			 * @brief The number of bits in the storage type.
			 */
			static constexpr int storage_length = sizeof(storage_type) * CHAR_BIT;
			static_assert(storage_length >= total_length,
						  "ccmath - FPBits: Some how the storage is not able to hold the entire floating point number. This should never happen!");

			/**
			 * @brief The exponent bias for the floating point type that should always be positive.
			 */
			static constexpr std::int32_t exponent_bias = (1U << (exponent_length - 1U)) - 1U;
			static_assert(exponent_bias > 0, "ccmath - FPBits: The exponent bias should always be positive. This should never happen!");

			/**
			 * @brief The bit mask that keeps only the significand part.
			 */
			static constexpr storage_type significand_mask = support::mask_trailing_ones<storage_type, significand_length>();

			/**
			 * @brief The bit mask that keeps only the exponent part.
			 */
			static constexpr storage_type exponent_mask = support::mask_trailing_ones<storage_type, exponent_length>() << significand_length;

			/**
			 * @brief The bit mask that keeps only the sign part.
			 */
			static constexpr storage_type sign_mask = support::mask_trailing_ones<storage_type, sign_length>() << (exponent_length + significand_length);

			/**
			 * @brief The bit mask that keeps only the exponent and significand parts.
			 */
			static constexpr storage_type exponent_significand_mask = support::mask_trailing_ones<storage_type, exponent_length + significand_length>();

			/**
			 * @brief The bit mask that keeps the entire bit pattern. (sign + exponent + significand)
			 */
			static constexpr storage_type fp_bits_mask = support::mask_trailing_ones<storage_type, total_length>();

			/**
			 * @brief The bit mask that keeps only the fraction part. (i.e., the significand without the leading one)
			 */
			static constexpr storage_type fraction_mask = support::mask_trailing_ones<storage_type, fraction_length>();

			static_assert((significand_mask & exponent_mask & sign_mask) == 0,
						  "Bitmasks overlap! significand_mask, exponent_mask, and sign_mask must be disjoint");
			static_assert((significand_mask | exponent_mask | sign_mask) == fp_bits_mask, "Bitmasks do not fully cover the floating-point mask");

		protected:
			/**
			 * @brief Merges the bits from 'a' and 'b' values according to 'mask'.
			 * @param a Value to merge in non-masked bits
			 * @param b Value to merge in masked bits
			 * @param mask Mask that uses 'a' bits when corresponding 'mask' bits are zeroes and 'b' bits when corresponding bits are ones.
			 * @return Merged bits from 'a' and 'b' according to 'mask'.
			 */
			static constexpr storage_type merge(storage_type a, storage_type b, storage_type mask)
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

				[[nodiscard]] constexpr storage_type to_storage_type() const { return storage_type(value); }

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
				using BASE = TypedInt<std::int32_t>;
				using BASE::BASE;

				static constexpr auto subnormal() { return Exponent(-exponent_bias); }

				static constexpr auto min() { return Exponent(1 - exponent_bias); }

				static constexpr auto zero() { return Exponent(0); }

				static constexpr auto max() { return Exponent(exponent_bias); }

				static constexpr auto inf() { return Exponent(exponent_bias + 1); }
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
				using BASE = TypedInt<std::uint32_t>;
				using BASE::BASE;

				// NOLINTNEXTLINE(google-explicit-constructor) - This constructor CANNOT be explicit.
				constexpr BiasedExponent(Exponent exp) : BASE(static_cast<std::uint32_t>(static_cast<std::int32_t>(exp) + exponent_bias)) {}

				/**
				 * @brief Cast operator to convert from BiasedExponent to Exponent.
				 */
				explicit constexpr operator Exponent() const { return Exponent(static_cast<std::int32_t>(BASE::value) - exponent_bias); }

				constexpr BiasedExponent & operator++()
				{
					assert(*this != BiasedExponent(Exponent::inf())); // Legal in C++14 and above
					++BASE::value;
					return *this;
				}

				constexpr BiasedExponent & operator--()
				{
					assert(*this != BiasedExponent(Exponent::subnormal())); // Legal in C++14 and above
					--BASE::value;
					return *this;
				}
			};

			/**
			 * @brief An opaque type for storing a floating point significand.
			 *
			 * Inherits from TypedInt<storage_type> and allows storage of significand values.
			 * Provides operators for bitwise OR, XOR, and right shift.
			 * Defines special values like zero, lsb (least significant bit), msb (most significant bit),
			 * and all bits set to one. Arbitrary values are valid within the range [zero, bits_all_ones].
			 * Values exceeding this range are truncated.
			 *
			 * @attention The semantics of the Significand are implementation dependent.
			 */
			struct Significand : TypedInt<storage_type>
			{
				using BASE = TypedInt<storage_type>;
				using BASE::BASE;

				friend constexpr Significand operator|(const Significand a, const Significand b)
				{
					return Significand(storage_type(a.to_storage_type() | b.to_storage_type()));
				}

				friend constexpr Significand operator^(const Significand a, const Significand b)
				{
					return Significand(storage_type(a.to_storage_type() ^ b.to_storage_type()));
				}

				friend constexpr Significand operator>>(const Significand a, int shift) { return Significand(storage_type(a.to_storage_type() >> shift)); }

				static constexpr auto zero() { return Significand(storage_type(0)); }

				static constexpr auto lsb() { return Significand(storage_type(1)); }

				static constexpr auto msb() { return Significand(storage_type(1) << (significand_length - 1)); }

				static constexpr auto bits_all_ones() { return Significand(significand_mask); }
			};

			static constexpr storage_type encode(BiasedExponent exp) { return (exp.to_storage_type() << significand_length) & exponent_mask; }

			static constexpr storage_type encode(Significand value) { return value.to_storage_type() & significand_mask; }

			static constexpr storage_type encode(BiasedExponent exp, Significand sig) { return encode(exp) | encode(sig); }

			static constexpr storage_type encode(types::Sign sign, BiasedExponent exp, Significand sig)
			{
				if (sign.is_neg()) { return sign_mask | encode(exp, sig); }
				return encode(exp, sig);
			}

			constexpr FPStorage() : bits(0) {}

			constexpr explicit FPStorage(storage_type value) : bits(value) {}

			/// Observer Functions

			[[nodiscard]] constexpr storage_type exp_bits() const { return bits & exponent_mask; }

			[[nodiscard]] constexpr storage_type sig_bits() const { return bits & significand_mask; }

			[[nodiscard]] constexpr storage_type exp_sig_bits() const { return bits & exponent_significand_mask; }

			/// Parts of the floating point number

			[[nodiscard]] constexpr BiasedExponent biased_exponent() const
			{
				return BiasedExponent(static_cast<std::uint32_t>(exp_bits() >> significand_length));
			}

			constexpr void set_biased_exponent(BiasedExponent biased) { bits = merge(bits, encode(biased), exponent_mask); }

			/**
			 * @brief The bits of the floating point number represented as an unsigned integer.
			 */
			storage_type bits{};

		public:
			[[nodiscard]] constexpr types::Sign sign() const { return (bits & sign_mask) ? types::Sign::NEG : types::Sign::POS; }
			constexpr void set_sign(types::Sign signVal)
			{
				if (sign() != signVal) { bits ^= sign_mask; }
			}
		};

		/**
		 * @brief This layer defines all functions related to the encoding of the floating point type.
		 *
		 * It facilitates the construction, modification, and observation of values manipulated as 'storage_type'.
		 */
		template <FPType fp_type, typename RetT>
		struct FPRepSem : FPStorage<fp_type>
		{
			using BASE = FPStorage<fp_type>;
			using BASE::fraction_length;
			using BASE::fraction_mask;
			using typename BASE::storage_type;

		protected:
			using BASE::BASE;
			using BASE::bits;
			using BASE::encode;
			using BASE::exp_bits;
			using BASE::exp_sig_bits;
			using BASE::sig_bits;
			using typename BASE::Exponent;
			using typename BASE::Significand;

		public:
			/// Builder Functions

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

			static constexpr RetT signaling_nan(types::Sign sign = types::Sign::POS, storage_type v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), (v ? Significand(v) : (Significand::msb() >> 1))));
			}

			static constexpr RetT quiet_nan(types::Sign sign = types::Sign::POS, storage_type v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | Significand(v)));
			}

			/// Observer Functions

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
				if (is_finite()) { return RetT(bits + storage_type(1)); }
				return RetT(bits);
			}

			/**
			 * @brief Returns the mantissa with the implicit bit set if the current value is a valid normal number.
			 * @return The explicit mantissa as a storage_type.
			 *
			 * This function checks if the current value is subnormal. If it is, it returns the significand bits.
			 * Otherwise, it returns the mantissa with the implicit bit set.
			 */
			constexpr storage_type get_explicit_mantissa() const
			{
				if (is_subnormal()) { return sig_bits(); }
				return (storage_type(1) << BASE::significand_length) | sig_bits();
			}
		};

		/**
		 * @brief Template specialization for the X86 Extended Precision floating point type.
		 */
		template <typename RetT>
		struct FPRepSem<FPType::eBinary80, RetT> : FPStorage<FPType::eBinary80>
		{
			using BASE = FPStorage;
			using BASE::fraction_length;
			using BASE::fraction_mask;
			using BASE::storage_type;

			/**
			 * @brief Mask for the leading digit of the mantissa in the x86 80-bit float.
			 *
			 * The x86 80-bit float represents the leading digit of the mantissa explicitly.
			 * This constant defines the mask for that bit.
			 */
			static constexpr storage_type EXPLICIT_BIT_MASK = static_cast<storage_type>(1) << fraction_length;

			/// The X80 significand includes an explicit bit and the fractional part.
			static_assert((EXPLICIT_BIT_MASK & fraction_mask) == 0, "Explicit bit and fractional part must not overlap.");
			static_assert((EXPLICIT_BIT_MASK | fraction_mask) == significand_mask, "Explicit bit and fractional part must cover the entire significand.");

		protected:
			using BASE::BASE;
			using BASE::encode;
			using BASE::Exponent;
			using BASE::Significand;

		public:
			/// Builder Functions

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

			static constexpr RetT signaling_nan(types::Sign sign = types::Sign::POS, storage_type v = 0)
			{
				// NOLINTNEXTLINE(readability-implicit-bool-conversion) - We want v to implicitly convert to bool as if we don't sometimes MSVC breaks.
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | (v ? Significand(v) : (Significand::msb() >> 2))));
			}

			static constexpr RetT quiet_nan(types::Sign sign = types::Sign::POS, storage_type v = 0)
			{
				return RetT(encode(sign, Exponent::inf(), Significand::msb() | (Significand::msb() >> 1) | Significand(v)));
			}

			/// Observer Functions

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
					if (sig_bits() == significand_mask) { return RetT(encode(sign(), ++biased_exponent(), Significand::zero())); }
					return RetT(bits + static_cast<storage_type>(1));
				}
				return RetT(bits);
			}

			[[nodiscard]] constexpr storage_type get_explicit_mantissa() const { return sig_bits(); }

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
			using BASE		   = FPRepSem<fp_type, RetT>;
			using storage_type = typename BASE::storage_type;

		protected:
			using BASE::bits;
			using BASE::encode;
			using BASE::exp_bits;
			using BASE::exp_sig_bits;

			using typename BASE::BiasedExponent;
			using typename BASE::Exponent;
			using typename BASE::Significand;

			using BASE::fp_bits_mask;

		public:
			/// Constants

			using BASE::exponent_bias;
			using BASE::exponent_mask;
			using BASE::fraction_mask;
			using BASE::sign_mask;
			using BASE::significand_length;
			using BASE::significand_mask;
			static constexpr int MAX_BIASED_EXPONENT = (1 << BASE::exponent_length) - 1;

			/// Constructors

			constexpr FPRepImpl() = default;
			constexpr explicit FPRepImpl(storage_type x) : BASE(x) {}

			/// Comparison Operators

			constexpr friend bool operator==(FPRepImpl a, FPRepImpl b) { return a.uintval() == b.uintval(); }
			constexpr friend bool operator!=(FPRepImpl a, FPRepImpl b) { return a.uintval() != b.uintval(); }

			// Representation
			constexpr storage_type uintval() const { return bits & fp_bits_mask; }
			constexpr void set_uintval(storage_type value) { bits = (value & fp_bits_mask); }

			/// Builder  Functions

			using BASE::inf;
			using BASE::max_normal;
			using BASE::max_subnormal;
			using BASE::min_normal;
			using BASE::min_subnormal;
			using BASE::one;
			using BASE::quiet_nan;
			using BASE::signaling_nan;
			using BASE::zero;

			/// Modifier Functions

			constexpr RetT abs() const { return RetT(bits & BASE::exponent_significand_mask); }

			/// Observer Functions

			using BASE::get_explicit_mantissa;
			using BASE::is_finite;
			using BASE::is_inf;
			using BASE::is_nan;
			using BASE::is_normal;
			using BASE::is_quiet_nan;
			using BASE::is_signaling_nan;
			using BASE::is_subnormal;
			using BASE::is_zero;
			using BASE::next_toward_inf;
			using BASE::sign;

			[[nodiscard]] constexpr bool is_inf_or_nan() const { return !is_finite(); }

			[[nodiscard]] constexpr bool is_neg() const { return sign().is_neg(); }

			[[nodiscard]] constexpr bool is_pos() const { return sign().is_pos(); }

			[[nodiscard]] constexpr std::uint16_t get_biased_exponent() const
			{
				return static_cast<std::uint16_t>(static_cast<std::uint32_t>(BASE::biased_exponent()));
			}

			constexpr void set_biased_exponent(storage_type biased)
			{
				BASE::set_biased_exponent(BiasedExponent(static_cast<std::uint32_t>(static_cast<std::int32_t>(biased))));
			}

			[[nodiscard]] constexpr int get_exponent() const { return static_cast<std::int32_t>(Exponent(BASE::biased_exponent())); }

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
				Exponent exponent(BASE::biased_exponent());
				if (is_zero()) { exponent = Exponent::zero(); }
				if (exponent == Exponent::subnormal()) { exponent = Exponent::min(); }
				return static_cast<std::int32_t>(exponent);
			}

			constexpr storage_type get_mantissa() const { return bits & fraction_mask; }

			/**
			 * @brief Sets the mantissa value of the internal bits.
			 * @param mantVal The mantissa value to set.
			 */
			constexpr void set_mantissa(storage_type mantVal) { bits = BASE::merge(bits, mantVal, fraction_mask); }

			/**
			 * @brief Sets the significand value of the internal bits.
			 * @param sigVal The significand value to set.
			 */
			constexpr void set_significand(storage_type sigVal) { bits = BASE::merge(bits, sigVal, significand_mask); }

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
			static constexpr RetT create_value(types::Sign sign, storage_type biased_exp, storage_type mantissa)
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
			static constexpr RetT make_value(storage_type number, int expo)
			{
				// Result = number * 2^(expo + 1 - exponent_bias)
				FPRepImpl result(0);
				int lz = BASE::fraction_length + 1 - (BASE::storage_length - support::countl_zero(number));

				number <<= lz;
				expo -= lz;

				if (CCM_LIKELY(expo >= 0))
				{
					// Mask will remove implicit number bit
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
			using BASE		   = FPRepImpl<fp_type, FPRep<fp_type>>;
			using storage_type = typename BASE::storage_type;
			using BASE::BASE;

			constexpr explicit operator storage_type() const { return BASE::uintval(); }
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
			if constexpr (LDBL_MANT_DIG == 53) { return FPType::eBinary64; }		// long double is the same as double
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
		using BASE		   = internal::FPRepImpl<get_fp_type<T>(), FPBits<T>>;
		using storage_type = typename BASE::storage_type;

		constexpr FPBits() = default;

		template <typename XType>
		constexpr explicit FPBits(XType x)
		{
			using Unqual = std::remove_cv_t<XType>;
			if constexpr (std::is_same_v<Unqual, T>) { BASE::bits = support::bit_cast<storage_type>(x); }
			else if constexpr (std::is_same_v<Unqual, storage_type>) { BASE::bits = x; }
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
		constexpr T get_val() const { return support::bit_cast<T>(BASE::bits); }
	};
} // namespace ccm::support::fp
