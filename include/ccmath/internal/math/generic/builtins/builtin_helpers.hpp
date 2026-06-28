#pragma once

#include <type_traits>

namespace ccm::builtin
{
	/**
	 * @internal
	 * @brief Helper to check that a valid builtin type is used
	 */
	template <typename T> inline constexpr bool is_valid_builtin_type = std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>;

	/**
	 * @internal
	 * @brief Like is_valid_builtin_type, but false in deterministic builds.
	 *
	 * Transcendental builtins lower to system libm, which is not correctly rounded and differs
	 * across platforms. Deterministic mode routes those functions through the generic kernels by
	 * disabling their builtin dispatch (both the constexpr and runtime paths). Exact and
	 * correctly-rounded builtins (fma, sqrt, the rounding and fp-manipulation ops) stay on
	 * is_valid_builtin_type since they are bit-identical across hardware.
	 */
	template <typename T>
	inline constexpr bool is_valid_transcendental_builtin_type = is_valid_builtin_type<T>
#ifdef CCM_CONFIG_DETERMINISTIC
																 && false
#endif
		;
} // namespace ccm::builtin
