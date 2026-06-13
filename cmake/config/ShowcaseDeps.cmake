include_guard(GLOBAL)

include(FetchContent)

function(ccmath_showcase_fetch_gcem)
    if (NOT CCMATH_SHOWCASE_ENABLE_GCEM)
        return()
    endif ()
    if (TARGET showcase::gcem)
        return()
    endif ()

    set(_CCMATH_GCEM_VERSION v1.18.0)
    FetchContent_Declare(
            gcem
            GIT_REPOSITORY https://github.com/kthohr/gcem.git
            GIT_TAG ${_CCMATH_GCEM_VERSION}
            GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(gcem)
    if (NOT gcem_POPULATED)
        FetchContent_Populate(gcem)
    endif ()

    add_library(showcase_gcem INTERFACE)
    add_library(showcase::gcem ALIAS showcase_gcem)
    target_include_directories(showcase_gcem SYSTEM INTERFACE ${gcem_SOURCE_DIR}/include)
endfunction()

function(ccmath_showcase_fetch_gtest)
    if (TARGET gtest::gtest)
        return()
    endif ()

    set(_CCMATH_GTEST_VERSION v1.15.2)
    if (WIN32)
        set(gtest_force_shared_crt ON)
        set(gtest_disable_pthreads ON)
    endif ()
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_build_samples OFF CACHE BOOL "" FORCE)
    set(gtest_build_tests OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG ${_CCMATH_GTEST_VERSION}
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)

    if (NOT TARGET gtest::gtest)
        add_library(gtest::gtest ALIAS gtest)
    endif ()
endfunction()

function(ccmath_showcase_configure_target TARGET)
    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "ccmath_showcase_configure_target: ${TARGET} does not exist")
    endif ()
    target_compile_features(${TARGET} PRIVATE cxx_std_20)
    target_include_directories(${TARGET} PRIVATE
            ${CMAKE_SOURCE_DIR}/showcase/common
            ${CMAKE_SOURCE_DIR}/benchmarks
            ${CMAKE_SOURCE_DIR}/tests
            ${CMAKE_SOURCE_DIR}/tests/shared/utils
    )
    if (CCMATH_SHOWCASE_ENABLE_GCEM)
        target_link_libraries(${TARGET} PRIVATE showcase::gcem)
        target_compile_definitions(${TARGET} PRIVATE CCMATH_SHOWCASE_HAS_GCEM=1)
    endif ()
endfunction()
