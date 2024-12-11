include(CheckCXXSourceCompiles)

# Check if __builtin_bit_cast is supported
check_cxx_source_compiles("
    #include <type_traits>

    int main() {
        struct A { int x; };
        struct B { int y; };
        B b = __builtin_bit_cast(B, A{42});
        return 0;
    }
" CCMATH_BUILTIN_BIT_CAST_SUPPORT)

if (CCMATH_BUILTIN_BIT_CAST_SUPPORT)
  add_compile_definitions(CCM_CONFIG_HAS_BUILTIN_BIT_CAST)
else ()
  message(FATAL_ERROR "
  CCMath: __builtin_bit_cast is not supported by the compiler.
  CCmath MANDATES you have a compiler that supports __builtin_bit_cast!
")
endif ()
