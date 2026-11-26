# Packets

Game opcodes are the constructor `id` values. `ServerProt.values()` is in that same id order (`ServerProt.java:402`), and `Client.read` indexes it with `gIsaac1or2` (`Client.java:8549`). Client packets write `prot.id` through `PacketBit.pIsaac1` (`ClientMessage.java:51`).

Size `-1` means one following unsigned byte. Size `-2` means one `g2` unsigned big-endian short (`Client.java:8559` and `Client.java:8570`). A positive size is the payload length after the ISAAC opcode. Login opcodes below are a different stream and are not ISAAC-framed.

An empty field list means that opcode's handler was not read. Width names: `u16le+128` is `p2_alt3` / `g2_alt3`, `u16be+128` is `p2_alt2` / `g2_alt2`, `u32le` is `p4_alt1` / `g4_alt1`, `u8+128` is `p1_alt1` / `g1_alt1`, `u8 negated` is `p1_alt2` / `g1_alt2`, `u8 (128-value)` is `p1_alt3` / `g1_alt3`. `p4_alt2` and `p4_alt3` are the mixed 32-bit orders in `Packet.java:809` and `Packet.java:817`.

## Client to server

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 MAP_BUILD_STUCK | 15 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:8 |
| c2s | 1 RESUME_P_HSLDIALOG | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:10 |
| c2s | 2 OPNPC5 | 3 | u8 (128-ctrl), u16be+128 npcIndex | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2329 |
| c2s | 3 RESUME_P_STRINGDIALOG | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:14 |
| c2s | 4 OPPLAYER6 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 5 EVENT_NATIVE_MOUSE_MOVE | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:18 |
| c2s | 6 OPPLAYER9 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 7 MESSAGE_PRIVATE | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:22 |
| c2s | 8 AFFINEDCLANSETTINGS_ADDBANNED_FROMCHANNEL | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:24 |
| c2s | 9 CLIENT_DETAILOPTIONS_STATUS | -1 | size prefix u8 backpatched by psize1, then preferences.createPreferencesBlock bytes (block internals unread) | ClientWatch.doClientWatch6 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/ClientWatch.java:187 |
| c2s | 10 RESUME_P_COUNTDIALOG | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:28 |
| c2s | 11 SEND_SNAPSHOT | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:30 |
| c2s | 12 OPLOC1 | 9 | u8 negated ctrl, u16be worldZ, u32be locId, u16le+128 worldX | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2268 |
| c2s | 13 IGNORELIST_ADD | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:34 |
| c2s | 14 SOUND_SONGEND | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:36 |
| c2s | 15 clientProt | 9 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:38 |
| c2s | 16 OPOBJ5 | 7 | u16le objIndex, u16le worldX, u16be worldZ, u8 (128-flags) | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2439 |
| c2s | 17 MESSAGE_QUICKCHAT_PUBLIC | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:42 |
| c2s | 18 CLICKWORLDMAP | 4 | u32 p4_alt3 of tint65 shifted 28, plus tint64 shifted 14, plus tint66 | Client.updateGame2 | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:4139 |
| c2s | 19 clientProt2 | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:46 |
| c2s | 20 CREATE_ACCOUNT | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:48 |
| c2s | 21 OPLOCT | 17 | u8+128 ctrl, u16le worldX, u16le activeInvobject, u16le+128 worldZ, u32le activeParent, u32 p4_alt2 locId, u16be activeComponentId | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2378 |
| c2s | 22 TRANSMITVAR_VERIFYID | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:52 |
| c2s | 23 REFLECTION_CHECK_REPLY | -1 | size prefix u8 backpatched by psize1 over ReflectionCheck.createReflectionCheck (payload unread) | Client.updateGame2 | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:3955 |
| c2s | 24 NATIVE_LIBRARY_FAILURE | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:56 |
| c2s | 25 OPOBJ2 | 7 | u16le objIndex, u16le worldX, u16be worldZ, u8 (128-flags) | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2439 |
| c2s | 26 OPOBJ3 | 7 | u16le objIndex, u16le worldX, u16be worldZ, u8 (128-flags) | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2439 |
| c2s | 27 OPLOC3 | 9 | u8 negated ctrl, u16be worldZ, u32be locId, u16le+128 worldX | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2268 |
| c2s | 28 CREATE_CHECK_EMAIL | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:64 |
| c2s | 29 IF_BUTTON4 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 30 SOUND_SONGPRELOADED | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:68 |
| c2s | 31 OPNPC4 | 3 | u8 (128-ctrl), u16be+128 npcIndex | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2329 |
| c2s | 32 CLAN_KICKUSER | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:72 |
| c2s | 33 MOVE_GAMECLICK | 5 | u16be worldZ, u8 ctrl, u16le+128 worldX | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2511 |
| c2s | 34 CHANGE_EMAIL_ADDRESS | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:76 |
| c2s | 35 FRIENDLIST_DEL | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:78 |
| c2s | 36 OPNPC6 | 3 | u8 (128-ctrl), u16be+128 npcIndex | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2329 |
| c2s | 37 EVENT_CAMERA_POSITION | 4 | u16le+128 yaw>>3, u16be pitch>>3 | ClientWatch.doClientWatch6 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/ClientWatch.java:173 |
| c2s | 38 clientProt3 | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:84 |
| c2s | 39 OPLOC6 | 9 | u8 negated ctrl, u16be worldZ, u32be locId, u16le+128 worldX | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2268 |
| c2s | 40 CUTSCENE_FINISHED | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:88 |
| c2s | 41 OPPLAYERT | 11 | u16be activeComponentId, u8+128 ctrl, u16le activeInvobject, u16le+128 playerIndex, u32 p4_alt2 activeParent | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2410 |
| c2s | 42 IGNORE_SETNOTES | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:92 |
| c2s | 43 clientProt4 | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:94 |
| c2s | 44 CHAT_SETMODE | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:96 |
| c2s | 45 EVENT_MOUSE_CLICK | 6 | u32be (x plus y shifted left 16), u16le+128 (MouseLogger.doMouseLogger unread in the low 15 bits, bit 15 set when buttonAction==2) | AppletMouseLogger.doAppletMouseLogger | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/client/AppletMouseLogger.java:40 |
| c2s | 46 clientProt5 | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:100 |
| c2s | 47 IGNORELIST_DEL | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:102 |
| c2s | 48 OPPLAYER8 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 49 OPPLAYER5 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 50 IF_BUTTON8 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 51 OPNPC1 | 3 | u8 (128-ctrl), u16be+128 npcIndex | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2329 |
| c2s | 52 AFFINEDCLANSETTINGS_SETMUTED_FROMCHANNEL | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:112 |
| c2s | 53 SIMPLE_TOOLKIT_CHANGE | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:114 |
| c2s | 54 OPPLAYER7 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 55 DIRECTX_FAILURE | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:118 |
| c2s | 56 OPNPC2 | 3 | u8 (128-ctrl), u16be+128 npcIndex | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2329 |
| c2s | 57 CLOSE_MODAL | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:122 |
| c2s | 58 IF_BUTTONT | 16 | u32le parentlayer, u16be invobject, u16be+128 activeInvobject, u16be+128 componentId, u16be+128 activeComponentId, u32le activeParent | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:12966 |
| c2s | 59 APCOORDT | 12 | u16be+128 worldX, u32le activeParent, u16be activeInvobject, u16be+128 worldZ, u16be+128 activeComponentId | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2471 |
| c2s | 60 SEND_EMAIL_VALIDATION_CODE | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:128 |
| c2s | 61 OPLOC4 | 9 | u8 negated ctrl, u16be worldZ, u32be locId, u16le+128 worldX | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2268 |
| c2s | 62 OPPLAYER2 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 63 IF_BUTTON7 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 64 ADD_NEW_EMAIL_ADDRESS | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:136 |
| c2s | 65 CREATE_CHECK_NAME | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:138 |
| c2s | 66 IF_BUTTON9 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 67 IF_BUTTON10 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 68 IF_BUTTON2 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 69 CREATE_SUGGEST_NAMES | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:146 |
| c2s | 70 IF_BUTTON5 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 71 STORE_SERVERPERM_VARCS | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:150 |
| c2s | 72 EVENT_NATIVE_MOUSE_CLICK | 7 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:152 |
| c2s | 73 FRIEND_SETRANK | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:154 |
| c2s | 74 PING_STATISTICS | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:156 |
| c2s | 75 FRIEND_SETNOTES | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:158 |
| c2s | 76 OPPLAYER3 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 77 WORLDLIST_FETCH | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:162 |
| c2s | 78 MOVE_MINIMAPCLICK | 18 | u16be worldZ, u8 ctrl, u16le+128 worldX, u8 255, u8 255, u16be orbitCameraYaw, u8 57, u8 minimapAnticheatAngle, u8 minimapZoom, u8 89, u16be localPlayer.x, u16be localPlayer.z, u8 63 | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2276 |
| c2s | 79 MAP_BUILD_COMPLETE | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:166 |
| c2s | 80 TELEMETRY_ERROR | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:168 |
| c2s | 81 OPOBJ1 | 7 | u16le objIndex, u16le worldX, u16be worldZ, u8 (128-flags) | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2439 |
| c2s | 82 UID_PASSPORT_RESEND_REQUEST | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:172 |
| c2s | 83 IF_BUTTON6 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 84 CLIENT_CHEAT | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:176 |
| c2s | 85 FRIENDLIST_ADD | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:178 |
| c2s | 86 IF_BUTTON3 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 87 EVENT_KEYBOARD | -2 | size prefix u16be equal to count times 4, then that many payload bytes as count times (u8 keyCode, u24be delta clamped to 16777215) | ClientWatch.doClientWatch6 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/ClientWatch.java:131 |
| c2s | 88 SET_CHATFILTERSETTINGS | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:184 |
| c2s | 89 MOVE_SCRIPTED | 5 | u8+128 MoveSpeed.serialID, u16be+128 z, u16le x | ScriptRunner.movescripted | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/clientscript/ScriptRunner.java:17461 |
| c2s | 90 DETECT_MODIFIED_CLIENT | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:188 |
| c2s | 91 OPPLAYER10 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 92 CLAN_JOINCHAT_LEAVECHAT | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:192 |
| c2s | 93 EVENT_APPLET_FOCUS | 1 | u8 focus | ClientWatch.doClientWatch6 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/ClientWatch.java:181 |
| c2s | 94 CLANCHANNEL_KICKUSER | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:196 |
| c2s | 95 MESSAGE_PUBLIC | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:198 |
| c2s | 96 OPLOC2 | 9 | u8 negated ctrl, u16be worldZ, u32be locId, u16le+128 worldX | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2268 |
| c2s | 97 EVENT_MOUSE_MOVE | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:202 |
| c2s | 98 RESUME_PAUSEBUTTON | 6 | u32 p4_alt3, u16be+128 | Client.doClient41 | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:12933 |
| c2s | 99 RESUME_P_OBJDIALOG | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:206 |
| c2s | 100 SEND_PING_REPLY | 9 | u32le, u32 p4_alt3, u8 negated fps | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9999 |
| c2s | 101 RESUME_P_NAMEDIALOG | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:210 |
| c2s | 102 OPPLAYER4 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 103 NO_TIMEOUT | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:214 |
| c2s | 104 IF_PLAYER | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:216 |
| c2s | 105 OPOBJ6 | 7 | u16le objIndex, u16le worldX, u16be worldZ, u8 (128-flags) | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2439 |
| c2s | 106 ABORT_P_DIALOG | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:220 |
| c2s | 107 FACE_SQUARE | 4 | u16be+128 worldZ, u16le+128 worldX | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2398 |
| c2s | 108 BUG_REPORT | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:224 |
| c2s | 109 CLIENT_COMPRESSEDTEXTUREFORMAT_SUPPORT | -2 | size prefix u16be backpatched by psize2, then u8 0 or 1, SerializableEnums.encodeSet unread, pSmart1or2 count, and pSmart2or4 id deltas | ClientWatch.doClientWatch6 | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/ClientWatch.java:199 |
| c2s | 110 RESUME_P_CLANFORUMQFCDIALOG | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:228 |
| c2s | 111 IF_BUTTON1 | 8 | u16le+128 invobject, u16be+128 componentId, u32be parentlayer | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:13075 |
| c2s | 112 AUTO_SETUP_RESULT | 18 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:232 |
| c2s | 113 OPNPCT | 11 | u32be activeParent, u16be npcIndex, u8+128 ctrl, u16le activeInvobject, u16be activeComponentId | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2362 |
| c2s | 114 OPOBJT | 15 | u16le objIndex, u8+128 ctrl, u16le activeInvobject, u16le worldZ, u16le worldX, u32 p4_alt2 activeParent, u16le+128 activeComponentId | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2341 |
| c2s | 115 MESSAGE_QUICKCHAT_PRIVATE | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:238 |
| c2s | 116 OPNPC3 | 3 | u8 (128-ctrl), u16be+128 npcIndex | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2329 |
| c2s | 117 OPPLAYER1 | 3 | u16be playerIndex, u8+128 ctrl | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2240 |
| c2s | 118 CREATE_LOG_PROGRESS | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:244 |
| c2s | 119 OPLOC5 | 9 | u8 negated ctrl, u16be worldZ, u32be locId, u16le+128 worldX | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2268 |
| c2s | 120 URL_REQUEST | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ClientProt.java:248 |
| c2s | 121 OPOBJ4 | 7 | u16le objIndex, u16le worldX, u16be worldZ, u8 (128-flags) | MiniMenu.doMiniMenu | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2439 |
| c2s | 122 IF_BUTTOND | 16 | u16be sourceId, u32 p4_alt3 sourceParent, u16le sourceInvobject, u16le+128 destInvobject, u32 p4_alt3 destParent, u16le destId | Client.doClient | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:12941 |
| c2s | 123 WINDOW_STATUS | 6 | u8 windowMode, u16be canvasWid, u16be canvasHei, u8 antiAliasing | Client.notifyWindowStatus | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:3236 |

## Server to client

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 0 TELEMETRY_GRID_ADD_GROUP | 5 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:8 |
| s2c | 1 ENVIRONMENT_OVERRIDE | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:10 |
| s2c | 2 UPDATE_FRIENDCHAT_CHANNEL_SINGLEUSER | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:12 |
| s2c | 3 CREATE_CHECK_EMAIL_REPLY | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:14 |
| s2c | 4 PROJANIM_SPECIFIC | 23 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:16 |
| s2c | 5 CAM_LOOKAT | 6 | u8 negated, u8, u8 (128-value), u8 negated, u16be+128 << 2 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11404 |
| s2c | 6 UPDATE_INV_FULL | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:20 |
| s2c | 7 MESSAGE_PRIVATE_ECHO | -2 | jstr, then WordPack.doWordPack (decoder unread) | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10418 |
| s2c | 8 MESSAGE_PUBLIC | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:24 |
| s2c | 9 REBUILD_REGION | -2 | payload passed whole to World.rebuildRegionMap; see region.md | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10103 |
| s2c | 10 UPDATE_SITESETTINGS | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:28 |
| s2c | 11 NPC_ANIM_SPECIFIC | 19 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:30 |
| s2c | 12 RESET_ANIMS | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:32 |
| s2c | 13 MAP_PROJANIM | 18 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:34 |
| s2c | 14 SET_PLAYER_OP | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:36 |
| s2c | 15 TELEMETRY_GRID_VALUES_DELTA | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:38 |
| s2c | 16 UPDATE_INV_PARTIAL | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:40 |
| s2c | 17 MIDI_SONG | 3 | u16le+128 id (65535 means -1), u8 negated volume | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10408 |
| s2c | 18 SET_MOVEACTION | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:44 |
| s2c | 19 CREATE_SUGGEST_NAME_ERROR | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:46 |
| s2c | 20 TELEMETRY_GRID_ADD_COLUMN | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:48 |
| s2c | 21 CLIENT_SETVARCSTR_LARGE | -2 | u16be, jstr | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11467 |
| s2c | 22 MESSAGE_CLANCHANNEL | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:52 |
| s2c | 23 UPDATE_FRIENDCHAT_CHANNEL_FULL | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:54 |
| s2c | 24 EXECUTE_CLIENT_CHEAT | 2 | u16be | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11462 |
| s2c | 25 FRIENDLIST_LOADED | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:58 |
| s2c | 26 IF_OPENSUB_ACTIVE_LOC | 32 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:60 |
| s2c | 27 CLEAR_PLAYER_SNAPSHOT | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:62 |
| s2c | 28 UPDATE_STOCKMARKET_SLOT | 21 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:64 |
| s2c | 29 SONG_PRELOAD | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:66 |
| s2c | 30 CLIENT_SETVARCSTR_SMALL | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:68 |
| s2c | 31 IF_SETTEXTFONT | 8 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:70 |
| s2c | 32 TELEMETRY_GRID_REMOVE_COLUMN | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:72 |
| s2c | 33 UPDATE_STAT | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:74 |
| s2c | 34 LOC_CUSTOMISE | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:76 |
| s2c | 35 IF_OPENTOP | 19 | u32 p4_alt2, u32le, u32 p4_alt2, u32be, u8 discarded, u16le+128 interfaceId | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10007 |
| s2c | 36 MESSAGE_FRIENDCHANNEL | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:80 |
| s2c | 37 VORBIS_SOUND | 8 | u16be id (65535 means -1), u8, u16be, u8, u16be | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11391 |
| s2c | 38 IF_OPENSUB | 23 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:84 |
| s2c | 39 TELEMETRY_GRID_MOVE_COLUMN | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:86 |
| s2c | 40 PLAYER_GROUP_FULL | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:88 |
| s2c | 41 MESSAGE_PLAYER_GROUP | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:90 |
| s2c | 42 VORBIS_SOUND_GROUP | 10 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:92 |
| s2c | 43 IF_SETPLAYERHEAD | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:94 |
| s2c | 44 VARBIT_SMALL | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:96 |
| s2c | 45 LOC_DEL | 2 | zone body: u8, u8 (128-value), then LocPositionAdjustment constructor (that constructor unread) | Client.handleZonePacket | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11513 |
| s2c | 46 UPDATE_FRIENDLIST | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:100 |
| s2c | 47 SETDRAWORDER | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:102 |
| s2c | 48 SEND_PING | 8 | u32be, u32be | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9997 |
| s2c | 49 OBJ_DEL | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:106 |
| s2c | 50 VARP_LARGE | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:108 |
| s2c | 51 serverProt | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:110 |
| s2c | 52 DEBUG_SERVER_TRIGGERS | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:112 |
| s2c | 53 UPDATE_UID192 | 28 | bytes(24) plus trailing u32be crc; handler adds 28 then Packet.checkcrc | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10429 |
| s2c | 54 CLIENT_SETVARCBIT_SMALL | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:116 |
| s2c | 55 MIDI_JINGLE | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:118 |
| s2c | 56 UPDATE_ZONE_PARTIAL_FOLLOWS | 3 | u8 level, i8 negated << 3 zoneZ, i8 << 3 zoneX | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9657 |
| s2c | 57 LOGOUT_TRANSFER | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:122 |
| s2c | 58 UPDATE_ZONE_PARTIAL_ENCLOSED | -2 | u8+128 level, i8 negated << 3 zoneZ, i8 negated << 3 zoneX, then u8 ZoneProt values() index and that zone body, repeated until packetSize | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9602 |
| s2c | 59 CLANCHANNEL_FULL | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:126 |
| s2c | 60 URL_OPEN | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:128 |
| s2c | 61 IF_OPENSUB_ACTIVE_PLAYER | 25 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:130 |
| s2c | 62 IF_SETPLAYERHEAD_OTHER | 10 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:132 |
| s2c | 63 IF_SETRECOL | 9 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:134 |
| s2c | 64 CAM_REMOVEROOF | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:136 |
| s2c | 65 UPDATE_INV_STOP_TRANSMIT | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:138 |
| s2c | 66 CREATE_SUGGEST_NAME_REPLY | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:140 |
| s2c | 67 PLAYER_SNAPSHOT | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:142 |
| s2c | 68 TELEMETRY_GRID_REMOVE_GROUP | 1 | u8 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9646 |
| s2c | 69 CLIENT_SETVARCBIT_LARGE | 6 | u16be+128, u32le | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:8946 |
| s2c | 70 SOUND_AREA | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:148 |
| s2c | 71 MAP_PROJANIM_HALFSQ | 21 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:150 |
| s2c | 72 IF_SETPOSITION | 8 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:152 |
| s2c | 73 OBJ_COUNT | 7 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:154 |
| s2c | 74 CHAT_FILTER_SETTINGS_PRIVATECHAT | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:156 |
| s2c | 75 TELEMETRY_GRID_FULL | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:158 |
| s2c | 76 SHOW_FACE_HERE | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:160 |
| s2c | 77 SOCIAL_NETWORK_LOGOUT | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:162 |
| s2c | 78 HINT_ARROW | 14 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:164 |
| s2c | 79 IF_SETSCROLLPOS | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:166 |
| s2c | 80 MESSAGE_QUICKCHAT_PRIVATE | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:168 |
| s2c | 81 CAM2_ENABLE | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:170 |
| s2c | 82 MESSAGE_CLANCHANNEL_SYSTEM | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:172 |
| s2c | 83 NO_TIMEOUT | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:174 |
| s2c | 84 LOC_ANIM_SPECIFIC | 10 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:176 |
| s2c | 85 OBJ_ADD | 5 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:178 |
| s2c | 86 LOGOUT_FULL | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:180 |
| s2c | 87 CAM_SMOOTHRESET | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:182 |
| s2c | 88 REBUILD_NORMAL | -2 | payload passed whole to World.rebuildNormalMap; see region.md | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11422 |
| s2c | 89 MESSAGE_QUICKCHAT_CLANCHANNEL | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:186 |
| s2c | 90 MESSAGE_PRIVATE | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:188 |
| s2c | 91 VARCLAN_DISABLE | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:190 |
| s2c | 92 VORBIS_SOUND_GROUP_START | 2 | u16be | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11376 |
| s2c | 93 MESSAGE_QUICKCHAT_PRIVATE_ECHO | -1 | jstr, u16be phrase id, then QuickChatPhraseType.getText consumes the rest (that decoder unread) | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9595 |
| s2c | 94 SET_MAP_FLAG | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:196 |
| s2c | 95 VORBIS_SOUND_GROUP_STOP | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:198 |
| s2c | 96 TELEMETRY_GRID_MOVE_ROW | 3 | u8+128, u8 (128-value), u8 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10443 |
| s2c | 97 VORBIS_PRELOAD_SOUNDS | 2 | u16be | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9579 |
| s2c | 98 CAM_MOVETO | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:204 |
| s2c | 99 RESET_CLIENT_VARCACHE | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:206 |
| s2c | 100 IF_SETPLAYERMODEL_OTHER | 10 | u16le, u32be, u32be | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11449 |
| s2c | 101 CLANSETTINGS_FULL | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:210 |
| s2c | 102 IF_OPENSUB_ACTIVE_NPC | 25 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:212 |
| s2c | 103 IF_SETANIM | 8 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:214 |
| s2c | 104 CAM_RESET | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:216 |
| s2c | 105 LOBBY_APPEARANCE | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:218 |
| s2c | 106 VARCLAN_ENABLE | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:220 |
| s2c | 107 POINTLIGHT_COLOUR | 8 | u32 p4_alt3, u16le, u16le+128 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10478 |
| s2c | 108 IF_SETRETEX | 9 | u16le+128, u16le, u32be, u8+128 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11414 |
| s2c | 109 IF_SETHIDE | 5 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:226 |
| s2c | 110 LAST_LOGIN_INFO | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:228 |
| s2c | 111 REFLECTION_CHECKER | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:230 |
| s2c | 112 IF_SETANGLE | 10 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:232 |
| s2c | 113 TEXT_COORD | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:234 |
| s2c | 114 VORBIS_SPEECH_STOP | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:236 |
| s2c | 115 VORBIS_PRELOAD_SOUND_GROUP | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:238 |
| s2c | 116 IF_SETMODEL | 8 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:240 |
| s2c | 117 DO_CHEAT | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:242 |
| s2c | 118 OBJ_REVEAL | 7 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:244 |
| s2c | 119 HINT_TRAIL | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:246 |
| s2c | 120 MESSAGE_GAME | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:248 |
| s2c | 121 IF_OPENSUB_ACTIVE_OBJ | 29 | u16be, u32 p4_alt3, u32 p4_alt2, u32le, u32be, u16le, u32 p4_alt3, u8+128, u32 p4_alt2 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10456 |
| s2c | 122 PLAYER_INFO | -2 | bit payload consumed by ReceivePlayerPositions.readPlayerInfo; see sync.md | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:8942 |
| s2c | 123 SOUND_MIXBUSS_ADD | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:254 |
| s2c | 124 LOC_PREFETCH | 5 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:256 |
| s2c | 125 NPC_HEADICON_SPECIFIC | 9 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:258 |
| s2c | 126 UPDATE_DOB | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:260 |
| s2c | 127 MIDI_SONG_LOCATION | 11 | u32 p4_alt3 coord, i32be song, u8+128, u8, u8 negated | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10027 |
| s2c | 128 CLIENT_SETVARC_LARGE | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:264 |
| s2c | 129 SERVER_TICK_END | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:266 |
| s2c | 130 IF_SETPLAYERHEAD_IGNOREWORN | 10 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:268 |
| s2c | 131 CREATE_ACCOUNT_REPLY | 1 | u8 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9626 |
| s2c | 132 CLANSETTINGS_DELTA | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:272 |
| s2c | 133 TRIGGER_ONDIALOGABORT | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:274 |
| s2c | 134 REDUCE_PLAYER_ATTACK_PRIORITY | 1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:276 |
| s2c | 135 IF_SET_HTTP_IMAGE | 8 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:278 |
| s2c | 136 CUTSCENE | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:280 |
| s2c | 137 MINIMAP_TOGGLE | 1 | u8 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9914 |
| s2c | 138 CHAT_FILTER_SETTINGS | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:284 |
| s2c | 139 LOC_ADD_CHANGE | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:286 |
| s2c | 140 IF_SETCOLOUR | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:288 |
| s2c | 141 REDUCE_NPC_ATTACK_PRIORITY | 1 | u8 (128-value) | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9636 |
| s2c | 142 VARBIT_LARGE | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:292 |
| s2c | 143 VORBIS_SPEECH_SOUND | 6 | u16be id (65535 means -1), u8, u16be, u8 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9614 |
| s2c | 144 POINTLIGHT_INTENSITY | 5 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:296 |
| s2c | 145 MESSAGE_QUICKCHAT_FRIENDCHAT | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:298 |
| s2c | 146 CLANCHANNEL_DELTA | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:300 |
| s2c | 147 LOC_ANIM | 7 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:302 |
| s2c | 148 STORE_SERVERPERM_VARCS_ACK | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:304 |
| s2c | 149 CLIENT_SETVARC_SMALL | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:306 |
| s2c | 150 SET_TARGET | 2 | i16be with low byte -128 (g2s_alt2) | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11435 |
| s2c | 151 IF_SETPLAYERMODEL_SELF | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:310 |
| s2c | 152 CAMERA_UPDATE | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:312 |
| s2c | 153 JS5_RELOAD | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:314 |
| s2c | 154 CHANGE_LOBBY | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:316 |
| s2c | 155 IF_SETEVENTS | 12 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:318 |
| s2c | 156 RUNCLIENTSCRIPT | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:320 |
| s2c | 157 VARP_SMALL | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:322 |
| s2c | 158 IF_SETOBJECT | 10 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:324 |
| s2c | 159 PLAYER_GROUP_VARPS | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:326 |
| s2c | 160 TELEMETRY_GRID_REMOVE_ROW | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:328 |
| s2c | 161 UPDATE_RUNENERGY | 1 | u8 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9583 |
| s2c | 162 SOUND_MIXBUSS_SETLEVEL | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:332 |
| s2c | 163 CREATE_CHECK_NAME_REPLY | 1 | u8 | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:11439 |
| s2c | 164 CAM_FORCEANGLE | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:336 |
| s2c | 165 IF_SETTEXTANTIMACRO | 5 | u32 p4_alt3, u8 flag | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:8597 |
| s2c | 166 IF_CLOSESUB | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:340 |
| s2c | 167 WORLDLIST_FETCH_REPLY | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:342 |
| s2c | 168 LOGOUT | 1 | u8 logout reason id | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10424 |
| s2c | 169 UPDATE_RUNWEIGHT | 2 | i16be | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10099 |
| s2c | 170 serverProt2 | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:348 |
| s2c | 171 CAM_SHAKE | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:350 |
| s2c | 172 VARCLAN | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:352 |
| s2c | 173 TELEMETRY_GRID_SET_ROW_PINNED | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:354 |
| s2c | 174 UPDATE_IGNORELIST | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:356 |
| s2c | 175 UPDATE_ZONE_FULL_FOLLOWS | 3 | i8 negated << 3 zoneZ, u8 negated level, i8 (128-value) << 3 zoneX | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9918 |
| s2c | 176 MESSAGE_QUICKCHAT_PLAYER_GROUP | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:360 |
| s2c | 177 UPDATE_REBOOT_TIMER | 2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:362 |
| s2c | 178 SPOTANIM_SPECIFIC | 12 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:364 |
| s2c | 179 IF_SETTARGETPARAM | 10 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:366 |
| s2c | 180 IF_SETPLAYERMODEL_SNAPSHOT | 5 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:368 |
| s2c | 181 IF_SETTEXT | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:370 |
| s2c | 182 SYNTH_SOUND | 8 | u16be id (65535 means -1), u8, u16be, u8, u16be | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9295 |
| s2c | 183 IF_SETGRAPHIC | 8 | u32 p4_alt2, u32le | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:9588 |
| s2c | 184 IF_SETCLICKMASK | 5 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:376 |
| s2c | 185 TELEMETRY_GRID_ADD_ROW | 6 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:378 |
| s2c | 186 NPC_INFO | -2 | bit payload consumed by Client.doClient36; see sync.md | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10474 |
| s2c | 187 PLAYER_GROUP_DELTA | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:382 |
| s2c | 188 LOYALTY_UPDATE | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:384 |
| s2c | 189 IF_SETNPCHEAD | 8 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:386 |
| s2c | 190 MIDI_SONG_STOP | 0 |  | Client.read | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:10437 |
| s2c | 191 IF_MOVESUB | 8 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:390 |
| s2c | 192 TELEMETRY_CLEAR_GRID_VALUE | 3 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:392 |
| s2c | 193 MAP_ANIM | 10 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:394 |
| s2c | 194 JCOINS_UPDATE | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/ServerProt.java:396 |

## Login stream

These ids are `LoginProt` (`LoginProt.java:50`). The constructor's second argument is not stored. The sizes below are the literals in the constructors. The lobby and game login blocks backpatch their own `u16be` length with `psize2`.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 14 INIT_GAME_CONNECTION | 0 | opcode only; no payload. Client then reads u8 (must be 0) and u64be server seed | LoginManager.update | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/login/LoginManager.java:362 |
| c2s | 15 INIT_JS5REMOTE_CONNECTION | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:10 |
| c2s | 16 GAMELOGIN | -2 | see login.md game block. u8 opcode, u16be length backpatched by psize2, then the bytes at LoginManager.java:495 | LoginManager.update | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/login/LoginManager.java:491 |
| c2s | 19 LOBBYLOGIN | -2 | see login.md lobby block. u8 opcode 19, u16be length backpatched by psize2, then the bytes at LoginManager.java:558 | LoginManager.update | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/login/LoginManager.java:554 |
| c2s | 23 REQUEST_WORLDLIST | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:16 |
| c2s | 24 CHECK_WORLD_SUITABILITY | -1 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:18 |
| c2s | 26 GAMELOGIN_CONTINUE | 0 | opcode only | LoginManager.update | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/login/LoginManager.java:835 |
| c2s | 27 SSL_WEBCONNECTION | 0 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:22 |
| c2s | 28 CREATE_ACCOUNT_CONNECT | -2 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:24 |
| c2s | 29 INIT_SOCIAL_NETWORK_CONNECTION | -2 | u8 opcode 29, u16be length, u32be 910, u32be 1, optional u8 when requestState==211, then RSA packet from startRSAPAcket plus packLoginTOTPDetails | LoginManager.update | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/login/LoginManager.java:396 |
| c2s | 30 SOCIAL_NETWORK_LOGIN | -2 | same outer block as LOBBYLOGIN or GAMELOGIN, chosen by requestState, with this opcode instead | LoginManager.update | builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/login/LoginManager.java:549 |
| c2s | 31 INIT_DEBUG_CONNECTION | 4 |  | unread | builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/network/protocol/LoginProt.java:30 |
