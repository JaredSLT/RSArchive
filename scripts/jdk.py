#!/usr/bin/env python3
"""Locate a JDK 21+ home without treating one distro path as the only option.

Preference order when several installs qualify:
  1. $JAVA_HOME, if bin/java exists and the major version is >= 21.
  2. /usr/lib/jvm/java-21-openjdk-amd64, then other /usr/lib/jvm/java-21* homes.
  3. The home of `java` on PATH, if its major version is >= 21.

A JDK 21 home wins over a newer one when both exist.
"""

from __future__ import annotations

import os
import re
import shutil
import subprocess
from pathlib import Path

_CACHED: Path | None = None
_VERSION_RE = re.compile(r'version "(?P<ver>[^"]+)"')


def _major(java: Path) -> int | None:
    try:
        proc = subprocess.run(
            [str(java), "-version"],
            capture_output=True,
            text=True,
            timeout=30,
        )
    except (OSError, subprocess.TimeoutExpired):
        return None
    text = (proc.stderr or "") + "\n" + (proc.stdout or "")
    match = _VERSION_RE.search(text)
    if not match:
        return None
    ver = match.group("ver")
    if ver.startswith("1."):
        parts = ver.split(".")
        if len(parts) > 1 and parts[1].isdigit():
            return int(parts[1])
        return None
    head = ver.split(".", 1)[0]
    return int(head) if head.isdigit() else None


def _qualified(home: Path) -> tuple[Path, int] | None:
    java = home / "bin" / "java"
    if not java.is_file():
        return None
    major = _major(java)
    if major is None or major < 21:
        return None
    return home, major


def _candidates() -> list[tuple[Path, int]]:
    found: list[tuple[Path, int]] = []
    seen: set[Path] = set()

    def add(home: Path) -> None:
        try:
            key = home.resolve()
        except OSError:
            key = home
        if key in seen:
            return
        hit = _qualified(home)
        if hit is None:
            return
        seen.add(key)
        found.append(hit)

    env_home = os.environ.get("JAVA_HOME", "").strip()
    if env_home:
        add(Path(env_home))

    preferred = Path("/usr/lib/jvm/java-21-openjdk-amd64")
    add(preferred)
    jvm = Path("/usr/lib/jvm")
    if jvm.is_dir():
        for path in sorted(jvm.glob("java-21*")):
            add(path)

    which = shutil.which("java")
    if which:
        add(Path(which).resolve().parent.parent)
    return found


def java_home() -> Path:
    """Return a JDK 21+ home, preferring major version 21."""
    global _CACHED
    if _CACHED is not None:
        return _CACHED
    found = _candidates()
    if not found:
        raise SystemExit(
            "Install JDK 21 or newer. JAVA_HOME can point at a JDK 21+ installation."
        )
    twenty_one = [home for home, major in found if major == 21]
    chosen = twenty_one[0] if twenty_one else found[0][0]
    _CACHED = chosen
    return chosen


def java_bin() -> Path:
    return java_home() / "bin" / "java"


def javac_bin() -> Path:
    return java_home() / "bin" / "javac"


def jar_bin() -> Path:
    return java_home() / "bin" / "jar"


if __name__ == "__main__":
    print(java_home())
