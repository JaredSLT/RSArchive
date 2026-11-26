# Sync

Player list capacity is 2048 (`valueClient765`, `builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:145`). The local player index is 2047 (`valueClient766`, `Client.java:146`). NPC slots are `valueClient602`, length 16384 (`Client.java:75`).

`valuekind918` is tile X and `valuekind919` is tile Z (`PathingEntity.opkind913`, `builds/rs2/350-2005-12-30/src/src/main/java/deob/PathingEntity.java:103`). Direction `bits(3)`:

| bits | tile step |
| --- | --- |
| 0 | x-1, z+1 |
| 1 | z+1 |
| 2 | x+1, z+1 |
| 3 | x-1 |
| 4 | x+1 |
| 5 | x-1, z-1 |
| 6 | z-1 |
| 7 | x+1, z-1 |

The boolean passed to `opkind913` is stored as `valuekind9131[0]`. A walk step passes false. A run step passes true.

`opkind914(x, z, teleportFlag)` (`PathingEntity.java:166`): when the flag is clear and both deltas are inside -8..8, the move is queued as a step. Otherwise the path is cleared and the tile is snapped. Fine coordinates become `tile * 128 + size * 64`.

## Players

Opcode 37, size `-2`. `Client.readPlayerInfo` (`Client.java:1084`) order:

1. `opClient64` local player (`Client.java:3806`).
2. `readPlayers` other players already in the list (`Client.java:9384`).
3. `opClient74` new players (`Client.java:7933`).
4. `opClient65` masks (`Client.java:6309`).

After the masks, `valuekind8722` must equal the payload length (`Client.java:1101`).

### Local player

`opkind8723` then `bits(1)`. Zero means no movement. Otherwise `bits(2)`:

| value | body |
| --- | --- |
| 0 | queue the local index for a mask and stop |
| 1 | `bits(3)` direction, `opkind913(dir, false)`, `bits(1)` mask flag |
| 2 | two `bits(3)` directions, each `opkind913(dir, true)`, then `bits(1)` mask flag |
| 3 | `bits(7)` x, `bits(7)` z, `bits(2)` level into `valueClient935`, `bits(1)` teleport flag, `bits(1)` mask flag, then `opkind914(x, z, flag==1)` |

### Other players

`bits(8)` count. Players past that count are removed. For each kept index: `bits(1)`. Zero keeps the player with no mask. One is followed by `bits(2)`:

| value | body |
| --- | --- |
| 0 | keep, queue a mask |
| 1 | `bits(3)` walk, `bits(1)` mask flag |
| 2 | two `bits(3)` run steps, `bits(1)` mask flag |
| 3 | remove |

### New players

While `bitpos + 10 < size*8` (`Client.java:7936`):

1. `bits(11)` index. 2047 ends the loop.
2. `bits(5)` signed z offset. Values above 15 subtract 32.
3. `bits(1)` mask flag.
4. `bits(5)` signed x offset. Values above 15 subtract 32.
5. `bits(1)` teleport flag.
6. `opkind914(localX + xOffset, localZ + zOffset, flag==1)`.

Then `opkind8725` byte-aligns.

### Player masks

`opClient65` reads one `u8`. If bit 4 is set, it adds another `u8` shifted left 8 (`Client.java:6315`). Bit 4 is only the extension flag. `readPlayerUpdates` (`Client.java:3049`) tests the combined mask in this order:

| bit | fields |
| --- | --- |
| 32 | `u8-128` length, `bytes(n)`, then `PlayerEntity.opkind663` |
| 256 | `u16le+128` graphic id (65535 becomes -1), `u32mixA` (high 16 stored as `valuekind915`, low 16 added to the cycle) |
| 1 | `u16le` sequence (65535 becomes -1), `u8-128` delay |
| 2 | `u16be` `valuekind9147`, `u16le` `valuekind9148` |
| 1024 | `u8neg`, `u8sub`, `u8-128`, `u8-128`, `u16le`, `u16be+128`, `u8`, then `opkind912` |
| 8 | `jstr`. A leading `~` is stripped before the public chat line |
| 128 | `u16be` face (65535 becomes -1) |
| 512 | `u8`, `u8sub`, then later `u8neg`, `u8sub` |
| 64 | `u16le+128` (high byte color, low byte effect), `u8` rights, `u8sub` length, `opkind8752` of that length, then `WordPack.opkind771` |
| 16 | `u8sub`, `u8-128`, then later `u8`, `u8sub` |

### Appearance

`PlayerEntity.opkind663` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/PlayerEntity.java:345`), buffer rewound to 0:

1. `u8` gender.
2. `s8`, `s8`.
3. Twelve slots. Each starts with `u8`. Zero stores equipment 0. Otherwise another `u8` forms `(first<<8)+second`. If slot 0 is 65535, the next field is `u16be` npc id and the loop stops.
4. Five `u8` colors. A value outside `Client.valueClient658[i]` becomes 0.
5. Seven `u16be` sequences, 65535 stored as -1: `valuekind9116`, `valuekind9117`, `valuekind9127`, `valuekind9128`, `valuekind9129`, `valuekind9130`, `valuekind9149`.
6. `u64be` name, `u8` combat, `u16be`.

Slots 5 and 9 are swapped while the appearance hash is built, then swapped back (`PlayerEntity.java:424`).

## NPCs

Opcode 172, size `-2`. `Client.readNpcInfo` (`Client.java:7449`) order: `readNpcs`, `opClient91`, `opClient78`. The entity class is `C3c3d4b0f`.

### Existing NPCs

`readNpcs` (`Client.java:11789`) matches the other-player block: `bits(8)` count, then the same `bits(1)` and `bits(2)` cases, using `opkind913`. There is no local-NPC block.

### New NPCs

While `bitpos + 21 < size*8` (`Client.java:13751`):

1. `bits(14)` index. 16383 ends the loop.
2. `bits(1)` teleport flag passed to `opkind914`.
3. `bits(13)` type id. `NpcType.get` stores `valuekind825`.
4. `bits(1)` mask flag.
5. `bits(5)` signed z offset.
6. `bits(5)` signed x offset.
7. `opkind914(localX + xOffset, localZ + zOffset, flag==1)`.

Then byte-align.

### NPC masks

One `u8` only (`Client.java:9140`). Order:

| bit | fields |
| --- | --- |
| 64 | `jstr` |
| 32 | `u16le` `valuekind9147`, `u16be` `valuekind9148` |
| 1 | `u8neg`, `u8neg`, then later `u8neg`, `u8-128` |
| 2 | `u8neg`, `u8-128`, then later `u8sub`, `u8` |
| 128 | `u16be+128` graphic (65535 becomes -1), `u32mixA` |
| 16 | `u16le+128` type id |
| 4 | `u16be+128` face (65535 becomes -1) |
| 8 | `u16le` sequence (65535 becomes -1), `u8neg` delay |
