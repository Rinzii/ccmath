#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""CPU notes attached to asmlab reports."""

import json
import re
import sys
from pathlib import Path

import _asmlab_common as C

KNOWLEDGE_DIR = C.ASMLAB_DIR / "knowledge"
SCHEMA_PATH = KNOWLEDGE_DIR / "schema.json"

THIN_SLICE_MAX_INSN = 40
KERNEL_EXPOSED_MIN_INTERNAL = 10
KERNEL_EXPOSED_MIN_INSN = 50


def _read_json(path):
    if not path.exists():
        return {}
    return json.loads(path.read_text())


def _load_schema():
    return _read_json(SCHEMA_PATH)


def _source_ids():
    return {s["id"] for s in load_sources()}


def load_sources():
    return _read_json(KNOWLEDGE_DIR / "sources.json").get("sources", [])


def load_microarchitectures():
    return _read_json(KNOWLEDGE_DIR / "microarchitectures.json").get("microarchitectures", [])


def get_microarch(asmlab_arch):
    for entry in load_microarchitectures():
        if entry.get("asmlab_arch") == asmlab_arch:
            return entry
    return None


def get_calibration(asmlab_arch):
    key = asmlab_arch.replace("-", "_")
    if key == "x86_64_v3":
        path = KNOWLEDGE_DIR / "calibration" / "x86_64_v3.json"
    elif key == "apple_m1":
        path = KNOWLEDGE_DIR / "calibration" / "apple_m1.json"
    else:
        path = KNOWLEDGE_DIR / "calibration" / ("%s.json" % key)
    rec = _read_json(path)
    return rec if rec else None


def load_pattern_rules():
    rules = []
    index = _read_json(KNOWLEDGE_DIR / "pattern_rules" / "index.json")
    for rid in index.get("rules", []):
        rec = _read_json(KNOWLEDGE_DIR / "pattern_rules" / ("%s.json" % rid))
        if rec:
            rules.append(rec)
    return rules


def load_instruction_facts(isa=None):
    facts = []
    for sub in ("x86", "aarch64"):
        if isa == "x86_64" and sub != "x86":
            continue
        if isa == "aarch64" and sub != "aarch64":
            continue
        base = KNOWLEDGE_DIR / "instruction_facts" / sub
        if not base.exists():
            continue
        for path in sorted(base.glob("*.json")):
            doc = _read_json(path)
            facts.extend(doc.get("facts", []))
    return facts


def _region_text(variant_dir):
    variant_dir = Path(variant_dir) if variant_dir else None
    if not variant_dir:
        return ""
    for name in ("region.s", "asm.s"):
        p = variant_dir / name
        if p.exists():
            return p.read_text().lower()
    return ""


def _source_map_summary(variant_dir):
    variant_dir = Path(variant_dir) if variant_dir else None
    if not variant_dir:
        return {}
    for name in ("scenario_source_map.json", "source_map.json"):
        p = variant_dir / name
        if p.exists():
            return _read_json(p)
    return {}


def kernel_exposed_in_source_map(variant_dir, path_analysis=None):
    """Conservative check: enough internal attribution to allow kernel-level static notes."""
    sm = _source_map_summary(variant_dir)
    pa = path_analysis or {}
    if not sm and not pa:
        return False
    counts = sm.get("attribution_counts") or {}
    internal = counts.get("ccmath-internal", 0) + counts.get("ccmath_internal", 0)
    harness = counts.get("harness-only", 0)
    insn = sm.get("instruction_count") or pa.get("instruction_count") or 0
    primary = sm.get("primary_attribution", "")
    if internal >= KERNEL_EXPOSED_MIN_INTERNAL and internal > harness:
        return True
    if insn >= KERNEL_EXPOSED_MIN_INSN and internal > 0:
        return True
    if primary in ("ccmath-internal", "ccmath_internal", "ccmath_runtime_path"):
        if insn >= 30 and internal >= 5:
            return True
    return False


def detect_patterns(path_analysis=None, region_text="", isa_hint=None, variant_dir=None):
    """Return heuristic pattern matches. Advisory only, not measured proof."""
    pa = path_analysis or {}
    text = region_text or ""
    insn = pa.get("instruction_count", 0)
    call_count = pa.get("call_count", 0)
    local_calls = pa.get("local_calls", [])
    kernel_exposed = kernel_exposed_in_source_map(variant_dir, pa)
    matched = []

    def _add(rule_id, confidence, evidence):
        matched.append({
            "pattern_rule_id": rule_id,
            "confidence": confidence,
            "match_kind": "heuristic",
            "evidence": evidence,
        })

    if (call_count > 0 and insn and insn <= THIN_SLICE_MAX_INSN and local_calls
            and not kernel_exposed):
        _add("call_boundary_thin_slice", "high",
             "insn=%s call_count=%s local_calls=%s" % (insn, call_count, len(local_calls)))

    if re.search(r"vfmadd", text) and re.search(r"(\(%rsp\)|-4\(%rsp\))", text):
        _add("x86_stack_serial_fma", "medium", "vfmadd with rsp stack operand in region")

    if re.search(r"\bfmadd\b", text) and re.search(r"ldr\w*.*\[sp", text):
        _add("aarch64_stack_serial_fma", "medium", "fmadd with [sp] ldr in region")

    if re.search(r"(vfmadd|fmadd).*\n.*(vfmadd|fmadd)", text):
        _add("back_to_back_fma_dependency", "medium", "consecutive FMA in region text")

    if (isa_hint == "arm" and "bl\t" in text and insn
            and insn <= THIN_SLICE_MAX_INSN and not kernel_exposed):
        if not any(m["pattern_rule_id"] == "call_boundary_thin_slice" for m in matched):
            _add("call_boundary_thin_slice", "high", "AArch64 bl in thin region")

    return matched


def calibration_warnings(fn, arch, path_analysis=None, variant_dir=None,
                         report_kind="function", variant=None):
    """Host/arch-specific warnings from calibration records."""
    warnings = []
    cal = get_calibration(arch)
    if not cal:
        return warnings

    pa = path_analysis or {}
    insn = pa.get("instruction_count", 0)
    region = _region_text(variant_dir)
    kernel_exposed = kernel_exposed_in_source_map(variant_dir, pa)

    if fn == "powf_impl" and arch == "apple-m1":
        for item in cal.get("known_limitations", []):
            if "powf_impl" in item or "bl to the kernel" in item:
                warnings.append({
                    "id": "m1_powf_impl_thin_slice",
                    "message": item,
                    "confidence": "high",
                    "source": cal.get("source", "asmlab_local_calibration"),
                })
        thin = not kernel_exposed and (
            (insn and insn <= 30) or (
                region and "bl\t" in region and insn and insn <= THIN_SLICE_MAX_INSN))
        if thin:
            warnings.append({
                "id": "call_boundary_thin_slice",
                "message": (
                    "M1 powf_impl analyzed region is call-boundary-like. "
                    "Spill and static counts are not comparable to x86-64-v3 here."),
                "confidence": "high",
                "source": "asmlab_local_calibration",
                "pattern_rule_id": "call_boundary_thin_slice",
            })

    if fn == "powf_impl" and arch == "x86-64-v3" and kernel_exposed:
        warnings.append({
                "id": "x86_preferred_c1_static",
                "message": (
                    "On this artifact, x86-64-v3 scenario/source-map coverage may support "
                    "C1 static spill/dependency review. Confirm kernel exposure in source_map "
                    "before relying on static claims."),
                "confidence": "medium",
                "source": cal.get("source"),
            })

    return warnings



def annotate_scenario_report(report, path_analysis=None, variant_dir=None):
    arch = report.get("arch")
    fn = report.get("function")
    pa = path_analysis or {}
    uarch = get_microarch(arch)
    cal = get_calibration(arch)
    isa = (C.ARCHES.get(arch) or {}).get("isa")
    region = _region_text(variant_dir)
    patterns = detect_patterns(pa, region, isa_hint=isa, variant_dir=variant_dir)
    kernel_exposed = kernel_exposed_in_source_map(variant_dir, pa)
    warnings = calibration_warnings(
        fn, arch, path_analysis, variant_dir,
        report_kind="scenario", variant=report.get("variant"))

    return {
        "annotation_kind": "advisory",
        "microarchitecture": uarch,
        "calibration": _calibration_summary(cal),
        "matched_patterns": patterns,
        "warnings": warnings,
        "kernel_exposed_heuristic": kernel_exposed,
        "source_registry_version": _read_json(KNOWLEDGE_DIR / "sources.json").get("version"),
    }


def annotate_function_report(fn, arch, metrics_entry, variant_dir=None):
    pa = metrics_entry.get("path_analysis") or {}
    uarch = get_microarch(arch)
    cal = get_calibration(arch)
    isa = (C.ARCHES.get(arch) or {}).get("isa")
    region = _region_text(variant_dir)
    patterns = detect_patterns(pa, region, isa_hint=isa, variant_dir=variant_dir)
    kernel_exposed = kernel_exposed_in_source_map(variant_dir, pa)
    warnings = calibration_warnings(fn, arch, pa, variant_dir, report_kind="function")

    return {
        "annotation_kind": "advisory",
        "arch": arch,
        "microarchitecture_id": uarch.get("id") if uarch else None,
        "microarchitecture_confidence": uarch.get("confidence") if uarch else None,
        "calibration_status": cal.get("overall_status") if cal else None,
        "matched_patterns": patterns,
        "warnings": warnings,
        "kernel_exposed_heuristic": kernel_exposed,
    }


def _calibration_summary(cal):
    if not cal:
        return None
    return {
        "id": cal.get("id"),
        "arch": cal.get("arch"),
        "golden_status": cal.get("golden_status"),
        "overall_status": cal.get("overall_status"),
        "recommendation": cal.get("recommendation"),
        "source": cal.get("source"),
    }


def render_knowledge_md(annotation):
    lines = [
        "## CPU notes",
        "",
    ]
    uarch = annotation.get("microarchitecture")
    if uarch:
        lines.append("- microarchitecture: %s (%s, confidence %s)" % (
            uarch.get("id"), uarch.get("asmlab_arch"), uarch.get("confidence")))
        avoid = uarch.get("avoid_claims") or []
        if avoid:
            lines.append("- avoid claims:")
            for a in avoid[:3]:
                lines.append("  - %s" % a)
    cal = annotation.get("calibration")
    if cal:
        lines.append("- calibration: golden=%s overall=%s" % (
            cal.get("golden_status"), cal.get("overall_status")))
        if cal.get("recommendation"):
            lines.append("- recommendation: %s" % cal["recommendation"])
    for w in annotation.get("warnings", []):
        lines.append("- %s" % w.get("message"))
    patterns = annotation.get("matched_patterns") or []
    if patterns:
        lines.append("- pattern matches:")
        for p in patterns:
            lines.append("  - %s: %s" % (p.get("pattern_rule_id"), p.get("evidence")))
    if annotation.get("kernel_exposed_heuristic") is False and patterns:
        lines.append("- note: kernel exposure heuristic is false for this artifact")
    if len(lines) <= 4:
        lines.append("- no knowledge annotations for this arch")
    lines.append("")
    return lines


def _err(errors, path, msg):
    errors.append("%s: %s" % (path, msg))


def _check_confidence(errors, path, field, value, allowed):
    if value is None:
        return
    if value not in allowed:
        _err(errors, path, "invalid %s confidence/trust value: %r" % (field, value))


def _validate_timing_field(errors, path, field_name, field_obj, source_ids):
    if field_obj is None:
        return
    if not isinstance(field_obj, dict):
        _err(errors, path, "%s must be an object" % field_name)
        return
    val = field_obj.get("value")
    conf = field_obj.get("confidence")
    src = field_obj.get("source")
    _check_confidence(errors, path, field_name, conf,
                      set(_load_schema().get("enums", {}).get("confidence", [])))
    if val is None:
        if conf not in (None, "unknown"):
            _err(errors, path, "%s value is null but confidence is %r" % (field_name, conf))
    else:
        if not src or src == "unknown":
            _err(errors, path, "%s has numeric value but source is missing or unknown" % field_name)
        elif src not in source_ids and src != "unknown":
            _err(errors, path, "%s source id %r not in sources.json" % (field_name, src))


def validate_all():
    """Validate knowledge JSON files. Returns list of error strings."""
    errors = []
    schema = _load_schema()
    enums = schema.get("enums", {})
    source_ids = set()

    spath = KNOWLEDGE_DIR / "sources.json"
    if not spath.exists():
        _err(errors, spath, "missing")
        return errors
    try:
        sources_doc = json.loads(spath.read_text())
    except json.JSONDecodeError as e:
        _err(errors, spath, "invalid JSON: %s" % e)
        return errors

    for src in sources_doc.get("sources", []):
        sid = src.get("id")
        if not sid:
            _err(errors, spath, "source missing id")
            continue
        if sid in source_ids:
            _err(errors, spath, "duplicate source id %s" % sid)
        source_ids.add(sid)
        _check_confidence(errors, spath, "kind", src.get("kind"), set(enums.get("source_kind", [])))
        _check_confidence(errors, spath, "trust", src.get("trust"), set(enums.get("trust", [])))

    mpath = KNOWLEDGE_DIR / "microarchitectures.json"
    try:
        micro_doc = json.loads(mpath.read_text())
    except json.JSONDecodeError as e:
        _err(errors, mpath, "invalid JSON: %s" % e)
        micro_doc = {}
    micro_ids = set()
    asmlab_archs = set()
    for ent in micro_doc.get("microarchitectures", []):
        mid = ent.get("id")
        if not mid:
            _err(errors, mpath, "microarchitecture missing id")
            continue
        if mid in micro_ids:
            _err(errors, mpath, "duplicate microarchitecture id %s" % mid)
        micro_ids.add(mid)
        aa = ent.get("asmlab_arch")
        if aa:
            asmlab_archs.add(aa)
        _check_confidence(errors, mpath, "confidence", ent.get("confidence"),
                          set(enums.get("microarch_confidence", [])))
        for ref in ent.get("source_ids", []):
            if ref not in source_ids:
                _err(errors, mpath, "microarchitecture %s references unknown source %s" % (mid, ref))

    cal_dir = KNOWLEDGE_DIR / "calibration"
    required_cal = schema.get("required_calibration_fields", [])
    for cpath in sorted(cal_dir.glob("*.json")):
        try:
            cal = json.loads(cpath.read_text())
        except json.JSONDecodeError as e:
            _err(errors, cpath, "invalid JSON: %s" % e)
            continue
        for field in required_cal:
            if field not in cal:
                _err(errors, cpath, "missing required field %s" % field)
        _check_confidence(errors, cpath, "confidence", cal.get("confidence"),
                          set(enums.get("confidence", [])))
        _check_confidence(errors, cpath, "golden_status", cal.get("golden_status"),
                          set(enums.get("calibration_golden_status", [])))
        _check_confidence(errors, cpath, "overall_status", cal.get("overall_status"),
                          set(enums.get("calibration_overall_status", [])))
        if not cal.get("known_limitations"):
            _err(errors, cpath, "known_limitations must be non-empty")
        if cal.get("source") and cal["source"] not in source_ids:
            _err(errors, cpath, "unknown source %s" % cal["source"])
        cal_arch = cal.get("arch")
        if cal_arch and cal_arch not in asmlab_archs:
            _err(errors, cpath, "arch %r not registered in microarchitectures.json" % cal_arch)

    pindex = KNOWLEDGE_DIR / "pattern_rules" / "index.json"
    try:
        pindex_doc = json.loads(pindex.read_text())
    except json.JSONDecodeError as e:
        _err(errors, pindex, "invalid JSON: %s" % e)
        pindex_doc = {}
    pattern_ids = set()
    for rid in pindex_doc.get("rules", []):
        rpath = KNOWLEDGE_DIR / "pattern_rules" / ("%s.json" % rid)
        if not rpath.exists():
            _err(errors, pindex, "pattern id %s has no file %s" % (rid, rpath.name))
            continue
        try:
            rule = json.loads(rpath.read_text())
        except json.JSONDecodeError as e:
            _err(errors, rpath, "invalid JSON: %s" % e)
            continue
        if rule.get("id") != rid:
            _err(errors, rpath, "id %s does not match index entry %s" % (rule.get("id"), rid))
        pattern_ids.add(rid)
        if not rule.get("confidence"):
            _err(errors, rpath, "missing confidence")
        _check_confidence(errors, rpath, "confidence", rule.get("confidence"),
                          set(enums.get("confidence", [])))
        meaning = rule.get("meaning")
        if meaning and meaning not in enums.get("pattern_meaning", []):
            _err(errors, rpath, "unknown pattern meaning %r" % meaning)
        if not rule.get("description"):
            _err(errors, rpath, "missing description")
        refs = rule.get("source_ids") or []
        if not refs:
            _err(errors, rpath, "missing source_ids")
        for ref in refs:
            if ref not in source_ids:
                _err(errors, rpath, "unknown source id %s" % ref)

    for sub in ("x86", "aarch64"):
        idir = KNOWLEDGE_DIR / "instruction_facts" / sub
        if not idir.exists():
            continue
        for fpath in sorted(idir.glob("*.json")):
            try:
                doc = json.loads(fpath.read_text())
            except json.JSONDecodeError as e:
                _err(errors, fpath, "invalid JSON: %s" % e)
                continue
            for i, fact in enumerate(doc.get("facts", [])):
                label = "%s fact[%d]" % (fpath.name, i)
                refs = fact.get("source_ids") or []
                if not refs:
                    _err(errors, fpath, "%s missing source_ids" % label)
                for ref in refs:
                    if ref not in source_ids:
                        _err(errors, fpath, "%s references unknown source %s" % (label, ref))
                pr = fact.get("pattern_rule_id")
                if pr and pr not in pattern_ids:
                    _err(errors, fpath, "%s references unknown pattern %s" % (label, pr))
                for tfield in schema.get("timing_fields", []):
                    if tfield in fact:
                        _validate_timing_field(errors, fpath, "%s.%s" % (label, tfield),
                                               fact[tfield], source_ids)

    return errors


def main(argv=None):
    errors = validate_all()
    if errors:
        print("cpu knowledge validation FAILED (%d errors):" % len(errors), file=sys.stderr)
        for e in errors:
            print("  %s" % e, file=sys.stderr)
        return 1
    print("cpu knowledge validation OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
