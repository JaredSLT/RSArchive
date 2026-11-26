# Region

Static rebuild is server opcode 66, size `-2` (`DataBlock1193.cls2789b62767` at `builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1193.java:138`). `Statics.doStatics3` copies the payload into a `PositionedSoundPart4` and calls `World.doWorld` with `RebuildStage.rebuildStage3` (`deob/Statics.java:5382`). That stage selects `World.doWorld(PositionedSoundPart4)` (`com/jagex/game/scene/World.java:398` and `:458`).

Reads at `World.java:459`:

| Order | Width | Use |
| --- | --- | --- |
| 1 | u16be | zone Z |
| 2 | u8 | map-size id, `MapAlignmentY.doMapAlignmentY` (`com/jagex/game/client/MapAlignmentY.java:36`) |
| 3 | u16le | zone X |
| 4 | u8add128 | `1` is the force flag |
| 5 | u8add128 | length of the map-square arrays |
| 6 | u8add128 | `World.tint13`, the NPC view-distance bit count |

The square loop treats zone X as the outer coordinate and zone Z as the inner (`World.java:479`). Map x runs from `(zoneX - (tiles >> 4)) / 8` through `(zoneX + (tiles >> 4)) / 8`, and map z uses the same window on zone Z. `mapSquareGroupId(x, z)` is `x | (z << 7)` (`World.java:693`). A square is kept when `Statics.js5.doJs5(group, 3)` is true. File type 3 is `DataBlock1473.cls88d855f04` (`deob/DataBlock1473.java:12`). `Statics.js5` is JS5 index 5 (`RawCpuInfo.rawCpuInfo5` at `com/jagex/game/client/RawCpuInfo.java:16`, opened at `com/jagex/graphics/camera/CameraSettingType.java:337`). `World.tint12` is how many of the allocated slots that test filled (`World.java:489`).

`doWorld(zoneX, zoneZ, 10, force)` (`World.java:490`) sets the scene base to `DataBlock1375(0, (zoneX - (tiles >> 4)) * 8, (zoneZ - (tiles >> 4)) * 8)` (`World.java:754`). `getBase` returns that object (`World.java:176`). Map size 0 has 104 tiles, so `tiles >> 4` is 6 and the base is `(zone - 6) * 8`.

Map-size ids (`MapAlignmentY.java:8`):

| Id | Tiles |
| --- | --- |
| 0 | 104 |
| 1 | 120 |
| 2 | 136 |
| 3 | 168 |
| 4 | 72 |
| 5 | 256 |

Dynamic rebuild is server opcode 173, size `-2` (`DataBlock1193.java:352`). The switch passes `RebuildStage.rebuildStage4` (`Statics.java:5739`). `doRebuildStage()` is the first constructor flag, and it is true for `rebuildStage4` (`com/jagex/game/client/RebuildStage.java:14` and `:35`), so `World.doWorld(DataBlock1776)` calls `doWorld2` (`World.java:401`).

Reads at `World.java:495`:

| Order | Width | Use |
| --- | --- | --- |
| 1 | u8add128 | `1`..`4` select `rebuildStage4` through `rebuildStage7` |
| 2 | u16be | zone Z |
| 3 | u8add128 | map-size id |
| 4 | u16le+128 | zone X |
| 5 | u8 | bit 0 is the force flag |
| 6 | u8sub128 | `World.tint13` |

The buffer is then bit-aligned. For 4 planes and `(tiles >> 3)` by `(tiles >> 3)` chunks, one bit says the chunk is present. A set bit is followed by 26 bits (`World.java:520`). From those bits, `(value >> 14) & 1023` and `(value >> 3) & 2047` are shifted by 3 again to form the map square (`World.java:529`). The cursor returns to a byte boundary (`World.java:538`). Unique squares that exist in JS5 index 5, file type 3, are kept. The origin call is `doWorld(zoneX, zoneZ, 10, force)` (`World.java:580`), the same base formula as the static packet.

Neither `doWorld(PositionedSoundPart4)` nor `doWorld2` calls the XTEA methods. `doWorld23` and `doWorld232` in the same class are separate rebuild paths and are not opcodes 66 or 173.

`rslib/src/both/move/rev_876.c` uses the same zone widths for these two packets. There is no disagreement row.
