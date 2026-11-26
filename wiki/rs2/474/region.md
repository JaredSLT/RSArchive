# Region

Static rebuild is S2C opcode 61, size `-2` (`rt4/JavaMouseWheel.java:1054-1056`). The first packet after a successful login is also passed to `ByteArrayNodeSecondary.opByteArrayNodeSecondary24(false)`, and its opcode is ignored (`rt4/TextureOp29SubOp.java:292-295`). Dynamic rebuild is S2C opcode 175 (`rt4/JavaMouseWheel.java:778-780`).

Both paths end in `SceneGraph_Class120.opSceneGraph_Class12050(zoneZ, zoneX, localZ, plane, localX)` (`rt4/SceneGraph_Class120.java:78`). That method sets the zone origin to `zoneZ * 8 - 48` and `(zoneX - 6) * 8` (`rt4/SceneGraph_Class120.java:91-92`), stores the plane, and teleports the local player with `opJs5CacheRequest43(localZ, localX, false)` (`rt4/SceneGraph_Class120.java:127`). When `Loc.valueLoc42` is false the stored plane is forced to 0 (`rt4/SceneGraph_Class120.java:83-85`). The scene built at startup is 4 planes of 104 by 104 (`rt4/MidiNote.java:37`). State 25 loads the map bytes (`rt4/Client.java:306-307`).

`rslib/src/both/move/rev_474.c:26` says `rs_region_encode_474` swaps the two local coordinates. The Java read order below does not swap them. The static local z is the first field and the static local x is the field after the keys.

## Static packet

`opByteArrayNodeSecondary24(false)` (`rt4/ByteArrayNodeSecondary.java:107-149`):

| Order | Width | Use |
| --- | --- | --- |
| 1 | u16le_lo_m128 | local z |
| 2 | u8 | plane |
| 3 | u16be | zone x |
| 4 | `(remaining / 16)` groups of four u32le | XTEA keys. Integer division drops a trailing partial group |
| 5 | u16be_lo_m128 | local x |
| 6 | u16be | zone z |

Map squares run from `(zoneX - 6) / 8` through `(zoneX + 6) / 8`, and the same span for zone z. A square is skipped when zone x is in region column 48 or 49 and zone z is in row 48, or zone x is in column 48 and zone z is in row 148, and the square itself matches the inner test at `rt4/ByteArrayNodeSecondary.java:127-144`.

The terrain file id is `opSoftwareFont29` of the name `m` + region x + `_` + region z. The loc file uses the prefix `l`. The prefixes are `Ca2bb2752.valuekind2391` (`deob/Ca2bb2752.java:14`) and `HashTableIterator.valueHashTableIterator21` (`rt4/HashTableIterator.java:24`). The separator is `MapElementList.valueMapElementList23`. The map archive is index 5, `TextureOp16.valueTextureOp16132` (`rt4/MidiNote.java:67`).

## Dynamic packet

`opByteArrayNodeSecondary24(true)` (`rt4/ByteArrayNodeSecondary.java:38-105`):

The reader switches to bits and walks 4 planes, 13 by 13. Each cell is one presence bit. A set bit is followed by 26 bits stored in `MaterialManager_Class106_Sub2_Sub1.valueMaterialManager_Class106_Sub2_Sub18`. A clear bit stores -1. The reader then byte-aligns.

| Order | Width | Use |
| --- | --- | --- |
| 1 | bit block above | chunk tiles |
| 2 | `(remaining / 16)` groups of four u32me2 | XTEA keys. This endian is not the static u32le |
| 3 | u16le_lo_m128 | zone x |
| 4 | u16le | local x |
| 5 | u16le | local z |
| 6 | u8 | plane |
| 7 | u16be_lo_m128 | zone z |

The scene call is `(zone z, zone x, local z, plane, local x)`.

A set chunk tile splits as region `(tile & 16769282) >> 14` and coordinate `tile >> 3 & 2047`. The file key is `(region / 8 << 8) + (coordinate / 8)`. Terrain and loc ids are looked up with the same `m` and `l` names (`rt4/ByteArrayNodeSecondary.java:80-97`).

## XTEA and map load

`TextureOp35.opTextureOp351` loads terrain with `opSoftwareFont35(0, mapId)` and loc bytes with `call4bdb2339(0, locId, keys)` (`rt4/TextureOp35.java:47`, `rt4/TextureOp35.java:55`). `opSoftwareFont35(file, group)` indexes the group with the second argument (`rt4/SoftwareFont.java:291-292`). The key array is passed into that load. `opByteArrayNodeSecondary70` is the XTEA routine in [framing.md](framing.md). The method that applies those keys to the loc bytes, and the per-tile terrain decoder, were not read. See [gaps.md](gaps.md).

## Zone base

Opcode 168, size 2, sets the zone base used by the following zone packets: u8 into `FreieWelt.valuekind22316`, u8m128 into `GameShell.valueGameShell87` (`rt4/JavaMouseWheel.java:602-604`).

Opcode 163, size `-2`, writes the same two fields with u8 and u8n128, then reads a u8 opcode and calls `ShapedTile.opShapedTile20` until the packet ends (`rt4/JavaMouseWheel.java:264-271`).

Opcode 169, size 2, clears an 8 by 8 area. The fields are u8n128 into the z base and u8m128 into the x base (`rt4/JavaMouseWheel.java:1419-1421`).

Zone packets share that base. A coordinate byte contributes `(byte >> 4) & 7` to x and `byte & 7` to z. The Java masks differ (`124`, `115`, `114`, `117`, `116`) and still isolate those bits (`rt4/ShapedTile.java:118-421`).

| Opcode | Size | Fields |
| --- | --- | --- |
| 205 | 5 | u8 coord, u16be sound id, u8 packed radius and loops, u8 delay. Queued when the player is near, volume is nonzero, and the queue holds fewer than 50 |
| 28 | 3 | u16be_lo_m128 id, u8m128 coord. Removes a ground item whose id matches `id & 32767` |
| 55 | 4 | u8m128 `(shape << 2) \| rotation`, u8 coord, u16be animation. Shape class is `LightType.valueLightType3[shape]` |
| 38 | 15 | u8 coord, i8 x delta, i8 z delta, i16be, u16be spotanim skipped when 65535, u8 start height times 4, u8 end height times 4, u16be start delay, u16be end delay, u8, u8 |
| 49 | 6 | u8 coord, u16be id, u8 height, u16be delay |
| 188 | 4 | u8 `(shape << 2) \| rotation`, u16be_lo_m128 id, u8n128 coord |
| 191 | 7 | u8 coord, u16be id, u16be, u16be_lo_m128. Skipped when the second u16be equals the local player index |
| 220 | 7 | u8 coord, u16be id, u16be old count, u16be new count |
| 86 | 2 | u8neg `(shape << 2) \| rotation`, u8neg coord. The loc id passed onward is -1 |
| 7 | 5 | u8 coord, u16be id, u16le_lo_m128 amount |
| 18 | 14 | u8 `(shape << 2) \| rotation`, i8_128sub, i8, u8m128 coord, u16be_lo_m128, u16be loc id, u16be_lo_m128 player index, u16le, i8m128, i8neg. The player is local when the index equals `SpotAnimType.valueSpotAnimType99` |

`LightType.valueLightType3` is `{0,0,0,0,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,3}` (`rt4/LightType.java:8`).
