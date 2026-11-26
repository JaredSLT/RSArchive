# Scene

The software scene is 4 planes of 104 by 104 tiles, built in `MidiNote.opMidiNote10` (`builds/rs2/474-2007-10-19/src/src/main/java/rt4/MidiNote.java:37`). While the game state is 30, `Client.opClient164` calls `TextureOp19.opTextureOp197` (`rt4/Client.java:739-740`). The frame buffer is `WallDecor`, an `ImageProducer`. State 30 copies dirty rectangles through `RawModel.valueRawModel33.opWallDecor5` (`rt4/Client.java:748-756`).

Camera samples that leave the client are opcode 169 in [input.md](input.md). The two u16le fields are `ConnectingServer.valuekind23717` and `TextureOp11.valueTextureOp116`. The method that writes those fields from keys 96 through 99 was not read, so this page does not name yaw and pitch.

Zone x and zone z origins are `(zoneX - 6) * 8` and `zoneZ * 8 - 48` (`rt4/SceneGraph_Class120.java:91-92`). Low detail forces the render plane to 0 (`rt4/SceneGraph_Class120.java:83-85`).

Model, sprite, and terrain draw methods inside `TextureOp19`, `SoftwareModel`, and `PlayerAppearance` were not read. The draw entry point is the call above.

## HD entry points

This source tree has no `jaggl`, `jagdx`, or `jagtheora` files. HD types are loaded by name and are absent here:

| Site | Name |
| --- | --- |
| `rt4/VertexNormal.java:53` | `uc` |
| `rt4/ReflectionCheck.java:157` | `je` |
| `rt4/QuickChatPhrase.java:47` | `mc` |
| `rt4/MaterialManager_Class106_Sub1.java:52` | `jb` |
| `rt4/Scenery.java:139` | `hf` |

`VertexNormal.opVertexNormal39` catches the failure and returns `new TextureOpSprite()`. `WorldMapFont.java:16` is `Class.forName` used as a class lookup, not one of those HD names. `GlModel`, `GlSprite`, `GlTexture`, and the other `Gl*` types in this tree are Java classes.
