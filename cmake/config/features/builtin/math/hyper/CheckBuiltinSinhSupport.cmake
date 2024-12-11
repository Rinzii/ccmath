include(CheckCXXSourceCompiles)

# Check if __builtin_sinh is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_sinhf(x);
        double dr = __builtin_sinh(x);
        long double lr = __builtin_sinhl(x);
        return 0;
    }
" CCMATH_BUILTIN_SINH_SUPPORT)

if (CCMATH_BUILTIN_SINH_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_SINH)
endif ()

# Check if __builtin_sinh can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_sinhf(x) == __builtin_sinhf(x));
        static_assert(__builtin_sinh(x) == __builtin_sinh(x));
        static_assert(__builtin_sinhl(x) == __builtin_sinhl(x));
        return 0;
    }
" CCMATH_BUILTIN_SINH_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_SINH_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_SINH_CONSTEXPR)
endif ()
