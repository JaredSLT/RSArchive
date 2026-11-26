# Revision 790

| | |
| --- | --- |
| Revision | 790 |
| Date | 2013-12-16 |
| Family | rs3 |
| Cache kind | js5 |
| Game class | `rs2.client.Client` (`rs2/client/Client.java:261`), started with revision argument 790 (`Client.java:1322`) |
| Launcher | `AppletViewer` (`AppletViewer.java:12`). `builds/rs3/790-2013-12-16/src/MAIN_CLASS.txt` names that launcher. It hosts `127.0.0.1`. |
| Canvas | Compiled default 765×553 (`Client.java:957` through `Client.java:960`: `tint71` 32, `tint722` 16, `tint73` 765, `tint74` 553). `DataBlock2480` holds those ints (`deob/DataBlock2480.java:16`). `GameShell.startApplet` copies width to `Statics.tint47` and height to `Statics.tint60` (`com/jagex/game/client/GameShell.java:107`). A defaults-blob override is recorded in [gaps.md](gaps.md). |
| Nested git | This revision's source tree has no nested git SHA. |

`builds/rs3/790-2013-12-16/meta.json` records family `rs3`, revision 790, date `2013-12-16`, engine `js5`, cache home `.jagex_cache_32/runescape`, and `cache_downloaded` false.

## Tick

`Client.doClient2` (`Client.java:1429`) is the cycle, and it returns immediately when `tint72` is 1.

1. When `tint72` is 0 or `doClient222` or `doClient2222b7`, and `Statics.sparseVarDomain` is present, it ticks that domain (`Client.java:1432`).
2. `++tint9` (`Client.java:1451`). When `tint9 % 1000 == 1`, the client reseeds `random` from the clock (`Client.java:1452`).
3. Both sockets run `doPureJavaToolkitFactory22`, which updates byte counters every 50 cycles (`Client.java:1458`, `PureJavaToolkitFactory.java:107`).
4. `doClient2222` runs the JS5 master tick (`Client.java:1460`, `Client.java:1937`).
5. `sparseVarDomain.doSparseVarDomain2222` when the domain is present (`Client.java:1465`).
6. The keyboard-like `DataBlock2188` queue and the mouse `DataBlock2413` queue are drained (`Client.java:1479`, `Client.java:1514`).
7. State dispatch (`Client.java:1536`). The predicates are `doClient` (`Client.java:1835`), `doClient22` (`Client.java:1845`), `doClient222` (`Client.java:1850`), `doClient2222b7` (`Client.java:1860`), and `doClient2222b4` (`Client.java:1870`):
   - `doClient` is 12, 4, or 0, and runs `MapElementType.doMapElementType22` and `GameShell.doGameShell232`.
   - `doClient22` is 8, 10, 17, 19, or 7, and runs `clse1a06052.staticelements()` (`Client.java:1540`).
   - `doClient222` is 2, 8, 14, 11, 16, 18, or 7. Outside a rebuild it runs `doClient2222b14`, `WorldMapRenderer.doWorldMapRenderer2222b6`, and `Statics.jagtheora`.
   - `doClient2222b7` is 5, 10, 15, or 19. Outside a rebuild it runs `doClient2222b14` and `Statics.jagtheora`.
   - `tint72 == 13` runs `Statics.jagtheora`.
   - `doClient2222b4` is 9, 17, or 13. Outside a rebuild it runs `doClient2222b192`. Rebuild states and state 13 take the earlier branches, so the 100-packet pump runs for `tint72 == 9`.
   - `tint72` 3 or 6 is the logout path (`Client.java:1554`).
8. `Friend.doFriend` (`Client.java:1574`).

`doClient2222b192` (`Client.java:3195`) reads up to 100 server packets with `doClient2(pureJavaToolkitFactory2)` (`Client.java:3209`). At the end of that method the game socket flushes queued outbound bytes with `pureJavaToolkitFactory2.doPureJavaToolkitFactory2` (`Client.java:3540`). The login-state path flushes `pureJavaToolkitFactory` the same way (`Client.java:3144`).

## Pages

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
- [files.tsv](files.tsv)

## Status

| Subsystem | Status |
| --- | --- |
| framing | specified |
| login | partial |
| packets | partial |
| sync | partial |
| region | specified |
| cache | partial |
| input | partial |
| interfaces | partial |
| scene | gap |
| sound | gap |
