include(CheckCXXSourceCompiles)

# Check if __builtin_log1p is supported
check_cxx_source_compiles("
    int main() {
        double x = 2.0;
        float fr = __builtin_log1pf(x);
        double dr = __builtin_log1p(x);
        long double lr = __builtin_log1pl(x);
        return 0;
    }
" CCMATH_BUILTIN_LOG1P_SUPPORT)

if (CCMATH_BUILTIN_LOG1P_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_LOG1P)
endif ()

# Check if __builtin_log1p can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 2.0;
        static_assert(__builtin_log1pf(x) == __builtin_log1pf(x));
        static_assert(__builtin_log1p(x) == __builtin_log1p(x));
        static_assert(__builtin_log1pl(x) == __builtin_log1pl(x));
        return 0;
    }
" CCMATH_BUILTIN_LOG1P_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_LOG1P_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_LOG1P_CONSTEXPR)
endif ()
