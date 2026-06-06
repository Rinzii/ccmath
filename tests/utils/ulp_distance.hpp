#pragma once

#include "ccmath/internal/support/fp/fp_bits.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>

namespace ccm::test::ulp
{
	enum class relation
	{
		finite,
		both_nan,
		nan_mismatch,
		both_infinity_same_sign,
		infinity_mismatch,
	};

	template <typename T>
	struct distance_result
	{
		static_assert(std::is_floating_point_v<T>, "T must be floating-point");

		relation kind = relation::finite;
		bool exact_bitwise_equal = false;
		bool numerically_equal = false;
		std::uint64_t distance = 0;
	};

	template <typename Storage>
	constexpr std::uint64_t saturating_distance(Storage lhs, Storage rhs)
	{
		const Storage diff = (lhs > rhs) ? (lhs - rhs) : (rhs - lhs);
		if constexpr (sizeof(Storage) <= sizeof(std::uint64_t))
		{
			return static_cast<std::uint64_t>(diff);
		}
		else
		{
			return (diff > static_cast<Storage>(std::numeric_limits<std::uint64_t>::max()))
					   ? std::numeric_limits<std::uint64_t>::max()
					   : static_cast<std::uint64_t>(diff);
		}
	}

	template <typename T>
	constexpr auto ordered_bits(ccm::support::fp::FPBits<T> bits)
	{
		using fp_bits = ccm::support::fp::FPBits<T>;
		using storage_type = typename fp_bits::storage_type;

		const storage_type raw = bits.uintval();
		if ((raw & fp_bits::sign_mask) != 0)
		{
			const storage_type magnitude = raw & ~fp_bits::sign_mask;
			return fp_bits::sign_mask - magnitude;
		}
		return fp_bits::sign_mask + raw;
	}

	template <typename T>
	constexpr distance_result<T> classify_distance(T lhs, T rhs)
	{
		static_assert(std::is_floating_point_v<T>, "T must be floating-point");

		using fp_bits = ccm::support::fp::FPBits<T>;

		const fp_bits lhs_bits(lhs);
		const fp_bits rhs_bits(rhs);
		const bool exact_bitwise_equal = lhs_bits.uintval() == rhs_bits.uintval();
		const bool numerically_equal = (lhs == rhs) || (lhs_bits.is_zero() && rhs_bits.is_zero());

		if (lhs_bits.is_nan() && rhs_bits.is_nan())
		{
			return distance_result<T>{ relation::both_nan, exact_bitwise_equal, false, 0 };
		}
		if (lhs_bits.is_nan() || rhs_bits.is_nan())
		{
			return distance_result<T>{ relation::nan_mismatch, exact_bitwise_equal, false, 0 };
		}
		if (lhs_bits.is_inf() && rhs_bits.is_inf())
		{
			if (lhs_bits.sign() == rhs_bits.sign())
			{
				return distance_result<T>{ relation::both_infinity_same_sign, exact_bitwise_equal, numerically_equal, 0 };
			}
			return distance_result<T>{ relation::infinity_mismatch, exact_bitwise_equal, false, 0 };
		}
		if (lhs_bits.is_inf() || rhs_bits.is_inf())
		{
			return distance_result<T>{ relation::infinity_mismatch, exact_bitwise_equal, false, 0 };
		}
		if (lhs_bits.is_zero() && rhs_bits.is_zero())
		{
			// Signed zero is numerically equal but still exposed through exact_bitwise_equal.
			return distance_result<T>{ relation::finite, exact_bitwise_equal, true, 0 };
		}

		return distance_result<T>{
			relation::finite,
			exact_bitwise_equal,
			numerically_equal,
			saturating_distance(ordered_bits(lhs_bits), ordered_bits(rhs_bits)),
		};
	}

	template <typename T>
	constexpr bool has_finite_distance(const distance_result<T> & result)
	{
		return result.kind == relation::finite || result.kind == relation::both_infinity_same_sign;
	}

	template <typename T>
	constexpr std::uint64_t distance_or_max(T lhs, T rhs)
	{
		const auto result = classify_distance(lhs, rhs);
		return has_finite_distance(result) ? result.distance : std::numeric_limits<std::uint64_t>::max();
	}
} // namespace ccm::test::ulp
