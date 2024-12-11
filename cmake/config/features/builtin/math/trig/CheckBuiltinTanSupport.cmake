include(CheckCXXSourceCompiles)

# Check if __builtin_tan is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_tanf(x);
        double dr = __builtin_tan(x);
        long double lr = __builtin_tanl(x);
        return 0;
    }
" CCMATH_BUILTIN_TAN_SUPPORT)

if (CCMATH_BUILTIN_TAN_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_TAN)
endif ()

# Check if __builtin_tan can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_tanf(x) == __builtin_tanf(x));
        static_assert(__builtin_tan(x) == __builtin_tan(x));
        static_assert(__builtin_tanl(x) == __builtin_tanl(x));
        return 0;
    }
" CCMATH_BUILTIN_TAN_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_TAN_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_TAN_CONSTEXPR)
endif ()
