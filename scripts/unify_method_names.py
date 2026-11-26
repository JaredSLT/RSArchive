#!/usr/bin/env python3
"""Give each engine one method vocabulary across its yearly clients.

RSC, RS2, and RS3 are separate engines. A method keeps one name for every
year inside its own engine, and is never renamed to match a different engine.

Names come from the curated maps already in the archive:

  RS2   tools/rs-deob/remap/rs2-317.txt, then rs2-254, then RT4 (rev 530)
  RSC   Pazaz RSC-177 identifiers, then the RSC remaps
  RS3   tools/rs-deob/remap/rs3-910.txt (in-place name, not a member move)

Years that have no curated name share a stable fn+hash name when the same
method is present in more than one year. Remap lines are

    owner.name(descriptor)=sharedName

which is what LostCity's RemapTransformer applies after UniqueRenamer.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import math
import re
import shutil
import sys
import zipfile
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from generate_unified_remaps import (  # noqa: E402
    ORIGINAL_MEMBER,
    REFS,
    distinctive,
    parse_cp,
    u2,
    utf,
)

BUILDS = ROOT / "builds"
REMAP_DIR = ROOT / "tools" / "rs-deob" / "remap"
OUT_DIR = ROOT / "work" / "unified-remaps"
BACKUP_DIR = ROOT / "work" / "remap-backup-pre-method-unify"

# UniqueRenamer rewrites only these. RemapTransformer then rewrites names
# that start with "method". Anything else is left as it appears in the jar.
OBF_NAME = re.compile(r"^[A-Za-z]{1,3}$")
UNIQUE_RENAMER_KEEPS = {"run", "add", "put", "get", "set", "uid", "dns"}

SKIP_PREFIX = (
    "java/",
    "javax/",
    "sun/",
    "com/sun/",
    "jaclib/",
    "jaggl/",
    "jagex3/",
    "org/",
    "jdk/",
)

JAVA_KEYWORDS = {
    "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
    "class", "const", "continue", "default", "do", "double", "else", "enum",
    "extends", "final", "finally", "float", "for", "goto", "if", "implements",
    "import", "instanceof", "int", "interface", "long", "native", "new",
    "package", "private", "protected", "public", "return", "short", "static",
    "strictfp", "super", "switch", "synchronized", "this", "throw", "throws",
    "transient", "try", "void", "volatile", "while", "true", "false", "null",
    "var", "yield", "record", "sealed", "permits",
}

BAD_GENERATED = re.compile(
    r"^(?:method|field|anInt|anLong|aBoolean|aString|aByte|aChar|aFloat|aDouble|"
    r"aClass|anClass|class)\d+"
    r"|^(?:a|an)[A-Z][A-Za-z0-9_]*\d+$"
)
SYNTHETIC_NAME = re.compile(r"^fn[0-9a-f]{8}$")
IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")

ACC_STATIC = 0x0008
ACC_PRIVATE = 0x0002
ACC_BRIDGE = 0x0040
ACC_SYNTHETIC = 0x1000

# Interesting numeric literals. Tiny constants are in almost every method.
IGNORE_INTS = set(range(-5, 33)) | {64, 100, 128, 256, 512, 1000, 1024, 2048, 4096, 8192, 65535, 65536}

SANITY = {
    "rs2": (317, [
        "formatObjCount", "formatObjCountTagged", "setHighMemory", "setLowMemory",
        "drawChat", "buildScene", "drawMenu", "showLoadError", "addFriend", "removeFriend",
        "p1", "g1", "pjstr",
    ]),
    "rsc": (177, ["draw", "update", "load", "unload", "onKeyPress", "onMouseClick"]),
    "rs3": (910, ["parseInt", "drawGame", "drawScene", "split", "charIsAlpha"]),
}


@dataclass
class Method:
    owner: str
    name: str
    desc: str
    access: int
    strings: frozenset[str]
    ints: frozenset[int]
    call_descs: frozenset[str]
    calls: frozenset[tuple[str, str, str]]
    ops: dict[int, int]
    code_len: int
    sdesc: str

    @property
    def key(self) -> str:
        return f"{self.owner}.{self.name}{self.desc}"

    @property
    def mid(self) -> tuple:
        return (self.owner, self.name, self.desc)


@dataclass
class ClassInfo:
    name: str
    super_name: str | None
    interfaces: list[str]
    methods: list[Method] = field(default_factory=list)
    strings: set[str] = field(default_factory=set)

    def shape(self) -> Counter:
        return Counter(m.sdesc for m in self.methods if m.code_len >= 0 and m.name not in ("<init>", "<clinit>"))


def structural_desc(desc: str) -> str:
    return re.sub(r"L[^;]*;", "L*;", desc)


def good_name(name: str) -> bool:
    if not name or not IDENT.match(name):
        return False
    if name in JAVA_KEYWORDS or name in {"<init>", "<clinit>"}:
        return False
    if BAD_GENERATED.match(name):
        return False
    if SYNTHETIC_NAME.match(name):
        return False
    if re.fullmatch(r"[A-Za-z]", name):
        return False
    return True


def will_unique_rename(name: str) -> bool:
    return bool(OBF_NAME.match(name)) and name not in UNIQUE_RENAMER_KEEPS


def skip_class(name: str) -> bool:
    if not name or name.startswith("["):
        return True
    return any(name.startswith(p) for p in SKIP_PREFIX)


def cosine(a: dict[int, int], b: dict[int, int]) -> float:
    if not a or not b:
        return 0.0
    dot = na = nb = 0
    for k in set(a) | set(b):
        x = a.get(k, 0)
        y = b.get(k, 0)
        dot += x * y
        na += x * x
        nb += y * y
    if not na or not nb:
        return 0.0
    return dot / math.sqrt(na * nb)


def cp_int(entry) -> int | None:
    if not entry or entry[0] != 3:
        return None
    raw = ((entry[1] & 0xFFFF) << 16) | (entry[2] & 0xFFFF)
    if raw >= 2**31:
        raw -= 2**32
    return raw


def method_tokens(code: bytes, cp) -> tuple[set[str], set[int], set[str], set[tuple[str, str, str]], dict[int, int], int]:
    strings: set[str] = set()
    ints: set[int] = set()
    call_descs: set[str] = set()
    calls: set[tuple[str, str, str]] = set()
    ops: dict[int, int] = {}
    if not code or len(code) < 8:
        return strings, ints, call_descs, calls, ops, 0
    clen = int.from_bytes(code[4:8], "big")
    bc = code[8 : 8 + clen]
    i = 0
    wide = False
    while i < len(bc):
        op = bc[i]
        i += 1
        ops[op] = ops.get(op, 0) + 1
        if op == 18:  # ldc
            idx = bc[i]
            i += 1
            ent = cp[idx] if idx < len(cp) else None
            if ent and ent[0] == 8:
                s = utf(cp, idx)
                if s:
                    strings.add(s)
            elif ent and ent[0] == 3:
                n = cp_int(ent)
                if n is not None and n not in IGNORE_INTS:
                    ints.add(n)
        elif op in (19, 20):  # ldc_w, ldc2_w
            idx = (bc[i] << 8) | bc[i + 1]
            i += 2
            ent = cp[idx] if idx < len(cp) else None
            if ent and ent[0] == 8:
                s = utf(cp, idx)
                if s:
                    strings.add(s)
            elif ent and ent[0] == 3:
                n = cp_int(ent)
                if n is not None and n not in IGNORE_INTS:
                    ints.add(n)
        elif op == 16:  # bipush
            n = bc[i]
            if n >= 128:
                n -= 256
            i += 1
            if n not in IGNORE_INTS:
                ints.add(n)
        elif op == 17:  # sipush
            n = (bc[i] << 8) | bc[i + 1]
            if n >= 32768:
                n -= 65536
            i += 2
            if n not in IGNORE_INTS:
                ints.add(n)
        elif op in (182, 183, 184, 185):  # invoke*
            idx = (bc[i] << 8) | bc[i + 1]
            i += 2
            if op == 185:
                i += 2
            ent = cp[idx] if idx < len(cp) else None
            if ent and ent[0] in (10, 11) and ent[2] < len(cp):
                nat = cp[ent[2]]
                cls_name = utf(cp, ent[1])
                if (
                    nat
                    and nat[0] == 12
                    and cls_name
                    and nat[1] < len(cp)
                    and nat[2] < len(cp)
                    and cp[nat[1]]
                    and cp[nat[1]][0] == "Utf8"
                    and cp[nat[2]]
                    and cp[nat[2]][0] == "Utf8"
                ):
                    mname = cp[nat[1]][1]
                    mdesc = cp[nat[2]][1]
                    call_descs.add(structural_desc(mdesc))
                    calls.add((cls_name, mname, mdesc))
        elif op in (178, 179, 180, 181, 187, 189, 192, 193):
            i += 2
        elif op in (21, 22, 23, 24, 25, 54, 55, 56, 57, 58, 169) and wide:
            i += 2
            wide = False
        elif op == 132 and wide:
            i += 4
            wide = False
        elif op in (21, 22, 23, 24, 25, 54, 55, 56, 57, 58, 169, 188):
            i += 1
        elif op == 132:
            i += 2
        elif op in (
            153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166,
            167, 168, 198, 199,
        ):
            i += 2
        elif op in (200, 201):
            i += 4
        elif op == 186:  # invokedynamic
            i += 4
        elif op == 197:
            i += 3
        elif op == 170:  # tableswitch
            while i % 4:
                i += 1
            i += 4
            low = int.from_bytes(bc[i : i + 4], "big", signed=True)
            i += 4
            high = int.from_bytes(bc[i : i + 4], "big", signed=True)
            i += 4
            i += 4 * (high - low + 1)
        elif op == 171:  # lookupswitch
            while i % 4:
                i += 1
            i += 4
            npairs = int.from_bytes(bc[i : i + 4], "big", signed=True)
            i += 4
            i += 8 * max(npairs, 0)
        elif op == 196:
            wide = True
            continue
        wide = False
    return strings, ints, call_descs, calls, ops, clen


def read_code_attr(data: bytes, i: int, cp):
    n, i = u2(data, i)
    code = None
    for _ in range(n):
        ni, i = u2(data, i)
        ln, i = u4_at(data, i)
        body = data[i : i + ln]
        i += ln
        if utf(cp, ni) == "Code":
            code = body
    return code, i


def u4_at(data: bytes, i: int) -> tuple[int, int]:
    return int.from_bytes(data[i : i + 4], "big"), i + 4


def parse_class(data: bytes) -> ClassInfo | None:
    if len(data) < 10 or data[:4] != b"\xca\xfe\xba\xbe":
        return None
    try:
        cp, i = parse_cp(data)
    except Exception:
        return None
    try:
        _acc, i = u2(data, i)
        this_i, i = u2(data, i)
        super_i, i = u2(data, i)
        n_if, i = u2(data, i)
        ifaces = []
        for _ in range(n_if):
            ix, i = u2(data, i)
            name = utf(cp, ix)
            if name:
                ifaces.append(name)
        this_name = utf(cp, this_i)
        super_name = utf(cp, super_i) if super_i else None
        if not this_name:
            return None
        fn, i = u2(data, i)
        for _ in range(fn):
            i += 6
            i = skip_attrs_fast(data, i)
        mn, i = u2(data, i)
        methods = []
        strings: set[str] = set()
        for _ in range(mn):
            acc, i = u2(data, i)
            ni, i = u2(data, i)
            di, i = u2(data, i)
            code, i = read_code_attr(data, i, cp)
            mname = utf(cp, ni) or "?"
            mdesc = utf(cp, di) or "()V"
            raw_s, raw_i, call_descs, calls, ops, clen = set(), set(), set(), set(), {}, 0
            if code and not (acc & (ACC_BRIDGE | ACC_SYNTHETIC)):
                try:
                    raw_s, raw_i, call_descs, calls, ops, clen = method_tokens(code, cp)
                except Exception:
                    raw_s, raw_i, call_descs, calls, ops, clen = set(), set(), set(), set(), {}, 0
            dist = frozenset(distinctive(raw_s))
            strings |= dist
            methods.append(
                Method(
                    this_name,
                    mname,
                    mdesc,
                    acc,
                    dist,
                    frozenset(raw_i),
                    frozenset(call_descs),
                    frozenset(calls),
                    ops,
                    clen,
                    structural_desc(mdesc),
                )
            )
        return ClassInfo(this_name, super_name, ifaces, methods, strings)
    except Exception:
        return None


def skip_attrs_fast(data: bytes, i: int) -> int:
    n, i = u2(data, i)
    for _ in range(n):
        i += 2
        ln, i = u4_at(data, i)
        i += ln
    return i


def load_jar(path: Path) -> dict[str, ClassInfo]:
    out: dict[str, ClassInfo] = {}
    with zipfile.ZipFile(path) as zf:
        for name in zf.namelist():
            if not name.endswith(".class") or name.startswith("META-INF/"):
                continue
            info = parse_class(zf.read(name))
            if info and not skip_class(info.name):
                out[info.name] = info
    return out


def find_jars(family: str) -> list[tuple[int, Path]]:
    root = BUILDS / family
    found = []
    if not root.is_dir():
        return found
    for d in sorted(root.iterdir()):
        if not d.is_dir():
            continue
        head = d.name.split("-", 1)[0]
        if not head.isdigit():
            continue
        jar = d / "original" / "gamepack.jar"
        if jar.exists() and jar.stat().st_size > 0:
            found.append((int(head), jar))
    found.sort()
    return found


def parse_remap_methods(path: Path) -> dict[str, str]:
    """owner.name(desc) -> final simple name. Moves contribute the simple name only."""
    out = {}
    if not path.exists():
        return out
    for raw in path.read_text(errors="ignore").splitlines():
        line = raw.split("#", 1)[0].strip()
        if "=" not in line or "(" not in line.split("=", 1)[0]:
            continue
        left, right = line.split("=", 1)
        name = right.split(",")[-1].strip()
        if good_name(name) or SYNTHETIC_NAME.match(name):
            out[left.strip()] = name
    return out


def parse_java_anchors(root: Path) -> dict[str, str]:
    """Original owner.name(desc) -> Java identifier, from @OriginalMember trees."""
    out = {}
    if not root.exists():
        return out
    ident_re = re.compile(
        r"(?:public|private|protected|static|final|native|synchronized|\s)+"
        r"(?:[\w.<>\[\]]+)\s+(\w+)\s*(\(|;|=)"
    )
    for path in root.rglob("*.java"):
        text = path.read_text(errors="ignore")
        for om in ORIGINAL_MEMBER.finditer(text):
            owner, name, desc = om.group(1), om.group(2), om.group(3)
            if "!" in owner:
                owner = owner.split("!", 1)[1]
            owner = owner.replace(".", "/")
            after = text[om.end() : om.end() + 400]
            ident = ident_re.search(after)
            if not ident or ident.group(2) != "(":
                continue
            new = ident.group(1)
            if name in {"<init>", "<clinit>"}:
                continue
            if good_name(new):
                out[f"{owner}.{name}{desc}"] = new
    return out


class UnionFind:
    def __init__(self):
        self.p: dict[tuple, tuple] = {}

    def add(self, x: tuple) -> None:
        self.p.setdefault(x, x)

    def find(self, x: tuple) -> tuple:
        self.add(x)
        while self.p[x] != x:
            self.p[x] = self.p[self.p[x]]
            x = self.p[x]
        return x

    def union(self, a: tuple, b: tuple) -> None:
        ra, rb = self.find(a), self.find(b)
        if ra != rb:
            self.p[rb] = ra


def load_class_remap(path: Path) -> dict[str, str]:
    """Obfuscated internal name -> canonical Java name from a remap file."""
    out = {}
    if not path.exists():
        return out
    for raw in path.read_text(errors="ignore").splitlines():
        line = raw.split("#", 1)[0].strip()
        if "=" not in line:
            continue
        left, right = line.split("=", 1)
        if "(" in left or "." in left or "," in right:
            continue
        if right and not re.search(r"\s", right):
            out[left] = right
    return out


def link_classes(
    a: dict[str, ClassInfo],
    b: dict[str, ClassInfo],
    canon_a: dict[str, str] | None = None,
    canon_b: dict[str, str] | None = None,
    hard_a: dict[str, str] | None = None,
    hard_b: dict[str, str] | None = None,
) -> dict[str, str]:
    """Map class name in A to class name in B. One to one, conservative."""
    canon_a = canon_a or {}
    canon_b = canon_b or {}
    hard_a = hard_a or {}
    hard_b = hard_b or {}
    used_a, used_b = set(), set()
    linked: dict[str, str] = {}
    inv_b: dict[str, str] = {}
    for obf, fqcn in canon_b.items():
        inv_b.setdefault(fqcn, obf)
    for obf, fqcn in canon_a.items():
        other = inv_b.get(fqcn)
        if other and obf in a and other in b and obf not in used_a and other not in used_b:
            linked[obf] = other
            used_a.add(obf)
            used_b.add(other)

    rare_a: dict[str, str] = {}
    seen_a: dict[str, int] = Counter()
    for c in a.values():
        for s in c.strings:
            seen_a[s] += 1
    for c in a.values():
        for s in c.strings:
            if seen_a[s] == 1:
                rare_a[s] = c.name
    rare_b: dict[str, str] = {}
    seen_b: dict[str, int] = Counter()
    for c in b.values():
        for s in c.strings:
            seen_b[s] += 1
    for c in b.values():
        for s in c.strings:
            if seen_b[s] == 1:
                rare_b[s] = c.name

    votes: Counter = Counter()
    for s, ca in rare_a.items():
        cb = rare_b.get(s)
        if cb:
            votes[(ca, cb)] += 1

    # Same stable binary name (client, signlink) is the same class.
    for name in a:
        if name in b and (name in {"client", "signlink", "mudclient", "loader"} or "/" in name):
            votes[(name, name)] += 5

    pairs = []
    for (ca, cb), n in votes.items():
        if n >= 2 or (n >= 1 and ca == cb):
            pairs.append((n, ca, cb))
    pairs.sort(reverse=True)
    for n, ca, cb in pairs:
        if ca in used_a or cb in used_b:
            continue
        fa, fb = hard_a.get(ca), hard_b.get(cb)
        if fa and fb and fa != fb:
            continue
        linked[ca] = cb
        used_a.add(ca)
        used_b.add(cb)

    # Shape fallback for classes the string vote missed (buffers, ISAAC, fonts).
    shapes_b: dict[str, Counter] = {}
    for name, c in b.items():
        if name in used_b:
            continue
        sh = c.shape()
        if sum(sh.values()) >= 4:
            shapes_b[name] = sh
    shape_pairs = []
    for na, ca in a.items():
        if na in used_a:
            continue
        sha = ca.shape()
        total = sum(sha.values())
        if total < 4:
            continue
        for nb, shb in shapes_b.items():
            tb = sum(shb.values())
            if tb < 4:
                continue
            if min(total, tb) / max(total, tb) < 0.65:
                continue
            dot = na_ = nb_ = 0
            for k in set(sha) | set(shb):
                x, y = sha[k], shb[k]
                dot += x * y
                na_ += x * x
                nb_ += y * y
            if not na_ or not nb_:
                continue
            cos = dot / math.sqrt(na_ * nb_)
            shared = len(ca.strings & b[nb].strings)
            if cos >= 0.94 and shared >= 1:
                shape_pairs.append((cos + shared, na, nb))
            elif cos >= 0.98 and total >= 8:
                shape_pairs.append((cos, na, nb))
    shape_pairs.sort(reverse=True)
    for _sc, na, nb in shape_pairs:
        if na in used_a or nb in used_b:
            continue
        fa, fb = hard_a.get(na), hard_b.get(nb)
        if fa and fb and fa != fb:
            continue
        linked[na] = nb
        used_a.add(na)
        used_b.add(nb)
    return linked


def method_score(a: Method, b: Method) -> float:
    inter = len(a.strings & b.strings)
    union = len(a.strings | b.strings)
    jacc = inter / union if union else 0.0
    int_i = len(a.ints & b.ints)
    desc_same = a.sdesc == b.sdesc
    if a.code_len and b.code_len:
        ratio = min(a.code_len, b.code_len) / max(a.code_len, b.code_len)
    else:
        ratio = 0.0
    op = cosine(a.ops, b.ops)
    call_i = len(a.call_descs & b.call_descs)
    call_u = len(a.call_descs | b.call_descs)
    call_j = call_i / call_u if call_u else 0.0
    return (
        inter * 8
        + jacc * 4
        + min(int_i, 8) * 1.2
        + (4.0 if desc_same else 0.0)
        + ratio * 2
        + op * 4
        + call_j * 3
    )


def accept_pair(a: Method, b: Method, best: float, second: float) -> bool:
    inter = len(a.strings & b.strings)
    if inter >= 2 and best >= second + 2.5:
        return True
    if inter >= 1 and a.sdesc == b.sdesc and best >= second + 3.5 and best >= 12:
        return True
    if not a.code_len or not b.code_len:
        return False
    ratio = min(a.code_len, b.code_len) / max(a.code_len, b.code_len)
    op = cosine(a.ops, b.ops)
    # Dummy-parameter type changes (byte vs boolean vs int) keep the body.
    if inter >= 1 and op >= 0.97 and ratio >= 0.85 and a.code_len >= 40 and best >= second + 3:
        return True
    if a.sdesc != b.sdesc:
        return False
    # Near-identical bytecode, including the short Packet put/get helpers.
    if op >= 0.995 and ratio >= 0.92 and a.code_len >= 8 and best >= second + 0.4:
        return True
    if a.code_len < 20 or b.code_len < 20:
        return False
    if op >= 0.93 and ratio >= 0.8 and best >= second + 1.0:
        return True
    return False


def match_methods(ca: ClassInfo, cb: ClassInfo, rev_a: int, rev_b: int, uf: UnionFind) -> int:
    live_a = [m for m in ca.methods if m.name not in ("<init>", "<clinit>") and not (m.access & (ACC_BRIDGE | ACC_SYNTHETIC))]
    live_b = [m for m in cb.methods if m.name not in ("<init>", "<clinit>") and not (m.access & (ACC_BRIDGE | ACC_SYNTHETIC))]
    by_b: dict[str, list[Method]] = defaultdict(list)
    for m in live_b:
        by_b[m.sdesc].append(m)
    candidates = []
    for ma in live_a:
        pool = list(by_b.get(ma.sdesc, []))
        # Also consider a different descriptor when a literal is shared.
        # Jagex dummy parameters change type (byte, boolean, int) between years.
        if ma.strings:
            for mb in live_b:
                if mb.sdesc != ma.sdesc and (mb.strings & ma.strings):
                    pool.append(mb)
        if not pool:
            continue
        scored = []
        for mb in pool:
            if ma.sdesc != mb.sdesc and not (ma.strings & mb.strings):
                continue
            scored.append((method_score(ma, mb), mb))
        if not scored:
            continue
        scored.sort(key=lambda t: t[0], reverse=True)
        best, mb = scored[0]
        second = scored[1][0] if len(scored) > 1 else 0.0
        # Unique descriptor in both classes. Still require a similar body.
        # A descriptor match alone must not link two different methods.
        if len(by_b.get(ma.sdesc, [])) == 1 and sum(1 for m in live_a if m.sdesc == ma.sdesc) == 1:
            ratio = (min(ma.code_len, mb.code_len) / max(ma.code_len, mb.code_len)) if ma.code_len and mb.code_len else 0
            if ma.code_len >= 8 and cosine(ma.ops, mb.ops) >= 0.9 and ratio >= 0.75 and best >= second + 0.4:
                candidates.append((best + 2, ma, mb))
                continue
        if accept_pair(ma, mb, best, second):
            candidates.append((best, ma, mb))
    candidates.sort(key=lambda t: t[0], reverse=True)
    used_a, used_b = set(), set()
    n = 0
    for _sc, ma, mb in candidates:
        if ma.mid in used_a or mb.mid in used_b:
            continue
        uf.union((rev_a, *ma.mid), (rev_b, *mb.mid))
        used_a.add(ma.mid)
        used_b.add(mb.mid)
        n += 1
    return n, used_a, used_b


def link_unique_strings(
    ja, jb, rev_a, rev_b, uf: UnionFind, linked: dict[str, str],
    used_a: set, used_b: set,
) -> int:
    """Match a method that kept a long unique literal, including across a class move."""
    idx_a: dict[str, tuple] = {}
    count_a: Counter = Counter()
    for c in ja.values():
        for m in c.methods:
            for s in m.strings:
                if len(s) >= 12:
                    count_a[s] += 1
                    idx_a[s] = m.mid
    idx_b: dict[str, tuple] = {}
    count_b: Counter = Counter()
    for c in jb.values():
        for m in c.methods:
            for s in m.strings:
                if len(s) >= 12:
                    count_b[s] += 1
                    idx_b[s] = m.mid
    votes: dict[tuple, set[str]] = defaultdict(set)
    for s, mid in idx_a.items():
        if count_a[s] == 1 and count_b.get(s) == 1:
            votes[(mid, idx_b[s])].add(s)
    n = 0
    used_a, used_b = set(), set()
    items = sorted(votes.items(), key=lambda kv: len(kv[1]), reverse=True)
    for (ma, mb), strs in items:
        if len(strs) < 1:
            continue
        if ma in used_a or mb in used_b:
            continue
        same_class = linked.get(ma[0]) == mb[0]
        longest = max(len(s) for s in strs)
        if len(strs) == 1 and longest < (12 if same_class else 14):
            continue
        if not same_class and len(strs) == 1 and longest < 24:
            ma_m = next(m for m in ja[ma[0]].methods if m.mid == ma)
            mb_m = next(m for m in jb[mb[0]].methods if m.mid == mb)
            if not ma_m.code_len or not mb_m.code_len:
                continue
            ratio = min(ma_m.code_len, mb_m.code_len) / max(ma_m.code_len, mb_m.code_len)
            if cosine(ma_m.ops, mb_m.ops) < 0.8 or ratio < 0.45:
                continue
        uf.union((rev_a, *ma), (rev_b, *mb))
        used_a.add(ma)
        used_b.add(mb)
        n += 1
    return n


def refine_by_calls(ja, jb, rev_a, rev_b, uf: UnionFind, linked: dict[str, str], used_a: set, used_b: set) -> int:
    """Second pass: unmatched methods that call the same already-linked methods."""

    def roots(method: Method, rev: int) -> Counter:
        out: Counter = Counter()
        for owner, name, desc in method.calls:
            mid = (rev, owner, name, desc)
            if mid not in uf.p:
                continue
            out[uf.find(mid)] += 1
        return out

    n = 0
    for ca, cb in linked.items():
        live_a = [
            m for m in ja[ca].methods
            if m.mid not in used_a and m.name not in ("<init>", "<clinit>") and not (m.access & (ACC_BRIDGE | ACC_SYNTHETIC))
        ]
        live_b = [
            m for m in jb[cb].methods
            if m.mid not in used_b and m.name not in ("<init>", "<clinit>") and not (m.access & (ACC_BRIDGE | ACC_SYNTHETIC))
        ]
        root_b = [(m, roots(m, rev_b)) for m in live_b]
        cands = []
        for ma in live_a:
            ra = roots(ma, rev_a)
            if sum(ra.values()) < 4:
                continue
            scored = []
            for mb, rb in root_b:
                shared = sum((ra & rb).values()) if rb else 0
                if shared:
                    scored.append((shared, mb))
            if not scored:
                continue
            scored.sort(key=lambda t: t[0], reverse=True)
            best, mb = scored[0]
            second = scored[1][0] if len(scored) > 1 else 0
            if best >= 4 and best >= second + 2:
                cands.append((best, ma, mb))
        cands.sort(key=lambda t: t[0], reverse=True)
        for _sc, ma, mb in cands:
            if ma.mid in used_a or mb.mid in used_b:
                continue
            uf.union((rev_a, *ma.mid), (rev_b, *mb.mid))
            used_a.add(ma.mid)
            used_b.add(mb.mid)
            n += 1
    return n


def union_overrides(classes: dict[str, ClassInfo], rev: int, uf: UnionFind) -> None:
    by_sig: dict[tuple[str, str, str], tuple] = {}
    for c in classes.values():
        for m in c.methods:
            if m.name in ("<init>", "<clinit>"):
                continue
            if m.access & (ACC_PRIVATE | ACC_STATIC | ACC_BRIDGE | ACC_SYNTHETIC):
                continue
            by_sig[(c.name, m.name, m.desc)] = (rev, *m.mid)

    def walk(name: str, seen: set[str]) -> list[str]:
        if not name or name in seen:
            return []
        seen.add(name)
        c = classes.get(name)
        if not c:
            return []
        out = []
        if c.super_name:
            out.append(c.super_name)
            out.extend(walk(c.super_name, seen))
        for iface in c.interfaces:
            out.append(iface)
            out.extend(walk(iface, seen))
        return out

    for c in classes.values():
        parents = walk(c.super_name, set()) if c.super_name else []
        for iface in c.interfaces:
            parents.append(iface)
            parents.extend(walk(iface, set()))
        if not parents:
            continue
        for m in c.methods:
            if m.name in ("<init>", "<clinit>"):
                continue
            if m.access & (ACC_PRIVATE | ACC_STATIC | ACC_BRIDGE | ACC_SYNTHETIC):
                continue
            for p in parents:
                other = by_sig.get((p, m.name, m.desc))
                if other:
                    uf.union((rev, *m.mid), other)
                    break


def anchor_priority(engine: str, rev: int, source: str) -> int:
    if engine == "rs2" and rev == 317 and source == "remap":
        return 100
    if engine == "rs2" and rev == 254 and source == "remap":
        return 90
    if engine == "rs2" and rev == 530 and source == "rt4":
        return 80
    if engine == "rsc" and rev == 177 and source == "pazaz":
        return 100
    if engine == "rs3" and rev == 910 and source == "remap":
        return 100
    if source == "remap" and good_name_priority_source(source):
        return 50
    if SYNTHETIC_NAME.match(source):
        return 1
    return 40


def good_name_priority_source(source: str) -> bool:
    return source == "remap"


def load_anchors(engine: str, revs: list[int]) -> dict[tuple, list[tuple[int, str]]]:
    """(rev, owner, name, desc) -> [(priority, java name)]"""
    anchors: dict[tuple, list[tuple[int, str]]] = defaultdict(list)
    for rev in revs:
        text = parse_remap_methods(REMAP_DIR / f"{engine}-{rev}.txt")
        for key, name in text.items():
            owner, rest = key.split(".", 1)
            mname = rest[: rest.index("(")]
            desc = rest[rest.index("(") :]
            prio = anchor_priority(engine, rev, "remap")
            if SYNTHETIC_NAME.match(name):
                prio = 1
            elif not good_name(name):
                continue
            anchors[(rev, owner, mname, desc)].append((prio, name))
    if engine == "rsc":
        named = parse_java_anchors(REFS / "RSC-177" / "mudclient" / "src")
        named.update(parse_java_anchors(REFS / "RSC-177" / "mudclient" / "src" / "main" / "java"))
        # The tree is mudclient/src/main/java; the call above covers it once.
        for key, name in parse_java_anchors(REFS / "RSC-177").items():
            if not good_name(name):
                continue
            owner, rest = key.split(".", 1)
            mname = rest[: rest.index("(")]
            desc = rest[rest.index("(") :]
            anchors[(177, owner, mname, desc)].append((100, name))
    if engine == "rs2":
        for key, name in parse_java_anchors(REFS / "RT4-Client").items():
            if not good_name(name):
                continue
            owner, rest = key.split(".", 1)
            mname = rest[: rest.index("(")]
            desc = rest[rest.index("(") :]
            anchors[(530, owner, mname, desc)].append((80, name))
    return anchors


def choose_name(members: list[tuple], anchors, engine: str) -> str | None:
    best: tuple[int, str] | None = None
    for mid in members:
        for prio, name in anchors.get(mid, []):
            if not good_name(name):
                continue
            if best is None or prio > best[0] or (prio == best[0] and name < best[1]):
                best = (prio, name)
    if best:
        return best[1]
    # Stable synthetic name from the earliest member.
    earliest = min(members)
    rev, owner, name, desc = earliest
    digest = hashlib.sha1(f"{engine}|{rev}|{owner}|{name}|{desc}".encode()).hexdigest()[:8]
    return "fn" + digest


def cluster_spans(members: list[tuple]) -> int:
    return len({m[0] for m in members})


def build_engine(engine: str, revs: list[tuple[int, Path]]):
    print(f"\n== {engine} ==")
    jars = {}
    for rev, path in revs:
        print(f"  parse {engine}-{rev} {path.name}", flush=True)
        jars[rev] = load_jar(path)
        nmethods = sum(len(c.methods) for c in jars[rev].values())
        print(f"    classes={len(jars[rev])} methods={nmethods}")
    ordered = [rev for rev, _ in revs]
    uf = UnionFind()
    for rev, classes in jars.items():
        for c in classes.values():
            for m in c.methods:
                uf.add((rev, *m.mid))
        union_overrides(classes, rev, uf)
    hard = {rev: load_class_remap(REMAP_DIR / f"{engine}-{rev}.txt") for rev in ordered}
    canon = {rev: dict(hard[rev]) for rev in ordered}
    link_stats = []

    def match_pair(rev_a, rev_b, class_link, label: str) -> None:
        for ca, cb in class_link.items():
            fa, fb = canon[rev_a].get(ca), canon[rev_b].get(cb)
            if fa and not fb:
                canon[rev_b][cb] = fa
            elif fb and not fa:
                canon[rev_a][ca] = fb
            elif not fa and not fb:
                cid = f"@{rev_a}/{ca}"
                canon[rev_a][ca] = canon[rev_b][cb] = cid
        n_m = 0
        used_a, used_b = set(), set()
        for ca, cb in class_link.items():
            added, ua, ub = match_methods(jars[rev_a][ca], jars[rev_b][cb], rev_a, rev_b, uf)
            n_m += added
            used_a |= ua
            used_b |= ub
        n_move = link_unique_strings(
            jars[rev_a], jars[rev_b], rev_a, rev_b, uf, class_link, used_a, used_b
        )
        n_calls = refine_by_calls(
            jars[rev_a], jars[rev_b], rev_a, rev_b, uf, class_link, used_a, used_b
        )
        link_stats.append({
            "pair": label,
            "classes": len(class_link),
            "methods": n_m,
            "moved": n_move,
            "by_calls": n_calls,
        })
        print(
            f"  link {label}: classes={len(class_link)} methods={n_m} "
            f"moved={n_move} by_calls={n_calls}"
        )

    for rev_a, rev_b in zip(ordered, ordered[1:]):
        linked = link_classes(
            jars[rev_a], jars[rev_b], canon[rev_a], canon[rev_b], hard[rev_a], hard[rev_b]
        )
        match_pair(rev_a, rev_b, linked, f"{rev_a}->{rev_b}")
    # Bridge a year that has no class map. 317 and 377 both know Packet;
    # the December build between them does not, and must not sever the chain.
    hard_revs = [rev for rev in ordered if hard[rev]]
    for rev_a, rev_b in zip(hard_revs, hard_revs[1:]):
        if ordered.index(rev_b) == ordered.index(rev_a) + 1:
            continue
        linked = {}
        inv_b = {}
        for obf, fqcn in hard[rev_b].items():
            inv_b.setdefault(fqcn, obf)
        for obf, fqcn in hard[rev_a].items():
            other = inv_b.get(fqcn)
            if other and obf in jars[rev_a] and other in jars[rev_b]:
                linked[obf] = other
        if linked:
            match_pair(rev_a, rev_b, linked, f"{rev_a}=>{rev_b}")
    anchors = load_anchors(engine, ordered)
    groups: dict[tuple, list[tuple]] = defaultdict(list)
    index: dict[tuple, Method] = {}
    for rev, classes in jars.items():
        for c in classes.values():
            for m in c.methods:
                mid = (rev, *m.mid)
                groups[uf.find(mid)].append(mid)
                index[mid] = m
    # Collapse to root members lists.
    clusters = list(groups.values())
    print(f"  clusters={len(clusters)} anchors={sum(len(v) for v in anchors.values())}")
    return jars, ordered, clusters, anchors, index, link_stats


def assign(engine, jars, clusters, anchors, index):
    """Return rev -> {method key: new name} for clusters that span 2+ years."""
    # Claim order: semantic names first so they win collisions.
    planned = []
    for members in clusters:
        if cluster_spans(members) < 2:
            continue
        # Two curated names in the same year means two methods were fused.
        # Leave that year alone instead of giving both methods one name.
        by_rev: dict[int, set[str]] = defaultdict(set)
        for mid in members:
            for prio, nm in anchors.get(mid, []):
                if prio >= 80 and good_name(nm):
                    by_rev[mid[0]].add(nm)
        if any(len(names) > 1 for names in by_rev.values()):
            continue
        name = choose_name(members, anchors, engine)
        if not name:
            continue
        prio = 0
        for mid in members:
            for p, n in anchors.get(mid, []):
                if n == name:
                    prio = max(prio, p)
        if good_name(name):
            prio = max(prio, 10)
        planned.append((prio, name, members))
    planned.sort(key=lambda t: (-t[0], t[1]))

    # RemapTransformer keys its rename map by the simple name. A vanilla name
    # that already starts with "method" and is reused in two classes would
    # make both methods take whichever rename was stored last.
    ambiguous: dict[int, set[str]] = {}
    for rev, classes in jars.items():
        counts: Counter = Counter()
        for c in classes.values():
            for m in c.methods:
                if m.name.startswith("method") and not will_unique_rename(m.name):
                    counts[m.name] += 1
        ambiguous[rev] = {n for n, k in counts.items() if k > 1}
        if ambiguous[rev]:
            print(f"  {engine}-{rev}: {len(ambiguous[rev])} reused methodN names, left unrenamed")

    # Reserved vanilla names that UniqueRenamer will not rewrite, plus names
    # already claimed by a remap line. Two methods in one class cannot share
    # a name and a descriptor.
    reserved: dict[int, set[tuple[str, str, str]]] = defaultdict(set)
    existing_name: dict[int, dict[str, str]] = {}
    for rev, classes in jars.items():
        for c in classes.values():
            for m in c.methods:
                if not will_unique_rename(m.name) and not m.name.startswith("method"):
                    reserved[rev].add((m.owner, m.name, m.desc))
        existing_name[rev] = parse_remap_methods(REMAP_DIR / f"{engine}-{rev}.txt")
        for key, final in existing_name[rev].items():
            owner, rest = key.split(".", 1)
            desc = rest[rest.index("(") :]
            reserved[rev].add((owner, final, desc))

    out: dict[int, dict[str, str]] = defaultdict(dict)
    used: dict[int, set[tuple[str, str, str]]] = defaultdict(set)
    collisions = 0
    for _prio, name, members in planned:
        # Drop a member whose slot is already taken by a better cluster.
        kept = []
        for mid in members:
            rev, owner, _mname, desc = mid
            slot = (owner, name, desc)
            if slot in used[rev]:
                collisions += 1
                continue
            if slot in reserved[rev] and existing_name.get(rev, {}).get(index[mid].key) != name:
                # Would duplicate a method the renamer is leaving in place,
                # or a name already used by another method of this descriptor.
                collisions += 1
                continue
            if index[mid].name in ambiguous.get(rev, ()):
                collisions += 1
                continue
            if not will_unique_rename(index[mid].name) and not index[mid].name.startswith("method"):
                # RemapTransformer will not rename this method unless it already
                # starts with "method". Keep it only when the jar name is the shared name.
                if index[mid].name != name:
                    continue
            used[rev].add(slot)
            kept.append(mid)
        if cluster_spans(kept) < 2 and not any(anchors.get(m) for m in kept):
            for mid in kept:
                rev, owner, _n, desc = mid
                used[rev].discard((owner, name, desc))
            continue
        for mid in kept:
            rev, owner, mname, desc = mid
            out[rev][f"{owner}.{mname}{desc}"] = name
    # Fernflower drops a single unused primitive argument, so two overloads
    # like (Z)V and (I)V become the same Java method and no longer override.
    dummy = re.compile(r"^\([BSIJZ]\)")
    for rev, methods in out.items():
        by_owner_name: dict[tuple[str, str], list[str]] = defaultdict(list)
        for key, name in methods.items():
            owner, rest = key.split(".", 1)
            desc = rest[rest.index("(") :]
            if dummy.match(desc):
                by_owner_name[(owner, name)].append(key)
        for (_owner, name), keys in by_owner_name.items():
            if len(keys) < 2:
                continue
            for n, key in enumerate(sorted(keys)[1:], start=2):
                methods[key] = f"{name}b" if n == 2 else f"{name}b{n}"
    return out, collisions


def sanity_report(engine, ordered, clusters, anchors, assignment) -> list[str]:
    lines = []
    if engine not in SANITY:
        return lines
    rev0, names = SANITY[engine]
    # Map name -> cluster members, preferring the anchor on rev0.
    by_name: dict[str, list[tuple]] = {}
    for members in clusters:
        for mid in members:
            if mid[0] != rev0:
                continue
            for _p, name in anchors.get(mid, []):
                if name in names and name not in by_name:
                    by_name[name] = members
    lines.append(f"sanity {engine} (from rev {rev0}):")
    for name in names:
        members = by_name.get(name)
        if not members:
            lines.append(f"  {name}: not an anchor on {rev0}")
            continue
        present = sorted({m[0] for m in members})
        assigned = []
        for mid in members:
            rev = mid[0]
            key = f"{mid[1]}.{mid[2]}{mid[3]}"
            got = assignment.get(rev, {}).get(key)
            if got:
                assigned.append(f"{rev}:{got}")
        ok = all(assignment.get(m[0], {}).get(f"{m[1]}.{m[2]}{m[3]}") in {name, None} or True for m in members)
        # Check every spanned rev that we could rename got this exact name.
        bad = []
        for mid in members:
            rev = mid[0]
            key = f"{mid[1]}.{mid[2]}{mid[3]}"
            got = assignment.get(rev, {}).get(key)
            if got and got != name:
                bad.append(f"{rev}={got}")
        span = ",".join(str(r) for r in present)
        flag = "OK" if not bad and len(present) >= 2 else ("MISMATCH " + ",".join(bad) if bad else "ONE YEAR")
        lines.append(f"  {name}: years={span} [{flag}]")
    return lines


def merge_remap(engine: str, rev: int, methods: dict[str, str]) -> str:
    path = REMAP_DIR / f"{engine}-{rev}.txt"
    existing = []
    if path.exists():
        existing = path.read_text(errors="ignore").splitlines()
    kept = []
    seen = set()
    for raw in existing:
        line = raw.split("#", 1)[0].strip()
        if not line or "=" not in line:
            continue
        left, right = line.split("=", 1)
        if "(" in left:
            # A comma moves the member onto another class. Leave those lines
            # alone; only the simple name is shared with other years.
            if "," in right:
                methods.pop(left, None)
            elif left in methods:
                continue
        if left in seen:
            continue
        seen.add(left)
        kept.append(line)
    for key in sorted(methods):
        if key in seen:
            continue
        seen.add(key)
        kept.append(f"{key}={methods[key]}")
    return "\n".join(kept) + "\n"


STANDARD_DIR = ROOT / "work" / "method-standard"
JDK_KEEP = {
    "init", "start", "stop", "destroy", "paint", "update", "run",
    "keyPressed", "keyReleased", "keyTyped",
    "mouseClicked", "mousePressed", "mouseReleased", "mouseDragged",
    "mouseMoved", "mouseEntered", "mouseExited", "mouseWheelMoved",
    "focusGained", "focusLost",
    "windowActivated", "windowClosed", "windowClosing", "windowDeactivated",
    "windowDeiconified", "windowIconified", "windowOpened",
}
SIG_LINE = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|native|synchronized|abstract|strictfp)[ \t]+)*"
    r"(?:[\w.$]+(?:[ \t]*<[^>;{}]*>)?(?:[ \t]*\[[ \t]*\])*[ \t]+)?"
    r"([A-Za-z_$][\w$]*)[ \t]*\(([^)]*)\)[ \t]*(?:throws[ \t]+[\w., \t]+)?(\{|;)[ \t]*$"
)
SIG_SKIP = {
    "if", "for", "while", "switch", "catch", "do", "try", "else", "return",
    "new", "synchronized",
}
ANNO_LINE = re.compile(r'@ObfuscatedName\("([^"]+)"\)')
IDENT_TOKEN = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
STR_TOKEN = re.compile(r'"((?:\\.|[^"\\\n])*)"')
CALL_NAME = re.compile(r"^call[0-9a-f]{8}$")


def rev_dirs(engine: str) -> list[tuple[int, Path]]:
    root = BUILDS / engine
    found = []
    if not root.is_dir():
        return found
    for d in sorted(root.iterdir()):
        if not d.is_dir():
            continue
        head = d.name.split("-", 1)[0]
        if head.isdigit():
            found.append((int(head), d))
    found.sort()
    return found


def java_root_for(rev_dir: Path) -> Path | None:
    for rel in (Path("src/src/main/java"), Path("src/client/src/main/java")):
        p = rev_dir / rel
        if p.is_dir():
            return p
    return None


def anno_root_for(engine: str, rev: int, rev_dir: Path) -> Path | None:
    """Where @ObfuscatedName still names the original method."""
    if engine == "rs2" and rev != 742:
        p = rev_dir / "fernflower" / "extracted"
        return p if p.is_dir() else None
    return java_root_for(rev_dir)


def rel_java(path: Path, root: Path) -> str:
    return path.relative_to(root).as_posix()


def repo_rel(path: Path) -> str:
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return str(path)


def norm_file_hash(path: Path) -> str:
    """Hash a Java file with identifiers and annotation lines removed."""
    out = []
    for line in path.read_text(errors="ignore").splitlines():
        if ANNO_LINE.search(line):
            continue
        line = STR_TOKEN.sub('"S"', line)
        line = IDENT_TOKEN.sub("#", line)
        line = re.sub(r"\s+", "", line)
        out.append(line)
    return hashlib.sha1("\n".join(out).encode()).hexdigest()


def source_params_key(params: str) -> str:
    return re.sub(r"\s+", "", params or "")


def iter_java_files(root: Path) -> list[Path]:
    files = []
    for path in root.rglob("*.java"):
        if "build" in path.parts or "runtime-home" in path.parts:
            continue
        files.append(path)
    return files


def parse_methods(path: Path) -> list[dict]:
    """Declarations in source order. An ObfuscatedName with '(' binds the next method."""
    lines = path.read_text(errors="ignore").splitlines()
    pending = None
    found = []
    for i, line in enumerate(lines):
        am = ANNO_LINE.search(line)
        if am and "(" in am.group(1):
            pending = am.group(1).strip()
            continue
        m = SIG_LINE.match(line)
        if not m or m.group(1) in SIG_SKIP or m.group(1)[0].isupper():
            if pending and line.strip() and not line.strip().startswith("@"):
                pending = None
            continue
        anno = pending
        pending = None
        found.append({
            "line": i + 1,
            "name": m.group(1),
            "params": source_params_key(m.group(2)),
            "native": "native" in line,
            "obf": anno,
            "start": i,
        })
    for k, meth in enumerate(found):
        end = found[k + 1]["start"] if k + 1 < len(found) else len(lines)
        blob_lines = []
        for line in lines[meth["start"]:end]:
            if ANNO_LINE.search(line):
                continue
            blob_lines.append(line)
        blob = "\n".join(blob_lines)
        strings = frozenset(s for s in STR_TOKEN.findall(blob) if len(s) >= 8)
        norm = []
        for line in blob_lines:
            line = STR_TOKEN.sub('"S"', line)
            line = IDENT_TOKEN.sub("#", line)
            line = re.sub(r"\s+", "", line)
            norm.append(line)
        meth["strings"] = strings
        meth["body"] = hashlib.sha1("\n".join(norm).encode()).hexdigest()
        del meth["start"]
    return found


def strings_compatible(a: frozenset, b: frozenset) -> bool:
    if a and b and not (a & b):
        return False
    return True


def pair_files(ff_root: Path, src_root: Path) -> tuple[list[tuple[Path, Path]], dict]:
    ff_files = {rel_java(p, ff_root): p for p in iter_java_files(ff_root)}
    src_files = {rel_java(p, src_root): p for p in iter_java_files(src_root)}
    ff_hash = {rel: norm_file_hash(path) for rel, path in ff_files.items()}
    src_hash = {rel: norm_file_hash(path) for rel, path in src_files.items()}
    inv: dict[str, list[str]] = defaultdict(list)
    for rel, digest in src_hash.items():
        inv[digest].append(rel)
    pairs = []
    used_src: set[str] = set()
    same = renamed = 0
    for rel, digest in ff_hash.items():
        if rel in src_hash and src_hash[rel] == digest and rel not in used_src:
            pairs.append((ff_files[rel], src_files[rel]))
            used_src.add(rel)
            same += 1
            continue
        hits = [r for r in inv.get(digest, []) if r not in used_src]
        if len(hits) == 1:
            pairs.append((ff_files[rel], src_files[hits[0]]))
            used_src.add(hits[0])
            renamed += 1
    # Same path, body drifted, but method order and literals still agree.
    fallback = 0
    for rel, ff_path in ff_files.items():
        if rel not in src_files or rel in used_src:
            continue
        if any(ff_path == a for a, _b in pairs):
            continue
        ff_m = parse_methods(ff_path)
        src_m = parse_methods(src_files[rel])
        if not ff_m or len(ff_m) != len(src_m):
            continue
        if all(strings_compatible(a["strings"], b["strings"]) for a, b in zip(ff_m, src_m)):
            pairs.append((ff_path, src_files[rel]))
            used_src.add(rel)
            fallback += 1
    stats = {
        "ff_files": len(ff_files),
        "src_files": len(src_files),
        "paired_same_path": same,
        "paired_renamed": renamed,
        "paired_fallback": fallback,
        "unpaired_ff": len(ff_files) - same - renamed - fallback,
    }
    return pairs, stats


def zip_methods(ff_methods: list[dict], src_methods: list[dict]) -> list[tuple[dict, dict]]:
    if ff_methods and len(ff_methods) == len(src_methods):
        return list(zip(ff_methods, src_methods))
    inv: dict[str, list[dict]] = defaultdict(list)
    for meth in src_methods:
        inv[meth["body"]].append(meth)
    used = set()
    paired = []
    for fm in ff_methods:
        hits = [m for m in inv.get(fm["body"], []) if id(m) not in used]
        if len(hits) == 1:
            used.add(id(hits[0]))
            paired.append((fm, hits[0]))
    return paired


def bind_year(engine: str, rev: int, rev_dir: Path) -> tuple[dict[str, dict], dict]:
    """obf key -> source declaration for one revision."""
    src_root = java_root_for(rev_dir)
    stats = {"annotated": 0, "bound": 0, "native": 0, "duplicate_obf": 0}
    if src_root is None:
        stats["reason"] = "no sources"
        return {}, stats
    if engine != "rs2" or rev == 742:
        bound = {}
        for path in iter_java_files(src_root):
            for meth in parse_methods(path):
                if not meth["obf"]:
                    continue
                stats["annotated"] += 1
                if meth["native"]:
                    stats["native"] += 1
                    continue
                if meth["obf"] in bound:
                    stats["duplicate_obf"] += 1
                    continue
                bound[meth["obf"]] = {
                    "file": repo_rel(path),
                    "line": meth["line"],
                    "old": meth["name"],
                    "params": meth["params"],
                    "native": False,
                }
                stats["bound"] += 1
        return bound, stats

    ff_root = anno_root_for(engine, rev, rev_dir)
    if ff_root is None:
        stats["reason"] = "no fernflower"
        return {}, stats
    pairs, pair_stats = pair_files(ff_root, src_root)
    stats.update(pair_stats)
    bound = {}
    for ff_path, src_path in pairs:
        ff_m = parse_methods(ff_path)
        src_m = parse_methods(src_path)
        for fm, sm in zip_methods(ff_m, src_m):
            if not fm["obf"]:
                continue
            stats["annotated"] += 1
            if fm["native"] or sm["native"]:
                stats["native"] += 1
                continue
            if fm["obf"] in bound:
                stats["duplicate_obf"] += 1
                continue
            bound[fm["obf"]] = {
                "file": repo_rel(src_path),
                "line": sm["line"],
                "old": sm["name"],
                "params": sm["params"],
                "native": False,
            }
            stats["bound"] += 1
    return bound, stats


def public_name(name: str) -> str:
    if SYNTHETIC_NAME.match(name):
        return "call" + name[2:]
    return name


def readable_anchor(name: str) -> bool:
    if not good_name(name):
        return False
    if name.startswith("op") and len(name) > 2 and name[2].isupper():
        return False
    if name.startswith("do") and len(name) > 2 and name[2].isupper():
        return False
    if CALL_NAME.match(name):
        return False
    return True


def link_742(index: dict, clusters: list[list[tuple]]) -> dict[tuple, tuple]:
    """Map a 742 source method to a jar member in 695 or 745. High confidence only."""
    rev_dir = next((d for rev, d in rev_dirs("rs2") if rev == 742), None)
    root = java_root_for(rev_dir) if rev_dir else None
    if root is None:
        return {}
    by_mid = {}
    for members in clusters:
        for mid in members:
            by_mid[mid] = members
    jar_by_string: dict[int, dict[str, list[tuple]]] = {695: defaultdict(list), 745: defaultdict(list)}
    for mid, meth in index.items():
        rev = mid[0]
        if rev not in jar_by_string:
            continue
        for s in meth.strings:
            if len(s) >= 12:
                jar_by_string[rev][s].append(mid)
    src_methods = []
    src_count: Counter = Counter()
    for path in iter_java_files(root):
        for meth in parse_methods(path):
            longs = [s for s in meth["strings"] if len(s) >= 12]
            for s in longs:
                src_count[s] += 1
            src_methods.append((path, meth, longs))
    linked = {}
    for path, meth, longs in src_methods:
        votes: dict[tuple, list[str]] = defaultdict(list)
        for s in longs:
            if src_count[s] != 1:
                continue
            for rev in (745, 695):
                owners = jar_by_string[rev].get(s, [])
                if len(owners) == 1 and owners[0] in by_mid:
                    votes[owners[0]].append(s)
                    break
        if not votes:
            continue
        best = max(votes.items(), key=lambda kv: (len(kv[1]), max(len(s) for s in kv[1])))
        mid, strs = best
        longest = max(len(s) for s in strs)
        if len(strs) < 2 and longest < 24:
            continue
        linked[(repo_rel(path), meth["line"])] = (mid, meth, strs)
    return linked


def standardize_engine(engine, ordered, clusters, anchors, assignment, index) -> dict:
    bindings = {}
    bind_stats = {}
    for rev, rev_dir in rev_dirs(engine):
        if rev not in ordered and rev != 742:
            # Still bind years that have sources but no jar (742).
            if rev != 742:
                continue
        bound, stats = bind_year(engine, rev, rev_dir)
        bindings[rev] = bound
        bind_stats[str(rev)] = stats
        print(
            f"  bind {engine}-{rev}: annotated={stats.get('annotated', 0)} "
            f"bound={stats.get('bound', 0)} unpaired_ff={stats.get('unpaired_ff', 0)}",
            flush=True,
        )

    link742 = link_742(index, clusters) if engine == "rs2" else {}
    print(f"  742 string links={len(link742)}", flush=True)

    # Group 742 links by the jar cluster root (first member id).
    cluster_of = {}
    for members in clusters:
        root = id(members)
        for mid in members:
            cluster_of[mid] = root
    extra_742: dict[int, list] = defaultdict(list)
    for (_loc, (mid, meth, _strs)) in ((k, v) for k, v in link742.items()):
        root = cluster_of.get(mid)
        if root is None:
            continue
        extra_742[root].append(meth | {"file": _loc[0], "jar_mid": mid})

    counts = Counter()
    emitted = []
    for members in clusters:
        years = {m[0] for m in members}
        if len(years) < 2:
            counts["single_year"] += 1
            continue
        counts["multi_year"] += 1
        curated = {}
        for mid in members:
            for prio, nm in anchors.get(mid, []):
                if prio >= 80 and good_name(nm):
                    curated.setdefault(nm, set()).add(mid[0])
        if len(curated) > 1:
            counts["curated_conflict"] += 1
            bits = [f"{nm}@{','.join(str(r) for r in sorted(rs))}" for nm, rs in sorted(curated.items())]
            emitted.append({
                "skip": "curated_conflict",
                "conflict": bits,
                "years": sorted(years),
                "members": [],
            })
            continue
        names = []
        for mid in members:
            rev, owner, mname, desc = mid
            key = f"{owner}.{mname}{desc}"
            got = assignment.get(rev, {}).get(key)
            if got:
                names.append(public_name(got))
        if not names:
            counts["no_assignment"] += 1
            continue
        proposed = Counter(names).most_common(1)[0][0]
        root = id(members)
        lost = extra_742.get(root, [])
        if CALL_NAME.match(proposed):
            for meth in lost:
                if readable_anchor(meth["name"]):
                    proposed = meth["name"]
                    break
        out_members = []
        bound_years = set()
        for mid in members:
            rev, owner, mname, desc = mid
            key = f"{owner}.{mname}{desc}"
            decl = bindings.get(rev, {}).get(key)
            if decl is None:
                counts["unbound_member"] += 1
                continue
            if decl["old"] in JDK_KEEP:
                counts["jdk_kept"] += 1
                skip = "jdk"
                rename = False
                use = decl["old"]
            elif decl["old"] == proposed or (
                decl["old"].startswith(proposed) and CALL_NAME.match(proposed) and decl["old"] == proposed
            ):
                counts["already_named"] += 1
                skip = None
                rename = False
                use = decl["old"]
            else:
                skip = None
                rename = True
                use = proposed
                counts["renames"] += 1
            bound_years.add(rev)
            out_members.append({
                "rev": rev,
                "file": decl["file"],
                "line": decl["line"],
                "old": decl["old"],
                "params": decl["params"],
                "descriptor": desc,
                "obf": key,
                "proposed": use,
                "rename": rename,
                "skip": skip,
            })
        for meth in lost:
            old = meth["name"]
            if old in JDK_KEEP or meth.get("native"):
                continue
            rename = old != proposed
            out_members.append({
                "rev": 742,
                "file": meth["file"],
                "line": meth["line"],
                "old": old,
                "params": meth["params"],
                "descriptor": "",
                "obf": "",
                "proposed": proposed if rename else old,
                "rename": rename,
                "skip": None,
                "via": "string",
            })
            bound_years.add(742)
            if rename:
                counts["renames"] += 1
                counts["linked_742"] += 1
        if len(bound_years) < 2:
            counts["one_year_bound"] += 1
            continue
        # Source-signature collisions inside one file.
        by_slot: dict[tuple, list[dict]] = defaultdict(list)
        for mem in out_members:
            if mem["rename"]:
                by_slot[(mem["file"], mem["proposed"], mem["params"])].append(mem)
        for group in by_slot.values():
            if len(group) < 2:
                continue
            for n, mem in enumerate(sorted(group, key=lambda m: m["line"])[1:], start=2):
                suffix = "b" if n == 2 else f"b{n}"
                mem["proposed"] = mem["proposed"] + suffix
                mem["suffix"] = suffix
                counts["suffixed"] += 1
        counts["emitted"] += 1
        emitted.append({
            "skip": None,
            "proposed": proposed,
            "years": sorted(bound_years),
            "members": out_members,
        })
    return {
        "engine": engine,
        "revs": ordered,
        "bind": bind_stats,
        "counts": dict(counts),
        "clusters": emitted,
    }


def write_standard(report: dict) -> None:
    engine = report["engine"]
    STANDARD_DIR.mkdir(parents=True, exist_ok=True)
    dest = STANDARD_DIR / f"{engine}.json"
    dest.write_text(json.dumps(report, indent=2) + "\n")
    remap_dir = STANDARD_DIR / "remap"
    remap_dir.mkdir(parents=True, exist_ok=True)
    by_rev: dict[int, list[str]] = defaultdict(list)
    slices: dict[int, list[dict]] = defaultdict(list)
    for cluster in report["clusters"]:
        if cluster.get("skip"):
            continue
        for mem in cluster["members"]:
            if not mem.get("rename"):
                continue
            slices[mem["rev"]].append(mem)
            if mem.get("obf"):
                by_rev[mem["rev"]].append(f"{mem['obf']}={mem['proposed']}")
    for rev, lines in by_rev.items():
        (remap_dir / f"{engine}-{rev}.txt").write_text("\n".join(sorted(set(lines))) + "\n")
    for rev, mems in slices.items():
        (STANDARD_DIR / f"{engine}-{rev}.json").write_text(json.dumps(mems, indent=2) + "\n")
    print(f"  wrote {dest} clusters={report['counts'].get('emitted', 0)}", flush=True)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--engine", choices=("rsc", "rs2", "rs3"), action="append")
    ap.add_argument("--revs", help="Comma-separated revision filter")
    ap.add_argument("--write", action="store_true", help="Install remap files")
    ap.add_argument(
        "--standardize",
        action="store_true",
        help="Emit work/method-standard JSON. Does not edit builds or curated remaps",
    )
    args = ap.parse_args()
    engines = args.engine or ["rsc", "rs2", "rs3"]
    want = {int(x) for x in args.revs.split(",")} if args.revs else None
    summary = {}
    for engine in engines:
        revs = find_jars(engine)
        if want is not None:
            revs = [rv for rv in revs if rv[0] in want]
        if len(revs) < 1:
            print(f"{engine}: no jars")
            continue
        jars, ordered, clusters, anchors, index, link_stats = build_engine(engine, revs)
        assignment, collisions = assign(engine, jars, clusters, anchors, index)
        multi = sum(1 for c in clusters if cluster_spans(c) >= 2)
        named = sum(len(v) for v in assignment.values())
        print(f"  multi-year clusters={multi} remap methods={named} collisions skipped={collisions}")
        for line in sanity_report(engine, ordered, clusters, anchors, assignment):
            print(line)
        conflicts = []
        for members in clusters:
            named = {}
            for mid in members:
                for prio, nm in anchors.get(mid, []):
                    if prio >= 80 and good_name(nm):
                        named.setdefault(nm, set()).add(mid[0])
            if len(named) > 1:
                conflicts.append(named)
        print(f"  anchor-name conflicts={len(conflicts)}")
        for named in conflicts[:25]:
            bits = [f"{nm}@{','.join(str(r) for r in sorted(revs))}" for nm, revs in sorted(named.items())]
            print("   ", " vs ".join(bits))
        if args.standardize:
            report = standardize_engine(engine, ordered, clusters, anchors, assignment, index)
            write_standard(report)
            summary[engine] = {"counts": report["counts"], "bind": report["bind"]}
            continue
        summary[engine] = {
            "revs": ordered,
            "links": link_stats,
            "multi_year_clusters": multi,
            "methods_renamed": {str(k): len(v) for k, v in sorted(assignment.items())},
            "collisions_skipped": collisions,
        }
        if args.write:
            if not BACKUP_DIR.exists():
                BACKUP_DIR.mkdir(parents=True)
                for src in REMAP_DIR.glob("*.txt"):
                    shutil.copy2(src, BACKUP_DIR / src.name)
            REMAP_DIR.mkdir(parents=True, exist_ok=True)
            OUT_DIR.mkdir(parents=True, exist_ok=True)
            for rev, methods in sorted(assignment.items()):
                text = merge_remap(engine, rev, methods)
                dest = REMAP_DIR / f"{engine}-{rev}.txt"
                dest.write_text(text)
                (OUT_DIR / f"{engine}-{rev}.txt").write_text(text)
                print(f"  wrote {dest} ({len(methods)} methods)")
    if args.standardize:
        return 0
    if args.write or True:
        OUT_DIR.mkdir(parents=True, exist_ok=True)
        (OUT_DIR / "method-stats.json").write_text(json.dumps(summary, indent=2) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
