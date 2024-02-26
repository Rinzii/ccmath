project(ccmath-exe)

set(CMAKE_CXX_STANDARD 17)

add_executable(ccmath-exe main.cpp)

target_link_libraries(ccmath-exe ccmath::ccmath)
