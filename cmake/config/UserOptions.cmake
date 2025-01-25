# CCMATH_INSTALL:
# Controls whether installation and packaging steps are configured.
# If this is the root project (standalone), default to ON.
# If this project is included as a subproject, default to OFF.
option(CCMATH_INSTALL "Setup install and package steps" ${CCMATH_PROJECT_IS_TOP_LEVEL})

# CCMATH_BUILD_EXAMPLES:
# Build example code demonstrating usage of ccmath.
option(CCMATH_BUILD_EXAMPLES "Build ccmath examples" ${CCMATH_PROJECT_IS_TOP_LEVEL})

# CCMATH_BUILD_TESTS:
# Enable building of ccmath tests. If OFF, tests are skipped. Should only be used by ccmath developers.
option(CCMATH_BUILD_TESTS "Build ccmath tests" ${CCMATH_PROJECT_IS_TOP_LEVEL})

# CCMATH_BUILD_BENCHMARKS:
# Enable building of ccmath benchmarks. If OFF, benchmarks are skipped.
option(CCMATH_BUILD_BENCHMARKS "Build ccmath benchmarks" OFF)

# CCMATH_FIND_GTEST_PACKAGE:
# Look for gtest package rather than downloading it if needed.
option(CCMATH_FIND_GTEST_PACKAGE "Enable finding of gtest package" OFF)

# CCMATH_FIND_GBENCH_PACKAGE:
# Look for google benchmark package rather than downloading it if needed. Should only be used by ccmath developers.
option(CCMATH_FIND_GBENCH_PACKAGE "Enable finding of google benchmark package" OFF)

# CCMATH_ENABLE_EXTENSIONS:
# Enable non-standard extensions that go beyond the <cmath> interface.
option(CCMATH_ENABLE_EXTENSIONS
        "Enable the extended ccmath library that adds additional methods beyond the standard"
        ${CCMATH_PROJECT_IS_TOP_LEVEL})

# CCMATH_DISABLE_ERRNO:
# Disable usage of errno during runtime breaking standard compliance, but may lead to faster evaluation and smaller binaries.
option(CCMATH_DISABLE_ERRNO
        "Disable the use of errno in ccmath during runtime (may lead to faster evaluations but is non-standard)"
        OFF)

# CCMATH_ENABLE_RUNTIME_SIMD:
# Enable runtime SIMD optimizations for faster evaluations at runtime.
option(CCMATH_DISABLE_RUNTIME_SIMD
        "Enable SIMD optimization for runtime evaluation (does not affect compile-time)"
        OFF)

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

# CCMATH_DISABLE_CMAKE_BUILTIN_CHECKS:
# Disable cmakes ability to check for certain builtin functions at the CMake level.
option(CCMATH_DISABLE_CMAKE_BUILTIN_CHECKS
        "Disable the ability for CCMath to check for builtin functions at the CMake level"
        OFF)

# Project-specific development options (Only enable if we are the root project)
include(CMakeDependentOption)

# Enforce aggressive warnings (enabled by default for the root project)
cmake_dependent_option(CCMATH_ENABLE_AGGRESSIVE_WARNINGS
        "Enforce extremely aggressive warnings." ON
        "CCMATH_PROJECT_IS_TOP_LEVEL" ON
)

# Treat warnings as errors (enabled by default for the root project)
cmake_dependent_option(CCMATH_ENABLE_WARNINGS_AS_ERRORS
        "All warnings should be treated as errors." ON
        "CCMATH_PROJECT_IS_TOP_LEVEL" ON
)

# Enable heightened debug information (disabled by default)
cmake_dependent_option(CCMATH_ENABLE_DEBUG_INFO
        "Enable heightened debug information. Warning: this DOES have a performance cost." OFF
        "CCMATH_PROJECT_IS_TOP_LEVEL" ON
)

# Enable sanitizers in build (disabled by default)
cmake_dependent_option(CCMATH_ENABLE_SANITIZERS
        "Enable the usage of sanitizers in the build. Warning: this DOES have a performance cost." OFF
        "CCMATH_PROJECT_IS_TOP_LEVEL" ON
)
