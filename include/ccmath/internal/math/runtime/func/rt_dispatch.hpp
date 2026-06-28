/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/math/runtime/simd/func/impl/scalar/map.hpp"
#include "ccmath/internal/math/runtime/simd/simd.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"

#include <type_traits>

namespace ccm::rt::detail
{
	template <typename T> [[nodiscard]] inline bool simd_runtime_ok() noexcept
	{
		return ccm::support::fenv::get_rounding_mode() == FE_TONEAREST;
	}

#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
	template <typename T> inline constexpr bool simd_eligible_v = std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>;
#else
	template <typename T> inline constexpr bool simd_eligible_v = std::is_same_v<T, float> || std::is_same_v<T, double>;
#endif

	template <typename T, typename FloatFn, typename DoubleFn> [[nodiscard]] inline auto dispatch_float_double(T value, FloatFn float_fn, DoubleFn double_fn)
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return float_fn(value);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return double_fn(value);
		} else
		{
			return static_cast<T>(double_fn(static_cast<double>(value)));
		}
	}

	template <typename T, typename FloatFn, typename DoubleFn, typename LongDoubleFn = void>
	[[nodiscard]] inline auto dispatch_float_double(T value, FloatFn float_fn, DoubleFn double_fn, LongDoubleFn long_double_fn)
		-> std::enable_if_t<!std::is_same_v<LongDoubleFn, void>, decltype(long_double_fn(value))>
	{
		if constexpr (std::is_same_v<T, float>)
		{
			return float_fn(value);
		} else if constexpr (std::is_same_v<T, double>)
		{
			return double_fn(value);
		} else
		{
			return long_double_fn(value);
		}
	}
} // namespace ccm::rt::detail

namespace ccm::rt::simd_impl
{
#ifdef CCMATH_HAS_SIMD
	template <typename T, typename SimdOp, typename ScalarFn> [[nodiscard]] inline T unary(T value, SimdOp simd_op, ScalarFn scalar_fn) noexcept
	{
		if constexpr (detail::simd_eligible_v<T>)
		{
			if (CCM_UNLIKELY(!detail::simd_runtime_ok<T>()))
			{
				return scalar_fn(value);
			}
			intrin::simd<T, intrin::abi::native> const input(value);
			return simd_op(input).convert();
		} else
		{
			return scalar_fn(value);
		}
	}

	template <typename T, typename SimdOp, typename ScalarFn> [[nodiscard]] inline T binary(T lhs, T rhs, SimdOp simd_op, ScalarFn scalar_fn) noexcept
	{
		if constexpr (detail::simd_eligible_v<T>)
		{
			if (CCM_UNLIKELY(!detail::simd_runtime_ok<T>()))
			{
				return scalar_fn(lhs, rhs);
			}
			intrin::simd<T, intrin::abi::native> const left(lhs);
			intrin::simd<T, intrin::abi::native> const right(rhs);
			return simd_op(left, right).convert();
		} else
		{
			return scalar_fn(lhs, rhs);
		}
	}

	template <typename T, typename ScalarFn> [[nodiscard]] inline T unary_via_scalar_abi(T value, ScalarFn scalar_fn) noexcept
	{
		if constexpr (detail::simd_eligible_v<T>)
		{
			if (CCM_UNLIKELY(!detail::simd_runtime_ok<T>()))
			{
				return scalar_fn(value);
			}
			intrin::simd<T, intrin::abi::scalar> const input(value);
			return intrin::map_scalar(input, scalar_fn).convert();
		} else
		{
			return scalar_fn(value);
		}
	}

	template <typename T, typename ScalarFn> [[nodiscard]] inline T binary_via_scalar_abi(T lhs, T rhs, ScalarFn scalar_fn) noexcept
	{
		if constexpr (detail::simd_eligible_v<T>)
		{
			if (CCM_UNLIKELY(!detail::simd_runtime_ok<T>()))
			{
				return scalar_fn(lhs, rhs);
			}
			intrin::simd<T, intrin::abi::scalar> const left(lhs);
			intrin::simd<T, intrin::abi::scalar> const right(rhs);
			return intrin::map_scalar(left, right, scalar_fn).convert();
		} else
		{
			return scalar_fn(lhs, rhs);
		}
	}
#else
	template <typename T, typename SimdOp, typename ScalarFn> [[nodiscard]] inline T unary(T value, SimdOp /*simd_op*/, ScalarFn scalar_fn) noexcept
	{
		return scalar_fn(value);
	}

	template <typename T, typename SimdOp, typename ScalarFn> [[nodiscard]] inline T binary(T lhs, T rhs, SimdOp /*simd_op*/, ScalarFn scalar_fn) noexcept
	{
		return scalar_fn(lhs, rhs);
	}

	template <typename T, typename ScalarFn> [[nodiscard]] inline T unary_via_scalar_abi(T value, ScalarFn scalar_fn) noexcept
	{
		return scalar_fn(value);
	}

	template <typename T, typename ScalarFn> [[nodiscard]] inline T binary_via_scalar_abi(T lhs, T rhs, ScalarFn scalar_fn) noexcept
	{
		return scalar_fn(lhs, rhs);
	}
#endif
} // namespace ccm::rt::simd_impl
