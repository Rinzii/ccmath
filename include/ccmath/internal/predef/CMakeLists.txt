ccm_add_headers(
        assume.hpp
        expects_bool_condition.hpp
        has_attribute.hpp
        has_builtin.hpp
        has_const_builtin.hpp
        likely.hpp
        unlikely.hpp
)

add_subdirectory(attributes)
add_subdirectory(compiler_suppression)
add_subdirectory(versioning)
