include(CheckCXXSourceCompiles)

# Check if __builtin_atan is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_atanf(x);
        double dr = __builtin_atan(x);
        long double lr = __builtin_atanl(x);
        return 0;
    }
" CCMATH_BUILTIN_ATAN_SUPPORT)

if (CCMATH_BUILTIN_ATAN_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ATAN)
endif ()

# Check if __builtin_atan can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_atanf(x) == __builtin_atanf(x));
        static_assert(__builtin_atan(x) == __builtin_atan(x));
        static_assert(__builtin_atanl(x) == __builtin_atanl(x));
        return 0;
    }
" CCMATH_BUILTIN_ATAN_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_ATAN_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ATAN_CONSTEXPR)
endif ()
