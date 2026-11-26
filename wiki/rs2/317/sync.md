# Player and NPC sync

Opcode 81, size `-2`, calls `readPlayerInfo`. Opcode 65, size `-2`, calls `readNpcInfo`. Both require `packet.pos` to equal the payload size when they return.

`MAX_PLAYER_COUNT` is 2048. `LOCAL_PLAYER_INDEX` is 2047. The NPC array length is 16384. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:708`.

Bits are MSB-first. See [framing.md](framing.md). `PathingEntity.move(x, z, jump)` takes x first and z second. `PathingEntity.java:71`.

## Local player

`readLocalPlayer` at `Client.java:10268`.

`gBit(1)` is the update flag. Zero ends this section. Otherwise `gBit(2)`:

| Value | Bits |
| --- | --- |
| 0 | no further bits; the local player is queued for a mask |
| 1 | `gBit(3)` walk direction, `gBit(1)` mask flag. `step(false, dir)` |
| 2 | two `gBit(3)` run directions, each `step(true, dir)`, then `gBit(1)` mask flag |
| 3 | `gBit(2)` level, `gBit(1)` jump, `gBit(1)` mask, `gBit(7)` z, `gBit(7)` x. `move(x, z, jump)` |

A 5-bit or 7-bit delta is signed by subtracting 32 when the raw value is above 15. Teleport uses absolute local tiles, not a delta.

## Other players

`readPlayers` at `Client.java:11154`.

`gBit(8)` is the kept count. Players past that count are removed. A count above `playerCount` throws. Each kept player: `gBit(1)` changed. Zero keeps the player. One, then `gBit(2)`:

| Value | Bits |
| --- | --- |
| 0 | keep, and queue a mask |
| 1 | `gBit(3)` walk, `gBit(1)` mask |
| 2 | two `gBit(3)` run steps, `gBit(1)` mask |
| 3 | remove |

## New players

`readNewPlayers` at `Client.java:8327`. While `bitPos + 10 < size * 8`: `gBit(11)` pid. Value 2047 breaks the loop. Then `gBit(1)` mask, `gBit(1)` jump, `gBit(5)` signed z, `gBit(5)` signed x. `move(local.pathTileX[0] + x, local.pathTileZ[0] + z, jump)`. Then `accessBytes`.

## Player masks

`readPlayerUpdates` at `Client.java:3203` reads `u8`. If bit `0x40` is set, a second `u8` is shifted left 8 and added. Bit `0x40` is the extension flag, not a mask. The mask is then consumed in this order at `Client.java:9674`:

| Bit | Fields |
| --- | --- |
| 0x400 | four `u8_sub` startX, startZ, endX, endZ; `u16le_add` end cycle; `u16be_add` start cycle; `u8_sub` direction |
| 0x100 | `u16le` spotanim id (65535 becomes -1); `i32be` with height in the high 16 bits and delay in the low 16 bits |
| 0x8 | `u16le` sequence (65535 becomes -1); `u8_neg` delay |
| 0x4 | `jstr`. A leading `~` is stripped and the text is also added as chat |
| 0x80 | `u16le` colour in the high byte and style in the low byte; `u8` privilege; `u8_neg` length; `bytes_rev(length)`; `WordPack.unpack` |
| 0x1 | `u16le` face entity, 65535 becomes -1 |
| 0x10 | `u8_neg` length; `bytes(length)` passed to `PlayerEntity.read` |
| 0x2 | `u16le_add` target tile x; `u16le` target tile z |
| 0x20 | `u8` damage; `u8_add` type; `u8_neg` health; `u8` total |
| 0x200 | `u8` damage; `u8_sub` type; `u8` health; `u8_neg` total |

### Appearance

`PlayerEntity.read` at `PlayerEntity.java:132`.

`u8` gender, `u8` headicons, then up to 12 slots. Each slot is `u8`. Zero stores an empty slot. Otherwise another `u8`, and the appearance is `(first << 8) + second`. If slot 0 is 65535, `u16be` is an npc id and the slot loop stops. Then five `u8` colours, forced into range. Then seven `u16be` sequences: stand, turn, walk, turn around, turn left, turn right, run. 65535 becomes -1. Then `u64be` name, `u8` combat level, `u16be` skill.

## Step directions

`PathingEntity.step` at `PathingEntity.java:141`. `pathTileX` is x and `pathTileZ` is z. The 3-bit direction changes the current tile by:

| Dir | x | z |
| --- | --- | --- |
| 0 | -1 | +1 |
| 1 | 0 | +1 |
| 2 | +1 | +1 |
| 3 | -1 | 0 |
| 4 | +1 | 0 |
| 5 | -1 | -1 |
| 6 | 0 | -1 |
| 7 | +1 | -1 |

If the primary sequence has `postanim_move == 1`, that sequence is cleared. The path length stays at most 9.

## NPCs

`readNpcs` at `Client.java:11633` uses the same bit layout as other players. The 8-bit count and the ids are NPC slots.

`readNewNpcs` at `Client.java:2933`. While `bitPos + 21 < size * 8`: `gBit(14)` id. Value 16383 ends the loop. Then `gBit(5)` signed z, `gBit(5)` signed x, `gBit(1)` jump, `gBit(12)` type, `gBit(1)` mask. `move(local.pathTileX[0] + x, local.pathTileZ[0] + z, jump)`. Then `accessBytes`.

`readNpcUpdates` at `Client.java:7455` reads one `u8`. Bit `0x40` is a real mask. Order:

| Bit | Fields |
| --- | --- |
| 0x10 | `u16le` sequence (65535 becomes -1); `u8` delay |
| 0x8 | `u8_add` damage; `u8_neg` type; `u8_add` health; `u8` total |
| 0x80 | `u16be` spotanim (65535 becomes -1); `i32be` height and delay packed as for players |
| 0x20 | `u16be` face entity, 65535 becomes -1 |
| 0x1 | `jstr`, chat timer 100 |
| 0x40 | `u8_neg` damage; `u8_sub` type; `u8_sub` health; `u8_neg` total |
| 0x2 | `u16le_add` npc id. Size and sequences are copied from the type |
| 0x4 | `u16le` target tile x; `u16le` target tile z |
