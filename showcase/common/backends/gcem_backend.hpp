#pragma once

#if defined(CCMATH_SHOWCASE_HAS_GCEM)
	#include <gcem.hpp>
#endif

namespace ccm::showcase::backends
{
	struct gcem_lib
	{
		static constexpr std::string_view label() { return "gcem"; }

		static double eval_sqrt(double x)
		{
			volatile double input = x;
#if defined(CCMATH_SHOWCASE_HAS_GCEM)
			return gcem::sqrt(input);
#else
			(void)input;
			return 0.0;
#endif
		}

		static double eval_sin(double x)
		{
			volatile double input = x;
#if defined(CCMATH_SHOWCASE_HAS_GCEM)
			return gcem::sin(input);
#else
			(void)input;
			return 0.0;
#endif
		}
	};
} // namespace ccm::showcase::backends
