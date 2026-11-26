#!/usr/bin/env python3
"""Replace Cls/hex RS3 identifiers with shared readable names.

910's real names win. Earlier classes match a 910 type by a distinctive
string (length >= 8, no '(') when that string identifies one class on each
side. Hex fields and m-hash methods inside a matched class take the 910 name
when that type or signature is unique; otherwise a camelCase name from a
distinctive string. The same token keeps one name in every year it occurs.
"""
from __future__ import annotations

import argparse
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

BUILDS = ROOT / "builds" / "rs3"
REMAP_DIR = ROOT / "tools" / "rs-deob" / "remap"
JAVA_HOME = str(jdk.java_home())
REVS = [790, 876, 887, 902, 910]
FOLDERS = {
    790: "790-2013-12-16",
    876: "876-2016-11-28",
    887: "887-2017-11-01",
    902: "902-2018-12-14",
    910: "910-2019-10-16",
}
PRIO = {910: 100, 902: 80, 887: 60, 876: 40, 790: 20}
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
SKIP_TYPES = {
    "int", "boolean", "byte", "long", "short", "float", "double", "char", "void",
    "String", "Object", "Integer", "Long", "Boolean", "Byte", "Short", "Float",
    "Double", "Character", "Class",
}
BAD_RE = re.compile(
    r"^Cls[0-9a-f]{6,}$"
    r"|^(?:method|field|class|anInt|anLong|aBoolean|aString|aClass)\d+$"
    r"|^fn[0-9a-f]{8,}$"
    r"|^[a-f0-9]{8,}$"
    r"|^m[0-9a-f]{8,}$"
)
IDENT_RE = re.compile(r"[A-Za-z_$][A-Za-z0-9_$]*")
STR_RE = re.compile(r'"(?:\\.|[^"\\])*"')
CLASS_RE = re.compile(
    r"\b(class|interface|enum)\s+([A-Za-z_$][\w$]*)"
    r"(?:\s+extends\s+([\w.$]+))?"
)
SIG_RE = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|native|synchronized|abstract|default)\s+)*"
    r"(?:[\w.$]+(?:\s*<[^>;{}]*>)?(?:\s*\[\s*\])*\s+)?"
    r"([A-Za-z_$][\w$]*)\s*\(([^)]*)\)\s*(?:throws\s+[\w.,\s]+)?(?:\{|;)\s*$"
)
FIELD_RE = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|volatile|transient)\s+)+"
    r"([\w.$]+(?:\s*<[^>;{}]*>)?(?:\s*\[\s*\])*)\s+"
    r"([A-Za-z_$][\w$]*)\s*(?:=|;)"
)


def log(msg: str) -> None:
    print(msg, flush=True)


def is_bad(name: str) -> bool:
    return bool(name) and bool(BAD_RE.match(name))


def good_name(name: str) -> bool:
    if not name or name in KEYWORDS or is_bad(name):
        return False
    if not re.match(r"^[A-Za-z_][A-Za-z0-9_]*$", name):
        return False
    if len(name) <= 2:
        return False
    if name.startswith("Cls") and re.fullmatch(r"Cls[0-9a-f]{6,}", name):
        return False
    return True


def java_root(rev: int) -> Path:
    return BUILDS / FOLDERS[rev] / "src" / "src" / "main" / "java"


def distinctive(body: str) -> bool:
    if "(" in body or "\\" in body or len(body) < 8 or len(body) > 160:
        return False
    if body.strip() == "":
        return False
    return True


def strings_in(text: str) -> set[str]:
    found = set()
    for raw in STR_RE.findall(text):
        body = raw[1:-1]
        if distinctive(body):
            found.add(body)
    return found


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
        types.append("".join(toks[:-1]) if len(toks) >= 2 else toks[0])
    return tuple(types)


def camel(text: str, pascal: bool) -> str:
    words = re.findall(r"[A-Za-z][A-Za-z0-9]{1,}", text)
    stop = {
        "the", "and", "for", "null", "true", "false", "string", "error",
        "exception", "this", "that", "with", "from", "into", "your", "class",
    }
    words = [w for w in words if w.lower() not in stop]
    if not words:
        return ""
    parts = words[:4]
    if pascal:
        name = "".join(p[:1].upper() + p[1:] for p in parts)
    else:
        name = parts[0][:1].lower() + parts[0][1:]
        name += "".join(p[:1].upper() + p[1:] for p in parts[1:])
    name = re.sub(r"[^A-Za-z0-9_]", "", name)
    if not name or not name[0].isalpha():
        return ""
    if name in KEYWORDS or is_bad(name) or len(name) < 3:
        return ""
    return name[:48]


def sanitize(name: str, pascal: bool) -> str:
    name = re.sub(r"[^A-Za-z0-9_]", "", name)
    if not name:
        name = "Value"
    if name[0].isdigit():
        name = ("T" if pascal else "t") + name
    if pascal:
        name = name[:1].upper() + name[1:]
    elif name[0].isupper() and not name.isupper():
        name = name[:1].lower() + name[1:]
    if name in KEYWORDS or is_bad(name):
        name = ("T" if pascal else "t") + name
    return name


def alloc(base: str, taken: set[str]) -> str:
    base = sanitize(base, base[:1].isupper())
    if base not in taken and base not in KEYWORDS and not is_bad(base):
        taken.add(base)
        return base
    n = 2
    while True:
        cand = f"{base}{n}"
        if cand not in taken and cand not in KEYWORDS and not is_bad(cand):
            taken.add(cand)
            return cand
        n += 1


@dataclass
class JMethod:
    name: str
    params: tuple[str, ...]
    strings: set[str] = field(default_factory=set)


@dataclass
class JField:
    name: str
    jtype: str
    strings: set[str] = field(default_factory=set)


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


def strip_line(line: str, state: dict) -> str:
    out = []
    i = 0
    n = len(line)
    block = state.get("block", False)
    while i < n:
        if block:
            end = line.find("*/", i)
            if end < 0:
                state["block"] = True
                return ""
            i = end + 2
            block = False
            continue
        ch = line[i]
        if ch == "/" and i + 1 < n and line[i + 1] == "/":
            break
        if ch == "/" and i + 1 < n and line[i + 1] == "*":
            block = True
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
            out.append(" ")
            i = j
            continue
        out.append(ch)
        i += 1
    state["block"] = False
    return "".join(out)


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
    methods: list[JMethod] = []
    fields: list[JField] = []
    seen_m: set[tuple] = set()
    seen_f: set[str] = set()
    for line in text.splitlines():
        fm = FIELD_RE.match(line)
        if fm and fm.group(2) not in KEYWORDS and fm.group(2) not in seen_f:
            seen_f.add(fm.group(2))
            fields.append(JField(fm.group(2), re.sub(r"\s+", "", fm.group(1)), strings_in(line)))
        sm = SIG_RE.match(line)
        if sm and sm.group(1) not in KEYWORDS and sm.group(1) != simple:
            key = (sm.group(1), sm.group(2) or "")
            if key in seen_m:
                continue
            seen_m.add(key)
            methods.append(JMethod(sm.group(1), split_params(sm.group(2) or ""), strings_in(line)))
    return JClass(rev, path, package, simple, super_name, strings_in(text), methods, fields)


def load_all() -> dict[int, list[JClass]]:
    out: dict[int, list[JClass]] = {}
    for rev in REVS:
        classes = []
        root = java_root(rev)
        for path in root.rglob("*.java"):
            if "fernflower" in path.parts or "build" in path.parts:
                continue
            jc = parse_file(rev, path)
            if jc:
                classes.append(jc)
        out[rev] = classes
        log(f"parsed {rev}: {len(classes)} classes")
    return out


def class_groups(classes: dict[int, list[JClass]]) -> dict[tuple, list[JClass]]:
    uf = UnionFind()
    by_key = {}
    for items in classes.values():
        for jc in items:
            uf.add(jc.key)
            by_key[jc.key] = jc
    years: dict[tuple, set[int]] = {}

    def years_of(root: tuple) -> set[int]:
        cached = years.get(root)
        if cached is not None:
            return cached
        found = {k[0] for k in uf.p if uf.find(k) == root}
        years[root] = found
        return found

    def safe_union(a: tuple, b: tuple) -> bool:
        ra, rb = uf.find(a), uf.find(b)
        if ra == rb:
            return False
        if years_of(ra) & years_of(rb):
            return False
        uf.union(a, b)
        for key in (ra, rb, uf.find(a)):
            years.pop(key, None)
        return True

    by_name: dict[str, list[JClass]] = defaultdict(list)
    for items in classes.values():
        for jc in items:
            if good_name(jc.simple):
                by_name[jc.simple].append(jc)
    named = 0
    for group in by_name.values():
        base = group[0]
        for other in group[1:]:
            if safe_union(base.key, other.key):
                named += 1

    rare: dict[int, dict[str, JClass]] = {}
    for rev, items in classes.items():
        counts: dict[str, int] = defaultdict(int)
        owner: dict[str, JClass] = {}
        for jc in items:
            for s in jc.strings:
                counts[s] += 1
                owner[s] = jc
        rare[rev] = {s: owner[s] for s, n in counts.items() if n == 1}

    pairs: list[tuple[int, int, JClass, JClass]] = []
    for i, rev in enumerate(REVS):
        targets = []
        if i + 1 < len(REVS):
            targets.append(REVS[i + 1])
        if rev != 910:
            targets.append(910)
        for other in targets:
            inv: dict[str, JClass] = rare[other]
            tallies: dict[tuple, int] = defaultdict(int)
            longest: dict[tuple, int] = defaultdict(int)
            for s, jc in rare[rev].items():
                dest = inv.get(s)
                if not dest or dest.simple == jc.simple:
                    continue
                if not (is_bad(jc.simple) or is_bad(dest.simple)):
                    continue
                key = (jc.key, dest.key)
                tallies[key] += 1
                longest[key] = max(longest[key], len(s))
            for key, sc in tallies.items():
                if sc >= 2 or longest[key] >= 24:
                    pairs.append((sc, longest[key], by_key[key[0]], by_key[key[1]]))
    pairs.sort(key=lambda t: (-t[0], -t[1]))
    linked = 0
    used: set[tuple] = set()
    for _sc, _ln, a, b in pairs:
        if a.key in used or b.key in used:
            continue
        if a.rev != 910 and b.rev != 910:
            used.add(a.key)
            used.add(b.key)
        if safe_union(a.key, b.key):
            linked += 1
    groups: dict[tuple, list[JClass]] = defaultdict(list)
    for key, jc in by_key.items():
        groups[uf.find(key)].append(jc)
    log(f"class clusters={len(groups)} named-links={named} string-links={linked}")
    return groups


def choose_class_name(members: list[JClass]) -> str:
    ranked = []
    for jc in members:
        if good_name(jc.simple):
            ranked.append((PRIO[jc.rev], jc.simple))
    if ranked:
        ranked.sort(reverse=True)
        return ranked[0][1]
    pool: set[str] = set()
    for jc in members:
        pool |= jc.strings
    best = ""
    best_len = 0
    for s in pool:
        name = camel(s, True)
        if name and len(s) > best_len:
            best = name
            best_len = len(s)
    if best:
        return best
    supers = [jc.super_name for jc in members if jc.super_name and good_name(jc.super_name)]
    if supers:
        return supers[0] + "Part"
    return "DataBlock"


def assign_class_names(groups: dict[tuple, list[JClass]]) -> dict[tuple, str]:
    chosen: dict[tuple, str] = {}
    for root, members in groups.items():
        if any(is_bad(jc.simple) for jc in members):
            chosen[root] = choose_class_name(members)
        else:
            chosen[root] = ""
    taken_global: dict[str, tuple] = {}
    # Good names already occupy their simple name.
    for root, members in groups.items():
        for jc in members:
            if good_name(jc.simple):
                taken_global.setdefault(jc.simple, root)
    final: dict[tuple, str] = {}
    ordered = sorted(
        ((root, members) for root, members in groups.items() if chosen.get(root)),
        key=lambda kv: (-max(PRIO[c.rev] for c in kv[1]), kv[1][0].simple),
    )
    for root, _members in ordered:
        base = sanitize(chosen[root], True)
        owner = taken_global.get(base)
        if owner in (None, root):
            final[root] = base
            taken_global[base] = root
            continue
        n = 2
        while True:
            cand = f"{base}{n}"
            if taken_global.get(cand) in (None, root) and not is_bad(cand):
                final[root] = cand
                taken_global[cand] = root
                break
            n += 1
    out: dict[tuple, str] = {}
    for root, members in groups.items():
        name = final.get(root)
        if not name:
            continue
        for jc in members:
            if jc.simple != name and is_bad(jc.simple):
                out[jc.key] = name
    log(f"class renames={len(out)}")
    return out


def class_root_of(groups: dict[tuple, list[JClass]]) -> dict[tuple, tuple]:
    out = {}
    for root, members in groups.items():
        for jc in members:
            out[jc.key] = root
    return out


def simple_type(jtype: str) -> str:
    t = re.sub(r"<.*?>", "", jtype)
    t = t.replace("[]", "Array")
    t = t.split(".")[-1]
    return re.sub(r"[^A-Za-z0-9]", "", t)


def assign_members(classes, groups, class_names) -> dict[int, dict[str, str]]:
    """Per-year token -> new name for bad methods and fields."""
    class_of = class_root_of(groups)
    # token -> list of (rev, class, kind, extra)
    token_sites: dict[str, list[tuple]] = defaultdict(list)
    for rev, items in classes.items():
        for jc in items:
            for m in jc.methods:
                if is_bad(m.name):
                    token_sites[m.name].append((rev, jc, "m", m))
            for f in jc.fields:
                if is_bad(f.name):
                    token_sites[f.name].append((rev, jc, "f", f))

    # Link tokens that are the same member: same bad token already, plus
    # unique type / unique signature / shared distinctive string inside a class cluster.
    uf = UnionFind()
    linked_names: set[str] = set(token_sites)
    for token in token_sites:
        uf.add(("t", token))

    def link_tokens(a: str, b: str) -> None:
        if not a or not b or a == b:
            return
        if good_name(a) and good_name(b):
            return
        linked_names.add(a)
        linked_names.add(b)
        uf.union(("t", a), ("t", b))

    # Index members by class root and year.
    by_root: dict[tuple, dict[int, JClass]] = defaultdict(dict)
    for root, members in groups.items():
        for jc in members:
            # One class per year. If duplicates, keep the bad-named one so it can be renamed.
            prev = by_root[root].get(jc.rev)
            if prev is None or (is_bad(jc.simple) and not is_bad(prev.simple)):
                by_root[root][jc.rev] = jc

    string_links = 0
    type_links = 0
    for root, by_rev in by_root.items():
        years = [r for r in REVS if r in by_rev]
        for i, rev in enumerate(years):
            if i + 1 >= len(years):
                break
            other = years[i + 1]
            a = by_rev[rev]
            b = by_rev[other]
            # Methods by rare strings.
            def meth_map(jc: JClass):
                counts: dict[str, int] = defaultdict(int)
                owner: dict[str, str] = {}
                for m in jc.methods:
                    for s in m.strings:
                        if len(s) >= 8:
                            counts[s] += 1
                            owner[s] = m.name
                return {s: owner[s] for s, n in counts.items() if n == 1}

            ma, mb = meth_map(a), meth_map(b)
            votes: dict[tuple[str, str], int] = defaultdict(int)
            long: dict[tuple[str, str], int] = defaultdict(int)
            for s, name in ma.items():
                other_name = mb.get(s)
                if not other_name or other_name == name:
                    continue
                if good_name(name) and good_name(other_name) and name != other_name:
                    continue
                if not (is_bad(name) or is_bad(other_name)):
                    continue
                votes[(name, other_name)] += 1
                long[(name, other_name)] = max(long[(name, other_name)], len(s))
            for (na, nb), sc in votes.items():
                if sc >= 2 or long[(na, nb)] >= 20:
                    link_tokens(na, nb)
                    string_links += 1
            # Unique parameter lists.
            def sigs(jc: JClass) -> dict[tuple, str]:
                counts: dict[tuple, list[str]] = defaultdict(list)
                for m in jc.methods:
                    if m.params:
                        counts[m.params].append(m.name)
                return {sig: names[0] for sig, names in counts.items() if len(names) == 1 and len(sig) >= 1}

            sa, sb = sigs(a), sigs(b)
            for sig, na in sa.items():
                nb = sb.get(sig)
                if nb and na != nb and (is_bad(na) or is_bad(nb)):
                    if good_name(na) and good_name(nb):
                        continue
                    link_tokens(na, nb)
                    type_links += 1
            # Unique field types.
            def types(jc: JClass) -> dict[str, str]:
                counts: dict[str, list[str]] = defaultdict(list)
                for f in jc.fields:
                    st = simple_type(f.jtype)
                    if st and st not in SKIP_TYPES and not is_bad(st):
                        counts[st].append(f.name)
                return {t: names[0] for t, names in counts.items() if len(names) == 1}

            ta, tb = types(a), types(b)
            for t, na in ta.items():
                nb = tb.get(t)
                if nb and na != nb and (is_bad(na) or is_bad(nb)):
                    if good_name(na) and good_name(nb):
                        continue
                    link_tokens(na, nb)
                    type_links += 1
    log(f"member string-links={string_links} type-links={type_links}")

    clusters: dict[tuple, list[str]] = defaultdict(list)
    info: dict[str, list[tuple]] = token_sites
    for token in linked_names:
        clusters[uf.find(("t", token))].append(token)

    # Preferred name per cluster.
    preferred: dict[str, str] = {}
    taken_names: set[str] = set()
    # Reserve good names so we don't mint over them unnecessarily... minting the
    # same simple name in different classes is legal, so taken_names is only used
    # to keep minted names from being pure hex / keywords, not globally unique.
    for root, tokens in clusters.items():
        ranked = []
        strings: set[str] = set()
        kind = "f"
        sample_type = "value"
        sample_class = "Data"
        for token in tokens:
            if good_name(token):
                ranked.append((100, token))
                continue
            if token not in info:
                continue
            kind = info[token][0][2]
            for rev, jc, k, member in info[token]:
                if k == "m":
                    strings |= member.strings
                    sample_class = class_names.get(jc.key, jc.simple)
                else:
                    strings |= member.strings
                    sample_type = simple_type(member.jtype) or "value"
                    sample_class = class_names.get(jc.key, jc.simple)
        if ranked:
            ranked.sort(reverse=True)
            name = ranked[0][1]
        else:
            best = ""
            best_len = 0
            for s in strings:
                made = camel(s, False)
                if made and len(s) > best_len:
                    best = made
                    best_len = len(s)
            if best:
                name = best
            elif kind == "m":
                base = sanitize(sample_class, False)
                name = "do" + base[:1].upper() + base[1:]
            else:
                base = sanitize(sample_type or "value", False)
                if base in KEYWORDS or base in SKIP_TYPES:
                    base = base + "Value"
                name = base[:1].lower() + base[1:]
        name = sanitize(name, False)
        if is_bad(name) or name in KEYWORDS:
            name = "valueItem"
        preferred[root] = name

    token_name = {token: preferred[root] for root, tokens in clusters.items() for token in tokens}

    # Disambiguate inside each class (fields by name, methods by name+params).
    def bump(token: str, blocked: set[str]) -> str:
        base = token_name[token]
        n = 2
        while True:
            cand = f"{base}{n}"
            if cand not in blocked and cand not in KEYWORDS and not is_bad(cand):
                token_name[token] = cand
                return cand
            n += 1

    for _round in range(8):
        changed = False
        for items in classes.values():
            for jc in items:
                field_seen: dict[str, str] = {}
                for f in jc.fields:
                    name = token_name.get(f.name, f.name)
                    if name in field_seen and field_seen[name] != f.name and is_bad(f.name):
                        bump(f.name, set(field_seen))
                        name = token_name[f.name]
                        changed = True
                    field_seen[name] = f.name
                meth_seen: dict[tuple, str] = {}
                for m in jc.methods:
                    name = token_name.get(m.name, m.name)
                    slot = (name, m.params)
                    if slot in meth_seen and meth_seen[slot] != m.name and is_bad(m.name):
                        bump(m.name, {s[0] for s in meth_seen})
                        name = token_name[m.name]
                        slot = (name, m.params)
                        changed = True
                    meth_seen[slot] = m.name
        if not changed:
            break

    per_year: dict[int, dict[str, str]] = defaultdict(dict)
    for token, sites in token_sites.items():
        new = token_name.get(token, token)
        if not new or new == token or is_bad(new) or new in KEYWORDS:
            continue
        for rev, _jc, _k, _m in sites:
            per_year[rev][token] = new
    log(f"member tokens renamed={sum(len(v) for v in per_year.values())}")
    return per_year


def apply_text(text: str, mapping: dict[str, str]) -> str:
    if not mapping:
        return text
    out = []
    i = 0
    n = len(text)
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n and text[i + 1] == "/":
            end = text.find("\n", i)
            end = n if end < 0 else end + 1
            out.append(text[i:end])
            i = end
            continue
        if ch == "/" and i + 1 < n and text[i + 1] == "*":
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
            if j < n and text[j] != "'":
                j += 1
            if j < n and text[j] == "'":
                j += 1
            out.append(text[i:j])
            i = j
            continue
        m = IDENT_RE.match(text, i)
        if m:
            word = m.group(0)
            out.append(mapping.get(word, word))
            i = m.end()
            continue
        out.append(ch)
        i += 1
    return "".join(out)


def rewrite(classes, class_names, member_names) -> None:
    for rev, items in classes.items():
        mapping = dict(member_names.get(rev, {}))
        for jc in items:
            new = class_names.get(jc.key)
            if new and new != jc.simple:
                mapping[jc.simple] = new
        root = java_root(rev)
        written = 0
        planned: list[tuple[Path, Path, str]] = []
        used_dest: set[Path] = set()
        for path in root.rglob("*.java"):
            if "fernflower" in path.parts or "build" in path.parts:
                continue
            text = path.read_text(errors="ignore")
            new_text = apply_text(text, mapping)
            cm = CLASS_RE.search(new_text)
            simple = cm.group(2) if cm else path.stem
            dest = path.with_name(simple + ".java")
            if dest in used_dest and dest != path:
                simple = simple + "Alt"
                new_text = new_text.replace(f"class {cm.group(2)}", f"class {simple}", 1) if cm else new_text
                dest = path.with_name(simple + ".java")
            used_dest.add(dest)
            planned.append((path, dest, new_text))
            written += 1
        temps = []
        for i, (src, dest, text) in enumerate(planned):
            dest.parent.mkdir(parents=True, exist_ok=True)
            tmp = dest.with_name(f".{i}.{dest.name}.rs3name")
            tmp.write_text(text)
            temps.append((src, dest, tmp))
        new_paths = {dest for _s, dest, _t in temps}
        for src, dest, tmp in temps:
            if src != dest and src.exists() and src not in new_paths:
                src.unlink()
            if dest.exists() and dest != tmp:
                dest.unlink()
            tmp.rename(dest)
        log(f"rewrote {rev}: files={written} mapped={len(mapping)}")


def update_remaps(member_names, class_names) -> None:
    class_by_rev: dict[int, dict[str, str]] = defaultdict(dict)
    for key, name in class_names.items():
        class_by_rev[key[0]][key[2]] = name
    for rev in REVS:
        path = REMAP_DIR / f"rs3-{rev}.txt"
        if not path.exists():
            continue
        mapping = {}
        mapping.update(member_names.get(rev, {}))
        mapping.update(class_by_rev.get(rev, {}))
        out = []
        for raw in path.read_text(errors="ignore").splitlines():
            body = raw.split("#", 1)[0].strip()
            if "=" not in body:
                out.append(raw)
                continue
            left, right = body.split("=", 1)
            if "," in right:
                out.append(body)
                continue
            def repl(m: re.Match) -> str:
                return mapping.get(m.group(0), m.group(0))
            new_right = IDENT_RE.sub(repl, right)
            out.append(f"{left}={new_right}")
        path.write_text("\n".join(out) + "\n")
        log(f"remap rs3-{rev}.txt updated")


def scan() -> dict[int, dict[str, int]]:
    pats = {
        "Cls": re.compile(r"\bCls[0-9a-f]{6,}\b"),
        "hex": re.compile(r"\b[a-f0-9]{8,}\b"),
        "field": re.compile(r"\bfield\d+\b"),
        "method": re.compile(r"\bmethod\d+\b"),
        "classn": re.compile(r"\bclass\d+\b"),
        "fn": re.compile(r"\bfn[0-9a-f]{8}\b"),
        "mhex": re.compile(r"\bm[0-9a-f]{8,}\b"),
    }
    out = {}
    for rev in REVS:
        counts = {k: 0 for k in pats}
        # Identifier scan: skip strings and comments.
        for path in java_root(rev).rglob("*.java"):
            text = path.read_text(errors="ignore")
            masked = apply_text(text, {})  # no, need mask
            counts = counts
            blanked = []
            i = 0
            n = len(text)
            buf = []
            while i < n:
                ch = text[i]
                if ch == "/" and i + 1 < n and text[i + 1] == "/":
                    end = text.find("\n", i)
                    end = n if end < 0 else end + 1
                    buf.append(" " * (end - i))
                    i = end
                    continue
                if ch == "/" and i + 1 < n and text[i + 1] == "*":
                    end = text.find("*/", i + 2)
                    end = n if end < 0 else end + 2
                    buf.append(" " * (end - i))
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
                    buf.append(" " * (j - i))
                    i = j
                    continue
                buf.append(ch)
                i += 1
            code = "".join(buf)
            for key, pat in pats.items():
                counts[key] += len(pat.findall(code))
        out[rev] = counts
        log(f"scan {rev} {counts}")
    return out


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()
    classes = load_all()
    groups = class_groups(classes)
    class_names = assign_class_names(groups)
    # Class names must be visible while naming members.
    for key, name in class_names.items():
        for jc in classes[key[0]]:
            if jc.key == key:
                jc.simple = name if False else jc.simple
    member_names = assign_members(classes, groups, class_names)
    if not args.apply:
        log("dry-run")
        return 0
    rewrite(classes, class_names, member_names)
    update_remaps(member_names, class_names)
    scan()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
