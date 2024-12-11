include(CheckCXXSourceCompiles)

# Check if __builtin_atan2 is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0, y = 2.0;
        float fr = __builtin_atan2f(x, y);
        double dr = __builtin_atan2(x, y);
        long double lr = __builtin_atan2l(x, y);
        return 0;
    }
" CCMATH_BUILTIN_ATAN2_SUPPORT)

if (CCMATH_BUILTIN_ATAN2_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ATAN2)
endif ()

# Check if __builtin_atan2 can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0, y = 2.0;
        static_assert(__builtin_atan2f(x, y) == __builtin_atan2f(x, y));
        static_assert(__builtin_atan2(x, y) == __builtin_atan2(x, y));
        static_assert(__builtin_atan2l(x, y) == __builtin_atan2l(x, y));
        return 0;
    }
" CCMATH_BUILTIN_ATAN2_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_ATAN2_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ATAN2_CONSTEXPR)
endif ()
