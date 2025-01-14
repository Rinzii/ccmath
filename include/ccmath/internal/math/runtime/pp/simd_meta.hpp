
#pragma once

#include "fwddecl.hpp"
//#include "flags.hpp"

namespace ccm::pp::detail
{
	template <typename T>
struct assert_unreachable
	{
		static_assert(!std::is_same_v<T, T>, "this should be unreachable");
	};


	template <typename Abi, typename = void>
struct is_simd_abi_tag : std::false_type {};

	template <typename Abi>
	struct is_simd_abi_tag<Abi, std::void_t<
		decltype(!Abi::template IsValid<void>::value),
		typename Abi::IsValidAbiTag>> : std::true_type {};
}