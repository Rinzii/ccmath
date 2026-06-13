# Rigorous oracle campaign registration (registry-driven executables and ctests).
#
# Include OracleCampaignRegistry.cmake before calling ccmath_apply_oracle_registries().

include_guard(GLOBAL)

function(_ccmath_oracle_test_requires TEST_PREFIX OUT_VAR)
    set(_requires_var ${TEST_PREFIX}_REQUIRES)
    if (DEFINED ${_requires_var})
        set(${OUT_VAR} ${${_requires_var}} PARENT_SCOPE)
    else ()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif ()
endfunction()

function(_ccmath_oracle_test_condition_ok REQUIRES_EXPR)
    if (REQUIRES_EXPR STREQUAL "")
        set(_ok TRUE PARENT_SCOPE)
        return ()
    endif ()
    if (${REQUIRES_EXPR})
        set(_ok TRUE PARENT_SCOPE)
    else ()
        set(_ok FALSE PARENT_SCOPE)
    endif ()
endfunction()

function(_ccmath_oracle_register_ctest BACKEND TEST_ID LOG_DIR)
    set(_prefix CCMATH_ORACLE_${BACKEND}_TEST_${TEST_ID})

    if (NOT DEFINED ${_prefix}_NAME)
        message(FATAL_ERROR "Oracle registry: missing ${_prefix}_NAME")
    endif ()
    if (NOT DEFINED ${_prefix}_TARGET)
        message(FATAL_ERROR "Oracle registry: missing ${_prefix}_TARGET")
    endif ()

    _ccmath_oracle_test_requires(${_prefix} _requires)
    _ccmath_oracle_test_condition_ok("${_requires}" _register)
    if (NOT _register)
        return ()
    endif ()

    set(_ctest_args)
    if (DEFINED ${_prefix}_ARGS)
        list(APPEND _ctest_args ${${_prefix}_ARGS})
    endif ()

    set(_log_var ${_prefix}_LOG_OUTPUT)
    set(_json_var ${_prefix}_JSON_OUTPUT)
    if (DEFINED ${_log_var})
        ccmath_add_oracle_ctest(${${_prefix}_NAME}
                TARGET ${${_prefix}_TARGET}
                LABELS ${${_prefix}_LABELS}
                TIMEOUT ${${_prefix}_TIMEOUT}
                ARGS ${_ctest_args}
                LOG_OUTPUT ${${_log_var}}
                WORKING_DIRECTORY ${LOG_DIR})
    elseif (DEFINED ${_json_var})
        ccmath_add_oracle_ctest(${${_prefix}_NAME}
                TARGET ${${_prefix}_TARGET}
                LABELS ${${_prefix}_LABELS}
                TIMEOUT ${${_prefix}_TIMEOUT}
                ARGS ${_ctest_args}
                JSON_OUTPUT ${${_json_var}}
                WORKING_DIRECTORY ${LOG_DIR})
    else ()
        ccmath_add_oracle_ctest(${${_prefix}_NAME}
                TARGET ${${_prefix}_TARGET}
                LABELS ${${_prefix}_LABELS}
                TIMEOUT ${${_prefix}_TIMEOUT}
                ARGS ${_ctest_args}
                WORKING_DIRECTORY ${LOG_DIR})
    endif ()
endfunction()

function(_ccmath_oracle_register_backend BACKEND LOG_DIR)
    set(_exe_ids_var CCMATH_ORACLE_${BACKEND}_EXECUTABLE_IDS)
    if (NOT DEFINED ${_exe_ids_var})
        return ()
    endif ()

    foreach (_exe_id IN LISTS ${_exe_ids_var})
        set(_target_var CCMATH_ORACLE_${BACKEND}_${_exe_id}_TARGET)
        set(_source_var CCMATH_ORACLE_${BACKEND}_${_exe_id}_SOURCE)
        if (NOT DEFINED ${_target_var} OR NOT DEFINED ${_source_var})
            message(FATAL_ERROR "Oracle registry: incomplete entry for ${BACKEND} ${_exe_id}")
        endif ()

        if (BACKEND STREQUAL "MPFR")
            ccmath_ensure_mpfr_oracle_target(${${_target_var}} SOURCES ${${_source_var}})
        elseif (BACKEND STREQUAL "COREMATH")
            ccmath_ensure_coremath_oracle_target(${${_target_var}} SOURCES ${${_source_var}})
        else ()
            message(FATAL_ERROR "Unknown oracle backend ${BACKEND}")
        endif ()

        set(_ctest_ids_var CCMATH_ORACLE_${BACKEND}_${_exe_id}_CTESTS)
        if (DEFINED ${_ctest_ids_var})
            foreach (_test_id IN LISTS ${_ctest_ids_var})
                _ccmath_oracle_register_ctest(${BACKEND} ${_test_id} ${LOG_DIR})
            endforeach ()
        endif ()
    endforeach ()
endfunction()

function(ccmath_apply_oracle_registries LOG_DIR)
    if (CCMATH_ENABLE_MPFR_TESTS AND CCMATH_MPFR_FOUND)
        _ccmath_oracle_register_backend(MPFR ${LOG_DIR})
    endif ()

    if (CCMATH_ENABLE_COREMATH_TESTS AND CCMATH_COREMATH_FOUND AND TARGET ccmath::coremath_oracle)
        _ccmath_oracle_register_backend(COREMATH ${LOG_DIR})
    endif ()
endfunction()
