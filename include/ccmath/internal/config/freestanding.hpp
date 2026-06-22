/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

// Single point of truth for whether ccmath is building for a freestanding C++ environment.
//
// A freestanding environment (a console SDK, an embedded target, a kernel) provides only the
// freestanding subset of the standard library. ccmath stays inside that subset: every standard
// header it pulls into the library is a freestanding header in C++26 (Table 27 in [compliance]),
// including <cerrno>, and ccmath only ever touches the non-throwing members of the partially
// freestanding ones. The host floating-point environment is optional and resolved separately in
// host_fenv.hpp, which uses the host <cfenv> / <fenv.h> when the toolchain ships them and degrades
// to round-to-nearest when it does not. The only headers a freestanding build deliberately drops
// are the test-only <bitset> path in the runtime SIMD layer and the MSVC <math.h> system-math path.
//
// CCM_CONFIG_FREESTANDING is auto-detected from __STDC_HOSTED__ but can be set either way by hand:
// define CCM_CONFIG_FREESTANDING to force it on (the build option does this), or define
// CCM_CONFIG_HOSTED to force it off on a toolchain that reports freestanding. Like every other
// CCM_CONFIG_ flag it is presence based, so define it, do not set it to a value.

#if defined(CCM_CONFIG_FREESTANDING)
// Forced on: keep it defined and skip detection.
#elif defined(CCM_CONFIG_HOSTED)
// Forced off: leave CCM_CONFIG_FREESTANDING undefined so every consumer takes the hosted branch.
#elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0)
	#define CCM_CONFIG_FREESTANDING 1
#endif
