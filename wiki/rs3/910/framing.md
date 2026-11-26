# Framing

Archive commit `962888d50a69a95e6011e2018a39fe668ed4982a`. Sizes are copied from the constructors in this client. `ClientProt` stores `id` and `size` (`ClientProt.java:260`). `ServerProt` stores both (`ServerProt.java:407`). `LoginProt` stores only `id` (`LoginProt.java:50`); the size column is the unused constructor argument. `ZoneProt` discards its constructor argument (`ZoneProt.java:43`). Zone opcodes are the index in `ZoneProt.values()` (`ZoneProt.java:39`) and are described in `region.md`.

## Server to client

`Client.read` (`Client.java:8519`) waits for one byte, and if `PacketBit.isIsaac2` (`PacketBit.java:35`) says the peeked byte minus `Isaac.getInt` is at least 128, it reads one more byte. The opcode is `gIsaac1or2` (`PacketBit.java:41`): one ISAAC byte when the value is below 128, otherwise `(value-128)<<8` plus a second ISAAC byte. ISAAC subtraction is `byte - isaac.nextInt()` masked to 8 bits (`PacketBit.java:30`).

The size is `ServerProt.size`. If it is `-1`, the next byte is the payload length. If it is `-2`, the next `g2` is the payload length (`Client.java:8559`). The payload is then read in one slice. There is no further length outside that prefix.

Inbound ISAAC is `new Isaac` on the four login `outKey` ints each plus 50. Outbound ISAAC is `new Isaac(outKey)`. `connection.in.setIsaac(randomIn)` (`LoginManager.java:600`).

| opcode | name | size |
| --- | --- | --- |
| 0 | TELEMETRY_GRID_ADD_GROUP | 5 |
| 1 | ENVIRONMENT_OVERRIDE | -1 |
| 2 | UPDATE_FRIENDCHAT_CHANNEL_SINGLEUSER | -1 |
| 3 | CREATE_CHECK_EMAIL_REPLY | 1 |
| 4 | PROJANIM_SPECIFIC | 23 |
| 5 | CAM_LOOKAT | 6 |
| 6 | UPDATE_INV_FULL | -2 |
| 7 | MESSAGE_PRIVATE_ECHO | -2 |
| 8 | MESSAGE_PUBLIC | -1 |
| 9 | REBUILD_REGION | -2 |
| 10 | UPDATE_SITESETTINGS | -1 |
| 11 | NPC_ANIM_SPECIFIC | 19 |
| 12 | RESET_ANIMS | 0 |
| 13 | MAP_PROJANIM | 18 |
| 14 | SET_PLAYER_OP | -1 |
| 15 | TELEMETRY_GRID_VALUES_DELTA | -2 |
| 16 | UPDATE_INV_PARTIAL | -2 |
| 17 | MIDI_SONG | 3 |
| 18 | SET_MOVEACTION | -1 |
| 19 | CREATE_SUGGEST_NAME_ERROR | 1 |
| 20 | TELEMETRY_GRID_ADD_COLUMN | 6 |
| 21 | CLIENT_SETVARCSTR_LARGE | -2 |
| 22 | MESSAGE_CLANCHANNEL | -1 |
| 23 | UPDATE_FRIENDCHAT_CHANNEL_FULL | -2 |
| 24 | EXECUTE_CLIENT_CHEAT | 2 |
| 25 | FRIENDLIST_LOADED | 0 |
| 26 | IF_OPENSUB_ACTIVE_LOC | 32 |
| 27 | CLEAR_PLAYER_SNAPSHOT | 1 |
| 28 | UPDATE_STOCKMARKET_SLOT | 21 |
| 29 | SONG_PRELOAD | 2 |
| 30 | CLIENT_SETVARCSTR_SMALL | -1 |
| 31 | IF_SETTEXTFONT | 8 |
| 32 | TELEMETRY_GRID_REMOVE_COLUMN | 2 |
| 33 | UPDATE_STAT | 6 |
| 34 | LOC_CUSTOMISE | -1 |
| 35 | IF_OPENTOP | 19 |
| 36 | MESSAGE_FRIENDCHANNEL | -1 |
| 37 | VORBIS_SOUND | 8 |
| 38 | IF_OPENSUB | 23 |
| 39 | TELEMETRY_GRID_MOVE_COLUMN | 3 |
| 40 | PLAYER_GROUP_FULL | -2 |
| 41 | MESSAGE_PLAYER_GROUP | -1 |
| 42 | VORBIS_SOUND_GROUP | 10 |
| 43 | IF_SETPLAYERHEAD | 4 |
| 44 | VARBIT_SMALL | 3 |
| 45 | LOC_DEL | 2 |
| 46 | UPDATE_FRIENDLIST | -2 |
| 47 | SETDRAWORDER | 1 |
| 48 | SEND_PING | 8 |
| 49 | OBJ_DEL | 3 |
| 50 | VARP_LARGE | 6 |
| 51 | serverProt | -2 |
| 52 | DEBUG_SERVER_TRIGGERS | -1 |
| 53 | UPDATE_UID192 | 28 |
| 54 | CLIENT_SETVARCBIT_SMALL | 3 |
| 55 | MIDI_JINGLE | 3 |
| 56 | UPDATE_ZONE_PARTIAL_FOLLOWS | 3 |
| 57 | LOGOUT_TRANSFER | -1 |
| 58 | UPDATE_ZONE_PARTIAL_ENCLOSED | -2 |
| 59 | CLANCHANNEL_FULL | -2 |
| 60 | URL_OPEN | -2 |
| 61 | IF_OPENSUB_ACTIVE_PLAYER | 25 |
| 62 | IF_SETPLAYERHEAD_OTHER | 10 |
| 63 | IF_SETRECOL | 9 |
| 64 | CAM_REMOVEROOF | 4 |
| 65 | UPDATE_INV_STOP_TRANSMIT | 3 |
| 66 | CREATE_SUGGEST_NAME_REPLY | -1 |
| 67 | PLAYER_SNAPSHOT | -2 |
| 68 | TELEMETRY_GRID_REMOVE_GROUP | 1 |
| 69 | CLIENT_SETVARCBIT_LARGE | 6 |
| 70 | SOUND_AREA | 6 |
| 71 | MAP_PROJANIM_HALFSQ | 21 |
| 72 | IF_SETPOSITION | 8 |
| 73 | OBJ_COUNT | 7 |
| 74 | CHAT_FILTER_SETTINGS_PRIVATECHAT | 1 |
| 75 | TELEMETRY_GRID_FULL | -2 |
| 76 | SHOW_FACE_HERE | 1 |
| 77 | SOCIAL_NETWORK_LOGOUT | -2 |
| 78 | HINT_ARROW | 14 |
| 79 | IF_SETSCROLLPOS | 6 |
| 80 | MESSAGE_QUICKCHAT_PRIVATE | -1 |
| 81 | CAM2_ENABLE | 1 |
| 82 | MESSAGE_CLANCHANNEL_SYSTEM | -1 |
| 83 | NO_TIMEOUT | 0 |
| 84 | LOC_ANIM_SPECIFIC | 10 |
| 85 | OBJ_ADD | 5 |
| 86 | LOGOUT_FULL | 1 |
| 87 | CAM_SMOOTHRESET | 0 |
| 88 | REBUILD_NORMAL | -2 |
| 89 | MESSAGE_QUICKCHAT_CLANCHANNEL | -1 |
| 90 | MESSAGE_PRIVATE | -2 |
| 91 | VARCLAN_DISABLE | 0 |
| 92 | VORBIS_SOUND_GROUP_START | 2 |
| 93 | MESSAGE_QUICKCHAT_PRIVATE_ECHO | -1 |
| 94 | SET_MAP_FLAG | 2 |
| 95 | VORBIS_SOUND_GROUP_STOP | 2 |
| 96 | TELEMETRY_GRID_MOVE_ROW | 3 |
| 97 | VORBIS_PRELOAD_SOUNDS | 2 |
| 98 | CAM_MOVETO | 6 |
| 99 | RESET_CLIENT_VARCACHE | 0 |
| 100 | IF_SETPLAYERMODEL_OTHER | 10 |
| 101 | CLANSETTINGS_FULL | -2 |
| 102 | IF_OPENSUB_ACTIVE_NPC | 25 |
| 103 | IF_SETANIM | 8 |
| 104 | CAM_RESET | 0 |
| 105 | LOBBY_APPEARANCE | -2 |
| 106 | VARCLAN_ENABLE | 0 |
| 107 | POINTLIGHT_COLOUR | 8 |
| 108 | IF_SETRETEX | 9 |
| 109 | IF_SETHIDE | 5 |
| 110 | LAST_LOGIN_INFO | 4 |
| 111 | REFLECTION_CHECKER | -2 |
| 112 | IF_SETANGLE | 10 |
| 113 | TEXT_COORD | -1 |
| 114 | VORBIS_SPEECH_STOP | 0 |
| 115 | VORBIS_PRELOAD_SOUND_GROUP | 2 |
| 116 | IF_SETMODEL | 8 |
| 117 | DO_CHEAT | -1 |
| 118 | OBJ_REVEAL | 7 |
| 119 | HINT_TRAIL | -2 |
| 120 | MESSAGE_GAME | -1 |
| 121 | IF_OPENSUB_ACTIVE_OBJ | 29 |
| 122 | PLAYER_INFO | -2 |
| 123 | SOUND_MIXBUSS_ADD | 6 |
| 124 | LOC_PREFETCH | 5 |
| 125 | NPC_HEADICON_SPECIFIC | 9 |
| 126 | UPDATE_DOB | 4 |
| 127 | MIDI_SONG_LOCATION | 11 |
| 128 | CLIENT_SETVARC_LARGE | 6 |
| 129 | SERVER_TICK_END | 0 |
| 130 | IF_SETPLAYERHEAD_IGNOREWORN | 10 |
| 131 | CREATE_ACCOUNT_REPLY | 1 |
| 132 | CLANSETTINGS_DELTA | -2 |
| 133 | TRIGGER_ONDIALOGABORT | 0 |
| 134 | REDUCE_PLAYER_ATTACK_PRIORITY | 1 |
| 135 | IF_SET_HTTP_IMAGE | 8 |
| 136 | CUTSCENE | -2 |
| 137 | MINIMAP_TOGGLE | 1 |
| 138 | CHAT_FILTER_SETTINGS | 2 |
| 139 | LOC_ADD_CHANGE | -1 |
| 140 | IF_SETCOLOUR | 6 |
| 141 | REDUCE_NPC_ATTACK_PRIORITY | 1 |
| 142 | VARBIT_LARGE | 6 |
| 143 | VORBIS_SPEECH_SOUND | 6 |
| 144 | POINTLIGHT_INTENSITY | 5 |
| 145 | MESSAGE_QUICKCHAT_FRIENDCHAT | -1 |
| 146 | CLANCHANNEL_DELTA | -2 |
| 147 | LOC_ANIM | 7 |
| 148 | STORE_SERVERPERM_VARCS_ACK | 0 |
| 149 | CLIENT_SETVARC_SMALL | 3 |
| 150 | SET_TARGET | 2 |
| 151 | IF_SETPLAYERMODEL_SELF | 4 |
| 152 | CAMERA_UPDATE | -2 |
| 153 | JS5_RELOAD | 0 |
| 154 | CHANGE_LOBBY | -1 |
| 155 | IF_SETEVENTS | 12 |
| 156 | RUNCLIENTSCRIPT | -2 |
| 157 | VARP_SMALL | 3 |
| 158 | IF_SETOBJECT | 10 |
| 159 | PLAYER_GROUP_VARPS | -2 |
| 160 | TELEMETRY_GRID_REMOVE_ROW | 2 |
| 161 | UPDATE_RUNENERGY | 1 |
| 162 | SOUND_MIXBUSS_SETLEVEL | 4 |
| 163 | CREATE_CHECK_NAME_REPLY | 1 |
| 164 | CAM_FORCEANGLE | 4 |
| 165 | IF_SETTEXTANTIMACRO | 5 |
| 166 | IF_CLOSESUB | 4 |
| 167 | WORLDLIST_FETCH_REPLY | -2 |
| 168 | LOGOUT | 1 |
| 169 | UPDATE_RUNWEIGHT | 2 |
| 170 | serverProt2 | -2 |
| 171 | CAM_SHAKE | 6 |
| 172 | VARCLAN | -1 |
| 173 | TELEMETRY_GRID_SET_ROW_PINNED | 3 |
| 174 | UPDATE_IGNORELIST | -2 |
| 175 | UPDATE_ZONE_FULL_FOLLOWS | 3 |
| 176 | MESSAGE_QUICKCHAT_PLAYER_GROUP | -1 |
| 177 | UPDATE_REBOOT_TIMER | 2 |
| 178 | SPOTANIM_SPECIFIC | 12 |
| 179 | IF_SETTARGETPARAM | 10 |
| 180 | IF_SETPLAYERMODEL_SNAPSHOT | 5 |
| 181 | IF_SETTEXT | -2 |
| 182 | SYNTH_SOUND | 8 |
| 183 | IF_SETGRAPHIC | 8 |
| 184 | IF_SETCLICKMASK | 5 |
| 185 | TELEMETRY_GRID_ADD_ROW | 6 |
| 186 | NPC_INFO | -2 |
| 187 | PLAYER_GROUP_DELTA | -2 |
| 188 | LOYALTY_UPDATE | 4 |
| 189 | IF_SETNPCHEAD | 8 |
| 190 | MIDI_SONG_STOP | 0 |
| 191 | IF_MOVESUB | 8 |
| 192 | TELEMETRY_CLEAR_GRID_VALUE | 3 |
| 193 | MAP_ANIM | 10 |
| 194 | JCOINS_UPDATE | 4 |

## Client to server

`ClientMessage.createMessage` (`ClientMessage.java:34`) writes `pIsaac1(prot.id)`, which stores `(opcode + isaac.nextInt())` as one byte (`PacketBit.java:25`). Fixed sizes have no extra length. Size `-1` writers backpatch one byte with `psize1` (`Packet.java:251`). Size `-2` writers backpatch a `u16be` with `psize2` (`Packet.java:241`). `ServerConnection.queue` records `buf.pos` (`ServerConnection.java:99`). `flush` copies those bytes to the socket (`ServerConnection.java:75`).

| opcode | name | size |
| --- | --- | --- |
| 0 | MAP_BUILD_STUCK | 15 |
| 1 | RESUME_P_HSLDIALOG | 2 |
| 2 | OPNPC5 | 3 |
| 3 | RESUME_P_STRINGDIALOG | -1 |
| 4 | OPPLAYER6 | 3 |
| 5 | EVENT_NATIVE_MOUSE_MOVE | -1 |
| 6 | OPPLAYER9 | 3 |
| 7 | MESSAGE_PRIVATE | -2 |
| 8 | AFFINEDCLANSETTINGS_ADDBANNED_FROMCHANNEL | -1 |
| 9 | CLIENT_DETAILOPTIONS_STATUS | -1 |
| 10 | RESUME_P_COUNTDIALOG | 4 |
| 11 | SEND_SNAPSHOT | -1 |
| 12 | OPLOC1 | 9 |
| 13 | IGNORELIST_ADD | -1 |
| 14 | SOUND_SONGEND | 4 |
| 15 | clientProt | 9 |
| 16 | OPOBJ5 | 7 |
| 17 | MESSAGE_QUICKCHAT_PUBLIC | -1 |
| 18 | CLICKWORLDMAP | 4 |
| 19 | clientProt2 | 1 |
| 20 | CREATE_ACCOUNT | -2 |
| 21 | OPLOCT | 17 |
| 22 | TRANSMITVAR_VERIFYID | 4 |
| 23 | REFLECTION_CHECK_REPLY | -1 |
| 24 | NATIVE_LIBRARY_FAILURE | -1 |
| 25 | OPOBJ2 | 7 |
| 26 | OPOBJ3 | 7 |
| 27 | OPLOC3 | 9 |
| 28 | CREATE_CHECK_EMAIL | -2 |
| 29 | IF_BUTTON4 | 8 |
| 30 | SOUND_SONGPRELOADED | 4 |
| 31 | OPNPC4 | 3 |
| 32 | CLAN_KICKUSER | -1 |
| 33 | MOVE_GAMECLICK | 5 |
| 34 | CHANGE_EMAIL_ADDRESS | -2 |
| 35 | FRIENDLIST_DEL | -1 |
| 36 | OPNPC6 | 3 |
| 37 | EVENT_CAMERA_POSITION | 4 |
| 38 | clientProt3 | -2 |
| 39 | OPLOC6 | 9 |
| 40 | CUTSCENE_FINISHED | 1 |
| 41 | OPPLAYERT | 11 |
| 42 | IGNORE_SETNOTES | -1 |
| 43 | clientProt4 | 1 |
| 44 | CHAT_SETMODE | 1 |
| 45 | EVENT_MOUSE_CLICK | 6 |
| 46 | clientProt5 | -2 |
| 47 | IGNORELIST_DEL | -1 |
| 48 | OPPLAYER8 | 3 |
| 49 | OPPLAYER5 | 3 |
| 50 | IF_BUTTON8 | 8 |
| 51 | OPNPC1 | 3 |
| 52 | AFFINEDCLANSETTINGS_SETMUTED_FROMCHANNEL | -1 |
| 53 | SIMPLE_TOOLKIT_CHANGE | 1 |
| 54 | OPPLAYER7 | 3 |
| 55 | DIRECTX_FAILURE | 2 |
| 56 | OPNPC2 | 3 |
| 57 | CLOSE_MODAL | 0 |
| 58 | IF_BUTTONT | 16 |
| 59 | APCOORDT | 12 |
| 60 | SEND_EMAIL_VALIDATION_CODE | -1 |
| 61 | OPLOC4 | 9 |
| 62 | OPPLAYER2 | 3 |
| 63 | IF_BUTTON7 | 8 |
| 64 | ADD_NEW_EMAIL_ADDRESS | -2 |
| 65 | CREATE_CHECK_NAME | -1 |
| 66 | IF_BUTTON9 | 8 |
| 67 | IF_BUTTON10 | 8 |
| 68 | IF_BUTTON2 | 8 |
| 69 | CREATE_SUGGEST_NAMES | 0 |
| 70 | IF_BUTTON5 | 8 |
| 71 | STORE_SERVERPERM_VARCS | -2 |
| 72 | EVENT_NATIVE_MOUSE_CLICK | 7 |
| 73 | FRIEND_SETRANK | -1 |
| 74 | PING_STATISTICS | 4 |
| 75 | FRIEND_SETNOTES | -1 |
| 76 | OPPLAYER3 | 3 |
| 77 | WORLDLIST_FETCH | 4 |
| 78 | MOVE_MINIMAPCLICK | 18 |
| 79 | MAP_BUILD_COMPLETE | 4 |
| 80 | TELEMETRY_ERROR | 0 |
| 81 | OPOBJ1 | 7 |
| 82 | UID_PASSPORT_RESEND_REQUEST | 0 |
| 83 | IF_BUTTON6 | 8 |
| 84 | CLIENT_CHEAT | -1 |
| 85 | FRIENDLIST_ADD | -1 |
| 86 | IF_BUTTON3 | 8 |
| 87 | EVENT_KEYBOARD | -2 |
| 88 | SET_CHATFILTERSETTINGS | 3 |
| 89 | MOVE_SCRIPTED | 5 |
| 90 | DETECT_MODIFIED_CLIENT | 4 |
| 91 | OPPLAYER10 | 3 |
| 92 | CLAN_JOINCHAT_LEAVECHAT | -1 |
| 93 | EVENT_APPLET_FOCUS | 1 |
| 94 | CLANCHANNEL_KICKUSER | -1 |
| 95 | MESSAGE_PUBLIC | -1 |
| 96 | OPLOC2 | 9 |
| 97 | EVENT_MOUSE_MOVE | -1 |
| 98 | RESUME_PAUSEBUTTON | 6 |
| 99 | RESUME_P_OBJDIALOG | 2 |
| 100 | SEND_PING_REPLY | 9 |
| 101 | RESUME_P_NAMEDIALOG | -1 |
| 102 | OPPLAYER4 | 3 |
| 103 | NO_TIMEOUT | 0 |
| 104 | IF_PLAYER | -1 |
| 105 | OPOBJ6 | 7 |
| 106 | ABORT_P_DIALOG | 0 |
| 107 | FACE_SQUARE | 4 |
| 108 | BUG_REPORT | -2 |
| 109 | CLIENT_COMPRESSEDTEXTUREFORMAT_SUPPORT | -2 |
| 110 | RESUME_P_CLANFORUMQFCDIALOG | -1 |
| 111 | IF_BUTTON1 | 8 |
| 112 | AUTO_SETUP_RESULT | 18 |
| 113 | OPNPCT | 11 |
| 114 | OPOBJT | 15 |
| 115 | MESSAGE_QUICKCHAT_PRIVATE | -1 |
| 116 | OPNPC3 | 3 |
| 117 | OPPLAYER1 | 3 |
| 118 | CREATE_LOG_PROGRESS | 1 |
| 119 | OPLOC5 | 9 |
| 120 | URL_REQUEST | -2 |
| 121 | OPOBJ4 | 7 |
| 122 | IF_BUTTOND | 16 |
| 123 | WINDOW_STATUS | 6 |

## Login opcodes

Login writes happen before ISAAC is installed. They are raw `p1` opcodes on `ClientMessage.createMessage()` with no `pIsaac1` (`LoginManager.java:361`).

| opcode | name | constructor size | stored |
| --- | --- | --- | --- |
| 14 | INIT_GAME_CONNECTION | 0 | no |
| 15 | INIT_JS5REMOTE_CONNECTION | -1 | no |
| 16 | GAMELOGIN | -2 | no |
| 19 | LOBBYLOGIN | -2 | no |
| 23 | REQUEST_WORLDLIST | 4 | no |
| 24 | CHECK_WORLD_SUITABILITY | -1 | no |
| 26 | GAMELOGIN_CONTINUE | 0 | no |
| 27 | SSL_WEBCONNECTION | 0 | no |
| 28 | CREATE_ACCOUNT_CONNECT | -2 | no |
| 29 | INIT_SOCIAL_NETWORK_CONNECTION | -2 | no |
| 30 | SOCIAL_NETWORK_LOGIN | -2 | no |
| 31 | INIT_DEBUG_CONNECTION | 4 | no |

## Byte orders

`p1`/`g1` are unsigned bytes. `g1b` is a signed byte. `p2`/`g2` are unsigned big-endian shorts. `g2s` is a signed big-endian short. `p3`/`g3` are unsigned big-endian 24-bit. `g3s` is signed. `p4`/`g4s` are big-endian 32-bit. `ip2` and `g2_alt1`/`p2_alt1` are little-endian shorts. `ip4`/`ig4s`/`p4_alt1`/`g4_alt1` are little-endian 32-bit. `g5` is one unsigned byte then one big-endian unsigned 32-bit value, joined as a 40-bit integer (`Packet.java:375`). `g6` is `g2` then `g4s`. `g8` is two `g4s` values. `pjstr` is Cp1252 plus a NUL (`Packet.java:192`). `pjstr2` writes a leading 0 then that string (`Packet.java:208`). `gUTF8` reads a 0, a varint length, then UTF-8 (`Packet.java:455`).

`tinyenc` and `tinydec` are 32-round XTEA with delta `-1640531527` (`Packet.java:553` and `Packet.java:576`). Login calls the ranged overload `tinyenc(outKey, start, end)` (`LoginManager.java:594`).

Bit access is MSB-first inside each byte. `accessBits` sets `bitPos = pos*8` (`PacketBit.java:55`). `gBit` uses `BITMASK` (`PacketBit.java:60`). `accessBytes` sets `pos = (bitPos+7)/8` (`PacketBit.java:81`).
