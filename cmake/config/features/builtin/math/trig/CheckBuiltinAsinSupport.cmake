include(CheckCXXSourceCompiles)

# Check if __builtin_asin is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_asinf(x);
        double dr = __builtin_asin(x);
        long double lr = __builtin_asinl(x);
        return 0;
    }
" CCMATH_BUILTIN_ASIN_SUPPORT)

if (CCMATH_BUILTIN_ASIN_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ASIN)
endif ()

# Check if __builtin_asin can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_asinf(x) == __builtin_asinf(x));
        static_assert(__builtin_asin(x) == __builtin_asin(x));
        static_assert(__builtin_asinl(x) == __builtin_asinl(x));
        return 0;
    }
" CCMATH_BUILTIN_ASIN_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_ASIN_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ASIN_CONSTEXPR)
endif ()
