@rnd = float<ieee_64, ne>;

# pow double exp2 polynomial evaluation (non-FMA path — conservative bound).
# Corresponds to pow_impl.hpp:
#   POW_EXP2_COEFFS = {C0=1, C1, C2, C3, C4, C5}
#   d0 = fma(lo6, C2, C1)   (C0=1 is folded into exp2_hm_hi separately)
#   d1 = fma(lo6, C4, C3)
#   pp = polyeval(lo6_sqr, d0, d1, C5)
# Domain: lo6 in [-2^-1, 2^-1].
#
# Each assertion is the tight interval computed by Gappa on first run.

x2 = rnd(x * x);
x2e = x * x;

m0 = rnd(x * 0x1.ebfbdff82a23ap-15);
m0e = x * 0x1.ebfbdff82a23ap-15;
d0 = rnd(m0 + 0x1.62e42fefa39efp-7);
d0e = m0e + 0x1.62e42fefa39efp-7;

m1 = rnd(x * 0x1.3b2ad33f8b48bp-31);
m1e = x * 0x1.3b2ad33f8b48bp-31;
d1 = rnd(m1 + 0x1.c6b08d7076268p-23);
d1e = m1e + 0x1.c6b08d7076268p-23;

# Horner: polyeval(x2, d0, d1, C5) = d0 + x2*(d1 + x2*C5)
h0 = rnd(x2 * 0x1.5d870c4d84445p-40);
h0e = x2e * 0x1.5d870c4d84445p-40;
h1 = rnd(h0 + d1);
h1e = h0e + d1e;
h2 = rnd(x2 * h1);
h2e = x2e * h1e;
exp_eval = rnd(h2 + d0);
P = h2e + d0e;

{ x in [-0x1p-1, 0x1p-1] ->
  x2 - x2e in [-1b-56, 1b-56] /\
  d0 - d0e in [-513b-69, 513b-69] /\
  d1 - d1e in [-513b-85, 513b-85] /\
  h0 - h0e in [-15156152616109125b-148, 15156152616109125b-148] /\
  h1 - h1e in [-288512313657493937b-133, 288512313657493937b-133] /\
  h2 - h2e in [-560784715382587265b-135, 560784715382587265b-135] /\
  exp_eval - P in [-577027980704685549b-118, 577027980704685549b-118] }
