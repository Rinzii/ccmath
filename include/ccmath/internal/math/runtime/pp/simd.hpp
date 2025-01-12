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

/*
#include "detail.h"
#include "simd_mask.h"
#include "flags.h"
#include "simd_iterator.h"
#include <array>
#include <type_traits>
#include <iterator>

namespace ccm::pp {

template <typename T, typename Abi>
struct basic_simd {
private:
    using Traits = detail::SimdTraits<T, Abi>;
    using MemberType = typename Traits::SimdMember;
    static constexpr T* type_tag = nullptr;

    alignas(Traits::SimdAlign) MemberType data_;

public:
    using value_type = T;
    using abi_type = Abi;
    using mask_type = simd_namespace::basic_simd_mask<sizeof(T), Abi>;

    static constexpr std::size_t size = Traits::SimdSize;

    // Default constructor
    basic_simd() = default;

    // Copy constructor
    template <typename U, typename UAbi>
    explicit basic_simd(const basic_simd<U, UAbi>& other)
        -> std::enable_if_t<Traits::template is_simd_ctor_arg<U>(), basic_simd>
        : data_(detail::SimdConverter<U, UAbi, T, Abi>()(other.data_)) {}

    // Implicit broadcast constructor
    template <typename U>
    explicit basic_simd(U value)
        -> std::enable_if_t<std::is_convertible<U, T>::value, basic_simd>
        : data_(Traits::SimdBroadcast(value)) {}

    // Load constructor
    template <typename Iter>
    basic_simd(Iter first)
        -> std::enable_if_t<std::is_pointer<Iter>::value, basic_simd>
        : data_(Traits::SimdLoad(first, type_tag)) {}

    // Conversion operator
    template <typename U>
    explicit operator U() const
        -> std::enable_if_t<Traits::template is_simd_conversion<U>(), U> {
        return Traits::template SimdConversion<U>(data_);
    }

    // Access data
    auto operator[](std::size_t index) const -> T {
        if (index >= size) throw std::out_of_range("Index out of range");
        return Traits::GetElement(data_, index);
    }

    // Unary operators
    auto operator-() const -> basic_simd {
        return basic_simd(Traits::UnaryMinus(data_));
    }

    // Binary operators
    friend auto operator+(const basic_simd& lhs, const basic_simd& rhs) -> basic_simd {
        return basic_simd(Traits::Add(lhs.data_, rhs.data_));
    }

    // Store function
    template <typename Iter>
    auto store(Iter first) const
        -> std::enable_if_t<std::is_pointer<Iter>::value, void> {
        Traits::SimdStore(data_, first, type_tag);
    }
};

template <typename T, typename Abi>
struct is_simd<basic_simd<T, Abi>> : std::true_type {};

}  // namespace ccm::pp
*/