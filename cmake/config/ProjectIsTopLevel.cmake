# this variable is set by project() in CMake 3.21+
string(
        COMPARE EQUAL
        "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
        CCMATH_PROJECT_IS_TOP_LEVEL
)
