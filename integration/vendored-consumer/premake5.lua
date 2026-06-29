workspace "vendored-ccmath-consumer"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    cppdialect "C++17"

project "vendored-ccmath-consumer"
    kind "ConsoleApp"
    language "C++"
    includedirs { "third_party/ccmath/include" }
    files { "main.cpp" }

filter "configurations:Debug"
    symbols "On"
    optimize "Off"

filter "configurations:Release"
    symbols "Off"
    optimize "Speed"

filter {}
