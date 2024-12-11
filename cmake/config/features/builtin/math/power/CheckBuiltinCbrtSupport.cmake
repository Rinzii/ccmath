include(CheckCXXSourceCompiles)

# Check if __builtin_cbrt is supported
check_cxx_source_compiles("
    int main() {
        double x = 2.0;
        float fr = __builtin_cbrtf(x);
        double dr = __builtin_cbrt(x);
        long double lr = __builtin_cbrtl(x);
        return 0;
    }
" CCMATH_BUILTIN_CBRT_SUPPORT)

if (CCMATH_BUILTIN_CBRT_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_CBRT)
endif ()

# Check if __builtin_cbrt can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 2.0;
        static_assert(__builtin_cbrtf(x) == __builtin_cbrtf(x));
        static_assert(__builtin_cbrt(x) == __builtin_cbrt(x));
        static_assert(__builtin_cbrtl(x) == __builtin_cbrtl(x));
        return 0;
    }
" CCMATH_BUILTIN_CBRT_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_CBRT_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_CBRT_CONSTEXPR)
endif ()
