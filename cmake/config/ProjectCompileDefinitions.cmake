include(cmake/config/BuildManifest.cmake)
ccmath_manifest_apply_library_compile_definitions(ccmath)

# ccmath is header-only, so the math compiles in the consumer's translation unit.
# Deterministic mode must stop the compiler from contracting stray a*b+c into a
# hardware FMA (which would diverge between FMA and non-FMA targets). The flag has
# to be INTERFACE so it reaches consumers. MSVC does not contract under the default
# /fp:precise, so no MSVC flag is needed.
if (CCMATH_ENABLE_DETERMINISTIC)
    target_compile_options(ccmath INTERFACE
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-ffp-contract=off>
    )
endif ()

# On MSVC the runtime path calls the system libm (via <math.h>) for the expensive transcendentals,
# which has no foldable __builtin and no SVML, so the generic kernels would otherwise be used and are
# not accurate enough. This is on by default for MSVC. Disabling it keeps the build free of <math.h>
# and falls back to the generic kernels.
option(CCMATH_DISABLE_SYSTEM_MATH "Do not call the system libm on MSVC (keeps the build free of <math.h>)" OFF)
if (CCMATH_DISABLE_SYSTEM_MATH)
    target_compile_definitions(ccmath INTERFACE CCM_CONFIG_DISABLE_SYSTEM_MATH)
endif ()
