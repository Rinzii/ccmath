/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// Wrapper for type_traits with additional specializations

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/support/meta_compare.hpp"
#include "ccmath/internal/types/float128.hpp"
#include "ccmath/internal/types/int128_types.hpp"

#include <type_traits>

namespace ccm::support::traits
{
	// clang-format off

	template<class T>
	struct type_identity { using type = T; };

	template<class T>
	using type_identity_t = typename type_identity<T>::type;

	/// is_char specialization

	/// @cond internal
	template <typename T> struct internal_is_char : std::false_type {};
	template <> struct internal_is_char<char> : std::true_type {};
	template <> struct internal_is_char<wchar_t> : std::true_type {};
	#if (__cplusplus >= 202002L) || defined(__cpp_char8_t) || defined(__cpp_lib_char8_t) // C++20 defines char8_t
	template <> struct internal_is_char<char8_t> : std::true_type {};
	#endif
	template <> struct internal_is_char<char16_t> : std::true_type {};
	template <> struct internal_is_char<char32_t> : std::true_type {};
	template <> struct internal_is_char<signed char> : std::true_type {};
	template <> struct internal_is_char<unsigned char> : std::true_type {};
	/// @endcond
	
	template <typename T> constexpr bool is_char_v = internal_is_char<T>::value;

	
	/// is_integral with additional specializations - for internal use only. Prefer std::is_integral.

	/// @cond internal
	template<typename> struct internal_is_integral_helper : std::false_type {};
	template<> struct internal_is_integral_helper<bool> : std::true_type {};
	template<> struct internal_is_integral_helper<char> : std::true_type {};
	template<> struct internal_is_integral_helper<signed char> : std::true_type {};
	template<> struct internal_is_integral_helper<unsigned char>  : std::true_type {};
	template<> struct internal_is_integral_helper<wchar_t> : std::true_type {};
#if (__cplusplus >= 202002L) || defined(__cpp_char8_t) || defined(__cpp_lib_char8_t) // C++20 defines char8_t
	template<> struct internal_is_integral_helper<char8_t> : std::true_type {};
#endif
	template<> struct internal_is_integral_helper<char16_t> : std::true_type {};
	template<> struct internal_is_integral_helper<char32_t> : std::true_type {};
	template<> struct internal_is_integral_helper<short> : std::true_type {};
	template<> struct internal_is_integral_helper<unsigned short> : std::true_type {};
	template<> struct internal_is_integral_helper<int> : std::true_type {};
	template<> struct internal_is_integral_helper<unsigned int> : std::true_type {};
	template<> struct internal_is_integral_helper<long> : std::true_type {};
	template<> struct internal_is_integral_helper<unsigned long> : std::true_type {};
	template<> struct internal_is_integral_helper<long long> : std::true_type {};
	template<> struct internal_is_integral_helper<unsigned long long> : std::true_type {};
	#ifdef CCM_TYPES_HAS_INT128
	template<> struct internal_is_integral_helper<types::int128_t> : std::true_type {};
	template<> struct internal_is_integral_helper<types::uint128_t> : std::true_type {};
	#endif
	/// @endcond
	
	template<typename T> struct ccm_is_integral : internal_is_integral_helper<std::remove_cv_t<T>>::type {};
	template<typename T> constexpr bool ccm_is_integral_v = ccm_is_integral<T>::value;

	/// is_floating_point

	/// @cond internal
	template <typename> struct internal_is_floating_point_helper : std::false_type{};
	template <> struct internal_is_floating_point_helper<float> : std::true_type{};
	template <> struct internal_is_floating_point_helper<double> : std::true_type{};
	template <> struct internal_is_floating_point_helper<long double> : std::true_type{};
#ifdef __STDCPP_FLOAT16_T__
	template <> struct internal_is_floating_point_helper<_Float16> : public std::true_type{};
#endif
#ifdef __STDCPP_FLOAT32_T__
	template <> struct internal_is_floating_point_helper<_Float32> : public std::true_type{};
#endif
#ifdef __STDCPP_FLOAT64_T__
	template <> struct internal_is_floating_point_helper<_Float64> : public std::true_type{};
#endif
#ifdef __STDCPP_FLOAT128_T__
	template <> struct internal_is_floating_point_helper<_Float128> : public std::true_type{};
#endif
#ifdef __STDCPP_BFLOAT16_T__
	template <> struct internal_is_floating_point_helper<__gnu_cxx::__bfloat16_t> : std::public true_type{};
#endif
#ifdef CCM_TYPES_HAS_FLOAT128
	template <> struct internal_is_floating_point_helper<types::float128> : std::true_type{};
#endif
	/// @endcond

	template <typename T> struct ccm_is_floating_point : internal_is_floating_point_helper<std::remove_cv_t<T>>::type{};
	template <typename T> constexpr bool ccm_is_floating_point_v = ccm_is_floating_point<T>::value;


	/// is_arithmetic

	template <typename T> struct ccm_is_arithmetic : or_<ccm_is_integral<T>, ccm_is_floating_point<T>>::type {};
	template <typename T> constexpr bool ccm_is_arithmetic_v = ccm_is_arithmetic<T>::value;


	/// is_signed

	/// @cond internal
	template <typename T, bool = ccm_is_arithmetic<T>::value> struct internal_is_signed_helper : std::false_type {};
	template <typename T> struct internal_is_signed_helper<T, true> : std::integral_constant<bool, T(-1) < T(0)> {};
	/// @endcond

	template <typename T> struct ccm_is_signed : internal_is_signed_helper<T>::type {};
	template <typename T> constexpr bool ccm_is_signed_v = ccm_is_signed<T>::value;


	/// is_unsigned

	template <typename T> struct ccm_is_unsigned : and_<ccm_is_arithmetic<T>, not_<ccm_is_signed<T>>>::type {};
	template <typename T> constexpr bool ccm_is_unsigned_v = ccm_is_unsigned<T>::value;


	/// is_unsigned_integer custom trait

	template <typename T> constexpr bool is_unsigned_integer_v = std::conjunction_v<ccm_is_integral<T>, ccm_is_unsigned<T>>;

	// clang-format on


} // namespace ccm::support::traits
