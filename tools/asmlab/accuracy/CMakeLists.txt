cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# Isolated powf_impl candidate accuracy probes (no Google Benchmark dependency).

if (NOT TARGET ccmath)
    message(FATAL_ERROR "asmlab accuracy requires the ccmath interface target")
endif ()

set(_asmlab_acc_src "${CMAKE_CURRENT_SOURCE_DIR}/candidate_accuracy_powf_impl.cpp")

function(asmlab_add_powf_impl_accuracy target_suffix extra_defs extra_includes)
    set(_target "asmlab_candidate_accuracy_powf_impl${target_suffix}")
    add_executable(${_target} "${_asmlab_acc_src}")
    target_link_libraries(${_target} PRIVATE ccmath::ccmath)
    target_compile_features(${_target} PRIVATE cxx_std_17)
    target_include_directories(${_target} PRIVATE
            "${CMAKE_SOURCE_DIR}/include"
            "${CMAKE_SOURCE_DIR}/tests"
            ${extra_includes})
    if (extra_defs)
        target_compile_definitions(${_target} PRIVATE ${extra_defs})
    endif ()
endfunction()

asmlab_add_powf_impl_accuracy("" "" "")

if (ASMLAB_POWF_IMPL_VARIANT_DIR AND ASMLAB_POWF_IMPL_VARIANT_NAME)
    asmlab_add_powf_impl_accuracy(
            "_${ASMLAB_POWF_IMPL_VARIANT_NAME}"
            "ASMLAB_POWF_IMPL_CANDIDATE;ASMLAB_VARIANT_NAME=${ASMLAB_POWF_IMPL_VARIANT_NAME}"
            "${ASMLAB_POWF_IMPL_VARIANT_DIR}")
    message(STATUS "asmlab accuracy: powf_impl candidate '${ASMLAB_POWF_IMPL_VARIANT_NAME}'")
endif ()
