#!/usr/bin/env python3
"""Install unified remaps and, only when asked, rebuild deobfuscated sources.

A bare run does not delete sources or set FORCE_DEOB / FORCE_SOURCES.
Pass --rebuild to deobfuscate again. Pass --rebuild --force-sources to
replace existing Java trees. --install-generated may still install remap files.
"""
from __future__ import annotations

import os
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from generate_unified_remaps import main as generate  # noqa: E402

REMAP_DIR = ROOT / "tools" / "rs-deob" / "remap"
OUT_DIR = ROOT / "work" / "unified-remaps"
DEOB_JAR = ROOT / "tools" / "deobfuscator" / "Deobfuscator.jar"
FERN = ROOT / "tools" / "bin" / "fernflower.jar"
_FLAGS = {"--install-generated", "--rebuild", "--force-sources"}


def install_remaps() -> None:
    generate()
    REMAP_DIR.mkdir(parents=True, exist_ok=True)
    for src in OUT_DIR.glob("*.txt"):
        dest = REMAP_DIR / src.name
        shutil.copy2(src, dest)
        print(f"installed {dest}")


def find_build(family: str, rev: int) -> Path | None:
    root = ROOT / "builds" / family
    if not root.is_dir():
        return None
    matches = sorted(path for path in root.glob(f"{rev}-*") if path.is_dir())
    return matches[0] if matches else None


def clear_rs2_deob() -> None:
    root = ROOT / "builds" / "rs2"
    if not root.is_dir():
        return
    for bdir in sorted(root.iterdir()):
        if not bdir.is_dir():
            continue
        if bdir.name.startswith("742-"):
            print(f"skip clear {bdir.name}: Lost City checkout")
            continue
        for p in [
            bdir / "deob" / "gamepack-deob.jar",
            bdir / "fernflower",
            bdir / "src" / "src" / "main" / "java",
        ]:
            if p.is_file():
                p.unlink()
            elif p.is_dir():
                shutil.rmtree(p)
        marker = bdir / "fernflower" / ".decompiled"
        if marker.exists():
            marker.unlink()
    for work in (ROOT / "work").glob("deob-*"):
        shutil.rmtree(work, ignore_errors=True)


def apply_rsc(rev: int, year: int, *, replace_sources: bool) -> None:
    bdir = find_build("rsc", rev)
    if bdir is None:
        print(f"skip rsc {rev}: no builds/rsc/{rev}-* directory")
        return
    orig = bdir / "original" / "gamepack.jar"
    remap = REMAP_DIR / f"rsc-{rev}.txt"
    if not orig.exists() or not remap.exists():
        print(f"skip rsc {rev}: missing jar or remap")
        return
    dest = bdir / "src" / "src" / "main" / "java"
    if dest.is_dir() and any(dest.rglob("*.java")) and not replace_sources:
        print(f"skip rsc {rev}: renamed sources were kept")
        return
    work = ROOT / "work" / f"rsc-deob-{rev}"
    if work.exists():
        shutil.rmtree(work)
    (work / "ref").mkdir(parents=True)
    shutil.copy2(orig, work / "ref" / "gamepack.jar")
    shutil.copy2(remap, work / "remap.txt")
    toml = (ROOT / "tools" / "deobfuscator" / "rsc.toml").read_text()
    toml = toml.replace("enable = false", "enable = true", 1)  # remap
    (work / "deob.toml").write_text(toml)
    log = ROOT / "logs" / f"deob-rsc-{rev}.log"
    log.parent.mkdir(exist_ok=True)
    proc = subprocess.run(
        ["java", "-Xmx2G", "-jar", str(DEOB_JAR)],
        cwd=work,
        stdout=log.open("w"),
        stderr=subprocess.STDOUT,
    )
    if proc.returncode != 0:
        raise RuntimeError(f"rsc {rev} deob failed, see {log}")
    produced = work / "ref" / "gamepack-deob.jar"
    if not produced.exists():
        cands = list(work.rglob("*-deob.jar"))
        if not cands:
            raise RuntimeError(f"rsc {rev} no deob jar")
        produced = cands[0]
    deob_dir = bdir / "deob"
    deob_dir.mkdir(exist_ok=True)
    shutil.copy2(produced, deob_dir / "gamepack-deob.jar")
    fern_out = bdir / "fernflower"
    if fern_out.exists():
        shutil.rmtree(fern_out)
    fern_out.mkdir()
    subprocess.run(
        [
            "java",
            "-Xmx2G",
            "-jar",
            str(FERN),
            "-dgs=1",
            "-hdc=0",
            "-nls=1",
            str(deob_dir / "gamepack-deob.jar"),
            str(fern_out),
        ],
        check=False,
        stdout=(ROOT / "logs" / f"fernflower-{rev}.log").open("w"),
        stderr=subprocess.STDOUT,
    )
    jars = list(fern_out.glob("*.jar"))
    extracted = fern_out / "extracted"
    if jars:
        extracted.mkdir(exist_ok=True)
        with zipfile.ZipFile(jars[0]) as zf:
            zf.extractall(extracted)
        java_root = extracted
    else:
        java_root = fern_out
    if dest.exists():
        shutil.rmtree(dest)
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(java_root, dest)
    print(f"rsc {year} rev {rev}: {len(list(dest.rglob('*.java')))} java files")


def _wants_rsc(only: list[str]) -> bool:
    if not only:
        return True
    return any(x.startswith("rsc") or x in {"115", "159", "177"} for x in only)


def _only_rsc(only: list[str]) -> bool:
    return bool(only) and all(x.startswith("rsc") or x in {"115", "159", "177"} for x in only)


def main() -> int:
    # Remaps are installed as-is. Do not regenerate here: generate_unified_remaps.py
    # can overwrite a curated remap (e.g. 377 Packet members) with guessed members.
    raw = sys.argv[1:]
    install = "--install-generated" in raw
    rebuild = "--rebuild" in raw
    force_sources = "--force-sources" in raw
    only = [arg for arg in raw if arg not in _FLAGS]
    if install:
        install_remaps()
    if not rebuild:
        print("not rebuilding; pass --rebuild to deobfuscate, and --force-sources to replace sources")
        return 0
    replace_sources = force_sources
    if _wants_rsc(only):
        apply_rsc(177, 2003, replace_sources=replace_sources)
        apply_rsc(159, 2002, replace_sources=replace_sources)
        apply_rsc(115, 2001, replace_sources=replace_sources)
    if _only_rsc(only):
        return 0
    env = os.environ.copy()
    env.pop("FORCE_DEOB", None)
    env.pop("FORCE_SOURCES", None)
    if force_sources:
        clear_rs2_deob()
        env["FORCE_DEOB"] = "1"
        env["FORCE_SOURCES"] = "1"
    cmd = [sys.executable, str(ROOT / "scripts" / "archive_builds.py")]
    if only:
        cmd.extend(only)
    print("+", " ".join(cmd))
    return subprocess.call(cmd, env=env, cwd=ROOT)


if __name__ == "__main__":
    sys.exit(main())
