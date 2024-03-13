add_subdirectory(impl)

target_sources(${PROJECT_NAME} PRIVATE
        exp.hpp
        exp2.hpp
        expm1.hpp
        log.hpp
        log1p.hpp
        log2.hpp
        log10.hpp
)
