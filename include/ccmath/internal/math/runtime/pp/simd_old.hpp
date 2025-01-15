#pragma once

#include <type_traits>

// ISA extension detection. The following defines all the CCMATH_SIMD_HAVE_XXX macros

// ARM Neon
#if defined __ARM_NEON
	#define CCMATH_SIMD_HAVE_NEON 1
#else
	#define CCMATH_SIMD_HAVE_NEON 0
#endif
#if defined __ARM_NEON && (__ARM_ARCH >= 8 || defined __aarch64__)
	#define CCMATH_SIMD_HAVE_NEON_A32 1
#else
	#define CCMATH_SIMD_HAVE_NEON_A32 0
#endif
#if defined __ARM_NEON && defined __aarch64__
	#define CCMATH_SIMD_HAVE_NEON_A64 1
#else
	#define CCMATH_SIMD_HAVE_NEON_A64 0
#endif

// x86 SIMD Extensions
#if defined __SSE2__ || defined __x86_64__
	#define CCMATH_SIMD_HAVE_SSE2 1
#else
	#define CCMATH_SIMD_HAVE_SSE2 0
#endif
#ifdef __SSE4_2__
	#define CCMATH_SIMD_HAVE_SSE4_2 1
#else
	#define CCMATH_SIMD_HAVE_SSE4_2 0
#endif
#ifdef __AVX2__
	#define CCMATH_SIMD_HAVE_AVX2 1
#else
	#define CCMATH_SIMD_HAVE_AVX2 0
#endif
#ifdef __BMI__
	#define CCMATH_SIMD_HAVE_BMI1 1
#else
	#define CCMATH_SIMD_HAVE_BMI1 0
#endif
#ifdef __BMI2__
	#define CCMATH_SIMD_HAVE_BMI2 1
#else
	#define CCMATH_SIMD_HAVE_BMI2 0
#endif
#ifdef __FMA__
	#define CCMATH_SIMD_HAVE_FMA 1
#else
	#define CCMATH_SIMD_HAVE_FMA 0
#endif
#ifdef __FMA4__
	#define CCMATH_SIMD_HAVE_FMA4 1
#else
	#define CCMATH_SIMD_HAVE_FMA4 0
#endif
#ifdef __F16C__
	#define CCMATH_SIMD_HAVE_F16C 1
#else
	#define CCMATH_SIMD_HAVE_F16C 0
#endif
#ifdef __POPCNT__
	#define CCMATH_SIMD_HAVE_POPCNT 1
#else
	#define CCMATH_SIMD_HAVE_POPCNT 0
#endif

#if defined(__GNUC__) && !defined(__clang__) // GCC
	#define CCM_SIMD_INTRINSIC [[gnu::always_inline, gnu::artificial]] inline
#elif defined(_MSC_VER) && !defined(__clang__) // MSVC
	#define CCM_SIMD_INTRINSIC __forceinline
#elif defined(__clang__) // Clang
	#define CCM_SIMD_INTRINSIC [[gnu::always_inline, gnu::artificial]] inline
#else // Other compilers
	#define CCM_SIMD_INTRINSIC inline
#endif

#if defined(CCMATH_SIMD_HAVE_AVX2) || defined(CCMATH_SIMD_HAVE_SSE2) || defined(CCMATH_SIMD_HAVE_SSE4_2)
	#define CCMATH_SIMD_IS_X86 1
#else
	#define CCMATH_SIMD_IS_X86 0
#endif

#if defined(CCMATH_SIMD_HAVE_NEON) || defined(CCMATH_SIMD_HAVE_NEON_A32) || defined(CCMATH_SIMD_HAVE_NEON_A64)
	#define CCMATH_SIMD_IS_ARM 1
#else
	#define CCMATH_SIMD_IS_ARM 0
#endif

// C++20 explicit - if we don't have C++20 explicit then to be safe, we won't enforce explicit.
#ifndef CCM_CPP20_EXPLICIT
	#if __cplusplus >= 202002L
		#define CCM_CPP20_EXPLICIT(EXPR) explicit(EXPR)
	#else
		#define CCM_CPP20_EXPLICIT(EXPR)
	#endif
#endif

namespace ccm::pp
{
	template <typename T>
	struct is_vectorizable : std::is_arithmetic<T>
	{
	};

	template <>
	struct is_vectorizable<bool> : std::false_type
	{
	};

	template <typename T>
	inline constexpr bool is_vectorizable_v = is_vectorizable<T>::value;

	template <typename T, typename = std::enable_if_t<is_vectorizable_v<T>>>
	using Vectorizable = T;

	template <typename T, typename = std::void_t<>>
	struct is_bitmask : std::false_type
	{
	};

	template <typename T>
	struct is_bitmask<T, std::void_t<decltype(std::declval<unsigned &>() = std::declval<T>() & 1u)>> : std::true_type
	{
	};

	template <typename T>
	inline constexpr bool is_bitmask_v = is_bitmask<T>::value;

	// Feature checks
	constexpr inline bool have_neon		= CCMATH_SIMD_HAVE_NEON;
	constexpr inline bool have_neon_a32 = CCMATH_SIMD_HAVE_NEON_A32;
	constexpr inline bool have_neon_a64 = CCMATH_SIMD_HAVE_NEON_A64;
	constexpr inline bool have_sse2		= CCMATH_SIMD_HAVE_SSE2;
	constexpr inline bool have_sse4_2	= CCMATH_SIMD_HAVE_SSE4_2;
	constexpr inline bool have_avx2		= CCMATH_SIMD_HAVE_AVX2;
	constexpr inline bool have_bmi1		= CCMATH_SIMD_HAVE_BMI1;
	constexpr inline bool have_bmi2		= CCMATH_SIMD_HAVE_BMI2;
	constexpr inline bool have_fma		= CCMATH_SIMD_HAVE_FMA;
	constexpr inline bool have_fma4		= CCMATH_SIMD_HAVE_FMA4;
	constexpr inline bool have_f16c		= CCMATH_SIMD_HAVE_F16C;
	constexpr inline bool have_popcnt	= CCMATH_SIMD_HAVE_POPCNT;
	// TODO: Add support for checking on msvc
	// clang-format off
	constexpr inline bool support_neon_float =
		#if defined(_MSC_VER)
		#if defined(_M_ARM) || defined(_M_ARM64)
		#if defined(_M_FP_FAST)
					true;
		#else
					false;
		#endif
		#else
		false;
		#endif
		#else
		#if defined(__GCC_IEC_559)
		__GCC_IEC_559 == 0;
	#elif defined(__FAST_MATH__)
				true;
	#else
				false;
	#endif
	#endif
	// clang-format on

	template <typename T>
	constexpr size_t vectorized_sizeof()
	{
		if constexpr (!is_vectorizable_v<T>) { return 0; }

		if constexpr (sizeof(T) <= 8)
		{
			// X86:
			if constexpr (have_avx2) { return 32; }

			if constexpr (have_sse2) { return 16; }

			// ARM:
			if constexpr (have_neon_a64 || (have_neon_a32 && !std::is_same_v<T, double>)) { return 16; }
			if constexpr (have_neon &&
						  sizeof(T) < 8
						  // Only allow fp if the user allows non-ICE559 fp (e.g.
						  // via -ffast-math). ARMv7 NEON fp is not conforming to
						  // IEC559.
						  && (support_neon_float || !std::is_floating_point_v<T>))
			{
				return 16;
			}
		}

		return sizeof(T);
	}

	namespace simd_abi
	{
		template <int UsedBytes>
		struct vec_builtin
		{
			template <typename T>
			static constexpr size_t size = UsedBytes / sizeof(T);

			struct is_valid_abi_tag : std::bool_constant<(UsedBytes > 1)>
			{
			};

			template <typename T>
			struct is_valid_size_for
				: std::bool_constant<(UsedBytes / sizeof(T) > 1 && UsedBytes % sizeof(T) == 0 && UsedBytes <= vectorized_sizeof<T>() && UsedBytes <= 32)>
			{
			};

			template <typename T>
			struct is_valid : std::conjunction<is_valid_abi_tag, is_vectorizable<T>, is_valid_size_for<T>>
			{
			};

			template <typename T>
			static constexpr bool is_valid_v = is_valid<T>::value;

			// https://github.com/VcDevel/std-simd/blob/a0054893e8f0dc89d4f694c63a080e4b2e32850b/experimental/bits/simd_builtin.h#L954
		};

		template <class T>
		struct scalar
		{
		};

		template <class T>
		struct sse2
		{
		};

		template <class T>
		struct sse4_2
		{
		};

		template <class T>
		struct avx2
		{
		};

		template <class T>
		struct neon
		{
		};

#if CCMATH_SIMD_HAVE_NEON
		using native = neon;
#elif CCMATH_SIMD_HAVE_AVX2
		using native = avx2;
#elif CCMATH_SIMD_HAVE_SSE4_2
		using native = sse4_2;
#elif CCMATH_SIMD_HAVE_SSE2
		using native = sse2;
#else
		using native = scalar;
#endif

	} // namespace simd_abi

	// https://isocpp.org/files/papers/P1928R15.pdf
	// page 42
	template <class T, class Abi>
	struct basic_simd
	{
		using value_type = T;
		using mask_type	 = basic_simd_mask<sizeof(T), Abi>;
		using abi_type	 = Abi;

		// static constexpr integral_constant<simd-size-type, simd-size-v<T, Abi>> size {
		// static constexpr std::integral_constant<>

		constexpr basic_simd() noexcept = default;

		// ctors

		template <class U>
		constexpr basic_simd(U &&value) noexcept
		{
		}

		template <class U, class UAbi>
		constexpr CCM_CPP20_EXPLICIT(true) basic_simd(basic_simd<U, UAbi> const &value) noexcept
		{
		}
	};

	template <size_t Bytes, class Abi>
	struct basic_simd_mask
	{
		using type = T;
	};

	template <class T>
	using simd = basic_simd<T, simd_abi::native>;
} // namespace ccm::pp

// Cleanup global namespace
#undef CCMATH_SIMD_HAVE_NEON
#undef CCMATH_SIMD_HAVE_NEON_A32
#undef CCMATH_SIMD_HAVE_NEON_A64
#undef CCMATH_SIMD_HAVE_SSE2
#undef CCMATH_SIMD_HAVE_SSE4_2
#undef CCMATH_SIMD_HAVE_AVX2
#undef CCMATH_SIMD_HAVE_BMI1
#undef CCMATH_SIMD_HAVE_BMI2
#undef CCMATH_SIMD_HAVE_FMA
#undef CCMATH_SIMD_HAVE_FMA4
#undef CCMATH_SIMD_HAVE_F16C
#undef CCMATH_SIMD_HAVE_POPCNT

#undef CCMATH_SIMD_IS_X86
#undef CCMATH_SIMD_IS_ARM
