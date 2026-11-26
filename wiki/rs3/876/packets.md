# Packets

Game opcodes come from the constructors in `deob/DataBlock1675` (client to server) and `deob/DataBlock1193` (server to client). `tint` is the opcode and `tint2` is the size. Sizes `-1` and `-2` are the variable-length markers read in `Statics.doStatics3` (`deob/Statics.java:4118` and `:4129`). There is no constructor above opcode 122 client to server or opcode 192 server to client, and this client does not use a `-3` size.

The opcode byte is not part of `size`. Client to server, `PositionedSoundPart4.doPositionedSoundPart4(int)` writes `opcode + isaacNext` (`deob/PositionedSoundPart4.java:25`), called from `DynamicGroundDecorEntity.doDynamicGroundDecorEntity` (`com/jagex/game/world/entity/DynamicGroundDecorEntity.java:47`). Server to client, the opcode is removed with the ISAAC cipher before the payload in `Statics.doStatics3` (`deob/Statics.java:4097`).

Width names are defined in [framing.md](framing.md). An empty field list means that handler body was not read. Login-stream opcodes are a separate table: their numeric values overlap these game opcodes and they are written before the ISAAC cipher is installed.

## Client to server

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | 7 | u16le+128 index, u16be (baseX+x), u8 ((booleanArg?2:0)\|(Friend.doFriend()?1:0)), u16be (baseZ+z). Menu action 1004. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2139 |
| c2s | 1 | 15 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:8 |
| c2s | 2 | 1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:10 |
| c2s | 3 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:12 |
| c2s | 4 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:14 |
| c2s | 5 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:16 |
| c2s | 6 | 0 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:18 |
| c2s | 7 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 8 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:22 |
| c2s | 9 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:24 |
| c2s | 10 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:26 |
| c2s | 11 | 5 | u16be (baseX+localX), u16le (baseZ+localZ), u8sub128 (128-(Friend.doFriend()?1:0)) | Friend.doFriend(int,int,int) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2356 |
| c2s | 12 | 16 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:30 |
| c2s | 13 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:32 |
| c2s | 14 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:34 |
| c2s | 15 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:36 |
| c2s | 16 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:38 |
| c2s | 17 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 18 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:42 |
| c2s | 19 | 1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:44 |
| c2s | 20 | 6 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:46 |
| c2s | 21 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:48 |
| c2s | 22 | 0 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:50 |
| c2s | 23 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:52 |
| c2s | 24 | 7 | u16le+128 index, u16be (baseX+x), u8 ((booleanArg?2:0)\|(Friend.doFriend()?1:0)), u16be (baseZ+z). Menu action 18. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2129 |
| c2s | 25 | 1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:56 |
| c2s | 26 | 7 | u16le+128 index, u16be (baseX+x), u8 ((booleanArg?2:0)\|(Friend.doFriend()?1:0)), u16be (baseZ+z). Menu action 20. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2133 |
| c2s | 27 | 7 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:60 |
| c2s | 28 | 7 | u16le+128 index, u16be (baseX+x), u8 ((booleanArg?2:0)\|(Friend.doFriend()?1:0)), u16be (baseZ+z). Menu action 22. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2137 |
| c2s | 29 | 9 | u8 (Friend.doFriend()?1:0), u16be+128 (baseZ+z), u32m-32222222b3 ((objLong>>>32)&0x7fffffff), u16le+128 (baseX+x) | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2326 |
| c2s | 30 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:66 |
| c2s | 31 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:68 |
| c2s | 32 | 11 | u32m-32222222b3 Statics.tint51, u8 (Friend.doFriend()?1:0), u16be+128 Client.tint86, u16be+128 target, u16le Client.tint85 | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2296 |
| c2s | 33 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:72 |
| c2s | 34 | 2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:74 |
| c2s | 35 | 2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:76 |
| c2s | 36 | 9 | u8 (Friend.doFriend()?1:0), u16be+128 (baseZ+z), u32m-32222222b3 ((objLong>>>32)&0x7fffffff), u16le+128 (baseX+x) | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2326 |
| c2s | 37 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:80 |
| c2s | 38 | 3 | u8neg (0-(Friend.doFriend()?1:0)), u16le npcIndex. Menu action 1003. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2093 |
| c2s | 39 | 9 | u8 (Friend.doFriend()?1:0), u16be+128 (baseZ+z), u32m-32222222b3 ((objLong>>>32)&0x7fffffff), u16le+128 (baseX+x) | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2326 |
| c2s | 40 | 9 | u8 (Friend.doFriend()?1:0), u16be+128 (baseZ+z), u32m-32222222b3 ((objLong>>>32)&0x7fffffff), u16le+128 (baseX+x) | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2326 |
| c2s | 41 | 9 | u8 (Friend.doFriend()?1:0), u16be+128 (baseZ+z), u32m-32222222b3 ((objLong>>>32)&0x7fffffff), u16le+128 (baseX+x) | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2326 |
| c2s | 42 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:90 |
| c2s | 43 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 44 | 18 | u16be (baseX+localX), u16le (baseZ+localZ), u8sub128 (128-(Friend.doFriend()?1:0)); then when the third argument is 1: u8 255, u8 255, u16be (int)Client.tfloat2, u8 57, u8 Client.tint44, u8 Client.tint46, u8 89, u16be (int)local.tfloat2, u16be (int)local.tfloat3, u8 63 | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2217 |
| c2s | 45 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:96 |
| c2s | 46 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:98 |
| c2s | 47 | 12 | u16le+128 Client.tint86, u16be+128 Client.tint85, u16le+128 (baseX+x), u32be Statics.tint51, u16be+128 (baseZ+z). Menu action 59. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2117 |
| c2s | 48 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:102 |
| c2s | 49 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 50 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:106 |
| c2s | 51 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:108 |
| c2s | 52 | 3 | u8neg (0-(Friend.doFriend()?1:0)), u16le npcIndex. Menu action 11. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2087 |
| c2s | 53 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:112 |
| c2s | 54 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 55 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:116 |
| c2s | 56 | 0 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:118 |
| c2s | 57 | 0 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:120 |
| c2s | 58 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:122 |
| c2s | 59 | 3 | u8neg (0-(Friend.doFriend()?1:0)), u16le npcIndex. Menu action 9. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2083 |
| c2s | 60 | 1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:126 |
| c2s | 61 | 3 | u8neg (0-(Friend.doFriend()?1:0)), u16le npcIndex. Menu action 12. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2089 |
| c2s | 62 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:130 |
| c2s | 63 | 4 | u16le+128 (baseZ+z), u16be+128 (baseX+x) | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2164 |
| c2s | 64 | 15 | u16be+128 target, u16be (baseX+x), u8neg (0-(Friend.doFriend()?1:0)), u16be+128 Client.tint85, u16le Client.tint86, u16le+128 (baseZ+z), u32m-32222222b3 Statics.tint51 | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2176 |
| c2s | 65 | 3 | u8neg (0-(Friend.doFriend()?1:0)), u16le npcIndex. Menu action 10. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2085 |
| c2s | 66 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:138 |
| c2s | 67 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 68 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 69 | 9 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:144 |
| c2s | 70 | 7 | u16le+128 index, u16be (baseX+x), u8 ((booleanArg?2:0)\|(Friend.doFriend()?1:0)), u16be (baseZ+z). Menu action 19. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2131 |
| c2s | 71 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:148 |
| c2s | 72 | 8 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:150 |
| c2s | 73 | 6 | u16le (mouseCode \| (button==2?1:0)<<15), u32m-32222222 (y \| x<<16) | NativeMouse.doNativeMouse | builds/rs3/876-2016-11-28/src/src/main/java/com/jagex/game/client/NativeMouse.java:42 |
| c2s | 74 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:154 |
| c2s | 75 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:156 |
| c2s | 76 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:158 |
| c2s | 77 | 1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:160 |
| c2s | 78 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 79 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:164 |
| c2s | 80 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:166 |
| c2s | 81 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:168 |
| c2s | 82 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:170 |
| c2s | 83 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:172 |
| c2s | 84 | 3 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:174 |
| c2s | 85 | 11 | u32be Statics.tint51, u16le Client.tint85, u16be index, u16be Client.tint86, u8 (Friend.doFriend()?1:0) | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2046 |
| c2s | 86 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:178 |
| c2s | 87 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:180 |
| c2s | 88 | 3 | u8neg (0-(Friend.doFriend()?1:0)), u16le npcIndex. Menu action 13. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2091 |
| c2s | 89 | 5 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:184 |
| c2s | 90 | 9 | u8 (Friend.doFriend()?1:0), u16be+128 (baseZ+z), u32m-32222222b3 ((objLong>>>32)&0x7fffffff), u16le+128 (baseX+x) | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2326 |
| c2s | 91 | 2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:188 |
| c2s | 92 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:190 |
| c2s | 93 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:192 |
| c2s | 94 | 7 | u16le+128 index, u16be (baseX+x), u8 ((booleanArg?2:0)\|(Friend.doFriend()?1:0)), u16be (baseZ+z). Menu action 21. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2135 |
| c2s | 95 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:196 |
| c2s | 96 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:198 |
| c2s | 97 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:200 |
| c2s | 98 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:202 |
| c2s | 99 | 9 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:204 |
| c2s | 100 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 101 | 3 | u8sub128 (128-(Friend.doFriend()?1:0)), u16le playerIndex | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2272 |
| c2s | 102 | 16 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:210 |
| c2s | 103 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:212 |
| c2s | 104 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:214 |
| c2s | 105 | -1 | u8 0, bytes written by AbstractSocket.doAbstractSocket (unread), u8 length patched over that leading 0 | Client.doClient3222b122 | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/Client.java:3214 |
| c2s | 106 | 0 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:218 |
| c2s | 107 | 17 | u16be+128 Client.tint86, u32be Statics.tint51, u8sub128 (128-(Friend.doFriend()?1:0)), u32be ((menuLong>>>32)&0x7fffffff), u16le (baseX+x), u16be Client.tint85, u16be (baseZ+z). Menu action 2. | Friend.doFriend(DataBlock1140,int,int,boolean) | builds/rs3/876-2016-11-28/src/src/main/java/rs2/client/logic/friendchat/Friend.java:2069 |
| c2s | 108 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:222 |
| c2s | 109 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:224 |
| c2s | 110 | 18 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:226 |
| c2s | 111 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:228 |
| c2s | 112 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:230 |
| c2s | 113 | 6 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:232 |
| c2s | 114 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:234 |
| c2s | 115 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:236 |
| c2s | 116 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:238 |
| c2s | 117 | 1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:240 |
| c2s | 118 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:242 |
| c2s | 119 | 0 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:244 |
| c2s | 120 | -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:246 |
| c2s | 121 | -2 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:248 |
| c2s | 122 | 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1675.java:250 |

## Server to client

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 0 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6853 |
| s2c | 1 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4926 |
| s2c | 2 | 10 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4513 |
| s2c | 3 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4966 |
| s2c | 4 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6584 |
| s2c | 5 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6574 |
| s2c | 6 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4974 |
| s2c | 7 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5029 |
| s2c | 8 | 23 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6879 |
| s2c | 9 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5810 |
| s2c | 10 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6221 |
| s2c | 11 | 8 | u16be (65535 means -1), u8, u16be, u8, u16be | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5320 |
| s2c | 12 | 5 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6237 |
| s2c | 13 | 25 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5113 |
| s2c | 14 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4608 |
| s2c | 15 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4534 |
| s2c | 16 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5532 |
| s2c | 17 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5751 |
| s2c | 18 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6353 |
| s2c | 19 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6830 |
| s2c | 20 | 11 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6551 |
| s2c | 21 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6767 |
| s2c | 22 | 28 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6397 |
| s2c | 23 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5654 |
| s2c | 24 | 7 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4941 |
| s2c | 25 | 2 | i16be | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4180 |
| s2c | 26 | 5 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4555 |
| s2c | 27 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6015 |
| s2c | 28 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6119 |
| s2c | 29 | 1 | u8 | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4185 |
| s2c | 30 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4989 |
| s2c | 31 | 3 | u8neg, u16be (65535 means -1) | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6922 |
| s2c | 32 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4843 |
| s2c | 33 | 10 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5098 |
| s2c | 34 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5133 |
| s2c | 35 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6612 |
| s2c | 36 | 32 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6330 |
| s2c | 37 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6455 |
| s2c | 38 | 2 | u16be | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4350 |
| s2c | 39 | -2 | see sync.md | Client.doClient3222b | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6315 |
| s2c | 40 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5644 |
| s2c | 41 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5076 |
| s2c | 42 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6835 |
| s2c | 43 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5952 |
| s2c | 44 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4879 |
| s2c | 45 | -2 | jstr descriptor; for each char from the end, s means jstr else u32be; then u32be script id | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4327 |
| s2c | 46 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5063 |
| s2c | 47 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6319 |
| s2c | 48 | 4 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6251 |
| s2c | 49 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4510 |
| s2c | 50 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5019 |
| s2c | 51 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5039 |
| s2c | 52 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5465 |
| s2c | 53 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6029 |
| s2c | 54 | -2 | see sync.md | DeveloperConsole.doDeveloperConsole | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4323 |
| s2c | 55 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4978 |
| s2c | 56 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5008 |
| s2c | 57 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6044 |
| s2c | 58 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5348 |
| s2c | 59 | 1 | u8sub128 | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5333 |
| s2c | 60 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6227 |
| s2c | 61 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4224 |
| s2c | 62 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6805 |
| s2c | 63 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4204 |
| s2c | 64 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6405 |
| s2c | 65 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4663 |
| s2c | 66 | -2 | u16be zoneZ, u8 mapSizeId, u16le zoneX, u8add128 force, u8add128 mapCount, u8add128 npcViewBits | World.doWorld | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5382 |
| s2c | 67 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4313 |
| s2c | 68 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5171 |
| s2c | 69 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5394 |
| s2c | 70 | 3 | u8, u16le-sub128 (65535 means -1) | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6941 |
| s2c | 71 | 4 | u32m-3322b6 stored in Statics.tint87 | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5505 |
| s2c | 72 | 4 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5536 |
| s2c | 73 | 19 | u8sub128, u32be, u32le, u32m-3322b6, u16be Client.tint87, u32m-32222222b16 | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4429 |
| s2c | 74 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4220 |
| s2c | 75 | 2 | u16be | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6960 |
| s2c | 76 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6375 |
| s2c | 77 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5572 |
| s2c | 78 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6727 |
| s2c | 79 | 4 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4477 |
| s2c | 80 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6529 |
| s2c | 81 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4838 |
| s2c | 82 | 12 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4674 |
| s2c | 83 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5165 |
| s2c | 84 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4945 |
| s2c | 85 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5140 |
| s2c | 86 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5844 |
| s2c | 87 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4548 |
| s2c | 88 | 19 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4413 |
| s2c | 89 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5108 |
| s2c | 90 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4355 |
| s2c | 91 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6139 |
| s2c | 92 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6592 |
| s2c | 93 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4269 |
| s2c | 94 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4190 |
| s2c | 95 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6358 |
| s2c | 96 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5840 |
| s2c | 97 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4669 |
| s2c | 98 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5451 |
| s2c | 99 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4483 |
| s2c | 100 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6868 |
| s2c | 101 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6671 |
| s2c | 102 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5445 |
| s2c | 103 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5015 |
| s2c | 104 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6544 |
| s2c | 105 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5797 |
| s2c | 106 | 7 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6311 |
| s2c | 107 | -2 | u32be, u16le-sub128, u16le-sub128, u16le | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6932 |
| s2c | 108 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5521 |
| s2c | 109 | 10 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5083 |
| s2c | 110 | 18 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4649 |
| s2c | 111 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6563 |
| s2c | 112 | 21 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6517 |
| s2c | 113 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5193 |
| s2c | 114 | 5 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4230 |
| s2c | 115 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6347 |
| s2c | 116 | 25 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6792 |
| s2c | 117 | 4 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5649 |
| s2c | 118 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6826 |
| s2c | 119 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6781 |
| s2c | 120 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5126 |
| s2c | 121 | -2 | no payload decode; Client.loggedout(false) | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6965 |
| s2c | 122 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5189 |
| s2c | 123 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5053 |
| s2c | 124 | 12 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4278 |
| s2c | 125 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5983 |
| s2c | 126 | -2 | u8 (1 means a second jstr), jstr, optional jstr, u16be, u24be, u8, then DataBlock1562.doDataBlock1791 (unread) | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6971 |
| s2c | 127 | 2 | u16be | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5362 |
| s2c | 128 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4449 |
| s2c | 129 | 14 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6058 |
| s2c | 130 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4155 |
| s2c | 131 | 21 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6599 |
| s2c | 132 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6208 |
| s2c | 133 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4407 |
| s2c | 134 | 5 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5833 |
| s2c | 135 | 10 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5584 |
| s2c | 136 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6126 |
| s2c | 137 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4563 |
| s2c | 138 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6903 |
| s2c | 139 | 10 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4933 |
| s2c | 140 | -2 | u16be id, u8 flags (bit0, bit1), u16be count; per entry u16be, u8 (255 then u32be replaces it); if bit1, u8 count of doPublicKeysPart records | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5471 |
| s2c | 141 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6266 |
| s2c | 142 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5623 |
| s2c | 143 | 4 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6891 |
| s2c | 144 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6682 |
| s2c | 145 | 22 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6470 |
| s2c | 146 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6899 |
| s2c | 147 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4346 |
| s2c | 148 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4864 |
| s2c | 149 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5514 |
| s2c | 150 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4559 |
| s2c | 151 | 5 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4527 |
| s2c | 152 | 4 | u32m-3322b6 stored in Statics.tint84 | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6951 |
| s2c | 153 | 10 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6521 |
| s2c | 154 | 8 | u32m-32222222b16, u32m-32222222b16 | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5341 |
| s2c | 155 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4653 |
| s2c | 156 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5629 |
| s2c | 157 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5566 |
| s2c | 158 | 10 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6809 |
| s2c | 159 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5860 |
| s2c | 160 | 4 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5102 |
| s2c | 161 | 4 | u16be, u16be | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6324 |
| s2c | 162 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6387 |
| s2c | 163 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6675 |
| s2c | 164 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5819 |
| s2c | 165 | 1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4455 |
| s2c | 166 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5528 |
| s2c | 167 | 9 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4846 |
| s2c | 168 | 5 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5735 |
| s2c | 169 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4522 |
| s2c | 170 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6872 |
| s2c | 171 | 8 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4541 |
| s2c | 172 | 29 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6839 |
| s2c | 173 | -2 | u8add128 type 1..4, u16be zoneZ, u8add128 mapSizeId, u16le+128 zoneX, u8 force bit0, u8sub128 npcViewBits, then 4*(tiles>>3)*(tiles>>3) of (1 bit present, optional 26 bits) | World.doWorld2 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5739 |
| s2c | 174 | -1 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4237 |
| s2c | 175 | 9 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6195 |
| s2c | 176 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5807 |
| s2c | 177 | 6 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4459 |
| s2c | 178 | 10 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5147 |
| s2c | 179 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4571 |
| s2c | 180 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4505 |
| s2c | 181 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4265 |
| s2c | 182 | 0 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6051 |
| s2c | 183 | 4 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6368 |
| s2c | 184 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5619 |
| s2c | 185 | 5 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4469 |
| s2c | 186 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5725 |
| s2c | 187 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:4645 |
| s2c | 188 | 2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5175 |
| s2c | 189 | 3 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:5367 |
| s2c | 190 | -2 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6173 |
| s2c | 191 | 7 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6204 |
| s2c | 192 | 9 |  | Statics.doStatics3 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:6772 |

## Login stream

`DataBlock1802` stores only the first constructor argument (`deob/DataBlock1802.java:49`). The second argument is discarded, so it is not a packet size. The writers below choose the bytes. Game-table `-1` and `-2` rules do not apply on this stream.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 14 | discarded 0 | u8 opcode 14 and no further bytes | BlankWs.blankWs | builds/rs3/876-2016-11-28/src/src/main/java/deob/BlankWs.java:304 |
| c2s | 15 | discarded -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:8 |
| c2s | 16 | discarded -2 | see login.md lobby stage 80 | BlankWs.blankWs | builds/rs3/876-2016-11-28/src/src/main/java/deob/BlankWs.java:398 |
| c2s | 19 | discarded -2 | see login.md world stage 80 | BlankWs.blankWs | builds/rs3/876-2016-11-28/src/src/main/java/deob/BlankWs.java:461 |
| c2s | 23 | discarded 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:14 |
| c2s | 24 | discarded -1 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:16 |
| c2s | 26 | discarded 0 | u8 opcode 26 and no further bytes | BlankWs.blankWs | builds/rs3/876-2016-11-28/src/src/main/java/deob/BlankWs.java:744 |
| c2s | 27 | discarded 0 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:20 |
| c2s | 28 | discarded -2 | see login.md opcode 28 | Statics.doStatics3222b9 | builds/rs3/876-2016-11-28/src/src/main/java/deob/Statics.java:2595 |
| c2s | 29 | discarded -2 | see login.md stage 11 reconnect | BlankWs.blankWs | builds/rs3/876-2016-11-28/src/src/main/java/deob/BlankWs.java:277 |
| c2s | 30 | discarded -2 | see login.md stage 80 when tboolean2 is set | BlankWs.blankWs | builds/rs3/876-2016-11-28/src/src/main/java/deob/BlankWs.java:396 |
| c2s | 31 | discarded 4 |  | unread | builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1802.java:28 |
