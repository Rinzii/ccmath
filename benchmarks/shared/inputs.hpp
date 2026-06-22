#pragma once

#include <cassert>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

namespace ccm::bench
{
	struct Randomizer
	{
		explicit Randomizer(std::uint_fast32_t seed = 937162211) : m_gen{ seed } {}

		std::vector<int> integers(std::int64_t count, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max())
		{
			assert(count > 0);
			std::vector<int> out;
			out.reserve(static_cast<std::size_t>(count));
			std::uniform_int_distribution dist(min, max);
			for (std::int64_t i = 0; i < count; ++i) { out.push_back(dist(m_gen)); }
			return out;
		}

		std::vector<double> doubles(std::int64_t count, double min = 0.0, double max = 1.0)
		{
			assert(count > 0);
			std::vector<double> out;
			out.reserve(static_cast<std::size_t>(count));
			std::uniform_real_distribution dist(min, max);
			for (std::int64_t i = 0; i < count; ++i) { out.push_back(dist(m_gen)); }
			return out;
		}

	private:
		std::mt19937 m_gen;
	};

	inline Randomizer& thread_rng()
	{
		static thread_local Randomizer rng;
		return rng;
	}

	inline std::vector<int> random_integers(std::int64_t count)
	{ return thread_rng().integers(count); }

	inline std::vector<double> random_doubles(std::int64_t count)
	{ return thread_rng().doubles(count); }

} // namespace ccm::bench
