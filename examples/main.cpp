/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#include "ccmath/ccmath.hpp"
#include <cmath>
#include <iostream>

void try_all_ccm_basic_funcs();
void try_all_ccm_compare_funcs();

int main()
{
	try_all_ccm_basic_funcs();
	try_all_ccm_compare_funcs();
    return 0;
}

void try_all_ccm_basic_funcs()
{
	int remquo_ptr = 0;
	const auto try_abs = ccm::abs(-1.0);
	const auto try_fdim = ccm::fdim(1.0, 0.0);
	const auto try_fma = ccm::fma(1.0, 1.0, 1.0);
	const auto try_fmax = ccm::fmax(1.0, 1.0);
	const auto try_fmin = ccm::fmin(1.0, 1.0);
	const auto try_fmod = ccm::fmod(1.0, 1.0);
	const auto try_nan = ccm::nan("");
	const auto try_remainder = ccm::remainder(1.0, 1.0);
	const auto try_remquo = ccm::remquo(1.0, 1.0, &remquo_ptr);

	// print all basics output
	std::cout << "ccm::abs(-1.0): " << try_abs << std::endl;
	std::cout << "ccm::fdim(1.0, 0.0): " << try_fdim << std::endl;
	std::cout << "ccm::fma(1.0, 1.0, 1.0): " << try_fma << std::endl;
	std::cout << "ccm::fmax(1.0, 1.0): " << try_fmax << std::endl;
	std::cout << "ccm::fmin(1.0, 1.0): " << try_fmin << std::endl;
	std::cout << "ccm::fmod(1.0, 1.0): " << try_fmod << std::endl;
	std::cout << "ccm::nan(\"\"): " << try_nan << std::endl;
	std::cout << "ccm::remainder(1.0, 1.0): " << try_remainder << std::endl;
	std::cout << "ccm::remquo(1.0, 1.0, *0): " << try_remquo << " - " << remquo_ptr << std::endl;
}

void try_all_ccm_compare_funcs()
{
	const auto try_fpclassify = ccm::fpclassify(1.0);
	const auto try_isfinite = ccm::isfinite(1.0);
	constexpr auto try_isgreater = ccm::isgreater(1.0, 0.0);
	constexpr auto try_isgreaterequal = ccm::isgreaterequal(1.0, 0.0);
	constexpr auto try_isless = ccm::isless(1.0, 0.0);
	constexpr auto try_islessequal = ccm::islessequal(1.0, 0.0);
	constexpr auto try_islessgreater = ccm::islessgreater(1.0, 0.0);
	const auto try_isnan = ccm::isnan(1.0);
	const auto try_isnormal = ccm::isnormal(1.0);
	const auto try_isunordered = ccm::isunordered(1.0, 0.0);
	const auto try_signbit = ccm::signbit(1.0);

	// print all compares output
	std::cout << "ccm::fpclassify(1.0): " << try_fpclassify << std::endl;
	std::cout << "ccm::isfinite(1.0): " << try_isfinite << std::endl;
	std::cout << "ccm::isgreater(1.0, 0.0): " << try_isgreater << std::endl;
	std::cout << "ccm::isgreaterequal(1.0, 0.0): " << try_isgreaterequal << std::endl;
	std::cout << "ccm::isless(1.0, 0.0): " << try_isless << std::endl;
	std::cout << "ccm::islessequal(1.0, 0.0): " << try_islessequal << std::endl;
	std::cout << "ccm::islessgreater(1.0, 0.0): " << try_islessgreater << std::endl;
	std::cout << "ccm::isnan(1.0): " << try_isnan << std::endl;
	std::cout << "ccm::isnormal(1.0): " << try_isnormal << std::endl;
	std::cout << "ccm::isunordered(1.0, 0.0): " << try_isunordered << std::endl;
	std::cout << "ccm::signbit(1.0): " << try_signbit << std::endl;
}