
#pragma once

// include config

#include "const_eval.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "ccmath/internal/predef/attributes/always_inline.hpp"

#include <cstdint>
#include <stdfloat>

#include <functional>
#include <type_traits>

#include "ccmath/internal/support/type_traits.hpp"

namespace ccm::pp
{
	template <int Width>
	struct VecAbi;

	template <int Width>
	struct Avx512Abi;

	struct ScalarAbi;

	namespace detail
	{
		template <size_t N, bool Sanitized = false>
		struct BitMask;

		template <size_t N>
		using SanitizedBitMask = BitMask<N, true>;

		template <size_t Bytes>
		struct make_unsigned_int;

		template <>
		struct make_unsigned_int<sizeof(unsigned int)>
		{
			using type = unsigned int;
		};

		template <>
		struct make_unsigned_int<sizeof(unsigned long) + (sizeof(unsigned long) == sizeof(unsigned int))>
		{
			using type = unsigned long;
		};

		template <>
		struct make_unsigned_int<sizeof(unsigned long long) + (sizeof(unsigned long long) == sizeof(unsigned long))>
		{
			using type = unsigned long long;
		};

		template <>
		struct make_unsigned_int<sizeof(unsigned short)>
		{
			using type = unsigned short;
		};

		template <>
		struct make_unsigned_int<sizeof(unsigned char)>
		{
			using type = unsigned char;
		};

		template <typename T>
		using make_unsigned_int_t = typename make_unsigned_int<sizeof(T)>::type;

		template <typename T>
		using make_signed_int_t = std::make_signed_t<make_unsigned_int_t<T>>;

		template <size_t B>
		using mask_integer_from = std::make_signed_t<typename make_unsigned_int<B>::type>;

		template <typename T>
		struct is_vectorizable : std::bool_constant<false>
		{
		};

		template <>
		struct is_vectorizable<char> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<wchar_t> : std::bool_constant<true>
		{
		};
// C++20
#if __cpp_char8_t >= 201811L
		template <>
		struct is_vectorizable<char8_t> : std::bool_constant<true>
		{
		};
#endif
		template <>
		struct is_vectorizable<char16_t> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<char32_t> : std::bool_constant<true>
		{
		};

		template <>
		struct is_vectorizable<signed char> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<unsigned char> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<signed short> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<unsigned short> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<signed int> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<unsigned int> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<signed long> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<unsigned long> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<signed long long> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<unsigned long long> : std::bool_constant<true>
		{
		};

		template <>
		struct is_vectorizable<float> : std::bool_constant<true>
		{
		};
		template <>
		struct is_vectorizable<double> : std::bool_constant<true>
		{
		};
		// GCC specific
#ifdef __STDCPP_FLOAT16_T__
// template <> struct is_vectorizable<std::float16_t> : std::bool_constant<true> {};
#endif
#ifdef __STDCPP_FLOAT32_T__
		template <>
		struct is_vectorizable<std::float32_t> : std::bool_constant<true>
		{
		};
#endif
#ifdef __STDCPP_FLOAT64_T__
		template <>
		struct is_vectorizable<std::float64_t> : std::bool_constant<true>
		{
		};
#endif

		template <typename T, typename>
		struct make_dependent
		{
			using type = T;
		};

		template <typename T, typename U>
		using make_dependent_t = typename make_dependent<T, U>::type;

		template <int B, typename T>
		CCM_CONSTEVAL auto native_abi_impl_recursive()
		{
			constexpr int Width = B / sizeof(T);
			if constexpr (Avx512Abi<Width>::template IsValid<T>::value) { return Avx512Abi<Width>(); }
			else if constexpr (VecAbi<Width>::template IsValid<T>::value) { return VecAbi<Width>(); }
			else if constexpr (B > sizeof(T)) { return native_abi_impl_recursive<B / 2, T>(); }
			else { return make_dependent_t<ScalarAbi, T>(); }
		}

		struct InvalidAbi
		{
		};

		template <typename T>
		CCM_CONSTEVAL auto native_abi_impl()
		{
			if constexpr (is_vectorizable<T>::value)
			{
				// one is used to make VecAbi a dependent type
				constexpr int one = sizeof(T) / sizeof(T);
				return native_abi_impl_recursive<one * 256, T>();
			}
			else { return InvalidAbi(); }
		}

		template <typename T>
		using NativeAbi = decltype(native_abi_impl<T>());

		using SimdSizeType = int;

		template <typename T, SimdSizeType N>
		struct DeduceAbi;

		template <typename T, SimdSizeType N>
		using deduce_t = typename DeduceAbi<T, N>::type;
	} // namespace detail

	template <typename Abi0, detail::SimdSizeType N>
	struct AbiArray;

	template <detail::SimdSizeType N, typename Tag>
	struct AbiCombine;

	template <typename T, typename Abi = detail::NativeAbi<T>>
	class basic_simd;

	template <size_t Bytes, typename Abi = detail::NativeAbi<detail::mask_integer_from<Bytes>>>
	class basic_simd_mask;

	template <typename... Flags>
	struct simd_flags;

	template <typename>
	struct is_simd : std::bool_constant<false>
	{
	};

	template <typename T>
	inline constexpr bool is_simd_v = is_simd<T>::value;

	template <typename>
	struct is_simd_mask : std::bool_constant<false>
	{
	};

	template <typename T>
	inline constexpr bool is_simd_mask_v = is_simd_mask<T>::value;

	template <typename T, typename Abi = detail::NativeAbi<T>>
	struct simd_size
	{
	};

	template <typename T, typename Abi = detail::NativeAbi<T>>
	inline constexpr detail::SimdSizeType simd_size_v = simd_size<T, Abi>::value;

	template <typename T, typename Up = typename T::value_type>
	struct simd_alignment
	{
	};

	template <typename T, typename Up = typename T::value_type>
	inline constexpr size_t simd_alignment_v = simd_alignment<T, Up>::value;

	template <typename, typename>
	struct rebind_simd
	{
	};

	template <typename T, typename V>
	using rebind_simd_t = typename rebind_simd<T, V>::type;

	template <detail::SimdSizeType, typename>
	struct resize_simd
	{
	};

	template <detail::SimdSizeType N, typename V>
	using resize_simd_t = typename resize_simd<N, V>::type;

	template <typename T, detail::SimdSizeType N = basic_simd<T>::size()>
	using simd = basic_simd<T, detail::deduce_t<T, N>>;

	template <typename T, detail::SimdSizeType N = basic_simd<T>::size()>
	using simd_mask = basic_simd_mask<sizeof(T), detail::deduce_t<T, N>>;

	// mask_reductions.h

	namespace detail
	{
		// Define a helper trait for C++17 to simulate std::totally_ordered
		template <typename T>
		struct is_totally_ordered : std::integral_constant<bool, std::is_arithmetic<T>::value || std::is_pointer<T>::value>
		{
		};
	} // namespace detail

	template <size_t B, typename Abi>
	CCM_ALWAYS_INLINE constexpr bool all_of(const basic_simd_mask<B, Abi> & mask) noexcept;

	template <typename Bool, std::enable_if_t<std::is_same_v<Bool, bool>, int> = 0>
	CCM_ALWAYS_INLINE constexpr bool all_of(Bool x) noexcept;

	template <size_t B, typename Abi>
	CCM_ALWAYS_INLINE constexpr bool any_of(const basic_simd_mask<B, Abi> & mask) noexcept;

	template <typename Bool, std::enable_if_t<std::is_same_v<Bool, bool>, int> = 0>
	CCM_ALWAYS_INLINE constexpr bool any_of(Bool value) noexcept;

	template <size_t B, typename Abi>
	CCM_ALWAYS_INLINE constexpr bool none_of(const basic_simd_mask<B, Abi> & mask) noexcept;

	template <typename Bool, std::enable_if_t<std::is_same_v<Bool, bool>, int> = 0>
	CCM_ALWAYS_INLINE constexpr bool none_of(Bool value) noexcept;

	template <size_t B, typename Abi>
	CCM_ALWAYS_INLINE constexpr detail::SimdSizeType reduce_count(const basic_simd_mask<B, Abi> & mask) noexcept;

	template <typename Bool, std::enable_if_t<std::is_same_v<Bool, bool>, int> = 0>
	CCM_ALWAYS_INLINE constexpr detail::SimdSizeType reduce_count(Bool value) noexcept;

	template <size_t B, typename Abi>
	CCM_ALWAYS_INLINE constexpr detail::SimdSizeType reduce_min_index(const basic_simd_mask<B, Abi> & mask);

	template <typename Bool, std::enable_if_t<std::is_same_v<Bool, bool>, int> = 0>
	CCM_ALWAYS_INLINE constexpr detail::SimdSizeType reduce_min_index(Bool value) noexcept;

	template <size_t B, typename Abi>
	CCM_ALWAYS_INLINE constexpr detail::SimdSizeType reduce_max_index(const basic_simd_mask<B, Abi> & mask);

	template <typename Bool, std::enable_if_t<std::is_same_v<Bool, bool>, int> = 0>
	CCM_ALWAYS_INLINE constexpr detail::SimdSizeType reduce_max_index(Bool value) noexcept;

	template <typename VecType, typename ScalarType, typename Abi>
	CCM_ALWAYS_INLINE constexpr auto simd_split(const basic_simd<ScalarType, Abi> & vec) noexcept;

	template <typename MaskType, size_t B, typename Abi>
	CCM_ALWAYS_INLINE constexpr auto simd_split(const basic_simd_mask<B, Abi> & mask) noexcept;

	template <typename T, typename... Abis>
	CCM_ALWAYS_INLINE constexpr simd<T, (simd_size_v<T, Abis> + ...)> simd_cat(const basic_simd<T, Abis> &... xs) noexcept;

	template <size_t B, typename... Abis>
	CCM_ALWAYS_INLINE constexpr simd_mask<detail::mask_integer_from<B>, (basic_simd_mask<B, Abis>::size.value + ...)>
	simd_cat(const basic_simd_mask<B, Abis> &... xs) noexcept;

	template <typename T, typename Abi, typename BinaryOperation = std::plus<>,
			  typename = std::enable_if_t<std::is_invocable_v<BinaryOperation, simd<T, 1>, simd<T, 1>>>>
	constexpr T reduce(const basic_simd<T, Abi> & x, BinaryOperation binary_op = {});

	template <typename T, typename Abi, typename BinaryOperation = std::plus<>, typename = std::enable_if_t<std::is_invocable_v<BinaryOperation, T, T>>>
	constexpr T reduce(const basic_simd<T, Abi> & x, const typename basic_simd<T, Abi>::mask_type & k, support::traits::type_identity_t<T> identity_element,
					   BinaryOperation binary_op = {});

	template <typename T, typename Abi>
	constexpr T reduce(const basic_simd<T, Abi> & x, const typename basic_simd<T, Abi>::mask_type & k, std::plus<> binary_op = {}) noexcept;

	template <typename T, typename Abi>
	constexpr T reduce(const basic_simd<T, Abi> & x, const typename basic_simd<T, Abi>::mask_type & k, std::multiplies<> binary_op) noexcept;

	template <typename T, typename Abi, typename = std::enable_if_t<std::is_integral_v<T>>>
	constexpr T reduce(const basic_simd<T, Abi> & x, const typename basic_simd<T, Abi>::mask_type & k, std::bit_and<> binary_op) noexcept;

	template <typename T, typename Abi, typename = std::enable_if_t<std::is_integral_v<T>>>
	constexpr T reduce(const basic_simd<T, Abi> & x, const typename basic_simd<T, Abi>::mask_type & k, std::bit_or<> binary_op) noexcept;

	template <typename T, typename Abi, typename = std::enable_if_t<std::is_integral_v<T>>>
	constexpr T reduce(const basic_simd<T, Abi> & x, const typename basic_simd<T, Abi>::mask_type & k, std::bit_xor<> binary_op) noexcept;

	template <typename T, typename Abi, typename = std::enable_if_t<detail::is_totally_ordered<T>::value>>
	constexpr T reduce_min(const basic_simd<T, Abi> & x) noexcept;

	template <typename T, typename Abi, typename = std::enable_if_t<detail::is_totally_ordered<T>::value>>
	constexpr T reduce_min(const basic_simd<T, Abi> & x, const typename basic_simd<T, Abi>::mask_type & k) noexcept;

	template <typename T, typename Abi, typename = std::enable_if_t<detail::is_totally_ordered<T>::value>>
	constexpr T reduce_max(const basic_simd<T, Abi> & x) noexcept;

	template <typename T, typename Abi, typename = std::enable_if_t<detail::is_totally_ordered<T>::value>>
	constexpr T reduce_max(const basic_simd<T, Abi> & x, const typename basic_simd<T, Abi>::mask_type & k) noexcept;

} // namespace ccm::pp
