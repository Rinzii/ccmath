cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# asmlab is driven by the Python scripts under tools/asmlab/scripts. These targets
# are thin convenience wrappers so the lab is reachable from a configured build tree
# without anyone memorising script paths. They never touch include/ccmath.

if (NOT TARGET ccmath)
    message(FATAL_ERROR "tools/asmlab requires the ccmath interface target")
endif ()

find_package(Python3 COMPONENTS Interpreter QUIET)
if (NOT Python3_FOUND)
    message(WARNING "asmlab: Python3 not found; convenience targets will be skipped")
    return()
endif ()

set(ASMLAB_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

# asmlab-bootstrap: provision llvm-mca / uiCA / OSACA and cache reference tables.
add_custom_target(asmlab-bootstrap
        COMMAND bash "${ASMLAB_SCRIPTS}/bootstrap.sh"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "asmlab: provisioning analysis tooling")

# asmlab-seed: emit + analyze + report the seeded power functions across the matrix.
add_custom_target(asmlab-seed
        COMMAND ${Python3_EXECUTABLE} "${ASMLAB_SCRIPTS}/asmlab.py" report pow_impl powf_impl sqrt sqrtf
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "asmlab: seeding baseline reports for power functions")

# asmlab-verify-pow_impl: path verification smoke for pow_impl.
add_custom_target(asmlab-verify-pow_impl
        COMMAND ${Python3_EXECUTABLE} "${ASMLAB_SCRIPTS}/asmlab.py" verify pow_impl --arch x86-64-v3
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "asmlab: verify pow_impl path classification")

# asmlab-source-map-sqrt: source traceability smoke for sqrt.
add_custom_target(asmlab-source-map-sqrt
        COMMAND ${Python3_EXECUTABLE} "${ASMLAB_SCRIPTS}/asmlab.py" report sqrt --arch x86-64-v3 --source-map
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "asmlab: source map smoke for sqrt")
