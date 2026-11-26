#!/usr/bin/env python3
"""Give RSC 115, 159, and 177 one vocabulary, without touching literals.

177 is the anchor. Obfuscated classes are renamed first, then methods, then
fields. A member is shared across years only when a unique initializer, a
unique string, or a unique field type says it is the same member. Replacements
never run inside string literals, character literals, or comments, and two
fields in one class never receive the same name.

Do not pass --apply unless the per-phase plan has been reviewed. A global
token replace is intentionally not what this script does: each old name maps
to one new name, and that name is checked against every class that declares it.
"""
from __future__ import annotations

import os
import re
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import jdk  # noqa: E402

BUILDS = ROOT / "builds" / "rsc"
JAVA_HOME = str(jdk.java_home())
REVS = (115, 159, 177)

OBF_RE = re.compile(
    r"^(?:method\d+|field\d+|class\d+|kind\d+|Cls[0-9a-fA-F]+|fn[0-9a-f]{8}|(?:a|an)[A-Z]\w*\d+)$"
)
KEYWORDS = {
    "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
    "class", "const", "continue", "default", "do", "double", "else", "enum",
    "extends", "final", "finally", "float", "for", "goto", "if", "implements",
    "import", "instanceof", "int", "interface", "long", "native", "new",
    "package", "private", "protected", "public", "return", "short", "static",
    "strictfp", "super", "switch", "synchronized", "this", "throw", "throws",
    "transient", "try", "void", "volatile", "while", "true", "false", "null",
    "var", "yield", "record",
}
STOP = {
    "a", "an", "the", "to", "of", "for", "and", "or", "in", "on", "at", "by",
    "with", "from", "your", "you", "please", "this", "that", "is", "are", "was",
    "be", "it", "as", "if", "do", "not", "no", "into", "onto", "than", "then",
    "all", "any", "our", "we", "can", "will", "just", "have", "has", "had",
}
PRIM = {"int", "long", "boolean", "byte", "short", "char", "float", "double", "void"}
TRIVIAL_INIT = {"0", "1", "-1", "2", "-2", "true", "false", "null", '""', "0L", "1L", "-1L", "0l"}
WORD_RE = re.compile(r"[A-Za-z][A-Za-z']*")
CLASS_HEAD = re.compile(
    r"\b(class|interface|enum)\s+(\w+)\s*(?:extends\s+([\w.]+))?(?:\s+implements\s+([^{]+))?\s*\{"
)
MOD = r"(?:public|protected|private|static|final|volatile|transient|synchronized|native|abstract|strictfp)"
FIELD_RE = re.compile(
    rf"^[ \t]*((?:{MOD})\s+)+([\w.$]+(?:\s*\[\s*\])*)\s+(\w+)\s*(=|;)",
    re.M,
)
METH_RE = re.compile(
    rf"^[ \t]*(?:(?:{MOD})\s+)*(?:[\w.$]+(?:\s*\[\s*\])*\s+)+(\w+)\s*\(([^)]*)\)\s*(?:throws\s+[\w.,\s]+)?\{{",
    re.M,
)
IDENT_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")


@dataclass
class Member:
    name: str
    kind: str  # field or method
    type_name: str
    init: str
    strings: set[str] = field(default_factory=set)
    params: str = ""


@dataclass
class JClass:
    rev: int
    path: Path
    package: str
    simple: str
    extends: str
    text: str
    strings: set[str]
    fields: list[Member]
    methods: list[Member]
    int_literals: set[int]


def src_root(rev: int) -> Path:
    return next(BUILDS.glob(f"{rev}-*")) / "src"


def java_root(rev: int) -> Path:
    return src_root(rev) / "src" / "main" / "java"


def java_files(rev: int) -> list[Path]:
    return sorted(p for p in java_root(rev).rglob("*.java"))


def is_obf(name: str) -> bool:
    return bool(OBF_RE.match(name))


def is_obf_class(name: str) -> bool:
    return bool(re.fullmatch(r"class\d+", name)) or len(name) <= 2 or name == "PANICRANDOMISEDBLOCK"


# --- literal-aware scan -------------------------------------------------------

def _skip_string(text: str, i: int) -> int:
    j = i + 1
    n = len(text)
    while j < n:
        if text[j] == "\\":
            j += 2
            continue
        if text[j] == '"':
            return j + 1
        if text[j] == "\n":
            return j
        j += 1
    return n


def _skip_char(text: str, i: int) -> int:
    j = i + 1
    n = len(text)
    if j >= n:
        return n
    if text[j] == "\\":
        j += 1
        if j < n and text[j] == "u":
            while j < n and text[j] == "u":
                j += 1
            j = min(n, j + 4)
        else:
            j = min(n, j + 1)
    else:
        j += 1
    if j < n and text[j] == "'":
        j += 1
    return j


def segments(text: str) -> list[tuple[bool, int, int]]:
    """Split into (is_code, start, end) spans. Literals and comments are not code."""
    n = len(text)
    out: list[tuple[bool, int, int]] = []
    i = 0
    code_start = 0
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            if i > code_start:
                out.append((True, code_start, i))
            j = text.find("\n", i)
            j = n if j < 0 else j
            out.append((False, i, j))
            i = j
            code_start = i
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "*":
            if i > code_start:
                out.append((True, code_start, i))
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append((False, i, j))
            i = j
            code_start = i
            continue
        if c == '"':
            if i > code_start:
                out.append((True, code_start, i))
            j = _skip_string(text, i)
            out.append((False, i, j))
            i = j
            code_start = i
            continue
        if c == "'":
            if i > code_start:
                out.append((True, code_start, i))
            j = _skip_char(text, i)
            out.append((False, i, j))
            i = j
            code_start = i
            continue
        i += 1
    if code_start < n:
        out.append((True, code_start, n))
    return out


def mask_noncode(text: str) -> str:
    chars = list(text)
    for is_code, a, b in segments(text):
        if is_code:
            continue
        for i in range(a, b):
            if chars[i] != "\n":
                chars[i] = " "
    return "".join(chars)


def string_literals(text: str) -> list[tuple[int, str]]:
    found = []
    for is_code, a, b in segments(text):
        if is_code or a >= len(text) or text[a] != '"':
            continue
        raw = text[a + 1 : b - 1] if b > a + 1 and text[b - 1] == '"' else text[a + 1 : b]
        found.append((a, raw))
    return found


def content_string(value: str) -> bool:
    if len(value) < 4 or len(value) > 220:
        return False
    if not re.search(r"[A-Za-z]", value):
        return False
    if "(" in value and " " not in value:
        return False
    if "/" in value and " " not in value and "://" not in value:
        return False
    if re.fullmatch(r"[\w$]+\.[\w$]+", value):
        return False
    return True


def replace_idents(text: str, mapping: dict[str, str]) -> str:
    if not mapping:
        return text
    parts: list[str] = []
    for is_code, a, b in segments(text):
        chunk = text[a:b]
        if not is_code:
            parts.append(chunk)
            continue
        def sub(match: re.Match) -> str:
            return mapping.get(match.group(0), match.group(0))
        parts.append(IDENT_RE.sub(sub, chunk))
    return "".join(parts)


def code_idents(text: str) -> set[str]:
    names: set[str] = set()
    for is_code, a, b in segments(text):
        if is_code:
            names.update(IDENT_RE.findall(text[a:b]))
    return names


def simple_idents(text: str) -> set[str]:
    """Identifiers that bind as simple names. `System.out` does not reserve `out`."""
    names: set[str] = set()
    for is_code, a, b in segments(text):
        if not is_code:
            continue
        chunk = text[a:b]
        for match in IDENT_RE.finditer(chunk):
            i = match.start()
            if i > 0 and chunk[i - 1] == ".":
                continue
            names.add(match.group(0))
    return names


# --- parse --------------------------------------------------------------------

def _norm_ws(expr: str) -> str:
    return re.sub(r"\s+", "", expr.strip())


def _stable_init(expr: str) -> bool:
    norm = _norm_ws(expr)
    if not norm or norm in TRIVIAL_INIT or len(norm) > 500:
        return False
    stripped = re.sub(r'"(?:\\.|[^"\\])*"', '""', expr)
    idents = IDENT_RE.findall(stripped)
    for ident in idents:
        if ident in {"new", "true", "false", "null"} or ident in PRIM:
            continue
        if ident[0].isupper():
            continue
        return False
    return True


def _read_expr(masked: str, original: str, start: int) -> str:
    """Read an expression starting at start until a depth-0 semicolon."""
    n = len(masked)
    depth_paren = depth_brack = depth_brace = 0
    i = start
    while i < n:
        c = masked[i]
        if c == "(":
            depth_paren += 1
        elif c == ")":
            depth_paren -= 1
        elif c == "[":
            depth_brack += 1
        elif c == "]":
            depth_brack -= 1
        elif c == "{":
            depth_brace += 1
        elif c == "}":
            depth_brace -= 1
        elif c == ";" and depth_paren == depth_brack == depth_brace == 0:
            return original[start:i]
        i += 1
    return original[start:n]


def _brace_end(masked: str, open_idx: int) -> int:
    depth = 1
    i = open_idx + 1
    n = len(masked)
    while i < n:
        c = masked[i]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return n


def _depth_before(masked: str) -> list[int]:
    depth = []
    d = 0
    for ch in masked:
        depth.append(d)
        if ch == "{":
            d += 1
        elif ch == "}":
            d = max(0, d - 1)
    return depth


def parse_file(rev: int, path: Path) -> JClass | None:
    text = path.read_text(errors="ignore")
    masked = mask_noncode(text)
    pkg = ""
    pm = re.search(r"^\s*package\s+([\w.]+)\s*;", masked, re.M)
    if pm:
        pkg = pm.group(1)
    cm = CLASS_HEAD.search(masked)
    if not cm:
        return None
    simple = cm.group(2)
    extends = (cm.group(3) or "").split(".")[-1]
    strings = {s for _, s in string_literals(text) if content_string(s)}
    depth = _depth_before(masked)
    literals = string_literals(text)

    fields: list[Member] = []
    for fm in FIELD_RE.finditer(masked):
        if depth[fm.start()] != 1:
            continue
        name = fm.group(3)
        if name in KEYWORDS:
            continue
        type_name = re.sub(r"\s+", "", fm.group(2))
        init = ""
        if fm.group(4) == "=":
            init = _read_expr(masked, text, fm.end()).strip()
        fields.append(Member(name, "field", type_name, init))

    methods: list[Member] = []
    for mm in METH_RE.finditer(masked):
        if depth[mm.start()] != 1:
            continue
        name = mm.group(1)
        if name in KEYWORDS or name == simple:
            continue
        open_idx = mm.end() - 1
        end = _brace_end(masked, open_idx)
        body_strings = {s for pos, s in literals if open_idx < pos < end and content_string(s)}
        methods.append(Member(name, "method", "", "", body_strings, re.sub(r"\s+", "", mm.group(2))))

    # A single stable assignment is an initializer when the declaration has none.
    # One scan, so MudClient's hundreds of fields do not rescan the file.
    pending = {member.name: member for member in fields if not member.init}
    found_assign: dict[str, list[str]] = defaultdict(list)
    if pending:
        for match in IDENT_RE.finditer(masked):
            name = match.group(0)
            member = pending.get(name)
            if member is None:
                continue
            j = match.end()
            while j < len(masked) and masked[j] in " \t":
                j += 1
            if j < len(masked) and masked[j] == "=" and (j + 1 >= len(masked) or masked[j + 1] != "="):
                found_assign[name].append(_read_expr(masked, text, j + 1).strip())
        for name, exprs in found_assign.items():
            if len(exprs) == 1:
                pending[name].init = exprs[0]

    int_literals: set[int] = set()
    for m in re.finditer(r"(?<![\w.])(\d{1,6})(?![\w])", masked):
        int_literals.add(int(m.group(1)))
    return JClass(rev, path, pkg, simple, extends, text, strings, fields, methods, int_literals)


def parse_rev(rev: int) -> list[JClass]:
    out = []
    for path in java_files(rev):
        if path.name == "ObfuscatedName.java":
            continue
        jc = parse_file(rev, path)
        if jc:
            out.append(jc)
    return out


# --- names --------------------------------------------------------------------

def slug(text: str, kind: str) -> str:
    words = []
    short = []
    for raw in WORD_RE.findall(text):
        w = raw.replace("'", "")
        if w.lower() in STOP:
            continue
        if len(w) > 20:
            continue
        if len(w) >= 3:
            words.append(w)
        elif len(w) == 2:
            short.append(w)
        if len(words) == 4:
            break
    if not words:
        words = short[:3]
    if not words:
        return ""
    if kind == "class":
        name = "".join(w[:1].upper() + w[1:] for w in words)
    else:
        head = words[0][:1].lower() + words[0][1:]
        name = head + "".join(w[:1].upper() + w[1:] for w in words[1:])
    name = re.sub(r"[^A-Za-z0-9_]", "", name)[:48]
    if not name or name[0].isdigit() or name in KEYWORDS or is_obf(name):
        return ""
    return name


def type_slug(type_name: str) -> str:
    arrays = 0
    base = type_name
    while base.endswith("[]"):
        arrays += 1
        base = base[:-2]
    base = base.split(".")[-1]
    special = {
        "int": "number",
        "long": "longValue",
        "boolean": "flag",
        "byte": "byteValue",
        "short": "shortValue",
        "char": "charValue",
        "float": "floatValue",
        "double": "doubleValue",
        "String": "text",
        "BigInteger": "bigInteger",
    }
    name = special.get(base, base[:1].lower() + base[1:] if base else "value")
    if arrays:
        name += "Array" * arrays
    if not name or name in KEYWORDS or is_obf(name):
        name = "value"
    return name


def digits_of(name: str) -> str:
    m = re.search(r"(\d+)$", name)
    if m:
        return m.group(1)
    m = re.search(r"([0-9a-f]{4})$", name)
    return m.group(1) if m else "0"


def stable_tail(name: str) -> str:
    """Stable suffix so an fn hash keeps one name in every year."""
    if name.startswith("fn") and len(name) > 8:
        return name[-6:]
    return digits_of(name)


def free_name(base: str, banned: set[str]) -> str:
    if not base or base[0].isdigit():
        base = "value"
    base = re.sub(r"[^A-Za-z0-9_]", "", base)[:48] or "value"
    name = base
    n = 2
    while name in banned or name in KEYWORDS or is_obf(name) or not re.match(r"^[A-Za-z_]", name):
        name = f"{base}{n}"
        n += 1
        if n > 8000:
            raise RuntimeError(f"could not uniquify {base}")
    return name


def class_should_rename(old: str, new: str) -> bool:
    if not new or old == new:
        return False
    if is_obf_class(old):
        return True
    if old == "Entity" and new == "Version":
        return True
    return False


# --- class plan ---------------------------------------------------------------

def structural_target(jc: JClass) -> str:
    blob = "\n".join(jc.strings)
    if "PANIC! RANDOMISED BLOCK!" in blob or "PANIC! RANDOMISED BLOCK!" in jc.text:
        return "BZip2"
    if {18002, 258, 23, 4096} <= jc.int_literals and "PANIC! RANDOMISED BLOCK!" not in jc.text:
        # The decompressor state, not the routine that prints the panic line.
        # class6 / c were the pre-rename spellings; the class is already BZipState.
        if is_obf_class(jc.simple) or jc.simple == "BZipState":
            return "BZipState"
    if jc.extends == "PixMap" and any(f.type_name == "MudClient" for f in jc.fields):
        return "MudPix"
    if jc.extends == "Frame":
        return "ViewBox"
    if jc.extends == "InputStream":
        return "PcmPlayer"
    static_ints = [f.init.strip() for f in jc.fields if f.type_name == "int" and f.init.strip().isdigit()]
    if (
        jc.fields
        and all(f.type_name == "int" for f in jc.fields)
        and not jc.methods
        and str(jc.rev) in static_ints
        and len(jc.fields) >= 4
    ):
        return "Version"
    norms = [_norm_ws(f.init) for f in jc.fields if f.init]
    if (
        not jc.extends
        and 8 <= len(jc.fields) <= 45
        and norms.count("newint[10]") >= 2
        and "newint[12]" in norms
    ):
        return "Entity"
    if (
        not jc.extends
        and not jc.methods
        and len(jc.fields) == 4
        and all(f.type_name == "int" for f in jc.fields)
    ):
        return "ScanLine"
    if (
        not jc.extends
        and not jc.methods
        and 8 <= len(jc.fields) <= 30
        and any(f.type_name == "boolean" for f in jc.fields)
        and any(_norm_ws(f.init) == "-1" for f in jc.fields)
        and "newint[10]" not in norms
    ):
        return "Polygon3D"
    return ""


def plan_classes(parsed: dict[int, list[JClass]]) -> dict[int, dict[str, str]]:
    """rev -> {old simple name: new simple name}."""
    out: dict[int, dict[str, str]] = {rev: {} for rev in REVS}
    used: set[tuple[int, str]] = set()

    by_target: dict[str, list[JClass]] = defaultdict(list)
    for rev in REVS:
        for jc in parsed[rev]:
            target = structural_target(jc)
            if target:
                by_target[target].append(jc)
    for target, group in by_target.items():
        seen_rev = set()
        for jc in group:
            if jc.rev in seen_rev:
                continue
            seen_rev.add(jc.rev)
            used.add((jc.rev, jc.simple))
            if class_should_rename(jc.simple, target):
                out[jc.rev][jc.simple] = target

    # ClientStream's obfuscated parent is Stream.
    for rev in REVS:
        stream_parent = ""
        for jc in parsed[rev]:
            if jc.simple == "ClientStream" and jc.extends and jc.extends not in {"Stream", "Object"}:
                stream_parent = jc.extends
        if stream_parent and class_should_rename(stream_parent, "Stream"):
            out[rev][stream_parent] = "Stream"
            used.add((rev, stream_parent))

    # Same simple name that nobody structurally moved stays linked.
    for rev in (115, 159):
        anchor_names = {jc.simple for jc in parsed[177]}
        for jc in parsed[rev]:
            if (rev, jc.simple) in used:
                continue
            if jc.simple in anchor_names and jc.simple not in out[rev]:
                used.add((rev, jc.simple))
                used.add((177, jc.simple))

    def df_for(classes: list[JClass]) -> dict[str, int]:
        counts: dict[str, int] = defaultdict(int)
        for jc in classes:
            for s in jc.strings:
                counts[s] += 1
        return counts

    def score(a: JClass, b: JClass, df: dict[str, int]) -> float:
        total = 0.0
        for s in a.strings & b.strings:
            # Shared boilerplate such as the charset scores less than a rare line.
            total += 1.0 / max(1, df.get(s, 1))
        return total

    paired_177 = {name for rev, name in used if rev == 177}
    anchor_left = [jc for jc in parsed[177] if jc.simple not in paired_177 and not is_obf_class(jc.simple) and not is_obf(jc.simple)]
    pool = anchor_left + [jc for rev in (159, 115) for jc in parsed[rev] if (rev, jc.simple) not in used]
    df = df_for(pool + [jc for jc in parsed[177]])

    for rev in (159, 115):
        left = [jc for jc in parsed[rev] if (rev, jc.simple) not in used and is_obf_class(jc.simple)]
        pairs = []
        for other in left:
            ranked = []
            for anchor in anchor_left:
                sc = score(anchor, other, df)
                if sc > 0:
                    ranked.append((sc, anchor))
            ranked.sort(key=lambda item: -item[0])
            if not ranked:
                print(f"  {rev} {other.simple}: no string overlap")
                continue
            best_sc, best = ranked[0]
            second = ranked[1][0] if len(ranked) > 1 else 0.0
            print(f"  {rev} {other.simple} ~ {best.simple} score {best_sc:.2f} second {second:.2f}")
            long_hit = any(len(s) >= 12 for s in other.strings & best.strings)
            if long_hit and best_sc >= 0.4 and (second == 0 or best_sc >= second * 1.35):
                pairs.append((best_sc, other, best))
        pairs.sort(key=lambda item: -item[0])
        taken_anchor: set[str] = set()
        for _sc, other, anchor in pairs:
            if anchor.simple in taken_anchor or (rev, other.simple) in used:
                continue
            if not class_should_rename(other.simple, anchor.simple):
                continue
            out[rev][other.simple] = anchor.simple
            used.add((rev, other.simple))
            taken_anchor.add(anchor.simple)

    # Classes such as Object3D and World3D share no English, but they share a
    # set of integer literals that occur in only one class that year.
    rare: dict[int, dict[int, str]] = {rev: {} for rev in REVS}
    seen_int: dict[int, dict[int, str]] = {rev: {} for rev in REVS}
    dup_int: dict[int, set[int]] = {rev: set() for rev in REVS}
    for rev, classes in parsed.items():
        for jc in classes:
            for number in jc.int_literals:
                if number < 40:
                    continue
                prev = seen_int[rev].get(number)
                if prev is None:
                    seen_int[rev][number] = jc.simple
                elif prev != jc.simple:
                    dup_int[rev].add(number)
        for number, simple in seen_int[rev].items():
            if number not in dup_int[rev]:
                rare[rev].setdefault(number, simple)
    by_simple = {rev: {jc.simple: jc for jc in classes} for rev, classes in parsed.items()}
    anchor_rare: dict[str, set[int]] = {}
    for number, simple in rare[177].items():
        anchor_rare.setdefault(simple, set()).add(number)
    for rev in (159, 115):
        owner: dict[str, set[int]] = {}
        for number, simple in rare[rev].items():
            owner.setdefault(simple, set()).add(number)
        pairs = []
        for simple, numbers in owner.items():
            if (rev, simple) in used or not is_obf_class(simple):
                continue
            ranked = []
            for anchor, anums in anchor_rare.items():
                if is_obf_class(anchor) or is_obf(anchor):
                    continue
                overlap = len(numbers & anums)
                if overlap:
                    ranked.append((overlap, anchor))
            ranked.sort(key=lambda item: -item[0])
            if not ranked:
                continue
            best_n, best = ranked[0]
            second = ranked[1][0] if len(ranked) > 1 else 0
            print(f"  {rev} {simple} ints ~ {best} overlap {best_n} second {second}")
            if best_n >= 3 and best_n > second:
                pairs.append((best_n, simple, best))
        pairs.sort(key=lambda item: -item[0])
        taken_anchor = set()
        for _n, simple, anchor in pairs:
            if anchor in taken_anchor or (rev, simple) in used:
                continue
            if not class_should_rename(simple, anchor):
                continue
            out[rev][simple] = anchor
            used.add((rev, simple))
            taken_anchor.add(anchor)

    # Obfuscated classes with no anchor partner: one PascalCase name per matched set.
    leftovers = []
    for rev in REVS:
        for jc in parsed[rev]:
            if (rev, jc.simple) in used:
                continue
            if is_obf_class(jc.simple) or is_obf(jc.simple):
                leftovers.append(jc)
    # Cluster leftovers across years by the same string score.
    clusters: list[list[JClass]] = []
    for jc in leftovers:
        placed = False
        for cluster in clusters:
            if any(other.rev == jc.rev for other in cluster):
                continue
            if score(jc, cluster[0], df_for(leftovers)) >= 1.4:
                cluster.append(jc)
                placed = True
                break
        if not placed:
            clusters.append([jc])
    taken_names = {jc.simple for rev in REVS for jc in parsed[rev]}
    taken_names.update(name for mapping in out.values() for name in mapping.values())
    for cluster in clusters:
        longest = ""
        for jc in cluster:
            for s in sorted(jc.strings, key=len, reverse=True):
                # The shared glyph charset is not a class name.
                letters = sum(ch.isalpha() for ch in s)
                if letters > 40 and " " not in s:
                    continue
                if len(s) > len(longest):
                    longest = s
        base = slug(longest, "class") if longest else ""
        if not base:
            base = "ClassicType"
        name = free_name(base, taken_names)
        taken_names.add(name)
        for jc in cluster:
            if class_should_rename(jc.simple, name):
                out[jc.rev][jc.simple] = name
    # Collision check inside one year.
    for rev, mapping in out.items():
        values = list(mapping.values())
        if len(values) != len(set(values)):
            raise SystemExit(f"class rename collision in {rev}: {mapping}")
        existing = {jc.simple for jc in parsed[rev]}
        for old, new in mapping.items():
            if new in existing and new not in mapping:
                raise SystemExit(f"{rev}: {old} -> {new} collides with an existing class")
            # A swap (Entity -> Version, class2 -> Entity) is fine: new is also an old key.
            if new in existing and new not in mapping and new != old:
                raise SystemExit(f"{rev}: refusing {old} -> {new}")
    return out


# --- member plan --------------------------------------------------------------

def effective_init(member: Member) -> str:
    if member.init and _stable_init(member.init):
        return _norm_ws(member.init)
    return ""


def plan_members(parsed: dict[int, list[JClass]], kind: str) -> dict[int, dict[str, str]]:
    """rev -> {old member name: new name}. One new name per old token."""
    by_rev_name: dict[int, dict[str, JClass]] = {}
    for rev, classes in parsed.items():
        by_rev_name[rev] = {}
        for jc in classes:
            by_rev_name[rev][jc.simple] = jc

    # Groups of (rev, class simple, member) that are the same logical member.
    parent: dict[tuple[int, str, str], tuple[int, str, str]] = {}

    def find(key: tuple[int, str, str]) -> tuple[int, str, str]:
        while parent.get(key, key) != key:
            parent[key] = parent.get(parent[key], parent[key])
            key = parent[key]
        return key

    group_of: dict[tuple[int, str, str], list[tuple[int, str, str]]] = {}

    def union(a: tuple[int, str, str], b: tuple[int, str, str]) -> None:
        aa, bb = find(a), find(b)
        if aa == bb:
            return
        ga = group_of.setdefault(aa, [aa])
        gb = group_of.setdefault(bb, [bb])
        # One class in one year cannot hold two members of the same group.
        seen: dict[tuple[int, str], str] = {}
        for rev, simple, name in ga + gb:
            prev = seen.get((rev, simple))
            if prev is not None and prev != name:
                return
            seen[(rev, simple)] = name
        parent[bb] = aa
        group_of[aa] = ga + gb
        group_of.pop(bb, None)

    def members_of(jc: JClass) -> list[Member]:
        return jc.methods if kind == "method" else jc.fields

    shared_classes = set(by_rev_name[115]) & set(by_rev_name[159]) & set(by_rev_name[177])
    shared_classes |= set(by_rev_name[115]) & set(by_rev_name[177])
    shared_classes |= set(by_rev_name[159]) & set(by_rev_name[177])
    shared_classes |= set(by_rev_name[115]) & set(by_rev_name[159])

    def match_string(value: str) -> bool:
        if not content_string(value):
            return False
        letters = sum(ch.isalpha() for ch in value)
        if letters > 40 and " " not in value:
            return False
        return len(value) >= 8 or " " in value

    for simple in sorted(shared_classes):
        present = [rev for rev in REVS if simple in by_rev_name[rev]]
        if kind == "method":
            # The fnXXXXXXXX spelling is stable across these three clients.
            # methodN numbers are not, so only identical names are linked here.
            by_obf: dict[str, list[int]] = defaultdict(list)
            for rev in present:
                for member in members_of(by_rev_name[rev][simple]):
                    if is_obf(member.name):
                        by_obf[member.name].append(rev)
            for name, revs in by_obf.items():
                if len(revs) < 2:
                    continue
                head = (revs[0], simple, name)
                for rev in revs[1:]:
                    union(head, (rev, simple, name))
            unique: dict[int, dict[str, str]] = {}
            for rev in present:
                jc = by_rev_name[rev][simple]
                owner: dict[str, list[str]] = defaultdict(list)
                for member in members_of(jc):
                    for s in member.strings:
                        if match_string(s):
                            owner[s].append(member.name)
                unique[rev] = {s: names[0] for s, names in owner.items() if len(names) == 1}
            for left, right in zip(present, present[1:]):
                for s, lname in unique[left].items():
                    rname = unique[right].get(s)
                    if rname:
                        union((left, simple, lname), (right, simple, rname))
            # Also link a year directly to 177 when the middle year lacks the string.
            if 177 in present:
                for rev in present:
                    if rev == 177:
                        continue
                    for s, name in unique[rev].items():
                        other = unique[177].get(s)
                        if other:
                            union((rev, simple, name), (177, simple, other))
        else:
            # Unique stable initializer.
            for signal in ("init", "type"):
                buckets: dict[int, dict[str, list[str]]] = {}
                for rev in present:
                    jc = by_rev_name[rev][simple]
                    buckets[rev] = defaultdict(list)
                    for member in members_of(jc):
                        if signal == "init":
                            key = effective_init(member)
                        else:
                            key = member.type_name
                        if key:
                            buckets[rev][key].append(member.name)
                for left, right in zip(present, present[1:]):
                    for key, lnames in buckets[left].items():
                        rnames = buckets[right].get(key, [])
                        if len(lnames) == 1 and len(rnames) == 1:
                            union((left, simple, lnames[0]), (right, simple, rnames[0]))
                if 177 in present:
                    for rev in present:
                        if rev == 177:
                            continue
                        for key, names in buckets[rev].items():
                            other = buckets[177].get(key, [])
                            if len(names) == 1 and len(other) == 1:
                                union((rev, simple, names[0]), (177, simple, other[0]))

    groups: dict[tuple[int, str, str], list[tuple[int, str, str]]] = defaultdict(list)
    seen_keys = set()
    for rev, classes in parsed.items():
        for jc in classes:
            for member in members_of(jc):
                key = (rev, jc.simple, member.name)
                if key in seen_keys:
                    continue
                seen_keys.add(key)
                groups[find(key)].append(key)

    # Desired name and priority per group, then per old token inside one year.
    # token identity is (rev, old name) because the same spelling in two classes
    # is an override and must stay one token.
    token_best: dict[tuple[int, str], tuple[int, str]] = {}
    token_classes: dict[tuple[int, str], set[str]] = defaultdict(set)

    index: dict[tuple[int, str, str], Member] = {}
    owners: dict[tuple[int, str], JClass] = {}
    for rev, classes in parsed.items():
        for jc in classes:
            owners[(rev, jc.simple)] = jc
            for member in members_of(jc):
                index[(rev, jc.simple, member.name)] = member
                if is_obf(member.name):
                    token_classes[(rev, member.name)].add(jc.simple)

    def consider(rev: int, old: str, pri: int, new: str) -> None:
        if not new or not is_obf(old):
            return
        prev = token_best.get((rev, old))
        if prev is None or pri > prev[0] or (pri == prev[0] and len(new) > len(prev[1])):
            token_best[(rev, old)] = (pri, new)

    for keys in groups.values():
        good = []
        for rev, simple, name in keys:
            # Single-letter leftovers such as Link.s are not worth spreading.
            # Two-letter anchor names (Client.in, ClientStream.out) are.
            if not is_obf(name) and len(name) >= 2:
                pri = 100 if rev == 177 else 90
                good.append((pri, name))
        if good:
            good.sort(key=lambda item: (-item[0], -len(item[1]), item[1]))
            chosen_pri, chosen = good[0]
            # High enough to beat a local slug, low enough that a real 177 name wins.
            spread_pri = 80 if chosen_pri >= 90 else 70
            for rev, simple, name in keys:
                consider(rev, name, spread_pri if is_obf(name) else chosen_pri, chosen)
            continue
        # Shared slug from the longest shared content string, else from an init string.
        shared_strings: set[str] | None = None
        for rev, simple, name in keys:
            member = index[(rev, simple, name)]
            blob = {s for s in member.strings if content_string(s)}
            if member.init:
                blob.update(s for s in re.findall(r'"((?:\\.|[^"\\])*)"', member.init) if content_string(s))
            if shared_strings is None:
                shared_strings = set(blob)
            else:
                shared_strings &= blob
        base = ""
        candidates = list(shared_strings or [])
        if not candidates and len(keys) > 1:
            for rev, simple, name in keys:
                candidates.extend(index[(rev, simple, name)].strings)
        for s in sorted(set(candidates), key=len, reverse=True):
            letters = sum(ch.isalpha() for ch in s)
            if letters > 40 and " " not in s:
                continue
            base = slug(s, "field" if kind == "field" else "method")
            if base:
                break
        if not base and len(keys) > 1 and kind == "field":
            rep = index[next((key for key in keys if key[0] == 177), keys[0])]
            init = effective_init(rep)
            if re.fullmatch(r"-?\d+", init or "") and init not in TRIVIAL_INIT:
                base = "value" + init.replace("-", "Neg")
            else:
                base = type_slug(rep.type_name)
        if not base and len(keys) > 1 and kind == "method":
            names = {name for _rev, _simple, name in keys}
            if len(names) == 1:
                base = "call" + stable_tail(next(iter(names)))
        if base:
            for rev, simple, name in keys:
                consider(rev, name, 60 if shared_strings else 45, base)

    # Local names for obfuscated members that still have no proposal.
    for rev, classes in parsed.items():
        for jc in classes:
            for member in members_of(jc):
                if not is_obf(member.name) or (rev, member.name) in token_best:
                    continue
                base = ""
                pool = sorted(member.strings, key=len, reverse=True)
                if member.init:
                    pool = re.findall(r'"((?:\\.|[^"\\])*)"', member.init) + pool
                for s in pool:
                    if content_string(s) or (len(s) >= 4 and re.search(r"[A-Za-z]", s)):
                        base = slug(s, "field" if kind == "field" else "method")
                        if base:
                            break
                pri = 30 if base else 10
                if not base and kind == "field":
                    init = effective_init(member)
                    num = re.fullmatch(r"-?\d+", init or "")
                    if num and init not in TRIVIAL_INIT:
                        base = "value" + init.replace("-", "Neg")
                        pri = 40
                    else:
                        base = type_slug(member.type_name) + stable_tail(member.name)
                if not base:
                    prefix = "call" if kind == "method" else "value"
                    base = prefix + stable_tail(member.name)
                consider(rev, member.name, pri, base)

    # Apply higher-priority tokens first so they keep the unsuffixed name.
    ranked_tokens = sorted(token_best.items(), key=lambda item: (-item[1][0], item[0][1]))
    class_banned: dict[tuple[int, str], set[str]] = {}
    renaming: dict[tuple[int, str], set[str]] = defaultdict(set)
    for (rev, old), _choice in token_best.items():
        for simple in token_classes[(rev, old)]:
            renaming[(rev, simple)].add(old)
    for rev, classes in parsed.items():
        for jc in classes:
            # Declared members stay reserved even when every use is qualified
            # (`this.username`). Qualified names such as System.out do not.
            banned = simple_idents(jc.text) - renaming[(rev, jc.simple)]
            banned.add(jc.simple)
            if jc.extends:
                banned.add(jc.extends)
            for member in jc.fields:
                if member.name not in renaming[(rev, jc.simple)]:
                    banned.add(member.name)
            for member in jc.methods:
                banned.add(member.name)
            banned |= KEYWORDS
            class_banned[(rev, jc.simple)] = banned

    used_in_class: dict[tuple[int, str], set[str]] = defaultdict(set)
    final: dict[int, dict[str, str]] = {rev: {} for rev in REVS}
    for (rev, old), (_pri, base) in ranked_tokens:
        banned_sets = []
        for simple in token_classes[(rev, old)]:
            banned_sets.append(class_banned[(rev, simple)] | used_in_class[(rev, simple)])
        # Never collapse two old names onto one new name inside a class.
        name = free_name(base, set().union(*banned_sets) if banned_sets else set())
        # free_name against the union is stricter than necessary across classes,
        # but keeps an override's single spelling free everywhere it is declared.
        if name == old:
            continue
        final[rev][old] = name
        for simple in token_classes[(rev, old)]:
            used_in_class[(rev, simple)].add(name)

    for rev, mapping in final.items():
        if len(mapping.values()) != len(set(mapping.values())):
            # Same new spelling for two old tokens is allowed across classes
            # (both may be `socket`) but not if it is the same token twice.
            pass
        # Two old names in one class must not share a new name. used_in_class
        # already enforced that. Assert it.
        for simple, names in used_in_class.items():
            if simple[0] != rev:
                continue
        per_class: dict[str, list[str]] = defaultdict(list)
        for old, new in mapping.items():
            for simple in token_classes[(rev, old)]:
                per_class[simple].append(new)
        for simple, news in per_class.items():
            if len(news) != len(set(news)):
                raise SystemExit(f"{rev} {simple}: duplicate member name in {kind} plan")
    return final


# --- apply --------------------------------------------------------------------

def apply_mapping(rev: int, mapping: dict[str, str]) -> int:
    mapping = {old: new for old, new in mapping.items() if old != new and new}
    if not mapping:
        print(f"rsc {rev}: nothing to rename")
        return 0
    # Short names are only safe when every code use is the class, which is true
    # for `c` in 177. Refuse a short rename that also looks like a local.
    for old, new in mapping.items():
        if len(old) > 2:
            continue
        uses = 0
        for path in java_files(rev):
            text = path.read_text(errors="ignore")
            for is_code, a, b in segments(text):
                if not is_code:
                    continue
                uses += len(re.findall(rf"\b{re.escape(old)}\b", text[a:b]))
        print(f"  short rename {old} -> {new} has {uses} code uses")
    changed = 0
    renames: list[tuple[Path, Path]] = []
    for path in java_files(rev):
        text = path.read_text(errors="ignore")
        updated = replace_idents(text, mapping)
        if updated != text:
            path.write_text(updated)
            changed += 1
            text = updated
        head = CLASS_HEAD.search(mask_noncode(text))
        if head and head.group(2) != path.stem and path.name != "ObfuscatedName.java":
            dest = path.with_name(head.group(2) + ".java")
            if dest != path:
                renames.append((path, dest))
    # Two-step rename so Entity.java and class2.java can swap roles.
    temp_paths = []
    for src, _dest in renames:
        tmp = src.with_name(src.stem + ".__renaming__.java")
        src.rename(tmp)
        temp_paths.append(tmp)
    for tmp, (_src, dest) in zip(temp_paths, renames):
        if dest.exists():
            raise SystemExit(f"refusing to overwrite {dest}")
        tmp.rename(dest)
    print(f"rsc {rev}: rewrote {changed} files, renamed {len(renames)} class files, {len(mapping)} identifiers")
    return changed


def compile_all() -> int:
    env = os.environ.copy()
    env["JAVA_HOME"] = JAVA_HOME
    env["PATH"] = str(Path(JAVA_HOME) / "bin") + os.pathsep + env.get("PATH", "")
    failed = 0
    for rev in REVS:
        proj = src_root(rev)
        print(f"compile {rev}", flush=True)
        rc = subprocess.call(
            [sys.executable, str(ROOT / "scripts" / "fix_decompiled_java.py"), str(proj)],
            cwd=ROOT,
            env=env,
        )
        if rc != 0:
            failed += 1
            print(f"COMPILE FAILED {rev}", flush=True)
    return failed


def scan() -> int:
    total = 0
    per_rev = {}
    for rev in REVS:
        n = 0
        files = 0
        for path in java_files(rev):
            text = path.read_text(errors="ignore")
            hits = 0
            for is_code, a, b in segments(text):
                if not is_code:
                    continue
                hits += sum(1 for ident in IDENT_RE.findall(text[a:b]) if is_obf(ident))
            # Also count a raw hit, which currently matches the code count.
            raw = len(OBF_RE.findall(text)) if False else len(re.findall(
                r"\b(?:method\d+|field\d+|class\d+|kind\d+|Cls[0-9a-fA-F]+|fn[0-9a-f]{8}|(?:a|an)[A-Z]\w*\d+)\b",
                text,
            ))
            if raw:
                files += 1
            n += raw
        per_rev[rev] = n
        total += n
        print(f"scan {rev}: {n} obfuscated tokens in {files} files")
    return total


def self_test() -> None:
    sample = (
        "class Demo {\n"
        "  char tick = '\\'';\n"
        "  char slash = '\\\\';\n"
        "  char wide = '\\uffff';\n"
        "  String msg = \"can't field1 'x'\";\n"
        "  int field1 = 1;\n"
        "  void method1() { this.field1++; }\n"
        "}\n"
    )
    updated = replace_idents(sample, {"field1": "alpha", "method1": "bump", "Demo": "Sample"})
    if "can't field1" not in updated:
        raise SystemExit("self-test: string literal was modified")
    if "'\\''" not in updated or "'\\\\'" not in updated or "'\\uffff'" not in updated:
        raise SystemExit(f"self-test: character literal changed: {updated!r}")
    if "int alpha" not in updated or "void bump" not in updated or "class Sample" not in updated:
        raise SystemExit(f"self-test: identifier missed: {updated!r}")
    if updated.count("field1") != 1 or "method1" in updated:
        raise SystemExit("self-test: code identifier survived or string was the only copy")
    # Brace and string attribution on a tiny class.
    masked = mask_noncode(sample)
    if "field1" not in masked or "can't" in masked:
        raise SystemExit("self-test: mask failed")
    print("self-test ok")


def dump_class_plan(parsed: dict[int, list[JClass]], plan: dict[int, dict[str, str]]) -> None:
    print("class plan:")
    for rev in REVS:
        items = sorted(plan[rev].items())
        print(f"  {rev}: {len(items)} renames")
        for old, new in items:
            print(f"    {old} -> {new}")
    print("structural targets:")
    for rev in REVS:
        for jc in parsed[rev]:
            target = structural_target(jc)
            if target and target != jc.simple:
                print(f"  {rev} {jc.simple} => {target}")


def shared_names(kind: str) -> list[str]:
    """Member or class names present in all three years."""
    found = None
    for rev in REVS:
        names = set()
        for path in java_files(rev):
            if path.name == "ObfuscatedName.java":
                continue
            text = path.read_text(errors="ignore")
            head = CLASS_HEAD.search(mask_noncode(text))
            if head:
                names.add(head.group(2))
            if kind == "class":
                continue
            jc = parse_file(rev, path)
            if not jc:
                continue
            members = jc.methods if kind == "method" else jc.fields
            for member in members:
                if not is_obf(member.name):
                    names.add(f"{jc.simple}.{member.name}")
        found = names if found is None else found & names
    return sorted(found or [])


def main() -> int:
    self_test()
    args = sys.argv[1:]
    dry = "--apply" not in args
    if "--phase" in args:
        phase = args[args.index("--phase") + 1]
    elif any(arg in {"class", "method", "field", "all", "scan"} for arg in args):
        phase = next(arg for arg in args if arg in {"class", "method", "field", "all", "scan"})
    else:
        phase = "all"
    if phase == "scan":
        return 0 if scan() == 0 else 1
    phases = ["class", "method", "field"] if phase == "all" else [phase]
    for current in phases:
        parsed = {rev: parse_rev(rev) for rev in REVS}
        for rev, classes in parsed.items():
            fields = sum(len(jc.fields) for jc in classes)
            methods = sum(len(jc.methods) for jc in classes)
            obf_f = sum(1 for jc in classes for member in jc.fields if is_obf(member.name))
            obf_m = sum(1 for jc in classes for member in jc.methods if is_obf(member.name))
            print(f"rsc {rev}: {len(classes)} classes, {fields} fields ({obf_f} obf), {methods} methods ({obf_m} obf)")
        if current == "class":
            plan = plan_classes(parsed)
            dump_class_plan(parsed, plan)
        else:
            plan = plan_members(parsed, current)
            for rev in REVS:
                print(f"  {rev} {current} renames: {len(plan[rev])}")
                sample = list(plan[rev].items())[:8]
                for old, new in sample:
                    print(f"    {old} -> {new}")
        if dry:
            print(f"dry-run only for {current}; pass --apply to edit sources")
            continue
        for rev in REVS:
            apply_mapping(rev, plan[rev])
        failed = compile_all()
        if failed:
            return failed
    if not dry and phase == "all":
        print("shared class names:", ", ".join(shared_names("class")[:12]))
        left = scan()
        return 0 if left == 0 else 1
    if not dry:
        scan()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
