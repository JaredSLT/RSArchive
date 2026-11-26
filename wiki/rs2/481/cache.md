# Cache

JS5. `SieBefindenSichEinemMitgliederGebiet.opkind2782` creates sixteen archives (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SieBefindenSichEinemMitgliederGebiet.java:60`). The three constructor booleans on `C105f7f5d.opkind3426` were not read.

| Index | Field |
| --- | --- |
| 0 | `QuickChatPhrase.valueQuickChatPhrase25` |
| 1 | `C5afc506d.valuekind3493` |
| 2 | `MouseRecorder.valueMouseRecorder30` |
| 3 | `WorldFull.valuekind29111` |
| 4 | `TextureOp29SubOp2.valueTextureOp29SubOp247` |
| 5 | `VarpType.valueVarpType88` |
| 6 | `VorbisSound.valueVorbisSound72` |
| 7 | `ByteArrayNode.valueByteArrayNode52` |
| 8 | `Js5GlTextureProvider.valueJs5GlTextureProvider13` |
| 9 | `StringInterface.valueStringInterface56` |
| 10 | `BenutzenSieDieWpasswortNdernOption.valuekind31113` |
| 11 | `C78d60e9c.valuekind3383` |
| 12 | `TextureOp29SubOp2.valueTextureOp29SubOp245` |
| 13 | `Sprite.valueSprite92` |
| 14 | `Song.valueSong18` |
| 15 | `SceneGraph_Class120.valueSceneGraph_Class12070` |

Index 2 is the config archive passed into the loaders at lines 172-184. Index 3 is interfaces (`C1e98a145.opkind3302` stores it on `C1fd292b8.valuekind2605`). Index 5 is maps. Index 7 is models (`C68c7fcfe.opkind2882` stores it on `Node.valueNode33`). Index 6 is the music archive used by opcode 148. Index 11 is the sound archive used by opcode 46. Index 4 is passed to `Sprite.callffb0c548` with indexes 14 and 15 (`SieBefindenSichEinemMitgliederGebiet.java:113`).

A config file is a `u8` opcode loop until 0.

## Groups whose decode was read to the end

| Group | Archive field | Class | Opcodes |
| --- | --- | --- | --- |
| 1 | `C6e114c83.valuekind3002` | `C68c7fcfe.opkind2888` | 1: u24be. 2: u16be, 65535 becomes -1. 3: u16be discarded. `builds/rs2/481-2007-12-11/src/src/main/java/deob/C68c7fcfe.java:256`. Loaded at `SichtbareKarteVorbereitet.java:32`. |
| 3 | `SekundeXnUbertragen.valuekind22627` | `Cd9f578c5.opkind3124` | 1: u8. 2: u8 count, count u16be. 3: flag, no read. 40: u8 count, pairs of u16be. 41: same. 60 through 69: one u16be each. `builds/rs2/481-2007-12-11/src/src/main/java/deob/Cd9f578c5.java:120`. Loaded at `TextureOp17.java:240`. |
| 4 | `IntHashTable.valueIntHashTable30` | `HardReferenceNode.opHardReferenceNode16` | 1: u24be. 2: u8. 3: u16be, 65535 becomes -1. 5: flag, no read. 7: u24be. 8: no read. 9: u16be discarded. `builds/rs2/481-2007-12-11/src/src/main/java/deob/HardReferenceNode.java:393`. Loaded at `WorldFull.java:38`. |
| 5 | `TextureOpCurve.valueTextureOpCurve71` | `C6e114c83.opkind3001` | 2: u16be. `builds/rs2/481-2007-12-11/src/src/main/java/deob/C6e114c83.java:164`. Loaded at `ByteArrayNodeSecondary.java:48`. |
| 8 | `C4148662f.valuekind2585` | `TextureOp27.opTextureOp2717` | 1: u8. 2: u8. 3: jstr. 4: u32be. 5: u16be count, then pairs of u32be and jstr. 6: u16be count, then pairs of u32be and u32be. `builds/rs2/481-2007-12-11/src/src/main/java/deob/TextureOp27.java:219`. Loaded at `VarpType.java:203`. |
| 12 | `TextureOp17.valueTextureOp176` | `C218af8f4.opkind3089` | 1: u16be count, count u16be frames, count u16be, then each second value adds another u16be shifted 16. 2: u16be. 3: u8 count, count u8, then one extra slot 9999999. 4: flag, no read. 5: u8. 6: u16be. 7: u16be. 8: u8. 9: u8. 10: u8. 11: u8. 12: u8 count, count u16be, then each adds a u16be shifted 16. 13: u8 count, count u24be. `builds/rs2/481-2007-12-11/src/src/main/java/deob/C218af8f4.java:158`. Loaded at `BenutzenSieDieWpasswortNdernOption.java:86`. |
| 13 | `Midi_Class162.valueMidi_Class16220`, which is index 2 | `IntHashTable.opIntHashTable9` | 1: u16be. 2: u16be. 4: u16be. 5: u16be. 6: u16be. 7: u8. 8: u8. 40: u8 count, pairs of u16be. 41: same. `builds/rs2/481-2007-12-11/src/src/main/java/deob/IntHashTable.java:53`. Loaded at `AccountLockedAsWeSuspectHasBeenStolen.java:96`. |
| 14 | `Cd9f578c5.valuekind31214` | `TextureOp.opTextureOp22` | 1: u16be, u8, u8. `builds/rs2/481-2007-12-11/src/src/main/java/deob/TextureOp.java:88`. Loaded at `C24ca5256.java:41`. |
| 16 | `C218af8f4.valuekind30811` | `Ccc509c66.opkind3525` | 5: u16be. `builds/rs2/481-2007-12-11/src/src/main/java/deob/Ccc509c66.java:193`. Count is `opkind34711(16)` at `Node.java:35`. Loaded at `NoResponseServer.java:114`. |

## Groups whose switch continues

Group 6 is `StringInterface.opStringInterface19` from `CursorType.valueCursorType40` (`Ccc509c66.java:112`). The method starts at `builds/rs2/481-2007-12-11/src/src/main/java/deob/StringInterface.java:434` and was not read to its end.

Group 9 is `ImageProducerFrameBuffer.call8e0819bb` from `MsiType.valueMsiType98` (`Js5.java:81`). NPC add passes the type id to `Js5.opJs512`. The method starts at `builds/rs2/481-2007-12-11/src/src/main/java/deob/ImageProducerFrameBuffer.java:324` and was not read to its end.

Group 10 is `PlayerEntity.opPlayerEntity31` from `PathingEntity_Class147.valuePathingEntity_Class14721` (`VarpType.java:73`). The method starts at `builds/rs2/481-2007-12-11/src/src/main/java/deob/PlayerEntity.java:391` and was not read to its end.

Map terrain and loc bytes are requested by name from index 5. See [region.md](region.md). `MalformedLoginPacket.opkind2289`, `ByteArrayNodeSecondary.call099838cc`, `WorldMapFont.opWorldMapFont12`, and `BenutzenSieDieWpasswortNdernOption.call9b06a231` were not read.

Interfaces are index 3. See [interfaces.md](interfaces.md).
