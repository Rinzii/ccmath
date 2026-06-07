#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Generate self-contained HTML source/assembly viewer for local review."""

import html
import json
import webbrowser
from pathlib import Path

import _asmlab_common as C


def _load_json(path):
    if path.exists():
        return json.loads(path.read_text())
    return None


def generate_html(fn, arch, flags, compiler, variant_dir=None, view_subpath=None):
    """Write HTML viewer. Default: out/asmlab/views/<fn>/<arch>/index.html."""
    variant_dir = Path(variant_dir) if variant_dir else C.variant_dir(fn, arch, flags, compiler)
    sm = _load_json(variant_dir / "source_map.json")
    if not sm:
        return None, "source_map.json missing; run with --source-map first"

    metrics = _load_json(variant_dir / "metrics.json")
    path_a = _load_json(variant_dir / "path_analysis.json")
    emit_meta = _load_json(variant_dir / "emit_meta.json")
    asm_diff = _load_json(variant_dir / "asm_diff.json")
    analysis_diff = _load_json(variant_dir / "analysis_diff.json")
    cfg = _load_json(variant_dir / "cfg.json")
    mca_deep = _load_json(variant_dir / "mca.json")
    opt_remarks = _load_json(variant_dir / "opt_remarks.json")
    reg_pressure = _load_json(variant_dir / "reg_pressure.json")
    microarch = _load_json(variant_dir / "microarch.json")
    vectorization = _load_json(variant_dir / "vectorization.json")
    semantic = _load_json(variant_dir / "semantic.json")
    perf = _load_json(variant_dir / "perf.json")
    analysis_summary = _load_json(variant_dir / "analysis_summary.json")
    run_meta = _load_json(C.OUT_DIR / "reports" / ("%s.run.json" % fn))
    if run_meta is None:
        run_meta = _load_json(C.OUT_DIR / "reports" / ("%s.provenance.json" % fn))

    view_dir = C.OUT_DIR / "views" / (view_subpath or (fn + "/" + arch))
    view_dir.mkdir(parents=True, exist_ok=True)

    # Group source lines for left pane
    source_groups = {}
    for ent in sm.get("instructions", []):
        loc = ent.get("source", {})
        f = loc.get("file", "unknown")
        ln = loc.get("line", 0)
        key = "%s:%d" % (f, ln)
        if key not in source_groups:
            source_groups[key] = {
                "file": f,
                "line": ln,
                "snippet": ent.get("source_snippet", ""),
                "attribution": ent.get("source_attribution", ""),
                "confidence": ent.get("mapping_confidence", ""),
            }

    data = {
        "function": fn,
        "arch": arch,
        "compiler": compiler,
        "flags": flags,
        "source_map": sm,
        "metrics": metrics,
        "path_analysis": path_a,
        "emit_meta": emit_meta,
        "asm_diff": asm_diff,
        "analysis_diff": analysis_diff,
        "cfg": cfg,
        "mca_deep": mca_deep,
        "opt_remarks": opt_remarks,
        "reg_pressure": reg_pressure,
        "microarch": microarch,
        "vectorization": vectorization,
        "semantic": semantic,
        "perf": perf,
        "analysis_summary": analysis_summary,
        "run": run_meta,
        "source_groups": list(source_groups.values()),
    }
    payload = json.dumps(data)

    page = """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>asmlab view: %(fn)s (%(arch)s)</title>
<style>
body { font-family: ui-monospace, Menlo, monospace; margin: 0; font-size: 13px; }
header { background: #1e1e2e; color: #cdd6f4; padding: 12px 16px; }
header h1 { margin: 0 0 8px 0; font-size: 16px; }
.meta { color: #a6adc8; font-size: 12px; }
.layout { display: grid; grid-template-columns: 1fr 1fr; height: calc(100vh - 120px); }
.pane { overflow: auto; border-top: 1px solid #45475a; }
.pane h2 { position: sticky; top: 0; background: #313244; color: #cdd6f4;
           margin: 0; padding: 8px 12px; font-size: 13px; }
.source-line, .asm-line { padding: 4px 12px; border-bottom: 1px solid #313244; cursor: pointer; }
.source-line:hover, .asm-line:hover { background: #45475a; }
.source-line.active, .asm-line.active { background: #585b70; }
.file-header { background: #181825; color: #89b4fa; padding: 6px 12px; font-weight: bold; }
.tags { color: #f9e2af; font-size: 11px; }
.conf-low { color: #fab387; }
.conf-unknown { color: #f38ba8; }
footer { padding: 8px 16px; background: #11111b; color: #a6adc8; font-size: 11px; }
.filter-bar { padding: 8px 12px; background: #181825; }
.filter-bar label { color: #cdd6f4; margin-right: 12px; }
.diff-panel { padding: 12px; background: #181825; color: #cdd6f4; max-height: 200px; overflow: auto; }
.analysis-panel { padding: 12px 16px; background: #11111b; color: #bac2de; border-top: 1px solid #45475a;
                  max-height: 280px; overflow: auto; font-size: 12px; }
.analysis-panel h3 { margin: 8px 0 4px 0; color: #89b4fa; font-size: 12px; }
.block-tag { background: #313244; padding: 1px 4px; border-radius: 3px; color: #f9e2af; }
.bottleneck { color: #fab387; }
</style>
</head>
<body>
<header>
<h1>asmlab: %(fn)s / %(arch)s</h1>
<div class="meta" id="meta"></div>
</header>
<div class="filter-bar">
<label><input type="checkbox" id="hide-harness" checked> Hide harness lines</label>
<label><input type="checkbox" id="ccmath-only"> ccmath-internal only</label>
</div>
<div class="layout">
<div class="pane" id="source-pane"><h2>Source</h2><div id="source-list"></div></div>
<div class="pane" id="asm-pane"><h2>Assembly</h2><div id="asm-list"></div></div>
</div>
<div class="diff-panel" id="diff-panel"></div>
<div class="analysis-panel" id="analysis-panel"></div>
<footer>Static metrics and path classification are estimates. Deep analysis attributions are correlational unless perf counters agree.</footer>
<script>
const DATA = %(payload)s;
const meta = document.getElementById('meta');
const pa = DATA.path_analysis || {};
const mca = (DATA.metrics || {}).llvm_mca || {};
meta.textContent = [
  'path: ' + (pa.path_category || '-') + ' (' + (pa.confidence || '-') + ')',
  'attribution: ' + (DATA.source_map.primary_attribution || '-'),
  'static est.: ' + (mca.static_model_estimate_cycles || mca.block_rthroughput || '-'),
  'insns: ' + (DATA.source_map.instruction_count || 0)
].join(' | ');

function lineKey(file, line) { return file + ':' + line; }

const blockByOrdinal = {};
if (DATA.cfg && DATA.cfg.blocks) {
  DATA.cfg.blocks.forEach(b => {
    const start = b.start_ordinal || 0;
    const end = b.end_ordinal || start;
    for (let o = start; o <= end; o++) blockByOrdinal[o] = b.id;
  });
}

const asmByLine = {};
(DATA.source_map.instructions || []).forEach((ent, idx) => {
  const loc = ent.source || {};
  const k = lineKey(loc.file || '', loc.line || 0);
  if (!asmByLine[k]) asmByLine[k] = [];
  asmByLine[k].push({ ent, idx });
});

const sourceList = document.getElementById('source-list');
const asmList = document.getElementById('asm-list');
let activeKey = null;

function shouldShow(ent) {
  const hideHarness = document.getElementById('hide-harness').checked;
  const ccmathOnly = document.getElementById('ccmath-only').checked;
  const attr = ent.source_attribution || '';
  if (hideHarness && attr === 'harness-only') return false;
  if (ccmathOnly && attr !== 'ccmath-internal') return false;
  return true;
}

function render() {
  sourceList.innerHTML = '';
  asmList.innerHTML = '';
  const seen = {};
  (DATA.source_map.instructions || []).forEach(ent => {
    const loc = ent.source || {};
    const f = loc.file || 'unknown';
    const ln = loc.line || 0;
    const k = lineKey(f, ln);
    if (seen[k] || !shouldShow(ent)) return;
    seen[k] = true;
    const div = document.createElement('div');
    div.className = 'source-line';
    div.dataset.key = k;
    const conf = ent.mapping_confidence || 'unknown';
    div.innerHTML = '<div>' + f + ':' + ln + ' <span class="conf-' + conf + '">(' + conf + ')</span></div>'
      + '<div>' + (ent.source_snippet || '').replace(/</g,'&lt;') + '</div>';
    div.onclick = () => selectLine(k);
    sourceList.appendChild(div);
  });
  (DATA.source_map.instructions || []).forEach((ent, idx) => {
    if (!shouldShow(ent)) return;
    const loc = ent.source || {};
    const k = lineKey(loc.file || '', loc.line || 0);
    const div = document.createElement('div');
    div.className = 'asm-line';
    div.dataset.key = k;
    div.dataset.idx = idx;
    const blk = blockByOrdinal[ent.ordinal] ? ' <span class="block-tag">' + blockByOrdinal[ent.ordinal] + '</span>' : '';
    div.innerHTML = '[' + ent.ordinal + ']' + blk + ' ' + ent.assembly.replace(/</g,'&lt;')
      + ' <span class="tags">' + (ent.tags || []).join(',') + '</span>';
    div.onclick = () => selectLine(k);
    asmList.appendChild(div);
  });
}

function selectLine(k) {
  activeKey = k;
  document.querySelectorAll('.source-line, .asm-line').forEach(el => {
    el.classList.toggle('active', el.dataset.key === k);
  });
}

document.getElementById('hide-harness').onchange = render;
document.getElementById('ccmath-only').onchange = render;
render();

const diffPanel = document.getElementById('diff-panel');
if (DATA.asm_diff && DATA.asm_diff.changed_source_lines) {
  diffPanel.innerHTML = '<b>Changed source lines (vs baseline)</b><br>'
    + DATA.asm_diff.changed_source_lines.slice(0, 20).map(ch =>
      ch.file + ':' + ch.line + ' ' + ch.status
      + ' (' + ch.before_insn_count + ' -> ' + ch.after_insn_count + ' insns)'
    ).join('<br>');
}

const ap = document.getElementById('analysis-panel');
let ah = '';
if (DATA.analysis_summary) {
  const s = DATA.analysis_summary;
  ah += '<h3>Analysis summary</h3>';
  ah += 'blocks=' + (s.block_count||'-') + ' | mca IPC=' + (s.mca_ipc||'-')
    + ' | spills=' + (s.spill_count||0) + ' | vectorized=' + (s.vectorized||false) + '<br>';
}
if (DATA.mca_deep && DATA.mca_deep.bottleneck) {
  ah += '<h3>MCA bottleneck</h3><span class="bottleneck">'
    + DATA.mca_deep.bottleneck.primary + '</span> ('
    + (DATA.mca_deep.bottleneck.confidence||'correlation') + ')<br>';
}
if (DATA.vectorization) {
  ah += '<h3>Vectorization</h3>width=' + (DATA.vectorization.dominant_width||'-')
    + ' vector insns=' + (DATA.vectorization.vector_insn_count||0) + '<br>';
}
if (DATA.reg_pressure && DATA.reg_pressure.spill_count) {
  ah += '<h3>Register pressure</h3>spills=' + DATA.reg_pressure.spill_count
    + ' reloads=' + (DATA.reg_pressure.reload_count||0) + '<br>';
}
if (DATA.semantic && DATA.semantic.classifications) {
  ah += '<h3>Semantic classifications</h3>'
    + DATA.semantic.classifications.slice(0,8).map(c => c.kind).join(', ') + '<br>';
}
if (DATA.analysis_diff) {
  const d = DATA.analysis_diff;
  ah += '<h3>Deep diff vs baseline</h3>IPC delta=' + (d.mca&&d.mca.ipc_delta)
    + ' spill delta=' + (d.register_pressure&&d.register_pressure.spill_delta) + '<br>';
}
if (DATA.opt_remarks && DATA.opt_remarks.by_tag) {
  ah += '<h3>Opt remarks</h3>' + Object.keys(DATA.opt_remarks.by_tag).slice(0,6).join(', ') + '<br>';
}
ap.innerHTML = ah || '<i>Run with --deep-analyze for CFG, MCA, and semantic panels.</i>';
</script>
</body>
</html>
""" % {
        "fn": html.escape(fn),
        "arch": html.escape(arch),
        "payload": payload,
    }

    out = view_dir / "index.html"
    out.write_text(page)
    return out, None


def main(argv=None):
    import argparse
    ap = argparse.ArgumentParser(description="Generate HTML viewer for source/assembly map.")
    ap.add_argument("fn")
    ap.add_argument("--arch", default=C.DEFAULT_ARCHES[0])
    ap.add_argument("--flags", default=C.DEFAULT_FLAGS)
    ap.add_argument("--compiler", default="clang")
    ap.add_argument("--open", action="store_true")
    args = ap.parse_args(argv)

    arches = C.parse_arch_list(args.arch)
    for arch in arches:
        out, err = generate_html(args.fn, arch, args.flags, args.compiler)
        if err:
            print("error: %s" % err, file=__import__("sys").stderr)
            return 1
        print("view -> %s" % out.relative_to(C.PROJECT_ROOT))
        if args.open:
            webbrowser.open(out.as_uri())
    return 0


if __name__ == "__main__":
    import sys
    sys.exit(main())
