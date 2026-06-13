#!/usr/bin/env python3
"""Write libFuzzer seed inputs (little-endian IEEE-754 bit patterns)."""

from __future__ import annotations

import pathlib
import struct
import sys


def write_seed(out_dir: pathlib.Path, name: str, payload: bytes) -> None:
    path = out_dir / name
    path.write_bytes(payload)


def pack_f64(*values: float) -> bytes:
    return b"".join(struct.pack("<d", v) for v in values)


def pack_f32(*values: float) -> bytes:
    return b"".join(struct.pack("<f", v) for v in values)


def main() -> int:
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} <seed_dir>", file=sys.stderr)
        return 1

    out_dir = pathlib.Path(sys.argv[1])
    out_dir.mkdir(parents=True, exist_ok=True)

    write_seed(out_dir, "zero.bin", pack_f64(0.0))
    write_seed(out_dir, "neg_zero.bin", pack_f64(-0.0))
    write_seed(out_dir, "one.bin", pack_f64(1.0))
    write_seed(out_dir, "denorm_min.bin", struct.pack("<d", 5e-324))
    write_seed(out_dir, "max.bin", pack_f64(float.fromhex("0x1.ffffffffffffp+1023")))
    write_seed(out_dir, "nan.bin", struct.pack("<Q", 0x7FF8_0000_0000_0001))
    write_seed(out_dir, "pos_inf.bin", pack_f64(float("inf")))
    write_seed(out_dir, "neg_inf.bin", pack_f64(float("-inf")))

    write_seed(out_dir, "binary_pair.bin", pack_f64(1.5, -2.25) + struct.pack("<i", 42))
    write_seed(out_dir, "lerp_triple.bin", pack_f64(1.0, 10.0, 0.25))
    write_seed(out_dir, "float_edge.bin", pack_f32(0.0, -0.0, 1.0))

    # Domain-relevant pow/exp seeds: integer and half-integer exponents, base shortcuts,
    # and the huge-exponent clamp region.
    write_seed(out_dir, "pow_base10.bin", pack_f64(10.0, -10.0))
    write_seed(out_dir, "pow_base2.bin", pack_f64(2.0, 0.5))
    write_seed(out_dir, "pow_half_int.bin", pack_f64(3.0, 2.5))
    write_seed(out_dir, "pow_near_one.bin", pack_f64(1.0000000000000002, 1.0e18))
    write_seed(out_dir, "pow_neg_base.bin", pack_f64(-2.0, 3.0))
    write_seed(out_dir, "expm1_large_neg.bin", pack_f64(-895.0))
    write_seed(out_dir, "log1p_mid.bin", pack_f64(0.388235289741717726))

    # Selector-byte seeds: trailing bytes with the high bit set substitute table specials
    # (here index 10 = type max), exercising the special-value injection path for both the
    # binary float and binary double decoders.
    write_seed(out_dir, "select_double_pair.bin", pack_f64(1.0, 1.0) + bytes([0x8A, 0x8A]))
    write_seed(out_dir, "select_float_pair.bin", pack_f32(1.0, 1.0) + bytes([0x8D, 0xCD]))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
