@rnd = float<ieee_64, ne>;

# powf primary exp2 polynomial evaluation (non-FMA path — conservative bound).
# Corresponds to powf_impl.hpp lines 450-458:
#   EXP2_COEFFS = {C0=1, C1, C2, C3, C4, C5}
#   d0 = fma(lo6, C1, C0)   (includes C0=1, unlike the double path)
#   d1 = fma(lo6, C3, C2)
#   d2 = fma(lo6, C5, C4)
#   pp = polyeval(lo6_sqr, d0, d1, d2) = d0 + lo6_sqr*(d1 + lo6_sqr*d2)
# Domain: lo6 in [-2^-1, 2^-1] (lo6 = 2^6 * lo, |lo| <= 2^-7).
#
# Each assertion is the tight interval computed by Gappa on first run.

x2 = rnd(x * x);
x2e = x * x;

m0 = rnd(x * 0x1.62e42fefa39efp-7);
m0e = x * 0x1.62e42fefa39efp-7;
d0 = rnd(m0 + 0x1p0);
d0e = m0e + 0x1p0;

m1 = rnd(x * 0x1.c6b08d7076268p-23);
m1e = x * 0x1.c6b08d7076268p-23;
d1 = rnd(m1 + 0x1.ebfbdff82a23ap-15);
d1e = m1e + 0x1.ebfbdff82a23ap-15;

m2 = rnd(x * 0x1.5d870c4d84445p-40);
m2e = x * 0x1.5d870c4d84445p-40;
d2 = rnd(m2 + 0x1.3b2ad33f8b48bp-31);
d2e = m2e + 0x1.3b2ad33f8b48bp-31;

# Horner: polyeval(x2, d0, d1, d2) = d0 + x2*(d1 + x2*d2)
h0 = rnd(x2 * d2);
h0e = x2e * d2e;
h1 = rnd(h0 + d1);
h1e = h0e + d1e;
h2 = rnd(x2 * h1);
h2e = x2e * h1e;
exp_eval = rnd(h2 + d0);
P = h2e + d0e;

{ x in [-0x1p-1, 0x1p-1] ->
  x2 - x2e in [-1b-56, 1b-56] /\
  d0 - d0e in [-257b-61, 257b-61] /\
  d1 - d1e in [-513b-77, 513b-77] /\
  d2 - d2e in [-1025b-94, 1025b-94] /\
  h0 - h0e in [-5893420242074279b-137, 5893420242074279b-137] /\
  h1 - h1e in [-36064161243965461b-122, 36064161243965461b-122] /\
  h2 - h2e in [-71419992251276287b-124, 71419992251276287b-124] /\
  exp_eval - P in [-577595370471038973b-111, 577595370471038973b-111] }
