# LLVM-style smoke fuzz: writable corpus (arg 1), read-only seeds (arg 2), bounded -runs.

if (NOT CORPUS_DIR OR NOT SEED_DIR OR NOT ARTIFACT_PREFIX OR NOT FUZZER)
	message(FATAL_ERROR "run_fuzz_smoke.cmake requires CORPUS_DIR, SEED_DIR, ARTIFACT_PREFIX, and FUZZER")
endif ()

if (NOT DEFINED FUZZ_SMOKE_RUNS)
	set(FUZZ_SMOKE_RUNS 256)
endif ()

set(FUZZ_SEED_DIR "${SEED_DIR}")
file(REMOVE_RECURSE "${FUZZ_SEED_DIR}" "${CORPUS_DIR}")
include("${CMAKE_CURRENT_LIST_DIR}/generate_fuzz_seeds.cmake")

file(MAKE_DIRECTORY "${CORPUS_DIR}")
file(MAKE_DIRECTORY "${ARTIFACT_PREFIX}")

# ./fuzzer WRITABLE_CORPUS/ READ_ONLY_SEEDS/ -runs=N -artifact_prefix=...
execute_process(
	COMMAND "${FUZZER}" "${CORPUS_DIR}" "${SEED_DIR}"
		-runs=${FUZZ_SMOKE_RUNS}
		-artifact_prefix=${ARTIFACT_PREFIX}
	RESULT_VARIABLE fuzz_result
	TIMEOUT 120)

if (NOT fuzz_result EQUAL 0)
	if (fuzz_result STREQUAL "Process timeout")
		message(FATAL_ERROR "Fuzz smoke test timed out for ${FUZZER} after 120s")
	endif ()
	message(FATAL_ERROR "Fuzz smoke test failed for ${FUZZER} (exit ${fuzz_result})")
endif ()
