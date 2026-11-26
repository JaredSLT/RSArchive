# Region

## Non-instanced rebuild

Server opcode 231 calls `TextureOpCurve.opTextureOpCurve14(false)` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1761`). Login success calls the same method (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:247`). `rslib/src/server/packets/468.c` binds region to opcode 222. Opcode 222 is a different branch in this switch and its body was not read.

Wire order for `false` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/TextureOpCurve.java:96`):

| Order | Width | Use |
| --- | --- | --- |
| 1 | u8 stored as `-value` | plane |
| 2 | u16be, low byte stored as `value + 128` | zone z |
| 3 | u16be | zone x |
| 4 | `(remainingBytes) / 16` groups of 4 | each word is `opMouseRecorder47`: wire bytes `>>8`, low, `>>24`, `>>16`. Stored in `SieHabenGeradeEineAndereWeltVerlassen.valuekind18232` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/SieHabenGeradeEineAndereWeltVerlassen.java`) |
| 5 | u16le | local scene x |
| 6 | u16be | local scene z |

`GameShell.opGameShell11(plane, localZ, zoneZ, zoneX, localX)` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/TextureOpCurve.java:138`) sets:

- base x = `zoneX * 8 - 48`
- base z = `(zoneZ - 6) * 8`
- plane = the first argument, forced to 0 when `Packet.valuePacket64` is false
- local player `opSoftReferenceNode12(localZ, false, localX)`

Map squares run `mx` from `(zoneX - 6) / 8` through `(zoneX + 6) / 8` and `mz` from `(zoneZ - 6) / 8` through `(zoneZ + 6) / 8`. The file id is `(mx << 8) | mz`.

Squares are skipped when `(zoneX / 8` is 48 or 49 and `zoneZ / 8` is 48) or (`zoneX / 8` is 48 and `zoneZ / 8` is 148). Inside that case the loop also skips `mz` 49, 149, and 147, `mx` 50, and the pair `mx == 49 && mz == 47` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/TextureOpCurve.java:110`).

Land file name is `"m" + mx + "_" + mz`. Loc file name is `"l" + mx + "_" + mz`. The pieces are `TextureOp16.valueTextureOp1627` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp16.java:58`), `ClientScript.valueClientScript44` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/ClientScript.java:19`), and `ChangeLocRequest.valueChangeLocRequest14` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/ChangeLocRequest.java:8`). Both names are looked up in archive 5, `TextureOpInvert.valueTextureOpInvert3`.

`EmpfNger.opkind1389` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/EmpfNger.java:90`) loads land with `opByteArrayNode13(landId, 0)` and locs with `opByteArrayNode24(locId, key[i], 0)`. A missing buffer increments `PleaseEnterUsername.valuekind2113` and leaves the state loading.

`opGameShell11` continues after the local-player place (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GameShell.java:72`). That tail was not read.

## Instanced rebuild

Server opcode 169 calls `opTextureOpCurve14(true)` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1022`). Wire order (`builds/rs2/468-2007-08-08/src/src/main/java/deob/TextureOpCurve.java:140`):

| Order | Width | Use |
| --- | --- | --- |
| 1 | u16be | local scene z (`var14`, first argument of the later place) |
| 2 | u8 stored as `value + 128` | plane |
| 3 | u16be | zone x |
| 4 | u16be, low byte `+ 128` | local scene x |
| 5 | bits | planes 0..3, x 0..12, z 0..12. One bit. If it is 1, 26 more bits are stored in `Scenery.valueScenery6`; otherwise the cell is -1 |
| 6 | `(remainingBytes) / 16` groups of 4 | each word is `opMouseRecorder9`: wire bytes `>>16`, `>>24`, low, `>>8` |
| 7 | u16le | zone z |

The chunk id in a set bit is split as `(value & 16767716) >> 14` and `(value & 16383) >> 3`. The map id is `(thatX / 8 << 8) | (thatZ / 8)`. The same `"m"` and `"l"` names are used. `opGameShell11(plane, localZ, zoneZ, zoneX, localX)` is `opGameShell11(var15, var14, var25, var16, var17)`.

## Zone origin

Opcode 115 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1767`): u8 stored as `-value` goes to `SecondaryLinkedList.valueSecondaryLinkedList28` (scene z origin), then u8 stored as `value + 128` goes to `CursorManager.valueCursorManager13` (scene x origin).

Opcode 33 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1571`): u8+128 into the x origin, u8 stored as `-value` into the z origin, then while the buffer position is still below the packet size: a u8 sub-opcode stored in `TextureOp4.valueTextureOp420`, and `TracingException.opTracingException7`.

Opcodes 195, 138, 164, 103, 120, 82, 32, 26, 147, 150, and 105 have no header of their own. Each calls `opTracingException7` once (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:916`).

Tile x is the x origin plus the high part of the coord byte. Tile z is the z origin plus `coord & 7`. Area-sound opcode 195 compares the x tile with `valueSoftReferenceNode130` and the z tile with `valueSoftReferenceNode108` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:291`). The loc shape class is `DerServerWirdGeradeAktualisiert.valuekind1517[shape >> 2]`, and that table is `{0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3}` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:34`). Class 0, 1, 2, and 3 are the four branches in opcode 147.

## Zone bodies

All of these are `TracingException.opTracingException7` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:43`).

| Opcode | Fields |
| --- | --- |
| 164 | u16le, i8 stored as `-raw`, u16le, u8 stored as `128 - value` (shape is `>> 2`, rotation is `& 3`), i8 stored as `raw - 128`, u16be with low `+ 128` (player index; equal to `PleaseEnterUsername.valuekind21113` selects the local player), u16be loc id, i8 stored as `128 - raw`, i8 stored as `raw - 128`, u8 stored as `128 - value` (x uses `(byte & 112) >> 4`, z uses `byte & 7`) |
| 26 | u16le obj id, u8 coord (`>> 4 & 7` plus x origin, `& 7` plus z origin), u16le with low `+ 128` amount |
| 147 | u8 (rotation `& 3`, shape `>> 2`), u8 stored as `-value` (z `& 7`, x `(byte >> 4) & 7`), u16be with low `+ 128` |
| 32 | u8 stored as `-value` (x `(byte >> 4) & 7`, z `& 7`), u16le id matched as `id & 32767` |
| 150 | u8 stored as `value + 128` (shape `>> 2`, rotation `& 3`), u8 stored as `-value`. x is `(byte & 120) >> 4`. z is `byte & 7`. Removal passes id `-1` |
| 120 | u8 coord, u16be spot id, u8 start height, u16be end id |
| 103 | u16le, u16be with low `+ 128` player index (the local index is skipped), u8 stored as `128 - value`. x is `(byte & 122) >> 4`. z is `byte & 7`. Then u16le with low `+ 128` |
| 105 | u8 stored as `-value` (shape `>> 2`, rotation `& 3`), u16be with low `+ 128` loc id, u8+128 coord (`>> 4 & 7`, `& 7`) |
| 195 | u8 coord, u16be sound id, u8 (low 3 bits and high nibble), u8. Plays when the local player is inside the radius, the sound level is not 0, and the queue is below 50 |
| 82 | u8 coord. x uses `(byte & 113) >> 4` and z uses `byte & 7`. Then raw i8, raw i8, signed u16be, u16be graphic (65535 skips the spawn), u8 times 4, u8 times 4, u16be, u16be, u8, u8 |
| 138 | u8 coord, u16be old id matched as `& 32767`, u16be old amount, u16be new amount |

Opcode 150's x mask is `& 120` as decompiled. Opcode 103 uses `& 122`. Opcode 82 uses `& 113`. Opcode 164 uses `& 112`. Those expressions are the ones in the method. Loc add and remove go through `UnlitMaterialRenderer.opUnlitMaterialRenderer7` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/UnlitMaterialRenderer.java:30`). The current zone opcode is `TextureOp4.valueTextureOp420` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp4.java`).
