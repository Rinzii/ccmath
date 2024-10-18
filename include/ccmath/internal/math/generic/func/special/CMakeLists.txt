macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/assoc_laguerre_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/assoc_legendre_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/beta_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/comp_ellint_1_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/comp_ellint_2_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/comp_ellint_3_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cyl_bessel_i_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cyl_bessel_j_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cyl_bessel_k_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cyl_neumann_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ellint_1_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ellint_2_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ellint_3_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/expint_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/hermite_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/laguerre_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/legendre_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/riemann_zeta_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sph_bessel_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sph_legendre_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sph_neumann_gen.hpp
)
