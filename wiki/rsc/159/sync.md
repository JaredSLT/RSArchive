# Sync

Player and NPC positions are bit-packed in the frame buffer. The cursor starts at bit 8. `Tools.gBit` is MSB first and the caller adds the bit count. Waypoints live on `Entity`: `numberArray22` and `numberArray23` each have length 10 (`Entity.java:6-7`). `number21` is the last slot. A step stores the next slot at `(number21 + 1) % 10`.

Pixel positions are scene-local tiles times `value128` (128) plus 64. Opcode 255 subtracts `menuAction` from the x tile and `number484` from the y tile before that multiply. Those two fields are the scene origin from [region.md](region.md).

There is no 2047 terminator and no separate teleport bit.

## Step deltas

The same 3-bit step is used for other players and for NPCs. `value128` is 128.

| Step | x | y |
| --- | --- | --- |
| 0 | 0 | -128 |
| 1 | +128 | -128 |
| 2 | +128 | 0 |
| 3 | +128 | +128 |
| 4 | 0 | +128 |
| 5 | -128 | +128 |
| 6 | -128 | 0 |
| 7 | -128 | -128 |

## Opcode 255, players

`MudClient.call5fac7c41` at `MudClient.java:3167`.

Local player, from bit 8:

| Bits | Field |
| --- | --- |
| 11 | world x tile, stored in `number211` |
| 13 | world y tile, stored in `number98` |
| 4 | facing, passed to `calla598fe3b` as `number18` |

`call8390e4c9(number211, number98)` may reload the map. Then `number211 -= menuAction` and `number98 -= number484`. The local pixel is `(number211 * 128 + 64, number98 * 128 + 64)`.

`calla598fe3b(number227, x, y, facing)` (`MudClient.java` from the call at 3194): `number227` is the server index from opcode 244. If that index is already known, `number19` becomes the 4-bit facing and a new waypoint is pushed when the coordinate changed. If the index is new, the position is stored immediately and `number19`, `number18`, and `number17` are set from that facing and 0. The method takes the absolute pixel it is given. A large step is another waypoint, not a flag on the wire.

Then `bits(8)` is the count of known other players. The loop reads `entityArray12[i + 1]`, so index 0 of the previous list is not updated here. For each known player:

- `bits(1)` update. 0 keeps the player and copies it into the new list.
- If 1, another `bits(1)` is the type.
- Type 0: `bits(3)` step, applied with the table above, and `number19` becomes that step.
- Type 1: peek `bits(4)` without the cursor having moved past this peek. If `(value & 12) == 12`, add 2 to the cursor and drop the player. Otherwise read `bits(4)` again into `number19` and add 4. That second read repeats the same bit offset (`MudClient.java:3233-3240`).

New players run while `bitpos + 24 < length * 8` (`MudClient.java:3249`). Each record is:

| Bits | Field |
| --- | --- |
| 11 | player index |
| 5 | dx, subtract 32 when greater than 15 |
| 5 | dy, same signed rule |
| 4 | facing |
| 1 | 0 means the index is queued for an appearance request |

Placement is `(number211 + dx) * 128 + 64` and `(number98 + dy) * 128 + 64`, using the already rebased local tiles.

If `number70` is at least 45, the request list gains a `-1` entry and `number70` is cleared. When the list is not empty the client writes c2s opcode 254: `u16be` count, then for each entry either `u16be` `entity.number12` and `u16be` `entity.number13` when the index is at least 0, or `u16be` `1600 + (number55 & 1023)` and `u16be` 1 when the entry is the `-1` sentinel (`MudClient.java:3281-3296`). The handler then returns.

## Opcode 248, NPCs

`MudClient.java:3693`. Bit cursor starts at 8.

`bits(8)` known count. Each known NPC uses the same update bit, type bit, 3-bit step, and `(bits(4) & 12) == 12` drop as players. The drop adds 2 to the cursor and does not copy that NPC forward (`MudClient.java:3740-3743`).

New NPCs run while `bitpos + 34 < length * 8` (`MudClient.java:3759`). Each record is:

| Bits | Field |
| --- | --- |
| 12 | npc index |
| 5 | signed dx, same `> 15` then `- 32` rule |
| 5 | signed dy |
| 4 | facing |
| 10 | npc type. If the type is at least `ClientConfig.number489`, it becomes 24 |

`callf0db336f` stores the type on `number16` and the facing. There is no appearance-request bit. Placement uses the same rebased `number211` and `number98` as players.

## Opcode 250, player appearance and chat

`MudClient.java:3492`. `u16be` count at offset 1. The per-record cursor starts at 3. Each record is `u16be` index, then `u8` type (the raw byte, not masked).

| Type | Fields |
| --- | --- |
| 0 | `u16be` into `number27`, and `number28 = 150` |
| 1 | `u8` length, then `packed` chat of that length (`call85aa3cc8` with the color flag true). Skipped when `longValue` is on the ignore list. The cursor always advances by the length |
| 2 | three `u8`: `number29`, `number30`, `number31`, and `number32 = 200`. On the local player, skill slot 3 current and base become `number30` and `number31`, and `flag306` and `flag313` clear |
| 3 | `u16be` `number38`, `u16be` `number40`, `number39 = -1`, `number41 = value40` (40) |
| 4 | `u16be` `number38`, `u16be` `number39`, `number40 = -1`, `number41 = 40` |
| 5 | appearance, below |
| 6 | `u8` length, then `packed` with the color flag false. `number26 = 150`. If the entity is the local player, the line is also a type-5 message |

A type other than 0 through 6 does not advance the cursor. The next record reads the same byte again (`MudClient.java:3571-3615`).

Appearance when the entity is missing: skip 14 bytes, read `u8` name length, skip that many bytes plus the length byte already consumed (`var133 += 14`, then `call174`, then `var133 += length + 1`, `MudClient.java:3586-3589`).

Appearance when the entity exists:

| Order | Field |
| --- | --- |
| 1 | `u16be` `number13` |
| 2 | `u64be` `longValue`, and `text11 = fromBase37` |
| 3 | `u8` equipment count, then that many `u8` into `numberArray`. Slots from the count through 11 are set to 0. `numberArray` has length 12 (`Entity.java:10`) |
| 4 | six `u8`: `number34`, `number35`, `number36`, `number37`, `number33`, `number44` |

## Opcode 247, NPC aux

`MudClient.java:3788`. `u16be` count. Each record is `u16be` npc index into `entityArray134`, then `u8` type.

| Type | Fields |
| --- | --- |
| 1 | `u16be` speaker index, `u8` text length, `packed` bytes. If the speaker index equals the local player's `number12`, the line is a type-5 message prefixed with the npc name |
| 2 | three `u8` into `number29`, `number30`, `number31`, and `number32 = 200` |

Any other type does not advance the cursor.

## What is not a mask

Opcode 244 is not a player mask. It is five `u16be` values used by the region origin. See [region.md](region.md). Opcode 250 types are not bit masks. The only bit fields in this revision's entity update are the ones in opcodes 255 and 248.
