cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(ccmath-fuzzing LANGUAGES CXX)

if (NOT TARGET ccmath)
	find_package(ccmath CONFIG REQUIRED)
endif ()

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	message(FATAL_ERROR "CCMath fuzzing requires Clang with libFuzzer (-fsanitize=fuzzer).")
endif ()

include(CTest)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules")
include(CcmLibFuzzer)

ccmath_detect_libfuzzer()

if (NOT CCMATH_LIBFUZZER_AVAILABLE)
	message(FATAL_ERROR
		"libFuzzer link probe failed with ${CMAKE_CXX_COMPILER}. "
		"Use a Clang toolchain that ships libFuzzer (full LLVM Clang, not Apple Clang), "
		"or set LIB_FUZZING_ENGINE for OSS-Fuzz-style builds. "
		"Reconfigure with -DCMAKE_CXX_COMPILER=... pointing at that toolchain.")
endif ()

function(ccmath_add_fuzzer TARGET)
	set(SOURCES ${ARGN})

	add_executable(${TARGET} ${SOURCES})
	target_link_libraries(${TARGET} PRIVATE ccmath::ccmath)
	target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_compile_features(${TARGET} PRIVATE cxx_std_17)
	target_compile_options(${TARGET} PRIVATE ${CCMATH_FUZZ_COMPILE_FLAGS} -Wno-everything)
	target_link_options(${TARGET} PRIVATE ${CCMATH_FUZZ_LINK_FLAGS})

	set(CCMATH_FUZZ_CORPUS_DIR "${CMAKE_CURRENT_BINARY_DIR}/corpus/${TARGET}")
	set(CCMATH_FUZZ_SEED_DIR "${CMAKE_CURRENT_BINARY_DIR}/seeds/${TARGET}")
	set(CCMATH_FUZZ_ARTIFACT_DIR "${CMAKE_CURRENT_BINARY_DIR}/artifacts/${TARGET}/")

	add_test(
		NAME ${TARGET}-smoke
		COMMAND ${CMAKE_COMMAND}
			-DCORPUS_DIR=${CCMATH_FUZZ_CORPUS_DIR}
			-DSEED_DIR=${CCMATH_FUZZ_SEED_DIR}
			-DARTIFACT_PREFIX=${CCMATH_FUZZ_ARTIFACT_DIR}
			-DFUZZER=$<TARGET_FILE:${TARGET}>
			-P ${CMAKE_CURRENT_SOURCE_DIR}/run_fuzz_smoke.cmake)
	set_tests_properties(${TARGET}-smoke PROPERTIES TIMEOUT 180)

	# Regenerate seeds once at configure time for manual runs without CTest.
	set(_seed_stamp "${CMAKE_CURRENT_BINARY_DIR}/seeds/${TARGET}/.generated")
	add_custom_command(
		OUTPUT "${_seed_stamp}"
		COMMAND ${CMAKE_COMMAND} -E rm -rf "${CCMATH_FUZZ_SEED_DIR}"
		COMMAND ${CMAKE_COMMAND}
			-DFUZZ_SEED_DIR=${CCMATH_FUZZ_SEED_DIR}
			-P ${CMAKE_CURRENT_SOURCE_DIR}/generate_fuzz_seeds.cmake
		COMMAND ${CMAKE_COMMAND} -E touch "${_seed_stamp}"
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate_fuzz_seeds.cmake
		VERBATIM)
	add_custom_target(${TARGET}-seeds DEPENDS "${_seed_stamp}")
	add_dependencies(${TARGET} ${TARGET}-seeds)
endfunction()

ccmath_add_fuzzer(ccmath-fuzz-basic basic_fuzz.cpp)
ccmath_add_fuzzer(ccmath-fuzz-compare compare_fuzz.cpp)
ccmath_add_fuzzer(ccmath-fuzz-expo expo_fuzz.cpp)
ccmath_add_fuzzer(ccmath-fuzz-fmanip fmanip_fuzz.cpp)
ccmath_add_fuzzer(ccmath-fuzz-nearest nearest_fuzz.cpp)
ccmath_add_fuzzer(ccmath-fuzz-power power_fuzz.cpp)
ccmath_add_fuzzer(ccmath-fuzz-trig trig_fuzz.cpp)
ccmath_add_fuzzer(ccmath-fuzz-misc misc_fuzz.cpp)
