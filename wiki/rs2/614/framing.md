# Framing

Cache kind is js5. The incoming buffer is `EnvironmentManager.valueEnvironmentManager2`, an `ParticleEmitterRelated` of 7500 bytes (builds/rs2/614-2010-07-13/src/src/main/java/deob/EnvironmentManager.java:4). The outgoing buffer is `TextureOp19.valueTextureOp1999`, 5000 bytes (builds/rs2/614-2010-07-13/src/src/main/java/deob/TextureOp19.java:7). `ParticleEmitterRelated` extends `Unused6`.

## ISAAC direction

Each buffer holds its own `Pix3D`. `opPix3D7` consumes one word (builds/rs2/614-2010-07-13/src/src/main/java/deob/Pix3D.java:197). `opPix3D8` peeks the next word (builds/rs2/614-2010-07-13/src/src/main/java/deob/Pix3D.java:210).

C2S opcode byte, `opParticleEmitterRelated1` (builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitterRelated.java:55): one byte, `isaac.opPix3D7 + opcode`. The guard byte is 108. There is no second opcode byte on write.

S2C opcode, `opParticleEmitterRelated4` (builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitterRelated.java:63): `byte - isaac.opPix3D7`. If that value is below 128 it is the opcode. Otherwise a second byte is read, `second - isaac`, and the opcode is `((first - 128) << 8) + second`. The guard is 23774.

Before that decode, `opParticleEmitterRelated2` peeks `(data[offset] - isaac.opPix3D8) & 255` and, when the result is at least 128, the framer reads one extra socket byte (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:158).

The four login seeds initialise the outgoing cipher. Each seed is then increased by 50 and those values initialise the incoming cipher (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:206).

## S2C size rule

`CameraEffectType` stores the opcode and the size (builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraEffectType.java:37). `MdaWrite.opMdaWrite1` returns the 117 packet objects (builds/rs2/614-2010-07-13/src/src/main/java/deob/MdaWrite.java:13). The framer indexes that array with the decrypted opcode (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:169). Every array index equals that entry's constructor opcode.

`GlModelRelated5.opGlModelRelated52` (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:141) then applies the size:

| constructor size | what the framer reads |
| --- | --- |
| -1 | one u8 length, then that many payload bytes |
| -2 | one u16be length (`opUnused6160`), then that many payload bytes |
| 0 | no payload |
| n > 0 | n payload bytes |

The length byte or u16be is not left in the buffer the handler reads. The payload read resets the cursor to 0. A positive size test is `~size < -1` (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:179).

`Client.callad91983c` polls `BaseTexture.opBaseTexture3` up to 100 times when the world state is 7 and the login state is 0 (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:2173, builds/rs2/614-2010-07-13/src/src/main/java/deob/BaseTexture.java:23). Static rebuild and dynamic rebuild return false from the poll. The in-game tick calls this poll, then on one branch `KeyHeldBinding.opKeyHeldBinding8`, then `MapCoordUtil.calla978325d(3)` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:2021).

## C2S size rule

`FloorUnderlayType` stores only the opcode. The size argument is discarded (builds/rs2/614-2010-07-13/src/src/main/java/deob/FloorUnderlayType.java:123). The sizes below are the constructor arguments in this client. `opFloorUnderlayType5(15633)` returns the opcode (builds/rs2/614-2010-07-13/src/src/main/java/deob/FloorUnderlayType.java:132).

`FullscreenMode.call0d0697f8` writes the ISAAC opcode byte (builds/rs2/614-2010-07-13/src/src/main/java/deob/FullscreenMode.java:44). Packets are concatenated in the outgoing buffer. When its offset is greater than 0 and the world state is 7, the client flushes that buffer raw, with no extra length header (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:2314). A per-packet length exists only when the writer itself writes one. `opUnused6143` backpatches a u16be length (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:293). `call170b03fc` backpatches one length byte (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:619).

Opcode 0 is `Material.valueMaterial6`, constructor size 0. When `GlRelated8.valueGlRelated82` passes 50 the client writes that opcode and no body (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:2308).

## Bit cursor

`opParticleEmitterRelated3(bitCount, 8)` reads `bitCount` bits. The second argument is 8 at the call sites read for sync and region. `opParticleEmitterRelated9` sets the bit cursor to `offset * 8` (builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitterRelated.java:100). `opParticleEmitterRelated6(-29878)` sets the byte offset to `(bitCursor + 7) / 8` (builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitterRelated.java:79).

## Words that are not one endianness

`opUnused6127(false)` is `u32_badc` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:21). Buffer bytes A, B, C, D become `(B<<24) + (A<<16) + (D<<8) + C`. A true argument writes another word before those four bytes. The sites named `u32_badc` pass false.

`call1cda1d8d(18523)` is `u32_cdab` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:128). The same byte order becomes `(C<<24) + (D<<16) + (A<<8) + B`. Any other argument nulls the buffer. The sites named `u32_cdab` pass 18523.

## S2C constructor sizes

Opcode, size, field, source. Size -1 and -2 keep the meaning in the table above.

| opcode | size | field | citation |
| --- | --- | --- | --- |
| 0 | 8 | valueCutsceneEntity3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CutsceneEntity.java:9 |
| 1 | 0 | valueWorldMapFont114 | builds/rs2/614-2010-07-13/src/src/main/java/rt4/WorldMapFont.java:12 |
| 2 | -1 | valueJdaInit11 | builds/rs2/614-2010-07-13/src/src/main/java/deob/JdaInit.java:6 |
| 3 | 3 | valueSynthInstrument32 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SynthInstrument.java:14 |
| 4 | -1 | valueKcaInit4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/KcaInit.java:9 |
| 5 | -2 | valueGpuRelated131 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuRelated13.java:10 |
| 6 | 3 | valueSoundPcmStream126 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SoundPcmStream.java:5 |
| 7 | 8 | valuekind10793 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C10c7aac2.java:14 |
| 8 | -2 | valuekind10594 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cd8dce00d.java:4 |
| 9 | 14 | valueBZip2State91 | builds/rs2/614-2010-07-13/src/src/main/java/deob/BZip2State.java:5 |
| 10 | -2 | valueStaticPointLight15 | builds/rs2/614-2010-07-13/src/src/main/java/deob/StaticPointLight.java:12 |
| 11 | 2 | valueStructTypeList5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/StructTypeList.java:9 |
| 12 | 0 | valueWorldMapRelated4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/WorldMapRelated.java:6 |
| 13 | 0 | valueTriggerType3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/TriggerType.java:8 |
| 14 | 8 | valueUnused1643 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused16.java:26 |
| 15 | 3 | valueParticleEffectorType1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEffectorType.java:7 |
| 16 | 12 | valueEntityBounds1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/EntityBounds.java:7 |
| 17 | 2 | valueSampleRateConverter5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SampleRateConverter.java:5 |
| 18 | 3 | valueModelShader6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ModelShader.java:4 |
| 19 | 14 | valueCameraTrackableType2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraTrackableType.java:10 |
| 20 | -1 | valueEntityBounds3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/EntityBounds.java:4 |
| 21 | 7 | valueClientScriptCommand17 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ClientScriptCommand.java:17 |
| 22 | -1 | valueNdaInit7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/NdaInit.java:5 |
| 23 | 1 | valueSkyBoxRelated8 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SkyBoxRelated.java:4 |
| 24 | -1 | valueColourUtils5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ColourUtils.java:9 |
| 25 | 6 | valueLoginManager17 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoginManager.java:18 |
| 26 | -2 | valueRayTracing9 | builds/rs2/614-2010-07-13/src/src/main/java/deob/RayTracing.java:9 |
| 27 | 8 | valueMidiSoundRelated32 | builds/rs2/614-2010-07-13/src/src/main/java/deob/MidiSoundRelated3.java:6 |
| 28 | 10 | valueUnused131 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused13.java:6 |
| 29 | -1 | valueIndexBuffer1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/IndexBuffer.java:6 |
| 30 | -1 | valuekind10797 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C10c7aac2.java:11 |
| 31 | -1 | valueRaaInit2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/RaaInit.java:6 |
| 32 | 8 | valueByteArrayCopier13 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ByteArrayCopier.java:14 |
| 33 | 1 | valueObjTypeListRelated3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ObjTypeListRelated.java:9 |
| 34 | -1 | valueMaterialManager_Class106_Sub2_Sub175 | builds/rs2/614-2010-07-13/src/src/main/java/deob/MaterialManager_Class106_Sub2_Sub1.java:6 |
| 35 | 7 | valueEnvironment74 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Environment.java:11 |
| 36 | 20 | valuekind105414 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cb1b02109.java:13 |
| 37 | -2 | valueGpuShader1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuShader.java:7 |
| 38 | 4 | valueSynthInstrument30 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SynthInstrument.java:13 |
| 39 | 6 | valueUnused181 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused18.java:13 |
| 40 | 10 | valueSkyBoxType3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SkyBoxType.java:8 |
| 41 | 0 | valueUnused12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused1.java:7 |
| 42 | -2 | valueTextCoord1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/TextCoord.java:20 |
| 43 | 4 | valueBasicMaterialTypeList2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/BasicMaterialTypeList.java:5 |
| 44 | 6 | valueBinding1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Binding.java:6 |
| 45 | -2 | valuePathingEntity_Class147114 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PathingEntity_Class147.java:8 |
| 46 | -2 | valuePreLoadingScreen33 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PreLoadingScreen.java:8 |
| 47 | 4 | valueAudioRelatedPcm122 | builds/rs2/614-2010-07-13/src/src/main/java/deob/AudioRelatedPcm12.java:4 |
| 48 | -2 | valuekind10532 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C36c2094a.java:5 |
| 49 | -1 | valueMouseBinding2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/MouseBinding.java:4 |
| 50 | -1 | valueJs5Index16 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5Index.java:5 |
| 51 | 11 | valueSoftLruHashTable60 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SoftLruHashTable.java:10 |
| 52 | 8 | valueJavaAwtComponent3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/JavaAwtComponent.java:9 |
| 53 | 3 | valueCacheKey2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CacheKey.java:8 |
| 54 | 3 | valuePureJavaModelRelated14 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PureJavaModelRelated1.java:8 |
| 55 | 28 | valueGlxRelated1211 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlxRelated12.java:9 |
| 56 | 6 | valueCameraEffectType8 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraEffectType.java:4 |
| 57 | 3 | valueObaInit7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ObaInit.java:10 |
| 58 | -1 | valueGraphicsPacketQueue21 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GraphicsPacketQueue.java:51 |
| 59 | 12 | valueWorldMap10 | builds/rs2/614-2010-07-13/src/src/main/java/deob/WorldMap.java:10 |
| 60 | 6 | valueSynthEnvelope79 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SynthEnvelope.java:9 |
| 61 | 6 | valueCutsceneEntity2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CutsceneEntity.java:7 |
| 62 | 5 | valuekind10381 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cbc0ee57f.java:6 |
| 63 | -2 | valueLoadableResource11 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoadableResource.java:4 |
| 64 | -2 | valueKbaFinalize6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/KbaFinalize.java:8 |
| 65 | 3 | valueOggTheoraStreamRelated8 | builds/rs2/614-2010-07-13/src/src/main/java/deob/OggTheoraStreamRelated.java:13 |
| 66 | 10 | valueServerConnection15 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ServerConnection.java:15 |
| 67 | -2 | valueGlRelated12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlRelated1.java:9 |
| 68 | 12 | valuekind10451 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C88be13e2.java:5 |
| 69 | 2 | valueGlRenderList34 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlRenderList.java:21 |
| 70 | 6 | valueFfaInit2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/FfaInit.java:7 |
| 71 | 12 | valueTextCoord12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/TextCoord.java:6 |
| 72 | 7 | valueHdaInit1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/HdaInit.java:13 |
| 73 | 0 | valueGlTexture32 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlTexture.java:7 |
| 74 | 0 | valueSpriteData1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SpriteData.java:9 |
| 75 | 8 | valueWcaInit4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/WcaInit.java:5 |
| 76 | 17 | valueGraphNode2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GraphNode.java:8 |
| 77 | 7 | valueLoadableResourceType52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoadableResourceType.java:51 |
| 78 | -2 | valueGlBinding9 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlBinding.java:10 |
| 79 | 6 | valueRcaInit1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/RcaInit.java:6 |
| 80 | 5 | valuePureJavaToolkitContext10 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PureJavaToolkitContext.java:5 |
| 81 | -2 | valueQbaInit5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/QbaInit.java:5 |
| 82 | 5 | valueLoadingScreenClearConfig4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoadingScreenClearConfig.java:10 |
| 83 | 6 | valuekind10563 | builds/rs2/614-2010-07-13/src/src/main/java/deob/NonCpCharacter.java:7 |
| 84 | 16 | valuekind10935 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cc61b8c1a.java:13 |
| 85 | 9 | valueLookatMode7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LookatMode.java:11 |
| 86 | 6 | valueEnvironmentSampler3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/EnvironmentSampler.java:8 |
| 87 | 6 | valueCameraHelpers7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:4 |
| 88 | 8 | valueGpuRendererRelated22 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuRendererRelated2.java:8 |
| 89 | 4 | valueClanChannelUser27 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ClanChannelUser.java:18 |
| 90 | 8 | valuePrioritizedCacheMode5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PrioritizedCacheMode.java:6 |
| 91 | 4 | valueUnused52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused5.java:8 |
| 92 | 5 | valueLocShape5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LocShape.java:8 |
| 93 | 8 | valueFont18 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Font.java:6 |
| 94 | 2 | valueGpuTexture5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuTexture.java:5 |
| 95 | -2 | valueLaaToString1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LaaToString.java:7 |
| 96 | 12 | valuekind10931 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cc61b8c1a.java:11 |
| 97 | 1 | valuePingProvider1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PingProvider.java:5 |
| 98 | 6 | valueCutsceneManager3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CutsceneManager.java:4 |
| 99 | 7 | valueVarPlayerTypeList2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/VarPlayerTypeList.java:8 |
| 100 | -1 | valueClientCameraTrackableProvider7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ClientCameraTrackableProvider.java:7 |
| 101 | -1 | valuekind10324 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cf9df8543.java:7 |
| 102 | 8 | valueJs573 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5.java:12 |
| 103 | 2 | valueTriggerType5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/TriggerType.java:5 |
| 104 | 12 | valueJs564 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5.java:8 |
| 105 | 2 | valueMatrix4x32 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Matrix4x3.java:4 |
| 106 | 6 | valuekind10971 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C7643b519.java:4 |
| 107 | -1 | valueGpuCubeTexture4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuCubeTexture.java:20 |
| 108 | 19 | valueCameraHelpers2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:12 |
| 109 | 0 | valueNativeMouseListener5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/NativeMouseListener.java:18 |
| 110 | -1 | valueWorldMapTileShapes3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/WorldMapTileShapes.java:4 |
| 111 | -1 | valueInetAddressLookupRequest4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/InetAddressLookupRequest.java:4 |
| 112 | -1 | valueCameraEffectType1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraEffectType.java:7 |
| 113 | -2 | valueBaseTexture1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/BaseTexture.java:7 |
| 114 | -1 | valuePcmPlayer_Sub12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PcmPlayer_Sub1.java:8 |
| 115 | 2 | valueGpuModelUnknown1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuModelUnknown.java:8 |
| 116 | 0 | valueWaterRelated51 | builds/rs2/614-2010-07-13/src/src/main/java/deob/WaterRelated5.java:14 |

## C2S constructor sizes

| opcode | size | field | citation |
| --- | --- | --- | --- |
| 0 | 0 | valueMaterial6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Material.java:8 |
| 1 | 7 | valueGlPostProcessing4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlPostProcessing.java:7 |
| 2 | 2 | valueJs5ArchiveResourceLoader4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5ArchiveResourceLoader.java:7 |
| 3 | -1 | valueOggTheoraStreamRelated3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/OggTheoraStreamRelated.java:6 |
| 4 | -1 | valueBgaInit1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/BgaInit.java:9 |
| 5 | 4 | valueSpotAnimType150 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SpotAnimType.java:6 |
| 6 | -1 | valueLookatMode5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LookatMode.java:9 |
| 7 | 18 | valueClanChannelUser20 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ClanChannelUser.java:17 |
| 8 | -1 | valueEntity50 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Entity.java:4 |
| 9 | 15 | valueEntityBounds4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/EntityBounds.java:8 |
| 10 | 3 | valueQaaToString3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/QaaToString.java:6 |
| 11 | 7 | valueShaderRelated2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ShaderRelated.java:4 |
| 12 | 7 | valueCutsceneRoute28 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CutsceneRoute.java:7 |
| 13 | 8 | valueJs5FileResourceLoader1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5FileResourceLoader.java:7 |
| 14 | 4 | valueCameraTrackableType6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraTrackableType.java:7 |
| 15 | 8 | valuekind10323 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cf9df8543.java:5 |
| 16 | 16 | valueHbaFinalize4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/HbaFinalize.java:4 |
| 17 | 0 | valueColourUtils1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ColourUtils.java:8 |
| 18 | 7 | valueClientOptionsParticles4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ClientOptionsParticles.java:7 |
| 19 | 2 | valueFdaInit9 | builds/rs2/614-2010-07-13/src/src/main/java/deob/FdaInit.java:11 |
| 20 | 2 | valuekind10382 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cbc0ee57f.java:5 |
| 21 | -1 | valueGraphicsRelated7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GraphicsRelated.java:7 |
| 22 | 3 | valuekind10772 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cf951faab.java:6 |
| 23 | -1 | valueRawCpuInfo1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/RawCpuInfo.java:6 |
| 24 | 4 | valueMiniMenu1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/MiniMenu.java:4 |
| 25 | 8 | valueCacheKey4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CacheKey.java:10 |
| 26 | 5 | valuePathingEntity_Class147110 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PathingEntity_Class147.java:4 |
| 27 | 16 | valueClientOptionsLightingDetail10 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ClientOptionsLightingDetail.java:15 |
| 28 | 4 | valueGraphicsDeletable1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GraphicsDeletable.java:4 |
| 29 | 7 | valueLookatMode2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LookatMode.java:10 |
| 30 | 8 | valueStaticPointLight21 | builds/rs2/614-2010-07-13/src/src/main/java/deob/StaticPointLight.java:23 |
| 31 | -1 | valueSpriteShader2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SpriteShader.java:10 |
| 32 | -1 | valueAnimFrame129 | builds/rs2/614-2010-07-13/src/src/main/java/deob/AnimFrame.java:5 |
| 33 | -1 | valueLoadableResourceManager5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoadableResourceManager.java:6 |
| 34 | -1 | valueOcclusionSomething2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/OcclusionSomething.java:4 |
| 35 | 7 | valuekind10603 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C4a895028.java:6 |
| 36 | 6 | valueLoadingScreen1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoadingScreen.java:6 |
| 37 | -1 | valueCameraTrackableType3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraTrackableType.java:5 |
| 38 | 2 | valueJs5ArchiveResourceLoader5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5ArchiveResourceLoader.java:4 |
| 39 | 3 | valueUnused173 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused17.java:6 |
| 40 | 8 | valueQfaInit2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/QfaInit.java:5 |
| 41 | 4 | valuekind10955 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C7ef06aa1.java:7 |
| 42 | 8 | valueRayTracing5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/RayTracing.java:8 |
| 43 | -1 | valuekind11032 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C59796a6b.java:5 |
| 44 | 3 | valueQuickChatCatTypeList11 | builds/rs2/614-2010-07-13/src/src/main/java/deob/QuickChatCatTypeList.java:36 |
| 45 | 3 | valueRendererToolkit1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/RendererToolkit.java:4 |
| 46 | 3 | valuekind10394 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C234869d1.java:7 |
| 47 | -1 | valueMsiType154 | builds/rs2/614-2010-07-13/src/src/main/java/deob/MsiType.java:4 |
| 48 | 6 | valueJs5DllResourceLoader4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5DllResourceLoader.java:4 |
| 49 | 0 | valuePathingEntity_Class147112 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PathingEntity_Class147.java:7 |
| 50 | 3 | valueUnblendedTileData241 | builds/rs2/614-2010-07-13/src/src/main/java/deob/UnblendedTileData2.java:88 |
| 51 | 4 | valueBASType12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/BASType.java:11 |
| 52 | 7 | valueSecondaryLinkedList64 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SecondaryLinkedList.java:7 |
| 53 | -1 | valueParticleEmitterRelated2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitterRelated.java:6 |
| 54 | 12 | valueModelParticleEffector71 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ModelParticleEffector.java:15 |
| 55 | 8 | valueKbaFinalize2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/KbaFinalize.java:5 |
| 56 | 4 | valueSoundPcmStream125 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SoundPcmStream.java:4 |
| 57 | -1 | valueInterface1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Interface.java:5 |
| 58 | 1 | valueSimpleToolkitChange3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/SimpleToolkitChange.java:10 |
| 59 | 2 | valueBase642 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Base64.java:7 |
| 60 | 8 | valueCreateAccountReply9 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CreateAccountReply.java:13 |
| 61 | 2 | valueCameraTrackableType7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraTrackableType.java:8 |
| 62 | 3 | valueNPCTypeCustomisation1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/NPCTypeCustomisation.java:9 |
| 63 | 15 | valueOcclusionManager7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/OcclusionManager.java:10 |
| 64 | -1 | valuekind10993 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C0e4ebe77.java:7 |
| 65 | 3 | valueMapLoader12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/MapLoader.java:6 |
| 66 | 3 | valueGraphicsRelated10 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GraphicsRelated.java:11 |
| 67 | 3 | valuePureJavaModelRelated13 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PureJavaModelRelated1.java:4 |
| 68 | 11 | valueAudioChannel48 | builds/rs2/614-2010-07-13/src/src/main/java/deob/AudioChannel.java:6 |
| 69 | -1 | valueMouse21 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Mouse.java:4 |
| 70 | 1 | valueMapElement91 | builds/rs2/614-2010-07-13/src/src/main/java/deob/MapElement.java:5 |
| 71 | -1 | valueCgaFinalize12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CgaFinalize.java:18 |
| 72 | 11 | valuePingRequest2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PingRequest.java:9 |
| 73 | 6 | valueJcaInit10 | builds/rs2/614-2010-07-13/src/src/main/java/deob/JcaInit.java:5 |
| 74 | 7 | valueLoginManager25 | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoginManager.java:19 |
| 75 | 3 | valuekind10853 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C9e211cac.java:4 |
| 76 | 3 | valueNpcType221 | builds/rs2/614-2010-07-13/src/src/main/java/deob/NpcType.java:6 |
| 77 | -1 | valueGpuBuffer13 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuBuffer.java:16 |
| 78 | 7 | valueWfaInit1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/WfaInit.java:9 |
| 79 | 8 | valueWfaInit4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/WfaInit.java:10 |
| 80 | 7 | valueGraphicsPacketQueue13 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GraphicsPacketQueue.java:19 |
| 81 | -1 | valueKdaToString1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/KdaToString.java:4 |
| 82 | 8 | valuePingRequest3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/PingRequest.java:10 |
| 83 | 3 | valuekind10986 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C1b0f82ac.java:13 |

## Login opcode declarations

The second argument is not stored ({cite('deob/PathFinderEntitySize.java', 7)}).

| opcode | declared size | field | citation |
| --- | --- | --- | --- |
| 14 | 1 | valueCameraHelpers8 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:9 |
| 15 | 4 | valueCameraHelpers9 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:11 |
| 16 | -2 | valueCameraHelpers10 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:7 |
| 17 | 0 | valueCameraHelpers11 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:17 |
| 18 | -2 | valueCameraHelpers12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:5 |
| 19 | -2 | valueCameraHelpers13 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:14 |
| 20 | 6 | valueCameraHelpers14 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:10 |
| 21 | 9 | valueCameraHelpers15 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:18 |
| 22 | -2 | valueCameraHelpers16 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:19 |
| 23 | 4 | valueCameraHelpers17 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:20 |
| 24 | -1 | valueCameraHelpers18 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:21 |
| 25 | -2 | valueCameraHelpers19 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:22 |
| 26 | 0 | valueCameraHelpers20 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:23 |
| 27 | 0 | valueCameraHelpers21 | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:24 |
