# Input

`opClient490` queues keyboard records from `Statics.valueStatics927` and mouse records from `Statics.valueStatics857` (`Client.java:904-954`). The packet a key press sends was not found. `ClientWatch` is the class RSLib names for camera and focus. That class was not opened, so game opcodes 74 and 100 stay empty in [packets.md](packets.md).

Run mode for the clicks below is `MiniMenu.opkind35268`. That method was not opened. Callers pass 1 when it returns true and 0 otherwise.

## Walk and minimap

`Statics.opStatics116` (`Statics.java:3826-3844`). Argument 0 selects game opcode 72, size 5. Argument 1 selects game opcode 78, size 18. Both write:

| Order | Width | Value |
| --- | --- | --- |
| 1 | u16le_add128 | scene base x plus the local x |
| 2 | u16be | scene base z plus the local z |
| 3 | u8neg | 1 when running |

The base is `Client.valueClient2019.opkind346745()`, the origin from [region.md](region.md). There are no step bytes. The same method stores the local x and z on `MiniMap.valuekind37029` and `MiniMap.valuekind37026` and clears `MiniMap.valuekind370211` (`Statics.java:3840-3842`).

Menu type 23 calls this with the menu's low integer as the third argument (`MiniMenu.java:1501-1505`). When that integer is 1, the minimap packet appends (`MiniMenu.java:1506-1517`):

| Order | Width | Value |
| --- | --- | --- |
| 4 | u8 | `-1` |
| 5 | u8 | `-1` |
| 6 | u16be | `Client.valueClient1871` |
| 7 | u8 | 57 |
| 8 | u8 | `valueClient1863` |
| 9 | u8 | `valueClient1865` |
| 10 | u8 | 89 |
| 11 | u16be | player fine x |
| 12 | u16be | player fine z |
| 13 | u8 | 63 |

5 + 13 = 18, which is opcode 78. Opcode 72 is the five-byte body. `rslib/src/both/move/rev_745.c` `walk_encode_745` writes the same five bytes. The 13-byte tail is in the Java caller, not in that encoder.

If `valueClient1914 > 0` and `MiniMenu.opkind352611()` is true, type 23 calls `Client.opClient470` instead (`MiniMenu.java:1502-1503`). `opClient470` was not opened.

## Mouse

Opcode 3, size 7, `C5eb941fa.opkind39973` (`C5eb941fa.java:21-25`): u16le of `opkind36131`, u8sub128 of `(button << 1) | (extra & 1)`, u32_476 of `y | (x << 16)`. The header opcode is 32, size `-1`, returned by `opkind39975` with no bytes in that method (`C5eb941fa.java:34-36`). The base logger that fills a `-1` header was not opened.

Opcode 9, size 6, `Ca1ac0f8e.opkind39973` (`Ca1ac0f8e.java:29-31`): u32le of `y | (x << 16)`, u16be_add128 of `packed | ((click == 2) ? 1 : 0) << 15`. The header opcode is 45, size `-1`, with an empty `opkind39972` (`Ca1ac0f8e.java:45-50`).

## Menu actions

`MiniMenu.opkind352621` (`MiniMenu.java:1391`). Type values at or above 2000 subtract 2000 first (`MiniMenu.java:1398-1400`).

Loc types 3, 4, 5, 6, 1001, and 1002 are opcodes 73, 25, 40, 47, 68, and 24, all size 9 (`MiniMenu.java:1404-1428`): u16be_add128 of base z plus local z, u16be of base x plus local x, u32_476 of the menu long's high half, u8add128 of the run flag.

Type 17 is opcode 43, size 15 (`MiniMenu.java:1432-1445`): u16be_add128 `valueClient1937`, u16be base z plus local z, u8add128 run, u16le_add128 `valueClient1938`, u32_460 `valueStatics703`, u16le_add128 base x plus local x, u16le of the menu low integer.

Type 8 is opcode 85, size 11 (`MiniMenu.java:1449-1462`): u16be `valueClient1937`, u16le of the npc index, u8sub128 run, u32le `valueStatics703`, u16be_add128 `valueClient1938`.

Type 16 is opcode 76, size 11 (`MiniMenu.java:1468-1478`): u16le_add128 `valueClient1937`, u32_460 `valueStatics703`, u16be_add128 `valueClient1938`, u16le_add128 local player index, u8add128 run.

Type 59 is opcode 1, size 12 (`MiniMenu.java:1486-1497`): u32_476 `valueStatics703`, u16be_add128 `valueClient1938`, u16le_add128 base x plus local x, u16le_add128 `valueClient1937`, u16be base z plus local z.

Ground types 18, 19, 20, 21, 22, and 1004 are opcodes 97, 88, 59, 27, 28, and 29, all size 7 (`MiniMenu.java:1537-1561`): u16le base z plus local z, u16le_add128 base x plus local x, u8add128 run, u16be of the menu low integer.

NPC types 9, 10, 11, 12, 13, and 1003 are opcodes 99, 15, 77, 21, 89, and 60, all size 3 (`MiniMenu.java:1566-1592`): u8sub128 run, u16le_add128 index.

Type 60 is opcode 103, size 4 (`MiniMenu.java:1605-1607`): u16le base x plus local x, u16be_add128 base z plus local z.

Type 2 is opcode 51, size 17 (`MiniMenu.java:1617-1625`): u16le_add128 `valueClient1937`, u8neg run, u16be base z plus local z, u32_460 of the menu long's high half, u16be_add128 `valueClient1938`, u32_476 `valueStatics703`, u16le_add128 base x plus local x.

Types 57 and 1007 call `Client.opClient614` (`MiniMenu.java:1482-1484`). Type 58 calls `Client.opClient466` (`MiniMenu.java:1530-1533`). Those methods were not opened. The menu method continues after line 1631 and was not read further.

## Other game writes that were read

Opcode 20, size 0, is queued with no payload when a connection's idle counter passes 50 (`Cf9cb6c0f14.java:16-24`, `Client.java:2537-2540`, `Client.java:2941-2944`).

Opcode 17, size 0, is `Client.opClient454` with no payload (`Client.java:12367-12369`).

Opcode 67, size 4, is `Client.opClient480`: one u32be of `valueClient1831` (`Client.java:12823-12827`). `opClient523` increments that counter (`Client.java:12817-12820`).

Opcode 107, size `-1`, starts with u8 `0`, then `ReflectionCheck.opkind37271`, then a backpatched u8 length (`Client.java:2612-2617`). The reflection method was not opened.

Opcode 36, size 4, is one u32be `-1` from the music start path (`C2a31b481.java:266-268`).
