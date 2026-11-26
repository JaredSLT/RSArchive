# Packets

Game c2s opcodes are `DataBlock420` (`deob/DataBlock420.java`). Game s2c opcodes are `DataBlock457` (`deob/DataBlock457.java`). Login opcodes are `DataBlock351` on the login socket, not the game opcode table. A repeated opcode number is a different stream.

Codec names are defined in [framing.md](framing.md). An empty field list means the handler body was not read. Those rows are in [gaps.md](gaps.md).

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | 4 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:9199 |
| c2s | 1 | 2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:9313 |
| c2s | 2 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:10038 |
| c2s | 3 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:13984 |
| c2s | 4 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11717 |
| c2s | 5 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 6 | 6 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11696 |
| c2s | 7 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 8 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11729 |
| c2s | 9 | 2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:9257 |
| c2s | 10 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/cursortype/CursorTypeFactory.java:186 |
| c2s | 11 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/graphics/DefaultSprites.java:72 |
| c2s | 12 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:13955 |
| c2s | 13 | 4 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:3434 |
| c2s | 14 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/graphics/DefaultSprites.java:98 |
| c2s | 15 | 7 | u16le baseX+x, u16le baseZ+z, u8 ((menuFlag?2:0) or (run-binding?1:0)), u16be id | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2364 |
| c2s | 16 | 9 | u16le baseZ+z, u16le_low+128 baseX+x, u32_alt13 id, u8(0-run) | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2214 |
| c2s | 17 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11735 |
| c2s | 18 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11741 |
| c2s | 19 | 9 | u16le baseZ+z, u16le_low+128 baseX+x, u32_alt13 id, u8(0-run) | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2214 |
| c2s | 20 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:14090 |
| c2s | 21 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:11925 |
| c2s | 22 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 23 | 7 | u16le baseX+x, u16le baseZ+z, u8 ((menuFlag?2:0) or (run-binding?1:0)), u16be id | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2364 |
| c2s | 24 | 7 | u16le baseX+x, u16le baseZ+z, u8 ((menuFlag?2:0) or (run-binding?1:0)), u16be id | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2364 |
| c2s | 25 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/graphics/DefaultSprites.java:56 |
| c2s | 26 | 11 | u32_alt14 Statics.tint4, u16le_low+128 Client.tint85, u8 run-binding, u16le target index, u16le_low+128 Client.tint84 | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2116 |
| c2s | 27 | 7 | u16le baseX+x, u16le baseZ+z, u8 ((menuFlag?2:0) or (run-binding?1:0)), u16be id | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2364 |
| c2s | 28 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/cursortype/CursorTypeFactory.java:198 |
| c2s | 29 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 30 | 4 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/audio/api/BussType.java:168 |
| c2s | 31 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:11778 |
| c2s | 32 | 9 | u16le baseZ+z, u16le_low+128 baseX+x, u32_alt13 id, u8(0-run) | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2214 |
| c2s | 33 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:10063 |
| c2s | 34 | 3 | u16be npc index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2150 |
| c2s | 35 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:620 |
| c2s | 36 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:11796 |
| c2s | 37 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:14033 |
| c2s | 38 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:10048 |
| c2s | 39 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:11939 |
| c2s | 40 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 41 | 5 | u8+128 run-binding, u16le baseX+x, u16be_low+128 baseZ+z | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2427 |
| c2s | 42 | 4 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/client/RebuildStage.java:1314 |
| c2s | 43 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/client/AppletMouseLogger.java:53 |
| c2s | 44 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:9321 |
| c2s | 45 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 46 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11771 |
| c2s | 47 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:14119 |
| c2s | 48 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/CutsceneLocationPart.java:227 |
| c2s | 49 | 6 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/script/SubInterface.java:42 |
| c2s | 50 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock420.java:106 |
| c2s | 51 | 9 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7669 |
| c2s | 52 | 12 | u16le baseX+x, u16le_low+128 baseZ+z, u16be_low+128 Client.tint84, u16be Client.tint85, u32be Statics.tint4 | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2236 |
| c2s | 53 | 3 | u16be npc index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2150 |
| c2s | 54 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11753 |
| c2s | 55 | 0 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/graphics/DefaultSprites.java:88 |
| c2s | 56 | 15 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/NativeLibraryFailureTypePart.java:15 |
| c2s | 57 | 17 | u16le Client.tint84, u32_alt13 (id >>> 32), u32be Statics.tint4, u16le_low+128 baseX+x, u16be Client.tint85, u8+128 run-binding, u16be baseZ+z | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2163 |
| c2s | 58 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 59 | 0 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:3175 |
| c2s | 60 | 11 | u16le_low+128 npc index, u32be Statics.tint4, u16be_low+128 Client.tint85, u16be_low+128 Client.tint84, u8(128-run) | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2184 |
| c2s | 61 | 4 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/client/RebuildStage.java:1346 |
| c2s | 62 | 0 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:13768 |
| c2s | 63 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/cursortype/CursorTypeFactory.java:130 |
| c2s | 64 | 9 | u16le baseZ+z, u16le_low+128 baseX+x, u32_alt13 id, u8(0-run) | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2214 |
| c2s | 65 | 0 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:9224 |
| c2s | 66 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11747 |
| c2s | 67 | 4 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/cursortype/CursorTypeFactory.java:172 |
| c2s | 68 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock420.java:142 |
| c2s | 69 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/Statics.java:5317 |
| c2s | 70 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:13995 |
| c2s | 71 | 6 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:2564 |
| c2s | 72 | 9 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock420.java:150 |
| c2s | 73 | 4 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/audio/api/BussType.java:581 |
| c2s | 74 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:13945 |
| c2s | 75 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/script/SubInterface.java:62 |
| c2s | 76 | 15 | u16le baseZ+z, u16be_low+128 baseX+x, u8(128-run), u32_alt14 Statics.tint4, u16be_low+128 id, u16le Client.tint84, u16be Client.tint85 | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2294 |
| c2s | 77 | 7 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/client/AppletMouseLogger.java:37 |
| c2s | 78 | 18 | u8+128 run-binding, u16le baseX+x, u16be_low+128 baseZ+z, then when kind==1: u8 -1, u8 -1, u16be (int)Client.tfloat2, u8 57, u8 Client.tint44, u8 Client.tint46, u8 89, u16be player fine x, u16be player fine z, u8 63 | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2427 |
| c2s | 79 | -1 | u8 0, bytes from ConfigTypeListPreload.doConfigTypeListPreload (body unread), u8 backpatch of that length | Client.doClient2222b242 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:3249 |
| c2s | 80 | 3 | u16be npc index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2150 |
| c2s | 81 | 4 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:14525 |
| c2s | 82 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/Statics.java:5374 |
| c2s | 83 | 3 | u16be npc index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2150 |
| c2s | 84 | 7 | u16le baseX+x, u16le baseZ+z, u8 ((menuFlag?2:0) or (run-binding?1:0)), u16be id | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2364 |
| c2s | 85 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:14149 |
| c2s | 86 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11765 |
| c2s | 87 | 3 | u16be npc index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2150 |
| c2s | 88 | 1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:11654 |
| c2s | 89 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:11558 |
| c2s | 90 | 1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/cursortype/CursorTypeFactory.java:180 |
| c2s | 91 | 7 | u16le baseX+x, u16le baseZ+z, u8 ((menuFlag?2:0) or (run-binding?1:0)), u16be id | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2364 |
| c2s | 92 | 1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/cutscene/CutsceneRoute.java:212 |
| c2s | 93 | 1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock420.java:192 |
| c2s | 94 | 3 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:11534 |
| c2s | 95 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:9207 |
| c2s | 96 | 0 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:13706 |
| c2s | 97 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:14017 |
| c2s | 98 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:15693 |
| c2s | 99 | 18 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/BasicTypeFactoryPart.java:15 |
| c2s | 100 | 1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock209.java:19 |
| c2s | 101 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/client/ClientDirectXFailure.java:32 |
| c2s | 102 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 103 | 9 | u16le baseZ+z, u16le_low+128 baseX+x, u32_alt13 id, u8(0-run) | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2214 |
| c2s | 104 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/world/entity/SceneManager.java:688 |
| c2s | 105 | 2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/graphics/GraphicsPacketQueue.java:21 |
| c2s | 106 | 4 | u8+128 gc percent or -1, u16le Statics.questCommands.tint, u8 Client.tint | Client.doClient2222b242 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:3290 |
| c2s | 107 | 4 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:14239 |
| c2s | 108 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:13969 |
| c2s | 109 | 0 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:14434 |
| c2s | 110 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 111 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:9216 |
| c2s | 112 | 3 | u16be_low+128 player index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2336 |
| c2s | 113 | 3 | u16be npc index, u8+128 run-binding | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2150 |
| c2s | 114 | 16 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/Statics.java:4944 |
| c2s | 115 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11723 |
| c2s | 116 | 8 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11759 |
| c2s | 117 | 1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/Statics.java:2527 |
| c2s | 118 | 4 | u16be_low+128 baseX+x, u16be_low+128 baseZ+z | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2396 |
| c2s | 119 | 5 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/game/config/vartype/VarClanSettingsDomain.java:15856 |
| c2s | 120 | 9 | u16le baseZ+z, u16le_low+128 baseX+x, u32_alt13 id, u8(0-run) | AccountAppealLogin.doAccountAppealLogin | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/login/AccountAppealLogin.java:2214 |
| c2s | 121 | -1 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/Statics.java:4977 |
| c2s | 122 | 16 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:11704 |
| c2s | 14 | 0 | login stream: u8 opcode only | SecureRandomTask.blankWs | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/core/utils/SecureRandomTask.java:320 |
| c2s | 15 | -1 | login stream:  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock351.java:8 |
| c2s | 16 | -2 | login stream: game login opcode 16; see login.md | SecureRandomTask.blankWs | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/core/utils/SecureRandomTask.java:441 |
| c2s | 19 | -2 | login stream: lobby login opcode 19; see login.md | SecureRandomTask.blankWs | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/core/utils/SecureRandomTask.java:504 |
| c2s | 23 | 4 | login stream:  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock351.java:14 |
| c2s | 24 | -1 | login stream:  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock351.java:16 |
| c2s | 26 | 0 | login stream: u8 opcode 26 only | SecureRandomTask.blankWs | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/core/utils/SecureRandomTask.java:793 |
| c2s | 27 | 0 | login stream:  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock351.java:20 |
| c2s | 28 | -2 | login stream: early lobby opcode 28; see login.md | DefaultSprites.doDefaultSprites2222b | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/graphics/DefaultSprites.java:187 |
| c2s | 29 | -2 | login stream: u8 opcode 29, u16be length placeholder, u32be 902, u32be 1, game-only u8 (state==4), RSA block from doSecureRandomTask2222b4 plus the bytes appended before RSA, u16be length backpatch | SecureRandomTask.blankWs | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/core/utils/SecureRandomTask.java:352 |
| c2s | 30 | -2 | login stream: u8 opcode 30, then the same cleartext tail as opcode 16 or 19 without the revision and RSA prefix; see login.md | SecureRandomTask.blankWs | builds/rs3/902-2018-12-14/src/src/main/java/com/jagex/core/utils/SecureRandomTask.java:441 |
| c2s | 31 | 4 | login stream:  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock351.java:28 |
| s2c | 0 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8519 |
| s2c | 1 | -2 | bit stream of high-res then low-res players, then masks; see sync.md | UnblendedTileData2.doUnblendedTileData2 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8341 |
| s2c | 2 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9066 |
| s2c | 3 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7406 |
| s2c | 4 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8065 |
| s2c | 5 | 23 | u32_alt13, u8(0-b), u16le_low+128, u32_alt13, u32_alt13, u32_alt14, u32_alt14 | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8313 |
| s2c | 6 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock457.java:18 |
| s2c | 7 | -2 | bit stream of existing NPCs, new NPCs, then masks; coord width is RebuildStage.tint13; see sync.md | Client.doClient2222 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:10189 |
| s2c | 8 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7710 |
| s2c | 9 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8652 |
| s2c | 10 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7615 |
| s2c | 11 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8603 |
| s2c | 12 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9421 |
| s2c | 13 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:10095 |
| s2c | 14 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9437 |
| s2c | 15 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7904 |
| s2c | 16 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9868 |
| s2c | 17 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9573 |
| s2c | 18 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7907 |
| s2c | 19 | 18 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7798 |
| s2c | 20 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8135 |
| s2c | 21 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8055 |
| s2c | 22 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7414 |
| s2c | 23 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8608 |
| s2c | 24 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9442 |
| s2c | 25 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7608 |
| s2c | 26 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7570 |
| s2c | 27 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7716 |
| s2c | 28 | -2 | jstr signature; for each char from the end, jstr if the char is s else u32be; then u32be | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9215 |
| s2c | 29 | -2 |  | unread | builds/rs3/902-2018-12-14/src/src/main/java/deob/DataBlock457.java:64 |
| s2c | 30 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8972 |
| s2c | 31 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8401 |
| s2c | 32 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8977 |
| s2c | 33 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8567 |
| s2c | 34 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8739 |
| s2c | 35 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8439 |
| s2c | 36 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8392 |
| s2c | 37 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7632 |
| s2c | 38 | 12 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8749 |
| s2c | 39 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7912 |
| s2c | 40 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7969 |
| s2c | 41 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9947 |
| s2c | 42 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7753 |
| s2c | 43 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:10158 |
| s2c | 44 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:10148 |
| s2c | 45 | 9 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9641 |
| s2c | 46 | 1 | u8(128-b) == 1 | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8307 |
| s2c | 47 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8411 |
| s2c | 48 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9147 |
| s2c | 49 | -2 | dynamic rebuild; see region.md | RebuildStage.doRebuildStage2 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7993 |
| s2c | 50 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7973 |
| s2c | 51 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9786 |
| s2c | 52 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9940 |
| s2c | 53 | 21 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7949 |
| s2c | 54 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8206 |
| s2c | 55 | 19 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9657 |
| s2c | 56 | 9 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:10171 |
| s2c | 57 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7398 |
| s2c | 58 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7802 |
| s2c | 59 | 7 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7945 |
| s2c | 60 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8514 |
| s2c | 61 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8005 |
| s2c | 62 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9968 |
| s2c | 63 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7806 |
| s2c | 64 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8215 |
| s2c | 65 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7494 |
| s2c | 66 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8121 |
| s2c | 67 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9772 |
| s2c | 68 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9827 |
| s2c | 69 | 25 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9679 |
| s2c | 70 | 29 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9601 |
| s2c | 71 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9879 |
| s2c | 72 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8125 |
| s2c | 73 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9700 |
| s2c | 74 | 5 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9594 |
| s2c | 75 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9650 |
| s2c | 76 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7654 |
| s2c | 77 | 10 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8157 |
| s2c | 78 | 10 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8675 |
| s2c | 79 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9431 |
| s2c | 80 | 12 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9251 |
| s2c | 81 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8219 |
| s2c | 82 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8428 |
| s2c | 83 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9705 |
| s2c | 84 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7988 |
| s2c | 85 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9834 |
| s2c | 86 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9415 |
| s2c | 87 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8172 |
| s2c | 88 | 10 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7881 |
| s2c | 89 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9937 |
| s2c | 90 | 28 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9692 |
| s2c | 91 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7392 |
| s2c | 92 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7810 |
| s2c | 93 | -2 | static rebuild; see region.md | RebuildStage.doRebuildStage | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8727 |
| s2c | 94 | 5 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9779 |
| s2c | 95 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7666 |
| s2c | 96 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8588 |
| s2c | 97 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9637 |
| s2c | 98 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7357 |
| s2c | 99 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8445 |
| s2c | 100 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8967 |
| s2c | 101 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9140 |
| s2c | 102 | 10 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8420 |
| s2c | 103 | 19 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9195 |
| s2c | 104 | 14 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8453 |
| s2c | 105 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7602 |
| s2c | 106 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:10140 |
| s2c | 107 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8784 |
| s2c | 108 | 10 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7953 |
| s2c | 109 | 5 | i8(128-b), u32_alt13 | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8293 |
| s2c | 110 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8096 |
| s2c | 111 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8893 |
| s2c | 112 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8009 |
| s2c | 113 | 5 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8150 |
| s2c | 114 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8082 |
| s2c | 115 | 25 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7730 |
| s2c | 116 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7938 |
| s2c | 117 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8960 |
| s2c | 118 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8834 |
| s2c | 119 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8541 |
| s2c | 120 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9890 |
| s2c | 121 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8912 |
| s2c | 122 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9962 |
| s2c | 123 | 32 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8325 |
| s2c | 124 | 10 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8397 |
| s2c | 125 | 10 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9132 |
| s2c | 126 | 7 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7900 |
| s2c | 127 | 5 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8714 |
| s2c | 128 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9052 |
| s2c | 129 | 5 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7927 |
| s2c | 130 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8570 |
| s2c | 131 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8626 |
| s2c | 132 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8234 |
| s2c | 133 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9758 |
| s2c | 134 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7382 |
| s2c | 135 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8659 |
| s2c | 136 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9163 |
| s2c | 137 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7647 |
| s2c | 138 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8899 |
| s2c | 139 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8046 |
| s2c | 140 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7706 |
| s2c | 141 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:10193 |
| s2c | 142 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7375 |
| s2c | 143 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9081 |
| s2c | 144 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7410 |
| s2c | 145 | 10 | u16be id (65535 means -1), u8, u16be, u8, u16be, u16be | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8275 |
| s2c | 146 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8229 |
| s2c | 147 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8710 |
| s2c | 148 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8920 |
| s2c | 149 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9023 |
| s2c | 150 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8584 |
| s2c | 151 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9765 |
| s2c | 152 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8257 |
| s2c | 153 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8621 |
| s2c | 154 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7890 |
| s2c | 155 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9627 |
| s2c | 156 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7743 |
| s2c | 157 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8271 |
| s2c | 158 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7361 |
| s2c | 159 | 7 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9823 |
| s2c | 160 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8075 |
| s2c | 161 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8562 |
| s2c | 162 | 11 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9615 |
| s2c | 163 | 10 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8718 |
| s2c | 164 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9727 |
| s2c | 165 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8526 |
| s2c | 166 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9027 |
| s2c | 167 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8239 |
| s2c | 168 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8631 |
| s2c | 169 | 9 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8141 |
| s2c | 170 | 22 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8345 |
| s2c | 171 | -1 | jstr, u16be, u16be, u16be | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9240 |
| s2c | 172 | 5 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9547 |
| s2c | 173 | 21 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9534 |
| s2c | 174 | 3 | u16le_low+128, u8(b-128) | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9234 |
| s2c | 175 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7553 |
| s2c | 176 | 2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9863 |
| s2c | 177 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8106 |
| s2c | 178 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8547 |
| s2c | 179 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7931 |
| s2c | 180 | 5 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8885 |
| s2c | 181 | 0 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:10144 |
| s2c | 182 | 6 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9558 |
| s2c | 183 | -1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7466 |
| s2c | 184 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7557 |
| s2c | 185 | 4 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7676 |
| s2c | 186 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7624 |
| s2c | 187 | 1 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7628 |
| s2c | 188 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8645 |
| s2c | 189 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9884 |
| s2c | 190 | -2 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9176 |
| s2c | 191 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:8533 |
| s2c | 192 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9856 |
| s2c | 193 | 3 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:9673 |
| s2c | 194 | 8 |  | Client.doClient22 | builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:7640 |
