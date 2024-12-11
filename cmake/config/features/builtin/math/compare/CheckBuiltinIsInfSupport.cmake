include(CheckCXXSourceCompiles)

# We only care about the constexpr support for isinf
check_cxx_source_compiles("
    int main() {
        static_assert(__builtin_isinf(1.0 / 0.0));
        return 0;
    }
" CCMATH_BUILTIN_ISINF_CONSTEXPR_SUPPORT)

if (CCMATH_BUILTIN_ISINF_CONSTEXPR_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_ISINF_CONSTEXPR)
endif ()
