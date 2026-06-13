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

#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

namespace ccm::fuzz
{
	// The x86 80-bit extended long double stores an explicit integer bit, so most bit patterns
	// are non-canonical encodings: unnormals, pseudo-NaN, pseudo-infinity and pseudo-denormals.
	// Both the C++ library and the x87 hardware give unspecified results for those, so comparing
	// ccmath against the system libm on them is not a meaningful differential test (the same
	// soundness reason the harness does not cycle rounding modes). Raw byte mutation lands on
	// them constantly, so reject them. Every binary32/binary64/binary128 bit pattern is a valid
	// encoding, so this is a no-op for those types.
	template <typename T>
	bool is_canonical_encoding(T value)
	{
		if constexpr (std::numeric_limits<T>::digits == 64 && sizeof(T) >= 10)
		{
			unsigned char bytes[sizeof(T)];
			std::memcpy(bytes, &value, sizeof(T));
			const bool integer_bit	= (bytes[7] & 0x80U) != 0;
			const unsigned exponent = (static_cast<unsigned>(bytes[9] & 0x7FU) << 8) | bytes[8];
			// Canonical iff the integer bit is set exactly when the biased exponent is non-zero.
			return integer_bit == (exponent != 0);
		}
		else
		{
			static_cast<void>(value);
			return true;
		}
	}

	// ISO C does not specify how the math functions treat signaling NaN operands, so the system
	// libm used as the differential oracle disagrees with itself across platforms for them: for
	// example glibc returns NaN for pow(sNaN, 0) while the Annex F rule that ccmath follows
	// returns 1. Reject signaling-NaN operands so the comparison only covers behavior the
	// standard actually pins down (quiet NaN classification is still exercised).
	template <typename T>
	bool is_testable_operand(T value)
	{ return is_canonical_encoding(value) && !ccm::support::fp::FPBits<T>(value).is_signaling_nan(); }

	template <typename T>
	T load_floating(uint8_t const * data, size_t size, size_t byte_offset = 0)
	{
		static_assert(std::is_floating_point_v<T>, "T must be floating-point");

		T value{};
		if (byte_offset + sizeof(T) > size) { return value; }

		std::memcpy(&value, data + byte_offset, sizeof(T));
		return value;
	}

	inline int32_t load_i32(uint8_t const * data, size_t size, size_t byte_offset = 0)
	{
		int32_t value{};
		if (byte_offset + sizeof(value) > size) { return 0; }

		std::memcpy(&value, data + byte_offset, sizeof(value));
		return value;
	}

	// Adversarial values raw byte mutation essentially never lands on exactly: format
	// boundaries, integer and half-integer parity thresholds, kernel range-reduction and
	// clamp constants. Indexed by the low selector bits so every entry stays reachable.
	template <typename T>
	T special_value(uint8_t index)
	{
		using limits = std::numeric_limits<T>;
		switch (index % 24)
		{
		case 0: return T(0);
		case 1: return limits::denorm_min();
		case 2: return limits::min() - limits::denorm_min(); // largest subnormal
		case 3: return limits::min();
		case 4: return T(1);
		case 5: return T(1) + limits::epsilon();
		case 6: return T(1) - limits::epsilon() / T(2); // largest value below one
		case 7: return T(2);
		case 8: return T(10);
		case 9: return T(0.5);
		case 10: return limits::max();
		case 11: return limits::infinity();
		case 12: return limits::quiet_NaN();
		case 13: return limits::signaling_NaN();
		case 14: return T(1) / limits::epsilon();				// integer parity threshold 2^p
		case 15: return T(2) / limits::epsilon();				// 2^(p+1), everything above is even
		case 16: return T(2.5);									// half integer
		case 17: return T(1023.5);								// pow half-integer bound region
		case 18: return T(1024);								// pow integer ipow bound
		case 19: return T(2048);								// pow two_exp bound
		case 20: return static_cast<T>(0x1.74910d52d3052p+62); // pow huge-exponent clamp threshold
		case 21: return static_cast<T>(0x1.62e42fefa39efp+9);	// near the exp overflow boundary
		case 22: return T(0x1.0p-100);
		case 23: return T(0x1.0p100);
		default: return T(0);
		}
	}

	// Optional per-operand selector bytes trail the raw operands. A set high bit swaps the
	// operand for a table entry; bit 6 flips the sign. Inputs without trailing bytes (and
	// every pre-existing seed) decode exactly as before.
	template <typename T>
	void apply_selector(T & value, uint8_t const * data, size_t size, size_t selector_offset)
	{
		if (selector_offset >= size) { return; }
		const uint8_t selector = data[selector_offset];
		if ((selector & 0x80U) == 0) { return; }
		T substituted = special_value<T>(static_cast<uint8_t>(selector & 0x3FU));
		if ((selector & 0x40U) != 0) { substituted = -substituted; }
		value = substituted;
	}

	template <typename T>
	struct Inputs
	{
		T x{};
		T y{};
		T z{};
		int32_t i{};

		bool load_x(uint8_t const * data, size_t size)
		{
			if (size < sizeof(T)) { return false; }
			x = load_floating<T>(data, size, 0);
			apply_selector(x, data, size, sizeof(T));
			return is_testable_operand(x);
		}

		bool load_xy(uint8_t const * data, size_t size)
		{
			if (size < 2 * sizeof(T)) { return false; }
			x = load_floating<T>(data, size, 0);
			y = load_floating<T>(data, size, sizeof(T));
			apply_selector(x, data, size, 2 * sizeof(T));
			apply_selector(y, data, size, 2 * sizeof(T) + 1);
			return is_testable_operand(x) && is_testable_operand(y);
		}

		bool load_xyz(uint8_t const * data, size_t size)
		{
			if (size < 3 * sizeof(T)) { return false; }
			x = load_floating<T>(data, size, 0);
			y = load_floating<T>(data, size, sizeof(T));
			z = load_floating<T>(data, size, 2 * sizeof(T));
			apply_selector(x, data, size, 3 * sizeof(T));
			apply_selector(y, data, size, 3 * sizeof(T) + 1);
			apply_selector(z, data, size, 3 * sizeof(T) + 2);
			return is_testable_operand(x) && is_testable_operand(y) && is_testable_operand(z);
		}

		bool load_xy_i(uint8_t const * data, size_t size)
		{
			if (size < 2 * sizeof(T) + sizeof(int32_t)) { return false; }
			x = load_floating<T>(data, size, 0);
			y = load_floating<T>(data, size, sizeof(T));
			i = load_i32(data, size, 2 * sizeof(T));
			apply_selector(x, data, size, 2 * sizeof(T) + sizeof(int32_t));
			apply_selector(y, data, size, 2 * sizeof(T) + sizeof(int32_t) + 1);
			return is_testable_operand(x) && is_testable_operand(y);
		}
	};

} // namespace ccm::fuzz
