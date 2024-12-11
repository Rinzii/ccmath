include(CheckCXXSourceCompiles)

# We only care about the constexpr support for isnan
check_cxx_source_compiles("
    int main() {
        static_assert(__builtin_isnan(0.0 / 0.0));
        return 0;
    }
" CCMATH_BUILTIN_ISNAN_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_ISNAN_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ISNAN_CONSTEXPR)
endif ()
