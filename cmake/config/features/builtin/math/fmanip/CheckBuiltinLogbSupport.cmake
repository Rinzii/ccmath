include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
    int main() {
        return __builtin_copysign(1.0, -1.0) +
               __builtin_copysignf(1.0f, -1.0f) +
               __builtin_copysignl(1.0l, -1.0l);
    }
" CCMATH_BUILTIN_COPYSIGN_SUPPORT)

if (CCMATH_BUILTIN_COPYSIGN_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_COPYSIGN)
endif ()

check_cxx_source_compiles("
    int main() {
        static_assert(__builtin_copysign(1.0, -1.0) == -1.0);
        static_assert(__builtin_copysignf(1.0f, -1.0f) == -1.0f);
        static_assert(__builtin_copysignl(1.0l, -1.0l) == -1.0l);
        return 0;
    }
" CCMATH_BUILTIN_COPYSIGN_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_COPYSIGN_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_COPYSIGN_CONSTEXPR)
endif ()
