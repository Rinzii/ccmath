include(CheckCXXSourceCompiles)

# Check if __builtin_sqrt is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_sqrtf(x);
        double dr = __builtin_sqrt(x);
        long double lr = __builtin_sqrtl(x);
        return 0;
    }
" CCMATH_BUILTIN_SQRT_SUPPORT)

if (CCMATH_BUILTIN_SQRT_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_SQRT)
endif ()

# Check if __builtin_sqrt can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_sqrtf(x) == __builtin_sqrtf(x));
        static_assert(__builtin_sqrt(x) == __builtin_sqrt(x));
        static_assert(__builtin_sqrtl(x) == __builtin_sqrtl(x));
        return 0;
    }
" CCMATH_BUILTIN_SQRT_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_SQRT_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_SQRT_CONSTEXPR)
endif ()
