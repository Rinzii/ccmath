# Select and apply a preferred SIMD instruction set for ccmath's in-tree build.
#
# Controlled by CCMATH_SIMD_PREFER (DEFAULT | LOWEST | HIGHEST), declared in
# cmake/config/UserOptions.cmake:
#   DEFAULT  leave whatever the toolchain / -march already selects (no extra flag).
#   LOWEST   target the most portable baseline for the architecture
#            (SSE2 on x86_64, NEON on AArch64).
#   HIGHEST  target the best instruction set in ccmath's supported ladder that the
#            compiler can emit (up to AVX2). AVX-512 is intentionally excluded.
#
# This is opt-in: the default does nothing. When enabled, the chosen flag is
# attached to the ccmath interface target behind a BUILD_INTERFACE generator
# expression, so it affects this build tree (library, tests, examples,
# benchmarks and in-tree consumers that opt in) without leaking into the
# installed / exported package. CMAKE_REQUIRED_FLAGS is also updated so the later
# FMA / SVML feature probes are evaluated at the selected level.

if (NOT DEFINED CCMATH_SIMD_PREFER OR CCMATH_SIMD_PREFER STREQUAL "DEFAULT")
    return()
endif ()

if (NOT CCMATH_SIMD_PREFER STREQUAL "LOWEST" AND NOT CCMATH_SIMD_PREFER STREQUAL "HIGHEST")
    message(WARNING "CCMath: unknown CCMATH_SIMD_PREFER='${CCMATH_SIMD_PREFER}' (expected DEFAULT, LOWEST or HIGHEST). Ignoring.")
    return()
endif ()

if (NOT DEFINED CCMATH_TARGET_NAME)
    set(CCMATH_TARGET_NAME ccmath)
endif ()

include(CheckCXXSourceCompiles)

# Probe whether the compiler accepts ${_flag} and can compile ${_src}. A flag of
# NONE means "no extra flag" (used for baseline rungs, an empty string cannot be
# carried as a list element).
function(_ccmath_probe_isa _flag _src _resultvar)
    set(_saved "${CMAKE_REQUIRED_FLAGS}")
    if (_flag AND NOT _flag STREQUAL "NONE")
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${_flag}")
    endif ()
    check_cxx_source_compiles("${_src}" ${_resultvar})
    set(CMAKE_REQUIRED_FLAGS "${_saved}")
    set(${_resultvar} "${${_resultvar}}" PARENT_SCOPE)
endfunction()

# Representative intrinsic per rung.
set(_src_sse2 [=[
#include <emmintrin.h>
int main() { __m128d v = _mm_set1_pd(1.0); v = _mm_add_pd(v, v); return (int) _mm_cvtsd_f64(v); }
]=])
set(_src_sse42 [=[
#include <immintrin.h>
int main() { __m128 v = _mm_set1_ps(1.0f); v = _mm_dp_ps(v, v, 0xFF); return (int) _mm_cvtss_f32(v); }
]=])
set(_src_avx [=[
#include <immintrin.h>
int main() { __m256 v = _mm256_set1_ps(1.0f); v = _mm256_add_ps(v, v); return (int) _mm256_cvtss_f32(v); }
]=])
set(_src_avx2 [=[
#include <immintrin.h>
int main() { __m256i v = _mm256_set1_epi32(1); v = _mm256_add_epi32(v, v); return _mm256_extract_epi32(v, 0); }
]=])
set(_src_neon [=[
#include <arm_neon.h>
int main() { float32x4_t v = vdupq_n_f32(1.0f); v = vaddq_f32(v, v); return (int) vgetq_lane_f32(v, 0); }
]=])

# Build the rung ladder (low -> high) for this architecture and compiler. Only
# the rung names and flags are kept as lists. The source snippets contain
# semicolons and must never be stored as list elements (they are looked up by
# name as _src_<name> instead).
set(_names "")
set(_flags "")

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64|i[3-6]86|x86)$")
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        # MSVC: SSE2 is the x64 baseline (no flag), and no SSE4 arch switch exists.
        set(_names SSE2 AVX AVX2)
        set(_flags NONE "/arch:AVX" "/arch:AVX2")
    else ()
        # Use the x86-64 microarchitecture levels (matches cmake/coremath): v1 is
        # the SSE2 baseline, v2 ~ SSE4.2, v3 ~ AVX2+FMA. v4 (AVX-512) is excluded.
        # A march level also caps the ISA, overriding any higher -m flag a preset
        # may have set earlier on the command line.
        set(_names SSE2 SSE4.2 AVX2)
        set(_flags "-march=x86-64" "-march=x86-64-v2" "-march=x86-64-v3")
    endif ()
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
    # NEON is the mandatory AArch64 baseline. SVE is intentionally out of scope.
    set(_names NEON)
    set(_flags NONE)
else ()
    message(STATUS "CCMath: CCMATH_SIMD_PREFER=${CCMATH_SIMD_PREFER} has no instruction-set ladder for '${CMAKE_SYSTEM_PROCESSOR}'; leaving toolchain default.")
    return()
endif ()

list(LENGTH _names _count)
math(EXPR _last "${_count} - 1")

# LOWEST: first rung that compiles. HIGHEST: highest rung that compiles.
set(_chosen -1)
foreach (_i RANGE 0 ${_last})
    list(GET _names ${_i} _nm)
    list(GET _flags ${_i} _fl)
    # Look up the matching source snippet by name (e.g. SSE4.2 -> _src_sse42).
    string(TOLOWER "${_nm}" _lc)
    string(REPLACE "." "" _lc "${_lc}")
    set(_sr "${_src_${_lc}}")
    string(MAKE_C_IDENTIFIER "CCMATH_PROBE_ISA_${_nm}" _rv)
    _ccmath_probe_isa("${_fl}" "${_sr}" ${_rv})
    if (${_rv})
        set(_chosen ${_i})
        if (CCMATH_SIMD_PREFER STREQUAL "LOWEST")
            break()
        endif ()
    endif ()
endforeach ()

if (_chosen LESS 0)
    message(WARNING "CCMath: could not select a SIMD instruction set for CCMATH_SIMD_PREFER=${CCMATH_SIMD_PREFER}; leaving toolchain default.")
    return()
endif ()

list(GET _names ${_chosen} _chosen_name)
list(GET _flags ${_chosen} _chosen_flag)

if (_chosen_flag AND NOT _chosen_flag STREQUAL "NONE")
    target_compile_options(${CCMATH_TARGET_NAME} INTERFACE "$<BUILD_INTERFACE:${_chosen_flag}>")
    string(APPEND CMAKE_REQUIRED_FLAGS " ${_chosen_flag}")
    message(STATUS "CCMath: SIMD instruction set (${CCMATH_SIMD_PREFER}) -> ${_chosen_name} (${_chosen_flag})")
else ()
    message(STATUS "CCMath: SIMD instruction set (${CCMATH_SIMD_PREFER}) -> ${_chosen_name} (toolchain baseline)")
endif ()
