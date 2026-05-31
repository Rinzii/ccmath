include(CheckCXXSourceCompiles)

# Check if __builtin_fma is supported
check_cxx_source_compiles("
    int main() {
        double x = 1.0, y = 2.0, z = 3.0;
        float fr = __builtin_fmaf(x, y, z);
        double dr = __builtin_fma(x, y, z);
        long double lr = __builtin_fmal(x, y, z);
        return 0;
    }
" CCMATH_BUILTIN_FMA_SUPPORT)

if (CCMATH_BUILTIN_FMA_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_FMA)
endif ()

# Check if __builtin_fma can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 1.0, y = 2.0, z = 3.0;
        static_assert(__builtin_fmaf(x, y, z) == __builtin_fmaf(x, y, z));
        static_assert(__builtin_fma(x, y, z) == __builtin_fma(x, y, z));
        static_assert(__builtin_fmal(x, y, z) == __builtin_fmal(x, y, z));
        return 0;
    }
" CCMATH_BUILTIN_FMA_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_FMA_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_FMA_CONSTEXPR)
endif ()
