add_subdirectory(basic)
add_subdirectory(compare)
add_subdirectory(exponential)
add_subdirectory(fmanip)
add_subdirectory(hyperbolic)
add_subdirectory(nearest)
add_subdirectory(power)
add_subdirectory(special)
add_subdirectory(trig)

target_sources(${PROJECT_NAME} PRIVATE
        gamma.hpp
        lerp.hpp
        lgamma.hpp
)
