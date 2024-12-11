include(CheckCXXSourceCompiles)

# Check if __builtin_exp2 is supported
check_cxx_source_compiles("
    int main() {
        double x = 2.0;
        float fr = __builtin_exp2f(x);
        double dr = __builtin_exp2(x);
        long double lr = __builtin_exp2l(x);
        return 0;
    }
" CCMATH_BUILTIN_EXP2_SUPPORT)

if (CCMATH_BUILTIN_EXP2_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_EXP2)
endif ()

# Check if __builtin_exp2 can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 2.0;
        static_assert(__builtin_exp2f(x) == __builtin_exp2f(x));
        static_assert(__builtin_exp2(x) == __builtin_exp2(x));
        static_assert(__builtin_exp2l(x) == __builtin_exp2l(x));
        return 0;
    }
" CCMATH_BUILTIN_EXP2_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_EXP2_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_EXP2_CONSTEXPR)
endif ()
