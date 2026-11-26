# Region

Opcode 88 `REBUILD_NORMAL` copies its payload into `World.rebuildNormalMap` (`builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11422`, `World.java:458`). Opcode 9 `REBUILD_REGION` copies its payload into `World.rebuildRegionMap` (`Client.java:10103`, `World.java:494`). Neither packet contains XTEA keys.

## REBUILD_NORMAL

| order | read | meaning |
| --- | --- | --- |
| 1 | `g2_alt2` | zone X |
| 2 | `g1` | `npcViewDistance` |
| 3 | `g1_alt3` | map-square array length, not a list of ids |
| 4 | `g1` | build-area id |
| 5 | `g2_alt2` | zone Z |
| 6 | `g1_alt3` equals 1 | force |

`BuildAreaSize.buildAreaSizeForId` (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/core/constants/BuildAreaSize.java:36`): id 0 is 104, 1 is 120, 2 is 136, 3 is 168, 4 is 72, 5 is 256.

Map squares are the 8-zone window around `(zoneX, zoneZ)` that has a valid LAND group. Group id is `x | (z << 7)` (`World.java:693`).

`doWorld` sets the scene origin to level 0 and tiles `(zoneX - (mapSizeX >> 4)) * 8`, `(zoneZ - (mapSizeZ >> 4)) * 8` (`World.java:754`). For build area 0, `mapSizeX >> 4` is 6.

## REBUILD_REGION

| order | read | meaning |
| --- | --- | --- |
| 1 | `g1` | npc view distance |
| 2 | `g1` | build-area id |
| 3 | `g1` | 1..4 selects a `RebuildType` |
| 4 | `g2` | zone X |
| 5 | `g2_alt1` | zone Z, little-endian |
| 6 | `g1_alt1` | bit 0 is force |

Then `accessBits`. For 4 planes times `(mapSizeX >> 3)` times `(mapSizeZ >> 3)`, `gBit(1)`. When that bit is 1, `gBit(26)` is a chunk id stored in `intArrayArrayArray`. Map squares are derived from those chunk ids. `accessBytes` follows. The same `doWorld` origin formula uses the packet's zone X and zone Z.

## Zone base

| packet | reads | source |
| --- | --- | --- |
| `UPDATE_ZONE_PARTIAL_ENCLOSED` | `g1_alt1` level, `g1b_alt2 << 3` z, `g1b_alt2 << 3` x, then `g1` indexes `ZoneProt.values()` until `packetSize` | `Client.java:9601` |
| `UPDATE_ZONE_PARTIAL_FOLLOWS` | `g1` level, `g1b_alt2 << 3` z, `g1b << 3` x | `Client.java:9656` |
| `UPDATE_ZONE_FULL_FOLLOWS` | `g1b_alt2 << 3` z, `g1_alt2` level, `g1b_alt3 << 3` x, then obj stacks in that 8 by 8 are cleared | `Client.java:9917` |

## Zone opcodes

`ZoneProt.values()` order is the opcode (`ZoneProt.java:39`). The constructor argument is discarded (`ZoneProt.java:43`) and is not a framed length.

| index | name | body that was read |
| --- | --- | --- |
| 0 | LOC_PREFETCH | unread |
| 1 | MAP_ANIM | unread |
| 2 | TEXT_COORD | unread |
| 3 | OBJ_COUNT | `g1` tile, `g2` obj index, `g2` old count, `g2` new count (`Client.java:11485`) |
| 4 | LOC_ANIM | unread |
| 5 | LOC_DEL | `g1`, `g1_alt3`, then `LocPositionAdjustment` (constructor unread) (`Client.java:11512`) |
| 6 | zoneProt | unread |
| 7 | MAP_PROJANIM | unread |
| 8 | OBJ_REVEAL | unread |
| 9 | OBJ_DEL | unread |
| 10 | LOC_ADD_CHANGE | unread |
| 11 | LOC_CUSTOMISE | unread |
| 12 | MAP_PROJANIM_HALFSQ | unread |
| 13 | OBJ_ADD | unread |
| 14 | SOUND_AREA | `g1` tile, `g2` id (65535 means -1), `g1` (high nibble range, low 3 bits), `g1`, `g1`, `g2` (`Client.java:11523`) |

`ServerProt.LOC_DEL` calls `handleZonePacket(ZoneProt.LOC_DEL)` with no extra header (`Client.java:10469`). The other same-named server opcodes were not opened, so their rows in `packets.md` stay empty.

## Map file ids

`Js5MapFile` (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/js5/Js5MapFile.java:8`): LOC 0, UNDERWATER_LOC 1, NPC 2, LAND 3, UNDERWATER_LAND 4, NXT_LOC 5, ENVIRONMENT 6, STATIC_POINTLIGHTS 7, WATER 8. Land is loaded when `mapsJs5.isGroupValid(group, LAND.id)`.

JS5 group decryption uses `tinydec`. The call site that passes the keys was not opened. The rebuild packets themselves have no keys.
