#!/usr/bin/env python3
"""Download, deobfuscate, and decompile one RuneScape client per year (2005-2011)."""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import jdk  # noqa: E402

TOOLS = ROOT / "tools"
BUILDS = ROOT / "builds"
WORK = ROOT / "work"
LOGS = ROOT / "logs"

OPENRS2 = "https://archive.openrs2.org"

# Canonical yearly snapshots used by the major emulator projects:
# 254 (2004, remap + cache), 317 (2006scape), 377, 468 (Aug 2007, pre-GWD),
# 474 (pre-trade-removal), 508 (HD), 530 (2009scape), 614 (mid-2010),
# 667 (2011Scape), 742 (2012 / LostCity).
CATALOG = [
    {
        "year": 2004,
        "rev": 254,
        "date": "2004-09-07",
        "client_id": 33304,
        "cache_id": 1229,
        "cache_scope": "runescape",
        "profile": "rs2-2004",
        "remap": "rs2-254.txt",
        "engine": "old",
        "cache_home": ".file_store_32",
        "main_class": "client",
        "run_args": ["10", "0", "highmem", "members", "32"],
        "emulator": "Lost City-era 2004 servers (254 is September 2004; Lost City itself targets 225)",
        "notes": "RS2 with a matching OpenRS2 cache and a RuneWiki remap. Construction not yet in this client.",
    },
    {
        "year": 2005,
        "rev": 317,
        "date": "2005-06-13",
        "client_id": 33303,
        "cache_id": 876,
        "cache_scope": "runescape",
        "profile": "rs2-2005",
        "remap": "rs2-317.txt",
        "engine": "old",
        "cache_home": ".file_store_32",
        "main_class": "client",
        "run_args": ["10", "0", "highmem", "members", "32"],
        "emulator": "2006scape / Elvarg-style 317 servers, port 43594",
        "notes": "Most common RS2 deob. Construction not yet in this client.",
    },
    {
        "year": 2006,
        "rev": 377,
        "date": "2006-05-02",
        "client_id": 31589,
        "cache_id": 657,
        "cache_scope": "runescape",
        "profile": "rs2-2005",
        "remap": "rs2-377.txt",
        "engine": "old",
        "cache_home": ".file_store_32",
        "main_class": "client",
        "run_args": ["10", "0", "highmem", "members", "32"],
        "emulator": "377 servers (Construction-era)",
        "notes": "Construction, 2006 gameframe.",
    },
    {
        "year": 2007,
        "rev": 468,
        "date": "2007-08-08",
        "client_id": 31163,
        "cache_id": 633,
        "cache_scope": "runescape",
        "profile": "rs2-2007",
        "remap": "rs2-468.txt",
        "engine": "js5",
        "cache_home": ".jagex_cache_32/runescape",
        "main_class": "client",
        "run_args": [],
        "emulator": "468 servers (August 2007, pre-GWD)",
        "notes": "August 2007 snapshot, just before the God Wars Dungeon era (474).",
    },
    {
        "year": 2007,
        "rev": 474,
        "date": "2007-10-19",
        "client_id": 30895,
        "cache_id": 237,
        "cache_scope": "runescape",
        "profile": "rs2-2007",
        "remap": "rs2-474.txt",
        "engine": "js5",
        "cache_home": ".jagex_cache_32/runescape",
        "main_class": "client",
        "run_args": [],
        "emulator": "474 servers (GWD, last pre-trade-removal snapshot)",
        "notes": "God Wars Dungeon present. Free trade / Wilderness still live.",
    },
    {
        "year": 2008,
        "rev": 508,
        "date": "2008-08-05",
        "client_id": 30983,
        "cache_id": 251,
        "cache_scope": "runescape",
        "profile": "rs2-2009",
        "remap": "rs2-508.txt",
        "engine": "js5",
        "cache_home": ".jagex_cache_32/runescape",
        "main_class": "client",
        "run_args": [],
        "emulator": "508 HD-era servers",
        "notes": "RuneScape HD + resizable window.",
    },
    {
        "year": 2009,
        "rev": 530,
        "date": "2009-01-28",
        "client_id": 31194,
        "cache_id": 254,
        "cache_scope": "runescape",
        "profile": "rs2-2009",
        "remap": "rs2-530.txt",
        "engine": "js5",
        "cache_home": ".jagex_cache_32/runescape",
        "main_class": "client",
        "run_args": [],
        "emulator": "2009scape (revision 530)",
        "notes": "Software client matching 2009scape's original target.",
    },
    {
        "year": 2010,
        "rev": 614,
        "date": "2010-07-13",
        "client_id": 31098,
        "cache_id": 615,
        "cache_scope": "runescape",
        "profile": "rs2-2011",
        "remap": "rs2-614.txt",
        "engine": "js5",
        "cache_home": ".jagex_cache_32/runescape",
        "main_class": "client",
        "run_args": [],
        "emulator": "614-era RT5 servers",
        "notes": "RuneTek 5, Constitution (10x HP) era.",
    },
    {
        "year": 2011,
        "rev": 667,
        "date": "2011-09-29",
        "client_id": 31169,
        "cache_id": 1473,
        "cache_scope": "runescape",
        "profile": "rs2-2011",
        "remap": "rs2-667.txt",
        "engine": "js5",
        "cache_home": ".jagex_cache_32/runescape",
        "main_class": "client",
        "run_args": [],
        "emulator": "2011Scape (revision 667)",
        "notes": "Software client for October 2011. 2011Scape uses this revision.",
    },
    {
        "year": 2012,
        "rev": 742,
        "date": "2012-11-07",
        "client_id": None,
        "cache_id": 544,
        "cache_scope": "runescape",
        "profile": "rs2-2012",
        "remap": None,
        "engine": "js5",
        "cache_home": ".jagex_cache_32/runescape",
        "main_class": "AppletViewer",
        "run_args": [],
        "emulator": "742 / 2012Scape-style RT5 EoC-era servers",
        "notes": "OpenRS2 has the 742 cache but no vanilla gamepack. Original 742.2 is lost; this year uses LostCity RS742.1 source plus the OpenRS2 cache.",
        "skip_gamepack": True,
    },
]


def log(msg: str) -> None:
    print(msg, flush=True)


def run(cmd, cwd=None, env=None, check=True):
    log("+ " + " ".join(map(str, cmd)))
    return subprocess.run(cmd, cwd=cwd, env=env, check=check)


def download(url: str, dest: Path) -> None:
    dest.parent.mkdir(parents=True, exist_ok=True)
    if dest.exists() and dest.stat().st_size > 0:
        log(f"skip download (exists): {dest}")
        return
    tmp = dest.with_suffix(dest.suffix + ".part")
    run(
        [
            "curl",
            "-L",
            "--fail",
            "--retry",
            "5",
            "--retry-all-errors",
            "-C",
            "-",
            "-o",
            str(tmp),
            url,
        ]
    )
    tmp.replace(dest)


def build_dir(entry) -> Path:
    version = str(entry["profile"]).split("-", 1)[0]
    return BUILDS / version / f"{entry['rev']}-{entry['date']}"


def repo_relative(path: Path) -> str:
    try:
        return str(path.resolve().relative_to(ROOT.resolve()))
    except ValueError:
        return str(path)


def read_status(bdir: Path) -> dict:
    path = bdir / "status.json"
    if not path.is_file():
        return {}
    try:
        data = json.loads(path.read_text())
    except json.JSONDecodeError:
        return {}
    return data if isinstance(data, dict) else {}


def kept_java_tree(bdir: Path) -> Path | None:
    """Existing renamed sources. Not replaced unless FORCE_SOURCES=1."""
    if os.environ.get("FORCE_SOURCES") == "1":
        return None
    for path in (
        bdir / "src" / "src" / "main" / "java",
        bdir / "src" / "client" / "src" / "main" / "java",
    ):
        if path.is_dir() and any(path.rglob("*.java")):
            return path
    return None


def relativize_status_paths(data: dict) -> dict:
    for key in ("deob_jar", "compiled_jar", "folder"):
        val = data.get(key)
        if not isinstance(val, str) or not val.startswith("/"):
            continue
        try:
            data[key] = str(Path(val).resolve().relative_to(ROOT.resolve()))
        except ValueError:
            continue
    return data


def install_cache(entry, bdir: Path) -> Path:
    keys_url = f"{OPENRS2}/caches/{entry['cache_scope']}/{entry['cache_id']}/keys.json"
    cache_url = f"{OPENRS2}/caches/{entry['cache_scope']}/{entry['cache_id']}/disk.zip"
    download(keys_url, bdir / "cache" / "keys.json")
    cache_zip = bdir / "original" / "cache-disk.zip"
    download(cache_url, cache_zip)
    cache_dir = bdir / "cache"
    extracted_flag = cache_dir / ".extracted"
    if not extracted_flag.exists():
        with zipfile.ZipFile(cache_zip) as zf:
            zf.extractall(cache_dir)
        inner = cache_dir / "cache"
        if inner.is_dir():
            for child in inner.iterdir():
                target = cache_dir / child.name
                if not target.exists():
                    shutil.move(str(child), str(target))
        extracted_flag.write_text("ok\n")
    return cache_dir


def summary_rows() -> list[dict]:
    rows = []
    if not BUILDS.is_dir():
        return rows
    for status_path in sorted(BUILDS.glob("*/*/status.json")):
        bdir = status_path.parent
        data = read_status(bdir)
        rev = data.get("rev")
        if not isinstance(rev, int):
            head = bdir.name.split("-", 1)[0]
            rev = int(head) if head.isdigit() else rev
        compiled = data.get("compiled")
        rows.append(
            {
                "family": bdir.parent.name,
                "rev": rev,
                "folder": repo_relative(bdir),
                "compiled": compiled is True,
                "cache": (bdir / "cache").is_dir(),
            }
        )
    return rows


def write_summary() -> list[dict]:
    rows = summary_rows()
    (ROOT / "SUMMARY.json").write_text(json.dumps(rows, indent=2) + "\n")
    return rows


def write_deob_toml(dest: Path, entry) -> None:
    profile_src = TOOLS / "rs-deob" / "profiles" / f"{entry['profile']}.toml"
    text = profile_src.read_text()
    # Bytecode-only first: we decompile with the installed fernflower afterwards.
    text = text.replace("decompile = true", "decompile = false")
    text = text.replace("cleanup = true", "cleanup = false")
    if entry["remap"]:
        text = text.replace("[profile.remap]\nenable = true", "[profile.remap]\nenable = true")
    else:
        text = text.replace("[profile.remap]\nenable = true", "[profile.remap]\nenable = false")
    dest.write_text(text)


def deobfuscate(entry, bdir: Path) -> Path:
    """Produce gamepack-deob.jar via LostCity deobfuscator."""
    deob_jar = bdir / "deob" / "gamepack-deob.jar"
    force = os.environ.get("FORCE_DEOB") == "1"
    if deob_jar.exists() and deob_jar.stat().st_size > 0 and not force:
        log(f"skip deob (exists): {deob_jar}")
        return deob_jar

    work = WORK / f"deob-{entry['rev']}"
    if work.exists():
        shutil.rmtree(work)
    (work / "ref").mkdir(parents=True)
    shutil.copy2(bdir / "original" / "gamepack.jar", work / "ref" / "gamepack.jar")
    write_deob_toml(work / "deob.toml", entry)
    if entry["remap"]:
        remap = TOOLS / "rs-deob" / "remap" / entry["remap"]
        if remap.exists():
            shutil.copy2(remap, work / "remap.txt")

    logf = LOGS / f"deob-{entry['rev']}.log"
    with logf.open("w") as fh:
        proc = subprocess.run(
            [
                str(jdk.java_bin()),
                "-Xmx4G",
                "-jar",
                str(TOOLS / "deobfuscator" / "Deobfuscator.jar"),
            ],
            cwd=work,
            stdout=fh,
            stderr=subprocess.STDOUT,
        )
    if proc.returncode != 0:
        raise RuntimeError(f"deob failed for {entry['rev']}, see {logf}")

    produced = work / "ref" / "gamepack-deob.jar"
    if not produced.exists():
        # fallback: search
        cands = list(work.rglob("*-deob.jar"))
        if not cands:
            raise RuntimeError(f"no deob jar produced for {entry['rev']}, see {logf}")
        produced = cands[0]
    deob_jar.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(produced, deob_jar)
    return deob_jar


def fernflower_outdated(deob_jar: Path, out_dir: Path) -> bool:
    """Refresh fernflower/ only when it is missing or the deob jar is newer."""
    if not out_dir.exists():
        return True
    if not deob_jar.exists():
        return False
    return deob_jar.stat().st_mtime > out_dir.stat().st_mtime


def fernflower_decompile(deob_jar: Path, out_dir: Path, rev: int) -> None:
    marker = out_dir / ".decompiled"
    if not fernflower_outdated(deob_jar, out_dir):
        log(f"skip fernflower (up to date): {out_dir}")
        return
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True)
    logf = LOGS / f"fernflower-{rev}.log"
    with logf.open("w") as fh:
        proc = subprocess.run(
            [
                str(jdk.java_bin()),
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
        )
    if proc.returncode != 0:
        raise RuntimeError(f"fernflower failed for {rev}, see {logf}")
    # fernflower writes a jar of sources
    src_jars = list(out_dir.glob("*.jar"))
    if src_jars:
        with zipfile.ZipFile(src_jars[0]) as zf:
            zf.extractall(out_dir / "extracted")
        java_root = out_dir / "extracted"
    else:
        java_root = out_dir
    java_files = list(java_root.rglob("*.java"))
    if not java_files:
        raise RuntimeError(f"fernflower produced no java for {rev}, see {logf}")
    marker.write_text(f"{len(java_files)} files\n")


def copy_sources_into_project(entry, bdir: Path, fern_out: Path) -> Path:
    proj = bdir / "src"
    src_java = proj / "src" / "main" / "java"
    kept = kept_java_tree(bdir)
    if kept is not None:
        log(f"renamed sources were kept: {kept}")
        return proj
    if src_java.exists():
        shutil.rmtree(src_java)
    extracted = fern_out / "extracted"
    if not extracted.exists():
        extracted = fern_out
    src_java.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(extracted, src_java)

    # gradle wrapper + build files
    template_base = TOOLS / "rs-deob" / "template" / "base"
    template_prof = TOOLS / "rs-deob" / "template" / entry["profile"]
    for item in template_base.iterdir():
        dest = proj / item.name
        if item.is_dir():
            if dest.exists():
                shutil.rmtree(dest)
            shutil.copytree(item, dest)
        else:
            shutil.copy2(item, dest)
    if template_prof.exists():
        for item in template_prof.iterdir():
            dest = proj / item.name
            if item.name == "src":
                # merge extra java (AppletViewer) if present
                extra = item / "main" / "java"
                if extra.exists():
                    for f in extra.rglob("*"):
                        if f.is_file():
                            target = src_java / f.relative_to(extra)
                            target.parent.mkdir(parents=True, exist_ok=True)
                            shutil.copy2(f, target)
                continue
            if item.is_dir():
                if dest.exists() and dest.name != "src":
                    shutil.rmtree(dest)
                if not dest.exists():
                    shutil.copytree(item, dest)
            else:
                shutil.copy2(item, dest)

    # Prefer a discoverable main class
    mains = []
    for p in src_java.rglob("*.java"):
        text = p.read_text(errors="ignore")
        if "public static void main(" in text:
            rel = p.relative_to(src_java).with_suffix("")
            mains.append(".".join(rel.parts))
    main = entry["main_class"]
    if mains:
        # prefer exact 'client' or 'Client' or 'RS2Loader'
        for cand in mains:
            simple = cand.split(".")[-1]
            if simple.lower() in {"client", "rs2loader", "appletviewer", "game"}:
                main = cand
                break
        else:
            main = mains[0]

    build_gradle = proj / "build.gradle"
    text = build_gradle.read_text()
    text = text.replace('mainClass = "deob.client"', f'mainClass = "{main}"')
    text = text.replace("mainClass = 'deob.client'", f"mainClass = '{main}'")
    text = text.replace("'Main-Class': 'deob.client'", f"'Main-Class': '{main}'")
    build_gradle.write_text(text)

    (proj / "MAIN_CLASS.txt").write_text(main + "\n")
    return proj


def patch_cache_dir(src_java: Path, cache_home: str) -> None:
    """Make findcachedir() prefer user.home so run.sh's private HOME is used."""
    wrapper = (
        "\tpublic static String findcachedir() {\n"
        f'\t\tString override = System.getProperty("user.home") + "/{cache_home}/";\n'
        "\t\tFile dir = new File(override);\n"
        "\t\tif (dir.exists() || dir.mkdirs()) {\n"
        "\t\t\treturn override.endsWith(\"/\") ? override : override + \"/\";\n"
        "\t\t}\n"
        "\t\treturn findcachedir_original();\n"
        "\t}\n\n"
    )
    defn_re = re.compile(
        r"(public static String findcachedir\s*\(\s*\))",
        re.M,
    )
    for path in src_java.rglob("*.java"):
        text = path.read_text(errors="ignore")
        if "findcachedir_original" in text:
            continue
        m = defn_re.search(text)
        if not m:
            continue
        updated = text[: m.start()] + wrapper + "\tpublic static String findcachedir_original()" + text[m.end() :]
        if "import java.io.File;" not in updated:
            updated = updated.replace("\npublic ", "\nimport java.io.File;\n\npublic ", 1)
        path.write_text(updated)
        log(f"patched cache dir in {path.relative_to(src_java)}")
        return


def fix_duplicate_methods(src_java: Path) -> int:
    """Rename overloaded methods that Fernflower collapsed to identical signatures."""
    method_re = re.compile(
        r"(?P<pre>^[ \t]*(?:(?:public|private|protected|static|final|synchronized|native|abstract|strictfp)\s+)+)"
        r"(?P<ret>[\w.<>,\[\]\s]+?)\s+"
        r"(?P<name>[A-Za-z_][\w$]*)\s*"
        r"\((?P<args>[^)]*)\)"
        r"(?P<post>\s*(?:throws\s+[^{;]+)?\s*\{)",
        re.M,
    )
    fixed = 0
    for path in src_java.rglob("*.java"):
        text = path.read_text(errors="ignore")
        seen = {}
        out = []
        last = 0
        for m in method_re.finditer(text):
            name = m.group("name")
            args = re.sub(r"\s+", " ", m.group("args")).strip()
            key = (name, args)
            out.append(text[last : m.start()])
            if key in seen:
                seen[key] += 1
                new_name = f"{name}_dup{seen[key]}"
                out.append(m.group("pre") + m.group("ret") + " " + new_name + "(" + m.group("args") + ")" + m.group("post"))
                fixed += 1
            else:
                seen[key] = 0
                out.append(m.group(0))
            last = m.end()
        out.append(text[last:])
        new_text = "".join(out)
        if new_text != text:
            path.write_text(new_text)
    return fixed


def parse_compile_errors(log_text: str) -> list[tuple[Path, int, str]]:
    errors = []
    for m in re.finditer(r"^(.+\.java):(\d+): error: (.+)$", log_text, re.M):
        errors.append((Path(m.group(1)), int(m.group(2)), m.group(3)))
    return errors


def apply_error_fixes(errors: list[tuple[Path, int, str]]) -> int:
    """Best-effort source edits for common Fernflower compile failures."""
    changed = 0
    for path, line_no, msg in errors:
        if not path.exists():
            continue
        lines = path.read_text(errors="ignore").splitlines(True)
        idx = line_no - 1
        if idx < 0 or idx >= len(lines):
            continue
        if "is already defined" in msg:
            m = re.search(r"\b([A-Za-z_][\w$]*)\s*\(", lines[idx])
            if m:
                lines[idx] = lines[idx].replace(m.group(1) + "(", m.group(1) + "_dup(", 1)
                path.write_text("".join(lines))
                changed += 1
        elif "cannot find symbol" in msg:
            # Drop obviously broken override annotations that confuse javac
            if "@Override" in lines[max(0, idx - 3) : idx + 1]:
                for j in range(max(0, idx - 3), idx + 1):
                    if "@Override" in lines[j]:
                        lines[j] = lines[j].replace("@Override", "// @Override")
                        path.write_text("".join(lines))
                        changed += 1
                        break
    return changed


def inject_missing_symbols(src_java: Path, log_text: str) -> int:
    """Add stub fields for Fernflower-dropped symbols so the tree can compile."""
    changed = 0
    # javac: path:line: error: cannot find symbol\n  ...\n  symbol:   variable NAME\n  location: class CLASS
    for m in re.finditer(
        r"^(?P<path>.+\.java):(?P<line>\d+): error: cannot find symbol\n(?:.*\n){0,3}\s*symbol:\s+variable (?P<name>\w+)\n\s*location: (?:class|variable) (?P<cls>[\w.]+)",
        log_text,
        re.M,
    ):
        path = Path(m.group("path"))
        name = m.group("name")
        if not path.exists():
            # try relative to src_java
            cands = list(src_java.rglob(path.name))
            path = cands[0] if cands else path
        if not path.exists():
            continue
        text = path.read_text(errors="ignore")
        if re.search(rf"\b{re.escape(name)}\s*[=;]", text):
            # might still be missing as a field; check field decl
            if re.search(rf"\b(?:int|boolean|byte|short|long|float|double|char|String|int\[\])\s+{re.escape(name)}\b", text):
                continue
        # Guess type from the usage line
        usage = ""
        try:
            usage = path.read_text(errors="ignore").splitlines()[int(m.group("line")) - 1]
        except Exception:
            usage = ""
        if re.search(rf"{re.escape(name)}\s*\?", usage) or "boolean" in usage:
            typ = "boolean"
            init = " = false"
        elif re.search(rf"{re.escape(name)}\s*\[", usage):
            typ = "int[]"
            init = " = null"
        elif re.search(rf"{re.escape(name)}\s*[\+\-\*/]|{re.escape(name)}\s*,", usage):
            typ = "int"
            init = " = 0"
        else:
            typ = "boolean"
            init = " = true"
        # Insert after the class opening brace
        new_text, n = re.subn(
            r"(public(?:\s+final)?\s+class\s+\w+[^{]*\{)",
            rf"\1\n\n\tpublic static {typ} {name}{init};\n",
            text,
            count=1,
        )
        if n:
            path.write_text(new_text)
            changed += 1
            log(f"injected {typ} {name} into {path.name}")
    return changed


def try_compile(proj: Path, rev: int) -> bool:
    logf = LOGS / f"compile-{rev}.log"
    gradlew = proj / "gradlew"
    if gradlew.exists():
        os.chmod(gradlew, 0o755)
        cmd = [str(gradlew), "compileJava", "--no-daemon"]
    else:
        cmd = ["gradle", "compileJava", "--no-daemon"]
    src_java = proj / "src" / "main" / "java"
    env = os.environ.copy()
    home = jdk.java_home()
    env["JAVA_HOME"] = str(home)
    env["PATH"] = str(home / "bin") + os.pathsep + env.get("PATH", "")
    for attempt in range(6):
        with logf.open("w") as fh:
            proc = subprocess.run(cmd, cwd=proj, env=env, stdout=fh, stderr=subprocess.STDOUT)
        if proc.returncode == 0:
            return True
        log_text = logf.read_text(errors="ignore")
        errors = parse_compile_errors(log_text)
        n = fix_duplicate_methods(src_java)
        n += apply_error_fixes(errors)
        n += inject_missing_symbols(src_java, log_text)
        log(f"compile attempt {attempt + 1} for {rev} failed ({len(errors)} errors), applied {n} fixes")
        if n == 0:
            break
    return False


def lostcity_source_cleanup(entry, bdir: Path) -> None:
    """Re-run deobfuscator with fernflower+AST cleanup into the gradle project."""
    kept = kept_java_tree(bdir)
    if kept is not None:
        log(f"renamed sources were kept: {kept}")
        return
    work = WORK / f"deob-src-{entry['rev']}"
    if work.exists():
        shutil.rmtree(work)
    (work / "ref").mkdir(parents=True)
    shutil.copy2(bdir / "original" / "gamepack.jar", work / "ref" / "gamepack.jar")
    profile_src = TOOLS / "rs-deob" / "profiles" / f"{entry['profile']}.toml"
    text = profile_src.read_text()
    if not entry["remap"]:
        text = text.replace("[profile.remap]\nenable = true", "[profile.remap]\nenable = false")
    (work / "deob.toml").write_text(text)
    if entry["remap"]:
        remap = TOOLS / "rs-deob" / "remap" / entry["remap"]
        if remap.exists():
            shutil.copy2(remap, work / "remap.txt")
    # copy gradle template so cleanup sees src tree
    template_base = TOOLS / "rs-deob" / "template" / "base"
    for item in template_base.iterdir():
        dest = work / item.name
        if item.is_dir():
            shutil.copytree(item, dest)
        else:
            shutil.copy2(item, dest)
    logf = LOGS / f"deob-src-{entry['rev']}.log"
    with logf.open("w") as fh:
        proc = subprocess.run(
            [
                str(jdk.java_bin()),
                "-Xmx4G",
                "-jar",
                str(TOOLS / "deobfuscator" / "Deobfuscator.jar"),
            ],
            cwd=work,
            stdout=fh,
            stderr=subprocess.STDOUT,
        )
    if proc.returncode != 0:
        raise RuntimeError(f"lostcity source deob failed for {entry['rev']}, see {logf}")
    produced = work / "src" / "main" / "java"
    if not produced.exists():
        raise RuntimeError(f"no source produced for {entry['rev']}, see {logf}")
    dest = bdir / "src" / "src" / "main" / "java"
    if dest.exists():
        shutil.rmtree(dest)
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(produced, dest)


_APP_MAIN_RE = re.compile(r"""mainClass(?:\.set)?\s*(?:=|\()\s*['"]([^'"]+)['"]""")
_MANIFEST_MAIN_RE = re.compile(r"""['"]Main-Class['"]\s*:\s*['"]([^'"]+)['"]""")


def gradle_launch_names(bdir: Path) -> tuple[str | None, str | None]:
    app_main = None
    manifest_main = None
    for path in (
        bdir / "src" / "build.gradle",
        bdir / "src" / "build.gradle.kts",
        bdir / "src" / "client" / "build.gradle",
        bdir / "src" / "client" / "build.gradle.kts",
    ):
        if not path.is_file():
            continue
        text = path.read_text(errors="ignore")
        if app_main is None:
            match = _APP_MAIN_RE.search(text)
            if match:
                app_main = match.group(1)
        if manifest_main is None:
            match = _MANIFEST_MAIN_RE.search(text)
            if match:
                manifest_main = match.group(1)
    return app_main, manifest_main


def class_source_exists(bdir: Path, name: str) -> bool:
    rel = Path(*name.split(".")).with_suffix(".java")
    for root in (
        bdir / "src" / "src" / "main" / "java",
        bdir / "src" / "client" / "src" / "main" / "java",
    ):
        if (root / rel).is_file():
            return True
    return False


def resolved_main_class(entry, bdir: Path) -> str:
    app_main, manifest_main = gradle_launch_names(bdir)
    for cand in (app_main, manifest_main):
        if cand and class_source_exists(bdir, cand):
            return cand
    if app_main:
        return app_main
    if manifest_main:
        return manifest_main
    main_file = bdir / "src" / "MAIN_CLASS.txt"
    if main_file.is_file():
        text = main_file.read_text().strip()
        if text:
            return text
    return entry.get("main_class") or "client"


def preserved_cache_home(entry, bdir: Path) -> str:
    run_sh = bdir / "run.sh"
    if run_sh.is_file():
        match = re.search(r'mkdir -p "\$RUNTIME/([^"]+)"', run_sh.read_text(errors="ignore"))
        if match:
            return match.group(1)
    return entry["cache_home"]


def run_script_ok(entry, bdir: Path) -> bool:
    run_sh = bdir / "run.sh"
    if not run_sh.is_file():
        return False
    text = run_sh.read_text(errors="ignore")
    if "RS_HOST" not in text or "RS_PORT" not in text:
        return False
    cache_home = entry.get("cache_home") or ""
    if cache_home and cache_home not in text:
        return False
    match = re.search(r'-cp\s+"\$CP"\s+(\S+)', text)
    if not match:
        return False
    return match.group(1) == resolved_main_class(entry, bdir)


def write_run_scripts(entry, bdir: Path, *, readme: bool = True, skipping: bool = False) -> None:
    if entry.get("rev") == 742 or entry.get("skip_gamepack"):
        log(f"leave run.sh unchanged for rev {entry.get('rev')} (Lost City checkout)")
        return
    if skipping and run_script_ok(entry, bdir):
        log(f"leave run.sh unchanged (already correct): {repo_relative(bdir / 'run.sh')}")
        return
    cache_rel = preserved_cache_home(entry, bdir)
    main = resolved_main_class(entry, bdir)
    args = " ".join(entry["run_args"])
    run_sh = bdir / "run.sh"
    run_sh.write_text(
        f"""#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
RUNTIME="$DIR/runtime-home"
mkdir -p "$RUNTIME/{cache_rel}"
# Place the OpenRS2 cache where this era of client looks for it.
if [ -d "$DIR/cache" ]; then
  cp -an "$DIR/cache/." "$RUNTIME/{cache_rel}/" 2>/dev/null || true
fi
export HOME="$RUNTIME"
USER_HOME="$RUNTIME"

# Prefer a freshly compiled jar; fall back to the deobfuscated gamepack.
if [ -f "$DIR/src/build/libs/client.jar" ]; then
  CP="$DIR/src/build/libs/client.jar"
elif [ -f "$DIR/src/build/libs/rs2client.jar" ]; then
  CP="$DIR/src/build/libs/rs2client.jar"
elif ls "$DIR/src/build/libs/"*.jar >/dev/null 2>&1; then
  CP="$(ls "$DIR/src/build/libs/"*.jar | head -1)"
else
  CP="$DIR/deob/gamepack-deob.jar"
fi

# Default to localhost so the client talks to a local emulator.
# Override with RS_HOST / RS_PORT if needed.
HOST="${{RS_HOST:-127.0.0.1}}"
PORT="${{RS_PORT:-43594}}"
echo "Launching rev {entry['rev']} ({entry['year']}) main={main}"
echo "Cache: $RUNTIME/{cache_rel}"
echo "Host: $HOST:$PORT  (start a matching emulator first)"
echo "Classpath: $CP"
cd "$DIR"
exec java -Xmx1G \\
  -Duser.home="$RUNTIME" \\
  -Dcom.jagex.config.address="$HOST" \\
  -cp "$CP" {main} {args} "$@"
"""
    )
    os.chmod(run_sh, 0o755)

    if not readme:
        return
    readme_path = bdir / "README.md"
    readme_path.write_text(
        f"""# RuneScape {entry['year']} — revision {entry['rev']}

Released {entry['date']}.

{entry['notes']}

Matching emulator: {entry['emulator']}

## Layout

- `original/gamepack.jar` — vanilla client downloaded from OpenRS2
- `deob/gamepack-deob.jar` — LostCity deobfuscator output
- `src/` — Gradle project with Fernflower-decompiled Java
- `cache/` — OpenRS2 disk cache (dat/idx) plus `keys.json` (XTEA)
- `fernflower/` — raw Fernflower jar output
- `run.sh` — launch with the cache remapped into a private home directory

## Compile

```bash
cd src
./gradlew compileJava
./gradlew jar
```

## Run (localhost emulator)

Start a server that speaks revision {entry['rev']}, then:

```bash
./run.sh
```

The client will use `runtime-home/{cache_rel}` so it does not touch your real Jagex cache.

RSA keys, JS5/ondemand hosts, and world-list URLs still need to match the emulator.
Search the decompiled sources for `jagex`, `runescape.com`, and `modulus` after the first launch if login fails.

OpenRS2 client: {OPENRS2}/clients/{entry['client_id']}
OpenRS2 cache:  {OPENRS2}/caches/{entry['cache_scope']}/{entry['cache_id']}
"""
    )


def process_cache_only(entry, bdir: Path) -> dict:
    """742-style entry: cache and keys only. Do not touch src/ or run.sh."""
    install_cache(entry, bdir)
    log(f"rev {entry['rev']} has no OpenRS2 gamepack; left src/ untouched")
    old = read_status(bdir)
    result = {
        "year": entry["year"],
        "rev": entry["rev"],
        "folder": repo_relative(bdir),
    }
    if "compiled" in old:
        result["compiled"] = old["compiled"]
    return result


def process_entry(entry) -> dict:
    bdir = build_dir(entry)
    orig = bdir / "original"
    orig.mkdir(parents=True, exist_ok=True)

    log(f"\n======== {entry['year']} rev {entry['rev']} ========")
    if entry.get("skip_gamepack") or not entry.get("client_id"):
        return process_cache_only(entry, bdir)

    download(f"{OPENRS2}/clients/{entry['client_id']}.dat", orig / "gamepack.jar")
    cache_dir = install_cache(entry, bdir)

    meta = {
        "year": entry["year"],
        "rev": entry["rev"],
        "openrs2_client_id": entry["client_id"],
        "openrs2_cache_id": entry["cache_id"],
        "profile": entry["profile"],
        "version": str(entry["profile"]).split("-", 1)[0],
    }
    (bdir / "meta.json").write_text(json.dumps({**entry, **meta}, indent=2) + "\n")

    kept = kept_java_tree(bdir)
    deob_jar = deobfuscate(entry, bdir)
    fern_out = bdir / "fernflower"
    fernflower_decompile(deob_jar, fern_out, entry["rev"])
    deob_rel = repo_relative(deob_jar)

    if kept is not None:
        log(f"renamed sources were kept: {kept}")
        write_run_scripts(entry, bdir, readme=False, skipping=True)
        status = read_status(bdir)
        status["year"] = entry["year"]
        status["rev"] = entry["rev"]
        status["deob_jar"] = deob_rel
        relativize_status_paths(status)
        (bdir / "status.json").write_text(json.dumps(status, indent=2) + "\n")
        result = {
            "year": entry["year"],
            "rev": entry["rev"],
            "folder": repo_relative(bdir),
            "deob_jar": deob_rel,
            "java_files": len(list(kept.rglob("*.java"))),
            "cache_files": len([p for p in cache_dir.iterdir() if p.is_file()]),
        }
        if "compiled" in status:
            result["compiled"] = status["compiled"]
        log(
            f"DONE {entry['year']} rev {entry['rev']}: kept sources"
            f" compiled={result.get('compiled')} java={result['java_files']}"
        )
        return result

    proj = copy_sources_into_project(entry, bdir, fern_out)
    src_java = proj / "src" / "main" / "java"
    n = fix_duplicate_methods(src_java)
    if n:
        log(f"renamed {n} duplicate methods after fernflower")
    patch_cache_dir(src_java, entry["cache_home"])
    compiled = try_compile(proj, entry["rev"])
    if not compiled and kept_java_tree(bdir) is None:
        log(f"compile failed for {entry['rev']}; retrying with LostCity fernflower+AST cleanup")
        try:
            lostcity_source_cleanup(entry, bdir)
            n = fix_duplicate_methods(src_java)
            if n:
                log(f"renamed {n} duplicate methods after cleanup")
            patch_cache_dir(src_java, entry["cache_home"])
            compiled = try_compile(proj, entry["rev"])
        except Exception as ex:
            log(f"cleanup retry failed: {ex}")
            compiled = False
    elif not compiled:
        log(f"compile failed for {entry['rev']}; renamed sources were kept")
    write_run_scripts(entry, bdir)
    status = {
        "year": entry["year"],
        "rev": entry["rev"],
        "deob_jar": deob_rel,
        "compiled": compiled,
        "java_files": len(list(src_java.rglob("*.java"))),
        "cache_files": len([p for p in cache_dir.iterdir() if p.is_file()]),
    }
    (bdir / "status.json").write_text(json.dumps(status, indent=2) + "\n")
    log(f"DONE {entry['year']} rev {entry['rev']}: compiled={compiled} java={status['java_files']}")
    return status


def main() -> int:
    LOGS.mkdir(parents=True, exist_ok=True)
    BUILDS.mkdir(parents=True, exist_ok=True)
    results = []
    only = sys.argv[1:]
    catalog = CATALOG
    if only:
        catalog = [e for e in CATALOG if str(e["rev"]) in only or str(e["year"]) in only]
    for entry in catalog:
        try:
            results.append(process_entry(entry))
        except Exception as ex:
            log(f"FAILED {entry['year']} {entry['rev']}: {ex}")
            results.append({"year": entry["year"], "rev": entry["rev"], "error": str(ex)})
    rows = write_summary()
    log("\n==== SUMMARY ====")
    log(json.dumps(rows, indent=2))
    return 0 if all("error" not in r for r in results) else 1


if __name__ == "__main__":
    sys.exit(main())
