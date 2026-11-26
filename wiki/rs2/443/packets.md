# Packets

S2C sizes are `Client.valueClient1464` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:16`). Those 256 values match `rslib/src/server/packets/443.c`. Field lists below are from this client's handlers. An opcode with no branch logs `T1` or `T2` and logs out (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1500`). Those unused slots are not rows.

C2S has no size array. The size column is `none`. Width names are the get and put table in [framing.md](framing.md). A row with an empty field list was not read. See [gaps.md](gaps.md).

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 205 | 2 | u16le; 65535 becomes -1 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:219 |
| s2c | 155 | 5 | u24mix; u16beA; 65535 becomes -1 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:230 |
| s2c | 156 | 4 | u8; u8; u8; u8 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:242 |
| s2c | 148 | 1 | u8c | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:256 |
| s2c | 192 | -2 | jstr format; for each char from the end, u32be when the char is not 's' (115) else jstr; then u32be script id | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:272 |
| s2c | 111 | 6 | u8; u8; u16be; u8; u8 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:291 |
| s2c | 41 | 0 | empty; closes the game socket and sets login state 10 | NpcType.opNpcType12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:307 |
| s2c | 137 | 10 | u32be; u32be; u16be; 65535 becomes -1 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:313 |
| s2c | 3 | 6 | u32mix49; u16le | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:344 |
| s2c | 204 | -2 | u8 count; u32be id; then per entry u8 type. Types 0, 1, and 2: jstr, jstr, and type 1 also u32be. Types 3 and 4: jstr, jstr, u8 count, that many jstr, and type 3 also per slot u32be length plus bytes(length) | VorbisSound.opVorbisSound4 | builds/rs2/443-2006-12-11/src/src/main/java/deob/VorbisSound.java:104 |
| s2c | 77 | -1 | u64be; smartu decoded length; huffman bits, high bit first, walked by ServerActiveProperties.opServerActiveProperties5 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:362 |
| s2c | 58 | 6 | u8; u8a; u32mix32 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:371 |
| s2c | 147 | 10 | u32mix49; u16le; u16be; u16leA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:390 |
| s2c | 159 | -2 | count is size/8 u64be names | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:406 |
| s2c | 74 | 6 | u32mix32; u16be | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:414 |
| s2c | 29 | -2 | see sync.md | ReferenceNodeFactory.opReferenceNodeFactory3 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:433 |
| s2c | 221 | 4 | u16be; u16leA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:439 |
| s2c | 228 | -2 | u32be; u16be; u16be count; then count times (u8c, or u32be when that byte is 255, then u16be) | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:486 |
| s2c | 57 | 1 | u8 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:526 |
| s2c | 109 | 5 | u8 packed tile; u16be; u8; u8 | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:276 |
| s2c | 79 | 7 | u8 packed tile; u16be; u16be; u16be | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:13 |
| s2c | 129 | 14 | i8neg; i8s; u16beA; i8s; i8a; u16beA; u8c; u8s; u16be; u16leA | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:211 |
| s2c | 94 | 7 | u16beA; u16beA; u8c; u16beA | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:34 |
| s2c | 115 | 6 | u8 packed tile; u16be; u8; u16be | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:196 |
| s2c | 101 | 15 | u8 packed tile; i8; i8; i16be; u16be; u8; u8; u16be; u16be; u8; u8 | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:88 |
| s2c | 84 | 3 | u16le; u8 packed tile | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:53 |
| s2c | 207 | 5 | u16beA; u16le; u8 packed tile | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:112 |
| s2c | 170 | 4 | u16leA; u8a; u8s | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:130 |
| s2c | 69 | 2 | u8; u8a packed tile | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:76 |
| s2c | 122 | 4 | u8c packed tile; u16be; u8a | DerServerWirdGeradeAktualisiert.opkind1511 | builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:183 |
| s2c | 146 | 4 | u16leA; u16be | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:539 |
| s2c | 154 | 4 | u32le | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:587 |
| s2c | 179 | 10 | u64be; u16be | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:594 |
| s2c | 87 | 1 | u8 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:651 |
| s2c | 72 | 2 | i16be | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:661 |
| s2c | 140 | 2 | u16beA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:667 |
| s2c | 104 | 0 | empty | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:674 |
| s2c | 73 | 8 | u32mix49; u16leA; u16be | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:692 |
| s2c | 88 | 6 | u32mix49; u16beA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:702 |
| s2c | 219 | 2 | i16beA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:722 |
| s2c | 149 | 2 | u16beA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:738 |
| s2c | 160 | 2 | u16beA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:783 |
| s2c | 121 | -2 | static rebuild, see region.md | Timer.opTimer4 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:828 |
| s2c | 238 | -2 | see sync.md | SpielerBitteHlenSieEineAndereWelt.opkind1532 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:834 |
| s2c | 62 | 3 | i8neg; u16be | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:840 |
| s2c | 242 | 0 | empty | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:859 |
| s2c | 230 | 2 | u16leA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:870 |
| s2c | 244 | -2 | u8c zone base x; u8 zone base z; then while bytes remain, u8 raw sub-opcode and the matching zone layout. Sub-opcodes are not ISAAC masked | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:876 |
| s2c | 130 | -1 | u8; jstr; u8a | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:889 |
| s2c | 232 | 8 | i16beA; i16be; u32mix32 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:906 |
| s2c | 199 | 3 | u8s; u8c; u8c | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:917 |
| s2c | 163 | 0 | empty | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:928 |
| s2c | 117 | 2 | u16beA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:946 |
| s2c | 96 | 6 | u8 type. Type 1 reads one u16be. Types 2 through 6 read u16be, u16be, u8. Type 10 reads one u16be. The table length is 6; bytes the branch does not read stay in the already-consumed payload | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:952 |
| s2c | 6 | 2 | i16le | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:998 |
| s2c | 90 | 3 | u8; u16beA; 65535 becomes -1 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1011 |
| s2c | 15 | 0 | empty | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1032 |
| s2c | 245 | 4 | u32mix32 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1038 |
| s2c | 10 | 1 | u8 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1051 |
| s2c | 180 | -2 | u32le; jstr | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1059 |
| s2c | 234 | 5 | u8a; u32mix32 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1072 |
| s2c | 25 | -1 | u64be; u16be; u24be; u8; smartu decoded length and huffman bits only when the message is not a duplicate and WenOchtenSieHinzufUgen.valuekind1801 is 0. A skipped read leaves those bytes in the already-consumed payload | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1081 |
| s2c | 97 | 2 | u16leA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1124 |
| s2c | 213 | -2 | u32be; u16be; then while bytes remain: smartu slot, u16be item, and if the item is not 0 a u8, or a u32be when that u8 is 255 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1165 |
| s2c | 82 | 2 | u8s; u8s | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1200 |
| s2c | 95 | 6 | u32le; i16leA | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1207 |
| s2c | 157 | -1 | jstr | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1221 |
| s2c | 193 | -2 | dynamic rebuild, see region.md | Timer.opTimer4 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1279 |
| s2c | 24 | 6 | u16le; u32mix49 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1285 |
| s2c | 241 | 6 | u8; u8; u16be; u8; u8 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1296 |
| s2c | 91 | 3 | u8; u8; u8 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1325 |
| s2c | 133 | 2 | u8a; u8s | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1335 |
| s2c | 226 | 1 | u8 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1362 |
| s2c | 32 | 0 | empty | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1368 |
| s2c | 81 | 5 | u16be; u8; u16be | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1382 |
| s2c | 236 | 1 | u8 | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1391 |
| s2c | 178 | 0 | empty | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1396 |
| s2c | 22 | 10 | u16leA; 65535 becomes -1; u32mix32; u32le | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1437 |
| s2c | 220 | 0 | empty | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1453 |
| s2c | 227 | 6 | u16be; u32be | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1468 |
| s2c | 31 | 4 | u32le | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1478 |
| s2c | 51 | 0 | empty | PathingEntity.opPathingEntity12 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1486 |
| c2s | 0 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:169 |
| c2s | 3 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:49 |
| c2s | 4 | none | u8 length placeholder 0; u8 chat type; u8 effects; smart of the decoded length; huffman bytes from ServerActiveProperties.callb37bcaf7; u8 length backpatched | MonotonicClock.opMonotonicClock34 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:426 |
| c2s | 8 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:85 |
| c2s | 11 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:584. Also line 932 |
| c2s | 13 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:31 |
| c2s | 21 | none |  | MaterialManager_Class106_Sub1.opMaterialManager_Class106_Sub111 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MaterialManager_Class106_Sub1.java:451 |
| c2s | 22 | none | u64be | MonotonicClock.opMonotonicClock34 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:304 |
| c2s | 28 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:79 |
| c2s | 29 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:616 |
| c2s | 36 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:887 |
| c2s | 38 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:338 |
| c2s | 40 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:55 |
| c2s | 46 | none |  | SoundPcmStream.opSoundPcmStream1 | builds/rs2/443-2006-12-11/src/src/main/java/deob/SoundPcmStream.java:126 |
| c2s | 47 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:672 |
| c2s | 48 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:864 |
| c2s | 50 | none | u8 length placeholder 0; u64be recipient; smart of the decoded length; huffman bytes from ServerActiveProperties.callb37bcaf7; u8 length backpatched | MonotonicClock.opMonotonicClock34 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:483 |
| c2s | 54 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:535. Also lines 725 and 739 |
| c2s | 57 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:67 |
| c2s | 64 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:607 |
| c2s | 65 | none | u8 | MonotonicClock.opMonotonicClock43 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:673 |
| c2s | 66 | none | u16beA; u16be | Cc317a406.opkind1793 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:306 |
| c2s | 69 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:346 |
| c2s | 70 | none |  | QuickChatCatType.opQuickChatCatType6 | builds/rs2/443-2006-12-11/src/src/main/java/deob/QuickChatCatType.java:75 |
| c2s | 73 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:61 |
| c2s | 74 | none | u32be | MonotonicClock.opMonotonicClock34 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:284 |
| c2s | 76 | none | u8; u8; u8 | MonotonicClock.opMonotonicClock34 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:436. Same three bytes at MonotonicClock.java:492 and TextureOp38.java:35 |
| c2s | 78 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:99 |
| c2s | 80 | none | u8 length of 3+2*n, n capped at 25 including the destination; u16leA destination scene X plus base X; u8neg 1 when key 82 is down, else 0; u16le destination scene Z plus base Z; then each earlier waypoint, destination backward: u8 sceneX minus destination X, u8s sceneZ minus destination Z; then u8 click X offset; u8 click Y offset; u16be camera yaw; u8 57; u8 Cd2890cde.valuekind1942; u8 zoom; u8 89; u16be player fine X; u16be player fine Z; u8 EmpfNger.valuekind1388; u8 63 | Keyboard.opKeyboard5 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Keyboard.java:328. The 14 trailing bytes are written at Cb5815139.java:366 |
| c2s | 81 | none | u8 length of 3+2*n, n capped at 25 including the destination; u16leA destination scene X plus base X; u8neg 1 when key 82 is down, else 0; u16le destination scene Z plus base Z; then each earlier waypoint, destination backward: u8 sceneX minus destination X, u8s sceneZ minus destination Z | Keyboard.opKeyboard5 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Keyboard.java:333 |
| c2s | 85 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:485 |
| c2s | 86 | none | empty | Js5CacheRequest.opJs5CacheRequest5 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Js5CacheRequest.java:139. Also written with no payload at Cc317a406.java:623 |
| c2s | 87 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:686 |
| c2s | 89 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:699 |
| c2s | 90 | none | u64be | MidiNote.opMidiNote1 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MidiNote.java:40 |
| c2s | 99 | none | u8 length of 3+2*n, n capped at 25 including the destination; u16leA destination scene X plus base X; u8neg 1 when key 82 is down, else 0; u16le destination scene Z plus base Z; then each earlier waypoint, destination backward: u8 sceneX minus destination X, u8s sceneZ minus destination Z | Keyboard.opKeyboard5 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Keyboard.java:323 |
| c2s | 101 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:579. Also line 712 |
| c2s | 104 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:375 |
| c2s | 113 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:788 |
| c2s | 114 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:282 |
| c2s | 118 | none |  | BitteVersuchenSieEs.opkind1971 | builds/rs2/443-2006-12-11/src/src/main/java/deob/BitteVersuchenSieEs.java:87 |
| c2s | 119 | none |  | BitteVersuchenSieEs.opkind1971 | builds/rs2/443-2006-12-11/src/src/main/java/deob/BitteVersuchenSieEs.java:98 |
| c2s | 120 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:92 |
| c2s | 133 | none | u8 length placeholder 0; while the added bytes stay below 240, one sample. Idle below 8 and both deltas in -32..31: u16be of (idle<<12) + ((dx+32)<<6) + (dy+32). Idle below 8 otherwise: u24be of 0x800000 + (idle<<19) + packed. Else u32be of the Java int 0xC0000000 + (idle<<19) + packed. packed is x+y*765, x clamped 0..764 and y 0..502, or 524287 when both coordinates were -1. Length backpatched. Sent only when ServerActiveProperties.valueServerActiveProperties21 is set | Cc317a406.opkind1793 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:194 |
| c2s | 136 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:828 |
| c2s | 141 | none |  | MaterialManager_Class106_Sub1.opMaterialManager_Class106_Sub111 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MaterialManager_Class106_Sub1.java:424 |
| c2s | 144 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:187 |
| c2s | 145 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:839 |
| c2s | 146 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:251 |
| c2s | 147 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:649 |
| c2s | 149 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:123 |
| c2s | 153 | none | u16le; u32be | PathingEntity.opPathingEntity10 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:150 |
| c2s | 154 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:238 |
| c2s | 158 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:559 |
| c2s | 159 | none |  | PleaseTryAgain.opkind1621 | builds/rs2/443-2006-12-11/src/src/main/java/deob/PleaseTryAgain.java:35 |
| c2s | 161 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:43 |
| c2s | 162 | none | u32mix47 of (deltaTime<<20)+(button<<19)+(x+y*765). Button is 1 when the click state is 2, else 0. Time is clamped to 4095. x is clamped 0..764 and y 0..502 | Cc317a406.opkind1793 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:291 |
| c2s | 167 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:73 |
| c2s | 169 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:782 |
| c2s | 171 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:292 |
| c2s | 174 | none | u8 of length minus 1; jstr of the text from index 2. Written only when the text starts with WirVermutenDassJemandIhrPasswortKennt.valuekind15710; the other branch is opcode 4 | MonotonicClock.opMonotonicClock34 | builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:357 |
| c2s | 178 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:443 |
| c2s | 182 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:73 |
| c2s | 188 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:310 |
| c2s | 190 | none | u16be; u32be; u8; u16be | Cc317a406.opkind1793 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:413 |
| c2s | 192 | none | empty | Cc317a406.opkind1793 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:548 |
| c2s | 194 | none | u8 length placeholder 0; u32be id; per entry, if a status is already stored, u8 status. Otherwise type 0 writes u8 0 and u32be of the field value; type 1 writes u8 0; type 2 writes u8 0 and u32be modifiers; type 3 writes u8 0, or u8 1 and u64be, or u8 2 and jstr, or u8 4; type 4 writes u8 0 and u32be modifiers. Failures write one u8 from -10 through -21. opMonotonicClock11 then writes a u32be CRC, and the backpatched u8 length includes that CRC | Cda06986b.opkind16110 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cda06986b.java:301 |
| c2s | 195 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:384 |
| c2s | 198 | none |  | SpielFensterGeladen.opkind1475 | builds/rs2/443-2006-12-11/src/src/main/java/deob/SpielFensterGeladen.java:148 |
| c2s | 200 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:360 |
| c2s | 202 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:436 |
| c2s | 206 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:503 |
| c2s | 207 | none | u8, 1 or 0 | Cc317a406.opkind1793 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:313 |
| c2s | 217 | none | u32be; u16be | Cc317a406.opkind1791 | builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:37 |
| c2s | 219 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:398. Also line 757 |
| c2s | 221 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:417 |
| c2s | 222 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:267 |
| c2s | 224 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:769 |
| c2s | 229 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:919 |
| c2s | 236 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:641 |
| c2s | 243 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:892 |
| c2s | 245 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:732 |
| c2s | 250 | none |  | ShapedTile.opShapedTile1 | builds/rs2/443-2006-12-11/src/src/main/java/deob/ShapedTile.java:69 |
| c2s | 252 | none |  | LadeFreundeListe.opkind1235 | builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:162 |
