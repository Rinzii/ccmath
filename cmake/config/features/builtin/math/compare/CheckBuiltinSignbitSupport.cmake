include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
    int main() {
        return __builtin_signbit(1.0) +
               __builtin_signbitf(1.0f) +
               __builtin_signbitl(1.0l);
    }
" CCMATH_BUILTIN_SIGNBIT_SUPPORT)

if (CCMATH_BUILTIN_SIGNBIT_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_SIGNBIT)
endif ()

check_cxx_source_compiles("
    int main() {
        static_assert(__builtin_signbit(-1.0));
        static_assert(__builtin_signbitf(-1.0f));
        static_assert(__builtin_signbitl(-1.0l));
        return 0;
    }
" CCMATH_BUILTIN_SIGNBIT_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_SIGNBIT_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_SIGNBIT_CONSTEXPR)
endif ()
