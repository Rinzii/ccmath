// Shows the same expressions producing different bits under each IEEE
// rounding mode.
//
// Build and run from the repository root:
//   c++ -std=c++17 -O2 -frounding-math \
//       tools/proofs/math/approx_doc/cpp/directed_rounding_demo.cpp \
//       -o directed_rounding_demo && ./directed_rounding_demo

#include <cfenv>
#include <cstdio>

#pragma STDC FENV_ACCESS ON

int main()
{
	struct
	{
		int mode;
		const char* name;
	} modes[] = {
		{ FE_TONEAREST, "FE_TONEAREST " },
		{ FE_UPWARD, "FE_UPWARD    " },
		{ FE_DOWNWARD, "FE_DOWNWARD  " },
		{ FE_TOWARDZERO, "FE_TOWARDZERO" },
	};

	volatile double one	  = 1.0;
	volatile double three = 3.0;
	volatile double tenth = 0.1;

	for (const auto& m : modes)
	{
		std::fesetround(m.mode);
		const double third = one / three;
		const double sum   = tenth + tenth + tenth;
		std::printf("%s  1/3 = %a   0.1+0.1+0.1 = %a\n", m.name, third, sum);
	}

	std::fesetround(FE_TONEAREST);
	return 0;
}
