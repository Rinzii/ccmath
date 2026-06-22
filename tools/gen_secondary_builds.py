#!/usr/bin/env python3
"""Generate Meson and Premake secondary build files from ccmath-build-manifest.json."""

from __future__ import annotations

import argparse
import json
import os
import re
import sys
from pathlib import Path
from typing import Any


GENERATED_BEGIN_MESON = "# BEGIN GENERATED LIBRARY MANIFEST"
GENERATED_END_MESON = "# END GENERATED LIBRARY MANIFEST"
GENERATED_BEGIN_PREMAKE = "-- BEGIN GENERATED LIBRARY MANIFEST"
GENERATED_END_PREMAKE = "-- END GENERATED LIBRARY MANIFEST"
ROOT_SECONDARY_INCLUDE = Path("out") / "secondary" / "include"

BIT_CAST_PROBE = """#include <type_traits>
int main() {
  struct A { int x; };
  struct B { int y; };
  B b = __builtin_bit_cast(B, A{42});
  return b.y;
}
"""

SIMD_FMA_PROBE = """#include <immintrin.h>
int main() {
  __m128 fma_test = _mm_fmadd_ps(_mm_set1_ps(1.0f), _mm_set1_ps(2.0f), _mm_set1_ps(3.0f));
  return static_cast<int>(_mm_cvtss_f32(fma_test));
}
"""

SIMD_SVML_PROBE = """#include <immintrin.h>
int main() {
  __m128d value = _mm_sin_pd(_mm_set1_pd(1.0));
  return static_cast<int>(_mm_cvtsd_f64(value));
}
"""


def load_manifest(path: Path) -> dict[str, Any]:
    with path.open(encoding="utf-8") as handle:
        return json.load(handle)


def relative_posix_path(path: Path, base_dir: Path) -> str:
    return os.path.relpath(path.resolve(), base_dir.resolve()).replace("\\", "/")


def version_components(manifest: dict[str, Any]) -> tuple[str, str, str]:
    version = manifest["version"]
    parts = version.split(".")
    if len(parts) != 3:
        raise ValueError(f"expected semantic version 'major.minor.patch', got {version!r}")
    return parts[0], parts[1], parts[2]


def render_version_header(manifest: dict[str, Any], source_dir: Path) -> str:
    template_path = source_dir / manifest["version_template"]
    major, minor, patch = version_components(manifest)
    template = template_path.read_text(encoding="utf-8")
    return (
        template.replace("@CCMATH_VERSION_MAJOR@", major)
        .replace("@CCMATH_VERSION_MINOR@", minor)
        .replace("@CCMATH_VERSION_PATCH@", patch)
    )


def write_version_header(include_dir: Path, header: str) -> None:
    version_path = include_dir / "ccmath" / "version.hpp"
    version_path.parent.mkdir(parents=True, exist_ok=True)
    version_path.write_text(header, encoding="utf-8")


def _deterministic_meson_option(options: list[dict[str, Any]]) -> str | None:
    option = next((opt for opt in options if opt["key"] == "deterministic"), None)
    return option["meson_option"] if option is not None else None


def meson_define_lines(options: list[dict[str, Any]]) -> list[str]:
    deterministic = _deterministic_meson_option(options)
    lines: list[str] = []
    for option in options:
        meson_name = option["meson_option"]
        define = option["define"]
        # Deterministic mode routes every call through the generic kernels, so the runtime SIMD
        # path (a separate, non-generic evaluation) must stay off even if the user left it enabled.
        condition = f"get_option('{meson_name}')"
        if deterministic is not None and option["key"] == "runtime_simd":
            condition += f" and not get_option('{deterministic}')"
        lines.extend(
            [
                f"if {condition}",
                f"  _ccmath_defines += '-D{define}'",
                "endif",
            ]
        )
    if deterministic is not None:
        # Stop the compiler from contracting stray a*b+c into a hardware FMA, which would diverge
        # between FMA and non-FMA targets. Mirrors the CMake INTERFACE flag.
        lines.extend(
            [
                f"if get_option('{deterministic}')",
                "  _ccmath_defines += '-ffp-contract=off'",
                "endif",
            ]
        )
    return lines


def meson_runtime_probe_lines(options: list[dict[str, Any]]) -> list[str]:
    runtime_option = next((option for option in options if option["key"] == "runtime_simd"), None)
    if runtime_option is None:
        return []

    meson_name = runtime_option["meson_option"]
    deterministic = _deterministic_meson_option(options)
    guard = f"get_option('{meson_name}')"
    if deterministic is not None:
        guard += f" and not get_option('{deterministic}')"
    return [
        "",
        "_ccmath_simd_fma_probe = '''",
        SIMD_FMA_PROBE.rstrip(),
        "'''",
        "",
        f"if {guard} and cpp.compiles(_ccmath_simd_fma_probe, name : 'ccmath runtime SIMD FMA intrinsics')",
        "  _ccmath_defines += '-DCCM_CONFIG_RT_SIMD_HAS_FMA'",
        "endif",
        "",
        "_ccmath_simd_svml_probe = '''",
        SIMD_SVML_PROBE.rstrip(),
        "'''",
        "",
        f"if {guard} and cpp.compiles(_ccmath_simd_svml_probe, name : 'ccmath runtime SIMD SVML intrinsics')",
        "  _ccmath_defines += '-DCCM_CONFIG_RT_SIMD_HAS_SVML'",
        "endif",
    ]


def meson_dependency_block(source_include: str, generated_include: str, options: list[dict[str, Any]]) -> str:
    lines = [
        "cpp = meson.get_compiler('cpp')",
        "",
        "_ccmath_builtin_bit_cast_probe = '''",
        BIT_CAST_PROBE.rstrip(),
        "'''",
        "",
        "if not cpp.compiles(_ccmath_builtin_bit_cast_probe, name : 'ccmath builtin bit_cast support')",
        "  error('CCMath requires compiler support for __builtin_bit_cast')",
        "endif",
        "",
        "_ccmath_defines = []",
    ]
    lines.extend(meson_define_lines(options))
    lines.extend(meson_runtime_probe_lines(options))
    lines.extend(
        [
            "",
            "ccmath_inc = include_directories(",
            f"  '{source_include}',",
            f"  '{generated_include}',",
            ")",
            "",
            "ccmath_dep = declare_dependency(",
            "  include_directories: ccmath_inc,",
            "  compile_args: _ccmath_defines,",
            ")",
            "",
            "meson.override_dependency('ccmath', ccmath_dep)",
            "",
        ]
    )
    return "\n".join(lines)


def meson_options_txt(options: list[dict[str, Any]]) -> str:
    lines = [
        "# Generated by tools/gen_secondary_builds.py from ccmath-build-manifest.json.",
        "# CMake is the primary build system.",
        "",
    ]
    for option in options:
        default = "true" if option["meson_default"] else "false"
        description = option["description"].replace("'", "\\'")
        lines.append(
            f"option('{option['meson_option']}', type: 'boolean', value: {default}, "
            f"description: '{description}')"
        )
    lines.append("")
    return "\n".join(lines)


def meson_build_content(manifest: dict[str, Any], source_dir: Path, base_dir: Path) -> str:
    cpp_standard = manifest["cpp_standard"]
    body = meson_dependency_block(
        source_include=relative_posix_path(source_dir / "include", base_dir),
        generated_include=relative_posix_path(base_dir / "include", base_dir),
        options=manifest["options"],
    )
    return "\n".join(
        [
            "# Generated by tools/gen_secondary_builds.py from ccmath-build-manifest.json.",
            "# CMake is the primary build system.",
            "#",
            "# This entry point exposes CCMath as a Meson dependency only.",
            "# It does not define examples, tests, or smoke executables.",
            "",
            f"project('{manifest['name']}', 'cpp',",
            f"  version: '{manifest['version']}',",
            "  meson_version: '>=1.0.0',",
            f"  default_options: ['cpp_std=c++{cpp_standard}'],",
            ")",
            "",
            body.rstrip(),
            "",
        ]
    )


def premake_trigger_name(meson_option: str) -> str:
    return "ccmath-" + meson_option.replace("_", "-")


def premake_options(options: list[dict[str, Any]]) -> str:
    lines: list[str] = []
    for option in options:
        default = "true" if option["meson_default"] else "false"
        description = option["description"].replace('"', '\\"')
        trigger = premake_trigger_name(option["meson_option"])
        lines.extend(
            [
                "newoption {",
                f'    trigger = "{trigger}",',
                f'    description = "{description}",',
                '    allowed = { { "true", "Enable" }, { "false", "Disable" } },',
                f'    default = "{default}",',
                "}",
                "",
            ]
        )
    return "\n".join(lines)


def premake_defines_block(options: list[dict[str, Any]]) -> str:
    deterministic = next((opt for opt in options if opt["key"] == "deterministic"), None)
    deterministic_trigger = premake_trigger_name(deterministic["meson_option"]) if deterministic is not None else None
    lines = [
        "function ccmath.defines()",
        "    local defs = {}",
        "",
    ]
    for option in options:
        trigger = premake_trigger_name(option["meson_option"])
        default = "true" if option["meson_default"] else "false"
        define = option["define"]
        # Deterministic mode forces the runtime SIMD path off. The generic kernels are the only
        # cross-hardware-stable evaluation.
        condition = f'_ccmath_option_enabled("{trigger}", {default})'
        if deterministic_trigger is not None and option["key"] == "runtime_simd":
            condition += f' and not _ccmath_option_enabled("{deterministic_trigger}", false)'
        lines.extend(
            [
                f"    if {condition} then",
                f'        table.insert(defs, "{define}")',
                "    end",
                "",
            ]
        )
    lines.extend(
        [
            "    return defs",
            "end",
            "",
        ]
    )
    return "\n".join(lines)


def premake_buildoptions_block(options: list[dict[str, Any]]) -> str:
    deterministic = next((opt for opt in options if opt["key"] == "deterministic"), None)
    lines = [
        "function ccmath.buildoptions()",
        "    local opts = {}",
        "",
    ]
    if deterministic is not None:
        trigger = premake_trigger_name(deterministic["meson_option"])
        # Stop the compiler from contracting stray a*b+c into a hardware FMA, which would diverge
        # between FMA and non-FMA targets. Mirrors the CMake INTERFACE flag (skipped on MSVC).
        lines.extend(
            [
                f'    if _ccmath_option_enabled("{trigger}", false) and not _ccmath_is_msvc() then',
                '        table.insert(opts, "-ffp-contract=off")',
                "    end",
                "",
            ]
        )
    lines.extend(
        [
            "    return opts",
            "end",
            "",
        ]
    )
    return "\n".join(lines)


def premake_version_writer(version_header: str) -> list[str]:
    return [
        "-- Generate ccmath/version.hpp here so Premake consumers never depend on a CMake configure.",
        "local _ccmath_version_header = [==[",
        version_header.rstrip("\n"),
        "]==]",
        "",
        "local function _ccmath_write_version_header()",
        '    local generated = path.join(_ccmath_root, "out/secondary/include/ccmath")',
        "    os.mkdir(generated)",
        '    local handle = io.open(path.join(generated, "version.hpp"), "w")',
        "    handle:write(_ccmath_version_header)",
        "    handle:close()",
        "end",
        "",
        "_ccmath_write_version_header()",
    ]


def premake_content(
    manifest: dict[str, Any],
    source_dir: Path,
    generated_include_dir: Path,
    base_dir: Path,
    version_header: str | None = None,
) -> str:
    source_include = relative_posix_path(source_dir / "include", base_dir)
    generated_include = relative_posix_path(generated_include_dir, base_dir)

    parts = [
        "-- Generated by tools/gen_secondary_builds.py from ccmath-build-manifest.json.",
        "-- CMake is the primary build system, but this module is self-contained.",
        "--",
        "-- Consumer usage:",
        '--   include("path/to/ccmath/premake5.lua")',
        '--   project "my-app"',
        '--       kind "ConsoleApp"',
        '--       language "C++"',
        '--       files { "main.cpp" }',
        "--       ccmath.use()",
        "",
        premake_options(manifest["options"]).rstrip(),
        "",
        "ccmath = ccmath or {}",
        "",
    ]

    if version_header is not None:
        parts.extend(
            [
                "-- Resolve paths against ccmath's own location, not the consumer's.",
                "local _ccmath_root = _SCRIPT_DIR",
                "",
            ]
        )
        parts.extend(premake_version_writer(version_header))
        parts.append("")

    parts.extend(
        [
            f'local _ccmath_include_dirs = {{ "{source_include}", "{generated_include}" }}',
            "",
            "local function _ccmath_option_enabled(trigger, default_value)",
            "    local value = _OPTIONS[trigger]",
            "    if value == nil then",
            "        return default_value",
            "    end",
            '    return value == "true"',
            "end",
            "",
            "local function _ccmath_is_msvc()",
            '    return string.match(_ACTION or "", "vs") ~= nil',
            "end",
            "",
        ]
    )

    if version_header is not None:
        parts.extend(
            [
                "function ccmath.include_dirs()",
                "    local resolved = {}",
                "    for _, dir in ipairs(_ccmath_include_dirs) do",
                "        table.insert(resolved, path.getabsolute(path.join(_ccmath_root, dir)))",
                "    end",
                "    return resolved",
                "end",
                "",
            ]
        )
    else:
        parts.extend(
            [
                "function ccmath.include_dirs()",
                "    return _ccmath_include_dirs",
                "end",
                "",
            ]
        )

    parts.extend(
        [
            premake_defines_block(manifest["options"]).rstrip(),
            "",
            premake_buildoptions_block(manifest["options"]).rstrip(),
            "",
            "function ccmath.use()",
            "    includedirs(ccmath.include_dirs())",
            "    defines(ccmath.defines())",
            "    buildoptions(ccmath.buildoptions())",
            "end",
            "",
        ]
    )
    return "\n".join(parts)


def root_meson_wrapper(manifest: dict[str, Any]) -> str:
    generated = "\n".join(
        [
            f"project('{manifest['name']}', 'cpp',",
            f"  version: '{manifest['version']}',",
            "  meson_version: '>=1.0.0',",
            f"  default_options: ['cpp_std=c++{manifest['cpp_standard']}'],",
            ")",
            "",
            "# Regenerate this block:",
            "#   cmake --build <build-dir> --target ccmath-generate-secondary-builds",
            "#",
            "# Consumer usage:",
            "#   ccmath_dep = dependency('ccmath', fallback : ['ccmath', 'ccmath_dep'])",
            "",
            "subdir('cmake/gen')",
            "",
        ]
    )

    return "\n".join(
        [
            "# CCMath secondary build (library consumption). CMake is the primary build system.",
            "# Regenerate generated files:",
            "#   cmake --build <build-dir> --target ccmath-generate-secondary-builds",
            "",
            GENERATED_BEGIN_MESON,
            generated.rstrip(),
            GENERATED_END_MESON,
            "",
        ]
    )


def root_premake_wrapper(manifest: dict[str, Any], source_dir: Path) -> str:
    body = premake_content(
        manifest,
        source_dir,
        source_dir / ROOT_SECONDARY_INCLUDE,
        source_dir,
        version_header=render_version_header(manifest, source_dir),
    )
    return "\n".join(
        [
            "-- CCMath secondary build (library consumption). CMake is the primary build system.",
            "-- Regenerate generated files:",
            "--   cmake --build <build-dir> --target ccmath-generate-secondary-builds",
            "",
            GENERATED_BEGIN_PREMAKE,
            body.rstrip(),
            GENERATED_END_PREMAKE,
            "",
        ]
    )


def cmake_gen_meson(manifest: dict[str, Any], source_dir: Path) -> str:
    gen_subdir = source_dir / "cmake" / "gen"
    source_include = relative_posix_path(source_dir / "include", gen_subdir)
    options = manifest["options"]

    lines = [
        "cpp = meson.get_compiler('cpp')",
        "",
        "_ccmath_builtin_bit_cast_probe = '''",
        BIT_CAST_PROBE.rstrip(),
        "'''",
        "",
        "if not cpp.compiles(_ccmath_builtin_bit_cast_probe, name : 'ccmath builtin bit_cast support')",
        "  error('CCMath requires compiler support for __builtin_bit_cast')",
        "endif",
        "",
        "_ccmath_defines = []",
    ]
    lines.extend(meson_define_lines(options))
    lines.extend(meson_runtime_probe_lines(options))
    lines.extend(
        [
            "",
            "# Generate ccmath/version.hpp natively so Meson consumers never depend on a CMake configure.",
            "subdir('ccmath')",
            "",
            "ccmath_inc = include_directories(",
            f"  '{source_include}',",
            "  '.',",
            ")",
            "",
            "ccmath_dep = declare_dependency(",
            "  include_directories: ccmath_inc,",
            "  compile_args: _ccmath_defines,",
            "  sources: ccmath_version_hpp,",
            ")",
            "",
            "meson.override_dependency('ccmath', ccmath_dep)",
        ]
    )

    return "\n".join(
        [
            "# Generated by tools/gen_secondary_builds.py. Do not edit by hand.",
            "# Exposes CCMath as a Meson dependency only.",
            "",
            "\n".join(lines),
            "",
        ]
    )


def cmake_gen_meson_version(manifest: dict[str, Any]) -> str:
    version_template = manifest["version_template"]
    return "\n".join(
        [
            "# Generated by tools/gen_secondary_builds.py. Do not edit by hand.",
            "# Produces <builddir>/cmake/gen/ccmath/version.hpp from the version template, so the",
            "# 'ccmath' include prefix resolves without a CMake configure or a checked-in header.",
            "",
            "_ccmath_version_parts = meson.project_version().split('.')",
            "_ccmath_version_conf = configuration_data()",
            "_ccmath_version_conf.set('CCMATH_VERSION_MAJOR', _ccmath_version_parts[0])",
            "_ccmath_version_conf.set('CCMATH_VERSION_MINOR', _ccmath_version_parts[1])",
            "_ccmath_version_conf.set('CCMATH_VERSION_PATCH', _ccmath_version_parts[2])",
            "",
            "ccmath_version_hpp = configure_file(",
            f"  input: meson.project_source_root() / '{version_template}',",
            "  output: 'version.hpp',",
            "  configuration: _ccmath_version_conf,",
            ")",
            "",
        ]
    )


def write_outputs(
    manifest: dict[str, Any],
    output_dir: Path,
    source_dir: Path,
    update_root: bool,
) -> None:
    output_dir = output_dir.resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    version_header = render_version_header(manifest, source_dir)
    write_version_header(output_dir / "include", version_header)

    (output_dir / "meson.build").write_text(
        meson_build_content(manifest, source_dir, output_dir),
        encoding="utf-8",
    )
    options_txt = meson_options_txt(manifest["options"])
    (output_dir / "meson_options.txt").write_text(options_txt, encoding="utf-8")
    (output_dir / "premake5.lua").write_text(
        premake_content(manifest, source_dir, output_dir / "include", output_dir),
        encoding="utf-8",
    )

    if update_root:
        gen_dir = source_dir / "cmake" / "gen"
        gen_dir.mkdir(parents=True, exist_ok=True)
        (gen_dir / "meson.build").write_text(
            cmake_gen_meson(manifest, source_dir),
            encoding="utf-8",
        )
        gen_version_dir = gen_dir / "ccmath"
        gen_version_dir.mkdir(parents=True, exist_ok=True)
        (gen_version_dir / "meson.build").write_text(
            cmake_gen_meson_version(manifest),
            encoding="utf-8",
        )
        (source_dir / "meson_options.txt").write_text(options_txt, encoding="utf-8")
        (source_dir / "meson.build").write_text(
            root_meson_wrapper(manifest),
            encoding="utf-8",
        )
        (source_dir / "premake5.lua").write_text(
            root_premake_wrapper(manifest, source_dir),
            encoding="utf-8",
        )


def check_root_markers(source_dir: Path) -> int:
    for name in ("meson.build", "premake5.lua"):
        path = source_dir / name
        if not path.exists():
            print(f"check failed: missing {path}", file=sys.stderr)
            return 1
        text = path.read_text(encoding="utf-8")
        markers = (
            (GENERATED_BEGIN_MESON, GENERATED_END_MESON)
            if name == "meson.build"
            else (GENERATED_BEGIN_PREMAKE, GENERATED_END_PREMAKE)
        )
        if markers[0] not in text or markers[1] not in text:
            print(f"check failed: markers missing in {path}", file=sys.stderr)
            return 1
        if name == "premake5.lua":
            for line in text.splitlines():
                if "_ccmath_include_dirs" not in line:
                    continue
                if re.search(r'["\'](/|[A-Za-z]:[/\\])', line):
                    print(f"check failed: absolute include path in {path}", file=sys.stderr)
                    return 1
    return 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--source-dir", type=Path, required=True)
    parser.add_argument("--update-root", action="store_true")
    parser.add_argument("--check", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    manifest = load_manifest(args.manifest)

    if args.check:
        return check_root_markers(args.source_dir)

    write_outputs(
        manifest=manifest,
        output_dir=args.output_dir.resolve(),
        source_dir=args.source_dir.resolve(),
        update_root=args.update_root,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
