# Player and NPC sync

Server packet 74 is the player update. `GlFont.opGlFont5` calls `JString.opJString13` and reads no header of its own (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1374`). The body is bit-packed. `rslib/src/server/packets/468.c` binds player update to opcode 90. That binding is not this switch.

Server packet 36 is the NPC update, `PathingEntity.opPathingEntity14` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:967`). `rslib/src/server/packets/468.c` binds NPC update to opcode 71.

Both packets finish by requiring `valueMouseRecorder6 == SoftReferenceNode.valueSoftReferenceNode76`. Players throw `gpp1` or `gpp2` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/JString.java:280`). NPCs throw `gnp1` or `gnp2` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/PathingEntity.java:47`).

## Axes

`GameShell.opGameShell11` stores zone x as `TextureOp29SubOp2.valueTextureOp29SubOp223 = zoneX * 8 - 48` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp29SubOp2.java`) and zone z as `EmpfNger.valuekind13827 = (zoneZ - 6) * 8` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GameShell.java:34`). When those bases change, `valueSoftReferenceNode130` is shifted by the x delta and `valueSoftReferenceNode108` is shifted by the z delta (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GameShell.java:40`).

`SoftReferenceNode.opSoftReferenceNode12(a, clear, b)` writes `a` into `valueSoftReferenceNode108` and `b` into `valueSoftReferenceNode130` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/SoftReferenceNode.java:213`). So the first integer is scene z and the third is scene x.

`opSoftReferenceNode10` steps from `valueSoftReferenceNode130` (x) and `valueSoftReferenceNode108` (z) (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/SoftReferenceNode.java:150`):

| Direction | x | z |
| --- | --- | --- |
| 0 | -1 | +1 |
| 1 | 0 | +1 |
| 2 | +1 | +1 |
| 3 | -1 | 0 |
| 4 | +1 | 0 |
| 5 | -1 | -1 |
| 6 | 0 | -1 |
| 7 | +1 | -1 |

The walk search records the same axes as flags 2 (x - 1), 8 (x + 1), 1 (z - 1), and 4 (z + 1) (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonochromeImageCache.java:199`).

Orientation for a new actor is `LongNode.valueLongNode13[3-bit index]`. The table is `{768, 1024, 1280, 512, 1536, 256, 0, 1792}` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/LongNode.java:45`). It is applied only when the slot was just created.

Player objects are `IntNode`, stored in `ByteArrayNodeSecondary.valueByteArrayNodeSecondary22` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/ByteArrayNodeSecondary.java`). The local player is `SecondaryLinkedList.valueSecondaryLinkedList32` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/SecondaryLinkedList.java`). NPC objects are `ReferenceNodeFactory` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/ReferenceNodeFactory.java`), stored in `ServerActiveProperties.valueServerActiveProperties22`. The kept-player index list is `MidiNoteStream.valueMidiNoteStream11` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MidiNoteStream.java`). The rebuild loops treat player indices below 2048 and NPC indices below 32768 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GameShell.java:40`).

## Local player

`ShapedTile.opShapedTile10` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/ShapedTile.java:45`) starts bit mode.

| Bits | Meaning |
| --- | --- |
| 1 | 0 ends the local block |
| 2 | type |

Type 0 queues mask index 2047 and reads nothing else.

Type 1: 3-bit direction, `opSoftReferenceNode10(direction, false)`, then 1 bit. The bit queues 2047.

Type 2: 3-bit direction with `run` true, another 3-bit direction with `run` true, then 1 mask bit for 2047.

Type 3: 7-bit scene z, 7-bit scene x, 1 mask bit for 2047, 2-bit plane stored in `CursorManager.valueCursorManager9`, 1-bit clear. The place call is `opSoftReferenceNode12(z, clear == 1, x)`.

## Other players

`World.opWorld68` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/World.java:21`) reads an 8-bit count. It does not use a sentinel index. A count below the previous list removes the extra indices. A count above the previous list throws `gppov1`.

For each kept index: 1 bit. 0 keeps the player and does not queue a mask. 1 is followed by a 2-bit type:

| Type | Bits after the type |
| --- | --- |
| 0 | none; the player is kept and queued for a mask |
| 1 | 3-bit direction with `run` false, then 1 mask bit |
| 2 | two 3-bit directions, both with `run` true, then 1 mask bit |
| 3 | none; the index is removed |

## New players

`SoundPcmStream.opSoundPcmStream10` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/SoundPcmStream.java:35`) loops while `opCache6(packetSize) >= 11`.

| Bits | Meaning |
| --- | --- |
| 11 | index. 2047 ends the loop |
| 5 | signed scene-x delta from the local player's `valueSoftReferenceNode130`. Values above 15 subtract 32 |
| 5 | signed scene-z delta from `valueSoftReferenceNode108`, same bias |
| 3 | orientation index, stored only if this call created the `IntNode` |
| 1 | clear flag |
| 1 | 1 queues the index for a mask |

The place call is `opSoftReferenceNode12(localZ + zDelta, clear == 1, localX + xDelta)`. A cached appearance buffer at `ProjAnimNode.valueProjAnimNode4[index]` is applied when the slot is created. The method then `opCache7`.

## Player masks

`CursorManager.opCursorManager4` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:285`) reads a u8 per queued player. If bit `0x80` is set, it reads a second u8 and adds it shifted left 8. `opCursorManager6` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:311`) then tests bits in this order:

| Bit | Fields |
| --- | --- |
| 0x400 | u16le with low byte +128 (65535 becomes -1); u32le. The low 16 bits of the u32le are added to the client cycle |
| 0x20 | u8 stored as `-value`, u8, passed to `opSoftReferenceNode16`; then u8 stored as `128 - value`; then u8 stored as `-value` |
| 0x4 | u16be (65535 becomes -1), u8 stored as `value + 128` |
| 0x10 | u16le with low byte +128, u8, u8 stored as `128 - value` as a length, then that many bytes in order |
| 0x200 | u8+128, u8+128, u8, u8+128, u16le+128 added to the cycle, u16be added to the cycle, u8 |
| 0x100 | u8 stored as `128 - value`, u8+128, u8, u8 stored as `-value` |
| 0x2 | jstr. If `opScenery26(0)` returns 126, the string is cut from index 1 |
| 0x8 | u16le with low byte +128 (65535 becomes -1) |
| 0x40 | u8 stored as `128 - value` as a length, then that many bytes reversed into a buffer, then `IntNode.opIntNode6` |
| 0x1 | u16le with low byte +128, u16le |

## Appearance

`IntNode.opIntNode6` reads the reversed buffer from the start (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/IntNode.java:81`):

| Order | Width | Use |
| --- | --- | --- |
| 1 | u8 | passed as `byte == 1` into `LongNode.call90268241` |
| 2 | raw i8 | `valueIntNode32` |
| 3 | raw i8 | `valueIntNode40` |
| 4 | 12 slots | u8. 0 stores 0. Otherwise another u8, and the value is `(first << 8) \| second`. If slot 0 is 65535, a u16be is the npc id and the slot loop stops |
| 5 | 5 × u8 | colour. A value outside `DirectByteArray.valueDirectByteArray13[i]` becomes 0 |
| 6 | 7 × u16be | anim ids. 65535 becomes -1. Stored on `valueSoftReferenceNode65`, `116`, `83`, `106`, `97`, `102`, `80`. `valueSoftReferenceNode90` copies `116` |
| 7 | u64be | name, decoded by `World.opWorld70` |
| 8 | u8 | `valueIntNode19` |
| 9 | u16be | `valueIntNode34` |

`call90268241` itself was not read.

## Existing NPCs

`MonochromeImageCache.opMonochromeImageCache12` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonochromeImageCache.java:95`) is the same 8-bit count and the same 1-bit / 2-bit types as other players. A count that is too large throws `gnpov1`. There is no sentinel index.

## New NPCs

`MouseWheel.opMouseWheel5` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MouseWheel.java:98`) loops while remaining bits are at least 27 (`~opCache6(size) <= -28`).

| Bits | Meaning |
| --- | --- |
| 15 | index. 32767 ends the loop |
| 3 | orientation index, stored only if the slot was created |
| 5 | signed scene-x delta from the local player. Values above 15 subtract 32 |
| 1 | 1 queues a mask |
| 14 | type id, `QuickChatPhraseType.opQuickChatPhraseType16` |
| 1 | clear flag. The call treats `~bit == -2`, which is bit == 1 |
| 5 | signed scene-z delta, same bias |

The place call is `opSoftReferenceNode12(localZ + zDelta, clear, localX + xDelta)`. The type's anim and size fields are copied onto the NPC before that call. The method then byte-aligns.

## NPC masks

`Scenery.opScenery8` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Scenery.java:59`) reads one u8. Bit `0x80` is a mask, not an extension byte. Test order:

| Bit | Fields |
| --- | --- |
| 0x1 | u16le type id, then the type's anim and size fields are copied |
| 0x8 | u16le with low byte +128 (65535 becomes -1); u32 with wire bytes `>>16`, `>>24`, low, `>>8`. The low 16 bits are added to the cycle |
| 0x4 | u8 stored as `-value`, another u8 stored as `-value`, u8+128, u8 stored as `128 - value` |
| 0x40 | jstr. The timer is set to 100 |
| 0x20 | u16le (65535 becomes -1) |
| 0x80 | u8, u8+128, u8+128, u8 |
| 0x10 | u16le with low byte +128 (65535 becomes -1), u8+128 |
| 0x2 | u16le, u16le with low byte +128 |

## Short place

Server packet 45 is a separate place, not the bit packet (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1734`):

| Order | Width | Use |
| --- | --- | --- |
| 1 | u8 stored as `128 - value` | plane is `byte >> 1`. Bit 0 is the clear flag |
| 2 | u8 stored as `-value` | scene x, the third argument |
| 3 | u8 stored as `value + 128` | scene z, the first argument |

`opSoftReferenceNode12(z, (first & 1) == 1, x)` runs on the local player. The plane is also written to `CursorManager.valueCursorManager9`.
