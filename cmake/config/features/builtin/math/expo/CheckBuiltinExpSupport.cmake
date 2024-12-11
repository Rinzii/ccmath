include(CheckCXXSourceCompiles)

# Check if __builtin_exp is supported
check_cxx_source_compiles("
    int main() {
        double x = 2.0;
        float fr = __builtin_expf(x);
        double dr = __builtin_exp(x);
        long double lr = __builtin_expl(x);
        return 0;
    }
" CCMATH_BUILTIN_EXP_SUPPORT)

if (CCMATH_BUILTIN_EXP_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_EXP)
endif ()

# Check if __builtin_exp can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 2.0;
        static_assert(__builtin_expf(x) == __builtin_expf(x));
        static_assert(__builtin_exp(x) == __builtin_exp(x));
        static_assert(__builtin_expl(x) == __builtin_expl(x));
        return 0;
    }
" CCMATH_BUILTIN_EXP_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_EXP_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_EXP_CONSTEXPR)
endif ()
