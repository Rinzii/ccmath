/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "common.hpp"
#include "simd_vectorize.hpp"

namespace ccm::intrin
{

	namespace abi
	{

		template <int N>
		struct pack;
	} // namespace abi

	template <int N>
	struct simd_mask<float, abi::pack<N>>
	{
		using value_type					 = bool;
		using simd_type						 = simd<float, abi::pack<N>>;
		using abi_type						 = abi::pack<N>;
		CCM_ALWAYS_INLINE inline simd_mask() = default;
		[[nodiscard]] static constexpr int size() { return N; }
		CCM_ALWAYS_INLINE inline explicit simd_mask(bool value)
		{
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				m_value[i] = value;
			}
		}
		constexpr bool operator[](int i) const { return m_value[i]; }
		CCM_ALWAYS_INLINE inline int & operator[](int i) { return m_value[i]; }
		CCM_ALWAYS_INLINE inline simd_mask operator||(simd_mask const & other) const
		{
			simd_mask result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result.m_value[i] = m_value[i] || other.m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE inline simd_mask operator&&(simd_mask const & other) const
		{
			simd_mask result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result.m_value[i] = m_value[i] && other.m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE inline simd_mask operator!() const
		{
			simd_mask result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result.m_value[i] = !m_value[i];
			}
			return result;
		}

	private:
		std::array<int, N> m_value;
	};

	template <int N>
	struct simd_mask<double, abi::pack<N>>
	{
		using value_type					 = bool;
		using simd_type						 = simd<double, abi::pack<N>>;
		using abi_type						 = abi::pack<N>;
		CCM_ALWAYS_INLINE inline simd_mask() = default;
		[[nodiscard]] static constexpr int size() { return N; }
		CCM_ALWAYS_INLINE inline explicit simd_mask(bool value)
		{
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				m_value[i] = value;
			}
		}
		constexpr bool operator[](int i) const { return m_value[i]; }
		CCM_ALWAYS_INLINE inline std::int64_t & operator[](int i) { return m_value[i]; }
		CCM_ALWAYS_INLINE inline simd_mask operator||(simd_mask const & other) const
		{
			simd_mask result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result.m_value[i] = m_value[i] || other.m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE inline simd_mask operator&&(simd_mask const & other) const
		{
			simd_mask result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result.m_value[i] = m_value[i] && other.m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE inline simd_mask operator!() const
		{
			simd_mask result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result.m_value[i] = !m_value[i];
			}
			return result;
		}

	private:
		std::array<std::int64_t, N> m_value;
	};

	template <class T, int N>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline bool all_of(simd_mask<T, abi::pack<N>> const & a)
	{
		bool result = true;
		CCM_SIMD_VECTORIZE for (int i = 0; i < a.size(); ++i)
		{
			result = result && a[i];
		}
		return result;
	}

	template <class T, int N>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline bool any_of(simd_mask<T, abi::pack<N>> const & a)
	{
		bool result = false;
		CCM_SIMD_VECTORIZE for (int i = 0; i < a.size(); ++i)
		{
			result = result || a[i];
		}
		return result;
	}

	template <class T, int N>
	struct simd<T, abi::pack<N>>
	{

		using value_type				= T;
		using abi_type					= abi::pack<N>;
		using mask_type					= simd_mask<T, abi_type>;
		using storage_type				= simd_storage<T, abi_type>;
		CCM_ALWAYS_INLINE inline simd() = default;
		[[nodiscard]] static constexpr int size() { return N; }
		CCM_ALWAYS_INLINE inline explicit simd(T value)
		{
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				m_value[i] = value;
			}
		}
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline explicit simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE simd(T const * ptr, Flags flags)
		{
			copy_from(ptr, flags);
		}
		CCM_ALWAYS_INLINE simd operator*(simd const & other) const
		{
			simd result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result[i] = m_value[i] * other.m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE simd operator/(simd const & other) const
		{
			simd result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result[i] = m_value[i] / other.m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE simd operator+(simd const & other) const
		{
			simd result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result[i] = m_value[i] + other.m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE simd operator-(simd const & other) const
		{
			simd result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result[i] = m_value[i] - other.m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator-() const
		{
			simd result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result[i] = -m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE void copy_from(T const * ptr, element_aligned_tag /*unused*/)
		{
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				m_value[i] = ptr[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			}
		}
		CCM_ALWAYS_INLINE void copy_to(T * ptr, element_aligned_tag /*unused*/) const
		{
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				ptr[i] = m_value[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			}
		}
		CCM_ALWAYS_INLINE constexpr T operator[](int i) const { return m_value[i]; }
		CCM_ALWAYS_INLINE T & operator[](int i) { return m_value[i]; }
		CCM_ALWAYS_INLINE simd_mask<T, abi::pack<N>> operator<(simd const & other) const
		{
			simd_mask<T, abi::pack<N>> result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result[i] = m_value[i] < other.m_value[i];
			}
			return result;
		}
		CCM_ALWAYS_INLINE simd_mask<T, abi::pack<N>> operator==(simd const & other) const
		{
			simd_mask<T, abi::pack<N>> result;
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				result[i] = m_value[i] == other.m_value[i];
			}
			return result;
		}

	private:
		std::array<T, N> m_value;
	};

	template <class T, int N>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, abi::pack<N>> choose(simd_mask<T, abi::pack<N>> const & a, simd<T, abi::pack<N>> const & b,
																			  simd<T, abi::pack<N>> const & c)
	{
		simd<T, abi::pack<N>> result;
		CCM_SIMD_VECTORIZE for (int i = 0; i < a.size(); ++i)
		{
			result[i] = a[i] ? b[i] : c[i];
		}
		return result;
	}

} // namespace ccm::intrin