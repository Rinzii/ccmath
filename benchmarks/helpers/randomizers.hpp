/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cassert>
#include <random>
#include <vector>

namespace ccm::bench
{
	struct Randomizer
	{
	public:
		explicit Randomizer(std::uint_fast32_t seed = 937162211) : m_gen{seed} {}

		std::vector<int> generateRandomIntegers(std::int64_t count, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max())
		{
			assert(count > 0);
			std::vector<int> randomIntegers;
			randomIntegers.reserve(static_cast<unsigned long>(count));
			std::uniform_int_distribution dist(min, max);
			for (std::int64_t i = 0; i < count; ++i) { randomIntegers.push_back(dist(m_gen)); }
			return randomIntegers;
		}

		std::vector<double> generateRandomDoubles(std::int64_t count, double min = 0.0,
												  double max = 1.0)
		{
			assert(count > 0);
			std::vector<double> randomDouble;
			randomDouble.reserve(static_cast<unsigned long>(count));
			std::uniform_real_distribution dist(min, max);
			for (std::int64_t i = 0; i < count; ++i) { randomDouble.push_back(dist(m_gen)); }
			return randomDouble;
		}

	private:
		std::mt19937 m_gen;
	};
} // namespace ccm::bench