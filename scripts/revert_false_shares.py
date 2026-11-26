#!/usr/bin/env python3
"""Revert readable class names that were copied onto a different type.

Default is a dry-run: print planned renames and exit 0 without writing.
``--apply`` rewrites those revisions, compiles each one, and restores a
revision from its backup if compileJava fails.

Engines stay separate. Anchor revisions (RSC 177, RS2 254/317/377, RS3 910)
and RS2 742 are never edited.
"""

from __future__ import annotations

import argparse
import hashlib
import os
import re
import shutil
import subprocess
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILDS = ROOT / "builds"
WORK = ROOT / "work" / "revert-false-shares"
BACKUP = WORK / "backup"
JAVA_HOME = Path("/usr/lib/jvm/java-21-openjdk-amd64")

FAMILIES = ("rsc", "rs2", "rs3")
# First existing revision in this list is the name's anchor.
ANCHOR_ORDER = {
    "rsc": (177,),
    "rs2": (317, 254, 377),
    "rs3": (910,),
}
NEVER_EDIT = {
    "rsc": {177},
    "rs2": {254, 317, 377, 742},
    "rs3": {910},
}
SKIP_REV = {("rs2", 742)}

JACCARD_MIN = 0.20
MIN_METHODS = 4
SENTENCE_RATIO = 0.75

KEYWORDS = {
    "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
    "class", "const", "continue", "default", "do", "double", "else", "enum",
    "extends", "final", "finally", "float", "for", "goto", "if", "implements",
    "import", "instanceof", "int", "interface", "long", "native", "new",
    "package", "private", "protected", "public", "return", "short", "static",
    "strictfp", "super", "switch", "synchronized", "this", "throw", "throws",
    "transient", "try", "void", "volatile", "while", "true", "false", "null",
    "var", "yield", "record", "sealed", "permits",
}
SKIP_WORDS = KEYWORDS | {"if", "for", "while", "switch", "catch", "do", "try", "else", "return", "new", "throw", "synchronized"}
PRIMITIVES = {"void", "boolean", "byte", "short", "int", "long", "char", "float", "double"}
# java.lang types that need no import. Other JDK supers are detected from imports.
JAVA_LANG = {
    "Object", "Enum", "Exception", "Error", "Throwable", "RuntimeException",
    "Thread", "Number", "ClassLoader", "Class", "String", "StringBuilder",
    "StringBuffer", "Integer", "Long", "Short", "Byte", "Float", "Double",
    "Boolean", "Character", "Void", "ThreadGroup", "Process", "Package",
    "Record", "Error", "SecurityException", "IllegalArgumentException",
    "IllegalStateException", "NullPointerException", "IndexOutOfBoundsException",
    "ArrayIndexOutOfBoundsException", "StringIndexOutOfBoundsException",
    "NumberFormatException", "ClassCastException", "CloneNotSupportedException",
    "InterruptedException", "UnsupportedOperationException",
}

READABLE_RE = re.compile(r"^[A-Z][A-Za-z0-9_]{7,}$")
CLASS_DIGITS_RE = re.compile(r"^class\d+$")
DATABLOCK_RE = re.compile(r"^DataBlock\d+$")
CHEX_RE = re.compile(r"^C[0-9a-fA-F]{8}$")
IDENT_RE = re.compile(r"[A-Za-z_$][A-Za-z0-9_$]*")
LEGAL_RE = re.compile(r"^[A-Za-z_$][A-Za-z0-9_$]*$")
PKG_RE = re.compile(r"^package\s+([\w.]+)\s*;")
IMPORT_RE = re.compile(r"^import\s+(?:static\s+)?([\w.]+)(\.\*)?\s*;")
OBF_RE = re.compile(r"@ObfuscatedName\(\s*\"([^\"]*)\"\s*\)")
CLASS_RE = re.compile(
    r"\b(class|interface|enum|record)\s+([A-Za-z_$][\w$]*)"
    r"(?:\s+extends\s+([^{]+?))?"
    r"(?:\s+implements\s+([^{]+?))?"
)
SIG_RE = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|native|synchronized|abstract|default)[ \t]+)*"
    r"(?:(?P<ret>[\w.$]+(?:[ \t]*<[^>;{}]*>)?(?:[ \t]*\[[ \t]*\])*)[ \t]+)?"
    r"(?P<name>[A-Za-z_$][\w$]*)[ \t]*\((?P<params>[^)]*)\)"
    r"[ \t]*(?:throws[ \t]+[\w.$, \t]+)?(?P<end>\{|;)[ \t]*$"
)
CAMEL_RE = re.compile(r"[A-Z]+(?![a-z])|[A-Z][a-z]*|[a-z]+|[0-9]+")

WATCH = {"PlayerEntity", "Js5Index"}


def log(msg: str) -> None:
    print(msg, flush=True)


def is_readable(name: str) -> bool:
    if not READABLE_RE.fullmatch(name or ""):
        return False
    if CLASS_DIGITS_RE.fullmatch(name) or DATABLOCK_RE.fullmatch(name):
        return False
    if CHEX_RE.fullmatch(name):
        return False
    if name in KEYWORDS:
        return False
    return True


def legal_ident(name: str) -> bool:
    return bool(name) and bool(LEGAL_RE.fullmatch(name)) and name not in KEYWORDS


def camel_words(name: str) -> list[str]:
    return [part for part in CAMEL_RE.findall(name) if part.isalpha() and len(part) >= 2]


def sentence_ratio(words: list[str], text: str) -> float:
    """Fraction of letters between the first and last hit that are the words.

    Consecutive words must be separated by a non-letter in the literal.
    That keeps a camel-case token such as ``mudclient`` from matching
    ``Mud`` + ``Client``, and still matches ``Bitte benutzen`` or ``d(Wamis``.
    """
    hay = text.casefold()
    pos = 0
    start = None
    end = None
    for index, word in enumerate(words):
        folded = word.casefold()
        found = hay.find(folded, pos)
        if found < 0:
            return 0.0
        if index > 0:
            gap = hay[pos:found]
            if gap == "" or gap.isalpha():
                # No separator, or only more letters. Not a sentence break.
                # Search further so a later separated copy can still hit.
                found = hay.find(folded, found + 1)
                while found >= 0:
                    gap = hay[end:found] if end is not None else hay[pos:found]
                    if gap and not gap.isalpha():
                        break
                    found = hay.find(folded, found + 1)
                if found < 0:
                    return 0.0
        if start is None:
            start = found
        end = found + len(folded)
        pos = end
    span = hay[start:end]
    letters = sum(1 for ch in span if ch.isalpha())
    if letters <= 0:
        return 0.0
    return sum(len(word) for word in words) / letters


def is_sentence(name: str, strings: list[str]) -> bool:
    words = camel_words(name)
    if len(words) < 2:
        return False
    for text in strings:
        if sentence_ratio(words, text) >= SENTENCE_RATIO:
            return True
    return False


def strip_code(text: str) -> tuple[str, list[str]]:
    """Mask comments and literals. Return masked text and string bodies."""
    out: list[str] = []
    strings: list[str] = []
    i = 0
    n = len(text)
    while i < n:
        ch = text[i]
        nxt = text[i + 1] if i + 1 < n else ""
        if ch == "/" and nxt == "/":
            end = text.find("\n", i)
            end = n if end < 0 else end
            out.append(" " * (end - i))
            i = end
            continue
        if ch == "/" and nxt == "*":
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
            out.append(" " * (end - i))
            i = end
            continue
        if ch == '"':
            j = i + 1
            while j < n:
                if text[j] == "\\":
                    j += 2
                    continue
                if text[j] == '"':
                    j += 1
                    break
                j += 1
            body = text[i + 1:j - 1] if j > i + 1 and j <= n and text[j - 1] == '"' else text[i + 1:j]
            strings.append(body)
            out.append('""' + " " * max(0, j - i - 2))
            i = j
            continue
        if ch == "'" and i + 1 < n:
            j = i + 1
            if j < n and text[j] == "\\":
                j += 2
            elif j < n:
                j += 1
            if j < n and text[j] == "'":
                j += 1
            out.append(" " * (j - i))
            i = j
            continue
        out.append(ch)
        i += 1
    return "".join(out), strings


def split_params(param_text: str) -> tuple[str, ...]:
    param_text = param_text.strip()
    if not param_text:
        return ()
    parts: list[str] = []
    depth = 0
    cur: list[str] = []
    for ch in param_text:
        if ch == "<":
            depth += 1
        elif ch == ">":
            depth = max(0, depth - 1)
        elif ch == "," and depth == 0:
            parts.append("".join(cur))
            cur = []
            continue
        cur.append(ch)
    if cur:
        parts.append("".join(cur))
    types: list[str] = []
    for part in parts:
        part = re.sub(r"@[\w.$]+(?:\([^)]*\))?\s*", "", part)
        part = re.sub(r"\bfinal\s+", "", part.strip())
        part = re.sub(r"\s+", " ", part).strip()
        if not part:
            continue
        if part.endswith("..."):
            part = part[:-3].strip() + "[]"
        toks = part.split(" ")
        if len(toks) >= 2:
            types.append("".join(toks[:-1]))
        else:
            types.append(toks[0])
    return tuple(types)


def shape_type(jtype: str) -> str:
    jtype = re.sub(r"\s+", "", jtype or "void")
    jtype = re.sub(r"<[^<>]*>", "", jtype)
    # Nested generics can leave a stray angle; drop them.
    jtype = jtype.replace("<", "").replace(">", "")
    dims = jtype.count("[")
    base = jtype.replace("[", "").replace("]", "")
    base = base.split(".")[-1]
    if base in PRIMITIVES:
        return base + ("[]" * dims)
    return "ref" + ("[]" * dims)


def shape_of(ret: str, params: tuple[str, ...]) -> str:
    return "(" + ",".join(shape_type(part) for part in params) + ")" + shape_type(ret)


def jaccard(left: Counter, right: Counter) -> float:
    if not left and not right:
        return 1.0
    keys = set(left) | set(right)
    inter = sum(min(left[key], right[key]) for key in keys)
    union = sum(max(left[key], right[key]) for key in keys)
    if union <= 0:
        return 1.0
    return inter / union


def containment(smaller: Counter, larger: Counter) -> float:
    """Share of ``smaller``'s method shapes that also occur in ``larger``."""
    total = sum(smaller.values())
    if total <= 0:
        return 1.0
    inter = sum(min(smaller[key], larger[key]) for key in smaller)
    return inter / total


def shapes_disagree(left_n: int, left_shapes: Counter, right_n: int, right_shapes: Counter) -> bool:
    """True when both bodies are large and neither Jaccard nor retention agrees.

    A later year of the same type often adds methods, which drives Jaccard
    under 0.20 even though the earlier shapes are still there. That retention
    is not a false share. Bodies disagree only when the overlap itself is below
    0.20 as well.
    """
    if left_n < MIN_METHODS or right_n < MIN_METHODS:
        return False
    if jaccard(left_shapes, right_shapes) >= JACCARD_MIN:
        return False
    if sum(left_shapes.values()) <= sum(right_shapes.values()):
        retained = containment(left_shapes, right_shapes)
    else:
        retained = containment(right_shapes, left_shapes)
    return retained < JACCARD_MIN


def extends_names(bare_line: str, name_end: int) -> list[str]:
    """Superclass names from a class header. The extends regex must be greedy.

    A non-greedy ``extends`` group with an optional ``implements`` tail captures
    only the first character, so every later year looks like a new parent.
    """
    tail = bare_line[name_end:]
    tail = tail.split("{", 1)[0]
    match = re.search(r"\bextends\s+(.+?)(?:\s+implements\b|$)", tail)
    if not match:
        return []
    return simple_super(match.group(1))


def simple_super(extends_text: str | None) -> list[str]:
    if not extends_text:
        return []
    text = extends_text.strip()
    text = re.sub(r"\{.*", "", text).strip()
    # Drop implements if the regex was too greedy.
    text = re.split(r"\bimplements\b", text)[0]
    parts = []
    depth = 0
    cur: list[str] = []
    for ch in text:
        if ch == "<":
            depth += 1
        elif ch == ">":
            depth = max(0, depth - 1)
        elif ch == "," and depth == 0:
            parts.append("".join(cur))
            cur = []
            continue
        cur.append(ch)
    if cur:
        parts.append("".join(cur))
    names = []
    for part in parts:
        part = re.sub(r"<.*", "", part).strip()
        part = part.split()[-1] if part else ""
        if part:
            names.append(part)
    return names


def game_supers(extends_names: list[str], imports: dict[str, str]) -> str | None:
    """None when there is no game superclass. java.* / javax.* are ignored."""
    kept: list[str] = []
    for name in extends_names:
        if name.startswith("java.") or name.startswith("javax."):
            continue
        simple = name.split(".")[-1]
        if "." in name:
            kept.append(simple)
            continue
        full = imports.get(simple)
        if full and (full.startswith("java.") or full.startswith("javax.")):
            continue
        if simple in JAVA_LANG:
            continue
        kept.append(simple)
    if not kept:
        return None
    return "+".join(kept)


# Superclass names are not subject to the 8-character floor. ``Entity`` is a
# real parent. Synthetic parents are not evidence of a different type.
SYNTH_SUPER_RE = re.compile(r"^(?:class\d+|DataBlock\d+|kind\d+|C[0-9a-fA-F]{6,}\d*)$")


def super_kind(name: str | None) -> str:
    if not name:
        return "missing"
    if SYNTH_SUPER_RE.fullmatch(name) or not name[0].isupper():
        return "unknown"
    return "readable"


def anchor_supers(family: str, members: list[JClass]) -> set[str | None]:
    """Parents used by any anchor revision that declares this name.

    254's ``ModelSource`` and 317's ``Entity`` are the same parent renamed, so
    a later year that still uses either name is not a false share.
    """
    revs = set(ANCHOR_ORDER[family])
    return {item.game_super for item in members if item.rev in revs}


def supers_conflict(member_super: str | None, accepted: set[str | None]) -> bool:
    """True when this parent cannot be one of the anchor parents.

    A synthetic parent is inconclusive when an anchor has a real parent: the
    later client often never got that readable name. Missing still differs
    from a present parent. A readable parent that no anchor uses is a mismatch.
    """
    if not accepted:
        return False
    if member_super in accepted:
        return False
    kind = super_kind(member_super)
    accepted_kinds = {super_kind(name) for name in accepted}
    if kind == "unknown":
        return accepted_kinds == {"missing"}
    if kind == "missing":
        return any(name is not None for name in accepted)
    return True


@dataclass
class JClass:
    family: str
    rev: int
    path: Path
    rel: str
    pkg: str
    simple: str
    kind: str
    game_super: str | None
    raw_super: str | None
    method_count: int
    shapes: Counter
    strings: list[str]
    obf_name: str | None
    line: int


@dataclass
class Rename:
    family: str
    rev: int
    old: str
    new: str
    reason: str
    rel: str
    pkg: str
    path: Path
    source: str
    compile_result: str = ""


def java_root_for(folder: Path) -> Path | None:
    candidate = folder / "src" / "src" / "main" / "java"
    if candidate.is_dir():
        return candidate
    return None


def discover() -> list[tuple[str, int, Path, Path]]:
    found = []
    for family in FAMILIES:
        base = BUILDS / family
        if not base.is_dir():
            continue
        for folder in sorted(base.glob("*-*")):
            if not folder.is_dir():
                continue
            rev_text = folder.name.split("-", 1)[0]
            if not rev_text.isdigit():
                continue
            rev = int(rev_text)
            if (family, rev) in SKIP_REV:
                continue
            root = java_root_for(folder)
            if root is None:
                continue
            found.append((family, rev, folder, root))
    return found


def parse_java(family: str, rev: int, path: Path, java_root: Path) -> list[JClass]:
    raw = path.read_bytes()
    text = raw.decode("latin-1")
    imports: dict[str, str] = {}
    for line in text.splitlines():
        match = IMPORT_RE.match(line.strip())
        if not match or match.group(2):
            continue
        full = match.group(1)
        imports[full.split(".")[-1]] = full
    pkg = ""
    for line in text.splitlines():
        pkg_match = PKG_RE.match(line.strip())
        if pkg_match:
            pkg = pkg_match.group(1)
            break
    masked, string_list = strip_code(text)
    # Map each string body back by walking the original in order. strip_code
    # already returns them. Assign strings to classes by approximate offset:
    # re-scan masked lines with a parallel string cursor is awkward, so collect
    # strings per class by scanning the original with the same state machine
    # while tracking brace depth on the masked text.
    classes: list[JClass] = []
    lines = text.splitlines()
    masked_lines = masked.splitlines()
    # Keep line counts aligned if the mask dropped nothing (it preserves newlines).
    if len(masked_lines) != len(lines):
        masked_lines = masked.splitlines()
        if len(masked_lines) < len(lines):
            masked_lines += [""] * (len(lines) - len(masked_lines))
        else:
            masked_lines = masked_lines[:len(lines)]

    depth = 0
    stack: list[tuple[JClass, int, list[str]]] = []
    current_method_depth: int | None = None
    pending_obf: str | None = None

    def current() -> JClass | None:
        return stack[-1][0] if stack else None

    for lineno, line in enumerate(lines, start=1):
        bare = masked_lines[lineno - 1] if lineno - 1 < len(masked_lines) else line
        # Strings on this original line, ignoring ones inside comments.
        line_strings = strings_on_line(line)
        obf_match = OBF_RE.search(line)
        if obf_match and "(" not in obf_match.group(1):
            pending_obf = obf_match.group(1).strip()

        jc = current()
        header = None
        if current_method_depth is None:
            header = CLASS_RE.search(bare)
        if (
            header
            and "{" in bare
            and not bare.strip().endswith(";")
            and (jc is None or depth >= stack[-1][1])
        ):
            simple = header.group(2)
            extends = extends_names(bare, header.end(2))
            game = game_supers(extends, imports)
            raw_super = extends[0].split(".")[-1] if extends else None
            if raw_super and "<" in raw_super:
                raw_super = raw_super.split("<", 1)[0]
            born = JClass(
                family=family,
                rev=rev,
                path=path,
                rel=path.relative_to(java_root).as_posix(),
                pkg=pkg,
                simple=simple,
                kind=header.group(1),
                game_super=game,
                raw_super=raw_super,
                method_count=0,
                shapes=Counter(),
                strings=[],
                obf_name=pending_obf if pending_obf and legal_ident(pending_obf) else None,
                line=lineno,
            )
            pending_obf = None
            body_depth = depth + 1
            own_strings: list[str] = []
            stack.append((born, body_depth, own_strings))
            classes.append(born)
            # Strings on the header line belong to this class.
            own_strings.extend(line_strings)
            depth += bare.count("{") - bare.count("}")
            continue

        if jc is not None and current_method_depth is None and depth == stack[-1][1]:
            sig = SIG_RE.match(bare)
            if sig and sig.group("name") not in SKIP_WORDS:
                name = sig.group("name")
                if name != jc.simple and not name[:1].isupper():
                    ret = sig.group("ret") or "void"
                    ret = re.sub(r"\s+", "", ret)
                    params = split_params(sig.group("params") or "")
                    formed = shape_of(ret, params)
                    jc.shapes[formed] += 1
                    jc.method_count += 1
                    pending_obf = None
                    depth += bare.count("{") - bare.count("}")
                    if sig.group("end") == "{":
                        current_method_depth = depth
                    stack[-1][2].extend(line_strings)
                    continue
            # Class-body strings (field initializers).
            stack[-1][2].extend(line_strings)
        elif jc is not None and current_method_depth is not None:
            stack[-1][2].extend(line_strings)

        opens = bare.count("{")
        closes = bare.count("}")
        depth += opens - closes
        if depth < 0:
            depth = 0
        if current_method_depth is not None and depth < current_method_depth:
            current_method_depth = None
        while stack and depth < stack[-1][1]:
            finished, _body, own = stack.pop()
            finished.strings = own
            current_method_depth = None
    while stack:
        finished, _body, own = stack.pop()
        finished.strings = own
    return classes


def strings_on_line(line: str) -> list[str]:
    """String literal bodies on one line, skipping comments."""
    found: list[str] = []
    i = 0
    n = len(line)
    while i < n:
        ch = line[i]
        nxt = line[i + 1] if i + 1 < n else ""
        if ch == "/" and nxt == "/":
            break
        if ch == "/" and nxt == "*":
            end = line.find("*/", i + 2)
            if end < 0:
                break
            i = end + 2
            continue
        if ch == '"':
            j = i + 1
            while j < n:
                if line[j] == "\\":
                    j += 2
                    continue
                if line[j] == '"':
                    j += 1
                    break
                j += 1
            body = line[i + 1:j - 1] if j > i + 1 and j <= n and line[j - 1] == '"' else line[i + 1:j]
            found.append(body)
            i = j
            continue
        if ch == "'" and i + 1 < n:
            j = i + 1
            if j < n and line[j] == "\\":
                j += 2
            elif j < n:
                j += 1
            if j < n and line[j] == "'":
                j += 1
            i = j
            continue
        i += 1
    return found


def load_all(rows: list[tuple[str, int, Path, Path]]) -> dict[tuple[str, int], list[JClass]]:
    out: dict[tuple[str, int], list[JClass]] = {}
    for family, rev, _folder, java_root in rows:
        classes: list[JClass] = []
        files = [path for path in java_root.rglob("*.java") if "build" not in path.parts]
        for path in files:
            try:
                classes.extend(parse_java(family, rev, path, java_root))
            except Exception as exc:  # noqa: BLE001 — keep going and report the file
                log(f"parse error {family} {rev} {path}: {exc}")
        out[(family, rev)] = classes
        log(f"parsed {family} {rev}: {len(classes)} classes, {len(files)} files")
    return out


def fernflower_simple(folder: Path, rel: str) -> str | None:
    path = folder / "fernflower" / "extracted" / rel
    if not path.is_file():
        return None
    text = path.read_bytes().decode("latin-1")
    masked, _strings = strip_code(text)
    match = CLASS_RE.search(masked)
    if not match:
        return None
    return match.group(2)


def pick_anchor(family: str, members: list[JClass]) -> JClass | None:
    by_rev: dict[int, list[JClass]] = defaultdict(list)
    for member in members:
        by_rev[member.rev].append(member)
    for rev in ANCHOR_ORDER[family]:
        group = by_rev.get(rev)
        if not group:
            continue
        group = sorted(group, key=lambda item: (item.rel, item.line))
        return group[0]
    return None


def cluster_key(member: JClass) -> str | None:
    """Readable parents stay distinct. Synthetic parents share one bucket.

    A ``C``+hex superclass is a different string every year even when it is the
    same type, so it must not split an otherwise agreeing cluster by itself.
    """
    if super_kind(member.game_super) == "unknown":
        return "?"
    return member.game_super


def cluster_members(members: list[JClass]) -> list[list[JClass]]:
    """Group by game superclass, then split shaped classes that disagree."""
    by_super: dict[str | None, list[JClass]] = defaultdict(list)
    for member in members:
        by_super[cluster_key(member)].append(member)
    clusters: list[list[JClass]] = []
    for group in by_super.values():
        shaped = [item for item in group if item.method_count >= MIN_METHODS]
        small = [item for item in group if item.method_count < MIN_METHODS]
        shaped_sorted = sorted(shaped, key=lambda item: (-item.method_count, item.rev, item.rel))
        subs: list[list[JClass]] = []
        for member in shaped_sorted:
            placed = False
            order = sorted(range(len(subs)), key=lambda index: len(subs[index]), reverse=True)
            for index in order:
                cluster = subs[index]
                if all(
                    not shapes_disagree(member.method_count, member.shapes, other.method_count, other.shapes)
                    for other in cluster
                ):
                    cluster.append(member)
                    placed = True
                    break
            if not placed:
                subs.append([member])
        if not subs:
            if small:
                clusters.append(small)
            continue
        if len(subs) == 1:
            subs[0].extend(small)
        elif small:
            subs.append(small)
        clusters.extend(subs)
    return clusters


def reason_join(parts: list[str]) -> str:
    ordered = []
    for part in ("superclass mismatch", "body mismatch", "sentence name"):
        if part in parts and part not in ordered:
            ordered.append(part)
    return ", ".join(ordered)


def classify(family: str, members: list[JClass]) -> list[tuple[JClass, str]]:
    """Return (class, reason) for classes that must lose this simple name."""
    editable = [item for item in members if item.rev not in NEVER_EDIT[family]]
    anchor = pick_anchor(family, members)
    false_rows: list[tuple[JClass, str]] = []
    if anchor is not None:
        accepted = anchor_supers(family, members)
        anchor_decls = [item for item in members if item.rev in set(ANCHOR_ORDER[family])]
        if not anchor_decls:
            anchor_decls = [anchor]
        for member in editable:
            parts: list[str] = []
            super_bad = supers_conflict(member.game_super, accepted)
            body_bad = all(
                shapes_disagree(member.method_count, member.shapes, decl.method_count, decl.shapes)
                for decl in anchor_decls
            )
            if super_bad:
                parts.append("superclass mismatch")
            if body_bad:
                parts.append("body mismatch")
            # A class that still matches an anchor can mention its own name in
            # a diagnostic string. Sentence promotion only applies when the
            # shape does not agree, and when there is no anchor at all.
            if is_sentence(member.simple, member.strings) and (super_bad or body_bad):
                parts.append("sentence name")
            if parts:
                false_rows.append((member, reason_join(parts)))
        return false_rows

    clusters = cluster_members(members)
    if not clusters:
        return []
    sizes = sorted((len(cluster) for cluster in clusters), reverse=True)
    tie = len(sizes) >= 2 and sizes[0] == sizes[1]
    keeper_ids: set[int] = set()
    keeper_key: str | None = None
    if not tie:
        keeper = max(clusters, key=lambda cluster: (len(cluster), -min(item.rev for item in cluster)))
        keeper_ids = {id(item) for item in keeper}
        keeper_key = cluster_key(keeper[0])
    for member in editable:
        parts = []
        if id(member) not in keeper_ids:
            if tie or cluster_key(member) != keeper_key:
                if tie:
                    supers = {cluster_key(item) for cluster in clusters for item in cluster}
                    if len(supers) > 1:
                        parts.append("superclass mismatch")
                    else:
                        parts.append("body mismatch")
                else:
                    parts.append("superclass mismatch")
            else:
                parts.append("body mismatch")
        if is_sentence(member.simple, member.strings):
            parts.append("sentence name")
        if parts:
            false_rows.append((member, reason_join(parts)))
    return false_rows


def hash_name(family: str, rev: int, rel: str) -> str:
    digest = hashlib.sha256(f"{family}:{rev}:{rel}".encode()).hexdigest()[:8]
    return "C" + digest


def choose_name(
    member: JClass,
    anchor_name: str | None,
    folder: Path,
    rev_classes: set[str],
    rev_stems: set[str],
    engine_readable: set[str],
    planned_rev: set[str],
    planned_readable: set[str],
) -> tuple[str, str]:
    """Return (new name, source tag)."""

    def free(candidate: str) -> bool:
        if not legal_ident(candidate) or candidate == member.simple:
            return False
        if anchor_name and candidate == anchor_name:
            return False
        if candidate in rev_classes and candidate != member.simple:
            return False
        if candidate in rev_stems and candidate != member.path.stem:
            return False
        if candidate in planned_rev:
            return False
        if is_readable(candidate):
            if candidate in engine_readable or candidate in planned_readable:
                return False
            if is_sentence(candidate, member.strings):
                return False
            # A readable replacement would be re-checked on the next run.
            # Only accept it when it cannot join an existing name.
        return True

    obf = member.obf_name
    if obf and free(obf):
        return obf, "obfuscated-name"
    fern = fernflower_simple(folder, member.rel)
    if fern and fern != member.simple and free(fern):
        return fern, "fernflower"
    base = hash_name(member.family, member.rev, member.rel)
    candidate = base
    suffix = 0
    while not free(candidate):
        suffix += 1
        candidate = f"{base}{suffix}"
        if suffix > 50:
            break
    return candidate, "hash"


def plan(
    classes: dict[tuple[str, int], list[JClass]],
    folders: dict[tuple[str, int], Path],
) -> tuple[list[Rename], list[str]]:
    by_family: dict[str, list[JClass]] = defaultdict(list)
    for key, items in classes.items():
        by_family[key[0]].extend(items)

    renames: list[Rename] = []
    skips: list[str] = []
    for family, items in by_family.items():
        groups: dict[str, list[JClass]] = defaultdict(list)
        for item in items:
            if is_readable(item.simple):
                groups[item.simple].append(item)
        engine_readable = set(groups)
        planned_rev: dict[int, set[str]] = defaultdict(set)
        planned_readable: set[str] = set()
        reserved_classes: dict[int, set[str]] = {}
        reserved_stems: dict[int, set[str]] = {}
        for name, members in sorted(groups.items()):
            anchor = pick_anchor(family, members)
            anchor_name = anchor.simple if anchor else None
            false_rows = classify(family, members)
            if not false_rows:
                continue
            by_rev: dict[int, list[tuple[JClass, str]]] = defaultdict(list)
            for member, reason in false_rows:
                by_rev[member.rev].append((member, reason))
            decl_counts: dict[int, int] = Counter(item.rev for item in members)
            # Declaration count is every class of this simple name, not only false ones.
            for rev, rows in sorted(by_rev.items()):
                declared = decl_counts[rev]
                if declared > 1:
                    skips.append(
                        f"skip {family} {rev} {name}: declared {declared} times"
                    )
                    continue
                if rev not in reserved_classes:
                    reserved_classes[rev] = {item.simple for item in classes[(family, rev)]}
                    reserved_stems[rev] = {item.path.stem for item in classes[(family, rev)]}
                folder = folders[(family, rev)]
                for member, reason in rows:
                    new, source = choose_name(
                        member, anchor_name, folder, reserved_classes[rev], reserved_stems[rev],
                        engine_readable, planned_rev[rev], planned_readable,
                    )
                    planned_rev[rev].add(new)
                    reserved_classes[rev].add(new)
                    reserved_stems[rev].add(new)
                    if is_readable(new):
                        planned_readable.add(new)
                    renames.append(Rename(
                        family=family,
                        rev=rev,
                        old=name,
                        new=new,
                        reason=reason,
                        rel=member.rel,
                        pkg=member.pkg,
                        path=member.path,
                        source=source,
                    ))
    return renames, skips


TYPE_IMPORT_RE = re.compile(r"^import\s+(?!static\b)([\w.]+)(\.\*)?\s*;")


def file_type_scope(text: str) -> tuple[str, dict[str, str], set[str]]:
    """Package, single-type imports (simple -> full), and star-imported packages."""
    pkg = ""
    single: dict[str, str] = {}
    stars: set[str] = set()
    for line in text.splitlines():
        stripped = line.strip()
        if stripped.startswith("//"):
            continue
        pkg_match = PKG_RE.match(stripped)
        if pkg_match and not pkg:
            pkg = pkg_match.group(1)
            continue
        imported = TYPE_IMPORT_RE.match(stripped)
        if not imported:
            continue
        full = imported.group(1)
        if imported.group(2):
            stars.add(full)
        else:
            single[full.split(".")[-1]] = full
    return pkg, single, stars


def qualifier_before(text: str, start: int) -> str | None:
    """Dotted qualifier immediately before an identifier, or None if unqualified."""
    j = start - 1
    while j >= 0 and text[j] in " \t\r\n\f":
        j -= 1
    if j < 0 or text[j] != ".":
        return None
    parts: list[str] = []
    while j >= 0 and text[j] == ".":
        j -= 1
        while j >= 0 and text[j] in " \t\r\n\f":
            j -= 1
        end = j + 1
        while j >= 0 and (text[j].isalnum() or text[j] in "_$"):
            j -= 1
        if end == j + 1:
            return None
        parts.append(text[j + 1:end])
        while j >= 0 and text[j] in " \t\r\n\f":
            j -= 1
    parts.reverse()
    return ".".join(parts) if parts else None


def resolves_to_mapped(
    word: str,
    qualifier: str | None,
    file_pkg: str,
    single: dict[str, str],
    stars: set[str],
    packages: dict[str, str],
) -> bool:
    """True when this occurrence names the local class, not a JDK type of the same spelling."""
    target = packages.get(word)
    if target is None:
        return False
    if qualifier is not None:
        return qualifier == target
    imported = single.get(word)
    if imported is not None:
        expect = f"{target}.{word}" if target else word
        return imported == expect
    if file_pkg == target:
        return True
    return target in stars


def apply_idents(text: str, mapping: dict[str, str], packages: dict[str, str]) -> str:
    if not mapping:
        return text
    file_pkg, single, stars = file_type_scope(text)
    out: list[str] = []
    i = 0
    n = len(text)
    while i < n:
        ch = text[i]
        nxt = text[i + 1] if i + 1 < n else ""
        if ch == "/" and nxt == "/":
            end = text.find("\n", i)
            end = n if end < 0 else end
            out.append(text[i:end])
            i = end
            continue
        if ch == "/" and nxt == "*":
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
            out.append(text[i:end])
            i = end
            continue
        if ch == '"':
            j = i + 1
            while j < n:
                if text[j] == "\\":
                    j += 2
                    continue
                if text[j] == '"':
                    j += 1
                    break
                j += 1
            out.append(text[i:j])
            i = j
            continue
        if ch == "'":
            j = i + 1
            if j < n and text[j] == "\\":
                j += 2
            elif j < n:
                j += 1
            if j < n and text[j] == "'":
                j += 1
            out.append(text[i:j])
            i = j
            continue
        match = IDENT_RE.match(text, i)
        if match:
            word = match.group(0)
            if word in mapping and resolves_to_mapped(
                word, qualifier_before(text, i), file_pkg, single, stars, packages
            ):
                out.append(mapping[word])
            else:
                out.append(word)
            i = match.end()
            continue
        out.append(ch)
        i += 1
    return "".join(out)


def capture_preimage(family: str, rev: int, java_root: Path) -> Path:
    """Copy the Java root before writing.

    The first copy stays at backup/<family>-<rev>/. A later attempt keeps that
    pre-image and snapshots the live tree, so a failed compile cannot roll a
    revision back past a compile that already succeeded.
    """
    canonical = BACKUP / f"{family}-{rev}"
    if not canonical.exists():
        canonical.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(java_root, canonical)
        return canonical
    live = BACKUP / f"{family}-{rev}-live"
    if live.exists():
        shutil.rmtree(live)
    shutil.copytree(java_root, live)
    return live


def restore_root(backup: Path, java_root: Path) -> None:
    shutil.rmtree(java_root)
    shutil.copytree(backup, java_root)


def apply_revision(family: str, rev: int, java_root: Path, rows: list[Rename]) -> None:
    mapping = {row.old: row.new for row in rows}
    packages = {row.old: row.pkg for row in rows}
    file_renames: dict[Path, Path] = {}
    for row in rows:
        if row.path.stem != row.old:
            continue
        dest = row.path.with_name(row.new + ".java")
        if dest in file_renames.values() and file_renames.get(row.path) != dest:
            raise SystemExit(f"{family} {rev}: two classes want {dest.name}")
        file_renames[row.path] = dest
    files = [path for path in java_root.rglob("*.java") if "build" not in path.parts]
    final: dict[Path, str] = {}
    for path in files:
        text = apply_idents(path.read_bytes().decode("latin-1"), mapping, packages)
        dest = file_renames.get(path, path)
        if dest in final:
            raise SystemExit(f"{family} {rev}: duplicate destination {dest}")
        final[dest] = text
    staged: list[tuple[Path, Path]] = []
    for dest, text in final.items():
        tmp = dest.with_name(dest.name + ".revert-tmp")
        tmp.write_bytes(text.encode("latin-1"))
        staged.append((tmp, dest))
    for src in file_renames:
        if src.exists() and src not in final:
            src.unlink()
    for tmp, dest in staged:
        if dest.exists():
            dest.unlink()
        tmp.rename(dest)


def gradle_dir(folder: Path) -> Path:
    return folder / "src"


def compile_rev(family: str, rev: int, folder: Path) -> tuple[bool, str]:
    gradle = gradle_dir(folder)
    env = os.environ.copy()
    env["JAVA_HOME"] = str(JAVA_HOME)
    env["PATH"] = str(JAVA_HOME / "bin") + os.pathsep + env.get("PATH", "")
    home = Path(f"/tmp/gradle-{family}-{rev}")
    if home.is_dir():
        env["GRADLE_USER_HOME"] = str(home)
    log_path = WORK / f"compile-{family}-{rev}.log"
    WORK.mkdir(parents=True, exist_ok=True)
    cmd = ["./gradlew", "compileJava", "--offline", "--no-daemon"]
    with log_path.open("w") as handle:
        proc = subprocess.run(
            cmd,
            cwd=gradle,
            env=env,
            stdout=handle,
            stderr=subprocess.STDOUT,
            check=False,
        )
    text = log_path.read_text(errors="replace")
    if proc.returncode != 0 and "No cached version" in text:
        cmd = ["./gradlew", "compileJava", "--no-daemon"]
        with log_path.open("w") as handle:
            proc = subprocess.run(
                cmd,
                cwd=gradle,
                env=env,
                stdout=handle,
                stderr=subprocess.STDOUT,
                check=False,
            )
        text = log_path.read_text(errors="replace")
    tail = "\n".join(text.splitlines()[-20:])
    ok = proc.returncode == 0 and "BUILD SUCCESSFUL" in text
    return ok, tail


def prior_successes(current: list[Rename]) -> list[tuple[str, str, str, str, str, str]]:
    """Success rows from an earlier apply that this run does not repeat."""
    path = WORK / "report.md"
    if not path.is_file():
        return []
    seen = {(row.family, str(row.rev), row.old) for row in current}
    kept: list[tuple[str, str, str, str, str, str]] = []
    for line in path.read_text().splitlines():
        if not line.startswith("| ") or line.startswith("| ---") or line.startswith("| family"):
            continue
        cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
        if len(cells) < 6 or cells[5] != "success":
            continue
        key = (cells[0], cells[1], cells[2])
        if key in seen:
            continue
        seen.add(key)
        kept.append((cells[0], cells[1], cells[2], cells[3], cells[4], cells[5]))
    return kept


def write_report(renames: list[Rename], skips: list[str], restored: list[str]) -> None:
    WORK.mkdir(parents=True, exist_ok=True)
    earlier = prior_successes(renames)
    lines = ["# Revert false shares", ""]
    lines.append("| family | rev | old | new | reason | compile |")
    lines.append("| --- | --- | --- | --- | --- | --- |")
    for row in renames:
        lines.append(
            f"| {row.family} | {row.rev} | {row.old} | {row.new} | {row.reason} | {row.compile_result} |"
        )
    for family, rev, old, new, reason, result in earlier:
        lines.append(f"| {family} | {rev} | {old} | {new} | {reason} | {result} |")
    lines.append("")
    counts: dict[tuple[str, int], int] = Counter(
        (row.family, row.rev) for row in renames if row.compile_result == "success"
    )
    for family, rev, _old, _new, _reason, _result in earlier:
        counts[(family, int(rev))] += 1
    lines.append("## Counts kept")
    lines.append("")
    if not counts:
        lines.append("No rename was left in the tree.")
    else:
        for (family, rev), count in sorted(counts.items()):
            lines.append(f"- {family} {rev}: {count}")
    if skips:
        lines.append("")
        lines.append("## Skipped")
        lines.append("")
        for skip in skips:
            lines.append(f"- {skip}")
    if restored:
        lines.append("")
        lines.append("## Restored")
        lines.append("")
        for item in restored:
            lines.append(f"- {item}")
    (WORK / "report.md").write_text("\n".join(lines) + "\n")


def watch_lines(
    classes: dict[tuple[str, int], list[JClass]],
    renames: list[Rename],
) -> list[str]:
    planned = {(row.family, row.rev, row.old, row.rel): row for row in renames}
    lines = []
    for family in FAMILIES:
        items = [item for key, group in classes.items() if key[0] == family for item in group]
        for name in sorted(WATCH):
            members = [item for item in items if item.simple == name]
            if not members:
                continue
            anchor = pick_anchor(family, members)
            lines.append(f"{family} {name}:")
            for member in sorted(members, key=lambda item: (item.rev, item.rel)):
                row = planned.get((member.family, member.rev, member.simple, member.rel))
                jac = ""
                if anchor is not None and member is not anchor:
                    score = jaccard(member.shapes, anchor.shapes)
                    if sum(anchor.shapes.values()) <= sum(member.shapes.values()):
                        retained = containment(anchor.shapes, member.shapes)
                    else:
                        retained = containment(member.shapes, anchor.shapes)
                    jac = (
                        f" jaccard={score:.3f} retain={retained:.3f}"
                        f" methods={member.method_count}/{anchor.method_count}"
                    )
                else:
                    jac = f" methods={member.method_count}"
                super_name = member.game_super or "-"
                if row:
                    lines.append(
                        f"  {member.rev} RENAME {row.new} [{row.reason}] super={super_name}{jac} {member.rel}"
                    )
                else:
                    why = "anchor" if member.rev in NEVER_EDIT[family] else "keep"
                    lines.append(
                        f"  {member.rev} {why} super={super_name}{jac} {member.rel}"
                    )
    return lines


def main() -> int:
    parser = argparse.ArgumentParser(description="Revert false shared class names")
    parser.add_argument("--apply", action="store_true", help="rewrite sources and compile")
    args = parser.parse_args()

    rows = discover()
    folders = {(family, rev): folder for family, rev, folder, _root in rows}
    roots = {(family, rev): root for family, rev, _folder, root in rows}
    classes = load_all(rows)
    renames, skips = plan(classes, folders)

    by_rev: dict[tuple[str, int], list[Rename]] = defaultdict(list)
    for row in renames:
        by_rev[(row.family, row.rev)].append(row)

    log(f"planned renames: {len(renames)} across {len(by_rev)} revisions")
    for key in sorted(by_rev):
        log(f"  {key[0]} {key[1]}: {len(by_rev[key])}")
    for skip in skips:
        log(skip)
    for row in renames:
        log(f"{row.family} {row.rev} {row.old} -> {row.new} [{row.reason}] {row.rel} ({row.source})")
    log("--- watched ---")
    for line in watch_lines(classes, renames):
        log(line)

    if not args.apply:
        return 0
    if not renames:
        log("no renames to apply")
        return 0

    restored: list[str] = []
    for (family, rev), group in sorted(by_rev.items()):
        java_root = roots[(family, rev)]
        folder = folders[(family, rev)]
        log(f"apply {family} {rev}: {len(group)} classes")
        backup = capture_preimage(family, rev, java_root)
        try:
            apply_revision(family, rev, java_root, group)
        except Exception as exc:  # noqa: BLE001
            log(f"apply failed {family} {rev}: {exc}; restoring")
            restore_root(backup, java_root)
            for row in group:
                row.compile_result = "apply failed, restored"
            restored.append(f"{family} {rev}: apply failed ({exc})")
            continue
        ok, tail = compile_rev(family, rev, folder)
        if ok:
            log(f"compile {family} {rev}: SUCCESS")
            for row in group:
                row.compile_result = "success"
            if backup.name.endswith("-live"):
                shutil.rmtree(backup, ignore_errors=True)
            continue
        log(f"compile {family} {rev}: FAILED, restoring")
        log(tail)
        restore_root(backup, java_root)
        for row in group:
            row.compile_result = "compile failed, restored"
        restored.append(f"{family} {rev}: compile failed, sources restored")

    write_report(renames, skips, restored)
    kept = sum(1 for row in renames if row.compile_result == "success")
    log(f"applied and compiled: {kept}; restored revisions: {len(restored)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
