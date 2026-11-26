# Gaps

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| RSLib player teleport word | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/client/ReceivePlayerPositions.java:442 | Java adds the 30-bit plane and the two 14-bit fields to the current waypoint, then subtracts the region base. rslib/src/both/move/rev_910.c:213 writes absolute local_x, local_z, and plane into that word. | implementable |
| RSLib low-res skip 2045 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/client/ReceivePlayerPositions.java:84 | Login placement reads one 30-bit local position and then an 18-bit record for every index from 1 to 2047 except the local uid. PLAYER_INFO then uses four skip-counted passes. rslib/src/both/move/rev_910.c:59 always skips 2045 low-res slots. | implementable |
| RSLib appearance extend bit | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/client/ReceivePlayerPositions.java:275 | Java queues an appearance block when the high-res extend bit is 1. rslib/src/both/move/rev_910.c:172 forces that bit clear. | implementable |
| RSLib minimap click length | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2280 | Opcode 78 is size 18. The minimap caller appends 13 bytes after the shared 5-byte walk prefix. rslib/src/both/move/rev_910.c:9 writes only those 5 bytes and rejects nsteps. | implementable |
| PlayerEntity.getAppearance | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/client/ReceivePlayerPositions.java:733 | Mask bit 4 reads a length and transformed bytes, then calls getAppearance. That method was not opened. | implementable |
| decodeVarValue inside player masks | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/client/ReceivePlayerPositions.java:634 | Mask bits 262144 and 131072 call varPlayerTypeList.decodeVarValue. That decoder was not opened. | implementable |
| NPC mask bodies after bit 4096 | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:12281 | doClient392 continues through bit 4194304 at Client.java:12653. Those bodies were not read. | implementable |
| LocPositionAdjustment | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11517 | LOC_DEL reads two bytes then constructs LocPositionAdjustment. The constructor was not opened. | implementable |
| Other zone opcode bodies | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11482 | handleZonePacket was read for OBJ_COUNT, LOC_DEL, and SOUND_AREA. The other ZoneProt arms were not read. Constructor ints on ZoneProt are discarded and are not lengths. | implementable |
| MouseLogger.doMouseLogger | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/client/AppletMouseLogger.java:19 | EVENT_MOUSE_CLICK calls the parent logger for the low 15 bits of the second field. AppletMouseLogger.doAppletMouseLogger for EVENT_MOUSE_MOVE is empty. | implementable |
| SerializableEnums.encodeSet | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/ClientWatch.java:223 | CLIENT_COMPRESSEDTEXTUREFORMAT_SUPPORT calls encodeSet. That method was not opened. | implementable |
| preferences.createPreferencesBlock | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/ClientWatch.java:189 | The preferences byte block written at login and in CLIENT_DETAILOPTIONS_STATUS was not opened. | implementable |
| HardwarePlatform.createHardwareBlock | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/login/LoginManager.java:584 | Lobby and game login copy a hardware block whose size comes from calculateHardwarePlatformSize. Those methods were not opened. | implementable |
| PublicKeys modulus bytes | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/login/LoginManager.java:1194 | createLoginRSAPacket calls rsaenc(PublicKeys.LOGIN_RSAE, PublicKeys.LOGIN_RSAN). The key bytes were not opened. | implementable |
| WordPack.doWordPack | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10419 | MESSAGE_PRIVATE_ECHO reads a jstr and then a word pack. The pack decoder was not opened. | implementable |
| QuickChatPhraseType.getText | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9597 | MESSAGE_QUICKCHAT_PRIVATE_ECHO passes the remainder of the packet to getText. That decoder was not opened. | implementable |
| Camera.decode | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11457 | CAMERA_UPDATE calls cam2.decode. That method was not opened. | implementable |
| ClientScriptCommand values slots | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/script/ClientScriptCommand.java:2876 | The wire opcode is the index in values(), not the constructor argument. The values() array was not transcribed. executeCommand was not read. | implementable |
| Config field widths beyond the cited slices | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/config/objtype/ObjType.java:317 | ObjType.decode was read through the start of opcode 42. GraphicsDefaults.decode was read through opcode 11. Other config decode bodies were not opened. | implementable |
| JS5 group XTEA keys | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/core/io/Packet.java:599 | Region rebuild packets contain no XTEA keys. Group decryption uses tinydec. The Js5 call site that supplies the keys was not opened. | implementable |
| Software raster bodies | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/graphics/safe/PureJavaToolkit.java:711 | drawRectangle, drawHorizontalLine, drawVerticalLine, drawLine, drawParticles, and drawBillboard were named. Their pixel loops were not read. | implementable |
| No disk cache | builds/rs3/910-2019-10-16/meta.json:19 | cache_downloaded is false and openrs2_cache_id is null. Numeric interface ids and map bytes are not in this archive. no-cache | blocked |
| jaggl and jagdx | builds/rs3/910-2019-10-16/src/src/main/java/jaggl/OpenGL.java:1 | Platform JNI bindings. There is no jagtheora directory. platform-native | blocked |
| c2s opcode 0 MAP_BUILD_STUCK | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:8 | handler not read | implementable |
| c2s opcode 1 RESUME_P_HSLDIALOG | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:10 | handler not read | implementable |
| c2s opcode 3 RESUME_P_STRINGDIALOG | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:14 | handler not read | implementable |
| c2s opcode 5 EVENT_NATIVE_MOUSE_MOVE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:18 | handler not read | implementable |
| c2s opcode 7 MESSAGE_PRIVATE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:22 | handler not read | implementable |
| c2s opcode 8 AFFINEDCLANSETTINGS_ADDBANNED_FROMCHANNEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:24 | handler not read | implementable |
| c2s opcode 10 RESUME_P_COUNTDIALOG | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:28 | handler not read | implementable |
| c2s opcode 11 SEND_SNAPSHOT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:30 | handler not read | implementable |
| c2s opcode 13 IGNORELIST_ADD | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:34 | handler not read | implementable |
| c2s opcode 14 SOUND_SONGEND | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:36 | handler not read | implementable |
| c2s opcode 15 clientProt | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:38 | handler not read | implementable |
| c2s opcode 17 MESSAGE_QUICKCHAT_PUBLIC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:42 | handler not read | implementable |
| c2s opcode 19 clientProt2 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:46 | handler not read | implementable |
| c2s opcode 20 CREATE_ACCOUNT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:48 | handler not read | implementable |
| c2s opcode 22 TRANSMITVAR_VERIFYID | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:52 | handler not read | implementable |
| c2s opcode 24 NATIVE_LIBRARY_FAILURE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:56 | handler not read | implementable |
| c2s opcode 28 CREATE_CHECK_EMAIL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:64 | handler not read | implementable |
| c2s opcode 30 SOUND_SONGPRELOADED | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:68 | handler not read | implementable |
| c2s opcode 32 CLAN_KICKUSER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:72 | handler not read | implementable |
| c2s opcode 34 CHANGE_EMAIL_ADDRESS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:76 | handler not read | implementable |
| c2s opcode 35 FRIENDLIST_DEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:78 | handler not read | implementable |
| c2s opcode 38 clientProt3 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:84 | handler not read | implementable |
| c2s opcode 40 CUTSCENE_FINISHED | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:88 | handler not read | implementable |
| c2s opcode 42 IGNORE_SETNOTES | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:92 | handler not read | implementable |
| c2s opcode 43 clientProt4 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:94 | handler not read | implementable |
| c2s opcode 44 CHAT_SETMODE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:96 | handler not read | implementable |
| c2s opcode 46 clientProt5 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:100 | handler not read | implementable |
| c2s opcode 47 IGNORELIST_DEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:102 | handler not read | implementable |
| c2s opcode 52 AFFINEDCLANSETTINGS_SETMUTED_FROMCHANNEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:112 | handler not read | implementable |
| c2s opcode 53 SIMPLE_TOOLKIT_CHANGE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:114 | handler not read | implementable |
| c2s opcode 55 DIRECTX_FAILURE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:118 | handler not read | implementable |
| c2s opcode 57 CLOSE_MODAL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:122 | handler not read | implementable |
| c2s opcode 60 SEND_EMAIL_VALIDATION_CODE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:128 | handler not read | implementable |
| c2s opcode 64 ADD_NEW_EMAIL_ADDRESS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:136 | handler not read | implementable |
| c2s opcode 65 CREATE_CHECK_NAME | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:138 | handler not read | implementable |
| c2s opcode 69 CREATE_SUGGEST_NAMES | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:146 | handler not read | implementable |
| c2s opcode 71 STORE_SERVERPERM_VARCS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:150 | handler not read | implementable |
| c2s opcode 72 EVENT_NATIVE_MOUSE_CLICK | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:152 | handler not read | implementable |
| c2s opcode 73 FRIEND_SETRANK | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:154 | handler not read | implementable |
| c2s opcode 74 PING_STATISTICS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:156 | handler not read | implementable |
| c2s opcode 75 FRIEND_SETNOTES | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:158 | handler not read | implementable |
| c2s opcode 77 WORLDLIST_FETCH | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:162 | handler not read | implementable |
| c2s opcode 79 MAP_BUILD_COMPLETE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:166 | handler not read | implementable |
| c2s opcode 80 TELEMETRY_ERROR | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:168 | handler not read | implementable |
| c2s opcode 82 UID_PASSPORT_RESEND_REQUEST | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:172 | handler not read | implementable |
| c2s opcode 84 CLIENT_CHEAT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:176 | handler not read | implementable |
| c2s opcode 85 FRIENDLIST_ADD | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:178 | handler not read | implementable |
| c2s opcode 88 SET_CHATFILTERSETTINGS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:184 | handler not read | implementable |
| c2s opcode 90 DETECT_MODIFIED_CLIENT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:188 | handler not read | implementable |
| c2s opcode 92 CLAN_JOINCHAT_LEAVECHAT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:192 | handler not read | implementable |
| c2s opcode 94 CLANCHANNEL_KICKUSER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:196 | handler not read | implementable |
| c2s opcode 95 MESSAGE_PUBLIC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:198 | handler not read | implementable |
| c2s opcode 97 EVENT_MOUSE_MOVE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:202 | handler not read | implementable |
| c2s opcode 99 RESUME_P_OBJDIALOG | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:206 | handler not read | implementable |
| c2s opcode 101 RESUME_P_NAMEDIALOG | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:210 | handler not read | implementable |
| c2s opcode 103 NO_TIMEOUT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:214 | handler not read | implementable |
| c2s opcode 104 IF_PLAYER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:216 | handler not read | implementable |
| c2s opcode 106 ABORT_P_DIALOG | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:220 | handler not read | implementable |
| c2s opcode 108 BUG_REPORT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:224 | handler not read | implementable |
| c2s opcode 110 RESUME_P_CLANFORUMQFCDIALOG | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:228 | handler not read | implementable |
| c2s opcode 112 AUTO_SETUP_RESULT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:232 | handler not read | implementable |
| c2s opcode 115 MESSAGE_QUICKCHAT_PRIVATE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:238 | handler not read | implementable |
| c2s opcode 118 CREATE_LOG_PROGRESS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:244 | handler not read | implementable |
| c2s opcode 120 URL_REQUEST | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:248 | handler not read | implementable |
| s2c opcode 0 TELEMETRY_GRID_ADD_GROUP | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:8 | handler not read | implementable |
| s2c opcode 1 ENVIRONMENT_OVERRIDE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:10 | handler not read | implementable |
| s2c opcode 2 UPDATE_FRIENDCHAT_CHANNEL_SINGLEUSER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:12 | handler not read | implementable |
| s2c opcode 3 CREATE_CHECK_EMAIL_REPLY | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:14 | handler not read | implementable |
| s2c opcode 4 PROJANIM_SPECIFIC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:16 | handler not read | implementable |
| s2c opcode 6 UPDATE_INV_FULL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:20 | handler not read | implementable |
| s2c opcode 8 MESSAGE_PUBLIC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:24 | handler not read | implementable |
| s2c opcode 10 UPDATE_SITESETTINGS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:28 | handler not read | implementable |
| s2c opcode 11 NPC_ANIM_SPECIFIC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:30 | handler not read | implementable |
| s2c opcode 12 RESET_ANIMS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:32 | handler not read | implementable |
| s2c opcode 13 MAP_PROJANIM | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:34 | handler not read | implementable |
| s2c opcode 14 SET_PLAYER_OP | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:36 | handler not read | implementable |
| s2c opcode 15 TELEMETRY_GRID_VALUES_DELTA | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:38 | handler not read | implementable |
| s2c opcode 16 UPDATE_INV_PARTIAL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:40 | handler not read | implementable |
| s2c opcode 18 SET_MOVEACTION | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:44 | handler not read | implementable |
| s2c opcode 19 CREATE_SUGGEST_NAME_ERROR | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:46 | handler not read | implementable |
| s2c opcode 20 TELEMETRY_GRID_ADD_COLUMN | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:48 | handler not read | implementable |
| s2c opcode 22 MESSAGE_CLANCHANNEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:52 | handler not read | implementable |
| s2c opcode 23 UPDATE_FRIENDCHAT_CHANNEL_FULL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:54 | handler not read | implementable |
| s2c opcode 25 FRIENDLIST_LOADED | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:58 | handler not read | implementable |
| s2c opcode 26 IF_OPENSUB_ACTIVE_LOC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:60 | handler not read | implementable |
| s2c opcode 27 CLEAR_PLAYER_SNAPSHOT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:62 | handler not read | implementable |
| s2c opcode 28 UPDATE_STOCKMARKET_SLOT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:64 | handler not read | implementable |
| s2c opcode 29 SONG_PRELOAD | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:66 | handler not read | implementable |
| s2c opcode 30 CLIENT_SETVARCSTR_SMALL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:68 | handler not read | implementable |
| s2c opcode 31 IF_SETTEXTFONT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:70 | handler not read | implementable |
| s2c opcode 32 TELEMETRY_GRID_REMOVE_COLUMN | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:72 | handler not read | implementable |
| s2c opcode 33 UPDATE_STAT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:74 | handler not read | implementable |
| s2c opcode 34 LOC_CUSTOMISE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:76 | handler not read | implementable |
| s2c opcode 36 MESSAGE_FRIENDCHANNEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:80 | handler not read | implementable |
| s2c opcode 38 IF_OPENSUB | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:84 | handler not read | implementable |
| s2c opcode 39 TELEMETRY_GRID_MOVE_COLUMN | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:86 | handler not read | implementable |
| s2c opcode 40 PLAYER_GROUP_FULL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:88 | handler not read | implementable |
| s2c opcode 41 MESSAGE_PLAYER_GROUP | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:90 | handler not read | implementable |
| s2c opcode 42 VORBIS_SOUND_GROUP | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:92 | handler not read | implementable |
| s2c opcode 43 IF_SETPLAYERHEAD | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:94 | handler not read | implementable |
| s2c opcode 44 VARBIT_SMALL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:96 | handler not read | implementable |
| s2c opcode 46 UPDATE_FRIENDLIST | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:100 | handler not read | implementable |
| s2c opcode 47 SETDRAWORDER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:102 | handler not read | implementable |
| s2c opcode 49 OBJ_DEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:106 | handler not read | implementable |
| s2c opcode 50 VARP_LARGE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:108 | handler not read | implementable |
| s2c opcode 51 serverProt | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:110 | handler not read | implementable |
| s2c opcode 52 DEBUG_SERVER_TRIGGERS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:112 | handler not read | implementable |
| s2c opcode 54 CLIENT_SETVARCBIT_SMALL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:116 | handler not read | implementable |
| s2c opcode 55 MIDI_JINGLE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:118 | handler not read | implementable |
| s2c opcode 57 LOGOUT_TRANSFER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:122 | handler not read | implementable |
| s2c opcode 59 CLANCHANNEL_FULL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:126 | handler not read | implementable |
| s2c opcode 60 URL_OPEN | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:128 | handler not read | implementable |
| s2c opcode 61 IF_OPENSUB_ACTIVE_PLAYER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:130 | handler not read | implementable |
| s2c opcode 62 IF_SETPLAYERHEAD_OTHER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:132 | handler not read | implementable |
| s2c opcode 63 IF_SETRECOL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:134 | handler not read | implementable |
| s2c opcode 64 CAM_REMOVEROOF | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:136 | handler not read | implementable |
| s2c opcode 65 UPDATE_INV_STOP_TRANSMIT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:138 | handler not read | implementable |
| s2c opcode 66 CREATE_SUGGEST_NAME_REPLY | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:140 | handler not read | implementable |
| s2c opcode 67 PLAYER_SNAPSHOT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:142 | handler not read | implementable |
| s2c opcode 70 SOUND_AREA | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:148 | handler not read | implementable |
| s2c opcode 71 MAP_PROJANIM_HALFSQ | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:150 | handler not read | implementable |
| s2c opcode 72 IF_SETPOSITION | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:152 | handler not read | implementable |
| s2c opcode 73 OBJ_COUNT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:154 | handler not read | implementable |
| s2c opcode 74 CHAT_FILTER_SETTINGS_PRIVATECHAT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:156 | handler not read | implementable |
| s2c opcode 75 TELEMETRY_GRID_FULL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:158 | handler not read | implementable |
| s2c opcode 76 SHOW_FACE_HERE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:160 | handler not read | implementable |
| s2c opcode 77 SOCIAL_NETWORK_LOGOUT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:162 | handler not read | implementable |
| s2c opcode 78 HINT_ARROW | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:164 | handler not read | implementable |
| s2c opcode 79 IF_SETSCROLLPOS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:166 | handler not read | implementable |
| s2c opcode 80 MESSAGE_QUICKCHAT_PRIVATE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:168 | handler not read | implementable |
| s2c opcode 81 CAM2_ENABLE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:170 | handler not read | implementable |
| s2c opcode 82 MESSAGE_CLANCHANNEL_SYSTEM | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:172 | handler not read | implementable |
| s2c opcode 83 NO_TIMEOUT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:174 | handler not read | implementable |
| s2c opcode 84 LOC_ANIM_SPECIFIC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:176 | handler not read | implementable |
| s2c opcode 85 OBJ_ADD | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:178 | handler not read | implementable |
| s2c opcode 86 LOGOUT_FULL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:180 | handler not read | implementable |
| s2c opcode 87 CAM_SMOOTHRESET | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:182 | handler not read | implementable |
| s2c opcode 89 MESSAGE_QUICKCHAT_CLANCHANNEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:186 | handler not read | implementable |
| s2c opcode 90 MESSAGE_PRIVATE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:188 | handler not read | implementable |
| s2c opcode 91 VARCLAN_DISABLE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:190 | handler not read | implementable |
| s2c opcode 94 SET_MAP_FLAG | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:196 | handler not read | implementable |
| s2c opcode 95 VORBIS_SOUND_GROUP_STOP | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:198 | handler not read | implementable |
| s2c opcode 98 CAM_MOVETO | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:204 | handler not read | implementable |
| s2c opcode 99 RESET_CLIENT_VARCACHE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:206 | handler not read | implementable |
| s2c opcode 101 CLANSETTINGS_FULL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:210 | handler not read | implementable |
| s2c opcode 102 IF_OPENSUB_ACTIVE_NPC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:212 | handler not read | implementable |
| s2c opcode 103 IF_SETANIM | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:214 | handler not read | implementable |
| s2c opcode 104 CAM_RESET | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:216 | handler not read | implementable |
| s2c opcode 105 LOBBY_APPEARANCE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:218 | handler not read | implementable |
| s2c opcode 106 VARCLAN_ENABLE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:220 | handler not read | implementable |
| s2c opcode 109 IF_SETHIDE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:226 | handler not read | implementable |
| s2c opcode 110 LAST_LOGIN_INFO | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:228 | handler not read | implementable |
| s2c opcode 111 REFLECTION_CHECKER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:230 | handler not read | implementable |
| s2c opcode 112 IF_SETANGLE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:232 | handler not read | implementable |
| s2c opcode 113 TEXT_COORD | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:234 | handler not read | implementable |
| s2c opcode 114 VORBIS_SPEECH_STOP | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:236 | handler not read | implementable |
| s2c opcode 115 VORBIS_PRELOAD_SOUND_GROUP | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:238 | handler not read | implementable |
| s2c opcode 116 IF_SETMODEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:240 | handler not read | implementable |
| s2c opcode 117 DO_CHEAT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:242 | handler not read | implementable |
| s2c opcode 118 OBJ_REVEAL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:244 | handler not read | implementable |
| s2c opcode 119 HINT_TRAIL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:246 | handler not read | implementable |
| s2c opcode 120 MESSAGE_GAME | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:248 | handler not read | implementable |
| s2c opcode 123 SOUND_MIXBUSS_ADD | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:254 | handler not read | implementable |
| s2c opcode 124 LOC_PREFETCH | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:256 | handler not read | implementable |
| s2c opcode 125 NPC_HEADICON_SPECIFIC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:258 | handler not read | implementable |
| s2c opcode 126 UPDATE_DOB | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:260 | handler not read | implementable |
| s2c opcode 128 CLIENT_SETVARC_LARGE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:264 | handler not read | implementable |
| s2c opcode 129 SERVER_TICK_END | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:266 | handler not read | implementable |
| s2c opcode 130 IF_SETPLAYERHEAD_IGNOREWORN | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:268 | handler not read | implementable |
| s2c opcode 132 CLANSETTINGS_DELTA | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:272 | handler not read | implementable |
| s2c opcode 133 TRIGGER_ONDIALOGABORT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:274 | handler not read | implementable |
| s2c opcode 134 REDUCE_PLAYER_ATTACK_PRIORITY | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:276 | handler not read | implementable |
| s2c opcode 135 IF_SET_HTTP_IMAGE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:278 | handler not read | implementable |
| s2c opcode 136 CUTSCENE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:280 | handler not read | implementable |
| s2c opcode 138 CHAT_FILTER_SETTINGS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:284 | handler not read | implementable |
| s2c opcode 139 LOC_ADD_CHANGE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:286 | handler not read | implementable |
| s2c opcode 140 IF_SETCOLOUR | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:288 | handler not read | implementable |
| s2c opcode 142 VARBIT_LARGE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:292 | handler not read | implementable |
| s2c opcode 144 POINTLIGHT_INTENSITY | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:296 | handler not read | implementable |
| s2c opcode 145 MESSAGE_QUICKCHAT_FRIENDCHAT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:298 | handler not read | implementable |
| s2c opcode 146 CLANCHANNEL_DELTA | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:300 | handler not read | implementable |
| s2c opcode 147 LOC_ANIM | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:302 | handler not read | implementable |
| s2c opcode 148 STORE_SERVERPERM_VARCS_ACK | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:304 | handler not read | implementable |
| s2c opcode 149 CLIENT_SETVARC_SMALL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:306 | handler not read | implementable |
| s2c opcode 151 IF_SETPLAYERMODEL_SELF | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:310 | handler not read | implementable |
| s2c opcode 152 CAMERA_UPDATE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:312 | handler not read | implementable |
| s2c opcode 153 JS5_RELOAD | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:314 | handler not read | implementable |
| s2c opcode 154 CHANGE_LOBBY | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:316 | handler not read | implementable |
| s2c opcode 155 IF_SETEVENTS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:318 | handler not read | implementable |
| s2c opcode 156 RUNCLIENTSCRIPT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:320 | handler not read | implementable |
| s2c opcode 157 VARP_SMALL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:322 | handler not read | implementable |
| s2c opcode 158 IF_SETOBJECT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:324 | handler not read | implementable |
| s2c opcode 159 PLAYER_GROUP_VARPS | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:326 | handler not read | implementable |
| s2c opcode 160 TELEMETRY_GRID_REMOVE_ROW | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:328 | handler not read | implementable |
| s2c opcode 162 SOUND_MIXBUSS_SETLEVEL | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:332 | handler not read | implementable |
| s2c opcode 164 CAM_FORCEANGLE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:336 | handler not read | implementable |
| s2c opcode 166 IF_CLOSESUB | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:340 | handler not read | implementable |
| s2c opcode 167 WORLDLIST_FETCH_REPLY | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:342 | handler not read | implementable |
| s2c opcode 170 serverProt2 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:348 | handler not read | implementable |
| s2c opcode 171 CAM_SHAKE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:350 | handler not read | implementable |
| s2c opcode 172 VARCLAN | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:352 | handler not read | implementable |
| s2c opcode 173 TELEMETRY_GRID_SET_ROW_PINNED | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:354 | handler not read | implementable |
| s2c opcode 174 UPDATE_IGNORELIST | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:356 | handler not read | implementable |
| s2c opcode 176 MESSAGE_QUICKCHAT_PLAYER_GROUP | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:360 | handler not read | implementable |
| s2c opcode 177 UPDATE_REBOOT_TIMER | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:362 | handler not read | implementable |
| s2c opcode 178 SPOTANIM_SPECIFIC | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:364 | handler not read | implementable |
| s2c opcode 179 IF_SETTARGETPARAM | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:366 | handler not read | implementable |
| s2c opcode 180 IF_SETPLAYERMODEL_SNAPSHOT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:368 | handler not read | implementable |
| s2c opcode 181 IF_SETTEXT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:370 | handler not read | implementable |
| s2c opcode 184 IF_SETCLICKMASK | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:376 | handler not read | implementable |
| s2c opcode 185 TELEMETRY_GRID_ADD_ROW | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:378 | handler not read | implementable |
| s2c opcode 187 PLAYER_GROUP_DELTA | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:382 | handler not read | implementable |
| s2c opcode 188 LOYALTY_UPDATE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:384 | handler not read | implementable |
| s2c opcode 189 IF_SETNPCHEAD | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:386 | handler not read | implementable |
| s2c opcode 191 IF_MOVESUB | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:390 | handler not read | implementable |
| s2c opcode 192 TELEMETRY_CLEAR_GRID_VALUE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:392 | handler not read | implementable |
| s2c opcode 193 MAP_ANIM | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:394 | handler not read | implementable |
| s2c opcode 194 JCOINS_UPDATE | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:396 | handler not read | implementable |
| login opcode 15 INIT_JS5REMOTE_CONNECTION | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:10 | handler not read | implementable |
| login opcode 23 REQUEST_WORLDLIST | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:16 | handler not read | implementable |
| login opcode 24 CHECK_WORLD_SUITABILITY | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:18 | handler not read | implementable |
| login opcode 27 SSL_WEBCONNECTION | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:22 | handler not read | implementable |
| login opcode 28 CREATE_ACCOUNT_CONNECT | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:24 | handler not read | implementable |
| login opcode 31 INIT_DEBUG_CONNECTION | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:30 | handler not read | implementable |
