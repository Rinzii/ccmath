#include "ccmath/internal/types/big_int.hpp"

#include <gmp.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	template <std::size_t Bits, bool Signed>
	using BigInt = ccm::types::BigInt<Bits, Signed, std::uint64_t>;

	using U256 = BigInt<256, false>;
	using S256 = BigInt<256, true>;

	struct mpz_holder
	{
		mpz_t value;

		mpz_holder() { mpz_init(value); }

		mpz_holder(mpz_holder&& other) noexcept
		{
			mpz_init(value);
			mpz_swap(value, other.value);
		}

		mpz_holder& operator=(mpz_holder&& other) noexcept
		{
			if (this != &other) { mpz_swap(value, other.value); }
			return *this;
		}

		mpz_holder(const mpz_holder&)			 = delete;
		mpz_holder& operator=(const mpz_holder&) = delete;

		~mpz_holder() { mpz_clear(value); }
	};

	template <typename Big>
	[[nodiscard]] Big make_big(const std::array<std::uint64_t, Big::WORD_COUNT>& words)
	{ return Big(words); }

	[[nodiscard]] std::array<std::uint64_t, 4> generated_words(std::uint64_t seed)
	{
		std::array<std::uint64_t, 4> out{};
		std::uint64_t state = seed;
		for (std::size_t i = 0; i < out.size(); ++i)
		{
			state ^= state >> 12U;
			state ^= state << 25U;
			state ^= state >> 27U;
			out[i] = state * 0x2545F4914F6CDD1DULL;
		}
		return out;
	}

	[[nodiscard]] std::vector<U256> build_unsigned_cases()
	{
		std::vector<U256> out;
		const std::array<std::array<std::uint64_t, 4>, 11> patterns = { {
			{ 0ULL, 0ULL, 0ULL, 0ULL },
			{ 1ULL, 0ULL, 0ULL, 0ULL },
			{ std::numeric_limits<std::uint64_t>::max(),
			  std::numeric_limits<std::uint64_t>::max(),
			  std::numeric_limits<std::uint64_t>::max(),
			  std::numeric_limits<std::uint64_t>::max() },
			{ std::numeric_limits<std::uint64_t>::max(), 0ULL, 0ULL, 0ULL },
			{ 0ULL, 0ULL, 0ULL, 0x8000000000000000ULL },
			{ 0xAAAAAAAAAAAAAAAAULL, 0xAAAAAAAAAAAAAAAAULL, 0xAAAAAAAAAAAAAAAAULL, 0xAAAAAAAAAAAAAAAAULL },
			{ 0x5555555555555555ULL, 0x5555555555555555ULL, 0x5555555555555555ULL, 0x5555555555555555ULL },
			{ 0x001FFFFFFFFFFFFEULL, 0ULL, 0ULL, 0ULL },
			{ 0x001FFFFFFFFFF000ULL, 0ULL, 0ULL, 0ULL },
			{ 0xFEDCBA9876543210ULL, 0x0123456789ABCDEFULL, 0x0F0E0D0C0B0A0908ULL, 0x8070605040302010ULL },
			{ 0xFFFFFFFF00000000ULL, 0x00000000FFFFFFFFULL, 0x7FFFFFFF80000000ULL, 0x800000007FFFFFFFULL },
		} };

		for (const auto& pattern : patterns) { out.push_back(make_big<U256>(pattern)); }
		for (std::uint64_t seed : { 0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL, 0x9E3779B97F4A7C15ULL, 0xD1B54A32D192ED03ULL, 0x94D049BB133111EBULL })
		{
			out.push_back(make_big<U256>(generated_words(seed)));
		}
		return out;
	}

	[[nodiscard]] std::vector<S256> build_signed_cases()
	{
		std::vector<S256> out;
		const std::array<std::array<std::uint64_t, 4>, 9> patterns = { {
			{ 0ULL, 0ULL, 0ULL, 0ULL },
			{ 1ULL, 0ULL, 0ULL, 0ULL },
			{ std::numeric_limits<std::uint64_t>::max(),
			  std::numeric_limits<std::uint64_t>::max(),
			  std::numeric_limits<std::uint64_t>::max(),
			  std::numeric_limits<std::uint64_t>::max() },
			{ 0ULL, 0ULL, 0ULL, 0x8000000000000000ULL },
			{ 0xFFFFFFFFFFFFFFFEULL,
			  std::numeric_limits<std::uint64_t>::max(),
			  std::numeric_limits<std::uint64_t>::max(),
			  std::numeric_limits<std::uint64_t>::max() },
			{ 0x0123456789ABCDEFULL, 0x0FEDCBA987654321ULL, 0ULL, 0ULL },
			{ 0x0123456789ABCDEFULL, 0x0FEDCBA987654321ULL, 0ULL, 0x8000000000000000ULL },
			{ 0xAAAAAAAAAAAAAAAAULL, 0x5555555555555555ULL, 0xAAAAAAAAAAAAAAAAULL, 0xD555555555555555ULL },
			{ 0x001FFFFFFFFFFFFEULL, 0x001FFFFFFFFFFFFEULL, 0ULL, 0x8000000000000000ULL },
		} };

		for (const auto& pattern : patterns) { out.push_back(make_big<S256>(pattern)); }
		for (std::uint64_t seed : { 0x2545F4914F6CDD1DULL, 0x94D049BB133111EBULL, 0xD1B54A32D192ED03ULL })
		{
			auto words = generated_words(seed);
			words.back() |= 0x8000000000000000ULL;
			out.push_back(make_big<S256>(words));
		}
		return out;
	}

	template <typename Big>
	[[nodiscard]] mpz_holder mpz_from_unsigned_big(const Big& value)
	{
		mpz_holder out;
		std::array<std::uint64_t, Big::WORD_COUNT> words{};
		for (std::size_t i = 0; i < Big::WORD_COUNT; ++i) { words[i] = value[i]; }
		mpz_import(out.value, words.size(), -1, sizeof(std::uint64_t), 0, 0, words.data());
		return out;
	}

	template <std::size_t Bits>
	[[nodiscard]] mpz_holder mpz_from_signed_big(const BigInt<Bits, true>& value)
	{
		mpz_holder out = mpz_from_unsigned_big(value);
		if (value.is_neg())
		{
			mpz_holder two_to_bits;
			mpz_set_ui(two_to_bits.value, 1U);
			mpz_mul_2exp(two_to_bits.value, two_to_bits.value, Bits);
			mpz_sub(out.value, out.value, two_to_bits.value);
		}
		return out;
	}

	template <typename Big>
	[[nodiscard]] bool matches_unsigned_bits(const Big& actual, const mpz_t expected)
	{
		mpz_holder reduced;
		mpz_fdiv_r_2exp(reduced.value, expected, Big::BITS);
		for (std::size_t bit = 0; bit < Big::BITS; ++bit)
		{
			if (actual.get_bit(bit) != (mpz_tstbit(reduced.value, bit) != 0)) { return false; }
		}
		return true;
	}

	template <typename Big>
	[[nodiscard]] std::string bits_hex(const Big& value)
	{
		std::ostringstream out;
		out << "0x";
		for (std::size_t i = Big::WORD_COUNT; i-- > 0;) { out << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value[i]; }
		return out.str();
	}

	template <typename Big>
	int fail(const char* label, const Big& lhs, const Big& rhs)
	{
		std::cerr << label << '\n' << "lhs=" << bits_hex(lhs) << '\n' << "rhs=" << bits_hex(rhs) << '\n';
		return 1;
	}
} // namespace

int main()
{
	const auto unsigned_cases = build_unsigned_cases();
	for (const auto& lhs : unsigned_cases)
	{
		for (const auto& rhs : unsigned_cases)
		{
			const mpz_holder lhs_mpz = mpz_from_unsigned_big(lhs);
			const mpz_holder rhs_mpz = mpz_from_unsigned_big(rhs);

			mpz_holder expected;

			mpz_add(expected.value, lhs_mpz.value, rhs_mpz.value);
			if (!matches_unsigned_bits(lhs + rhs, expected.value)) { return fail("unsigned add mismatch", lhs, rhs); }

			mpz_sub(expected.value, lhs_mpz.value, rhs_mpz.value);
			if (!matches_unsigned_bits(lhs - rhs, expected.value)) { return fail("unsigned sub mismatch", lhs, rhs); }

			mpz_mul(expected.value, lhs_mpz.value, rhs_mpz.value);
			if (!matches_unsigned_bits(lhs * rhs, expected.value)) { return fail("unsigned mul mismatch", lhs, rhs); }
			if (!matches_unsigned_bits(lhs.ful_mul(rhs), expected.value)) { return fail("unsigned full mul mismatch", lhs, rhs); }

			if (!rhs.is_zero())
			{
				mpz_holder q;
				mpz_holder r;
				mpz_tdiv_qr(q.value, r.value, lhs_mpz.value, rhs_mpz.value);
				if (!matches_unsigned_bits(lhs / rhs, q.value)) { return fail("unsigned div mismatch", lhs, rhs); }
				if (!matches_unsigned_bits(lhs % rhs, r.value)) { return fail("unsigned mod mismatch", lhs, rhs); }
			}
		}
	}

	for (const auto& numerator : unsigned_cases)
	{
		const mpz_holder numerator_mpz = mpz_from_unsigned_big(numerator);
		for (std::uint32_t divisor : { 1U, 3U, 5U, 0x7FFFFFFFU, 0x80000001U, 0xFFFFFFFFU })
		{
			for (std::size_t shift : { std::size_t(0),
									   std::size_t(1),
									   std::size_t(31),
									   std::size_t(32),
									   std::size_t(63),
									   std::size_t(64),
									   std::size_t(95),
									   std::size_t(127),
									   std::size_t(191),
									   std::size_t(255),
									   std::size_t(320) })
			{
				U256 quotient		 = numerator;
				const auto remainder = quotient.div_uint_half_times_pow_2(divisor, shift);
				if (!remainder.has_value())
				{
					std::cerr << "div_uint_half_times_pow_2 unexpectedly returned nullopt\n";
					return 1;
				}

				mpz_holder denominator;
				mpz_set_ui(denominator.value, divisor);
				mpz_mul_2exp(denominator.value, denominator.value, shift);

				mpz_holder q;
				mpz_holder r;
				mpz_tdiv_qr(q.value, r.value, numerator_mpz.value, denominator.value);

				if (!matches_unsigned_bits(quotient, q.value) || !matches_unsigned_bits(*remainder, r.value))
				{
					std::cerr << "div_uint_half_times_pow_2 mismatch\n";
					std::cerr << "numerator=" << bits_hex(numerator) << " divisor=" << divisor << " shift=" << shift << '\n';
					return 1;
				}
			}
		}
	}

	const auto signed_cases = build_signed_cases();
	for (const auto& lhs : signed_cases)
	{
		for (const auto& rhs : signed_cases)
		{
			const mpz_holder lhs_mpz = mpz_from_signed_big(lhs);
			const mpz_holder rhs_mpz = mpz_from_signed_big(rhs);
			mpz_holder expected;

			mpz_add(expected.value, lhs_mpz.value, rhs_mpz.value);
			if (!matches_unsigned_bits(lhs + rhs, expected.value)) { return fail("signed add raw mismatch", lhs, rhs); }

			mpz_sub(expected.value, lhs_mpz.value, rhs_mpz.value);
			if (!matches_unsigned_bits(lhs - rhs, expected.value)) { return fail("signed sub raw mismatch", lhs, rhs); }

			mpz_mul(expected.value, lhs_mpz.value, rhs_mpz.value);
			if (!matches_unsigned_bits(lhs * rhs, expected.value)) { return fail("signed mul raw mismatch", lhs, rhs); }

			if (rhs.is_zero()) { continue; }

			mpz_holder q;
			mpz_holder r;
			mpz_tdiv_qr(q.value, r.value, lhs_mpz.value, rhs_mpz.value);

			if (!matches_unsigned_bits(lhs / rhs, q.value)) { return fail("signed div mismatch", lhs, rhs); }
			if (!matches_unsigned_bits(lhs % rhs, r.value)) { return fail("signed mod mismatch", lhs, rhs); }
		}
	}

	std::cout << "big_int GMP oracle passed unsigned_cases=" << unsigned_cases.size() << " signed_cases=" << signed_cases.size() << '\n';
	return 0;
}
