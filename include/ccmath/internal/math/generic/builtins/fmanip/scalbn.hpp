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

/// CCMATH_HAS_CONSTEXPR_BUILTIN_SCALBN
/// This is a macro that is defined if the compiler has constexpr __builtin functions for scalbn that allow static_assert
///
/// Compilers with Support:
/// - GCC 6.1+

#ifndef CCMATH_HAS_CONSTEXPR_BUILTIN_SCALBN
#if defined(__GNUC__) && (__GNUC__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ >= 1)) && !defined(__clang__) && !defined(__NVCOMPILER_MAJOR__)
#define CCMATH_HAS_CONSTEXPR_BUILTIN_SCALBN
#endif
#endif

namespace ccm::builtin
{
    // clang-format off
    template <typename T>
    inline constexpr bool has_constexpr_scalbn =
#ifdef CCMATH_HAS_CONSTEXPR_BUILTIN_SCALBN
        is_valid_builtin_type<T>;
#else
        false;
#endif
    // clang-format on

    /**
     * Wrapper for constexpr __builtin scalbn functions.
     * This should be used internally and always be wrapped in an if constexpr statement.
     * It exists only to allow for usage of __builtin scalbn functions without triggering a compiler error
     * when the compiler does not support them.
     */
    template <typename T>
    constexpr auto scalbn(T x, int exp) -> std::enable_if_t<has_constexpr_scalbn<T>, T>
    {
        if constexpr (std::is_same_v<T, float>)
        {
            return __builtin_scalbnf(x, exp);
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            return __builtin_scalbn(x, exp);
        }
        else if constexpr (std::is_same_v<T, long double>)
        {
            return __builtin_scalbnl(x, exp);
        }
        // This should never be reached
        return T{};
    }
} // namespace ccm::builtin

// Cleanup the global namespace
#undef CCMATH_HAS_CONSTEXPR_BUILTIN_SCALBN
