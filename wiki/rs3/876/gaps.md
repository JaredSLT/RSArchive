# Gaps

An empty field list in [packets.md](packets.md) has a row here. `implementable` means the Java source contains the bytes and a later pass can read them. `blocked` means unread source, no cache, no gamepack, or a platform binding.

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| disk cache | builds/rs3/876-2016-11-28/meta.json | cache_downloaded is false and this archive has no cache; no-cache | blocked |
| c2s opcodes above 122 and s2c opcodes above 192 | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:256 | RSLib 876 tables fill missing opcodes with -3. This client's constructors stop at 122 and 192. Do not invent those sizes. | implementable |
| login hardware block | builds/rs3/876-2016-11-28/src/src/main/java/deob/BlankWs.java:432 | HardwarePlatform.createHardwareBlock layout was not read | implementable |
| login preferences blob | builds/rs3/876-2016-11-28/src/src/main/java/deob/BlankWs.java:428 | clsf87db028.doDataBlock1501 interior was not read. The login writer prefixes it with a u8 length. | implementable |
| login opcode 28 seed bytes | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:2600 | Statics.doStatics(DynamicGroundDecorEntity) which fills intArray9 was not read | implementable |
| login opcode 28 seven-byte skip | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:2617 | tint is advanced by 7 with no put call. The bytes are whatever the buffer already held. | implementable |
| jagex string codec | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/world/entity/PositionedSound.java:193 | jstr is NUL-terminated bytes from EffectAnimTypeFactory.doEffectAnimTypeFactory. That encoder was not read. | implementable |
| client script opcode bodies | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:638 | PingRequest.doPingRequest(DataBlock1253, DataBlock1625) switch was not read past the dispatch shape | implementable |
| loc obj seq and var config decoders | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/clientscript/emoji/Emoji.java:515 | Emoji.doEmoji was read through its end, including opcodes 106 and 118. Loc, object, sequence, and var decoders were not opened. | implementable |
| player and npc varp block | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/shared/console/DeveloperConsole.java:608 | doPublicKeysPart payload inside player mask bits 262144 and 131072, npc mask bits 65536 and 524288, and s2c opcode 140 was not read | implementable |
| equipment slot table | builds/rs3/876-2016-11-28/src/src/main/java/deob/SoundJingleCutsceneActionPart.java:99 | slot count is Statics.clsfc26e159.intArray.length. That array's constructor was not read. | implementable |
| interface ids in js5 | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:2145 | Client.tint87 is also loaded from Statics.playerSkill.tint6 and tint7. Those archive values need the missing cache. | blocked |
| software scene draw | builds/rs3/876-2016-11-28/src/src/main/java/deob/PureJava3.java:18 | PureJava3 is the software toolkit. Its draw methods were not read. | implementable |
| synth versus vorbis selection | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/audio/vorbis/VorbisRelated.java:40 | VorbisRelated.doVorbisRelated reads a header. Which packet feeds it, versus the other audio classes, was not traced. | implementable |
| jaggl/OpenGL.java | builds/rs3/876-2016-11-28/src/src/main/java/jaggl/OpenGL.java | platform-native platform-gl | blocked |
| jaggl/MapBuffer.java | builds/rs3/876-2016-11-28/src/src/main/java/jaggl/MapBuffer.java | platform-native platform-gl | blocked |
| jagdx/RuntimeExceptionPart4.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/RuntimeExceptionPart4.java | platform-native platform-dx | blocked |
| jagdx/Jagdx3.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/Jagdx3.java | platform-native platform-dx | blocked |
| jagdx/IUnknown.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IUnknown.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DVolumeTexture.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DVolumeTexture.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DVertexBuffer.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DVertexBuffer.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DTexture.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DTexture.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DSwapChain.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DSwapChain.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DSurface.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DSurface.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DIndexBuffer.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DIndexBuffer.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DEventQuery.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DEventQuery.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DDevice.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DDevice.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DCubeTexture.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DCubeTexture.java | platform-native platform-dx | blocked |
| jagdx/IDirect3DBaseTexture.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3DBaseTexture.java | platform-native platform-dx | blocked |
| jagdx/IDirect3D.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/IDirect3D.java | platform-native platform-dx | blocked |
| jagdx/DataBlock1773.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/DataBlock1773.java | platform-native platform-dx | blocked |
| jagdx/DataBlock1668.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/DataBlock1668.java | platform-native platform-dx | blocked |
| jagdx/DataBlock1585.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/DataBlock1585.java | platform-native platform-dx | blocked |
| jagdx/DataBlock1241.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/DataBlock1241.java | platform-native platform-dx | blocked |
| jagdx/DataBlock1238.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/DataBlock1238.java | platform-native platform-dx | blocked |
| jagdx/DataBlock1208.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/DataBlock1208.java | platform-native platform-dx | blocked |
| jagdx/D3DVSHADERCAPS2_0.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/D3DVSHADERCAPS2_0.java | platform-native platform-dx | blocked |
| jagdx/D3DSURFACE_DESC.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/D3DSURFACE_DESC.java | platform-native platform-dx | blocked |
| jagdx/D3DPSHADERCAPS2_0.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/D3DPSHADERCAPS2_0.java | platform-native platform-dx | blocked |
| jagdx/D3DPRESENT_PARAMETERS.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/D3DPRESENT_PARAMETERS.java | platform-native platform-dx | blocked |
| jagdx/D3DLIGHT.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/D3DLIGHT.java | platform-native platform-dx | blocked |
| jagdx/D3DDISPLAYMODE.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/D3DDISPLAYMODE.java | platform-native platform-dx | blocked |
| jagdx/D3DCAPS.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/D3DCAPS.java | platform-native platform-dx | blocked |
| jagdx/D3DADAPTER_IDENTIFIER.java | builds/rs3/876-2016-11-28/src/src/main/java/jagdx/D3DADAPTER_IDENTIFIER.java | platform-native platform-dx | blocked |
| c2s opcode 1 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/graphics/FloorModelNode.java:18 | write site located and payload not read | implementable |
| c2s opcode 2 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:7528 | write site located and payload not read | implementable |
| c2s opcode 3 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:10869 | write site located and payload not read | implementable |
| c2s opcode 4 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:9043 | write site located and payload not read | implementable |
| c2s opcode 5 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10659 | write site located and payload not read | implementable |
| c2s opcode 6 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:12914 | write site located and payload not read | implementable |
| c2s opcode 8 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:3224 | write site located and payload not read | implementable |
| c2s opcode 9 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10790 | write site located and payload not read | implementable |
| c2s opcode 10 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1200.java:168 | write site located and payload not read | implementable |
| c2s opcode 12 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:8917 | write site located and payload not read | implementable |
| c2s opcode 13 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1735.java:28 | write site located and payload not read | implementable |
| c2s opcode 14 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:9001 | write site located and payload not read | implementable |
| c2s opcode 15 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:11004 | write site located and payload not read | implementable |
| c2s opcode 16 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:8969 | write site located and payload not read | implementable |
| c2s opcode 18 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:9007 | write site located and payload not read | implementable |
| c2s opcode 19 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1678.java:103 | write site located and payload not read | implementable |
| c2s opcode 20 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:3408 | write site located and payload not read | implementable |
| c2s opcode 21 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:9031 | write site located and payload not read | implementable |
| c2s opcode 22 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:8530 | write site located and payload not read | implementable |
| c2s opcode 23 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:9037 | write site located and payload not read | implementable |
| c2s opcode 25 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1200.java:162 | write site located and payload not read | implementable |
| c2s opcode 27 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Unused23Part.java:32 | write site located and payload not read | implementable |
| c2s opcode 30 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:608 | write site located and payload not read | implementable |
| c2s opcode 31 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:9013 | write site located and payload not read | implementable |
| c2s opcode 33 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:11106 | write site located and payload not read | implementable |
| c2s opcode 34 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:8585 | write site located and payload not read | implementable |
| c2s opcode 35 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/io/SocketStreamWriter.java:21 | write site located and payload not read | implementable |
| c2s opcode 37 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:3398 | write site located and payload not read | implementable |
| c2s opcode 42 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:8649 | write site located and payload not read | implementable |
| c2s opcode 45 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:9019 | write site located and payload not read | implementable |
| c2s opcode 46 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1672.java:223 | write site located and payload not read | implementable |
| c2s opcode 48 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:102 | size constructor only; no write site found | implementable |
| c2s opcode 50 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:8989 | write site located and payload not read | implementable |
| c2s opcode 51 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1200.java:180 | write site located and payload not read | implementable |
| c2s opcode 53 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Unused23Part.java:48 | write site located and payload not read | implementable |
| c2s opcode 55 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10645 | write site located and payload not read | implementable |
| c2s opcode 56 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:8404 | write site located and payload not read | implementable |
| c2s opcode 57 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/client/RuneScapeSetup.java:84 | write site located and payload not read | implementable |
| c2s opcode 58 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:10958 | write site located and payload not read | implementable |
| c2s opcode 60 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:126 | size constructor only; no write site found | implementable |
| c2s opcode 62 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:9025 | write site located and payload not read | implementable |
| c2s opcode 66 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10700 | write site located and payload not read | implementable |
| c2s opcode 69 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:144 | size constructor only; no write site found | implementable |
| c2s opcode 71 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:11092 | write site located and payload not read | implementable |
| c2s opcode 72 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:8995 | write site located and payload not read | implementable |
| c2s opcode 74 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:2012 | write site located and payload not read | implementable |
| c2s opcode 75 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10876 | write site located and payload not read | implementable |
| c2s opcode 76 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/scene/World.java:1352 | write site located and payload not read | implementable |
| c2s opcode 77 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1237.java:18 | write site located and payload not read | implementable |
| c2s opcode 79 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1200.java:154 | write site located and payload not read | implementable |
| c2s opcode 80 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10847 | write site located and payload not read | implementable |
| c2s opcode 81 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:8536 | write site located and payload not read | implementable |
| c2s opcode 82 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1334.java:140 | write site located and payload not read | implementable |
| c2s opcode 83 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/client/RuneScapeSetup.java:52 | write site located and payload not read | implementable |
| c2s opcode 84 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:10845 | write site located and payload not read | implementable |
| c2s opcode 86 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1200.java:112 | write site located and payload not read | implementable |
| c2s opcode 87 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:7652 | write site located and payload not read | implementable |
| c2s opcode 89 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:14896 | write site located and payload not read | implementable |
| c2s opcode 91 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:8641 | write site located and payload not read | implementable |
| c2s opcode 92 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10685 | write site located and payload not read | implementable |
| c2s opcode 93 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1334.java:553 | write site located and payload not read | implementable |
| c2s opcode 95 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10717 | write site located and payload not read | implementable |
| c2s opcode 96 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10906 | write site located and payload not read | implementable |
| c2s opcode 97 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10674 | write site located and payload not read | implementable |
| c2s opcode 98 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:9369 | write site located and payload not read | implementable |
| c2s opcode 99 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5056 | write site located and payload not read | implementable |
| c2s opcode 102 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:8941 | write site located and payload not read | implementable |
| c2s opcode 103 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10635 | write site located and payload not read | implementable |
| c2s opcode 104 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:9379 | write site located and payload not read | implementable |
| c2s opcode 105 reflection bytes | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:3217 | AbstractSocket.doAbstractSocket payload was not read | implementable |
| c2s opcode 106 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:8561 | write site located and payload not read | implementable |
| c2s opcode 108 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:13706 | write site located and payload not read | implementable |
| c2s opcode 109 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/Unused1.java:790 | write site located and payload not read | implementable |
| c2s opcode 110 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/EnumTypePart2.java:15 | write site located and payload not read | implementable |
| c2s opcode 111 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:8553 | write site located and payload not read | implementable |
| c2s opcode 112 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:14754 | write site located and payload not read | implementable |
| c2s opcode 113 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:8909 | write site located and payload not read | implementable |
| c2s opcode 114 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:8544 | write site located and payload not read | implementable |
| c2s opcode 115 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/client/RuneScapeSetup.java:68 | write site located and payload not read | implementable |
| c2s opcode 116 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/client/NativeMouse.java:62 | write site located and payload not read | implementable |
| c2s opcode 117 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/client/RuneScapeSetup.java:94 | write site located and payload not read | implementable |
| c2s opcode 118 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/core/utils/PingRequest.java:9394 | write site located and payload not read | implementable |
| c2s opcode 119 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:3139 | write site located and payload not read | implementable |
| c2s opcode 120 field list | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:10733 | write site located and payload not read | implementable |
| c2s opcode 121 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:248 | size constructor only; no write site found | implementable |
| c2s opcode 122 field list | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/scene/World.java:1320 | write site located and payload not read | implementable |
| s2c opcode 0 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6853 | switch arm located and payload not read | implementable |
| s2c opcode 1 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4926 | switch arm located and payload not read | implementable |
| s2c opcode 2 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4513 | switch arm located and payload not read | implementable |
| s2c opcode 3 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4966 | switch arm located and payload not read | implementable |
| s2c opcode 4 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6584 | switch arm located and payload not read | implementable |
| s2c opcode 5 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6574 | switch arm located and payload not read | implementable |
| s2c opcode 6 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4974 | switch arm located and payload not read | implementable |
| s2c opcode 7 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5029 | switch arm located and payload not read | implementable |
| s2c opcode 8 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6879 | switch arm located and payload not read | implementable |
| s2c opcode 9 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5810 | switch arm located and payload not read | implementable |
| s2c opcode 10 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6221 | switch arm located and payload not read | implementable |
| s2c opcode 12 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6237 | switch arm located and payload not read | implementable |
| s2c opcode 13 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5113 | switch arm located and payload not read | implementable |
| s2c opcode 14 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4608 | switch arm located and payload not read | implementable |
| s2c opcode 15 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4534 | switch arm located and payload not read | implementable |
| s2c opcode 16 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5532 | switch arm located and payload not read | implementable |
| s2c opcode 17 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5751 | switch arm located and payload not read | implementable |
| s2c opcode 18 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6353 | switch arm located and payload not read | implementable |
| s2c opcode 19 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6830 | switch arm located and payload not read | implementable |
| s2c opcode 20 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6551 | switch arm located and payload not read | implementable |
| s2c opcode 21 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6767 | switch arm located and payload not read | implementable |
| s2c opcode 22 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6397 | switch arm located and payload not read | implementable |
| s2c opcode 23 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5654 | switch arm located and payload not read | implementable |
| s2c opcode 24 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4941 | switch arm located and payload not read | implementable |
| s2c opcode 26 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4555 | switch arm located and payload not read | implementable |
| s2c opcode 27 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6015 | switch arm located and payload not read | implementable |
| s2c opcode 28 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6119 | switch arm located and payload not read | implementable |
| s2c opcode 30 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4989 | switch arm located and payload not read | implementable |
| s2c opcode 32 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4843 | switch arm located and payload not read | implementable |
| s2c opcode 33 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5098 | switch arm located and payload not read | implementable |
| s2c opcode 34 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5133 | switch arm located and payload not read | implementable |
| s2c opcode 35 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6612 | switch arm located and payload not read | implementable |
| s2c opcode 36 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6330 | switch arm located and payload not read | implementable |
| s2c opcode 37 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6455 | switch arm located and payload not read | implementable |
| s2c opcode 40 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5644 | switch arm located and payload not read | implementable |
| s2c opcode 41 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5076 | switch arm located and payload not read | implementable |
| s2c opcode 42 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6835 | switch arm located and payload not read | implementable |
| s2c opcode 43 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5952 | switch arm located and payload not read | implementable |
| s2c opcode 44 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4879 | switch arm located and payload not read | implementable |
| s2c opcode 46 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5063 | switch arm located and payload not read | implementable |
| s2c opcode 47 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6319 | switch arm located and payload not read | implementable |
| s2c opcode 48 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6251 | switch arm located and payload not read | implementable |
| s2c opcode 49 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4510 | switch arm located and payload not read | implementable |
| s2c opcode 50 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5019 | switch arm located and payload not read | implementable |
| s2c opcode 51 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5039 | switch arm located and payload not read | implementable |
| s2c opcode 52 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5465 | switch arm located and payload not read | implementable |
| s2c opcode 53 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6029 | switch arm located and payload not read | implementable |
| s2c opcode 55 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4978 | switch arm located and payload not read | implementable |
| s2c opcode 56 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5008 | switch arm located and payload not read | implementable |
| s2c opcode 57 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6044 | switch arm located and payload not read | implementable |
| s2c opcode 58 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5348 | switch arm located and payload not read | implementable |
| s2c opcode 60 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6227 | switch arm located and payload not read | implementable |
| s2c opcode 61 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4224 | switch arm located and payload not read | implementable |
| s2c opcode 62 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6805 | switch arm located and payload not read | implementable |
| s2c opcode 63 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4204 | switch arm located and payload not read | implementable |
| s2c opcode 64 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6405 | switch arm located and payload not read | implementable |
| s2c opcode 65 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4663 | switch arm located and payload not read | implementable |
| s2c opcode 67 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4313 | switch arm located and payload not read | implementable |
| s2c opcode 68 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5171 | switch arm located and payload not read | implementable |
| s2c opcode 69 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5394 | switch arm located and payload not read | implementable |
| s2c opcode 72 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5536 | switch arm located and payload not read | implementable |
| s2c opcode 74 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4220 | switch arm located and payload not read | implementable |
| s2c opcode 76 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6375 | switch arm located and payload not read | implementable |
| s2c opcode 77 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5572 | switch arm located and payload not read | implementable |
| s2c opcode 78 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6727 | switch arm located and payload not read | implementable |
| s2c opcode 79 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4477 | switch arm located and payload not read | implementable |
| s2c opcode 80 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6529 | switch arm located and payload not read | implementable |
| s2c opcode 81 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4838 | switch arm located and payload not read | implementable |
| s2c opcode 82 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4674 | switch arm located and payload not read | implementable |
| s2c opcode 83 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5165 | switch arm located and payload not read | implementable |
| s2c opcode 84 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4945 | switch arm located and payload not read | implementable |
| s2c opcode 85 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5140 | switch arm located and payload not read | implementable |
| s2c opcode 86 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5844 | switch arm located and payload not read | implementable |
| s2c opcode 87 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4548 | switch arm located and payload not read | implementable |
| s2c opcode 88 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4413 | switch arm located and payload not read | implementable |
| s2c opcode 89 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5108 | switch arm located and payload not read | implementable |
| s2c opcode 90 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4355 | switch arm located and payload not read | implementable |
| s2c opcode 91 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6139 | switch arm located and payload not read | implementable |
| s2c opcode 92 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6592 | switch arm located and payload not read | implementable |
| s2c opcode 93 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4269 | switch arm located and payload not read | implementable |
| s2c opcode 94 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4190 | switch arm located and payload not read | implementable |
| s2c opcode 95 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6358 | switch arm located and payload not read | implementable |
| s2c opcode 96 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5840 | switch arm located and payload not read | implementable |
| s2c opcode 97 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4669 | switch arm located and payload not read | implementable |
| s2c opcode 98 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5451 | switch arm located and payload not read | implementable |
| s2c opcode 99 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4483 | switch arm located and payload not read | implementable |
| s2c opcode 100 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6868 | switch arm located and payload not read | implementable |
| s2c opcode 101 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6671 | switch arm located and payload not read | implementable |
| s2c opcode 102 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5445 | switch arm located and payload not read | implementable |
| s2c opcode 103 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5015 | switch arm located and payload not read | implementable |
| s2c opcode 104 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6544 | switch arm located and payload not read | implementable |
| s2c opcode 105 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5797 | switch arm located and payload not read | implementable |
| s2c opcode 106 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6311 | switch arm located and payload not read | implementable |
| s2c opcode 108 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5521 | switch arm located and payload not read | implementable |
| s2c opcode 109 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5083 | switch arm located and payload not read | implementable |
| s2c opcode 110 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4649 | switch arm located and payload not read | implementable |
| s2c opcode 111 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6563 | switch arm located and payload not read | implementable |
| s2c opcode 112 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6517 | switch arm located and payload not read | implementable |
| s2c opcode 113 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5193 | switch arm located and payload not read | implementable |
| s2c opcode 114 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4230 | switch arm located and payload not read | implementable |
| s2c opcode 115 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6347 | switch arm located and payload not read | implementable |
| s2c opcode 116 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6792 | switch arm located and payload not read | implementable |
| s2c opcode 117 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5649 | switch arm located and payload not read | implementable |
| s2c opcode 118 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6826 | switch arm located and payload not read | implementable |
| s2c opcode 119 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6781 | switch arm located and payload not read | implementable |
| s2c opcode 120 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5126 | switch arm located and payload not read | implementable |
| s2c opcode 122 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5189 | switch arm located and payload not read | implementable |
| s2c opcode 123 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5053 | switch arm located and payload not read | implementable |
| s2c opcode 124 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4278 | switch arm located and payload not read | implementable |
| s2c opcode 125 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5983 | switch arm located and payload not read | implementable |
| s2c opcode 126 string tail | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:7010 | DataBlock1562.doDataBlock1791 and DataBlock1537.doDataBlock1525 were not read | implementable |
| s2c opcode 128 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4449 | switch arm located and payload not read | implementable |
| s2c opcode 129 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6058 | switch arm located and payload not read | implementable |
| s2c opcode 130 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4155 | switch arm located and payload not read | implementable |
| s2c opcode 131 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6599 | switch arm located and payload not read | implementable |
| s2c opcode 132 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6208 | switch arm located and payload not read | implementable |
| s2c opcode 133 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4407 | switch arm located and payload not read | implementable |
| s2c opcode 134 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5833 | switch arm located and payload not read | implementable |
| s2c opcode 135 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5584 | switch arm located and payload not read | implementable |
| s2c opcode 136 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6126 | switch arm located and payload not read | implementable |
| s2c opcode 137 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4563 | switch arm located and payload not read | implementable |
| s2c opcode 138 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6903 | switch arm located and payload not read | implementable |
| s2c opcode 139 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4933 | switch arm located and payload not read | implementable |
| s2c opcode 141 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6266 | switch arm located and payload not read | implementable |
| s2c opcode 142 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5623 | switch arm located and payload not read | implementable |
| s2c opcode 143 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6891 | switch arm located and payload not read | implementable |
| s2c opcode 144 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6682 | switch arm located and payload not read | implementable |
| s2c opcode 145 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6470 | switch arm located and payload not read | implementable |
| s2c opcode 146 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6899 | switch arm located and payload not read | implementable |
| s2c opcode 147 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4346 | switch arm located and payload not read | implementable |
| s2c opcode 148 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4864 | switch arm located and payload not read | implementable |
| s2c opcode 149 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5514 | switch arm located and payload not read | implementable |
| s2c opcode 150 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4559 | switch arm located and payload not read | implementable |
| s2c opcode 151 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4527 | switch arm located and payload not read | implementable |
| s2c opcode 153 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6521 | switch arm located and payload not read | implementable |
| s2c opcode 155 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4653 | switch arm located and payload not read | implementable |
| s2c opcode 156 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5629 | switch arm located and payload not read | implementable |
| s2c opcode 157 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5566 | switch arm located and payload not read | implementable |
| s2c opcode 158 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6809 | switch arm located and payload not read | implementable |
| s2c opcode 159 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5860 | switch arm located and payload not read | implementable |
| s2c opcode 160 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5102 | switch arm located and payload not read | implementable |
| s2c opcode 162 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6387 | switch arm located and payload not read | implementable |
| s2c opcode 163 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6675 | switch arm located and payload not read | implementable |
| s2c opcode 164 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5819 | switch arm located and payload not read | implementable |
| s2c opcode 165 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4455 | switch arm located and payload not read | implementable |
| s2c opcode 166 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5528 | switch arm located and payload not read | implementable |
| s2c opcode 167 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4846 | switch arm located and payload not read | implementable |
| s2c opcode 168 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5735 | switch arm located and payload not read | implementable |
| s2c opcode 169 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4522 | switch arm located and payload not read | implementable |
| s2c opcode 170 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6872 | switch arm located and payload not read | implementable |
| s2c opcode 171 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4541 | switch arm located and payload not read | implementable |
| s2c opcode 172 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6839 | switch arm located and payload not read | implementable |
| s2c opcode 174 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4237 | switch arm located and payload not read | implementable |
| s2c opcode 175 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6195 | switch arm located and payload not read | implementable |
| s2c opcode 176 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5807 | switch arm located and payload not read | implementable |
| s2c opcode 177 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4459 | switch arm located and payload not read | implementable |
| s2c opcode 178 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5147 | switch arm located and payload not read | implementable |
| s2c opcode 179 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4571 | switch arm located and payload not read | implementable |
| s2c opcode 180 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4505 | switch arm located and payload not read | implementable |
| s2c opcode 181 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4265 | switch arm located and payload not read | implementable |
| s2c opcode 182 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6051 | switch arm located and payload not read | implementable |
| s2c opcode 183 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6368 | switch arm located and payload not read | implementable |
| s2c opcode 184 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5619 | switch arm located and payload not read | implementable |
| s2c opcode 185 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4469 | switch arm located and payload not read | implementable |
| s2c opcode 186 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5725 | switch arm located and payload not read | implementable |
| s2c opcode 187 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4645 | switch arm located and payload not read | implementable |
| s2c opcode 188 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5175 | switch arm located and payload not read | implementable |
| s2c opcode 189 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5367 | switch arm located and payload not read | implementable |
| s2c opcode 190 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6173 | switch arm located and payload not read | implementable |
| s2c opcode 191 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6204 | switch arm located and payload not read | implementable |
| s2c opcode 192 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6772 | switch arm located and payload not read | implementable |
| login opcode 15 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:8 | constructor only; no writer read | implementable |
| login opcode 23 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:14 | constructor only; no writer read | implementable |
| login opcode 24 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:16 | constructor only; no writer read | implementable |
| login opcode 27 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:20 | constructor only; no writer read | implementable |
| login opcode 31 field list | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:28 | constructor only; no writer read | implementable |
