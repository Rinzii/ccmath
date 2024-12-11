include(CheckCXXSourceCompiles)

# Check if __builtin_log10 is supported
check_cxx_source_compiles("
    int main() {
        double x = 2.0;
        float fr = __builtin_log10f(x);
        double dr = __builtin_log10(x);
        long double lr = __builtin_log10l(x);
        return 0;
    }
" CCMATH_BUILTIN_LOG10_SUPPORT)

if (CCMATH_BUILTIN_LOG10_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_LOG10)
endif ()

# Check if __builtin_log10 can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 2.0;
        static_assert(__builtin_log10f(x) == __builtin_log10f(x));
        static_assert(__builtin_log10(x) == __builtin_log10(x));
        static_assert(__builtin_log10l(x) == __builtin_log10l(x));
        return 0;
    }
" CCMATH_BUILTIN_LOG10_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_LOG10_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_LOG10_CONSTEXPR)
endif ()
