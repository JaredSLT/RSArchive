# Login

The state machine is `CursorManager.call08d5362c` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:51`). The stage is `ByteArrayNodeSecondary.valueByteArrayNodeSecondary24`. The socket is `ProjAnimNode.valueProjAnimNode14`, a `SoundPcmStream` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/ProjAnimNode.java`). Outbound bytes are staged in `ParticleSystem.valueParticleSystem8` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/ParticleSystem.java`) and `ByteArray.valueByteArray1` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/ByteArray.java`), both `Cache` buffers. Inbound bytes land in `MaterialManager_Class106_Sub1.valueMaterialManager_Class106_Sub118`.

Buffer widths are the methods in [framing.md](framing.md).

## Stage 0 and 1

Stage 0 closes an existing socket and sets stage 1 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:53`).

Stage 1 connects with `Packet.valuePacket52.opLoc19(Js5.valueJs59, KeineAntwortVomServer.valuekind16819)` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:67`). Status 2 throws `IOException`. Status equal to 1 (the decompiler test `~status == -2`) installs the socket and moves to stage 2.

## Handshake the client writes

Stage 2 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:81`):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 14 |
| 2 | u8 | `(usernameLong >> 16) & 31` |

`usernameLong` is `TextureOp19.valueTextureOp1913.opScenery25()`. Both bytes are `opMouseRecorder14`. The socket send length is 2.

## Handshake the client reads

Stage 3 reads one raw byte with `SoundPcmStream.opSoundPcmStream11` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:101`). A value other than 0 is passed to `MouseRecorder.opMouseRecorder40` and the method returns. 0 continues to stage 4.

Stage 4 reads 8 bytes and decodes them with `opMouseRecorder26`, a u64be (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:133`). That value is `QuickChatCommandDecoder.valueQuickChatCommandDecoder3`, the server seed.

## Login block

Stage 5 builds an inner buffer, RSA-wraps it, then writes an outer header (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:140`).

Inner buffer, before RSA:

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 10 |
| 2 | u32be | `(int)(Math.random() * 9.9999999E7)` |
| 3 | u32be | a second value from the same expression |
| 4 | u32be | server seed >> 32 |
| 5 | u32be | server seed low 32 bits |
| 6 | u64be | username long (`opMouseRecorder39`) |
| 7 | jstr | password (`opMouseRecorder21`, characters plus a 0 byte) |

`opMouseRecorder50(Js5.valueJs51, Js5NetQueue.valueJs5NetQueue60)` replaces that buffer with `u8` length plus the `modPow` bytes (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MouseRecorder.java:654`). The modulus is `Js5.valueJs51` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5.java:20`):

`7162900525229798032761816791230527296329313291232324290237849263501208207972894053929065636522363163621000728841182238772712427862772219676577293600221789`

The exponent is `Js5NetQueue.valueJs5NetQueue60` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5NetQueue.java:27`):

`58778699976184461502525193738213253649000149147835990136706041084440742975821`

Outer buffer:

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 18 when `TextureOpInvert.valueTextureOpInvert2 == 40`, otherwise 16 |
| 2 | u8 | RSA block length + 93 |
| 3 | u32be | 468 |
| 4 | u8 | 1 when `Packet.valuePacket64` is true, otherwise 0 |
| 5 | bytes(24) | `Js5ResourceProvider.opJs5ResourceProvider3` |
| 6 | 16 × u32be | archive CRCs, index 0 through 15, in the order written below |
| 7 | bytes | the RSA block, including its length byte |

The 24-byte block is read from `TextureOp29SubOp4.valueTextureOp29SubOp411`. If that stream is missing, the read throws, or all 24 bytes are 0, every byte is written as `0xFF` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5ResourceProvider.java:51`).

CRC order (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:162`): `ColFfff.valuekind21521`, `SpotAnimType.valueSpotAnimType87`, `Js5NetQueue.valueJs5NetQueue62`, `MouseRecorder.valueMouseRecorder2`, `PleaseWaitMinutesBeforeTryingAgain.valuekind12327`, `TextureOpInvert.valueTextureOpInvert3`, `Song.valueSong6`, `MidiPcmStream.valueMidiPcmStream32`, `ByteArrayNode.valueByteArrayNode32`, `PleaseSubscribeUseDifferentWorld.valuekind13021`, `TextureOp29SubOp2.valueTextureOp29SubOp224`, `StringInterface.valueStringInterface44`, `StringInterface.valueStringInterface25`, `WirVermutenIhrSpielkontoWurdeGestohlen.valuekind15538`, `QuickChatCommandDecoder.valueQuickChatCommandDecoder21`, `Js5ResourceProvider.valueJs5ResourceProvider4`. That is the same index order as `Js5.java:88`.

The four ints from the inner block seed the outbound ISAAC with `ParticleSystem.valueParticleSystem8.opCache11`. Each int is then increased by 50 and that array seeds the inbound ISAAC (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:180`).

## Status byte

Stage 6 reads one raw byte when the socket has data (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:190`):

| Byte | Result |
| --- | --- |
| 21, and client state is 20 | stage 7 |
| 2 | stage 9, success |
| 15, and client state is 40 | `VertexNormal.opVertexNormal11`, then return. That method was not read |
| 23, and `Cef468e02.valuekind2079 < 1` | stage 0 and the attempt counter increments |
| anything else | `MouseRecorder.opMouseRecorder40` |

Stage 7 reads one raw byte. The wait is `180 + byte * 60`, stored in `Sprite.valueSprite71` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:212`). Stage 8 decrements that counter and returns to stage 0 at 0.

## Success bytes

Stage 9 runs when at least 8 socket bytes are available (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:225`):

| Order | Width | Stored as |
| --- | --- | --- |
| 1 | u8 | `Keyboard.valueKeyboard35` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Keyboard.java`) |
| 2 | u8 | members flag, true when the byte is 1 (`SceneGraph_Class120.valueSceneGraph_Class12016`) |
| 3 | u8 | high half of `PleaseEnterUsername.valuekind21113`, shifted left 8 |
| 4 | u8 | low half, added to that index |
| 5 | u8 | `MaterialManager_Class106_Sub1.valueMaterialManager_Class106_Sub116` |
| 6 | ISAAC u8 | `TextureOp4.valueTextureOp420` via `opCache2` |
| 7 | u16be | `SoftReferenceNode.valueSoftReferenceNode76` via `opMouseRecorder30` |

This first game packet does not consult `RawModel.valueRawModel23`. Stage 10 waits until that many body bytes are present, then calls `HashTable.opHashTable7` (not read) and `TextureOpCurve.opTextureOpCurve14(false)` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:241`).

## Timeout and the failure display

If `KeineAntwortVomServer.valuekind16818` passes 2000, the client swaps `KeineAntwortVomServer.valuekind16819` between `SoftwareFont.valueSoftwareFont38` and `Component.valueComponent197`, returns to stage 0 when the attempt counter is still 0, and otherwise calls `opMouseRecorder40(-3)` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:253`). An `IOException` does the same swap and calls `opMouseRecorder40(-2)` after the first attempt (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:269`).

`MouseRecorder.opMouseRecorder40` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MouseRecorder.java:272`) has branches for -3, -2, -1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 22, 23, 24, 25, 26, 27, and a default. Each branch passes three `Scenery` values to `ClientScript.calld56c3336`. Those fields were not traced to their literal strings. Response 2 is not a branch of this method.
