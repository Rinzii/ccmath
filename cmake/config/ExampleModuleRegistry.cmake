# Declarative example registry.
# Add an example: drop examples/src/<module>/<id>.cpp and append <id> below.

set(CCMATH_EXAMPLE_MODULE_ORDER smoke basic trig power expo nearest compare)

set(CCMATH_EXAMPLE_MODULE_smoke_EXAMPLES constexpr_smoke)
set(CCMATH_EXAMPLE_MODULE_basic_EXAMPLES constexpr_abs signed_magnitude)
set(CCMATH_EXAMPLE_MODULE_trig_EXAMPLES unit_circle)
set(CCMATH_EXAMPLE_MODULE_power_EXAMPLES hypot_distance)
set(CCMATH_EXAMPLE_MODULE_expo_EXAMPLES decibel_gain)
set(CCMATH_EXAMPLE_MODULE_nearest_EXAMPLES rounding_edge_cases)
set(CCMATH_EXAMPLE_MODULE_compare_EXAMPLES fp_classes)
