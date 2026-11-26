# Sync

Player positions are server opcode 255. NPC positions are server opcode 248. Appearance and hits are separate packets, opcodes 250 and 247. There is no bit mask on the position packets. `Tools.gBit` does not advance the bit offset. The handler adds the width itself (`jagex/Tools.java:94`).

`ClientConfig.number92` is 0 on the live path. Branches guarded by `var19 != 0` in `call5fac7c41` are the fernflower duplicate and are not the live read. The 4-bit value used to remove an entity is peeked: the offset advances by 2 or by 4 only after the test (`deob/MudClient.java:7805`).

Tile size `value128` is 128 (`deob/MudClient.java:15239`). A local pixel is `localTile * 128 + 64`.

## Opcode 255 players

Handler `MudClient.call5fac7c41`, the `else` of `if (var1 != 255)` (`deob/MudClient.java:5769`, `deob/MudClient.java:7604`). Bit offset starts at 8, after the opcode byte.

| order | bits | use |
| --- | --- | --- |
| region x | 11 | `number428`, then subtract scene origin `number410` |
| region y | 13 | `number429`, then subtract `number411` |
| local direction | 4 | passed into `calla598fe3b` |
| known count | 8 | other players already known. Index 0 of the copied array is skipped |

Before the known loop the previous `entityArray2` is copied to `entityArray3`. The local player is rebuilt with `calla598fe3b(number430, pixelX, pixelY, direction)` (`deob/MudClient.java:7637`). `number430` is whatever opcode 244 last wrote, or the field initial value if 244 has not arrived. The known loop reads `entityArray3[i + 1]` (`deob/MudClient.java:7717`).

`call8390e4c9(regionX, regionY)` runs before the origin subtract (`deob/MudClient.java:7624`). The Java method is a stub that returns false (`deob/MudClient.java:9670`). The reload predicate is in the deob class. See [region](region.md). If that call returns true, the local waypoint is reset to the new pixel.

### Known player

One bit. 0 keeps the player and writes nothing else.

One bit type:

- Type 0: 3-bit step, then keep. The `number92 != 0` path would also read 4 bits. The live path does not (`deob/MudClient.java:7723`, `deob/MudClient.java:7800`).
- Type 1: peek 4 bits at the current offset. If `(value & 12) == 12`, add 2 to the bit offset and drop the player. Else read those same 4 bits as the new direction, add 4, and keep (`deob/MudClient.java:7805`).

Step directions, in units of 128 (`deob/MudClient.java:7729`):

| bits | dx | dy |
| --- | --- | --- |
| 0 | 0 | -128 |
| 1 | +128 | -128 |
| 2 | +128 | 0 |
| 3 | +128 | +128 |
| 4 | 0 | +128 |
| 5 | -128 | +128 |
| 6 | -128 | 0 |
| 7 | -128 | -128 |

### New players

While `bitpos + 24 < payloadLength * 8` (`deob/MudClient.java:7878`):

| bits | meaning |
| --- | --- |
| 11 | server index |
| 5 | dx. If the value is greater than 15, subtract 32 |
| 5 | dy. Same sign rule |
| 4 | direction |
| 1 | skip appearance when 1 |

Pixel is `(region + delta) * 128 + 64`. The region used here is already origin-subtracted. If the skip bit is 0, the index is queued.

There is no sentinel id. The loop stops when fewer than 24 bits remain.

### Appearance request

If the queue is not empty the client writes opcode 254, threat 120: u16be count, then count times u16be `Entity.number290` and u16be `Entity.number291` (`deob/MudClient.java:7880`, `deob/Entity.java:8`).

## Opcode 248 NPCs

Same method, `var1 == 248` (`deob/MudClient.java:6409`). Copy `entityArray5` to `entityArray6`. Bit offset 8. An 8-bit count. The known loop uses `entityArray6[i]` with no `+ 1`. The keep, 3-bit step, and 4-bit peek-remove use the same widths as players (`deob/MudClient.java:6465`).

New NPCs while `bitpos + 34 < payloadLength * 8` (`deob/MudClient.java:6432`):

| bits | meaning |
| --- | --- |
| 12 | server index |
| 5 | signed dx, same `> 15` then `- 32` rule |
| 5 | signed dy |
| 4 | direction |
| 10 | npc type. If `type >= ClientConfig.npcCount`, store 24 |

`callf0db336f` adds the NPC. No appearance-request packet is written for NPCs. The loop stops when fewer than 34 bits remain.

## Opcode 250 player aux

u16be count at offset 1. Each record starts at offset 3 (`deob/MudClient.java:6105`).

| order | width |
| --- | --- |
| player index | u16be into `entityArray1` |
| type | u8 |

| type | fields |
| --- | --- |
| 0 | u16be into `number301`. Sets `number302 = 150`. |
| 1 | u8 length, WordPack bytes of that length, then `WordFilter.filter`. Ignored names are skipped. Bubble timer `number300 = 150`. |
| 2 | three u8: `number303`, `number304`, `number305`. Local player also writes skill arrays index 3 and clears welcome and dialog flags. |
| 3 | two u16be: `number312`, `number314`. `number313 = -1`. `number315 = value40` (40). |
| 4 | two u16be: `number312`, `number313`. `number314 = -1`. `number315 = 40`. |
| 5 | present player: u16be `number291`, u64be name, u8 equip count, that many u8 equip ids, then pad `numberArray` to 12 with 0, then four u8 colors (`number308` hair, `number309` top, `number310` bottom, `number311` skin), u8 `combatLevel`, u8 `number317` (`deob/MudClient.java:6232`). |
| 6 | u8 length, WordPack bytes, no `WordFilter`. Local player also prints chat type 5. |
| other | the live path does not advance the cursor for an unknown type |

The count is the terminator. There is no trailing sentinel.

Missing player, type 5 only: the decompile adds 14 to the cursor, reads one u8 `n`, then adds `n + 1` (`deob/MudClient.java:6272`). The present-player body is 2 + 8 + 1 + n + 4 + 1 + 1 bytes. Those two skips differ by one byte. See [gaps](gaps.md).

## Opcode 247 NPC aux

u16be count, then each record u16be index into `entityArray4` and u8 type (`deob/MudClient.java:6611`).

| type | fields |
| --- | --- |
| 1 | u16be player index, u8 length, WordPack bytes. If the player index equals the local `number290`, the client prints the NPC chat. |
| 2 | three u8 hit splat, same slots as player type 2 |
| other | live path does not consume further fields for that record |

## Opcode 244 local index

Five u16be at offsets 1, 3, 5, 7, 9: `number430`, `number406`, `number407`, `number412`, `number408`. Then `number407 -= number412 * number408` (`deob/MudClient.java:6693`). Opcode 255 uses `number430` as the local server index. `number406` and `number407` are the offsets added before the sector test in the map reload. See [region](region.md).
