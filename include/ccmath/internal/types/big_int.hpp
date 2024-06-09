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

#include "ccmath/internal/config/compiler.hpp"
#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/predef/assume.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/math_support.hpp"
#include "ccmath/internal/support/type_traits.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>

namespace ccm::types
{
	namespace multiword
	{

		/**
		 * @brief Type trait that maps unsigned integers to their corresponding half-width types.
		 *
		 * This struct provides a type alias for the half-width equivalent of the given unsigned integer type.
		 * For example, std::uint32_t maps to std::uint16_t.
		 */
		template <typename T>
		struct half_width;

		template <>
		struct half_width<std::uint16_t> : support::traits::type_identity<std::uint8_t>
		{
		};

		template <>
		struct half_width<std::uint32_t> : support::traits::type_identity<std::uint16_t>
		{
		};
#ifdef CCM_TYPES_HAS_INT64
		template <>
		struct half_width<std::uint64_t> : support::traits::type_identity<std::uint32_t>
		{
		};
	#ifdef CCM_TYPES_HAS_INT128
		template <>
		struct half_width<__uint128_t> : support::traits::type_identity<std::uint64_t>
		{
		};
	#endif
#endif

		/**
		 * @brief Alias for the half-width type of the given unsigned integer type.
		 */
		template <typename T>
		using half_width_t = typename half_width<T>::type;

		/**
		 * @brief An array of two elements for use in multiword operations.
		 *
		 * This struct provides a two-element array, useful for operations that require handling multiword data.
		 */
		template <typename T>
		struct DoubleWide final : std::array<T, 2>
		{
			using UP = std::array<T, 2>;
			using UP::UP;

			/**
			 * @brief Constructs a DoubleWide with the given low and high values.
			 * @param lo The low part of the double-wide value.
			 * @param hi The high part of the double-wide value.
			 */
			constexpr DoubleWide(T lo, T hi) : UP({lo, hi}) {}
		};

		/**
		 * @brief Splits an unsigned value into a DoubleWide containing its half-width components.
		 * @tparam T The type of the input value, which must be an unsigned integer.
		 * @param value The unsigned integer value to be split.
		 * @return A DoubleWide object containing the lower and upper half-width parts of the input value.
		 */
		template <typename T>
		constexpr auto split(T value)
		{
			static_assert(ccm::support::traits::ccm_is_unsigned_v<T>);
			using half_type = half_width_t<T>;
			return DoubleWide<half_type>(half_type(value), half_type(value >> std::numeric_limits<half_type>::digits));
		}

		/**
		 * @brief Returns the lower half of a DoubleWide value.
		 * @tparam T The type of the DoubleWide value.
		 * @param value The DoubleWide value to split.
		 * @return The lower half of the DoubleWide value.
		 */
		template <typename T>
		constexpr T lo(const DoubleWide<T> & value)
		{
			return value[0];
		}

		/**
		 * @brief Returns the upper half of a DoubleWide value.
		 * @tparam T The type of the DoubleWide value.
		 * @param value The DoubleWide value to split.
		 * @return The upper half of the DoubleWide value.
		 */
		template <typename T>
		constexpr T hi(const DoubleWide<T> & value)
		{
			return value[1];
		}

		/**
		 * @brief Returns the lower half of a unsigned value
		 * @tparam T The type of the input value, which must be an unsigned integer.
		 * @param value The unsigned integer value to be split.
		 * @return The lower half of the input value.
		 */
		template <typename T>
		constexpr half_width_t<T> lo(T value)
		{
			return lo(split<T>(value));
		}

		/**
		 * @brief Returns the upper half of a unsigned value
		 * @tparam T The type of the input value, which must be an unsigned integer.
		 * @param value The unsigned integer value to be split.
		 * @return The upper half of the input value.
		 */
		template <typename T>
		constexpr half_width_t<T> hi(T value)
		{
			return hi(split<T>(value));
		}

		/**
		 * @brief Multiplies two words and returns the result as a DoubleWide containing the product.
		 *
		 * This function performs multiplication of two words (unsigned integers) and returns the result
		 * in a DoubleWide object, which stores the product in two parts (lower and upper half-width components).
		 *
		 * @tparam word The type of the input values, which must be an unsigned integer type.
		 * @param lhs The left-hand side operand.
		 * @param rhs The right-hand side operand.
		 * @return A DoubleWide object containing the lower and upper parts of the product.
		 *
		 * This function uses different multiplication strategies based on the size of the input type:
		 * - For 8-bit and 16-bit types, it performs a straightforward multiplication and splits the result.
		 * - For 32-bit and 64-bit types (if supported), it performs a straightforward multiplication and splits the result.
		 * - For larger types, it uses a long multiplication approach, splitting the inputs into half words and performing
		 *   the multiplication in steps to avoid overflow, then combines the results.
		 */
		template <typename word>
		constexpr DoubleWide<word> mul2(word lhs, word rhs)
		{
			if constexpr (std::is_same_v<word, std::uint8_t>)
			{
				return split<std::uint16_t>(static_cast<std::uint16_t>(lhs) * static_cast<std::uint16_t>(rhs));
			}
			else if constexpr (std::is_same_v<word, std::uint16_t>)
			{
				return split<std::uint32_t>(static_cast<std::uint32_t>(lhs) * static_cast<std::uint32_t>(rhs));
			}
#ifdef CCM_TYPES_HAS_INT64
			else if constexpr (std::is_same_v<word, std::uint32_t>)
			{
				return split<std::uint64_t>(static_cast<std::uint64_t>(lhs) * static_cast<std::uint64_t>(rhs));
			}
#endif
#ifdef CCM_TYPES_HAS_INT128
			else if constexpr (std::is_same_v<word, std::uint64_t>)
			{
				return split<__uint128_t>(static_cast<__uint128_t>(lhs) * static_cast<__uint128_t>(rhs));
			}
#endif
			else
			{
				// When dealing with an unknown type, this section performs a single-digit multiplication with 'lhs' and 'rhs' of type 'word'.
				// It splits 'lhs' and 'rhs' into half-words and executes a classic long multiplication,
				// treating 'lhs' and 'rhs' as two-digit numbers.
				//
				//    lhs      lhs_hi lhs_lo
				//  x rhs => x rhs_hi rhs_lo
				// ----    -----------
				//    result
				//
				// We convert 'lo' and 'hi' from 'half_word' to 'word' to prevent overflow during multiplication.

				using half_word	  = half_width_t<word>;
				const auto shiftl = [](word value) -> word { return value << std::numeric_limits<half_word>::digits; };
				const auto shiftr = [](word value) -> word { return value >> std::numeric_limits<half_word>::digits; };

				const word lhs_lo	= lo(lhs);
				const word rhs_lo	= lo(rhs);
				const word lhs_hi	= hi(lhs);
				const word rhs_hi	= hi(rhs);
				const word step1	= rhs_lo * lhs_lo; // Won't overflow
				const word step2	= rhs_lo * lhs_hi; // Won't overflow
				const word step3	= rhs_hi * lhs_lo; // Won't overflow
				const word step4	= rhs_hi * lhs_hi; // Won't overflow
				word lo_digit		= step1;
				word hi_digit		= step4;
				const word no_carry = 0;
				word carry;
				word _; // Placeholder variable for add_with_carry
				lo_digit = ccm::support::add_with_carry<word>(lo_digit, shiftl(step2), no_carry, carry);
				hi_digit = ccm::support::add_with_carry<word>(hi_digit, shiftr(step2), carry, _);
				lo_digit = ccm::support::add_with_carry<word>(lo_digit, shiftl(step3), no_carry, carry);
				hi_digit = ccm::support::add_with_carry<word>(hi_digit, shiftr(step3), carry, _);
				return DoubleWide<word>(lo_digit, hi_digit);
			}
		}

		/**
		 * @brief Performs in-place binary operation 'dst op= rhs' with carry propagation.
		 *
		 * This function applies the provided binary operation with carry propagation to each element in the 'dst' array,
		 * using elements from the 'rhs' array. It returns the final carry value.
		 *
		 * @tparam Function The type of the binary operation with carry.
		 * @tparam word The type of the elements in the arrays.
		 * @tparam N The size of the 'dst' array.
		 * @tparam M The size of the 'rhs' array.
		 * @param op_with_carry The binary operation function that includes carry propagation.
		 * @param dst The destination array to be modified.
		 * @param rhs The source array for the operation.
		 * @return The carry value after the operation completes.
		 */
		template <typename Function, typename word, std::size_t N, std::size_t M>
		constexpr word inplace_binop(Function op_with_carry, std::array<word, N> & dst, const std::array<word, M> & rhs)
		{
			static_assert(N >= M);
			word carry_out = 0;
			for (std::size_t i = 0; i < N; ++i)
			{
				const bool has_rhs_value = i < M;
				const word rhs_value	 = has_rhs_value ? rhs[i] : 0;
				const word carry_in		 = carry_out;
				dst[i]					 = op_with_carry(dst[i], rhs_value, carry_in, carry_out);

				// Stop early if 'rhs' is exhausted and no carry remains to be propagated.
				if (!has_rhs_value && carry_out == 0) { break; }
			}
			return carry_out;
		}

		/**
		 * @brief Performs in-place addition and returns the final carry.
		 *
		 * This function adds elements from the 'rhs' array to the 'dst' array with carry propagation,
		 * using the 'add_with_carry' function. It returns the final carry value.
		 *
		 * @tparam word The type of the elements in the arrays.
		 * @tparam N The size of the 'dst' array.
		 * @tparam M The size of the 'rhs' array.
		 * @param dst The destination array to be modified by the addition.
		 * @param rhs The source array providing the values to add.
		 * @return The carry value after the addition completes.
		 */
		template <typename word, std::size_t N, std::size_t M>
		constexpr word add_with_carry(std::array<word, N> & dst, const std::array<word, M> & rhs)
		{
			return inplace_binop(ccm::support::add_with_carry<word>, dst, rhs);
		}

		/**
		 * @brief Performs in-place subtraction and returns the final borrow.
		 *
		 * This function subtracts elements from the 'rhs' array from the 'dst' array with borrow propagation.
		 * It uses the 'sub_with_borrow' function to handle the subtraction with borrow. The final borrow value is returned.
		 *
		 * @tparam word The type of the elements in the arrays.
		 * @tparam N The size of the 'dst' array.
		 * @tparam M The size of the 'rhs' array.
		 * @param dst The destination array to be modified by the subtraction.
		 * @param rhs The source array providing the values to subtract.
		 * @return The borrow value after the subtraction completes.
		 */
		template <typename word, std::size_t N, std::size_t M>
		constexpr word sub_with_borrow(std::array<word, N> & dst, const std::array<word, M> & rhs)
		{
			return inplace_binop(ccm::support::sub_with_borrow<word>, dst, rhs);
		}

		/**
		 * @brief Performs in-place multiply-add and returns the final carry.
		 *
		 * This function multiplies 'b' and 'c', adds the result to the 'dst' array with carry propagation,
		 * and returns the final carry value.
		 *
		 * @tparam word The type of the elements in the arrays.
		 * @tparam N The size of the 'dst' array.
		 * @param dst The destination array to be modified by the multiply-add operation.
		 * @param b The multiplier value.
		 * @param c The multiplicand value.
		 * @return The carry value after the multiply-add operation completes.
		 */
		template <typename word, std::size_t N>
		constexpr word mul_add_with_carry(std::array<word, N> & dst, word b, word c)
		{
			return add_with_carry(dst, mul2(b, c));
		}

		/**
		 * @brief An array of two elements used as an accumulator in multiword computations.
		 *
		 * This struct serves as an accumulator for multiword computations, featuring methods to manage carry propagation and summation.
		 *
		 * @tparam T The type of the elements in the array.
		 */
		template <typename T>
		struct Accumulator final : std::array<T, 2>
		{
			using UP = std::array<T, 2>;

			/**
			 * @brief Initializes the Accumulator with zero values.
			 */
			constexpr Accumulator() : UP({0, 0}) {}

			/**
			 * @brief Advances the accumulator with a new carry-in value.
			 *
			 * Shifts the current values in the accumulator, moves the back value to the front,
			 * and sets the back value to the new carry-in value.
			 *
			 * @param carry_in The new carry-in value.
			 * @return The previous front value.
			 */
			constexpr T advance(T carry_in)
			{
				auto result = UP::front();
				UP::front() = UP::back();
				UP::back()	= carry_in;
				return result;
			}

			/**
			 * @brief Returns the sum (front value) of the accumulator.
			 * @return The front value of the accumulator.
			 */
			constexpr T sum() const { return UP::front(); }

			/**
			 * @brief Returns the carry (back value) of the accumulator.
			 * @return The back value of the accumulator.
			 */
			constexpr T carry() const { return UP::back(); }
		};

		/**
		 * @brief Performs in-place multiplication by a single word and returns the final carry.
		 *
		 * This function multiplies each element in the 'dst' array by the given word 'x',
		 * using an accumulator to manage carry propagation. It returns the final carry value.
		 *
		 * @tparam word The type of the elements in the array and the multiplier.
		 * @tparam N The size of the 'dst' array.
		 * @param dst The array to be modified by the multiplication.
		 * @param x The word to multiply each element of the 'dst' array by.
		 * @return The carry value after the multiplication completes.
		 */
		template <typename word, std::size_t N>
		constexpr word scalar_multiply_with_carry(std::array<word, N> & dst, word x)
		{
			Accumulator<word> acc;
			for (auto & val : dst)
			{
				const word carry = mul_add_with_carry(acc, val, x);
				val				 = acc.advance(carry);
			}
			return acc.carry();
		}

		/**
		 * @brief Multiplies 'lhs' by 'rhs' and stores the result in 'dst', returning the final carry.
		 *
		 * This function multiplies the elements of 'lhs' and 'rhs', storing the result in 'dst' and managing carry propagation.
		 * It is safe to use for signed numbers.
		 *
		 * References:
		 * - https://stackoverflow.com/a/20793834
		 * - https://pages.cs.wisc.edu/%7Emarkhill/cs354/Fall2008/beyond354/int.mult.html
		 *
		 * @tparam word The type of the elements in the arrays.
		 * @tparam N The size of the 'dst' array.
		 * @param dst The array to store the result of the multiplication.
		 * @param lhs The left-hand side operand array.
		 * @param rhs The right-hand side operand array.
		 * @return The carry value after the multiplication completes.
		 */
		template <typename word, size_t O, size_t M, size_t N>
		constexpr word multiply_with_carry(std::array<word, O> & dst, const std::array<word, M> & lhs, const std::array<word, N> & rhs)
		{
			static_assert(O >= M + N);
			Accumulator<word> acc;
			for (std::size_t i = 0; i < O; ++i)
			{
				const std::size_t lower_idx = i < N ? 0 : i - N + 1;
				const std::size_t upper_idx = i < M ? i : M - 1;
				word carry					= 0;
				for (std::size_t j = lower_idx; j <= upper_idx; ++j) { carry += mul_add_with_carry(acc, lhs[j], rhs[i - j]); }
				dst[i] = acc.advance(carry);
			}
			return acc.carry();
		}

		/**
		 * @brief Multiplies 'lhs' by 'rhs' and stores the high part of the result in 'dst'.
		 *
		 * This function performs multiplication of 'lhs' and 'rhs' arrays, accumulating the high part of the product in 'dst'.
		 *
		 * @tparam word The type of the elements in the arrays.
		 * @tparam N The size of the 'dst', 'lhs', and 'rhs' arrays.
		 * @param dst The array to store the high part of the product.
		 * @param lhs The left-hand side operand array.
		 * @param rhs The right-hand side operand array.
		 */
		template <typename word, std::size_t N>
		constexpr void quick_mul_hi(std::array<word, N> & dst, const std::array<word, N> & lhs, const std::array<word, N> & rhs)
		{
			Accumulator<word> acc;
			word carry = 0;

			// Initial accumulation for elements at N - 1 in the full product.
			for (std::size_t i = 0; i < N; ++i) { carry += mul_add_with_carry(acc, lhs[i], rhs[N - 1 - i]); }

			// Accumulate and propagate carry for the remaining elements.
			for (std::size_t i = N; i < 2 * N - 1; ++i)
			{
				acc.advance(carry);
				carry = 0;
				for (std::size_t j = i - N + 1; j < N; ++j) { carry += mul_add_with_carry(acc, lhs[j], rhs[i - j]); }
				dst[i - N] = acc.sum();
			}
			dst.back() = acc.carry();
		}

		/**
		 * @brief Checks if the value represented by the array is negative.
		 *
		 * @tparam word The type of the elements in the array.
		 * @tparam N The size of the array.
		 * @param array The array to check.
		 * @return True if the value is negative, false otherwise.
		 */
		template <typename word, std::size_t N>
		constexpr bool is_negative(std::array<word, N> & array)
		{
			using signed_word = std::make_signed_t<word>;
			return support::bit_cast<signed_word>(array.back()) < 0;
		}

		/**
		 * @brief Internal representation of a multiword shift direction.
		 */
		enum class Direction : std::uint8_t
		{
			eLEFT,
			eRIGHT
		};

		/**
		 * @brief Performs a bitwise shift on an array of elements.
		 *
		 * @tparam direction The direction of the shift (LEFT or RIGHT).
		 * @tparam is_signed Whether the elements are signed.
		 * @tparam word The type of the elements in the array.
		 * @tparam N The size of the array.
		 * @param array The array to be shifted.
		 * @param offset The number of bits to shift.
		 * @return The shifted array.
		 *
		 * @note 'offset' must be less than TOTAL_BITS (sizeof(word) * CHAR_BIT * N).
		 */
		template <Direction direction, bool is_signed, typename word, std::size_t N>
		constexpr std::array<word, N> shift(std::array<word, N> array, std::size_t offset)
		{
			static_assert(direction == Direction::eLEFT || direction == Direction::eRIGHT);
			constexpr std::size_t WORD_BITS = std::numeric_limits<word>::digits;
#ifdef CCM_TYPES_HAS_INT128
			if constexpr (constexpr std::size_t TOTAL_BITS = N * WORD_BITS; TOTAL_BITS == 128)
			{
				using type = std::conditional_t<is_signed, __int128_t, __uint128_t>;
				auto tmp   = ccm::support::bit_cast<type>(array);
				if constexpr (direction == Direction::eLEFT) { tmp <<= offset; }
				else { tmp >>= offset; }
				return ccm::support::bit_cast<std::array<word, N>>(tmp);
			}
#endif
			if (CCM_UNLIKELY(offset == 0)) { return array; }
			const bool is_neg = is_signed && is_negative(array);
			constexpr auto at = [](std::size_t index) -> int
			{
				// Reverse iteration when the direction is LEFT.
				if constexpr (direction == Direction::eLEFT) { return static_cast<int>(N) - static_cast<int>(index) - 1; }
				return static_cast<int>(index);
			};
			const auto safe_get_at = [&](std::size_t index) -> word
			{
				// Return appropriate value for out-of-bound elements.
				const int i = at(index);
				if (i < 0) { return 0; }
				if (i >= static_cast<int>(N)) { return is_neg ? static_cast<word>(-1) : 0; }
				return array[static_cast<std::size_t>(i)];
			};
			const std::size_t index_offset = offset / WORD_BITS;
			const std::size_t bit_offset   = offset % WORD_BITS;
#ifdef CCMATH_COMPILER_CLANG
			CCM_ASSUME(index_offset < N); // CLion Nova may say this is an error. It is not.
#endif
			std::array<word, N> out = {};
			for (std::size_t index = 0; index < N; ++index)
			{
				const word part1 = safe_get_at(index + index_offset);
				const word part2 = safe_get_at(index + index_offset + 1);
				word & dst		 = out[static_cast<std::size_t>(at(index))];
				if (bit_offset == 0) { dst = part1; } // No crosstalk between parts.
				else if constexpr (direction == Direction::eLEFT) { dst = (part1 << bit_offset) | (part2 >> (WORD_BITS - bit_offset)); }
				else { dst = (part1 >> bit_offset) | (part2 << (WORD_BITS - bit_offset)); }
			}
			return out;
		}

#define DECLARE_COUNTBIT(NAME, INDEX_EXPR)                                                                                                                     \
	template <typename word, std::size_t N>                                                                                                                    \
	constexpr int NAME(const std::array<word, N> & val)                                                                                                        \
	{                                                                                                                                                          \
		int bit_count = 0;                                                                                                                                     \
		for (std::size_t i = 0; i < N; ++i)                                                                                                                    \
		{                                                                                                                                                      \
			const int word_count = ccm::support::NAME<word>(val[INDEX_EXPR]);                                                                                  \
			bit_count += word_count;                                                                                                                           \
			if (word_count != std::numeric_limits<word>::digits) break;                                                                                        \
		}                                                                                                                                                      \
		return bit_count;                                                                                                                                      \
	}

		// Internal implementations for specializations for counting leading and trailing zeros and ones
		DECLARE_COUNTBIT(countr_zero, i)		 // iterating forward
		DECLARE_COUNTBIT(countr_one, i)			 // iterating forward
		DECLARE_COUNTBIT(countl_zero, N - i - 1) // iterating backward
		DECLARE_COUNTBIT(countl_one, N - i - 1)	 // iterating backward
	} // namespace multiword

	template <std::size_t Bits, bool Signed, typename WordType = std::uint64_t>
	struct BigInt
	{
	private:
		static_assert(ccm::support::traits::ccm_is_integral_v<WordType> && ccm::support::traits::ccm_is_unsigned_v<WordType>,
					  "WordType must be unsigned integer!");

		struct Division
		{
			BigInt quotient;
			BigInt remainder;
		};

	public:
		using word_type		= WordType;
		using unsigned_type = BigInt<Bits, false, word_type>;
		using signed_type	= BigInt<Bits, true, word_type>;

		static constexpr bool SIGNED		   = Signed;
		static constexpr std::size_t BITS	   = Bits;
		static constexpr std::size_t WORD_SIZE = sizeof(WordType) * CHAR_BIT;

		static_assert(Bits > 0 && Bits % WORD_SIZE == 0, "Number of bits in BigInt should be a multiple of WORD_SIZE.");

		static constexpr std::size_t WORD_COUNT = Bits / WORD_SIZE;

		std::array<WordType, WORD_COUNT> val{}; // Defaults zero-initialized

		constexpr BigInt() = default;

		constexpr BigInt(const BigInt & other) = default;

		/**
		 * @brief Constructs a BigInt from another BigInt with potentially different size and signedness.
		 *
		 * This constructor initializes a BigInt from another BigInt of potentially different bit width and signedness.
		 * If the source BigInt has more bits, it truncates the extra bits. Otherwise, it extends the value by either
		 * zero or sign extension based on the signedness of the source BigInt.
		 *
		 * @tparam OtherBits The bit width of the source BigInt.
		 * @tparam OtherSigned The signedness of the source BigInt.
		 * @param other The source BigInt to construct from.
		 */
		template <size_t OtherBits, bool OtherSigned>
		constexpr BigInt(const BigInt<OtherBits, OtherSigned, WordType> & other)
		{
			if (OtherBits >= Bits)
			{
				// Truncate the extra bits
				for (size_t i = 0; i < WORD_COUNT; ++i) { val[i] = other[i]; }
			}
			else
			{
				// Zero or sign extend based on the signedness
				size_t i = 0;
				for (; i < OtherBits / WORD_SIZE; ++i) { val[i] = other[i]; }
				extend(i, Signed && other.is_neg());
			}
		}

		/**
		 * @brief Constructs a BigInt from a C-style array of words.
		 * @tparam N The size of the input array, which must match the WORD_COUNT.
		 * @param nums The input array of WordType values.
		 */
		template <std::size_t N>
		constexpr BigInt(
			const WordType (&nums)[N]) // NOLINT(cppcoreguidelines-avoid-c-arrays) - We are intentionally using C-style arrays here.
		{
			static_assert(N == WORD_COUNT);
			for (std::size_t i = 0; i < WORD_COUNT; ++i) { val[i] = nums[i]; }
		}

		/**
		 * @brief Constructs a BigInt from a std::array of words.
		 * @param words The input std::array of WordType values.
		 */
		constexpr explicit BigInt(const std::array<WordType, WORD_COUNT> & words) : val(words) {}

		/**
		 * @brief Constructs a BigInt from an integral value.
		 *
		 * This constructor initializes the BigInt with the given integral value. The first word is set to the value,
		 * and the remaining words are initialized to zero, with sign extension if needed.
		 *
		 * @tparam T The type of the integral value.
		 * @param v The integral value to initialize the BigInt with.
		 */
		template <typename T, typename = std::enable_if_t<support::traits::ccm_is_integral_v<T>>>
		// NOLINTNEXTLINE(google-explicit-constructor) - Cannot be marked explicit.
		constexpr BigInt(T v)
		{
			constexpr std::size_t T_SIZE = sizeof(T) * CHAR_BIT;
			const bool is_neg			 = Signed && (v < 0);
			for (std::size_t i = 0; i < WORD_COUNT; ++i)
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

		constexpr BigInt & operator=(const BigInt & other) = default;

		template <typename T>
		constexpr BigInt & operator=(T v)
		{
			*this = BigInt(v);
			return *this;
		}

		/**
		 * @brief Creates a BigInt with the value zero.
		 */
		static constexpr BigInt zero() { return BigInt(); }

		/**
		 * @brief Creates a BigInt with the value one.
		 */
		static constexpr BigInt one() { return BigInt(1); }

		/**
		 * @brief Creates a BigInt with all bits set to one.
		 */
		static constexpr BigInt all_ones() { return ~zero(); }

		/**
		 * @brief Creates a BigInt with the minimum value.
		 */
		static constexpr BigInt min()
		{
			BigInt out; // NOLINT(misc-const-correctness)
			if constexpr (SIGNED) { out.set_msb(); }
			return out;
		}

		/**
		 * @brief Creates a BigInt with the maximum value.
		 */
		static constexpr BigInt max()
		{
			BigInt out = all_ones(); // NOLINT(misc-const-correctness)
			if constexpr (SIGNED) { out.clear_msb(); }
			return out;
		}

		/**
		 * @brief Determines if the value of the BigInt is negative.
		 */
		[[nodiscard]] constexpr bool is_neg() const { return SIGNED && get_msb(); }

		/**
		 * @brief Converts the BigInt to another integral type.
		 *
		 * This operator converts the BigInt to the specified integral type.
		 *
		 * @tparam T The target integral type.
		 * @return The BigInt value converted to type T.
		 */
		template <typename T>
		constexpr explicit operator T() const
		{
			return to<T>();
		}

		/**
		 * @brief Converts the BigInt to another integral type.
		 *
		 * This function converts the BigInt to the specified integral type T.
		 *
		 * @tparam T The target integral type.
		 * @return The BigInt value converted to type T.
		 *
		 * @note This function is only enabled for integral types excluding bool.
		 */
		template <typename T>
		constexpr std::enable_if_t<ccm::support::traits::ccm_is_integral_v<T> && !std::is_same_v<T, bool>, T> to() const
		{
			constexpr std::size_t T_SIZE = sizeof(T) * CHAR_BIT;   // Identify the size of the target type.
			auto lo						 = static_cast<T>(val[0]); // Initialize the result with the first word of the BigInt.

			// If the target size is less than or equal to the word size, return the initial value.
			if constexpr (T_SIZE <= WORD_SIZE) { return lo; }

			// Calculate the maximum number of words to process.
			constexpr std::size_t MAX_COUNT = T_SIZE > Bits ? WORD_COUNT : T_SIZE / WORD_SIZE;

			// Combine the words into the result, shifting each word to its proper position.
			for (std::size_t i = 1; i < MAX_COUNT; ++i) { lo += static_cast<T>(val[i]) << (WORD_SIZE * i); }

			// If the BigInt is signed and the target size is greater than the BigInt size,
			// sign-extend the result if the original value was negative.
			if constexpr (Signed && (T_SIZE > Bits))
			{
				constexpr T MASK = (~T(0) << Bits);
				if (is_neg()) { lo |= MASK; }
			}

			return lo;
		}

		/**
		 * @brief Converts the BigInt to a boolean.
		 *
		 * @return True if the BigInt is non-zero, false otherwise.
		 */
		constexpr explicit operator bool() const { return !is_zero(); }

		/**
		 * @brief Checks if the BigInt is zero.
		 *
		 * @return True if all parts of the BigInt are zero, false otherwise.
		 */
		[[nodiscard]] constexpr bool is_zero() const
		{
			// If at any point this operation see's a value that is not zero, it will return false.
			return std::none_of(val.begin(), val.end(), [](auto part) { return part != 0; });
		}

		/**
		 * @brief Adds 'rhs' to this BigInt and stores the result.
		 * @param rhs The BigInt to add.
		 * @return The carry value produced by the addition.
		 */
		constexpr WordType add_overflow(const BigInt & rhs) { return multiword::add_with_carry(val, rhs.val); }

		constexpr BigInt operator+(const BigInt & other) const
		{
			BigInt result = *this;
			result.add_overflow(other);
			return result;
		}

		/**
		 * @brief Adds this BigInt to another and returns the result.
		 *
		 * This operator adds the current BigInt to the given BigInt, using the constexpr version of add_with_carry.
		 * It takes advantage of addition commutativity to modify and return the given BigInt, avoiding additional allocation.
		 *
		 * @param other The BigInt to add.
		 * @return The result of the addition.
		 */
		constexpr BigInt operator+(BigInt && other) const
		{
			std::move(other); // We ignore the moved value here.
			other.add_overflow(*this); // We ignore the returned carry value here.
			return other;
		}

		constexpr BigInt & operator+=(const BigInt & other)
		{
			add_overflow(other); // We ignore the returned carry value here.
			return *this;
		}

		/**
		 * @brief Subtracts 'rhs' from this BigInt and stores the result in this BigInt.
		 * @param rhs The BigInt to subtract.
		 * @return The carry value produced by the subtraction.
		 */
		constexpr WordType sub_overflow(const BigInt & rhs) { return multiword::sub_with_borrow(val, rhs.val); }

		constexpr BigInt operator-(const BigInt & other) const
		{
			BigInt result = *this;
			result.sub_overflow(other); // We ignore the returned carry value here.
			return result;
		}

		constexpr BigInt operator-(BigInt && other) const
		{
			BigInt result = *this;
			result.sub_overflow(std::move(other)); // We ignore the returned carry value here.
			return result;
		}

		constexpr BigInt & operator-=(const BigInt & other)
		{
			if (sub_overflow(other)) // Perform subtraction, and if the returned carry is true, we have an overflow.
			{
				// Attempt to set errno to ERANGE and raise FE_OVERFLOW if we are able to.
				support::fenv::set_errno_if_required(ERANGE);
				support::fenv::raise_except_if_required(FE_OVERFLOW);
			}
			return *this;
		}

		/**
		 * @brief Multiplies this BigInt by a scalar and stores the result in this BigInt.
		 * @param x The scalar to multiply with.
		 * @return The carry value produced by the multiplication.
		 */
		constexpr WordType mul(WordType x) { return multiword::scalar_multiply_with_carry(val, x); }

		/**
		 * @brief Multiplies this BigInt with another and returns the full product.
		 * @tparam OtherBits The bit width of the other BigInt.
		 * @param other The BigInt to multiply with.
		 * @return The full product as a new BigInt.
		 */
		template <std::size_t OtherBits>
		constexpr auto ful_mul(const BigInt<OtherBits, Signed, WordType> & other) const
		{
			BigInt<Bits + OtherBits, Signed, WordType> result;
			multiword::multiply_with_carry(result.val, val, other.val);
			return result;
		}

		/**
		 * @brief Multiplies this BigInt with another and returns the full product tructated.
		 */
		constexpr BigInt operator*(const BigInt & other) const { return BigInt(ful_mul(other)); }

		/**
		 * @brief Approximates the high bits of the full product of two BigInts.
		 *
		 * This function computes an approximation of the high 'Bits' of the full product of two BigInts.
		 * The error is bounded by: 0 <= (a.full_mul(b) >> Bits) - a.quick_mul_hi(b)) <= WORD_COUNT - 1.
		 *
		 * Example usage:
		 * Quickly (but less accurately) compute the product of (normalized) mantissas of floating point numbers:
		 * - (mant_1, mant_2) -> quick_mul_hi -> normalize leading bit
		 * This method is more efficient than:
		 * - (mant_1, mant_2) -> ful_mul -> normalize leading bit -> convert back to same Bits width by shifting/rounding,
		 * especially for higher precisions.
		 *
		 * Performance summary:
		 * Number of 64-bit x 64-bit -> 128-bit multiplications performed.
		 * | Bits | WORD_COUNT | ful_mul | quick_mul_hi | Error bound |
		 * |------|-------------|---------|--------------|-------------|
		 * |  128 |           2 |       4 |            3 |           1 |
		 * |  196 |           3 |       9 |            6 |           2 |
		 * |  256 |           4 |      16 |           10 |           3 |
		 * |  512 |           8 |      64 |           36 |           7 |
		 *
		 * @param other The BigInt to multiply with.
		 * @return The approximate high bits of the product as a new BigInt.
		 */
		[[nodiscard]] constexpr BigInt quick_mul_hi(const BigInt & other) const
		{
			BigInt result;
			multiword::quick_mul_hi(result.val, val, other.val);
			return result;
		}

		/**
		 * @brief Computes this BigInt raised to the power of 'expo'.
		 * @note 0 ^ 0 is equal to 1.
		 */
		constexpr void pow_n(std::uint64_t expo)
		{
			static_assert(!Signed);
			BigInt result	 = one();
			BigInt cur_power = *this;
			while (expo > 0)
			{
				if ((expo % 2) > 0) { result *= cur_power; }
				expo >>= 1;
				cur_power *= cur_power;
			}
			*this = result;
		}

		/**
		 * @brief Performs in-place signed or unsigned division and returns the remainder.
		 *
		 * This function divides the current BigInt by the given divisor and stores the quotient in the current BigInt.
		 * It returns the remainder if the division is not by zero. For signed numbers, it behaves like C++ signed integer division,
		 * truncating the fractional part.
		 *
		 * @param divider The BigInt to divide by.
		 * @return An optional containing the remainder if the division is valid, or std::nullopt if dividing by zero.
		 * Reference used:
		 * - https://stackoverflow.com/a/3602857
		 */
		constexpr std::optional<BigInt> div(const BigInt & divider)
		{
			if (CCM_UNLIKELY(divider.is_zero())) { return std::nullopt; }
			if (CCM_UNLIKELY(divider == BigInt::one())) { return BigInt::zero(); }
			Division result;
			if constexpr (SIGNED) { result = divide_signed(*this, divider); }
			else { result = divide_unsigned(*this, divider); }
			*this = result.quotient;
			return result.remainder;
		}

		/**
		 * @brief Efficiently performs BigInt / (x * 2^e), where x is a half-word-size unsigned integer, and returns the remainder.
		 *
		 * This function divides the BigInt by (x * 2^e) efficiently, using the following approach:
		 * - Let q = y / (x * 2^e) be the quotient, and r = y % (x * 2^e) be the remainder.
		 * - The remainder r % (2^e) is y % (2^e), so we only need to focus on bits of y that are >= 2^e.
		 * - To speed up the division, we use x as the divisor and perform 32-bit shifts instead of bit-by-bit shifts.
		 * - Since the remainder of each division step is < x < 2^(WORD_SIZE / 2), the computation stays within WordType.
		 * - Finally, extra alignment steps are performed for the remaining bits.
		 *
		 * @param x The half-word-size unsigned integer.
		 * @param e The exponent for the power of 2.
		 * @return An optional containing the remainder if the division is valid, or std::nullopt if x is zero.
		 */
		constexpr std::optional<BigInt> div_uint_half_times_pow_2(multiword::half_width_t<WordType> x, size_t e)
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
			auto x_word							 = static_cast<WordType>(x);
			constexpr std::size_t LOG2_WORD_SIZE = ccm::support::bit_width(WORD_SIZE) - 1;
			constexpr std::size_t HALF_WORD_SIZE = WORD_SIZE >> 1;
			constexpr WordType HALF_MASK		 = ((WordType(1) << HALF_WORD_SIZE) - 1);

			// "lower" is equal to the smallest multiple of WORD_SIZE that is >= e.
			std::size_t lower = ((e >> LOG2_WORD_SIZE) + static_cast<std::size_t>((e & (WORD_SIZE - 1)) != 0)) << LOG2_WORD_SIZE;

			// "lower_pos" is the index of the closest WORD_SIZE bit chunk >= 2^e.
			std::size_t lower_pos = lower / WORD_SIZE;

			WordType rem = 0; // Track the current remainder of mod x * 2^(32*i)

			// pos represents the current index of the current 64-bit chunk that we are currently processing.
			std::size_t pos = WORD_COUNT;

			// Process each WORD_SIZE-bit chunk.
			for (std::size_t q_pos = WORD_COUNT - lower_pos; q_pos > 0; --q_pos)
			{
				// q_pos is the index of the current WORD_SIZE-bit chunk of the quotient being processed, plus 1.
				// Perform division and modulus with the divisor:
				//   x * 2^(WORD_SIZE * q_pos - WORD_SIZE / 2),
				// This uses the upper half (WORD_SIZE / 2) bits of the current WORD_SIZE-bit chunk.
				rem <<= HALF_WORD_SIZE;
				rem += val[--pos] >> HALF_WORD_SIZE;
				WordType q_tmp = rem / x_word;
				rem %= x_word;

				/// Perform division and modulus with the divisor:
				//   x * 2^(WORD_SIZE * (q_pos - 1)),
				// This uses the lower half (WORD_SIZE / 2) bits of the current WORD_SIZE-bit chunk.
				rem <<= HALF_WORD_SIZE;
				rem += val[pos] & HALF_MASK;
				quotient.val[q_pos - 1] = (q_tmp << HALF_WORD_SIZE) + rem / x_word;
				rem %= x_word;
			}

			// At this point, we have:
			//   quotient = y / (x * 2^lower)
			//   rem = (y % (x * 2^lower)) / 2^lower
			// If lower > e, we need to adjust the quotient and remainder:
			//   y / (x * 2^e) = [ y / (x * 2^lower) ] * 2^(lower - e) + (rem * 2^(lower - e)) / x
			//   (y % (x * 2^e)) / 2^e = (rem * 2^(lower - e)) % x
			if (std::size_t last_shift = lower - e; last_shift > 0)
			{
				quotient <<= last_shift;
				WordType q_tmp = 0;
				WordType d	   = val[--pos];
				if (last_shift >= HALF_WORD_SIZE)
				{
					// When performing the shifting of (rem * 2^(lower - e)), we might overflow our word type.
					// Due to this, we perform a half-word shift to avoid overflow.
					rem <<= HALF_WORD_SIZE;
					rem += d >> HALF_WORD_SIZE;
					d &= HALF_MASK;
					q_tmp = rem / x_word;
					rem %= x_word;
					last_shift -= HALF_WORD_SIZE;
				}
				else { d >>= HALF_WORD_SIZE; } // Use the upper half-word of the current chunk only.

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
					// IF we have the remainder rem * 2^(lower - e), it might overflow to a higher word type chunk.
					// Due to this, we will adjust the remainder to defend against overflow.
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

		constexpr BigInt operator/(const BigInt & other) const
		{
			BigInt result(*this);
			result.div(other);
			return result;
		}

		constexpr BigInt & operator/=(const BigInt & other)
		{
			div(other);
			return *this;
		}

		constexpr BigInt operator%(const BigInt & other) const
		{
			BigInt result(*this);
			return *result.div(other);
		}

		constexpr BigInt & operator*=(const BigInt & other)
		{
			*this = *this * other;
			return *this;
		}

		constexpr BigInt & operator<<=(std::size_t s)
		{
			val = multiword::shift<multiword::Direction::eLEFT, SIGNED>(val, s);
			return *this;
		}

		constexpr BigInt operator<<(std::size_t s) const { return BigInt(multiword::shift<multiword::Direction::eLEFT, SIGNED>(val, s)); }

		template <typename T>
		constexpr BigInt operator<<(T s) const
		{
			return BigInt(multiword::shift<multiword::Direction::eLEFT, SIGNED>(val, s));
		}

		constexpr BigInt & operator>>=(std::size_t s)
		{
			val = multiword::shift<multiword::Direction::eRIGHT, SIGNED>(val, s);
			return *this;
		}

		constexpr BigInt operator>>(std::size_t s) const { return BigInt(multiword::shift<multiword::Direction::eRIGHT, SIGNED>(val, s)); }

#define DEFINE_BINOP(OP)                                                                                                                                       \
	friend constexpr BigInt operator OP(const BigInt & lhs, const BigInt & rhs)                                                                                \
	{                                                                                                                                                          \
		BigInt result;                                                                                                                                         \
		for (std::size_t i = 0; i < WORD_COUNT; ++i) result[i] = lhs[i] OP rhs[i];                                                                             \
		return result;                                                                                                                                         \
	}                                                                                                                                                          \
	friend constexpr BigInt operator OP##=(BigInt & lhs, const BigInt & rhs)                                                                                   \
	{                                                                                                                                                          \
		for (std::size_t i = 0; i < WORD_COUNT; ++i) lhs[i] OP## = rhs[i];                                                                                     \
		return lhs;                                                                                                                                            \
	}

		DEFINE_BINOP(&) // Defines the bitwise operator & and &=.
		DEFINE_BINOP(|) // Defines the bitwise operator | and |=.
		DEFINE_BINOP(^) // Defines the bitwise operator ^ and ^=.
#undef DEFINE_BINOP

		constexpr BigInt operator~() const
		{
			BigInt result;
			for (std::size_t i = 0; i < WORD_COUNT; ++i) { result[i] = ~val.at(i); }
			return result;
		}

		constexpr BigInt operator-() const
		{
			BigInt result(*this);
			result.negate();
			return result;
		}

		friend constexpr bool operator==(const BigInt & lhs, const BigInt & rhs)
		{
			for (std::size_t i = 0; i < WORD_COUNT; ++i)
			{
				if (lhs.val[i] != rhs.val[i]) { return false; }
			}
			return true;
		}

		template <typename T>
		friend constexpr bool operator==(const BigInt & lhs, T rhs)
		{
			return lhs == BigInt(rhs);
		}

		friend constexpr bool operator!=(const BigInt & lhs, const BigInt & rhs) { return !(lhs == rhs); }

		friend constexpr bool operator>(const BigInt & lhs, const BigInt & rhs) { return cmp(lhs, rhs) > 0; }

		friend constexpr bool operator>=(const BigInt & lhs, const BigInt & rhs) { return cmp(lhs, rhs) >= 0; }

		friend constexpr bool operator<(const BigInt & lhs, const BigInt & rhs) { return cmp(lhs, rhs) < 0; }

		friend constexpr bool operator<=(const BigInt & lhs, const BigInt & rhs) { return cmp(lhs, rhs) <= 0; }

		constexpr BigInt & operator++()
		{
			increment();
			return *this;
		}

		constexpr BigInt operator++(int)
		{
			BigInt oldval(*this);
			increment();
			return oldval;
		}

		constexpr BigInt & operator--()
		{
			decrement();
			return *this;
		}

		constexpr BigInt operator--(int)
		{
			BigInt oldval(*this);
			decrement();
			return oldval;
		}

		constexpr const WordType & operator[](std::size_t i) const { return val[i]; }

		constexpr WordType & operator[](std::size_t i) { return val.at(i); }

	private:
		friend constexpr int cmp(const BigInt & lhs, const BigInt & rhs)
		{
			constexpr auto compare = [](WordType a, WordType b)
			{
				if (a > b) { return a == b ? 0 : 1; }
				return a == b ? 0 : -1;
			};
			if constexpr (Signed)
			{
				const bool lhs_is_neg = lhs.is_neg();
				if (const bool rhs_is_neg = rhs.is_neg(); lhs_is_neg != rhs_is_neg) { return rhs_is_neg ? 1 : -1; }
			}
			for (std::size_t i = WORD_COUNT; i-- > 0;)
			{
				if (auto cmp = compare(lhs[i], rhs[i]); cmp != 0) { return cmp; }
			}
			return 0;
		}

		constexpr void bitwise_not()
		{
			for (auto & part : val) { part = ~part; }
		}

		constexpr void negate()
		{
			bitwise_not();
			increment();
		}

		constexpr void increment() { multiword::add_with_carry(val, std::array<WordType, 1>{1}); }

		constexpr void decrement() { multiword::add_with_carry(val, std::array<WordType, 1>{1}); }

		constexpr void extend(std::size_t index, bool is_neg)
		{
			const WordType value = is_neg ? std::numeric_limits<WordType>::max() : std::numeric_limits<WordType>::min();
			for (std::size_t i = index; i < WORD_COUNT; ++i) { val[i] = value; }
		}

		[[nodiscard]] constexpr bool get_msb() const { return val.back() >> (WORD_SIZE - 1); }

		constexpr void set_msb() { val.back() |= support::mask_leading_ones<WordType, 1>(); }

		constexpr void clear_msb() { val.back() &= support::mask_trailing_ones<WordType, WORD_SIZE - 1>(); }

		constexpr void set_bit(std::size_t i)
		{
			const std::size_t word_index = i / WORD_SIZE;
			val[word_index] |= WordType(1) << (i % WORD_SIZE);
		}

		constexpr static Division divide_unsigned(const BigInt & dividend, const BigInt & divider)
		{
			BigInt remainder = dividend;
			BigInt quotient;
			if (remainder >= divider)
			{
				BigInt subtractor = divider;
				int cur_bit		  = multiword::countl_zero(subtractor.val) - multiword::countl_zero(remainder.val);
				subtractor <<= cur_bit;

				while (cur_bit >= 0 && remainder > 0)
				{
					if (remainder >= subtractor)
					{
						remainder -= subtractor;
						quotient.set_bit(cur_bit);
					}
					--cur_bit;
					subtractor >>= 1;
				}
			}
			return Division{quotient, remainder};
		}

		constexpr static Division divide_signed(const BigInt & dividend, const BigInt & divider)
		{
			// This is a special case for the minimum value, as it is not possible to negate the minimum value of a signed integer.
			if (dividend == min() && divider == min()) { return Division{one(), zero()}; }

			// Convert the dividend and divider to unsigned values.
			unsigned_type udividend(dividend);
			unsigned_type udivider(divider);

			// If the dividend is negative, negate it.
			const bool dividend_is_neg = dividend.is_neg();
			const bool divider_is_neg  = divider.is_neg();
			if (dividend_is_neg) { udividend.negate(); }
			if (divider_is_neg) { udivider.negate(); }

			// Use the unsigned division function to calculate the quotient and remainder.
			const auto unsigned_result = divide_unsigned(udividend, udivider);

			// Convert the unsigned quotient and remainder back to signed values.
			Division result;
			result.quotient	 = signed_type(unsigned_result.quotient);
			result.remainder = signed_type(unsigned_result.remainder);

			// If the dividend and divisor have different signs, negate the quotient.
			if (dividend_is_neg != divider_is_neg) { result.quotient.negate(); }

			// If the dividend was negative, negate the remainder.
			if (dividend_is_neg) { result.remainder.negate(); }
			return result;
		}

		friend signed_type;
		friend unsigned_type;
	};

	namespace internal
	{

		/**
		 * @brief Selects the appropriate default word type for the given bit width based on the available types.
		 * @tparam Bits The bit width for which to select the word type.
		 */
		template <std::size_t Bits>
		struct WordTypeSelector : support::traits::type_identity<
#ifdef CCM_TYPES_HAS_INT64
									  std::uint64_t
#else
									  std::uint32_t
#endif
									  >
		{
		};

		// In special cases where a size of 32 bits is requested, we use a 32-bit word type instead.
		template <>
		struct WordTypeSelector<32> : support::traits::type_identity<std::uint32_t>
		{
		};

		template <std::size_t Bits>
		using WordTypeSelectorT = typename WordTypeSelector<Bits>::type;
	} // namespace internal

	template <std::size_t Bits>
	using UInt = BigInt<Bits, false, internal::WordTypeSelectorT<Bits>>;

	template <std::size_t Bits>
	using Int = BigInt<Bits, true, internal::WordTypeSelectorT<Bits>>;
} // namespace ccm::types

/// Specialization of std::numeric_limits for BigInt types.
namespace std
{
	template <>
	struct numeric_limits<ccm::types::UInt<128>>
	{
	public:
		static constexpr ccm::types::UInt<128> max() { return ccm::types::UInt<128>::max(); }
		static constexpr ccm::types::UInt<128> min() { return ccm::types::UInt<128>::min(); }

		static constexpr int digits = 128;
	};

	template <>
	struct numeric_limits<ccm::types::Int<128>>
	{
	public:
		static constexpr ccm::types::Int<128> max() { return ccm::types::Int<128>::max(); }
		static constexpr ccm::types::Int<128> min() { return ccm::types::Int<128>::min(); }

		static constexpr int digits = 128;
	};
} // namespace std

/// Special helper type traits for BigInt types.
namespace ccm::types
{
	template <typename T>
	struct is_big_int : std::false_type
	{
	};

	template <std::size_t Bits, bool Signed, typename T>
	struct is_big_int<BigInt<Bits, Signed, T>> : std::true_type
	{
	};

	template <class T>
	constexpr bool is_big_int_v = is_big_int<T>::value;

	template <typename T>
	struct is_integral_or_big_int : std::bool_constant<(ccm::support::traits::ccm_is_integral_v<T> || is_big_int_v<T>)>
	{
	};

	template <typename T>
	constexpr bool is_integral_or_big_int_v = is_integral_or_big_int<T>::value;

	template <typename T>
	struct make_big_int_unsigned;

	template <std::size_t Bits, bool Signed, typename T>
	struct make_big_int_unsigned<BigInt<Bits, Signed, T>> : support::traits::type_identity<BigInt<Bits, false, T>>
	{
	};

	template <typename T>
	using make_big_int_unsigned_t = typename make_big_int_unsigned<T>::type;

	template <typename T>
	struct make_big_int_signed;

	template <std::size_t Bits, bool Signed, typename T>
	struct make_big_int_signed<BigInt<Bits, Signed, T>> : support::traits::type_identity<BigInt<Bits, true, T>>
	{
	};

	template <typename T>
	using make_big_int_signed_t = typename make_big_int_signed<T>::type;

	template <typename T, class = void>
	struct make_integral_or_big_int_unsigned;

	template <typename T>
	struct make_integral_or_big_int_unsigned<T, std::enable_if_t<support::traits::ccm_is_integral_v<T>>> : support::traits::ccm_make_unsigned<T>
	{
	};

	template <typename T>
	struct make_integral_or_big_int_unsigned<T, std::enable_if_t<is_big_int_v<T>>> : make_big_int_unsigned<T>
	{
	};

	template <typename T>
	using make_integral_or_big_int_unsigned_t = typename make_integral_or_big_int_unsigned<T>::type;

	template <typename T, class = void>
	struct make_integral_or_big_int_signed;

	template <typename T>
	struct make_integral_or_big_int_signed<T, std::enable_if_t<ccm::support::traits::ccm_is_integral_v<T>>> : support::traits::ccm_make_signed<T>
	{
	};

	template <typename T>
	struct make_integral_or_big_int_signed<T, std::enable_if_t<is_big_int_v<T>>> : make_big_int_signed<T>
	{
	};

	template <typename T>
	using make_integral_or_big_int_signed_t = typename make_integral_or_big_int_signed<T>::type;
} // namespace ccm::types

/// Specialization of the bits.hpp header for BigInt types.
namespace ccm::support
{
	template <typename To, typename From>
	constexpr std::enable_if_t<
		(sizeof(To) == sizeof(From)) && std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From> && ccm::types::is_big_int<To>::value, To>
	bit_cast(const From & from)
	{
		To out;
		using Storage = decltype(out.val);
		out.val		  = ccm::support::bit_cast<Storage>(from);
		return out;
	}

	template <typename To, std::size_t Bits>
	constexpr std::enable_if_t<sizeof(To) == sizeof(types::UInt<Bits>) && std::is_trivially_constructible_v<To> && std::is_trivially_copyable_v<To> &&
								   std::is_trivially_copyable_v<types::UInt<Bits>>,
							   To>
	bit_cast(const types::UInt<Bits> & from)
	{
		return support::bit_cast<To>(from.val);
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> popcount(T value)
	{
		int bits = 0;
		for (auto word : value.val)
		{
			if (word) { bits += popcount(word); }
		}
		return bits;
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, bool> has_single_bit(T value)
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

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> countr_zero(const T & value)
	{
		return types::multiword::countr_zero(value.val);
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> countl_zero(const T & value)
	{
		return types::multiword::countl_zero(value.val);
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> countl_one(T value)
	{
		return types::multiword::countl_one(value.val);
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> countr_one(T value)
	{
		return types::multiword::countr_one(value.val);
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> bit_width(T value)
	{
		return std::numeric_limits<T>::digits - support::countl_zero(value);
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, T> rotr(T value, int rotate);

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, T> rotl(T value, int rotate)
	{
		constexpr unsigned N = std::numeric_limits<T>::digits;
		rotate				 = rotate % N;
		if (!rotate) { return value; }
		if (rotate < 0) { return support::rotr<T>(value, -rotate); }
		return (value << rotate) | (value >> (N - rotate));
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, T> rotr(T value, int rotate)
	{
		constexpr unsigned N = std::numeric_limits<T>::digits;
		rotate				 = rotate % N;
		if (!rotate) { return value; }
		if (rotate < 0) { return support::rotl<T>(value, -rotate); }
		return (value >> rotate) | (value << (N - rotate));
	}

	template <typename T, std::size_t count>
	constexpr std::enable_if_t<types::is_big_int_v<T>, T> mask_trailing_ones()
	{
		static_assert(!T::SIGNED && count <= T::BITS);
		if (count == T::BITS) { return T::all_ones(); }
		constexpr std::size_t QUOTIENT	= count / T::WORD_SIZE;
		constexpr std::size_t REMAINDER = count % T::WORD_SIZE;
		T out; // zero initialized
		for (std::size_t i = 0; i <= QUOTIENT; ++i) { out[i] = i < QUOTIENT ? -1 : mask_trailing_ones<typename T::word_type, REMAINDER>(); }
		return out;
	}

	template <typename T, std::size_t count>
	constexpr std::enable_if_t<types::is_big_int_v<T>, T> mask_leading_ones()
	{
		static_assert(!T::SIGNED && count <= T::BITS);
		if (count == T::BITS) { return T::all_ones(); }
		constexpr std::size_t QUOTIENT	= (T::BITS - count - 1U) / T::WORD_SIZE;
		constexpr std::size_t REMAINDER = count % T::WORD_SIZE;
		T out; // zero initialized
		for (std::size_t i = QUOTIENT; i < T::WORD_COUNT; ++i) { out[i] = i > QUOTIENT ? -1 : mask_leading_ones<typename T::word_type, REMAINDER>(); }
		return out;
	}

	template <typename T, size_t count>
	constexpr std::enable_if_t<types::is_big_int_v<T>, T> mask_trailing_zeros()
	{
		return mask_leading_ones<T, T::BITS - count>();
	}

	template <typename T, std::size_t count>
	constexpr std::enable_if_t<types::is_big_int_v<T>, T> mask_leading_zeros()
	{
		return mask_trailing_ones<T, T::BITS - count>();
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> count_zeros(T value)
	{
		return support::popcount(~value);
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> first_leading_zero(T value)
	{
		return value == std::numeric_limits<T>::max() ? 0 : support::countl_one(value) + 1;
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> first_leading_one(T value)
	{
		return first_leading_zero(~value);
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> first_trailing_zero(T value)
	{
		return value == std::numeric_limits<T>::max() ? 0 : support::countr_zero(~value) + 1;
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<types::is_big_int_v<T>, int> first_trailing_one(T value)
	{
		return value == std::numeric_limits<T>::max() ? 0 : support::countr_zero(value) + 1;
	}
} // namespace ccm::support