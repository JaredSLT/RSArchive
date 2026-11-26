# Sync

Players are `CutsceneAction_Sub1Part` in `Client.cls89a651f2Array` (2048 slots). NPCs are `BrokenInputStream` inside `CoverMarkerClickbox` on `Client.whirlpool`.

Server packet opcode 1, size -2, calls `ServerConnection.doServerConnection(buf, payloadLen)` (`builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6700`). Opcode 153, size -2, calls `Client.doClient2222b9` (`Statics.java:3988`).

`intArray9` is x. `intArray10` is z. Single-step z signs are the opposite of `rslib/src/both/move/rev_887.c` `k_dz`. See [gaps.md](gaps.md).

## Initial player snapshot

`ServerConnection.doServerConnection(PositionedSoundPart)` at `builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/network/ServerConnection.java:54`.

The local index is the existing `Client.tint76`. This method does not read it. It reads bit(30):

| bits | use |
| --- | --- |
| 2 | `>> 28`, stored as `tbyte` |
| 14 | `>> 14 & 16383`, x minus `base.tint2` |
| 14 | `& 16383`, z minus `base.tint3` |

Then for every index from 1 through 2047 except the local index, bit(18): level `>> 16 & 3`, zone x `>> 8 & 255`, zone z `& 255`, packed as `(level << 28) + (zoneX << 14) + zoneZ` (`:87`). Then byte-align (`:102`).

## Per-tick player movement

`doServerConnection(buf, len)` calls `doServerConnection2` then `Statics.doStatics` and requires the buffer offset to equal the payload length (`ServerConnection.java:118`).

`doServerConnection2` (`:128`) byte-aligns around four passes. Skip counts use `doServerConnection22` (`:257`): bit(2) selects 0, then bit(5), bit(8), or bit(11).

| pass | list | low bit of `byteArray[slot]` |
| --- | --- | --- |
| 1 | active `intArray` | clear |
| 2 | active | set |
| 3 | inactive `intArray2` | set |
| 4 | inactive | clear |

Each considered slot: if a skip counter is running, decrement it and set bit 1 of the flag byte. Else bit(1). Zero starts a skip. One calls `Statics.doStatics(buf, index)` for an active slot, or `doServerConnection2(buf, index)` for an inactive slot. The inactive method was not read.

After the passes, slots 1 through 2047 are split again into active (entity non-null) and inactive (`:239`). Slot 0 is not in that rebuild.

`Statics.doStatics(buf, index)` (`Statics.java:2233`):

1. bit(1) needs a mask. If set, append the index.
2. bit(2) type.
3. Type 0 and the mask bit was clear: remove the player. Removing `Client.tint76` throws. Zone coords are stored from the current tile. If another bit(1) is set, `doServerConnection2` runs.
4. Type 1: bit(3) direction, bit(1) extra. If the extra bit is set, bit(2) selects a half step first: 0 z+1, 1 x-1, 2 x+1, 3 z-1 (`:2278`). Then the 3-bit step, with x = `intArray9` and z = `intArray10` (`:2293`):

| dir | dx | dz |
| --- | --- | --- |
| 0 | -1 | -1 |
| 1 | 0 | -1 |
| 2 | +1 | -1 |
| 3 | -1 | 0 |
| 4 | +1 | 0 |
| 5 | -1 | +1 |
| 6 | 0 | +1 |
| 7 | +1 | +1 |

5. Type 2: bit(4). Deltas (`:2320`), x then z:

| code | dx | dz | code | dx | dz |
| --- | --- | --- | --- | --- | --- |
| 0 | -2 | -2 | 8 | +2 | 0 |
| 1 | -1 | -2 | 9 | -2 | +1 |
| 2 | 0 | -2 | 10 | +2 | +1 |
| 3 | +1 | -2 | 11 | -2 | +2 |
| 4 | +2 | -2 | 12 | -1 | +2 |
| 5 | -2 | -1 | 13 | 0 | +2 |
| 6 | +2 | -1 | 14 | +1 | +2 |
| 7 | -2 | 0 | 15 | +2 | +2 |

This 16-entry table matches `k_run_dx` and `k_run_dz` in `rslib/src/both/move/rev_887.c`.

6. Type 3: bit(1). If 0, bit(15): level 3 bits (`>> 12 & 7`), turn 2 bits, dx 5 bits signed (`>> 5 & 31`, values above 15 subtract 32), dz 5 bits signed. If 1, bit(3) then bit(30): level `>> 28 & 3`, dx `>> 14 & 16383`, dz `& 16383`, added to the current tile and wrapped with the base (`:2401`). The 3-bit value is compared to `Unused24.unused245.tint`, which was not read.

## Player masks

`Statics.doStatics(PositionedSoundPart)` (`Statics.java:2871`) for each queued index:

1. Discard one u16be (`doPositionedSound322`, return ignored).
2. u8 mask.
3. If bit 0 is set, add the next u8 shifted 8.
4. If bit 1024 is set, add the next u8 shifted 16.

`ServerConnection.doServerConnection(buf, index, player, mask)` (`ServerConnection.java:399`) tests bits in this order. Widths below are the ones read in that method.

| bit | bytes read in this pass |
| --- | --- |
| 131072 | skip 2, u8 count, then per entry u8_neg and `doDataBlock721` (unread) |
| 1048576 | clear, skip 2, u8 count, u8_sub128 and `doDataBlock721` |
| 8388608 | u16be (65535 is -1), u32 `doPositionedSound4222b7`, u8 flags |
| 32768 | u8 length, then `doPositionedSound2` (reverse, minus 128) into that array |
| 2097152 | u8_add128 count; each value is u16be_low128, plus another u16be when the high bits are 49152; then another u16be_low128 |
| 16384 | jstr |
| 262144 | i8_neg, i8_sub128, i8_neg, u8_sub128, u16be_low128, u16be_low128 |
| 4194304 | jstr, u8_add128 |
| 524288 | u16le_low128 (65535 is -1), u32be, u8_sub128 flags |

Bits 512, 256, 64, 16, 8192, 4096, 8, 2, 32, 65536, 4, and 128 are tested from `ServerConnection.java:518` through `:689`. Those bodies were not transcribed.

RSLib `put_move` writes the mask bit as 0 (`rslib/src/both/move/rev_887.c:246`). This client reads the mask bytes.

## NPC update

`Client.doClient2222b9` (`builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:8000`) runs existing NPCs, new NPCs, then masks. The buffer offset must equal the payload length (`:8054`).

Existing, `doClient2222b422` (`:8077`): bit(8) count. A count below the current list queues the extras for removal. A larger count throws. For each counted index: bit(1) zero keeps it. Else bit(2):

| value | effect |
| --- | --- |
| 0 | keep, and queue a mask |
| 1 | keep; bit(3) direction; opcode `unused243`; bit(1) queues a mask |
| 2 | keep; bit(1) selects two bit(3) directions with opcode `unused244`, or one bit(3) with opcode `unused242`; bit(1) queues a mask |
| 3 | remove |

Direction enum `QuestCommands` was not read.

New, `doClient2222b13` (`:8141`), while remaining bits are at least 15:

| bits | use |
| --- | --- |
| 15 | index. 32767 breaks. This is the terminator |
| 3 | plus 4, shifted left 11, masked to 16383 |
| 2 | level |
| 15 | type id |
| npc view | signed delta added to local z (`intArray10`) |
| 1 | queue a mask |
| npc view | signed delta added to local x (`intArray9`) |
| 1 | passed into `doBrokenInputStream` |

Npc view width is the argument, `Client.world.tint13`. Signed means values above `(1 << (n-1)) - 1` subtract `1 << n`. Then byte-align (`:8203`).

Masks, `doClient2222b32` (`:8207`): for each queued index, discard u16be, read u8. Bit 64 adds another u8 shifted 8. Bit 16384 adds a u8 shifted 16. Bit 4194304 adds a u8 shifted 24. Bit 1 starts a hitsplat loop using unsigned smarts, with 32767 and 32766 as sentinels (`:8227`). Later mask bits in that method were not transcribed.
