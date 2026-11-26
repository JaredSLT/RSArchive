# Cache

Archive open is `Js5.callf78aac57` at stage 30 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5.java:88`). Each call is `Cef468e02.opkind2071(index, a, b, c)`. The body of `opkind2071` was not read. The flags below are the three booleans at the call.

| Index | Field | Flags |
| --- | --- | --- |
| 0 | `ColFfff.valuekind21521` | true, false, true |
| 1 | `SpotAnimType.valueSpotAnimType87` | true, false, true |
| 2 | `Js5NetQueue.valueJs5NetQueue62` | true, true, false |
| 3 | `MouseRecorder.valueMouseRecorder2` | true, false, true |
| 4 | `PleaseWaitMinutesBeforeTryingAgain.valuekind12327` | true, false, true |
| 5 | `TextureOpInvert.valueTextureOpInvert3` | true, true, true |
| 6 | `Song.valueSong6` | false, true, true |
| 7 | `MidiPcmStream.valueMidiPcmStream32` | true, false, true |
| 8 | `ByteArrayNode.valueByteArrayNode32` | true, false, true |
| 9 | `PleaseSubscribeUseDifferentWorld.valuekind13021` | true, false, true |
| 10 | `TextureOp29SubOp2.valueTextureOp29SubOp224` | true, false, true |
| 11 | `StringInterface.valueStringInterface44` | true, false, true |
| 12 | `StringInterface.valueStringInterface25` | true, false, true |
| 13 | `WirVermutenIhrSpielkontoWurdeGestohlen.valuekind15538` | true, true, false |
| 14 | `QuickChatCommandDecoder.valueQuickChatCommandDecoder21` | false, false, true |
| 15 | `Js5ResourceProvider.valueJs5ResourceProvider4` | true, false, true |

Index 3 is the interface archive passed to `GlFont.call65419ac6` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5.java:204`). Index 5 is the map archive. See [region.md](region.md).

Stage 70 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5.java:189`) passes archive 2 into `MidiNote.opMidiNote9`, `ByteArrayNodeSecondary.opByteArrayNodeSecondary19`, `ByteArray.opByteArray3` (also archive 7), `TextureOp19.opTextureOp193`, `GlModel.opGlModel10`, `ChangeLocRequest.callacc4b7b5`, `MapElement.call8d5bcb84` (archives 1, 2, and 0), `Node.opNode2`, `SoftwareFont.opSoftwareFont10`, `ByteArray.opByteArray4`, `World.call354043a1`, and `StringInterface.opStringInterface7`. Those loaders were not read except for the group ids below.

## Groups read by file id

`opByteArrayNode13(group, file)` sites that use a numeric group on a config archive:

| Group | Archive field | Read at | Consumer |
| --- | --- | --- | --- |
| 1 | `JString.valueJString16` | `builds/rs2/468-2007-08-08/src/src/main/java/deob/ServerBeingUpdated.java:84` | not decoded here |
| 3 | `Component.valueComponent200` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOpInvert.java:32` | not decoded here |
| 4 | `TextureOpCurve.valueTextureOpCurve26` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GzipDecompressor.java:332` | not decoded here |
| 5 | `GlSolidColorTexture.valueGlSolidColorTexture19` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlSolidColorTexture.java`) | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/JString.java:237` | not decoded here |
| 6 | `Js5.valueJs52` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonotonicClock.java:214` | `VertexNormal`, the loc type |
| 8 | `ReferenceNode.valueReferenceNode26` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ProjAnim.java:132` | not decoded here |
| 9 | `Packet.valuePacket50` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/QuickChatPhraseType.java:101` | `JavaMouseWheel` NPC type via `opQuickChatPhraseType16` |
| 10 | `PathingEntity.valuePathingEntity108` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5ResourceProvider.java:27` | `TextureOp` obj type |
| 12 | `Texture.valueTexture4` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Texture.java`) | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/JavaMouseWheel.java:281` | `TextureOp38` sequence via `opJavaMouseWheel16` |
| 13 | `PathingEntity.valuePathingEntity114` | `builds/rs2/468-2007-08-08/src/src/main/java/deob/SpotAnimType.java:79` | not decoded here |
| 14 | `TextureOpTiledSprite.valueTextureOpTiledSprite4` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:455` | not decoded here |
| 16 | `ParticleSystem.valueParticleSystem12` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/PacketBit.java:317` | not decoded here |

`MapMarker.java:170` reads file `(0, 0)` of the archive it is given. Groups 2, 7, 11, and 15 were not found as a numeric `opByteArrayNode13` argument. No names are assigned to them here.

## Decode opcodes

Each loop reads a u8 and stops on 0. Field widths inside an opcode are unread unless the row says otherwise.

Obj, `TextureOp.opTextureOp9` / `opTextureOp11` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp.java:338`): 1 (u16be), 2 (jstr), 4, 6, 7, 8, 11, 16, 24, 26, 78, 91, 92, 93, 95, 98, 110, 111, 113, 115. Only opcodes 1 and 2 were read (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp.java:350`).

NPC, `JavaMouseWheel.opJavaMouseWheel15` / `opJavaMouseWheel19` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/JavaMouseWheel.java:382`): 1, 2, 13, 15, 60, 98, 99, 100, 101, 102, 107, 109. The comparisons are at `builds/rs2/468-2007-08-08/src/src/main/java/rt4/JavaMouseWheel.java:464`.

Loc, `VertexNormal.opVertexNormal12` / `opVertexNormal9` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/VertexNormal.java:654`): 1, 15, 17, 18, 19, 21, 22, 23, 24, 28, 39, 41, 62, 65, 67, 68, 69, 70, 71, 73, 74, 75, 77, 78, 81. The comparisons are at `builds/rs2/468-2007-08-08/src/src/main/java/rt4/VertexNormal.java:388`.

Sequence, `TextureOp38.opTextureOp389` / `opTextureOp3811` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp38.java:87`): 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13. The comparisons are at `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp38.java:135`.

Widget bytes are in [interfaces.md](interfaces.md).
