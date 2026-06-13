cmake_minimum_required(VERSION 3.18)

project(vendored-ccmath-consumer LANGUAGES CXX)

add_library(ccmath-vendored INTERFACE)
target_include_directories(ccmath-vendored INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ccmath/include"
)
target_compile_features(ccmath-vendored INTERFACE cxx_std_17)

add_executable(vendored-ccmath-consumer main.cpp)
target_link_libraries(vendored-ccmath-consumer PRIVATE ccmath-vendored)
