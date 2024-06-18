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

#if (defined(__clang__) && (__clang_major__ >= 11))

	#ifndef CCMATH_SIMD_ENABLE_VECTOR_SIZE
		#define CCMATH_SIMD_ENABLE_VECTOR_SIZE
	#endif

namespace ccm::intrin
{
	namespace abi
	{

		template <int N>
		struct vector_size
		{
		};

	} // namespace abi

	template <class T, int N>
	struct simd_mask<T, abi::vector_size<N>>
	{
	private:
		using native_type =
			__attribute__((__vector_size__(N / (sizeof(T) / sizeof(int)) * sizeof(int)))) int __attribute__((vector_size(N / (sizeof(T) / sizeof(int)))));

	public:
		using value_type					 = bool;
		using simd_type						 = simd<float, abi::vector_size<N>>;
		using abi_type						 = abi::vector_size<N>;
		CCM_ALWAYS_INLINE inline simd_mask() = default;
		static constexpr int size() { return N / sizeof(T); }
		CCM_ALWAYS_INLINE inline explicit simd_mask(bool value) : m_value(static_cast<int>(value)) {}
		CCM_ALWAYS_INLINE inline explicit simd_mask(native_type value) : m_value(value) {}
		CCM_ALWAYS_INLINE inline int operator[](int i) { return reinterpret_cast<int *>(&m_value)[i]; }
		[[nodiscard]] CCM_ALWAYS_INLINE inline native_type const & get() const { return m_value; }
		CCM_ALWAYS_INLINE inline simd_mask operator||(simd_mask const & other) const { return simd_mask(m_value || other.m_value); }
		CCM_ALWAYS_INLINE inline simd_mask operator&&(simd_mask const & other) const { return simd_mask(m_value && other.m_value); }
		CCM_ALWAYS_INLINE inline simd_mask operator!() const { return simd_mask(!m_value); }

	private:
		native_type m_value;
	};

	template <int N>
	struct simd_mask<double, abi::vector_size<N>>
	{
	private:
		using native_type = __attribute__((__vector_size__(N * sizeof(long long)))) long long __attribute__((vector_size(N)));

	public:
		using value_type					 = bool;
		using simd_type						 = simd<double, abi::vector_size<N>>;
		using abi_type						 = abi::vector_size<N>;
		CCM_ALWAYS_INLINE inline simd_mask() = default;
		static constexpr int size() { return N / sizeof(long long); }
		CCM_ALWAYS_INLINE inline explicit simd_mask(bool value) : m_value(static_cast<long long>(value)) {}
		CCM_ALWAYS_INLINE inline explicit simd_mask(native_type value) : m_value(value) {}
		CCM_ALWAYS_INLINE inline long long operator[](int i) { return reinterpret_cast<long long *>(&m_value)[i]; }
		[[nodiscard]] CCM_ALWAYS_INLINE inline native_type const & get() const { return m_value; }
		CCM_ALWAYS_INLINE inline simd_mask operator||(simd_mask const & other) const { return simd_mask(m_value || other.m_value); }
		CCM_ALWAYS_INLINE inline simd_mask operator&&(simd_mask const & other) const { return simd_mask(m_value && other.m_value); }
		CCM_ALWAYS_INLINE inline simd_mask operator!() const { return simd_mask(!m_value); }

	private:
		native_type m_value;
	};

	template <class T, int N>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline bool all_of(simd_mask<T, abi::vector_size<N>> const & a)
	{
		bool result = true;
		CCM_SIMD_VECTORIZE for (int i = 0; i < a.size(); ++i)
		{
			result = result && a.get()[i];
		}
		return result;
	}

	template <class T, int N>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline bool any_of(simd_mask<T, abi::vector_size<N>> const & a)
	{
		bool result = false;
		CCM_SIMD_VECTORIZE for (int i = 0; i < a.size(); ++i)
		{
			result = result || a.get()[i];
		}
		return result;
	}

	template <class T, int N>
	struct simd<T, abi::vector_size<N>>
	{
	private:
		using native_type = __attribute__((__vector_size__(N * sizeof(T)))) T __attribute__((vector_size(N)));

	public:
		using value_type				= T;
		using abi_type					= abi::vector_size<N>;
		using mask_type					= simd_mask<T, abi_type>;
		using storage_type				= simd_storage<T, abi_type>;
		CCM_ALWAYS_INLINE inline simd() = default;
		static constexpr int size() { return N / sizeof(T); }
		CCM_ALWAYS_INLINE inline explicit simd(T value)
		{
			for (int i = 0; i < size(); i++) { reinterpret_cast<T *>(&m_value)[i] = value; }
		}
		explicit CCM_ALWAYS_INLINE simd(const native_type & value) : m_value(value) {}
		CCM_ALWAYS_INLINE inline explicit simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
		CCM_ALWAYS_INLINE inline simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE simd(T const * ptr, Flags flags)
		{
			copy_from(ptr, flags);
		}
		CCM_ALWAYS_INLINE simd operator*(simd const & other) const { return simd(m_value * other.m_value); }
		CCM_ALWAYS_INLINE simd operator/(simd const & other) const { return simd(m_value / other.m_value); }
		CCM_ALWAYS_INLINE simd operator+(simd const & other) const { return simd(m_value + other.m_value); }
		CCM_ALWAYS_INLINE simd operator+=(simd const & other) const { return m_value += other.m_value; }
		CCM_ALWAYS_INLINE simd operator-(simd const & other) const { return simd(m_value - other.m_value); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator-() const { return simd(-m_value); }
		CCM_ALWAYS_INLINE void copy_from(T const * ptr, element_aligned_tag /*unused*/)
		{
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				reinterpret_cast<T *>(&m_value)[i] = ptr[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			}
		}
		CCM_ALWAYS_INLINE void copy_to(T * ptr, element_aligned_tag /*unused*/) const
		{
			CCM_SIMD_VECTORIZE for (int i = 0; i < size(); ++i)
			{
				ptr[i] = reinterpret_cast<T *>(&m_value)[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			}
		}
		CCM_ALWAYS_INLINE constexpr T operator[](int i) const { return m_value[i]; }
		[[nodiscard]] CCM_ALWAYS_INLINE native_type const & get() const { return m_value; }
		CCM_ALWAYS_INLINE native_type & get() { return m_value; }
		CCM_ALWAYS_INLINE simd_mask<T, abi::vector_size<N>> operator<(simd const & other) const
		{
			return simd_mask<T, abi::vector_size<N>>(m_value < other.m_value);
		}
		CCM_ALWAYS_INLINE simd_mask<T, abi::vector_size<N>> operator==(simd const & other) const
		{
			return simd_mask<T, abi::vector_size<N>>(m_value == other.m_value);
		}

	private:
		native_type m_value;
	};

	template <class T, int N>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, abi::vector_size<N>>
	choose(simd_mask<T, abi::vector_size<N>> const & a, simd<T, abi::vector_size<N>> const & b, simd<T, abi::vector_size<N>> const & c)
	{
		simd<T, abi::vector_size<N>> result;
		CCM_SIMD_VECTORIZE for (int i = 0; i < a.size(); ++i)
		{
			result.get()[i] = a.get()[i] ? b.get()[i] : c.get()[i];
		}
		return result;
	}

	template <class T, int N>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, abi::vector_size<N>> max(simd<T, abi::vector_size<N>> const & a,
																				  simd<T, abi::vector_size<N>> const & b)
	{
		return choose(b < a, a, b);
	}

	template <class T, int N>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, abi::vector_size<N>> min(simd<T, abi::vector_size<N>> const & a,
																				  simd<T, abi::vector_size<N>> const & b)
	{
		return choose(a < b, a, b);
	}

} // namespace ccm::intrin

#endif