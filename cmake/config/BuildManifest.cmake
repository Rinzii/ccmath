# Library-facing build metadata for CMake, manifest emission, and secondary build codegen.

set(CCMATH_LIBRARY_MANIFEST_CPP_STANDARD 17)

set(CCMATH_LIBRARY_MANIFEST_GENERATED_HEADERS
        ccmath/version.hpp
)

set(CCMATH_LIBRARY_MANIFEST_VERSION_TEMPLATE cmake/in/version.hpp.in)

# Ordered option keys. Each key sets CCMATH_LIBRARY_MANIFEST_OPTION_<key>_* variables.
set(CCMATH_LIBRARY_MANIFEST_OPTION_KEYS
        runtime_simd
        disable_errno
        reduced_precision_powl
        deterministic
)

set(CCMATH_LIBRARY_MANIFEST_OPTION_runtime_simd_CMAKE_VAR CCMATH_DISABLE_RUNTIME_SIMD)
set(CCMATH_LIBRARY_MANIFEST_OPTION_runtime_simd_CMAKE_DEFAULT OFF)
set(CCMATH_LIBRARY_MANIFEST_OPTION_runtime_simd_CMAKE_INVERT TRUE)
set(CCMATH_LIBRARY_MANIFEST_OPTION_runtime_simd_MESON_OPTION enable_runtime_simd)
set(CCMATH_LIBRARY_MANIFEST_OPTION_runtime_simd_DEFINE CCM_CONFIG_USE_RT_SIMD)
set(CCMATH_LIBRARY_MANIFEST_OPTION_runtime_simd_DESCRIPTION
        "Enable SIMD optimization for runtime evaluation (does not affect compile-time)")

set(CCMATH_LIBRARY_MANIFEST_OPTION_disable_errno_CMAKE_VAR CCMATH_DISABLE_ERRNO)
set(CCMATH_LIBRARY_MANIFEST_OPTION_disable_errno_CMAKE_DEFAULT OFF)
set(CCMATH_LIBRARY_MANIFEST_OPTION_disable_errno_CMAKE_INVERT FALSE)
set(CCMATH_LIBRARY_MANIFEST_OPTION_disable_errno_MESON_OPTION disable_errno)
set(CCMATH_LIBRARY_MANIFEST_OPTION_disable_errno_DEFINE CCM_CONFIG_DISABLE_ERRNO)
set(CCMATH_LIBRARY_MANIFEST_OPTION_disable_errno_DESCRIPTION
        "Disable the use of errno in ccmath during runtime (may lead to faster evaluations but is non-standard)")

set(CCMATH_LIBRARY_MANIFEST_OPTION_reduced_precision_powl_CMAKE_VAR CCMATH_DISABLE_REDUCED_PRECISION_POWL)
set(CCMATH_LIBRARY_MANIFEST_OPTION_reduced_precision_powl_CMAKE_DEFAULT OFF)
set(CCMATH_LIBRARY_MANIFEST_OPTION_reduced_precision_powl_CMAKE_INVERT FALSE)
set(CCMATH_LIBRARY_MANIFEST_OPTION_reduced_precision_powl_MESON_OPTION disable_reduced_precision_powl)
set(CCMATH_LIBRARY_MANIFEST_OPTION_reduced_precision_powl_DEFINE CCM_CONFIG_DISABLE_REDUCED_PRECISION_POWL)
set(CCMATH_LIBRARY_MANIFEST_OPTION_reduced_precision_powl_DESCRIPTION
        "Return quiet NaN from powl on non-binary80 long double instead of the default reduced-precision double fallback")

set(CCMATH_LIBRARY_MANIFEST_OPTION_deterministic_CMAKE_VAR CCMATH_ENABLE_DETERMINISTIC)
set(CCMATH_LIBRARY_MANIFEST_OPTION_deterministic_CMAKE_DEFAULT OFF)
set(CCMATH_LIBRARY_MANIFEST_OPTION_deterministic_CMAKE_INVERT FALSE)
set(CCMATH_LIBRARY_MANIFEST_OPTION_deterministic_MESON_OPTION deterministic)
set(CCMATH_LIBRARY_MANIFEST_OPTION_deterministic_DEFINE CCM_CONFIG_DETERMINISTIC)
set(CCMATH_LIBRARY_MANIFEST_OPTION_deterministic_DESCRIPTION
        "Produce bit-identical cross-hardware math: route transcendentals through the generic kernels (no libm), force the correctly-rounded FMA path, disable runtime SIMD, and evaluate long double in double precision (GCC/Clang)")

function(ccmath_manifest_declare_library_options)
    foreach (_ccmath_manifest_key IN LISTS CCMATH_LIBRARY_MANIFEST_OPTION_KEYS)
        set(_ccmath_cmake_var "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_CMAKE_VAR}")
        set(_ccmath_description "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_DESCRIPTION}")
        set(_ccmath_default "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_CMAKE_DEFAULT}")
        option("${_ccmath_cmake_var}" "${_ccmath_description}" "${_ccmath_default}")
    endforeach ()
endfunction()

function(ccmath_manifest_apply_library_compile_definitions target)
    foreach (_ccmath_manifest_key IN LISTS CCMATH_LIBRARY_MANIFEST_OPTION_KEYS)
        set(_ccmath_cmake_var "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_CMAKE_VAR}")
        set(_ccmath_define "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_DEFINE}")
        set(_ccmath_invert "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_CMAKE_INVERT}")

        if (_ccmath_invert)
            if (NOT ${_ccmath_cmake_var})
                target_compile_definitions("${target}" INTERFACE "${_ccmath_define}")
            endif ()
        else ()
            if (${_ccmath_cmake_var})
                target_compile_definitions("${target}" INTERFACE "${_ccmath_define}")
            endif ()
        endif ()
    endforeach ()
endfunction()

function(ccmath_manifest_meson_default option_key)
    set(_ccmath_cmake_default "${CCMATH_LIBRARY_MANIFEST_OPTION_${option_key}_CMAKE_DEFAULT}")
    set(_ccmath_invert "${CCMATH_LIBRARY_MANIFEST_OPTION_${option_key}_CMAKE_INVERT}")

    if (_ccmath_invert)
        if (_ccmath_cmake_default)
            set(_ccmath_meson_default false PARENT_SCOPE)
        else ()
            set(_ccmath_meson_default true PARENT_SCOPE)
        endif ()
    else ()
        if (_ccmath_cmake_default)
            set(_ccmath_meson_default true PARENT_SCOPE)
        else ()
            set(_ccmath_meson_default false PARENT_SCOPE)
        endif ()
    endif ()
endfunction()

function(ccmath_manifest_option_enabled option_key out_var)
    set(_ccmath_cmake_var "${CCMATH_LIBRARY_MANIFEST_OPTION_${option_key}_CMAKE_VAR}")
    set(_ccmath_invert "${CCMATH_LIBRARY_MANIFEST_OPTION_${option_key}_CMAKE_INVERT}")

    if (_ccmath_invert)
        if (NOT ${_ccmath_cmake_var})
            set("${out_var}" TRUE PARENT_SCOPE)
        else ()
            set("${out_var}" FALSE PARENT_SCOPE)
        endif ()
    else ()
        if (${_ccmath_cmake_var})
            set("${out_var}" TRUE PARENT_SCOPE)
        else ()
            set("${out_var}" FALSE PARENT_SCOPE)
        endif ()
    endif ()
endfunction()
