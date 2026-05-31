# Portable libFuzzer setup for out-of-tree fuzz targets (Kitware + LLVM patterns).
#
# Sets in parent scope:
#   CCMATH_LIBFUZZER_AVAILABLE - TRUE when linking works
#   CCMATH_FUZZ_COMPILE_FLAGS  - compile options for fuzz executables
#   CCMATH_FUZZ_LINK_FLAGS     - link options for fuzz executables
#
# Optional environment:
#   LIB_FUZZING_ENGINE - OSS-Fuzz / external engine path (link-only; compile uses fuzzer-no-link)

include(CheckCXXSourceCompiles)

set(CCMATH_FUZZ_SANITIZERS "fuzzer,address,undefined")

function(ccmath_probe_libfuzzer compile_flags link_flags result_var)
	set(CMAKE_REQUIRED_FLAGS ${compile_flags})
	set(CMAKE_REQUIRED_LINK_OPTIONS ${link_flags})
	check_cxx_source_compiles("
#include <cstddef>
#include <cstdint>
extern \"C\" int LLVMFuzzerTestOneInput(uint8_t const*, size_t) { return 0; }
int main() { return 0; }
" ${result_var})
endfunction()

function(ccmath_append_compiler_libcxx_flags compile_flags_var link_flags_var)
	get_filename_component(_cxx_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
	get_filename_component(_cxx_root "${_cxx_bin_dir}" DIRECTORY)
	set(_libcxx_dir "${_cxx_root}/lib/c++")

	if (NOT EXISTS "${_libcxx_dir}")
		return()
	endif ()

	set(${compile_flags_var} ${${compile_flags_var}} "-stdlib=libc++" PARENT_SCOPE)
	set(_link_flags
		${${link_flags_var}}
		"-stdlib=libc++"
		"-L${_libcxx_dir}"
		"-L${_cxx_root}/lib")

	if (APPLE)
		list(APPEND _link_flags "-Wl,-rpath,${_libcxx_dir}" "-Wl,-rpath,${_cxx_root}/lib")
	endif ()

	set(${link_flags_var} ${_link_flags} PARENT_SCOPE)
endfunction()

function(ccmath_detect_libfuzzer)
	set(_compile_base "-fno-omit-frame-pointer" "-O1")
	set(_available FALSE)

	if (DEFINED ENV{LIB_FUZZING_ENGINE})
		set(_compile_flags ${_compile_base} "-fsanitize=fuzzer-no-link,address,undefined")
		set(_link_flags "$ENV{LIB_FUZZING_ENGINE}" "-fsanitize=address,undefined")
	else ()
		set(_compile_flags ${_compile_base} "-fsanitize=${CCMATH_FUZZ_SANITIZERS}")
		set(_link_flags "-fsanitize=${CCMATH_FUZZ_SANITIZERS}")
	endif ()

	# When the selected Clang ships its own libc++, link the fuzzer runtime against it.
	ccmath_append_compiler_libcxx_flags(_compile_flags _link_flags)
	ccmath_probe_libfuzzer("${_compile_flags}" "${_link_flags}" _probe_ok)

	if (_probe_ok)
		set(_available TRUE)
	endif ()

	set(CCMATH_LIBFUZZER_AVAILABLE ${_available} PARENT_SCOPE)
	set(CCMATH_FUZZ_COMPILE_FLAGS ${_compile_flags} PARENT_SCOPE)
	set(CCMATH_FUZZ_LINK_FLAGS ${_link_flags} PARENT_SCOPE)
endfunction()
