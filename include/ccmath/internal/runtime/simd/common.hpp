/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/attributes/always_inline.hpp"
#include "ccmath/internal/predef/attributes/gpu_host_device.hpp"

#include <array>
#include <cstdint>

namespace ccm::intrin
{
	template <typename T, typename Abi>
	struct simd;

	template <class T, class Abi>
	struct simd_mask;

	struct element_aligned_tag
	{
	};

	template <class T>
	CCM_GPU_HOST_DEVICE constexpr T const & choose(bool a, T const & b, T const & c)
	{
		return a ? b : c;
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> & operator+=(simd<T, Abi> & a, simd<T, Abi> const & b)
	{
		a = a + b;
		return a;
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> & operator-=(simd<T, Abi> & a, simd<T, Abi> const & b)
	{
		a = a - b;
		return a;
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> & operator*=(simd<T, Abi> & a, simd<T, Abi> const & b)
	{
		a = a * b;
		return a;
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> & operator/=(simd<T, Abi> & a, simd<T, Abi> const & b)
	{
		a = a / b;
		return a;
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> operator+(T const & a, simd<T, Abi> const & b)
	{
		return simd<T, Abi>(a) + b;
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> operator+(simd<T, Abi> const & a, T const & b)
	{
		return a + simd<T, Abi>(b);
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> operator-(T const & a, simd<T, Abi> const & b)
	{
		return simd<T, Abi>(a) - b;
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> operator-(simd<T, Abi> const & a, T const & b)
	{
		return a - simd<T, Abi>(b);
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> operator*(T const & a, simd<T, Abi> const & b)
	{
		return simd<T, Abi>(a) * b;
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> operator*(simd<T, Abi> const & a, T const & b)
	{
		return a * simd<T, Abi>(b);
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> operator/(T const & a, simd<T, Abi> const & b)
	{
		return simd<T, Abi>(a) / b;
	}

	template <class T, class Abi>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, Abi> operator/(simd<T, Abi> const & a, T const & b)
	{
		return a / simd<T, Abi>(b);
	}

	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline bool all_of(bool a)
	{
		return a;
	}
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline bool any_of(bool a)
	{
		return a;
	}

	template <class T, class Abi>
	struct simd_storage
	{
		using value_type						= T;
		using simd_type							= simd<T, Abi>;

		CCM_ALWAYS_INLINE inline simd_storage() = default;
		static constexpr int size() { return simd<T, Abi>::size(); }
		CCM_ALWAYS_INLINE explicit inline simd_storage(simd<T, Abi> const & value) { value.copy_to(m_value, element_aligned_tag()); }
		CCM_ALWAYS_INLINE explicit inline simd_storage(T value) : simd_storage(simd<T, Abi>(value)) {}
		CCM_ALWAYS_INLINE inline simd_storage & operator=(simd<T, Abi> const & value)
		{
			value.copy_to(m_value, element_aligned_tag());
			return *this;
		}
		CCM_ALWAYS_INLINE inline T const * data() const { return m_value.data(); }
		CCM_ALWAYS_INLINE inline T * data() { return m_value; }
		CCM_ALWAYS_INLINE inline T const & operator[](int i) const { return m_value[i]; }
		CCM_ALWAYS_INLINE inline T & operator[](int i) { return m_value[i]; }

	private:
		std::array<T, simd<T, Abi>::size()> m_value;
	};

	template <class T>
	struct simd_size
	{
		static constexpr int value = 1;
	};

	template <class T, class Abi>
	struct simd_size<simd<T, Abi>>
	{
		static constexpr int value = simd<T, Abi>::size();
	};

} // namespace ccm::intrin