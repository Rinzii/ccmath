/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/fp_bits.hpp"
#include "ccmath/internal/predef/unlikely.hpp"

namespace ccm
{
	/**
	 * @brief Returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @tparam T The type of the input.
	 * @param num The value to truncate.
	 * @return Returns a truncated value.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T trunc(T num) noexcept
	{
		using FPBits_t	= ccm::support::FPBits<T>;
		using Storage_t = typename FPBits_t::storage_type;

		FPBits_t bits(num);

		// If x == ±∞ then return num
		// If x == ±NaN then return num
		if (CCM_UNLIKELY(bits.is_inf_or_nan())) { return num; }

		// If x == ±0 then return num
		if (CCM_UNLIKELY(num == 0.0)) { return num; }

		const int exponent = bits.get_exponent();

		// If the exponent is greater than or equal to the fraction length, then we will return the number as is since it is already an integer.
		if (exponent >= FPBits_t::fraction_length) { return num; }

		// If our exponent is set up such that the abs(x) is less than 1 we will instead return 0.
		if (exponent <= -1) { return FPBits_t::zero(bits.sign()).get_val(); }

		// Perform the truncation
		const int trimming_size = FPBits_t::fraction_length - exponent;
		const auto truncated_mantissa = static_cast<Storage_t>((bits.get_mantissa() >> trimming_size) << trimming_size);
		bits.set_mantissa(truncated_mantissa);
		return bits.get_val();
	}

	/**
	 * @brief Returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @tparam Integer The type of the input.
	 * @param num The value to truncate.
	 * @return Returns a truncated value.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double trunc(Integer num) noexcept
	{
		return static_cast<double>(num);
	}

	/**
	 * @brief Specialization for float that returns the integral value nearest to x with the magnitude of the integral value always less than or equal to x.
	 * @param num The float to truncate.
	 * @return Returns a truncated float.
	 */
	constexpr float truncf(float num) noexcept
	{
		return ccm::trunc<float>(num);
	}

	/**
	 * @brief Specialization for long double that returns the integral value nearest to x with the magnitude of the integral value always less than or equal to
	 * x.
	 * @param num The long double to truncate.
	 * @return Returns a truncated long double.
	 */
	constexpr long double truncl(long double num) noexcept
	{
		return ccm::trunc<long double>(num);
	}
} // namespace ccm
