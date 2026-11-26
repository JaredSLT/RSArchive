#!/usr/bin/env python3
"""Share RS2 names by structural fingerprint.

RS2 only. Links a class across revisions when its method-descriptor multiset
and field-type multiset are unique in each year. Readable class names that
differ are never united. No new prefix is minted. String and char literals
are not edited.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import jdk  # noqa: E402
import share_rs2_names as base

ROOT = base.ROOT
WORK = base.WORK
BACKUP = WORK / "backup-struct"
JAVA_HOME = jdk.java_home()
REVS = base.REVS
ANCHORS = (317, 254, 377)
STRING_MIN = 8
SIZE_MIN = 8
WIDE_FIELD_TYPES = {"int", "long", "boolean", "byte", "short", "String"}
HASH_CLASS_RE = base.HASH_CLASS_RE
OP_METHOD_RE = re.compile(r"^op\w*\d\w*$")


def log(msg: str) -> None:
    print(msg, flush=True)


def type_name(jtype: str) -> str:
    """Simple type name, package stripped. Arrays keep their brackets."""
    dims = jtype.count("[")
    base_name = re.sub(r"<.*$", "", jtype)
    base_name = base_name.replace("[", "").replace("]", "").strip()
    base_name = base_name.split(".")[-1]
    return base_name + ("[]" * dims)


def method_desc(method: base.JMethod) -> str:
    params = ",".join(type_name(part) for part in method.params)
    return f"({params}){type_name(method.ret)}"


def fingerprint(jc: base.JClass) -> tuple:
    methods = tuple(sorted(Counter(method_desc(method) for method in jc.methods).items()))
    fields = tuple(sorted(Counter(type_name(fld.jtype) for fld in jc.fields).items()))
    return (methods, fields)


def name_rank(rev: int, name: str) -> tuple:
    if rev == 317:
        prio = 400
    elif rev == 254:
        prio = 300
    elif rev == 377:
        prio = 200
    elif rev == 530:
        prio = 100
    else:
        prio = 10
    return (-prio, name)


def choose_name(cands: list[tuple[int, str]]) -> str:
    """cands are (rev, simple name). Anchors win, then 530, then any readable."""
    readable = [(rev, name) for rev, name in cands if base.is_readable(name)]
    if readable:
        return sorted(readable, key=lambda item: name_rank(item[0], item[1]))[0][1]
    freq = Counter(name for _rev, name in cands)
    return sorted(freq, key=lambda name: (-freq[name], name))[0]


def shares_string(left: base.JClass, right: base.JClass) -> bool:
    small, large = (left, right) if len(left.strings) <= len(right.strings) else (right, left)
    for text in small.strings:
        if len(text) >= STRING_MIN and text in large.strings:
            return True
    return False


def size_ok(left: base.JClass, right: base.JClass) -> bool:
    if len(left.methods) >= SIZE_MIN or len(left.fields) >= SIZE_MIN:
        return True
    return shares_string(left, right)


def find_gradle_dist() -> Path | None:
    found = sorted(Path("/tmp").glob("gradle-rs2-*/wrapper/dists/gradle-9.3.0-bin"))
    found += sorted(Path("/tmp").glob("gradle-rs3-*/wrapper/dists/gradle-9.3.0-bin"))
    for path in found:
        if path.is_dir() and any(path.iterdir()):
            return path
    return None


def seed_gradle_home(rev: int, dist: Path | None) -> None:
    home = Path(f"/tmp/gradle-rs2-struct-{rev}")
    dest = home / "wrapper" / "dists" / "gradle-9.3.0-bin"
    if dest.exists() or dist is None:
        home.mkdir(parents=True, exist_ok=True)
        return
    dest.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(["cp", "-a", str(dist), str(dest)], check=False)


def gradle_cmd(folder: Path) -> list[str]:
    if (folder / "src" / "build.gradle").exists():
        return ["./gradlew", "--no-daemon", "--offline", "compileJava"]
    return ["./gradlew", "--no-daemon", "--offline", ":client:compileJava"]


class ClusterState:
    def __init__(self) -> None:
        self.uf = base.UnionFind()
        self.years: dict[tuple, set[int]] = {}
        self.readable: dict[tuple, set[str]] = {}

    def add(self, jc: base.JClass) -> None:
        key = jc.key
        self.uf.add(key)
        self.years[key] = {jc.rev}
        self.readable[key] = {jc.simple} if base.is_readable(jc.simple) else set()

    def try_union(self, left: base.JClass, right: base.JClass) -> bool:
        ra, rb = self.uf.find(left.key), self.uf.find(right.key)
        if ra == rb:
            return False
        if self.years[ra] & self.years[rb]:
            return False
        names = self.readable[ra] | self.readable[rb]
        if len(names) > 1:
            return False
        self.uf.union(left.key, right.key)
        root = self.uf.find(ra)
        other = rb if root == ra else ra
        self.years[root] = self.years[ra] | self.years[rb]
        self.readable[root] = names
        self.years.pop(other, None)
        self.readable.pop(other, None)
        return True


def link_classes(classes: dict[int, list[base.JClass]]) -> dict[tuple, list[base.JClass]]:
    state = ClusterState()
    fp_of: dict[tuple, tuple] = {}
    by_year: dict[int, dict[tuple, list[base.JClass]]] = {}
    for rev, items in classes.items():
        bucket: dict[tuple, list[base.JClass]] = defaultdict(list)
        for jc in items:
            state.add(jc)
            fp = fingerprint(jc)
            fp_of[jc.key] = fp
            bucket[fp].append(jc)
        by_year[rev] = bucket
    unique: dict[int, dict[tuple, base.JClass]] = {}
    for rev, bucket in by_year.items():
        unique[rev] = {fp: lst[0] for fp, lst in bucket.items() if len(lst) == 1}
    log(
        "unique fingerprints "
        + " ".join(f"{rev}:{len(unique[rev])}" for rev in REVS)
    )

    linked = 0
    skipped_size = 0
    skipped_readable = 0

    def consider(left: base.JClass, right: base.JClass) -> None:
        nonlocal linked, skipped_size, skipped_readable
        if left.rev == right.rev:
            return
        if not size_ok(left, right):
            skipped_size += 1
            return
        if state.try_union(left, right):
            linked += 1
        else:
            ra, rb = state.uf.find(left.key), state.uf.find(right.key)
            if ra != rb and len(state.readable[ra] | state.readable[rb]) > 1:
                skipped_readable += 1

    for rev, nxt in zip(REVS, REVS[1:]):
        for fp, jc in unique[rev].items():
            other = unique[nxt].get(fp)
            if other is not None:
                consider(jc, other)
    adjacent_links = linked
    for rev in REVS:
        for anchor in ANCHORS:
            if rev == anchor:
                continue
            for fp, jc in unique[rev].items():
                other = unique[anchor].get(fp)
                if other is not None:
                    consider(jc, other)
    log(
        f"structural links={linked} adjacent={adjacent_links} "
        f"anchor_added={linked - adjacent_links} "
        f"skipped_size={skipped_size} skipped_readable={skipped_readable}"
    )
    groups: dict[tuple, list[base.JClass]] = defaultdict(list)
    for items in classes.values():
        for jc in items:
            groups[state.uf.find(jc.key)].append(jc)
    return groups


def assign_classes(groups: dict[tuple, list[base.JClass]], classes: dict[int, list[base.JClass]]) -> int:
    """Rename obfuscated classes onto the cluster's chosen name. Skip a cluster that would clobber a file."""
    by_package: dict[tuple[int, str, str], base.JClass] = {}
    for items in classes.values():
        for jc in items:
            by_package[(jc.rev, jc.package, jc.simple)] = jc
    renamed = 0
    skipped = 0
    multi = 0
    for members in groups.values():
        years = {jc.rev for jc in members}
        if len(years) < 2:
            continue
        multi += 1
        chosen = choose_name([(jc.rev, jc.simple) for jc in members])
        clash = False
        for jc in members:
            if jc.simple == chosen or base.is_readable(jc.simple):
                continue
            if jc.path.stem == jc.simple:
                dest = jc.path.with_name(chosen + ".java")
                if dest.exists():
                    clash = True
                    break
            other = by_package.get((jc.rev, jc.package, chosen))
            if other is not None and other.key != jc.key:
                clash = True
                break
        if clash:
            skipped += 1
            continue
        for jc in members:
            if jc.simple == chosen or base.is_readable(jc.simple):
                jc.proposed = jc.simple
                continue
            jc.proposed = chosen
            renamed += 1
    log(f"class multi-year clusters={multi} proposed renames={renamed} clobber_skipped={skipped}")
    return renamed


def assign_methods(groups: dict[tuple, list[base.JClass]]) -> None:
    multi = renamed = 0
    for members in groups.values():
        if len({jc.rev for jc in members}) < 2:
            continue
        counts: dict[tuple, Counter] = {}
        owners: dict[str, list[tuple[base.JClass, base.JMethod]]] = defaultdict(list)
        for jc in members:
            seen: Counter = Counter(method_desc(method) for method in jc.methods)
            counts[jc.key] = seen
            for method in jc.methods:
                owners[method_desc(method)].append((jc, method))
        for desc, group in owners.items():
            if any(counts[jc.key][desc] != 1 for jc, _method in group):
                continue
            years = {jc.rev for jc, _method in group}
            if len(years) < 2 or len(years) != len(group):
                continue
            chosen = choose_name([(jc.rev, method.name) for jc, method in group])
            if any(base.jdk_method(jc, method) and method.name != chosen for jc, method in group):
                continue
            multi += 1
            for jc, method in group:
                method.cluster = desc
                if method.name == chosen:
                    continue
                if base.jdk_method(jc, method):
                    continue
                method.proposed = chosen
                renamed += 1
    log(f"method multi-year clusters={multi} proposed renames={renamed}")


def enforce_local_clashes(classes: dict[int, list[base.JClass]]) -> int:
    """Drop a rename that would duplicate a member inside one class."""
    reverted = 0
    for items in classes.values():
        for jc in items:
            by_field: dict[str, list[base.JField]] = defaultdict(list)
            for fld in jc.fields:
                by_field[fld.proposed].append(fld)
            for group in by_field.values():
                if len({fld.name for fld in group}) < 2:
                    continue
                for fld in group:
                    if fld.proposed != fld.name:
                        fld.proposed = fld.name
                        reverted += 1
            field_names = {fld.proposed for fld in jc.fields}
            class_names = {jc.simple, jc.proposed}
            for method in jc.methods:
                if method.proposed == method.name:
                    continue
                if method.proposed in field_names or method.proposed in class_names:
                    method.proposed = method.name
                    reverted += 1
            method_names = {method.proposed for method in jc.methods}
            for fld in jc.fields:
                if fld.proposed == fld.name:
                    continue
                if fld.proposed in method_names or fld.proposed in class_names:
                    fld.proposed = fld.name
                    reverted += 1
    log(f"local clashes reverted members={reverted}")
    return reverted


def assign_fields(groups: dict[tuple, list[base.JClass]]) -> None:
    multi = renamed = 0
    for members in groups.values():
        if len({jc.rev for jc in members}) < 2:
            continue
        counts: dict[tuple, Counter] = {}
        owners: dict[str, list[tuple[base.JClass, base.JField]]] = defaultdict(list)
        for jc in members:
            seen: Counter = Counter(type_name(fld.jtype) for fld in jc.fields)
            counts[jc.key] = seen
            for fld in jc.fields:
                owners[type_name(fld.jtype)].append((jc, fld))
        for jtype, group in owners.items():
            if any(counts[jc.key][jtype] != 1 for jc, _fld in group):
                continue
            if jtype in WIDE_FIELD_TYPES and any(counts[jc.key][jtype] > 1 for jc, _fld in group):
                continue
            years = {jc.rev for jc, _fld in group}
            if len(years) < 2 or len(years) != len(group):
                continue
            chosen = choose_name([(jc.rev, fld.name) for jc, fld in group])
            multi += 1
            for _jc, fld in group:
                fld.cluster = jtype
                if fld.name == chosen:
                    continue
                fld.proposed = chosen
                renamed += 1
    log(f"field multi-year clusters={multi} proposed renames={renamed}")


def backup_file(rev: int, src: Path, path: Path) -> None:
    dest = BACKUP / str(rev) / path.relative_to(src)
    if dest.exists():
        return
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(path, dest)


def apply_maps(classes: dict[int, list[base.JClass]], maps: dict[int, dict[str, str]]) -> dict[str, list[dict]]:
    records: dict[str, list[dict]] = {}
    for rev, items in classes.items():
        mapping = dict(maps.get(rev, {}))
        if not mapping:
            records[str(rev)] = []
            continue
        src = base.source_root(base.year_dir(rev))
        by_path: dict[Path, list[base.JClass]] = defaultdict(list)
        for jc in items:
            by_path[jc.path].append(jc)
        blocked: set[str] = set()
        for path in by_path:
            new_simple = mapping.get(path.stem)
            if not new_simple or new_simple == path.stem:
                continue
            dest = path.with_name(new_simple + ".java")
            if dest.exists():
                blocked.add(path.stem)
                log(f"skip file clobber {rev} {path.name} -> {dest.name}")
        if blocked:
            mapping = {old: new for old, new in mapping.items() if old not in blocked}
        rev_records: list[dict] = []
        renames: list[tuple[Path, Path]] = []
        for path in by_path:
            original = path.read_text(errors="ignore")
            updated = base.apply_idents(original, mapping)
            if updated == original:
                continue
            backup_file(rev, src, path)
            dest = path
            if path.stem in mapping and mapping[path.stem] != path.stem:
                candidate = path.with_name(mapping[path.stem] + ".java")
                if not candidate.exists():
                    dest = candidate
            path.write_text(updated)
            if dest != path:
                renames.append((path, dest))
            rev_records.append({
                "old": str(path.relative_to(src)),
                "new": str(dest.relative_to(src)),
            })
        temps: list[tuple[Path, Path]] = []
        for src_path, dest in renames:
            tmp = src_path.with_name(f".__share_struct_{src_path.stem}__.java")
            n = 0
            while tmp.exists():
                n += 1
                tmp = src_path.with_name(f".__share_struct_{src_path.stem}_{n}__.java")
            src_path.rename(tmp)
            temps.append((tmp, dest))
        for tmp, dest in temps:
            if dest.exists():
                tmp.rename(dest.with_name(tmp.name + ".clash.java"))
                log(f"clash leaving {tmp.name} beside {dest.name}")
                continue
            tmp.rename(dest)
        records[str(rev)] = rev_records
        log(f"applied {rev} files={len(rev_records)} idents={len(mapping)}")
    return records


def restore_rev(rev: int, records: list[dict]) -> None:
    src = base.source_root(base.year_dir(rev))
    bdir = BACKUP / str(rev)
    for rec in records:
        old = src / rec["old"]
        new = src / rec["new"]
        backed = bdir / rec["old"]
        if rec["new"] != rec["old"] and new.exists() and new != old:
            new.unlink()
        if not backed.exists():
            log(f"missing backup for {rev} {rec['old']}")
            continue
        old.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(backed, old)


def compile_one(rev: int, dist: Path | None, log_name: str) -> tuple[int, str]:
    folder = base.year_dir(rev)
    src = folder / "src"
    seed_gradle_home(rev, dist)
    env = os.environ.copy()
    env["JAVA_HOME"] = str(JAVA_HOME)
    env["PATH"] = str(JAVA_HOME / "bin") + os.pathsep + env.get("PATH", "")
    env["GRADLE_USER_HOME"] = f"/tmp/gradle-rs2-struct-{rev}"
    log_path = WORK / log_name
    with log_path.open("w") as handle:
        proc = subprocess.run(
            gradle_cmd(folder),
            cwd=src,
            env=env,
            stdout=handle,
            stderr=subprocess.STDOUT,
            check=False,
        )
    text = log_path.read_text(errors="ignore")
    tail = "\n".join(text.splitlines()[-25:])
    return proc.returncode, tail


def clean_compile_dirs(rev: int) -> None:
    folder = base.year_dir(rev) / "src"
    for rel in (
        Path("build/classes"),
        Path("build/tmp/compileJava"),
        Path("client/build/classes"),
        Path("client/build/tmp/compileJava"),
    ):
        target = folder / rel
        if target.exists():
            shutil.rmtree(target, ignore_errors=True)


def member_share(classes, groups, maps) -> dict:
    """Sharing after the year map, keyed by the class cluster object."""
    root_of: dict[tuple, tuple] = {}
    for root, members in groups.items():
        for jc in members:
            root_of[jc.key] = root
    class_multi = class_shared = 0
    for members in groups.values():
        if len({jc.rev for jc in members}) < 2:
            continue
        class_multi += 1
        names = {maps.get(jc.rev, {}).get(jc.simple, jc.simple) for jc in members}
        if len(names) == 1:
            class_shared += 1
    method_groups: dict[tuple, list[tuple[base.JClass, base.JMethod]]] = defaultdict(list)
    field_groups: dict[tuple, list[tuple[base.JClass, base.JField]]] = defaultdict(list)
    class_renames = method_renames = field_renames = 0
    for items in classes.values():
        for jc in items:
            if maps.get(jc.rev, {}).get(jc.simple, jc.simple) != jc.simple:
                class_renames += 1
            root = root_of[jc.key]
            for method in jc.methods:
                if maps.get(jc.rev, {}).get(method.name, method.name) != method.name:
                    method_renames += 1
                cluster = getattr(method, "cluster", None)
                if cluster is not None:
                    method_groups[(root, cluster)].append((jc, method))
            for fld in jc.fields:
                if maps.get(jc.rev, {}).get(fld.name, fld.name) != fld.name:
                    field_renames += 1
                cluster = getattr(fld, "cluster", None)
                if cluster is not None:
                    field_groups[(root, cluster)].append((jc, fld))
    method_multi = method_shared = 0
    for group in method_groups.values():
        years = {jc.rev for jc, _method in group}
        if len(years) < 2:
            continue
        method_multi += 1
        names = {maps.get(jc.rev, {}).get(method.name, method.name) for jc, method in group}
        if len(names) == 1:
            method_shared += 1
    field_multi = field_shared = 0
    for group in field_groups.values():
        years = {jc.rev for jc, _fld in group}
        if len(years) < 2:
            continue
        field_multi += 1
        names = {maps.get(jc.rev, {}).get(fld.name, fld.name) for jc, fld in group}
        if len(names) == 1:
            field_shared += 1
    return {
        "class_renames": class_renames,
        "method_renames": method_renames,
        "field_renames": field_renames,
        "class_multi_clusters": class_multi,
        "class_multi_shared": class_shared,
        "method_multi_clusters": method_multi,
        "method_multi_shared": method_shared,
        "field_multi_clusters": field_multi,
        "field_multi_shared": field_shared,
    }


def count_remaining() -> dict[str, int]:
    classes = base.load_all()
    chex = op_methods = 0
    for items in classes.values():
        for jc in items:
            if HASH_CLASS_RE.fullmatch(jc.simple):
                chex += 1
            for method in jc.methods:
                if OP_METHOD_RE.fullmatch(method.name):
                    op_methods += 1
    return {"c_hex_classes": chex, "op_methods": op_methods}


def plan(classes: dict[int, list[base.JClass]]):
    groups = link_classes(classes)
    assign_classes(groups, classes)
    assign_methods(groups)
    assign_fields(groups)
    base.enforce_overloads(classes)
    base.enforce_overrides(classes)
    base.enforce_overloads(classes)
    base.enforce_overrides(classes)
    enforce_local_clashes(classes)
    maps = base.build_maps(classes)
    counts = member_share(classes, groups, maps)
    return groups, maps, counts


def append_report(counts: dict, compiled: dict[str, int], reverted: dict[str, str], remaining: dict[str, int]) -> None:
    path = WORK / "report.md"
    lines = [
        "",
        "## Structural pass",
        "",
        "Classes linked by a unique method-descriptor and field-type fingerprint.",
        "Adjacent revisions are linked first, then a direct link to 317, 254, or 377.",
        "No new prefix was minted. String and char literals were not edited.",
        "",
        "### Renames applied",
        "",
        f"- class renames: {counts.get('class_renames', 0)}",
        f"- method renames: {counts.get('method_renames', 0)}",
        f"- field renames: {counts.get('field_renames', 0)}",
        "",
        "### Multi-year clusters that share one name",
        "",
        f"- class clusters spanning 2+ years: {counts.get('class_multi_clusters', 0)}",
        f"- of those, one shared simple name: {counts.get('class_multi_shared', 0)}",
        f"- method clusters spanning 2+ years: {counts.get('method_multi_clusters', 0)}",
        f"- of those, one shared simple name: {counts.get('method_multi_shared', 0)}",
        f"- field clusters spanning 2+ years: {counts.get('field_multi_clusters', 0)}",
        f"- of those, one shared simple name: {counts.get('field_multi_shared', 0)}",
        "",
        "### Names still synthetic",
        "",
        f"- C-hex classes remaining: {remaining.get('c_hex_classes', 0)}",
        f"- op* methods remaining: {remaining.get('op_methods', 0)}",
        "",
        "### Compile exit codes",
        "",
    ]
    for rev in REVS:
        code = compiled.get(str(rev))
        if code is None:
            lines.append(f"- {rev}: not compiled")
        elif str(rev) in reverted:
            lines.append(f"- {rev}: restored tree exit {code}")
        else:
            lines.append(f"- {rev}: exit {code}")
    lines.extend(["", "### Reverted revisions", ""])
    if not reverted:
        lines.append("None.")
    else:
        for rev, reason in reverted.items():
            lines.append(f"- {rev}: {reason}")
    lines.append("")
    with path.open("a") as handle:
        handle.write("\n".join(lines))
    log(f"appended {path}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--compile", action="store_true")
    args = parser.parse_args()
    WORK.mkdir(parents=True, exist_ok=True)
    classes = base.load_all()
    groups, maps, counts = plan(classes)
    log(
        "dry-run class={class_renames} method={method_renames} field={field_renames} "
        "class_clusters={class_multi_clusters} shared={class_multi_shared} "
        "method_clusters={method_multi_clusters} method_shared={method_multi_shared} "
        "field_clusters={field_multi_clusters} field_shared={field_multi_shared}".format(**counts)
    )
    shown = 0
    for rev, mapping in sorted(maps.items()):
        if not mapping:
            continue
        sample = list(mapping.items())[:6]
        log(f"map {rev} n={len(mapping)} sample={sample}")
        shown += 1
    (WORK / "struct-plan.json").write_text(json.dumps({
        "counts": counts,
        "maps": {str(rev): mapping for rev, mapping in maps.items()},
    }))
    if not args.apply and not args.compile:
        return
    records: dict[str, list] = {}
    if args.apply:
        records = apply_maps(classes, maps)
        (WORK / "struct-applied.json").write_text(json.dumps({"counts": counts, "records": records}))
    elif (WORK / "struct-applied.json").exists():
        records = json.loads((WORK / "struct-applied.json").read_text()).get("records", {})
    if not args.compile:
        return
    dist = find_gradle_dist()
    if dist is None:
        log("no local gradle 9.3.0 distribution; not downloading")
    else:
        log(f"gradle dist {dist}")
    compiled: dict[str, int] = {}
    reverted: dict[str, str] = {}
    pending = list(REVS)
    running: list[tuple[int, subprocess.Popen, object]] = []

    def start(rev: int) -> None:
        folder = base.year_dir(rev)
        seed_gradle_home(rev, dist)
        env = os.environ.copy()
        env["JAVA_HOME"] = str(JAVA_HOME)
        env["PATH"] = str(JAVA_HOME / "bin") + os.pathsep + env.get("PATH", "")
        env["GRADLE_USER_HOME"] = f"/tmp/gradle-rs2-struct-{rev}"
        log_path = WORK / f"compile-struct-{rev}.log"
        handle = log_path.open("w")
        proc = subprocess.Popen(
            gradle_cmd(folder),
            cwd=folder / "src",
            env=env,
            stdout=handle,
            stderr=subprocess.STDOUT,
        )
        running.append((rev, proc, handle))
        log(f"compile start {rev}")

    while pending or running:
        while pending and len(running) < 3:
            start(pending.pop(0))
        if not running:
            break
        rev, proc, handle = running.pop(0)
        code = proc.wait()
        handle.close()
        compiled[str(rev)] = code
        log(f"compile {rev} exit={code}")
        if code != 0:
            tail = "\n".join((WORK / f"compile-struct-{rev}.log").read_text(errors="ignore").splitlines()[-30:])
            reason = f"gradlew exit {code}: " + " ".join(tail.split())[-700:]
            restore_rev(rev, records.get(str(rev), []))
            reverted[str(rev)] = reason
            clean_compile_dirs(rev)
            again, _tail = compile_one(rev, dist, f"compile-struct-{rev}-restored.log")
            compiled[str(rev)] = again
            log(f"restore compile {rev} exit={again}")
            if again != 0:
                reverted[str(rev)] = reason + f" | restore compile exit {again}"
            base.update_status(rev, again == 0)
        else:
            base.update_status(rev, True)
    remaining = count_remaining()
    log(f"remaining {remaining}")
    if reverted:
        restored_revs = {int(rev) for rev in reverted}
        kept = {rev: ({} if rev in restored_revs else mapping) for rev, mapping in maps.items()}
        counts = member_share(classes, groups, kept)
        log(
            "after restore class={class_renames} method={method_renames} field={field_renames} "
            "class_shared={class_multi_shared}/{class_multi_clusters}".format(**counts)
        )
    (WORK / "struct-result.json").write_text(json.dumps({
        "counts": counts,
        "compiled": compiled,
        "reverted": reverted,
        "remaining": remaining,
    }, indent=2))
    append_report(counts, compiled, reverted, remaining)


if __name__ == "__main__":
    main()
