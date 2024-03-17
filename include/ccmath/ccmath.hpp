/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

/*
CCMATH REQUIREMENTS:
 * The entire library mandates everything MUST be constexpr. If it is not constexpr, it is not part of the library.
 * We do not allow exceptions.
 * We do allow static_asserts.
 * We must mimic the api of the standard cmath library.
 * Anything that could be considered UB is strictly forbidden.
 * Performance is are primary concern. Everything must be as fast as possible.
 * Anything implemented by cmath that is already constexpr is allowed to be wrapped by ccmath and not implemented by ccmath.
 */

// Includes ccm::float_t and ccm::double_t
#include "ccmath/internal/types/fp_types.hpp"

/// Basic math functions
#include "ccmath/basic.hpp"

/// Comparison functions
#include "ccmath/compare.hpp"

/// Exponential functions
#include "ccmath/exponential.hpp"

/// Float manipulation functions
#include "ccmath/fmanip.hpp"

/// Hyperbolic functions
#include "ccmath/hyperbolic.hpp"

/// Nearest functions
#include "ccmath/nearest.hpp"

/// Power functions
#include "ccmath/power.hpp"

/// Special functions
#include "ccmath/special.hpp"

/// Trigonometric functions
#include "ccmath/trig.hpp"

/// Uncategorized functions
#include "ccmath/math/gamma.hpp"
#include "ccmath/math/lerp.hpp"
#include "ccmath/math/lgamma.hpp"
