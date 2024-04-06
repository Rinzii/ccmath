/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// Large chunks of this code is inspired by or uses code from LLVM
// https://github.com/llvm/llvm-project/

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/predef/compiler_warnings_and_errors.hpp" // Required to disable -fpermissive
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/limits.hpp"
#include "ccmath/internal/support/math_support.hpp"
#include "ccmath/internal/support/type_identity.hpp"

#include <array>
#include <cstddef> // For std::size_t
#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>

#if defined(UINT64_MAX)
	#define CCM_HAS_INT64
#endif // UINT64_MAX

#if defined(__SIZEOF_INT128__)
	#define CCM_HAS_INT128
namespace ccm::internal
{
	// This is a workaround to get -Wpedantic to not complain about the use of
	// unsigned __int128, a non-iso type, inside of an iso function.
	__extension__ using uint128_t = unsigned __int128;
} // namespace ccm::internal
#endif // defined(__SIZEOF_INT128__)

// Since unsigned __int128 is not a standard type, we need to add a specialization for it.
#ifdef CCM_HAS_INT128
namespace std
{
	template <>
	struct [[maybe_unused]] is_unsigned<ccm::internal::uint128_t> : std::true_type // NOLINT
	{
	};
} // namespace std
#endif

namespace ccm
{
	namespace multiword
	{
		// A type trait mapping unsigned integers to their half-width unsigned
		// counterparts.
		template <typename T>
		struct half_width;
		template <>
		struct half_width<uint16_t> : ccm::type_identity<uint8_t>
		{
		};
		template <>
		struct half_width<uint32_t> : ccm::type_identity<uint16_t>
		{
		};
#ifdef CCM_HAS_INT64
		template <>
		struct half_width<uint64_t> : ccm::type_identity<uint32_t>
		{
		};
#endif	   // CCM_HAS_INT64
#ifdef CCM_HAS_INT128
		template <>
		struct half_width<__uint128_t> : ccm::type_identity<uint64_t>
		{
		};
#endif // CCM_HAS_INT128
		template <typename T>
		using half_width_t = typename half_width<T>::type;

		// An array of two elements that can be used in multiword operations.
		template <typename T>
		struct DoubleWide final : std::array<T, 2>
		{
			using UP = std::array<T, 2>;
			using UP::UP;
			inline constexpr DoubleWide(T lo, T hi) : UP({lo, hi}) {}
		};

		// Converts an unsigned value into a DoubleWide<half_width_t<T>>.
		template <typename T>
		inline constexpr auto split(T value)
		{
			static_assert(std::is_unsigned_v<T>);
			using half_type = half_width_t<T>;
			return DoubleWide<half_type>(static_cast<half_type>(value), static_cast<half_type>(value >> ccm::support::numeric_limits<half_type>::digits));
		}

		// The low part of a DoubleWide value.
		template <typename T>
		inline constexpr T lo(const DoubleWide<T> & value)
		{
			return value[0];
		}
		// The high part of a DoubleWide value.
		template <typename T>
		inline constexpr T hi(const DoubleWide<T> & value)
		{
			return value[1];
		}
		// The low part of an unsigned value.
		template <typename T>
		inline constexpr half_width_t<T> lo(T value)
		{
			return lo(split(value));
		}
		// The high part of an unsigned value.
		template <typename T>
		inline constexpr half_width_t<T> hi(T value)
		{
			return hi(split(value));
		}

		// In-place 'dst op= rhs' with operation with carry propagation. Returns carry.
		template <typename Function, typename word, size_t N, size_t M>
		inline constexpr word inplace_binop(Function op_with_carry, std::array<word, N> & dst, const std::array<word, M> & rhs)
		{
			static_assert(N >= M);
			word carry_out = 0;
			for (size_t i = 0; i < N; ++i)
			{
				const bool has_rhs_value = i < M;
				const word rhs_value	 = has_rhs_value ? rhs[i] : 0;
				const word carry_in		 = carry_out;
				dst[i]					 = op_with_carry(dst[i], rhs_value, carry_in, carry_out);
				// stop early when rhs is over and no carry is to be propagated.
				if (!has_rhs_value && carry_out == 0) { break; }
			}
			return carry_out;
		}

		// In-place addition. Returns carry.
		template <typename word, size_t N, size_t M>
		inline constexpr word add_with_carry(std::array<word, N> & dst, const std::array<word, M> & rhs)
		{
			return inplace_binop(ccm::multiword::add_with_carry<word>, dst, rhs);
		}

		// In-place subtraction. Returns borrow.
		template <typename word, size_t N, size_t M>
		inline constexpr word sub_with_borrow(std::array<word, N> & dst, const std::array<word, M> & rhs)
		{
			return inplace_binop(ccm::multiword::sub_with_borrow<word>, dst, rhs);
		}

		// In-place multiply-add. Returns carry.
		// i.e., 'dst += b * c'
		template <typename word, size_t N>
		inline constexpr word mul_add_with_carry(std::array<word, N> & dst, word b, word c)
		{
			return add_with_carry(dst, mul2(b, c));
		}

		// Returns 'a' times 'b' in a DoubleWide<word>. Cannot overflow by construction.
		template <typename word>
		inline constexpr DoubleWide<word> mul2(word a, word b)
		{
			if constexpr (std::is_same_v<word, uint8_t>) { return split<uint16_t>(uint16_t(a) * uint16_t(b)); }
			else if constexpr (std::is_same_v<word, uint16_t>) { return split<uint32_t>(uint32_t(a) * uint32_t(b)); }
#ifdef CCM_HAS_INT64
			else if constexpr (std::is_same_v<word, uint32_t>) { return split<uint64_t>(uint64_t(a) * uint64_t(b)); }
#endif
#ifdef CCM_HAS_INT128
			else if constexpr (std::is_same_v<word, uint64_t>) { return split<__uint128_t>(__uint128_t(a) * __uint128_t(b)); }
#endif
			else
			{
				using half_word	  = half_width_t<word>;
				const auto shiftl = [](word value) -> word { return value << ccm::support::numeric_limits<half_word>::digits; };
				const auto shiftr = [](word value) -> word { return value >> ccm::support::numeric_limits<half_word>::digits; };
				// Here we do a one digit multiplication where 'a' and 'b' are of type
				// word. We split 'a' and 'b' into half words and perform the classic long
				// multiplication with 'a' and 'b' being two-digit numbers.

				//    a      a_hi a_lo
				//  x b => x b_hi b_lo
				// ----    -----------
				//    c         result
				// We convert 'lo' and 'hi' from 'half_word' to 'word' so multiplication
				// doesn't overflow.
				const word a_lo		= lo(a);
				const word b_lo		= lo(b);
				const word a_hi		= hi(a);
				const word b_hi		= hi(b);
				const word step1	= b_lo * a_lo; // no overflow;
				const word step2	= b_lo * a_hi; // no overflow;
				const word step3	= b_hi * a_lo; // no overflow;
				const word step4	= b_hi * a_hi; // no overflow;
				word lo_digit		= step1;
				word hi_digit		= step4;
				const word no_carry = 0;
				word carry;
				word _; // unused carry variable.
				lo_digit = add_with_carry<word>(lo_digit, shiftl(step2), no_carry, carry);
				hi_digit = add_with_carry<word>(hi_digit, shiftr(step2), carry, _);
				lo_digit = add_with_carry<word>(lo_digit, shiftl(step3), no_carry, carry);
				hi_digit = add_with_carry<word>(hi_digit, shiftr(step3), carry, _);
				return DoubleWide<word>(lo_digit, hi_digit);
			}
		}

		// An array of two elements serving as an accumulator during multiword
		// computations.
		template <typename T>
		struct Accumulator final : std::array<T, 2>
		{
			using UP = std::array<T, 2>;
			inline constexpr Accumulator() : UP({0, 0}) {}
			inline constexpr T advance(T carry_in)
			{
				auto result = UP::front();
				UP::front() = UP::back();
				UP::back()	= carry_in;
				return result;
			}
			inline constexpr T sum() const { return UP::front(); }
			inline constexpr T carry() const { return UP::back(); }
		};

		// In-place multiplication by a single word. Returns carry.
		template <typename word, size_t N>
		inline constexpr word scalar_multiply_with_carry(std::array<word, N> & dst, word x)
		{
			Accumulator<word> acc;
			for (auto & val : dst)
			{
				const word carry = mul_add_with_carry(acc, val, x);
				val				 = acc.advance(carry);
			}
			return acc.carry();
		}

		// Multiplication of 'lhs' by 'rhs' into 'dst'. Returns carry.
		// This function is safe to use for signed numbers.
		// https://stackoverflow.com/a/20793834
		// https://pages.cs.wisc.edu/%7Emarkhill/cs354/Fall2008/beyond354/int.mult.html
		template <typename word, size_t O, size_t M, size_t N>
		inline constexpr word multiply_with_carry(std::array<word, O> & dst, const std::array<word, M> & lhs, const std::array<word, N> & rhs)
		{
			static_assert(O >= M + N);
			Accumulator<word> acc;
			for (size_t i = 0; i < O; ++i)
			{
				const size_t lower_idx = i < N ? 0 : i - N + 1;
				const size_t upper_idx = i < M ? i : M - 1;
				word carry			   = 0;
				for (size_t j = lower_idx; j <= upper_idx; ++j) { carry += mul_add_with_carry(acc, lhs[j], rhs[i - j]); }
				dst[i] = acc.advance(carry);
			}
			return acc.carry();
		}

		template <typename word, size_t N>
		inline constexpr void quick_mul_hi(std::array<word, N> & dst, const std::array<word, N> & lhs, const std::array<word, N> & rhs)
		{
			Accumulator<word> acc;
			word carry = 0;
			// First round of accumulation for those at N - 1 in the full product.
			for (size_t i = 0; i < N; ++i) { carry += mul_add_with_carry(acc, lhs[i], rhs[N - 1 - i]); }
			for (size_t i = N; i < 2 * N - 1; ++i)
			{
				acc.advance(carry);
				carry = 0;
				for (size_t j = i - N + 1; j < N; ++j) { carry += mul_add_with_carry(acc, lhs[j], rhs[i - j]); }
				dst[i - N] = acc.sum();
			}
			dst.back() = acc.carry();
		}

		template <typename word, size_t N>
		inline constexpr bool is_negative(std::array<word, N> & array)
		{
			using signed_word = std::make_signed_t<word>;
			return ccm::support::bit_cast<signed_word>(array.back()) < 0;
		}

		// An enum for the shift function below.
		enum Direction : std::uint8_t
		{
			LEFT,
			RIGHT
		};

		// A bitwise shift on an array of elements.
		// TODO: Make the result UB when 'offset' is greater or equal to the number of
		// bits in 'array'. This will allow for better code performance.
		template <Direction direction, bool is_signed, typename word, size_t N>
		inline constexpr std::array<word, N> shift(std::array<word, N> array, size_t offset)
		{
			static_assert(direction == LEFT || direction == RIGHT);
			constexpr size_t WORD_BITS	= ccm::support::numeric_limits<word>::digits;
			constexpr size_t TOTAL_BITS = N * WORD_BITS;
			if (CCM_UNLIKELY(offset == 0)) { return array; }
			if (CCM_UNLIKELY(offset >= TOTAL_BITS)) { return {}; }
#ifdef CCM_HAS_INT128
			if constexpr (TOTAL_BITS == 128)
			{
				using type = std::conditional_t<is_signed, __int128_t, __uint128_t>;
				auto tmp   = ccm::support::bit_cast<type>(array);
				if constexpr (direction == LEFT) { tmp <<= offset; }
				else { tmp >>= offset; }
				return ccm::support::bit_cast<std::array<word, N>>(tmp);
			}
#endif
			const bool is_neg = is_signed && is_negative(array);
			constexpr auto at = [](size_t index) -> int
			{
				// reverse iteration when direction == LEFT.
				if constexpr (direction == LEFT) { return static_cast<int>(N) - static_cast<int>(index) - 1; }
				return static_cast<int>(index);
			};
			const auto safe_get_at = [&](size_t index) -> word
			{
				// return appropriate value when accessing out of bound elements.
				const int i = at(index);
				if (i < 0) { return 0; }
				if (i >= static_cast<int>(N)) { return is_neg ? -1 : 0; }
				return array[i];
			};
			const size_t index_offset = offset / WORD_BITS;
			const size_t bit_offset	  = offset % WORD_BITS;
#ifdef __clang__
			__builtin_assume(index_offset < N);
#endif
			std::array<word, N> out = {};
			for (size_t index = 0; index < N; ++index)
			{
				const word part1 = safe_get_at(index + index_offset);
				const word part2 = safe_get_at(index + index_offset + 1);
				word & dst		 = out[at(index)];
				if (bit_offset == 0)
				{
					dst = part1; // no crosstalk between parts.
				}
				else if constexpr (direction == LEFT) { dst = (part1 << bit_offset) | (part2 >> (WORD_BITS - bit_offset)); }
				else { dst = (part1 >> bit_offset) | (part2 << (WORD_BITS - bit_offset)); }
			}
			return out;
		}

#define DECLARE_COUNTBIT(NAME, INDEX_EXPR)                                                                                                                     \
	template <typename word, size_t N>                                                                                                                         \
	inline constexpr int NAME(const std::array<word, N> & val)                                                                                                 \
	{                                                                                                                                                          \
		int bit_count = 0;                                                                                                                                     \
		for (size_t i = 0; i < N; ++i)                                                                                                                         \
		{                                                                                                                                                      \
			const int word_count = ccm::support::NAME<word>(val[INDEX_EXPR]);                                                                                  \
			bit_count += word_count;                                                                                                                           \
			if (word_count != ccm::support::numeric_limits<word>::digits) break;                                                                                        \
		}                                                                                                                                                      \
		return bit_count;                                                                                                                                      \
	}

		DECLARE_COUNTBIT(countr_zero, i)		 // iterating forward
		DECLARE_COUNTBIT(countr_one, i)			 // iterating forward
		DECLARE_COUNTBIT(countl_zero, N - i - 1) // iterating backward
		DECLARE_COUNTBIT(countl_one, N - i - 1)	 // iterating backward

	} // namespace multiword

	template <size_t Bits, bool Signed, typename WordType = uint64_t>
	struct BigInt // NOLINT(cppcoreguidelines-special-member-functions)
	{
	private:
		static_assert(std::is_integral_v<WordType> && std::is_unsigned_v<WordType>, "WordType must be unsigned integer.");

		struct Division
		{
			BigInt quotient;
			BigInt remainder;
		};

	public:
		using word_type		= WordType;
		using unsigned_type = BigInt<Bits, false, word_type>;
		using signed_type	= BigInt<Bits, true, word_type>;

		inline static constexpr bool SIGNED		 = Signed;
		inline static constexpr size_t BITS		 = Bits;
		inline static constexpr size_t WORD_SIZE = sizeof(WordType) * ccm::support::numeric_limits<unsigned char>::digits;

		static_assert(Bits > 0 && Bits % WORD_SIZE == 0, "Number of bits in BigInt should be a multiple of WORD_SIZE.");

		inline static constexpr size_t WORD_COUNT = Bits / WORD_SIZE;

		std::array<WordType, WORD_COUNT> val{}; // zero initialized.

		inline constexpr BigInt() = default;

		inline constexpr BigInt(const BigInt & other) = default;

		template <size_t OtherBits, bool OtherSigned>
		inline constexpr explicit BigInt(const BigInt<OtherBits, OtherSigned, WordType> & other)
		{
			if (OtherBits >= Bits)
			{ // truncate
				for (size_t i = 0; i < WORD_COUNT; ++i) { val[i] = other[i]; }
			}
			else
			{ // zero or sign extend
				size_t i = 0;
				for (; i < OtherBits / WORD_SIZE; ++i) { val[i] = other[i]; }
				extend(i, Signed && other.is_neg());
			}
		}

		// Construct a BigInt from a C array.
		template <size_t N>
		inline constexpr explicit BigInt(const std::array<WordType, N> & nums)
		{
			static_assert(N == WORD_COUNT);
			for (size_t i = 0; i < WORD_COUNT; ++i) { val[i] = nums[i]; }
		}

		inline constexpr explicit BigInt(const std::array<WordType, WORD_COUNT> & words) : val(words) {}

		// Initialize the first word to |v| and the rest to 0.
		template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
		inline constexpr explicit BigInt(T v)
		{
			constexpr size_t T_SIZE = sizeof(T) * ccm::support::numeric_limits<unsigned char>::digits;
			const bool is_neg		= Signed && (v < 0);
			for (size_t i = 0; i < WORD_COUNT; ++i)
			{
				if (v == 0)
				{
					extend(i, is_neg);
					return;
				}
				val[i] = static_cast<WordType>(v);
				if constexpr (T_SIZE > WORD_SIZE) { v >>= WORD_SIZE; }
				else { v = 0; }
			}
		}
		inline constexpr BigInt & operator=(const BigInt & other) = default;

		// constants
		inline static constexpr BigInt zero() { return BigInt(); }
		inline static constexpr BigInt one() { return BigInt(1); }
		inline static constexpr BigInt all_ones() { return ~zero(); }
		inline static constexpr BigInt min()
		{
			BigInt out;
			if constexpr (SIGNED) { out.set_msb(); }
			return out;
		}
		inline static constexpr BigInt max()
		{
			BigInt out = all_ones();
			if constexpr (SIGNED) { out.clear_msb(); }
			return out;
		}

		[[nodiscard]] inline constexpr bool is_neg() const { return SIGNED && get_msb(); }

		template <typename T>
		inline constexpr explicit operator T() const
		{
			return to<T>();
		}

		template <typename T>
		inline constexpr std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, T> to() const
		{
			constexpr size_t T_SIZE = sizeof(T) * ccm::support::numeric_limits<unsigned char>::digits;
			T lo					= static_cast<T>(val[0]);
			if constexpr (T_SIZE <= WORD_SIZE) { return lo; }
			constexpr size_t MAX_COUNT = T_SIZE > Bits ? WORD_COUNT : T_SIZE / WORD_SIZE;
			for (size_t i = 1; i < MAX_COUNT; ++i) { lo += static_cast<T>(val[i]) << (WORD_SIZE * i); }
			if constexpr (Signed && (T_SIZE > Bits))
			{
				// Extend sign for negative numbers.
				constexpr T MASK = (~T(0) << Bits);
				if (is_neg()) { lo |= MASK; }
			}
			return lo;
		}

		inline constexpr explicit operator bool() const { return !is_zero(); }

		[[nodiscard]] inline constexpr bool is_zero() const
		{
			for (auto part : val)
			{
				if (part != 0) { return false; }
			}
			return true;
		}

		// Add 'rhs' to this number and store the result in this number.
		// Returns the carry value produced by the addition operation.
		inline constexpr WordType add_overflow(const BigInt & rhs) { return multiword::add_with_carry(val, rhs.val); }

		inline constexpr BigInt operator+(const BigInt & other) const
		{
			BigInt result = *this;
			result.add_overflow(other);
			return result;
		}

		// This will only apply when initializing a variable from constant values, so
		// it will always use the constexpr version of add_with_carry.
		inline constexpr BigInt operator+(BigInt && other) const // NOLINT
		{
			// We use addition commutativity to reuse 'other' and prevent allocation.
			other.add_overflow(*this); // Returned carry value is ignored.
			return other;
		}

		inline constexpr BigInt & operator+=(const BigInt & other)
		{
			add_overflow(other); // Returned carry value is ignored.
			return *this;
		}

		// Subtract 'rhs' to this number and store the result in this number.
		// Returns the carry value produced by the subtraction operation.
		inline constexpr WordType sub_overflow(const BigInt & rhs) { return multiword::sub_with_borrow(val, rhs.val); }

		inline constexpr BigInt operator-(const BigInt & other) const
		{
			BigInt result = *this;
			result.sub_overflow(other); // Returned carry value is ignored.
			return result;
		}

		inline constexpr BigInt operator-(BigInt && other) const // NOLINT
		{
			BigInt result = *this;
			result.sub_overflow(other); // Returned carry value is ignored.
			return result;
		}

		inline constexpr BigInt & operator-=(const BigInt & other)
		{
			// TODO(lntue): Set overflow flag / errno when carry is true.
			sub_overflow(other); // Returned carry value is ignored.
			return *this;
		}

		// Multiply this number with x and store the result in this number.
		inline constexpr WordType mul(WordType x) { return multiword::scalar_multiply_with_carry(val, x); }

		// Return the full product.
		template <size_t OtherBits>
		inline constexpr auto ful_mul(const BigInt<OtherBits, Signed, WordType> & other) const
		{
			BigInt<Bits + OtherBits, Signed, WordType> result;
			multiword::multiply_with_carry(result.val, val, other.val);
			return result;
		}

		inline constexpr BigInt operator*(const BigInt & other) const
		{
			// Perform full mul and truncate.
			return BigInt(ful_mul(other));
		}

		// Fast hi part of the full product.  The normal product `operator*` returns
		// `Bits` least significant bits of the full product, while this function will
		// approximate `Bits` most significant bits of the full product with errors
		// bounded by:
		//   0 <= (a.full_mul(b) >> Bits) - a.quick_mul_hi(b)) <= WORD_COUNT - 1.
		//
		// An example usage of this is to quickly (but less accurately) compute the
		// product of (normalized) mantissas of floating point numbers:
		//   (mant_1, mant_2) -> quick_mul_hi -> normalize leading bit
		// is much more efficient than:
		//   (mant_1, mant_2) -> ful_mul -> normalize leading bit
		//                    -> convert back to same Bits width by shifting/rounding,
		// especially for higher precisions.
		//
		// Performance summary:
		//   Number of 64-bit x 64-bit -> 128-bit multiplications performed.
		//   Bits  WORD_COUNT  ful_mul  quick_mul_hi  Error bound
		//    128      2         4           3            1
		//    196      3         9           6            2
		//    256      4        16          10            3
		//    512      8        64          36            7
		[[nodiscard]] inline constexpr BigInt quick_mul_hi(const BigInt & other) const
		{
			BigInt result;
			multiword::quick_mul_hi(result.val, val, other.val);
			return result;
		}

		// BigInt(x).pow_n(n) computes x ^ n.
		// Note 0 ^ 0 == 1.
		inline constexpr void pow_n(uint64_t power)
		{
			static_assert(!Signed);
			BigInt result	 = one();
			BigInt cur_power = *this;
			while (power > 0)
			{
				if ((power % 2) > 0) { result *= cur_power; }
				power >>= 1;
				cur_power *= cur_power;
			}
			*this = result;
		}

		// Performs inplace signed / unsigned division. Returns remainder if not
		// dividing by zero.
		// For signed numbers it behaves like C++ signed integer division.
		// That is by truncating the fractionnal part
		// https://stackoverflow.com/a/3602857
		inline constexpr std::optional<BigInt> div(const BigInt & divider)
		{
			if (CCM_UNLIKELY(divider.is_zero())) { return std::nullopt; }
			if (CCM_UNLIKELY(divider == BigInt::one())) { return BigInt::zero(); }
			Division result;
			if constexpr (SIGNED) { result = divide_signed(*this, divider); }
			else { result = divide_unsigned(*this, divider); }
			*this = result.quotient;
			return result.remainder;
		}

		// Efficiently perform BigInt / (x * 2^e), where x is a half-word-size
		// unsigned integer, and return the remainder. The main idea is as follow:
		//   Let q = y / (x * 2^e) be the quotient, and
		//       r = y % (x * 2^e) be the remainder.
		//   First, notice that:
		//     r % (2^e) = y % (2^e),
		// so we just need to focus on all the bits of y that is >= 2^e.
		//   To speed up the shift-and-add steps, we only use x as the divisor, and
		// performing 32-bit shiftings instead of bit-by-bit shiftings.
		//   Since the remainder of each division step < x < 2^(WORD_SIZE / 2), the
		// computation of each step is now properly contained within WordType.
		//   And finally we perform some extra alignment steps for the remaining bits.
		inline constexpr std::optional<BigInt> div_uint_half_times_pow_2(multiword::half_width_t<WordType> x, size_t e)
		{
			BigInt remainder;
			if (x == 0) { return std::nullopt; }
			if (e >= Bits)
			{
				remainder = *this;
				*this	  = BigInt<Bits, false, WordType>();
				return remainder;
			}
			BigInt quotient;
			auto x_word						= static_cast<WordType>(x);
			constexpr size_t LOG2_WORD_SIZE = ccm::support::bit_width(WORD_SIZE) - 1;
			constexpr size_t HALF_WORD_SIZE = WORD_SIZE >> 1;
			constexpr WordType HALF_MASK	= ((WordType(1) << HALF_WORD_SIZE) - 1);
			// lower = smallest multiple of WORD_SIZE that is >= e.
			size_t lower = ((e >> LOG2_WORD_SIZE) + static_cast<size_t>((e & (WORD_SIZE - 1)) != 0)) << LOG2_WORD_SIZE;
			// lower_pos is the index of the closest WORD_SIZE-bit chunk >= 2^e.
			size_t lower_pos = lower / WORD_SIZE;
			// Keep track of current remainder mod x * 2^(32*i)
			WordType rem = 0;
			// pos is the index of the current 64-bit chunk that we are processing.
			size_t pos = WORD_COUNT;

			for (size_t q_pos = WORD_COUNT - lower_pos; q_pos > 0; --q_pos)
			{
				// q_pos is 1 + the index of the current WORD_SIZE-bit chunk of the
				// quotient being processed. Performing the division / modulus with
				// divisor:
				//   x * 2^(WORD_SIZE*q_pos - WORD_SIZE/2),
				// i.e. using the upper (WORD_SIZE/2)-bit of the current WORD_SIZE-bit
				// chunk.
				rem <<= HALF_WORD_SIZE;
				rem += val[--pos] >> HALF_WORD_SIZE;
				WordType q_tmp = rem / x_word;
				rem %= x_word;

				// Performing the division / modulus with divisor:
				//   x * 2^(WORD_SIZE*(q_pos - 1)),
				// i.e. using the lower (WORD_SIZE/2)-bit of the current WORD_SIZE-bit
				// chunk.
				rem <<= HALF_WORD_SIZE;
				rem += val[pos] & HALF_MASK;
				quotient.val[q_pos - 1] = (q_tmp << HALF_WORD_SIZE) + rem / x_word;
				rem %= x_word;
			}

			// So far, what we have is:
			//   quotient = y / (x * 2^lower), and
			//        rem = (y % (x * 2^lower)) / 2^lower.
			// If (lower > e), we will need to perform an extra adjustment of the
			// quotient and remainder, namely:
			//   y / (x * 2^e) = [ y / (x * 2^lower) ] * 2^(lower - e) +
			//                   + (rem * 2^(lower - e)) / x
			//   (y % (x * 2^e)) / 2^e = (rem * 2^(lower - e)) % x
			size_t last_shift = lower - e;

			if (last_shift > 0)
			{
				// quotient * 2^(lower - e)
				quotient <<= last_shift;
				WordType q_tmp = 0;
				WordType d	   = val[--pos];
				if (last_shift >= HALF_WORD_SIZE)
				{
					// The shifting (rem * 2^(lower - e)) might overflow WordTyoe, so we
					// perform a HALF_WORD_SIZE-bit shift first.
					rem <<= HALF_WORD_SIZE;
					rem += d >> HALF_WORD_SIZE;
					d &= HALF_MASK;
					q_tmp = rem / x_word;
					rem %= x_word;
					last_shift -= HALF_WORD_SIZE;
				}
				else
				{
					// Only use the upper HALF_WORD_SIZE-bit of the current WORD_SIZE-bit
					// chunk.
					d >>= HALF_WORD_SIZE;
				}

				if (last_shift > 0)
				{
					rem <<= HALF_WORD_SIZE;
					rem += d;
					q_tmp <<= last_shift;
					x_word <<= HALF_WORD_SIZE - last_shift;
					q_tmp += rem / x_word;
					rem %= x_word;
				}

				quotient.val[0] += q_tmp;

				if (lower - e <= HALF_WORD_SIZE)
				{
					// The remainder rem * 2^(lower - e) might overflow to the higher
					// WORD_SIZE-bit chunk.
					if (pos < WORD_COUNT - 1) { remainder[pos + 1] = rem >> HALF_WORD_SIZE; }
					remainder[pos] = (rem << HALF_WORD_SIZE) + (val[pos] & HALF_MASK);
				}
				else { remainder[pos] = rem; }
			}
			else { remainder[pos] = rem; }

			// Set the remaining lower bits of the remainder.
			for (; pos > 0; --pos) { remainder[pos - 1] = val[pos - 1]; }

			*this = quotient;
			return remainder;
		}

		inline constexpr BigInt operator/(const BigInt & other) const
		{
			BigInt result(*this);
			result.div(other);
			return result;
		}

		inline constexpr BigInt & operator/=(const BigInt & other)
		{
			div(other);
			return *this;
		}

		inline constexpr BigInt operator%(const BigInt & other) const
		{
			BigInt result(*this);
			return *result.div(other);
		}

		inline constexpr BigInt & operator*=(const BigInt & other)
		{
			*this = *this * other;
			return *this;
		}

		inline constexpr BigInt & operator<<=(size_t s)
		{
			val = multiword::shift<multiword::LEFT, SIGNED>(val, s);
			return *this;
		}

		inline constexpr BigInt operator<<(size_t s) const { return BigInt(multiword::shift<multiword::LEFT, SIGNED>(val, s)); }

		inline constexpr BigInt & operator>>=(size_t s)
		{
			val = multiword::shift<multiword::RIGHT, SIGNED>(val, s);
			return *this;
		}

		inline constexpr BigInt operator>>(size_t s) const { return BigInt(multiword::shift<multiword::RIGHT, SIGNED>(val, s)); }

#define DEFINE_BINOP(OP)                                                                                                                                       \
	inline friend constexpr BigInt operator OP(const BigInt & lhs, const BigInt & rhs)                                                                         \
	{                                                                                                                                                          \
		BigInt result;                                                                                                                                         \
		for (size_t i = 0; i < WORD_COUNT; ++i) result[i] = lhs[i] OP rhs[i];                                                                                  \
		return result;                                                                                                                                         \
	}                                                                                                                                                          \
	inline friend constexpr BigInt operator OP##=(BigInt & lhs, const BigInt & rhs)                                                                            \
	{                                                                                                                                                          \
		for (size_t i = 0; i < WORD_COUNT; ++i) lhs[i] OP## = rhs[i];                                                                                          \
		return lhs;                                                                                                                                            \
	}

		DEFINE_BINOP(&) // & and &=
		DEFINE_BINOP(|) // | and |=
		DEFINE_BINOP(^) // ^ and ^=
#undef DEFINE_BINOP

		inline constexpr BigInt operator~() const
		{
			BigInt result;
			for (size_t i = 0; i < WORD_COUNT; ++i)
			{
				result[i] = ~val[i]; // NOLINT
			}
			return result;
		}

		inline constexpr BigInt operator-() const
		{
			BigInt result(*this);
			result.negate();
			return result;
		}

		inline friend constexpr bool operator==(const BigInt & lhs, const BigInt & rhs)
		{
			for (size_t i = 0; i < WORD_COUNT; ++i)
			{
				if (lhs.val[i] != rhs.val[i]) { return false; }
			}
			return true;
		}

		inline friend constexpr bool operator!=(const BigInt & lhs, const BigInt & rhs) { return lhs != rhs; }

		inline friend constexpr bool operator>(const BigInt & lhs, const BigInt & rhs) { return cmp(lhs, rhs) > 0; }
		inline friend constexpr bool operator>=(const BigInt & lhs, const BigInt & rhs) { return cmp(lhs, rhs) >= 0; }
		inline friend constexpr bool operator<(const BigInt & lhs, const BigInt & rhs) { return cmp(lhs, rhs) < 0; }
		inline friend constexpr bool operator<=(const BigInt & lhs, const BigInt & rhs) { return cmp(lhs, rhs) <= 0; }

		inline constexpr BigInt & operator++()
		{
			increment();
			return *this;
		}

		inline constexpr BigInt operator++(int) // NOLINT
		{
			BigInt oldval(*this);
			increment();
			return oldval;
		}

		inline constexpr BigInt & operator--()
		{
			decrement();
			return *this;
		}

		inline constexpr BigInt operator--(int) // NOLINT
		{
			BigInt oldval(*this);
			decrement();
			return oldval;
		}

		// Return the i-th word of the number.
		inline constexpr const WordType & operator[](size_t i) const { return val[i]; }

		// Return the i-th word of the number.
		inline constexpr WordType & operator[](size_t i) { return val[i]; } // NOLINT

	private:
		inline friend constexpr int cmp(const BigInt & lhs, const BigInt & rhs)
		{
			constexpr auto compare = [](WordType a, WordType b) { return a == b ? 0 : a > b ? 1 : -1; };
			if constexpr (Signed)
			{
				const bool lhs_is_neg = lhs.is_neg();
				const bool rhs_is_neg = rhs.is_neg();
				if (lhs_is_neg != rhs_is_neg) { return rhs_is_neg ? 1 : -1; }
			}
			for (size_t i = WORD_COUNT; i-- > 0;)
			{
				if (auto cmp = compare(lhs[i], rhs[i]); cmp != 0) { return cmp; }
			}
			return 0;
		}

		inline constexpr void bitwise_not()
		{
			for (auto & part : val) { part = ~part; }
		}

		inline constexpr void negate()
		{
			bitwise_not();
			increment();
		}

		inline constexpr void increment() { multiword::add_with_carry(val, std::array<WordType, 1>{1}); }

		inline constexpr void decrement() { multiword::add_with_carry(val, std::array<WordType, 1>{1}); }

		inline constexpr void extend(size_t index, bool is_neg)
		{
			const WordType value = is_neg ? ccm::support::numeric_limits<WordType>::max() : ccm::support::numeric_limits<WordType>::min();
			for (size_t i = index; i < WORD_COUNT; ++i) { val[i] = value; }
		}

		[[nodiscard]] inline constexpr bool get_msb() const { return val.back() >> (WORD_SIZE - 1); }

		inline constexpr void set_msb() { val.back() |= ccm::support::mask_leading_ones<WordType, 1>(); }

		inline constexpr void clear_msb() { val.back() &= ccm::support::mask_trailing_ones<WordType, WORD_SIZE - 1>(); }

		inline constexpr void set_bit(size_t i)
		{
			const size_t word_index = i / WORD_SIZE;
			val[word_index] |= WordType(1) << (i % WORD_SIZE);
		}

		inline constexpr static Division divide_unsigned(const BigInt & dividend, const BigInt & divider)
		{
			BigInt remainder = dividend;
			BigInt quotient;
			if (remainder >= divider)
			{
				BigInt subtractor = divider;
				int cur_bit		  = multiword::countl_zero(subtractor.val) - multiword::countl_zero(remainder.val);
				subtractor <<= cur_bit;
				for (; cur_bit >= 0 && remainder > 0; --cur_bit, subtractor >>= 1)
				{
					if (remainder < subtractor) { continue; }
					remainder -= subtractor;
					quotient.set_bit(cur_bit);
				}
			}
			return Division{quotient, remainder};
		}

		inline constexpr static Division divide_signed(const BigInt & dividend, const BigInt & divider)
		{
			// Special case because it is not possible to negate the min value of a
			// signed integer.
			if (dividend == min() && divider == min()) { return Division{one(), zero()}; }
			// 1. Convert the dividend and divisor to unsigned representation.
			unsigned_type udividend(dividend);
			unsigned_type udivider(divider);
			// 2. Negate the dividend if it's negative, and similarly for the divisor.
			const bool dividend_is_neg = dividend.is_neg();
			const bool divider_is_neg  = divider.is_neg();
			if (dividend_is_neg) { udividend.negate(); }
			if (divider_is_neg) { udivider.negate(); }
			// 3. Use unsigned multiword division algorithm.
			const auto unsigned_result = divide_unsigned(udividend, udivider);
			// 4. Convert the quotient and remainder to signed representation.
			Division result;
			result.quotient	 = signed_type(unsigned_result.quotient);
			result.remainder = signed_type(unsigned_result.remainder);
			// 5. Negate the quotient if the dividend and divisor had opposite signs.
			if (dividend_is_neg != divider_is_neg) { result.quotient.negate(); }
			// 6. Negate the remainder if the dividend was negative.
			if (dividend_is_neg) { result.remainder.negate(); }
			return result;
		}

		friend signed_type;
		friend unsigned_type;
	};

	namespace internal
	{
		// We default BigInt's WordType to 'uint64_t' or 'uint32_t' depending on type
		// availability.
		template <size_t Bits>
		struct WordTypeSelector : ccm::type_identity<
#ifdef CCM_HAS_INT64
									  uint64_t
#else
									  uint32_t
#endif // CCM_HAS_INT64
									  >
		{
		};
		// Except if we request 32 bits explicitly.
		template <>
		struct WordTypeSelector<32> : ccm::type_identity<uint32_t>
		{
		};
		template <size_t Bits>
		using WordTypeSelectorT = typename WordTypeSelector<Bits>::type;
	} // namespace internal

	template <size_t Bits>
	using UInt = BigInt<Bits, false, internal::WordTypeSelectorT<Bits>>;

	template <size_t Bits>
	using Int = BigInt<Bits, true, internal::WordTypeSelectorT<Bits>>;

} // namespace ccm

namespace ccm::support
{
	// Provides limits of U/Int<128>.
	template <>
	class numeric_limits<ccm::UInt<128>>
	{
	public:
		static constexpr ccm::UInt<128> max() { return ccm::UInt<128>::max(); }
		static constexpr ccm::UInt<128> min() { return ccm::UInt<128>::min(); }
		// Meant to match std::numeric_limits interface.
		// NOLINTNEXTLINE(readability-identifier-naming)
		static constexpr int digits = 128;
	};

	template <>
	class numeric_limits<ccm::Int<128>>
	{
	public:
		static constexpr ccm::Int<128> max() { return ccm::Int<128>::max(); }
		static constexpr ccm::Int<128> min() { return ccm::Int<128>::min(); }
		// Meant to match std::numeric_limits interface.
		// NOLINTNEXTLINE(readability-identifier-naming)
		static constexpr int digits = 128;
	};
} // namespace ccm::support

//CCM_RESTORE_GCC_WARNING()

namespace ccm
{
	// type traits to determine whether a T is a BigInt.
	template <typename T>
	struct is_big_int : std::false_type
	{
	};

	template <size_t Bits, bool Signed, typename T>
	struct is_big_int<BigInt<Bits, Signed, T>> : std::true_type
	{
	};

	template <class T>
	inline constexpr bool is_big_int_v = is_big_int<T>::value;

	// extensions of type traits to include BigInt

	// is_integral_or_big_int
	template <typename T>
	struct is_integral_or_big_int : std::bool_constant<(std::is_integral_v<T> || is_big_int_v<T>)>
	{
	};

	template <typename T>
	inline constexpr bool is_integral_or_big_int_v = is_integral_or_big_int<T>::value;

	// make_big_int_unsigned
	template <typename T>
	struct make_big_int_unsigned;

	template <size_t Bits, bool Signed, typename T>
	struct make_big_int_unsigned<BigInt<Bits, Signed, T>> : ccm::type_identity<BigInt<Bits, false, T>>
	{
	};

	template <typename T>
	using make_big_int_unsigned_t = typename make_big_int_unsigned<T>::type;

	// make_big_int_signed
	template <typename T>
	struct make_big_int_signed;

	template <size_t Bits, bool Signed, typename T>
	struct make_big_int_signed<BigInt<Bits, Signed, T>> : ccm::type_identity<BigInt<Bits, true, T>>
	{
	};

	template <typename T>
	using make_big_int_signed_t = typename make_big_int_signed<T>::type;

	// make_integral_or_big_int_unsigned
	template <typename T, class = void>
	struct make_integral_or_big_int_unsigned;

	template <typename T>
	struct make_integral_or_big_int_unsigned<T, std::enable_if_t<std::is_integral_v<T>>> : std::make_unsigned<T>
	{
	};

	template <typename T>
	struct make_integral_or_big_int_unsigned<T, std::enable_if_t<is_big_int_v<T>>> : make_big_int_unsigned<T>
	{
	};

	template <typename T>
	using make_integral_or_big_int_unsigned_t = typename make_integral_or_big_int_unsigned<T>::type;

	// make_integral_or_big_int_signed
	template <typename T, class = void>
	struct make_integral_or_big_int_signed;

	template <typename T>
	struct make_integral_or_big_int_signed<T, std::enable_if_t<std::is_integral_v<T>>> : std::make_signed<T>
	{
	};

	template <typename T>
	struct make_integral_or_big_int_signed<T, std::enable_if_t<is_big_int_v<T>>> : make_big_int_signed<T>
	{
	};

	template <typename T>
	using make_integral_or_big_int_signed_t = typename make_integral_or_big_int_signed<T>::type;

	namespace cpp
	{

		// Specialization of ccm::support::bit_cast ('<bit>') from T to BigInt.
		template <typename To, typename From>
		inline constexpr std::enable_if_t<
			(sizeof(To) == sizeof(From)) && std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From> && is_big_int<To>::value, To>
		bit_cast(const From & from)
		{
			To out;
			using Storage = decltype(out.val);
			out.val		  = ccm::support::bit_cast<Storage>(from);
			return out;
		}

		// Specialization of ccm::support::bit_cast ('<bit>') from BigInt to T.
		template <typename To, size_t Bits>
		inline constexpr std::enable_if_t<sizeof(To) == sizeof(UInt<Bits>) && std::is_trivially_constructible_v<To> && std::is_trivially_copyable_v<To> &&
											  std::is_trivially_copyable_v<UInt<Bits>>,
										  To>
		bit_cast(const UInt<Bits> & from)
		{
			return ccm::support::bit_cast<To>(from.val);
		}

		// Specialization of std::popcount ('<bit>') for BigInt.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> popcount(T value)
		{
			int bits = 0;
			for (auto word : value.val)
			{
				if (word) { bits += popcount(word); }
			}
			return bits;
		}

		// Specialization of std::has_single_bit ('<bit>') for BigInt.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, bool> has_single_bit(T value)
		{
			int bits = 0;
			for (auto word : value.val)
			{
				if (word == 0) { continue; }
				bits += popcount(word);
				if (bits > 1) { return false; }
			}
			return bits == 1;
		}

		// Specialization of std::countr_zero ('<bit>') for BigInt.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> countr_zero(const T & value)
		{
			return multiword::countr_zero(value.val);
		}

		// Specialization of std::countl_zero ('<bit>') for BigInt.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> countl_zero(const T & value)
		{
			return multiword::countl_zero(value.val);
		}

		// Specialization of std::countl_one ('<bit>') for BigInt.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> countl_one(T value)
		{
			return multiword::countl_one(value.val);
		}

		// Specialization of std::countr_one ('<bit>') for BigInt.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> countr_one(T value)
		{
			return multiword::countr_one(value.val);
		}

		// Specialization of ccm::support::bit_width ('<bit>') for BigInt.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> bit_width(T value)
		{
			return ccm::support::numeric_limits<T>::digits - ccm::support::countl_zero(value);
		}

		// Forward-declare rotr so that rotl can use it.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, T> rotr(T value, int rotate);

		// Specialization of std::rotl ('<bit>') for BigInt.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, T> rotl(T value, int rotate)
		{
			constexpr unsigned N = ccm::support::numeric_limits<T>::digits;
			rotate				 = rotate % N;
			if (!rotate) { return value; }
			if (rotate < 0) { return ccm::support::rotr<T>(value, -rotate); }
			return (value << rotate) | (value >> (N - rotate));
		}

		// Specialization of std::rotr ('<bit>') for BigInt.
		template <typename T>
		[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, T> rotr(T value, int rotate)
		{
			constexpr unsigned N = ccm::support::numeric_limits<T>::digits;
			rotate				 = rotate % N;
			if (!rotate) { return value; }
			if (rotate < 0) { return ccm::support::rotl<T>(value, -rotate); }
			return (value >> rotate) | (value << (N - rotate));
		}

	} // namespace cpp

	// Specialization of mask_trailing_ones ('math_support.hpp') for BigInt.
	template <typename T, size_t count>
	inline constexpr std::enable_if_t<is_big_int_v<T>, T> mask_trailing_ones()
	{
		static_assert(!T::SIGNED && count <= T::BITS);
		if (count == T::BITS) { return T::all_ones(); }
		constexpr size_t QUOTIENT  = count / T::WORD_SIZE;
		constexpr size_t REMAINDER = count % T::WORD_SIZE;
		T out; // zero initialized
		for (size_t i = 0; i <= QUOTIENT; ++i) { out[i] = i < QUOTIENT ? -1 : mask_trailing_ones<typename T::word_type, REMAINDER>(); }
		return out;
	}

	// Specialization of mask_leading_ones ('math_support.hpp') for BigInt.
	template <typename T, size_t count>
	inline constexpr std::enable_if_t<is_big_int_v<T>, T> mask_leading_ones()
	{
		static_assert(!T::SIGNED && count <= T::BITS);
		if (count == T::BITS) { return T::all_ones(); }
		constexpr size_t QUOTIENT  = (T::BITS - count - 1U) / T::WORD_SIZE;
		constexpr size_t REMAINDER = count % T::WORD_SIZE;
		T out; // zero initialized
		for (size_t i = QUOTIENT; i < T::WORD_COUNT; ++i) { out[i] = i > QUOTIENT ? -1 : mask_leading_ones<typename T::word_type, REMAINDER>(); }
		return out;
	}

	// Specialization of mask_trailing_zeros ('math_support.hpp') for BigInt.
	template <typename T, size_t count>
	inline constexpr std::enable_if_t<is_big_int_v<T>, T> mask_trailing_zeros()
	{
		return mask_leading_ones<T, T::BITS - count>();
	}

	// Specialization of mask_leading_zeros ('math_support.hpp') for BigInt.
	template <typename T, size_t count>
	inline constexpr std::enable_if_t<is_big_int_v<T>, T> mask_leading_zeros()
	{
		return mask_trailing_ones<T, T::BITS - count>();
	}

	// Specialization of count_zeros ('math_support.hpp) for BigInt.
	template <typename T>
	[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> count_zeros(T value)
	{
		return ccm::support::popcount(~value);
	}

	// Specialization of first_leading_zero ('math_support.hpp') for BigInt.
	template <typename T>
	[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> first_leading_zero(T value)
	{
		return value == ccm::support::numeric_limits<T>::max() ? 0 : ccm::support::countl_one(value) + 1;
	}

	// Specialization of first_leading_one ('math_support.hpp') for BigInt.
	template <typename T>
	[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> first_leading_one(T value)
	{
		return first_leading_zero(~value);
	}

	// Specialization of first_trailing_zero ('math_support.hpp') for BigInt.
	template <typename T>
	[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> first_trailing_zero(T value)
	{
		return value == ccm::support::numeric_limits<T>::max() ? 0 : ccm::support::countr_zero(~value) + 1;
	}

	// Specialization of first_trailing_one ('math_support.hpp') for BigInt.
	template <typename T>
	[[nodiscard]] inline constexpr std::enable_if_t<is_big_int_v<T>, int> first_trailing_one(T value)
	{
		return value == ccm::support::numeric_limits<T>::max() ? 0 : ccm::support::countr_zero(value) + 1;
	}

} // namespace ccm
