# Packets

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`.

S2C sizes are `C9389b5a8.valuekind5393`. C2S has no size table; the size column is `none`.
Tokens are defined in [framing.md](framing.md). A field list is the buffer reads or writes in source order. Where a row says a value is conditional, the branch is in the cited method.
An opcode missing from the handler chain logs `T1` and disconnects (`DelayedStateChange.java:1656`).
Zone opcodes 60, 115, 73, 9, 156, 34, 31, 110, 189, 192, 215, and 22 are dispatched by `C0bfaa0f7.opkind4571` (`DelayedStateChange.java:447` and `DelayedStateChange.java:1682`). Opcode 227 repeats that parser on raw sub-opcodes.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 2 | -1 | u64be, u16be, u8, jstr when the u16be is greater than 0 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:132 |
| s2c | 238 | 8 | u32le, u16le_sub128lo, u16le_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:218 |
| s2c | 124 | 4 | u16le_sub128lo, u16be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:228 |
| s2c | 138 | 2 | u16le_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:236 |
| s2c | 162 | 5 | u16be_sub128lo, u24le | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:245 |
| s2c | 151 | -1 | u64be, i8, u64be, u16be, u24be, u8, huffstr | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:255 |
| s2c | 206 | -2 | u8 count, u32be, then per entry the kind block in interfaces.md | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:309 |
| s2c | 25 | 8 | u16be, u16le, u32be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:313 |
| s2c | 120 | -2 | u32be, u16be; while pos < size: smart, u16be, and if that u16be is not 0 then u8, and if the u8 is 255 then u32be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:327 |
| s2c | 75 | -1 | jstr when the payload length is not 0 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:369 |
| s2c | 143 | -2 | payload length / 8 times u64be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:378 |
| s2c | 80 | 1 | u8 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:389 |
| s2c | 220 | -1 | u64be, u16be, u24be, u8, huffstr (source order; a later read may sit in a conditional or a loop) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:395 |
| s2c | 242 | 2 | u16le_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:448 |
| s2c | 125 | -2 | dynamic rebuild; see region.md | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:453 |
| s2c | 181 | 8 | u32_b2b3b0b1, u16le_sub128lo, u16le_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:457 |
| s2c | 84 | 10 | u32_b2b3b0b1, u32_b2b3b0b1, u16le_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:467 |
| s2c | 88 | 2 | u16be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:501 |
| s2c | 224 | -1 | u64be, u16be, i8; jstr only when the high bit of the u64be is clear | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:507 |
| s2c | 15 | 6 | u8, u8, u8, u8, u16be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:590 |
| s2c | 35 | -1 | jstr (source order; a later read may sit in a conditional or a loop) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:603 |
| s2c | 209 | 6 | u32le, u16le | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:741 |
| s2c | 237 | 3 | u8_128minus, u16be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:747 |
| s2c | 14 | 7 | u32le, u8_sub128, u16le_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:753 |
| s2c | 155 | 0 | none | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:772 |
| s2c | 244 | 0 | none | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:778 |
| s2c | 185 | -2 | u16be, jstr, u16be_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:793 |
| s2c | 69 | 2 | u8_128minus, u8_128minus | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:803 |
| s2c | 203 | -2 | u64be; if it is 0 the handler stops; else u64be, i8, u8; if that u8 is 255 the handler stops; else that many of u64be, u16be, i8, jstr | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:808 |
| s2c | 205 | 4 | u32le (source order; a later read may sit in a conditional or a loop) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:868 |
| s2c | 91 | 0 | none | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:880 |
| s2c | 79 | 4 | u32_b1b0b3b2 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:891 |
| s2c | 57 | 6 | u32le, u16be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:896 |
| s2c | 85 | 3 | u16le, i8_sub128 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:910 |
| s2c | 26 | 1 | u8 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:916 |
| s2c | 21 | 6 | u16be, u32be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:920 |
| s2c | 253 | 12 | u32be, u16be_sub128lo, u32_b2b3b0b1, u16le | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:937 |
| s2c | 90 | 8 | u32_b2b3b0b1, u16le_sub128lo, u16be_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:972 |
| s2c | 249 | -1 | u16be_sub128lo, u16be_sub128lo, jstr | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:982 |
| s2c | 102 | 0 | none; C3c656823.opkind50012 logs out | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:992 |
| s2c | 93 | -2 | static rebuild; see region.md | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:996 |
| s2c | 204 | 8 | i16be, u16le, u32le | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1000 |
| s2c | 148 | 6 | u8, u8, u16be, u8, u8 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1010 |
| s2c | 193 | -1 | u64be, u16be, u24be, u8, u16be; when the message is shown, TextureOp11.opTextureOp1112 reads one big-endian integer per quickchat parameter, width C046d0af1.valuekind4716[type] (C046d0af1.java:10, ObjStackNode.java:446) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1019 |
| s2c | 118 | 0 | none | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1066 |
| s2c | 140 | 10 | u16be_sub128lo, u32le, i16le, i16be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1073 |
| s2c | 177 | 6 | u8, u8, u16be, u8, u8 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1084 |
| s2c | 76 | 20 | u8 index, u8 flag; if the flag is 0 the handler stops, else that byte is re-read as i8 and then u16be, u32be, u32be, u32be, u32be (WestConnect.java:46) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1093 |
| s2c | 225 | 10 | u32le, u16be, u16be, u16be_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1105 |
| s2c | 247 | 3 | u8_128minus, u8_neg, u8_neg | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1188 |
| s2c | 66 | 10 | u16le_sub128lo, u16be, u16be, u32_b1b0b3b2 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1196 |
| s2c | 70 | -1 | u64be, u16be, then TextureOp11.opTextureOp1112 reads one big-endian integer per quickchat parameter, width C046d0af1.valuekind4716[type] (C046d0af1.java:10, ObjStackNode.java:446) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1207 |
| s2c | 213 | 0 | none | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1214 |
| s2c | 250 | 5 | u16be, u8, u16be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1218 |
| s2c | 190 | 8 | u16be, u16le, u32be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1229 |
| s2c | 131 | 1 | u8 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1243 |
| s2c | 103 | 9 | u8, u8 type; type 2..6 then u16be, u16be, u8; type 1 or 10 then u16be, skip 3 bytes; then u16be (65535 means -1) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1248 |
| s2c | 168 | -1 | u64be, huffstr | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1303 |
| s2c | 43 | -1 | u64be, i8, u64be, u16be, u24be, u8, u16be; when the message is shown, TextureOp11.opTextureOp1112 reads one big-endian integer per quickchat parameter, width C046d0af1.valuekind4716[type] (C046d0af1.java:10, ObjStackNode.java:446) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1309 |
| s2c | 236 | -2 | u32be, u16be, u16be count; count times: u8, replaced by u32le when the u8 is 255, then u16le_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1360 |
| s2c | 241 | -1 | u8_128minus, u8, jstr | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1407 |
| s2c | 128 | -1 | isaac_bytes(size), decoded as a jstr | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1422 |
| s2c | 183 | 0 | none | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1436 |
| s2c | 201 | -2 | npc update; see sync.md | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1440 |
| s2c | 44 | 5 | u16le, u8_sub128, u16le_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1444 |
| s2c | 119 | -1 | jstr | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1454 |
| s2c | 106 | 5 | u16be, u16le_sub128lo, u8_128minus | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1458 |
| s2c | 221 | 10 | u16le_sub128lo, u16be_sub128lo, u16be_sub128lo, u32_b2b3b0b1 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1469 |
| s2c | 227 | -2 | u8_sub128 baseZ, u8_128minus baseX; while pos < size: u8 raw sub-opcode, then C0bfaa0f7.opkind4571 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1480 |
| s2c | 154 | -2 | u16be_sub128lo, u32_b2b3b0b1, jstr | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1491 |
| s2c | 197 | 2 | i16be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1501 |
| s2c | 202 | 2 | u8_neg, u8_sub128 (source order; a later read may sit in a conditional or a loop) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1507 |
| s2c | 191 | 14 | u16le, u32_b1b0b3b2, u16be_sub128lo, u32be, u16le (source order; a later read may sit in a conditional or a loop) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1528 |
| s2c | 23 | 24 | bytes(24) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1557 |
| s2c | 141 | 9 | u16le, u8_neg, u16be, u32_b2b3b0b1 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1561 |
| s2c | 173 | 6 | u16be_sub128lo, u32le | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1577 |
| s2c | 99 | 12 | u16le_sub128lo, u16le_sub128lo, u32le, u16le, u16be_sub128lo | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1583 |
| s2c | 187 | -2 | player update; see sync.md | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1595 |
| s2c | 144 | -2 | u16be, jstr; for each character, jstr if the byte is 115 else u32be; then u32be | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1599 |
| s2c | 72 | 3 | u8, u8, u8 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1621 |
| s2c | 89 | 6 | u32le, u8, u8 (source order; a later read may sit in a conditional or a loop) | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1627 |
| s2c | 41 | 7 | u32le, u16be, u8_sub128 | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1645 |
| s2c | 230 | 5 | u16be, u16be root interface, u8_neg | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1660 |
| s2c | 9 | 14 | u8, u16be, u16le, i8, u16be_sub128lo, i8_sub128, i8, i8, u16le_sub128lo, u8_neg | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:137 |
| s2c | 22 | 4 | u8, u8_128minus, u16le | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:52 |
| s2c | 31 | 15 | u8, i8, i8, i16be, u16be, u8, u8, u16be, u16be, u8, u8 | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:113 |
| s2c | 34 | 6 | u8, u16be, u8, u16be | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:65 |
| s2c | 60 | 15 | u8, i8, i8, i16be, u16be, u8, u8, u16be, u16be, u8, u8 | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:291 |
| s2c | 73 | 7 | u8, u16be, u16be, u16be | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:79 |
| s2c | 110 | 3 | u8_128minus, u16be | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:29 |
| s2c | 115 | 5 | u8, u16be, u8, u8 | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:266 |
| s2c | 156 | 7 | u16be_sub128lo, u8, u16be_sub128lo, u16le_sub128lo | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:215 |
| s2c | 189 | 5 | u8_128minus, u16be, u16be_sub128lo | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:248 |
| s2c | 192 | 4 | u16le_sub128lo, u8_sub128, u8 | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:234 |
| s2c | 215 | 2 | u8_sub128, u8_sub128 | C0bfaa0f7.opkind4571 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:101 |
| s2c | 0 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 1 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 3 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 4 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 5 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 6 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 7 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 8 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 10 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 11 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 12 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 13 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 16 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 17 | -1 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 18 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 19 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 20 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 24 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 27 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 28 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 29 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 30 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 32 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 33 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 36 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 37 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 38 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 39 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 40 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 42 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 45 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 46 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 47 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 48 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 49 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 50 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 51 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 52 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 53 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 54 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 55 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 56 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 58 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 59 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 61 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 62 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 63 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 64 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 65 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 67 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 68 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 71 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 74 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 77 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 78 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 81 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 82 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 83 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 86 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 87 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 92 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 94 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 95 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 96 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 97 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 98 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 100 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 101 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 104 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 105 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 107 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 108 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 109 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 111 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 112 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 113 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 114 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 116 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 117 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 121 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 122 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 123 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 126 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 127 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 129 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 130 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 132 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 133 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 134 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 135 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 136 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 137 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 139 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 142 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 145 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 146 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 147 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 149 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 150 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 152 | 1 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 153 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 157 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 158 | 1 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 159 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 160 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 161 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 163 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 164 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 165 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 166 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 167 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 169 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 170 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 171 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 172 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 174 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 175 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 176 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 178 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 179 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 180 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 182 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 184 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 186 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 188 | 6 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 194 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 195 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 196 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 198 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 199 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 200 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 207 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 208 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 210 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 211 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 212 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 214 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 216 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 217 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 218 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 219 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 222 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 223 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 226 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 228 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 229 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 231 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 232 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 233 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 234 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 235 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 239 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 240 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 243 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 245 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 246 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 248 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 251 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 252 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 254 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| s2c | 255 | 0 | none; logs T1 and disconnects | DelayedStateChange.opDelayedStateChange28 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:1656 |
| c2s | 14 | none | raw u8 14, u8 ((username >>> 16) & 31) | ObjStack.opObjStack20 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ObjStack.java:202 |
| c2s | 15 | none | raw u8 15, u32be 525 | Client.call81a680bc | builds/rs2/525-2008-12-15/src/src/main/java/deob/Client.java:1070 |
| c2s | 16 | none | raw login outer packet; see login.md | ObjStack.opObjStack20 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ObjStack.java:259 |
| c2s | 17 | none | isaac opcode and no payload | ObjStack.opObjStack20 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ObjStack.java:363 |
| c2s | 18 | none | raw login outer packet; see login.md | ObjStack.opObjStack20 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ObjStack.java:257 |
| c2s | 31 | none | isaac u64be | C0bfaa0f7.opkind4573 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0bfaa0f7.java:355 |
| c2s | 53 | none | isaac u16le_add128lo ((button << 15) \| ticks), u32le ((y << 16) \| x) | ExpiresThuJanGmtMaxAge.opkind4441 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:158 |
| c2s | 62 | none | raw u8 62, u8, u8, u16be, u16be | ObjStack.opObjStack19 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ObjStack.java:147 |
| c2s | 69 | none | raw u8 69, u8 block length, RSA block; see login.md | Light.opLight6 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Light.java:45 |
| c2s | 97 | none | isaac u8 1, or isaac u8 0 | ExpiresThuJanGmtMaxAge.opkind4441 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:192 |
| c2s | 98 | none | isaac walk, kind 1; see input.md | MouseWheel.opMouseWheel20 | builds/rs2/525-2008-12-15/src/src/main/java/deob/MouseWheel.java:49 |
| c2s | 122 | none | raw u8 122, u8 block length, RSA block; see login.md | TextureOp35.opTextureOp3518 | builds/rs2/525-2008-12-15/src/src/main/java/deob/TextureOp35.java:31 |
| c2s | 142 | none | isaac u32le of Cd50b3810.opkind5644() | ExpiresThuJanGmtMaxAge.opkind4441 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:203 |
| c2s | 208 | none | isaac mouse samples; see input.md | ExpiresThuJanGmtMaxAge.opkind4441 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:41 |
| c2s | 218 | none | isaac walk, kind 2; see input.md | MouseWheel.opMouseWheel20 | builds/rs2/525-2008-12-15/src/src/main/java/deob/MouseWheel.java:54 |
| c2s | 249 | none | isaac walk, kind 0; see input.md | MouseWheel.opMouseWheel20 | builds/rs2/525-2008-12-15/src/src/main/java/deob/MouseWheel.java:44 |
| c2s | 254 | none | isaac u16be_add128lo, u16be_add128lo | ExpiresThuJanGmtMaxAge.opkind4441 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:185 |
| c2s | 2 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:68 |
| c2s | 9 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:259 |
| c2s | 10 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:162 |
| c2s | 23 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:104 |
| c2s | 24 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1543 |
| c2s | 25 | none |  | deob/RunescapeCom.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/RunescapeCom.java:56 |
| c2s | 26 | none |  | deob/Cee767227.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/Cee767227.java:63 |
| c2s | 29 | none |  | deob/ExpiresThuJanGmtMaxAge.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:503 |
| c2s | 32 | none |  | deob/Ceea51ef6.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/Ceea51ef6.java:174 |
| c2s | 34 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:706 |
| c2s | 37 | none |  | deob/WallDecor.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/WallDecor.java:124 |
| c2s | 40 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:621 |
| c2s | 45 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1558 |
| c2s | 50 | none |  | deob/ConnexionPerdue.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/ConnexionPerdue.java:184 |
| c2s | 51 | none |  | deob/WallDecor.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/WallDecor.java:132 |
| c2s | 56 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:414 |
| c2s | 59 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:602 |
| c2s | 60 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:486 |
| c2s | 70 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:277 |
| c2s | 75 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:93 |
| c2s | 78 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:686 |
| c2s | 85 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:184 |
| c2s | 89 | none |  | deob/C5a9c80ea.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C5a9c80ea.java:18 |
| c2s | 90 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:496 |
| c2s | 91 | none |  | deob/C68af3879.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C68af3879.java:84 |
| c2s | 93 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:589 |
| c2s | 96 | none |  | deob/SynthInstrument.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SynthInstrument.java:47 |
| c2s | 99 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:657 |
| c2s | 102 | none |  | deob/FrameBuffer.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/FrameBuffer.java:247 |
| c2s | 103 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:61 |
| c2s | 107 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:86 |
| c2s | 114 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:798 |
| c2s | 115 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1349 |
| c2s | 116 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:562 |
| c2s | 117 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:110 |
| c2s | 118 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:391 |
| c2s | 119 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:134 |
| c2s | 120 | none |  | deob/TextureOp25.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/TextureOp25.java:10 |
| c2s | 126 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:743 |
| c2s | 130 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:428 |
| c2s | 132 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:80 |
| c2s | 135 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:640 |
| c2s | 143 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:2746 |
| c2s | 144 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:856 |
| c2s | 147 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:86 |
| c2s | 149 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:249 |
| c2s | 150 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:331 |
| c2s | 151 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:543 |
| c2s | 158 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:864 |
| c2s | 161 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:98 |
| c2s | 164 | none |  | deob/ObjType.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/ObjType.java:49 |
| c2s | 165 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:62 |
| c2s | 166 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:92 |
| c2s | 167 | none |  | deob/C90133226.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C90133226.java:27 |
| c2s | 170 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:347 |
| c2s | 171 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:775 |
| c2s | 175 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:294 |
| c2s | 177 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:147 |
| c2s | 178 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:74 |
| c2s | 179 | none |  | deob/C8876ac7c.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C8876ac7c.java:89 |
| c2s | 184 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:513 |
| c2s | 187 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:765 |
| c2s | 191 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:168 |
| c2s | 196 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:528 |
| c2s | 199 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:909 |
| c2s | 200 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:470 |
| c2s | 202 | none |  | deob/Cc33039b3.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/Cc33039b3.java:155 |
| c2s | 203 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:615 |
| c2s | 204 | none |  | deob/ExpiresThuJanGmtMaxAge.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:494 |
| c2s | 205 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:75 |
| c2s | 207 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1336 |
| c2s | 209 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:576 |
| c2s | 211 | none |  | deob/ExpiresThuJanGmtMaxAge.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:570 |
| c2s | 213 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1720 |
| c2s | 219 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1733 |
| c2s | 221 | none |  | deob/Ceea51ef6.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/Ceea51ef6.java:149 |
| c2s | 223 | none |  | deob/C0fa1de35.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:116 |
| c2s | 228 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:646 |
| c2s | 230 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:753 |
| c2s | 231 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:363 |
| c2s | 232 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:220 |
| c2s | 236 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:234 |
| c2s | 237 | none |  | deob/Caadef800.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/Caadef800.java:10 |
| c2s | 239 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:197 |
| c2s | 240 | none |  | deob/SoftwareFont.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:872 |
| c2s | 242 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:476 |
| c2s | 244 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:270 |
| c2s | 246 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:713 |
| c2s | 248 | none |  | deob/C98d1be1a.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:678 |
| c2s | 255 | none |  | deob/ExpiresThuJanGmtMaxAge.java | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:413 |
