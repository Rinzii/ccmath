include(cmake/config/BuildManifest.cmake)
ccmath_manifest_apply_library_compile_definitions(ccmath)

# ccmath is header-only, so the math compiles in the consumer's translation unit.
# Deterministic mode must stop the compiler from contracting stray a*b+c into a
# hardware FMA (which would diverge between FMA and non-FMA targets); the flag has
# to be INTERFACE so it reaches consumers. MSVC does not contract under the default
# /fp:precise, so no MSVC flag is needed.
if (CCMATH_ENABLE_DETERMINISTIC)
    target_compile_options(ccmath INTERFACE
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-ffp-contract=off>
    )
endif ()
