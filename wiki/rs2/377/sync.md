# Player and NPC sync

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`.

S2C 90, size `-2`, calls `readPlayerInfo` (`jagex2/client/Client.java:3788`). S2C 71, size `-2`, calls `readNpcInfo` (`Client.java:3644`). Both start with `accessBits` and end with `accessBytes` before the mask bytes.

The local player index is 2047 (`Client.java:771`). Other-player insertion stops when `gBit(11)` is 2047 (`Client.java:1014`). New NPCs stop when `gBit(14)` is 16383 (`Client.java:12352`).

## Directions

`PathingEntity.opPathingEntity2` changes tile x (`valuePathingEntity9`) and tile z (`valuePathingEntity10`) (`jagex2/dash3d/PathingEntity.java:88`). The second argument true is a run step.

| bits | step |
| --- | --- |
| 0 | x-1, z+1 |
| 1 | z+1 |
| 2 | x+1, z+1 |
| 3 | x-1 |
| 4 | x+1 |
| 5 | x-1, z-1 |
| 6 | z-1 |
| 7 | x+1, z-1 |

`opPathingEntity4(z, dummy, force, x)` (`PathingEntity.java:168`): when `force` is false and both deltas are in -8..8, enqueue one non-running step. Otherwise clear the queue and set the tile. World fine coordinates are `tile * 128 + size * 64` only on the teleport branch (`PathingEntity.java:201`).

## Local player

`opClient138` (`Client.java:4346`), still inside the bit stream opened by `readPlayerInfo`:

| bits | meaning |
| --- | --- |
| 1 | no update when 0 |
| 2 | type |
| type 0 | queue the local index 2047 for masks, no movement |
| type 1 | `gBit(3)` walk direction, `gBit(1)` mask |
| type 2 | two `gBit(3)` directions, both run, then `gBit(1)` mask |
| type 3 | `gBit(1)` force flag, `gBit(2)` level `valueClient1219`, `gBit(7)` z, `gBit(7)` x, `gBit(1)` mask, then `opPathingEntity4(z, …, flag==1, x)` (`Client.java:4373`) |

## Existing players and NPCs

`readPlayers` (`Client.java:10267`) and `readNpcs` (`Client.java:4748`):

`gBit(8)` count. A count above the current list throws. For each previous slot, in list order: `gBit(1)` 0 keeps the slot. Otherwise `gBit(2)`: 0 keep and queue a mask, 1 walk `gBit(3)` plus `gBit(1)` mask, 2 two run `gBit(3)` plus `gBit(1)` mask, 3 remove. Slots past the count are removed.

## New players

`opClient129` (`Client.java:1010`). The loop condition is `bitpos + 10 < size * 8`. Each record:

| bits | meaning |
| --- | --- |
| 11 | index, 2047 ends the list |
| 5 | signed dx added to the local player's x; values above 15 subtract 32 |
| 1 | queue a mask |
| 1 | teleport flag passed to `opPathingEntity4` |
| 5 | signed dz added to the local player's z |

A cached appearance buffer for that index is applied before the placement (`Client.java:1018`).

## New NPCs

`opClient122` (`Client.java:12348`). The loop condition is `bitpos + 21 < size * 8`. The body is 14+1+5+5+1+13 bits. The client uses the 21-bit test as written.

| bits | meaning |
| --- | --- |
| 14 | index, 16383 ends the list |
| 1 | queue a mask |
| 5 | signed delta added to local z |
| 5 | signed delta added to local x |
| 1 | teleport flag |
| 13 | `NpcType` id |

## Player masks

`opClient137` (`Client.java:4326`) reads `u8`. When bit `0x20` is set it reads another `u8` and adds it `<< 8`. Bit `0x20` is only that extension. `readPlayerUpdates` (`Client.java:5725`) tests the other bits in this order:

| bit | fields |
| --- | --- |
| 8 | `u16be` seq, 65535 means -1; `u8` `g1_alt3` delay |
| 16 | `jstr`. A leading `~` is stripped and the line is shown as chat type 2. The local player also shows it. Overhead timer 150 |
| 256 | `g1_alt1`, `g1_alt2`, `g1_alt3`, `g1`, `u16be` start, `g2_alt2` end, `g1`, then `opPathingEntity1` clears the walk queue |
| 1 | `g2_alt2` spotanim, 65535 means -1 |
| 2 | `u16be`, `u16be` face coordinates |
| 512 | `g2_alt2` id, 65535 means -1; `g4_alt2` with high 16 height and low 16 delay added to the tick |
| 4 | `u8` length, `gdata_alt1` of that many bytes, stored and passed to `PlayerEntity.opPlayerEntity5` |
| 1024 | `g1_alt1`, `g1_alt3`, `g1_alt2`, `g1` |
| 64 | `u16be` colour in the high byte and effect in the low byte; `g1_alt2` rights; `g1_alt1` packed length; `gdata_alt2` of that length; `WordPack.opWordPack1`. Rights 1 uses `@cr1@` and chat type 1. Rights 2 or 3 use `@cr2@` and type 1. Otherwise type 2. Skipped when the name is ignored or the public filter is off |
| 128 | `g1_alt3`, `g1_alt2`, `g1_alt3`, `g1` |

## Appearance

`PlayerEntity.opPlayerEntity5` (`jagex2/dash3d/PlayerEntity.java:353`) is variable length:

| order | fields |
| --- | --- |
| 1 | `u8` gender |
| 2 | `i8`, `i8` |
| 3 | 12 slots. Each slot starts with `u8`. 0 stores 0. Otherwise a second `u8` forms `(first<<8)+second`. Slot 0 equal to 65535 reads `u16be` npc id and stops the slot loop |
| 4 | five `u8` colours, each clamped to `valueClient1140[i].length` |
| 5 | seven `u16be` seq ids, 65535 means -1: stand, turn, walk, turn-180, turn-90 clockwise, turn-90 counter-clockwise, run |
| 6 | `u64be` name, `u8` combat level, `u16be` skill or total |

Slots 5 and 9 are swapped for the appearance hash and then swapped back (`PlayerEntity.java:432`).

Design defaults are not the constants 0, 10, 18, 26, 33, 36, 42. `opClient132` (`Client.java:1518`) picks, for each of seven slots, the first `IdkType` whose part equals `slot + (male ? 0 : 7)` and whose disable flag is false. Male is `valueClient1271`, which login sets true. The C2S appearance packet writes gender 0 for male and 1 for female (`Client.java:5445`).

## NPC masks

`readNpcUpdates` (`Client.java:5619`) reads one `u8` and tests in this order. There is no extension byte.

| bit | fields |
| --- | --- |
| 1 | `g2_alt2` `NpcType` id, then copy size and seq fields from the type |
| 64 | `g2_alt1` graphic, 65535 means -1 |
| 128 | `g1_alt1`, `g1_alt1`, hit, timer plus 300, `g1`, `g1_alt3` |
| 4 | `u16be` id, `g4_alt2` packed as the player bit 512, 65535 means -1 |
| 32 | `jstr`, overhead timer 100 |
| 8 | `g2_alt3`, `g2_alt1` |
| 2 | `u16be` seq, 65535 means -1, then `g1_alt3` delay. Restart rules match the player bit 8 and use `SeqType` |
| 16 | `g1_alt3`, `g1_alt3`, timer plus 300, `g1`, `g1_alt2` |
