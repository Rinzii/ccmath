#pragma once

#include <type_traits>

namespace ccm::internal::builtin
{
	template <typename T>
	inline constexpr bool has_constexpr_fma =
		#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__)
			(std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>);
	#else
				false;
	#endif

	template <typename T>
	constexpr auto fma(T x, T y, T z)
		-> std::enable_if_t<has_constexpr_fma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) {
			return __builtin_fmaf(x, y, z);
		}
		if constexpr (std::is_same_v<T, double>) {
			return __builtin_fma(x, y, z);
		}
		if constexpr (std::is_same_v<T, long double>) {
			return __builtin_fmal(x, y, z);
		}
	}
} // namespace ccm::internal::builtin
