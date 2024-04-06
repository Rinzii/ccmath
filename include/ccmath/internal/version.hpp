/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <string>

namespace ccm
{
	struct Version
	{
	public:
		constexpr Version(int major, int minor, int patch) : m_major(major), m_minor(minor), m_patch(patch) {}

		[[nodiscard]] std::string to_string() const { return std::to_string(m_major) + "." + std::to_string(m_minor) + "." + std::to_string(m_patch); }

		[[nodiscard]] constexpr int major() const { return m_major; }
		[[nodiscard]] constexpr int minor() const { return m_minor; }
		[[nodiscard]] constexpr int patch() const { return m_patch; }
		[[nodiscard]] constexpr int full() const { return m_major * 10000 + m_minor * 100 + m_patch; }
		[[nodiscard]] constexpr int get() const { return full(); }

		constexpr bool operator==(const Version & other) const { return m_major == other.m_major && m_minor == other.m_minor && m_patch == other.m_patch; }

		constexpr bool operator!=(const Version & other) const { return !(*this == other); }

		constexpr bool operator<(const Version & other) const
		{
			if (m_major < other.m_major) { return true; }
			if (m_major > other.m_major) { return false; }

			if (m_minor < other.m_minor) { return true; }
			if (m_minor > other.m_minor) { return false; }

			return m_patch < other.m_patch;
		}

		constexpr bool operator>(const Version & other) const { return other < *this; }

		constexpr bool operator<=(const Version & other) const { return !(other < *this); }

		constexpr bool operator>=(const Version & other) const { return !(*this < other); }

	private:
		int m_major;
		int m_minor;
		int m_patch;
	};

} // namespace ccm
