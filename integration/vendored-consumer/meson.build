project(
    'vendored-ccmath-consumer',
    'cpp',
    version: '0.0.0',
    meson_version: '>=1.0.0',
    default_options: ['cpp_std=c++17'],
)

ccmath_inc = include_directories('third_party/ccmath/include')

executable(
    'vendored-ccmath-consumer',
    'main.cpp',
    include_directories: ccmath_inc,
)
