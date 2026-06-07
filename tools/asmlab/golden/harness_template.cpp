/*
 * asmlab golden calibration harness template.
 * Tool-owned fixture only. Not part of CCMath.
 */

#include "@@KERNEL_HEADER@@"

#if defined(__GNUC__) || defined(__clang__)
#define ASMLAB_FLATTEN @@FLATTEN_ATTR@@
#else
#define ASMLAB_FLATTEN
#endif

extern "C" volatile unsigned long long asmlab_opaque_seed;

extern "C" ASMLAB_FLATTEN @@RET@@ @@SYMBOL@@(@@PARAMS@@)
{
	@@VOLATILE_LOADS@@
	return @@EXPR@@;
}
