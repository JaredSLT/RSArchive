# Region

`RebuildStage.doRebuildStage(DataBlock123)` (`com/jagex/game/client/RebuildStage.java:387`) picks the reader from `DataBlock241`:

| Value | `tboolean` | Reader |
| --- | --- | --- |
| `cls86419faa` | false | `doRebuildStage2222b25` local, no packet (`:402`) |
| `cls86419faa3` | false | static packet `doRebuildStage(WorldMapElementPart)` (`:452`) |
| `cls86419faa2` | true, checked first | cutscene `doRebuildStage2222b27` from queued records, not from leftover packet bytes (`:578`) |
| `doDataBlock119()` true | | dynamic `doRebuildStage2` (`:488`) |

`doDataBlock119` returns the first constructor flag (`deob/DataBlock241.java:33`). `cls86419faa4` through `cls86419faa7` are true. Server packet opcode 49 (`DataBlock457.clsfbe8a09950`, `Client.java:7993`) passes `cls86419faa4`. Opcode 93 (`clsfbe8a09994`, `Client.java:8727`) passes `cls86419faa3`.

Neither packet reader reads XTEA keys. The key source is unread (`gaps.md`).

## Static opcode 93

`RebuildStage.java:452`:

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | u8 | build-area id |
| 2 | u16be | zone x |
| 3 | u8(128-b) | stored as `tint13`, the NPC coordinate bit width |
| 4 | u16be | zone z |
| 5 | u8(0-b) | force when the value is 1 |
| 6 | u8(128-b) | square count used to size the arrays |

`NPCTypeFactory` maps the build-area id to tiles (`com/jagex/game/config/npctype/NPCTypeFactory.java:8`): 0 is 104, 1 is 120, 2 is 136, 3 is 168, 4 is 72, 5 is 256. `doRebuildStage(NPCTypeFactory)` sets both scene axes to that tile count (`RebuildStage.java:691`).

Scene origin (`RebuildStage.java:748`):

```text
baseX = (zoneX - (tiles >> 4)) * 8
baseZ = (zoneZ - (tiles >> 4)) * 8
```

Map squares are the zone plus or minus `tiles >> 4`, divided by 8 (`:473`). The file id is `mapX | (mapZ << 7)` (`:687`). The request uses JS5 index `FileUtil.fileUtil5` (archive id 5) and group type `DataBlock321.clsa8c4ca914.tint` (integer unread).

## Dynamic opcode 49

`RebuildStage.java:488`:

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | u8(b-128) | kind 1..4 selects `cls86419faa4` .. `cls86419faa7` |
| 2 | u8(0-b) | `tint13` |
| 3 | u16le_low+128 | zone x |
| 4 | u8 | build-area id |
| 5 | u16le | zone z |
| 6 | u8(0-b) | bit 0 is force |

Then the buffer goes to bits. For 4 levels times `(tiles >> 3)` times `(tiles >> 3)`: one presence bit, and if set a 26-bit value. `value >> 14 & 1023` and `value >> 3 & 2047` form the map square (`:520`). The same file id `mapX | (mapZ << 7)` is used. Zone x and zone z are then passed to the same origin formula with type byte 8 (`:574`).

## RSLib

`rslib/src/both/move/rev_902.c` encodes the static header with `g1`, `g2`, `g1_sub128`, `g2`, `g1_neg`, `g1_sub128`. Those match `u8`, `u16be`, `u8(128-b)`, `u16be`, `u8(0-b)`, `u8(128-b)`. Its region opcode is 93 only. Opcode 49 is a gap.
