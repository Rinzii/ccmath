/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cstdint>

#include "ccmath/internal/helpers/not_null.hpp"

#ifndef NON_NULL
	#if defined(__has_attribute)
		#if __has_attribute(nonnull)
			#define NON_NULL __attribute__((nonnull))
		#else
			#define NON_NULL
		#endif
	#else
		#define NON_NULL
	#endif
#endif

namespace ccm::helpers
{

	constexpr uint64_t make_mantissa_base8(const char * tagp NON_NULL)
	{
		uint64_t r = 0;
		while (*tagp != '\0')
		{
			char tmp = *tagp;

			if (tmp >= '0' && tmp <= '7')
				r = (r * 8u) + static_cast<uint64_t>(tmp) - '0';
			else
				return 0;

			++tagp;
		}

		return r; // This should never be reached
	}

	constexpr uint64_t make_mantissa_base10(const char * tagp NON_NULL)
	{
		uint64_t r = 0;
		while (*tagp != '\0')
		{
			char tmp = *tagp;

			if (tmp >= '0' && tmp <= '9')
				r = (r * 10u) + static_cast<uint64_t>(tmp) - '0';
			else
				return 0;

			++tagp;
		}

		return r; // This should never be reached
	}

	constexpr uint64_t make_mantissa_base16(const char * tagp NON_NULL)
	{
		uint64_t r = 0;
		while (*tagp != '\0')
		{
			char tmp = *tagp;

			if (tmp >= '0' && tmp <= '9')
				r = (r * 16u) + static_cast<uint64_t>(tmp) - '0';
			else if (tmp >= 'a' && tmp <= 'f')
				r = (r * 16u) + static_cast<uint64_t>(tmp) - 'a' + 10;
			else if (tmp >= 'A' && tmp <= 'F')
				r = (r * 16u) + static_cast<uint64_t>(tmp) - 'A' + 10;
			else
				return 0;

			++tagp;
		}

		return r; // This should never be reached
	}

	constexpr uint64_t make_mantissa(const char * tagp NON_NULL)
	{
		if (*tagp == '0')
		{
			++tagp;

			if (*tagp == 'x' || *tagp == 'X')
				return make_mantissa_base16(tagp);
			else
				return make_mantissa_base8(tagp);
		}

		return make_mantissa_base10(tagp);
	}

} // namespace ccm::helpers

#undef NON_NULL
