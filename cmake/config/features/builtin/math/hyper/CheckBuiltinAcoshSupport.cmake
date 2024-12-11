include(CheckCXXSourceCompiles)

# Check if __builtin_acosh is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_acoshf(x);
        double dr = __builtin_acosh(x);
        long double lr = __builtin_acoshl(x);
        return 0;
    }
" CCMATH_BUILTIN_ACOSH_SUPPORT)

if (CCMATH_BUILTIN_ACOSH_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ACOSH)
endif ()

# Check if __builtin_acosh can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_acoshf(x) == __builtin_acoshf(x));
        static_assert(__builtin_acosh(x) == __builtin_acosh(x));
        static_assert(__builtin_acoshl(x) == __builtin_acoshl(x));
        return 0;
    }
" CCMATH_BUILTIN_ACOSH_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_ACOSH_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ACOSH_CONSTEXPR)
endif ()
