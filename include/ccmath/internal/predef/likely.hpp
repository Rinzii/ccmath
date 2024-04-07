/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

#ifndef CCM_LIKELY
   #if defined(__cplusplus) && __cplusplus >= 202003L
       #define CCM_LIKELY(x) [[likely]] (x)
   #elif defined(__GNUC__) || defined(__clang__)
       #define CCM_LIKELY(x) __builtin_expect(!!(x), 1)
   #else
       #define CCM_LIKELY(x) (!!(x))
   #endif // defined(__cplusplus) && __cplusplus >= 202003L
#endif // CCM_LIKELY
