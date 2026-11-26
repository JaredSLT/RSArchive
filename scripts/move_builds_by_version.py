#!/usr/bin/env python3
"""One-shot move of builds/<year>-<rev> to builds/<version>/<rev>-<release-date>.

Version is derived from each build's profile prefix (rsc/rs2/rs3/osrs).
Release dates come from the archive's own OpenRS2 client index (work/clients.html)
and the per-build meta.json; nothing is guessed.
"""
import os
import shutil
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILDS = os.path.join(ROOT, "builds")

# old flat dir -> (version, rev, release-date)
PLAN = {
    "2001-115": ("rsc", "115", "2001-12-24"),
    "2002-159": ("rsc", "159", "2002-11-25"),
    "2003-177": ("rsc", "177", "2003-10-31"),
    "2004-254": ("rs2", "254", "2004-09-07"),
    "2005-317": ("rs2", "317", "2005-06-13"),
    "2006-377": ("rs2", "377", "2006-05-02"),
    "2007-468": ("rs2", "468", "2007-08-08"),
    "2007-474": ("rs2", "474", "2007-10-19"),
    "2008-508": ("rs2", "508", "2008-08-05"),
    "2009-530": ("rs2", "530", "2009-01-28"),
    "2010-614": ("rs2", "614", "2010-07-13"),
    "2011-667": ("rs2", "667", "2011-09-29"),
    "2012-742": ("rs2", "742", "2012-11-07"),
}

# Version slots with no builds yet (kept so the tree starts at the version).
EMPTY_VERSIONS = ("rs3", "osrs")


def new_name(version, rev, date):
    return f"{rev}-{date}"


def main():
    # 1) Validate every source exists before touching anything.
    for old, (ver, rev, date) in PLAN.items():
        src = os.path.join(BUILDS, old)
        if not os.path.isdir(src):
            sys.exit(f"missing source dir: {src}")
    # 2) No target collisions.
    for old, (ver, rev, date) in PLAN.items():
        dst = os.path.join(BUILDS, ver, new_name(ver, rev, date))
        if os.path.exists(dst):
            sys.exit(f"target already exists: {dst}")

    # 3) Create version parent dirs (and placeholder dirs for empty versions).
    for ver in set(v for v, _, _ in PLAN.values()):
        os.makedirs(os.path.join(BUILDS, ver), exist_ok=True)
    for ver in EMPTY_VERSIONS:
        d = os.path.join(BUILDS, ver)
        os.makedirs(d, exist_ok=True)
        keep = os.path.join(d, "README.md")
        with open(keep, "w") as fh:
            fh.write(f"# {ver.upper()}\n\nNo builds archived yet. Reserved version slot.\n")

    # 4) Move each build (rename on the same filesystem -> no data copy).
    for old, (ver, rev, date) in PLAN.items():
        src = os.path.join(BUILDS, old)
        dst = os.path.join(BUILDS, ver, new_name(ver, rev, date))
        os.rename(src, dst)
        print(f"moved  builds/{old}  ->  builds/{ver}/{new_name(ver, rev, date)}")

    # 5) Rewrite the absolute deob_jar path inside each moved status.json
    #    so the per-build status stays accurate in the new layout.
    for old, (ver, rev, date) in PLAN.items():
        leaf = new_name(ver, rev, date)
        s = os.path.join(BUILDS, ver, leaf, "status.json")
        if os.path.exists(s):
            t = open(s).read()
            t2 = t.replace(f"builds/{old}/", f"builds/{ver}/{leaf}/")
            if t2 != t:
                open(s, "w").write(t2)
                print(f"patched status.json paths in builds/{ver}/{leaf}")

    # 6) Fix the last-run artifact SUMMARY.json (absolute path reference).
    summary = os.path.join(ROOT, "SUMMARY.json")
    if os.path.exists(summary):
        t = open(summary).read()
        changed = t
        for old, (ver, rev, date) in PLAN.items():
            leaf = new_name(ver, rev, date)
            changed = changed.replace(f"builds/{old}/", f"builds/{ver}/{leaf}/")
        if changed != t:
            open(summary, "w").write(changed)
            print("patched SUMMARY.json paths")

    print("DONE")


if __name__ == "__main__":
    main()
