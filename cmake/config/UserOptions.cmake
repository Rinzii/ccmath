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

# CCMATH_BUILD_SIMPLE_TESTS:
# Build the lightweight unit/regression suite intended for every developer and CI run.
option(CCMATH_BUILD_SIMPLE_TESTS "Build the lightweight ccmath test project" ${CCMATH_PROJECT_IS_TOP_LEVEL})

# CCMATH_BUILD_RIGOROUS_TESTS:
# Build the heavier validation/oracle suite intended for post-merge, nightly, or release validation.
option(CCMATH_BUILD_RIGOROUS_TESTS "Build the rigorous ccmath validation project" OFF)

# CCMATH_ENABLE_MPFR_TESTS:
# Enable MPFR-backed oracle validation binaries when MPFR and GMP are available.
option(CCMATH_ENABLE_MPFR_TESTS "Enable MPFR-backed rigorous oracle executables" OFF)

# CCMATH_ENABLE_COREMATH_TESTS:
# Enable CORE-MATH-backed oracle validation binaries when CORE-MATH is discoverable.
option(CCMATH_ENABLE_COREMATH_TESTS "Enable CORE-MATH-backed rigorous oracle executables" OFF)

# CCMATH_COREMATH_ROOT:
# Optional install prefix for a pre-built CORE-MATH oracle library (include + lib).
set(CCMATH_COREMATH_ROOT "" CACHE PATH "Install prefix for CORE-MATH oracle library")

# CCMATH_FETCH_COREMATH:
# Last-resort FetchContent build when prefix/pkg-config discovery fails.
option(CCMATH_FETCH_COREMATH "Fetch and build CORE-MATH sources when not found on the system" OFF)

# CCMATH_ENABLE_SEARCH_TOOLS:
# Build standalone adversarial-search tools (worst-case input hunting).
option(CCMATH_ENABLE_SEARCH_TOOLS "Build standalone adversarial-search tools (worst-case input hunting)" OFF)

# CCMATH_ENABLE_CROSS_LIBM_TESTS:
# Build optional cross-libm differential pow validation tools.
option(CCMATH_ENABLE_CROSS_LIBM_TESTS "Build optional cross-libm pow differential tools" OFF)

# CCMATH_TEST_DISABLE_RUNTIME_BUILTIN:
# Disable runtime __builtin_pow dispatch in pow_rt for validation builds.
option(CCMATH_TEST_DISABLE_RUNTIME_BUILTIN "Disable runtime builtin pow in validation builds" OFF)

# CCMATH_TEST_DISABLE_FMA:
# Force non-FMA generic pow kernel branches in validation builds.
option(CCMATH_TEST_DISABLE_FMA "Disable FMA pow kernel branches in validation builds" OFF)

# CCMATH_TEST_FORCE_FMA:
# Request FMA-capable compilation for validation builds when supported.
option(CCMATH_TEST_FORCE_FMA "Force FMA-capable compilation for validation builds" OFF)

# CCMATH_BUILD_BENCHMARKS:
# Enable building of ccmath benchmarks. If OFF, benchmarks are skipped.
option(CCMATH_BUILD_BENCHMARKS "Build ccmath benchmarks" OFF)

# CCMATH_BUILD_SHOWCASE:
# Build the external library comparison showcase (GCEM first, MPFR accuracy, gbench perf).
option(CCMATH_BUILD_SHOWCASE "Build ccmath showcase comparisons" OFF)

# CCMATH_SHOWCASE_ENABLE_GCEM:
# Fetch and enable the GCEM backend in showcase targets.
option(CCMATH_SHOWCASE_ENABLE_GCEM "Enable GCEM in showcase comparisons" ${CCMATH_PROJECT_IS_TOP_LEVEL})

# CCMATH_SHOWCASE_ENABLE_MPFR:
# Build MPFR-backed showcase accuracy executables when MPFR is available.
option(CCMATH_SHOWCASE_ENABLE_MPFR "Enable MPFR accuracy in showcase comparisons" ${CCMATH_PROJECT_IS_TOP_LEVEL})

# CCMATH_BUILD_FUZZING:
# Build libFuzzer targets (Clang only). Couples fuzzer with ASan and UBSan.
option(CCMATH_BUILD_FUZZING "Build libFuzzer fuzz targets (Clang only)" OFF)

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

include(cmake/config/BuildManifest.cmake)
ccmath_manifest_declare_library_options()

# CCMATH_DISABLE_SVML_USAGE:
# Disable the use of SVML (Short Vector Math Library) if supported by the compiler.
option(CCMATH_DISABLE_SVML_USAGE
        "Disable the use of SVML functions in ccmath (if supported by compiler)"
        OFF)

# CCMATH_SIMD_PREFER:
# Opt-in: choose which SIMD instruction set ccmath builds compile for.
#   DEFAULT - use whatever the toolchain / -march already selects (no extra flag).
#   LOWEST  - the most portable baseline for the architecture (SSE2 on x86_64, NEON on AArch64).
#   HIGHEST - the best instruction set in ccmath's supported ladder the compiler can emit (up to AVX2; AVX-512 excluded).
# Defaults to DEFAULT (does nothing); downstream consumers may enable or disable it.
set(CCMATH_SIMD_PREFER "DEFAULT" CACHE STRING
        "Preferred SIMD instruction set for ccmath builds: DEFAULT, LOWEST or HIGHEST")
set_property(CACHE CCMATH_SIMD_PREFER PROPERTY STRINGS DEFAULT LOWEST HIGHEST)

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

option(CCMATH_ENABLE_AGGRESSIVE_WARNINGS
        "Enforce extremely aggressive warnings."
        OFF
)

option(CCMATH_ENABLE_WARNINGS_AS_ERRORS
        "All warnings should be treated as errors."
        OFF
)

# Project-specific development options (Only enable if we are the root project)
include(CMakeDependentOption)

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
