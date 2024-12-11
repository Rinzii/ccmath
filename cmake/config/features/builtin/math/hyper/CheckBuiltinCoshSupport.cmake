include(CheckCXXSourceCompiles)

# Check if __builtin_cosh is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_coshf(x);
        double dr = __builtin_cosh(x);
        long double lr = __builtin_coshl(x);
        return 0;
    }
" CCMATH_BUILTIN_COSH_SUPPORT)

if (CCMATH_BUILTIN_COSH_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_COSH)
endif ()

# Check if __builtin_cosh can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_coshf(x) == __builtin_coshf(x));
        static_assert(__builtin_cosh(x) == __builtin_cosh(x));
        static_assert(__builtin_coshl(x) == __builtin_coshl(x));
        return 0;
    }
" CCMATH_BUILTIN_COSH_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_COSH_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_COSH_CONSTEXPR)
endif ()
