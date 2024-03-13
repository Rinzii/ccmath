add_subdirectory(impl)

target_sources(${PROJECT_NAME} PRIVATE
        abs.hpp
        fdim.hpp
        fma.hpp
        fmod.hpp
        max.hpp
        min.hpp
        remainder.hpp
        remquo.hpp
)
