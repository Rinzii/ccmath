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

#include <limits>

namespace ccm::config
{
	enum class LongDoubleFormat
	{
		Double,
		X87Extended,
		IEEEBinary128,
		Unknown,
	};

	constexpr LongDoubleFormat detect_long_double_format() noexcept
	{
		constexpr int digits = std::numeric_limits<long double>::digits;
		constexpr int max_exponent = std::numeric_limits<long double>::max_exponent;

		if (digits == 53 && max_exponent == 1024) { return LongDoubleFormat::Double; }
		if (digits == 64 && max_exponent == 16384) { return LongDoubleFormat::X87Extended; }
		if (digits == 113 && max_exponent == 16384) { return LongDoubleFormat::IEEEBinary128; }
		return LongDoubleFormat::Unknown;
	}

	constexpr const char * long_double_format_name(LongDoubleFormat format) noexcept
	{
		switch (format)
		{
		case LongDoubleFormat::Double: return "double_shaped";
		case LongDoubleFormat::X87Extended: return "x87_float80";
		case LongDoubleFormat::IEEEBinary128: return "ieee_binary128";
		case LongDoubleFormat::Unknown: return "unknown";
		}
		return "unknown";
	}

} // namespace ccm::config
