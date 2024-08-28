
#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/runtime/simd/simd_vectorize.hpp"
#include "ccmath/internal/support/always_false.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/rounding_mode.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include "ccmath/internal/types/normalized_float.hpp"
#include "ccmath/internal/types/sign.hpp"

#include <type_traits>


namespace ccm::gen::internal
{

static constexpr int QUOTIENT_LSB_BITS = 3;

// The implementation is a bit-by-bit algorithm which uses integer division
// to evaluate the quotient and remainder.
template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
constexpr T remquo(T x, T y, int &q)
{
using FPBits_t = support::fp::FPBits<T>;
  FPBits_t xbits(x), ybits(y);
  if (xbits.is_nan())
    return x;
  if (ybits.is_nan())
    return y;
  if (xbits.is_inf() || ybits.is_zero())
    return FPBits_t::quiet_nan().get_val();

  if (xbits.is_zero()) {
    q = 0;
    return LIBC_NAMESPACE::fputil::copysign(T(0.0), x);
  }

  if (ybits.is_inf()) {
    q = 0;
    return x;
  }

  const types::Sign result_sign =
      (xbits.sign() == ybits.sign() ? types::Sign::POS : types::Sign::NEG);

  // Once we know the sign of the result, we can just operate on the absolute
  // values. The correct sign can be applied to the result after the result
  // is evaluated.
  xbits.set_sign(types::Sign::POS);
  ybits.set_sign(types::Sign::POS);

  types::NormalizedFloat<T> normalx(xbits), normaly(ybits);
  int exp = normalx.exponent - normaly.exponent;
  typename types::NormalizedFloat<T>::StorageType mx = normalx.mantissa,
                                       my = normaly.mantissa;

  q = 0;
  while (exp >= 0) {
    unsigned shift_count = 0;
    typename types::NormalizedFloat<T>::StorageType n = mx;
    for (shift_count = 0; n < my; n <<= 1, ++shift_count)
      ;

    if (static_cast<int>(shift_count) > exp)
      break;

    exp -= shift_count;
    if (0 <= exp && exp < QUOTIENT_LSB_BITS)
      q |= (1 << exp);

    mx = n - my;
    if (mx == 0) {
      q = result_sign.is_neg() ? -q : q;
      return LIBC_NAMESPACE::fputil::copysign(T(0.0), x);
    }
  }

  NormalFloat<T> remainder(Sign::POS, exp + normaly.exponent, mx);

  // Since NormalFloat to native type conversion is a truncation operation
  // currently, the remainder value in the native type is correct as is.
  // However, if NormalFloat to native type conversion is updated in future,
  // then the conversion to native remainder value should be updated
  // appropriately and some directed tests added.
  T native_remainder(remainder);
  T absy = ybits.get_val();
  int cmp = remainder.mul2(1).cmp(normaly);
  if (cmp > 0) {
    q = q + 1;
    if (x >= T(0.0))
      native_remainder = native_remainder - absy;
    else
      native_remainder = absy - native_remainder;
  } else if (cmp == 0) {
    if (q & 1) {
      q += 1;
      if (x >= T(0.0))
        native_remainder = -native_remainder;
    } else {
      if (x < T(0.0))
        native_remainder = -native_remainder;
    }
  } else {
    if (x < T(0.0))
      native_remainder = -native_remainder;
  }

  q = result_sign.is_neg() ? -q : q;
  if (native_remainder == T(0.0))
    return LIBC_NAMESPACE::fputil::copysign(T(0.0), x);
  return native_remainder;
}
}