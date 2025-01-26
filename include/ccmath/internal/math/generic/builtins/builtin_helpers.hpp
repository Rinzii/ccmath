#pragma once

#include <type_traits>

namespace ccm::builtin
{
	/**
	 * @internal
	 * @brief Helper to check that a valid builtin type is used
	 */
	template <typename T>
	inline constexpr bool is_valid_builtin_type = std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>;
} // namespace ccm::builtin
