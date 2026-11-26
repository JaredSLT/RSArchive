# Revision 876

| | |
| --- | --- |
| Family | rs3 |
| Date | 2016-11-28 |
| Main class | `rs2.client.Client` (obfuscated name `client`). `AppletViewer` is the jar entry. `Client.init` passes revision 876 to `startApplet` (`builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:1400`). |
| Canvas | 765 by 553 (`Client.tint74`, `Client.tint75` at `rs2/client/Client.java:943`). When `doClient3222b29()` returns 1, those values are copied into `Statics.tint39` and `Statics.tint53` (`rs2/client/Client.java:2593`). |
| Cache | js5. `meta.json` records home `.jagex_cache_32/runescape`, engine `js5`, and `cache_downloaded` false. |
| Gamepack | sha256 `840243a7dcf53e8b85008790020420101b6a86f250498af35fae2ee0f9fc932b` |

Source root for every citation: `builds/rs3/876-2016-11-28/src/src/main/java`. The sibling `fernflower/` tree was not used. Decompiled names are the names in that source.

Tick: `GameShell.tlong` is `20000000` nanoseconds (`com/jagex/game/client/GameShell.java:47`). The loop in `run_inner` asks `doDataBlock1492(tlong)` how many cycles are due, calls `doGameShell2222` that many times, then calls `doGameShell3` once (`GameShell.java:641`). `doGameShell2222` calls `Client.doClient22` (`GameShell.java:684`). `doGameShell3` calls `Client.doClient222` (`GameShell.java:713`).

`Client.doClient22` (`rs2/client/Client.java:1597`): states 16, 7, 1, 3, 11, 13, and 5 call the lobby handshake `Statics.doStatics3222b9` and `BlankWs.blankWs`. States 0, 14, 8, and 6 call `BlankWs.blankWs`. State 4 calls `BlankWs.blankWs` and does not enter the game pump. States 9 and 10 call `doClient3222b122`, which reads up to 100 server packets on `Client.pureJavaToolkitContext2` (`rs2/client/Client.java:3209`).

## Pages

- [files.tsv](files.tsv)
- [login.md](login.md)
- [framing.md](framing.md)
- [packets.md](packets.md)
- [sync.md](sync.md)
- [region.md](region.md)
- [cache.md](cache.md)
- [input.md](input.md)
- [interfaces.md](interfaces.md)
- [scene.md](scene.md)
- [sound.md](sound.md)
- [gaps.md](gaps.md)

## Status

| Subsystem | Status |
| --- | --- |
| login | partial |
| framing | specified |
| packets | partial |
| sync | partial |
| region | specified |
| cache | partial |
| input | partial |
| interfaces | partial |
| scene | partial |
| sound | partial |
