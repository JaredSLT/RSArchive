# Region

Static rebuild is server opcode 126, size -2 (`DataBlock832.java:258`). The switch copies the payload and calls `World.doWorld` with `DataBlock589.cls2fbb6b303` (`builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5491`).

Dynamic rebuild is server opcode 128, size -2 (`DataBlock832.java:262`), kind `cls2fbb6b304` (`Statics.java:6513`).

`World.doWorld(DataBlock512)` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/scene/World.java:395`): `cls2fbb6b303` calls `doWorld(buf)`. A kind whose `tboolean` is set calls `doWorld3`. `cls2fbb6b304` has that flag (`builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock589.java:12`). `cls2fbb6b30` clears the environment and reads no buffer. `cls2fbb6b302` rebuilds from the zone list already stored.

Neither packet contains XTEA keys. Map squares are loaded from JS5.

## Build area

`NPCType` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/config/npctype/NPCType.java`). `Statics.doStatics2322b6` returns the instance whose id matches (`Statics.java:1470`). `World.doWorld(NPCType)` sets both axis lengths to `tint2` (`World.java:700`).

| id | tiles |
| --- | --- |
| 0 | 104 |
| 1 | 120 |
| 2 | 136 |
| 3 | 168 |
| 4 | 72 |
| 5 | 256 |

## Static header

`World.doWorld(PositionedSoundPart)` (`World.java:460`). Eight bytes. This matches `region_decode` in `rslib/src/both/move/rev_887.c`.

| order | width | use |
| --- | --- | --- |
| 1 | u8 | build-area id |
| 2 | u8_sub128 | force reload when equal to 1 |
| 3 | u8_sub128 | array length only. The square list is recomputed from the zones |
| 4 | u16le | zone x |
| 5 | u8 | npc view distance, `tint13` |
| 6 | u16le_low128 | zone z |

Scene origin (`World.java:756`): `(0, (zoneX - (size >> 4)) * 8, (zoneZ - (size >> 4)) * 8)`.

Map squares are the 8-square window around that zone. Group id is `x | (z << 7)` (`:695`). A square is kept when `Statics.js529.doJs5(groupId, 3)` is true. File id 3 is `DataBlock473.cls00df93644` (`builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock473.java:12`). `js529` is JS5 archive 5 (`CameraTrackableType.java:269`). Other file ids inside the group were not read. The load stage passed into `doWorld(int, int, int, boolean)` from a packet is 14.

## Dynamic header

`World.doWorld3(PositionedSoundPart)` (`World.java:496`).

| order | width | use |
| --- | --- | --- |
| 1 | u8_neg | 1, 2, 3, or 4 selects `cls2fbb6b304` through `cls2fbb6b307` |
| 2 | u16le_low128 | second argument of the origin call |
| 3 | u16be | first argument of the origin call |
| 4 | u8_neg | build-area id |
| 5 | u8 | bit 0 is force reload |
| 6 | u8_add128 | npc view, `tint13` |

Then byte-to-bit. For 4 levels, and for `(size >> 3)` by `(size >> 3)` tiles: bit(1). If set, bit(26) is stored in `intArrayArrayArray`. Unique map squares use the same group-id formula and archive 5 file 3. No XTEA keys.

RSLib has no decoder for this packet.
