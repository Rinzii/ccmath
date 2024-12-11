include(CheckCXXSourceCompiles)

# Check if __builtin_hypot is supported
check_cxx_source_compiles("
    int main() {
        double x = 3.0, y = 4.0;
        float fr = __builtin_hypotf(x, y);
        double dr = __builtin_hypot(x, y);
        long double lr = __builtin_hypotl(x, y);
        return 0;
    }
" CCMATH_BUILTIN_HYPOT_SUPPORT)

if (CCMATH_BUILTIN_HYPOT_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_HYPOT)
endif ()

# Check if __builtin_hypot can be used as a constexpr in C++17
check_cxx_source_compiles("
    int main() {
        constexpr double x = 3.0, y = 4.0;
        static_assert(__builtin_hypotf(x, y) == __builtin_hypotf(x, y));
        static_assert(__builtin_hypot(x, y) == __builtin_hypot(x, y));
        static_assert(__builtin_hypotl(x, y) == __builtin_hypotl(x, y));
        return 0;
    }
" CCMATH_BUILTIN_HYPOT_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_HYPOT_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_HYPOT_CONSTEXPR)
endif ()
