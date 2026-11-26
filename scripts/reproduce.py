#!/usr/bin/env python3
"""Reproduce archived clients from a checkout.

    python3 scripts/reproduce.py

Stages, in order:
  1. Resolve a JDK 21+ home (scripts/jdk.py). Fail if none qualifies.
  2. Ensure work/clients.html, downloading the OpenRS2 catalog if it is missing.
  3. Run scripts/archive_builds.py for the milestone catalog. Revision 742 has
     no gamepack and must not fail the process. FORCE_SOURCES is not set, so
     existing renamed sources are kept.
  4. Run scripts/fetch_year_end_clients.py. An existing non-empty gamepack is
     left alone unless --force-download sets FORCE_DOWNLOAD=1.
  5. Run scripts/deob_all_clients.py without --force. Finished source trees are
     skipped. --force on that script sets FORCE_DEOB only, never FORCE_SOURCES.
  6. Run scripts/repair_rs3_831_860.py. The repair is idempotent (empty
     twitchtv trees, foreach/catch/fourcc/byte edits no-op once applied).
  7. Run scripts/revert_false_shares.py --apply when that file exists.
  8. Compile with ./gradlew compileJava under JAVA_HOME from jdk.py, only for
     clients whose status.json is missing or whose compiled flag is not true,
     unless --compile-all.
  9. Rewrite SUMMARY.json from every builds/*/*/status.json. Paths are
     repo-relative.

This command does not call generify_*.py, unify_method_names.py, share_rs*.py,
or download_java_clients.py. download_java_clients.py bulk-downloads every
OpenRS2 client and stays out of the default path.

Flags:
  --dry-run         print each stage and write nothing
  --only REV        limit the milestone catalog, year-end fetch, and compile
  --force-sources   set FORCE_SOURCES=1 for archive_builds.py only
  --force-download  set FORCE_DOWNLOAD=1 for fetch_year_end_clients.py
  --compile-all     compile every client that has a Gradle wrapper
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import jdk  # noqa: E402

OPENRS2_CLIENTS = "https://archive.openrs2.org/clients"


def log(msg: str) -> None:
    print(msg, flush=True)


def base_env() -> dict:
    env = os.environ.copy()
    home = jdk.java_home()
    env["JAVA_HOME"] = str(home)
    env["PATH"] = str(home / "bin") + os.pathsep + env.get("PATH", "")
    return env


def run_cmd(cmd: list[str], env: dict, dry_run: bool) -> int:
    log("+ " + " ".join(cmd))
    if dry_run:
        return 0
    proc = subprocess.run(cmd, cwd=ROOT, env=env)
    return proc.returncode


def python(script: str, args: list[str]) -> list[str]:
    return [sys.executable, str(ROOT / "scripts" / script), *args]


def ensure_clients_html(dry_run: bool) -> int:
    html_path = ROOT / "work" / "clients.html"
    if html_path.is_file() and html_path.stat().st_size > 0:
        log("stage 2: work/clients.html present")
        return 0
    log(f"stage 2: download {OPENRS2_CLIENTS} -> work/clients.html")
    if dry_run:
        return 0
    import archive_builds as ab  # noqa: E402

    ab.download(OPENRS2_CLIENTS, html_path)
    if not html_path.is_file() or html_path.stat().st_size == 0:
        log("stage 2: clients.html download failed")
        return 1
    return 0


def build_dirs(only: list[str]) -> list[Path]:
    out = []
    builds = ROOT / "builds"
    if not builds.is_dir():
        return out
    wanted = set(only)
    for fam in sorted(path for path in builds.iterdir() if path.is_dir()):
        for bdir in sorted(path for path in fam.iterdir() if path.is_dir()):
            head = bdir.name.split("-", 1)[0]
            if wanted and head not in wanted and bdir.name not in wanted:
                continue
            out.append(bdir)
    return out


def needs_compile(bdir: Path, compile_all: bool) -> bool:
    if compile_all:
        return True
    status = bdir / "status.json"
    if not status.is_file():
        return True
    try:
        data = json.loads(status.read_text())
    except json.JSONDecodeError:
        return True
    return data.get("compiled") is not True


def gradle_project(bdir: Path) -> Path | None:
    for cand in (bdir / "src", bdir / "src" / "client", bdir):
        if (cand / "gradlew").is_file():
            return cand
    return None


def mark_compiled(bdir: Path) -> None:
    path = bdir / "status.json"
    data: dict = {}
    if path.is_file():
        try:
            loaded = json.loads(path.read_text())
        except json.JSONDecodeError:
            loaded = {}
        if isinstance(loaded, dict):
            data = loaded
    data["compiled"] = True
    data.setdefault("family", bdir.parent.name)
    data.setdefault("folder", str(bdir.relative_to(ROOT)))
    head = bdir.name.split("-", 1)[0]
    if "rev" not in data and head.isdigit():
        data["rev"] = int(head)
    for key in ("deob_jar", "compiled_jar", "folder"):
        val = data.get(key)
        if isinstance(val, str) and val.startswith("/"):
            try:
                data[key] = str(Path(val).resolve().relative_to(ROOT.resolve()))
            except ValueError:
                pass
    path.write_text(json.dumps(data, indent=2) + "\n")


def compile_clients(only: list[str], compile_all: bool, dry_run: bool) -> int:
    env = base_env()
    rc = 0
    planned = []
    for bdir in build_dirs(only):
        if not needs_compile(bdir, compile_all):
            continue
        proj = gradle_project(bdir)
        if proj is None:
            log(f"stage 8: no gradlew for {bdir.relative_to(ROOT)}, skip")
            continue
        planned.append((bdir, proj))
    if not planned:
        log("stage 8: no clients need compileJava")
        return 0
    log(f"stage 8: compileJava for {len(planned)} client(s) with JAVA_HOME={jdk.java_home()}")
    for bdir, proj in planned:
        rel = proj.relative_to(ROOT)
        cmd = [str(proj / "gradlew"), "compileJava", "--no-daemon"]
        log(f"stage 8: {rel} {' '.join(cmd)}")
        if dry_run:
            continue
        proc = subprocess.run(cmd, cwd=proj, env=env)
        if proc.returncode == 0:
            mark_compiled(bdir)
        else:
            log(f"stage 8: compile failed for {bdir.relative_to(ROOT)}")
            rc = proc.returncode or 1
    return rc


def write_root_summary(dry_run: bool) -> int:
    log("stage 9: rewrite SUMMARY.json from builds/*/*/status.json")
    if dry_run:
        return 0
    import archive_builds as ab  # noqa: E402

    rows = ab.write_summary()
    text = (ROOT / "SUMMARY.json").read_text()
    if "/home/" in text:
        log("stage 9: SUMMARY.json contains an absolute home path")
        return 1
    log(f"stage 9: wrote SUMMARY.json ({len(rows)} builds)")
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Download, deobfuscate, standardize, and compile archived clients.")
    parser.add_argument("--dry-run", action="store_true", help="print each stage and write nothing")
    parser.add_argument("--only", action="append", default=[], metavar="REV", help="limit to this revision")
    parser.add_argument("--force-sources", action="store_true", help="set FORCE_SOURCES=1 for archive_builds.py")
    parser.add_argument("--force-download", action="store_true", help="set FORCE_DOWNLOAD=1 for year-end fetches")
    parser.add_argument("--compile-all", action="store_true", help="compile every client, not only uncompiled ones")
    args = parser.parse_args(sys.argv[1:] if argv is None else argv)
    dry = args.dry_run
    only = [str(rev) for rev in args.only]
    rc = 0

    log("stage 1: resolve JDK 21+")
    try:
        home = jdk.java_home()
    except SystemExit as ex:
        log(str(ex))
        return 1
    log(f"stage 1: JAVA_HOME={home}")

    log("stage 2: ensure work/clients.html")
    rc = ensure_clients_html(dry) or rc

    log("stage 3: archive_builds.py milestone catalog (no FORCE_SOURCES unless --force-sources)")
    archive_env = base_env()
    archive_env.pop("FORCE_DEOB", None)
    if args.force_sources:
        archive_env["FORCE_SOURCES"] = "1"
        log("stage 3: FORCE_SOURCES=1")
    else:
        archive_env.pop("FORCE_SOURCES", None)
    code = run_cmd(python("archive_builds.py", only), archive_env, dry)
    rc = code or rc

    log("stage 4: fetch_year_end_clients.py (skip existing gamepacks unless --force-download)")
    fetch_env = base_env()
    if args.force_download:
        fetch_env["FORCE_DOWNLOAD"] = "1"
        log("stage 4: FORCE_DOWNLOAD=1")
    else:
        fetch_env.pop("FORCE_DOWNLOAD", None)
    code = run_cmd(python("fetch_year_end_clients.py", only), fetch_env, dry)
    rc = code or rc

    log("stage 5: deob_all_clients.py without --force (existing source trees skipped)")
    deob_env = base_env()
    deob_env.pop("FORCE_DEOB", None)
    deob_env.pop("FORCE_SOURCES", None)
    code = run_cmd(python("deob_all_clients.py", []), deob_env, dry)
    rc = code or rc

    log("stage 6: repair_rs3_831_860.py (idempotent; second run does not rewrite sources)")
    code = run_cmd(python("repair_rs3_831_860.py", []), base_env(), dry)
    rc = code or rc

    renamer = ROOT / "scripts" / "revert_false_shares.py"
    if renamer.is_file():
        log("stage 7: revert_false_shares.py --apply")
        code = run_cmd(python("revert_false_shares.py", ["--apply"]), base_env(), dry)
        rc = code or rc
    else:
        log("stage 7: skip scripts/revert_false_shares.py (not present)")

    log("stage 8: compileJava for clients that are not yet compiled")
    code = compile_clients(only, args.compile_all, dry)
    rc = code or rc

    code = write_root_summary(dry)
    rc = code or rc
    return rc


if __name__ == "__main__":
    sys.exit(main())
