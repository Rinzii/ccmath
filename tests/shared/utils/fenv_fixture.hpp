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

#include <gtest/gtest.h>

#include <array>
#include <cfenv>
#include <cstdio>

namespace ccm::test
{
	enum class RoundingModeKind
	{
		Nearest,
		Upward,
		Downward,
		TowardZero,
	};

	inline constexpr std::array<int, 4> kStdRoundingModes = { FE_TONEAREST, FE_UPWARD, FE_DOWNWARD, FE_TOWARDZERO };

	inline constexpr const char * RoundingModeName(int rounding_mode)
	{
		switch (rounding_mode)
		{
		case FE_TONEAREST : return "FE_TONEAREST";
		case FE_UPWARD	  : return "FE_UPWARD";
		case FE_DOWNWARD  : return "FE_DOWNWARD";
		case FE_TOWARDZERO: return "FE_TOWARDZERO";
		default			  : return "unknown";
		}
	}

	inline RoundingModeKind ToRoundingModeKind(int rounding_mode)
	{
		switch (rounding_mode)
		{
		case FE_UPWARD	  : return RoundingModeKind::Upward;
		case FE_DOWNWARD  : return RoundingModeKind::Downward;
		case FE_TOWARDZERO: return RoundingModeKind::TowardZero;
		default			  : return RoundingModeKind::Nearest;
		}
	}

	inline bool TrySetRoundingMode(int rounding_mode)
	{
		if (std::fesetround(rounding_mode) != 0)
		{
			return false;
		}
		return std::fegetround() == rounding_mode;
	}

	inline bool FenvIsSupported()
	{
		const int saved = std::fegetround();
		for (int mode : kStdRoundingModes)
		{
			if (!TrySetRoundingMode(mode))
			{
				std::fesetround(saved);
				return false;
			}
		}
		std::fesetround(saved);
		return true;
	}

	class ScopedRoundingMode
	{
	public:
		explicit ScopedRoundingMode(int rounding_mode) : saved_(std::fegetround()), active_(TrySetRoundingMode(rounding_mode)) {}

		ScopedRoundingMode(const ScopedRoundingMode &)			   = delete;
		ScopedRoundingMode & operator=(const ScopedRoundingMode &) = delete;

		~ScopedRoundingMode()
		{
			if (active_)
			{
				std::fesetround(saved_);
			}
		}

		[[nodiscard]] bool active() const
		{
			return active_;
		}

	private:
		int saved_;
		bool active_;
	};

	class ScopedFenvEnvironment
	{
	public:
		ScopedFenvEnvironment() : active_(std::fegetenv(&saved_) == 0) {}

		ScopedFenvEnvironment(const ScopedFenvEnvironment &)			 = delete;
		ScopedFenvEnvironment & operator=(const ScopedFenvEnvironment &) = delete;

		~ScopedFenvEnvironment()
		{
			if (active_)
			{
				std::fesetenv(&saved_);
			}
		}

		[[nodiscard]] bool active() const
		{
			return active_;
		}

	private:
		std::fenv_t saved_{};
		bool active_;
	};

	inline bool ClearFenvExceptions()
	{
		return std::feclearexcept(FE_ALL_EXCEPT) == 0;
	}

	inline int CurrentFenvExceptions()
	{
		return std::fetestexcept(FE_ALL_EXCEPT);
	}

	/// LLVM-libc-style RAII wrapper. Restores the previous mode on destruction.
	class ForceRoundingMode
	{
	public:
		explicit ForceRoundingMode(int rounding_mode) : scope_(rounding_mode) {}

		[[nodiscard]] bool succeeded() const
		{
			return scope_.active();
		}

		explicit operator bool() const
		{
			return scope_.active();
		}

	private:
		ScopedRoundingMode scope_;
	};

	template <typename Fn> void ForEachRoundingMode(Fn && fn)
	{
		for (int mode : kStdRoundingModes)
		{
			SCOPED_TRACE(RoundingModeName(mode));
			ScopedRoundingMode scope(mode);
			if (!scope.active())
			{
				ADD_FAILURE() << "failed to set " << RoundingModeName(mode);
				continue;
			}
			fn(mode);
		}
	}

	template <typename Fn> void ForEachRoundingModeOrSkip(Fn && fn)
	{
		if (!FenvIsSupported())
		{
			GTEST_SKIP() << "floating-point rounding modes are not supported on this platform";
		}
		ForEachRoundingMode(std::forward<Fn>(fn));
	}

} // namespace ccm::test
