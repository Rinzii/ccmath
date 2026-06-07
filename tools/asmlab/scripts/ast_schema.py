#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Forward-looking Clang AST entity attribution schema (not fully implemented)."""

# Future work maps optimization events to Clang AST entity IDs.
# This module defines the stable JSON shape only.

AST_ENTITY_SCHEMA = {
    "schema_version": 1,
    "description": "Placeholder for Clang AST entity attribution hooks.",
    "entity_kinds": [
        "function_decl",
        "template_specialization",
        "constexpr_if",
        "lambda",
        "overload_candidate",
        "polynomial_region",
    ],
    "mapping_fields": {
        "entity_id": "stable hash of USR or serialized AST path",
        "source_range": {"file": "path", "begin_line": 0, "end_line": 0},
        "parent_entity_id": "optional",
        "template_args": "optional string",
    },
    "join_keys": [
        "source_range overlaps instruction .loc",
        "optimization remark DebugLoc",
        "semantic classification file:line",
    ],
    "status": "architecture_only",
    "notes": [
        "Full AST extraction requires clang -ast-dump=json or libclang batch tooling.",
        "Do not claim AST-level causation until entity IDs are populated.",
    ],
}


def write_ast_schema(variant_dir):
    import analysis_common as AC
    AC.write_artifact(variant_dir, "ast_schema", AST_ENTITY_SCHEMA, [
        "# AST attribution schema",
        "",
        "Status: architecture only. See ast_schema.json for join keys.",
        "",
    ])
    return AST_ENTITY_SCHEMA
