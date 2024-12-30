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

#include "ccmath/internal/support/floating_point_traits.hpp"
#include "ccmath/internal/support/fp/bit_mask_traits.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/unreachable.hpp"
#include "ccmath/internal/types/big_int.hpp"
#include "ccmath/math/basic/fabs.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/power/sqrt.hpp"

#include <limits>

namespace ccm::gen
{
   namespace internal::impl
   {

	   template <typename T>
	   constexpr std::enable_if_t<std::is_floating_point_v<T>, T> exp2_impl(T x, T y) noexcept
	   {

		   return 0;
	   }
   } // namespace internal::impl

   template <typename T>
   constexpr std::enable_if_t<std::is_floating_point_v<T>, T> exp2_gen(T base, T exp) noexcept
   {
	   return internal::impl::exp2_impl(base, exp);
   }
} // namespace ccm::gen
