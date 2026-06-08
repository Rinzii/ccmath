#pragma once

namespace ccm::test
{
	// Prevent constexpr evaluation of inputs when exercising runtime dispatch paths.
	template <typename T>
	inline T runtime_value(T value)
	{
		volatile T sink = value;
		return sink;
	}
} // namespace ccm::test
