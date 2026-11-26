#!/usr/bin/env python3
"""Install the last Java client of each RS2 and RS3 year.

RS2 (2004-2012) and RS3 Java (2013-2019). Existing milestone snapshots are
left in place. OpenRS2 class jars are used through 2010. Later years come
from the RuneWiki gamepack mirror on archive.openrs2.org. From 2016 the
mirror stores encrypted outer gamepacks; those are decrypted with the saved
jav_config parameters and unpacked from pack200.

2014 (831) and 2015 (860) keep the jav_config secret inside the outer jar
as parameters.txt, next to inner.pack.gz. That file uses "key" = "value"
lines rather than the later client_parameters.put form.

RS3 disk caches for these years are several gigabytes each and are not
downloaded. keys.json is saved when OpenRS2 has an exact build match and the
disk zip is under CACHE_MAX_BYTES.
"""

from __future__ import annotations

import base64
import hashlib
import io
import json
import os
import re
import shutil
import ssl
import subprocess
import sys
import urllib.request
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import archive_builds as ab  # noqa: E402
import jdk  # noqa: E402
from download_java_clients import parse_clients_html  # noqa: E402

BUILDS = ROOT / "builds"
WORK = ROOT / "work" / "year-end"
MIRROR = "https://archive.openrs2.org/pub/mirrors/runewiki.org/archive/client/gamepack"
OPENRS2 = "https://archive.openrs2.org"
PACK200_JAR_URL = "https://repo1.maven.org/maven2/org/glavo/pack200/0.3.0/pack200-0.3.0.jar"
CACHE_MAX_BYTES = 800 * 1024 * 1024
CTX = ssl.create_default_context()

# Exact OpenRS2 cache ids (live, English, disk_store_valid) from work/caches.json.
CACHE_IDS = {
    350: 1478,
    443: 1191,
    481: 568,
    525: 665,
    578: 259,
    634: 975,
}

UNPACK_JAVA = r"""
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.jar.JarOutputStream;
import org.glavo.pack200.Pack200;

public class UnpackPack200 {
    public static void main(String[] args) throws Exception {
        try (FileInputStream in = new FileInputStream(args[0]);
                JarOutputStream out = new JarOutputStream(new FileOutputStream(args[1]))) {
            Pack200.newUnpacker().unpack(in, out);
        }
    }
}
"""


def log(msg: str) -> None:
    print(msg, flush=True)


def fetch(url: str) -> bytes:
    req = urllib.request.Request(url, headers={"User-Agent": "rsarchive-year-end"})
    with urllib.request.urlopen(req, context=CTX, timeout=180) as resp:
        return resp.read()


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def class_month(data: bytes) -> str:
    zf = zipfile.ZipFile(io.BytesIO(data))
    skip = ("jaggl/", "jagdx/", "jagtheora/", "jaclib/", "META-INF/")
    months = []
    for info in zf.infolist():
        if not info.filename.endswith(".class") or info.filename.startswith(skip):
            continue
        y, m, *_ = info.date_time
        months.append(f"{y:04d}-{m:02d}")
    if not months:
        return ""
    return max(set(months), key=months.count)


def parse_params(text: str) -> dict[str, str]:
    keys = dict(re.findall(r'put\("([^"]+)",\s*"([^"]*)"\)', text))
    keys.update(re.findall(r'name="([^"]+)"\s+value="([^"]*)"', text))
    keys.update(re.findall(r'"([^"]+)"\s*=\s*"([^"]*)"', text))
    return keys


def key_bytes(value: str) -> bytes:
    if not value:
        return b""
    size = 3 * ((len(value) - 1) // 4) + 1
    padded = value.replace("*", "+").replace("-", "/")
    padded += "=" * ((4 - len(padded) % 4) % 4)
    raw = base64.b64decode(padded)
    out = bytearray(size)
    out[: min(len(raw), size)] = raw[:size]
    return bytes(out)


def decrypt_inner(blob: bytes, secret: str, vector: str) -> bytes:
    from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes

    key = key_bytes(secret)
    iv = key_bytes(vector)
    dec = Cipher(algorithms.AES(key), modes.CBC(iv)).decryptor()
    pt = dec.update(blob) + dec.finalize()
    if pt[:2] != b"\x1f\x8b":
        raise RuntimeError("decrypted inner pack is not gzip")
    import zlib

    return zlib.decompress(pt, 31)


def ensure_unpacker() -> tuple[Path, Path]:
    dest = WORK / "pack200"
    dest.mkdir(parents=True, exist_ok=True)
    jar = dest / "pack200-0.3.0.jar"
    if not jar.exists() or jar.stat().st_size < 1000:
        log(f"download {PACK200_JAR_URL}")
        jar.write_bytes(fetch(PACK200_JAR_URL))
    src = dest / "UnpackPack200.java"
    src.write_text(UNPACK_JAVA)
    cls = dest / "UnpackPack200.class"
    if not cls.exists() or cls.stat().st_mtime < src.stat().st_mtime:
        subprocess.run(
            [str(jdk.javac_bin()), "--class-path", str(jar), "-d", str(dest), str(src)],
            check=True,
        )
    return dest, jar


def unpack_pack200(pack: bytes, dest_jar: Path) -> None:
    classes, lib = ensure_unpacker()
    pack_path = WORK / "inner.pack"
    pack_path.write_bytes(pack)
    if pack[:4] != b"\xca\xfe\xd0\x0d":
        raise RuntimeError(f"not a pack200 stream ({pack[:4].hex()})")
    subprocess.run(
        [
            str(jdk.java_bin()),
            "-cp",
            f"{classes}:{lib}",
            "UnpackPack200",
            str(pack_path),
            str(dest_jar),
        ],
        check=True,
    )


def ensure_clients_html() -> Path:
    html_path = ROOT / "work" / "clients.html"
    if html_path.is_file() and html_path.stat().st_size > 0:
        return html_path
    html_path.parent.mkdir(parents=True, exist_ok=True)
    ab.download(f"{OPENRS2}/clients", html_path)
    return html_path


def openrs2_client(rev: int, date: str) -> tuple[int, bytes]:
    html = ensure_clients_html().read_text(errors="replace")
    rows = parse_clients_html(html)
    cands = [
        r
        for r in rows
        if r["game"] == "runescape"
        and r["type"] == "client"
        and r["format"] == "jar"
        and r["build"] == str(rev)
        and r["date"] == date
        and r["id"] is not None
    ]
    if not cands:
        raise RuntimeError(f"OpenRS2 has no client jar for {rev} {date}")
    cands.sort(key=lambda r: r["timestamp"])
    rec = cands[-1]
    log(f"OpenRS2 client {rec['id']} rev {rev} {rec['timestamp']}")
    return rec["id"], fetch(f"{OPENRS2}/clients/{rec['id']}.dat")


def mirror_file(folder: str, name: str) -> bytes:
    url = f"{MIRROR}/{folder}/{urllib.request.quote(name)}"
    log(f"GET {url}")
    return fetch(url)


def list_mirror(folder: str) -> list[str]:
    html = fetch(f"{MIRROR}/{folder}/").decode("utf-8", "replace")
    return [
        h
        for h in re.findall(r'href="([^"]+)"', html)
        if not h.startswith(("?", "/")) and h not in ("../",) and not h.endswith("/")
    ]


def encrypted_class_jar(folder: str) -> tuple[str, str, bytes, bytes]:
    """Return (jar name, params name, outer jar bytes, unpacked class jar bytes)."""
    names = list_mirror(folder)
    jars = [n for n in names if n.endswith(".jar")]
    params = [n for n in names if n.endswith(".txt")]
    if not jars or not params:
        raise RuntimeError(f"{folder} has jars={len(jars)} params={len(params)}")
    best = None
    for jar_name in jars:
        outer = mirror_file(folder, jar_name)
        with zipfile.ZipFile(io.BytesIO(outer)) as zf:
            gz_name = next(n for n in zf.namelist() if n.endswith(".gz"))
            info = zf.getinfo(gz_name)
            gz = zf.read(gz_name)
        y, m, d, hh, mm, ss = info.date_time
        stamp = f"{y:04d}-{m:02d}-{d:02d} {hh:02d}:{mm:02d}:{ss:02d}"
        for param_name in params:
            text = mirror_file(folder, param_name).decode("utf-8", "replace")
            keys = parse_params(text)
            if "0" not in keys or "-1" not in keys:
                continue
            try:
                pack = decrypt_inner(gz, keys["0"], keys["-1"])
            except Exception as ex:
                log(f"  {jar_name} + {param_name} decrypt failed: {ex}")
                continue
            unpacked = WORK / "unpacked.jar"
            unpack_pack200(pack, unpacked)
            blob = unpacked.read_bytes()
            score = (stamp, len(blob))
            if best is None or score > best[0]:
                best = (score, jar_name, param_name, outer, blob, text)
                log(f"  decrypted {folder}/{jar_name} via {param_name} inner={stamp} classes-month={class_month(blob)}")
    if best is None:
        raise RuntimeError(f"could not decrypt any jar in {folder}")
    _, jar_name, param_name, outer, blob, text = best
    (WORK / "last-params.txt").write_text(text)
    return jar_name, param_name, outer, blob


def profile_for(rev: int) -> str:
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


def cache_home_for(rev: int) -> str:
    if rev >= 400:
        return ".jagex_cache_32/runescape"
    return ".file_store_32"


def install_cache(bdir: Path, cache_id: int | None) -> None:
    if cache_id is None:
        return
    keys = bdir / "cache" / "keys.json"
    ab.download(f"{OPENRS2}/caches/runescape/{cache_id}/keys.json", keys)
    # Size check via a HEAD is unreliable on this host; the known ids are small.
    zpath = bdir / "original" / "cache-disk.zip"
    ab.download(f"{OPENRS2}/caches/runescape/{cache_id}/disk.zip", zpath)
    if zpath.stat().st_size > CACHE_MAX_BYTES:
        log(f"cache {cache_id} is {zpath.stat().st_size} bytes; removing zip")
        zpath.unlink()
        return
    flag = bdir / "cache" / ".extracted"
    if flag.exists():
        return
    with zipfile.ZipFile(zpath) as zf:
        zf.extractall(bdir / "cache")
    inner = bdir / "cache" / "cache"
    if inner.is_dir():
        for p in inner.iterdir():
            target = bdir / "cache" / p.name
            if not target.exists():
                shutil.move(str(p), str(target))
        if inner.exists() and not any(inner.iterdir()):
            inner.rmdir()
    flag.write_text("ok\n")


def write_readme(bdir: Path, entry: dict) -> None:
    cache_line = (
        f"OpenRS2 cache {entry['cache_id']} is in `cache/` and `original/cache-disk.zip`."
        if entry.get("cache_downloaded")
        else "No matching OpenRS2 disk cache was downloaded for this revision."
    )
    (bdir / "README.md").write_text(
        f"""# {entry['family'].upper()} {entry['year']} — revision {entry['rev']}

Last Java client archived for {entry['year']} ({entry['date']}).

{entry['notes']}

{cache_line}

- Source: {entry['source']}
- SHA-256 (`original/gamepack.jar`): `{entry.get('sha256', '')}`

## Layout

- `original/gamepack.jar` — class jar used for deobfuscation
- `deob/gamepack-deob.jar` — LostCity deobfuscator output, when produced
- `src/` — Gradle project with Fernflower-decompiled Java, when produced
- `run.sh` — launch with a private home directory

## Compile

```bash
cd src && ./gradlew compileJava jar
```

## Run

```bash
./run.sh
```
"""
    )


# family, year, rev, date, kind, locator
# kind: openrs2 | inner | encrypted | outer
SPECS = [
    ("rs2", 2004, 282, "2004-12-21", "openrs2", {}),
    ("rs2", 2005, 350, "2005-12-30", "openrs2", {}),
    ("rs2", 2006, 443, "2006-12-11", "openrs2", {}),
    ("rs2", 2007, 481, "2007-12-11", "openrs2", {}),
    ("rs2", 2008, 525, "2008-12-15", "openrs2", {}),
    ("rs2", 2009, 578, "2009-12-16", "openrs2", {}),
    ("rs2", 2010, 634, "2010-12-10", "openrs2", {}),
    ("rs2", 2011, 695, "2011-12-22", "inner", {"folder": "695", "jar": "inner.pack-1.jar"}),
    ("rs2", 2012, 745, "2012-12-07", "inner", {"folder": "745", "jar": "inner.pack-1.jar"}),
    ("rs3", 2013, 790, "2013-12-16", "inner", {"folder": "790", "jar": "inner.pack-1.jar"}),
    (
        "rs3",
        2014,
        831,
        "2014-12-04",
        "outer",
        {"folder": "831", "jar": "-1048777251.jar"},
    ),
    (
        "rs3",
        2015,
        860,
        "2015-12-01",
        "outer",
        {"folder": "860", "jar": "-1062147599.jar"},
    ),
    ("rs3", 2016, 876, "2016-11-28", "encrypted", {"folder": "876-1"}),
    ("rs3", 2017, 887, "2017-11-01", "encrypted", {"folder": "887-1"}),
    ("rs3", 2018, 902, "2018-12-14", "encrypted", {"folder": "902-1"}),
    ("rs3", 2019, 910, "2019-10-16", "encrypted", {"folder": "910-1"}),
]


def build_one(spec) -> None:
    family, year, rev, date, kind, loc = spec
    bdir = BUILDS / family / f"{rev}-{date}"
    gamepack = bdir / "original" / "gamepack.jar"
    if (
        gamepack.is_file()
        and gamepack.stat().st_size > 0
        and os.environ.get("FORCE_DOWNLOAD") != "1"
    ):
        log(f"skip {gamepack.relative_to(ROOT)}: already exists")
        return
    orig = bdir / "original"
    orig.mkdir(parents=True, exist_ok=True)
    cache_id = CACHE_IDS.get(rev)
    client_id = None
    outer_name = None
    param_name = None
    source = ""
    class_jar: bytes | None = None
    outer_jar: bytes | None = None

    if kind == "openrs2":
        client_id, class_jar = openrs2_client(rev, date)
        source = f"{OPENRS2}/clients/{client_id}"
    elif kind == "inner":
        class_jar = mirror_file(loc["folder"], loc["jar"])
        source = f"{MIRROR}/{loc['folder']}/{loc['jar']}"
        month = class_month(class_jar)
        if not month.startswith(str(year)):
            raise RuntimeError(f"rev {rev} class files are {month}, expected {year}")
    elif kind == "encrypted":
        outer_name, param_name, outer_jar, class_jar = encrypted_class_jar(loc["folder"])
        source = f"{MIRROR}/{loc['folder']}/{outer_name} decrypted with {param_name}"
        month = class_month(class_jar)
        if month and not month.startswith(str(year)):
            raise RuntimeError(f"rev {rev} unpacked classes are {month}, expected {year}")
    elif kind == "outer":
        outer_jar = mirror_file(loc["folder"], loc["jar"])
        outer_name = loc["jar"]
        with zipfile.ZipFile(io.BytesIO(outer_jar)) as zf:
            names = set(zf.namelist())
            if "inner.pack.gz" not in names or "parameters.txt" not in names:
                raise RuntimeError(f"rev {rev} outer jar has no inner pack and parameters")
            text = zf.read("parameters.txt").decode("utf-8", "replace")
            gz = zf.read("inner.pack.gz")
        keys = parse_params(text)
        if "0" not in keys or "-1" not in keys:
            raise RuntimeError(f"rev {rev} parameters.txt has no key 0 and -1")
        pack = decrypt_inner(gz, keys["0"], keys["-1"])
        unpacked = WORK / "unpacked.jar"
        unpack_pack200(pack, unpacked)
        class_jar = unpacked.read_bytes()
        param_name = "parameters.txt"
        (WORK / "last-params.txt").write_text(text)
        source = f"{MIRROR}/{loc['folder']}/{outer_name} decrypted with in-jar {param_name}"
        month = class_month(class_jar)
        if month and not month.startswith(str(year)):
            raise RuntimeError(f"rev {rev} unpacked classes are {month}, expected {year}")
        kind = "encrypted"
    else:
        raise RuntimeError(kind)

    if class_jar is not None:
        dest = orig / "gamepack.jar"
        if not dest.exists() or sha256(dest.read_bytes()) != sha256(class_jar):
            dest.write_bytes(class_jar)
        digest = sha256(class_jar)
    else:
        digest = sha256(outer_jar or b"")

    if outer_jar is not None:
        (orig / "gamepack-outer.jar").write_bytes(outer_jar)
        if param_name:
            text = (WORK / "last-params.txt").read_text()
            (orig / "parameters.txt").write_text(text)

    notes = {
        "openrs2": "OpenRS2 live client jar. Last catalogued build of this year.",
        "inner": "Unpacked class jar (inner.pack) from the RuneWiki gamepack mirror. OpenRS2's client catalog ends at build 668.",
        "encrypted": "Outer gamepack from the RuneWiki mirror, decrypted with the archived jav_config parameters (0 and -1) and unpacked from pack200. `original/gamepack.jar` is that class jar. `original/gamepack-outer.jar` is the encrypted download.",
        "outer": "Encrypted outer gamepack. The RuneWiki mirror has no jav_config secret for this revision, so the inner pack is not unpacked and this year is not deobfuscated.",
    }[kind]
    if year == 2016:
        notes += " No December 2016 gamepack is in the mirror; 876 (28 November) is the last one."
    if year == 2017:
        notes += " No December 2017 gamepack is in the mirror; 887 (1 November) is the last one."
    if year == 2019:
        notes += " Java clients were shut down on 18 December 2019. 910 (16 October) is the last 2019 gamepack in the mirror; 912 is February 2020."

    entry = {
        "family": family,
        "year": year,
        "rev": rev,
        "date": date,
        "client_id": client_id,
        "cache_id": cache_id,
        "cache_scope": "runescape",
        "profile": profile_for(rev),
        "remap": None,
        "engine": "old" if rev < 400 else "js5",
        "cache_home": cache_home_for(rev),
        "main_class": "client",
        "run_args": ["10", "0", "highmem", "members", "32"] if rev < 400 else [],
        "emulator": f"{year} year-end Java client",
        "notes": notes,
        "source": source,
        "sha256": digest,
        "cache_downloaded": False,
        "openrs2_client_id": client_id,
        "openrs2_cache_id": cache_id,
        "version": family,
        "kind": kind,
    }
    if cache_id is not None and kind != "outer":
        install_cache(bdir, cache_id)
        entry["cache_downloaded"] = (bdir / "original" / "cache-disk.zip").exists()
    (bdir / "meta.json").write_text(json.dumps(entry, indent=2) + "\n")
    if class_jar is not None:
        ab.write_run_scripts(entry, bdir)
    write_readme(bdir, entry)
    log(f"installed {bdir.relative_to(ROOT)} kind={kind} sha={digest[:12]}")


def main() -> int:
    WORK.mkdir(parents=True, exist_ok=True)
    ensure_clients_html()
    only = {int(a) for a in sys.argv[1:] if a.isdigit()}
    for spec in SPECS:
        if only and spec[2] not in only:
            continue
        build_one(spec)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
