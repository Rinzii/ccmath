include(CheckCXXSourceCompiles)

# Check if __builtin_asinh is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_asinhf(x);
        double dr = __builtin_asinh(x);
        long double lr = __builtin_asinhl(x);
        return 0;
    }
" CCMATH_BUILTIN_ASINH_SUPPORT)

if (CCMATH_BUILTIN_ASINH_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ASINH)
endif ()

# Check if __builtin_asinh can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_asinhf(x) == __builtin_asinhf(x));
        static_assert(__builtin_asinh(x) == __builtin_asinh(x));
        static_assert(__builtin_asinhl(x) == __builtin_asinhl(x));
        return 0;
    }
" CCMATH_BUILTIN_ASINH_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_ASINH_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ASINH_CONSTEXPR)
endif ()
