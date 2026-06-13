cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# Isolated impl-kernel benchmarks for asmlab (original and candidate variants).

if (NOT TARGET ccmath)
    message(FATAL_ERROR "asmlab bench requires the ccmath interface target")
endif ()
if (NOT TARGET benchmark::benchmark)
    message(FATAL_ERROR "asmlab bench requires Google Benchmark (CCMATH_BUILD_BENCHMARKS=ON)")
endif ()

set(_asmlab_bench_src "${CMAKE_CURRENT_SOURCE_DIR}/powf_impl_bench.cpp")

function(asmlab_add_powf_impl_bench target_suffix extra_defs extra_includes)
    set(_target "asmlab_bench_powf_impl${target_suffix}")
    add_executable(${_target} "${_asmlab_bench_src}")
    target_link_libraries(${_target} PRIVATE ccmath::ccmath benchmark::benchmark)
    target_compile_features(${_target} PRIVATE cxx_std_17)
    target_include_directories(${_target} PRIVATE
            "${CMAKE_SOURCE_DIR}/include"
            ${extra_includes})
    if (extra_defs)
        target_compile_definitions(${_target} PRIVATE ${extra_defs})
    endif ()
endfunction()

# Original production impl path (isolated TU, not public powf wrapper).
asmlab_add_powf_impl_bench("" "" "")

# Golden calibration bench (Horner vs Estrin). No ccmath link.
add_executable(asmlab_bench_golden_poly "${CMAKE_CURRENT_SOURCE_DIR}/golden_poly_bench.cpp")
target_link_libraries(asmlab_bench_golden_poly PRIVATE benchmark::benchmark)
target_compile_features(asmlab_bench_golden_poly PRIVATE cxx_std_17)
target_include_directories(asmlab_bench_golden_poly PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/tools/asmlab")

# Optional candidate variant bench: pass -DASMLAB_POWF_IMPL_VARIANT_DIR=... at configure time.
if (ASMLAB_POWF_IMPL_VARIANT_DIR AND ASMLAB_POWF_IMPL_VARIANT_NAME)
    asmlab_add_powf_impl_bench(
            "_${ASMLAB_POWF_IMPL_VARIANT_NAME}"
            "ASMLAB_POWF_IMPL_CANDIDATE;ASMLAB_VARIANT_NAME=${ASMLAB_POWF_IMPL_VARIANT_NAME}"
            "${ASMLAB_POWF_IMPL_VARIANT_DIR}")
    message(STATUS "asmlab bench: powf_impl candidate '${ASMLAB_POWF_IMPL_VARIANT_NAME}'")
endif ()
