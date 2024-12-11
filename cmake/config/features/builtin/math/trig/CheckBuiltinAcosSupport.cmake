include(CheckCXXSourceCompiles)

# Check if __builtin_acos is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_acosf(x);
        double dr = __builtin_acos(x);
        long double lr = __builtin_acosl(x);
        return 0;
    }
" CCMATH_BUILTIN_ACOS_SUPPORT)

if (CCMATH_BUILTIN_ACOS_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ACOS)
endif ()

# Check if __builtin_acos can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_acosf(x) == __builtin_acosf(x));
        static_assert(__builtin_acos(x) == __builtin_acos(x));
        static_assert(__builtin_acosl(x) == __builtin_acosl(x));
        return 0;
    }
" CCMATH_BUILTIN_ACOS_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_ACOS_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ACOS_CONSTEXPR)
endif ()
