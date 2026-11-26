# Packets

One row per opcode this client reads or writes. `size` is the constructor value. Fields are reads or writes in source order. A token prefixed `if:` is inside a branch. Width names are defined in framing.md. An empty field list means this handler performs no `Packet` read or write. Delegates point at the page that has the bit layout.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 0 valueServerProt92 | 8 | u32_alt3, u32_alt2 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8659 |
| s2c | 1 valueServerProt2 | -2 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8518 |
| s2c | 2 MESSAGE_GAME | -1 | smart1or2, u32be, u8, if:jstr, if:jstr, jstr | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7217 |
| s2c | 3 IF_SETANGLE | 10 | u32le, u16be, u16be, u16be_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8226 |
| s2c | 4 UPDATE_STAT | 6 | u8_sub128, u8, u32_alt3 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8578 |
| s2c | 5 valueServerProt21 | 10 | u32be, u16le, u32_alt3 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6705 |
| s2c | 6 UPDATE_ZONE_PARTIAL_FOLLOWS | 3 | u8, i8, i8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7244 |
| s2c | 7 valueServerProt3 | 32 | u32le, u32be, u32_alt3, u32_alt2, u16be_sub128lo, u32_alt2, u8_neg, u32_alt3, u8_neg, u32_alt2 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6514 |
| s2c | 8 valueServerProt4 | -1 | jstr, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8043 |
| s2c | 9 valueServerProt5 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7915 |
| s2c | 10 VARP_SMALL | 3 | i8_128sub, u16le | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8235 |
| s2c | 11 valueServerProt6 | 2 | i16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7295 |
| s2c | 12 NO_TIMEOUT | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7595 |
| s2c | 13 valueServerProt7 | 8 | u16le, u16be, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7873 |
| s2c | 14 valueServerProt8 | 8 | u32_alt2, u32le | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6907 |
| s2c | 15 valueServerProt9 | 8 | u16be, u8, u16be, u8, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7548 |
| s2c | 16 IF_SETPOSITION | 8 | i16le, u32_alt3, i16le_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8551 |
| s2c | 17 IF_SETANIM | 8 | u32_alt2, u32_alt3 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6900 |
| s2c | 18 valueServerProt10 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8537 |
| s2c | 19 valueServerProt11 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8601 |
| s2c | 20 valueServerProt12 | 10 | u16be, u64be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7723 |
| s2c | 21 LOGOUT | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7942 |
| s2c | 22 CLIENT_SETVARC_SMALL | 3 | i8, u16le_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6638 |
| s2c | 23 valueServerProt13 | -2 | jstr, u8, if:jstr, u64be, i8, u8, if:jstr, if:u8, if:jstr, if:u16be, if:i8, if:jstr | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8686 |
| s2c | 24 valueServerProt15 | -2 | u8 count, u32be id, then per entry u8 type; type 0/2: jstr, jstr; type 1: jstr, jstr, u32be; type 3: jstr, jstr, u8 n, jstr*n, jstr, then per param u32be len, bytes(len); type 4: jstr, jstr, u8 n, jstr*n, jstr | ReflectionCheck.push | builds/rs2/742-2012-11-07/src/client/src/main/java/ReflectionCheck.java:116 |
| s2c | 25 CAM_MOVETO | 6 | u8_128sub, u8_128sub, u8, u8_128sub, u16le_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8606 |
| s2c | 26 valueServerProt76 | 6 | u16be, u8, u16be, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7838 |
| s2c | 27 valueServerProt17 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7849 |
| s2c | 28 CAM_RESET | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7598 |
| s2c | 29 RUNCLIENTSCRIPT | -2 | jstr, if:jstr, if:u32be, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6541 |
| s2c | 30 valueServerProt18 | 5 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6586 |
| s2c | 31 valueServerProt110 | -1 | u8, jstr, if:jstr, u16be, u24be, u8, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8390 |
| s2c | 32 valueServerProt20 | 1 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7613 |
| s2c | 33 valueServerProt54 | 3 | u8, u16le | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7343 |
| s2c | 34 valueServerProt22 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6671 |
| s2c | 35 valueServerProt23 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6506 |
| s2c | 36 valueServerProt24 | -1 | u16be, jstr | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7920 |
| s2c | 37 valueServerProt53 | 2 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7326 |
| s2c | 38 valueServerProt14 | 4 | u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7202 |
| s2c | 39 valueServerProt67 | 4 | u8, u16be, u8_128sub | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8559 |
| s2c | 40 valueServerProt27 | 1 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6946 |
| s2c | 41 valueServerProt28 | 7 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8077 |
| s2c | 42 IF_SETOBJECT | 10 | u32be, u32_alt2, u16le | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7154 |
| s2c | 43 valueServerProt29 | 6 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8616 |
| s2c | 44 valueServerProt30 | -1 | u8, jstr, if:jstr, u64be, u16be, u24be, u8, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8001 |
| s2c | 45 IF_OPENTOP | 19 | u32le, u16le_sub128lo, u32_alt2, u8_128sub, u32be, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8638 |
| s2c | 46 valueServerProt103 | -1 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7033 |
| s2c | 47 IF_OPENSUB | 23 | u32le, u32le, u32_alt2, u8_128sub, u16le_sub128lo, u32_alt2, u32_alt2 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6993 |
| s2c | 48 valueServerProt32 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7705 |
| s2c | 49 valueServerProt33 | 4 | u32le | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8443 |
| s2c | 50 CLIENT_SETVARCSTR_SMALL | -1 | u16be_sub128lo, jstr | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6922 |
| s2c | 51 valueServerProt34 | 5 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7591 |
| s2c | 52 valueServerProt59 | 2 | u8_128sub, i8_neg | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6579 |
| s2c | 53 valueServerProt36 | 6 | u16be_sub128lo, u32_alt3 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7515 |
| s2c | 54 UPDATE_RUNWEIGHT | 2 | i16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6876 |
| s2c | 55 CLIENT_SETVARCSTR_LARGE | -2 | jstr, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6558 |
| s2c | 56 valueServerProt64 | 4 | u16be_sub128lo, u16le_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7881 |
| s2c | 57 valueServerProt87 | -1 | u16be, u16be, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8163 |
| s2c | 58 valueServerProt39 | -1 | u16be, jstr, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6713 |
| s2c | 59 valueServerProt40 | -2 | isaac-subtracted bytes(packetSize), then one Cp1252 string. ENABLE_ISAAC is false, so the subtractend is 0 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7313 |
| s2c | 60 valueServerProt46 | -1 | u8, jstr, u16be, u24be, u8, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7079 |
| s2c | 61 URL_OPEN | -2 | u8 flag, then isaac-subtracted bytes(packetSize-1) holding jstr and, when the flag is 1, a second jstr. ENABLE_ISAAC is false, so the subtractend is 0 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8088 |
| s2c | 62 valueServerProt43 | 3 | u8, i8_sub128, i8_sub128 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7472 |
| s2c | 63 valueServerProt44 | 4 | u32le | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6611 |
| s2c | 64 valueServerProt45 | 25 | u32_alt3, u16le_sub128lo, u32le, u8_neg, u32_alt3, u32le, u16le_sub128lo, u32_alt3 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7502 |
| s2c | 65 NPC_INFO_LARGE | -2 | see sync.md receiveNpcInfo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8514 |
| s2c | 66 valueServerProt25 | 6 | u32be, u16be_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7950 |
| s2c | 67 valueServerProt47 | -2 | u16be, u8, u16be, if:u8, if:u32le, if:u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6929 |
| s2c | 68 valueServerProt104 | 20 | u8, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7351 |
| s2c | 69 UPDATE_RUNENERGY | 1 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7362 |
| s2c | 70 valueServerProt16 | 3 | u16be, i8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6569 |
| s2c | 71 valueServerProt49 | 3 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6667 |
| s2c | 72 NPC_INFO | -2 | see sync.md receiveNpcInfo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7946 |
| s2c | 73 valueServerProt50 | 1 | u8_sub128 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8126 |
| s2c | 74 valueServerProt51 | 14 | u8 (high 3 bits slot, low 5 bits type). Type 0 returns after that byte. Else u8. Type 1 or 10: u16be, u16be, bytes(4), u32be. Type 2 through 6: u8, u16be, u16be, u8, u16be, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7654 |
| s2c | 75 valueServerProt52 | 2 | u8_neg, u8_neg | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7718 |
| s2c | 76 valueServerProt120 | 1 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6985 |
| s2c | 77 valueServerProt115 | 19 | u8_neg, if:u32_alt3, u16le | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6724 |
| s2c | 78 valueServerProt55 | 8 | u32le, u32_alt2 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7866 |
| s2c | 79 valueServerProt56 | 5 | u32_alt3, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7299 |
| s2c | 80 valueServerProt57 | -1 | u8, jstr, if:jstr, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6848 |
| s2c | 81 valueServerProt58 | 4 | u32_alt3 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6914 |
| s2c | 82 VARP_LARGE | 6 | u32le, u16be_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7936 |
| s2c | 83 IF_SETCLICKMASK | 5 | u32_alt3, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8081 |
| s2c | 84 valueServerProt60 | 9 | u32_alt3, u16le_sub128lo, u8, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8620 |
| s2c | 85 valueServerProt61 | 12 | u32be, u16le_sub128lo, u8_sub128, u8_128sub, u16le_sub128lo, u16be_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8241 |
| s2c | 86 valueServerProt62 | -1 | u8, jstr, if:jstr, u64be, u16be, u24be, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7250 |
| s2c | 87 valueServerProt63 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6565 |
| s2c | 88 valueServerProt108 | 28 | bytes(24), u32be crc (checkcrc compares crc of the preceding bytes) | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7147 |
| s2c | 89 REBUILD_NORMAL | -2 | see region.md | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6955 |
| s2c | 90 PLAYER_INFO | -2 | see sync.md MapElement.update | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7888 |
| s2c | 91 CAM_SHAKE | 6 | u8_neg, u8_neg, u8_128sub, u16be, u8_sub128 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8449 |
| s2c | 92 valueServerProt66 | 2 | u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7560 |
| s2c | 93 valueServerProt65 | -2 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7902 |
| s2c | 94 valueServerProt68 | -2 | u16be, u8, if:smart1or2, if:u16be, if:u8, if:u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6966 |
| s2c | 95 valueServerProt111 | 6 | u16be, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7603 |
| s2c | 96 valueServerProt70 | 6 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7752 |
| s2c | 97 valueServerProt69 | 8 | u32_alt2, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6590 |
| s2c | 98 valueServerProt101 | -2 | if:u8, if:jstr, if:jstr | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7168 |
| s2c | 99 VARBIT_LARGE | 6 | u32le, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7930 |
| s2c | 100 valueServerProt79 | 8 | u16be, u8, u16be, u8, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6888 |
| s2c | 101 valueServerProt75 | 2 | u8, u8_neg | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7568 |
| s2c | 102 valueServerProt37 | 4 | i24be, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6843 |
| s2c | 103 valueServerProt77 | 9 | u16le, u32le, u8_sub128, u16be_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8569 |
| s2c | 104 valueServerProt78 | 2 | u16be_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7522 |
| s2c | 105 valueServerProt26 | 10 | u32le, u32_alt2, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6597 |
| s2c | 106 valueServerProt48 | -2 | isaac-subtracted bytes(packetSize), then read as one Cp1252 string. ENABLE_ISAAC is false, so the subtractend is 0 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8431 |
| s2c | 107 IF_SETTEXT | -2 | jstr, u32_alt3 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7711 |
| s2c | 108 IF_SETCOLOUR | 6 | u16le, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7528 |
| s2c | 109 CAM_LOOKAT | 6 | u8, u8_128sub, u8_neg, u8, u16be_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7892 |
| s2c | 110 valueServerProt81 | 29 | u32_alt2, u32_alt2, u16le_sub128lo, u32le, u32be, u32le, u8_128sub, u16le_sub128lo, u32_alt2 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6675 |
| s2c | 111 valueServerProt82 | 1 | u8_128sub | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8634 |
| s2c | 112 valueServerProt83 | 5 | u8_neg, u32_alt3 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7336 |
| s2c | 113 IF_SETHIDE | 5 | u32_alt2, u8_neg | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7210 |
| s2c | 114 valueServerProt90 | -2 | u8, bytes(conn.packetSize-1) | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7647 |
| s2c | 115 valueServerProt85 | 10 | u16le, u16le_sub128lo, u32le, u16le_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7037 |
| s2c | 116 valueServerProt86 | -1 | jstr | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6737 |
| s2c | 117 valueServerProt41 | 11 | u32be, u32_alt2, u8_sub128, u8_neg, u8_neg | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8061 |
| s2c | 118 IF_SETEVENTS | 12 | u32le, u16be_sub128lo, u32_alt2, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7440 |
| s2c | 119 valueServerProt88 | 6 | u16le_sub128lo, u8_128sub, u24_alt3 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6531 |
| s2c | 120 valueServerProt80 | -1 | u8, jstr, u16be, u24be, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7367 |
| s2c | 121 valueServerProt89 | -1 | u16be, u16be, u16be, if:u24be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7064 |
| s2c | 122 valueServerProt74 | 2 | u16be_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7330 |
| s2c | 123 valueServerProt91 | -2 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7733 |
| s2c | 124 valueServerProt72 | 1 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7578 |
| s2c | 125 valueServerProt38 | 7 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6989 |
| s2c | 126 valueServerProt94 | 10 | u16be, u16le_sub128lo, u32le, u16le | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7005 |
| s2c | 127 valueServerProt95 | 18 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8073 |
| s2c | 128 valueServerProt96 | 3 | u16be, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6881 |
| s2c | 129 valueServerProt97 | 9 | u32be, u32_alt3, u8_128sub | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6689 |
| s2c | 130 valueServerProt98 | -2 | u8, smart2or4null | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7414 |
| s2c | 131 valueServerProt114 | -1 | jstr | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7587 |
| s2c | 132 valueServerProt99 | -1 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8057 |
| s2c | 133 valueServerProt100 | 10 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6510 |
| s2c | 134 valueServerProt71 | 8 | u32_alt3, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8463 |
| s2c | 135 valueServerProt102 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7427 |
| s2c | 136 valueServerProt84 | -2 | u8, jstr, if:jstr, u16be, u24be, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8470 |
| s2c | 137 valueServerProt19 | -2 | jstr | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8110 |
| s2c | 138 valueServerProt105 | 0 |  | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7834 |
| s2c | 139 valueServerProt106 | -1 | u8, jstr, if:jstr, u8, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8138 |
| s2c | 140 valueServerProt107 | -2 | if:u8, if:jstr, if:jstr, if:u16be, if:u8, if:u8, if:jstr, if:u8, if:u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6742 |
| s2c | 141 valueServerProt31 | 21 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7622 |
| s2c | 142 valueServerProt109 | 4 | u32_alt2 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6605 |
| s2c | 143 valueServerProt73 | -2 | u16be, u16be count, then count groups of four u32be, then u8 length, bytes(length) | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7626 |
| s2c | 144 valueServerProt42 | -1 | jstr, u8, if:jstr, u16be, i8, if:jstr | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7756 |
| s2c | 145 valueServerProt112 | 2 | u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8629 |
| s2c | 146 valueServerProt113 | -1 | jstr, u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7123 |
| s2c | 147 UPDATE_REBOOT_TIMER | 2 | if:u16be, if:u16be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8542 |
| s2c | 148 IF_CLOSESUB | 4 | u32le | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7535 |
| s2c | 149 valueServerProt93 | -2 | u8 mode. Bits 0, 1-2, and 3-4 select enums and read no further bytes. If bit 7: u16be mask. Mask bit index is MouseRecorder.valueMouseRecorder85 (0..14, MouseRecorder.java:7). Bits 0-3: 3*f32be each (Vector3.opVector314, Vector3.java:101). Bits 4 and 5: f32be, f32be. Bit 6: u8. Bit 7: u24be, u8. Bit 8: u8 count, then per entry u8, u8, and if the first u8 is non-zero the effect body is unread. Bit 9: u16be, f32be. Bit 10: u8. Bit 11: 3*f32be, 3*f32be, f32be, f32be. Bits 12, 13, and 14: f32be. If mode bit 5 and the lookat object is non-null, opLookat_Sub13 is unread. If mode bit 6 and the position object is non-null, opPositionSpline6 is unread. gFloat is f32be | BasicCamera.opBasicCamera2 | builds/rs2/742-2012-11-07/src/client/src/main/java/BasicCamera.java:25 |
| s2c | 150 valueServerProt116 | 8 | u32be, u32be, u8_neg, u32_alt3, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8216 |
| s2c | 151 valueServerProt1 | -1 | jstr, u8_neg, u16le_sub128lo, u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7046 |
| s2c | 152 CLIENT_SETVARC_LARGE | 6 | u32le, u16le_sub128lo | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8594 |
| s2c | 153 REBUILD_REGION | -2 | see region.md | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6645 |
| s2c | 154 valueServerProt117 | 1 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8051 |
| s2c | 155 valueServerProt118 | 22 | u16le_sub128lo, u16be, u8_neg, u16be_sub128lo, u16le, u16le_sub128lo, i8_neg, u16le_sub128lo, i8_neg, i16be, u8, u8_128sub, i16le, u8_sub128 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7957 |
| s2c | 156 VARBIT_SMALL | 3 | u16le_sub128lo, u8_neg | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8132 |
| s2c | 157 valueServerProt119 | 6 | see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6663 |
| s2c | 158 valueServerProt35 | 1 | i8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6656 |
| s2c | 159 valueServerProt121 | 1 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7309 |
| s2c | 160 valueServerProt122 | 25 | u32_alt2, u8_128sub, u32_alt3, u16be_sub128lo, u16be_sub128lo, u32_alt3, u32_alt2, u32be | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7853 |
| s2c | 161 valueServerProt123 | -2 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7130 |
| s2c | 162 valueServerProt124 | -2 | u8_128sub, i8_neg, i8, if:u8; see zone rows | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8116 |
| s2c | 163 CUSTOM_OLDITEMSLOOK | 1 | u8 | Client.handleServerProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8748 |
| c2s | 0 OPNPC6 | 3 | u16le, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1955 |
| c2s | 1 SIMPLE_TOOLKIT_CHANGE | 1 | u8_neg | SimpleToolkitChange | builds/rs2/742-2012-11-07/src/client/src/main/java/SimpleToolkitChange.java:15 |
| c2s | 2 CUTSCENE_FINISHED | 1 | u8 | CutsceneManager | builds/rs2/742-2012-11-07/src/client/src/main/java/CutsceneManager.java:193 |
| c2s | 3 OPLOC2 | 9 | u8_neg, u16le_add128lo, u32le, u16le_add128lo | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1777 |
| c2s | 4 IF_BUTTON1 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 5 RESUME_P_HSLDIALOG | 2 | u16be | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:7926 |
| c2s | 6 OPNPCT | 11 | u16le_add128lo, u32_alt2, u16be_add128lo, u16be, u8_128sub | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1923 |
| c2s | 7 IF_BUTTOND | 16 | u16le_add128lo, u16le, u32_alt2, u16be, u32_alt3, u16le_add128lo | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9878 |
| c2s | 8 DETECT_MODIFIED_CLIENT | 4 | u32be | World | builds/rs2/742-2012-11-07/src/client/src/main/java/World.java:1223 |
| c2s | 9 CLANCHANNEL_KICKUSER | -1 | u8, u8, u16be, jstr | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:11986 |
| c2s | 10 CREATE_CHECK_EMAIL | -2 | u16be, jstr | AccountCreationManager | builds/rs2/742-2012-11-07/src/client/src/main/java/AccountCreationManager.java:31 |
| c2s | 11 URL_REQUEST | -2 | u16be, jstr, jstr, jstr, u8 | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:381 |
| c2s | 12 valueClientProt21 | -1 | jstr | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:9787 |
| c2s | 13 IF_BUTTON3 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 14 CREATE_ACCOUNT | -2 | u16be, jstr, jstr, u8, u8 | Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/Statics.java:1971 |
| c2s | 15 AFFINEDCLANSETTINGS_ADDBANNED_FROMCHANNEL | -1 | u8, u16be, jstr | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:12004 |
| c2s | 16 OPLOC3 | 9 | u8_neg, u16le_add128lo, u32le, u16le_add128lo | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1777 |
| c2s | 17 IGNORELIST_DEL | -1 | u8, jstr | Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/Statics.java:3715 |
| c2s | 18 valueClientProt22 | 2 | u16be | VideoTypeList | builds/rs2/742-2012-11-07/src/client/src/main/java/VideoTypeList.java:52 |
| c2s | 19 valueClientProt23 | 4 | u32be | TextureOp19 | builds/rs2/742-2012-11-07/src/client/src/main/java/TextureOp19.java:207 |
| c2s | 20 CLIENT_CHEAT | -1 | u8, u8, u8, jstr | DeveloperConsole | builds/rs2/742-2012-11-07/src/client/src/main/java/DeveloperConsole.java:597 |
| c2s | 21 RESUME_P_STRINGDIALOG | -1 | u8, jstr | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:7837 |
| c2s | 22 IF_BUTTON2 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 23 OPOBJ5 | 7 | u8, u16le, u16be, u16be | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1734 |
| c2s | 24 EVENT_KEYBOARD | -2 | u16be length (count*4, the -2 length); repeating (u8 key, u24be dt) | ClientWatch | builds/rs2/742-2012-11-07/src/client/src/main/java/ClientWatch.java:114 |
| c2s | 25 REFLECTION_CHECK_REPLY | -1 |  | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2927 |
| c2s | 26 IF_BUTTONT | 16 | u16le, u32le, u32le, u16be_add128lo, u16be, u16le_add128lo | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9903 |
| c2s | 27 OPOBJ4 | 7 | u8, u16le, u16be, u16be | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1734 |
| c2s | 28 IF_BUTTON10 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 29 ADD_NEW_EMAIL_ADDRESS | -2 | u16be, jstr, u8 | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:8503 |
| c2s | 30 NO_TIMEOUT | 0 |  | Client,MapLogicRelated | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2863 |
| c2s | 31 WINDOW_STATUS | 6 | u8, u16be, u16be, u8 | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2372 |
| c2s | 32 OPNPC5 | 3 | u16le, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1955 |
| c2s | 33 EVENT_CAMERA_POSITION | 4 | u16le_add128lo, u16le_add128lo | ClientWatch | builds/rs2/742-2012-11-07/src/client/src/main/java/ClientWatch.java:134 |
| c2s | 34 OPOBJ2 | 7 | u8, u16le, u16be, u16be | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1734 |
| c2s | 35 OPPLAYER7 | 3 | u16le_add128lo, u8 | MiniMenu,Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 36 OPPLAYER6 | 3 | u16le_add128lo, u8 | MiniMenu,Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 37 IF_BUTTON7 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 38 IGNORELIST_ADD | -1 | u8, jstr, u8 | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:12113 |
| c2s | 39 CLIENT_DETAILOPTIONS_STATUS | -1 | bytes(var9.pos) | ClientWatch | builds/rs2/742-2012-11-07/src/client/src/main/java/ClientWatch.java:146 |
| c2s | 40 OPLOC1 | 9 | u8_neg, u16le_add128lo, u32le, u16le_add128lo | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1777 |
| c2s | 41 MOVE_MINIMAPCLICK | 18 | u16be, u16le_add128lo, u8, u8, u8, u16be, u8, u8, u8, u8, u16be, u16be, u8 | MiniMenu.createMoveMessage | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1982 |
| c2s | 42 OPOBJ6 | 7 | u8, u16le, u16be, u16be | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1734 |
| c2s | 43 OPNPC3 | 3 | u16le, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1955 |
| c2s | 44 OPPLAYER1 | 3 | u16le_add128lo, u8 | MiniMenu,Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 45 CLAN_KICKUSER | -1 | u8, jstr | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:11948 |
| c2s | 46 OPPLAYER10 | 3 | u16le_add128lo, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 47 valueClientProt26 | -1 | u8, jstr | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:7934 |
| c2s | 48 FACE_SQUARE | 4 | u16le_add128lo, u16le_add128lo | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1873 |
| c2s | 49 OPLOC6 | 9 | u8_neg, u16le_add128lo, u32le, u16le_add128lo | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1777 |
| c2s | 50 OPPLAYER3 | 3 | u16le_add128lo, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 51 OPOBJ1 | 7 | u8, u16le, u16be, u16be | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1734 |
| c2s | 52 MESSAGE_QUICKCHAT_PRIVATE | -1 | jstr, u16be | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:10245 |
| c2s | 53 OPPLAYER9 | 3 | u16le_add128lo, u8 | MiniMenu,Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 54 OPNPC2 | 3 | u16le, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1955 |
| c2s | 55 OPPLAYER8 | 3 | u16le_add128lo, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 56 TRANSMITVAR_VERIFYID | 4 | u32be | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:12189 |
| c2s | 57 CLOSE_MODAL | 0 |  | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:11768 |
| c2s | 58 OPPLAYER5 | 3 | u16le_add128lo, u8 | MiniMenu,Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 59 IF_BUTTON5 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 60 valueClientProt24 | 4 | u32be | TextureOp19 | builds/rs2/742-2012-11-07/src/client/src/main/java/TextureOp19.java:304 |
| c2s | 61 EVENT_NATIVE_MOUSE_CLICK | 7 | u8_add128, u16be_add128lo, u32_alt3 | NativeMouseLogger | builds/rs2/742-2012-11-07/src/client/src/main/java/NativeMouseLogger.java:25 |
| c2s | 62 OPOBJ3 | 7 | u8, u16le, u16be, u16be | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1734 |
| c2s | 63 DIRECTX_FAILURE | 2 | u16be | ClientDirectXFailure | builds/rs2/742-2012-11-07/src/client/src/main/java/ClientDirectXFailure.java:15 |
| c2s | 64 MAP_BUILD_COMPLETE | 0 |  | World | builds/rs2/742-2012-11-07/src/client/src/main/java/World.java:1254 |
| c2s | 65 RESUME_P_COUNTDIALOG | 4 | u32be | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:7820 |
| c2s | 66 CLICKWORLDMAP | 4 | u32le | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:3109 |
| c2s | 67 EVENT_MOUSE_MOVE | -1 | u8 (timeSum/count), u8 (timeSum%count), then per sample one of: u16be packed; or u8, u16be; or u8, u32be; or u16be, u32be. Applet hook opNativeMouseLogger4 writes nothing | MouseLogger.opMouseLogger41 | builds/rs2/742-2012-11-07/src/client/src/main/java/MouseLogger.java:24 |
| c2s | 68 RESUME_P_NAMEDIALOG | -1 | u8, jstr | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:7828 |
| c2s | 69 IF_BUTTON8 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 70 CHAT_SETMODE | 1 | u8 | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:9885 |
| c2s | 71 FRIENDLIST_ADD | -1 | u8, jstr | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:12059 |
| c2s | 72 OPNPC1 | 3 | u16le, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1955 |
| c2s | 73 valueClientProt25 | 3 | u8, u8, u8 | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:9819 |
| c2s | 74 MOVE_GAMECLICK | 5 | u16be, u16le_add128lo, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1985 |
| c2s | 75 FRIEND_SETRANK | -1 | u8, u8_128sub, jstr | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:11935 |
| c2s | 76 IF_BUTTON4 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 77 SEND_ABUSE_REPORT | -1 | u8, jstr, u8, u8, jstr | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:9841 |
| c2s | 78 WORLDLIST_FETCH | 4 | u32be | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:12491 |
| c2s | 79 OPPLAYER2 | 3 | u16le_add128lo, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 80 IF_BUTTON9 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 81 FRIENDLIST_DEL | -1 | u8, jstr | Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/Statics.java:3686 |
| c2s | 82 OPLOC5 | 9 | u8_neg, u16le_add128lo, u32le, u16le_add128lo | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1777 |
| c2s | 83 RESUME_PAUSEBUTTON | 6 | u16le, u32_alt2 | Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/Statics.java:3626 |
| c2s | 84 CLAN_JOINCHAT_LEAVECHAT | -1 | u8, jstr OR u8 | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:11960 |
| c2s | 85 OPLOC4 | 9 | u8_neg, u16le_add128lo, u32le, u16le_add128lo | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1777 |
| c2s | 86 MESSAGE_PUBLIC | -1 | u8, u8, wordpack(unread) | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:10009 |
| c2s | 87 MESSAGE_QUICKCHAT_PUBLIC | -1 | u8, u16be | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:10231 |
| c2s | 88 OPNPC4 | 3 | u16le, u8 | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1955 |
| c2s | 89 AUTO_SETUP_RESULT | 15 | u16le, u24be, u8, u24le, u8_128sub, u16be, u24le | ClientAutoSetupResult | builds/rs2/742-2012-11-07/src/client/src/main/java/ClientAutoSetupResult.java:8 |
| c2s | 90 EVENT_MOUSE_CLICK | 6 | u32le, u16be | AppletMouseLogger | builds/rs2/742-2012-11-07/src/client/src/main/java/AppletMouseLogger.java:29 |
| c2s | 91 valueClientProt27 | 9 | u8_neg, u32_alt3, u32be | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8219 |
| c2s | 92 CREATE_LOG_PROGRESS | 1 | u8 | AccountCreationManager | builds/rs2/742-2012-11-07/src/client/src/main/java/AccountCreationManager.java:45 |
| c2s | 93 PING_STATISTICS | 4 | u8, u8, u16be_add128lo | Client | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2939 |
| c2s | 94 EVENT_APPLET_FOCUS | 1 | u8 | ClientWatch | builds/rs2/742-2012-11-07/src/client/src/main/java/ClientWatch.java:141 |
| c2s | 95 MESSAGE_PRIVATE | -2 | jstr, wordpack(unread). The p2(0)/psize2 pair is the -2 length, not a second field | ScriptRunner.opScriptRunner594 | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:10028 |
| c2s | 96 OPOBJT | 15 | u16le_add128lo, u32be, u16be_add128lo, u16le_add128lo, u16be_add128lo, u16be, u8_128sub | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1854 |
| c2s | 97 BUG_REPORT | -2 | u16be, jstr2, jstr2, u8_add128 | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:13563 |
| c2s | 98 EVENT_NATIVE_MOUSE_MOVE | -1 | same framing as EVENT_MOUSE_MOVE, and opNativeMouseLogger1 appends u8 per sample | MouseLogger.opMouseLogger41 | builds/rs2/742-2012-11-07/src/client/src/main/java/NativeMouseLogger.java:44 |
| c2s | 99 valueClientProt20 | 12 | u16le_add128lo, u16le_add128lo, u16be, u16be, u32be | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1706 |
| c2s | 100 UID_PASSPORT_RESEND_REQUEST | 0 |  | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:11798 |
| c2s | 101 SEND_EMAIL_VALIDATION_CODE | -1 | u8, jstr | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:8478 |
| c2s | 102 CHANGE_EMAIL_ADDRESS | -2 | u16be, jstr, jstr | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:8488 |
| c2s | 103 IF_BUTTON6 | 8 | u16le_add128lo, u16le_add128lo, u32_alt3 | Client.opClient319 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987 |
| c2s | 104 OPPLAYER4 | 3 | u16le_add128lo, u8 | MiniMenu,Statics | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1908 |
| c2s | 105 RESUME_P_OBJDIALOG | 2 | u16be | ScriptRunner | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:7872 |
| c2s | 106 OPPLAYERT | 11 | u16be_add128lo, u8, u32be, u16be, u16le | Client,MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9835 |
| c2s | 107 OPLOCT | 17 | u16be_add128lo, u16le_add128lo, u16be, u8_neg, u32be, u32le, u16le_add128lo | MiniMenu | builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1747 |

## Zone sub-opcodes

Consumed only after a zone base. `size` is unread (constructor argument discarded). Index is `g1()` into `opZoneProt13()`.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | zone:0 valueZoneProt21 | unread | u8, u16be, u8, u8, u8, u16be, u8 | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9014 |
| s2c | zone:1 valueZoneProt20 | unread | u8_neg, u8_sub128 | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9035 |
| s2c | zone:2 valueZoneProt29 | unread | u16le, u16le, u8_neg | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8928 |
| s2c | zone:3 valueZoneProt22 | unread | u16le, u8_sub128, u16le, u16be | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9078 |
| s2c | zone:4 valueZoneProt23 | unread | u32_alt2, u8_128sub, u8 | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9066 |
| s2c | zone:5 valueZoneProt24 | unread | u8, i8, i8, i16be, u16be, u8, u8, u16be, u16be, u8, u16be, i16be | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8944 |
| s2c | zone:6 valueZoneProt33 | unread | u32_alt3, u8_128sub, u8_sub128 | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8882 |
| s2c | zone:7 valueZoneProt26 | unread | u8, u8, i8, i8, i16be, i16be, u16be, u8, u8, u16be, u16be, u8, u16be, i16be | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8975 |
| s2c | zone:8 valueZoneProt27 | unread | u8, u16be, u8, u8, u8, u16be | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9046 |
| s2c | zone:9 valueZoneProt28 | unread | u16le, u8_128sub | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8788 |
| s2c | zone:10 valueZoneProt31 | unread | u32be, u8 | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8894 |
| s2c | zone:11 valueZoneProt30 | unread | u8_128sub, u8_128sub, u32_alt3, u8_128sub, if:u32be, if:u16be, if:u16be | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8811 |
| s2c | zone:12 valueZoneProt19 | unread | u8, u16be, u16be, u16be, u8, i16be | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8898 |
| s2c | zone:13 valueZoneProt32 | unread | u8, u8, u16be, u8, u24be, jstr | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8870 |
| s2c | zone:14 valueZoneProt25 | unread | u8, u16be, u16be, u16be | Client.handleZoneProt | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8762 |
