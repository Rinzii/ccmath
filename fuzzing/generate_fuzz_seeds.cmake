# Generate read-only libFuzzer seed inputs in the build tree.

if (NOT FUZZ_SEED_DIR)
	message(FATAL_ERROR "generate_fuzz_seeds.cmake requires FUZZ_SEED_DIR")
endif ()

find_program(CCMATH_PYTHON3 NAMES python3 python)
if (NOT CCMATH_PYTHON3)
	message(FATAL_ERROR "python3 is required to generate fuzz seed inputs")
endif ()

file(MAKE_DIRECTORY "${FUZZ_SEED_DIR}")

function(ccmath_fuzz_write_seed_hex filename hex)
	if (NOT hex MATCHES "^[0-9a-fA-F]*$" OR hex STREQUAL "")
		message(FATAL_ERROR "Invalid hex seed for ${filename}")
	endif ()

	execute_process(
		COMMAND "${CCMATH_PYTHON3}" -c
			"import pathlib; pathlib.Path(r'${FUZZ_SEED_DIR}/${filename}').write_bytes(bytes.fromhex('${hex}'))"
		RESULT_VARIABLE write_result)

	if (NOT write_result EQUAL 0)
		message(FATAL_ERROR "Failed to write fuzz seed ${filename}")
	endif ()
endfunction()

# double bit patterns (little-endian)
ccmath_fuzz_write_seed_hex("zero.bin" "0000000000000000")
ccmath_fuzz_write_seed_hex("neg_zero.bin" "0000000000000080")
ccmath_fuzz_write_seed_hex("one.bin" "000000000000f03f")
ccmath_fuzz_write_seed_hex("denorm_min.bin" "0100000000000000")
ccmath_fuzz_write_seed_hex("max.bin" "ffffffffffffef7f")
ccmath_fuzz_write_seed_hex("nan.bin" "010000000000f87f")
ccmath_fuzz_write_seed_hex("pos_inf.bin" "000000000000f07f")
ccmath_fuzz_write_seed_hex("neg_inf.bin" "000000000000f0ff")

# two doubles (1.5, -2.25) plus int32 exponent 42
ccmath_fuzz_write_seed_hex("binary_pair.bin" "000000000000f83f00000000000002c02a000000")

# lerp(a=1, b=10, t=0.25)
ccmath_fuzz_write_seed_hex("lerp_triple.bin" "000000000000f03f0000000000002440000000000000d03f")
