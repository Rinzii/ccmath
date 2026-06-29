#pragma once

#include "utils/math_samples.hpp"

#include <vector>

namespace ccm::showcase::accuracy
{
	inline std::vector<double> sqrt_corpus()
	{
		std::vector<double> values;
		values.reserve(ccm::test::samples::kSqrtDouble.size() + 8);
		for (double v : ccm::test::samples::kSqrtDouble)
		{
			values.push_back(v);
		}
		values.push_back(1e-200);
		values.push_back(1e200);
		values.push_back(0x1.fffffffffffffp-1022);
		values.push_back(0x1.0p512);
		return values;
	}

	inline std::vector<double> sin_corpus()
	{
		std::vector<double> values;
		values.reserve(ccm::test::samples::kTrigDouble.size() + 4);
		for (double v : ccm::test::samples::kTrigDouble)
		{
			values.push_back(v);
		}
		values.push_back(100.0);
		values.push_back(-100.0);
		values.push_back(1e-8);
		values.push_back(1e8);
		return values;
	}
} // namespace ccm::showcase::accuracy
