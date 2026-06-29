/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "ccmath/internal/support/math_support.hpp"
#include "ccmath/internal/types/big_int.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{
	template <std::size_t Bits> struct RefUint
	{
		static_assert(Bits > 0 && Bits % 32 == 0, "RefUint requires a 32-bit limb width");

		static constexpr std::size_t limb_bits	= 32;
		static constexpr std::size_t limb_count = Bits / limb_bits;

		std::array<std::uint32_t, limb_count> limbs{};

		[[nodiscard]] static RefUint zero()
		{
			return {};
		}

		[[nodiscard]] static RefUint one()
		{
			RefUint out;
			out.limbs[0] = 1U;
			return out;
		}

		[[nodiscard]] static RefUint all_ones()
		{
			RefUint out;
			out.limbs.fill(std::numeric_limits<std::uint32_t>::max());
			return out;
		}

		[[nodiscard]] static RefUint from_u64(std::uint64_t value)
		{
			RefUint out;
			if constexpr (limb_count > 0)
			{
				out.limbs[0] = static_cast<std::uint32_t>(value);
			}
			if constexpr (limb_count > 1)
			{
				out.limbs[1] = static_cast<std::uint32_t>(value >> 32U);
			}
			return out;
		}

		template <std::size_t WordCount> [[nodiscard]] static RefUint from_words64(const std::array<std::uint64_t, WordCount> & words)
		{
			RefUint out;
			for (std::size_t i = 0; i < WordCount; ++i)
			{
				const std::size_t lo_index = i * 2;
				if (lo_index < limb_count)
				{
					out.limbs[lo_index] = static_cast<std::uint32_t>(words[i]);
				}
				if (lo_index + 1 < limb_count)
				{
					out.limbs[lo_index + 1] = static_cast<std::uint32_t>(words[i] >> 32U);
				}
			}
			return out;
		}

		[[nodiscard]] bool is_zero() const
		{
			for (std::uint32_t limb : limbs)
			{
				if (limb != 0U)
				{
					return false;
				}
			}
			return true;
		}

		[[nodiscard]] bool get_bit(std::size_t bit) const
		{
			if (bit >= Bits)
			{
				return false;
			}
			const std::size_t limb_index = bit / limb_bits;
			const std::size_t bit_index	 = bit % limb_bits;
			return ((limbs[limb_index] >> bit_index) & 1U) != 0U;
		}

		void set_bit(std::size_t bit)
		{
			const std::size_t limb_index = bit / limb_bits;
			const std::size_t bit_index	 = bit % limb_bits;
			limbs[limb_index] |= (1U << bit_index);
		}

		void clear_bit(std::size_t bit)
		{
			const std::size_t limb_index = bit / limb_bits;
			const std::size_t bit_index	 = bit % limb_bits;
			limbs[limb_index] &= ~(1U << bit_index);
		}

		[[nodiscard]] int compare(const RefUint & other) const
		{
			for (std::size_t i = limb_count; i-- > 0;)
			{
				if (limbs[i] < other.limbs[i])
				{
					return -1;
				}
				if (limbs[i] > other.limbs[i])
				{
					return 1;
				}
			}
			return 0;
		}

		[[nodiscard]] bool operator==(const RefUint & other) const
		{
			return limbs == other.limbs;
		}
		[[nodiscard]] bool operator!=(const RefUint & other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] RefUint bitwise_not() const
		{
			RefUint out;
			for (std::size_t i = 0; i < limb_count; ++i)
			{
				out.limbs[i] = ~limbs[i];
			}
			return out;
		}

		[[nodiscard]] std::pair<RefUint, bool> add(const RefUint & other) const
		{
			RefUint out;
			std::uint64_t carry = 0;
			for (std::size_t i = 0; i < limb_count; ++i)
			{
				const std::uint64_t sum = static_cast<std::uint64_t>(limbs[i]) + static_cast<std::uint64_t>(other.limbs[i]) + carry;
				out.limbs[i]			= static_cast<std::uint32_t>(sum);
				carry					= sum >> limb_bits;
			}
			return { out, carry != 0 };
		}

		[[nodiscard]] std::pair<RefUint, bool> sub(const RefUint & other) const
		{
			RefUint out;
			std::uint64_t borrow = 0;
			for (std::size_t i = 0; i < limb_count; ++i)
			{
				const std::uint64_t lhs = limbs[i];
				const std::uint64_t rhs = static_cast<std::uint64_t>(other.limbs[i]) + borrow;
				out.limbs[i]			= static_cast<std::uint32_t>(lhs - rhs);
				borrow					= lhs < rhs ? 1U : 0U;
			}
			return { out, borrow != 0 };
		}

		template <std::size_t OtherBits> [[nodiscard]] RefUint<Bits + OtherBits> full_mul(const RefUint<OtherBits> & other) const
		{
			RefUint<Bits + OtherBits> out;
			for (std::size_t i = 0; i < limb_count; ++i)
			{
				std::uint64_t carry = 0;
				for (std::size_t j = 0; j < RefUint<OtherBits>::limb_count; ++j)
				{
					const std::size_t out_index = i + j;
					const std::uint64_t accum	= static_cast<std::uint64_t>(out.limbs[out_index]) +
												  static_cast<std::uint64_t>(limbs[i]) * static_cast<std::uint64_t>(other.limbs[j]) + carry;
					out.limbs[out_index]		= static_cast<std::uint32_t>(accum);
					carry						= accum >> limb_bits;
				}
				for (std::size_t out_index = i + RefUint<OtherBits>::limb_count; carry != 0 && out_index < RefUint<Bits + OtherBits>::limb_count; ++out_index)
				{
					const std::uint64_t accum = static_cast<std::uint64_t>(out.limbs[out_index]) + carry;
					out.limbs[out_index]	  = static_cast<std::uint32_t>(accum);
					carry					  = accum >> limb_bits;
				}
			}
			return out;
		}

		[[nodiscard]] RefUint shift_left(std::size_t shift) const
		{
			if (shift >= Bits)
			{
				return {};
			}

			RefUint out;
			const std::size_t limb_shift = shift / limb_bits;
			const std::size_t bit_shift	 = shift % limb_bits;

			for (std::size_t i = 0; i < limb_count; ++i)
			{
				if (limbs[i] == 0U)
				{
					continue;
				}
				const std::size_t out_index = i + limb_shift;
				if (out_index >= limb_count)
				{
					continue;
				}

				const std::uint64_t wide = static_cast<std::uint64_t>(limbs[i]) << bit_shift;
				out.limbs[out_index] |= static_cast<std::uint32_t>(wide);
				if (bit_shift != 0 && out_index + 1 < limb_count)
				{
					out.limbs[out_index + 1] |= static_cast<std::uint32_t>(wide >> limb_bits);
				}
			}
			return out;
		}

		[[nodiscard]] RefUint shift_right(std::size_t shift) const
		{
			if (shift >= Bits)
			{
				return {};
			}

			RefUint out;
			const std::size_t limb_shift = shift / limb_bits;
			const std::size_t bit_shift	 = shift % limb_bits;

			for (std::size_t i = limb_shift; i < limb_count; ++i)
			{
				const std::size_t out_index = i - limb_shift;
				out.limbs[out_index] |= limbs[i] >> bit_shift;
				if (bit_shift != 0 && i + 1 < limb_count)
				{
					out.limbs[out_index] |= static_cast<std::uint32_t>(static_cast<std::uint64_t>(limbs[i + 1]) << (limb_bits - bit_shift));
				}
			}
			return out;
		}

		void shift_left_one_in_place()
		{
			std::uint64_t carry = 0;
			for (std::size_t i = 0; i < limb_count; ++i)
			{
				const std::uint64_t shifted = (static_cast<std::uint64_t>(limbs[i]) << 1U) | carry;
				limbs[i]					= static_cast<std::uint32_t>(shifted);
				carry						= shifted >> limb_bits;
			}
		}

		[[nodiscard]] std::pair<RefUint, RefUint> divmod(const RefUint & divider) const
		{
			RefUint quotient;
			RefUint remainder;
			if (divider.is_zero())
			{
				return { quotient, remainder };
			}

			for (std::size_t bit = Bits; bit-- > 0;)
			{
				remainder.shift_left_one_in_place();
				if (get_bit(bit))
				{
					remainder.limbs[0] |= 1U;
				}
				if (remainder.compare(divider) >= 0)
				{
					remainder = remainder.sub(divider).first;
					quotient.set_bit(bit);
				}
			}
			return { quotient, remainder };
		}

		template <std::size_t NewBits> [[nodiscard]] RefUint<NewBits> truncate_low() const
		{
			RefUint<NewBits> out;
			const std::size_t max_limbs = std::min<std::size_t>(limb_count, RefUint<NewBits>::limb_count);
			for (std::size_t i = 0; i < max_limbs; ++i)
			{
				out.limbs[i] = limbs[i];
			}
			return out;
		}

		template <std::size_t NewBits> [[nodiscard]] RefUint<NewBits> slice(std::size_t bit_offset) const
		{
			RefUint<NewBits> out;
			for (std::size_t bit = 0; bit < NewBits; ++bit)
			{
				if (get_bit(bit_offset + bit))
				{
					out.set_bit(bit);
				}
			}
			return out;
		}

		[[nodiscard]] bool fits_u64() const
		{
			for (std::size_t i = 2; i < limb_count; ++i)
			{
				if (limbs[i] != 0U)
				{
					return false;
				}
			}
			return true;
		}

		[[nodiscard]] std::uint64_t to_u64() const
		{
			std::uint64_t out = 0;
			if constexpr (limb_count > 0)
			{
				out |= limbs[0];
			}
			if constexpr (limb_count > 1)
			{
				out |= static_cast<std::uint64_t>(limbs[1]) << 32U;
			}
			return out;
		}

		[[nodiscard]] std::string hex_string() const
		{
			std::ostringstream out;
			out << "0x";
			bool started = false;
			for (std::size_t i = limb_count; i-- > 0;)
			{
				if (!started)
				{
					if (limbs[i] == 0U && i != 0)
					{
						continue;
					}
					out << std::hex << std::uppercase << limbs[i];
					started = true;
				} else
				{
					out << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << limbs[i];
				}
			}
			if (!started)
			{
				out << '0';
			}
			return out.str();
		}
	};

	template <std::size_t Bits> struct RefInt
	{
		RefUint<Bits> raw{};

		[[nodiscard]] static RefInt zero()
		{
			return {};
		}

		[[nodiscard]] static RefInt one()
		{
			RefInt out;
			out.raw = RefUint<Bits>::one();
			return out;
		}

		[[nodiscard]] static RefInt minus_one()
		{
			RefInt out;
			out.raw = RefUint<Bits>::all_ones();
			return out;
		}

		[[nodiscard]] static RefInt min()
		{
			RefInt out;
			out.raw.set_bit(Bits - 1);
			return out;
		}

		[[nodiscard]] static RefInt max()
		{
			RefInt out;
			out.raw = RefUint<Bits>::all_ones();
			out.raw.clear_bit(Bits - 1);
			return out;
		}

		[[nodiscard]] static RefInt from_i64(std::int64_t value)
		{
			RefInt out;
			const std::uint64_t raw_bits = static_cast<std::uint64_t>(value);
			for (std::size_t bit = 0; bit < std::min<std::size_t>(Bits, 64); ++bit)
			{
				if (((raw_bits >> bit) & 1ULL) != 0ULL)
				{
					out.raw.set_bit(bit);
				}
			}
			if (value < 0)
			{
				for (std::size_t bit = 64; bit < Bits; ++bit)
				{
					out.raw.set_bit(bit);
				}
			}
			return out;
		}

		[[nodiscard]] bool is_negative() const
		{
			return raw.get_bit(Bits - 1);
		}

		[[nodiscard]] int compare(const RefInt & other) const
		{
			if (is_negative() != other.is_negative())
			{
				return is_negative() ? -1 : 1;
			}
			return raw.compare(other.raw);
		}

		[[nodiscard]] bool operator==(const RefInt & other) const
		{
			return raw == other.raw;
		}

		[[nodiscard]] RefInt negated() const
		{
			RefInt out;
			out.raw = raw.bitwise_not().add(RefUint<Bits>::one()).first;
			return out;
		}

		[[nodiscard]] RefInt add(const RefInt & other) const
		{
			return RefInt{ raw.add(other.raw).first };
		}
		[[nodiscard]] RefInt sub(const RefInt & other) const
		{
			return RefInt{ raw.sub(other.raw).first };
		}
		[[nodiscard]] RefInt mul(const RefInt & other) const
		{
			return RefInt{ raw.full_mul(other.raw).template truncate_low<Bits>() };
		}

		[[nodiscard]] RefUint<Bits> abs_unsigned() const
		{
			return is_negative() ? negated().raw : raw;
		}

		[[nodiscard]] RefInt arithmetic_shift_right(std::size_t shift) const
		{
			if (shift >= Bits)
			{
				return is_negative() ? minus_one() : zero();
			}

			RefInt out;
			out.raw = raw.shift_right(shift);
			if (is_negative())
			{
				for (std::size_t bit = Bits - shift; bit < Bits; ++bit)
				{
					out.raw.set_bit(bit);
				}
			}
			return out;
		}

		[[nodiscard]] std::pair<RefInt, RefInt> divmod(const RefInt & divider) const
		{
			RefInt quotient;
			RefInt remainder;
			if (divider.raw.is_zero())
			{
				return { quotient, remainder };
			}

			const RefUint<Bits> lhs_mag = abs_unsigned();
			const RefUint<Bits> rhs_mag = divider.abs_unsigned();
			const auto [uq, ur]			= lhs_mag.divmod(rhs_mag);

			quotient.raw  = uq;
			remainder.raw = ur;

			if (is_negative() != divider.is_negative())
			{
				quotient = quotient.negated();
			}
			if (is_negative())
			{
				remainder = remainder.negated();
			}
			return { quotient, remainder };
		}

		[[nodiscard]] std::string hex_string() const
		{
			return raw.hex_string();
		}
	};

	template <typename Big, std::size_t Bits> [[nodiscard]] Big to_bigint(const RefUint<Bits> & ref)
	{
		Big out{};
		for (std::size_t bit = 0; bit < Bits; ++bit)
		{
			if (ref.get_bit(bit))
			{
				out.set_bit(bit);
			}
		}
		return out;
	}

	template <typename Big, std::size_t Bits> [[nodiscard]] Big to_bigint(const RefInt<Bits> & ref)
	{
		return to_bigint<Big>(ref.raw);
	}

	template <std::size_t Bits, bool Signed, typename WordType>
	[[nodiscard]] RefUint<Bits> to_ref_unsigned(const ccm::types::BigInt<Bits, Signed, WordType> & value)
	{
		RefUint<Bits> out;
		for (std::size_t bit = 0; bit < Bits; ++bit)
		{
			if (value.get_bit(bit))
			{
				out.set_bit(bit);
			}
		}
		return out;
	}

	template <std::size_t Bits, typename WordType> [[nodiscard]] RefInt<Bits> to_ref_signed(const ccm::types::BigInt<Bits, true, WordType> & value)
	{
		return RefInt<Bits>{ to_ref_unsigned(value) };
	}

	template <std::size_t Bits> [[nodiscard]] RefUint<Bits> repeat_limb(std::uint32_t value)
	{
		RefUint<Bits> out;
		out.limbs.fill(value);
		return out;
	}

	template <std::size_t Bits> [[nodiscard]] RefUint<Bits> single_bit(std::size_t bit)
	{
		RefUint<Bits> out;
		out.set_bit(bit);
		return out;
	}

	template <std::size_t Bits> [[nodiscard]] RefUint<Bits> generated_case(std::uint64_t seed)
	{
		RefUint<Bits> out;
		std::uint64_t state = seed;
		for (std::size_t i = 0; i < RefUint<Bits>::limb_count; ++i)
		{
			state ^= state >> 12U;
			state ^= state << 25U;
			state ^= state >> 27U;
			const std::uint64_t mixed = state * 0x2545F4914F6CDD1DULL;
			out.limbs[i]			  = static_cast<std::uint32_t>(mixed ^ (mixed >> 32U) ^ (seed >> (i % 17U)));
		}
		return out;
	}

	template <std::size_t Bits> [[nodiscard]] std::vector<RefUint<Bits>> build_unsigned_cases()
	{
		std::vector<RefUint<Bits>> out;
		out.push_back(RefUint<Bits>::zero());
		out.push_back(RefUint<Bits>::one());
		out.push_back(RefUint<Bits>::all_ones());
		out.push_back(single_bit<Bits>(0));
		out.push_back(single_bit<Bits>(Bits - 1));
		out.push_back(single_bit<Bits>(Bits / 2));
		out.push_back(repeat_limb<Bits>(0xAAAAAAAAU));
		out.push_back(repeat_limb<Bits>(0x55555555U));
		out.push_back(repeat_limb<Bits>(0xFFFFFFFFU));
		out.push_back(repeat_limb<Bits>(0x80000000U));
		out.push_back(repeat_limb<Bits>(0x7FFFFFFFU));

		RefUint<Bits> carry_chain = RefUint<Bits>::all_ones();
		carry_chain.clear_bit(0);
		out.push_back(carry_chain);

		RefUint<Bits> sparse = RefUint<Bits>::zero();
		for (std::size_t bit : { static_cast<std::size_t>(0),
								 static_cast<std::size_t>(1),
								 static_cast<std::size_t>(31),
								 static_cast<std::size_t>(32),
								 Bits / 2 - 1,
								 Bits / 2,
								 Bits - 2,
								 Bits - 1 })
		{
			sparse.set_bit(bit);
		}
		out.push_back(sparse);

		RefUint<Bits> descending = RefUint<Bits>::zero();
		for (std::size_t i = 0; i < RefUint<Bits>::limb_count; ++i)
		{
			descending.limbs[i] = static_cast<std::uint32_t>(0xF0E1D2C3U - static_cast<std::uint32_t>(0x11111111U * i));
		}
		out.push_back(descending);

		for (std::uint64_t seed :
			 { 0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL, 0x9E3779B97F4A7C15ULL, 0xD1B54A32D192ED03ULL, 0x94D049BB133111EBULL, 0x2545F4914F6CDD1DULL })
		{
			out.push_back(generated_case<Bits>(seed));
		}

		return out;
	}

	template <std::size_t Bits> [[nodiscard]] std::vector<RefInt<Bits>> build_signed_cases()
	{
		std::vector<RefInt<Bits>> out = {
			RefInt<Bits>::zero(),
			RefInt<Bits>::one(),
			RefInt<Bits>::minus_one(),
			RefInt<Bits>::min(),
			RefInt<Bits>::max(),
			RefInt<Bits>::from_i64(0x123456789ABLL),
			RefInt<Bits>::from_i64(-0x123456789ABLL),
			RefInt<Bits>::from_i64(std::numeric_limits<std::int64_t>::max()),
			RefInt<Bits>::from_i64(std::numeric_limits<std::int64_t>::min()),
		};

		const auto unsigned_cases = build_unsigned_cases<Bits>();
		for (std::size_t i = 0; i < unsigned_cases.size() && i < 8; ++i)
		{
			RefInt<Bits> value{ unsigned_cases[i] };
			value.raw.set_bit(Bits - 1);
			out.push_back(value);
		}
		return out;
	}

	template <std::size_t Bits> [[nodiscard]] int countl_zero_ref(const RefUint<Bits> & value)
	{
		int count = 0;
		for (std::size_t bit = Bits; bit-- > 0;)
		{
			if (value.get_bit(bit))
			{
				break;
			}
			++count;
		}
		return count;
	}

	template <std::size_t Bits> [[nodiscard]] int countr_zero_ref(const RefUint<Bits> & value)
	{
		int count = 0;
		for (std::size_t bit = 0; bit < Bits; ++bit)
		{
			if (value.get_bit(bit))
			{
				break;
			}
			++count;
		}
		return count;
	}

	template <std::size_t Bits> [[nodiscard]] int countl_one_ref(const RefUint<Bits> & value)
	{
		int count = 0;
		for (std::size_t bit = Bits; bit-- > 0;)
		{
			if (!value.get_bit(bit))
			{
				break;
			}
			++count;
		}
		return count;
	}

	template <std::size_t Bits> [[nodiscard]] int countr_one_ref(const RefUint<Bits> & value)
	{
		int count = 0;
		for (std::size_t bit = 0; bit < Bits; ++bit)
		{
			if (!value.get_bit(bit))
			{
				break;
			}
			++count;
		}
		return count;
	}

	template <std::size_t Bits> [[nodiscard]] int bit_width_ref(const RefUint<Bits> & value)
	{
		return value.is_zero() ? 0 : static_cast<int>(Bits) - countl_zero_ref(value);
	}

	template <typename Big, typename Ref> void expect_bigint_equals_ref(const Big & actual, const Ref & expected)
	{
		const auto actual_ref = to_ref_unsigned(actual);
		EXPECT_TRUE(actual_ref == expected) << "actual=" << actual_ref.hex_string() << " expected=" << expected.hex_string();
	}

	template <typename Big, std::size_t Bits> void expect_signed_bigint_equals_ref(const Big & actual, const RefInt<Bits> & expected)
	{
		const auto actual_ref = to_ref_signed(actual);
		EXPECT_TRUE(actual_ref == expected) << "actual=" << actual_ref.hex_string() << " expected=" << expected.hex_string();
	}

	using UInt64 = ccm::types::UInt<64>;
	using UInt32 = ccm::types::UInt<32>;
} // namespace

TEST(CcmathInternalTypesTests, BigIntGetBitAndDecrement)
{
	UInt64 value(0xAULL);
	EXPECT_FALSE(value.get_bit(0));
	EXPECT_TRUE(value.get_bit(1));
	EXPECT_TRUE(value.get_bit(3));

	value = UInt64(1);
	--value;
	EXPECT_TRUE(value.is_zero());

	value = UInt64(0);
	--value;
	EXPECT_TRUE(value.get_bit(0));
	EXPECT_TRUE(value.get_bit(63));
}

TEST(CcmathInternalTypesTests, BigIntCrossWordConversion)
{
	UInt32 small(0x12345678U);
	const ccm::types::UInt<128> wide(small);

	EXPECT_EQ(wide[0], 0x12345678U);
	EXPECT_EQ(wide[1], 0U);

	const UInt32 narrowed(wide);
	EXPECT_EQ(static_cast<std::uint32_t>(narrowed), 0x12345678U);
}

TEST(CcmathInternalTypesTests, AddWithCarryDetectsUint64Wrap)
{
	std::uint64_t carry = 0;

	EXPECT_EQ(ccm::support::add_with_carry<std::uint64_t>(std::numeric_limits<std::uint64_t>::max(), 1ULL, 0ULL, carry), 0ULL);
	EXPECT_EQ(carry, 1ULL);

	EXPECT_EQ(ccm::support::add_with_carry<std::uint64_t>(std::numeric_limits<std::uint64_t>::max(), 0ULL, 1ULL, carry), 0ULL);
	EXPECT_EQ(carry, 1ULL);
}

TEST(CcmathInternalTypesTests, BigIntFullMultiplyPreservesLowWordCarries)
{
	using UInt128x64 = ccm::types::BigInt<128, false, std::uint64_t>;

	const UInt128x64 value(std::numeric_limits<std::uint64_t>::max());
	const auto product = value.ful_mul(value);

	EXPECT_EQ(product[0], 1ULL);
	EXPECT_EQ(product[1], std::numeric_limits<std::uint64_t>::max() - 1ULL);
	EXPECT_EQ(product[2], 0ULL);
	EXPECT_EQ(product[3], 0ULL);
}

TEST(CcmathInternalTypesTests, BigIntMultiwordMul2MatchesReference)
{
	const std::array<std::pair<std::uint64_t, std::uint64_t>, 7> cases = { {
		{ 0ULL, 0ULL },
		{ 1ULL, std::numeric_limits<std::uint64_t>::max() },
		{ std::numeric_limits<std::uint64_t>::max(), std::numeric_limits<std::uint64_t>::max() },
		{ 0x001FFFFFFFFFFFFEULL, 0x001FFFFFFFFFFFFEULL },
		{ 0x001FFFFFFFFFF000ULL, 0x001FFFFFFFFFF000ULL },
		{ 0x8000000000000005ULL, 0x8000000000000007ULL },
		{ 0xFEDCBA9876543210ULL, 0x0123456789ABCDEFULL },
	} };

	for (const auto & [lhs, rhs] : cases)
	{
		SCOPED_TRACE(lhs);
		SCOPED_TRACE(rhs);

		const auto actual	= ccm::types::multiword::mul2<std::uint64_t>(lhs, rhs);
		const auto expected = RefUint<64>::from_u64(lhs).full_mul(RefUint<64>::from_u64(rhs));

		EXPECT_EQ(actual[0], expected.template truncate_low<64>().to_u64());
		EXPECT_EQ(actual[1], expected.template slice<64>(64).to_u64());
	}
}

TEST(CcmathInternalTypesTests, BigIntUnsignedShiftsAndCountHelpersMatchReference)
{
	using Big = ccm::types::BigInt<256, false, std::uint64_t>;

	const auto cases						 = build_unsigned_cases<256>();
	const std::array<std::size_t, 15> shifts = { 0, 1, 7, 31, 32, 33, 63, 64, 65, 95, 127, 128, 129, 191, 255 };

	for (const auto & value : cases)
	{
		const Big big = to_bigint<Big>(value);
		SCOPED_TRACE(value.hex_string());

		EXPECT_EQ(ccm::support::countl_zero(big), countl_zero_ref(value));
		EXPECT_EQ(ccm::support::countr_zero(big), countr_zero_ref(value));
		EXPECT_EQ(ccm::support::countl_one(big), countl_one_ref(value));
		EXPECT_EQ(ccm::support::countr_one(big), countr_one_ref(value));
		EXPECT_EQ(ccm::support::bit_width(big), bit_width_ref(value));

		for (std::size_t shift : shifts)
		{
			SCOPED_TRACE(shift);
			expect_bigint_equals_ref(big << shift, value.shift_left(shift));
			expect_bigint_equals_ref(big >> shift, value.shift_right(shift));
		}
	}
}

TEST(CcmathInternalTypesTests, BigIntUnsignedArithmeticMatchesReference)
{
	using Big = ccm::types::BigInt<256, false, std::uint64_t>;

	const auto cases = build_unsigned_cases<256>();
	for (const auto & lhs_ref : cases)
	{
		for (const auto & rhs_ref : cases)
		{
			const Big lhs = to_bigint<Big>(lhs_ref);
			const Big rhs = to_bigint<Big>(rhs_ref);

			SCOPED_TRACE(lhs_ref.hex_string());
			SCOPED_TRACE(rhs_ref.hex_string());

			expect_bigint_equals_ref(lhs + rhs, lhs_ref.add(rhs_ref).first);
			expect_bigint_equals_ref(lhs - rhs, lhs_ref.sub(rhs_ref).first);

			const auto full_product = lhs_ref.full_mul(rhs_ref);
			expect_bigint_equals_ref(lhs * rhs, full_product.template truncate_low<256>());
			expect_bigint_equals_ref(lhs.ful_mul(rhs), full_product);

			if (!rhs_ref.is_zero())
			{
				const auto [expected_q, expected_r] = lhs_ref.divmod(rhs_ref);
				expect_bigint_equals_ref(lhs / rhs, expected_q);
				expect_bigint_equals_ref(lhs % rhs, expected_r);
			}
		}
	}
}

TEST(CcmathInternalTypesTests, BigIntQuickMulHiRespectsReferenceErrorBound)
{
	using Big = ccm::types::BigInt<256, false, std::uint64_t>;

	const auto cases = build_unsigned_cases<256>();
	for (const auto & lhs_ref : cases)
	{
		for (const auto & rhs_ref : cases)
		{
			const Big lhs = to_bigint<Big>(lhs_ref);
			const Big rhs = to_bigint<Big>(rhs_ref);

			const auto full_product = lhs_ref.full_mul(rhs_ref);
			const auto full_hi		= full_product.template slice<256>(256);
			const auto quick_hi		= to_ref_unsigned(lhs.quick_mul_hi(rhs));

			SCOPED_TRACE(lhs_ref.hex_string());
			SCOPED_TRACE(rhs_ref.hex_string());

			EXPECT_GE(full_hi.compare(quick_hi), 0) << "full_hi=" << full_hi.hex_string() << " quick_hi=" << quick_hi.hex_string();

			const auto diff = full_hi.sub(quick_hi).first;
			ASSERT_TRUE(diff.fits_u64()) << diff.hex_string();
			EXPECT_LE(diff.to_u64(), static_cast<std::uint64_t>(Big::WORD_COUNT - 1)) << diff.hex_string();
		}
	}
}

TEST(CcmathInternalTypesTests, BigIntPowNMatchesReference)
{
	using Big = ccm::types::BigInt<128, false, std::uint64_t>;

	const auto cases = build_unsigned_cases<128>();
	for (std::size_t i = 0; i < std::min<std::size_t>(cases.size(), 10); ++i)
	{
		for (std::uint64_t exponent : { 0ULL, 1ULL, 2ULL, 3ULL, 5ULL, 8ULL, 13ULL })
		{
			RefUint<128> expected = RefUint<128>::one();
			RefUint<128> power	  = cases[i];

			std::uint64_t exp = exponent;
			while (exp > 0)
			{
				if ((exp & 1ULL) != 0ULL)
				{
					expected = expected.full_mul(power).template truncate_low<128>();
				}
				exp >>= 1U;
				power = power.full_mul(power).template truncate_low<128>();
			}

			Big actual = to_bigint<Big>(cases[i]);
			actual.pow_n(exponent);

			SCOPED_TRACE(cases[i].hex_string());
			SCOPED_TRACE(exponent);
			expect_bigint_equals_ref(actual, expected);
		}
	}
}

TEST(CcmathInternalTypesTests, BigIntDivHalfWordTimesPowerOfTwoMatchesReference)
{
	using Big	   = ccm::types::BigInt<256, false, std::uint64_t>;
	using HalfWord = ccm::types::multiword::half_width_t<std::uint64_t>;

	const auto cases						 = build_unsigned_cases<256>();
	const std::array<HalfWord, 6> divisors	 = { 1U, 3U, 5U, 0x7FFFFFFFU, 0x80000001U, 0xFFFFFFFFU };
	const std::array<std::size_t, 12> shifts = { 0, 1, 7, 31, 32, 33, 63, 64, 95, 127, 191, 320 };

	for (const auto & numerator_ref : cases)
	{
		for (HalfWord divisor : divisors)
		{
			for (std::size_t shift : shifts)
			{
				Big actual					= to_bigint<Big>(numerator_ref);
				const auto actual_remainder = actual.div_uint_half_times_pow_2(divisor, shift);

				ASSERT_TRUE(actual_remainder.has_value());

				const RefUint<384> wide_numerator = numerator_ref.template slice<384>(0);
				const RefUint<384> wide_divisor	  = RefUint<384>::from_u64(divisor).shift_left(shift);
				const auto qr					  = wide_numerator.divmod(wide_divisor);
				const RefUint<256> expected_q	  = qr.first.template truncate_low<256>();
				const RefUint<256> expected_r	  = qr.second.template truncate_low<256>();

				SCOPED_TRACE(numerator_ref.hex_string());
				SCOPED_TRACE(divisor);
				SCOPED_TRACE(shift);
				expect_bigint_equals_ref(actual, expected_q);
				expect_bigint_equals_ref(*actual_remainder, expected_r);
			}
		}
	}
}

TEST(CcmathInternalTypesTests, BigIntSignedArithmeticAndDivisionMatchReference)
{
	using Big = ccm::types::BigInt<128, true, std::uint64_t>;

	const auto cases						 = build_signed_cases<128>();
	const std::array<std::size_t, 10> shifts = { 0, 1, 2, 31, 32, 33, 63, 64, 65, 127 };

	for (const auto & lhs_ref : cases)
	{
		const Big lhs = to_bigint<Big>(lhs_ref);
		SCOPED_TRACE(lhs_ref.hex_string());

		for (std::size_t shift : shifts)
		{
			SCOPED_TRACE(shift);
			expect_signed_bigint_equals_ref(lhs >> shift, lhs_ref.arithmetic_shift_right(shift));
			expect_signed_bigint_equals_ref(lhs << shift, RefInt<128>{ lhs_ref.raw.shift_left(shift) });
		}

		for (const auto & rhs_ref : cases)
		{
			const Big rhs = to_bigint<Big>(rhs_ref);

			SCOPED_TRACE(rhs_ref.hex_string());
			{
				SCOPED_TRACE("add");
				expect_signed_bigint_equals_ref(lhs + rhs, lhs_ref.add(rhs_ref));
			}
			{
				SCOPED_TRACE("sub");
				expect_signed_bigint_equals_ref(lhs - rhs, lhs_ref.sub(rhs_ref));
			}
			{
				SCOPED_TRACE("mul");
				expect_signed_bigint_equals_ref(lhs * rhs, lhs_ref.mul(rhs_ref));
			}

			if (rhs_ref.raw.is_zero())
			{
				continue;
			}

			const auto [expected_q, expected_r] = lhs_ref.divmod(rhs_ref);
			{
				SCOPED_TRACE("div");
				expect_signed_bigint_equals_ref(lhs / rhs, expected_q);
			}
			{
				SCOPED_TRACE("mod");
				expect_signed_bigint_equals_ref(lhs % rhs, expected_r);
			}
		}
	}
}

TEST(CcmathInternalTypesTests, BigIntSignedDivisionHandlesMinimumOperandEdges)
{
	using Big = ccm::types::BigInt<128, true, std::uint64_t>;

	const Big min_value = Big::min();
	const Big max_value = Big::max();
	const Big one		= Big::one();
	const Big neg_one	= Big(-1);
	const Big zero		= Big::zero();

	EXPECT_EQ(min_value / neg_one, min_value);
	EXPECT_EQ(min_value % neg_one, zero);
	EXPECT_EQ(min_value / one, min_value);
	EXPECT_EQ(min_value % one, zero);
	EXPECT_EQ(min_value / min_value, one);
	EXPECT_EQ(min_value % min_value, zero);
	EXPECT_EQ(min_value / max_value, neg_one);
	EXPECT_EQ(min_value % max_value, neg_one);
	EXPECT_EQ(max_value / min_value, zero);
	EXPECT_EQ(max_value % min_value, max_value);
}

TEST(CcmathInternalTypesTests, BigIntCrossWordSignedAndUnsignedConversionsPreserveBits)
{
	using SmallUnsigned = ccm::types::BigInt<96, false, std::uint32_t>;
	using WideUnsigned	= ccm::types::BigInt<192, false, std::uint64_t>;
	using SmallSigned	= ccm::types::BigInt<96, true, std::uint32_t>;
	using WideSigned	= ccm::types::BigInt<192, true, std::uint64_t>;

	const RefUint<96> unsigned_ref			  = generated_case<96>(0xD1B54A32D192ED03ULL);
	const RefUint<192> wide_unsigned_expected = unsigned_ref.template slice<192>(0);
	const SmallUnsigned small_u				  = to_bigint<SmallUnsigned>(unsigned_ref);
	const WideUnsigned wide_u(small_u);
	const SmallUnsigned narrow_u(wide_u);

	expect_bigint_equals_ref(wide_u, wide_unsigned_expected);
	expect_bigint_equals_ref(narrow_u, unsigned_ref);

	RefInt<96> signed_ref{ generated_case<96>(0x94D049BB133111EBULL) };
	signed_ref.raw.set_bit(95);
	RefInt<192> wide_signed_expected{ signed_ref.raw.template slice<192>(0) };
	if (signed_ref.is_negative())
	{
		for (std::size_t bit = 96; bit < 192; ++bit)
		{
			wide_signed_expected.raw.set_bit(bit);
		}
	}

	const SmallSigned small_s = to_bigint<SmallSigned>(signed_ref);
	const WideSigned wide_s(small_s);
	const SmallSigned narrow_s(wide_s);

	expect_signed_bigint_equals_ref(wide_s, wide_signed_expected);
	expect_signed_bigint_equals_ref(narrow_s, signed_ref);
}

TEST(CcmathInternalSupportTests, BigIntMaskHelpersMatchReference)
{
	using Big = ccm::types::BigInt<256, false, std::uint64_t>;

	const auto expect_trailing = [](const auto & actual, std::size_t count) {
		RefUint<256> expected;
		for (std::size_t bit = 0; bit < count; ++bit)
		{
			expected.set_bit(bit);
		}
		expect_bigint_equals_ref(actual, expected);
	};

	const auto expect_leading = [](const auto & actual, std::size_t count) {
		RefUint<256> expected;
		for (std::size_t bit = 256 - count; bit < 256; ++bit)
		{
			expected.set_bit(bit);
		}
		expect_bigint_equals_ref(actual, expected);
	};

	expect_trailing(ccm::support::mask_trailing_ones<Big, 0>(), 0);
	expect_trailing(ccm::support::mask_trailing_ones<Big, 1>(), 1);
	expect_trailing(ccm::support::mask_trailing_ones<Big, 63>(), 63);
	expect_trailing(ccm::support::mask_trailing_ones<Big, 64>(), 64);
	expect_trailing(ccm::support::mask_trailing_ones<Big, 65>(), 65);
	expect_trailing(ccm::support::mask_trailing_ones<Big, 127>(), 127);
	expect_trailing(ccm::support::mask_trailing_ones<Big, 128>(), 128);
	expect_trailing(ccm::support::mask_trailing_ones<Big, 255>(), 255);
	expect_trailing(ccm::support::mask_trailing_ones<Big, 256>(), 256);

	expect_leading(ccm::support::mask_leading_ones<Big, 0>(), 0);
	expect_leading(ccm::support::mask_leading_ones<Big, 1>(), 1);
	expect_leading(ccm::support::mask_leading_ones<Big, 63>(), 63);
	expect_leading(ccm::support::mask_leading_ones<Big, 64>(), 64);
	expect_leading(ccm::support::mask_leading_ones<Big, 65>(), 65);
	expect_leading(ccm::support::mask_leading_ones<Big, 127>(), 127);
	expect_leading(ccm::support::mask_leading_ones<Big, 128>(), 128);
	expect_leading(ccm::support::mask_leading_ones<Big, 255>(), 255);
	expect_leading(ccm::support::mask_leading_ones<Big, 256>(), 256);
}
