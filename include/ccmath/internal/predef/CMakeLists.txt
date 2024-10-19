ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/assume.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/expects_bool_condition.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/has_attribute.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/has_builtin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/has_const_builtin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/likely.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/unlikely.hpp
)

add_subdirectory(attributes)
add_subdirectory(compiler_suppression)
add_subdirectory(versioning)
