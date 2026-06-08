# Approximating Functions: A Practical Guide to Sollya and Function Implementation

This guide teaches you how to add a new elementary-function implementation to CCMath: how to take a
function that has no closed-form solution, design an algorithm that computes it accurately, generate
the polynomial it needs with [Sollya](https://www.sollya.org/), and validate the result. It stays at
the level of *theory and method*, the reasoning you apply to any function, and keeps the discussion
of CCMath's own internals light; the goal is to make the task of building correct functions less daunting.

Read it once end to end, then use it as a checklist (§7) the next time you pick up a function.

Inline citations such as `[Muller §3.6]` and `[Handbook §10.3]` point to the two reference works
listed under [References](#9-references-and-further-reading); consult them for full derivations and
proofs.

---

## 1. The core problem: functions with no clean solution

A handful of operations are *correctly rounded* on a computer: addition, subtraction, multiplication,
division, square root, and the fused multiply-add are all required by IEEE-754 to behave as if the
exact result were computed to infinite precision and then rounded once to the destination format
([IEEE Std 754-2019](https://doi.org/10.1109/IEEESTD.2019.8766229), §5.1 and the operation list in
§5.4.1; the fused multiply-add, which rounds the whole `x*y + z` only once, joined the standard in
its 2008 revision). Almost everything else cannot.

`exp`, `log`, `sin`, `cos`, `atan`, `cbrt`, `pow`, `tgamma`, and the rest are *transcendental* or
*algebraic-but-irrational* functions. There is no finite formula in terms of the basic operations
that yields their value. You cannot "just compute" `sin(x)`; you can only compute something that is
provably close to it, then round that to the target format.

So implementing such a function is never a search for *the* solution. It is an engineering process
with four recurring stages [Muller, Introduction and §2.1; Handbook §10.1.2, "the various steps of
function evaluation"]:

1. **Argument reduction.** Use mathematical identities to map the (possibly huge) input into a
   small, well-behaved interval where the function is easy to approximate.
2. **Approximation.** On that small interval, replace the true function by a polynomial (or
   rational function) whose error is small and *bounded by a proof*, not just by spot checks.
3. **Reconstruction.** Combine the approximation with the information thrown away during reduction
   to recover the full-range result.
4. **Rounding.** Deliver the result correctly (or faithfully) rounded in the target format, in every
   rounding mode you support.

Stages 1, 3, and 4 are function-specific algebra and careful floating-point bookkeeping. Stage 2 is
where Sollya is used, and where most of the approximation theory lives. The rest of this guide covers
the theory, then the tooling.

---

## 2. Why approximation is a theory, not a guess

### 2.1 Taylor series are the wrong default

The first instinct is usually a Taylor series. It is the wrong default for a finished
implementation. A Taylor expansion is optimal *at a single point* and concentrates all of its
accuracy there; the error grows rapidly toward the ends of the interval. For a fixed polynomial
degree you pay for that imbalance with extra terms [Muller §3.1, and Figures 3.1 to 3.2, which
contrast interpolation and minimax approximations of `exp`].

Taylor and related series are still valuable in two places: as a *starting point* for deriving an
approximation, and inside multiple-precision evaluation where you genuinely add many terms [Muller
§7.4 on power-series evaluation and binary splitting]. For a fixed-degree polynomial in a fixed
format, you want the *minimax* polynomial instead.

### 2.2 Minimax and the equioscillation theorem

The right objective is to minimize the *worst-case* error over the whole interval, not the error at
one point. The polynomial of degree `n` that minimizes the maximum (supremum norm) of the error is
called the **minimax** polynomial.

Chebyshev's equioscillation theorem characterizes it: the best degree-`n` approximation is the
unique polynomial whose error curve reaches its maximum magnitude at least `n + 2` times, with
alternating sign [Muller §3.3 and §3.4]. Intuitively, the optimal polynomial spreads the error out
evenly and "wastes" nothing. You can often recognize a good approximation just by plotting the error
and checking that it oscillates with roughly equal peaks.

A closely related, easy-to-compute shortcut is **Chebyshev interpolation**: interpolating the
function at the Chebyshev nodes (rather than at evenly spaced points) gives a polynomial that is
already very close to minimax and avoids the wild end-of-interval oscillation that plagues
equispaced interpolation [Muller §3.2.2]. It is a fine way to get a first candidate. The contrast
between the least-squares (`L2`) and the minimax (`L-infinity`) objectives is developed in
[Muller Ch 3] and [Handbook §10.3.1 to §10.3.2].

### 2.3 The Remez algorithm

The minimax polynomial is found numerically by the **Remez exchange algorithm**, an iterative method
that repeatedly locates the error extrema, solves a small linear system to equalize them, and
converges to the equioscillating solution [Muller §3.6]. You will rarely run Remez by hand; Sollya's
`remez` command does it for you. What matters is understanding what it returns and why it is better
than a truncated series.

### 2.4 Absolute versus relative error

You almost always care about *relative* error, because floating-point accuracy is measured in ULPs
(units in the last place), which scale with the magnitude of the result. Minimizing absolute error
`|p(x) - f(x)|` is not the same as minimizing relative error `|p(x)/f(x) - 1|`. The two diverge
badly when `f` ranges over several orders of magnitude on the interval, or when `f` has a zero
inside it [Muller §3.7]. When you drive the tools below, decide up front which error you are
minimizing, and weight the approximation accordingly.

### 2.5 The coefficients must be machine numbers

Remez returns a minimax polynomial with *real* coefficients. The moment you round those coefficients
to `double` (or `float`, or a double-double pair), the polynomial is no longer minimax, and the
equioscillation property is usually lost. The error can grow by a large factor [Muller Ch 4, and
Figures 4.1 to 4.3, which show `arctan` losing its equioscillation after naive coefficient
rounding].

The fix is to search directly for the best polynomial *whose coefficients are already representable
in the target formats*. This is a constrained approximation problem solved with lattice-reduction
techniques [Brisebarre and Chevillard; Muller §4.1.3, and the Euclidean-lattice background in
Handbook Appendix A.2]. You do not implement this yourself: it is exactly what Sollya's `fpminimax`
does [Muller §4.2; Handbook §10.3.3 to §10.3.4]. This is the single most important reason to use
Sollya rather than rounding a Remez result by hand.

---

## 3. Sollya: the practical tool

[Sollya](https://www.sollya.org/) is an interactive environment for safe floating-point code
development. For function implementation its key abilities are: computing minimax polynomials with
real or constrained coefficients, computing *certified* error bounds, and helping you choose the
polynomial degree. Computing constrained approximations together with certified error bounds in
Sollya is a standard, well-documented workflow [Muller §4.2], [Handbook §10.3.4]; the rest of this
section is how you drive it.

### 3.1 Installing and running

Sollya is available from your package manager or from <https://www.sollya.org/>. On macOS,
`brew install sollya` typically works; on Linux it is packaged as `sollya`. Start the interactive
prompt by running `sollya`, or run a script with `sollya script.sollya`. Statements end with a
semicolon. Appending `!` to a setting assignment suppresses its echo.

### 3.2 The commands you will actually use

| Command | What it does |
|---|---|
| `remez(f, n, [a;b], w)` | Minimax polynomial of degree `n` (real coefficients) for `f` over `[a;b]`, optionally with weight `w` (use a weight to target relative error). |
| `fpminimax(f, n, formats, [a;b], ...)` | Near-minimax polynomial whose coefficients are *rounded into the given formats*. The workhorse for production coefficients. |
| `guessdegree(f, [a;b], eps, w)` | The polynomial degree needed to reach error `eps` over `[a;b]`. Use it before `fpminimax`. |
| `dirtyinfnorm(g, [a;b])` | Fast, non-certified supremum norm of `g` over the interval. Good for quick error checks. |
| `infnorm(g, [a;b])` | Interval-arithmetic *enclosure* of the supremum norm (safe upper bound). |
| `supnorm(p, f, [a;b], errtype, acc)` | Certified relative or absolute error between polynomial `p` and `f` [Chevillard-Joldes-Lauter; Muller §3.9]. |
| `taylor(f, n, x0)` | Taylor polynomial, handy as a starting candidate. |
| `roundcoefficients(p, formats)` | Round an existing polynomial's coefficients to formats. |
| `horner(p)`, `canonical(p)` | Display the polynomial in Horner or expanded form. |
| `implementpoly(...)` | Emit C code for evaluating a polynomial together with a Gappa-checkable error bound. |
| `prec = 165!;` | Set Sollya's internal working precision (raise it well above your target format). |
| `display = hexadecimal;` | Print coefficients as exact hex floats so you can paste them without re-rounding. |

`D` denotes the `double` format, `SG` single, `DD` double-double; a bare integer like `24` means
"that many bits". A format list such as `[|D, D, D...|]` assigns formats per coefficient (the `...`
repeats the last one).

### 3.3 A worked session: approximating `exp` on a reduced interval

Suppose argument reduction has already mapped the problem to evaluating `exp(r)` for
`r` in a small interval around zero (a real implementation reduces `exp(x)` to `2^k * exp(r)` with
`r` in roughly `[-ln(2)/2, ln(2)/2]`; see §4). Here is how you would build and check a `double`
precision polynomial for that step:

```text
prec = 200!;                         // work far above double precision
display = hexadecimal!;              // exact, paste-ready coefficients
f = exp(x);
I = [-1/2 * log(2); 1/2 * log(2)];   // the reduced interval

// 1. How many terms do we need for ~ double-precision relative accuracy?
guessdegree(f, I, 1b-53);            // 1b-53 means 2^-53

// 2. Best polynomial with double coefficients, minimizing RELATIVE error.
p = fpminimax(f, 6, [|D...|], I, relative);

// 3. Measure the achieved relative error, quickly then safely.
eps_fast = dirtyinfnorm(p/f - 1, I);
print("approx relative error:", eps_fast, "=", log2(eps_fast), "bits");

// 4. A certified bound you could cite in a proof:
supnorm(p, f, I, relative, 1b-40);
```

Read the result as a number of correct bits: `log2(error)` near `-53` means about double precision.
If `guessdegree` says you need degree 8 but you asked for 6, the error will be visibly worse, and you
raise the degree. If degree 6 already reaches `2^-60`, you may be able to drop a term.

### 3.4 Tricks that matter

- **Exploit symmetry.** If `f` is odd (like `sin`), approximate with odd monomials only, and if it
  is even (like `cos`), use even ones. In Sollya, pass an explicit monomial list instead of a degree:
  `fpminimax(sin(x), [|1,3,5,7|], [|D...|], [-pi/4; pi/4], relative)`. This halves the term count and
  preserves the exact symmetry of the true function.
- **Pin the leading term.** For functions like `sin(x) ~ x + ...` you often want the linear
  coefficient to be exactly `1` so that tiny arguments are returned untouched. Sollya lets you fix
  coefficients (for example by supplying a constrained part), which both improves small-argument
  behavior and saves a coefficient.
- **Factor out the singular behavior.** To approximate something like `sin(x)/x` or
  `(exp(x) - 1)/x` near zero, approximate the smooth quotient and multiply back afterwards. This
  keeps relative error well behaved where the naive form would divide small by small.
- **Choose the format per coefficient.** The high-order coefficients contribute least to the result,
  so they can often be `float` or low-bit while the low-order ones are `double` or double-double.
  `fpminimax` honors a per-coefficient format list, which shrinks tables and speeds evaluation.
- **Always raise `prec`.** Sollya's internal precision must be well above your target format or the
  "certified" bounds are meaningless. Set it once at the top of the script.

---

## 4. Argument reduction (high level)

Approximation only works on a small interval, so the first algorithmic decision for most functions is
how to get there. Reduction is function-specific algebra [Muller Ch 11; Handbook §10.2]:

- **Exponentials and logs** use the binary exponent: `exp(x) = 2^k * exp(r)` where `k = round(x /
  ln 2)` and `r = x - k * ln 2` is small; `log(m * 2^e) = e * ln 2 + log(m)` with `m` in `[1, 2)`.
- **Trigonometric functions** reduce modulo `pi/2` (or `pi/4`), then use quadrant identities to
  reconstruct. Reducing large arguments accurately is itself hard, because `pi` is irrational and the
  subtraction `x - k * pi/2` cancels catastrophically.

Two classical methods cover most cases. **Cody-Waite** reduction stores the reduction constant
(`ln 2`, `pi/2`) as a sum of several machine numbers so that `x - k*C` is computed with extra
precision, and is excellent when a fused multiply-add is available [Muller §11.2]. **Payne-Hanek**
reduction handles arbitrarily large arguments correctly by using many bits of the constant [Muller
§11.4]. The *worst cases* for reduction, the inputs nearest a multiple of the constant, can be found
with continued fractions [Muller §11.3]; knowing them tells you how much extra precision the
reduction step must carry.

The practical rule: budget your error. Decide how many bits the reduced argument `r` must be correct
to, and make the reduction carry enough extra precision (often a double-double `r`) that its error is
negligible compared to the polynomial's error. Bounding the relative error contributed by the
reduction step is treated in detail in [Handbook §10.2.2].

---

## 5. Evaluating the polynomial

Once you have coefficients, you must evaluate the polynomial in floating point, and that evaluation
has its own rounding error.

- **Horner's scheme** is the default: it is compact, numerically good, and maps well to the fused
  multiply-add [Muller §5.1.1; Handbook §10.4.1]. Each step is one FMA, which both speeds evaluation
  and halves the rounding error per term.
- **Estrin's scheme** exposes parallelism by splitting the polynomial into independent sub-sums,
  which can be faster on a wide, pipelined processor at the cost of a few extra operations [Muller
  §5.2.2].
- For the low-order terms, where accuracy matters most, you may carry a **double-word** (double-double)
  accumulator so that the evaluation error stays well below one ULP. The building blocks are the
  *error-free transforms*: `2Sum`/`Fast2Sum` for addition and `2MultFMA` (or Veltkamp-Dekker without
  an FMA) for multiplication, and the compensated evaluation schemes built on them [Muller §2.2.1 to
  §2.2.2; Handbook §4.3 to §4.4 and §5.5; double-word and triple-word arithmetic in Handbook §14.1].

The key discipline is an **error budget**. The final error is the sum of the approximation error
(from §2 to §3), the argument-reduction error (§4), and the evaluation error here. Each must be
bounded *and proven* to combine below your accuracy target. Tools like Gappa [Muller §2.2.4;
Handbook §13.3] and Sollya's `implementpoly` exist specifically to produce machine-checkable bounds
for the evaluation step. Bounding evaluation error is treated at length in Muller §5.3 and Handbook
§10.4.2.

The floating-point code you write is not necessarily the code that runs, and the defaults differ
across the compilers CCMath supports. A compiler may contract `a*b + c` into a single fused
multiply-add, reassociate sums under fast-math options, or assume the default rounding environment:

- GCC contracts by default
  ([`-ffp-contract=fast`](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)).
- Clang and Apple Clang contract within an expression by default
  ([`-ffp-contract=on`](https://clang.llvm.org/docs/UsersManual.html#controlling-floating-point-behavior)).
- MSVC does not contract by default since Visual Studio 2022, but does under
  [`/fp:contract`](https://learn.microsoft.com/en-us/cpp/build/reference/fp-specify-floating-point-behavior).

The `#pragma STDC FENV_ACCESS` control that is supposed to make a function respect the dynamic
rounding mode and status flags is also unevenly supported:
[GCC does not implement the pragma](https://gcc.gnu.org/wiki/FloatingPointMath) (access keys off
`-frounding-math`),
[Clang disables it by default](https://clang.llvm.org/docs/UsersManual.html#controlling-floating-point-behavior)
and honors the environment under `-ffp-model=strict`, and
[MSVC uses its own `fenv_access` pragma](https://learn.microsoft.com/en-us/cpp/preprocessor/fenv-access)
or `/fp:strict`. Enabling strict access also disables contraction on each of these compilers.

An error proof that assumes a specific sequence of rounded operations is only valid if the compiler
preserves that sequence. Pin the operations your bound depends on, and select the appropriate strict
or precise mode for each target compiler [Handbook §6.2.3 to §6.2.5].

---

## 6. Correct rounding and the Table Maker's Dilemma

The last stage is delivering a correctly rounded result. This is harder than it looks because of the
**Table Maker's Dilemma** [Muller Ch 12; Handbook §10.5]: to round `f(x)` correctly you must know its
value to enough precision to decide which side of a rounding boundary it falls on, but some inputs
land extraordinarily close to a boundary (a "hard-to-round" case) and require far more intermediate
precision than the target format.

You do not need to solve this from scratch. The standard strategies are:

- **Ziv's rounding test** [Muller §12.4; Handbook §10.6.1]: evaluate the function to a slightly higher
  "working" precision, and check whether the result is far enough from a rounding boundary that
  rounding the approximation equals rounding the true value. If not, recompute with more precision. In
  practice a first cheap step succeeds on the vast majority of inputs, and a slower, more accurate
  second step handles the rest. This two-step structure is the heart of the CRlibm design [Handbook
  §10.6, "Some Implementation Tricks Used in the CRlibm Library"].
- **Round-to-odd and sticky-bit techniques** let you compute a wider intermediate result and round it
  once to the target format without double rounding. These are the same ideas used to make a fused
  operation correctly rounded in every mode.
- **Precomputed worst cases.** For some functions and formats, exhaustive or near-exhaustive searches
  [the L-algorithm and SLZ algorithm; Muller §12.8; Handbook §10.5.3 on finding the hardest-to-round
  points] have found the hardest-to-round inputs. Knowing the worst case tells you the maximum
  intermediate precision you will ever need, which turns "correct rounding" from an open-ended problem
  into a bounded one. Sometimes the structure of the function means there is no TMD to solve at all
  [Handbook §10.5.1].

Two clarifying points. First, *faithful* rounding (returning one of the two floats bracketing the
exact result) is much easier than *correct* rounding (always returning the nearer one) and is a
reasonable intermediate target. Second, correct rounding must hold in *all* rounding modes you
support, not just round-to-nearest; the directed modes change where the boundaries are and must be
validated separately.

Exact and special cases deserve their own attention: signed zeros, infinities, NaNs, and inputs where
the mathematically exact result is representable (so no rounding error is allowed at all) are handled
before the approximation path and are governed by the IEEE-754 special-case rules [Muller §13.1;
Handbook §2.5 on exceptions and §3.1.7 on special values].

---

## 7. A method for adding a new function

Putting the stages together, here is a high-level recipe. It is intentionally tool-and-theory
oriented; the place each piece lives in the codebase is a separate, smaller question.

1. **Specify.** Write down the function, its domain, the special cases (signed zero, infinities,
   NaN, exact results), the target formats (`float`, `double`), and the accuracy goal (correctly
   rounded, or a stated ULP bound) for every rounding mode.
2. **Reduce.** Choose the argument-reduction identity and the precision it must carry. Identify the
   worst cases that stress it (§4).
3. **Pick the kernel function and interval.** Decide exactly what smooth function you will
   approximate on what interval after reduction (often a normalized or singularity-removed form, per
   the tricks in §3.4).
4. **Size the polynomial.** Use `guessdegree` to find the degree, and decide absolute versus relative
   error (§2.4).
5. **Generate coefficients.** Use `fpminimax` with the target formats to get machine-representable
   coefficients, and confirm the error with `dirtyinfnorm` then a certified `supnorm` (§3).
6. **Choose evaluation.** Horner with FMA by default; double-word for the sensitive terms; Estrin if
   you need the parallelism (§5).
7. **Reconstruct and round.** Recombine the reduced pieces, handle special cases, and apply a
   correct-rounding strategy across all modes (§6).
8. **Budget and prove.** Sum the approximation, reduction, and evaluation errors; show the total is
   below target. Gappa and Sollya's `implementpoly` help make this rigorous.
9. **Validate.** Test against a higher-precision oracle [GNU MPFR; Handbook §14.4.4] across random,
   structured, boundary, and adversarial inputs, in every rounding mode, and measure the ULP error.
   The repository already has MPFR-oracle and ULP test harnesses for exactly this; reuse them rather
   than rolling your own.

Steps 1 to 8 are the design; step 9 is non-negotiable evidence. A function is not "done" because it
looks right on a few inputs; it is done when its error is bounded by argument and validated against an
independent oracle.

---

## 8. Where the pieces live in CCMath (orientation only)

This guide is about method, not internals, but a few pointers help you connect the two. CCMath keeps
a single correctly-rounded generic kernel per function and dispatches to it for both runtime and
constant-evaluation use. The artifacts you produce with the workflow above, the reduction constants
and the Sollya-generated polynomial coefficients, are what populate those kernels. When you add a
function, you are mostly supplying stage-1 to stage-4 ingredients; the surrounding dispatch,
exception handling, and test scaffolding already exist. Per-function status and rounding-mode
coverage are tracked in [STATUS.md](STATUS.md) and [ROUNDING_STATUS.md](ROUNDING_STATUS.md).

---

## 9. References and further reading

Primary references, cited throughout:

- **[Muller]** Jean-Michel Muller, *Elementary Functions: Algorithms and Implementation*, 3rd ed.,
  Birkhäuser, 2016. ISBN 978-1-4899-7981-0, DOI 10.1007/978-1-4899-7983-4. Especially:
  computer-arithmetic and error-free transforms (Ch 2), classical approximation theory and Remez
  (Ch 3), constrained approximation and Sollya (Ch 4), polynomial evaluation and error bounds (Ch 5),
  table-based methods (Ch 6), multiple-precision evaluation (Ch 7), range reduction (Ch 11), final
  rounding and the Table Maker's Dilemma (Ch 12), exceptions and special cases (Ch 13), and worked
  library examples (Ch 14).

- **[Handbook]** Jean-Michel Muller, Nicolas Brunie, Florent de Dinechin, Claude-Pierre Jeannerod,
  Mioara Joldes, Vincent Lefèvre, Guillaume Melquiond, Nathalie Revol, and Serge Torres, *Handbook of
  Floating-Point Arithmetic*, 2nd ed., Birkhäuser, 2018. ISBN 978-3-319-76525-9,
  DOI 10.1007/978-3-319-76526-6. Especially: definitions, ULPs and rounding (Ch 2), the IEEE-754
  standard and hardware (Ch 3), error-free transforms and FMA-based building blocks (Ch 4), enhanced
  and compensated sums and polynomial evaluation (Ch 5), the language and compiler hazards that can
  invalidate an error proof (Ch 6), evaluating elementary functions end to end (Ch 10: range reduction
  §10.2, polynomial approximation and Sollya §10.3, evaluation §10.4, the Table Maker's Dilemma §10.5,
  CRlibm tricks §10.6), formal verification and Gappa (Ch 13),
  double-word and extended-precision arithmetic and MPFR (Ch 14), and number-theory tools, continued
  fractions and Euclidean lattices (Appendix A).

Tools:

- Sollya, polynomial approximation and certified error bounds: <https://www.sollya.org/>.
- Gappa, machine-checkable proofs of floating-point error bounds: <https://gappa.gitlabpages.inria.fr/>.
- GNU MPFR, correctly-rounded multiple-precision reference used as a validation oracle:
  <https://www.mpfr.org/>.

Reference implementations worth studying:

- CR-LIBM, a correctly-rounded `libm` and the origin of much of the Ziv-test methodology [Muller
  §14.4].
- CORE-MATH, modern correctly-rounded elementary functions with proofs: <https://core-math.org/>.
- The RLIBM project, polynomial generation that targets the correctly-rounded result directly.
