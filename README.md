# RSArchive

Scripts that download, deobfuscate, and decompile yearly RuneScape client snapshots, and RSLib (`rslib/`), the C library for those clients.

This repository is the project only. A clone does not contain gamepacks, caches, decompiled or deobfuscated clients, `original/parameters.txt`, saves, logs, or `work/`. The scripts write those under `builds/` and `work/` on the machine that runs them, and `.gitignore` keeps them out of git. The sources the scripts produce are patched trees, not a clean decompile.

## What this is not

This is not a server, not a playable game by itself, not OSRS, and not a clean-room reimplementation. NXT/C++ clients are out of scope. OSRS is only the placeholder in `builds/osrs/README.md`. RSLib (`rslib/`) is a separate C library; the reproduce pipeline does not build it.

| Year | Rev | Date | Why this snapshot | Typical emulator |
|------|-----|------|-------------------|------------------|
| 2001 | 115 | 2001-12-24 | RSC | Classic 115 |
| 2002 | 159 | 2002-11-25 | RSC | Classic 159 |
| 2003 | 177 | 2003-10-31 | RSC | Classic 177 |
| 2004 | 254 | 2004-09-07 | Client + cache + remap (Lost City is 225) | 2004-era / Lost City-adjacent |
| 2005 | 317 | 2005-06-13 | The standard 2006-era deob | 2006scape / Elvarg, port 43594 |
| 2006 | 377 | 2006-05-02 | Construction, 2006 gameframe | 377 servers |
| 2007 | 468 | 2007-08-08 | August 2007, just pre-GWD | 468 servers |
| 2007 | 474 | 2007-10-19 | GWD, last pre-trade-removal | 474 servers |
| 2008 | 508 | 2008-08-05 | HD + resizable | 508 HD-era servers |
| 2009 | 530 | 2009-01-28 | 2009scape's original target | 2009scape |
| 2010 | 614 | 2010-07-13 | RuneTek 5, Constitution | 614 servers |
| 2011 | 667 | 2011-09-29 | 2011Scape | 2011Scape |
| 2012 | 742 | 2012-11-07 | LostCity 742.1 + OpenRS2 cache (vanilla 742.2 is lost) | 742.1 servers |

The rows above are the emulator milestones RSLib is built around. A second set, `scripts/fetch_year_end_clients.py`, adds the last Java client of each RS2 year (2004–2012) and each RS3 Java year (2013–2019). Those live beside the milestones and are not a replacement for them. NXT/C++ clients are not part of this archive. See `builds/rs3/README.md`.

| Year | Game | Rev | Date | Folder |
|------|------|-----|------|--------|
| 2004 | RS2 | 282 | 2004-12-21 | `builds/rs2/282-2004-12-21` |
| 2005 | RS2 | 350 | 2005-12-30 | `builds/rs2/350-2005-12-30` |
| 2006 | RS2 | 443 | 2006-12-11 | `builds/rs2/443-2006-12-11` |
| 2007 | RS2 | 481 | 2007-12-11 | `builds/rs2/481-2007-12-11` |
| 2008 | RS2 | 525 | 2008-12-15 | `builds/rs2/525-2008-12-15` |
| 2009 | RS2 | 578 | 2009-12-16 | `builds/rs2/578-2009-12-16` |
| 2010 | RS2 | 634 | 2010-12-10 | `builds/rs2/634-2010-12-10` |
| 2011 | RS2 | 695 | 2011-12-22 | `builds/rs2/695-2011-12-22` |
| 2012 | RS2 | 745 | 2012-12-07 | `builds/rs2/745-2012-12-07` |
| 2013 | RS3 | 790 | 2013-12-16 | `builds/rs3/790-2013-12-16` |
| 2014 | RS3 | 831 | 2014-12-04 | `builds/rs3/831-2014-12-04` |
| 2015 | RS3 | 860 | 2015-12-01 | `builds/rs3/860-2015-12-01` |
| 2016 | RS3 | 876 | 2016-11-28 | `builds/rs3/876-2016-11-28` |
| 2017 | RS3 | 887 | 2017-11-01 | `builds/rs3/887-2017-11-01` |
| 2018 | RS3 | 902 | 2018-12-14 | `builds/rs3/902-2018-12-14` |
| 2019 | RS3 | 910 | 2019-10-16 | `builds/rs3/910-2019-10-16` |

831 and 860 are decrypted class jars with deobfuscated bytecode and Gradle sources, not outer-jar-only stubs. Clients and caches come from [OpenRS2 Archive](https://archive.openrs2.org/). Bytecode deobfuscation uses [LostCityRS/Deobfuscator](https://github.com/LostCityRS/Deobfuscator) with [RuneWiki/rs-deob](https://github.com/RuneWiki/rs-deob) profiles and remaps. Decompilation uses the Fernflower submodule, and the jar used by the scripts is `tools/bin/fernflower.jar`.

## Layout

```
builds/<version>/<rev>-<release-date>/
  original/gamepack.jar       vanilla client, when one exists
  original/gamepack-outer.jar encrypted download (RS3 831 and later)
  original/parameters.txt     jav_config block from inside that outer jar
  original/cache-disk.zip     OpenRS2 disk.zip, when a cache was archived
  cache/                      extracted cache (not every folder)
  deob/gamepack-deob.jar      deobfuscated bytecode
  fernflower/                 raw Fernflower output
  src/                        Gradle project (decompiled Java)
  run.sh                      launch against a local home directory
  status.json                 per-build result, including "compiled"
```

`<version>` is the client family — `rsc/`, `rs2/`, `rs3/`, `osrs/`. Each build folder is named `<revision>-<release-date>`, e.g. `rs2/317-2005-06-13`.

742 has an OpenRS2 cache and Lost City sources, and no vanilla gamepack (`original/` is only `cache-disk.zip`). `archive_builds.py` skips that missing gamepack instead of aborting the whole run. Every milestone folder has `cache/`. Year-end 350, 443, 481, 525, 578, and 634 do too. 282, 695, 745, and every RS3 folder have no `cache/`. RSC caches are `.jag` / `.mem`. Early RS2 caches are `dat` plus `idx`. Later ones are `dat2` plus `idx255`, with `keys.json` where OpenRS2 published XTEA keys.

`builds/rs2/742-2012-11-07/src` is the Lost City tree, not a Fernflower dump of a vanilla 742.2 gamepack.

## Tools

One command reproduces the archive:

```bash
pip install -r requirements.txt   # cryptography, for the year-end decrypt
python3 scripts/reproduce.py
```

`reproduce.py` resolves JDK 21 (`JAVA_HOME`, else `/usr/lib/jvm/java-21*`, else `java` on `PATH` if the major version is at least 21). It downloads `https://archive.openrs2.org/clients` into `work/clients.html` if that file is missing, runs `scripts/archive_builds.py` for the milestone years, runs `scripts/fetch_year_end_clients.py` for year-end RS2 and RS3, runs `scripts/deob_all_clients.py` only for clients that do not already have sources, runs `scripts/repair_rs3_831_860.py` only if that repair is still needed, runs `scripts/revert_false_shares.py --apply` to drop class names that disagree with the anchor, compiles clients whose `status.json` `"compiled"` flag is not true, then rewrites `SUMMARY.json` from every `builds/*/*/status.json` using repo-relative paths.

A second run does not delete `src/**/java`. Wiping sources requires `FORCE_SOURCES=1`. `scripts/generify_rs2.py` does not write on a bare run; it needs `--apply`. `reproduce.py` does not call `generify_rs2.py`, `generify_rs3.py`, `generify_rsc.py`, `share_rs*`, `unify_method_names.py`, or `download_java_clients.py`. `download_java_clients.py` is a full OpenRS2/OSRS dump and is not the supported path.

Upstream tools are not stored in this repository. Clone them beside the wrappers, at these commits:

| Path | URL | SHA |
|------|-----|-----|
| `tools/fernflower` | https://github.com/JetBrains/fernflower.git | `032de386b93d1327b4fd022a3420e50c985cd263` |
| `tools/rs-deob` | https://github.com/RuneWiki/rs-deob.git | `2df24dc3693ec2434b2549305d837631dc68fe3d` |

The Lost City 742 sources (`https://github.com/LostCityRS/RS742.git`, `a6b67f4f03dead8b26d30ad8c444b40001749c75`) are a client tree. They are not part of this repository. Local remap edits and renamed 742 sources stay on the machine that produced them.

`tools/bin/fernflower` and `tools/bin/deobfuscator` are the wrappers. Build `tools/bin/fernflower.jar` from the Fernflower checkout, and place Lost City's `Deobfuscator.jar` at `tools/deobfuscator/Deobfuscator.jar`. Neither jar is committed.

```
tools/bin/fernflower        java -jar wrapper
tools/bin/fernflower.jar    built from the Fernflower checkout
tools/deobfuscator/Deobfuscator.jar
```

Rebuild the Fernflower jar after changing that checkout:

```bash
cd tools/fernflower && ./gradlew jar
cp build/libs/fernflower.jar ../bin/fernflower.jar
```

`SUMMARY.json` is generated from every `builds/*/*/status.json`. Paths in it are repo-relative. It is not a hand-maintained status.

## Names

Class names are stable for RSC filenames and for RS2 254 and 317 (`PlayerEntity` extends `PathingEntity`, `Client.login`). They are not stable across later RS2. The same spelling with a different superclass is a bad rename. `scripts/revert_false_shares.py --apply` removes it. Do not edit the anchors: RSC 177, RS2 317, 254, and 377, and RS3 910.

The archive is not fully readable. A Java programmer can read RSC types, but methods are still `call` plus hex. 254 and 317 are readable. 377 types are readable and methods are mostly `op*`. From 443 onward, no. 831 and 860, no. 910 types are mostly named, and methods are still `do` plus digits. Later clients still contain `C` plus 8 hex class names, `classN`, `DataBlockN`, and `opkind` / `valuekind` / `do`+digits method names.

## Compile and run

All 29 `builds/*/*/status.json` files currently have `"compiled": true`. That flag means the patched trees on disk compiled. It does not mean a clean Fernflower run compiles. `scripts/fix_decompiled_java.py` does not clear every Fernflower type error. The extra repair for 831 and 860 is `scripts/repair_rs3_831_860.py`: Twitch types moved to the default package, and jagdx fourcc constants were inlined. Do not treat a fresh decompile as compile-clean.

`reproduce.py` only compiles a client whose `status.json` flag is not true. To compile one tree by hand:

```bash
cd builds/rs2/317-2005-06-13/src
./gradlew compileJava
./gradlew jar
cd ..
# start a matching emulator first (317/377: port 43594)
./run.sh
```

742's Gradle project is `src/client`, and its `run.sh` uses `src/client/build/libs/client.jar`.

`run.sh` points `user.home` at `runtime-home/` and, when `cache/` exists, copies it into the directory that era searches (`.file_store_32` through 377, `.jagex_cache_32/runescape` from 443). It prints `RS_HOST` / `RS_PORT`, which default to `127.0.0.1` and `43594`. Except on 742, it also sets `-Dcom.jagex.config.address` to the host only. It does not patch RSA moduli, JS5 hosts, or world-list URLs. `RS_PORT` is not passed into the client.

`run.sh` launches the `application.mainClass` from that client's `build.gradle`, and that source file is in the tree. RSC is `deob.MudClient`. 254, 317, and 377 are `jagex2.client.Client`. 282, 350, 443, 481, 525, 578, 614, 634, 667, and 695 are `deob.Client`. 468, 474, 508, and 530 are `rt4.Client`. 745 and the RS3 clients that have `AppletViewer.java` launch `AppletViewer`. 831 and 860 launch the default-package `client`. 742 launches `AppletViewer`.

317 and 377 print `RS2 user client - release #N` and load the cache immediately. 474 and later need an external JS5 emulator. Without one they start and then show `error_game_js5connect` (`error_game_` plus `js5connect`), which is expected. Point them at a matching emulator (2009scape for 530, 2011Scape for 667, and so on). After the first launch, search the decompiled sources for `jagex`, `runescape.com`, and `modulus` if login still fails. Do not publish what you find there.

## Legal

The gamepacks, caches, and `original/parameters.txt` files are Jagex's. Those parameter files are the jav_config secrets that shipped inside the outer jar. This repository has no LICENSE that separates the scripts from those jars and caches. The tree is a preservation and research archive for connecting historical clients to community server emulators. Do not redistribute the gamepacks, caches, or parameter files as your own work, and do not publish the secrets.
