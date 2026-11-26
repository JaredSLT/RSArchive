#!/usr/bin/env python3
"""Compile repairs for RS3 831 and 860 Fernflower sources.

Does not run the LostCity deobfuscator and does not touch deob/, original/,
or fernflower/. Java 21 cannot name a default-package type from a named
package, so the twitchtv sources move to the default package (their only
unresolved types live there) and jagdx fourcc constants are inlined.
Raw for-each loops dropped by Fernflower are cast to Iterable<T>.
"""
from __future__ import annotations

import re
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BACKUP = ROOT / "work" / "share-rs3" / "backup"
FOLDERS = {
    831: "831-2014-12-04",
    860: "860-2015-12-01",
}
BOX = {
    "int": "Integer",
    "long": "Long",
    "boolean": "Boolean",
    "byte": "Byte",
    "short": "Short",
    "char": "Character",
    "float": "Float",
    "double": "Double",
}
FOR_RE = re.compile(
    r"^(\s*)for\s*\(\s*([\w.]+(?:\s*\[\s*\])*)\s+(\w+)\s*:\s*(.+)\)\s*\{\s*$"
)
FOURCC_RE = re.compile(
    r"=\s*[\w.]+\s*\(\s*'([^'])'\s*,\s*'([^'])'\s*,\s*'([^'])'\s*,\s*'([^'])'\s*\)\s*;"
)


def log(msg: str) -> None:
    print(msg, flush=True)


def backup(src: Path, rev_folder: str) -> None:
    rel = src.relative_to(ROOT / "builds" / "rs3" / rev_folder)
    dest = BACKUP / rev_folder / rel
    if dest.exists():
        return
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dest)


def java_root(rev: int) -> Path:
    return ROOT / "builds" / "rs3" / FOLDERS[rev] / "src" / "src" / "main" / "java"


def fourcc(chars: tuple[str, str, str, str]) -> str:
    parts = []
    shifts = (0, 8, 16, 24)
    for ch, shift in zip(chars, shifts):
        piece = f"('{ch}' & 255)"
        if shift:
            piece = f"({piece} << {shift})"
        parts.append(piece)
    return " = " + " | ".join(parts) + ";"


def fix_jagdx(path: Path) -> bool:
    text = path.read_text(errors="ignore")
    new, n = FOURCC_RE.subn(lambda m: fourcc(m.groups()), text)
    if n and new != text:
        path.write_text(new)
        return True
    return False


def matching_brace(text: str, open_at: int) -> int:
    depth = 0
    i = open_at
    n = len(text)
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n and text[i + 1] == "/":
            i = text.find("\n", i)
            i = n if i < 0 else i
            continue
        if ch == "/" and i + 1 < n and text[i + 1] == "*":
            end = text.find("*/", i + 2)
            i = n if end < 0 else end + 2
            continue
        if ch == '"':
            i += 1
            while i < n:
                if text[i] == "\\":
                    i += 2
                    continue
                if text[i] == '"':
                    i += 1
                    break
                i += 1
            continue
        if ch == "'":
            i += 1
            if i < n and text[i] == "\\":
                i += 2
            if i < n and text[i] != "'":
                i += 1
            if i < n and text[i] == "'":
                i += 1
            continue
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return -1


def statement_end(text: str, start: int) -> int:
    """End index (exclusive) of the statement/block starting at start."""
    i = start
    n = len(text)
    while i < n and text[i] in " \t\r\n":
        i += 1
    if i < n and text[i] == "{":
        end = matching_brace(text, i)
        return n if end < 0 else end + 1
    # if (...) stmt
    depth = 0
    seen = False
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n and text[i + 1] == "/":
            nl = text.find("\n", i)
            i = n if nl < 0 else nl + 1
            continue
        if ch == '"':
            i += 1
            while i < n:
                if text[i] == "\\":
                    i += 2
                    continue
                if text[i] == '"':
                    i += 1
                    break
                i += 1
            continue
        if ch == "(":
            depth += 1
            seen = True
        elif ch == ")":
            depth -= 1
        elif ch == "{" and depth == 0:
            end = matching_brace(text, i)
            return n if end < 0 else end + 1
        elif ch == ";" and depth == 0 and seen:
            return i + 1
        i += 1
    return n


def fix_catch_scope(text: str) -> str:
    needle = "} catch (Throwable "
    start = 0
    while True:
        at = text.find(needle, start)
        if at < 0:
            return text
        name_at = at + len(needle)
        m = re.match(r"(\w+)\)", text[name_at:])
        if not m:
            start = name_at
            continue
        name = m.group(1)
        brace = text.find("{", name_at)
        if brace < 0:
            return text
        close = matching_brace(text, brace)
        if close < 0:
            return text
        after = close + 1
        if_at = re.match(rf"\s*if\s*\(\s*{re.escape(name)}\s+instanceof\b", text[after:])
        if not if_at:
            start = after
            continue
        if_rel = after + if_at.start()
        # skip whitespace so the if starts at the keyword
        if_kw = text.find("if", if_rel)
        stmt_end = statement_end(text, if_kw)
        if_text = text[if_kw:stmt_end].strip("\n")
        # indent if-body one level under the catch close line
        line_start = text.rfind("\n", 0, close) + 1
        indent = re.match(r"[ \t]*", text[line_start:close]).group(0) + "   "
        moved = "\n" + "\n".join(
            (indent + ln) if ln.strip() else ln for ln in if_text.splitlines()
        ) + "\n" + indent[:-3]
        # insert before the catch close brace
        text = text[:close] + moved + text[close:if_kw] + text[stmt_end:]
        # collapse the blank line left where the if was
        text = re.sub(r"\n[ \t]*\n[ \t]*\n", "\n\n", text, count=1)
        start = close
    return text


def fix_foreach_line(line: str) -> str:
    m = FOR_RE.match(line.rstrip("\n"))
    if not m:
        return line
    indent, typ, name, expr = m.groups()
    if expr.startswith("("):
        return line
    cast = BOX.get(typ.replace(" ", ""), typ.replace(" ", ""))
    nl = "\n" if line.endswith("\n") else ""
    return f"{indent}for({typ} {name} : (Iterable<{cast}>) ({expr})) {{{nl}"


def fix_foreach_file(path: Path, lines_needed: set[int] | None) -> int:
    raw = path.read_text(errors="ignore").splitlines(True)
    changed = 0
    for i, line in enumerate(raw):
        if lines_needed is not None and (i + 1) not in lines_needed:
            continue
        if "for" not in line or ":" not in line:
            continue
        new = fix_foreach_line(line)
        if new != line:
            raw[i] = new
            changed += 1
    if changed:
        path.write_text("".join(raw))
    return changed


def move_twitch(root: Path, rev_folder: str) -> int:
    src_dir = root / "com" / "jagex" / "twitchtv"
    if not src_dir.is_dir():
        return 0
    moved = 0
    for path in sorted(src_dir.glob("*.java")):
        text = path.read_text(errors="ignore")
        if not text.startswith("package com.jagex.twitchtv;"):
            continue
        dest = root / path.name
        if dest.exists():
            raise SystemExit(f"refusing to clobber {dest}")
        backup(path, rev_folder)
        body = re.sub(r"^package com\.jagex\.twitchtv;\n+", "", text, count=1)
        dest.write_text(body)
        path.unlink()
        moved += 1
    if moved:
        for path in root.rglob("*.java"):
            text = path.read_text(errors="ignore")
            new = re.sub(r"^import com\.jagex\.twitchtv\.[A-Za-z0-9_]+;\n", "", text, flags=re.M)
            new = new.replace("com.jagex.twitchtv.", "")
            if new != text:
                backup(path, rev_folder)
                path.write_text(new)
    return moved


def repair(rev: int) -> None:
    folder = FOLDERS[rev]
    root = java_root(rev)
    log_path = ROOT / "logs" / f"compile-{folder}.log"
    foreach_lines: dict[Path, set[int]] = {}
    if log_path.exists():
        for line in log_path.read_text(errors="ignore").splitlines():
            m = re.match(r"^(/.*\.java):(\d+): error: incompatible types: Object cannot be converted", line)
            if m:
                foreach_lines.setdefault(Path(m.group(1)), set()).add(int(m.group(2)))
    n_for = 0
    for path, lines in foreach_lines.items():
        if not path.exists():
            continue
        backup(path, folder)
        n_for += fix_foreach_file(path, lines)
    # byte[] local Fernflower spelled as "B"
    n_b = 0
    for path in root.rglob("*.java"):
        text = path.read_text(errors="ignore")
        if "\n         B var22;\n" not in text and "B var22;" not in text:
            continue
        if "return (byte[])var22;" not in text:
            continue
        backup(path, folder)
        path.write_text(text.replace("B var22;", "byte[] var22;"))
        n_b += 1
    n_catch = 0
    for path in (root / "client.java",):
        if not path.exists():
            continue
        text = path.read_text(errors="ignore")
        new = fix_catch_scope(text)
        if new != text:
            backup(path, folder)
            path.write_text(new)
            n_catch += 1
    jag = root / "jagdx" / "class3.java"
    n_jag = 0
    if jag.exists():
        backup(jag, folder)
        n_jag = 1 if fix_jagdx(jag) else 0
    n_tw = move_twitch(root, folder)
    log(
        f"{folder}: foreach={n_for} byte-local={n_b} catch={n_catch} "
        f"fourcc={n_jag} twitch-moved={n_tw}"
    )


def main() -> None:
    for rev in (831, 860):
        repair(rev)


if __name__ == "__main__":
    main()
