ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/copysign.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/frexp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ilogb.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ldexp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/logb.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modf.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nextafter.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nexttowards.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/scalbn.hpp
)

add_subdirectory(impl)
