/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <random>
#include <vector>

namespace ccm::bench
{
	struct Randomizer
	{
	public:
		explicit Randomizer(std::uint_fast32_t seed = 937162211) : m_gen{seed} {}

		std::vector<int> generateRandomIntegers(std::size_t count, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max())
		{
			std::vector<int> randomIntegers;
			randomIntegers.reserve(count);
			std::uniform_int_distribution dist(min, max);
			for (std::size_t i = 0; i < count; ++i) { randomIntegers.push_back(dist(m_gen)); }
			return randomIntegers;
		}

		std::vector<double> generateRandomDoubles(std::size_t count, double min = 0.0,
												  double max = 1.0)
		{
			std::vector<double> randomDouble;
			randomDouble.reserve(count);
			std::uniform_real_distribution dist(min, max);
			for (std::size_t i = 0; i < count; ++i) { randomDouble.push_back(dist(m_gen)); }
			return randomDouble;
		}

	private:
		std::mt19937 m_gen;
	};
} // namespace ccm::bench