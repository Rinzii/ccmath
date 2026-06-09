# Optional proof-tool discovery for tests/rigorous proof smoke targets.

function(ccmath_find_optional_proof_tools)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    find_program(CCMATH_GAPPA_EXECUTABLE gappa)
    find_program(CCMATH_SOLLYA_EXECUTABLE sollya)
endfunction()

function(ccmath_missing_optional_tools OUT_VAR)
    ccmath_find_optional_proof_tools()
    set(_missing "")
    if (NOT Python3_Interpreter_FOUND AND NOT Python3_EXECUTABLE)
        list(APPEND _missing "Python3")
    endif ()
    if (NOT CCMATH_GAPPA_EXECUTABLE)
        list(APPEND _missing "gappa")
    endif ()
    if (NOT CCMATH_SOLLYA_EXECUTABLE)
        list(APPEND _missing "sollya")
    endif ()
    set(${OUT_VAR} "${_missing}" PARENT_SCOPE)
endfunction()

function(ccmath_warn_missing_optional_tools CONTEXT)
    ccmath_missing_optional_tools(_missing)
    if (_missing)
        string(REPLACE ";" ", " _missing_text "${_missing}")
        message(WARNING "CCMath ${CONTEXT}: ${_missing_text} not found; target skipped")
    endif ()
endfunction()

function(ccmath_add_pow_proof_smoke_target)
    ccmath_missing_optional_tools(_missing)
    ccmath_warn_missing_optional_tools("pow proof smoke")

    set(_script ${CMAKE_CURRENT_SOURCE_DIR}/../proofs/math/power/proof_freshness_check.py)
    set(_workdir ${CMAKE_CURRENT_SOURCE_DIR}/..)

    if (_missing)
        string(REPLACE ";" ", " _missing_text "${_missing}")
        add_custom_target(ccmath-rigorous-pow-proof-smoke
                COMMAND ${CMAKE_COMMAND} -E echo "error: ${_missing_text} not found"
                COMMAND ${CMAKE_COMMAND} -E false
                COMMENT "pow proof smoke (${_missing_text} missing)")
    else ()
        add_custom_target(ccmath-rigorous-pow-proof-smoke
                COMMAND ${Python3_EXECUTABLE} ${_script} --smoke-only --require-tools
                WORKING_DIRECTORY ${_workdir}
                COMMENT "pow proof smoke")
    endif ()
endfunction()

function(ccmath_add_pow_proof_ulp_aspiration_target)
    ccmath_missing_optional_tools(_missing)
    ccmath_warn_missing_optional_tools("pow proof ULP aspiration")

    set(_script ${CMAKE_CURRENT_SOURCE_DIR}/../proofs/math/power/proof_freshness_check.py)
    set(_workdir ${CMAKE_CURRENT_SOURCE_DIR}/..)

    if (_missing)
        string(REPLACE ";" ", " _missing_text "${_missing}")
        add_custom_target(ccmath-rigorous-pow-proof-ulp-aspiration
                COMMAND ${CMAKE_COMMAND} -E echo "ccmath-rigorous-pow-proof-ulp-aspiration skipped: ${_missing_text} not found"
                COMMENT "pow proof ULP aspiration (${_missing_text} missing)")
    else ()
        add_custom_target(ccmath-rigorous-pow-proof-ulp-aspiration
                COMMAND ${Python3_EXECUTABLE} ${_script} --aspirational-ulp
                WORKING_DIRECTORY ${_workdir}
                COMMENT "pow proof aspirational 0.5 and 1.5 ULP checks (non-blocking)")

        add_test(NAME ccmath-rigorous-pow-proof-ulp-aspiration
                COMMAND ${Python3_EXECUTABLE} ${_script} --aspirational-ulp
                WORKING_DIRECTORY ${_workdir})
        set_tests_properties(ccmath-rigorous-pow-proof-ulp-aspiration PROPERTIES
                LABELS "aspirational;proof")
    endif ()
endfunction()
