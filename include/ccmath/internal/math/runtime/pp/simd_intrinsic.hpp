
#pragma once

#if defined(CCM_CONFIG_NO_SIMD_INLINE)
	#define CCM_ATTR_SIMD_INTRINSIC
#elif defined(CCM_CONFIG_NO_FORCED_SIMD_INLINE)
	#define CCM_ATTR_SIMD_INTRINSIC inline
#elif defined(__GNUC__) || (defined(__clang__) && !defined(_MSC_VER))
	#define CCM_ATTR_SIMD_INTRINSIC __attribute__((always_inline, artificial)) inline
#elif defined(_MSC_VER)
	#define CCM_ATTR_SIMD_INTRINSIC __forceinline
#else
	#define CCM_ATTR_SIMD_INTRINSIC inline
#endif
