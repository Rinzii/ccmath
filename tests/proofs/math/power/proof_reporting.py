from __future__ import annotations

import hashlib
import json
import shlex
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha256_text(text: str) -> str:
    return sha256_bytes(text.encode("utf-8"))


def sha256_file(path: Path) -> str:
    return sha256_bytes(path.read_bytes())


def command_string(cmd: list[str]) -> str:
    return " ".join(shlex.quote(part) for part in cmd)


def run_command(cmd: list[str], *, input_text: str | None = None) -> dict[str, Any]:
    result: dict[str, Any] = {
        "command": cmd,
        "command_string": command_string(cmd),
        "stdin_sha256": sha256_text(input_text) if input_text is not None else None,
    }

    try:
        completed = subprocess.run(
            cmd,
            input=input_text,
            text=True,
            capture_output=True,
            check=False,
        )
    except FileNotFoundError:
        result.update(
            {
                "available": False,
                "returncode": None,
                "stdout": "",
                "stderr": "",
            }
        )
        return result

    result.update(
        {
            "available": True,
            "returncode": completed.returncode,
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }
    )
    return result


def utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat()


def write_json_report(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
