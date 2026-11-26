# Sync

Player capacity is 2048. The local index is `valueClient185` = 2047 (`Client.java:666`). NPC slots are 16384 (`Client.java:10428`). Bits are MSB-first via `Packet.opkind2724`. Directions are `PathingEntity.opkind552` (`PathingEntity.java:108`):

| bits | x | z |
| --- | --- | --- |
| 0 | -1 | +1 |
| 1 | 0 | +1 |
| 2 | +1 | +1 |
| 3 | -1 | 0 |
| 4 | +1 | 0 |
| 5 | -1 | -1 |
| 6 | 0 | -1 |
| 7 | +1 | -1 |

The third argument of `opkind552` is stored on the step as the run flag. A teleport or a delta outside ±8 tiles sets the tile immediately (`PathingEntity.opkind551`, `PathingEntity.java:68`). `opkind553` clears the step queue.

## Players

S2C opcode 167, size `-2`. `Client.read` calls `readPlayerInfo` (`Client.java:6820`, method at `:7811`). Order:

1. `opClient41` local player, already entering bit mode (`Client.java:4145`).
2. `readPlayers` other players (`Client.java:8589`).
3. `getPlayerNewVis` new players (`Client.java:1457`).
4. `opClient14` masks, after byte-align (`Client.java:9183`).

The packet is consumed exactly: `valuekind2715` must equal the payload length (`Client.java:7828`).

### Local player

`opClient41` starts bits.

- 1 bit present. 0 means no local movement.
- If 1, 2-bit type:
  - 0: queue local index 2047 for a mask only.
  - 1: 3-bit walk direction, 1-bit update. Update 1 queues the local index.
  - 2: 3-bit walk, 3-bit run, 1-bit update.
  - 3: teleport. 2-bit plane into `valueClient143`, 7-bit local x, 7-bit local z, 1-bit snap (`opkind551` second argument), 1-bit update.

### Other players

`readPlayers` reads an 8-bit count. If the count is below the previous list, the extra ids are removed. If the count is above the previous list, the client throws. For each kept slot:

- 1 bit unchanged. 0 keeps the player with no movement.
- If 1, a 2-bit type: 0 keeps and queues a mask; 1 is a 3-bit walk plus a 1-bit mask flag; 2 is a 3-bit walk, a 3-bit run, and a 1-bit mask flag; 3 removes the player.

### New players

`getPlayerNewVis` loops while `bitpos + 10 < payloadBits`. Each entry is an 11-bit id. 2047 ends the list, then the stream is byte-aligned. Otherwise: 5-bit dx (subtract 32 when above 15), 5-bit dz the same way, 1-bit discard-walk, 1-bit needs mask. Position is the local player's current tile plus dx and dz. A new player applies a stored appearance buffer `valueClient191[id]` when one exists.

### Player masks

`opClient14` reads `u8` mask per queued id. If bit 7 is set, it adds the next `u8 << 8`. Bit 7 (value 128) is only that extension. `readPlayerUpdates` (`Client.java:4259`) then reads, in this order:

| bit | fields |
| --- | --- |
| 1 | `u8` length, then that many bytes, passed to `PlayerEntity.opkind111` |
| 2 | `u16be` sequence (65535 becomes -1), `u8` delay |
| 4 | `u16be` spotanim (65535 becomes -1) |
| 8 | `jstr` forced chat |
| 16 | `u8` type, `u8` damage, then `u8`, `u8` |
| 32 | `u16be` face entity, `u16be` |
| 64 | `u16be` colour and effects, `u8` rights, `u8` packed length, then `WordPack.opkind591` of that length; the position is restored to the start of the packed bytes plus the length |
| 256 | `u16be` graphic (65535 becomes -1), `u32be` packed (high 16 height, low 16 delay added to `valueClient169`) |
| 512 | four `u8`, two `u16be` added to `valueClient169`, `u8` direction, then the path queue is cleared |
| 1024 | same four `u8` shape as bit 16 |

### Appearance

`PlayerEntity.opkind111` (`PlayerEntity.java:38`) resets its own position to 0 and reads:

- `u8` gender
- `u8` head icon
- 12 slots. `u8` 0 means empty. Otherwise `u8` high and `u8` low form `(high << 8) + low`. If slot 0 equals 65535, the next field is `u16be` npc id and the slot loop stops.
- 5 `u8` colours. A value outside `Client.valueClient238[slot]` becomes 0.
- 7 `u16be` animations: standing, turn, walk, turn 180, turn 90 clockwise, turn 90 counter-clockwise, run. 65535 becomes -1.
- `u64be` name, decoded with `JString.opkind562`
- `u8` combat level
- `u16be` extra

Design defaults are not the constants 0, 10, 18, 26, 33, 36, 42. `validateCharacterDesign` (`Client.java:9966`) selects, for each of 7 slots, the first `IdkType` that is enabled and whose body type equals the slot, plus 7 when `valueClient298` is false.

## NPCs

S2C opcode 197, size `-2`. `readNpcInfo` (`Client.java:5934`) calls `readNpcs`, then `opClient28`, then `opClient2`.

`readNpcs` (`Client.java:5250`) starts bits and uses the same 8-bit count and 1-bit / 2-bit movement as other players. Ids come from the existing NPC list. There is no 11-bit id in this section. Type 3 removes the NPC.

`opClient28` (`Client.java:1970`) loops while `bitpos + 21 < payloadBits`. 14-bit id. 16383 ends the list and byte-aligns. Otherwise: 11-bit type, 5-bit signed dx, 5-bit signed dz, 1-bit discard, 1-bit mask.

`opClient2` (`Client.java:5382`) reads one `u8` mask. There is no extension byte. Order:

| bit | fields |
| --- | --- |
| 1 | `u8`, `u8`, `u8`, `u8` hit |
| 2 | `u16be` sequence (65535 becomes -1), `u8` delay |
| 4 | `u16be` spotanim (65535 becomes -1) |
| 8 | `jstr` forced chat |
| 16 | `u8`, `u8`, `u8`, `u8` second hit |
| 32 | `u16be` npc type |
| 64 | `u16be` graphic (65535 becomes -1), `u32be` packed the same way as the player graphic |
| 128 | `u16be`, `u16be` face |

`rslib/src/both/move/rev_282.c` encodes only the local player, forces the other-player count to 0, and reads only appearance bit 1. The Java order and the other mask bits are the ones above.
