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

// Global seed value for random number generator
constexpr unsigned int DefaultSeed = 937162211; // Using a long prime number as our default seed

// Generate a fixed set of random integers for benchmarking
std::vector<int> generateRandomIntegers(size_t count, unsigned int seed) {
	std::vector<int> randomIntegers;
	std::mt19937 gen(seed);
	std::uniform_int_distribution dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	for (size_t i = 0; i < count; ++i) {
		randomIntegers.push_back(dist(gen));
	}
	return randomIntegers;
}

// Generate a fixed set of random integers for benchmarking
std::vector<double> generateRandomDoubles(size_t count, unsigned int seed) {
	std::vector<double> randomDouble;
	std::mt19937 gen(seed);
	std::uniform_real_distribution dist(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
	for (size_t i = 0; i < count; ++i) {
		randomDouble.push_back(dist(gen));
	}
	return randomDouble;
}
