#pragma once

#include <concepts>
#include <string_view>

namespace ccm::showcase
{
	template <typename Backend>
	concept RuntimeUnaryBackend = requires(double x)
	{
		{ Backend::label() }->std::convertible_to<std::string_view>;
		{ Backend::eval(x) }->std::same_as<double>;
	};

	template <typename Backend>
	concept ConstexprUnaryBackend = requires(double x)
	{
		{ Backend::label() }->std::convertible_to<std::string_view>;
		{ Backend::consteval_probe(x) }->std::same_as<double>;
	};
} // namespace ccm::showcase
