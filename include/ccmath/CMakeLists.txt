add_subdirectory(detail)
add_subdirectory(internal)

target_sources(${PROJECT_NAME} INTERFACE
        basic.hpp
        ccmath.hpp
        compare.hpp
        exponential.hpp
        fmanip.hpp
        hyperbolic.hpp
        nearest.hpp
        numbers.hpp
        power.hpp
        special.hpp
        trig.hpp
)
