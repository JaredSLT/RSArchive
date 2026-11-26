#!/usr/bin/env python3
"""Share RS3 class, method, and field names by structural fingerprint.

A class matches across revisions when the multiset of its method descriptors
and the multiset of its field types is unique in each of the two years.
Synthetic type names (classN, DataBlock<digits>, Cls+hex) are omitted from
the fingerprint. Readable superclass names are not required. A synthetic
superclass name is never part of the fingerprint.

Adjacent revisions are compared, and each revision is also compared directly
with 910. A pair links only when the fingerprint is unique on both sides,
the classes do not already have two different readable names, and the class
has at least 8 methods, or at least 8 fields, or the two classes share a
string literal of length >= 8.

910's readable name wins, then 902, 887, 876, 790. A cluster whose names are
all synthetic keeps the synthetic name that already occurs in the most years.
No new prefix is minted. A cluster whose rename would clobber a file is skipped.

Dry-run by default. --apply rewrites sources. The first backup of each edited
file is work/share-rs3/backup-struct/<rev>/ and is not overwritten.
"""
from __future__ import annotations

import argparse
import json
import re
import shutil
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import share_rs3_names as base  # noqa: E402

ROOT = base.ROOT
REVS = base.REVS
FOLDERS = base.FOLDERS
JAVA_ROOT = base.java_root
BACKUP = ROOT / "work" / "share-rs3" / "backup-struct"
PRIO = {910: 70, 902: 60, 887: 50, 876: 40, 790: 30, 860: 20, 831: 10}
ADJACENT = list(zip(REVS, REVS[1:]))
PAIRS = list(dict.fromkeys([*ADJACENT, *[(r, 910) for r in REVS if r != 910]]))
SCALAR_FIELD = {"int", "long", "boolean", "byte", "short", "String"}
IDENT_RE = base.IDENT_RE
KEYWORDS = base.KEYWORDS


def log(msg: str) -> None:
    print(msg, flush=True)


def synthetic_type(name: str) -> bool:
    return bool(base.SYN_CLASS_RE.match(name))


def callback_name(name: str) -> bool:
    return base.callback_name(name)


def simple_type(raw: str) -> str:
    """Simple name plus array dimensions. Synthetic class names become '*'."""
    t = re.sub(r"\s+", "", raw or "")
    dims = ""
    while t.endswith("[]"):
        dims += "[]"
        t = t[:-2]
    if "<" in t:
        t = t[: t.find("<")]
    simple = t.split(".")[-1] if t else ""
    if not simple:
        return "*" + dims
    if simple in base.PRIM:
        return simple + dims
    if synthetic_type(simple):
        return "*" + dims
    return simple + dims


def method_desc(method: base.JMethod) -> str:
    ret = simple_type(getattr(method, "_ret", "void"))
    params = tuple(simple_type(p) for p in getattr(method, "_params", []))
    return f"({','.join(params)}){ret}"


def field_type(field: base.JField) -> str:
    return simple_type(field.descriptor)


def fingerprint(jc: base.JClass) -> tuple:
    methods = tuple(sorted(Counter(method_desc(m) for m in jc.methods).items()))
    fields = tuple(sorted(Counter(field_type(f) for f in jc.fields).items()))
    return (methods, fields)


def shares_string(a: base.JClass, b: base.JClass) -> bool:
    if not a.strings or not b.strings:
        return False
    return any(len(s) >= 8 and s in b.strings for s in a.strings)


def big_enough(a: base.JClass, b: base.JClass) -> bool:
    return (
        (len(a.methods) >= 8 and len(b.methods) >= 8)
        or (len(a.fields) >= 8 and len(b.fields) >= 8)
        or shares_string(a, b)
    )


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


def load_all() -> dict[int, list[base.JClass]]:
    out: dict[int, list[base.JClass]] = {}
    for rev in REVS:
        items: list[base.JClass] = []
        root = JAVA_ROOT(rev)
        for path in root.rglob("*.java"):
            if "build" in path.parts:
                continue
            text = path.read_text(errors="ignore")
            items.extend(base.parse_file(rev, path, text))
        out[rev] = items
        log(f"parsed {rev}: classes={len(items)}")
    return out


def link_classes(classes: dict[int, list[base.JClass]]):
    uf = UnionFind()
    ids: dict[tuple, int] = {}
    by_id: dict[int, base.JClass] = {}
    fp_of: dict[tuple, tuple] = {}
    n = 0
    for rev, items in classes.items():
        for jc in items:
            ids[jc.key] = n
            by_id[n] = jc
            fp_of[jc.key] = fingerprint(jc)
            label = jc.simple if base.readable(jc.simple) else None
            uf.add(n, rev, label)
            n += 1
    by_fp: dict[int, dict[tuple, list[base.JClass]]] = defaultdict(lambda: defaultdict(list))
    for jc in by_id.values():
        by_fp[jc.rev][fp_of[jc.key]].append(jc)
    linked = 0
    considered = 0
    for left, right in PAIRS:
        fl = by_fp[left]
        fr = by_fp[right]
        for fp, group in fl.items():
            if len(group) != 1:
                continue
            other = fr.get(fp)
            if not other or len(other) != 1:
                continue
            a, b = group[0], other[0]
            considered += 1
            if not big_enough(a, b):
                continue
            if uf.union(ids[a.key], ids[b.key]):
                linked += 1
    clusters: dict[int, list[base.JClass]] = defaultdict(list)
    cluster_of: dict[tuple, int] = {}
    for key, i in ids.items():
        root = uf.find(i)
        clusters[root].append(by_id[i])
        cluster_of[key] = root
    log(f"classes={n} pair-candidates={considered} links={linked} clusters={len(clusters)}")
    return clusters, cluster_of


def choose_name(pairs: list[tuple[base.JClass, str]]) -> str:
    """pairs are (owning class, existing name)."""
    ranked = []
    for jc, name in pairs:
        if base.readable(name):
            bonus = 100 if callback_name(name) else 0
            ranked.append((PRIO[jc.rev] + bonus, name))
    if ranked:
        ranked.sort(reverse=True)
        return ranked[0][1]
    counts: dict[str, set[int]] = defaultdict(set)
    for jc, name in pairs:
        counts[name].add(jc.rev)
    ranked_s = sorted(counts.items(), key=lambda kv: (-len(kv[1]), kv[0]))
    return ranked_s[0][0]


def class_plan(clusters: dict[int, list[base.JClass]]) -> dict[tuple, str]:
    """key -> new simple name, for synthetic classes in multi-year clusters."""
    out: dict[tuple, str] = {}
    for members in clusters.values():
        years = {jc.rev for jc in members}
        if len(years) < 2:
            continue
        chosen = choose_name([(jc, jc.simple) for jc in members])
        if chosen in KEYWORDS or not IDENT_RE.fullmatch(chosen):
            continue
        for jc in members:
            if jc.simple != chosen and base.synthetic_class(jc.simple):
                out[jc.key] = chosen
    return out


def drop_clobbering_clusters(classes, clusters, plan: dict[tuple, str]) -> int:
    """Skip a whole cluster when any rename would take an existing file or simple name."""
    by_key = {jc.key: jc for items in classes.values() for jc in items}
    by_pkg: dict[tuple, set[str]] = defaultdict(set)
    names_in_rev: dict[int, set[str]] = defaultdict(set)
    files: dict[tuple, set[Path]] = defaultdict(set)
    for items in classes.values():
        for jc in items:
            by_pkg[(jc.rev, jc.package)].add(jc.simple)
            names_in_rev[jc.rev].add(jc.simple)
            if jc.top:
                files[(jc.rev, jc.package)].add(jc.path)
    cluster_of_key: dict[tuple, int] = {}
    for root, members in clusters.items():
        for jc in members:
            cluster_of_key[jc.key] = root
    blocked: set[int] = set()
    wanted: dict[int, set[tuple]] = defaultdict(set)
    for key, new in plan.items():
        root = cluster_of_key[key]
        wanted[root].add((key[0], key[1], new))
        jc = by_key[key]
        if new in by_pkg[(jc.rev, jc.package)] and new != jc.simple:
            blocked.add(root)
            continue
        # A second top-level type with the same simple name is ambiguous
        # even in another package, and javac rejects the single-type import.
        if new in names_in_rev[jc.rev] and new != jc.simple:
            blocked.add(root)
            continue
        dest = jc.path.with_name(new + ".java")
        if dest.exists() and dest != jc.path:
            blocked.add(root)
    # Two classes in one year/package aiming at the same new file.
    for root, slots in wanted.items():
        seen: set[tuple] = set()
        for slot in slots:
            if slot in seen:
                blocked.add(root)
            seen.add(slot)
    dropped = 0
    for key in list(plan):
        if cluster_of_key[key] in blocked:
            del plan[key]
            dropped += 1
    log(f"class-renames={len(plan)} skipped-clobber={dropped} blocked-clusters={len(blocked)}")
    return dropped


def norm_member_type(raw: str, rev: int, cluster_of: dict[tuple, int], by_name) -> str:
    t = re.sub(r"\s+", "", raw or "")
    dims = ""
    while t.endswith("[]"):
        dims += "[]"
        t = t[:-2]
    if "<" in t:
        t = t[: t.find("<")]
    simple = t.split(".")[-1] if t else ""
    if simple in base.PRIM:
        return simple + dims
    owners = by_name.get((rev, simple), [])
    if len(owners) == 1:
        cid = cluster_of.get(owners[0].key)
        if cid is not None and len({jc.rev for jc in []}) == 0:
            return f"#{cid}{dims}"
    if synthetic_type(simple):
        return "*" + dims
    return (simple or "*") + dims


def resolved_type(raw: str, rev: int, cluster_of: dict[tuple, int], by_name, clusters) -> str:
    t = re.sub(r"\s+", "", raw or "")
    dims = ""
    while t.endswith("[]"):
        dims += "[]"
        t = t[:-2]
    if "<" in t:
        t = t[: t.find("<")]
    simple = t.split(".")[-1] if t else ""
    if simple in base.PRIM:
        return simple + dims
    owners = by_name.get((rev, simple), [])
    if len(owners) == 1:
        cid = cluster_of.get(owners[0].key)
        if cid is not None:
            return f"#{cid}{dims}"
    if synthetic_type(simple):
        return "*" + dims
    return (simple or "*") + dims


def member_desc(method: base.JMethod, rev: int, cluster_of, by_name) -> str:
    ret = resolved_type(getattr(method, "_ret", "void"), rev, cluster_of, by_name, None)
    params = [resolved_type(p, rev, cluster_of, by_name, None) for p in getattr(method, "_params", [])]
    return f"({','.join(params)}){ret}"


def link_members(classes, clusters, cluster_of):
    by_name: dict[tuple, list[base.JClass]] = defaultdict(list)
    for items in classes.values():
        for jc in items:
            by_name[(jc.rev, jc.simple)].append(jc)
    method_props: dict[int, dict[tuple, str]] = defaultdict(dict)
    field_props: dict[int, dict[tuple, str]] = defaultdict(dict)
    m_groups = 0
    f_groups = 0
    for members in clusters.values():
        if len({jc.rev for jc in members}) < 2:
            continue
        if len(members) != len({jc.rev for jc in members}):
            continue
        desc_methods: dict[str, list[tuple[base.JClass, base.JMethod]]] = defaultdict(list)
        for jc in members:
            seen: Counter[str] = Counter()
            descs = []
            for m in jc.methods:
                d = member_desc(m, jc.rev, cluster_of, by_name)
                descs.append(d)
                seen[d] += 1
            for m, d in zip(jc.methods, descs):
                if seen[d] == 1:
                    desc_methods[d].append((jc, m))
        for d, group in desc_methods.items():
            years = [jc.rev for jc, _m in group]
            if len(years) != len(set(years)) or len(years) < 2:
                continue
            m_groups += 1
            chosen = choose_name([(jc, m.name) for jc, m in group])
            if chosen in KEYWORDS or callback_name(chosen) and any(m.name != chosen for _jc, m in group):
                if chosen in KEYWORDS:
                    continue
            for jc, m in group:
                if callback_name(m.name) or not base.synthetic_member(m.name):
                    continue
                if m.name == chosen:
                    continue
                method_props[jc.rev][(jc.key, m.name, d, m.arity)] = chosen
        type_fields: dict[str, list[tuple[base.JClass, base.JField]]] = defaultdict(list)
        for jc in members:
            seen_t: Counter[str] = Counter()
            types = []
            for f in jc.fields:
                ty = resolved_type(f.descriptor, jc.rev, cluster_of, by_name, None)
                types.append(ty)
                seen_t[ty] += 1
            raw_counts: Counter[str] = Counter(field_type(f) for f in jc.fields)
            for f, ty in zip(jc.fields, types):
                raw = field_type(f)
                bare = raw.replace("[]", "")
                if bare in SCALAR_FIELD and raw_counts[raw] > 1:
                    continue
                if seen_t[ty] != 1:
                    continue
                type_fields[ty].append((jc, f))
        for ty, group in type_fields.items():
            years = [jc.rev for jc, _f in group]
            if len(years) != len(set(years)) or len(years) < 2:
                continue
            f_groups += 1
            chosen = choose_name([(jc, f.name) for jc, f in group])
            if chosen in KEYWORDS:
                continue
            for jc, f in group:
                if not base.synthetic_member(f.name) or f.name == chosen:
                    continue
                field_props[jc.rev][(jc.key, f.name, ty)] = chosen
    log(f"method-groups={m_groups} field-groups={f_groups}")
    return method_props, field_props


def freeze(classes, clusters, cluster_of, method_props, field_props) -> None:
    by_name: dict[tuple, list[base.JClass]] = defaultdict(list)
    for items in classes.values():
        for jc in items:
            by_name[(jc.rev, jc.simple)].append(jc)
    frozen = 0

    def method_new(rev, jc, m, d) -> str:
        return method_props.get(rev, {}).get((jc.key, m.name, d, m.arity), m.name)

    for rev, items in classes.items():
        for jc in items:
            groups: dict[tuple, list[tuple[base.JMethod, str]]] = defaultdict(list)
            for m in jc.methods:
                d = member_desc(m, jc.rev, cluster_of, by_name)
                groups[(m.name, m.arity)].append((m, d))
            for group in groups.values():
                if len(group) > 1:
                    for m, d in group:
                        key = (jc.key, m.name, d, m.arity)
                        if key in method_props.get(rev, {}):
                            del method_props[rev][key]
                            frozen += 1
            # New names must not pile distinct descriptors onto one name+descriptor,
            # and must not collide with a field name.
            planned: dict[tuple, list[tuple[base.JMethod, str]]] = defaultdict(list)
            for m in jc.methods:
                d = member_desc(m, jc.rev, cluster_of, by_name)
                planned[(method_new(rev, jc, m, d), d)].append((m, d))
            for group in planned.values():
                olds = {m.name for m, _d in group}
                if len(group) > 1 and len(olds) > 1:
                    for m, d in group:
                        key = (jc.key, m.name, d, m.arity)
                        if key in method_props.get(rev, {}):
                            del method_props[rev][key]
                            frozen += 1
            field_names: dict[str, list[base.JField]] = defaultdict(list)
            for f in jc.fields:
                ty = resolved_type(f.descriptor, jc.rev, cluster_of, by_name, None)
                new = field_props.get(rev, {}).get((jc.key, f.name, ty), f.name)
                field_names[new].append(f)
            for group in field_names.values():
                if len({f.name for f in group}) > 1:
                    for f in group:
                        ty = resolved_type(f.descriptor, jc.rev, cluster_of, by_name, None)
                        key = (jc.key, f.name, ty)
                        if key in field_props.get(rev, {}):
                            del field_props[rev][key]
                            frozen += 1

    def parent_of(jc: base.JClass, simple: str) -> base.JClass | None:
        cands = [c for c in by_name.get((jc.rev, simple), []) if c is not jc]
        same = [c for c in cands if c.package == jc.package]
        if len(same) == 1:
            return same[0]
        if len(cands) == 1:
            return cands[0]
        return None

    for _ in range(6):
        changed = False
        for rev, items in classes.items():
            for jc in items:
                chain = []
                seen = {jc.key}
                queue = list(jc.supers)
                cursor = jc
                while queue:
                    name = queue.pop()
                    parent = parent_of(cursor, name)
                    if parent is None or parent.key in seen:
                        continue
                    seen.add(parent.key)
                    chain.append(parent)
                    cursor = parent
                    queue.extend(parent.supers)
                pmap: dict[tuple, tuple[base.JClass, base.JMethod, str]] = {}
                for parent in chain:
                    for pm in parent.methods:
                        d = member_desc(pm, parent.rev, cluster_of, by_name)
                        pmap.setdefault((pm.name, d), (parent, pm, d))
                for m in jc.methods:
                    d = member_desc(m, jc.rev, cluster_of, by_name)
                    found = pmap.get((m.name, d))
                    if not found:
                        continue
                    parent, pm, pd = found
                    child_new = method_new(rev, jc, m, d)
                    parent_new = method_new(rev, parent, pm, pd)
                    if child_new == parent_new:
                        continue
                    ck = (jc.key, m.name, d, m.arity)
                    pk = (parent.key, pm.name, pd, pm.arity)
                    if ck in method_props.get(rev, {}):
                        del method_props[rev][ck]
                        frozen += 1
                        changed = True
                    if pk in method_props.get(rev, {}):
                        del method_props[rev][pk]
                        frozen += 1
                        changed = True
        if not changed:
            break
    log(f"frozen-members={frozen}")


def global_maps(classes, class_plan_map, method_props, field_props, cluster_of):
    by_name: dict[tuple, list[base.JClass]] = defaultdict(list)
    for items in classes.values():
        for jc in items:
            by_name[(jc.rev, jc.simple)].append(jc)
    per: dict[int, dict[str, str]] = {}
    detail = {}
    for rev, items in classes.items():
        uses: dict[str, set[str | None]] = defaultdict(set)
        for jc in items:
            new_c = class_plan_map.get(jc.key, jc.simple)
            uses[jc.simple].add(new_c if new_c != jc.simple else None)
            for m in jc.methods:
                d = member_desc(m, jc.rev, cluster_of, by_name)
                new_m = method_props.get(rev, {}).get((jc.key, m.name, d, m.arity), m.name)
                uses[m.name].add(new_m if new_m != m.name else None)
            for f in jc.fields:
                ty = resolved_type(f.descriptor, jc.rev, cluster_of, by_name, None)
                new_f = field_props.get(rev, {}).get((jc.key, f.name, ty), f.name)
                uses[f.name].add(new_f if new_f != f.name else None)
        mapping: dict[str, str] = {}
        kinds = Counter()
        class_names = {jc.simple for jc in items}
        method_names = {m.name for jc in items for m in jc.methods}
        field_names = {f.name for jc in items for f in jc.fields}
        for old, targets in uses.items():
            news = {t for t in targets if t}
            if len(news) != 1 or None in targets:
                continue
            new = next(iter(news))
            if new == old or new in KEYWORDS or not IDENT_RE.fullmatch(new):
                continue
            mapping[old] = new
            if old in class_names and base.synthetic_class(old):
                kinds["class"] += 1
            elif old in method_names and base.synthetic_member(old):
                kinds["method"] += 1
            elif old in field_names and base.synthetic_member(old):
                kinds["field"] += 1
            else:
                kinds["other"] += 1
        per[rev] = mapping
        detail[rev] = dict(kinds)
    log("applied-idents " + json.dumps(detail))
    return per, detail


def sharing_stats(clusters, class_plan_map, maps) -> dict:
    multi = [m for m in clusters.values() if len({jc.rev for jc in m}) >= 2]
    shared = 0
    for members in multi:
        chosen = choose_name([(jc, jc.simple) for jc in members])
        ok = True
        for jc in members:
            final = maps.get(jc.rev, {}).get(jc.simple, jc.simple)
            if final != chosen and jc.simple != chosen:
                # A skipped clobber keeps the old synthetic name.
                if class_plan_map.get(jc.key, jc.simple) == chosen and final != chosen:
                    ok = False
                elif jc.simple != chosen and final != chosen:
                    ok = False
        if ok:
            shared += 1
    return {
        "multi_year_clusters": len(multi),
        "multi_year_clusters_sharing_one_name": shared,
    }


def apply_text(text: str, mapping: dict[str, str]) -> str:
    if not mapping:
        return text
    return base.apply_text(text, mapping)


def backup_file(rev: int, path: Path) -> None:
    root = JAVA_ROOT(rev)
    rel = path.relative_to(root)
    dest = BACKUP / str(rev) / rel
    if dest.exists():
        return
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(path, dest)


def apply_rev(rev: int, classes: list[base.JClass], mapping: dict[str, str]) -> dict:
    root = JAVA_ROOT(rev)
    by_path: dict[Path, list[base.JClass]] = defaultdict(list)
    for jc in classes:
        by_path[jc.path].append(jc)
    planned = []
    used_dest: set[Path] = set()
    renamed_files = 0
    for path, items in by_path.items():
        text = path.read_text(errors="ignore")
        new_text = apply_text(text, mapping)
        top = next((jc for jc in items if jc.top and jc.simple == path.stem), None)
        if top is None:
            top = next((jc for jc in items if jc.top), None)
        new_simple = mapping.get(top.simple, top.simple) if top else path.stem
        dest = path.with_name(new_simple + ".java") if top else path
        if dest in used_dest and dest != path:
            dest = path
            new_simple = path.stem
        if dest.exists() and dest != path and dest not in by_path:
            # Keep the identifier rewrite of the class only when the file can move.
            new_text = apply_text(text, {k: v for k, v in mapping.items() if not (top and k == top.simple)})
            dest = path
        used_dest.add(dest)
        if new_text != text or dest != path:
            backup_file(rev, path)
        if dest != path:
            renamed_files += 1
        planned.append((path, dest, new_text))
    manifest = [
        {"src": str(src.relative_to(root)), "dest": str(dest.relative_to(root))}
        for src, dest, _text in planned
        if dest != src
    ]
    man_path = BACKUP / str(rev) / "renames.json"
    man_path.parent.mkdir(parents=True, exist_ok=True)
    if not man_path.exists():
        man_path.write_text(json.dumps(manifest, indent=2))
    (BACKUP / str(rev) / "renames-last.json").write_text(json.dumps(manifest, indent=2))
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
    info = {"files_rewritten": len(temps), "file_renames": renamed_files, "idents": len(mapping)}
    log(f"applied {rev}: {info}")
    return info


def count_synthetic_files() -> dict[str, dict[str, int]]:
    out = {}
    for rev in REVS:
        root = JAVA_ROOT(rev)
        class_n = data_block = 0
        for path in root.rglob("*.java"):
            if "build" in path.parts:
                continue
            stem = path.stem
            if re.fullmatch(r"class\d+", stem):
                class_n += 1
            elif re.fullmatch(r"DataBlock\d+", stem):
                data_block += 1
        out[str(rev)] = {"classN": class_n, "DataBlock": data_block}
    return out


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--write", default="", help="Comma-separated revs to rewrite. Default: all.")
    parser.add_argument("--exclude", default="", help="Comma-separated revs left out of linking.")
    parser.add_argument("--classes-only", action="store_true")
    args = parser.parse_args()
    write_revs = [int(x) for x in args.write.split(",") if x] or list(REVS)
    exclude = {int(x) for x in args.exclude.split(",") if x}
    classes = load_all()
    if exclude:
        classes = {rev: items for rev, items in classes.items() if rev not in exclude}
    clusters, cluster_of = link_classes(classes)
    plan = class_plan(clusters)
    drop_clobbering_clusters(classes, clusters, plan)
    method_props, field_props = link_members(classes, clusters, cluster_of)
    freeze(classes, clusters, cluster_of, method_props, field_props)
    if args.classes_only:
        method_props = {}
        field_props = {}
        log("classes-only: member renames not applied")
    maps, detail = global_maps(classes, plan, method_props, field_props, cluster_of)
    shared = sharing_stats(clusters, plan, maps)
    proposed_m = sum(len(v) for v in method_props.values())
    proposed_f = sum(len(v) for v in field_props.values())
    log(f"proposed methods={proposed_m} fields={proposed_f}")
    log(
        f"multi-year={shared['multi_year_clusters']} "
        f"sharing-one-name={shared['multi_year_clusters_sharing_one_name']}"
    )
    before = count_synthetic_files()
    payload = {
        "identifier_detail": {str(k): v for k, v in detail.items()},
        "identifier_renames": {str(k): len(v) for k, v in maps.items()},
        "proposed_method_renames": proposed_m,
        "proposed_field_renames": proposed_f,
        "synthetic_files_before": before,
        **shared,
    }
    out_dir = ROOT / "work" / "share-rs3"
    out_dir.mkdir(parents=True, exist_ok=True)
    if not args.apply:
        (out_dir / "struct-stats.json").write_text(json.dumps(payload, indent=2))
        log("dry-run only")
        return
    applied = {}
    for rev in write_revs:
        if rev not in classes:
            continue
        applied[str(rev)] = apply_rev(rev, classes[rev], maps.get(rev, {}))
    payload["applied"] = applied
    payload["synthetic_files_after"] = count_synthetic_files()
    (out_dir / "struct-stats.json").write_text(json.dumps(payload, indent=2))


if __name__ == "__main__":
    main()
