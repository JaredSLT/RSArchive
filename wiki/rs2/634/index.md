# Revision 634

| | |
| --- | --- |
| Family | rs2 |
| Date | 2010-12-10 |
| Revision | 634 |
| Main class | `client` (`deob.Client`) |
| Canvas | 765×503 when the game id is `C4d106383.valuekind15924`; 640×480 when it is `C8daec638.valuekind67911` |
| Cache | js5 |
| Source | `builds/rs2/634-2010-12-10/src/src/main/java/` |
| OpenRS2 | client 31071, cache 975, jar sha256 `505922f76ab2a76b1e6fdd9758522746bd8b1b8efebee1f835f1ff6691dae6fa` |

`meta.json` records the OpenRS2 jar. This checkout has no nested git SHA for the deob tree.

Build string `Build: 634` is drawn by `Build.opkind16333` at builds/rs2/634-2010-12-10/src/src/main/java/deob/Build.java:192. `Client` passes revision 634 into `ProjAnim.call680fcd48` at builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2063. Canvas assignment is builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2052 through builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2059.

## Tick

`ProjAnim.run` (builds/rs2/634-2010-12-10/src/src/main/java/deob/ProjAnim.java:430) opens the disk cache (`Client.opClient228`, builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:1203), then loops while the client is alive:

1. `MouseLogger.valueMouseLogger32` logic steps. Each step is `opProjAnim26` (builds/rs2/634-2010-12-10/src/src/main/java/deob/ProjAnim.java:165), which calls `Client.opClient237` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:1900), which calls `Client.opClient241` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2275).
2. One draw step `opProjAnim29` (builds/rs2/634-2010-12-10/src/src/main/java/deob/ProjAnim.java:268), which calls `Client.opClient239` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2067), which calls `Client.calle71babc1` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2409).

`opClient241` increments the cycle, pumps on-demand (`opClient235`), polls keys (`Cdba8e8ea.valuekind11355.opkind16781`) and the mouse (`Cf5ea55c9.valuekind12991.opkind16197`), then dispatches on `C72191dbb.valuekind12333` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2291). `C42efb241.opkind12292` is states 3, 4, 5, and 6 (builds/rs2/634-2010-12-10/src/src/main/java/deob/C42efb241.java:23). `ProgramDefineData.opProgramDefineData6` is states 7, 8, and 9 (builds/rs2/634-2010-12-10/src/src/main/java/deob/ProgramDefineData.java:28). `Cbafceea7.opkind15853` is states 4, 8, and 11 (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cbafceea7.java:29). `Caf11c2b7.opkind15267` is states 10, 11, and 12 (builds/rs2/634-2010-12-10/src/src/main/java/deob/Caf11c2b7.java:87). At builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2365 the dispatch is:

- states 3, 5, and 6: `callad91983c`, `C084abe8f.opkind12644`, and `LswprPos.calla978325d`
- states 7 and 9: `callad91983c` and `LswprPos.calla978325d`
- state 12: `LswprPos.calla978325d`
- state 10: `Cafcd2a80.opkind11591` (body unread)
- state 13: `LswprPos.calla978325d`, then `C611b2b12.opkind11882` unless `HashMap.valueHashMap16` is -3 or 15 (that callee was not read)

States 4, 8, and 11 are excluded by `opkind15853`, so this dispatch does not call the login state machine for them.

`callad91983c` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2087) runs the in-game tick when the client state is 7 and the login state is 0: up to 100 calls of `C802d90122.opkind13153` (the packet switch), interface and NPC wander work, three widget transmit queues into `Unused3.opUnused319`, and a keepalive after 50 idle cycles.

## Pages

- [login.md](login.md)
- [framing.md](framing.md)
- [packets.md](packets.md)
- [sync.md](sync.md)
- [region.md](region.md)
- [cache.md](cache.md)
- [input.md](input.md)
- [scene.md](scene.md)
- [interfaces.md](interfaces.md)
- [sound.md](sound.md)
- [gaps.md](gaps.md)
- [files.tsv](files.tsv)

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
| scene | partial |
| interfaces | partial |
| sound | partial |

Login, player movement, player masks, appearance, NPC add/remove, region, walk, minimap, the click packet, keepalive, the obj config opcode switch, and the sound-start packet are field-level. NPC step dx/dz, most packet handlers, other config types, the widget script tail, and the software raster loop are in [gaps.md](gaps.md).
