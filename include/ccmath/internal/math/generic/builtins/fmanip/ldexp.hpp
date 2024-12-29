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

#include "ccmath/internal/math/generic/builtins/builtin_helpers.hpp"

#include <type_traits>

/// CCMATH_HAS_CONSTEXPR_BUILTIN_LDEXP
/// This is a macro that is defined if the compiler has constexpr __builtin functions for ldexp that allow static_assert
///
/// Compilers with Support:
/// - GCC 6.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_LDEXP
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
#define CCMATH_HAS_CONSTEXPR_BUILTIN_LDEXP
#endif
#endif

namespace ccm::builtin
{
    // clang-format off
    template <typename T>
    inline constexpr bool has_constexpr_ldexp =
#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_LDEXP
        is_valid_builtin_type<T>;
#else
        false;
#endif
    // clang-format on

    /**
     * Wrapper for constexpr __builtin ldexp functions.
     * This should be used internally and always be wrapped in an if constexpr statement.
     * It exists only to allow for usage of __builtin ldexp functions without triggering a compiler error
     * when the compiler does not support them.
     */
    template <typename T>
    constexpr auto ldexp(T x, int exp) -> std::enable_if_t<has_constexpr_ldexp<T>, T>
    {
        if constexpr (std::is_same_v<T, float>)
        {
            return __builtin_ldexpf(x, exp);
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            return __builtin_ldexp(x, exp);
        }
        else if constexpr (std::is_same_v<T, long double>)
        {
            return __builtin_ldexpl(x, exp);
        }
        // This should never be reached
        return T{};
    }
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_LDEXP
