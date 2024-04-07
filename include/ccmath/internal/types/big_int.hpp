/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ccm
{

	template <size_t Bits, bool Signed, typename WordType = std::uint64_t>
	struct BigInt
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

		static constexpr bool is_signed		   = Signed;
		static constexpr std::size_t bits	   = Bits;
		static constexpr std::size_t word_size = sizeof(word_type) * CHAR_BIT;

		static_assert(Bits > 0 && Bits % word_size == 0, "Number of bits in BigInt should be a multiple of word_size.");

		static constexpr std::size_t word_count = Bits / word_size;

		std::array<word_type, word_count> data{};

		constexpr BigInt()					   = default;
		constexpr BigInt(const BigInt & other) = default;

		/**
		 * @brief Extend the internal representation of the big integer by filling remaining words with a specific value.
		 * @param index The starting index from where the extension should begin.
		 * @param is_negative Boolean flag indicating whether the big integer is negative.
		 * @return void
		 */
		constexpr void extend(std::size_t index, bool is_negative)
		{
			const word_type value = is_negative ? std::numeric_limits<word_type>::max() : std::numeric_limits<word_type>::min();
			for (size_t i = index; i < word_count; ++i) { data[i] = value; }
		}

		template <std::size_t OtherBits, bool OtherSigned>
		constexpr BigInt(const BigInt<OtherBits, OtherSigned, word_type> & other)
		{
			if constexpr (OtherBits > Bits) // truncate bits
			{
				for (std::size_t i = 0; i < word_count; ++i) { data[i] = other.data[i]; }
			}
			else // Will be zero or sign extended
			{
				size_t i = 0;
				for (; i < OtherBits / word_size; ++i) { data[i] = other.data[i]; }
				extend(i, Signed && other.is_neg());
			}
		}

		/**
		 * @brief Construct a BigInt from a C-style array of words.
		 * @tparam N The size of the array.
		 * @param nums The C-style array of words.
		 */
		template <size_t N>
		constexpr BigInt(const WordType (&nums)[N]) // NOLINT
		{
			static_assert(N == word_count);
			for (size_t i = 0; i < word_count; ++i) { data[i] = nums[i]; }
		}

		constexpr explicit BigInt(const std::array<WordType, word_count> & words) : data(words) {}


	};
} // namespace ccm
