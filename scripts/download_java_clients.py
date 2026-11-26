#!/usr/bin/env python3
"""Download every available Java game client into builds/.

Sources:
  - OpenRS2 Archive (classic + RS2 gamepack jars, plus pack200-only builds)
  - runetech/osrs-gamepacks (OSRS Java applets, revisions 1–235)

Caches are not downloaded here: OpenRS2's full disk-store set is over a
terabyte. Each folder gets a meta.json with the best matching cache id.
Existing yearly snapshots keep their gamepack, deob, and source trees.
"""

from __future__ import annotations

import hashlib
import json
import os
import re
import shutil
import subprocess
import tarfile
import zipfile
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor, as_completed
from html.parser import HTMLParser
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILDS = ROOT / "builds"
WORK = ROOT / "work"
STAGING = WORK / "openrs2-clients"
OPENRS2 = "https://archive.openrs2.org"
OSRS_TARBALL = "https://codeload.github.com/runetech/osrs-gamepacks/tar.gz/refs/heads/master"
WORKERS = 8


def log(msg: str) -> None:
    print(msg, flush=True)


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as fh:
        for chunk in iter(lambda: fh.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def download(url: str, dest: Path) -> None:
    dest.parent.mkdir(parents=True, exist_ok=True)
    if dest.exists() and dest.stat().st_size > 0:
        return
    tmp = dest.with_suffix(dest.suffix + ".part")
    subprocess.run(
        [
            "curl",
            "-fsSL",
            "--retry",
            "5",
            "--retry-all-errors",
            "-C",
            "-",
            "-o",
            str(tmp),
            url,
        ],
        check=True,
    )
    tmp.replace(dest)


class ClientsTable(HTMLParser):
    def __init__(self) -> None:
        super().__init__()
        self.in_tbody = False
        self.in_tr = False
        self.in_td = False
        self.cur_row: list[str] = []
        self.cur_cell: list[str] = []
        self.cur_hrefs: list[str] = []
        self.td_hrefs: list[list[str]] = []
        self.rows: list[tuple[list[str], list[list[str]]]] = []

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if tag == "tbody":
            self.in_tbody = True
        elif self.in_tbody and tag == "tr":
            self.in_tr = True
            self.cur_row = []
            self.td_hrefs = []
        elif self.in_tr and tag == "td":
            self.in_td = True
            self.cur_cell = []
            self.cur_hrefs = []
        elif self.in_td and tag == "a" and "href" in attrs:
            self.cur_hrefs.append(attrs["href"])

    def handle_endtag(self, tag):
        if tag == "tbody":
            self.in_tbody = False
        elif tag == "tr" and self.in_tr:
            self.in_tr = False
            self.rows.append((self.cur_row, self.td_hrefs))
        elif tag == "td" and self.in_td:
            self.in_td = False
            self.cur_row.append(" ".join("".join(self.cur_cell).split()))
            self.td_hrefs.append(list(self.cur_hrefs))

    def handle_data(self, data):
        if self.in_td:
            self.cur_cell.append(data)


def parse_clients_html(html: str) -> list[dict]:
    p = ClientsTable()
    p.feed(html)
    out = []
    for cells, hrefs in p.rows:
        if len(cells) < 10:
            continue
        game, env, build, ts, typ, fmt, os_, arch, jvm, size = cells[:10]
        hrefs_flat = [h for hs in hrefs for h in hs]
        cid = None
        for h in hrefs_flat:
            m = re.search(r"/clients/(\d+)", h)
            if m:
                cid = int(m.group(1))
                break
        date = ts.split()[0] if ts else "unknown"
        out.append(
            {
                "game": game,
                "environment": env,
                "build": build.strip(),
                "timestamp": ts,
                "date": date,
                "type": typ,
                "format": fmt,
                "os": os_,
                "arch": arch,
                "jvm": jvm,
                "size": size,
                "id": cid,
            }
        )
    return out


def family_for(game: str, build: str) -> str:
    if game == "classic":
        return "rsc"
    if game == "oldschool":
        return "osrs"
    if game == "runescape":
        try:
            rev = int(build)
        except ValueError:
            return "rs2"
        if rev >= 750:
            return "rs3"
        return "rs2"
    return game


def jar_mtime(path: Path) -> str | None:
    try:
        with zipfile.ZipFile(path) as zf:
            times = [zf.getinfo(n).date_time for n in zf.namelist() if zf.getinfo(n).date_time[0] >= 1980]
        if not times:
            return None
        y, m, d, *_ = max(times)
        return f"{y:04d}-{m:02d}-{d:02d}"
    except zipfile.BadZipFile:
        return None


def best_cache(caches: list[dict], game: str, rev: int | None, date: str) -> dict | None:
    if rev is None:
        return None
    scope_game = "classic" if game == "classic" else game
    cands = []
    for c in caches:
        if c.get("game") != scope_game and not (
            game == "runescape" and c.get("game") == "runescape"
        ):
            continue
        majors = [b.get("major") for b in (c.get("builds") or []) if b.get("major") is not None]
        if rev in majors:
            cands.append(c)
    if not cands:
        return None
    def score(c):
        ts = c.get("timestamp") or ""
        return abs((ts[:10] > date) - (ts[:10] < date)), ts
    cands.sort(key=lambda c: (c.get("timestamp") or "9999", -(c.get("valid_groups") or 0)))
    # Prefer a cache whose timestamp is on/after the client date, else nearest.
    after = [c for c in cands if (c.get("timestamp") or "")[:10] >= date]
    pool = after or cands
    return pool[0]


def write_readme(bdir: Path, rec: dict) -> None:
    variants = rec.get("variants") or []
    var_txt = ""
    if variants:
        var_txt = "\nOther OpenRS2 copies of this build (different bytes) are in `original/variants/`.\n"
    (bdir / "README.md").write_text(
        f"""# {rec['family']} {rec.get('rev', '?')} ({rec.get('date', '')})

Java gamepack from {rec.get('source', 'OpenRS2')}.

- Game: {rec.get('game')}
- Timestamp: {rec.get('timestamp', rec.get('date', ''))}
- OpenRS2 client id: {rec.get('openrs2_client_id', 'n/a')}
- SHA-256: `{rec.get('sha256', '')}`
- Matching OpenRS2 cache id: {rec.get('openrs2_cache_id', 'none (not downloaded)')}
{var_txt}
Original jar: `original/gamepack.jar`
"""
    )


def install_openrs2_clients(rows: list[dict], caches: list[dict]) -> list[dict]:
    jars = [
        r
        for r in rows
        if r["type"] == "client"
        and r["format"] == "jar"
        and r["game"] in ("classic", "runescape")
        and r["id"] is not None
    ]
    pack_only_builds = set()
    jar_builds = {
        (r["game"], int(r["build"]))
        for r in jars
        if r["build"].isdigit()
    }
    packs = [
        r
        for r in rows
        if r["type"] == "client"
        and r["format"] == "pack200"
        and r["game"] == "runescape"
        and r["build"].isdigit()
        and r["id"] is not None
        and (r["game"], int(r["build"])) not in jar_builds
    ]
    log(f"OpenRS2 jar clients: {len(jars)}; pack200-only builds: {len(packs)}")

    STAGING.mkdir(parents=True, exist_ok=True)
    to_fetch = jars + packs

    def fetch_one(r):
        dest = STAGING / f"{r['id']}.{'pack200' if r['format']=='pack200' else 'jar'}"
        url = f"{OPENRS2}/clients/{r['id']}.dat"
        try:
            download(url, dest)
            return r["id"], dest, None
        except Exception as e:
            return r["id"], dest, str(e)

    errors = []
    done = 0
    with ThreadPoolExecutor(max_workers=WORKERS) as pool:
        futs = [pool.submit(fetch_one, r) for r in to_fetch]
        for fut in as_completed(futs):
            cid, dest, err = fut.result()
            done += 1
            if err:
                errors.append((cid, err))
                log(f"FAIL client {cid}: {err}")
            elif done % 25 == 0 or done == len(to_fetch):
                log(f"downloaded {done}/{len(to_fetch)}")

    # Group by family/rev/date; pick largest unique hash as canonical.
    groups: dict[tuple, list] = defaultdict(list)
    catalog = []
    for r in to_fetch:
        ext = "pack200" if r["format"] == "pack200" else "jar"
        path = STAGING / f"{r['id']}.{ext}"
        if not path.exists() or path.stat().st_size == 0:
            continue
        digest = sha256_file(path)
        r = dict(r)
        r["sha256"] = digest
        r["bytes"] = path.stat().st_size
        r["family"] = family_for(r["game"], r["build"])
        r["rev"] = int(r["build"]) if r["build"].isdigit() else None
        key = (r["family"], r["rev"] if r["rev"] is not None else r["id"], r["date"])
        groups[key].append((r, path))

    installed = []
    for key, items in sorted(groups.items(), key=lambda kv: (kv[0][0], kv[0][1] or 0, kv[0][2])):
        family, rev, date = key
        # unique hashes, largest first
        by_hash = {}
        for rec, path in items:
            prev = by_hash.get(rec["sha256"])
            if prev is None or rec["bytes"] > prev[0]["bytes"]:
                by_hash[rec["sha256"]] = (rec, path)
            else:
                # keep id on the existing record
                prev[0].setdefault("duplicate_ids", []).append(rec["id"])
        unique = sorted(by_hash.values(), key=lambda x: -x[0]["bytes"])
        primary, primary_path = unique[0]
        folder_rev = rev if rev is not None else f"id{primary['id']}"
        bdir = BUILDS / family / f"{folder_rev}-{date}"
        orig = bdir / "original"
        orig.mkdir(parents=True, exist_ok=True)
        dest_jar = orig / ("gamepack.pack200" if primary["format"] == "pack200" else "gamepack.jar")
        existing = dest_jar.exists() and dest_jar.stat().st_size > 0
        if not existing:
            shutil.copy2(primary_path, dest_jar)
        elif sha256_file(dest_jar) != primary["sha256"]:
            # keep the already-archived yearly snapshot; store OpenRS2 copy as variant
            vdir = orig / "variants"
            vdir.mkdir(exist_ok=True)
            shutil.copy2(primary_path, vdir / f"{primary['id']}.{dest_jar.suffix.lstrip('.')}")

        variant_ids = []
        for rec, path in unique[1:]:
            vdir = orig / "variants"
            vdir.mkdir(exist_ok=True)
            vname = f"{rec['id']}.{'pack200' if rec['format']=='pack200' else 'jar'}"
            if not (vdir / vname).exists():
                shutil.copy2(path, vdir / vname)
            variant_ids.append({"id": rec["id"], "sha256": rec["sha256"], "bytes": rec["bytes"]})

        cache = None
        if primary["rev"] is not None:
            cache = best_cache(caches, primary["game"], primary["rev"], date)

        all_ids = [primary["id"]] + primary.get("duplicate_ids", [])
        meta = {
            "family": family,
            "game": primary["game"],
            "rev": primary["rev"],
            "date": date,
            "timestamp": primary["timestamp"],
            "source": "openrs2",
            "openrs2_client_id": primary["id"],
            "openrs2_client_ids": all_ids,
            "openrs2_cache_id": cache["id"] if cache else None,
            "openrs2_cache_scope": cache.get("scope") if cache else None,
            "format": primary["format"],
            "sha256": primary["sha256"],
            "bytes": primary["bytes"],
            "variants": variant_ids,
            "environment": primary["environment"],
        }
        meta_path = bdir / "meta.json"
        if meta_path.exists():
            try:
                old = json.loads(meta_path.read_text())
            except json.JSONDecodeError:
                old = {}
            old.setdefault("openrs2_client_id", meta["openrs2_client_id"])
            old.setdefault("openrs2_client_ids", meta["openrs2_client_ids"])
            old.setdefault("sha256", meta["sha256"])
            old["openrs2_extra_ids"] = meta["openrs2_client_ids"]
            old["variants"] = meta["variants"]
            meta_path.write_text(json.dumps(old, indent=2) + "\n")
        else:
            meta_path.write_text(json.dumps(meta, indent=2) + "\n")
            write_readme(bdir, meta)
        installed.append(meta)

    log(f"installed {len(installed)} OpenRS2 client folders; {len(errors)} download errors")
    return installed, errors


def install_osrs() -> list[dict]:
    tarball = WORK / "osrs-gamepacks.tar.gz"
    extract = WORK / "osrs-gamepacks"
    log("downloading runetech/osrs-gamepacks snapshot")
    download(OSRS_TARBALL, tarball)
    if extract.exists():
        shutil.rmtree(extract)
    extract.mkdir(parents=True)
    with tarfile.open(tarball, "r:gz") as tf:
        tf.extractall(extract)
    pack_dirs = list(extract.glob("*/gamepacks"))
    if not pack_dirs:
        raise RuntimeError("osrs-gamepacks tarball had no gamepacks/")
    pack_dir = pack_dirs[0]
    installed = []
    for jar in sorted(pack_dir.glob("osrs-*.jar")):
        m = re.match(r"osrs-(-?\d+)\.jar$", jar.name)
        if not m:
            continue
        rev = int(m.group(1))
        date = jar_mtime(jar) or "unknown"
        folder = f"{rev}-{date}" if rev >= 0 else f"farewell-{date}"
        bdir = BUILDS / "osrs" / folder
        orig = bdir / "original"
        orig.mkdir(parents=True, exist_ok=True)
        dest = orig / "gamepack.jar"
        if not (dest.exists() and dest.stat().st_size > 0):
            shutil.copy2(jar, dest)
        digest = sha256_file(dest)
        meta = {
            "family": "osrs",
            "game": "oldschool",
            "rev": rev,
            "date": date,
            "source": "runetech/osrs-gamepacks",
            "sha256": digest,
            "bytes": dest.stat().st_size,
        }
        if not (bdir / "meta.json").exists():
            (bdir / "meta.json").write_text(json.dumps(meta, indent=2) + "\n")
            write_readme(bdir, meta)
        installed.append(meta)
    log(f"installed {len(installed)} OSRS gamepacks")
    return installed


def main() -> int:
    WORK.mkdir(exist_ok=True)
    html_path = WORK / "clients.html"
    log("fetching OpenRS2 clients catalog")
    download(f"{OPENRS2}/clients", html_path)
    rows = parse_clients_html(html_path.read_text(errors="ignore"))
    log(f"catalog rows: {len(rows)}")

    caches = []
    caches_path = WORK / "caches.json"
    if caches_path.exists() and caches_path.stat().st_size > 0:
        caches = json.loads(caches_path.read_text())

    openrs2, errors = install_openrs2_clients(rows, caches)
    osrs = install_osrs()

    summary = {
        "openrs2_folders": len(openrs2),
        "openrs2_errors": [{"id": i, "error": e} for i, e in errors],
        "osrs_folders": len(osrs),
        "families": {
            fam: sum(1 for r in openrs2 + osrs if r["family"] == fam)
            for fam in ("rsc", "rs2", "rs3", "osrs")
        },
    }
    (WORK / "java-clients-summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    (WORK / "java-clients.json").write_text(json.dumps(openrs2 + osrs, indent=2) + "\n")
    log(json.dumps(summary, indent=2))
    return 1 if errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
