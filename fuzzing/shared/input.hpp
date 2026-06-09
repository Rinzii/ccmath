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

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace ccm::fuzz
{
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
			return true;
		}

		bool load_xy(uint8_t const * data, size_t size)
		{
			if (size < 2 * sizeof(T)) { return false; }
			x = load_floating<T>(data, size, 0);
			y = load_floating<T>(data, size, sizeof(T));
			return true;
		}

		bool load_xyz(uint8_t const * data, size_t size)
		{
			if (size < 3 * sizeof(T)) { return false; }
			x = load_floating<T>(data, size, 0);
			y = load_floating<T>(data, size, sizeof(T));
			z = load_floating<T>(data, size, 2 * sizeof(T));
			return true;
		}

		bool load_xy_i(uint8_t const * data, size_t size)
		{
			if (size < 2 * sizeof(T) + sizeof(int32_t)) { return false; }
			x = load_floating<T>(data, size, 0);
			y = load_floating<T>(data, size, sizeof(T));
			i = load_i32(data, size, 2 * sizeof(T));
			return true;
		}
	};

} // namespace ccm::fuzz
