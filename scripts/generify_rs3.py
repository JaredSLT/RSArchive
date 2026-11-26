#!/usr/bin/env python3
"""Give RS3 yearly sources one in-place vocabulary and repair Fernflower breakage.

RSC and RS2 are different engines. Names come from the 910 remap (simple name
after a member move, never the move itself) and from multi-year method clusters.
Obfuscated methodN / fieldN / classN / fn-hash tokens are removed. 831 and 860
now have decompiled sources. They stay out of this pass until they have a
remap, because their classes are still in the default package.
"""
from __future__ import annotations

import argparse
import hashlib
import os
import re
import subprocess
import sys
import zipfile
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

import unify_method_names as um  # noqa: E402
from generate_unified_remaps import parse_cp, u2, utf  # noqa: E402

import jdk  # noqa: E402

BUILDS = ROOT / "builds" / "rs3"
REMAP_DIR = ROOT / "tools" / "rs-deob" / "remap"
JAVA = jdk.java_bin()
REVS = (790, 876, 887, 902, 910)
FOLDERS = {
    790: "790-2013-12-16",
    876: "876-2016-11-28",
    887: "887-2017-11-01",
    902: "902-2018-12-14",
    910: "910-2019-10-16",
}
SKIP_REVS = {831, 860}

OBF = re.compile(
    r"^(?:method|field|class|anInt|anLong|aBoolean|aString|aClass)\d+$|^fn[0-9a-f]{8,}$"
)
CLASS_N = re.compile(r"class\d+")
CLASS_PATH = re.compile(r"(^|/)class\d+$")
IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
KEYWORDS = set(um.JAVA_KEYWORDS) | {"null", "var", "yield", "record"}
JAVA_LANG = {
    "Boolean", "Byte", "Character", "Class", "Double", "Enum", "Error", "Exception",
    "Float", "Integer", "Long", "Math", "Number", "Object", "Process", "Runtime",
    "Short", "String", "StringBuffer", "StringBuilder", "System", "Thread",
    "Throwable", "Void", "Iterable", "Override", "Deprecated", "SuppressWarnings",
}
BOX = {
    "int": "Integer", "boolean": "Boolean", "long": "Long", "double": "Double",
    "float": "Float", "byte": "Byte", "short": "Short", "char": "Character",
}
METH_RE = re.compile(
    r"^[ \t]*"
    r"(?:(?:public|protected|private|static|final|synchronized|native|abstract|strictfp)\s+)*"
    r"(?:<[^;\n]+>\s+)?"
    r"([A-Za-z_][\w.]*(?:\s*\[\s*\])*)\s+"
    r"([A-Za-z_][\w]*)\s*"
    r"\(([^)]*)\)\s*"
    r"(?:throws\s+[^{]+)?\{",
    re.M,
)


def readable(name: str | None) -> bool:
    if not name or name in KEYWORDS or name in {"<init>", "<clinit>"}:
        return False
    if not re.match(r"^[A-Za-z_][A-Za-z0-9_]*$", name):
        return False
    if OBF.match(name):
        return False
    if len(name) == 1:
        return False
    return True


def legal(name: str) -> str:
    if name in KEYWORDS or not re.match(r"^[A-Za-z_][A-Za-z0-9_]*$", name):
        name = "N_" + re.sub(r"\W+", "_", name)
    if OBF.match(name):
        name = "n_" + name
    if name in KEYWORDS:
        name = name + "Name"
    return name


def digest(*parts: object) -> str:
    raw = "|".join(str(p) for p in parts)
    return hashlib.sha1(raw.encode()).hexdigest()[:8]


def u4(data: bytes, i: int) -> tuple[int, int]:
    return int.from_bytes(data[i : i + 4], "big"), i + 4


def skip_ev(data: bytes, i: int) -> int:
    tag = data[i]
    i += 1
    if tag == ord("["):
        n, i = u2(data, i)
        for _ in range(n):
            i = skip_ev(data, i)
        return i
    if tag == ord("@"):
        return skip_ann(data, i)
    if tag == ord("e"):
        return i + 4
    return i + 2


def skip_ann(data: bytes, i: int) -> int:
    i += 2
    n, i = u2(data, i)
    for _ in range(n):
        i += 2
        i = skip_ev(data, i)
    return i


def read_ann(data: bytes, i: int, cp) -> tuple[str | None, int]:
    type_i, i = u2(data, i)
    n, i = u2(data, i)
    found = None
    typ = utf(cp, type_i) or ""
    for _ in range(n):
        name_i, i = u2(data, i)
        tag = data[i]
        i += 1
        if tag == ord("s"):
            vi, i = u2(data, i)
            if typ.endswith("ObfuscatedName;") and (utf(cp, name_i) == "value"):
                found = utf(cp, vi)
        elif tag == ord("["):
            m, i = u2(data, i)
            for _j in range(m):
                i = skip_ev(data, i)
        elif tag == ord("@"):
            _f, i = read_ann(data, i, cp)
        elif tag == ord("e"):
            i += 4
        else:
            i += 2
    return found, i


def read_annotations(data: bytes, i: int, cp) -> tuple[str | None, int]:
    n, i = u2(data, i)
    found = None
    for _ in range(n):
        one, i = read_ann(data, i, cp)
        if one:
            found = one
    return found, i


def field_refs_in(code: bytes, cp) -> set[tuple[str, str, str]]:
    if not code or len(code) < 8:
        return set()
    clen = int.from_bytes(code[4:8], "big")
    bc = code[8 : 8 + clen]
    refs: set[tuple[str, str, str]] = set()
    i = 0
    wide = False
    while i < len(bc):
        op = bc[i]
        i += 1
        if op in (178, 179, 180, 181) and i + 1 < len(bc):
            idx = (bc[i] << 8) | bc[i + 1]
            i += 2
            ent = cp[idx] if idx < len(cp) else None
            if ent and ent[0] == 9 and ent[2] < len(cp):
                nat = cp[ent[2]]
                cls = utf(cp, ent[1])
                if nat and nat[0] == 12:
                    fname = utf(cp, nat[1])
                    fdesc = utf(cp, nat[2])
                    if cls and fname and fdesc:
                        refs.add((cls, fname, fdesc))
            continue
        if op in (21, 22, 23, 24, 25, 54, 55, 56, 57, 58, 169) and wide:
            i += 2
            wide = False
        elif op == 132 and wide:
            i += 4
            wide = False
        elif op in (21, 22, 23, 24, 25, 54, 55, 56, 57, 58, 169, 188, 16, 18):
            i += 1
        elif op in (19, 20, 178, 179, 180, 181, 182, 183, 184, 187, 189, 192, 193, 17, 132):
            i += 2
        elif op == 185:
            i += 4
        elif op == 186:
            i += 4
        elif op == 197:
            i += 3
        elif op in (153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 198, 199):
            i += 2
        elif op in (200, 201):
            i += 4
        elif op == 170:
            while i % 4:
                i += 1
            i += 4
            if i + 8 > len(bc):
                break
            low = int.from_bytes(bc[i : i + 4], "big", signed=True)
            i += 4
            high = int.from_bytes(bc[i : i + 4], "big", signed=True)
            i += 4
            i += 4 * (high - low + 1)
        elif op == 171:
            while i % 4:
                i += 1
            i += 4
            if i + 4 > len(bc):
                break
            npairs = int.from_bytes(bc[i : i + 4], "big", signed=True)
            i += 4
            i += 8 * max(npairs, 0)
        elif op == 196:
            wide = True
            continue
        wide = False
    return refs


def code_hash(code: bytes) -> str:
    if not code or len(code) < 8:
        return ""
    clen = int.from_bytes(code[4:8], "big")
    return hashlib.sha1(code[8 : 8 + clen]).hexdigest()[:12]


def parse_member_attrs(data: bytes, i: int, cp, want_code: bool):
    n, i = u2(data, i)
    obf = None
    refs: set[tuple[str, str, str]] = set()
    chash = ""
    for _ in range(n):
        ni, i = u2(data, i)
        ln, i = u4(data, i)
        name = utf(cp, ni) or ""
        body = data[i : i + ln]
        i += ln
        if name in ("RuntimeVisibleAnnotations", "RuntimeInvisibleAnnotations") and len(body) >= 2:
            try:
                found, _ = read_annotations(body, 0, cp)
            except Exception:
                found = None
            if found:
                obf = found
        elif want_code and name == "Code":
            try:
                refs |= field_refs_in(body, cp)
                chash = code_hash(body)
            except Exception:
                pass
    return obf, refs, chash, i


def split_key(key: str) -> tuple[str, str, str | None]:
    if "(" in key:
        head, desc = key.split("(", 1)
        desc = "(" + desc
        owner, name = head.rsplit(".", 1)
        return owner, name, desc
    if "." in key:
        owner, name = key.rsplit(".", 1)
        return owner, name, None
    return key, "", None


def parse_deob_class(data: bytes):
    cp, i = parse_cp(data)
    _acc, i = u2(data, i)
    this_i, i = u2(data, i)
    super_i, i = u2(data, i)
    n_if, i = u2(data, i)
    for _ in range(n_if):
        _ix, i = u2(data, i)
    internal = utf(cp, this_i) or ""
    nf, i = u2(data, i)
    fields = []
    for _ in range(nf):
        facc, i = u2(data, i)
        ni, i = u2(data, i)
        di, i = u2(data, i)
        obf, _refs, _h, i = parse_member_attrs(data, i, cp, False)
        fields.append({
            "name": utf(cp, ni) or "",
            "desc": utf(cp, di) or "",
            "obf": obf,
            "acc": facc,
            "refs": set(),
        })
    nm, i = u2(data, i)
    methods = []
    for _ in range(nm):
        macc, i = u2(data, i)
        ni, i = u2(data, i)
        di, i = u2(data, i)
        obf, refs, chash, i = parse_member_attrs(data, i, cp, True)
        methods.append({
            "name": utf(cp, ni) or "",
            "desc": utf(cp, di) or "",
            "obf": obf,
            "acc": macc,
            "refs": refs,
            "hash": chash,
        })
    cobf, _r, _h, i = parse_member_attrs(data, i, cp, False)
    return {
        "internal": internal,
        "obf": cobf,
        "fields": fields,
        "methods": methods,
    }


def load_deob(rev: int) -> list[dict]:
    jar = BUILDS / FOLDERS[rev] / "deob" / "gamepack-deob.jar"
    out = []
    with zipfile.ZipFile(jar) as zf:
        for name in zf.namelist():
            if not name.endswith(".class") or name.startswith("META-INF/"):
                continue
            try:
                rec = parse_deob_class(zf.read(name))
            except Exception:
                continue
            if not rec["internal"]:
                continue
            rec["rev"] = rev
            pkg, _, simple = rec["internal"].rpartition("/")
            rec["pkg"] = pkg.replace("/", ".")
            rec["simple"] = simple
            out.append(rec)
    return out


def src_root(rev: int) -> Path:
    return BUILDS / FOLDERS[rev] / "src" / "src" / "main" / "java"


def build_clusters(deob: dict[int, list[dict]]):
    revs = um.find_jars("rs3")
    revs = [(rev, path) for rev, path in revs if rev not in SKIP_REVS and rev in FOLDERS]
    print("parsing original jars", flush=True)
    jars = {}
    for rev, path in revs:
        print(f"  jar {rev}", flush=True)
        jars[rev] = um.load_jar(path)
    ordered = [rev for rev, _ in revs]
    method_uf = um.UnionFind()
    class_uf = um.UnionFind()
    hard = {rev: um.load_class_remap(REMAP_DIR / f"rs3-{rev}.txt") for rev in ordered}
    canon = {rev: dict(hard[rev]) for rev in ordered}
    for rev, classes in jars.items():
        for c in classes.values():
            for m in c.methods:
                method_uf.add((rev, *m.mid))
        um.union_overrides(classes, rev, method_uf)

    def match_pair(rev_a, rev_b, linked):
        for ca, cb in linked.items():
            class_uf.union(("c", rev_a, ca), ("c", rev_b, cb))
            fa, fb = canon[rev_a].get(ca), canon[rev_b].get(cb)
            if fa and not fb:
                canon[rev_b][cb] = fa
            elif fb and not fa:
                canon[rev_a][ca] = fb
        used_a, used_b = set(), set()
        n_m = 0
        for ca, cb in linked.items():
            if ca not in jars[rev_a] or cb not in jars[rev_b]:
                continue
            added, ua, ub = um.match_methods(jars[rev_a][ca], jars[rev_b][cb], rev_a, rev_b, method_uf)
            n_m += added
            used_a |= ua
            used_b |= ub
        n_move = um.link_unique_strings(jars[rev_a], jars[rev_b], rev_a, rev_b, method_uf, linked, used_a, used_b)
        n_calls = um.refine_by_calls(jars[rev_a], jars[rev_b], rev_a, rev_b, method_uf, linked, used_a, used_b)
        print(f"  link {rev_a}->{rev_b}: classes={len(linked)} methods={n_m} moved={n_move} calls={n_calls}", flush=True)

    for rev_a, rev_b in zip(ordered, ordered[1:]):
        linked = um.link_classes(jars[rev_a], jars[rev_b], canon[rev_a], canon[rev_b], hard[rev_a], hard[rev_b])
        match_pair(rev_a, rev_b, linked)
    hard_revs = [rev for rev in ordered if hard[rev]]
    for rev_a, rev_b in zip(hard_revs, hard_revs[1:]):
        if ordered.index(rev_b) == ordered.index(rev_a) + 1:
            continue
        inv_b = {}
        for obf, fqcn in hard[rev_b].items():
            inv_b.setdefault(fqcn, obf)
        linked = {}
        for obf, fqcn in hard[rev_a].items():
            other = inv_b.get(fqcn)
            if other and obf in jars[rev_a] and other in jars[rev_b]:
                linked[obf] = other
        if linked:
            match_pair(rev_a, rev_b, linked)

    def ident(cls: dict) -> str:
        if cls["obf"]:
            return cls["obf"]
        return "internal:" + cls["internal"]

    for rev, classes in deob.items():
        for cls in classes:
            class_uf.add(("c", rev, ident(cls)))
    by_internal: dict[str, list[tuple[int, str]]] = defaultdict(list)
    for rev, classes in deob.items():
        for cls in classes:
            if CLASS_PATH.search(cls["internal"]):
                continue
            by_internal[cls["internal"]].append((rev, ident(cls)))
    for _internal, items in by_internal.items():
        if len(items) < 2:
            continue
        base = ("c", items[0][0], items[0][1])
        for rev, cid in items[1:]:
            class_uf.union(base, ("c", rev, cid))

    anchors = um.load_anchors("rs3", ordered)
    return method_uf, class_uf, anchors, ident


def class_root(class_uf, rev: int, ident_s: str):
    return class_uf.find(("c", rev, ident_s))


def assign_names(deob, method_uf, class_uf, anchors, ident):
    # Class FQCN per cluster.
    class_members: dict[tuple, list[dict]] = defaultdict(list)
    for rev, classes in deob.items():
        for cls in classes:
            root = class_root(class_uf, rev, ident(cls))
            cls["croot"] = root
            class_members[root].append(cls)

    fqcn_of: dict[tuple, tuple[str, str]] = {}
    taken_fqcn: dict[tuple[str, str], tuple] = {}
    for root, members in class_members.items():
        choice = None
        for rev in (910, 902, 887, 876, 790):
            for cls in members:
                if cls["rev"] != rev:
                    continue
                simple = cls["simple"]
                if simple == "null":
                    choice = ("deob", "DeobNull")
                    break
                if readable(simple) and simple not in JAVA_LANG:
                    choice = (cls["pkg"], simple)
                    break
            if choice:
                break
        if not choice:
            earliest = min(members, key=lambda c: (c["rev"], c["internal"]))
            choice = (earliest["pkg"] or "deob", "Cls" + digest("class", earliest["rev"], earliest.get("obf") or earliest["internal"]))
        pkg, simple = choice
        simple = legal(simple)
        if simple == "null":
            simple = "DeobNull"
        key = (pkg, simple)
        owner = taken_fqcn.get(key)
        if owner is not None and owner != root:
            simple = simple + "X" + digest("clash", root)[:4]
            key = (pkg, simple)
        # A 910 class keeps its real FQCN even if a clash was detected; the other cluster is suffixed.
        real_910 = None
        for cls in members:
            if cls["rev"] == 910 and readable(cls["simple"]) and cls["simple"] not in JAVA_LANG:
                real_910 = (cls["pkg"], cls["simple"])
            if cls["rev"] == 910 and cls["simple"] == "null":
                real_910 = ("deob", "DeobNull")
        if real_910 and taken_fqcn.get(real_910) in (None, root):
            pkg, simple = real_910
            key = (pkg, simple)
        taken_fqcn[key] = root
        fqcn_of[root] = (pkg, simple)

    # Methods.
    method_groups: dict[tuple, list[tuple]] = defaultdict(list)
    current_name: dict[tuple, dict[int, str]] = defaultdict(dict)
    obf_by_mid: dict[tuple, str] = {}
    for rev, classes in deob.items():
        for cls in classes:
            for m in cls["methods"]:
                if m["name"] in ("<init>", "<clinit>"):
                    continue
                if not m["obf"] or "(" not in m["obf"]:
                    mid = (rev, "internal:" + cls["internal"], m["name"], m["desc"])
                    method_uf.add(mid)
                    root = method_uf.find(mid)
                else:
                    owner, name, desc = split_key(m["obf"])
                    mid = (rev, owner, name, desc)
                    method_uf.add(mid)
                    root = method_uf.find(mid)
                    obf_by_mid[mid] = m["obf"]
                method_groups[root].append(mid)
                current_name[root][rev] = m["name"]
                m["mroot"] = root

    # Exact bytecode-hash matches inside a class cluster, for methods unify missed.
    by_class_hash: dict[tuple, list[tuple]] = defaultdict(list)
    for rev, classes in deob.items():
        for cls in classes:
            for m in cls["methods"]:
                if m["name"] in ("<init>", "<clinit>") or not m.get("hash"):
                    continue
                if m["hash"] and len(m["hash"]) >= 8:
                    by_class_hash[(cls["croot"], m["hash"])].append(m.get("mroot"))
    for _key, roots in by_class_hash.items():
        uniq = [r for r in roots if r is not None]
        if len(set(map(str, uniq))) < 2:
            continue
        base = uniq[0]
        for other in uniq[1:]:
            if other != base:
                method_uf.union(base, other)

    # Rebuild groups after extra unions.
    method_groups = defaultdict(list)
    current_name = defaultdict(dict)
    for rev, classes in deob.items():
        for cls in classes:
            for m in cls["methods"]:
                root = m.get("mroot")
                if root is None:
                    continue
                root = method_uf.find(root)
                m["mroot"] = root
                method_groups[root].append((rev, m))
                current_name[root][rev] = m["name"]

    method_name: dict[tuple, str] = {}
    used_names: set[str] = set()
    for root, pairs in method_groups.items():
        best = None
        seen_mids = set()
        for rev, m in pairs:
            if not m["obf"] or "(" not in m["obf"]:
                continue
            owner, name, desc = split_key(m["obf"])
            seen_mids.add((rev, owner, name, desc))
        for mid in seen_mids:
            for prio, name in anchors.get(mid, []):
                if readable(name) and (best is None or prio > best[0] or (prio == best[0] and name < best[1])):
                    best = (prio, name)
        chosen = best[1] if best else None
        if not chosen:
            for rev in (910, 902, 887, 876, 790):
                cur = current_name[root].get(rev)
                if readable(cur):
                    chosen = cur
                    break
        if not chosen:
            rev0, m0 = min(pairs, key=lambda t: (t[0], t[1]["name"], t[1]["desc"]))
            chosen = "m" + digest("rs3", rev0, m0.get("obf") or m0["name"], m0["desc"])
        chosen = legal(chosen)
        method_name[root] = chosen
        used_names.add(chosen)

    # Fields, grouped by the class cluster of their original owner when known.
    internal_obf: dict[tuple[int, str], str] = {}
    for rev, classes in deob.items():
        for cls in classes:
            internal_obf[(rev, cls["internal"])] = ident(cls)

    def owner_root(rev: int, cls: dict, field: dict):
        if field["obf"] and "." in field["obf"] and "(" not in field["obf"]:
            owner, _name, _d = split_key(field["obf"])
            return class_root(class_uf, rev, owner)
        return cls["croot"]

    # Attach referencing method roots onto fields.
    field_index: dict[tuple[int, str, str, str], dict] = {}
    for rev, classes in deob.items():
        for cls in classes:
            for f in cls["fields"]:
                field_index[(rev, cls["internal"], f["name"], f["desc"])] = f
                f["mrefs"] = set()
                f["cls"] = cls
    for rev, classes in deob.items():
        for cls in classes:
            for m in cls["methods"]:
                root = m.get("mroot")
                if root is None:
                    continue
                for ointernal, fname, fdesc in m["refs"]:
                    f = field_index.get((rev, ointernal, fname, fdesc))
                    if f is not None:
                        f["mrefs"].add(str(method_uf.find(root)))

    def struct_desc(rev: int, desc: str) -> str:
        def repl(match: re.Match) -> str:
            internal = match.group(1)
            ident_s = internal_obf.get((rev, internal))
            if not ident_s:
                return match.group(0)
            return "L@" + digest(class_root(class_uf, rev, ident_s)) + ";"
        return re.sub(r"L([^;]+);", repl, desc)

    field_groups_src: dict[tuple, list[dict]] = defaultdict(list)
    for rev, classes in deob.items():
        for cls in classes:
            for f in cls["fields"]:
                f["sdesc"] = struct_desc(rev, f["desc"])
                f["froot_hint"] = owner_root(rev, cls, f)
                field_groups_src[f["froot_hint"]].append(f)

    field_uf = um.UnionFind()
    for fgroup in field_groups_src.values():
        by_rev: dict[int, list[dict]] = defaultdict(list)
        for f in fgroup:
            field_uf.add(("f", f["cls"]["rev"], f["cls"]["internal"], f["name"], f["desc"]))
            by_rev[f["cls"]["rev"]].append(f)
        years = sorted(by_rev)
        for ya, yb in zip(years, years[1:]):
            fa, fb = by_rev[ya], by_rev[yb]
            used_a, used_b = set(), set()
            da, db = defaultdict(list), defaultdict(list)
            for f in fa:
                da[f["sdesc"]].append(f)
            for f in fb:
                db[f["sdesc"]].append(f)
            for desc, la in da.items():
                lb = db.get(desc) or []
                if len(la) == 1 and len(lb) == 1:
                    a, b = la[0], lb[0]
                    field_uf.union(fid(a), fid(b))
                    used_a.add(id(a))
                    used_b.add(id(b))
            for a in fa:
                if id(a) in used_a or len(a["mrefs"]) < 1:
                    continue
                scored = []
                for b in fb:
                    if id(b) in used_b:
                        continue
                    if a["sdesc"] != b["sdesc"]:
                        continue
                    inter = len(a["mrefs"] & b["mrefs"])
                    if inter:
                        scored.append((inter, b))
                if not scored:
                    continue
                scored.sort(key=lambda t: t[0], reverse=True)
                best, b = scored[0]
                second = scored[1][0] if len(scored) > 1 else 0
                if best >= 1 and best >= second + 1:
                    field_uf.union(fid(a), fid(b))
                    used_a.add(id(a))
                    used_b.add(id(b))

    field_members: dict[tuple, list[dict]] = defaultdict(list)
    for fgroup in field_groups_src.values():
        for f in fgroup:
            root = field_uf.find(fid(f))
            f["froot"] = root
            field_members[root].append(f)
    field_name: dict[tuple, str] = {}
    for root, members in field_members.items():
        chosen = None
        for rev in (910, 902, 887, 876, 790):
            for f in members:
                if f["cls"]["rev"] == rev and readable(f["name"]):
                    chosen = f["name"]
                    break
            if chosen:
                break
        if not chosen:
            earliest = min(members, key=lambda f: (f["cls"]["rev"], f.get("obf") or f["name"], f["desc"]))
            chosen = "f" + digest("field", earliest["cls"]["rev"], earliest.get("obf") or earliest["name"], earliest["sdesc"])
        field_name[root] = legal(chosen)

    return fqcn_of, method_name, field_name


def fid(f: dict):
    return ("f", f["cls"]["rev"], f["cls"]["internal"], f["name"], f["desc"])


def per_year_maps(deob, fqcn_of, method_name, field_name):
    """old token -> new token, plus class moves. Tokens are unique inside one year."""
    out = {}
    for rev, classes in deob.items():
        class_map: dict[str, tuple[str, str]] = {}
        member_map: dict[str, str] = {}
        used_dest: set[tuple[str, str]] = set()
        ordered = sorted(
            classes,
            key=lambda c: (0 if (c["pkg"], c["simple"]) == fqcn_of[c["croot"]] else 1, c["simple"]),
        )
        for cls in ordered:
            pkg, simple = fqcn_of[cls["croot"]]
            if (pkg, simple) in used_dest:
                simple = simple + "Y" + digest(cls["internal"])[:4]
                pkg = cls["pkg"] or "deob"
            used_dest.add((pkg, simple))
            # Only obfuscated class tokens and the client/null holders are safe to
            # rewrite globally. Readable names such as add or Stream are not unique.
            if "$" not in cls["internal"] and (pkg != cls["pkg"] or simple != cls["simple"]):
                if CLASS_N.fullmatch(cls["simple"]) or cls["simple"] in {"client", "null"}:
                    class_map[cls["simple"]] = (pkg, simple)
            for m in cls["methods"]:
                if m["name"] in ("<init>", "<clinit>"):
                    continue
                new = method_name.get(m.get("mroot"))
                if new and new != m["name"]:
                    prev = member_map.get(m["name"])
                    if prev is None or prev == new:
                        member_map[m["name"]] = new
            for f in cls["fields"]:
                new = field_name.get(f.get("froot"))
                if new and new != f["name"]:
                    prev = member_map.get(f["name"])
                    if prev is None or prev == new:
                        member_map[f["name"]] = new
        # Class-local signature collisions for renamed methods.
        member_map = disambiguate_methods(rev, class_map, member_map)
        member_map = {k: v for k, v in member_map.items() if OBF.match(k) and v != k}
        out[rev] = (class_map, member_map)
    return out


def split_params(s: str) -> list[str]:
    parts = []
    buf = ""
    depth = 0
    for ch in s:
        if ch in "<(":
            depth += 1
            buf += ch
        elif ch in ">)":
            depth = max(0, depth - 1)
            buf += ch
        elif ch == "," and depth == 0:
            parts.append(buf.strip())
            buf = ""
        else:
            buf += ch
    if buf.strip():
        parts.append(buf.strip())
    return parts


def norm_type(typ: str, class_map: dict[str, tuple[str, str]]) -> str:
    def rep(match: re.Match) -> str:
        tok = match.group(0)
        if tok in class_map:
            return class_map[tok][1]
        return tok
    return re.sub(r"\s+", "", re.sub(r"[A-Za-z_]\w*", rep, typ))


def param_sig(params: str, class_map) -> tuple[str, ...]:
    out = []
    for part in split_params(params):
        m = re.match(r"^(.*\S)\s+([A-Za-z_]\w*)(\s*(?:\[\s*\])*)\s*$", part.strip())
        if not m:
            out.append(norm_type(part, class_map))
            continue
        out.append(norm_type(m.group(1) + (m.group(3) or ""), class_map))
    return tuple(out)


def disambiguate_methods(rev: int, class_map, member_map: dict[str, str]) -> dict[str, str]:
    final = dict(member_map)
    root = src_root(rev)
    if not root.exists():
        return final
    texts = [(path, path.read_text(errors="ignore")) for path in root.rglob("*.java")]
    for _round in range(5):
        changed = False
        for _path, text in texts:
            buckets: dict[tuple, list[str]] = defaultdict(list)
            for match in METH_RE.finditer(text):
                name = match.group(2)
                new = final.get(name, name)
                buckets[(new, param_sig(match.group(3), class_map))].append(name)
            for names in buckets.values():
                ranked = sorted(set(names), key=lambda n: (0 if final.get(n, n) == n else 1, n))
                if len(ranked) < 2:
                    continue
                seen: dict[str, str] = {}
                for name in ranked:
                    cur = final.get(name, name)
                    if cur not in seen:
                        seen[cur] = name
                        continue
                    base = member_map.get(name, name)
                    nn = base + "b"
                    k = 2
                    while nn in seen or OBF.match(nn) or nn in KEYWORDS:
                        k += 1
                        nn = f"{base}b{k}"
                    if final.get(name) != nn:
                        final[name] = nn
                        changed = True
                    seen[nn] = name
        if not changed:
            break
    return final


def scan_replace(text: str, on_ident) -> str:
    out = []
    i = 0
    n = len(text)
    while i < n:
        c = text[i]
        if c == '"':
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
        elif c == "'":
            j = i + 1
            while j < n:
                if text[j] == "\\":
                    j += 2
                    continue
                if text[j] == "'":
                    j += 1
                    break
                j += 1
            out.append(text[i:j])
            i = j
        elif c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            if j < 0:
                j = n
            else:
                j += 1
            out.append(text[i:j])
            i = j
        elif c == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append(text[i:j])
            i = j
        else:
            m = IDENT.match(text, i)
            if m:
                tok = m.group(0)
                nxt = text[m.end()] if m.end() < n else ""
                out.append(on_ident(tok, nxt))
                i = m.end()
            else:
                out.append(c)
                i += 1
    return "".join(out)


def rewrite_tree(rev: int, class_map: dict[str, tuple[str, str]], member_map: dict[str, str]) -> tuple[int, int]:
    root = src_root(rev)
    files = list(root.rglob("*.java"))
    # Classes that will live in each package after the move.
    pkg_simples: dict[str, set[str]] = defaultdict(set)
    file_class: dict[Path, str] = {}
    for path in files:
        simple = path.name[:-5]
        file_class[path] = simple
        if simple in class_map:
            pkg, news = class_map[simple]
        else:
            rel = path.parent.relative_to(root)
            pkg = ".".join(rel.parts)
            news = simple
        pkg_simples[pkg].add(news)

    planned: list[tuple[Path, Path, str]] = []
    for path in files:
        text = path.read_text(errors="ignore")
        simple = file_class[path]
        if simple in class_map:
            new_pkg, new_simple = class_map[simple]
        else:
            rel = path.parent.relative_to(root)
            new_pkg = ".".join(rel.parts)
            new_simple = simple
        imports = re.findall(r"^import\s+([\w.]+)\s*;", text, re.M)
        imported: dict[str, str] = {}
        for imp in imports:
            isimple = imp.rsplit(".", 1)[-1]
            ipkg = imp.rsplit(".", 1)[0] if "." in imp else ""
            if isimple in class_map:
                np, ns = class_map[isimple]
                imported[ns] = f"{np}.{ns}" if np else ns
            else:
                imported[isimple] = imp
        used: set[str] = set()

        same_pkg_holders = {"Statics", "ObfuscatedName"}

        def see(tok: str, nxt: str) -> str:
            if tok == "null":
                if nxt == ".":
                    used.add(tok)
                return tok
            if tok in class_map or tok in same_pkg_holders:
                used.add(tok)
            return tok

        scan_replace(text, see)
        qualify: set[str] = set()
        need: list[str] = []
        scope = set(pkg_simples.get(new_pkg, set()))
        scope |= JAVA_LANG
        for old in list(used):
            if old in {"Statics", "ObfuscatedName"}:
                if new_pkg != "deob":
                    need.append(f"deob.{old}")
                continue
            if old not in class_map:
                continue
            np, ns = class_map[old]
            fq = f"{np}.{ns}" if np else ns
            if np == new_pkg:
                continue
            other = imported.get(ns)
            if other and other != fq:
                qualify.add(old)
                continue
            if ns in scope and (np, ns) != (new_pkg, ns):
                # Same simple name already in scope via same package or java.lang.
                if ns in pkg_simples.get(new_pkg, set()) or ns in JAVA_LANG:
                    qualify.add(old)
                    continue
            if ns not in imported:
                need.append(fq)
                imported[ns] = fq
                scope.add(ns)
        # Drop imports that clash with the file's own new simple name or same-package class.
        clean_imports = []
        seen_imp = set()
        for _s, fq in list(imported.items()):
            isimple = fq.rsplit(".", 1)[-1]
            ipkg = fq.rsplit(".", 1)[0] if "." in fq else ""
            if isimple in pkg_simples.get(new_pkg, set()) and ipkg != new_pkg:
                continue
            if fq in seen_imp:
                continue
            seen_imp.add(fq)
            clean_imports.append(fq)
        for fq in need:
            if fq not in seen_imp:
                isimple = fq.rsplit(".", 1)[-1]
                ipkg = fq.rsplit(".", 1)[0] if "." in fq else ""
                if isimple in pkg_simples.get(new_pkg, set()) and ipkg != new_pkg:
                    continue
                seen_imp.add(fq)
                clean_imports.append(fq)

        prev = {"tok": ""}

        def emit(tok: str, nxt: str) -> str:
            last = prev["tok"]
            prev["tok"] = tok
            if tok == "null" and tok in class_map and (nxt == "." or last == "class"):
                return "DeobNull"
            if tok in class_map and tok != "null":
                np, ns = class_map[tok]
                if tok in qualify:
                    return f"{np}.{ns}" if np else ns
                return ns
            if tok in member_map:
                return member_map[tok]
            return tok

        body_lines = []
        for line in text.splitlines(True):
            if re.match(r"package\s+[\w.]+\s*;\s*$", line):
                continue
            if re.match(r"import\s+[\w.]+\s*;\s*$", line):
                continue
            body_lines.append(line)
        body = scan_replace("".join(body_lines), emit).lstrip("\n")
        header = []
        if new_pkg:
            header.append(f"package {new_pkg};\n\n")
        if clean_imports:
            header.append("\n".join(f"import {imp};" for imp in sorted(set(clean_imports))) + "\n\n")
        new_text = "".join(header) + body
        if new_pkg:
            dest = root.joinpath(*new_pkg.split("."), new_simple + ".java")
        else:
            dest = root / (new_simple + ".java")
        planned.append((path, dest, new_text))

    # Two classes must not land on one path.
    seen_dest: dict[Path, Path] = {}
    fixed: list[tuple[Path, Path, str]] = []
    for src, dest, text in planned:
        if dest in seen_dest and seen_dest[dest] != src:
            dest = dest.with_name(dest.stem + "Alt.java")
            print(f"dest clash {src} -> {dest}", flush=True)
        seen_dest[dest] = src
        fixed.append((src, dest, text))
    planned = fixed

    # Write via temporary names so case-only renames and swaps work.
    written = 0
    moved = 0
    tmp_files = []
    for i, (src, dest, text) in enumerate(planned):
        dest.parent.mkdir(parents=True, exist_ok=True)
        tmp = dest.with_name(f".{i}.{dest.name}.rs3tmp")
        tmp.write_text(text)
        tmp_files.append((src, dest, tmp))
    old_paths = {src for src, _dest, _tmp in tmp_files}
    new_paths = {dest for _src, dest, _tmp in tmp_files}
    for src, dest, tmp in tmp_files:
        if src != dest and src.exists() and src not in new_paths:
            src.unlink()
        if dest.exists() and dest != tmp:
            dest.unlink()
        tmp.rename(dest)
        written += 1
        if src != dest:
            moved += 1
    # Remove sources that were renamed onto a path we no longer keep.
    for src, dest, _tmp in tmp_files:
        if src != dest and src.exists() and src not in new_paths:
            src.unlink()
    _ = old_paths
    return written, moved


def patch_remaining(deob_maps: dict[int, dict[str, str]]) -> None:
    """Replace obfuscated tokens left on inner classes. Do not move files."""
    for rev, member_map in deob_maps.items():
        root = src_root(rev)
        changed = 0
        for path in root.rglob("*.java"):
            text = path.read_text(errors="ignore")

            def emit(tok: str, _nxt: str, mapping=member_map) -> str:
                if tok in mapping and OBF.match(tok):
                    return mapping[tok]
                return tok

            nt = scan_replace(text, emit)
            if nt != text:
                path.write_text(nt)
                changed += 1
        print(f"patched obf tokens in {changed} files for {rev}", flush=True)
        # Remap RHS still holding a jar name we renamed (fn/method/field).
        path = REMAP_DIR / f"rs3-{rev}.txt"
        if not path.exists():
            continue
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
            simple = right.strip()
            if simple in member_map and OBF.match(simple):
                out.append(f"{left.strip()}={member_map[simple]}")
            else:
                out.append(body)
        path.write_text("\n".join(out) + "\n")


def update_remaps(deob, method_name, field_name, fqcn_of, ident):
    for rev, classes in deob.items():
        path = REMAP_DIR / f"rs3-{rev}.txt"
        existing = path.read_text(errors="ignore").splitlines() if path.exists() else []
        updates: dict[str, str] = {}
        for cls in classes:
            if cls["obf"] and "." not in cls["obf"] and "(" not in cls["obf"]:
                pkg, simple = fqcn_of[cls["croot"]]
                fq = f"{pkg}.{simple}" if pkg else simple
                updates[cls["obf"]] = fq
            for m in cls["methods"]:
                if not m["obf"] or "(" not in m["obf"]:
                    continue
                new = method_name.get(m.get("mroot"))
                if new:
                    updates[m["obf"]] = new
            for f in cls["fields"]:
                if not f["obf"] or "(" in f["obf"] or "." not in f["obf"]:
                    continue
                new = field_name.get(f.get("froot"))
                if new:
                    updates[f["obf"]] = new
        kept = []
        seen = set()
        for raw in existing:
            body = raw.split("#", 1)[0].strip()
            if "=" not in body:
                kept.append(raw)
                continue
            left, right = body.split("=", 1)
            left = left.strip()
            if left in seen:
                kept.append(raw)
                continue
            seen.add(left)
            if "," in right:
                kept.append(body)
                updates.pop(left, None)
                continue
            if left in updates:
                kept.append(f"{left}={updates.pop(left)}")
            else:
                kept.append(body)
        for left in sorted(updates):
            if left in seen:
                continue
            kept.append(f"{left}={updates[left]}")
        path.write_text("\n".join(kept) + "\n")
        print(f"remap rs3-{rev}.txt lines={len(kept)}", flush=True)


def proactive_fixes():
    for rev in REVS:
        root = src_root(rev)
        for path in root.rglob("*.java"):
            text = path.read_text(errors="ignore")
            nt = text.replace("B var22;", "byte[] var22;")
            if path.name == "SimplePeer.java":
                nt = nt.replace("public static native void init();", "public static native void initClass();")
                nt = nt.replace("static {\n      init();\n   }", "static {\n      initClass();\n   }")
            if nt != text:
                path.write_text(nt)


def count_obf(rev: int) -> dict[str, int]:
    root = src_root(rev)
    pats = {
        "method": re.compile(r"\bmethod\d+\b"),
        "field": re.compile(r"\bfield\d+\b"),
        "class": re.compile(r"\bclass\d+\b"),
        "anInt": re.compile(r"\banInt\d+\b"),
        "fn": re.compile(r"\bfn[0-9a-f]{8,}\b"),
    }
    counts = {k: 0 for k in pats}
    counts["classFiles"] = 0
    counts["nullFiles"] = 0
    for path in root.rglob("*.java"):
        if CLASS_N.fullmatch(path.name[:-5]):
            counts["classFiles"] += 1
        if path.name == "null.java":
            counts["nullFiles"] += 1
        text = path.read_text(errors="ignore")
        for key, pat in pats.items():
            counts[key] += len(pat.findall(text))
    return counts


def ensure_maxerrs(proj: Path) -> None:
    p = proj / "build.gradle"
    text = p.read_text()
    if "-Xmaxerrs" in text:
        return
    text = text.replace(
        "options.encoding = 'UTF-8'",
        "options.encoding = 'UTF-8'\n    options.compilerArgs += ['-Xmaxerrs', '10000', '-Xmaxwarns', '10000']",
    )
    p.write_text(text)


def compile_rev(rev: int) -> tuple[int, str]:
    proj = BUILDS / FOLDERS[rev] / "src"
    ensure_maxerrs(proj)
    logf = ROOT / "logs" / f"compile-{FOLDERS[rev]}.log"
    env = os.environ.copy()
    env["JAVA_HOME"] = str(JAVA.parent.parent)
    env["PATH"] = str(JAVA.parent) + os.pathsep + env.get("PATH", "")
    gh = Path("/tmp/rsarchive-compile-now") / f"gradle-{FOLDERS[rev]}"
    gh.mkdir(parents=True, exist_ok=True)
    env["GRADLE_USER_HOME"] = str(gh)
    gradlew = proj / "gradlew"
    os.chmod(gradlew, 0o755)
    with logf.open("w") as fh:
        rc = subprocess.run(
            [str(gradlew), "--no-daemon", "compileJava"],
            cwd=proj, env=env, stdout=fh, stderr=subprocess.STDOUT,
        ).returncode
    return rc, logf.read_text(errors="ignore")


def parse_javac(log_text: str) -> list[tuple[Path, int, str, str]]:
    lines = log_text.splitlines()
    out = []
    for i, line in enumerate(lines):
        m = re.match(r"^(.+\.java):(\d+): error: (.+)$", line)
        if not m or line.startswith(" "):
            continue
        extra = "\n".join(lines[i + 1 : i + 4])
        out.append((Path(m.group(1)), int(m.group(2)), m.group(3), extra))
    return out


def split_foreach(header: str):
    depth = 0
    for i, ch in enumerate(header):
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        elif ch == ":" and depth == 0:
            left, right = header[:i].strip(), header[i + 1 :].strip()
            m = re.match(r"([A-Za-z_][\w.]*(?:\s*\[\s*\])*)\s+([A-Za-z_]\w*)$", left)
            if not m:
                return None
            return m.group(1).replace(" ", ""), m.group(2), right
    return None


def fix_foreach_line(line: str, target: str) -> str | None:
    m = re.match(r"^(\s*)for\s*\((.*)\)\s*\{\s*$", line)
    if not m:
        return None
    parts = split_foreach(m.group(2))
    if not parts:
        return None
    typ, var, expr = parts
    if typ != target and target not in typ:
        # Still the reported target type; trust the source type when it matches loosely.
        if target not in ("int", "Entry", "Integer") and target not in typ:
            typ = target
    if "Iterable<" in expr or expr.startswith("("):
        # Already cast, or parenthesized call. Still wrap once if not iterable.
        if "Iterable<" in expr:
            return None
    box = BOX.get(typ)
    cast = f"Iterable<{box}>" if box else f"Iterable<{typ}>"
    return f"{m.group(1)}for ({typ} {var} : ({cast}) ({expr})) {{\n"


def hoist_catch(text: str, name: str) -> str | None:
    m = re.search(rf"catch\s*\(\s*([\w.]+)\s+{re.escape(name)}\s*\)", text)
    if not m:
        return None
    typ = m.group(1)
    if re.search(rf"\b{re.escape(typ)}\s+{re.escape(name)}\s*=", text):
        return None
    # Rename the catch parameter and assign the outer local.
    nt = text[: m.start()] + f"catch ({typ} {name}Caught)" + text[m.end() :]
    # Insert assignment as the first statement of that catch.
    brace = nt.find("{", m.start())
    if brace < 0:
        return None
    insert = f"\n         {name} = {name}Caught;"
    nt = nt[: brace + 1] + insert + nt[brace + 1 :]
    # Declare before the matching try. Walk back to the nearest try.
    try_at = nt.rfind("try", 0, m.start())
    if try_at < 0:
        return None
    line_start = nt.rfind("\n", 0, try_at) + 1
    decl = f"{typ} {name} = null;\n" + " " * (try_at - line_start)
    nt = nt[:line_start] + decl + nt[line_start:]
    return nt


def fix_single_letter(text: str, letter: str) -> str | None:
    m = re.search(rf"(?m)^(\s*){re.escape(letter)}\s+(\w+)\s*;\s*$", text)
    if not m:
        var = m
        return None
    var = m.group(2)
    cast = re.search(rf"\(([A-Za-z_][\w.]*(?:\s*\[\s*\])*)\)\s*{re.escape(var)}\b", text)
    if not cast:
        repl = "byte[]" if letter == "B" else "Object"
    else:
        repl = re.sub(r"\s+", "", cast.group(1))
    nt = text[: m.start(0)] + f"{m.group(1)}{repl} {var};" + text[m.end(0) :]
    return nt if nt != text else None


def apply_log_fixes(log_text: str) -> int:
    errors = parse_javac(log_text)
    # One fix per file line, first error wins.
    seen = set()
    changed = 0
    file_cache: dict[Path, str] = {}
    for path, line_no, msg, extra in errors:
        key = (path, line_no, msg)
        if key in seen or not path.exists():
            continue
        seen.add(key)
        text = file_cache.get(path)
        if text is None:
            text = path.read_text(errors="ignore")
            file_cache[path] = text
        lines = text.splitlines(True)
        if line_no < 1 or line_no > len(lines):
            continue
        line = lines[line_no - 1]
        nt_line = None
        if msg.startswith("incompatible types: Object cannot be converted to "):
            target = msg.split("converted to ", 1)[1].strip()
            nt_line = fix_foreach_line(line, target)
            if nt_line:
                lines[line_no - 1] = nt_line
                file_cache[path] = "".join(lines)
                changed += 1
                continue
        if "cannot assign a value to static final variable" in msg:
            fname = msg.rsplit(" ", 1)[-1]
            nt = re.sub(
                rf"((?:public|protected|private|static|\s)+)final(\s+[\w.<>\[\]]+\s+{re.escape(fname)}\b)",
                r"\1\2",
                text,
                count=1,
            )
            if nt != text:
                file_cache[path] = nt
                changed += 1
            continue
        if "cannot find symbol" in msg:
            vm = re.search(r"symbol:\s+variable\s+(\w+)", extra)
            cm = re.search(r"symbol:\s+class\s+([A-Z])\b", extra)
            if cm:
                nt = fix_single_letter(text, cm.group(1))
                if nt:
                    file_cache[path] = nt
                    changed += 1
                continue
            if vm:
                nt = hoist_catch(text, vm.group(1))
                if nt:
                    file_cache[path] = nt
                    changed += 1
                continue
        if "<identifier> expected" in msg and "null" in line:
            nt = line.replace("deob.null", "deob.DeobNull").replace("class null", "class DeobNull")
            if nt != line:
                lines[line_no - 1] = nt
                file_cache[path] = "".join(lines)
                changed += 1
    for path, text in file_cache.items():
        if path.exists() and path.read_text(errors="ignore") != text:
            path.write_text(text)
    return changed


def compile_until(revs: list[int], rounds: int = 12) -> dict[int, bool]:
    status = {}
    for rev in revs:
        ok = False
        for attempt in range(rounds):
            rc, text = compile_rev(rev)
            if rc == 0 or "BUILD SUCCESSFUL" in text:
                print(f"compile {rev} OK", flush=True)
                ok = True
                break
            errors = parse_javac(text)
            # The fixer script handles a few patterns; ours handles foreach and scope.
            n = apply_log_fixes(text)
            sys.path.insert(0, str(ROOT / "scripts"))
            import fix_decompiled_java as fixer
            n += fixer.apply_fixes(fixer.parse_errors(text), src_root(rev))
            print(f"compile {rev} attempt {attempt + 1}: {len(errors)} errors, {n} fixes", flush=True)
            if n == 0:
                break
        status[rev] = ok
    return status


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--finish", action="store_true", help="Rename leftover inner-class obf tokens only")
    ap.add_argument("--compile-fix", action="store_true")
    ap.add_argument("--revs", default="790,876,887,902,910")
    args = ap.parse_args()
    want = [int(x) for x in args.revs.split(",") if int(x) not in SKIP_REVS]
    if args.compile_fix and not args.apply:
        status = compile_until(want)
        for rev in want:
            print(rev, "COMPILE" if status[rev] else "FAIL", count_obf(rev))
        return 0 if all(status.values()) else 1

    print("parsing deob jars", flush=True)
    deob = {rev: load_deob(rev) for rev in want}
    for rev, classes in deob.items():
        ann = sum(1 for c in classes if c["obf"])
        print(f"  {rev}: classes={len(classes)} annotated={ann}", flush=True)
    # Sanity: 790 class164 is ek.
    hit = [c for c in deob.get(790, []) if c["simple"] == "class164"]
    if hit:
        print(f"  class164 obf={hit[0]['obf']}", flush=True)
        if hit[0]["obf"] != "ek":
            print("annotation parser mismatch", flush=True)
            return 1

    method_uf, class_uf, anchors, ident = build_clusters(deob)
    print("assigning names", flush=True)
    fqcn_of, method_name, field_name = assign_names(deob, method_uf, class_uf, anchors, ident)
    maps = per_year_maps(deob, fqcn_of, method_name, field_name)
    for rev, (class_map, member_map) in maps.items():
        print(f"  {rev}: class renames={len(class_map)} member renames={len(member_map)}", flush=True)
    # Show a known 910 name spreading.
    for rev, classes in deob.items():
        if rev != 910:
            continue
        for cls in classes:
            if cls["simple"] != "StringTools":
                continue
            names = sorted({method_name.get(m.get("mroot")) for m in cls["methods"] if m["name"] not in ("<init>", "<clinit>")})
            print("  StringTools methods sample", names[:12], flush=True)
    if args.finish:
        patch_remaining({rev: member_map for rev, (_c, member_map) in maps.items()})
        for rev in want:
            print(f"obf {rev}", count_obf(rev), flush=True)
        if args.compile_fix:
            status = compile_until(want)
            for rev in want:
                print(rev, "COMPILE" if status[rev] else "FAIL", count_obf(rev), flush=True)
            return 0 if all(status.values()) else 1
        return 0
    if not args.apply:
        print("dry-run only; pass --apply to write sources")
        return 0
    for rev, (class_map, member_map) in maps.items():
        written, moved = rewrite_tree(rev, class_map, member_map)
        print(f"wrote {rev}: files={written} moved={moved}", flush=True)
    proactive_fixes()
    update_remaps(deob, method_name, field_name, fqcn_of, ident)
    for rev in want:
        print(f"obf {rev}", count_obf(rev), flush=True)
    if args.compile_fix:
        status = compile_until(want)
        for rev in want:
            print(rev, "COMPILE" if status[rev] else "FAIL", count_obf(rev), flush=True)
        return 0 if all(status.values()) else 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
