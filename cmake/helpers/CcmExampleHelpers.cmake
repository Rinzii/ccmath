# Example executable registration (showcase-style layout, registry-driven).

include_guard(GLOBAL)

if (NOT DEFINED CCMATH_EXAMPLE_ROOT)
    message(FATAL_ERROR "CCMATH_EXAMPLE_ROOT must be set before including CcmExampleHelpers.cmake")
endif ()

function(ccmath_add_example_target ID)
    set(options)
    set(oneValueArgs MODULE CXX_STANDARD)
    set(multiValueArgs SOURCES COMPILE_DEFINITIONS)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (CCM_MODULE AND NOT CCM_SOURCES)
        set(CCM_SOURCES "${CCMATH_EXAMPLE_ROOT}/src/${ID}.cpp")
    endif ()

    if (NOT CCM_SOURCES)
        message(FATAL_ERROR "ccmath_add_example_target(${ID}): SOURCES or MODULE is required")
    endif ()

    set(_target ccmath-example-${ID})
    add_executable(${_target} ${CCM_SOURCES})
    target_link_libraries(${_target} PRIVATE ccmath::ccmath)
    target_include_directories(${_target} PRIVATE "${CCMATH_EXAMPLE_ROOT}/common" "${CCMATH_EXAMPLE_ROOT}")
    set_property(TARGET ${_target} PROPERTY FOLDER "examples")
    if (CCM_CXX_STANDARD)
        target_compile_features(${_target} PRIVATE cxx_std_${CCM_CXX_STANDARD})
    else ()
        set_property(TARGET ${_target} PROPERTY CXX_STANDARD 17)
    endif ()

    if (CCM_COMPILE_DEFINITIONS)
        target_compile_definitions(${_target} PRIVATE ${CCM_COMPILE_DEFINITIONS})
    endif ()

    set(${_target}_REGISTERED TRUE PARENT_SCOPE)
endfunction()

function(ccmath_apply_example_registry)
    set(_all_targets "")
    foreach (_module IN LISTS CCMATH_EXAMPLE_MODULE_ORDER)
        set(_examples_var CCMATH_EXAMPLE_MODULE_${_module}_EXAMPLES)
        foreach (_example IN LISTS ${_examples_var})
            ccmath_add_example_target(${_example} MODULE ${_module})
            list(APPEND _all_targets ccmath-example-${_example})
        endforeach ()
    endforeach ()

    if (_all_targets)
        add_custom_target(ccmath-example-all)
        add_dependencies(ccmath-example-all ${_all_targets})
        set_property(TARGET ccmath-example-all PROPERTY FOLDER "examples")
    endif ()
endfunction()
