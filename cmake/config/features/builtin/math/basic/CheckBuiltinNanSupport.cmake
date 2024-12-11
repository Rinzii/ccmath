include(CheckCXXSourceCompiles)

# Check if __builtin_nan is supported
check_cxx_source_compiles("
    int main() {
        float fr = __builtin_nanf();
        double dr = __builtin_nan();
        long double lr = __builtin_nanl();
        return 0;
    }
" CCMATH_BUILTIN_NAN_SUPPORT)

if (CCMATH_BUILTIN_NAN_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_NAN)
endif ()

# Check if __builtin_nan can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr float fr = __builtin_nanf();
        constexpr double dr = __builtin_nan();
        constexpr long double lr = __builtin_nanl();
        static_assert(fr != fr);
        static_assert(dr != dr);
        static_assert(lr != lr);
        return 0;
    }
" CCMATH_BUILTIN_NAN_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_NAN_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_NAN_CONSTEXPR)
endif ()
