# Sync

Player movement is server opcode 255. NPC movement is server opcode 248. Appearance is server opcode 250 for players and 247 for NPCs. The local player is the prefix of opcode 255. There is no separate region opcode. See `region.md`.

Bit reads use `Tools.gBit` (`framing.md`). Both movement handlers start the cursor at bit 8, which skips the opcode byte.

## Local player

`MudClient.call5fac7c41` opcode 255 (`deob/MudClient.java:2833`):

| bits | field | stored |
| --- | --- | --- |
| 10 | x | `number12` |
| 12 | y | `menuParamEntityA` |
| 4 | facing | passed to `calla598fe3b` |

Then `call8390e4c9(x, y)` (`MudClient.java:2847`). The coordinates are reduced by `number414` and `number184`. Scene pixels are `tile * 128 + 64` (`value128` is 128, `MudClient.java:8360`). The local entity is `calla598fe3b(number211, pixelX, pixelY, facing)` (`MudClient.java:2860`). `number211` comes from opcode 244, not from this bit block (`MudClient.java:3509`). The client stores no start tile. The first position is this packet.

## Other players

Previous entities are copied to `appletMode`. The next field is bits8, the number of previous others to visit. The loop reads `appletMode[i + 1]`, so slot 0 of the previous list is skipped (`MudClient.java:2864`).

Each visited player:

1. bit1 update. 0 keeps the entity.
2. bit1 kind.
3. Kind 0: bits3 step. `number18` becomes that step and a waypoint is queued at `(number20 + 1) % 10`. Deltas of `value128` (128): steps 2, 1, 3 add x; 6, 5, 7 subtract x; 4, 3, 5 add y; 0, 1, 7 subtract y (`MudClient.java:2872`).
4. Kind 1: `gBit` of 4 bits into a temporary, and the cursor is not increased (`MudClient.java:2899`). If `(temporary & 12) == 12`, the cursor increases by 2 and the entity is left out. Otherwise `number18` is a second `gBit` of 4 bits at the same offset and the cursor increases by 4 (`MudClient.java:2905`).

Players past the bits8 count are dropped. A remove leaves the entity out of `entityArray120`.

New players follow while `bitpos + 24 < payloadLength * 8` (`MudClient.java:2915`):

| bits | field |
| --- | --- |
| 11 | server id |
| 5 | dx, subtract 32 when greater than 15 |
| 5 | dy, same sign rule |
| 4 | facing |
| 1 | appearance needed |

Pixel position is `(localTile + delta) * 128 + 64`. The record is 26 bits. The loop test is 24 bits. See `gaps.md`.

If the appearance bit is 0, the id is recorded. When any such ids exist the client writes c2s opcode 254 and returns, leaving the rest of the packet unread (`MudClient.java:2942`). An empty other-player list is a known count of 0 and fewer than 24 bits left. There is no terminator byte.

## Player opcode 250

Count is u16be at offset 1. Records start at offset 3 (`MudClient.java:3148`). Each record is u16be server index, then a type byte.

| type | fields |
| --- | --- |
| 0 | u16be into `number26`, `number27 = 150` |
| 1 | i8 length, dictionary bytes with the filter flag true. Ignored names are skipped. Public chat uses message type 2 |
| 2 | three u8 into `number28`, `number29`, `number30`, `number31 = 200`. The local player copies the second and third into skill index 3 |
| 3 | two u16be into `number37` and `number39`, `number38 = -1` |
| 4 | two u16be into `number37` and `number38`, `number39 = -1` |
| 5 | see below |
| 6 | i8 length, dictionary bytes with the filter flag false. The local player uses message type 5 |

Type 5, entity present (`MudClient.java:3245`): u16be `number12`, u64be name hash, u8 count, that many u8 worn ids, pad with 0 through index 11, then six u8: `number33`, `number34`, `number35`, `number36`, `number32`, `number43`.

Type 5, entity absent (`MudClient.java:3240`): skip 14 bytes, read u8 length, skip that many bytes. The 14 is a raw skip.

A type other than 0..6 consumes no bytes (`MudClient.java:3225`). Chat lengths on types 1 and 6 are assigned from a Java `byte`, so a value above 127 is negative.

## NPCs

Opcode 248 (`MudClient.java:3347`). The previous list is saved. bits8 is the known count. The update bit, kind bit, bits3 step, and kind-1 4-bit read match the player loop, including the read that does not advance (`MudClient.java:3394`). Step deltas match the player table.

New NPCs while `bitpos + 31 < payloadLength * 8` (`MudClient.java:3413`):

| bits | field |
| --- | --- |
| 11 | id |
| 5 | dx, signed the same way as players |
| 5 | dy |
| 4 | facing |
| 8 | npc type. If `type >= ClientConfig.number444`, store 24 |

Pixel position uses the same `* 128 + 64` formula. `callf0db336f` places the NPC. There is no appearance-request bit and no c2s follow-up. The fields above are 33 bits. The loop test is 31. See `gaps.md`.

Opcode 247 (`MudClient.java:3442`): u16be count at offset 1, records from offset 3, each u16be index and u8 type.

| type | fields |
| --- | --- |
| 1 | u16be speaker index, i8 length, dictionary bytes with the filter flag false |
| 2 | three u8 into `number28`, `number29`, `number30`, `number31 = 200` |

Entity fields live in `deob/Entity.java`. `number11` and `number12` are the pair sent by c2s 254. `numberArray` holds 12 appearance bytes. `numberArray21` and `numberArray22` hold 10 waypoints.
