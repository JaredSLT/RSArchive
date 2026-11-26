#!/usr/bin/env python3
"""Share one name for the same RS3 class, field, and method across revisions.

910's readable name wins, then 902, 887, 876, 790. 831 and 860 join a class
only when a distinctive string (length >= 8, no '(') is unique to one class
on each side. Members keep a name only when the descriptor matches and they
share a distinctive string or an already-readable name. Fields also need the
same initializer unless the name is already readable and shared. No new
prefix is minted. String and char literals are not edited.

Dry-run by default. --apply rewrites sources. A pre-rename snapshot is stored
under work/share-rs3/backup/<folder>/pre-rename-src and is not overwritten.
"""
from __future__ import annotations

import argparse
import json
import re
import shutil
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILDS = ROOT / "builds" / "rs3"
BACKUP = ROOT / "work" / "share-rs3" / "backup"
REVS = [790, 831, 860, 876, 887, 902, 910]
ANCHOR = [790, 876, 887, 902, 910]
FOLDERS = {
    790: "790-2013-12-16",
    831: "831-2014-12-04",
    860: "860-2015-12-01",
    876: "876-2016-11-28",
    887: "887-2017-11-01",
    902: "902-2018-12-14",
    910: "910-2019-10-16",
}
PRIO = {910: 70, 902: 60, 887: 50, 876: 40, 790: 30, 860: 20, 831: 10}
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
CALLBACKS = {"run", "paint", "update", "init", "start", "stop", "destroy"}
OBJECT_METHODS = {"toString", "equals", "hashCode", "finalize", "clone"}
PRIM = {
    "int", "boolean", "byte", "long", "short", "float", "double", "char", "void",
}
IDENT_RE = re.compile(r"[A-Za-z_$][A-Za-z0-9_$]*")
SYN_CLASS_RE = re.compile(r"^(?:class\d+|DataBlock\d+|Cls[0-9a-fA-F]{6,})$")
SYN_MEMBER_RE = re.compile(
    r"^(?:method\d+|field\d+|class\d+|DataBlock\d+"
    r"|do[A-Za-z0-9_]*\d+"
    r"|Cls[0-9a-fA-F]{6,}"
    r"|m[0-9a-fA-F]{8,}"
    r"|fn[0-9a-fA-F]{8,})$"
)


def log(msg: str) -> None:
    print(msg, flush=True)


def readable(name: str) -> bool:
    if not name or name in KEYWORDS or not re.match(r"^[A-Za-z_$][A-Za-z0-9_$]*$", name):
        return False
    return SYN_MEMBER_RE.match(name) is None and SYN_CLASS_RE.match(name) is None


def synthetic_class(name: str) -> bool:
    return bool(SYN_CLASS_RE.match(name))


def synthetic_member(name: str) -> bool:
    return bool(SYN_MEMBER_RE.match(name))


def callback_name(name: str) -> bool:
    if name in CALLBACKS or name in OBJECT_METHODS:
        return True
    low = name.lower()
    return low.startswith("key") or low.startswith("mouse") or low.startswith("focus")


def distinctive(body: str) -> bool:
    if len(body) < 8 or len(body) > 400 or "(" in body:
        return False
    if not any(ch.isalpha() for ch in body):
        return False
    return True


def java_root(rev: int) -> Path:
    return BUILDS / FOLDERS[rev] / "src" / "src" / "main" / "java"


@dataclass
class JMethod:
    name: str
    descriptor: str
    arity: int
    is_static: bool
    strings: set[str] = field(default_factory=set)
    readable: bool = False


@dataclass
class JField:
    name: str
    descriptor: str
    is_static: bool
    init: str
    readable: bool = False


@dataclass
class JClass:
    rev: int
    path: Path
    package: str
    simple: str
    supers: list[str]
    strings: set[str]
    methods: list[JMethod]
    fields: list[JField]
    top: bool

    @property
    def key(self) -> tuple:
        return (self.rev, self.package, self.simple)


class UnionFind:
    def __init__(self) -> None:
        self.parent: dict[int, int] = {}
        self.year: dict[int, int] = {}
        self.label: dict[int, str | None] = {}
        self.years: dict[int, set[int]] = {}
        self.labels: dict[int, set[str]] = {}
        self.members: dict[int, list[int]] = {}

    def add(self, i: int, year: int, label: str | None) -> None:
        self.parent[i] = i
        self.year[i] = year
        self.label[i] = label
        self.years[i] = {year}
        self.labels[i] = {label} if label else set()
        self.members[i] = [i]

    def find(self, i: int) -> int:
        while self.parent[i] != i:
            self.parent[i] = self.parent[self.parent[i]]
            i = self.parent[i]
        return i

    def can_union(self, a: int, b: int) -> bool:
        ra, rb = self.find(a), self.find(b)
        if ra == rb:
            return False
        if self.years[ra] & self.years[rb]:
            return False
        la, lb = self.labels[ra], self.labels[rb]
        if la and lb and la != lb:
            return False
        return True

    def union(self, a: int, b: int) -> bool:
        if not self.can_union(a, b):
            return False
        ra, rb = self.find(a), self.find(b)
        if len(self.members[ra]) < len(self.members[rb]):
            ra, rb = rb, ra
        self.parent[rb] = ra
        self.members[ra].extend(self.members.pop(rb))
        self.years[ra] |= self.years.pop(rb)
        self.labels[ra] |= self.labels.pop(rb)
        return True


def skip_string(text: str, i: int) -> tuple[int, str]:
    n = len(text)
    j = i + 1
    chars = []
    while j < n:
        ch = text[j]
        if ch == "\\":
            chars.append(ch)
            if j + 1 < n:
                chars.append(text[j + 1])
            j += 2
            continue
        if ch == '"':
            return j + 1, "".join(chars)
        chars.append(ch)
        j += 1
    return n, "".join(chars)


def skip_char(text: str, i: int) -> int:
    n = len(text)
    j = i + 1
    if j < n and text[j] == "\\":
        j += 2
    elif j < n:
        j += 1
    if j < n and text[j] == "'":
        j += 1
    return j


def strip_annotations(header: str) -> str:
    out = []
    i = 0
    n = len(header)
    while i < n:
        if header[i] == "@":
            i += 1
            while i < n and (header[i].isalnum() or header[i] in "_$."):
                i += 1
            while i < n and header[i] in " \t\r\n":
                i += 1
            if i < n and header[i] == "(":
                depth = 0
                while i < n:
                    ch = header[i]
                    if ch == '"':
                        i, _ = skip_string(header, i)
                        continue
                    if ch == "'":
                        i = skip_char(header, i)
                        continue
                    if ch == "(":
                        depth += 1
                    elif ch == ")":
                        depth -= 1
                        i += 1
                        if depth == 0:
                            break
                        continue
                    i += 1
            continue
        out.append(header[i])
        i += 1
    return "".join(out)


def split_params(param_text: str) -> list[str]:
    param_text = param_text.strip()
    if not param_text:
        return []
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
    if cur and "".join(cur).strip():
        parts.append("".join(cur))
    types = []
    for part in parts:
        part = re.sub(r"@\w+(?:\([^)]*\))?\s*", "", part)
        part = re.sub(r"\bfinal\b", "", part)
        part = re.sub(r"\s+", " ", part).strip()
        if not part:
            continue
        toks = part.split(" ")
        if len(toks) >= 2 and re.match(r"^[A-Za-z_$][\w$]*$", toks[-1]):
            types.append(" ".join(toks[:-1]))
        else:
            types.append(toks[0])
    return types


def parse_type_header(header: str) -> tuple[str, str, list[str]] | None:
    h = strip_annotations(header)
    h = re.sub(r"\s+", " ", h).strip()
    m = re.search(r"\b(class|interface|enum)\s+([A-Za-z_$][\w$]*)\b(.*)$", h)
    if not m:
        return None
    rest = m.group(3)
    supers: list[str] = []
    em = re.search(r"\bextends\s+(.+?)(?:\bimplements\b|$)", rest)
    im = re.search(r"\bimplements\s+(.+)$", rest)
    def names(blob: str) -> list[str]:
        blob = re.sub(r"<[^>]*>", "", blob)
        out = []
        for piece in blob.split(","):
            piece = piece.strip()
            if not piece:
                continue
            out.append(piece.split(".")[-1].strip())
        return [n for n in out if re.match(r"^[A-Za-z_$][\w$]*$", n)]
    if em:
        supers.extend(names(em.group(1)))
    if im:
        supers.extend(names(im.group(1)))
    return m.group(1), m.group(2), supers


def looks_like_method(header: str) -> tuple[str, str, list[str], bool] | None:
    h = strip_annotations(header)
    if "=" in re.sub(r'"[^"]*"', "", h) or "=" in re.sub(r"'[^']*'", "", h):
        # '=' can still appear inside an annotation we stripped. After strip, a
        # field initializer has '='. A method does not.
        if re.search(r"=(?![^=])", h):
            return None
    h = re.sub(r"\s+", " ", h).strip()
    if not h or h == "static" or h.startswith("static {"):
        return None
    m = re.match(
        r"^(?:(?:public|protected|private|static|final|native|synchronized|abstract|default|strictfp|transient|volatile)\s+)*"
        r"(?:(?P<ret>[\w.$]+(?:\s*<[^;{}]*>)?(?:\s*\[\s*\])*)\s+)?"
        r"(?P<name>[A-Za-z_$][\w$]*)\s*\((?P<params>.*)\)\s*(?:throws\s+[\w.,\s]+)?$",
        h,
    )
    if not m:
        return None
    name = m.group("name")
    if name in KEYWORDS:
        return None
    mods = h.split(name, 1)[0]
    is_static = bool(re.search(r"\bstatic\b", mods))
    ret = (m.group("ret") or "void").strip()
    return name, ret, split_params(m.group("params") or ""), is_static


def looks_like_field(header: str) -> tuple[str, str, bool, str] | None:
    h = strip_annotations(header)
    h = h.strip()
    if not h or "(" in strip_annotations(header).split("=")[0] and re.match(r".*\)\s*(?:throws\b|\{)?\s*$", re.sub(r"\s+", " ", strip_annotations(header).split("=")[0])):
        pass
    raw = h
    init = ""
    if "=" in raw:
        left, right = raw.split("=", 1)
        init = right.strip().rstrip(";")
        raw = left
    raw = re.sub(r"\s+", " ", raw).strip().rstrip(";")
    m = re.match(
        r"^(?:(?:public|protected|private|static|final|native|synchronized|abstract|default|strictfp|transient|volatile)\s+)*"
        r"(?P<typ>[\w.$]+(?:\s*<[^;{}]*>)?(?:\s*\[\s*\])*)\s+"
        r"(?P<name>[A-Za-z_$][\w$]*)$",
        raw,
    )
    if not m:
        return None
    name = m.group("name")
    if name in KEYWORDS:
        return None
    mods = raw.split(m.group("typ"), 1)[0]
    is_static = bool(re.search(r"\bstatic\b", mods))
    return name, m.group("typ"), is_static, re.sub(r"\s+", "", init)


def parse_file(rev: int, path: Path, text: str) -> list[JClass]:
    classes: list[JClass] = []
    n = len(text)
    i = 0

    def skip_ws(k: int) -> int:
        while k < n:
            if text[k] in " \t\r\n":
                k += 1
                continue
            if text.startswith("//", k):
                nl = text.find("\n", k)
                k = n if nl < 0 else nl + 1
                continue
            if text.startswith("/*", k):
                end = text.find("*/", k + 2)
                k = n if end < 0 else end + 2
                continue
            break
        return k

    package = ""
    pm = re.search(r"(?m)^package\s+([\w.]+)\s*;", text)
    if pm:
        package = pm.group(1)

    def parse_body(open_brace: int, simple: str, supers: list[str], top: bool) -> int:
        strings: set[str] = set()
        methods: list[JMethod] = []
        fields: list[JField] = []
        k = open_brace + 1
        depth = 1
        member_start = k
        current_method: JMethod | None = None
        method_depth = 0
        while k < n and depth > 0:
            ch = text[k]
            if text.startswith("//", k):
                nl = text.find("\n", k)
                k = n if nl < 0 else nl + 1
                continue
            if text.startswith("/*", k):
                end = text.find("*/", k + 2)
                k = n if end < 0 else end + 2
                continue
            if ch == '"':
                end, body = skip_string(text, k)
                if depth == 1 or current_method is not None:
                    if distinctive(body):
                        strings.add(body)
                        if current_method is not None and depth > 1:
                            current_method.strings.add(body)
                k = end
                continue
            if ch == "'":
                k = skip_char(text, k)
                continue
            if ch == "{":
                if depth == 1:
                    header = text[member_start:k]
                    kind = parse_type_header(header)
                    if kind and re.search(r"\b(class|interface|enum)\s+" + re.escape(kind[1]) + r"\b", header):
                        k = parse_body(k, kind[1], kind[2], False)
                        member_start = k
                        continue
                    meth = looks_like_method(header)
                    if meth and meth[0] != simple:
                        name, ret, params, is_static = meth
                        current_method = JMethod(name, "", len(params), is_static, set(), readable(name) and not callback_name(name))
                        current_method.descriptor = ""  # filled by caller via closure
                        current_method._ret = ret  # type: ignore[attr-defined]
                        current_method._params = params  # type: ignore[attr-defined]
                        methods.append(current_method)
                        method_depth = depth
                        depth += 1
                        k += 1
                        continue
                    depth += 1
                    k += 1
                    continue
                depth += 1
                k += 1
                continue
            if ch == "}":
                depth -= 1
                if current_method is not None and depth == method_depth:
                    current_method = None
                    member_start = k + 1
                elif depth == 1:
                    member_start = k + 1
                k += 1
                if depth == 0:
                    break
                continue
            if ch == ";" and depth == 1 and current_method is None:
                header = text[member_start:k]
                meth = looks_like_method(header)
                if meth and meth[0] != simple:
                    name, ret, params, is_static = meth
                    jm = JMethod(name, "", len(params), is_static, set(), readable(name) and not callback_name(name))
                    jm._ret = ret  # type: ignore[attr-defined]
                    jm._params = params  # type: ignore[attr-defined]
                    methods.append(jm)
                else:
                    fld = looks_like_field(header)
                    if fld and fld[0] != simple:
                        fname, ftyp, is_static, init = fld
                        fields.append(JField(fname, ftyp, is_static, init, readable(fname)))
                member_start = k + 1
                k += 1
                continue
            k += 1
        classes.append(JClass(rev, path, package, simple, supers, strings, methods, fields, top))
        return k

    while i < n:
        i = skip_ws(i)
        if i >= n:
            break
        if text.startswith("//", i) or text.startswith("/*", i) or text[i] == '"' or text[i] == "'":
            if text.startswith("//", i):
                nl = text.find("\n", i)
                i = n if nl < 0 else nl + 1
                continue
            if text.startswith("/*", i):
                end = text.find("*/", i + 2)
                i = n if end < 0 else end + 2
                continue
            if text[i] == '"':
                i, _ = skip_string(text, i)
                continue
            i = skip_char(text, i)
            continue
        if text.startswith("class ", i) or text.startswith("interface ", i) or text.startswith("enum ", i) or re.match(r"(?:public|private|protected|abstract|final|static|strictfp)\b", text[i:]):
            # find the type keyword at brace depth 0
            brace = text.find("{", i)
            if brace < 0:
                break
            header = text[i:brace]
            kind = parse_type_header(header)
            if not kind:
                i += 1
                continue
            # only a top-level type: no brace between i and the header start we already have
            i = parse_body(brace, kind[1], kind[2], True)
            continue
        i += 1
    return classes


def norm_type(raw: str, rev: int, cluster_of: dict[tuple, int], by_name: dict[tuple, list]) -> str:
    t = re.sub(r"\s+", "", raw)
    dims = ""
    while t.endswith("[]"):
        dims += "[]"
        t = t[:-2]
    if "<" in t:
        t = t[: t.find("<")]
    simple = t.split(".")[-1]
    if simple in PRIM:
        return simple + dims
    owners = by_name.get((rev, simple), [])
    if len(owners) == 1:
        cid = cluster_of.get(owners[0].key)
        if cid is not None:
            return f"#{cid}{dims}"
    return simple + dims


def build_descriptors(classes: list[JClass], cluster_of: dict[tuple, int], by_name: dict) -> None:
    for jc in classes:
        for m in jc.methods:
            ret = norm_type(getattr(m, "_ret", "void"), jc.rev, cluster_of, by_name)
            params = tuple(norm_type(p, jc.rev, cluster_of, by_name) for p in getattr(m, "_params", []))
            m.descriptor = f"{'s' if m.is_static else 'i'}({','.join(params)}){ret}"
            m.arity = len(params)
        for f in jc.fields:
            typ = norm_type(f.descriptor, jc.rev, cluster_of, by_name)
            f.descriptor = f"{'s' if f.is_static else 'i'}{typ}"


def index_names(classes: dict[int, list[JClass]]) -> dict[tuple, list[JClass]]:
    by_name: dict[tuple, list[JClass]] = defaultdict(list)
    for rev, items in classes.items():
        for jc in items:
            by_name[(rev, jc.simple)].append(jc)
    return by_name


def link_classes(classes: dict[int, list[JClass]]) -> tuple[dict[int, int], dict[int, list[JClass]], int, int]:
    uf = UnionFind()
    ids: dict[tuple, int] = {}
    by_id: dict[int, JClass] = {}
    n = 0
    for rev, items in classes.items():
        for jc in items:
            ids[jc.key] = n
            by_id[n] = jc
            label = jc.simple if readable(jc.simple) else None
            uf.add(n, rev, label)
            n += 1
    named = 0
    by_readable: dict[str, list[JClass]] = defaultdict(list)
    for rev in ANCHOR:
        for jc in classes[rev]:
            if readable(jc.simple):
                by_readable[jc.simple].append(jc)
    for name, group in by_readable.items():
        per_year: dict[int, list[JClass]] = defaultdict(list)
        for jc in group:
            per_year[jc.rev].append(jc)
        if any(len(v) != 1 for v in per_year.values()):
            continue
        ordered = [per_year[r][0] for r in ANCHOR if r in per_year]
        for jc in ordered[1:]:
            if uf.union(ids[ordered[0].key], ids[jc.key]):
                named += 1
    rare: dict[int, dict[str, JClass]] = {}
    for rev, items in classes.items():
        counts: dict[str, int] = defaultdict(int)
        owner: dict[str, JClass] = {}
        for jc in items:
            for s in jc.strings:
                counts[s] += 1
                owner[s] = jc
        rare[rev] = {s: owner[s] for s, c in counts.items() if c == 1}
    # A literal that is unique in several years but sits on two different
    # readable classes is not evidence those classes are the same type.
    owners_by_string: dict[str, list[JClass]] = defaultdict(list)
    for mapping in rare.values():
        for s, jc in mapping.items():
            owners_by_string[s].append(jc)
    bad_strings: set[str] = set()
    for s, owners in owners_by_string.items():
        names = {jc.simple for jc in owners if readable(jc.simple)}
        if len(names) > 1:
            bad_strings.add(s)
    log(f"ambiguous-strings={len(bad_strings)}")
    votes: dict[tuple[int, int], int] = defaultdict(int)
    longest: dict[tuple[int, int], int] = defaultdict(int)
    for i, rev in enumerate(REVS):
        for other in REVS[i + 1 :]:
            for s, jc in rare[rev].items():
                if s in bad_strings:
                    continue
                dest = rare[other].get(s)
                if not dest:
                    continue
                a, b = ids[jc.key], ids[dest.key]
                if a > b:
                    a, b = b, a
                votes[(a, b)] += 1
                longest[(a, b)] = max(longest[(a, b)], len(s))
    pairs = sorted(votes, key=lambda k: (-votes[k], -longest[k], k))
    linked = 0
    for a, b in pairs:
        if uf.union(a, b):
            linked += 1
    clusters: dict[int, list[JClass]] = defaultdict(list)
    cluster_of: dict[tuple, int] = {}
    for key, i in ids.items():
        root = uf.find(i)
        clusters[root].append(by_id[i])
        cluster_of[key] = root
    log(f"classes={n} clusters={len(clusters)} name-links={named} string-links={linked}")
    return cluster_of, clusters, named, linked


def choose_class_name(members: list[JClass]) -> str:
    ranked = [(PRIO[jc.rev], jc.simple) for jc in members if readable(jc.simple)]
    if ranked:
        ranked.sort(reverse=True)
        return ranked[0][1]
    counts: dict[str, set[int]] = defaultdict(set)
    for jc in members:
        counts[jc.simple].add(jc.rev)
    ranked_s = sorted(counts.items(), key=lambda kv: (-len(kv[1]), kv[0]))
    return ranked_s[0][0]


def class_renames(clusters: dict[int, list[JClass]]) -> dict[tuple, str]:
    out: dict[tuple, str] = {}
    for members in clusters.values():
        if len({jc.rev for jc in members}) < 2:
            continue
        chosen = choose_class_name(members)
        if chosen in KEYWORDS or not re.match(r"^[A-Za-z_$][\w$]*$", chosen):
            continue
        for jc in members:
            if jc.simple != chosen and synthetic_class(jc.simple):
                out[jc.key] = chosen
    return out


def link_members(classes, clusters, cluster_of):
    """Return method and field clusters as lists of (jc, member)."""
    by_name = index_names(classes)
    build_descriptors([jc for items in classes.values() for jc in items], cluster_of, by_name)
    method_uf = UnionFind()
    field_uf = UnionFind()
    mid: dict[tuple, int] = {}
    fid: dict[tuple, int] = {}
    methods_of: dict[int, tuple[JClass, JMethod]] = {}
    fields_of: dict[int, tuple[JClass, JField]] = {}
    next_m = 0
    next_f = 0
    for items in classes.values():
        for jc in items:
            root = cluster_of[jc.key]
            for m in jc.methods:
                mid[(jc.key, m.name, m.descriptor, id(m))] = next_m
                label = m.name if m.readable else None
                method_uf.add(next_m, jc.rev, label)
                methods_of[next_m] = (jc, m)
                next_m += 1
            for f in jc.fields:
                fid[(jc.key, f.name, f.descriptor, f.init, id(f))] = next_f
                label = f.name if f.readable else None
                field_uf.add(next_f, jc.rev, label)
                fields_of[next_f] = (jc, f)
                next_f += 1

    def method_id(jc: JClass, m: JMethod) -> int:
        return mid[(jc.key, m.name, m.descriptor, id(m))]

    def field_id(jc: JClass, f: JField) -> int:
        return fid[(jc.key, f.name, f.descriptor, f.init, id(f))]

    m_links = 0
    f_links = 0
    for root, members in clusters.items():
        by_rev: dict[int, list[JClass]] = defaultdict(list)
        for jc in members:
            by_rev[jc.rev].append(jc)
        years = [r for r in REVS if r in by_rev]
        # One class per year. If a year has several, leave them unlinked to each other.
        picked: dict[int, JClass | None] = {}
        for rev, group in by_rev.items():
            picked[rev] = group[0] if len(group) == 1 else None
        present = [r for r in years if picked[r] is not None]
        for i, rev in enumerate(present):
            a = picked[rev]
            assert a is not None
            rare_m: dict[str, JMethod] = {}
            counts: dict[str, int] = defaultdict(int)
            for m in a.methods:
                for s in m.strings:
                    counts[s] += 1
            owned: dict[str, JMethod] = {}
            for m in a.methods:
                for s in m.strings:
                    if counts[s] == 1:
                        owned[s] = m
            name_m: dict[tuple, list[JMethod]] = defaultdict(list)
            for m in a.methods:
                if m.readable:
                    name_m[(m.name, m.descriptor)].append(m)
            init_f: dict[tuple, JField] = {}
            fcounts: dict[tuple, int] = defaultdict(int)
            for f in a.fields:
                if not f.init:
                    continue
                fcounts[(f.descriptor, f.init)] += 1
            for f in a.fields:
                key = (f.descriptor, f.init)
                if f.init and fcounts[key] == 1:
                    init_f[key] = f
            name_f: dict[tuple, list[JField]] = defaultdict(list)
            for f in a.fields:
                if f.readable:
                    name_f[(f.name, f.descriptor)].append(f)
            for other in present[i + 1 :]:
                b = picked[other]
                assert b is not None
                bcounts: dict[str, int] = defaultdict(int)
                for m in b.methods:
                    for s in m.strings:
                        bcounts[s] += 1
                bowned: dict[str, JMethod] = {}
                for m in b.methods:
                    for s in m.strings:
                        if bcounts[s] == 1:
                            bowned[s] = m
                seen_pair: set[tuple[int, int]] = set()
                for s, ma in owned.items():
                    mb = bowned.get(s)
                    if not mb or ma.descriptor != mb.descriptor:
                        continue
                    ia, ib = method_id(a, ma), method_id(b, mb)
                    if (ia, ib) in seen_pair:
                        continue
                    seen_pair.add((ia, ib))
                    if method_uf.union(ia, ib):
                        m_links += 1
                bname: dict[tuple, list[JMethod]] = defaultdict(list)
                for m in b.methods:
                    if m.readable:
                        bname[(m.name, m.descriptor)].append(m)
                for key, group in name_m.items():
                    other_group = bname.get(key)
                    if not other_group or len(group) != 1 or len(other_group) != 1:
                        continue
                    if method_uf.union(method_id(a, group[0]), method_id(b, other_group[0])):
                        m_links += 1
                bfcounts: dict[tuple, int] = defaultdict(int)
                for f in b.fields:
                    if not f.init:
                        continue
                    bfcounts[(f.descriptor, f.init)] += 1
                binit: dict[tuple, JField] = {}
                for f in b.fields:
                    key = (f.descriptor, f.init)
                    if f.init and bfcounts[key] == 1:
                        binit[key] = f
                for key, fa in init_f.items():
                    fb = binit.get(key)
                    if not fb:
                        continue
                    if field_uf.union(field_id(a, fa), field_id(b, fb)):
                        f_links += 1
                bfname: dict[tuple, list[JField]] = defaultdict(list)
                for f in b.fields:
                    if f.readable:
                        bfname[(f.name, f.descriptor)].append(f)
                for key, group in name_f.items():
                    other_group = bfname.get(key)
                    if not other_group or len(group) != 1 or len(other_group) != 1:
                        continue
                    if field_uf.union(field_id(a, group[0]), field_id(b, other_group[0])):
                        f_links += 1
    m_clusters: dict[int, list[tuple[JClass, JMethod]]] = defaultdict(list)
    for i, pair in methods_of.items():
        m_clusters[method_uf.find(i)].append(pair)
    f_clusters: dict[int, list[tuple[JClass, JField]]] = defaultdict(list)
    for i, pair in fields_of.items():
        f_clusters[field_uf.find(i)].append(pair)
    log(f"method-links={m_links} field-links={f_links} method-clusters={len(m_clusters)} field-clusters={len(f_clusters)}")
    return m_clusters, f_clusters


def choose_member_name(pairs: list[tuple[JClass, object]]) -> str:
    ranked = []
    for jc, member in pairs:
        name = member.name
        if readable(name) and not callback_name(name):
            ranked.append((PRIO[jc.rev], name))
        elif readable(name) and callback_name(name):
            ranked.append((PRIO[jc.rev] + 100, name))
    if ranked:
        ranked.sort(reverse=True)
        return ranked[0][1]
    counts: dict[str, set[int]] = defaultdict(set)
    for jc, member in pairs:
        counts[member.name].add(jc.rev)
    ranked_s = sorted(counts.items(), key=lambda kv: (-len(kv[1]), kv[0]))
    return ranked_s[0][0]


def propose(clusters_m, clusters_f) -> dict[int, dict[tuple, str]]:
    """rev -> (kind, owner key, old name, descriptor) -> new name. kind m/f."""
    proposals: dict[int, dict[tuple, str]] = defaultdict(dict)
    for group in clusters_m.values():
        if len({jc.rev for jc, _m in group}) < 2:
            continue
        chosen = choose_member_name(group)
        if chosen in KEYWORDS or callback_name(chosen) and any(m.name != chosen for _jc, m in group):
            # renaming onto a callback is allowed only when that callback is the
            # readable name already chosen; keep it. Reject keywords.
            if chosen in KEYWORDS:
                continue
        for jc, m in group:
            if m.name == chosen:
                continue
            if not synthetic_member(m.name):
                continue
            if callback_name(m.name):
                continue
            proposals[jc.rev][("m", jc.key, m.name, m.descriptor)] = chosen
    for group in clusters_f.values():
        if len({jc.rev for jc, _f in group}) < 2:
            continue
        chosen = choose_member_name(group)
        if chosen in KEYWORDS:
            continue
        for jc, f in group:
            if f.name == chosen:
                continue
            if not synthetic_member(f.name):
                continue
            proposals[jc.rev][("f", jc.key, f.name, f.descriptor)] = chosen
    return proposals


def freeze_overloads_and_overrides(classes, proposals: dict[int, dict[tuple, str]]) -> int:
    frozen = 0
    by_name = index_names(classes)

    def prop_name(rev: int, kind: str, jc: JClass, member) -> str:
        return proposals.get(rev, {}).get((kind, jc.key, member.name, member.descriptor), member.name)

    def drop_method_cluster(rev: int, jc: JClass, m: JMethod) -> None:
        nonlocal frozen
        key = ("m", jc.key, m.name, m.descriptor)
        if key in proposals.get(rev, {}):
            del proposals[rev][key]
            frozen += 1

    # Same-arity overloads keep their old names.
    for rev, items in classes.items():
        for jc in items:
            groups: dict[tuple, list[JMethod]] = defaultdict(list)
            for m in jc.methods:
                groups[(m.name, m.arity)].append(m)
            for group in groups.values():
                if len(group) > 1:
                    for m in group:
                        drop_method_cluster(rev, jc, m)
            # resulting same-arity collisions
            for _ in range(4):
                planned: dict[tuple, list[JMethod]] = defaultdict(list)
                for m in jc.methods:
                    planned[(prop_name(rev, "m", jc, m), m.arity)].append(m)
                changed = False
                for group in planned.values():
                    names = {m.name for m in group}
                    if len(group) > 1 and len(names) > 1:
                        for m in group:
                            if ("m", jc.key, m.name, m.descriptor) in proposals.get(rev, {}):
                                drop_method_cluster(rev, jc, m)
                                changed = True
                if not changed:
                    break
            # duplicate field names
            seen: dict[str, list[JField]] = defaultdict(list)
            for f in jc.fields:
                seen[prop_name(rev, "f", jc, f)].append(f)
            for group in seen.values():
                if len(group) > 1 and len({f.name for f in group}) > 1:
                    for f in group:
                        key = ("f", jc.key, f.name, f.descriptor)
                        if key in proposals.get(rev, {}):
                            del proposals[rev][key]
                            frozen += 1
            # duplicate method descriptors
            seen_d: dict[tuple, list[JMethod]] = defaultdict(list)
            for m in jc.methods:
                seen_d[(prop_name(rev, "m", jc, m), m.descriptor)].append(m)
            for group in seen_d.values():
                if len(group) > 1 and len({m.name for m in group}) > 1:
                    for m in group:
                        drop_method_cluster(rev, jc, m)

    def parent_of(jc: JClass, simple: str) -> JClass | None:
        cands = by_name.get((jc.rev, simple), [])
        if not cands:
            return None
        same = [c for c in cands if c.package == jc.package and c is not jc]
        if len(same) == 1:
            return same[0]
        if len(cands) == 1 and cands[0] is not jc:
            return cands[0]
        return None

    for _ in range(6):
        changed = False
        for rev, items in classes.items():
            for jc in items:
                chain: list[JClass] = []
                seen = {jc.key}
                queue = list(jc.supers)
                while queue:
                    name = queue.pop()
                    parent = parent_of(jc if not chain else chain[-1], name)
                    if parent is None or parent.key in seen:
                        continue
                    seen.add(parent.key)
                    chain.append(parent)
                    queue.extend(parent.supers)
                pmap: dict[tuple, JMethod] = {}
                for parent in chain:
                    for pm in parent.methods:
                        pmap.setdefault((pm.name, pm.descriptor), pm)
                for m in jc.methods:
                    pm = pmap.get((m.name, m.descriptor))
                    if pm is None:
                        continue
                    child_new = prop_name(rev, "m", jc, m)
                    parent_new = prop_name(rev, "m", parent_of_method_owner(chain, pm), pm) if False else None
                    owner = next(p for p in chain if any(x is pm for x in p.methods))
                    parent_new = prop_name(rev, "m", owner, pm)
                    if child_new != parent_new:
                        if ("m", jc.key, m.name, m.descriptor) in proposals.get(rev, {}):
                            drop_method_cluster(rev, jc, m)
                            changed = True
                        if ("m", owner.key, pm.name, pm.descriptor) in proposals.get(rev, {}):
                            drop_method_cluster(rev, owner, pm)
                            changed = True
        if not changed:
            break
    log(f"frozen-member-renames={frozen}")
    return frozen


def parent_of_method_owner(chain: list[JClass], pm: JMethod) -> JClass:
    for parent in chain:
        if any(x is pm for x in parent.methods):
            return parent
    return chain[0]


def global_maps(classes, class_map: dict[tuple, str], proposals) -> dict[int, dict[str, str]]:
    """Identifier renames that are unambiguous inside one revision."""
    per: dict[int, dict[str, str]] = {}
    for rev, items in classes.items():
        uses: dict[str, set[str | None]] = defaultdict(set)
        # None means "this occurrence keeps the old spelling".
        declared: dict[str, int] = defaultdict(int)
        for jc in items:
            new_c = class_map.get(jc.key, jc.simple)
            declared[jc.simple] += 1
            uses[jc.simple].add(new_c if new_c != jc.simple else None)
            if new_c == jc.simple:
                uses[jc.simple].add(None)
            for m in jc.methods:
                new_m = proposals.get(rev, {}).get(("m", jc.key, m.name, m.descriptor), m.name)
                declared[m.name] += 1
                uses[m.name].add(new_m if new_m != m.name else None)
            for f in jc.fields:
                new_f = proposals.get(rev, {}).get(("f", jc.key, f.name, f.descriptor), f.name)
                declared[f.name] += 1
                uses[f.name].add(new_f if new_f != f.name else None)
        mapping: dict[str, str] = {}
        for old, targets in uses.items():
            news = {t for t in targets if t}
            if len(news) != 1:
                continue
            if None in targets:
                # some declaration keeps the old name; a global replace would hit it
                continue
            new = next(iter(news))
            if new == old or new in KEYWORDS or not re.match(r"^[A-Za-z_$][\w$]*$", new):
                continue
            mapping[old] = new
        per[rev] = mapping
    return per


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
            end, _ = skip_string(text, i)
            out.append(text[i:end])
            i = end
            continue
        if ch == "'":
            end = skip_char(text, i)
            out.append(text[i:end])
            i = end
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


def backup_tree(rev: int) -> None:
    folder = FOLDERS[rev]
    src = java_root(rev)
    dest_root = BACKUP / folder / "pre-rename-src"
    if dest_root.exists():
        return
    for path in src.rglob("*.java"):
        if "build" in path.parts:
            continue
        rel = path.relative_to(src)
        dest = dest_root / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, dest)


def apply_rev(rev: int, classes: list[JClass], mapping: dict[str, str]) -> tuple[int, int]:
    folder = FOLDERS[rev]
    root = java_root(rev)
    by_path: dict[Path, list[JClass]] = defaultdict(list)
    for jc in classes:
        by_path[jc.path].append(jc)
    planned: list[tuple[Path, Path, str]] = []
    used_dest: set[Path] = set()
    renamed_files = 0
    for path, items in by_path.items():
        text = path.read_text(errors="ignore")
        new_text = apply_text(text, mapping)
        top = next((jc for jc in items if jc.top and jc.simple == path.stem), None)
        if top is None:
            top = next((jc for jc in items if jc.top), items[0])
        new_simple = mapping.get(top.simple, top.simple)
        dest = path.with_name(new_simple + ".java")
        if dest in used_dest and dest != path:
            new_text = text
            dest = path
            new_simple = top.simple
        if dest.exists() and dest != path and dest not in by_path:
            new_text = apply_text(text, {k: v for k, v in mapping.items() if k != top.simple})
            dest = path
        used_dest.add(dest)
        if new_text != text or dest != path:
            rel = path.relative_to(BUILDS / folder)
            bdest = BACKUP / folder / rel
            if not bdest.exists():
                bdest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(path, bdest)
        if dest != path:
            renamed_files += 1
        planned.append((path, dest, new_text))
    temps = []
    for i, (src, dest, text) in enumerate(planned):
        if text == src.read_text(errors="ignore") and dest == src:
            continue
        dest.parent.mkdir(parents=True, exist_ok=True)
        tmp = dest.with_name(f".sharetmp-{i}-{dest.name}")
        tmp.write_text(text)
        temps.append((src, dest, tmp))
    new_paths = {dest for _s, dest, _t in temps}
    for src, dest, tmp in temps:
        if src != dest and src.exists() and src not in new_paths:
            src.unlink()
    for src, dest, tmp in temps:
        if dest.exists() and dest != tmp:
            dest.unlink()
        tmp.rename(dest)
    changed = len(temps)
    log(f"applied {rev}: files-rewritten={changed} file-renames={renamed_files} idents={len(mapping)}")
    return changed, renamed_files


def load_all() -> dict[int, list[JClass]]:
    out: dict[int, list[JClass]] = {}
    for rev in REVS:
        items: list[JClass] = []
        root = java_root(rev)
        for path in root.rglob("*.java"):
            if "build" in path.parts:
                continue
            text = path.read_text(errors="ignore")
            items.extend(parse_file(rev, path, text))
        out[rev] = items
        log(f"parsed {rev}: classes={len(items)} files={sum(1 for _ in root.rglob('*.java'))}")
    return out


def stats(classes, clusters, class_map, proposals, maps) -> dict:
    multi = [m for m in clusters.values() if len({jc.rev for jc in m}) >= 2]
    shared_class = 0
    for members in multi:
        chosen = choose_class_name(members)
        ok = True
        for jc in members:
            final = class_map.get(jc.key, jc.simple)
            # class_map is only the synthetic renames that were accepted into the
            # global map. Recompute from the global identifier map.
            final = maps.get(jc.rev, {}).get(jc.simple, jc.simple)
            if final != chosen and jc.simple != chosen:
                ok = False
        if ok:
            shared_class += 1
    unmatched = {}
    for rev in (831, 860):
        total = 0
        hit = 0
        readable_left = []
        synthetic_left = 0
        for jc in classes[rev]:
            total += 1
            root_members = None
            for members in clusters.values():
                if any(x.key == jc.key for x in members):
                    root_members = members
                    break
            has_910 = bool(root_members and any(x.rev == 910 for x in root_members))
            if has_910:
                hit += 1
            else:
                if readable(jc.simple):
                    readable_left.append(jc.simple)
                else:
                    synthetic_left += 1
        unmatched[rev] = {
            "classes": total,
            "matched_to_910": hit,
            "unmatched_readable": sorted(set(readable_left)),
            "unmatched_synthetic": synthetic_left,
        }
    renames = {rev: len(mapping) for rev, mapping in maps.items()}
    return {
        "multi_year_class_clusters": len(multi),
        "multi_year_class_clusters_sharing_one_name": shared_class,
        "identifier_renames": renames,
        "unmatched": unmatched,
    }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply", action="store_true")
    args = parser.parse_args()
    classes = load_all()
    cluster_of, clusters, _named, _linked = link_classes(classes)
    raw_class_renames = class_renames(clusters)
    m_clusters, f_clusters = link_members(classes, clusters, cluster_of)
    proposals = propose(m_clusters, f_clusters)
    freeze_overloads_and_overrides(classes, proposals)
    # Drop class renames that would collide inside a package.
    by_pkg: dict[tuple, set[str]] = defaultdict(set)
    for items in classes.values():
        for jc in items:
            by_pkg[(jc.rev, jc.package)].add(jc.simple)
    accepted_class: dict[tuple, str] = {}
    taken: dict[tuple, str] = {}
    for key, new in sorted(raw_class_renames.items(), key=lambda kv: -PRIO[kv[0][0]]):
        rev, package, old = key
        if new in by_pkg[(rev, package)] and new != old:
            continue
        slot = (rev, package, new)
        if slot in taken and taken[slot] != old:
            continue
        taken[slot] = old
        accepted_class[key] = new
    # Feed class renames through the same unambiguous-identifier filter.
    class_as_prop = defaultdict(dict)
    for key, new in accepted_class.items():
        rev = key[0]
        class_as_prop[rev][("c", key, key[2], "")] = new
    # global_maps reads proposals for methods and fields and class_map for classes.
    maps = global_maps(classes, accepted_class, proposals)
    info = stats(classes, clusters, accepted_class, proposals, maps)
    log("renames " + json.dumps(info["identifier_renames"]))
    log(
        f"multi-year class clusters={info['multi_year_class_clusters']} "
        f"sharing-one-name={info['multi_year_class_clusters_sharing_one_name']}"
    )
    for rev, row in info["unmatched"].items():
        log(
            f"unmatched {rev}: classes={row['classes']} matched910={row['matched_to_910']} "
            f"synthetic_left={row['unmatched_synthetic']} readable_left={len(row['unmatched_readable'])}"
        )
    out_dir = ROOT / "work" / "share-rs3"
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "stats.json").write_text(json.dumps(info, indent=2))
    if not args.apply:
        log("dry-run only")
        return
    for rev in REVS:
        backup_tree(rev)
        apply_rev(rev, classes[rev], maps.get(rev, {}))


if __name__ == "__main__":
    main()
