# Packets

Widths are the names in [framing.md](framing.md). `none` means the handler was read and it reads no fields. An empty field list means the handler was not read. Game sizes are the constructor values. `-1` is a u8 length and `-2` is a u16be length, and that prefix is not part of the field list.

Login handshake opcodes use the same numbers on a different socket. They are in the second table. Their `Cb5e68c59` size argument is discarded.

## Game client to server

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | 16 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:2 |
| c2s | 1 | 12 | u32_476, u16be_add128, u16le_add128, u16le_add128, u16be | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1491 |
| c2s | 2 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:4 |
| c2s | 3 | 7 | u16le, u8sub128, u32_476 | C5eb941fa.opkind39973 | builds/rs2/745-2012-12-07/src/src/main/java/C5eb941fa.java:21 |
| c2s | 4 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:6 |
| c2s | 5 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:7 |
| c2s | 6 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:8 |
| c2s | 7 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:9 |
| c2s | 8 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:10 |
| c2s | 9 | 6 | u32le, u16be_add128 | Ca1ac0f8e.opkind39973 | builds/rs2/745-2012-12-07/src/src/main/java/Ca1ac0f8e.java:29 |
| c2s | 10 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:12 |
| c2s | 11 | -2 | u16be length placeholder, jstr, jstr, u8, u8, bytes(7), xtea, u16be backpatch | C82f912d1.opkind353710 | builds/rs2/745-2012-12-07/src/src/main/java/C82f912d1.java:34 |
| c2s | 12 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:14 |
| c2s | 13 | 16 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:15 |
| c2s | 14 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:16 |
| c2s | 15 | 3 | u8sub128, u16le_add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1590 |
| c2s | 16 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:18 |
| c2s | 17 | 0 | none | Client.opClient454 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:12368 |
| c2s | 18 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:20 |
| c2s | 19 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:21 |
| c2s | 20 | 0 | none | Cf9cb6c0f14.opkind34641 | builds/rs2/745-2012-12-07/src/src/main/java/Cf9cb6c0f14.java:23 |
| c2s | 21 | 3 | u8sub128, u16le_add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1573 |
| c2s | 22 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:24 |
| c2s | 23 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:25 |
| c2s | 24 | 9 | u16be_add128, u16be, u32_476, u8add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1414 |
| c2s | 25 | 9 | u16be_add128, u16be, u32_476, u8add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1407 |
| c2s | 26 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:28 |
| c2s | 27 | 7 | u16le, u16le_add128, u8add128, u16be | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1546 |
| c2s | 28 | 7 | u16le, u16le_add128, u8add128, u16be | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1547 |
| c2s | 29 | 7 | u16le, u16le_add128, u8add128, u16be | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1548 |
| c2s | 30 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:32 |
| c2s | 31 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:33 |
| c2s | 32 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:34 |
| c2s | 33 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:35 |
| c2s | 34 | 1 | u8 | Statics.opStatics98 | builds/rs2/745-2012-12-07/src/src/main/java/Statics.java:1181 |
| c2s | 35 | 15 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:37 |
| c2s | 36 | 4 | u32be | C2a31b481.opkind351010 | builds/rs2/745-2012-12-07/src/src/main/java/C2a31b481.java:266 |
| c2s | 37 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:39 |
| c2s | 38 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:40 |
| c2s | 39 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:41 |
| c2s | 40 | 9 | u16be_add128, u16be, u32_476, u8add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1409 |
| c2s | 41 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:43 |
| c2s | 42 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:44 |
| c2s | 43 | 15 | u16be_add128, u16be, u8add128, u16le_add128, u32_460, u16le_add128, u16le | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1437 |
| c2s | 44 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:46 |
| c2s | 45 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:47 |
| c2s | 46 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:48 |
| c2s | 47 | 9 | u16be_add128, u16be, u32_476, u8add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1411 |
| c2s | 48 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:50 |
| c2s | 49 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:51 |
| c2s | 50 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:52 |
| c2s | 51 | 17 | u16le_add128, u8neg, u16be, u32_460, u16be_add128, u32_476, u16le_add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1617 |
| c2s | 52 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:54 |
| c2s | 53 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:55 |
| c2s | 54 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:56 |
| c2s | 55 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:57 |
| c2s | 56 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:58 |
| c2s | 57 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:59 |
| c2s | 58 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:60 |
| c2s | 59 | 7 | u16le, u16le_add128, u8add128, u16be | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1544 |
| c2s | 60 | 3 | u8sub128, u16le_add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1576 |
| c2s | 61 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:63 |
| c2s | 62 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:64 |
| c2s | 63 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:65 |
| c2s | 64 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:66 |
| c2s | 65 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:67 |
| c2s | 66 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:68 |
| c2s | 67 | 4 | u32be | Client.opClient480 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:12825 |
| c2s | 68 | 9 | u16be_add128, u16be, u32_476, u8add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1413 |
| c2s | 69 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:71 |
| c2s | 70 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:72 |
| c2s | 71 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:73 |
| c2s | 72 | 5 | u16le_add128, u16be, u8neg | Statics.opStatics116 | builds/rs2/745-2012-12-07/src/src/main/java/Statics.java:3837 |
| c2s | 73 | 9 | u16be_add128, u16be, u32_476, u8add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1405 |
| c2s | 74 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:76 |
| c2s | 75 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:77 |
| c2s | 76 | 11 | u16le_add128, u32_460, u16be_add128, u16le_add128, u8add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1474 |
| c2s | 77 | 3 | u8sub128, u16le_add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1572 |
| c2s | 78 | 18 | u16le_add128, u16be, u8neg, u8, u8, u16be, u8, u8, u8, u8, u16be, u16be, u8 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1505 |
| c2s | 79 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:81 |
| c2s | 80 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:82 |
| c2s | 81 | 15 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:83 |
| c2s | 82 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:84 |
| c2s | 83 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:85 |
| c2s | 84 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:86 |
| c2s | 85 | 11 | u16be, u16le, u8sub128, u32le, u16be_add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1457 |
| c2s | 86 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:88 |
| c2s | 87 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:89 |
| c2s | 88 | 7 | u16le, u16le_add128, u8add128, u16be | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1541 |
| c2s | 89 | 3 | u8sub128, u16le_add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1575 |
| c2s | 90 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:92 |
| c2s | 91 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:93 |
| c2s | 92 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:94 |
| c2s | 93 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:95 |
| c2s | 94 | 9 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:96 |
| c2s | 95 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:97 |
| c2s | 96 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:98 |
| c2s | 97 | 7 | u16le, u16le_add128, u8add128, u16be | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1539 |
| c2s | 98 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:100 |
| c2s | 99 | 3 | u8sub128, u16le_add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1568 |
| c2s | 100 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:102 |
| c2s | 101 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:103 |
| c2s | 102 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:104 |
| c2s | 103 | 4 | u16le, u16be_add128 | MiniMenu.opkind352621 | builds/rs2/745-2012-12-07/src/src/main/java/MiniMenu.java:1605 |
| c2s | 104 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:106 |
| c2s | 105 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:107 |
| c2s | 106 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:108 |
| c2s | 107 | -1 | u8, unread ReflectionCheck.opkind37271, u8 backpatch | Client.opClient579 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:2613 |
| c2s | 108 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:110 |
| c2s | 109 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C10c700cf.java:111 |

## Game server to client

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 0 | -2 | isaac_bytes(n) | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:7586 |
| s2c | 1 | -2 | bit block, see sync.md | MapElement.opkind36705 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:9014 |
| s2c | 2 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:4 |
| s2c | 3 | 28 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:5 |
| s2c | 4 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:6 |
| s2c | 5 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:7 |
| s2c | 6 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:8 |
| s2c | 7 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:9 |
| s2c | 8 | 10 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:10 |
| s2c | 9 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:11 |
| s2c | 10 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:12 |
| s2c | 11 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:13 |
| s2c | 12 | 18 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:14 |
| s2c | 13 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:15 |
| s2c | 14 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:16 |
| s2c | 15 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:17 |
| s2c | 16 | 19 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:18 |
| s2c | 17 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:19 |
| s2c | 18 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:20 |
| s2c | 19 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:21 |
| s2c | 20 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:22 |
| s2c | 21 | 10 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:23 |
| s2c | 22 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:24 |
| s2c | 23 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:25 |
| s2c | 24 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:26 |
| s2c | 25 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:27 |
| s2c | 26 | 5 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:28 |
| s2c | 27 | 7 | none | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:8907 |
| s2c | 28 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:30 |
| s2c | 29 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:31 |
| s2c | 30 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:32 |
| s2c | 31 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:33 |
| s2c | 32 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:34 |
| s2c | 33 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:35 |
| s2c | 34 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:36 |
| s2c | 35 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:37 |
| s2c | 36 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:38 |
| s2c | 37 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:39 |
| s2c | 38 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:40 |
| s2c | 39 | 4 | u32_421 | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:8067 |
| s2c | 40 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:42 |
| s2c | 41 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:43 |
| s2c | 42 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:44 |
| s2c | 43 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:45 |
| s2c | 44 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:46 |
| s2c | 45 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:47 |
| s2c | 46 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:48 |
| s2c | 47 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:49 |
| s2c | 48 | 0 | none | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:7774 |
| s2c | 49 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:51 |
| s2c | 50 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:52 |
| s2c | 51 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:53 |
| s2c | 52 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:54 |
| s2c | 53 | 6 | u24be, u8neg, u16le_add128 | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:8174 |
| s2c | 54 | 14 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:56 |
| s2c | 55 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:57 |
| s2c | 56 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:58 |
| s2c | 57 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:59 |
| s2c | 58 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:60 |
| s2c | 59 | -2 | bit block, see sync.md, 8-bit deltas | Statics.opStatics118 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:8922 |
| s2c | 60 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:62 |
| s2c | 61 | 12 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:63 |
| s2c | 62 | -2 | u8neg, u8sub128, u16be, u16be_add128, u32be*4*n | C50c59075.opkind346717 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:7599 |
| s2c | 63 | 11 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:65 |
| s2c | 64 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:66 |
| s2c | 65 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:67 |
| s2c | 66 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:68 |
| s2c | 67 | 8 | u32be, u32_430 | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:8915 |
| s2c | 68 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:70 |
| s2c | 69 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:71 |
| s2c | 70 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:72 |
| s2c | 71 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:73 |
| s2c | 72 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:74 |
| s2c | 73 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:75 |
| s2c | 74 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:76 |
| s2c | 75 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:77 |
| s2c | 76 | 9 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:78 |
| s2c | 77 | 5 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:79 |
| s2c | 78 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:80 |
| s2c | 79 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:81 |
| s2c | 80 | 29 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:82 |
| s2c | 81 | 19 | u32_421 * C33282969.opkind34741().length, u16be, u8 | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:8158 |
| s2c | 82 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:84 |
| s2c | 83 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:85 |
| s2c | 84 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:86 |
| s2c | 85 | 10 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:87 |
| s2c | 86 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:88 |
| s2c | 87 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:89 |
| s2c | 88 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:90 |
| s2c | 89 | 23 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:91 |
| s2c | 90 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:92 |
| s2c | 91 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:93 |
| s2c | 92 | 5 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:94 |
| s2c | 93 | 9 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:95 |
| s2c | 94 | -2 | u16be_add128, u8add128, u8neg, u8sub128, u16le_add128, zone bits, u32be*4*n | C50c59075.opkind346729 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:7741 |
| s2c | 95 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:97 |
| s2c | 96 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:98 |
| s2c | 97 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:99 |
| s2c | 98 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:100 |
| s2c | 99 | -2 | bit block, see sync.md, 5-bit deltas | Statics.opStatics118 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:8082 |
| s2c | 100 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:102 |
| s2c | 101 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:103 |
| s2c | 102 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:104 |
| s2c | 103 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:105 |
| s2c | 104 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:106 |
| s2c | 105 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:107 |
| s2c | 106 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:108 |
| s2c | 107 | 10 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:109 |
| s2c | 108 | 4 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:110 |
| s2c | 109 | 5 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:111 |
| s2c | 110 | 10 | u16be_add128, u16le_add128, u32_461, u16be_add128 | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:7753 |
| s2c | 111 | 9 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:113 |
| s2c | 112 | 10 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:114 |
| s2c | 113 | 5 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:115 |
| s2c | 114 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:116 |
| s2c | 115 | 4 | i24be, u8 | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:7736 |
| s2c | 116 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:118 |
| s2c | 117 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:119 |
| s2c | 118 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:120 |
| s2c | 119 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:121 |
| s2c | 120 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:122 |
| s2c | 121 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:123 |
| s2c | 122 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:124 |
| s2c | 123 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:125 |
| s2c | 124 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:126 |
| s2c | 125 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:127 |
| s2c | 126 | 32 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:128 |
| s2c | 127 | 25 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:129 |
| s2c | 128 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:130 |
| s2c | 129 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:131 |
| s2c | 130 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:132 |
| s2c | 131 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:133 |
| s2c | 132 | 10 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:134 |
| s2c | 133 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:135 |
| s2c | 134 | 20 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:136 |
| s2c | 135 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:137 |
| s2c | 136 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:138 |
| s2c | 137 | 1 | u8add128 | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:7779 |
| s2c | 138 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:140 |
| s2c | 139 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:141 |
| s2c | 140 | 5 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:142 |
| s2c | 141 | 22 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:143 |
| s2c | 142 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:144 |
| s2c | 143 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:145 |
| s2c | 144 | -2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:146 |
| s2c | 145 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:147 |
| s2c | 146 | 21 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:148 |
| s2c | 147 | 4 | u8sub128, u8add128, u16le | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:7725 |
| s2c | 148 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:150 |
| s2c | 149 | 0 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:151 |
| s2c | 150 | 8 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:152 |
| s2c | 151 | 6 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:153 |
| s2c | 152 | -1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:154 |
| s2c | 153 | 7 | none | Client.opClient457 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:8911 |
| s2c | 154 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:156 |
| s2c | 155 | 3 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:157 |
| s2c | 156 | 12 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:158 |
| s2c | 157 | 2 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:159 |
| s2c | 158 | 25 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:160 |
| s2c | 159 | 1 |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/C8db7d40a.java:161 |

## Login socket

These are not `C10c700cf` rows. `size` is `unused` because `Cb5e68c59` drops its second argument (`Cb5e68c59.java:29-31`).

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 14 | unused | u8 opcode only | Statics.opStatics142 | builds/rs2/745-2012-12-07/src/src/main/java/Statics.java:2171 |
| c2s | 15 | unused | u8 opcode, u8 length, u32be 745, u32be 1, jstr | Client.java state valueClient1812 | builds/rs2/745-2012-12-07/src/src/main/java/Client.java:1445 |
| c2s | 16 | unused | see login.md | Statics.opStatics142 | builds/rs2/745-2012-12-07/src/src/main/java/Statics.java:2268 |
| c2s | 19 | unused | see login.md | Statics.opStatics142 | builds/rs2/745-2012-12-07/src/src/main/java/Statics.java:2328 |
| c2s | 23 | unused |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/Cb5e68c59.java:6 |
| c2s | 24 | unused |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/Cb5e68c59.java:7 |
| c2s | 26 | unused | isaac opcode byte, no further payload | Statics.opStatics142 | builds/rs2/745-2012-12-07/src/src/main/java/Statics.java:2468 |
| c2s | 27 | unused |  | unread | builds/rs2/745-2012-12-07/src/src/main/java/Cb5e68c59.java:9 |
| c2s | 28 | unused | see login.md | C82f912d1.opkind35371 | builds/rs2/745-2012-12-07/src/src/main/java/C82f912d1.java:93 |
| c2s | 29 | unused | see login.md | Statics.opStatics142 | builds/rs2/745-2012-12-07/src/src/main/java/Statics.java:2145 |
| c2s | 30 | unused | see login.md | Statics.opStatics142 | builds/rs2/745-2012-12-07/src/src/main/java/Statics.java:2266 |
