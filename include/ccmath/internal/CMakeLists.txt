add_subdirectory(helpers)
add_subdirectory(predef)
add_subdirectory(setup)
add_subdirectory(type_traits)
add_subdirectory(utility)

target_sources(${PROJECT_NAME} PRIVATE
        setup.hpp
        version.hpp
)
