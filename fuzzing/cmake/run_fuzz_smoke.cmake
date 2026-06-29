# LLVM-style smoke fuzz: writable corpus (arg 1), read-only seeds (arg 2), bounded -runs.
# CCMATH_FUZZ_SMOKE_RUNS in the environment overrides the run budget, and
# CCMATH_FUZZ_KEEP_CORPUS=1 preserves the corpus across runs so cached CI corpora
# accumulate coverage instead of restarting from seeds every time.

if (NOT CORPUS_DIR OR NOT SEED_DIR OR NOT ARTIFACT_PREFIX OR NOT FUZZER)
    message(FATAL_ERROR "run_fuzz_smoke.cmake requires CORPUS_DIR, SEED_DIR, ARTIFACT_PREFIX, and FUZZER")
endif ()

if (DEFINED ENV{CCMATH_FUZZ_SMOKE_RUNS})
    set(FUZZ_SMOKE_RUNS "$ENV{CCMATH_FUZZ_SMOKE_RUNS}")
endif ()
if (NOT DEFINED FUZZ_SMOKE_RUNS)
    set(FUZZ_SMOKE_RUNS 256)
endif ()

if (NOT "$ENV{CCMATH_FUZZ_KEEP_CORPUS}" STREQUAL "1")
    file(REMOVE_RECURSE "${CORPUS_DIR}")
endif ()
file(MAKE_DIRECTORY "${CORPUS_DIR}")
file(MAKE_DIRECTORY "${ARTIFACT_PREFIX}")

# Inputs are at most three long doubles plus selector and mode bytes.
set(_fuzzer_args "${FUZZER}" "${CORPUS_DIR}" "${SEED_DIR}"
        "-runs=${FUZZ_SMOKE_RUNS}"
        "-max_len=80"
        "-print_final_stats=1"
        "-artifact_prefix=${ARTIFACT_PREFIX}")

if (DEFINED CCMATH_FUZZ_DICT AND EXISTS "${CCMATH_FUZZ_DICT}")
    list(APPEND _fuzzer_args "-dict=${CCMATH_FUZZ_DICT}")
endif ()

execute_process(
        COMMAND ${_fuzzer_args}
        RESULT_VARIABLE fuzz_result
        TIMEOUT 270)

if (NOT fuzz_result EQUAL 0)
    if (fuzz_result STREQUAL "Process timeout")
        message(FATAL_ERROR "Fuzz smoke test timed out for ${FUZZER} after 270s")
    endif ()
    message(FATAL_ERROR "Fuzz smoke test failed for ${FUZZER} (exit ${fuzz_result})")
endif ()
