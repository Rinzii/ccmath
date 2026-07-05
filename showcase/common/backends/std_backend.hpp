#pragma once

#include "utils/test_runtime.hpp"

#include <cmath>

namespace ccm::showcase::backends
{
	struct std_lib
	{
		static constexpr std::string_view label()
		{
			return "std";
		}

		static double eval_sqrt(double x)
		{
			return std::sqrt(ccm::test::runtime_value(x));
		}

		static double eval_sin(double x)
		{
			return std::sin(ccm::test::runtime_value(x));
		}
	};
} // namespace ccm::showcase::backends
