# Longer libFuzzer campaign using the LLVM two-directory corpus layout.
#
# Required: FUZZER, CORPUS_DIR, SEED_DIR, ARTIFACT_PREFIX
# Optional: FUZZ_MAX_TOTAL_TIME (default 300), FUZZ_RUNS (empty = libFuzzer default unlimited within time)

if (NOT FUZZER OR NOT CORPUS_DIR OR NOT SEED_DIR OR NOT ARTIFACT_PREFIX)
	message(FATAL_ERROR "run_fuzz_campaign.cmake requires FUZZER, CORPUS_DIR, SEED_DIR, ARTIFACT_PREFIX")
endif ()

if (NOT DEFINED FUZZ_MAX_TOTAL_TIME)
	set(FUZZ_MAX_TOTAL_TIME 300)
endif ()

file(MAKE_DIRECTORY "${CORPUS_DIR}")
file(MAKE_DIRECTORY "${ARTIFACT_PREFIX}")

if (NOT EXISTS "${SEED_DIR}/zero.bin")
	set(FUZZ_SEED_DIR "${SEED_DIR}")
	include("${CMAKE_CURRENT_LIST_DIR}/generate_fuzz_seeds.cmake")
endif ()

set(_fuzzer_args "${FUZZER}" "${CORPUS_DIR}" "${SEED_DIR}" "-max_total_time=${FUZZ_MAX_TOTAL_TIME}"
	"-artifact_prefix=${ARTIFACT_PREFIX}")

if (DEFINED FUZZ_RUNS AND NOT FUZZ_RUNS STREQUAL "")
	list(APPEND _fuzzer_args "-runs=${FUZZ_RUNS}")
endif ()

execute_process(COMMAND ${_fuzzer_args} RESULT_VARIABLE fuzz_result)

if (NOT fuzz_result EQUAL 0)
	message(FATAL_ERROR "Fuzz campaign failed for ${FUZZER} (exit ${fuzz_result})")
endif ()
