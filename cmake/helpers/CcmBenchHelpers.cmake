# Google Benchmark target registration (LLVM-libc style: shared harness, per-function drivers).

include_guard(GLOBAL)

if (NOT DEFINED CCMATH_BENCH_ROOT)
    message(FATAL_ERROR "CCMATH_BENCH_ROOT must be set before including CcmBenchHelpers.cmake")
endif ()

function(ccmath_bench_module_enabled MODULE OUT_VAR)
    set(_option_var CCMATH_BENCH_MODULE_${MODULE}_OPTION)
    if (NOT DEFINED ${_option_var})
        message(FATAL_ERROR "ccmath_bench_module_enabled(${MODULE}): missing ${_option_var}")
    endif ()

    if (CCM_BENCH_ALL)
        set(${OUT_VAR} TRUE PARENT_SCOPE)
        return ()
    endif ()

    if (${${_option_var}})
        set(${OUT_VAR} TRUE PARENT_SCOPE)
    else ()
        set(${OUT_VAR} FALSE PARENT_SCOPE)
    endif ()
endfunction()

function(ccmath_add_bench_target FUNCTION)
    set(options)
    set(oneValueArgs MODULE)
    set(multiValueArgs SOURCES COMPILE_DEFINITIONS)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (CCM_MODULE AND NOT CCM_SOURCES)
        set(CCM_SOURCES "${CCMATH_BENCH_ROOT}/src/math/${CCM_MODULE}/${FUNCTION}.bench.cpp")
    endif ()

    if (NOT CCM_SOURCES)
        message(FATAL_ERROR "ccmath_add_bench_target(${FUNCTION}): SOURCES or MODULE is required")
    endif ()

    set(_target ccm_benchmark_${FUNCTION})
    add_executable(${_target} ${CCM_SOURCES})
    target_link_libraries(${_target} PRIVATE ccmath::ccmath benchmark::benchmark)
    target_include_directories(${_target} PRIVATE "${CCMATH_BENCH_ROOT}/shared" "${CCMATH_BENCH_ROOT}")
    target_compile_features(${_target} PRIVATE cxx_std_17)

    if (CCM_COMPILE_DEFINITIONS)
        target_compile_definitions(${_target} PRIVATE ${CCM_COMPILE_DEFINITIONS})
    endif ()

    set(${_target}_REGISTERED TRUE PARENT_SCOPE)
endfunction()

function(ccmath_apply_bench_registry)
    if (NOT TARGET benchmark::benchmark)
        message(FATAL_ERROR "ccmath_apply_bench_registry requires benchmark::benchmark")
    endif ()

    set(_all_targets "")
    foreach (_module IN LISTS CCMATH_BENCH_MODULE_ORDER)
        ccmath_bench_module_enabled(${_module} _enabled)
        if (NOT _enabled)
            continue ()
        endif ()

        set(_functions_var CCMATH_BENCH_MODULE_${_module}_FUNCTIONS)
        foreach (_function IN LISTS ${_functions_var})
            ccmath_add_bench_target(${_function} MODULE ${_module})
            list(APPEND _all_targets ccm_benchmark_${_function})
        endforeach ()

        if (_module STREQUAL "power")
            if (EXISTS "${CMAKE_SOURCE_DIR}/tools/asmlab/bench/CMakeLists.txt")
                add_subdirectory("${CMAKE_SOURCE_DIR}/tools/asmlab/bench" "${CMAKE_BINARY_DIR}/asmlab_bench")
            endif ()
            if (EXISTS "${CMAKE_SOURCE_DIR}/tools/asmlab/accuracy/CMakeLists.txt")
                add_subdirectory("${CMAKE_SOURCE_DIR}/tools/asmlab/accuracy" "${CMAKE_BINARY_DIR}/asmlab_accuracy")
            endif ()
        endif ()
    endforeach ()

    if (_all_targets)
        add_custom_target(ccm_benchmark_all)
        add_dependencies(ccm_benchmark_all ${_all_targets})
    endif ()
endfunction()
