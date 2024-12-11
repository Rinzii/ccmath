include(CheckCXXSourceCompiles)

# Check if __builtin_log is supported
check_cxx_source_compiles("
    int main() {
        double x = 2.0;
        float fr = __builtin_logf(x);
        double dr = __builtin_log(x);
        long double lr = __builtin_logl(x);
        return 0;
    }
" CCMATH_BUILTIN_LOG_SUPPORT)

if (CCMATH_BUILTIN_LOG_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_LOG)
endif ()

# Check if __builtin_log can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 2.0;
        static_assert(__builtin_logf(x) == __builtin_logf(x));
        static_assert(__builtin_log(x) == __builtin_log(x));
        static_assert(__builtin_logl(x) == __builtin_logl(x));
        return 0;
    }
" CCMATH_BUILTIN_LOG_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_LOG_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_LOG_CONSTEXPR)
endif ()
