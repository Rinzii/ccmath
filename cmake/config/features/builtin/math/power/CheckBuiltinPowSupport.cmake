include(CheckCXXSourceCompiles)

# Check if __builtin_pow is supported
check_cxx_source_compiles("
    int main() {
        double x = 3.0, y = 2.0;
        float fr = __builtin_powf(x, y);
        double dr = __builtin_pow(x, y);
        long double lr = __builtin_powl(x, y);
        return 0;
    }
" CCMATH_BUILTIN_POW_SUPPORT)

if (CCMATH_BUILTIN_POW_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_POW)
endif ()

# Check if __builtin_pow can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 3.0, y = 2.0;
        static_assert(__builtin_powf(x, y) == __builtin_powf(x, y));
        static_assert(__builtin_pow(x, y) == __builtin_pow(x, y));
        static_assert(__builtin_powl(x, y) == __builtin_powl(x, y));
        return 0;
    }
" CCMATH_BUILTIN_POW_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_POW_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_POW_CONSTEXPR)
endif ()
