add_executable(showcase_accuracy showcase_accuracy.cpp)
target_link_libraries(showcase_accuracy PRIVATE ccmath::ccmath ccmath::mpfr_oracle gtest::gtest)
ccmath_showcase_configure_target(showcase_accuracy)
