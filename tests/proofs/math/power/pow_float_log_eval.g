@rnd = float<ieee_64, ne>;

# powf primary log2 polynomial evaluation (non-FMA path — conservative bound).
# Corresponds to powf_impl.hpp lines 360-368:
#   COEFFS = {C0, C1, C2, C3, C4, C5}
#   c0 = fma(dx, C1, C0)
#   c1 = fma(dx, C3, C2)
#   c2 = fma(dx, C5, C4)
#   p  = polyeval(dx2, c0, c1, c2) = c0 + dx2*(c1 + dx2*c2)
# Domain: dx in [-2^-8, 2^-7] (1-ULP float range-reduction output).
#
# Each assertion is the tight interval computed by Gappa on first run.

x2 = rnd(x * x);
x2e = x * x;

m0 = rnd(x * (-0x1.71547652b7a07p-1));
m0e = x * (-0x1.71547652b7a07p-1);
c0 = rnd(m0 + 0x1.71547652b82fep0);
c0e = m0e + 0x1.71547652b82fep0;

m1 = rnd(x * (-0x1.715479c2266c9p-2));
m1e = x * (-0x1.715479c2266c9p-2);
c1 = rnd(m1 + 0x1.ec709dc458db1p-2);
c1e = m1e + 0x1.ec709dc458db1p-2;

m2 = rnd(x * (-0x1.e7b2178870157p-3));
m2e = x * (-0x1.e7b2178870157p-3);
c2 = rnd(m2 + 0x1.2776ae1ddf8fp-2);
c2e = m2e + 0x1.2776ae1ddf8fp-2;

# Horner: polyeval(x2, c0, c1, c2) = c0 + x2*(c1 + x2*c2)
h0 = rnd(x2 * c2);
h0e = x2e * c2e;
h1 = rnd(h0 + c1);
h1e = h0e + c1e;
h2 = rnd(x2 * h1);
h2e = x2e * h1e;
log_eval = rnd(h2 + c0);
P = h2e + c0e;

{ x in [-0x1p-8, 0x1p-7] ->
  x2 - x2e in [-1b-68, 1b-68] /\
  c0 - c0e in [-257b-61, 257b-61] /\
  c1 - c1e in [-129b-62, 129b-62] /\
  c2 - c2e in [-257b-63, 257b-63] /\
  h0 - h0e in [-372227047963215627b-126, 372227047963215627b-126] /\
  h1 - h1e in [-578757989989174559b-113, 578757989989174559b-113] /\
  h2 - h2e in [-1145029739038678327b-127, 1145029739038678327b-127] /\
  log_eval - P in [-577604123977524393b-111, 577604123977524393b-111] }
