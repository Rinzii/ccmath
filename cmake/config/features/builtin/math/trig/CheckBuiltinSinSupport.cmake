include(CheckCXXSourceCompiles)

# Check if __builtin_sin is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_sinf(x);
        double dr = __builtin_sin(x);
        long double lr = __builtin_sinl(x);
        return 0;
    }
" CCMATH_BUILTIN_SIN_SUPPORT)

if (CCMATH_BUILTIN_SIN_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_SIN)
endif ()

# Check if __builtin_sin can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_sinf(x) == __builtin_sinf(x));
        static_assert(__builtin_sin(x) == __builtin_sin(x));
        static_assert(__builtin_sinl(x) == __builtin_sinl(x));
        return 0;
    }
" CCMATH_BUILTIN_SIN_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_SIN_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_SIN_CONSTEXPR)
endif ()
