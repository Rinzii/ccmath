/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "common.hpp"

namespace ccm::intrin
{
	namespace abi
	{
		struct scalar
		{
		};
	} // namespace abi

	template <class T>
	struct simd_mask<T, abi::scalar>
	{
		using value_type					 = bool;
		using simd_type						 = simd<T, abi::scalar>;
		using abi_type						 = abi::scalar;
		CCM_ALWAYS_INLINE inline simd_mask() = default;
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE static constexpr int size() { return 1; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline explicit simd_mask(bool value) : m_value(value) {}
		[[nodiscard]] CCM_GPU_HOST_DEVICE constexpr bool get() const { return m_value; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd_mask operator||(simd_mask const & other) const { return m_value || other.m_value; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd_mask operator&&(simd_mask const & other) const { return m_value && other.m_value; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd_mask operator!() const { return !m_value; }

	private:
		bool m_value;
	};

	template <class T>
	struct simd_storage<T, abi::scalar>
	{
	private:
		using Abi = abi::scalar;

	public:
		using value_type						= T;
		using simd_type							= simd<T, Abi>;
		CCM_ALWAYS_INLINE inline simd_storage() = default;
		static constexpr int size() { return simd<T, Abi>::size(); }
		CCM_ALWAYS_INLINE explicit CCM_GPU_HOST_DEVICE simd_storage(simd<T, Abi> const & value) CCM_GPU_HOST_DEVICE
		{
			value.copy_to(&m_value, element_aligned_tag());
		}
		CCM_ALWAYS_INLINE explicit CCM_GPU_HOST_DEVICE simd_storage(T value) : simd_storage(simd<T, Abi>(value)) {}
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE simd_storage & operator=(simd<T, Abi> const & value)
		{
			value.copy_to(&m_value, element_aligned_tag());
			return *this;
		}
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE T const * data() const { return &m_value; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE T * data() { return &m_value; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE T const & operator[](int /*unused*/) const { return m_value; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE T & operator[](int /*unused*/) { return m_value; }

	private:
		T m_value;

	};

	template <class T>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline bool all_of(simd_mask<T, abi::scalar> const & a)
	{
		return a.get();
	}

	template <class T>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline bool any_of(simd_mask<T, abi::scalar> const & a)
	{
		return a.get();
	}

	template <class T>
	struct simd<T, abi::scalar>
	{
		using value_type										= T;
		using abi_type											= abi::scalar;
		using mask_type											= simd_mask<T, abi_type>;
		using storage_type										= simd_storage<T, abi_type>;
		CCM_ALWAYS_INLINE inline simd()							= default;
		CCM_ALWAYS_INLINE inline simd(simd const &)				= default;
		CCM_ALWAYS_INLINE inline simd(simd &&)					= default;
		CCM_ALWAYS_INLINE inline simd & operator=(simd const &) = default;
		CCM_ALWAYS_INLINE inline simd & operator=(simd &&)		= default;
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE static constexpr int size() { return 1; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd(T value) : m_value(value) {}
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd(T const * ptr, Flags flags)
		{
			copy_from(ptr, flags);
		}
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd(T const * ptr, int /*stride*/) : m_value(ptr[0]) {}
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator*(simd const & other) const { return simd(m_value * other.m_value); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator/(simd const & other) const { return simd(m_value / other.m_value); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator+(simd const & other) const { return simd(m_value + other.m_value); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator-(simd const & other) const { return simd(m_value - other.m_value); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator-() const { return simd(-m_value); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE void copy_from(T const * ptr, element_aligned_tag /*unused*/) { m_value = *ptr; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE void copy_to(T * ptr, element_aligned_tag /*unused*/) const { *ptr = m_value; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE constexpr T get() const { return m_value; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE T convert() const { return m_value; }

		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd_mask<T, abi::scalar> operator<(simd const & other) const
		{
			return simd_mask<T, abi::scalar>(m_value < other.m_value);
		}
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd_mask<T, abi::scalar> operator==(simd const & other) const
		{
			return simd_mask<T, abi::scalar>(m_value == other.m_value);
		}

	private:
		T m_value;
	};

	template <class T>
	CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd<T, abi::scalar> choose(simd_mask<T, abi::scalar> const & a, simd<T, abi::scalar> const & b,
																			 simd<T, abi::scalar> const & c)
	{
		return simd<T, abi::scalar>(choose(a.get(), b.get(), c.get()));
	}
} // namespace ccm::intrin