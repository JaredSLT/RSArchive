#!/usr/bin/env python3
"""Give every RS2 client one generified name per logical member.

RS2 is a separate engine from RSC and RS3. Names prefer the rev 317
vocabulary, then 254, then later curated names. Obfuscated identifiers
(methodN, fieldN, classN, fn hashes, anInt/anLong/aBoolean/aString/aClass,
and 8-letter dumped class names) are removed from src/main/java.

Fernflower's cast-then-call form ((Type)(Qualifier.method))(args) is rewritten
to a plain call. Two methods that share a source signature keep distinct names.
"""
from __future__ import annotations

import hashlib
import re
import subprocess
import sys
import zlib
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import jdk  # noqa: E402

BUILDS = ROOT / "builds" / "rs2"
JAVA_HOME = str(jdk.java_home())

REVS = [
    254, 282, 317, 350, 377, 443, 468, 474, 481, 508, 525, 530, 578, 614, 634,
    667, 695, 742, 745,
]
# Higher wins when a cluster has several curated names. 317 beats 254.
PRIO = {
    317: 100, 254: 90, 377: 80, 282: 70, 350: 60, 530: 55, 667: 50, 614: 45,
    742: 40, 508: 35, 474: 30, 468: 28, 481: 26, 525: 24, 578: 22, 634: 20,
    695: 15, 443: 12, 745: 10,
}
ALIASES = {"client": "Client", "signlink": "SignLink"}
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
# kindN is an obfuscated class name minted by an older pass. It is not stable
# across years. Eight-letter ALL-CAPS is a dumped class name only (see
# dumped_class_name); constants such as IIIIIIII stay untouched.
OBF_RE = re.compile(
    r"^(?:kind|method|field|class)\d+$"
    r"|^(?:anInt|anLong|aBoolean|aString|aClass)\d+$"
    r"|^fn[0-9a-f]{8}[A-Za-z0-9]*$"
)
OBF_FIND = re.compile(
    r"\b(?:kind\d+|method\d+|field\d+|class\d+|anInt\d+|anLong\d+|aBoolean\d+|aString\d+|aClass\d+|fn[0-9a-f]{8}[A-Za-z0-9]*)\b"
)
DUMPED_CLASS_RE = re.compile(r"^[A-Z]{8}$")
PROTECTED_CLASS_NAMES = {"Client", "SignLink", "Packet"}
ANCHOR_REVS = {317, 254, 377}
JDK_SUPERS = {
    "Object", "Applet", "Frame", "Canvas", "Panel", "Component", "Thread",
    "Exception", "Throwable", "Error", "RuntimeException", "InputStream",
    "OutputStream", "FilterInputStream", "FilterOutputStream", "Socket",
    "ServerSocket", "URL", "URLConnection", "HttpURLConnection", "Enum",
    "Buffer", "ByteBuffer", "ClassLoader", "Timer", "TimerTask",
}
IDENT_RE = re.compile(r"[A-Za-z_$][A-Za-z0-9_$]*")
SIG_RE = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|native|synchronized|abstract|default)[ \t]+)*"
    r"(?:(?P<ret>[\w.$]+(?:[ \t]*<[^>;{}]*>)?(?:[ \t]*\[[ \t]*\])*)[ \t]+)?"
    r"(?P<name>[A-Za-z_$][\w$]*)[ \t]*\((?P<params>[^)]*)\)[ \t]*(?:throws[ \t]+[\w., \t]+)?\{[ \t]*$"
)
FIELD_RE = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|volatile|transient)[ \t]+)+"
    r"(?P<type>[\w.$]+(?:[ \t]*<[^>;{}]*>)?(?:[ \t]*\[[ \t]*\])*)[ \t]+"
    r"(?P<name>[A-Za-z_$][\w$]*)[ \t]*(?:=|;)"
)
CLASS_RE = re.compile(
    r"\b(class|interface|enum)[ \t]+([A-Za-z_$][\w$]*)"
    r"(?:[ \t]+extends[ \t]+([\w.]+))?"
)
STR_RE = re.compile(r'"(?:\\.|[^"\\])*"')
CAST_CALL_RE = re.compile(
    r"\(\((?:[\w.$]+(?:\[\])*)\)\(([A-Za-z_][\w.]*)\)\)\("
)

CARRY = ("drawChat", "formatObjCount", "formatObjCountTagged", "buildScene", "drawMenu", "addFriend")


def log(msg: str) -> None:
    print(msg, flush=True)


def year_dir(rev: int) -> Path | None:
    matches = sorted(BUILDS.glob(f"{rev}-*"))
    return matches[0] if matches else None


def java_files(rev: int) -> list[Path]:
    ydir = year_dir(rev)
    if ydir is None:
        return []
    out = []
    src = ydir / "src"
    if not src.exists():
        return []
    for path in src.rglob("*.java"):
        parts = path.parts
        if "build" in parts or "fernflower" in parts:
            continue
        if "src" in parts and "main" in parts and "java" in parts:
            out.append(path)
    return out


def is_obf(name: str) -> bool:
    return bool(OBF_RE.match(name))


def dumped_class_name(name: str) -> bool:
    return bool(DUMPED_CLASS_RE.fullmatch(name))


def valid_ident(name: str) -> bool:
    if not name or name in KEYWORDS or is_obf(name) or dumped_class_name(name):
        return False
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", name):
        return False
    return len(name) > 2


def readable_class_name(name: str) -> str | None:
    """Curated simple name, or None when the class still needs a shared name."""
    raw = preferred_name(name)
    if raw in PROTECTED_CLASS_NAMES:
        return raw
    if dumped_class_name(raw) or is_obf(raw):
        return None
    if good_name(raw):
        return raw
    return None


def good_name(name: str) -> bool:
    if not name or name in KEYWORDS or name in ALIASES:
        return False
    if not re.match(r"^[A-Za-z_][A-Za-z0-9_]*$", name):
        return False
    if is_obf(name):
        return False
    if len(name) <= 2:
        return False
    return True


def preferred_name(name: str) -> str:
    return ALIASES.get(name, name)


def strip_code(line: str, state: dict) -> str:
    """Blank out strings and comments so brace counting ignores them."""
    out = []
    i = 0
    n = len(line)
    in_block = state.get("block", False)
    while i < n:
        if in_block:
            end = line.find("*/", i)
            if end < 0:
                out.append(" " * (n - i))
                state["block"] = True
                return "".join(out)
            out.append(" " * (end + 2 - i))
            i = end + 2
            in_block = False
            continue
        ch = line[i]
        if ch == "/" and i + 1 < n and line[i + 1] == "/":
            out.append(" " * (n - i))
            break
        if ch == "/" and i + 1 < n and line[i + 1] == "*":
            in_block = True
            i += 2
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
            out.append('""' + " " * max(0, j - i - 2))
            i = j
            continue
        if ch == "'" and i + 1 < n:
            j = i + 1
            if j < n and line[j] == "\\":
                j += 2
            if j < n:
                j += 1
            if j < n and line[j] == "'":
                j += 1
            out.append(" " * (j - i))
            i = j
            continue
        out.append(ch)
        i += 1
    state["block"] = False
    return "".join(out)


def strings_in(text: str) -> set[str]:
    found = set()
    for raw in STR_RE.findall(text):
        body = raw[1:-1]
        if 8 <= len(body) <= 120 and not body.startswith("\\"):
            found.add(body)
    return found


def split_params(param_text: str) -> tuple[str, ...]:
    param_text = param_text.strip()
    if not param_text:
        return ()
    parts = []
    depth = 0
    cur = []
    for ch in param_text:
        if ch == "<":
            depth += 1
        elif ch == ">":
            depth -= 1
        elif ch == "," and depth == 0:
            parts.append("".join(cur))
            cur = []
            continue
        cur.append(ch)
    if cur:
        parts.append("".join(cur))
    types = []
    for part in parts:
        part = re.sub(r"\bfinal\s+", "", part.strip())
        part = re.sub(r"\s+", " ", part).strip()
        if not part:
            continue
        toks = part.split(" ")
        if len(toks) >= 2:
            types.append("".join(toks[:-1]))
        else:
            types.append(toks[0])
    return tuple(types)


@dataclass
class JMethod:
    name: str
    params: tuple[str, ...]
    strings: set[str] = field(default_factory=set)
    obf: bool = False


@dataclass
class JField:
    name: str
    jtype: str
    strings: set[str] = field(default_factory=set)
    obf: bool = False


@dataclass
class JClass:
    rev: int
    path: Path
    package: str
    simple: str
    super_name: str | None
    strings: set[str]
    methods: list[JMethod]
    fields: list[JField]

    @property
    def key(self) -> tuple:
        return (self.rev, self.package, self.simple)


class UnionFind:
    def __init__(self) -> None:
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

    def groups(self) -> dict[tuple, list[tuple]]:
        out: dict[tuple, list[tuple]] = defaultdict(list)
        for x in list(self.p):
            out[self.find(x)].append(x)
        return out


def parse_file(rev: int, path: Path) -> JClass | None:
    text = path.read_text(errors="ignore")
    package = ""
    pm = re.search(r"^package\s+([\w.]+)\s*;", text, re.M)
    if pm:
        package = pm.group(1)
    cm = CLASS_RE.search(text)
    if not cm:
        return None
    simple = cm.group(2)
    super_name = cm.group(3).split(".")[-1] if cm.group(3) else None
    lines = text.splitlines(True)
    state: dict = {"block": False}
    depth = 0
    methods: list[JMethod] = []
    fields: list[JField] = []
    current: JMethod | None = None
    current_end_depth = None
    body: list[str] = []
    for line in lines:
        bare = strip_code(line, state)
        delta = bare.count("{") - bare.count("}")
        if current is None and depth == 1:
            sm = SIG_RE.match(line)
            if sm and sm.group("name") not in KEYWORDS:
                name = sm.group("name")
                if name != simple:
                    current = JMethod(name, split_params(sm.group("params") or ""), obf=is_obf(name) or name in ALIASES)
                    body = [line]
                    current_end_depth = depth + delta
                    if delta <= 0:
                        current.strings = strings_in("".join(body))
                        methods.append(current)
                        current = None
                    depth += delta
                    continue
            fm = FIELD_RE.match(line)
            if fm:
                fname = fm.group("name")
                fields.append(JField(fname, re.sub(r"\s+", "", fm.group("type")), strings_in(line), obf=is_obf(fname)))
        elif current is not None:
            body.append(line)
            if depth + delta <= 1 and current_end_depth is not None:
                current.strings = strings_in("".join(body))
                methods.append(current)
                current = None
        depth += delta
        if depth < 0:
            depth = 0
    if current is not None:
        current.strings = strings_in("".join(body))
        methods.append(current)
    return JClass(rev, path, package, simple, super_name, strings_in(text), methods, fields)


def load_all(revs: list[int]) -> dict[int, list[JClass]]:
    out: dict[int, list[JClass]] = {}
    for rev in revs:
        classes = []
        for path in java_files(rev):
            jc = parse_file(rev, path)
            if jc:
                classes.append(jc)
        out[rev] = classes
        log(f"parsed {rev}: {len(classes)} classes")
    return out


def class_clusters(classes: dict[int, list[JClass]]) -> dict[tuple, list[JClass]]:
    uf = UnionFind()
    by_key: dict[tuple, JClass] = {}
    for rev, items in classes.items():
        for jc in items:
            uf.add(jc.key)
            by_key[jc.key] = jc
    year_cache: dict[tuple, set[int]] = {}

    def years_of(root: tuple) -> set[int]:
        cached = year_cache.get(root)
        if cached is not None:
            return cached
        found = {key[0] for key in uf.p if uf.find(key) == root}
        year_cache[root] = found
        return found

    def safe_union(a: tuple, b: tuple) -> bool:
        ra, rb = uf.find(a), uf.find(b)
        if ra == rb:
            return False
        # One year must not contribute two classes to the same logical type.
        if years_of(ra) & years_of(rb):
            return False
        uf.union(a, b)
        year_cache.pop(ra, None)
        year_cache.pop(rb, None)
        year_cache.pop(uf.find(a), None)
        return True

    # Same curated simple name across years.
    by_name: dict[str, list[JClass]] = defaultdict(list)
    for items in classes.values():
        for jc in items:
            if good_name(jc.simple) or jc.simple in ("Client", "SignLink"):
                by_name[jc.simple].append(jc)
            elif jc.simple in ALIASES and good_name(ALIASES[jc.simple]):
                by_name[ALIASES[jc.simple]].append(jc)
    for group in by_name.values():
        for other in group[1:]:
            safe_union(group[0].key, other.key)
    # Rare file-level strings, adjacent revs and each rev against 317.
    rare: dict[int, dict[str, JClass]] = {}
    for rev, items in classes.items():
        counts: dict[str, int] = defaultdict(int)
        owners: dict[str, JClass] = {}
        for jc in items:
            for s in jc.strings:
                if len(s) >= 12:
                    counts[s] += 1
                    owners[s] = jc
        rare[rev] = {s: owners[s] for s, n in counts.items() if n == 1}

    def score_pair(a: JClass, b: JClass) -> int:
        return len(set(rare[a.rev]) & set(rare[b.rev]) & a.strings & b.strings)

    pairs: list[tuple[int, JClass, JClass]] = []
    revs = [r for r in REVS if r in classes]
    for i, rev in enumerate(revs):
        others = []
        if i + 1 < len(revs):
            others.append(revs[i + 1])
        if rev != 317 and 317 in classes:
            others.append(317)
        for other in others:
            inv: dict[str, list[JClass]] = defaultdict(list)
            for s, jc in rare[other].items():
                inv[s].append(jc)
            tallies: dict[tuple, int] = defaultdict(int)
            for s, jc in rare[rev].items():
                for dest in inv.get(s, ()):
                    tallies[(jc.key, dest.key)] += 1
            for (ka, kb), sc in tallies.items():
                if sc >= 2 or (sc == 1 and any(len(s) >= 24 for s in (by_key[ka].strings & by_key[kb].strings))):
                    pairs.append((sc, by_key[ka], by_key[kb]))
    pairs.sort(key=lambda t: -t[0])
    used_a: set[tuple] = set()
    used_b: set[tuple] = set()
    linked = 0
    for sc, a, b in pairs:
        if a.key in used_a or b.key in used_b:
            continue
        # Keep a one-to-one match per adjacent comparison, but still allow 317 to claim it.
        if a.rev != 317 and b.rev != 317:
            used_a.add(a.key)
            used_b.add(b.key)
        if safe_union(a.key, b.key):
            linked += 1
    # Shared fn method names are the same member, so their classes are the same.
    fn_owners: dict[str, list[JClass]] = defaultdict(list)
    for items in classes.values():
        for jc in items:
            seen = set()
            for m in jc.methods:
                if m.name.startswith("fn") and is_obf(m.name) and m.name not in seen:
                    seen.add(m.name)
                    fn_owners[m.name].append(jc)
    pair_n: dict[tuple, int] = defaultdict(int)
    for owners in fn_owners.values():
        for i, a in enumerate(owners):
            for b in owners[i + 1 :]:
                if a.rev != b.rev:
                    pair_n[(a.key, b.key)] += 1
    fn_links = 0
    for (ka, kb), n in pair_n.items():
        if n >= 2 and safe_union(ka, kb):
            fn_links += 1
    groups: dict[tuple, list[JClass]] = defaultdict(list)
    for key, jc in by_key.items():
        groups[uf.find(key)].append(jc)
    log(f"class clusters={len(groups)} string-links={linked} fn-links={fn_links}")
    return groups


def choose_class_name(members: list[JClass], taken_global: set[str]) -> str:
    ranked = []
    for jc in members:
        raw = preferred_name(jc.simple)
        if good_name(raw) or raw in ("Client", "SignLink"):
            ranked.append((PRIO.get(jc.rev, 0), -jc.rev, raw))
    ranked.sort(reverse=True)
    if ranked:
        return ranked[0][2]
    return ""


def assign_class_names(groups: dict[tuple, list[JClass]]) -> dict[tuple, str]:
    """(rev, package, old simple) -> new simple."""
    proposed: dict[tuple, str] = {}
    minted = 0
    ordered = sorted(groups.items(), key=lambda kv: (min(c.rev for c in kv[1]), min(c.simple for c in kv[1])))
    chosen_for_root: dict[tuple, str] = {}
    for root, members in ordered:
        name = choose_class_name(members, set())
        if not name:
            minted += 1
            blob = "\n".join(sorted({s for jc in members for s in jc.strings if len(s) >= 8}))
            if not blob:
                blob = "|".join(sorted(f"{jc.rev}:{jc.simple}" for jc in members))
            # Stable across the cluster. Never mint kind+counter.
            name = "C" + hashlib.sha1(blob.encode()).hexdigest()[:8]
        chosen_for_root[root] = name
    # Disambiguate inside one package. A suffix is applied to every year so the
    # shared name stays stable.
    used: dict[tuple, set[str]] = defaultdict(set)
    # Seed with names that are not changing.
    for root, members in groups.items():
        new = chosen_for_root[root]
        for jc in members:
            if preferred_name(jc.simple) == new and not is_obf(jc.simple) and jc.simple not in ALIASES:
                used[(jc.rev, jc.package)].add(new)
    final_root: dict[tuple, str] = {}
    for root, members in ordered:
        base = chosen_for_root[root]
        name = base
        # Collision if any year/package already uses this simple name for a different class.
        def clashes(candidate: str) -> bool:
            for jc in members:
                slot = used[(jc.rev, jc.package)]
                if candidate in slot and preferred_name(jc.simple) != candidate:
                    return True
                # Another class in that package already has this source name and is not us.
                if candidate in slot and is_obf(jc.simple):
                    return True
            return False
        if clashes(name):
            n = 2
            while clashes(f"{base}{n}") or f"{base}{n}" in KEYWORDS:
                n += 1
            name = f"{base}{n}"
        final_root[root] = name
        for jc in members:
            used[(jc.rev, jc.package)].add(name)
    out: dict[tuple, str] = {}
    for root, members in groups.items():
        name = final_root[root]
        for jc in members:
            if jc.simple != name:
                out[jc.key] = name
    renamed = sum(1 for v in out.values())
    log(f"class renames={renamed} minted-hashes={minted}")
    return out


def method_clusters(classes: dict[int, list[JClass]], class_of: dict[tuple, tuple]) -> dict[tuple, str]:
    """Return (rev, method ident) -> shared name, only for idents that change."""
    uf = UnionFind()
    decls: dict[tuple, list[JMethod]] = defaultdict(list)
    decl_class: dict[tuple, list[JClass]] = defaultdict(list)
    for rev, items in classes.items():
        for jc in items:
            seen = set()
            for m in jc.methods:
                ident = (rev, m.name)
                uf.add(ident)
                if m.name not in seen:
                    decls[ident].append(m)
                    decl_class[ident].append(jc)
                    seen.add(m.name)
                else:
                    # Same ident declared again (override hierarchy or reuse).
                    decls[ident].append(m)
                    decl_class[ident].append(jc)
    # Identical fn hash is one method across years.
    by_fn: dict[str, list[tuple]] = defaultdict(list)
    for ident in list(uf.p):
        if is_obf(ident[1]) and ident[1].startswith("fn"):
            by_fn[ident[1]].append(ident)
    for group in by_fn.values():
        for other in group[1:]:
            uf.union(group[0], other)
    # Same curated name inside a linked class cluster.
    by_good: dict[tuple, list[tuple]] = defaultdict(list)
    for ident, jcs in decl_class.items():
        name = ident[1]
        if not good_name(name):
            continue
        for jc in jcs:
            by_good[(class_of.get(jc.key, jc.key), name)].append(ident)
    for group in by_good.values():
        for other in group[1:]:
            uf.union(group[0], other)

    def method_strings(ident: tuple) -> set[str]:
        ss: set[str] = set()
        for m in decls.get(ident, ()):
            ss |= m.strings
        return ss

    # String-match obfuscated methods onto curated ones inside linked classes.
    revs = [r for r in REVS if r in classes]
    index: dict[tuple, dict[tuple, list[tuple]]] = defaultdict(lambda: defaultdict(list))
    for ident, jcs in decl_class.items():
        for jc in jcs:
            root = class_of.get(jc.key, jc.key)
            index[root][ident[0]].append(ident)
    matches = 0
    for root, by_rev in index.items():
        for i, rev in enumerate(revs):
            if rev not in by_rev:
                continue
            for other in revs[i + 1 : i + 2]:
                if other not in by_rev:
                    continue
                left = by_rev[rev]
                right = by_rev[other]
                rare_l: dict[str, int] = defaultdict(int)
                rare_r: dict[str, int] = defaultdict(int)
                strs_l = {ident: method_strings(ident) for ident in left}
                strs_r = {ident: method_strings(ident) for ident in right}
                for ss in strs_l.values():
                    for s in ss:
                        if len(s) >= 10:
                            rare_l[s] += 1
                for ss in strs_r.values():
                    for s in ss:
                        if len(s) >= 10:
                            rare_r[s] += 1
                scored = []
                for a in left:
                    sa = {s for s in strs_l[a] if len(s) >= 10 and rare_l[s] == 1 and rare_r.get(s, 0) <= 1}
                    if not sa:
                        continue
                    for b in right:
                        inter = sa & strs_r[b]
                        if not inter:
                            continue
                        # Do not fuse two different curated names.
                        if good_name(a[1]) and good_name(b[1]) and a[1] != b[1]:
                            continue
                        if not (is_obf(a[1]) or is_obf(b[1]) or a[1] in ALIASES or b[1] in ALIASES or a[1] != b[1]):
                            continue
                        scored.append((len(inter), max(len(s) for s in inter), a, b))
                scored.sort(key=lambda t: (-t[0], -t[1]))
                used_a, used_b = set(), set()
                best_for: dict[tuple, tuple] = {}
                second: dict[tuple, int] = {}
                for n, _ln, a, b in scored:
                    if a not in best_for:
                        best_for[a] = (n, b)
                    elif a not in second:
                        second[a] = n
                for n, _ln, a, b in scored:
                    if a in used_a or b in used_b:
                        continue
                    top = best_for.get(a)
                    if not top or top[1] != b:
                        continue
                    if n < 2:
                        continue
                    if second.get(a, 0) >= n:
                        continue
                    uf.union(a, b)
                    used_a.add(a)
                    used_b.add(b)
                    matches += 1
    log(f"method string-matches={matches}")
    groups = uf.groups()
    # Name each cluster.
    cluster_name: dict[tuple, str] = {}
    minted_by_role: dict[str, list[tuple]] = defaultdict(list)
    for root, members in groups.items():
        ranked = []
        for rev, name in members:
            if good_name(name):
                ranked.append((PRIO.get(rev, 0), name))
        if ranked:
            ranked.sort(reverse=True)
            cluster_name[root] = ranked[0][1]
            continue
        # Role from the declaring class's generified name. Filled after class names exist.
        minted_by_role[root] = members
    return groups, cluster_name, minted_by_role, decls, decl_class


def class_root_map(groups: dict[tuple, list[JClass]]) -> dict[tuple, tuple]:
    out = {}
    for root, members in groups.items():
        for jc in members:
            out[jc.key] = root
    return out


def role_for_members(members: list[tuple], decl_class, class_names, class_of) -> str:
    best = None
    for ident in members:
        for jc in decl_class.get(ident, ()):
            new = class_names.get(jc.key, preferred_name(jc.simple))
            if is_obf(new):
                new = "Kind"
            rank = (PRIO.get(jc.rev, 0), new)
            if best is None or rank > best[0]:
                best = (rank, new)
    if not best:
        return "Kind"
    role = re.sub(r"[^A-Za-z0-9_]", "", best[1]) or "Kind"
    if role[0].isdigit():
        role = "K" + role
    return role


def assign_method_names(classes, class_groups, class_names) -> dict[int, dict[str, str]]:
    class_of = class_root_map(class_groups)
    groups, cluster_name, minted, decls, decl_class = method_clusters(classes, class_of)
    # Number minted clusters per role, stable by earliest ident.
    pending = []
    for root, members in minted.items():
        role = role_for_members(members, decl_class, class_names, class_of)
        anchor = min(members)
        pending.append((role, anchor, root, members))
    pending.sort()
    counters: dict[str, int] = defaultdict(int)
    for role, _anchor, root, _members in pending:
        counters[role] += 1
        cluster_name[root] = f"op{role}{counters[role]}"
    # ident -> name, then per-year maps. Keep a name only when it changes the ident.
    per_year: dict[int, dict[str, str]] = defaultdict(dict)
    # Collision bookkeeping: within one class, same source params cannot share a name.
    # Build proposed first, then suffix idents that collide.
    proposed_ident: dict[tuple, str] = {}
    for root, members in groups.items():
        name = cluster_name.get(root)
        if not name or name in KEYWORDS or is_obf(name):
            continue
        for ident in members:
            if ident[1] == name:
                continue
            if not is_obf(ident[1]) and ident[1] not in ALIASES:
                continue
            proposed_ident[ident] = name
    # Source-signature collisions inside one class.
    by_class_methods: dict[tuple, list[tuple[tuple, tuple, str]]] = defaultdict(list)
    for rev, items in classes.items():
        for jc in items:
            for m in jc.methods:
                ident = (rev, m.name)
                new = proposed_ident.get(ident, m.name)
                by_class_methods[jc.key].append((ident, m.params, new))
    bumped: dict[tuple, str] = {}
    for key, rows in by_class_methods.items():
        seen: dict[tuple, tuple] = {}
        for ident, params, new in rows:
            slot = (new, params)
            if slot in seen and seen[slot] != ident:
                # Keep the first ident, suffix this one.
                base = new
                suffix = bumped.get(ident)
                if suffix is None:
                    n = 2
                    candidate = f"{base}b" if n == 2 else f"{base}b{n}"
                    while any(r[2] == candidate and r[1] == params for r in rows) or candidate in KEYWORDS or is_obf(candidate):
                        n += 1
                        candidate = f"{base}b" if n == 2 else f"{base}b{n}"
                    bumped[ident] = candidate
            else:
                seen[slot] = ident
    for ident, name in proposed_ident.items():
        per_year[ident[0]][ident[1]] = bumped.get(ident, name)
    # If a bumped ident shares its old name with an override, the suffix applies
    # everywhere that ident is used, which keeps the hierarchy consistent.
    log(f"method idents renamed={sum(len(v) for v in per_year.values())} signature-suffixes={len(bumped)}")
    return per_year, groups, cluster_name, decls, decl_class


def norm_type(jtype: str, rev: int, class_names_by_simple: dict[tuple, str], class_of_simple: dict[tuple, tuple]) -> str:
    def repl(m: re.Match) -> str:
        name = m.group(0)
        if name in KEYWORDS or name in {"String", "Object", "Integer", "Long", "Boolean", "Iterable", "List", "Map", "Set", "HashMap", "ArrayList", "LinkedList"}:
            return name
        key = (rev, name)
        # class_of_simple keyed by (rev, simple) ignoring package — first hit
        root = class_of_simple.get(key)
        if root:
            return "T" + str(abs(hash(root)) % 10**8)
        mapped = class_names_by_simple.get(key)
        if mapped:
            return mapped
        return name
    return IDENT_RE.sub(repl, jtype)


def assign_field_names(classes, class_groups, class_names, method_groups) -> dict[int, dict[str, str]]:
    class_of = class_root_map(class_groups)
    # method ident -> cluster root
    method_root: dict[tuple, tuple] = {}
    for root, members in method_groups.items():
        for ident in members:
            method_root[ident] = root
    class_of_simple: dict[tuple, tuple] = {}
    class_names_by_simple: dict[tuple, str] = {}
    for root, members in class_groups.items():
        for jc in members:
            class_of_simple[(jc.rev, jc.simple)] = root
            class_names_by_simple[(jc.rev, jc.simple)] = class_names.get(jc.key, preferred_name(jc.simple))
    uf = UnionFind()
    fields: dict[tuple, JField] = {}
    field_class: dict[tuple, JClass] = {}
    for rev, items in classes.items():
        for jc in items:
            for f in jc.fields:
                ident = (rev, f.name)
                # Field names are globally unique inside a year except a handful of
                # inherited re-declarations. First declaration wins the cluster node.
                if ident not in fields:
                    uf.add(ident)
                    fields[ident] = f
                    field_class[ident] = jc
    # Same curated field name inside a class cluster.
    by_good: dict[tuple, list[tuple]] = defaultdict(list)
    for ident, jc in field_class.items():
        if good_name(ident[1]):
            by_good[(class_of.get(jc.key, jc.key), ident[1])].append(ident)
    for group in by_good.values():
        for other in group[1:]:
            uf.union(group[0], other)
    # Unique source type inside a class, linked across the class cluster.
    by_class_fields: dict[tuple, list[tuple]] = defaultdict(list)
    for ident, jc in field_class.items():
        by_class_fields[jc.key].append(ident)
    for root, members in class_groups.items():
        per_year: dict[int, list[tuple]] = defaultdict(list)
        for jc in members:
            for ident in by_class_fields.get(jc.key, ()):
                per_year[jc.rev].append(ident)
        # type -> ident, only when unique in that class-year
        typed: dict[int, dict[str, tuple]] = {}
        for rev, idents in per_year.items():
            counts: dict[str, list[tuple]] = defaultdict(list)
            for ident in idents:
                t = norm_type(fields[ident].jtype, rev, class_names_by_simple, class_of_simple)
                counts[t].append(ident)
            typed[rev] = {t: ids[0] for t, ids in counts.items() if len(ids) == 1 and t not in {"int", "boolean", "byte", "long", "short", "String", "Object"}}
        years = sorted(typed)
        for i, rev in enumerate(years):
            if i + 1 >= len(years):
                break
            other = years[i + 1]
            for t, ident in typed[rev].items():
                other_ident = typed[other].get(t)
                if other_ident:
                    uf.union(ident, other_ident)
    groups = uf.groups()
    cluster_name: dict[tuple, str] = {}
    pending = []
    for root, members in groups.items():
        ranked = []
        for ident in members:
            if good_name(ident[1]):
                ranked.append((PRIO.get(ident[0], 0), ident[1]))
        if ranked:
            ranked.sort(reverse=True)
            cluster_name[root] = ranked[0][1]
        else:
            role = "Kind"
            best = None
            for ident in members:
                jc = field_class.get(ident)
                if not jc:
                    continue
                new = class_names.get(jc.key, preferred_name(jc.simple))
                if is_obf(new) or new in ALIASES:
                    new = class_names.get(jc.key, "Kind")
                rank = (PRIO.get(jc.rev, 0), new)
                if best is None or rank > best:
                    best = rank
                    role = re.sub(r"[^A-Za-z0-9_]", "", new) or "Kind"
            pending.append((role, min(members), root))
    pending.sort()
    counters: dict[str, int] = defaultdict(int)
    for role, _anchor, root in pending:
        counters[role] += 1
        cluster_name[root] = f"value{role}{counters[role]}"
    per_year: dict[int, dict[str, str]] = defaultdict(dict)
    for root, members in groups.items():
        name = cluster_name.get(root)
        if not name or name in KEYWORDS or is_obf(name):
            continue
        for ident in members:
            if ident[1] != name and (is_obf(ident[1]) or ident[1] in ALIASES):
                per_year[ident[0]][ident[1]] = name
    # Duplicate field names inside one class get a suffix.
    bumped = 0
    for rev, items in classes.items():
        for jc in items:
            seen: dict[str, str] = {}
            for f in jc.fields:
                new = per_year[rev].get(f.name, f.name)
                if new in seen and seen[new] != f.name:
                    n = 2
                    candidate = f"{new}b"
                    used = {per_year[rev].get(ff.name, ff.name) for ff in jc.fields}
                    while candidate in used or candidate in KEYWORDS or is_obf(candidate):
                        n += 1
                        candidate = f"{new}b{n}"
                    per_year[rev][f.name] = candidate
                    bumped += 1
                else:
                    seen[new] = f.name
    log(f"field idents renamed={sum(len(v) for v in per_year.values())} suffixes={bumped}")
    return per_year


def fix_cast_calls(text: str) -> str:
    return CAST_CALL_RE.sub(lambda m: f"{m.group(1)}(", text)


JDK_ROOTS = {"java", "javax", "sun", "jdk", "com", "org", "android", "netscape", "javafx"}


def apply_text(text: str, mapping: dict[str, str], keep_jdk: bool = False) -> str:
    if not mapping:
        return fix_cast_calls(text)
    out = []
    tail = ""
    i = 0
    n = len(text)
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n and text[i + 1] == "/":
            end = text.find("\n", i)
            if end < 0:
                end = n
            else:
                end += 1
            out.append(text[i:end])
            tail = (tail + " ")[-200:]
            i = end
            continue
        if ch == "/" and i + 1 < n and text[i + 1] == "*":
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
            out.append(text[i:end])
            tail = (tail + " ")[-200:]
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
            tail = (tail + " ")[-200:]
            i = j
            continue
        if ch == "'":
            j = i + 1
            if j < n and text[j] == "\\":
                j += 2
            if j < n and text[j] != "'":
                j += 1
            if j < n and text[j] == "'":
                j += 1
            out.append(text[i:j])
            tail = (tail + " ")[-200:]
            i = j
            continue
        m = IDENT_RE.match(text, i)
        if m:
            word = m.group(0)
            repl = mapping.get(word, word)
            if keep_jdk and repl != word:
                qual = re.search(r"([A-Za-z_][\w]*)(?:\s*\.\s*[\w$]+)*\s*\.\s*$", tail)
                if qual and qual.group(1) in JDK_ROOTS:
                    repl = word
            out.append(repl)
            tail = (tail + repl)[-200:]
            i = m.end()
            continue
        out.append(ch)
        tail = (tail + ch)[-200:]
        i += 1
    return fix_cast_calls("".join(out))


def sweep_obf(text: str, used: set[str]) -> str:
    found = set(OBF_FIND.findall(text))
    if not found:
        return text
    mapping = {}
    for name in sorted(found, key=len, reverse=True):
        if name.startswith("method"):
            candidate = "opMisc" + name[6:]
        elif name.startswith("field"):
            candidate = "valueMisc" + name[5:]
        elif name.startswith("class"):
            candidate = "unit" + name[5:]
        elif name.startswith("anInt"):
            candidate = "intLocal" + name[5:]
        elif name.startswith("anLong"):
            candidate = "longLocal" + name[6:]
        elif name.startswith("aBoolean"):
            candidate = "boolLocal" + name[8:]
        elif name.startswith("aString"):
            candidate = "stringLocal" + name[7:]
        elif name.startswith("aClass"):
            candidate = "typeLocal" + name[6:]
        elif name.startswith("fn"):
            candidate = "opHex" + name[2:]
        else:
            candidate = "gen" + name
        n = 2
        base = candidate
        while candidate in used or candidate in KEYWORDS or is_obf(candidate):
            candidate = f"{base}x{n}"
            n += 1
        mapping[name] = candidate
        used.add(candidate)

    def repl(match: re.Match) -> str:
        return mapping.get(match.group(0), match.group(0))

    # Comments and string literals still count in a raw identifier scan.
    return OBF_FIND.sub(repl, text)


def public_class_name(text: str) -> str | None:
    m = re.search(
        r"^\s*public\s+(?:(?:abstract|final|static)\s+)*(?:class|interface|enum)\s+([A-Za-z_$][\w$]*)",
        text,
        re.M,
    )
    if m:
        return m.group(1)
    m = CLASS_RE.search(text)
    return m.group(2) if m else None


def rename_file(path: Path, text: str) -> Path:
    name = public_class_name(text)
    if not name or path.stem == name:
        return path
    dest = path.with_name(name + ".java")
    if dest.exists() and dest != path:
        # Keep the source name stable rather than clobbering another type.
        return path
    tmp = path.with_name(path.stem + ".__renaming__.java")
    path.rename(tmp)
    tmp.rename(dest)
    return dest


def _norm_cat(jtype: str) -> str:
    dims = jtype.count("[")
    base = re.sub(r"<.*$", "", jtype.replace("[", "").replace("]", ""))
    base = base.split(".")[-1]
    if base not in {
        "int", "long", "boolean", "byte", "short", "float", "double", "char",
        "void", "String", "Object",
    }:
        base = "R"
    return base + ("[]" * dims)


def _field_sig(jc: JClass) -> tuple:
    counts = Counter(_norm_cat(f.jtype) for f in jc.fields)
    return tuple(sorted(counts.items()))


def _stable_member(name: str) -> bool:
    if not good_name(name) or name.startswith("_flow"):
        return False
    # opRoleN / valueRoleN were minted per year and do not identify a class.
    if len(name) > 3 and name.startswith("op") and name[2].isupper():
        return False
    if len(name) > 6 and name.startswith("value") and name[5].isupper():
        return False
    return True


def _method_names(jc: JClass) -> set[str]:
    return {m.name for m in jc.methods if _stable_member(m.name)}


def _multiset_jaccard(a: Counter, b: Counter) -> float:
    if not a and not b:
        return 0.0
    inter = sum((a & b).values())
    union = sum((a | b).values())
    return inter / union if union else 0.0


_ROLL = 1315423911


def _class_tokens(text: str) -> list[str]:
    out: list[str] = []
    i = 0
    n = len(text)
    while i < n:
        ch = text[i]
        if ch.isspace():
            i += 1
            continue
        if ch == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            i = n if j < 0 else j + 1
            continue
        if ch == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            i = n if j < 0 else j + 2
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
            body = text[i + 1 : max(i + 1, j - 1)]
            out.append("S:" + body[:48] if 4 <= len(body) <= 80 else "S")
            i = max(j, i + 1)
            continue
        if ch == "'":
            j = i + 1
            if j < n and text[j] == "\\":
                j += 2
            elif j < n:
                j += 1
            if j < n and text[j] == "'":
                j += 1
            out.append("C")
            i = j
            continue
        m = IDENT_RE.match(text, i)
        if m:
            word = m.group(0)
            i = m.end()
            if word in KEYWORDS or word in ("this", "super"):
                out.append(word)
            elif i < n and text[i] == "(":
                out.append("M")
            else:
                out.append("I")
            continue
        if ch.isdigit():
            j = i + 1
            while j < n and (text[j].isalnum() or text[j] in "._"):
                j += 1
            out.append("N:" + text[i:j][:18])
            i = j
            continue
        if ch in "{}()[];,.=+-*/%<>!&|^~?:":
            if i + 1 < n:
                two = ch + text[i + 1]
                if two in {
                    "==", "!=", "<=", ">=", "++", "--", "+=", "-=", "*=", "/=",
                    "&&", "||", "<<", ">>", ">>>", "|=", "&=", "^=", "%=", "->",
                }:
                    out.append(two)
                    i += 2
                    continue
            out.append(ch)
            i += 1
            continue
        i += 1
    return out


def _shingles(toks: list[str], k: int = 8) -> set[int]:
    if not toks:
        return set()
    hs = [zlib.crc32(t.encode()) & 0xFFFFFFFF for t in toks]
    if len(hs) < k:
        acc = 0
        for h in hs:
            acc = (acc * _ROLL + h) & 0xFFFFFFFF
        return {acc}
    powk = pow(_ROLL, k, 2**32)
    acc = 0
    out: set[int] = set()
    for i, h in enumerate(hs):
        acc = (acc * _ROLL + h) & 0xFFFFFFFF
        if i >= k:
            acc = (acc - hs[i - k] * powk) & 0xFFFFFFFF
        if i >= k - 1:
            out.add(acc)
    return out


def _jaccard(a: set[int], b: set[int]) -> float:
    if not a or not b:
        return 0.0
    if len(a) > len(b):
        a, b = b, a
    inter = sum(1 for h in a if h in b)
    if not inter:
        return 0.0
    return inter / (len(a) + len(b) - inter)


def _camel_from_literal(raw: str) -> str:
    stop = {
        "the", "a", "an", "of", "to", "and", "or", "for", "in", "on", "at",
        "is", "it", "this", "that", "with", "from", "your", "you", "are",
    }
    parts = []
    for word in re.findall(r"[A-Za-z]+", raw):
        if len(word) < 2 or word.lower() in stop:
            continue
        parts.append(word[:1].upper() + word[1:].lower())
    if not parts:
        return ""
    name = "".join(parts)
    if len(name) > 42:
        name = name[:42]
    return name if valid_ident(name) and name not in PROTECTED_CLASS_NAMES else ""


def _hash_class_name(members: list[JClass]) -> str:
    rare = sorted({s for jc in members for s in jc.strings if 8 <= len(s) <= 120})
    if rare:
        payload = "\n".join(rare)
    else:
        jc = min(members, key=lambda c: (c.rev, c.package, c.simple))
        counts = Counter(_norm_cat(f.jtype) for f in jc.fields)
        payload = "|".join(
            [jc.super_name or "", str(len(jc.methods))]
            + [f"{k}*{v}" for k, v in sorted(counts.items())]
        )
    return "C" + hashlib.sha1(payload.encode()).hexdigest()[:8]


def _choose_shared_class_name(members: list[JClass], df: dict[str, int], reserved: set[str]) -> str:
    # An anchor spelling wins, and it is never rewritten to another year's name.
    anchor = []
    other = []
    for jc in members:
        rn = readable_class_name(jc.simple)
        if not rn:
            continue
        if jc.rev in ANCHOR_REVS:
            anchor.append((PRIO.get(jc.rev, 0), rn))
        else:
            other.append((PRIO.get(jc.rev, 0), rn in PROTECTED_CLASS_NAMES, rn))
    if anchor:
        anchor.sort(reverse=True)
        return anchor[0][1]
    if other:
        other.sort(reverse=True)
        return other[0][2]
    best: tuple | None = None
    seen: set[str] = set()
    cluster_n = len({jc.rev for jc in members})
    for jc in members:
        for raw in jc.strings:
            if raw in seen or not (8 <= len(raw) <= 120):
                continue
            seen.add(raw)
            camel = _camel_from_literal(raw)
            if not camel or camel in reserved or camel in JAVA_LANG_TYPES:
                continue
            outside = df.get(raw, 1) - 1
            if outside > cluster_n + 2:
                continue
            rank = (outside, -len(camel), -len(raw), camel)
            if best is None or rank < best:
                best = rank
    if best:
        return best[-1]
    name = _hash_class_name(members)
    if name in reserved or name in JAVA_LANG_TYPES:
        name += "x"
    return name


def build_shared_clusters(classes: dict[int, list[JClass]]) -> dict[tuple, list[JClass]]:
    """Cluster the same logical class across RS2 years.

    One year contributes at most one class. Readable names link first, then
    distinctive strings, normalized body shingles, and subclasses of a class
    that is already linked. kindN is not a readable name, so it cannot glue
    two different classes together.
    """
    uf = UnionFind()
    by_key: dict[tuple, JClass] = {}
    by_simple: dict[tuple, list[JClass]] = defaultdict(list)
    shingles: dict[tuple, set[int]] = {}
    fields: dict[tuple, Counter] = {}
    for rev, items in classes.items():
        for jc in items:
            uf.add(jc.key)
            by_key[jc.key] = jc
            by_simple[(rev, jc.simple)].append(jc)
            fields[jc.key] = Counter(_norm_cat(f.jtype) for f in jc.fields)
            shingles[jc.key] = _shingles(_class_tokens(jc.path.read_text(errors="ignore")))
    log(f"shingles ready classes={len(by_key)}")
    years_of: dict[tuple, set[int]] = {jc.key: {jc.rev} for jc in by_key.values()}
    names_of: dict[tuple, set[str]] = {}
    members_of: dict[tuple, list[tuple]] = {key: [key] for key in by_key}
    pkg_index: dict[tuple, list[tuple]] = defaultdict(list)
    for key, jc in by_key.items():
        rn = readable_class_name(jc.simple)
        names_of[key] = {rn} if rn else set()
        pkg_index[(jc.rev, jc.package)].append(key)

    def safe_union(a: tuple, b: tuple, strong: bool = False) -> bool:
        """Link two classes. Different readable names never join.

        A kindN (or dumped) class may join a readable cluster only when the
        caller has a unique strong match and that readable name is still free
        in the kind class's package. Anchor spellings stay exact.
        """
        ra, rb = uf.find(a), uf.find(b)
        if ra == rb:
            return False
        if years_of[ra] & years_of[rb]:
            return False
        na, nb = names_of[ra], names_of[rb]
        if na and nb and na != nb:
            return False
        if bool(na) != bool(nb) and not strong:
            return False
        if na and not nb:
            rnames, kroot = na, rb
        elif nb and not na:
            rnames, kroot = nb, ra
        else:
            rnames, kroot = None, None
        if rnames:
            for key in members_of[kroot]:
                jc = by_key[key]
                for other_key in pkg_index[(jc.rev, jc.package)]:
                    if other_key == key:
                        continue
                    oroot = uf.find(other_key)
                    if oroot == kroot:
                        continue
                    if rnames & names_of.get(oroot, set()):
                        return False
                    orn = readable_class_name(by_key[other_key].simple)
                    if orn and orn in rnames:
                        return False
        uf.union(a, b)
        years_of[ra] |= years_of.pop(rb)
        names_of[ra] |= names_of.pop(rb)
        members_of[ra].extend(members_of.pop(rb))
        return True

    revs = [r for r in REVS if r in classes]
    # Same curated simple name. Anchor years always chain. Later years chain
    # only when the bodies still look related, so a bad earlier rename cannot
    # pull an unrelated class into an anchor cluster.
    name_links = 0
    by_exact: dict[str, list[JClass]] = defaultdict(list)
    for jc in by_key.values():
        rn = readable_class_name(jc.simple)
        if rn:
            by_exact[rn].append(jc)
    for group in by_exact.values():
        per_year: dict[int, list[JClass]] = defaultdict(list)
        for jc in group:
            per_year[jc.rev].append(jc)
        singles = [(rev, lst[0]) for rev, lst in per_year.items() if len(lst) == 1]
        singles.sort()
        for i, (rev, jc) in enumerate(singles):
            for orev, other in singles[i + 1 :]:
                gap = revs.index(orev) - revs.index(rev)
                if gap > 3:
                    break
                same_anchor = rev in ANCHOR_REVS and orev in ANCHOR_REVS
                protected = readable_class_name(jc.simple) in PROTECTED_CLASS_NAMES
                jac = _jaccard(shingles[jc.key], shingles[other.key])
                if same_anchor or protected or jac >= 0.10 or gap == 1 and jac >= 0.05:
                    if safe_union(jc.key, other.key):
                        name_links += 1
    # Distinctive string literals. A string owned by one class in each year
    # votes for that pair. High scores are applied first so a chain can form
    # without a one-to-one lock between neighbours.
    owners: dict[int, dict[str, list[JClass]]] = {}
    df: Counter = Counter()
    for rev, items in classes.items():
        om: dict[str, list[JClass]] = defaultdict(list)
        for jc in items:
            for s in jc.strings:
                if len(s) >= 12:
                    om[s].append(jc)
                    df[s] += 1
        owners[rev] = om
    string_links = 0
    for i, rev in enumerate(revs):
        for orev in revs[i + 1 : i + 3]:
            scores: dict[tuple, int] = defaultdict(int)
            str_count: dict[tuple, int] = defaultdict(int)
            for s, lst in owners[rev].items():
                if len(lst) != 1 or len(s) < 12:
                    continue
                other = owners[orev].get(s)
                if not other or len(other) != 1:
                    continue
                if df[s] > 8:
                    continue
                pair = (lst[0].key, other[0].key)
                scores[pair] += len(s)
                str_count[pair] += 1
            order = sorted(scores.items(), key=lambda kv: -kv[1])
            best_a: dict[tuple, int] = {}
            best_b: dict[tuple, int] = {}
            second_a: dict[tuple, int] = {}
            for (ka, kb), sc in order:
                if ka not in best_a:
                    best_a[ka] = sc
                elif ka not in second_a:
                    second_a[ka] = sc
                if kb not in best_b:
                    best_b[kb] = sc
            for (ka, kb), sc in order:
                if sc < 24:
                    break
                if best_a.get(ka) != sc or best_b.get(kb, 0) < sc:
                    continue
                if second_a.get(ka, 0) * 1.5 >= sc and second_a.get(ka, 0) > 0:
                    continue
                adopting = bool(names_of[uf.find(ka)]) != bool(names_of[uf.find(kb)])
                # One shared literal is not enough to take over a readable name.
                if adopting and (str_count[(ka, kb)] < 2 or sc < 48):
                    continue
                if safe_union(ka, kb, strong=True):
                    string_links += 1

    def link_structure(left: int, right: int, thr: float, ratio: float) -> int:
        inv: dict[int, list[int]] = defaultdict(list)
        right_items = classes[right]
        for idx, jc in enumerate(right_items):
            for h in shingles[jc.key]:
                inv[h].append(idx)
        linked = 0
        for jc in classes[left]:
            scores: dict[int, int] = defaultdict(int)
            sh = shingles[jc.key]
            if not sh:
                continue
            for h in sh:
                for idx in inv.get(h, ()):
                    scores[idx] += 1
            if not scores:
                continue
            top = sorted(scores.items(), key=lambda kv: -kv[1])[:2]
            bi, inter = top[0]
            other = right_items[bi]
            union = len(sh) + len(shingles[other.key]) - inter
            jac = inter / union if union else 0.0
            jac2 = 0.0
            if len(top) > 1:
                bi2, inter2 = top[1]
                other2 = right_items[bi2]
                union2 = len(sh) + len(shingles[other2.key]) - inter2
                jac2 = inter2 / union2 if union2 else 0.0
            adopting = bool(names_of[uf.find(jc.key)]) != bool(names_of[uf.find(other.key)])
            if adopting:
                # A kindN class takes a readable name only when this body is
                # clearly closer than every other readable class.
                if jac < 0.55:
                    continue
                if len(top) > 1:
                    other2 = right_items[top[1][0]]
                    other2_named = bool(names_of[uf.find(other2.key)]) or bool(readable_class_name(other2.simple))
                    if other2_named and jac2 >= 0.40:
                        continue
                if jac2 > 0 and jac < jac2 * 1.6:
                    continue
            else:
                if jac < thr:
                    continue
                if jac2 > 0 and jac < jac2 * ratio:
                    continue
            if safe_union(jc.key, other.key, strong=True):
                linked += 1
        return linked

    struct_links = 0
    for i, rev in enumerate(revs[:-1]):
        struct_links += link_structure(rev, revs[i + 1], 0.40, 1.45)
    gap_links = 0
    for i, rev in enumerate(revs[:-2]):
        gap_links += link_structure(rev, revs[i + 2], 0.55, 1.6)

    def resolve_super(jc: JClass) -> tuple | None:
        if not jc.super_name or jc.super_name in JDK_SUPERS:
            if jc.super_name in JDK_SUPERS:
                return ("jdk", jc.super_name)
            return None
        cands = by_simple.get((jc.rev, jc.super_name), [])
        if len(cands) == 1:
            return uf.find(cands[0].key)
        same = [c for c in cands if c.package == jc.package]
        if len(same) == 1:
            return uf.find(same[0].key)
        return None

    extend_links = 0
    for _round in range(4):
        gained = 0
        for i, rev in enumerate(revs[:-1]):
            other = revs[i + 1]
            buckets_a: dict[tuple, list[JClass]] = defaultdict(list)
            buckets_b: dict[tuple, list[JClass]] = defaultdict(list)
            for jc in classes[rev]:
                sup = resolve_super(jc)
                # JDK supers are not a client class we have linked across years.
                if isinstance(sup, tuple) and sup and sup[0] != "jdk":
                    buckets_a[sup].append(jc)
            for jc in classes[other]:
                sup = resolve_super(jc)
                if isinstance(sup, tuple) and sup and sup[0] != "jdk":
                    buckets_b[sup].append(jc)
            for sup, left in buckets_a.items():
                right = buckets_b.get(sup)
                if not right or len(left) > 80 or len(right) > 80:
                    continue
                scored = []
                for a in left:
                    fa = fields[a.key]
                    ma = _method_names(a)
                    for b in right:
                        fj = _multiset_jaccard(fa, fields[b.key])
                        mb = _method_names(b)
                        mj = len(ma & mb) / len(ma | mb) if ma and mb else 0.0
                        shared_f = sum((fa & fields[b.key]).values())
                        shared_m = len(ma & mb)
                        score = max(fj, mj)
                        if score >= 0.6 and (shared_f >= 3 or shared_m >= 2):
                            scored.append((score, a.key, b.key))
                scored.sort(reverse=True)
                used_a: set[tuple] = set()
                used_b: set[tuple] = set()
                best_a = {}
                second = {}
                for score, ka, kb in scored:
                    if ka not in best_a:
                        best_a[ka] = (score, kb)
                    elif ka not in second:
                        second[ka] = score
                for score, ka, kb in scored:
                    if ka in used_a or kb in used_b:
                        continue
                    top = best_a.get(ka)
                    if not top or top[1] != kb:
                        continue
                    if second.get(ka, 0) >= score * 0.9 and second.get(ka, 0) > 0:
                        continue
                    if safe_union(ka, kb, strong=True):
                        used_a.add(ka)
                        used_b.add(kb)
                        gained += 1
        extend_links += gained
        if not gained:
            break

    sig_links = 0
    for i, rev in enumerate(revs[:-1]):
        other = revs[i + 1]
        sig_a: dict[tuple, list[JClass]] = defaultdict(list)
        sig_b: dict[tuple, list[JClass]] = defaultdict(list)
        for jc in classes[rev]:
            sig = _field_sig(jc)
            if sum(n for _k, n in sig) >= 4:
                sig_a[sig].append(jc)
        for jc in classes[other]:
            sig = _field_sig(jc)
            if sum(n for _k, n in sig) >= 4:
                sig_b[sig].append(jc)
        for sig, lst in sig_a.items():
            if len(lst) != 1:
                continue
            match = sig_b.get(sig)
            if not match or len(match) != 1:
                continue
            a, b = lst[0], match[0]
            jac = _jaccard(shingles[a.key], shingles[b.key])
            if jac < 0.12:
                continue
            adopting = bool(names_of[uf.find(a.key)]) != bool(names_of[uf.find(b.key)])
            if adopting and jac < 0.45:
                continue
            if safe_union(a.key, b.key, strong=not adopting or jac >= 0.45):
                sig_links += 1
    groups: dict[tuple, list[JClass]] = defaultdict(list)
    for key, jc in by_key.items():
        groups[uf.find(key)].append(jc)
    span = sum(1 for members in groups.values() if len({jc.rev for jc in members}) >= 2)
    mixed = 0
    for members in groups.values():
        readable = {readable_class_name(jc.simple) for jc in members}
        readable.discard(None)
        if len(readable) > 1:
            mixed += 1
    log(f"mixed-readable-clusters={mixed}")
    log(
        f"class clusters={len(groups)} span2+={span} name-links={name_links} "
        f"string-links={string_links} struct-links={struct_links} gap-links={gap_links} "
        f"extend-links={extend_links} sig-links={sig_links}"
    )
    return groups


def assign_shared_class_names(groups: dict[tuple, list[JClass]]) -> dict[tuple, str]:
    df: Counter = Counter()
    reserved: set[str] = set(JAVA_LANG_TYPES)
    for members in groups.values():
        for jc in members:
            for s in jc.strings:
                if len(s) >= 8:
                    df[s] += 1
            rn = readable_class_name(jc.simple)
            if rn:
                reserved.add(rn)
    bases: dict[tuple, str] = {}
    for root, members in groups.items():
        name = _choose_shared_class_name(members, df, reserved)
        if name in PROTECTED_CLASS_NAMES and not any(readable_class_name(jc.simple) == name for jc in members):
            name = _hash_class_name(members)
            if name in reserved:
                name += "x"
        bases[root] = name
    final = dict(bases)
    next_n: dict[str, int] = defaultdict(lambda: 2)

    def holds_readable(root: tuple, name: str) -> bool:
        # A cluster that already has this readable spelling keeps it.
        # Suffixing would rename Sprite, JagFile, LookatPoint, and the anchors.
        return any(readable_class_name(jc.simple) == name for jc in groups[root])

    def fresh(base: str) -> str:
        n = next_n[base]
        while True:
            cand = f"{base}{n}"
            n += 1
            if valid_ident(cand) and cand not in reserved and cand not in JAVA_LANG_TYPES:
                next_n[base] = n
                return cand

    for _guard in range(60):
        slots: dict[tuple, list[tuple]] = defaultdict(list)
        for root, members in groups.items():
            name = final[root]
            for jc in members:
                slots[(jc.rev, jc.package, name)].append(root)
        changed = False
        bumped: set[tuple] = set()
        for key in sorted(slots, key=lambda k: (k[0], k[1], k[2])):
            roots = slots[key]
            uniq = list(dict.fromkeys(roots))
            if len(uniq) <= 1:
                continue
            name = key[2]
            rev, package, _name = key

            def rank(root: tuple) -> tuple:
                members = groups[root]
                has_exact = any(
                    jc.simple == name and jc.rev == rev and jc.package == package for jc in members
                )
                has_readable = holds_readable(root, name)
                return (has_readable, has_exact, max(PRIO.get(jc.rev, 0) for jc in members), root)

            uniq.sort(key=rank, reverse=True)
            for root in uniq[1:]:
                if root in bumped or holds_readable(root, name):
                    continue
                final[root] = fresh(bases[root])
                bumped.add(root)
                changed = True
        if not changed:
            break
    return final


# Single-type imports hide a same-package class. On-demand imports (java.lang,
# import java.awt.*) do not. A renamed simple name has to stay unambiguous in
# each file: qualify it, or move the pre-existing JDK use to its FQCN.
JAVA_LANG_TYPES = {
    "Object", "String", "Class", "System", "Math", "Integer", "Long", "Boolean",
    "Byte", "Short", "Float", "Double", "Character", "Number", "Enum", "Void",
    "Error", "Throwable", "Exception", "RuntimeException", "Thread", "Runnable",
    "Iterable", "Comparable", "Override", "Deprecated", "SuppressWarnings",
    "StringBuilder", "StringBuffer", "Package", "Process", "Runtime",
    "ProcessBuilder", "ThreadGroup", "SecurityManager", "ClassLoader",
    "StrictMath", "Cloneable", "AutoCloseable", "Readable", "Appendable",
    "CharSequence", "Annotation", "ExceptionInInitializerError",
    "IllegalArgumentException", "IllegalStateException",
    "IndexOutOfBoundsException", "ArrayIndexOutOfBoundsException",
    "StringIndexOutOfBoundsException", "NegativeArraySizeException",
    "NullPointerException", "ClassCastException", "ArithmeticException",
    "UnsupportedOperationException", "NumberFormatException",
    "InterruptedException", "CloneNotSupportedException", "SecurityException",
    "IllegalMonitorStateException", "IllegalThreadStateException",
    "ClassNotFoundException", "NoSuchMethodException", "NoSuchFieldException",
    "InstantiationException", "LinkageError", "VirtualMachineError",
    "OutOfMemoryError", "StackOverflowError", "ThreadDeath", "AssertionError",
    "ClassCircularityError", "ClassFormatError", "IncompatibleClassChangeError",
    "NoClassDefFoundError", "UnsatisfiedLinkError", "VerifyError",
    "InternalError", "UnknownError",
}
JAVA_AWT_TYPES = {
    "Component", "Font", "FontMetrics", "Frame", "Canvas", "Graphics", "Color",
    "Image", "Point", "Dimension", "Rectangle", "Polygon", "Toolkit", "Cursor",
    "Container", "Window", "Dialog", "Panel", "Button", "Label", "Event",
    "Insets", "Menu", "Checkbox", "Choice", "TextField", "TextArea",
    "Scrollbar", "BorderLayout", "FlowLayout", "GridLayout", "GridBagLayout",
    "CardLayout", "MediaTracker", "EventQueue", "FileDialog", "PopupMenu",
    "MenuBar", "MenuItem", "CheckboxMenuItem", "SystemColor", "Graphics2D",
    "AWTEvent", "AWTException", "ImageObserver", "LayoutManager",
    "AlphaComposite", "BasicStroke", "RenderingHints", "KeyboardFocusManager",
    "GraphicsDevice", "GraphicsEnvironment", "GraphicsConfiguration",
    "DisplayMode", "Desktop", "SystemTray", "TrayIcon", "HeadlessException",
    "IllegalComponentStateException", "FontFormatException", "PrintJob",
    "GradientPaint", "TexturePaint", "Composite", "Paint", "Stroke", "Shape",
    "Robot", "PointerInfo",
}
JAVA_UTIL_TYPES = {
    "List", "Map", "Set", "Date", "Random", "Arrays", "Collections", "Calendar",
    "Timer", "TimerTask", "HashMap", "ArrayList", "HashSet", "Vector",
    "Hashtable", "Properties", "Locale", "UUID", "Optional", "Queue", "Deque",
    "Iterator", "Comparator", "Scanner", "BitSet", "Stack", "Dictionary",
    "Observable", "Observer", "TimeZone", "ResourceBundle", "StringTokenizer",
    "GregorianCalendar", "LinkedList", "TreeMap", "TreeSet", "WeakHashMap",
    "IdentityHashMap", "EnumMap", "EnumSet", "PriorityQueue", "ArrayDeque",
    "LinkedHashMap", "LinkedHashSet", "EventObject", "Objects",
}
JAVA_IO_TYPES = {
    "File", "InputStream", "OutputStream", "Reader", "Writer",
    "BufferedReader", "BufferedWriter", "InputStreamReader", "OutputStreamWriter",
    "FileInputStream", "FileOutputStream", "FileReader", "FileWriter",
    "PrintStream", "PrintWriter", "IOException", "Serializable",
    "DataInputStream", "DataOutputStream", "ByteArrayInputStream",
    "ByteArrayOutputStream", "FilterInputStream", "FilterOutputStream",
}
JAVA_NET_TYPES = {
    "URL", "URI", "Socket", "ServerSocket", "InetAddress", "URLConnection",
    "HttpURLConnection", "UnknownHostException", "SocketException",
    "MalformedURLException",
}
JAVA_APPLET_TYPES = {"Applet", "AppletContext", "AppletStub", "AudioClip"}
JAVA_ANNOTATION_TYPES = {
    "Target", "Retention", "Documented", "Inherited", "ElementType",
    "RetentionPolicy", "Repeatable", "Native",
}
JAVAX_SWING_TYPES = {
    "JFrame", "JPanel", "JLabel", "JButton", "JApplet", "SwingUtilities",
    "JComponent", "JDialog", "JWindow", "JMenu", "JMenuBar", "JMenuItem",
    "JOptionPane", "JTextField", "JScrollPane", "JList", "JTable", "JTree",
    "JComboBox", "JCheckBox", "JRadioButton", "JFileChooser", "Timer",
}
_IMPORT_RE = re.compile(
    r"^[ \t]*import[ \t]+(?P<static>static[ \t]+)?"
    r"(?P<name>[A-Za-z_][\w]*(?:\.[A-Za-z_][\w]*)*)"
    r"(?P<star>\.\*)?[ \t]*;",
    re.M,
)
_ON_DEMAND = (
    ("java.awt", JAVA_AWT_TYPES),
    ("java.util", JAVA_UTIL_TYPES),
    ("java.io", JAVA_IO_TYPES),
    ("java.net", JAVA_NET_TYPES),
    ("java.applet", JAVA_APPLET_TYPES),
    ("java.lang.annotation", JAVA_ANNOTATION_TYPES),
    ("javax.swing", JAVAX_SWING_TYPES),
)


def _parse_type_imports(text: str) -> tuple[dict[str, str], set[str]]:
    singles: dict[str, str] = {}
    stars: set[str] = set()
    for match in _IMPORT_RE.finditer(text):
        if match.group("static"):
            continue
        name = match.group("name")
        if match.group("star"):
            stars.add(name)
            continue
        if "." not in name:
            continue
        pkg, simple = name.rsplit(".", 1)
        singles[simple] = pkg
    return singles, stars


def _on_demand(simple: str, stars: set[str]) -> str | None:
    if simple in JAVA_LANG_TYPES:
        return "java.lang"
    for pkg, names in _ON_DEMAND:
        if pkg in stars and simple in names:
            return pkg
    return None


def _tail_has_pkg(tail: str, package: str) -> bool:
    if not package:
        return False
    body = r"\s*\.\s*".join(re.escape(part) for part in package.split("."))
    return re.search(r"(?:^|[^\w$])" + body + r"\s*\.\s*$", tail) is not None


def _tail_jdk(tail: str) -> bool:
    match = re.search(r"([A-Za-z_][\w]*)(?:\s*\.\s*[\w$]+)*\s*\.\s*$", tail)
    return bool(match and match.group(1) in JDK_ROOTS)


def _needs_qual(
    cpkg: str,
    new: str,
    is_own: bool,
    file_pkg: str,
    own_new: str | None,
    singles: dict[str, str],
    stars: set[str],
    local_names: set[str],
    users: dict[str, set[str]],
) -> bool:
    if is_own or not cpkg:
        return False
    if len(users.get(new, ())) > 1 and file_pkg != cpkg:
        return True
    single = singles.get(new)
    if single and single != cpkg:
        return True
    if own_new == new and file_pkg != cpkg:
        return True
    if new in local_names and file_pkg != cpkg:
        return True
    if new in local_names and file_pkg == cpkg:
        return False
    if not single and _on_demand(new, stars) and file_pkg != cpkg:
        return True
    return False


def _choose_pkg(
    word: str,
    pkgmap: dict[str, str],
    file_pkg: str,
    own_old: str | None,
    singles: dict[str, str],
    stars: set[str],
) -> str | None:
    # In its own compilation unit the unqualified name is the class being declared.
    if own_old == word and file_pkg in pkgmap:
        return file_pkg
    imported = singles.get(word)
    if imported in pkgmap:
        return imported
    if imported is not None and imported not in pkgmap:
        return None
    if file_pkg in pkgmap:
        return file_pkg
    if len(pkgmap) == 1:
        only = next(iter(pkgmap))
        if _on_demand(word, stars) and file_pkg != only:
            return None
        return only
    return None


def _drop_import_lines(text: str, drop: set[tuple[str, str]]) -> str:
    if not drop:
        return text
    kept = []
    for line in text.splitlines(True):
        body = line.rstrip("\r\n")
        match = _IMPORT_RE.match(body)
        if match and not match.group("static") and not match.group("star"):
            full = match.group("name")
            if "." in full:
                pkg, simple = full.rsplit(".", 1)
                if (pkg, simple) in drop:
                    continue
        kept.append(line)
    return "".join(kept)


def _rewrite_idents(text: str, repl) -> str:
    out = []
    tail = ""
    i = 0
    n = len(text)
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n and text[i + 1] == "/":
            end = text.find("\n", i)
            end = n if end < 0 else end + 1
            out.append(text[i:end])
            tail = (tail + " ")[-400:]
            i = end
            continue
        if ch == "/" and i + 1 < n and text[i + 1] == "*":
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
            out.append(text[i:end])
            tail = (tail + " ")[-400:]
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
            tail = (tail + " ")[-400:]
            i = j
            continue
        if ch == "'":
            j = i + 1
            if j < n and text[j] == "\\":
                j += 2
            if j < n and text[j] != "'":
                j += 1
            if j < n and text[j] == "'":
                j += 1
            out.append(text[i:j])
            tail = (tail + " ")[-400:]
            i = j
            continue
        match = IDENT_RE.match(text, i)
        if match:
            word = match.group(0)
            piece = repl(word, tail)
            out.append(piece)
            tail = (tail + piece)[-400:]
            i = match.end()
            continue
        out.append(ch)
        tail = (tail + ch)[-400:]
        i += 1
    return "".join(out)


def apply_scoped_renames(
    text: str,
    file_pkg: str,
    own_old: str | None,
    own_new: str | None,
    groups: dict[str, dict[str, str]],
    local_names: set[str],
    users: dict[str, set[str]],
) -> str:
    """Rename class idents. `groups` is old simple -> {package: new simple}."""
    if not groups:
        return text
    singles, stars = _parse_type_imports(text)
    steal: dict[str, str] = {}
    drop: set[tuple[str, str]] = set()
    for old, pkgmap in groups.items():
        for cpkg, new in pkgmap.items():
            if new == old:
                continue
            is_own = own_old == old and file_pkg == cpkg
            single = singles.get(new)
            ondemand = None if single else _on_demand(new, stars)
            if is_own and single and single != cpkg:
                steal[new] = f"{single}.{new}"
                drop.add((single, new))
            elif is_own and ondemand:
                steal[new] = f"{ondemand}.{new}"
            elif cpkg == file_pkg and ondemand:
                steal.setdefault(new, f"{ondemand}.{new}")
            elif cpkg == "" and file_pkg == "" and single:
                steal[new] = f"{single}.{new}"
                drop.add((single, new))
            elif cpkg == "" and file_pkg == "" and ondemand:
                steal.setdefault(new, f"{ondemand}.{new}")
            if singles.get(old) == cpkg and _needs_qual(
                cpkg, new, is_own, file_pkg, own_new, singles, stars, local_names, users
            ):
                drop.add((cpkg, new))
            if is_own:
                for simple, pkg in singles.items():
                    if simple == new:
                        drop.add((pkg, new))
                for other_old, other_map in groups.items():
                    for other_pkg, other_new in other_map.items():
                        if other_new == new and singles.get(other_old) == other_pkg:
                            drop.add((other_pkg, new))

    def repl(word: str, tail: str) -> str:
        pkgmap = groups.get(word)
        if pkgmap:
            matched = None
            for pkg in sorted(pkgmap, key=len, reverse=True):
                if pkg and _tail_has_pkg(tail, pkg):
                    matched = pkg
                    break
            if matched is not None:
                return pkgmap[matched]
            if _tail_jdk(tail):
                return word
            if re.search(r"\.\s*$", tail):
                if len(pkgmap) == 1:
                    return next(iter(pkgmap.values()))
                return word
            chosen = _choose_pkg(word, pkgmap, file_pkg, own_old, singles, stars)
            if chosen is None:
                return word
            new = pkgmap[chosen]
            is_own = own_old == word and file_pkg == chosen
            if _needs_qual(chosen, new, is_own, file_pkg, own_new, singles, stars, local_names, users):
                return f"{chosen}.{new}"
            return new
        stolen = steal.get(word)
        if stolen and not _tail_has_pkg(tail, stolen.rsplit(".", 1)[0]):
            return stolen
        return word

    return fix_cast_calls(_drop_import_lines(_rewrite_idents(text, repl), drop))


def _scoped_rename_self_test() -> None:
    def run(text, pkg, own_old, own_new, groups, local, users):
        return apply_scoped_renames(text, pkg, own_old, own_new, groups, local, users)

    out = run(
        "package deob;\nimport java.awt.Component;\nclass Client {\n  Component x = new kind2();\n}\n",
        "deob", "Client", "Client",
        {"kind2": {"deob": "Component"}},
        {"Client", "Component"},
        {"Component": {"deob"}, "Client": {"deob"}},
    )
    assert "import java.awt.Component;" in out, out
    assert "new deob.Component()" in out, out
    assert "Component x" in out, out

    out = run(
        "package deob;\nimport java.awt.Component;\npublic class kind2 {\n  public kind2() {}\n  public Component peer;\n}\n",
        "deob", "kind2", "Component",
        {"kind2": {"deob": "Component"}},
        {"Component"},
        {"Component": {"deob"}},
    )
    assert "import java.awt.Component;" not in out, out
    assert "public class Component" in out, out
    assert "public Component()" in out, out
    assert "java.awt.Component peer" in out, out

    out = run(
        "package rt4;\nimport deob.kind155;\npublic class GameShell extends Component {\n  kind155 x;\n}\n",
        "rt4", "GameShell", "GameShell",
        {"kind155": {"deob": "GameShell"}},
        {"GameShell", "Component"},
        {"GameShell": {"rt4", "deob"}, "Component": {"rt4"}},
    )
    assert "import deob.GameShell;" not in out and "import deob.kind155;" not in out, out
    assert "deob.GameShell x" in out, out
    assert "class GameShell extends Component" in out, out

    out = run(
        "package rt4;\nimport deob.kind600;\npublic class SoundPcmStream extends MixerPcmStream {\n  kind600 a;\n}\n",
        "rt4", "SoundPcmStream", "SoundPcmStream",
        {"kind600": {"deob": "MixerPcmStream"}},
        {"MixerPcmStream", "SoundPcmStream"},
        {"MixerPcmStream": {"deob", "rt4"}},
    )
    assert "extends MixerPcmStream" in out, out
    assert "deob.MixerPcmStream a" in out, out
    assert "import deob." not in out, out

    out = run(
        "package deob;\nimport java.awt.Component;\nclass Foo {\n  java.awt.Component c;\n  Component d;\n}\n",
        "deob", "Foo", "Foo",
        {"Component": {"deob": "Tone"}},
        {"Foo", "Tone"},
        {"Tone": {"deob"}},
    )
    assert "java.awt.Component c" in out, out
    assert "Component d" in out, out
    assert "Tone" not in out, out

    out = run(
        "package deob;\nclass Foo {\n  Component d;\n}\n",
        "deob", "Foo", "Foo",
        {"Component": {"deob": "Tone"}},
        {"Foo", "Tone"},
        {"Tone": {"deob"}},
    )
    assert "Tone d" in out, out

    out = run(
        "import java.awt.Font;\nclass Statics {\n  Font awt;\n  kind9 rs;\n}\n",
        "", "Statics", "Statics",
        {"kind9": {"": "Font"}},
        {"Statics", "Font"},
        {"Font": {""}},
    )
    assert "import java.awt.Font;" not in out, out
    assert "java.awt.Font awt" in out, out
    assert "Font rs" in out, out

    out = run(
        'package deob;\nclass Foo {\n  // kind2\n  String s = "kind2";\n  kind2 a;\n}\n',
        "deob", "Foo", "Foo",
        {"kind2": {"deob": "Widget"}},
        {"Foo", "Widget"},
        {"Widget": {"deob"}},
    )
    assert "// kind2" in out and '"kind2"' in out and "Widget a" in out, out

    out = run(
        "package jagex2.client;\nimport jagex2.graphics.Sprite;\nclass Client {\n  Sprite ui;\n  jagex2.dash3d.Sprite scene;\n}\n",
        "jagex2.client", "Client", "Client",
        {"Sprite": {"jagex2.graphics": "Sprite", "jagex2.dash3d": "Sprite2"}},
        {"Client"},
        {"Sprite": {"jagex2.graphics"}, "Sprite2": {"jagex2.dash3d"}},
    )
    assert "import jagex2.graphics.Sprite;" in out, out
    assert "Sprite ui" in out, out
    assert "jagex2.dash3d.Sprite2 scene" in out, out

    out = run(
        "package jagex2.dash3d;\npublic final class Sprite {\n  Sprite copy;\n}\n",
        "jagex2.dash3d", "Sprite", "Sprite2",
        {"Sprite": {"jagex2.graphics": "Sprite", "jagex2.dash3d": "Sprite2"}},
        {"Sprite2"},
        {"Sprite": {"jagex2.graphics"}, "Sprite2": {"jagex2.dash3d"}},
    )
    assert "class Sprite2" in out and "Sprite2 copy" in out, out
    log("scoped-rename self-test ok")


def _cluster_samples(groups: dict[tuple, list[JClass]], names: dict[tuple, str], limit: int = 10) -> list[str]:
    rows = []
    for root, members in groups.items():
        years = {jc.rev for jc in members}
        if len(years) < 2:
            continue
        if not any(is_obf(jc.simple) or dumped_class_name(jc.simple) for jc in members):
            continue
        shared = names[root]
        bits = []
        for jc in sorted(members, key=lambda c: c.rev):
            if jc.simple != shared:
                bits.append(f"{jc.rev}:{jc.simple}")
        if bits:
            rows.append((len(years), shared, ", ".join(bits)))
    rows.sort(key=lambda r: (-r[0], r[1]))
    lines = []
    for span, shared, bits in rows[:limit]:
        lines.append(f"  {shared} <= {bits}")
    return lines


def share_class_names(revs: list[int], apply: bool) -> None:
    classes = load_all(revs)
    groups = build_shared_clusters(classes)
    names = assign_shared_class_names(groups)
    span = sum(1 for members in groups.values() if len({jc.rev for jc in members}) >= 2)
    log(f"clusters={len(groups)} span2+={span}")
    root_of: dict[tuple, tuple] = {}
    for root, members in groups.items():
        for jc in members:
            root_of[jc.key] = root
    anchor_changes = []
    readable_changes = 0
    for rev, items in classes.items():
        for jc in items:
            new = names[root_of[jc.key]]
            if jc.simple == new:
                continue
            if readable_class_name(jc.simple):
                readable_changes += 1
                if jc.rev in ANCHOR_REVS:
                    anchor_changes.append(f"{jc.rev}:{jc.simple}->{new}")
    log(f"anchor readable renames={len(anchor_changes)} other-readable-renames={readable_changes - len(anchor_changes)}")
    for line in anchor_changes[:30]:
        log(f"  anchor {line}")
    log("sample renames (old kind name per year -> shared name):")
    for line in _cluster_samples(groups, names):
        log(line)
    kind_total = 0
    kind_shared = 0
    for rev, items in classes.items():
        for jc in items:
            if not (is_obf(jc.simple) or dumped_class_name(jc.simple)):
                continue
            kind_total += 1
            members = groups[root_of[jc.key]]
            if len({m.rev for m in members}) >= 2:
                kind_shared += 1
    log(f"obf-classes={kind_total} in-multi-year-clusters={kind_shared}")
    collisions = 0
    for rev, items in classes.items():
        used: dict[tuple, str] = {}
        for jc in items:
            new = names[root_of[jc.key]]
            slot = (jc.package, new)
            prev = used.get(slot)
            if prev is not None and prev != jc.simple:
                collisions += 1
                if collisions <= 15:
                    log(f"  collision {rev} {jc.package} {prev} and {jc.simple} -> {new}")
            else:
                used[slot] = jc.simple
    log(f"package-name-collisions={collisions}")
    lang_hits = sorted({n for n in names.values() if n in JAVA_LANG_TYPES})
    log(f"shared-names-matching-java.lang={lang_hits}")
    if not apply:
        log("dry-run only; sources not written")
        return
    for rev, items in classes.items():
        by_old: dict[str, dict[str, str]] = defaultdict(dict)
        users: dict[str, set[str]] = defaultdict(set)
        finals_by_pkg: dict[str, set[str]] = defaultdict(set)
        by_path = {jc.path: jc for jc in items}
        for jc in items:
            new = names[root_of[jc.key]]
            by_old[jc.simple][jc.package] = new
            users[new].add(jc.package)
            finals_by_pkg[jc.package].add(new)
        groups: dict[str, dict[str, str]] = {}
        changed = 0
        for old, pkgmap in by_old.items():
            if any(new != old for new in pkgmap.values()):
                groups[old] = dict(pkgmap)
                changed += sum(1 for new in pkgmap.values() if new != old)
                if len(pkgmap) > 1:
                    for pkg, new in sorted(pkgmap.items()):
                        if new != old:
                            log(f"scoped {rev} {pkg}.{old} -> {new}")
        claimed: dict[tuple, str] = {}
        for jc in items:
            new = by_old[jc.simple][jc.package]
            prev = claimed.get((jc.package, new))
            if prev is not None and prev != jc.simple:
                raise SystemExit(f"name collision in {rev} {jc.package}: {prev} and {jc.simple} -> {new}")
            claimed[(jc.package, new)] = jc.simple
        paths = java_files(rev)
        written = []
        for path in paths:
            text = path.read_text(errors="ignore")
            jc = by_path.get(path)
            if jc is None:
                file_pkg = ""
                pm = re.search(r"^package\s+([\w.]+)\s*;", text, re.M)
                if pm:
                    file_pkg = pm.group(1)
                own_old = None
                own_new = None
                local = finals_by_pkg.get(file_pkg, set())
            else:
                file_pkg = jc.package
                own_old = jc.simple
                own_new = by_old[jc.simple][jc.package]
                local = finals_by_pkg.get(file_pkg, set())
            nt = apply_scoped_renames(text, file_pkg, own_old, own_new, groups, local, users) if groups else text
            if nt != text:
                path.write_text(nt)
            new_name = public_class_name(nt)
            if new_name and path.stem != new_name:
                written.append((path, path.with_name(new_name + ".java")))
        temps = []
        for idx, (src, dest) in enumerate(written):
            tmp = src.with_name(f"__tmp_rename_{idx}__.java")
            if tmp.exists():
                raise SystemExit(f"temp exists {tmp}")
            src.rename(tmp)
            temps.append((tmp, dest))
        for tmp, dest in temps:
            if dest.exists():
                raise SystemExit(f"refusing to clobber {dest}")
            tmp.rename(dest)
        log(f"renamed {rev}: idents={changed} files={len(written)}")


def carry_report(classes, method_groups, cluster_name) -> list[str]:
    lines = []
    # Find 317 methods with the curated names and see which revs share the cluster.
    wanted = {}
    for rev, items in classes.items():
        if rev != 317:
            continue
        for jc in items:
            for m in jc.methods:
                if m.name in CARRY and m.name not in wanted:
                    wanted[m.name] = (317, m.name)
    rev_of_cluster: dict[str, set[int]] = {name: set() for name in CARRY}
    for root, members in method_groups.items():
        shared = cluster_name.get(root)
        names = {ident[1] for ident in members}
        for carry in CARRY:
            if carry in names or shared == carry:
                rev_of_cluster[carry] |= {ident[0] for ident in members}
    for name in CARRY:
        present = sorted(rev_of_cluster.get(name, ()))
        missing = [r for r in REVS if r not in present]
        lines.append(f"{name}: years={present} not-carried={missing}")
    return lines


def generify(revs: list[int]) -> None:
    for path in [p for rev in revs for p in java_files(rev)]:
        text = path.read_text(errors="ignore")
        nt = fix_cast_calls(text)
        if nt != text:
            path.write_text(nt)
    classes = load_all(revs)
    cgroups = class_clusters(classes)
    class_names = assign_class_names(cgroups)
    method_maps, mgroups, cluster_name, _decls, _decl_class = assign_method_names(classes, cgroups, class_names)
    field_maps = assign_field_names(classes, cgroups, class_names, mgroups)
    for line in carry_report(classes, mgroups, cluster_name):
        log(line)
    for rev, items in classes.items():
        mapping: dict[str, str] = {}
        mapping.update(field_maps.get(rev, {}))
        mapping.update(method_maps.get(rev, {}))
        for jc in items:
            new = class_names.get(jc.key)
            if new and new != jc.simple:
                mapping[jc.simple] = new
        if not mapping:
            # Still sweep leftovers such as names the parser did not see.
            pass
        used = set(mapping.values())
        for jc in items:
            used.add(class_names.get(jc.key, jc.simple))
            for m in jc.methods:
                used.add(mapping.get(m.name, m.name))
            for f in jc.fields:
                used.add(mapping.get(f.name, f.name))
        changed_files = 0
        for path in java_files(rev):
            text = path.read_text(errors="ignore")
            nt = apply_text(text, mapping)
            nt = sweep_obf(nt, used)
            if nt != text:
                path.write_text(nt)
                changed_files += 1
            rename_file(path, nt if nt != text else text)
        log(f"wrote {rev}: files-touched={changed_files} map={len(mapping)}")


def scan(revs: list[int]) -> int:
    total = 0
    for rev in revs:
        hits = 0
        class_files = 0
        for path in java_files(rev):
            if re.fullmatch(r"(?:class|kind)\d+\.java", path.name):
                class_files += 1
            text = path.read_text(errors="ignore")
            hits += len(OBF_FIND.findall(text))
        log(f"scan {rev}: obf-idents={hits} classN-files={class_files}")
        total += hits + class_files
    return total


def main() -> int:
    args = sys.argv[1:]
    if "--self-test" in args:
        _scoped_rename_self_test()
        return 0
    revs = REVS
    if "--rev" in args:
        i = args.index("--rev")
        revs = [int(x) for x in args[i + 1].split(",")]
    if "--scan" in args:
        return 1 if scan(revs) else 0
    apply = "--apply" in args
    if "--share-classes" in args:
        # Dry-run unless --apply is also present. --dry-run still wins.
        share_class_names(revs, apply=apply and "--dry-run" not in args)
        if not apply or "--dry-run" in args:
            return 0
        return 1 if scan(revs) else 0
    if not apply:
        print("refusing to write generified names; pass --apply")
        return 2
    generify(revs)
    left = scan(revs)
    return 1 if left else 0


if __name__ == "__main__":
    sys.exit(main())
