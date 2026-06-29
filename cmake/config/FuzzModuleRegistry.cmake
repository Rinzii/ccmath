# Declarative fuzz module registry.
# Add a family: drop fuzzing/src/math/<module>_fuzz.cpp and append MODULE below.

set(CCMATH_FUZZ_MODULE_ORDER
        basic
        compare
        expo
        fmanip
        nearest
        power
        trig
        misc
)
