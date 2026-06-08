-- CCMath secondary build (library consumption). CMake is the primary build system.
-- Regenerate generated files:
--   cmake --build <build-dir> --target ccmath-generate-secondary-builds

-- BEGIN GENERATED LIBRARY MANIFEST
workspace "ccmath"
    location "."
    configurations { "Debug", "Release" }
    architecture "x86_64"
    cppdialect "C++17"

newoption {
    trigger = "ccmath-enable-runtime-simd",
    description = "Enable SIMD optimization for runtime evaluation (does not affect compile-time)",
    allowed = { { "true", "Enable" }, { "false", "Disable" } },
    default = "true",
}

newoption {
    trigger = "ccmath-disable-errno",
    description = "Disable the use of errno in ccmath during runtime (may lead to faster evaluations but is non-standard)",
    allowed = { { "true", "Enable" }, { "false", "Disable" } },
    default = "false",
}

newoption {
    trigger = "ccmath-enable-reduced-precision-powl",
    description = "Allow reduced-precision double fallback for incomplete powl tiers",
    allowed = { { "true", "Enable" }, { "false", "Disable" } },
    default = "false",
}


project "ccmath-smoke"
    kind "ConsoleApp"
    language "C++"
    location "out/premake"
    includedirs { "include", "out/build/ninja/ninja-clang-debug/include" }
    files { "examples/smoke/constexpr_smoke.cpp" }

filter { "options:ccmath-enable-runtime-simd=true" }
    defines { "CCM_CONFIG_USE_RT_SIMD" }
filter {}

filter { "options:ccmath-disable-errno=true" }
    defines { "CCM_CONFIG_DISABLE_ERRNO" }
filter {}

filter { "options:ccmath-enable-reduced-precision-powl=true" }
    defines { "CCM_CONFIG_ENABLE_REDUCED_PRECISION_POWL" }
filter {}

filter "configurations:Debug"
    symbols "On"
    optimize "Off"

filter "configurations:Release"
    symbols "Off"
    optimize "Speed"

filter {}
-- END GENERATED LIBRARY MANIFEST
