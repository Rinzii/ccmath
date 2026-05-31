# Lowest Compiler Support

## ccm::signbit()

* GCC 5.1+ (6.1+ with __builtin_signbit)
* Clang 5.0.0+
* MSVC 19.14+ (19.27+ with __builtin_bit_cast) - Note: MSVC 19.14+ does not allow static_assert-able constexpr. MSVC
  19.27+ does.
* Intel DPC++/C++ 2021.1.2 (Can't test lower than this)
* Nvidia HPC C++ 22.7 (Can't test lower than this)

