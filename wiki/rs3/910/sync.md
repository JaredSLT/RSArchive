# Player and NPC sync

Players are a 2048-slot array. The local player is placed by `ReceivePlayerPositions.receivePlayerPositions` (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/client/ReceivePlayerPositions.java:53`), called from the game-login continuation (`LoginManager.java:1028`) and from reply 15 (`LoginManager.java:1069`). Opcode 122 `PLAYER_INFO` calls `readPlayerInfo` (`Client.java:8942`).

## Login placement

`accessBits`, then `gBit(30)` for the local player: level is bits 28..29, world X is bits 14..27, world Z is bits 0..13. Scene tiles are those world coordinates minus `world.getBase()`. Then, for every index from 1 through 2047 except the local uid, `gBit(18)`: 2-bit level, 8-bit zone X, 8-bit zone Z, stored in `LowResPlayerInfo.coord`. `accessBytes` follows. Index 0 is never written by this loop. There is no zero terminator.

## PLAYER_INFO

`readPlayers` (`ReceivePlayerPositions.java:127`) does four bit passes, in order: high-resolution indexes whose `nsn` bit 0 is clear, high-resolution indexes whose bit 0 is set, low-resolution indexes whose bit 0 is set, low-resolution indexes whose bit 0 is clear. After the passes, high and low lists are rebuilt from indexes 1 through 2047 (`ReceivePlayerPositions.java:238`).

For each index, `gBit(1) == 0` means skip. `skip` (`ReceivePlayerPositions.java:256`) reads `gBit(2)`: 0 means count 0, 1 means `gBit(5)`, 2 means `gBit(8)`, 3 means `gBit(11)`. The current player is marked and the count covers that many further players. A leftover count throws. A skip of 0 ends the run for the next index. There is no single empty-list opcode.

`rslib/src/both/move/rev_910.c` always writes one high-resolution step and then skip 2045. That is a narrower encoder than this reader. See `gaps.md`.

## High-resolution movement

`readHighResolution` (`ReceivePlayerPositions.java:273`):

1. `gBit(1)` queues the index for a mask block when set.
2. `gBit(2)` is the type.
3. Type 0 removes the player from high resolution unless the extend bit was set. The local player with type 0 and no extend bit throws. If the extend bit was clear, a further `gBit(1)` may chain `readLowResolution`.
4. Type 1 reads `gBit(3)` direction and `gBit(1)`. When that bit is 1, `gBit(2)` is an extra step with speed `MoveSpeed.RUN.serialID`: 0 is z+1, 1 is x-1, 2 is x+1, 3 is z-1. The 3-bit direction is then applied. Directions 0..7 are SW, S, SE, W, E, NW, N, NE with `(dx,dz)` `(-1,-1), (0,-1), (1,-1), (-1,0), (1,0), (-1,1), (0,1), (1,1)`.
5. Type 2 reads `gBit(4)` and applies one of the 16 offsets in `ReceivePlayerPositions.java:360` through `ReceivePlayerPositions.java:404`.
6. Type 3 reads `gBit(1)`. When it is 0, `gBit(15)` is a relative teleport: 3-bit speed index, 2-bit level delta, signed 5-bit x, signed 5-bit z. When it is 1, `gBit(3)` is the speed index and `gBit(30)` is added to the current waypoint (`ReceivePlayerPositions.java:442`): plane bits added to `level`, 14-bit x added to world x, 14-bit z added to world z, then the region base is subtracted. `MoveSpeed.INSTANT.index` is 4 (`MoveSpeed.java:16`), so speed nibble 4 teleports.

`rslib/src/both/move/rev_910.c:213` writes that 30-bit word as absolute local coordinates and forces the extend bit clear. Java adds the fields to the current waypoint.

## Low-resolution movement

`readLowResolution` (`ReceivePlayerPositions.java:470`):

| `gBit(2)` | bits |
| --- | --- |
| 0 | optional chained update, `gBit(6)` x, `gBit(6)` z, `gBit(1)` mask flag, then the player is created from the stored zone coordinate |
| 1 | `gBit(2)` added to the stored level |
| 2 | `gBit(5)`: 2-bit level delta and a 3-bit direction in the same SW..NE order |
| 3 | `gBit(20)`: 2-bit speed, 2-bit level delta, 8-bit x delta, 8-bit z delta |

## Player masks

`readPlayerUpdates` (`ReceivePlayerPositions.java:595`) discards one `g2` per extended player, then reads `g1`. Bit 64 adds another `g1` shifted 8. Bit 4096 adds another `g1` shifted 16. Blocks are tested in this order (`readPlayerUpdatesBlocks`, `ReceivePlayerPositions.java:615`):

| bit | body |
| --- | --- |
| 128 | four `gSmart2or4s`, then `g1_alt2` |
| 262144 | skip 2 bytes, `u8` count, then count times `g1_alt3` plus `decodeVarValue` (decoder unread) |
| 16 | `g2_alt3` facing |
| 65536 | `g1_alt3` equals 1 |
| 32 | hitmarks and headbars using smart values; see the method |
| 32768 | `g1_alt3` count, then count times `gSmart2or4s`, `g1_alt1`, `g2_alt3` |
| 1048576 | spotanim slot 4: `g2_alt3`, `g4_alt1`, `g1_alt1` |
| 4 | `g1_alt1` length, that many bytes with each byte minus 128, then `PlayerEntity.getAppearance` (method unread) |
| 524288 | `g1_alt1` count, then count times `g2_alt1` and sometimes another `g2_alt1`, then `g2_alt2` |
| 8192 | spotanim slot 2: `g2`, `g4_alt2`, `g1_alt1` |
| 2 | spotanim slot 0: `g2_alt3`, `g4_alt3`, `g1_alt1` |
| 2048 | `g1` length, that many raw bytes, then `getHeadIcons` |
| 131072 | clear vars, skip 2, `u8` count, then `decodeVarValue` (unread) |
| 512 | `gjstr` chat |
| 8 | force move: `g1b_alt2`, `g1b`, `g1b_alt2`, `g1b_alt1`, `g1b_alt1`, `g1b`, `g2_alt3`, `g2_alt1`, `g2_alt1` |
| 1 | `g2` target, 65535 means -1 |
| 8388608 | `g1b_alt2`, `g1b_alt3`, `g1b_alt2`, `g1_alt1`, `g2`, `g2` |
| 1024 | `g1_alt2` community-partner id |
| 256 | spotanim slot 1: `g2_alt3`, `g4_alt2`, `g1_alt2` |
| 2097152 | `gjstr`, then `g1` |
| 4194304 | spotanim slot 3: `g2_alt3`, `g4s`, `g1_alt1` |

A spotanim `u16` of 65535 is stored as -1. The following `u8` splits into bits 0..2, bits 3..6 (15 means -1), and bit 7.

## NPCs

Opcode 186 calls `doClient36` (`Client.java:10474`).

`doClient372` (`Client.java:12079`): `gBit(8)` is how many existing NPCs to keep. A smaller count removes the rest. For each kept index, `gBit(1) == 0` keeps it idle. Otherwise `gBit(2)`: 0 keeps it and queues a mask, 1 walks one `gBit(3)` `CompassPoint` at walk speed and may queue a mask, 2 is either two run steps or one crawl step, 3 removes it. `CompassPoint.getId` is `serialID`, not the `values()` index (`CompassPoint.java:41`). `SerializableEnums.decode` matches that id (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/core/datastruct/SerializableEnums.java:39`). Wire ids are NORTH 0, NORTHWEST 1, NORTHEAST 2, SOUTHWEST 3, SOUTH 4, WEST 5, EAST 6, SOUTHEAST 7.

`doClient38` (`Client.java:12143`) adds NPCs while `bitsAvailable(packetSize) >= 15`. `gBit(15) == 32767` ends that section. Otherwise the 15 bits are the slot, then `gBit(2)` plane, `gBit(npcViewDistance)` signed z delta from the local player, `gBit(1)` mask flag, `gBit(15)` type id, `gBit(1)`, `gBit(3)` turned into an angle, and `gBit(npcViewDistance)` signed x delta.

`doClient392` (`Client.java:12209`) discards `g2`, reads `g1`, and extends on bit 2, bit 512, bit 1048576, and the fourth byte is included in the mask word. Bodies read here: bit 67108864 spotanim slot 3 (`g2_alt1`, `g4_alt3`, `g1`), bit 8192 spotanim slot 1 (`g2_alt1`, `g4_alt3`, `g1_alt3`), bit 4096 a `g1_alt3` count of `gSmart2or4s`, `g1`, `g2_alt2`. The method continues at `Client.java:12281`. Those later bodies were not read.
