# Input

`Client.opClient165` reaches `TextureOp19.opTextureOp194` when the client state is 30 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:290`). That method writes client opcode 119 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp19.java:86`). The bytes after that opcode were not read. Server opcode 119 is a different packet: 24 raw bytes. See [packets.md](packets.md).

The keyboard listener is `TracingException` (`KeyListener`, `FocusListener`). The mouse listener class is `SceneGraph_Class120`. The wheel listener is `LightType`. Their event-to-packet bodies were not read, apart from the walk and chat writers below.

## Walk

`MonochromeImageCache.opMonochromeImageCache13` searches a 104 by 104 grid (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonochromeImageCache.java:151`). Collision is `ObjStackNode.valueObjStackNode19[plane].valueSprite80` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/ObjStackNode.java`, `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Sprite.java`). The x index is `HierKlickenUmFortzufahren.valuekind15732` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/HierKlickenUmFortzufahren.java`) and the z index is `TextureOp29SubOp.valueTextureOp29SubOp5` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp29SubOp.java`).

The last argument `var10` selects the opcode (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonochromeImageCache.java:354`):

| var10 | Opcode |
| --- | --- |
| 0 | 184 |
| 1 | 24 |
| 2 | 110 |

`var31` is the step count before the path index is decremented, capped at 25. The first u8 is a value, written with `opMouseRecorder14`, not `opMouseRecorder15`.

| Opcode | First u8 |
| --- | --- |
| 184 | `2 * steps + 3` |
| 24 | `2 * steps + 17` |
| 110 | `2 * steps + 3` |

Then, for every opcode:

| Order | Width | Value |
| --- | --- | --- |
| 1 | u16le, low byte `+ 128` | first waypoint x plus base x |
| 2 | for each later step, u8 stored as `-dx` | waypoint x minus the first x |
| 3 | and u8 stored as `dz + 128` | waypoint z minus the first z |
| 4 | u8 stored as `-value` | 1 when `MaterialRenderer.valueMaterialRenderer7[82]` is set, otherwise 0 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialRenderer.java`) |
| 5 | u16be | first waypoint z plus base z |

There is no client size table. `rslib/src/client/packets/468.c` publishes `rs_sizes_c2s_468`, including entries for 184, 24, and 110. This Java client does not have that array.

Which caller passes `var10` 0, 1, or 2 was not traced. Key 82 is the run flag in this writer.

## Chat

`ServerBeingUpdated` writes opcode 89 (`builds/rs2/468-2007-08-08/src/src/main/java/deob/ServerBeingUpdated.java:148`):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | `string.opScenery6() - 1` |
| 2 | jstr | `string.opScenery34(2)` |

The lines above that write were not read. Server opcode 89 is a different packet and its handler was not read.

## Other writes that were read

Opcode 205, `MonochromeImageCache.opMonochromeImageCache10` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonochromeImageCache.java:83`): u8 stored as `128 - value`, then u32le of the high half of a long and u32le of the low half (`opMouseRecorder12`).

Opcode 194, `JString` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/JString.java:258`): u64be of `valueJs5ResourceProvider11`.

Opcode 183, `JavaMouseWheel.opJavaMouseWheel10` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/JavaMouseWheel.java:118`): a u8 0 written as a placeholder, then u32be `valuePlayerEntity47`. The loop after that was not read to its end. The placeholder is the kind of byte `opMouseRecorder15` backpatches; the backpatch call itself was not in the lines that were read.

Opcode 78, `Client` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:145`), is followed by `VorbisFloor.valueVorbisFloor9.opLongNode3` of the same buffer. The bytes `opLongNode3` writes were not read.

Every other `opCache8` site is an opcode row in [packets.md](packets.md) with an empty field list.
