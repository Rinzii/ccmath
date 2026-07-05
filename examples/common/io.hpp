#pragma once

#include <iomanip>
#include <iostream>
#include <string_view>
#include <type_traits>

namespace ccm::examples
{
	inline void banner(std::string_view title)
	{
		std::cout << "\n=== " << title << " ===\n";
	}

	inline void section(std::string_view name)
	{
		std::cout << '\n' << name << '\n';
	}

	template <typename T> void print_row(std::string_view label, T value)
	{
		std::cout << std::left << std::setw(28) << label << std::right << std::setw(16);
		if constexpr (std::is_floating_point_v<T>)
			std::cout << std::setprecision(12) << std::showpos << value << std::noshowpos;
		else
			std::cout << value;
		std::cout << '\n';
	}

	inline void table_header(std::string_view col_a, std::string_view col_b, std::string_view col_c = {})
	{
		std::cout << std::left << std::setw(14) << col_a << std::setw(18) << col_b;
		if (!col_c.empty())
		{
			std::cout << std::setw(18) << col_c;
		}
		std::cout << '\n';
	}

} // namespace ccm::examples
