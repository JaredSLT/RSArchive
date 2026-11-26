#!/usr/bin/env python3
"""Deobfuscate and Fernflower-decompile every archived Java gamepack.

Resumable. Skips folders that already have deob + source. Compile is attempted
as verification; a compile failure still keeps the decompiled tree.

Usage:
  python3 scripts/deob_all_clients.py
  python3 scripts/deob_all_clients.py --workers 3
  python3 scripts/deob_all_clients.py --filter rsc --limit 5
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import threading
import time
import zipfile
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import archive_builds as ab  # noqa: E402
import jdk  # noqa: E402

BUILDS = ROOT / "builds"
TOOLS = ROOT / "tools"
WORK = ROOT / "work" / "deob-all"
LOGS = ROOT / "logs" / "deob-all"
SUMMARY = ROOT / "work" / "deob-all-summary.json"
JAVA = jdk.java_bin()
JAVA_HOME = str(jdk.java_home())
ENV = os.environ.copy()
ENV["JAVA_HOME"] = JAVA_HOME
ENV["PATH"] = str(JAVA.parent) + os.pathsep + ENV.get("PATH", "")

print_lock = threading.Lock()


def log(msg: str) -> None:
    with print_lock:
        print(msg, flush=True)


def parse_rev(name: str):
    head = name.split("-", 1)[0]
    if head.isdigit():
        return int(head)
    if name.startswith("farewell"):
        return -1
    return None


def family_of(bdir: Path) -> str:
    return bdir.parent.name


def profile_for(family: str, rev: int | None) -> str:
    if family == "rsc":
        return "rsc"
    if family == "osrs":
        if rev is None or rev < 80:
            return "osrs-2013"
        return "osrs-2024"
    if rev is None:
        return "rs2-2005"
    if rev < 280:
        return "rs2-2004"
    if rev < 400:
        return "rs2-2005"
    if rev < 500:
        return "rs2-2007"
    if rev < 580:
        return "rs2-2009"
    if rev < 720:
        return "rs2-2011"
    return "rs2-2012"


def remap_for(family: str, rev: int | None) -> str | None:
    if rev is None:
        return None
    name = f"{family}-{rev}.txt"
    path = TOOLS / "rs-deob" / "remap" / name
    if path.exists():
        return name
    return None


def cache_home_for(family: str, rev: int | None) -> str:
    if family == "rsc":
        return ".file_store_32"
    if family == "osrs":
        return ".runelite/jagexcache/oldschool"
    if rev is not None and rev >= 400:
        return ".jagex_cache_32/runescape"
    return ".file_store_32"


def discover(filter_fam: str | None) -> list[Path]:
    out = []
    for fam in ("rsc", "rs2", "rs3", "osrs"):
        if filter_fam and fam != filter_fam:
            continue
        root = BUILDS / fam
        if not root.is_dir():
            continue
        for d in sorted(root.iterdir()):
            if not d.is_dir():
                continue
            jar = d / "original" / "gamepack.jar"
            if jar.exists() and jar.stat().st_size > 0:
                out.append(d)
    return out


def already_done(bdir: Path) -> bool:
    deob = bdir / "deob" / "gamepack-deob.jar"
    src = bdir / "src" / "src" / "main" / "java"
    if not deob.exists() or deob.stat().st_size == 0:
        return False
    javas = list(src.rglob("*.java")) if src.exists() else []
    return bool(javas)


def write_toml(dest: Path, profile: str, use_remap: bool, fallback: bool) -> None:
    if fallback or profile == "rsc":
        src = TOOLS / "deobfuscator" / ("minimal.toml" if fallback else "rsc.toml")
        text = src.read_text()
    else:
        src = TOOLS / "rs-deob" / "profiles" / f"{profile}.toml"
        text = src.read_text()
        text = text.replace("decompile = true", "decompile = false")
        text = text.replace("cleanup = true", "cleanup = false")
    if use_remap:
        text = text.replace("[profile.remap]\nenable = false", "[profile.remap]\nenable = true")
        text = text.replace("[profile.remap]\nenable = true", "[profile.remap]\nenable = true")
    else:
        text = text.replace("[profile.remap]\nenable = true", "[profile.remap]\nenable = false")
    dest.write_text(text)


def run_deob(bdir: Path, profile: str, remap: str | None, fallback: bool) -> Path:
    key = f"{family_of(bdir)}-{bdir.name}"
    work = WORK / key
    if work.exists():
        shutil.rmtree(work)
    (work / "ref").mkdir(parents=True)
    shutil.copy2(bdir / "original" / "gamepack.jar", work / "ref" / "gamepack.jar")
    write_toml(work / "deob.toml", profile, bool(remap) and not fallback, fallback)
    if remap and not fallback:
        src = TOOLS / "rs-deob" / "remap" / remap
        if src.exists():
            shutil.copy2(src, work / "remap.txt")
    logf = LOGS / f"{key}-deob.log"
    logf.parent.mkdir(parents=True, exist_ok=True)
    with logf.open("w") as fh:
        proc = subprocess.run(
            [str(JAVA), "-Xmx4G", "-jar", str(TOOLS / "deobfuscator" / "Deobfuscator.jar")],
            cwd=work,
            stdout=fh,
            stderr=subprocess.STDOUT,
            env=ENV,
        )
    if proc.returncode != 0:
        raise RuntimeError(f"deob exit {proc.returncode}, see {logf}")
    produced = work / "ref" / "gamepack-deob.jar"
    if not produced.exists():
        cands = list(work.rglob("*-deob.jar"))
        if not cands:
            raise RuntimeError(f"no deob jar, see {logf}")
        produced = cands[0]
    dest = bdir / "deob" / "gamepack-deob.jar"
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(produced, dest)
    return dest


ANNO_LINE = re.compile(
    r"[ \t]*@\.?(?:deob\.)?(?:ObfuscatedName|OriginalArg|OriginalClass|OriginalMember|Pc)\(\"[^\"]*\"\)[ \t]*\n"
)
ANNO_PAYLOAD = re.compile(r"^[ \t]*\(\"[A-Za-z0-9_/$();[]+]+\"\)[ \t]*\n", re.M)


def strip_deob_annotations(src_java: Path) -> int:
    n = 0
    for p in src_java.rglob("*.java"):
        t = p.read_text(errors="ignore")
        nt, c = ANNO_LINE.subn("", t)
        nt2, c2 = ANNO_PAYLOAD.subn("", nt)
        nt2 = nt2.replace("@.ObfuscatedName", "@ObfuscatedName")
        if nt2 != t:
            p.write_text(nt2)
            n += c + c2
    return n


def fernflower(deob_jar: Path, out_dir: Path, key: str) -> int:
    marker = out_dir / ".decompiled"
    force = os.environ.get("FORCE_DEOB") == "1"
    if marker.exists() and any(out_dir.rglob("*.java")) and not force:
        return len(list(out_dir.rglob("*.java")))
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True)
    logf = LOGS / f"{key}-fernflower.log"
    with logf.open("w") as fh:
        proc = subprocess.run(
            [
                str(JAVA),
                "-Xmx4G",
                "-jar",
                str(TOOLS / "bin" / "fernflower.jar"),
                "-dgs=1",
                "-hdc=0",
                "-hes=0",
                "-nls=1",
                "-lit=1",
                "-ren=0",
                str(deob_jar),
                str(out_dir),
            ],
            stdout=fh,
            stderr=subprocess.STDOUT,
            env=ENV,
        )
    if proc.returncode != 0:
        raise RuntimeError(f"fernflower exit {proc.returncode}, see {logf}")
    src_jars = list(out_dir.glob("*.jar"))
    if src_jars:
        with zipfile.ZipFile(src_jars[0]) as zf:
            zf.extractall(out_dir / "extracted")
    java_files = list(out_dir.rglob("*.java"))
    if not java_files:
        raise RuntimeError(f"fernflower produced no java, see {logf}")
    marker.write_text(f"{len(java_files)} files\n")
    return len(java_files)


UNINIT_RE = re.compile(r"error: variable (\w+) might not have been initialized")
MISSING_VAR_RE = re.compile(
    r"error: cannot find symbol\n(?:.*\n){0,3}\s*symbol:\s+variable (\w+)", re.M
)


def fix_uninitialized_finals(src_java: Path, log_text: str) -> int:
    names = set(UNINIT_RE.findall(log_text))
    if not names:
        return 0
    n = 0
    for p in src_java.rglob("*.java"):
        t = p.read_text(errors="ignore")
        orig = t
        for name in names:
            t = re.sub(
                rf"\bfinal\s+int\s+{name}\s*;", f"final int {name} = 0;", t
            )
            t = re.sub(
                rf"\bfinal\s+boolean\s+{name}\s*;", f"final boolean {name} = false;", t
            )
            t = re.sub(
                rf"\bfinal\s+(\w+(?:\s*\[\s*\])+)\s+{name}\s*;",
                rf"final \1 {name} = null;",
                t,
            )
        if t != orig:
            p.write_text(t)
            n += 1
    return n


def fix_undeclared_locals(src_java: Path, errors: list) -> int:
    n = 0
    by_file = {}
    for path, line_no, msg in errors:
        if "cannot find symbol" not in msg:
            continue
        by_file.setdefault(path, []).append(line_no)
    for path, lines in by_file.items():
        if not path.exists():
            continue
        text_lines = path.read_text(errors="ignore").splitlines(True)
        changed = False
        for line_no in lines:
            i = line_no - 1
            if i < 0 or i >= len(text_lines):
                continue
            m = re.match(r"^(\s*)([A-Za-z_]\w*)\s*=\s*new\s+(\w+)(\s*\[)?", text_lines[i])
            if not m:
                continue
            indent, name, typ, arr = m.groups()
            decl = f"{typ}[] {name}" if arr else f"{typ} {name}"
            if re.search(rf"\b(?:{re.escape(typ)}\s*(?:\[\s*\])?\s+)?{re.escape(name)}\s*=", text_lines[i]) and "[" in text_lines[i][: text_lines[i].find("=")]:
                continue
            text_lines[i] = re.sub(
                rf"^(\s*){name}\s*=", rf"\1{decl} =", text_lines[i], count=1
            )
            changed = True
            n += 1
        if changed:
            path.write_text("".join(text_lines))
    return n


def compile_project(proj: Path, key: str) -> bool:
    logf = LOGS / f"{key}-compile.log"
    gradlew = proj / "gradlew"
    if gradlew.exists():
        os.chmod(gradlew, 0o755)
        cmd = [str(gradlew), "compileJava", "--no-daemon"]
    else:
        cmd = ["gradle", "compileJava", "--no-daemon"]
    src_java = proj / "src" / "main" / "java"
    home = WORK / "gradle-homes" / key
    home.mkdir(parents=True, exist_ok=True)
    env = ENV.copy()
    env["GRADLE_USER_HOME"] = str(home)
    for attempt in range(6):
        with logf.open("w") as fh:
            proc = subprocess.run(cmd, cwd=proj, stdout=fh, stderr=subprocess.STDOUT, env=env)
        if proc.returncode == 0:
            return True
        log_text = logf.read_text(errors="ignore")
        errors = ab.parse_compile_errors(log_text)
        n = ab.fix_duplicate_methods(src_java)
        n += ab.apply_error_fixes(errors)
        n += ab.inject_missing_symbols(src_java, log_text)
        n += fix_uninitialized_finals(src_java, log_text)
        n += fix_undeclared_locals(src_java, errors)
        n += strip_deob_annotations(src_java)
        for p in src_java.rglob("*.java"):
            t = p.read_text(errors="ignore")
            nt = re.sub(
                r"\b(byte|int|short|long|boolean|char|float|double) (\w+) = new \1\[",
                r"\1[] \2 = new \1[",
                t,
            )
            if nt != t:
                p.write_text(nt)
                n += 1
        log(f"  compile attempt {attempt + 1} {key}: {len(errors)} errors, {n} fixes")
        if n == 0:
            break
    return False


def process_one(bdir: Path) -> dict:
    fam = family_of(bdir)
    rev = parse_rev(bdir.name)
    key = f"{fam}-{bdir.name}"
    t0 = time.time()
    status = {
        "folder": str(bdir.relative_to(ROOT)),
        "family": fam,
        "rev": rev,
        "ok": False,
        "compiled": False,
        "java_files": 0,
        "profile": None,
        "remap": None,
        "error": None,
        "seconds": 0,
    }
    if already_done(bdir) and os.environ.get("FORCE_DEOB") != "1":
        javas = list((bdir / "src" / "src" / "main" / "java").rglob("*.java"))
        status.update(ok=True, skipped=True, java_files=len(javas), seconds=0)
        st = bdir / "status.json"
        if st.exists():
            try:
                old = json.loads(st.read_text())
                status["compiled"] = bool(old.get("compiled"))
            except json.JSONDecodeError:
                pass
        return status

    profile = profile_for(fam, rev)
    remap = remap_for(fam, rev)
    status["profile"] = profile
    status["remap"] = remap
    log(f"START {key} profile={profile} remap={remap}")

    try:
        try:
            deob_jar = run_deob(bdir, profile, remap, fallback=False)
        except Exception as e:
            log(f"  deob {key} failed ({e}); retrying minimal transformers")
            deob_jar = run_deob(bdir, profile, None, fallback=True)
            status["profile"] = "minimal"

        njava = fernflower(deob_jar, bdir / "fernflower", key)
        status["java_files"] = njava

        entry = {
            "rev": rev if rev is not None else 0,
            "profile": "rsc" if profile == "rsc" else profile,
            "remap": remap,
            "main_class": "client",
            "cache_home": cache_home_for(fam, rev),
        }
        # rsc profile has no template dir; use base only
        if entry["profile"] == "rsc":
            entry["profile"] = "rs2-2005"
        proj = ab.copy_sources_into_project(entry, bdir, bdir / "fernflower")
        src_java = proj / "src" / "main" / "java"
        n = ab.fix_duplicate_methods(src_java)
        n += strip_deob_annotations(src_java)
        stub = src_java / "deob" / "ObfuscatedName.java"
        if not stub.exists():
            stub.parent.mkdir(parents=True, exist_ok=True)
            stub.write_text(
                "package deob;\n"
                "import java.lang.annotation.*;\n"
                "@Retention(RetentionPolicy.CLASS)\n"
                "@Target({ElementType.TYPE, ElementType.FIELD, ElementType.METHOD, "
                "ElementType.CONSTRUCTOR, ElementType.PARAMETER})\n"
                "public @interface ObfuscatedName { String value(); }\n"
            )
        if n:
            log(f"  cleaned {n} duplicate methods/annotations in {key}")
        try:
            ab.patch_cache_dir(src_java, entry["cache_home"])
        except Exception:
            pass
        compiled = compile_project(proj, key)
        status["compiled"] = compiled
        status["ok"] = True
        status["java_files"] = len(list(src_java.rglob("*.java")))
    except Exception as e:
        status["error"] = str(e)
        log(f"FAIL {key}: {e}")
    status["seconds"] = round(time.time() - t0, 1)
    (bdir / "status.json").write_text(json.dumps(status, indent=2) + "\n")
    log(
        f"{'OK' if status['ok'] else 'FAIL'} {key} java={status['java_files']} "
        f"compiled={status['compiled']} {status['seconds']}s"
    )
    return status


def save_summary(results: list[dict]) -> None:
    SUMMARY.parent.mkdir(parents=True, exist_ok=True)
    ok = sum(1 for r in results if r.get("ok"))
    compiled = sum(1 for r in results if r.get("compiled"))
    failed = [r for r in results if not r.get("ok")]
    SUMMARY.write_text(
        json.dumps(
            {
                "total": len(results),
                "ok": ok,
                "compiled": compiled,
                "failed": len(failed),
                "failures": failed,
                "results": results,
            },
            indent=2,
        )
        + "\n"
    )


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--workers", type=int, default=3)
    ap.add_argument("--filter", choices=["rsc", "rs2", "rs3", "osrs"])
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--force", action="store_true")
    args = ap.parse_args()
    if args.force:
        # Re-deob may refresh jars. It must not set FORCE_SOURCES; renamed
        # Java is kept by archive_builds.copy_sources_into_project.
        os.environ["FORCE_DEOB"] = "1"

    LOGS.mkdir(parents=True, exist_ok=True)
    WORK.mkdir(parents=True, exist_ok=True)
    clients = discover(args.filter)
    if not args.force:
        pending = [c for c in clients if not already_done(c)]
    else:
        pending = clients
    if args.limit:
        pending = pending[: args.limit]
    log(f"{len(clients)} clients on disk, {len(pending)} to process, workers={args.workers}")
    if not pending:
        save_summary([])
        return 0

    results = []
    with ThreadPoolExecutor(max_workers=max(1, args.workers)) as pool:
        futs = {pool.submit(process_one, c): c for c in pending}
        for fut in as_completed(futs):
            try:
                results.append(fut.result())
            except Exception as e:
                bdir = futs[fut]
                results.append(
                    {
                        "folder": str(bdir.relative_to(ROOT)),
                        "ok": False,
                        "error": str(e),
                        "compiled": False,
                    }
                )
            if len(results) % 5 == 0 or len(results) == len(pending):
                save_summary(results)
                log(f"progress {len(results)}/{len(pending)}")

    save_summary(results)
    ok = sum(1 for r in results if r.get("ok"))
    compiled = sum(1 for r in results if r.get("compiled"))
    log(f"done ok={ok}/{len(results)} compiled={compiled}/{len(results)}")
    return 0 if ok == len(results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
