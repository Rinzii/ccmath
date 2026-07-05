#pragma once

#include "inputs.hpp"

#include <benchmark/benchmark.h>

#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

namespace ccm::bench
{
	inline constexpr std::int64_t kRangeMin = 8;
	inline constexpr std::int64_t kRangeMax = 8 << 10;

	template <typename T> T load_arg(T value)
	{
		volatile T storage = value;
		return storage;
	}

	template <typename T, typename Fn> void unary_fixed(benchmark::State & state, T value, Fn && fn)
	{
		T const x = load_arg(value);
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(fn(x));
		}
	}

	template <typename T, typename Fn> void binary_fixed(benchmark::State & state, T x_value, T y_value, Fn && fn)
	{
		T const x = load_arg(x_value);
		T const y = load_arg(y_value);
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(fn(x, y));
		}
	}

	template <typename T, typename Fn> void ternary_fixed(benchmark::State & state, T x_value, T y_value, T z_value, Fn && fn)
	{
		T const x = load_arg(x_value);
		T const y = load_arg(y_value);
		T const z = load_arg(z_value);
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(fn(x, y, z));
		}
	}

	template <typename T, typename Fn> void unary_scalar(benchmark::State & state, Fn && fn)
	{
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(fn(static_cast<T>(state.range(0))));
		}
		state.SetComplexityN(state.range(0));
	}

	template <typename Vec, typename Fn> void unary_vector(benchmark::State & state, Fn && fn, Vec const & inputs)
	{
		for (auto _ : state)
		{
			for (auto const & x : inputs)
			{
				benchmark::DoNotOptimize(fn(x));
			}
		}
		state.SetComplexityN(state.range(0));
	}

	template <typename T, typename Fn> void binary_scalar(benchmark::State & state, Fn && fn)
	{
		T const x = static_cast<T>(state.range(0));
		T const y = static_cast<T>(state.range(0));
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(fn(x, y));
		}
		state.SetComplexityN(state.range(0));
	}

	template <typename T, typename Fn> void ternary_scalar(benchmark::State & state, Fn && fn)
	{
		T const x = static_cast<T>(state.range(0));
		T const y = static_cast<T>(state.range(0));
		T const z = static_cast<T>(state.range(0));
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(fn(x, y, z));
		}
		state.SetComplexityN(state.range(0));
	}

	template <typename Fn, typename VecX, typename VecY, typename VecZ>
	void ternary_vector(benchmark::State & state, Fn && fn, VecX const & xs, VecY const & ys, VecZ const & zs)
	{
		for (auto _ : state)
		{
			for (std::size_t i = 0; i < xs.size(); ++i)
			{
				benchmark::DoNotOptimize(fn(xs[i], ys[i], zs[i]));
			}
		}
		state.SetComplexityN(state.range(0));
	}

	template <typename Fn, typename VecX, typename VecY> void binary_vector(benchmark::State & state, Fn && fn, VecX const & xs, VecY const & ys)
	{
		for (auto _ : state)
		{
			for (std::size_t i = 0; i < xs.size(); ++i)
			{
				benchmark::DoNotOptimize(fn(xs[i], ys[i]));
			}
		}
		state.SetComplexityN(state.range(0));
	}

} // namespace ccm::bench
