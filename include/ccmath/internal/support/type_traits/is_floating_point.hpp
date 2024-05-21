/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/support/type_traits/is_same.hpp"
#include "ccmath/internal/support/type_traits/remove_cv.hpp"
#include "ccmath/internal/types/float128.hpp"

namespace ccm::support::traits
{
	template <typename T>
	struct is_floating_point
	{
	private:
		template <typename Head, typename... Args>
		static constexpr bool internal_is_unqualified_any_of()
		{
			return (... || is_same_v<remove_cv_t<Head>, Args>);
		}

	public:
#if defined(CCM_TYPES_HAS_FLOAT128)
		static constexpr bool value = internal_is_unqualified_any_of<T, float, double, long double, float128>();
#else
		static constexpr bool value = internal_is_unqualified_any_of<T, float, double, long double>();
#endif // LIBC_TYPES_HAS_FLOAT128
	};
	template <typename T>
	constexpr bool is_floating_point_v = is_floating_point<T>::value;

} // namespace ccm::support::traits