# libFuzzer target registration (LLVM-libc style: shared harness, per-module drivers).
#
# Requires CCMATH_FUZZ_ROOT and CCMATH_FUZZ_COMPILE_FLAGS / CCMATH_FUZZ_LINK_FLAGS
# from ccmath_detect_libfuzzer() before include.

include_guard(GLOBAL)

if (NOT DEFINED CCMATH_FUZZ_ROOT)
    message(FATAL_ERROR "CCMATH_FUZZ_ROOT must be set before including CcmFuzzHelpers.cmake")
endif ()

set(CCMATH_FUZZ_SHARED_SEED_DIR "${CCMATH_FUZZ_ROOT}/seeds/common")
set(CCMATH_FUZZ_DICT "${CCMATH_FUZZ_ROOT}/dicts/ccmath.dict")

function(ccmath_fuzz_default_source MODULE OUT_VAR)
    if (MODULE STREQUAL "ext")
        set(_source "${CCMATH_FUZZ_ROOT}/src/ext/${MODULE}_fuzz.cpp")
    elseif (MODULE MATCHES "^internal/")
        set(_source "${CCMATH_FUZZ_ROOT}/src/math/${MODULE}/${MODULE}_fuzz.cpp")
    else ()
        set(_source "${CCMATH_FUZZ_ROOT}/src/math/${MODULE}_fuzz.cpp")
    endif ()
    set(${OUT_VAR} ${_source} PARENT_SCOPE)
endfunction()

function(ccmath_ensure_fuzz_seeds OUT_DIR)
    if (NOT EXISTS "${OUT_DIR}/zero.bin")
        file(MAKE_DIRECTORY "${OUT_DIR}")
        find_program(_ccmath_fuzz_python3 NAMES python3 python REQUIRED)
        execute_process(
                COMMAND "${_ccmath_fuzz_python3}"
                "${CCMATH_FUZZ_ROOT}/scripts/generate_seeds.py"
                "${OUT_DIR}"
                RESULT_VARIABLE _seed_result)
        if (NOT _seed_result EQUAL 0)
            message(FATAL_ERROR "Failed to generate fuzz seeds in ${OUT_DIR}")
        endif ()
    endif ()
endfunction()

function(ccmath_add_fuzz_target TARGET)
    set(options)
    set(oneValueArgs MODULE)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (CCM_MODULE AND NOT CCM_SOURCES)
        ccmath_fuzz_default_source(${CCM_MODULE} CCM_SOURCES)
    endif ()

    if (NOT CCM_SOURCES)
        message(FATAL_ERROR "ccmath_add_fuzz_target(${TARGET}): SOURCES or MODULE is required")
    endif ()

    add_executable(${TARGET} ${CCM_SOURCES})
    target_link_libraries(${TARGET} PRIVATE ccmath::ccmath)
    target_include_directories(${TARGET} PRIVATE "${CCMATH_FUZZ_ROOT}/shared")
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    target_compile_options(${TARGET} PRIVATE ${CCMATH_FUZZ_COMPILE_FLAGS} -Wno-everything)
    if (CCMATH_FUZZ_GENERIC)
        target_compile_definitions(${TARGET} PRIVATE CCMATH_FUZZ_GENERIC)
    endif ()
    target_link_options(${TARGET} PRIVATE ${CCMATH_FUZZ_LINK_FLAGS})

    set(_corpus_dir "${CMAKE_CURRENT_BINARY_DIR}/corpus/${TARGET}")
    set(_seed_dir "${CMAKE_CURRENT_BINARY_DIR}/seeds/${TARGET}")
    set(_artifact_dir "${CMAKE_CURRENT_BINARY_DIR}/artifacts/${TARGET}/")
    set(_common_seed_dir "${CMAKE_CURRENT_BINARY_DIR}/seeds/common")

    if (NOT TARGET ccmath-fuzz-seeds)
        ccmath_ensure_fuzz_seeds("${_common_seed_dir}")
        if (EXISTS "${CCMATH_FUZZ_SHARED_SEED_DIR}")
            file(GLOB _shared_seeds CONFIGURE_DEPENDS "${CCMATH_FUZZ_SHARED_SEED_DIR}/*.bin")
            foreach (_seed IN LISTS _shared_seeds)
                get_filename_component(_seed_name "${_seed}" NAME)
                configure_file("${_seed}" "${_common_seed_dir}/${_seed_name}" COPYONLY)
            endforeach ()
        endif ()
        add_custom_target(ccmath-fuzz-seeds)
    endif ()

    file(MAKE_DIRECTORY "${_seed_dir}")
    file(GLOB _common_seed_files CONFIGURE_DEPENDS "${_common_seed_dir}/*.bin")
    foreach (_seed IN LISTS _common_seed_files)
        get_filename_component(_seed_name "${_seed}" NAME)
        configure_file("${_seed}" "${_seed_dir}/${_seed_name}" COPYONLY)
    endforeach ()
    add_dependencies(${TARGET} ccmath-fuzz-seeds)

    add_test(
            NAME ${TARGET}-smoke
            COMMAND ${CMAKE_COMMAND}
            -DCORPUS_DIR=${_corpus_dir}
            -DSEED_DIR=${_seed_dir}
            -DARTIFACT_PREFIX=${_artifact_dir}
            -DFUZZER=$<TARGET_FILE:${TARGET}>
            -DCCMATH_FUZZ_DICT=${CCMATH_FUZZ_DICT}
            -P ${CCMATH_FUZZ_ROOT}/cmake/run_fuzz_smoke.cmake)
    set_tests_properties(${TARGET}-smoke PROPERTIES TIMEOUT 300 LABELS "fuzz")

    add_custom_target(${TARGET}-campaign
            COMMAND ${CMAKE_COMMAND}
            -DFUZZER=$<TARGET_FILE:${TARGET}>
            -DCORPUS_DIR=${_corpus_dir}
            -DSEED_DIR=${_seed_dir}
            -DARTIFACT_PREFIX=${_artifact_dir}
            -DCCMATH_FUZZ_DICT=${CCMATH_FUZZ_DICT}
            -P ${CCMATH_FUZZ_ROOT}/cmake/run_fuzz_campaign.cmake
            USES_TERMINAL
            COMMENT "Running libFuzzer campaign for ${TARGET}")
    add_dependencies(${TARGET}-campaign ${TARGET})
endfunction()

function(ccmath_add_fuzz_module TARGET)
    set(options)
    set(oneValueArgs MODULE)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CCM_MODULE)
        message(FATAL_ERROR "ccmath_add_fuzz_module(${TARGET}): MODULE is required")
    endif ()

    if (CCM_SOURCES)
        ccmath_add_fuzz_target(${TARGET} MODULE ${CCM_MODULE} SOURCES ${CCM_SOURCES})
    else ()
        ccmath_add_fuzz_target(${TARGET} MODULE ${CCM_MODULE})
    endif ()
endfunction()
