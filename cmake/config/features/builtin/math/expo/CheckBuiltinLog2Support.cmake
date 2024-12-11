include(CheckCXXSourceCompiles)

# Check if __builtin_log2 is supported
check_cxx_source_compiles("
    int main() {
        double x = 2.0;
        float fr = __builtin_log2f(x);
        double dr = __builtin_log2(x);
        long double lr = __builtin_log2l(x);
        return 0;
    }
" CCMATH_BUILTIN_LOG2_SUPPORT)

if (CCMATH_BUILTIN_LOG2_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_LOG2)
endif ()

# Check if __builtin_log2 can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 2.0;
        static_assert(__builtin_log2f(x) == __builtin_log2f(x));
        static_assert(__builtin_log2(x) == __builtin_log2(x));
        static_assert(__builtin_log2l(x) == __builtin_log2l(x));
        return 0;
    }
" CCMATH_BUILTIN_LOG2_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_LOG2_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_LOG2_CONSTEXPR)
endif ()
