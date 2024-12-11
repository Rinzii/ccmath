include(CheckCXXSourceCompiles)

# Check if __builtin_atanh is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_atanhf(x);
        double dr = __builtin_atanh(x);
        long double lr = __builtin_atanhl(x);
        return 0;
    }
" CCMATH_BUILTIN_ATANH_SUPPORT)

if (CCMATH_BUILTIN_ATANH_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ATANH)
endif ()

# Check if __builtin_atanh can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_atanhf(x) == __builtin_atanhf(x));
        static_assert(__builtin_atanh(x) == __builtin_atanh(x));
        static_assert(__builtin_atanhl(x) == __builtin_atanhl(x));
        return 0;
    }
" CCMATH_BUILTIN_ATANH_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_ATANH_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ATANH_CONSTEXPR)
endif ()
