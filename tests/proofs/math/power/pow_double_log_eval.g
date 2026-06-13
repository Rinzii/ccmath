@rnd = float<ieee_64, ne>;

# pow double log2 polynomial evaluation (non-FMA path — conservative bound).
# Corresponds to pow_impl.hpp:
#   POW_LOG2_COEFFS = {C0, C1, C2, C3, C4, C5, C6}
#   c0 = fma(dx, C2, C1)
#   c1 = fma(dx, C4, C3)
#   c2 = fma(dx, C6, C5)
#   p  = polyeval(dx2, c0, c1, c2)
#   log2_x contribution = dx*C0 + dx2*p
# Domain: dx in [-2^-8, 2^-7] (double range-reduction output).
#
# Each assertion is the tight interval computed by Gappa on first run.

x2 = rnd(x * x);
x2e = x * x;

m0 = rnd(x * 0x1.ec709dc3b1fd5p-2);
m0e = x * 0x1.ec709dc3b1fd5p-2;
c0 = rnd(m0 + (-0x1.71547652b82e7p-1));
c0e = m0e + (-0x1.71547652b82e7p-1);

m1 = rnd(x * 0x1.2776bd90259d8p-2);
m1e = x * 0x1.2776bd90259d8p-2;
c1 = rnd(m1 + (-0x1.7154766124215p-2));
c1e = m1e + (-0x1.7154766124215p-2);

m2 = rnd(x * 0x1.9c4775eccf524p-3);
m2e = x * 0x1.9c4775eccf524p-3;
c2 = rnd(m2 + (-0x1.ec586c6f3d311p-3));
c2e = m2e + (-0x1.ec586c6f3d311p-3);

h0 = rnd(x2 * c2);
h0e = x2e * c2e;
h1 = rnd(h0 + c1);
h1e = h0e + c1e;
h2 = rnd(x2 * h1);
h2e = x2e * h1e;
h3 = rnd(h2 + c0);
h3e = h2e + c0e;
h4 = rnd(x2 * h3);
h4e = x2e * h3e;
log_eval = rnd(h4 + x * 0x1.71547652b82fep0);
P = h4e + x * 0x1.71547652b82fep0;

{ x in [-0x1p-8, 0x1p-7] ->
  x2 - x2e in [-1b-68, 1b-68] /\
  c0 - c0e in [-257b-62, 257b-62] /\
  c1 - c1e in [-129b-62, 129b-62] /\
  c2 - c2e in [-129b-63, 129b-63] /\
  h0 - h0e in [-13387265408061443b-122, 13387265408061443b-122] /\
  h1 - h1e in [-578738699119858857b-113, 578738699119858857b-113] /\
  h2 - h2e in [-1075541560839878441b-127, 1075541560839878441b-127] /\
  h3 - h3e in [-36101217195867233b-108, 36101217195867233b-108] /\
  h4 - h4e in [-537155963485289135b-125, 537155963485289135b-125] /\
  log_eval - P in [-584853814232881131b-119, 584853814232881131b-119] }
