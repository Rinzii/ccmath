include(CheckCXXSourceCompiles)

# Check if __builtin_tanh is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_tanhf(x);
        double dr = __builtin_tanh(x);
        long double lr = __builtin_tanhl(x);
        return 0;
    }
" CCMATH_BUILTIN_TANH_SUPPORT)

if (CCMATH_BUILTIN_TANH_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_TANH)
endif ()

# Check if __builtin_tanh can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_tanhf(x) == __builtin_tanhf(x));
        static_assert(__builtin_tanh(x) == __builtin_tanh(x));
        static_assert(__builtin_tanhl(x) == __builtin_tanhl(x));
        return 0;
    }
" CCMATH_BUILTIN_TANH_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_TANH_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_TANH_CONSTEXPR)
endif ()
