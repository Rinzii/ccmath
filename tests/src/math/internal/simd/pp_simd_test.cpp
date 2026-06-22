/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// clang-format off
// <cmath> must come before pp.hpp. It pulls in the UCRT math declarations first,
// so the ccm::pp MSVC scalar fallback, which forward declares the same C runtime
// entry points, redeclares them rather than declaring them ahead of the UCRT.
// Including pp.hpp first makes MSVC reject the later UCRT declarations (C2375
// redefinition, C2733 cannot overload an extern "C" function).
#include <cmath>

#include "ccmath/internal/math/runtime/pp/pp.hpp"
// clang-format on

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <type_traits>

namespace
{
	using namespace ccm::pp;

	// Fill a buffer with a deterministic, non-degenerate pattern.
	template <typename T>
	void fill_a(T * p, int n)
	{
		for (int i = 0; i < n; ++i) { p[i] = static_cast<T>(i + 1); }
	}
	template <typename T>
	void fill_b(T * p, int n)
	{
		for (int i = 0; i < n; ++i) { p[i] = static_cast<T>((i % 3) + 1); }
	}

	template <typename V>
	void check_construction()
	{
		using T				  = typename V::value_type;
		constexpr int N		  = V::size();
		alignas(64) T a[16]	  = {};
		alignas(64) T out[16] = {};
		fill_a(a, N);

		// load constructor (vector aligned) + store roundtrip (element aligned)
		V va(a, vector_aligned);
		va.copy_to(out, element_aligned);
		for (int i = 0; i < N; ++i) { EXPECT_EQ(out[i], a[i]); }

		// broadcast
		V vb(static_cast<T>(7));
		for (int i = 0; i < N; ++i) { EXPECT_EQ(vb[i], static_cast<T>(7)); }

		// generator
		V vg([](auto i) { return static_cast<T>(static_cast<int>(i) * 2); });
		for (int i = 0; i < N; ++i) { EXPECT_EQ(vg[i], static_cast<T>(i * 2)); }

		// mutable lane proxy
		va[0] = static_cast<T>(42);
		EXPECT_EQ(va[0], static_cast<T>(42));

		// copy_from
		V vc;
		vc.copy_from(a, element_aligned);
		for (int i = 0; i < N; ++i) { EXPECT_EQ(vc[i], a[i]); }
	}

	template <typename V>
	void check_arithmetic()
	{
		using T			= typename V::value_type;
		constexpr int N = V::size();
		alignas(64) T a[16], b[16];
		fill_a(a, N);
		fill_b(b, N);
		V va(a, element_aligned);
		V vb(b, element_aligned);

		V vadd = va + vb;
		V vsub = va - vb;
		V vmul = va * vb;
		V vdiv = va / vb;
		V vneg = -va;
		for (int i = 0; i < N; ++i)
		{
			EXPECT_EQ(vadd[i], static_cast<T>(a[i] + b[i]));
			EXPECT_EQ(vsub[i], static_cast<T>(a[i] - b[i]));
			EXPECT_EQ(vmul[i], static_cast<T>(a[i] * b[i]));
			EXPECT_EQ(vdiv[i], static_cast<T>(a[i] / b[i]));
			EXPECT_EQ(vneg[i], static_cast<T>(-a[i]));
		}

		V vc = va;
		vc += vb;
		vc -= vb;
		vc *= vb;
		vc /= vb;
		for (int i = 0; i < N; ++i) { EXPECT_EQ(vc[i], static_cast<T>(static_cast<T>(a[i] * b[i]) / b[i])); }
	}

	template <typename V>
	void check_masks()
	{
		using T			= typename V::value_type;
		constexpr int N = V::size();
		alignas(64) T a[16], b[16];
		fill_a(a, N); // 1, 2, 3, ...
		fill_b(b, N); // 1, 2, 3, 1, 2, 3, ...
		V va(a, element_aligned);
		V vb(b, element_aligned);

		auto lt		 = va < vb;
		auto eq		 = va == vb;
		auto ge		 = va >= vb;
		int lt_count = 0, eq_count = 0;
		for (int i = 0; i < N; ++i)
		{
			EXPECT_EQ(lt[i], a[i] < b[i]);
			EXPECT_EQ(eq[i], a[i] == b[i]);
			EXPECT_EQ(ge[i], a[i] >= b[i]);
			lt_count += (a[i] < b[i]) ? 1 : 0;
			eq_count += (a[i] == b[i]) ? 1 : 0;
		}

		EXPECT_EQ(reduce_count(lt), lt_count);
		EXPECT_EQ(any_of(eq), eq_count > 0);
		EXPECT_EQ(none_of(lt), lt_count == 0);
		EXPECT_EQ(all_of(va == va), true);
		EXPECT_EQ(none_of(va != va), true);

		// logical combinators
		auto both	 = (va == va) & ge;
		auto either	 = lt | ge;
		auto negated = !lt;
		for (int i = 0; i < N; ++i)
		{
			EXPECT_EQ(both[i], ge[i]);
			EXPECT_EQ(either[i], (a[i] < b[i]) || (a[i] >= b[i]));
			EXPECT_EQ(negated[i], !(a[i] < b[i]));
		}
		EXPECT_TRUE(all_of(either));
	}

	template <typename V>
	void check_where()
	{
		using T			= typename V::value_type;
		constexpr int N = V::size();
		alignas(64) T a[16], b[16];
		fill_a(a, N);
		fill_b(b, N);
		V va(a, element_aligned);
		V vb(b, element_aligned);

		auto m	  = va < vb;
		V blended = simd_select(m, va, vb); // where m: va else vb
		for (int i = 0; i < N; ++i) { EXPECT_EQ(blended[i], (a[i] < b[i]) ? a[i] : b[i]); }

		V w			= vb;
		where(m, w) = static_cast<T>(0);
		where(m, w) += va; // only lanes where m are touched (now 0 + va)
		for (int i = 0; i < N; ++i)
		{
			T expected = (a[i] < b[i]) ? static_cast<T>(a[i]) : b[i];
			EXPECT_EQ(w[i], expected);
		}
	}

	template <typename V>
	void check_all()
	{
		check_construction<V>();
		check_arithmetic<V>();
		check_masks<V>();
		check_where<V>();
	}

	inline void expect_close(float got, float ref)
	{ EXPECT_FLOAT_EQ(got, ref); }
	inline void expect_close(double got, double ref)
	{ EXPECT_DOUBLE_EQ(got, ref); }

	template <typename Abi>
	void check_integer_ops()
	{
		using V			= basic_simd<std::int32_t, Abi>;
		constexpr int N = V::size();
		alignas(64) std::int32_t a[16], b[16];
		for (int i = 0; i < N; ++i)
		{
			a[i] = static_cast<std::int32_t>((static_cast<std::uint32_t>(i) * 2654435761u) & 0x7fffffffu);
			b[i] = (i % 5) + 1; // 1..5, safe shift / nonzero modulo
		}
		V va(a, element_aligned);
		V vb(b, element_aligned);

		V vand = va & vb, vor = va | vb, vxor = va ^ vb, vnot = ~va;
		V vshl = va << vb, vshr = va >> vb, vmod = va % vb;
		for (int i = 0; i < N; ++i)
		{
			EXPECT_EQ(vand[i], a[i] & b[i]);
			EXPECT_EQ(vor[i], a[i] | b[i]);
			EXPECT_EQ(vxor[i], a[i] ^ b[i]);
			EXPECT_EQ(vnot[i], ~a[i]);
			EXPECT_EQ(vshl[i], a[i] << b[i]);
			EXPECT_EQ(vshr[i], a[i] >> b[i]);
			EXPECT_EQ(vmod[i], a[i] % b[i]);
		}

		V c = va;
		c &= vb;
		c |= vb;
		c ^= vb;
		c <<= vb;
		c >>= vb;
		c %= vb;
		for (int i = 0; i < N; ++i)
		{
			std::int32_t e = a[i];
			e &= b[i];
			e |= b[i];
			e ^= b[i];
			e <<= b[i];
			e >>= b[i];
			e %= b[i];
			EXPECT_EQ(c[i], e);
		}
	}

	template <typename V>
	void check_conversions()
	{
		using T			= typename V::value_type;
		using I			= std::conditional_t<sizeof(T) == 4, std::int32_t, std::int64_t>;
		constexpr int N = V::size();
		alignas(64) T x[16];
		for (int i = 0; i < N; ++i) { x[i] = static_cast<T>(i - N / 2) + static_cast<T>(0.5); }
		V vx(x, element_aligned);

		auto vi = static_simd_cast<I>(vx); // truncation toward zero
		for (int i = 0; i < N; ++i) { EXPECT_EQ(vi[i], static_cast<I>(x[i])); }

		auto vb = simd_bit_cast<I>(vx); // bit reinterpret
		for (int i = 0; i < N; ++i)
		{
			I ref;
			std::memcpy(&ref, &x[i], sizeof(I));
			EXPECT_EQ(vb[i], ref);
		}

		auto back = static_simd_cast<T>(vi);
		for (int i = 0; i < N; ++i) { expect_close(back[i], static_cast<T>(static_cast<I>(x[i]))); }
	}

	template <typename V>
	void check_math()
	{
		using T			= typename V::value_type;
		constexpr int N = V::size();
		alignas(64) T x[16], y[16];
		for (int i = 0; i < N; ++i)
		{
			x[i] = static_cast<T>(i + 1);							   // positive (sqrt, log)
			y[i] = static_cast<T>((i % 5) - 2) + static_cast<T>(0.25); // mixed sign, fractional
		}
		V vx(x, element_aligned);
		V vy(y, element_aligned);

		V r_sqrt  = sqrt(vx);
		V r_abs	  = abs(vy);
		V r_fabs  = fabs(vy);
		V r_floor = floor(vy);
		V r_ceil  = ceil(vy);
		V r_trunc = trunc(vy);
		V r_round = round(vy);
		V r_min	  = min(vx, vy);
		V r_max	  = max(vx, vy);
		V r_fma	  = fma(vx, vy, vx);
		V r_exp	  = exp(vy);
		V r_log	  = log(vx);
		V r_pow	  = pow(vx, V(static_cast<T>(2)));
		for (int i = 0; i < N; ++i)
		{
			expect_close(r_sqrt[i], std::sqrt(x[i]));
			expect_close(r_abs[i], std::abs(y[i]));
			expect_close(r_fabs[i], std::fabs(y[i]));
			expect_close(r_floor[i], std::floor(y[i]));
			expect_close(r_ceil[i], std::ceil(y[i]));
			expect_close(r_trunc[i], std::trunc(y[i]));
			expect_close(r_round[i], std::round(y[i]));
			expect_close(r_min[i], x[i] < y[i] ? x[i] : y[i]);
			expect_close(r_max[i], x[i] < y[i] ? y[i] : x[i]);
			expect_close(r_fma[i], std::fma(x[i], y[i], x[i]));
			expect_close(r_exp[i], std::exp(y[i]));
			expect_close(r_log[i], std::log(x[i]));
			expect_close(r_pow[i], std::pow(x[i], static_cast<T>(2)));
		}
	}

	template <typename V>
	void check_reduce()
	{
		using T			= typename V::value_type;
		using M			= typename V::mask_type;
		constexpr int N = V::size();
		alignas(64) T a[16];
		for (int i = 0; i < N; ++i) { a[i] = static_cast<T>((i * 7 % 13) + 1); }
		V v(a, element_aligned);

		T sum = 0, prod = 1, mn = a[0], mx = a[0];
		for (int i = 0; i < N; ++i)
		{
			sum += a[i];
			prod *= a[i];
			mn = a[i] < mn ? a[i] : mn;
			mx = a[i] > mx ? a[i] : mx;
		}
		expect_close(reduce(v), sum);
		expect_close(reduce(v, std::multiplies<>{}), prod);
		expect_close(reduce_min(v), mn);
		expect_close(reduce_max(v), mx);

		// Masked over the even lanes (deterministic mask via from_bits).
		M m	   = M::from_bits(0x5555555555555555ull);
		T msum = 0, mmn = 0, mmx = 0;
		bool any = false;
		for (int i = 0; i < N; ++i)
		{
			if ((i % 2) == 0)
			{
				msum += a[i];
				if (!any)
				{
					mmn = mmx = a[i];
					any		  = true;
				}
				else
				{
					mmn = a[i] < mmn ? a[i] : mmn;
					mmx = a[i] > mmx ? a[i] : mmx;
				}
			}
		}
		expect_close(reduce(v, m, static_cast<T>(0)), msum);
		expect_close(reduce_min(v, m), mmn);
		expect_close(reduce_max(v, m), mmx);
	}

	template <typename V>
	void check_masked_memory()
	{
		using T			= typename V::value_type;
		using M			= typename V::mask_type;
		constexpr int N = V::size();
		alignas(64) T a[16];
		fill_a(a, N);
		V v(a, element_aligned);
		M m = M::from_bits(0x5555555555555555ull); // even lanes

		alignas(64) T dst[16];
		for (int i = 0; i < N; ++i) { dst[i] = static_cast<T>(-1); }
		where(m, v).copy_to(dst);
		for (int i = 0; i < N; ++i) { EXPECT_EQ(dst[i], (i % 2 == 0) ? a[i] : static_cast<T>(-1)); }

		alignas(64) T src[16];
		for (int i = 0; i < N; ++i) { src[i] = static_cast<T>(100 + i); }
		V w(static_cast<T>(0));
		where(m, w).copy_from(src);
		for (int i = 0; i < N; ++i) { EXPECT_EQ(w[i], (i % 2 == 0) ? src[i] : static_cast<T>(0)); }

		alignas(64) T dst2[16];
		for (int i = 0; i < N; ++i) { dst2[i] = static_cast<T>(-1); }
		V const cw = v;
		where(m, cw).copy_to(dst2); // const_where_expression
		for (int i = 0; i < N; ++i) { EXPECT_EQ(dst2[i], (i % 2 == 0) ? a[i] : static_cast<T>(-1)); }
	}
} // namespace

TEST(PpSimdTest, ScalarAbiFloat)
{ check_all<basic_simd<float, ScalarAbi>>(); }
TEST(PpSimdTest, ScalarAbiDouble)
{ check_all<basic_simd<double, ScalarAbi>>(); }
TEST(PpSimdTest, ScalarAbiInt32)
{ check_all<basic_simd<std::int32_t, ScalarAbi>>(); }

TEST(PpSimdTest, VecAbiFloat4)
{ check_all<basic_simd<float, VecAbi<4>>>(); }
TEST(PpSimdTest, VecAbiFloat8)
{ check_all<basic_simd<float, VecAbi<8>>>(); }
TEST(PpSimdTest, VecAbiDouble2)
{ check_all<basic_simd<double, VecAbi<2>>>(); }
TEST(PpSimdTest, VecAbiInt32x4)
{ check_all<basic_simd<std::int32_t, VecAbi<4>>>(); }

TEST(PpSimdTest, NativeFloat)
{ check_all<native_simd<float>>(); }
TEST(PpSimdTest, NativeDouble)
{ check_all<native_simd<double>>(); }

TEST(PpSimdTest, AliasesAndTraits)
{
	static_assert(is_simd_v<simd<float, 4>>, "");
	static_assert(is_simd_mask_v<simd_mask<float, 4>>, "");
	static_assert(is_abi_tag_v<ScalarAbi>, "");
	static_assert(is_abi_tag_v<VecAbi<4>>, "");
	static_assert(simd_size_v<float, VecAbi<4>> == 4, "");
	static_assert(simd_size_v<double, VecAbi<2>> == 2, "");
	static_assert(std::is_same<simd<float, 1>::abi_type, ScalarAbi>::value, "");
	static_assert(std::is_same<simd<float, 8>::abi_type, VecAbi<8>>::value, "");

	EXPECT_EQ((simd<float, 4>::size()), 4);
	EXPECT_GE(native_simd<float>::size(), 1);
	EXPECT_GE(native_simd<double>::size(), 1);

	// scalar bool reduction overloads
	EXPECT_TRUE(all_of(true));
	EXPECT_FALSE(any_of(false));
	EXPECT_TRUE(none_of(false));
	EXPECT_EQ(reduce_count(true), 1);
}

TEST(PpSimdTest, IntegerOpsScalar)
{ check_integer_ops<ScalarAbi>(); }
TEST(PpSimdTest, IntegerOpsVec4)
{ check_integer_ops<VecAbi<4>>(); }
TEST(PpSimdTest, IntegerOpsVec8)
{ check_integer_ops<VecAbi<8>>(); }

TEST(PpSimdTest, ConversionsFloat)
{
	check_conversions<basic_simd<float, ScalarAbi>>();
	check_conversions<basic_simd<float, VecAbi<4>>>();
	check_conversions<native_simd<float>>();
}
TEST(PpSimdTest, ConversionsDouble)
{
	check_conversions<basic_simd<double, ScalarAbi>>();
	check_conversions<basic_simd<double, VecAbi<2>>>();
	check_conversions<native_simd<double>>();
}

TEST(PpSimdTest, MathFloat)
{
	check_math<basic_simd<float, ScalarAbi>>();
	check_math<basic_simd<float, VecAbi<4>>>();
	check_math<native_simd<float>>();
}
TEST(PpSimdTest, MathDouble)
{
	check_math<basic_simd<double, ScalarAbi>>();
	check_math<basic_simd<double, VecAbi<2>>>();
	check_math<native_simd<double>>();
}

TEST(PpSimdTest, ReduceFloat)
{
	check_reduce<basic_simd<float, ScalarAbi>>();
	check_reduce<basic_simd<float, VecAbi<4>>>();
	check_reduce<basic_simd<float, VecAbi<8>>>();
	check_reduce<native_simd<float>>();
}
TEST(PpSimdTest, ReduceDouble)
{
	check_reduce<basic_simd<double, ScalarAbi>>();
	check_reduce<basic_simd<double, VecAbi<2>>>();
	check_reduce<native_simd<double>>();
}

TEST(PpSimdTest, MaskedMemory)
{
	check_masked_memory<basic_simd<float, VecAbi<4>>>();
	check_masked_memory<basic_simd<float, VecAbi<8>>>();
	check_masked_memory<basic_simd<double, VecAbi<2>>>();
	check_masked_memory<basic_simd<int, ScalarAbi>>();
}

TEST(PpSimdTest, MaskIndexAndBits)
{
	using M = simd_mask<float, 8>;
	M m		= M::from_bits(0xA4ull); // bits 2, 5, 7 set
	EXPECT_EQ(m.to_ullong(), 0xA4ull);
	EXPECT_EQ(reduce_count(m), 3);
	EXPECT_EQ(reduce_min_index(m), 2);
	EXPECT_EQ(reduce_max_index(m), 7);
	EXPECT_EQ(m.to_bitset().count(), 3u);
	EXPECT_TRUE(all_of(m == M::from_bits(m.to_ullong())));
}

TEST(PpSimdTest, CatSplit)
{
	using V4			   = basic_simd<float, VecAbi<4>>;
	alignas(64) float a[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	V4 lo(a, element_aligned);
	V4 hi(a + 4, element_aligned);

	auto cat = simd_cat(lo, hi);
	static_assert(decltype(cat)::size() == 8, "");
	for (int i = 0; i < 8; ++i) { EXPECT_EQ(cat[i], a[i]); }

	auto parts = simd_split<V4>(cat);
	EXPECT_EQ(parts.size(), 2u);
	for (int i = 0; i < 4; ++i)
	{
		EXPECT_EQ(parts[0][i], a[i]);
		EXPECT_EQ(parts[1][i], a[i + 4]);
	}

	// mask cat / split via bits
	auto mlo  = (lo < V4(3.0F)); // lanes 0,1
	auto mcat = simd_cat(mlo, mlo);
	EXPECT_EQ(mcat.to_ullong(), 0x33ull);
	auto mparts = simd_split<decltype(mlo)>(mcat);
	EXPECT_EQ(mparts[0].to_ullong(), 0x3ull);
	EXPECT_EQ(mparts[1].to_ullong(), 0x3ull);
}

TEST(PpSimdTest, ResizeRebindConvert)
{
	using V = basic_simd<float, VecAbi<8>>;
	static_assert(resize_simd_t<4, V>::size() == 4, "");
	static_assert(std::is_same<resize_simd_t<1, V>, basic_simd<float, ScalarAbi>>::value, "");
	static_assert(std::is_same<rebind_simd_t<std::int32_t, V>, basic_simd<std::int32_t, VecAbi<8>>>::value, "");

	alignas(64) float a[8] = { 1.4F, 2.6F, -3.5F, 4.9F, 5.1F, -6.2F, 7.8F, 8.0F };
	V v(a, element_aligned);
	basic_simd<std::int32_t, VecAbi<8>> iv(v); // converting ctor (truncation)
	basic_simd<double, VecAbi<8>> dv(v);
	for (int i = 0; i < 8; ++i)
	{
		EXPECT_EQ(iv[i], static_cast<std::int32_t>(a[i]));
		EXPECT_DOUBLE_EQ(dv[i], static_cast<double>(a[i]));
	}
}
