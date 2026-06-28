#pragma once

#include "ccmath_paths.hpp"

namespace ccm::showcase::backends
{
	struct ccmath_public
	{
		static constexpr std::string_view label()
		{
			return "ccmath/public_default";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sqrt(ccmath_path::path::public_default, x);
		}
	};

	struct ccmath_generic_gen
	{
		static constexpr std::string_view label()
		{
			return "ccmath/generic_gen";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sqrt(ccmath_path::path::generic_gen, x);
		}
	};

	struct ccmath_runtime_rt
	{
		static constexpr std::string_view label()
		{
			return "ccmath/runtime_rt";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sqrt(ccmath_path::path::runtime_rt, x);
		}
	};

	struct ccmath_runtime_simd
	{
		static constexpr std::string_view label()
		{
			return "ccmath/runtime_simd";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sqrt(ccmath_path::path::runtime_simd, x);
		}
	};

	struct ccmath_runtime_builtin
	{
		static constexpr std::string_view label()
		{
			return "ccmath/runtime_builtin";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sqrt(ccmath_path::path::runtime_builtin, x);
		}
	};

	struct ccmath_sin_public
	{
		static constexpr std::string_view label()
		{
			return "ccmath/public_default";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sin(ccmath_path::path::public_default, x);
		}
	};

	struct ccmath_sin_generic_gen
	{
		static constexpr std::string_view label()
		{
			return "ccmath/generic_gen";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sin(ccmath_path::path::generic_gen, x);
		}
	};

	struct ccmath_sin_runtime_rt
	{
		static constexpr std::string_view label()
		{
			return "ccmath/runtime_rt";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sin(ccmath_path::path::runtime_rt, x);
		}
	};

	struct ccmath_sin_runtime_simd
	{
		static constexpr std::string_view label()
		{
			return "ccmath/runtime_simd";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sin(ccmath_path::path::runtime_simd, x);
		}
	};

	struct ccmath_sin_runtime_builtin
	{
		static constexpr std::string_view label()
		{
			return "ccmath/runtime_builtin";
		}

		static double eval(double x)
		{
			return ccmath_path::invoke_sin(ccmath_path::path::runtime_builtin, x);
		}
	};
} // namespace ccm::showcase::backends
