# Cache

The cache is JS5. `Client.opClient163` opens a dat2 file at `5200`, sixteen index files at `6000`, and reference table 255 (`builds/rs2/474-2007-10-19/src/src/main/java/rt4/Client.java:250-259`). `GlSprite.valueGlSprite9` is `Timer(255, dat, idx255, 500000)`. `NpcType.opNpcType16` wraps one index. The update-socket handshake is in [framing.md](framing.md).

`MidiNote.opMidiNote10` opens the sixteen indexes (`rt4/MidiNote.java:62-77`). Download weights until the loading percent hits 100 are 4, 4, 2, 2, 6, 4, 2, 60, 2, 2, 2, 2, 2, 2, 2, 2 (`rt4/MidiNote.java:82-97`).

| Index | Holder | Weight | Used as |
| --- | --- | --- | --- |
| 0 | `Timer.valueTimer30` | 4 | passed with index 1 into `TextureOp12.opTextureOp123` (`rt4/MidiNote.java:174`) |
| 1 | `Timer.valueTimer26` | 4 | same call |
| 2 | `SoftReferenceNodeFactory.valueSoftReferenceNodeFactory33` | 2 | config archive passed to the loaders below |
| 3 | `TextureOp.valueTextureOp83` | 2 | interface archive. `deob.World.opkind2385` stores this third argument and allocates `NoReplyLoginserver[fileCount][]` (`deob/World.java:93-98`, call at `rt4/MidiNote.java:178`) |
| 4 | `QuickChatPhrase.valueQuickChatPhrase23` | 6 | first argument of `Bzip2DState.opBzip2DState12` (`rt4/MidiNote.java:115`) |
| 5 | `TextureOp16.valueTextureOp16132` | 4 | map archive. Terrain and loc names are resolved on it (`rt4/ByteArrayNodeSecondary.java:96-97`) |
| 6 | `GlIndexedSprite.valueGlIndexedSprite2` | 2 | opened here. Later group use was not read |
| 7 | `CursorType.valueCursorType29` | 60 | first argument of `deob.World.opkind2385`, stored as `C48fb9567.valuekind2361` |
| 8 | `ClientScript.valueClientScript68` | 2 | second argument of `opkind2385`, stored as `Mouse.valueMouse5`. Title sprites are also loaded from this index by name (`rt4/MidiNote.java:238-316`) |
| 9 | `TextureOp27.valueTextureOp271` | 2 | textures. State 90 builds `TextureOp38` from it (`rt4/MidiNote.java:187-191`) |
| 10 | `TextureOpSprite.valueTextureOpSprite13` | 2 | state 120 loads `LongNode.valueLongNode17` from it (`rt4/MidiNote.java:205-209`) |
| 11 | `PleaseEnterUsername.valuekind2421` | 2 | opened here. Group use was not read |
| 12 | `Js5ResourceProvider.valueJs5ResourceProvider26` | 2 | loading continues while this archive is incomplete (`rt4/MidiNote.java:219-220`) |
| 13 | `ServerActiveProperties.valueServerActiveProperties36` | 2 | fourth argument of `opkind2385`, stored as `ByteArrayNodeSecondary.valueByteArrayNodeSecondary37` |
| 14 | `LadeFreundeListe.valuekind23311` | 2 | second argument of `Bzip2DState.opBzip2DState12` |
| 15 | `MaterialManager_Class106_Sub1_Sub1.valueMaterialManager_Class106_Sub1_Sub16` | 2 | fourth argument of that call |

Index 2 is handed to `TextureOp16.opTextureOp1615`, `Cache.opCache23`, `Js5QuickChatCommandDecoder.opJs5QuickChatCommandDecoder28`, `GlTile.opGlTile4`, `TextureOp12.opTextureOp123`, and `LongNode.opLongNode18` (`rt4/MidiNote.java:170-175`). Those methods store the archive and later read groups. `opSoftwareFont35(file, group)` uses the second argument as the group (`rt4/SoftwareFont.java:291-292`). Constant groups read from index 2 holders are 1 (`rt4/ColorImageCache.java:48`), 3 (`deob/World.java:73`), 4 (`rt4/ObjType.java:72`), 5 (`rt4/ReflectionCheck.java:128`), 6 (`rt4/GlTile.java:43`), 8 (`rt4/AudioChannel.java:94`), 9 (`rt4/Js5ResourceProvider.java:130`), 10 (`rt4/JString.java:92` and `rt4/GlTile.java:65`), 12 (`rt4/SceneGraph_Class120.java:234`), 13 (`deob/ConnectingServer.java:84`), 14 (`rt4/VertexNormal.java:64`), and 16 (`rt4/Packet.java:61`, `rt4/TextureOp35.java:303`). Decode loops that stop on a zero opcode were not read, so this page does not list config opcodes. See [gaps.md](gaps.md).

`ClientScript.opClientScript21` only stores the index 2 archive. It is not a separate script archive. The script interpreter is `TextureOpCurve.opTextureOpCurve18` (`deob/TextureOpCurve.java:113`), and that method was not read.
