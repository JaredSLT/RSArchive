# Input

State 30 sends these packets from `InvType.opInvType12` and the walk writer. There is no C2S size table. The first byte of a walk packet is a payload byte, not a `-1` length. Opcodes are ISAAC-added by `opTextureOp275`.

## Mouse

Opcode 121 is written when `RunescapeHasBeenUpdated.valuekind2204` is set and a click is down or at least 40 samples are queued (`rt4/InvType.java:83-138`). That flag is the login success byte compared with 1. Layout:

| Order | Width | Value |
| --- | --- | --- |
| 1 | opcode | 121 |
| 2 | u8 | placeholder 0, later replaced by the payload length |
| 3 | repeated | one record per sample, while the payload stays under 240 bytes |
| 4 | u8 backpatch | `opByteArrayNodeSecondary38` stores the length at `pos - 1 - length` |

Samples compare against the previous point. x is clamped to `0 .. 764` and y to `0 .. 502`. The pair `(-1, -1)` is encoded as `524287`.

| Condition | Width | Value |
| --- | --- | --- |
| time delta under 8 and both deltas in `-32 .. 31` | u16be | `(time << 12) + ((dx + 32) << 6) + (dy + 32)` |
| time delta under 8 | u24be | `(time << 19) + 8388608 + (y * 765 + x)` |
| otherwise | u32be | `(time << 19) + (-1073741824) + (y * 765 + x)` |

Opcode 167 is one click (`rt4/InvType.java:153-184`):

u32le of `(y * 765 + x) + ((elapsedMs / 50) capped at 4095) << 20 + (right << 19)`.

`right` is 1 when `GlSolidColorTexture.valueGlSolidColorTexture42` is 2. x is clamped to `0 .. 764` and y to `0 .. 502`.

Opcode 34 is focus. The payload is u8 `1` when `ProjAnimNode.valueProjAnimNode28` becomes true and u8 `0` when it becomes false (`rt4/InvType.java:203-212`).

## Camera and idle

Keys 96, 97, 98, and 99 set `GlSolidColorTexture.valueGlSolidColorTexture29`. On the next send, opcode 169 writes u16le `ConnectingServer.valuekind23717` and u16le `TextureOp11.valueTextureOp116` (`rt4/InvType.java:191-200`). Which field is yaw is not in this method.

Opcode 236 has no payload. It is sent when `HashTableIterator.opHashTableIterator12` and `TextureOpRange.opTextureOpRange5` are both greater than 4500 (`rt4/InvType.java:389-394`). The same branch sets `MapChunk.valueMapChunk35` to 250 and calls `TextureOp35.opTextureOp359(4000)`.

Opcode 186 has no payload. It is sent when `ObjType.valueObjType169` passes 50 (`rt4/InvType.java:468-469`). `SoftwareFont.java:128` writes the same opcode from another site. That second site was not read.

## Walk

`QuickChatPhrase.opQuickChatPhrase7` writes the path (`rt4/QuickChatPhrase.java:246-288`). `var5` selects the opcode. The step count `n` is capped at 25. The path arrays are x in `JavaAudioChannel.valueJavaAudioChannel25` and z in `deob.World.valuekind2381`. Steps after the first are deltas from the previous written step, walking backward from the end of the path.

| `var5` | Opcode | First u8 | Caller read |
| --- | --- | --- | --- |
| 0 | 46 | `2 * n + 3` | minimap-coordinate path in `InvType.java:338` |
| 1 | 11 | `2 * n + 17` | `MaterialManager_Class106_Sub2.opMaterialManager_Class106_Sub267` (`rt4/MaterialManager_Class106_Sub2.java:31`) |
| 2 | 59 | `2 * n + 3` | menu walks in `deob/HierWechseln.java` and `rt4/GlBoundingBox.java` |

Shared tail:

| Order | Width | Value |
| --- | --- | --- |
| 1 | opcode | 46, 11, or 59 |
| 2 | u8 | the length byte above |
| 3 | `(n - 1)` times | u8 `dx`, then u8 `(128 - dz)` via `opByteArrayNodeSecondary36` |
| 4 | u16be low+128 | last x plus `ObjStackNode.valueObjStackNode42` |
| 5 | u16le | last z plus `WaterfallMaterialRenderer.valueWaterfallMaterialRenderer39` |
| 6 | u8 | 1 when key 82 is held, else 0 |

For opcodes 46 and 59 the first u8 equals the number of bytes that follow it. Opcode 11's first u8 is 14 larger. Those 14 bytes are appended only after a successful opcode 11 walk (`rt4/MaterialManager_Class106_Sub2.java:32-43`):

u8, u8, u16be `TextureOp11.valueTextureOp116`, u8 `57`, u8 `Midi_Class162.valueMidi_Class1623`, u8 `ProjAnimNode.valueProjAnimNode24`, u8 `89`, u16be `player.valueJs5CacheRequest89`, u16be `player.valueJs5CacheRequest67`, u8 `ParticleSystem.valueParticleSystem30`, u8 `63`.

That is 14 bytes. The method runs when `deob.World.valuekind2389` is 0 or 3 and the click is inside the minimap rectangle (`rt4/MaterialManager_Class106_Sub2.java:14-19`).

## Widget and name packets

Interface drag opcode 166 (`rt4/InvType.java:288-292`): u16le low+128 destination slot, u8 `(128 - flag)`, u32me2 component id, u16le source slot.

`ChangeLocRequest` writes opcodes 234, 87, 37, 3, 70, 29, 192, 154, 65, and 156 for action indexes 1 through 10. Each is followed by u32be and u16be (`rt4/ChangeLocRequest.java:67-124`).

Opcode 152 is u64be of a name long (`rt4/Mouse.java:44-45`). Opcode 1 is u64be of a name long (`rt4/TextureOp29SubOp4.java:55-56`). Opcode 250 is u8neg then two u32me of a name long (`deob/ServerBeingUpdated.java:136-138`).

Opcode 149 is the reflection reply (`rt4/DirectByteArray.java:72-162`). It writes a placeholder u8, a u32be id, then per check a status byte and a payload that depends on the check type, then `opByteArrayNodeSecondary27` (CRC, method body unread) and a u8 length backpatch.

`HierWechseln.opkind2465` and `TextureOpCurve.opTextureOpCurve18` write more opcodes. Those payloads were not read. The opcode list is in [packets.md](packets.md) and [gaps.md](gaps.md).
