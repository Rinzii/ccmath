include(CheckCXXSourceCompiles)

# Check if __builtin_expm1 is supported
check_cxx_source_compiles("
    int main() {
        double x = 2.0;
        float fr = __builtin_expm1f(x);
        double dr = __builtin_expm1(x);
        long double lr = __builtin_expm1l(x);
        return 0;
    }
" CCMATH_BUILTIN_EXPM1_SUPPORT)

if (CCMATH_BUILTIN_EXPM1_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_EXPM1)
endif ()

# Check if __builtin_expm1 can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 2.0;
        static_assert(__builtin_expm1f(x) == __builtin_expm1f(x));
        static_assert(__builtin_expm1(x) == __builtin_expm1(x));
        static_assert(__builtin_expm1l(x) == __builtin_expm1l(x));
        return 0;
    }
" CCMATH_BUILTIN_EXPM1_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_EXPM1_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_EXPM1_CONSTEXPR)
endif ()
