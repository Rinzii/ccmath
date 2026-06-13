// MPFR oracle check for the restricted sine kernel from the
// approximation guide.
//
// Build and run from the repository root:
//   c++ -std=c++17 -O2 docs/approximating_functions/cpp/sin_small_check.cpp \
//       -lmpfr -lgmp -o sin_small_check && ./sin_small_check
// Add -I and -L flags if MPFR is not on the default search paths.

#include <mpfr.h>

#include <cinttypes>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{

	constexpr double sin_small(double x) noexcept
	{
		const double z = x * x;

		const double p =
			(((0x1.71de3a556c734p-19 * z - 0x1.a01a01a01a01ap-13) * z + 0x1.1111111111111p-7) * z - 0x1.5555555555555p-3) * z + 0x1.0000000000000p+0;

		return x * p;
	}

	// Maps a double onto the integers so that consecutive finite doubles map
	// to consecutive integers.  The difference of two mapped values is then
	// the number of representable doubles between them.
	std::int64_t ulp_order(double v)
	{
		std::int64_t bits;
		std::memcpy(&bits, &v, sizeof bits);
		return bits >= 0 ? bits : INT64_MIN - bits;
	}

	std::int64_t ulp_distance(double a, double b)
	{
		const std::int64_t d = ulp_order(a) - ulp_order(b);
		return d < 0 ? -d : d;
	}

	// Correctly rounded reference through MPFR.
	double sin_reference(double x)
	{
		mpfr_t t;
		mpfr_init2(t, 128);
		mpfr_set_d(t, x, MPFR_RNDN);
		mpfr_sin(t, t, MPFR_RNDN);
		const double r = mpfr_get_d(t, MPFR_RNDN);
		mpfr_clear(t);
		return r;
	}

} // namespace

int main()
{
	const double bound = 0x1p-4;
	const long grid	   = 200000;

	std::int64_t max_ulps = 0;
	double worst_x		  = 0.0;
	long exact			  = 0;

	for (long i = 0; i <= grid; ++i)
	{
		const double x		 = -bound + (2.0 * bound) * static_cast<double>(i) / static_cast<double>(grid);
		const std::int64_t d = ulp_distance(sin_small(x), sin_reference(x));
		if (d == 0) { ++exact; }
		if (d > max_ulps)
		{
			max_ulps = d;
			worst_x	 = x;
		}
	}

	std::printf("inputs tested:      %ld\n", grid + 1);
	std::printf("correctly rounded:  %ld\n", exact);
	std::printf("max ULP distance:   %" PRId64 "\n", max_ulps);
	std::printf("worst input:        %a\n", worst_x);

	return max_ulps > 1 ? 1 : 0;
}
