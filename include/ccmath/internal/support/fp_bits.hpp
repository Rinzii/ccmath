/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/types/float128.hpp"
#include "ccmath/internal/types/int128.hpp"
#include <climits>
#include <cstdint>
#include <type_traits>

namespace ccm::support
{
	/**
	 * @brief Constructs a bitmask with the count right-most bits set to 1, and all other bits set to 0.
	 * @tparam T An unsigned type.
	 * @tparam count The number of bits to set to 1.
	 * @return A bitmask with the count right-most bits set to 1.
	 */
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T>
	mask_trailing_ones() {
		constexpr unsigned T_BITS = CHAR_BIT * sizeof(T);
		static_assert(count <= T_BITS && "Invalid bit index");
		return count == 0 ? 0 : (T(-1) >> (T_BITS - count));
	}

	/**
	 * @brief Constructs a bitmask with the count left-most bits set to 1, and all other bits set to 0.
	 * @tparam T An unsigned type.
	 * @tparam count The number of bits to set to 1.
	 * @return A bitmask with the count left-most bits set to 1.
	 */
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T>
	mask_leading_ones() {
		return T(~mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>());
	}

	/**
	 * @brief Constructs a bitmask with the count right-most bits set to 0, and all other bits set to 1.
	 * @tparam T An unsigned type.
	 * @tparam count The number of bits to set to 0.
	 * @return A bitmask with the count right-most bits set to 0.
	 */
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T>
	mask_trailing_zeros() {
		return mask_leading_ones<T, CHAR_BIT * sizeof(T) - count>();
	}

	/**
	 * @brief Constructs a bitmask with the count left-most bits set to 0, and all other bits set to 1.
	 * @tparam T An unsigned type.
	 * @tparam count The number of bits to set to 0.
	 * @return A bitmask with the count left-most bits set to 0.
	 */
	template <typename T, std::size_t count>
	constexpr std::enable_if_t<std::is_unsigned_v<T>, T>
	mask_leading_zeros() {
		return mask_trailing_ones<T, CHAR_BIT * sizeof(T) - count>();
	}

	enum class FPType : std::uint8_t
	{
		eBinary32,
		eBinary64,
		eBinary128,
		eBinary80
	};

	template <FPType T>
	struct FPLayout;

	template <>
	struct FPLayout<FPType::eBinary32>
	{
		using StorageType		= std::uint32_t;
		using SignedStorageType = std::int32_t;

		static constexpr int kSignLength		= 1;
		static constexpr int kExponentLength	= 8;
		static constexpr int kSignificandLength = 23;
		static constexpr int kFractionLength	= kSignificandLength;
	};

	template <>
	struct FPLayout<FPType::eBinary64>
	{
		using StorageType		= std::uint64_t;
		using SignedStorageType = std::int64_t;

		static constexpr int kSignLength		= 1;
		static constexpr int kExponentLength	= 11;
		static constexpr int kSignificandLength = 52;
		static constexpr int kFractionLength	= kSignificandLength;
	};

#if defined(_MSVC_VER) && !defined(__clang__) // MSVC only supports 64-bit floating point types
	template <>
	struct FPLayout<FPType::eBinary128> : FPLayout<FPType::eBinary64>
	{
	};

	template <>
	struct FPLayout<FPType::eBinary80> : FPLayout<FPType::eBinary64>
	{
	};

#else
	template <>
	struct FPLayout<FPType::eBinary128>
	{
		using StorageType		= ccm::uint128_t;
		using SignedStorageType = ccm::int128_t;

		static constexpr int kSignLength		= 1;
		static constexpr int kExponentLength	= 15;
		static constexpr int kSignificandLength = 112;
		static constexpr int kFractionLength	= kSignificandLength;
	};

	template <>
	struct FPLayout<FPType::eBinary80>
	{
		using StorageType		= ccm::uint128_t;
		using SignedStorageType = ccm::int128_t;

		static constexpr int kSignLength		= 1;
		static constexpr int kExponentLength	= 15;
		static constexpr int kSignificandLength = 64;
		static constexpr int kFractionLength	= kSignificandLength;
	};
#endif

	template <FPType T>
	struct FPStorage : FPLayout<T>
	{
		using Layout = FPLayout<T>;

		using StorageType = typename Layout::StorageType;
		using SignedStorageType = typename Layout::SignedStorageType;

		using Layout::kSignLength;
		using Layout::kExponentLength;
		using Layout::kSignificandLength;
		using Layout::kFractionLength;

		/**
		 * @brief The sum of the sign, exponent, and significand lengths.
		 */
		static constexpr int kTotalLength = kSignLength + kExponentLength + kSignificandLength;

		/**
		 * @brief The number of bits in the storage type.
		 */
		static constexpr int kStorageLength = sizeof(StorageType) * CHAR_BIT;
		static_assert(kTotalLength <= kStorageLength, "Storage type is too small to hold the floating point type.");

		/**
		 * @brief The bias of the exponent.
		 * @note This value will always be positive.
		 */
		static constexpr SignedStorageType kExponentBias = (1 << (kExponentLength - 1)) - 1;
		static_assert(kExponentBias > 0, "Exponent bias must be positive.");

		/**
		 * @brief The mask for extracting the significands bits.
		 */
		static constexpr StorageType kSignificandMask = mask_trailing_ones<StorageType, kSignificandLength>();

		/**
		 * @brief The mask for extracting the exponents bits.
		 */
		static constexpr StorageType kExponentMask = mask_trailing_ones<StorageType, kExponentLength>() << kSignificandLength;

		/**
		 * @brief The mask for extracting the signs bit.
		 */
		static constexpr StorageType kSignMask = mask_trailing_ones<StorageType, kSignLength>() << (kSignificandLength + kExponentLength);

		/**
		 * @brief The mask for extracting the exponent and significand bits.
		 */
		static constexpr StorageType kExponentSignificandMask = mask_trailing_ones<StorageType, kSignificandLength + kExponentLength>();

		/**
		 * @brief The mask for extracting only the fractions bits. (i.e the significand without the leading one bit)
		 */
		static constexpr StorageType kFractionMask = mask_trailing_ones<StorageType, kSignificandLength>();

		/**
		 * @brief The mask for extracting all bits. (i.e the sign + exponent + significand)
		 */
		static constexpr StorageType kTotalMask = mask_trailing_ones<StorageType, kFractionLength>();

		static_assert((kSignificandMask & kExponentMask & kSignMask) == 0, "Then masks are disjoint");
		static_assert((kSignificandMask | kExponentMask | kSignMask) == kTotalMask, "The masks are covered");

	protected:

		/**
		 * @brief Merges the bits of two values as specified by a 'mask'. Will use the 'a' bits if the 'mask' bits are zeros and 'b' when the bits are ones.
		 * @param a The first value.
		 * @param b The second value.
		 * @param mask The mask to apply.
		 * @return The merged value.
		 */
		static constexpr StorageType merge(StorageType a, StorageType b, StorageType mask)
		{
			// https://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
			return a ^ ((a ^ b) & mask);
		}

		/**
		 * @brief A strongly typed integer that prevents mixing and matching integers with different semantics.
		 */
		template <typename Type> struct Integer // NOLINT
		{
			using value_type = Type;
			constexpr explicit Integer(Type value) : value(value) {}
			constexpr Integer(const Integer &value) = default;
			constexpr Integer &operator=(const Integer &value) = default;
			
			constexpr explicit operator Type() const { return value; }

			constexpr StorageType to_storage_type() const {
				return StorageType(value);
			}

			friend constexpr bool operator==(Integer a, Integer b) {
				return a.value == b.value;
			}
			friend constexpr bool operator!=(Integer a, Integer b) {
				return a.value != b.value;
			}

		private:
			Type value;
		};

		/**
		 * @brief
		 */
		struct Exponent : public Integer<std::int32_t>
		{
			using TypedInt = Integer<std::int32_t>;
		};

	};



	struct FPBits
	{
	};
} // namespace ccm::support