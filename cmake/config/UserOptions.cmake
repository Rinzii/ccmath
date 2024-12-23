# CCMATH_INSTALL:
# Controls whether installation and packaging steps are configured.
# If this is the root project (standalone), default to ON.
# If this project is included as a subproject, default to OFF.
option(CCMATH_INSTALL "Setup install and package steps" ${is_root_project})

# CCMATH_BUILD_TESTS:
# Enable building of ccmath tests. If OFF, tests are skipped. Should only be used by ccmath developers.
option(CCMATH_BUILD_TESTS "Build ccmath tests" ${is_root_project})

# CCMATH_FIND_GTEST_PACKAGE:
# Look for gtest package rather than downloading it if needed.
option(CCMATH_FIND_GTEST_PACKAGE "Enable finding of gtest package" OFF)

# CCMATH_BUILD_BENCHMARKS:
# Enable building of ccmath benchmarks. If OFF, benchmarks are skipped.
option(CCMATH_BUILD_BENCHMARKS "Build ccmath benchmarks" ${is_root_project})

# CCMATH_FIND_GBENCH_PACKAGE:
# Look for google benchmark package rather than downloading it if needed. Should only be used by ccmath developers.
option(CCMATH_FIND_GBENCH_PACKAGE "Enable finding of google benchmark package" OFF)

# CCMATH_BUILD_EXAMPLES:
# Build example code demonstrating usage of ccmath.
option(CCMATH_BUILD_EXAMPLES "Build ccmath examples" ${is_root_project})

# CCMATH_ENABLE_USER_DEFINED_OPTIMIZATION_MACROS:
# Allow users to supply their own optimization macros rather than CCMath defining them itself.
option(CCMATH_ENABLE_USER_DEFINED_OPTIMIZATION_MACROS
        "Enable user defined optimization macros instead of having ccmath define its own"
        OFF)

# CCMATH_ENABLE_EXTENSIONS:
# Enable non-standard extensions that go beyond the <cmath> interface.
option(CCMATH_ENABLE_EXTENSIONS
        "Enable the extended ccmath library that adds additional methods beyond the standard"
        OFF)

# CCMATH_DISABLE_ERRNO:
# Disable usage of errno during runtime breaking standard compliance, but may lead to faster evaluation and smaller binaries.
option(CCMATH_DISABLE_ERRNO
        "Disable the use of errno in ccmath during runtime (may lead to faster evaluations but is non-standard)"
        OFF)

# CCMATH_ENABLE_RUNTIME_SIMD:
# Enable runtime SIMD optimizations for faster evaluations at runtime.
option(CCMATH_ENABLE_RUNTIME_SIMD
        "Enable SIMD optimization for runtime evaluation (does not affect compile-time)"
        ON)

# CCMATH_DISABLE_SVML_USAGE:
# Disable the use of SVML (Short Vector Math Library) if supported by the compiler.
option(CCMATH_DISABLE_SVML_USAGE
        "Disable the use of SVML functions in ccmath (if supported by compiler)"
        OFF)

# CCMATH_DISABLE_CMAKE_FEATURE_CHECKS:
# Disable cmakes ability to check for certain features at the CMake level.
option(CCMATH_DISABLE_CMAKE_FEATURE_CHECKS
        "Disable the ability for CCMath to check for certain features at the CMake level"
        OFF)

# CCMATH_DISABLE_CMAKE_SIMD_CHECKS:
# Disable cmakes ability to check for certain SIMD features at the CMake level.
option(CCMATH_DISABLE_CMAKE_SIMD_CHECKS
        "Disable the ability for CCMath to check for SIMD features at the CMake level"
        OFF)

# CCMATH_DISABLE_CMAKE_BUILTIN_CHECKS:
# Disable cmakes ability to check for certain builtin functions at the CMake level.
option(CCMATH_DISABLE_CMAKE_BUILTIN_CHECKS
        "Disable the ability for CCMath to check for builtin functions at the CMake level"
        OFF)

# CCMATH_STRICT_WARNINGS:
# Enable strict warning handling, turning certain warnings into errors.
# This might be intrusive for downstream users, so disable if we are not the root project.
option(CCMATH_STRICT_WARNINGS
        "Enable strict warnings and treat certain warnings as errors. May be intrusive downstream."
        ${is_root_project})
