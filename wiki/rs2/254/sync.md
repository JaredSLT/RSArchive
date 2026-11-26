# Sync

Player update is server opcode 87, size `-2` (`Client.read` at `builds/rs2/254-2004-09-07/src/src/main/java/jagex2/client/Client.java:3709`). NPC update is server opcode 123, size `-2` (`Client.java:3538`). Both payloads are bit-packed from the start of the payload. `Packet.bits` sets the bit cursor to `pos * 8` (`Packet.java:308`). `gBit` takes the high bits of the current byte first (`Packet.java:317`). `bytes` sets `pos` to `(bitPos + 7) / 8` (`Packet.java:342`).

`MAX_PLAYER_COUNT` is 2048 and `LOCAL_PLAYER_INDEX` is 2047 (`Client.java:688`). The local player is stored at that index (`Client.java:4207`). NPC ids used by the new-NPC loop are 14 bits.

## Local player

`readPlayerInfo` (`Client.java:11617`) calls `getPlayerLocal`, `readPlayers`, `getPlayerNewVis`, then `projectFromGround`.

`getPlayerLocal` (`Client.java:5090`):

| Bits | Meaning |
| --- | --- |
| 1 | 0 means no local change. 1 continues. |
| 2 | 0: mask only. 1: one step. 2: two steps. 3: teleport. |
| 3, then 1 | Code 1. Direction, then a mask flag. `moveCode(false, dir)` (`PathingEntity.java:111`). |
| 3, 3, then 1 | Code 2. Two directions, each `moveCode(true, dir)`, then a mask flag. |
| 2, 7, 7, 1, then 1 | Code 3. Level, x, z, teleport flag, then a mask flag. `teleport(z, x, flag == 1)` (`PathingEntity.java:71`, call at `Client.java:5121`). |

A mask flag of 1 appends `LOCAL_PLAYER_INDEX` to the update list.

`teleport` (`PathingEntity.java:71`): if the flag is false and both axis deltas from `routeTile[0]` are in -8..8, the step is queued with run false. Otherwise the route is cleared and `x` / `z` become `tile * 128 + size * 64`.

## Other players already in view

`readPlayers` (`Client.java:9551`) reads an 8-bit count. If the count is below `playerCount`, the tail ids are removed. For each counted player: bit 1 is 0 to keep the player with no further bits. Otherwise a 2-bit code: 0 mask only, 1 one `moveCode(false, dir3)` plus a mask bit, 2 two `moveCode(true, dir3)` plus a mask bit, 3 remove.

## New players

`getPlayerNewVis` (`Client.java:11664`) loops while `bitPos + 10 < size * 8`. An 11-bit pid of 2047 ends the list. Otherwise a signed 5-bit dx and a signed 5-bit dz (subtract 32 when the raw value is above 15), a 1-bit teleport flag, and a 1-bit mask flag. The tile is `localPlayer.routeTile + delta`.

## Direction codes

`PathingEntity.moveCode` (`PathingEntity.java:111`). `+z` is north and `+x` is east. The boolean argument is stored as the step's run flag.

| Code | Tile delta |
| --- | --- |
| 0 | x - 1, z + 1 |
| 1 | z + 1 |
| 2 | x + 1, z + 1 |
| 3 | x - 1 |
| 4 | x + 1 |
| 5 | x - 1, z - 1 |
| 6 | z - 1 |
| 7 | x + 1, z - 1 |

## Player masks

`projectFromGround` (`Client.java:7643`) reads one u8. If bit 7 is set it reads a second u8 and adds it shifted left by 8. Bit 7 is only the extension flag. `readPlayerUpdates` (`Client.java:9257`) tests these bits, low byte first:

| Bit | Fields |
| --- | --- |
| 1 | u8 length, then that many bytes (`gdata` at `Client.java:9263`; `valueClient38` is -13 at `Client.java:754`, so `Packet.gdata` copies). Appearance layout is below. |
| 2 | u16be sequence (65535 becomes -1), u8 delay. |
| 4 | u16be target (65535 becomes -1). |
| 8 | jstr public chat. |
| 16 | u8 damage, u8 type, u8 health, u8 total health. |
| 32 | u16be target tile x, u16be target tile z. |
| 64 | u16be colour/effect, u8 rights, u8 length, then `WordPack.unpack` of that many bytes. `pos` is then forced to the start of this block plus the length (`Client.java:9370`). |
| 256 | u16be spotanim (65535 becomes -1), u32be. Height is the high 16 bits. Delay is the low 16 bits. |
| 512 | u8 start x, u8 start z, u8 end x, u8 end z, u16be end cycle, u16be start cycle, u8 face direction. |
| 1024 | Same four u8 fields as bit 16. |

Bits above 1024 consume nothing.

### Appearance bytes

`PlayerEntity.setAppearance` (`PlayerEntity.java:46`): u8 gender, u8 head icon, then 12 slots. A slot u8 of 0 stores 0. Any other slot u8 is followed by a second u8 and stored as `(first << 8) + second`. If slot 0 equals 65535, the next field is a u16be NPC id and the remaining slots are skipped. Then five u8 colours, then seven u16be anims (ready, turn, walk, walk back, walk left, walk right, run; 65535 becomes -1), then u64be name and u8 combat level.

## NPCs

`readNpcInfo` (`Client.java:8260`) calls `readNpcs`, `getNpcPosNewVis`, then `readNpcUpdates`.

`readNpcs` (`Client.java:6910`) uses the same 8-bit count and 1/2-bit movement as other players. Code 3 removes the NPC. There is no teleport code.

`getNpcPosNewVis` (`Client.java:7010`) loops while `bitPos + 21 < size * 8`. A 14-bit id of 16383 ends the list. Then an 11-bit type, a signed 5-bit dx, a signed 5-bit dz, and a 1-bit mask flag. `teleport` is called with the flag false.

`readNpcUpdates` (`Client.java:5579`) reads one u8. There is no second mask byte.

| Bit | Fields |
| --- | --- |
| 1 | u8 damage, u8 type, u8 health, u8 total health. |
| 2 | u16be sequence (65535 becomes -1), u8 delay. |
| 4 | u16be target (65535 becomes -1). |
| 8 | jstr chat. |
| 16 | Same four u8 fields as bit 1. |
| 32 | u16be type. Walk and run anims are copied from that type. |
| 64 | u16be spotanim (65535 becomes -1), u32be packed as for players. |
| 128 | u16be target tile x, u16be target tile z. |
