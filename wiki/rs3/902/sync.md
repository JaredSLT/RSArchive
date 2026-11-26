# Sync

Player update is server opcode 1, size `-2` (`DataBlock457.clsfbe8a0992`). `Client.doClient22` calls `UnblendedTileData2.doUnblendedTileData2(buffer, packetLength)` (`rs2/client/Client.java:8341`).

NPC update is server opcode 7, size `-2` (`clsfbe8a0998`, `Client.java:10189`). It calls `Client.doClient2222(rebuildStage.tint13)`. `tint13` is the `u8(128-b)` from the rebuild packet.

## Players

Slots are 2048 (`UnblendedTileData2.java:24`). The trailing rebuild loops `1 .. 2047` (`:173`), so index 0 is never placed in the high-res list `intArray` or the low-res list `intArray2`. The local player index is `Client.tint75`, not a fixed slot. Removing that index throws (`:219`).

`byteArray[slot]` bit 0 selects the pass. A skip sets bit 1 (value 2). At the end of the packet every slot is shifted right by 1 (`:175`).

Order in `doUnblendedTileData2(WorldMapElementPart)` (`:62`):

1. Byte-align. High-res slots whose flag bit 0 is clear.
2. Byte-align. High-res slots whose flag bit 0 is set.
3. Byte-align. Low-res slots whose flag bit 0 is set.
4. Byte-align. Low-res slots whose flag bit 0 is clear.
5. Byte-align, then rebuild the two lists from slots `1 .. 2047`.

For each visited slot, a remaining skip count marks the slot and decrements. Otherwise one bit: 0 reads a skip, 1 reads movement. Skip (`doUnblendedTileData22` `:191`): 2 bits, then 0 bits, 5 bits, 8 bits, or 11 bits. The count skips that many following slots. The current slot is also skipped. A leftover skip throws.

High-res movement (`:208`) starts with a needs-mask bit, then a 2-bit type.

Type 0: if the mask bit is clear, remove the player. A further bit, if set, calls the low-res update on this index.

Type 1, one step, 3-bit direction. `intArray9` is x and `intArray10` is z (`:268`):

| Bits | dx | dz |
| --- | --- | --- |
| 0 | -1 | -1 |
| 1 | 0 | -1 |
| 2 | +1 | -1 |
| 3 | -1 | 0 |
| 4 | +1 | 0 |
| 5 | -1 | +1 |
| 6 | 0 | +1 |
| 7 | +1 | +1 |

One more bit. If it is 1, `byteArray2` becomes `MoveSpeed.moveSpeed4.serialID` (2) and 2 bits apply a one-tile half-step first: 0 `+z`, 1 `-x`, 2 `+x`, 3 `-z`. If it is 0, `byteArray2` is not written in this branch. The direction step then uses whatever `byteArray2` holds.

Type 2, 4 bits (`:295`):

| Code | dx | dz | Code | dx | dz |
| --- | --- | --- | --- | --- | --- |
| 0 | -2 | -2 | 8 | +2 | 0 |
| 1 | -1 | -2 | 9 | -2 | +1 |
| 2 | 0 | -2 | 10 | +2 | +1 |
| 3 | +1 | -2 | 11 | -2 | +2 |
| 4 | +2 | -2 | 12 | -1 | +2 |
| 5 | -2 | -1 | 13 | 0 | +2 |
| 6 | +2 | -1 | 14 | +1 | +2 |
| 7 | -2 | 0 | 15 | +2 | +2 |

Type 3, one selector bit. 0 reads 15 bits (`:344`): plane `>> 12 & 7` compared with `MoveSpeed.moveSpeed5.tint` (4), sub-plane `(byte)(>> 10) & 3`, dx signed 5 at `>> 5`, dz signed 5 at `& 31`. 1 reads a 3-bit speed, compared with `moveSpeed5.tint`, then 30 bits: plane `>> 28 & 3`, x `>> 14 & 16383`, z `& 16383`. Those 14-bit values are added to the current waypoint and masked with 16383 (`:382`). They are not stored as absolute scene coordinates.

`MoveSpeed` (`com/jagex/game/client/MoveSpeed.java:8`): `(tint, serialID)` is `(0, -1)`, `(1, 0)`, `(2, 1)`, `(3, 2)`, `(4, 3)`. `getId` returns `serialID`. The teleport compare uses `tint`.

Low-res `Statics.doStatics` (`deob/Statics.java:2978`), 2 bits:

| Bits | Body |
| --- | --- |
| 0 | optional recursive bit, 6-bit local x, 6-bit local z, needs-mask bit. Place the player from the cached packed coordinate |
| 1 | 2-bit plane delta only |
| 2 | 5 bits: plane delta `>> 3 & 3`, direction `& 7` using the same 8-way table, applied to the cached coordinate |
| 3 | 20 bits: speed-1 `>> 18 & 3`, plane delta `>> 16 & 3`, dx `>> 8 & 255`, dz `& 255`, added to the cached coordinate |

Masks (`doUnblendedTileData222` `:405`): for each needs-mask index, discard one `u16be`, read `u8`, and if bit `0x80` is set add `u8 << 8`, and if bit `0x2000` is set add `u8 << 16`. Bits are then tested in this order (`:425`):

| Bit | Reads |
| --- | --- |
| 0x20 | `i8(128-b)`, `i8(b-128)`, `i8`, `i8(0-b)`, `i8`, `i8(0-b)`, `u16le`, `u16be_low-128`, `u16le` |
| 0x1 | `u16be_low-128` (`65535` means -1), `u32le`, `u8` split into `& 7`, `>> 3 & 15` (`15` means -1), `>> 7 & 1` |
| 0x4 | `u8(128-b)` count, then that many raw bytes |
| 0x800000 | `u16le` (`65535` means -1), `u32be`, `u8(b-128)` split the same way as 0x1 |
| 0x40000 | `u8(b-128)` count. Each entry is `u16be_low-128`. If `(value & 0xC000) == 0xC000`, read `u16le` and the id is `(first << 16) \| second`. Then `u16be` |
| 0x100 | `u8(128-b)` count. Each entry is `smart2or4`, `u8(b-128)`, `u16le_low+128` |
| 0x200 | `u8` count, then that many raw bytes |
| 0x10000 | `pos += 2`, then `u8` count. Each entry is `u8` plus `doVarTypeList` (unread) |
| 0x10 | `u8` count of hits using `smartu`, with `32767` and `32766` taking extra `smartu` values, then `u8(128-b)` count of a second list |
| 0x40 | `u16be_low-128` (`65535` means -1) |
| 0x8000 | `u16le_low+128` (`65535` means -1), `u32le`, `u8(b-128)` split as 0x1 |
| 0x4000 | `u16be` (`65535` means -1), `u32be`, `u8` split as 0x1 |
| 0x80000 | `i8(128-b)`, `i8`, `i8(0-b)`, `u8(128-b)`, two `u16le_low+128` |
| 0x1000 | `jstr` |
| 0x100000 | `jstr`, `u8(128-b)` |
| 0x200000 | `u8(128-b) == 1` |
| 0x8 | `u16le` |
| 0x400000 | `pos += 2`, `u8` count, each `u8(b-128)` plus `doVarTypeList` (unread) |
| 0x800 | `u8(b-128)` looked up in `DataBlock367` |
| 0x2 | four `smart2or4`, then `u8(128-b)` |
| 0x20000 | `u16be` (`65535` means -1), `u32_alt14`, `u8` split as 0x1 |

`rslib/src/both/move/rev_902.c` writes one high-res block and a low-res skip of 2045, leaves the mask bit clear, rejects the 15-bit teleport, and treats the 30-bit x and z as absolute. Those four points are gaps. The 2-bit skip widths match `doUnblendedTileData22`.

## NPCs

`Client.doClient2222` (`Client.java:10765`) runs three steps, then checks that the buffer is exhausted.

Existing NPCs, `doClient2222b152` (`:10842`): byte-align, then an 8-bit count. A count below `tint20` queues the missing indices for removal. A count above `tint20` throws. For each kept index, one bit of 0 keeps it with no movement. Otherwise 2 bits:

| Bits | Body |
| --- | --- |
| 0 | keep, and queue a mask |
| 1 | 3-bit direction, speed `MoveSpeed.moveSpeed3.serialID` (1), one bit queues a mask |
| 2 | one bit: if 1, two 3-bit directions at `moveSpeed4.serialID` (2); if 0, one 3-bit direction at `moveSpeed2.serialID` (0). Then one mask bit |
| 3 | remove |

The 3-bit value is `CreateAccountReply.getId()` (`StringComparator.java:24`). `getId` returns `tint2` (`CreateAccountReply.java:41`), which is 0 through 7. The tile delta for each id was not read.

New NPCs, `doClient2222b5` (`:10906`): while at least 15 bits remain, read 15 bits. `32767` ends the list. Then 2-bit plane, `tint13` bits of signed z relative to the local player's z, 15-bit type id, one bit, 3-bit orientation stored as `(bits + 4) << 11`, one mask bit, and `tint13` bits of signed x relative to the local player's x.

Masks, `doClient2222b132` (`:10972`): discard one `u16be`, read `u8`, and extend with `u8 << 8` if bit `0x10` is set, `u8 << 16` if bit `0x800` is set, and `u8 << 24` if bit `0x800000` is set. Test order:

| Bit | Reads |
| --- | --- |
| 0x8000 | `u8(128-b)` count. Each entry is `smart2or4`, `u8(b-128)`, `u16le` |
| 0x2000000 | `i8(0-b)`, `i8(0-b)`, `i8(b-128)`, `u8(0-b)`, `u16le_low+128`, `u16le` |
| 0x1 | `jstr` |
| 0x200000 | `u8(128-b)` of eight slot flags. A set bit reads `smart2or4` and `smart` `doWorldMapElement2222b29` |
| 0x80000 | `u16le_low+128` (`65535` keeps the type's value) |
| 0x20 | `u16le` (`65535` means -1) |
| 0x2 | hit list as in the player `0x10` mask, but the second count is a plain `u8` |
| 0x400 | `u16le_low+128` (`65535` means -1) |
| 0x8 | four `smart2or4`, then `u8(b-128)` |
| 0x40 | `smart2or4` type id. Bits `0x400000` and `0x80000` of the same mask change how it is applied |
| 0x1000 | `u8(b-128)` count, entries as the player `0x40000` mask but the second short is `u16le_low+128` and the tail is `u16be` |
| 0x400000 | `jstr` |
| 0x20000 | `pos += 2`, `u8` count, each `u8(0-b)` plus `doVarTypeList` (unread) |
| 0x4 | `u16le_low+128`, `u16be_low-128` |
| 0x40000 | `pos += 2`, `u8` count, each `u8` plus `doVarTypeList` (unread) |
| 0x100000 | `u8` count, each `u8(0-b)`, `u32_alt13`, `u24` `doWorldMapElement2222b26` |
| 0x10000000 | `u8(128-b) == 1` |
| 0x8000000 | `u16le` (`65535` means -1), `u32_alt13`, `u8(b-128)` split as a spotanim |
| 0x80 | `u16be_low-128` (`65535` means -1), `u32_alt13`, `u8(128-b)` split as a spotanim |
| 0x2000 | six signed bytes: `i8(0-b)`, `i8(128-b)`, `i8(0-b)`, `i8`, `i8(0-b)`, `i8(128-b)`, then `u16le_low+128`, `u16be_low-128`, `u16be_low-128` |
| 0x20000000 | `u16le` (`65535` means -1), `u32be`, `u8(0-b)` split as a spotanim |
| 0x4000 | `u16le` (`65535` means -1), `u32be`, `u8(128-b)` split as a spotanim |
| 0x4000000 | `u16be_low-128` (`65535` means -1), `u32be`, `u8(128-b)` split as a spotanim |
| 0x10000 | `u8(128-b)` flag. If bit 0 is clear: `pos += 2`, then optional model, recolour, and retexture arrays selected by bits 2, 4, and 8. The retexture count uses a zero local, so that array length is 0 in this method |
| 0x1000000 | `u8(0-b)` |
| 0x200 | `u8(b-128)` flag. Bit 0 clears the customisation. Otherwise `pos += 2` and bits 2, 4, 8, and 16 select models, optional per-model `float32` plus six `i16be`, recolours as `u16le_low+128`, and retextures as `u16le` |

`doWorldMapElement2222b29` (`WorldMapElement.java:492`) is `u8 - 1` when the next byte is below 128, otherwise `u16be` minus the code point U+8001.
