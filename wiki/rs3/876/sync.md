# Sync

Player update is server packet 54, size `-2`. `Statics.doStatics3` calls `DeveloperConsole.doDeveloperConsole(buf, length)` (`deob/Statics.java:4323`). That method requires the bit cursor to end on `length` (`com/jagex/game/shared/console/DeveloperConsole.java:123`).

NPC update is server packet 39, size `-2`. The handler calls `Client.doClient3222b(Client.world.tint13)` (`deob/Statics.java:6315`). `tint13` is the view-distance bit count from the last rebuild ([region.md](region.md)).

Local player x is `intArray9`. Local player z is `intArray10`. The local index is `Client.tint76`. The lobby info block is the only place `blankWs` stores it (`deob/BlankWs.java:808`).

## Initial snapshot

`DeveloperConsole.doDeveloperConsole(PositionedSoundPart4)` (`DeveloperConsole.java:55`), used by the lobby's first game packet and by response 15.

1. Bit-align.
2. 30 bits for the local player: level is `value >> 28`, x is `value >> 14 & 16383`, z is `value & 16383`.
3. For every index from 1 to 2047 except the local index, 18 bits: level `>> 16 & 3`, square x `>> 8 & 255`, square z `& 255`, packed as `(level << 28) + (squareX << 14) + squareZ`.
4. Byte-align.

There is no empty-list terminator. The count is fixed at 2047 other players.

## Ongoing player movement

`Statics.doStatics` (`deob/Statics.java:2073`) walks four lists. `DeveloperConsole.intArray` is the high-resolution list. `intArray2` is the low-resolution list. `byteArray[index] & 1` selects the pass. After all four passes, indexes 1 through 2047 are shifted down by one bit and split again into the two lists (`Statics.java:2184`). Index 0 is not rebuilt by that loop.

Each pass bit-aligns, then for each selected index:

- While a skip count remains, the index is marked and the count decreases.
- Otherwise one bit. `0` reads a skip and marks the index. `1` calls `doDeveloperConsole2(buf, index)`.

Skip (`DeveloperConsole.doDeveloperConsole2`, `DeveloperConsole.java:129`): 2 bits, then 0, 5, 8, or 11 bits for kinds 0, 1, 2, and 3. There is no single terminator byte. A leftover skip throws.

Per active player (`DeveloperConsole.java:146`), one bit is stored as "needs a later mask" when set. Then 2 bits:

| Code | Bits after it |
| --- | --- |
| 0 | No step. If the mask bit was clear and this is not the local player, the player is removed. One further bit selects an immediate low-resolution update (`doDeveloperConsole22`). |
| 1 | 3-bit direction, then 1 bit. If that bit is 1, speed byte `DataBlock1396` id 3 (`tbyte` 2, `DataBlock1396.java:12`) and 2 more bits: 0 is z+1, 1 is x-1, 2 is x+1, 3 is z-1. That 2-bit step is applied first. The 3-bit step is applied second. |
| 2 | 4-bit run code, applied as one step with the existing speed byte. |
| 3 | 1 bit. `0`: 15 bits, plane `>> 12 & 7`, level delta `>> 10 & 3`, signed dx `>> 5 & 31`, signed dz `& 31` (values above 15 subtract 32). If the 3-bit plane field equals `DataBlock1396` id 4, the step has no speed byte; otherwise the speed byte is `planeField - 1`. `1`: 3-bit speed id, then 30 bits of absolute level (2), x (14), and z (14), added to the current world coordinate and rebased. Id 4 again means no speed byte. |

3-bit direction on x (`intArray9`) and z (`intArray10`), `DeveloperConsole.java:206`:

| Code | Delta |
| --- | --- |
| 0 | x-1, z-1 |
| 1 | z-1 |
| 2 | x+1, z-1 |
| 3 | x-1 |
| 4 | x+1 |
| 5 | x-1, z+1 |
| 6 | z+1 |
| 7 | x+1, z+1 |

4-bit run codes, `DeveloperConsole.java:233`: 0 is (-2,-2), 1 is (-1,-2), 2 is (0,-2), 3 is (1,-2), 4 is (2,-2), 5 is (-2,-1), 6 is (2,-1), 7 is (-2,0), 8 is (2,0), 9 is (-2,1), 10 is (2,1), 11 is (-2,2), 12 is (-1,2), 13 is (0,2), 14 is (1,2), 15 is (2,2). The pair is (dx, dz).

Low-resolution add (`DeveloperConsole.java:343`), 2 bits:

| Code | Body |
| --- | --- |
| 0 | Optional recursive update bit, then 6-bit local x, 6-bit local z, 1-bit mask flag. The player is created from the stored square. |
| 1 | 2 bits added to the stored level. Returns false, so the index stays low-resolution. |
| 2 | 5 bits: level delta `>> 3 & 3` and a 3-bit direction in the same order as the table above. |
| 3 | 20 bits: speed id `>> 18 & 3` stored as `id - 1`, level delta `>> 16 & 3`, square-x delta `>> 8 & 255`, square-z delta `& 255`. |

## Player masks

For each index flagged above, `doDeveloperConsole22` (`DeveloperConsole.java:468`) reads a u16be and discards it, then a u8. Bit 7 of that byte adds another u8 shifted 8. Bit 11 (2048) adds another u8 shifted 16. The following bits are then consumed in this order, not in numeric order (`DeveloperConsole.java:488`):

| Bit | Fields |
| --- | --- |
| 32 | u16be, 65535 means -1 |
| 1024 | u8 `AudioApi` id |
| 16384 | jstr |
| 4 | u16le id (65535 means -1), u32m-32222222b16, u8add128. Low 3 bits, next 4 bits (15 means -1), and bit 7 are passed to the sequence call with mode 0 |
| 8192 | u8 length, then that many bytes copied in reverse |
| 1048576 | i8, i8 sub128 (`doPositionedSound32222222b10`), i8 add128 (`doPositionedSound32222222bb`), u8add128, u16le-sub128, u16le-sub128 |
| 8388608 | u8add128, compared with 1 |
| 2 | i8 sub128, i8 add128, i8, i8 neg, i8 neg, i8 neg, u16be+128, u16be, u16le |
| 65536 | same shape as bit 4, with u32m-3322b6 and mode 4 |
| 64 | u16be+128 |
| 262144 | cursor += 2, u8 count, then per entry u8add128 type and `doPublicKeysPart` (unread) |
| 1 | u8 count; per entry smart `doPositionedSound32222222b4`. 32767 reads four more smarts. 32766 reads u8add128. Otherwise one more smart. Then one more smart. A second u8 count follows; each entry is two smarts, and a value other than 32767 adds another smart, u8sub128, and u8neg when the second smart is positive |
| 32768 | u16be+128 (65535 means -1), u32m-3322b6, u8sub128 split like bit 4, mode 2 |
| 2097152 | u16be (65535 means -1), u32m-3322b6, u8 split like bit 4, mode 3 |
| 8 | u8neg length, that many bytes copied in reverse, then appearance below |
| 131072 | cursor += 2, u8 count, per entry u8neg type and `doPublicKeysPart` (unread) |
| 4194304 | jstr, u8 |
| 524288 | u8sub128 count. Per entry u16be+128; if `(value & 49152) == 49152`, also u16le. Then u16le-sub128 |
| 4096 | u16le-sub128 (65535 means -1), u32le, u8neg split like bit 4, mode 1 |
| 512 | u8sub128 count. Per entry `doPositionedSound32222222b4b`, u8sub128, u16le-sub128 |
| 16 | four `doPositionedSound32222222b4b`, then u8sub128 |

## Appearance

Mask bit 8 calls `SoundJingleCutsceneActionPart.doSoundJingleCutsceneActionPart(PositionedSound)` (`deob/SoundJingleCutsceneActionPart.java:205`).

1. u8 flags. Bit 0 is the gender byte passed into the equipment decoder. Bit 2 selects the alternate id branch. Bits 3..5 are passed to `doSoundJingleCutsceneAction22` as `(flags >> 3 & 7) + 1`. Bit 6 reads jstr0 into `string3`. Bit 7 reads jstr0 into `string4`.
2. i8 defaults-group id.
3. Equipment, `doSoundJingleCutsceneActionPart(buf, gender)` (`SoundJingleCutsceneActionPart.java:91`). For each index in `Statics.clsfc26e159.intArray` whose value is not 1: u8. Zero leaves the slot empty. Otherwise another u8 forms `(first << 8) + second`. Slot 0 equal to 65535 reads `doPositionedSound32222222b4b` and a u8, then stops the slot loop. A value `>= 16384` is an object id `value - 16384`. A smaller nonzero value is an idkit `value - 256`. The array length was not read (`gaps.md`).
4. If slot 0 was not the 65535 case: u16be bitfield. Each set bit, over slots whose `intArray` entry is 0, reads a `DataBlock1569`. That decoder was not read.
5. 10 u8 values, then another 10 u8 values.
6. u16be `tint13`.
7. Back in the outer method: u8 `this.tint`, jstr name. If flag bit 2: u16be (65535 means -1) and the previous u8 is copied to `tint2`, `tint3 = -1`. Else `tint4 = 0`, u8 `tint2`, u8 `tint3` (255 means -1).
8. u8 `tint11`. If nonzero: five u16be and one u8.

## NPC movement

`Client.doClient3222b172` (`rs2/client/Client.java:8055`).

1. Bit-align. 8-bit count of existing NPCs kept. A count above the current total throws. The tail is removed.
2. For each kept NPC, 1 bit. `0` keeps it with no step. `1` then 2 bits:
   - 0: keep, and queue a mask.
   - 1: 3-bit direction, speed byte 1 (`DataBlock1396` id 2, `tbyte` 1), then 1 bit which queues a mask when set.
   - 2: 1 bit. If 1, two 3-bit directions each with speed byte 2 (`DataBlock1396` id 3). If 0, one 3-bit direction with speed byte 0 (`DataBlock1396` id 1). Then 1 mask bit.
   - 3: remove.

The 3-bit value is `ClientScriptHelpers.getId()`, which is the constructor's second argument (`com/jagex/game/script/ClientScriptHelpers.java:41`). Movement uses the constructor's first argument (`deob/SoundJingleCutsceneActionPart2.java:326`):

| Wire (`tint2`) | `tint` | Delta |
| --- | --- | --- |
| 0 | 3 | z+1 |
| 1 | 4 | x+1, z+1 |
| 2 | 2 | x+1 |
| 3 | 6 | x+1, z-1 |
| 4 | 5 | z-1 |
| 5 | 0 | x-1, z-1 |
| 6 | 7 | x-1 |
| 7 | 1 | x-1, z+1 |

This table is not the player table above.

New NPCs (`Client.doClient3222`, `Client.java:8119`) while at least 15 bits remain:

1. 15-bit index. 32767 stops the loop. That is the empty-list terminator.
2. 2-bit level.
3. `tint13` bits of signed z delta from the local player. A value above `(1 << (n-1)) - 1` subtracts `1 << n`.
4. 1 bit.
5. 3 bits plus 4, shifted left 11, masked with 16383.
6. 15-bit type id, loaded through `Statics.lookatSub2` (the NPC config list).
7. `tint13` bits of signed x delta from the local player.
8. 1 bit, which queues a mask.

Byte-align after the loop.

## NPC masks

`Client.doClient3222b82` (`Client.java:8185`). Per queued NPC: discarded u16be, then u8. Bit 1 adds a u8 shifted 8. Bit 9 (512) adds a u8 shifted 16. Bit 22 (4194304) adds a u8 shifted 24. Bits are consumed in this order:

| Bit | Fields |
| --- | --- |
| 1 | `doPositionedSound32222222b4b` type id. Flag bits 8388608 and 131072 are passed into the type call |
| 2097152 | u8 count. Per entry u8sub128, u32m-3322b6, u24le |
| 262144 | u8sub128 bitfield over 8 slots. A set bit reads `doPositionedSound32222222b4b` and `doPositionedSound2222` |
| 134217728 | i8, i8, i8 neg, u8sub128, u16be+128, u16le |
| 1048576 | u8 flags. Bit 0 skips the rest after a 2-byte cursor skip. Otherwise cursor += 2, then bit 2 reads one `doPositionedSound32222222b4b` per model, bit 4 reads u16le-sub128 per recolour, bit 8 reads u16be+128 per retexture. Those array lengths come from the NPC type |
| 16 | u16be+128, 65535 means -1 |
| 16384 | u8neg count. Per entry u16be; if `(value & 49152) == 49152`, another u16be. Then u16le-sub128 |
| 128 | u8 hit count, same smart layout as player bit 1, then u8neg second count. The second loop uses u8 instead of u8neg for the positive branch |
| 268435456 | u16be (65535 means -1), u32m-3322b6, u8 split like the player sequence byte, mode 4 |
| 1024 | u16be+128 (65535 means -1), u32le, u8neg sequence byte, mode 1 |
| 8388608 | jstr |
| 65536 | cursor += 2, u8 count, per entry u8sub128 type and `doPublicKeysPart` (unread) |
| 2048 | u16le, 65535 means -1 |
| 32 | four `doPositionedSound32222222b4b`, then u8sub128 |
| 8192 | i8, i8 sub128, i8, i8 neg, i8 sub128, i8 neg, u16le-sub128, u16be+128, u16le |
| 64 | u16le-sub128, u16le |
| 67108864 | u16le (65535 means -1), u32be, u8add128 sequence byte, mode 3 |
| 131072 | u16be, 65535 means the type's `tint5` |
| 33554432 | u16le-sub128 (65535 means -1), u32be, u8neg sequence byte, mode 2 |
| 8 | jstr |
| 32768 | u8sub128 flags. Bit 0 clears the block. Otherwise cursor += 2. Bit 2 reads model ids and, when bit 4 of the flags is also set, a float (`doPositionedSound22`) and six further values per model. Bit 4 of the low flags reads u16le per recolour. Bit 8 reads u16be per retexture. Lengths come from the NPC type |
| 4096 | u8add128 count. Per entry `doPositionedSound32222222b4b`, u8sub128, u16le-sub128 |
| 536870912 | u8add128 compared with 1 |
| 524288 | cursor += 2, u8 count, per entry u8neg type and `doPublicKeysPart` (unread) |
| 16777216 | u8neg |
| 4 | u16be (65535 means -1), u32be, u8 sequence byte, mode 0 |
