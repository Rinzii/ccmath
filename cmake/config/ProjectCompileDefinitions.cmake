if (CCMATH_ENABLE_RUNTIME_SIMD)
    target_compile_definitions(ccmath INTERFACE CCM_CONFIG_USE_RT_SIMD)
endif ()

if (CCMATH_DISABLE_ERRNO)
    target_compile_definitions(ccmath INTERFACE CCM_CONFIG_DISABLE_ERRNO)
endif ()