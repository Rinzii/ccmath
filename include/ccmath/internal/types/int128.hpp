

#pragma once

#include "ccmath/internal/config/type_support.hpp"

#include <cassert>
// #include <cmath>
#include <cstdint>
#include <limits>

#if defined(_MSC_VER)
	// In very old versions of MSVC and when the /Zc:wchar_t flag is off, wchar_t is
	// a typedef for unsigned short.  Otherwise wchar_t is mapped to the __wchar_t
	// builtin type.  We need to make sure not to define operator wchar_t()
	// alongside operator unsigned short() in these instances.
	#define CCM_INTERNAL_WCHAR_T __wchar_t
	#if defined(_M_X64) && !defined(_M_ARM64EC)
		#include <intrin.h>
		#pragma intrinsic(_umul128)
	#endif // defined(_M_X64)
#else	   // defined(_MSC_VER)
	#define CCM_INTERNAL_WCHAR_T wchar_t
#endif // defined(_MSC_VER)

// Detect endianness.
//
// THis uses the built-in __BYTE_ORDER__ macro on GCC and Clang. MSVC assumes little endian.
#if defined(CCM_IS_BIG_ENDIAN)
	#error "CCM_IS_BIG_ENDIAN cannot be directly set."
#endif
#if defined(CCM_IS_LITTLE_ENDIAN)
	#error "CCM_IS_LITTLE_ENDIAN cannot be directly set."
#endif

#if (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
	#define CCM_IS_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define CCM_IS_BIG_ENDIAN 1
#elif defined(_WIN32)
	#define CCM_IS_LITTLE_ENDIAN 1
#else
	#error "ccm endian detection needs to be set up for your compiler"
#endif

namespace ccm::types
{
	class int128;

	class
#if defined(CCM_TYPES_HAS_INT128)
		alignas(unsigned __int128)
#endif // CCM_TYPES_HAS_INT128
			uint128
	{
	public:
		uint128() = default;

		// Constructors from arithmetic types
		constexpr uint128(int v);
		constexpr uint128(unsigned int v);
		constexpr uint128(long v);
		constexpr uint128(unsigned long v);
		constexpr uint128(long long v);
		constexpr uint128(unsigned long long v);
#ifdef CCM_TYPES_HAS_INT128
		constexpr uint128(__int128 v);
		constexpr uint128(unsigned __int128 v);
#endif // CCM_TYPES_HAS_INT128
		constexpr uint128(int128 v);
		explicit uint128(float v);
		explicit uint128(double v);
		explicit uint128(long double v);

		// Assignment operators from arithmetic types
		uint128 & operator=(int v);
		uint128 & operator=(unsigned int v);
		uint128 & operator=(long v);
		uint128 & operator=(unsigned long v);
		uint128 & operator=(long long v);
		uint128 & operator=(unsigned long long v);
#ifdef CCM_TYPES_HAS_INT128
		uint128 & operator=(__int128 v);
		uint128 & operator=(unsigned __int128 v);
#endif // CCM_TYPES_HAS_INT128
		uint128 & operator=(int128 v);

		// Conversion operators to other arithmetic types
		constexpr explicit operator bool() const;
		constexpr explicit operator char() const;
		constexpr explicit operator signed char() const;
		constexpr explicit operator unsigned char() const;
		constexpr explicit operator char16_t() const;
		constexpr explicit operator char32_t() const;
		constexpr explicit operator CCM_INTERNAL_WCHAR_T() const;
		constexpr explicit operator short() const;
		constexpr explicit operator unsigned short() const;
		constexpr explicit operator int() const;
		constexpr explicit operator unsigned int() const;
		constexpr explicit operator long() const;
		constexpr explicit operator unsigned long() const;
		constexpr explicit operator long long() const;
		constexpr explicit operator unsigned long long() const;
#ifdef CCM_TYPES_HAS_INT128
		constexpr explicit operator __int128() const;
		constexpr explicit operator unsigned __int128() const;
#endif // CCM_TYPES_HAS_INT128
		explicit operator float() const;
		explicit operator double() const;
		explicit operator long double() const;

		// Trivial copy constructor, assignment operator and destructor.

		// Arithmetic operators.
		uint128 & operator+=(uint128 other);
		uint128 & operator-=(uint128 other);
		uint128 & operator*=(uint128 other);
		// Long division/modulo for uint128.
		uint128 & operator/=(uint128 other);
		uint128 & operator%=(uint128 other);
		uint128 operator++(int);
		uint128 operator--(int);
		uint128 & operator<<=(int);
		uint128 & operator>>=(int);
		uint128 & operator&=(uint128 other);
		uint128 & operator|=(uint128 other);
		uint128 & operator^=(uint128 other);
		uint128 & operator++();
		uint128 & operator--();

		// Uint128Low64()
		//
		// Returns the lower 64-bit value of a `uint128` value.
		friend constexpr std::uint64_t Uint128Low64(uint128 v);

		// Uint128High64()
		//
		// Returns the higher 64-bit value of a `uint128` value.
		friend constexpr std::uint64_t Uint128High64(uint128 v);

		// MakeUInt128()
		//
		// Constructs a `uint128` numeric value from two 64-bit unsigned integers.
		// Note that this factory function is the only way to construct a `uint128`
		// from integer values greater than 2^64.
		//
		// Example:
		//
		//   ccm::types::uint128 big = ccm::types::MakeUint128(1, 0);
		friend constexpr uint128 MakeUint128(std::uint64_t high, std::uint64_t low);

		// Uint128Max()
		//
		// Returns the highest value for a 128-bit unsigned integer.
		friend constexpr uint128 Uint128Max();

	private:
		constexpr uint128(std::uint64_t high, std::uint64_t low);

#if defined(CCM_IS_LITTLE_ENDIAN)
		std::uint64_t lo_;
		std::uint64_t hi_;
#elif defined(CCM_IS_BIG_ENDIAN)
		std::uint64_t hi_;
		std::uint64_t lo_;
#else // byte order
	#error "Unsupported byte order: must be little-endian or big-endian."
#endif	// byte order
		/*
#if defined(CCM_TYPES_HAS_INT128)
		unsigned __int128 v_;
#else // CCM_HAVE_INTRINSIC_INT128
	#if defined(CCM_IS_LITTLE_ENDIAN)
		std::uint64_t lo_;
		std::uint64_t hi_;
	#elif defined(CCM_IS_BIG_ENDIAN)
		std::uint64_t hi_;
		std::uint64_t lo_;
	#else // byte order
		#error "Unsupported byte order: must be little-endian or big-endian."
	#endif // byte order
#endif	   // CCM_TYPES_HAS_INT128
*/
	};

	constexpr uint128 Uint128Max()
	{
		return {(std::numeric_limits<std::uint64_t>::max)(), (std::numeric_limits<std::uint64_t>::max)()};
	}
} // namespace ccm::types

// Specialized numeric_limits for uint128.
namespace std
{
	template <>
	struct numeric_limits<ccm::types::uint128>
	{
	public:
		static constexpr bool is_specialized		   = true;
		static constexpr bool is_signed				   = false;
		static constexpr bool is_integer			   = true;
		static constexpr bool is_exact				   = true;
		static constexpr bool has_infinity			   = false;
		static constexpr bool has_quiet_NaN			   = false;
		static constexpr bool has_signaling_NaN		   = false;
		static constexpr bool has_denorm_loss		   = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559				   = false;
		static constexpr bool is_bounded			   = true;
		static constexpr bool is_modulo				   = true;
		static constexpr int digits					   = 128;
		static constexpr int digits10				   = 38;
		static constexpr int max_digits10			   = 0;
		static constexpr int radix					   = 2;
		static constexpr int min_exponent			   = 0;
		static constexpr int min_exponent10			   = 0;
		static constexpr int max_exponent			   = 0;
		static constexpr int max_exponent10			   = 0;
#ifdef CCM_TYPES_HAS_INT128
		static constexpr bool traps = numeric_limits<unsigned __int128>::traps;
#else  // CCM_TYPES_HAS_INT128
		static constexpr bool traps = numeric_limits<std::uint64_t>::traps;
#endif // CCM_TYPES_HAS_INT128
		static constexpr bool tinyness_before = false;

		static constexpr ccm::types::uint128(min)() { return 0; }
		static constexpr ccm::types::uint128 lowest() { return 0; }
		static constexpr ccm::types::uint128(max)() { return ccm::types::Uint128Max(); }
		static constexpr ccm::types::uint128 epsilon() { return 0; }
		static constexpr ccm::types::uint128 round_error() { return 0; }
		static constexpr ccm::types::uint128 infinity() { return 0; }
		static constexpr ccm::types::uint128 quiet_NaN() { return 0; }
		static constexpr ccm::types::uint128 signaling_NaN() { return 0; }
		static constexpr ccm::types::uint128 denorm_min() { return 0; }
	};
} // namespace std

namespace ccm::types
{
	class int128
	{
	public:
		int128() = default;

		// Constructors from arithmetic types
		constexpr int128(int v);
		constexpr int128(unsigned int v);
		constexpr int128(long v);
		constexpr int128(unsigned long v);
		constexpr int128(long long v);
		constexpr int128(unsigned long long v);
#ifdef CCM_TYPES_HAS_INT128
		constexpr int128(__int128 v);
		constexpr explicit int128(unsigned __int128 v);
#endif // CCM_TYPES_HAS_INT128
		constexpr explicit int128(uint128 v);
		explicit int128(float v);
		explicit int128(double v);
		explicit int128(long double v);

		// Assignment operators from arithmetic types
		int128 & operator=(int v);
		int128 & operator=(unsigned int v);
		int128 & operator=(long v);
		int128 & operator=(unsigned long v);
		int128 & operator=(long long v);
		int128 & operator=(unsigned long long v);
#ifdef CCM_TYPES_HAS_INT128
		int128 & operator=(__int128 v);
#endif // CCM_TYPES_HAS_INT128

		// Conversion operators to other arithmetic types
		constexpr explicit operator bool() const;
		constexpr explicit operator char() const;
		constexpr explicit operator signed char() const;
		constexpr explicit operator unsigned char() const;
		constexpr explicit operator char16_t() const;
		constexpr explicit operator char32_t() const;
		constexpr explicit operator CCM_INTERNAL_WCHAR_T() const;
		constexpr explicit operator short() const;
		constexpr explicit operator unsigned short() const;
		constexpr explicit operator int() const;
		constexpr explicit operator unsigned int() const;
		constexpr explicit operator long() const;
		constexpr explicit operator unsigned long() const;
		constexpr explicit operator long long() const;
		constexpr explicit operator unsigned long long() const;
#ifdef CCM_TYPES_HAS_INT128
		constexpr explicit operator __int128() const;
		constexpr explicit operator unsigned __int128() const;
#endif // CCM_TYPES_HAS_INT128
		explicit operator float() const;
		explicit operator double() const;
		explicit operator long double() const;

		// Trivial copy constructor, assignment operator and destructor.

		// Arithmetic operators
		int128 & operator+=(int128 other);
		int128 & operator-=(int128 other);
		int128 & operator*=(int128 other);
		int128 & operator/=(int128 other);
		int128 & operator%=(int128 other);
		int128 operator++(int);
		int128 operator--(int);
		int128 & operator++();
		int128 & operator--();
		int128 & operator&=(int128 other);
		int128 & operator|=(int128 other);
		int128 & operator^=(int128 other);
		int128 & operator<<=(int amount);
		int128 & operator>>=(int amount);

		// Int128Low64()
		//
		// Returns the lower 64-bit value of a `int128` value.
		friend constexpr std::uint64_t Int128Low64(int128 v);

		// Int128High64()
		//
		// Returns the higher 64-bit value of a `int128` value.
		friend constexpr std::int64_t Int128High64(int128 v);

		friend constexpr int128 MakeInt128(std::int64_t high, std::uint64_t low);

		// Int128Max()
		//
		// Returns the maximum value for a 128-bit signed integer.
		friend constexpr int128 Int128Max();

		// Int128Min()
		//
		// Returns the minimum value for a 128-bit signed integer.
		friend constexpr int128 Int128Min();

	private:
		constexpr int128(std::int64_t high, std::uint64_t low);

#if defined(CCM_TYPES_HAS_INT128)
		__int128 v_;
#else // CCM_HAVE_INTRINSIC_INT128
	#if defined(CCM_IS_LITTLE_ENDIAN)
		std::uint64_t lo_;
		std::int64_t hi_;
	#elif defined(CCM_IS_BIG_ENDIAN)
		std::int64_t hi_;
		std::uint64_t lo_;
	#else // byte order
		#error "Unsupported byte order: must be little-endian or big-endian."
	#endif // byte order
#endif	   // CCM_TYPES_HAS_INT128
	};

	constexpr int128 Int128Max()
	{
		return {(std::numeric_limits<std::int64_t>::max)(), (std::numeric_limits<std::uint64_t>::max)()};
	}

	constexpr int128 Int128Min()
	{
		return {(std::numeric_limits<std::int64_t>::min)(), 0};
	}
} // namespace ccm::types

// Specialized numeric_limits for int128.
namespace std
{
	template <>
	struct numeric_limits<ccm::types::int128>
	{
	public:
		static constexpr bool is_specialized		   = true;
		static constexpr bool is_signed				   = true;
		static constexpr bool is_integer			   = true;
		static constexpr bool is_exact				   = true;
		static constexpr bool has_infinity			   = false;
		static constexpr bool has_quiet_NaN			   = false;
		static constexpr bool has_signaling_NaN		   = false;
		static constexpr bool has_denorm_loss		   = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559				   = false;
		static constexpr bool is_bounded			   = true;
		static constexpr bool is_modulo				   = false;
		static constexpr int digits					   = 127;
		static constexpr int digits10				   = 38;
		static constexpr int max_digits10			   = 0;
		static constexpr int radix					   = 2;
		static constexpr int min_exponent			   = 0;
		static constexpr int min_exponent10			   = 0;
		static constexpr int max_exponent			   = 0;
		static constexpr int max_exponent10			   = 0;
#ifdef CCM_TYPES_HAS_INT128
		static constexpr bool traps = numeric_limits<__int128>::traps;
#else  // CCM_TYPES_HAS_INT128
		static constexpr bool traps = numeric_limits<std::uint64_t>::traps;
#endif // CCM_TYPES_HAS_INT128
		static constexpr bool tinyness_before = false;

		static constexpr ccm::types::int128(min)() { return ccm::types::Int128Min(); }
		static constexpr ccm::types::int128 lowest() { return ccm::types::Int128Min(); }
		static constexpr ccm::types::int128(max)() { return ccm::types::Int128Max(); }
		static constexpr ccm::types::int128 epsilon() { return 0; }
		static constexpr ccm::types::int128 round_error() { return 0; }
		static constexpr ccm::types::int128 infinity() { return 0; }
		static constexpr ccm::types::int128 quiet_NaN() { return 0; }
		static constexpr ccm::types::int128 signaling_NaN() { return 0; }
		static constexpr ccm::types::int128 denorm_min() { return 0; }
	};
} // namespace std

/// BEGIN IMPLEMENTATION DETAILS

namespace ccm::types
{
	constexpr uint128 MakeUint128(std::uint64_t high, std::uint64_t low)
	{
		return {high, low};
	}

	/// Assignment from integer types.

	inline uint128 & uint128::operator=(int v)
	{
		return *this = uint128(v);
	}

	inline uint128 & uint128::operator=(unsigned int v)
	{
		return *this = uint128(v);
	}

	inline uint128 & uint128::operator=(long v)
	{
		return *this = uint128(v);
	}

	inline uint128 & uint128::operator=(unsigned long v)
	{
		return *this = uint128(v);
	}

	inline uint128 & uint128::operator=(long long v)
	{
		return *this = uint128(v);
	}

	inline uint128 & uint128::operator=(unsigned long long v)
	{
		return *this = uint128(v);
	}

#ifdef CCM_TYPES_HAS_INT128
	inline uint128 & uint128::operator=(__int128 v)
	{
		return *this = uint128(v);
	}

	inline uint128 & uint128::operator=(unsigned __int128 v)
	{
		return *this = uint128(v);
	}
#endif // CCM_TYPES_HAS_INT128

	inline uint128 & uint128::operator=(int128 v)
	{
		return *this = uint128(v);
	}

	/// Arithmetic operators.

	constexpr uint128 operator<<(uint128 lhs, int amount);
	constexpr uint128 operator>>(uint128 lhs, int amount);
	constexpr uint128 operator+(uint128 lhs, uint128 rhs);
	constexpr uint128 operator-(uint128 lhs, uint128 rhs);
	uint128 operator*(uint128 lhs, uint128 rhs);
	uint128 operator/(uint128 lhs, uint128 rhs);
	uint128 operator%(uint128 lhs, uint128 rhs);

	inline uint128 & uint128::operator<<=(int amount)
	{
		*this = *this << amount;
		return *this;
	}

	inline uint128 & uint128::operator>>=(int amount)
	{
		*this = *this >> amount;
		return *this;
	}

	inline uint128 & uint128::operator+=(uint128 other)
	{
		*this = *this + other;
		return *this;
	}

	inline uint128 & uint128::operator-=(uint128 other)
	{
		*this = *this - other;
		return *this;
	}

	inline uint128 & uint128::operator*=(uint128 other)
	{
		*this = *this * other;
		return *this;
	}

	inline uint128 & uint128::operator/=(uint128 other)
	{
		*this = *this / other;
		return *this;
	}

	inline uint128 & uint128::operator%=(uint128 other)
	{
		*this = *this % other;
		return *this;
	}

	constexpr std::uint64_t Uint128Low64(uint128 v)
	{
		return v.lo_;
	}

	constexpr std::uint64_t Uint128High64(uint128 v)
	{
		return v.hi_;
	}

	/// Constructors from integer types.

#if defined(CCM_IS_LITTLE_ENDIAN)

	constexpr uint128::uint128(std::uint64_t high, std::uint64_t low) : lo_{low}, hi_{high}
	{
	}

	constexpr uint128::uint128(int v) : lo_{static_cast<std::uint64_t>(v)}, hi_{v < 0 ? (std::numeric_limits<std::uint64_t>::max)() : 0}
	{
	}
	constexpr uint128::uint128(long v) : lo_{static_cast<std::uint64_t>(v)}, hi_{v < 0 ? (std::numeric_limits<std::uint64_t>::max)() : 0}
	{
	}
	constexpr uint128::uint128(long long v) : lo_{static_cast<std::uint64_t>(v)}, hi_{v < 0 ? (std::numeric_limits<std::uint64_t>::max)() : 0}
	{
	}

	constexpr uint128::uint128(unsigned int v) : lo_{v}, hi_{0}
	{
	}
	constexpr uint128::uint128(unsigned long v) : lo_{v}, hi_{0}
	{
	}
	constexpr uint128::uint128(unsigned long long v) : lo_{static_cast<std::uint64_t>(v)}, hi_{0}
	{
	}

	#ifdef CCM_TYPES_HAS_INT128
	constexpr uint128::uint128(__int128 v)
		: lo_{static_cast<std::uint64_t>(v & ~std::uint64_t{0})}, hi_{static_cast<std::uint64_t>(static_cast<unsigned __int128>(v) >> 64)}
	{
	}
	constexpr uint128::uint128(unsigned __int128 v) : lo_{static_cast<std::uint64_t>(v & ~std::uint64_t{0})}, hi_{static_cast<std::uint64_t>(v >> 64)}
	{
	}
	#endif // CCM_TYPES_HAS_INT128

	constexpr uint128::uint128(int128 v) : lo_{Int128Low64(v)}, hi_{static_cast<std::uint64_t>(Int128High64(v))}
	{
	}

#elif defined(CCM_IS_BIG_ENDIAN)

	constexpr uint128::uint128(std::uint64_t high, std::uint64_t low) : hi_{high}, lo_{low}
	{
	}

	constexpr uint128::uint128(int v) : hi_{v < 0 ? (std::numeric_limits<std::uint64_t>::max)() : 0}, lo_{static_cast<std::uint64_t>(v)}
	{
	}
	constexpr uint128::uint128(long v) : hi_{v < 0 ? (std::numeric_limits<std::uint64_t>::max)() : 0}, lo_{static_cast<std::uint64_t>(v)}
	{
	}
	constexpr uint128::uint128(long long v) : hi_{v < 0 ? (std::numeric_limits<std::uint64_t>::max)() : 0}, lo_{static_cast<std::uint64_t>(v)}
	{
	}

	constexpr uint128::uint128(unsigned int v) : hi_{0}, lo_{v}
	{
	}
	constexpr uint128::uint128(unsigned long v) : hi_{0}, lo_{v}
	{
	}
	constexpr uint128::uint128(unsigned long long v) : hi_{0}, lo_{v}
	{
	}

	#ifdef CCM_TYPES_HAS_INT128
	constexpr uint128::uint128(__int128 v)
		: hi_{static_cast<std::uint64_t>(static_cast<unsigned __int128>(v) >> 64)}, lo_{static_cast<std::uint64_t>(v & ~std::uint64_t{0})}
	{
	}
	constexpr uint128::uint128(unsigned __int128 v) : hi_{static_cast<std::uint64_t>(v >> 64)}, lo_{static_cast<std::uint64_t>(v & ~std::uint64_t{0})}
	{
	}
	#endif // CCM_TYPES_HAS_INT128

	constexpr uint128::uint128(int128 v) : hi_{static_cast<std::uint64_t>(Int128High64(v))}, lo_{Int128Low64(v)}
	{
	}

#else // byte order
	#error "Unsupported byte order: must be little-endian or big-endian."
#endif // byte order

	/// Conversion operators to integer types.

	constexpr uint128::operator bool() const
	{
		return (lo_ != 0U) || (hi_ != 0U);
	}

	constexpr uint128::operator char() const
	{
		return static_cast<char>(lo_);
	}

	constexpr uint128::operator signed char() const
	{
		return static_cast<signed char>(lo_);
	}

	constexpr uint128::operator unsigned char() const
	{
		return static_cast<unsigned char>(lo_);
	}

	constexpr uint128::operator char16_t() const
	{
		return static_cast<char16_t>(lo_);
	}

	constexpr uint128::operator char32_t() const
	{
		return static_cast<char32_t>(lo_);
	}

	constexpr uint128::operator CCM_INTERNAL_WCHAR_T() const
	{
		return static_cast<CCM_INTERNAL_WCHAR_T>(lo_);
	}

	constexpr uint128::operator short() const
	{
		return static_cast<short>(lo_);
	}

	constexpr uint128::operator unsigned short() const
	{
		return static_cast<unsigned short>(lo_);
	}

	constexpr uint128::operator int() const
	{
		return static_cast<int>(lo_);
	}

	constexpr uint128::operator unsigned int() const
	{
		return static_cast<unsigned int>(lo_);
	}

	constexpr uint128::operator long() const
	{
		return static_cast<long>(lo_);
	}

	constexpr uint128::operator unsigned long() const
	{
		return static_cast<unsigned long>(lo_);
	}

	constexpr uint128::operator long long() const
	{
		return static_cast<long long>(lo_);
	}

	constexpr uint128::operator unsigned long long() const
	{
		return static_cast<unsigned long long>(lo_);
	}

#ifdef CCM_TYPES_HAS_INT128
	constexpr uint128::operator __int128() const
	{
		return (static_cast<__int128>(hi_) << 64) + lo_;
	}

	constexpr uint128::operator unsigned __int128() const
	{
		return (static_cast<unsigned __int128>(hi_) << 64) + lo_;
	}
#endif // CCM_TYPES_HAS_INT128

	// Conversion operators to floating point types.

	/** TODO: Need ldexp working for this to work.
	inline uint128::operator float() const {
		return static_cast<float>(lo_) + ccm::ldexp(static_cast<float>(hi_), 64);
	}

	inline uint128::operator double() const {
		return static_cast<double>(lo_) + ccm::ldexp(static_cast<double>(hi_), 64);
	}

	inline uint128::operator long double() const {
		return static_cast<long double>(lo_) +
			   ccm::ldexp(static_cast<long double>(hi_), 64);
	}
	**/

	/// Comparison operators.

	constexpr bool operator==(uint128 lhs, uint128 rhs)
	{
#ifdef CCM_TYPES_HAS_INT128
		return static_cast<unsigned __int128>(lhs) == static_cast<unsigned __int128>(rhs);
#else
		return (Uint128Low64(lhs) == Uint128Low64(rhs) && Uint128High64(lhs) == Uint128High64(rhs));
#endif
	}

	constexpr bool operator!=(uint128 lhs, uint128 rhs)
	{
		return !(lhs == rhs);
	}

	constexpr bool operator<(uint128 lhs, uint128 rhs)
	{
#ifdef CCM_TYPES_HAS_INT128
		return static_cast<unsigned __int128>(lhs) < static_cast<unsigned __int128>(rhs);
#else
		return (Uint128High64(lhs) == Uint128High64(rhs)) ? (Uint128Low64(lhs) < Uint128Low64(rhs)) : (Uint128High64(lhs) < Uint128High64(rhs));
#endif
	}

	constexpr bool operator>(uint128 lhs, uint128 rhs)
	{
		return rhs < lhs;
	}

	constexpr bool operator<=(uint128 lhs, uint128 rhs)
	{
		return !(rhs < lhs);
	}

	constexpr bool operator>=(uint128 lhs, uint128 rhs)
	{
		return !(lhs < rhs);
	}

#ifdef __cpp_impl_three_way_comparison
	constexpr std::strong_ordering operator<=>(uint128 lhs, uint128 rhs)
	{
	#ifdef CCM_TYPES_HAS_INT128
		if (auto lhs_128 = static_cast<unsigned __int128>(lhs), rhs_128 = static_cast<unsigned __int128>(rhs); lhs_128 < rhs_128)
		{
			return std::strong_ordering::less;
		}
		else if (lhs_128 > rhs_128) { return std::strong_ordering::greater; }
		else { return std::strong_ordering::equal; }
	#else
		if (std::uint64_t lhs_high = Uint128High64(lhs), rhs_high = Uint128High64(rhs); lhs_high < rhs_high) { return std::strong_ordering::less; }
		else if (lhs_high > rhs_high) { return std::strong_ordering::greater; }
		else if (std::uint64_t lhs_low = Uint128Low64(lhs), rhs_low = Uint128Low64(rhs); lhs_low < rhs_low) { return std::strong_ordering::less; }
		else if (lhs_low > rhs_low) { return std::strong_ordering::greater; }
		else { return std::strong_ordering::equal; }
	#endif
	}
#endif

	/// Unary operators.

	constexpr uint128 operator+(uint128 val)
	{
		return val;
	}

	constexpr int128 operator+(int128 val)
	{
		return val;
	}

	constexpr uint128 operator-(uint128 val)
	{
#ifdef CCM_TYPES_HAS_INT128
		return -static_cast<unsigned __int128>(val);
#else
		return MakeUint128(~Uint128High64(val) + static_cast<unsigned long>(Uint128Low64(val) == 0), ~Uint128Low64(val) + 1);
#endif
	}

	constexpr bool operator!(uint128 val)
	{
#ifdef CCM_TYPES_HAS_INT128
		return static_cast<unsigned __int128>(val) == 0U;
#else
		return !Uint128High64(val) && !Uint128Low64(val);
#endif
	}

	/// Logical operators.

	constexpr uint128 operator~(uint128 val)
	{
#ifdef CCM_TYPES_HAS_INT128
		return ~static_cast<unsigned __int128>(val);
#else
		return MakeUint128(~Uint128High64(val), ~Uint128Low64(val));
#endif
	}

	constexpr uint128 operator|(uint128 lhs, uint128 rhs)
	{
#ifdef CCM_TYPES_HAS_INT128
		return static_cast<unsigned __int128>(lhs) | static_cast<unsigned __int128>(rhs);
#else
		return MakeUint128(Uint128High64(lhs) | Uint128High64(rhs), Uint128Low64(lhs) | Uint128Low64(rhs));
#endif
	}

	constexpr uint128 operator&(uint128 lhs, uint128 rhs)
	{
#ifdef CCM_TYPES_HAS_INT128
		return static_cast<unsigned __int128>(lhs) & static_cast<unsigned __int128>(rhs);
#else
		return MakeUint128(Uint128High64(lhs) & Uint128High64(rhs), Uint128Low64(lhs) & Uint128Low64(rhs));
#endif
	}

	constexpr uint128 operator^(uint128 lhs, uint128 rhs)
	{
#ifdef CCM_TYPES_HAS_INT128
		return static_cast<unsigned __int128>(lhs) ^ static_cast<unsigned __int128>(rhs);
#else
		return MakeUint128(Uint128High64(lhs) ^ Uint128High64(rhs), Uint128Low64(lhs) ^ Uint128Low64(rhs));
#endif
	}

	inline uint128 & uint128::operator|=(uint128 other)
	{
		*this = *this | other;
		return *this;
	}

	inline uint128 & uint128::operator&=(uint128 other)
	{
		*this = *this & other;
		return *this;
	}

	inline uint128 & uint128::operator^=(uint128 other)
	{
		*this = *this ^ other;
		return *this;
	}

	/// Arithmetic operators.

	constexpr uint128 operator<<(uint128 lhs, int amount)
	{
#ifdef CCM_TYPES_HAS_INT128
		return static_cast<unsigned __int128>(lhs) << amount;
#else
		// uint64_t shifts of >= 64 are undefined, so we will need some
		// special-casing.
		return amount >= 64	 ? MakeUint128(Uint128Low64(lhs) << (amount - 64), 0)
			   : amount == 0 ? lhs
							 : MakeUint128((Uint128High64(lhs) << amount) | (Uint128Low64(lhs) >> (64 - amount)), Uint128Low64(lhs) << amount);
#endif
	}

	constexpr uint128 operator>>(uint128 lhs, int amount)
	{
#ifdef CCM_TYPES_HAS_INT128
		return static_cast<unsigned __int128>(lhs) >> amount;
#else
		// uint64_t shifts of >= 64 are undefined, so we will need some
		// special-casing.
		return amount >= 64	 ? MakeUint128(0, Uint128High64(lhs) >> (amount - 64))
			   : amount == 0 ? lhs
							 : MakeUint128(Uint128High64(lhs) >> amount, (Uint128Low64(lhs) >> amount) | (Uint128High64(lhs) << (64 - amount)));
#endif
	}

#if !defined(CCM_TYPES_HAS_INT128)
	namespace int128_internal
	{
		constexpr uint128 AddResult(uint128 result, uint128 lhs)
		{
			// check for carry
			return (Uint128Low64(result) < Uint128Low64(lhs)) ? MakeUint128(Uint128High64(result) + 1, Uint128Low64(result)) : result;
		}
	} // namespace int128_internal
#endif

	constexpr uint128 operator+(uint128 lhs, uint128 rhs)
	{
#if defined(CCM_TYPES_HAS_INT128)
		return static_cast<unsigned __int128>(lhs) + static_cast<unsigned __int128>(rhs);
#else
		return int128_internal::AddResult(MakeUint128(Uint128High64(lhs) + Uint128High64(rhs), Uint128Low64(lhs) + Uint128Low64(rhs)), lhs);
#endif
	}

#if !defined(CCM_TYPES_HAS_INT128)
	namespace int128_internal
	{
		constexpr uint128 SubstructResult(uint128 result, uint128 lhs, uint128 rhs)
		{
			// check for carry
			return (Uint128Low64(lhs) < Uint128Low64(rhs)) ? MakeUint128(Uint128High64(result) - 1, Uint128Low64(result)) : result;
		}
	} // namespace int128_internal
#endif

	constexpr uint128 operator-(uint128 lhs, uint128 rhs)
	{
#if defined(CCM_TYPES_HAS_INT128)
		return static_cast<unsigned __int128>(lhs) - static_cast<unsigned __int128>(rhs);
#else
		return int128_internal::SubstructResult(MakeUint128(Uint128High64(lhs) - Uint128High64(rhs), Uint128Low64(lhs) - Uint128Low64(rhs)), lhs, rhs);
#endif
	}

	inline uint128 operator*(uint128 lhs, uint128 rhs)
	{
#if defined(CCM_TYPES_HAS_INT128)
		return static_cast<unsigned __int128>(lhs) * static_cast<unsigned __int128>(rhs);
#elif defined(_MSC_VER) && defined(_M_X64) && !defined(_M_ARM64EC)
		std::uint64_t carry;
		std::uint64_t low = _umul128(Uint128Low64(lhs), Uint128Low64(rhs), &carry);
		return MakeUint128(Uint128Low64(lhs) * Uint128High64(rhs) + Uint128High64(lhs) * Uint128Low64(rhs) + carry, low);
#else  // CCM_TYPES_HAS_INT128
		std::uint64_t a32 = Uint128Low64(lhs) >> 32;
		std::uint64_t a00 = Uint128Low64(lhs) & 0xffffffff;
		std::uint64_t b32 = Uint128Low64(rhs) >> 32;
		std::uint64_t b00 = Uint128Low64(rhs) & 0xffffffff;
		uint128 result	  = MakeUint128(Uint128High64(lhs) * Uint128Low64(rhs) + Uint128Low64(lhs) * Uint128High64(rhs) + a32 * b32, a00 * b00);
		result += uint128(a32 * b00) << 32;
		result += uint128(a00 * b32) << 32;
		return result;
#endif // CCM_TYPES_HAS_INT128
	}

#if defined(CCM_TYPES_HAS_INT128)
	inline uint128 operator/(uint128 lhs, uint128 rhs)
	{
		return static_cast<unsigned __int128>(lhs) / static_cast<unsigned __int128>(rhs);
	}

	inline uint128 operator%(uint128 lhs, uint128 rhs)
	{
		return static_cast<unsigned __int128>(lhs) % static_cast<unsigned __int128>(rhs);
	}
#endif

	/// Increment/decrement operators.

	inline uint128 uint128::operator++(int)
	{
		uint128 tmp(*this);
		*this += 1;
		return tmp;
	}

	inline uint128 uint128::operator--(int)
	{
		uint128 tmp(*this);
		*this -= 1;
		return tmp;
	}

	inline uint128 & uint128::operator++()
	{
		*this += 1;
		return *this;
	}

	inline uint128 & uint128::operator--()
	{
		*this -= 1;
		return *this;
	}

	constexpr int128 MakeInt128(std::int64_t high, std::uint64_t low)
	{
		return {high, low};
	}

	/// Assignment from integer types.

	inline int128 & int128::operator=(int v)
	{
		return *this = int128(v);
	}

	inline int128 & int128::operator=(unsigned int v)
	{
		return *this = int128(v);
	}

	inline int128 & int128::operator=(long v)
	{
		return *this = int128(v);
	}

	inline int128 & int128::operator=(unsigned long v)
	{
		return *this = int128(v);
	}

	inline int128 & int128::operator=(long long v)
	{
		return *this = int128(v);
	}

	inline int128 & int128::operator=(unsigned long long v)
	{
		return *this = int128(v);
	}

	/// Arithmetic operators.

	constexpr int128 operator-(int128 v);
	constexpr int128 operator+(int128 lhs, int128 rhs);
	constexpr int128 operator-(int128 lhs, int128 rhs);
	int128 operator*(int128 lhs, int128 rhs);
	int128 operator/(int128 lhs, int128 rhs);
	int128 operator%(int128 lhs, int128 rhs);
	constexpr int128 operator|(int128 lhs, int128 rhs);
	constexpr int128 operator&(int128 lhs, int128 rhs);
	constexpr int128 operator^(int128 lhs, int128 rhs);
	constexpr int128 operator<<(int128 lhs, int amount);
	constexpr int128 operator>>(int128 lhs, int amount);

	inline int128 & int128::operator+=(int128 other)
	{
		*this = *this + other;
		return *this;
	}

	inline int128 & int128::operator-=(int128 other)
	{
		*this = *this - other;
		return *this;
	}

	inline int128 & int128::operator*=(int128 other)
	{
		*this = *this * other;
		return *this;
	}

	inline int128 & int128::operator/=(int128 other)
	{
		*this = *this / other;
		return *this;
	}

	inline int128 & int128::operator%=(int128 other)
	{
		*this = *this % other;
		return *this;
	}

	inline int128 & int128::operator|=(int128 other)
	{
		*this = *this | other;
		return *this;
	}

	inline int128 & int128::operator&=(int128 other)
	{
		*this = *this & other;
		return *this;
	}

	inline int128 & int128::operator^=(int128 other)
	{
		*this = *this ^ other;
		return *this;
	}

	inline int128 & int128::operator<<=(int amount)
	{
		*this = *this << amount;
		return *this;
	}

	inline int128 & int128::operator>>=(int amount)
	{
		*this = *this >> amount;
		return *this;
	}

	/// Forward declaration for comparison operators.

	constexpr bool operator!=(int128 lhs, int128 rhs);

	namespace int128_internal
	{

		// Casts from unsigned to signed while preserving the underlying binary
		// representation.
		constexpr int64_t BitCastToSigned(uint64_t v)
		{
			// Casting an unsigned integer to a signed integer of the same
			// width is implementation defined behavior if the source value would not fit
			// in the destination type. We step around it with a roundtrip bitwise not
			// operation to make sure this function remains constexpr. Clang, GCC, and
			// MSVC optimize this to a no-op on x86-64.
			return (v & (std::uint64_t{1} << 63)) != 0U ? ~static_cast<std::int64_t>(~v) : static_cast<std::int64_t>(v);
		}
	} // namespace int128_internal

#if defined(CCM_TYPES_HAS_INT128)

	namespace int128_internal
	{

		// Casts from unsigned to signed while preserving the underlying binary
		// representation.
		constexpr __int128 BitCastToSigned(unsigned __int128 v)
		{
			// Casting an unsigned integer to a signed integer of the same
			// width is implementation defined behavior if the source value would not fit
			// in the destination type. We step around it with a roundtrip bitwise not
			// operation to make sure this function remains constexpr. Clang and GCC
			// optimize this to a no-op on x86-64.
			return v & (static_cast<unsigned __int128>(1) << 127) ? ~static_cast<__int128>(~v) : static_cast<__int128>(v);
		}

	} // namespace int128_internal

	inline int128 & int128::operator=(__int128 v)
	{
		v_ = v;
		return *this;
	}

	constexpr std::uint64_t Int128Low64(int128 v)
	{
		return static_cast<uint64_t>(v.v_ & ~uint64_t{0});
	}

	constexpr std::int64_t Int128High64(int128 v)
	{
		// Initially cast to unsigned to prevent a right shift on a negative value.
		return int128_internal::BitCastToSigned(static_cast<std::uint64_t>(static_cast<unsigned __int128>(v.v_) >> 64));
	}

	constexpr int128::int128(std::int64_t high, std::uint64_t low)
		// Initially cast to unsigned to prevent a left shift that overflows.
		: v_(int128_internal::BitCastToSigned(static_cast<unsigned __int128>(high) << 64) | low)
	{
	}

	constexpr int128::int128(int v) : v_{v}
	{
	}

	constexpr int128::int128(long v) : v_{v}
	{
	}

	constexpr int128::int128(long long v) : v_{v}
	{
	}

	constexpr int128::int128(__int128 v) : v_{v}
	{
	}

	constexpr int128::int128(unsigned int v) : v_{v}
	{
	}

	constexpr int128::int128(unsigned long v) : v_{v}
	{
	}

	constexpr int128::int128(unsigned long long v) : v_{v}
	{
	}

	constexpr int128::int128(unsigned __int128 v) : v_{static_cast<__int128>(v)}
	{
	}

	inline int128::int128(float v)
	{
		v_ = static_cast<__int128>(v);
	}

	inline int128::int128(double v)
	{
		v_ = static_cast<__int128>(v);
	}

	inline int128::int128(long double v)
	{
		v_ = static_cast<__int128>(v);
	}

	constexpr int128::int128(uint128 v) : v_{static_cast<__int128>(v)}
	{
	}

	constexpr int128::operator bool() const
	{
		return static_cast<bool>(v_);
	}

	constexpr int128::operator char() const
	{
		return static_cast<char>(v_);
	}

	constexpr int128::operator signed char() const
	{
		return static_cast<signed char>(v_);
	}

	constexpr int128::operator unsigned char() const
	{
		return static_cast<unsigned char>(v_);
	}

	constexpr int128::operator char16_t() const
	{
		return static_cast<char16_t>(v_);
	}

	constexpr int128::operator char32_t() const
	{
		return static_cast<char32_t>(v_);
	}

	constexpr int128::operator CCM_INTERNAL_WCHAR_T() const
	{
		return static_cast<CCM_INTERNAL_WCHAR_T>(v_);
	}

	constexpr int128::operator short() const
	{
		return static_cast<short>(v_);
	}

	constexpr int128::operator unsigned short() const
	{
		return static_cast<unsigned short>(v_);
	}

	constexpr int128::operator int() const
	{
		return static_cast<int>(v_);
	}

	constexpr int128::operator unsigned int() const
	{
		return static_cast<unsigned int>(v_);
	}

	constexpr int128::operator long() const
	{
		return static_cast<long>(v_);
	}

	constexpr int128::operator unsigned long() const
	{
		return static_cast<unsigned long>(v_);
	}

	constexpr int128::operator long long() const
	{
		return static_cast<long long>(v_);
	}

	constexpr int128::operator unsigned long long() const
	{
		return static_cast<unsigned long long>(v_);
	}

	constexpr int128::operator __int128() const
	{
		return v_;
	}

	constexpr int128::operator unsigned __int128() const
	{
		return static_cast<unsigned __int128>(v_);
	}

	// Clang on PowerPC sometimes produces incorrect __int128 to floating point
	// conversions. In that case, we do the conversion with a similar implementation
	// to the conversion operators in int128_no_intrinsic.inc.
	#if defined(__clang__) && !defined(__ppc64__)
	inline int128::operator float() const
	{
		return static_cast<float>(v_);
	}

	inline int128::operator double() const
	{
		return static_cast<double>(v_);
	}

	inline int128::operator long double() const
	{
		return static_cast<long double>(v_);
	}

	#else  // Clang on PowerPC

	/* TODO: Need ldexp working for this to work.
	inline int128::operator float() const {
	  // We must convert the absolute value and then negate as needed, because
	  // floating point types are typically sign-magnitude. Otherwise, the
	  // difference between the high and low 64 bits when interpreted as two's
	  // complement overwhelms the precision of the mantissa.
	  //
	  // Also check to make sure we don't negate Int128Min()
	  return v_ < 0 && *this != Int128Min()
				 ? -static_cast<float>(-*this)
				 : static_cast<float>(Int128Low64(*this)) +
					   ccm::ldexp(static_cast<float>(Int128High64(*this)), 64);
	}

	inline int128::operator double() const {
	  // See comment in int128::operator float() above.
	  return v_ < 0 && *this != Int128Min()
				 ? -static_cast<double>(-*this)
				 : static_cast<double>(Int128Low64(*this)) +
					   ccm::ldexp(static_cast<double>(Int128High64(*this)), 64);
	}

	inline int128::operator long double() const {
	  // See comment in int128::operator float() above.
	  return v_ < 0 && *this != Int128Min()
				 ? -static_cast<long double>(-*this)
				 : static_cast<long double>(Int128Low64(*this)) +
					   ccm::ldexp(static_cast<long double>(Int128High64(*this)),
								  64);
	}
	*/
	#endif // Clang on PowerPC

	// Comparison operators.

	constexpr bool operator==(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) == static_cast<__int128>(rhs);
	}

	constexpr bool operator!=(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) != static_cast<__int128>(rhs);
	}

	constexpr bool operator<(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) < static_cast<__int128>(rhs);
	}

	constexpr bool operator>(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) > static_cast<__int128>(rhs);
	}

	constexpr bool operator<=(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) <= static_cast<__int128>(rhs);
	}

	constexpr bool operator>=(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) >= static_cast<__int128>(rhs);
	}

	#ifdef __cpp_impl_three_way_comparison
	constexpr std::strong_ordering operator<=>(int128 lhs, int128 rhs)
	{
		if (auto lhs_128 = static_cast<__int128>(lhs), rhs_128 = static_cast<__int128>(rhs); lhs_128 < rhs_128) { return std::strong_ordering::less; }
		else if (lhs_128 > rhs_128) { return std::strong_ordering::greater; }
		else { return std::strong_ordering::equal; }
	}
	#endif

	// Unary operators.

	constexpr int128 operator-(int128 v)
	{
		return -static_cast<__int128>(v);
	}

	constexpr bool operator!(int128 v)
	{
		return !static_cast<__int128>(v);
	}

	constexpr int128 operator~(int128 val)
	{
		return ~static_cast<__int128>(val);
	}

	// Arithmetic operators.

	constexpr int128 operator+(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) + static_cast<__int128>(rhs);
	}

	constexpr int128 operator-(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) - static_cast<__int128>(rhs);
	}

	inline int128 operator*(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) * static_cast<__int128>(rhs);
	}

	inline int128 operator/(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) / static_cast<__int128>(rhs);
	}

	inline int128 operator%(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) % static_cast<__int128>(rhs);
	}

	inline int128 int128::operator++(int)
	{
		int128 tmp(*this);
		++v_;
		return tmp;
	}

	inline int128 int128::operator--(int)
	{
		int128 tmp(*this);
		--v_;
		return tmp;
	}

	inline int128 & int128::operator++()
	{
		++v_;
		return *this;
	}

	inline int128 & int128::operator--()
	{
		--v_;
		return *this;
	}

	constexpr int128 operator|(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) | static_cast<__int128>(rhs);
	}

	constexpr int128 operator&(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) & static_cast<__int128>(rhs);
	}

	constexpr int128 operator^(int128 lhs, int128 rhs)
	{
		return static_cast<__int128>(lhs) ^ static_cast<__int128>(rhs);
	}

	constexpr int128 operator<<(int128 lhs, int amount)
	{
		return static_cast<__int128>(lhs) << amount;
	}

	constexpr int128 operator>>(int128 lhs, int amount)
	{
		return static_cast<__int128>(lhs) >> amount;
	}

#else // CCM_TYPES_HAS_INT128

	constexpr std::uint64_t Int128Low64(int128 v)
	{
		return v.lo_;
	}

	constexpr std::int64_t Int128High64(int128 v)
	{
		return v.hi_;
	}

	#if defined(CCM_IS_LITTLE_ENDIAN)

	constexpr int128::int128(istd::nt64_t high, std::uint64_t low) : lo_(low), hi_(high)
	{
	}

	constexpr int128::int128(int v) : lo_{static_cast<std::uint64_t>(v)}, hi_{v < 0 ? ~std::int64_t{0} : 0}
	{
	}
	constexpr int128::int128(long v) : lo_{static_cast<std::uint64_t>(v)}, hi_{v < 0 ? ~std::int64_t{0} : 0}
	{
	}
	constexpr int128::int128(long long v) : lo_{static_cast<std::uint64_t>(v)}, hi_{v < 0 ? ~std::int64_t{0} : 0}
	{
	}

	constexpr int128::int128(unsigned int v) : lo_{v}, hi_{0}
	{
	}
	constexpr int128::int128(unsigned long v) : lo_{v}, hi_{0}
	{
	}
	constexpr int128::int128(unsigned long long v) : lo_{v}, hi_{0}
	{
	}

	constexpr int128::int128(uint128 v) : lo_{Uint128Low64(v)}, hi_{static_cast<std::int64_t>(Uint128High64(v))}
	{
	}

	#elif defined(CCM_IS_BIG_ENDIAN)

	constexpr int128::int128(std::int64_t high, std::uint64_t low) : hi_{high}, lo_{low}
	{
	}

	constexpr int128::int128(int v) : hi_{v < 0 ? ~std::int64_t{0} : 0}, lo_{static_cast<std::uint64_t>(v)}
	{
	}

	constexpr int128::int128(long v) : hi_{v < 0 ? ~std::int64_t{0} : 0}, lo_{static_cast<std::uint64_t>(v)}
	{
	}

	constexpr int128::int128(long long v) : hi_{v < 0 ? ~std::int64_t{0} : 0}, lo_{static_cast<std::uint64_t>(v)}
	{
	}

	constexpr int128::int128(unsigned int v) : hi_{0}, lo_{v}
	{
	}

	constexpr int128::int128(unsigned long v) : hi_{0}, lo_{v}
	{
	}

	constexpr int128::int128(unsigned long long v) : hi_{0}, lo_{v}
	{
	}

	constexpr int128::int128(uint128 v) : hi_{static_cast<std::int64_t>(Uint128High64(v))}, lo_{Uint128Low64(v)}
	{
	}

	#else // byte order
		#error "Unsupported byte order: must be little-endian or big-endian."
	#endif // byte order

	constexpr int128::operator bool() const
	{
		return lo_ || hi_;
	}

	constexpr int128::operator char() const
	{
		return static_cast<char>(static_cast<long long>(*this));
	}

	constexpr int128::operator signed char() const
	{
		return static_cast<signed char>(static_cast<long long>(*this));
	}

	constexpr int128::operator unsigned char() const
	{
		return static_cast<unsigned char>(lo_);
	}

	constexpr int128::operator char16_t() const
	{
		return static_cast<char16_t>(lo_);
	}

	constexpr int128::operator char32_t() const
	{
		return static_cast<char32_t>(lo_);
	}

	constexpr int128::operator CCM_INTERNAL_WCHAR_T() const
	{
		return static_cast<CCM_INTERNAL_WCHAR_T>(static_cast<long long>(*this));
	}

	constexpr int128::operator short() const
	{
		return static_cast<short>(static_cast<long long>(*this));
	}

	constexpr int128::operator unsigned short() const
	{
		return static_cast<unsigned short>(lo_);
	}

	constexpr int128::operator int() const
	{
		return static_cast<int>(static_cast<long long>(*this));
	}

	constexpr int128::operator unsigned int() const
	{
		return static_cast<unsigned int>(lo_);
	}

	constexpr int128::operator long() const
	{
		return static_cast<long>(static_cast<long long>(*this));
	}

	constexpr int128::operator unsigned long() const
	{
		return static_cast<unsigned long>(lo_);
	}

	constexpr int128::operator long long() const
	{
		// We don't bother checking the value of hi_. If *this < 0, lo_'s high bit
		// must be set in order for the value to fit into a long long. Conversely, if
		// lo_'s high bit is set, *this must be < 0 for the value to fit.
		return int128_internal::BitCastToSigned(lo_);
	}

	constexpr int128::operator unsigned long long() const
	{
		return static_cast<unsigned long long>(lo_);
	}

	inline int128::operator float() const
	{
		// We must convert the absolute value and then negate as needed, because
		// floating point types are typically sign-magnitude. Otherwise, the
		// difference between the high and low 64 bits when interpreted as two's
		// complement overwhelms the precision of the mantissa.
		//
		// Also check to make sure we don't negate Int128Min()
		return hi_ < 0 && *this != Int128Min() ? -static_cast<float>(-*this) : static_cast<float>(lo_) + std::ldexp(static_cast<float>(hi_), 64);
	}

	inline int128::operator double() const
	{
		// See comment in int128::operator float() above.
		return hi_ < 0 && *this != Int128Min() ? -static_cast<double>(-*this) : static_cast<double>(lo_) + std::ldexp(static_cast<double>(hi_), 64);
	}

	inline int128::operator long double() const
	{
		// See comment in int128::operator float() above.
		return hi_ < 0 && *this != Int128Min() ? -static_cast<long double>(-*this)
											   : static_cast<long double>(lo_) + std::ldexp(static_cast<long double>(hi_), 64);
	}

	// Comparison operators.

	constexpr bool operator==(int128 lhs, int128 rhs)
	{
		return (Int128Low64(lhs) == Int128Low64(rhs) && Int128High64(lhs) == Int128High64(rhs));
	}

	constexpr bool operator!=(int128 lhs, int128 rhs)
	{
		return !(lhs == rhs);
	}

	constexpr bool operator<(int128 lhs, int128 rhs)
	{
		return (Int128High64(lhs) == Int128High64(rhs)) ? (Int128Low64(lhs) < Int128Low64(rhs)) : (Int128High64(lhs) < Int128High64(rhs));
	}

	constexpr bool operator>(int128 lhs, int128 rhs)
	{
		return (Int128High64(lhs) == Int128High64(rhs)) ? (Int128Low64(lhs) > Int128Low64(rhs)) : (Int128High64(lhs) > Int128High64(rhs));
	}

	constexpr bool operator<=(int128 lhs, int128 rhs)
	{
		return !(lhs > rhs);
	}

	constexpr bool operator>=(int128 lhs, int128 rhs)
	{
		return !(lhs < rhs);
	}

	#ifdef __cpp_impl_three_way_comparison
	constexpr std::strong_ordering operator<=>(int128 lhs, int128 rhs)
	{
		if (std::int64_t lhs_high = Int128High64(lhs), rhs_high = Int128High64(rhs); lhs_high < rhs_high) { return std::strong_ordering::less; }
		else if (lhs_high > rhs_high) { return std::strong_ordering::greater; }
		else if (std::uint64_t lhs_low = Uint128Low64(lhs), rhs_low = Uint128Low64(rhs); lhs_low < rhs_low) { return std::strong_ordering::less; }
		else if (lhs_low > rhs_low) { return std::strong_ordering::greater; }
		else { return std::strong_ordering::equal; }
	}
	#endif

	// Unary operators.

	constexpr int128 operator-(int128 v)
	{
		return MakeInt128(~Int128High64(v) + (Int128Low64(v) == 0), ~Int128Low64(v) + 1);
	}

	constexpr bool operator!(int128 v)
	{
		return !Int128Low64(v) && !Int128High64(v);
	}

	constexpr int128 operator~(int128 val)
	{
		return MakeInt128(~Int128High64(val), ~Int128Low64(val));
	}

	// Arithmetic operators.

	namespace int128_internal
	{
		constexpr int128 SignedAddResult(int128 result, int128 lhs)
		{
			// check for carry
			return (Int128Low64(result) < Int128Low64(lhs)) ? MakeInt128(Int128High64(result) + 1, Int128Low64(result)) : result;
		}
	} // namespace int128_internal
	constexpr int128 operator+(int128 lhs, int128 rhs)
	{
		return int128_internal::SignedAddResult(MakeInt128(Int128High64(lhs) + Int128High64(rhs), Int128Low64(lhs) + Int128Low64(rhs)), lhs);
	}

	namespace int128_internal
	{
		constexpr int128 SignedSubstructResult(int128 result, int128 lhs, int128 rhs)
		{
			// check for carry
			return (Int128Low64(lhs) < Int128Low64(rhs)) ? MakeInt128(Int128High64(result) - 1, Int128Low64(result)) : result;
		}
	} // namespace int128_internal
	constexpr int128 operator-(int128 lhs, int128 rhs)
	{
		return int128_internal::SignedSubstructResult(MakeInt128(Int128High64(lhs) - Int128High64(rhs), Int128Low64(lhs) - Int128Low64(rhs)), lhs, rhs);
	}

	inline int128 operator*(int128 lhs, int128 rhs)
	{
		return MakeInt128(int128_internal::BitCastToSigned(Uint128High64(uint128(lhs) * rhs)), Uint128Low64(uint128(lhs) * rhs));
	}

	inline int128 int128::operator++(int)
	{
		int128 tmp(*this);
		*this += 1;
		return tmp;
	}

	inline int128 int128::operator--(int)
	{
		int128 tmp(*this);
		*this -= 1;
		return tmp;
	}

	inline int128 & int128::operator++()
	{
		*this += 1;
		return *this;
	}

	inline int128 & int128::operator--()
	{
		*this -= 1;
		return *this;
	}

	constexpr int128 operator|(int128 lhs, int128 rhs)
	{
		return MakeInt128(Int128High64(lhs) | Int128High64(rhs), Int128Low64(lhs) | Int128Low64(rhs));
	}

	constexpr int128 operator&(int128 lhs, int128 rhs)
	{
		return MakeInt128(Int128High64(lhs) & Int128High64(rhs), Int128Low64(lhs) & Int128Low64(rhs));
	}

	constexpr int128 operator^(int128 lhs, int128 rhs)
	{
		return MakeInt128(Int128High64(lhs) ^ Int128High64(rhs), Int128Low64(lhs) ^ Int128Low64(rhs));
	}

	constexpr int128 operator<<(int128 lhs, int amount)
	{
		// int64_t shifts of >= 63 are undefined, so we need some special-casing.
		assert(amount >= 0 && amount < 127);
		if (amount <= 0) { return lhs; }
		else if (amount < 63)
		{
			return MakeInt128((Int128High64(lhs) << amount) | static_cast<std::int64_t>(Int128Low64(lhs) >> (64 - amount)), Int128Low64(lhs) << amount);
		}
		else if (amount == 63)
		{
			return MakeInt128(((Int128High64(lhs) << 32) << 31) | static_cast<istd::nt64_t>(Int128Low64(lhs) >> 1), (Int128Low64(lhs) << 32) << 31);
		}
		else if (amount == 127) { return MakeInt128(static_cast<std::int64_t>(Int128Low64(lhs) << 63), 0); }
		else if (amount > 127) { return MakeInt128(0, 0); }
		else
		{
			// amount >= 64 && amount < 127
			return MakeInt128(static_cast<int64_t>(Int128Low64(lhs) << (amount - 64)), 0);
		}
	}

	constexpr int128 operator>>(std::int128 lhs, int amount)
	{
		// int64_t shifts of >= 63 are undefined, so we need some special-casing.
		assert(amount >= 0 && amount < 127);
		if (amount <= 0) { return lhs; }
		else if (amount < 63)
		{
			return MakeInt128(Int128High64(lhs) >> amount, Int128Low64(lhs) >> amount | static_cast<std::uint64_t>(Int128High64(lhs)) << (64 - amount));
		}
		else if (amount == 63)
		{
			return MakeInt128((Int128High64(lhs) >> 32) >> 31, static_cast<std::uint64_t>(Int128High64(lhs) << 1) | (Int128Low64(lhs) >> 32) >> 31);
		}
		else if (amount >= 127) { return MakeInt128((Int128High64(lhs) >> 32) >> 31, static_cast<std::uint64_t>((Int128High64(lhs) >> 32) >> 31)); }
		else
		{
			// amount >= 64 && amount < 127
			return MakeInt128((Int128High64(lhs) >> 32) >> 31, static_cast<std::uint64_t>(Int128High64(lhs) >> (amount - 64)));
		}
	}

#endif // CCM_TYPES_HAS_INT128

} // namespace ccm::types

#undef CCM_INTERNAL_WCHAR_T