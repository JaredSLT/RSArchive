# Player and NPC sync

Player update is S2C opcode 150, size `-2`. `LocType.opLocType9` (`builds/rs2/474-2007-10-19/src/src/main/java/rt4/LocType.java:23-47`) resets the removal and mask counts, then calls local movement, other-player movement, the new-player list, and masks. A position that does not equal the packet size throws `gpp1`. A listed player slot that is null throws `gpp2`.

NPC update is S2C opcode 146, size `-2`. `SlStars.opkind2503` (`deob/SlStars.java:35-59`) calls existing-NPC movement, the new-NPC list, and masks. The matching errors are `gnp1` and `gnp2`.

Players occupy 2048 slots (`EnumStringEntry.valueEnumStringEntry15` is `int[2048]`, `rt4/EnumStringEntry.java:25`). NPCs occupy 32768 slots. The local player is slot 2047 and is created before the first rebuild by `GlIndexedSprite.opGlIndexedSprite3` (`rt4/TextureOp29SubOp.java:292`). The bit reader is `TextureOp27.opTextureOp2711`.

Walk steps use `Js5CacheRequest.opJs5CacheRequest42`. The stored x is `valueJs5CacheRequest75` and the stored z is `valueJs5CacheRequest92`. Direction 0 through 7 changes those tiles by `(-1, +1), (0, +1), (+1, +1), (-1, 0), (+1, 0), (-1, -1), (0, -1), (+1, -1)` (`rt4/Js5CacheRequest.java:143-184`). Facing values in the new-entity blocks index `ImageProducerFrameBuffer.valueImageProducerFrameBuffer3`, which is `{768, 1024, 1280, 512, 1536, 256, 0, 1792}` (`deob/ImageProducerFrameBuffer.java:16`).

`opJs5CacheRequest43(z, x, flag)` snaps to those tiles. When `flag` is false and the x delta is in `-8 .. 8` (`var4 >= -8 && ~var4 >= -9`) and the z delta is in `-8 .. 8` (`~var5 <= 7 && var5 <= 8`), it enqueues a step instead (`rt4/Js5CacheRequest.java:206-214`).

## Local player

`Js5ResourceProvider.opJs5ResourceProvider6` (`rt4/Js5ResourceProvider.java:32-68`):

| Bits | Meaning |
| --- | --- |
| 1 | 0 ends the block |
| 2 | type |

Type 0 queues mask index 2047 and has no further bits.

Type 1 is `bits(3)` walk direction, not running, then `bits(1)`. A set bit queues mask index 2047.

Type 2 is two `bits(3)` directions, both marked running, then `bits(1)` for the mask queue.

Type 3 is `bits(1)` mask, `bits(1)` flag, `bits(7)` x, `bits(2)` plane into `SoundPcmStream.valueSoundPcmStream32`, `bits(7)` z, then `opJs5CacheRequest43(z, x, flag == 1)`.

## Other players

`C51284405.opkind2511` (`deob/C51284405.java:48-100`):

`bits(8)` is the count. A count below the current list length puts the surplus indexes on the removal list. A count above the current list throws `gppov1`. There is no terminator. For each counted player, `bits(1)` then, when set, `bits(2)`:

| Type | Bits after the type |
| --- | --- |
| 0 | keep the player and queue a mask |
| 1 | `bits(3)` walk, not running, then `bits(1)` mask |
| 2 | two `bits(3)` run directions, then `bits(1)` mask |
| 3 | put the index on the removal list |

## New players

`Js5CacheRequest.opJs5CacheRequest41` (`rt4/Js5CacheRequest.java:73-118`). While at least 11 bits remain:

| Bits | Meaning |
| --- | --- |
| 11 | index. 2047 ends the list |
| 1 | queue a mask when set |
| 3 | facing index into the direction table |
| 5 | signed x delta from the local player. A value above 15 loses 32 |
| 1 | flag passed to `opJs5CacheRequest43` |
| 5 | signed z delta, same bias |

A missing slot allocates `JavaAudioChannel`. If `MapElement.valueMapElement44[index]` already holds bytes, `opJavaAudioChannel15` runs on them before the placement bits. Placement is `opJs5CacheRequest43(localZ + zDelta, localX + xDelta, flag == 1)`. The reader then byte-aligns.

## Player masks

`Js5ResourceProvider.opJs5ResourceProvider8` reads one u8. If bit `0x20` is set, it adds a second u8 shifted left 8 (`rt4/Js5ResourceProvider.java:102-111`). Bit `0x20` is only that extension. `Cache.opCache25` never tests `0x20` as a payload (`rt4/Cache.java:95-229`).

Payload order:

| Bit | Fields |
| --- | --- |
| `0x10` | u8m128 length, then that many bytes stored in reverse (`opByteArrayNodeSecondary53`), then `JavaAudioChannel.opJavaAudioChannel15` |
| `0x100` | u16be_lo_m128 spotanim, 65535 becomes -1, then u32me2 |
| `0x01` | u16le, u16le_lo_m128 |
| `0x40` | jstr. A first character of `~` (126) is stripped. Chat timer 150 |
| `0x08` | u16be_lo_m128 animation, 65535 becomes -1, then u8, then `C910c4c9f.opkind2543` |
| `0x02` | u16be, 65535 becomes -1 |
| `0x04` | u8, u8m128, then u8n128, u8neg |
| `0x80` | u16le_lo_m128, u8m128, u8n128, then u8 length and `bytes(n)`. The bytes are passed to `PleaseEnterUsername.opkind2424` |
| `0x200` | u8n128, u8n128, u8, u8 |
| `0x400` | u8neg, u8n128, u8n128, u8neg, u16be, u16be_lo_m128, u8 |

`opJavaAudioChannel15` (`rt4/JavaAudioChannel.java:170`) and `PleaseEnterUsername.opkind2424` (`deob/PleaseEnterUsername.java:68`) were not read. Their layouts are empty in [gaps.md](gaps.md). `rslib/src/both/move/rev_474.c` comments a gender, slot, colour, and anim layout for the appearance block. That comment is not this client's method body.

## Existing NPCs

`Texture.opTexture12` (`rt4/Texture.java:253-306`) matches the other-player block: `bits(8)` count, `gnpov1` when the count is too large, then per NPC `bits(1)` and optional `bits(2)` with the same four types. Type 3 removes. The walk and run directions are the same 3-bit steps.

## New NPCs

`SceneGraph_Class120.callcf1c3987` (`rt4/SceneGraph_Class120.java:265-320`). While at least 27 bits remain:

| Bits | Meaning |
| --- | --- |
| 15 | index. 32767 ends the list |
| 5 | signed x delta from the local player, bias 32 when above 15 |
| 14 | type id, looked up with `Js5ResourceProvider.opJs5ResourceProvider11` |
| 5 | signed z delta, same bias |
| 1 | queue a mask when set |
| 3 | facing index |
| 1 | flag passed to `opJs5CacheRequest43` |

Placement is `opJs5CacheRequest43(localZ + zDelta, localX + xDelta, flag == 1)`. The reader then byte-aligns. The sign adjustments are applied to the first 5-bit value before the mask bit and to the second 5-bit value after it. The stream order is still x, type, z, mask, facing, flag.

## NPC masks

`MaterialRenderer.call8fda898f` reads one u8 and no extension byte (`rt4/MaterialRenderer.java:33-131`).

| Bit | Fields |
| --- | --- |
| `0x10` | u16le_lo_m128 animation, 65535 becomes -1, then u8neg |
| `0x04` | u16be, 65535 becomes -1 |
| `0x02` | u16le_lo_m128 type id |
| `0x40` | u16le_lo_m128, u16le_lo_m128 |
| `0x80` | u8, u8m128, then u8, u8n128 |
| `0x20` | u8m128, u8m128, u8m128, u8m128 |
| `0x01` | jstr. Chat timer 100 |
| `0x08` | u16be_lo_m128 spotanim, 65535 becomes -1, then u32me |
