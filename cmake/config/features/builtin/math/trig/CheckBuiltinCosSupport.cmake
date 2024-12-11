include(CheckCXXSourceCompiles)

# Check if __builtin_cos is supported
check_cxx_source_compiles("
    int main() {
        double x = 4.0;
        float fr = __builtin_cosf(x);
        double dr = __builtin_cos(x);
        long double lr = __builtin_cosl(x);
        return 0;
    }
" CCMATH_BUILTIN_COS_SUPPORT)

if (CCMATH_BUILTIN_COS_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_COS)
endif ()

# Check if __builtin_cos can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 4.0;
        static_assert(__builtin_cosf(x) == __builtin_cosf(x));
        static_assert(__builtin_cos(x) == __builtin_cos(x));
        static_assert(__builtin_cosl(x) == __builtin_cosl(x));
        return 0;
    }
" CCMATH_BUILTIN_COS_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_COS_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_COS_CONSTEXPR)
endif ()
