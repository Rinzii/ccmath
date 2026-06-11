# Declarative unit-test module registry.
# Add a new public math family: drop *_test.cpp under tests/src/math/<module>/ and
# append MODULE TARGET pair below (LLVM-libc test/src layout).

set(CCMATH_UNIT_MODULE_basic ccmath-simple-basic)
set(CCMATH_UNIT_MODULE_basic_EXCLUDE
        src/math/basic/fma_constexpr_upward_config_test.cpp)
set(CCMATH_UNIT_MODULE_compare ccmath-simple-compare)
set(CCMATH_UNIT_MODULE_expo ccmath-simple-exponential)
set(CCMATH_UNIT_MODULE_fmanip ccmath-simple-fmanip)
set(CCMATH_UNIT_MODULE_nearest ccmath-simple-nearest)
set(CCMATH_UNIT_MODULE_power ccmath-simple-power)
set(CCMATH_UNIT_MODULE_trig ccmath-simple-trigonometric)
set(CCMATH_UNIT_MODULE_ext ccmath-simple-ext)
set(CCMATH_UNIT_MODULE_ext_EXCLUDE
        src/ext/constexpr_smoke_test.cpp)

set(CCMATH_UNIT_MATH_MODULE_ORDER
        basic
        compare
        expo
        fmanip
        nearest
        power
        trig
)
