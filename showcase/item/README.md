# Showcase run configurations (CLion)

CLion picks up any `*.run.xml` file under the project tree. These live under `showcase/item/` grouped by feature.

## One-time setup

1. **CMake profile:** Settings → CMake → Profiles → add preset **CLion Clang Release + showcase** (`clion-clang-release-showcase`). Output directory: `out/clang-showcase`.
2. **Build:** Run **showcase / build** (or `./showcase/run build` from the repo root).
3. **Check:** Run **showcase / doctor** to confirm binaries were found.

## Run configurations

| Group | Name | What it runs |
|-------|------|----------------|
| setup | showcase / build | Configure + build showcase targets |
| setup | showcase / doctor | Print build dir and binary paths |
| suite | showcase / suite / all | Perf + accuracy + compile + summary |
| suite | showcase / suite / perf | Google Benchmark JSON for sqrt and sin |
| suite | showcase / suite / accuracy | MPFR campaigns for all paths |
| suite | showcase / suite / compile | Compile-time probe matrix |
| perf | showcase / perf / sqrt | Benchmark sqrt (all backends and paths) |
| perf | showcase / perf / sin | Benchmark sin |
| accuracy | showcase / accuracy / sqrt (all) | MPFR, all CCMath paths + GCEM |
| accuracy | showcase / accuracy / sin (all) | MPFR, all CCMath paths + GCEM |
| accuracy | showcase / accuracy / sqrt public | MPFR, ccmath public_default only |
| accuracy | showcase / accuracy / sqrt generic | MPFR, ccmath generic_gen only |
| accuracy | showcase / accuracy / sqrt rt | MPFR, ccmath runtime_rt only |
| accuracy | showcase / accuracy / sin public | MPFR, ccmath public_default only |
| accuracy | showcase / accuracy / sin generic | MPFR, ccmath generic_gen only |
| accuracy | showcase / accuracy / gcem sqrt | MPFR, GCEM sqrt only |
| accuracy | showcase / accuracy / gcem sin | MPFR, GCEM sin only |

Reports go to `showcase/reports/` (gitignored).

## Terminal shortcut

From the repo root:

```bash
./showcase/run build
./showcase/run all
./showcase/run help
```

Set `CCMATH_SHOWCASE_BUILD_DIR` to override the auto-detected build directory.
