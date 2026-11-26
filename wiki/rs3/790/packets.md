# Packets

Sizes for opcodes 0..118 are the second constructor argument of `DataBlock1959` (`deob/DataBlock1959.java:248`, field `tint2`). Sizes for opcodes 0..174 are the second constructor argument of `DataBlock1901` (`deob/DataBlock1901.java:365`, field `tint2`). Compared with `rslib/src/client/packets/790.c` and `rslib/src/server/packets/790.c`, every opcode 0..255 agrees: present Java sizes match, and opcodes with no constructor are `-3` in RSLib. No size-table gap.

Login and JS5 opcodes are `DataBlock2081`. The constructor stores only the first argument (`deob/DataBlock2081.java:46`), so their size column is `unused`. Opcodes 14, 15, 16, 19, 23, 24, 26, 27, 28, 29, and 30 also exist as `DataBlock1959` game opcodes. The `DataBlock2081` bytes are written before ISAAC is installed. The handler column separates the two rows.

An empty fields cell means that handler or writer was not read. Those opcodes are in [gaps.md](gaps.md). Codec names are defined in [framing.md](framing.md).

`jstr` is null-terminated modified Cp1252 (`PositionedSound.doPositionedSound2(String)` at `com/jagex/game/world/entity/PositionedSound.java:183`, decoded by `CoordGrid.doCoordGrid` at `com/jagex/game/shared/movement/CoordGrid.java:283`). `vstr` is a leading `u8` that must be 0, then a `jstr` (`doPositionedSound2322b16` at `PositionedSound.java:395`). `optjstr` is a leading `u8` 0 meaning null, otherwise a `jstr` (`doPositionedSound2322b11` at `PositionedSound.java:374`).

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:4541 |
| c2s | 1 | -2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2305.java:113 |
| c2s | 2 | 4 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:4533 |
| c2s | 3 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/Unused1.java:740 |
| c2s | 4 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:9508 |
| c2s | 5 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10942 |
| c2s | 6 | 6 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:2546 |
| c2s | 7 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:9575 |
| c2s | 8 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:6961 |
| c2s | 9 | 11 | u16be-m128 Client.tint84, u16le index, u16be-m128 Client.tint85, u8 run, u32w-b Statics.tint153 | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1675 |
| c2s | 10 | 3 |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1959.java:26 |
| c2s | 11 | -2 | jstr, chat-bytes DataBlock2443.doDataBlock2443 unread | MouseBinding.doMouseBinding2222b495 | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:6810 |
| c2s | 12 | 4 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2305.java:138 |
| c2s | 13 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10912 |
| c2s | 14 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 15 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10918 |
| c2s | 16 | 0 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:8675 |
| c2s | 17 | -1 | u8, u8, chat-bytes DataBlock2443.doDataBlock2443 unread | MouseBinding chat send | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:6792 |
| c2s | 18 | -2 |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1959.java:42 |
| c2s | 19 | 4 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/config/vartype/SparseVarDomain.java:169 |
| c2s | 20 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/world/WorldMapRenderer.java:80 |
| c2s | 21 | 0 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:3139 |
| c2s | 22 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10960 |
| c2s | 23 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10948 |
| c2s | 24 | 9 | u32be id&0x7fffffff, u16be-m128 baseZ+z, u16be-m128 baseX+x, u8+128 run | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1764 |
| c2s | 25 | 2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/TwitchRelated2.java:60 |
| c2s | 26 | 4 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/Staticelements2.java:1258 |
| c2s | 27 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:9170 |
| c2s | 28 | 9 | u32be id&0x7fffffff, u16be-m128 baseZ+z, u16be-m128 baseX+x, u8+128 run | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1764 |
| c2s | 29 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2170.java:56 |
| c2s | 30 | 15 | u16be baseX+x, u16be-m128 Client.tint84, u16be Client.tint85, u32w-b Statics.tint153, u16be-m128 baseZ+z, u8 run, u16le-m128 index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1795 |
| c2s | 31 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10936 |
| c2s | 32 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13093 |
| c2s | 33 | 5 | u16be-m128 baseZ+localZ, u8+128 run, u16be-m128 baseX+localX | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1992 |
| c2s | 34 | 9 | u32be id&0x7fffffff, u16be-m128 baseZ+z, u16be-m128 baseX+x, u8+128 run | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1764 |
| c2s | 35 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2422.java:47 |
| c2s | 36 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:6947 |
| c2s | 37 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10895 |
| c2s | 38 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13117 |
| c2s | 39 | 15 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/Unused23Part2.java:14 |
| c2s | 40 | 4 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:3224 |
| c2s | 41 | 9 | u32be id&0x7fffffff, u16be-m128 baseZ+z, u16be-m128 baseX+x, u8+128 run | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1764 |
| c2s | 42 | 9 | u32be id&0x7fffffff, u16be-m128 baseZ+z, u16be-m128 baseX+x, u8+128 run | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1764 |
| c2s | 43 | 3 |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1959.java:92 |
| c2s | 44 | 17 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1945 |
| c2s | 45 | -2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:5338 |
| c2s | 46 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13132 |
| c2s | 47 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10966 |
| c2s | 48 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13143 |
| c2s | 49 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 50 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 51 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 52 | 11 | u8 run, u16be Client.tint85, u16be Client.tint84, u32w-d Statics.tint153, u16le-m128 index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1779 |
| c2s | 53 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13256 |
| c2s | 54 | -2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1877.java:220 |
| c2s | 55 | 6 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2170.java:36 |
| c2s | 56 | 7 | u16le index, 128-u8 run, u16le baseX+x, u16be baseZ+z | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1841 |
| c2s | 57 | 0 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/Staticelements2.java:1289 |
| c2s | 58 | 1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:9183 |
| c2s | 59 | 6 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10833 |
| c2s | 60 | 1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/world/WorldMapRenderer.java:131 |
| c2s | 61 | 9 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9531 |
| c2s | 62 | 18 | u16be-m128 baseZ+localZ, u8+128 run, u16be-m128 baseX+localX, u8 255, u8 255, u16be (int)Client.tfloat2, u8 57, u8 Client.tint44, u8 Client.tint46, u8 89, u16be (int)camera.tfloat, u16be (int)camera.tfloat3, u8 63 | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1855 |
| c2s | 63 | 12 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1966 |
| c2s | 64 | 7 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2422.java:31 |
| c2s | 65 | 4 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13387 |
| c2s | 66 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 67 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:3214 |
| c2s | 68 | 7 | u16le index, 128-u8 run, u16le baseX+x, u16be baseZ+z | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1841 |
| c2s | 69 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10954 |
| c2s | 70 | 2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:4581 |
| c2s | 71 | 1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2003.java:18 |
| c2s | 72 | 9 | u32be id&0x7fffffff, u16be-m128 baseZ+z, u16be-m128 baseX+x, u8+128 run | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1764 |
| c2s | 73 | 3 |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1959.java:152 |
| c2s | 74 | -2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/world/WorldMapRenderer.java:106 |
| c2s | 75 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 76 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13103 |
| c2s | 77 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:4550 |
| c2s | 78 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:5328 |
| c2s | 79 | 4 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/config/vartype/SparseVarDomain.java:579 |
| c2s | 80 | 3 |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1959.java:166 |
| c2s | 81 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 82 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13183 |
| c2s | 83 | 3 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:6626 |
| c2s | 84 | 7 | u16le index, 128-u8 run, u16le baseX+x, u16be baseZ+z | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1841 |
| c2s | 85 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13285 |
| c2s | 86 | 0 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/world/WorldMapRenderer.java:96 |
| c2s | 87 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 88 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13199 |
| c2s | 89 | 4 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:9359 |
| c2s | 90 | 3 |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1959.java:186 |
| c2s | 91 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10930 |
| c2s | 92 | 1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:2492 |
| c2s | 93 | 4 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:3398 |
| c2s | 94 | 16 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10865 |
| c2s | 95 | 16 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10841 |
| c2s | 96 | 4 | u16be baseZ+z, u16le baseX+x | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1694 |
| c2s | 97 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:4637 |
| c2s | 98 | 15 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2227.java:13 |
| c2s | 99 | -2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:10407 |
| c2s | 100 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 101 | 2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2243.java:17 |
| c2s | 102 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2305.java:152 |
| c2s | 103 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1846.java:26 |
| c2s | 104 | 3 |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1959.java:214 |
| c2s | 105 | 0 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:12900 |
| c2s | 106 | 0 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:4558 |
| c2s | 107 | 1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2305.java:146 |
| c2s | 108 | -2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:565 |
| c2s | 109 | 3 | u8 run, u16le index | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1735 |
| c2s | 110 | -2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:5353 |
| c2s | 111 | 8 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:10924 |
| c2s | 112 | 9 |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1959.java:230 |
| c2s | 113 | -2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/world/WorldMapRenderer.java:64 |
| c2s | 114 | 2 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/MouseBinding.java:4629 |
| c2s | 115 | 7 | u16le index, 128-u8 run, u16le baseX+x, u16be baseZ+z | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1841 |
| c2s | 116 | 7 | u16le index, 128-u8 run, u16le baseX+x, u16be baseZ+z | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1841 |
| c2s | 117 | -1 |  | unread writer | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:13166 |
| c2s | 118 | 7 | u16le index, 128-u8 run, u16le baseX+x, u16be baseZ+z | LowResPlayerInfo.doLowResPlayerInfo | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/client/LowResPlayerInfo.java:1841 |
| c2s | 14 | unused | (no body) | Statics.jagtheora | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:2688 |
| c2s | 15 | unused | u8 length, u32be 790, u32be 1, jstr Client.string5 | Client JS5 handshake | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:2006 |
| c2s | 16 | unused | lobby block; login.md | Statics.jagtheora | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:2782 |
| c2s | 19 | unused | game-world block; login.md | Statics.jagtheora | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:2843 |
| c2s | 23 | unused |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2081.java:14 |
| c2s | 24 | unused |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2081.java:16 |
| c2s | 26 | unused | (no body) | Statics.jagtheora | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:3121 |
| c2s | 27 | unused |  | no writer found | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock2081.java:20 |
| c2s | 28 | unused | u16be 790, u16be 1, rsa block, jstr Client.string5, u32be Client.tint, u32be Client.tint4, u32be Client.tint3, jstr Statics.string10, u8 language, u8 modegame, GameShell.doGameShell unread, u8 flag, optional jstr, hardware createHardwareBlock unread, bytes(7) skipped | WorldMapRenderer.doWorldMapRenderer | builds/rs3/790-2013-12-16/src/src/main/java/com/jagex/game/world/WorldMapRenderer.java:204 |
| c2s | 29 | unused | u32be 790, u32be 1, u8 if lobby, rsa block; login.md | Statics.jagtheora | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:2662 |
| c2s | 30 | unused | revision rsa and username section omitted; login.md | Statics.jagtheora | builds/rs3/790-2013-12-16/src/src/main/java/deob/Statics.java:2780 |
| s2c | 0 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9797 |
| s2c | 1 | 4 | u32scr-b | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8169 |
| s2c | 2 | 4 | u32be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7746 |
| s2c | 3 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7882 |
| s2c | 4 | 28 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7678 |
| s2c | 5 | 10 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9220 |
| s2c | 6 | 8 | u32be, u32be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9528 |
| s2c | 7 | 3 | u16be, 128-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8343 |
| s2c | 8 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8188 |
| s2c | 9 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8662 |
| s2c | 10 | -2 | repeat until length consumed: u8, jstr, jstr, jstr | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/deob/DataBlock1901.java:26 |
| s2c | 11 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7719 |
| s2c | 12 | 6 | u16be, u32scr-b | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9562 |
| s2c | 13 | 3 | u8-128, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8717 |
| s2c | 14 | 2 | i16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9792 |
| s2c | 15 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7596 |
| s2c | 16 | 4 | u32scr-a | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8842 |
| s2c | 17 | 2 | u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8757 |
| s2c | 18 | 25 | u32scr-a, u32scr-a, u32scr-c, 128-u8, u16be-m128, u32scr-a, u16be, u32be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8926 |
| s2c | 19 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9519 |
| s2c | 20 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8210 |
| s2c | 21 | 8 | u16be, u8, u16be, u8, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9940 |
| s2c | 22 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9048 |
| s2c | 23 | 32 | 128-u8, neg-u8, u32be, u32be, u32scr-a, u32be, u32scr-a, u32scr-c, u16le, u32scr-c | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9538 |
| s2c | 24 | 1 | u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9877 |
| s2c | 25 | 1 | u8-128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9044 |
| s2c | 26 | 6 | neg-u8, u16le-m128, u8-128, u8-128, neg-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9782 |
| s2c | 27 | 10 | u16be, u8, u16be, u8, u16be, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8383 |
| s2c | 28 | 1 | u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8011 |
| s2c | 29 | 21 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7542 |
| s2c | 30 | 10 | u16le-m128, u32scr-b, u16be, u16le-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7807 |
| s2c | 31 | 7 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8290 |
| s2c | 32 | 5 | u32scr-b, u8-128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8635 |
| s2c | 33 | 25 | u32be, u32scr-b, u32scr-c, u32scr-b, 128-u8, u32scr-c, u16be-m128, u16be-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8434 |
| s2c | 34 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7551 |
| s2c | 35 | 2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9276 |
| s2c | 36 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9372 |
| s2c | 37 | 18 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9976 |
| s2c | 38 | 1 | u8-128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8280 |
| s2c | 39 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7782 |
| s2c | 40 | 11 | u32scr-c, u8-128, u8-128, u32be, 128-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8268 |
| s2c | 41 | 2 | u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7546 |
| s2c | 42 | 2 | u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9267 |
| s2c | 43 | 1 | 128-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7647 |
| s2c | 44 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8906 |
| s2c | 45 | -1 | jstr, u8-128, 128-u8, u16be-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9761 |
| s2c | 46 | 10 | u8-128, u32be, u8, u32scr-b | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8366 |
| s2c | 47 | 10 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7698 |
| s2c | 48 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7894 |
| s2c | 49 | 6 | u32scr-c, u16le | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9365 |
| s2c | 50 | 3 | u16be-m128, i8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8735 |
| s2c | 51 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9809 |
| s2c | 52 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8401 |
| s2c | 53 | 20 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7999 |
| s2c | 54 | 2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8425 |
| s2c | 55 | 6 | u16be, u32scr-b | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9654 |
| s2c | 56 | 4 | u16be, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8360 |
| s2c | 57 | 1 | u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9675 |
| s2c | 58 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7772 |
| s2c | 59 | 4 | i24be, u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9927 |
| s2c | 60 | 6 | u16le, u32scr-b | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8415 |
| s2c | 61 | 9 | u32scr-b, i16be-m128, u8-128, u16le | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9700 |
| s2c | 62 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9294 |
| s2c | 63 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7586 |
| s2c | 64 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8864 |
| s2c | 65 | 22 | u16le, i16be-m128, u8, u16be-m128, 128-u8, u16le, i16le, neg-i8, u16be-m128, u8, u16be, u16be, u8-128, 128-i8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9472 |
| s2c | 66 | -2 | u32scr-a, jstr | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8985 |
| s2c | 67 | -2 | npc large; sync.md | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9696 |
| s2c | 68 | 12 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8481 |
| s2c | 69 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7878 |
| s2c | 70 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9689 |
| s2c | 71 | 2 | i16be-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8021 |
| s2c | 72 | 8 | u32scr-a, u32scr-b | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8870 |
| s2c | 73 | 8 | u32scr-b, u32scr-b | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8162 |
| s2c | 74 | 2 | u16le | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8213 |
| s2c | 75 | 1 | u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7874 |
| s2c | 76 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8992 |
| s2c | 77 | 8 | u16be, u8, u16be, u8, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8829 |
| s2c | 78 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9892 |
| s2c | 79 | 14 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8768 |
| s2c | 80 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8622 |
| s2c | 81 | 19 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8642 |
| s2c | 82 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9887 |
| s2c | 83 | 5 | u8-128, u32scr-a | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8761 |
| s2c | 84 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8954 |
| s2c | 85 | 3 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7663 |
| s2c | 86 | 3 | neg-u8, u16le | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8741 |
| s2c | 87 | 3 | neg-i8, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9855 |
| s2c | 88 | 1 | u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8747 |
| s2c | 89 | 6 | u16be, u8, u16be, u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9255 |
| s2c | 90 | 8 | u32scr-a, u32scr-c | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9932 |
| s2c | 91 | 4 | u32scr-c | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7986 |
| s2c | 92 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9583 |
| s2c | 93 | 6 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7825 |
| s2c | 94 | 6 | u16be, u16be, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9661 |
| s2c | 95 | 5 | 128-u8, u32scr-a | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9900 |
| s2c | 96 | 8 | i16be, u32scr-c, i16le-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9869 |
| s2c | 97 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9685 |
| s2c | 98 | 8 | u32be, u32scr-c | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9454 |
| s2c | 99 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8286 |
| s2c | 100 | 12 | 128-u8, u16le, u32be, u16be, neg-u8, u16be-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9052 |
| s2c | 101 | 10 | u32scr-b, u16be-m128, u32scr-c | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8727 |
| s2c | 102 | 2 | neg-u8, neg-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7708 |
| s2c | 103 | -2 | static rebuild; region.md | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7686 |
| s2c | 104 | 2 | u16le-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7702 |
| s2c | 105 | 8 | u32scr-c, u32scr-c | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8852 |
| s2c | 106 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8459 |
| s2c | 107 | 3 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9722 |
| s2c | 108 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8025 |
| s2c | 109 | -2 | u16be, u8, jstr | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9359 |
| s2c | 110 | 2 | u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9461 |
| s2c | 111 | 5 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9524 |
| s2c | 112 | 6 | u16be, u32scr-a | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8336 |
| s2c | 113 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7803 |
| s2c | 114 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9555 |
| s2c | 115 | 7 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8848 |
| s2c | 116 | 6 | u32be, u16be-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9862 |
| s2c | 117 | 6 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9216 |
| s2c | 118 | -1 | jstr | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8429 |
| s2c | 119 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7788 |
| s2c | 120 | 29 | u16le-m128, u16le-m128, 128-u8, u32scr-c, u32scr-a, u32be, u32scr-c, u32scr-c, u32be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7732 |
| s2c | 121 | 8 | u32scr-b, u32scr-b | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9953 |
| s2c | 122 | 6 | neg-u8, u16be-m128, neg-u8, neg-u8, 128-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9569 |
| s2c | 123 | -1 | jstr, u16le-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9668 |
| s2c | 124 | 2 | u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8298 |
| s2c | 125 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8526 |
| s2c | 126 | -2 | player update; sync.md | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8294 |
| s2c | 127 | 6 | u32scr-c, u16le | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7759 |
| s2c | 128 | 4 | u16le, u16le | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9907 |
| s2c | 129 | 6 | u32be, u8, neg-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7532 |
| s2c | 130 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7829 |
| s2c | 131 | 2 | u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9756 |
| s2c | 132 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8902 |
| s2c | 133 | 7 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8421 |
| s2c | 134 | 3 | u8-128, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8516 |
| s2c | 135 | 3 | u16le-m128, 128-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7765 |
| s2c | 136 | 3 | u8-128, 128-i8, i8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9466 |
| s2c | 137 | -2 | jstr, u16be-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7752 |
| s2c | 138 | 10 | u16le, u32scr-c, u16be, u16le-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8976 |
| s2c | 139 | 19 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9438 |
| s2c | 140 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9960 |
| s2c | 141 | 5 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9896 |
| s2c | 142 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8304 |
| s2c | 143 | 3 | i8, u16be-m128 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8329 |
| s2c | 144 | 8 | u32be, u32scr-c | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7887 |
| s2c | 145 | 4 | u32be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9286 |
| s2c | 146 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8585 |
| s2c | 147 | 4 | u32scr-c | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9345 |
| s2c | 148 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9391 |
| s2c | 149 | -1 | u16be, jstr, u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7667 |
| s2c | 150 | 23 | u32scr-c, u32scr-c, u32scr-c, u16be-m128, 128-u8, u32scr-c, u32be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8447 |
| s2c | 151 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8596 |
| s2c | 152 | 0 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7528 |
| s2c | 153 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8184 |
| s2c | 154 | 1 | u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7653 |
| s2c | 155 | 10 | u16be-m128, u32scr-a, u32scr-c | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8600 |
| s2c | 156 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7995 |
| s2c | 157 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8693 |
| s2c | 158 | -1 | jstr | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9272 |
| s2c | 159 | -2 | npc small; sync.md | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8713 |
| s2c | 160 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8608 |
| s2c | 161 | -2 | dynamic rebuild; region.md | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9914 |
| s2c | 162 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7579 |
| s2c | 163 | 9 | u16be-m128, u16be-m128, u32scr-b, neg-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8351 |
| s2c | 164 | -1 | jstr | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8921 |
| s2c | 165 | -2 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9354 |
| s2c | 166 | 2 | u8, neg-u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7777 |
| s2c | 167 | 10 | u32scr-b, u32scr-c, u16le | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8939 |
| s2c | 168 | 1 | u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9718 |
| s2c | 169 | -1 |  | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8222 |
| s2c | 170 | 9 | u16le-m128, u16be, 128-u8, u32scr-b | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8626 |
| s2c | 171 | 4 | u32scr-a | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:9825 |
| s2c | 172 | 6 | 128-u8, neg-u8, u8, 128-u8, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8152 |
| s2c | 173 | 1 | u8 | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:8859 |
| s2c | 174 | -1 | jstr, u16be | Client.doClient23 | builds/rs3/790-2013-12-16/src/src/main/java/rs2/client/Client.java:7816 |
