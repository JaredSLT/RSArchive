# Packets

S2C sizes are `C105f7f5d.valuekind3423`. There is no C2S size array, so every C2S row has size `none`. The dispatcher is `SpotAnim.call72b59b7f`. A size-0 opcode with no branch, and opcode 208, reach `T1 -` and `Ca17abb5a.opkind2335` at `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1426`. Width names are the buffer methods in [framing.md](framing.md).

Zone opcodes 141, 65, 63, 174, 29, 159, 88, 39, 198, 127, and 61 are read by `C63d74d54.opkind2843`. Opcode 105 reads a zone base and then those same sub-opcodes until the payload ends.

`MapElement.opMapElement13` is the chat string (`builds/rs2/481-2007-12-11/src/src/main/java/deob/MapElement.java:58`). It calls `Ca0f54332.opkind3291`, which was not read.

## S2C

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 4 | -2 | u32be widget (lookup only when the value is >= 0; if the value is < -70000 the following id adds 32768), u16be id, u16be count, then count of u16be and u8 (u8 255 is replaced by u32be) | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1185` |
| s2c | 6 | 2 | u8_sub128 zone z, u8_sub128 zone x | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:839` |
| s2c | 8 | -1 | u8_sub128, jstr, u8_add128 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:272` |
| s2c | 10 | 8 | u32_ime, i16be_sub128, i16be_sub128 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:176` |
| s2c | 17 | 3 | u8_add128 local z, u8_add128 local x, u8_add128 (level is byte >> 1, jump is byte & 1), then `opDirectByteArray22(z, jump == 1, x)` | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:262` |
| s2c | 18 | 7 | u16le_sub128, u8_add128, u32_me | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:788` |
| s2c | 20 | 0 | | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:682` |
| s2c | 21 | 6 | u32be, u16be_sub128 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:245` |
| s2c | 29 | 2 | u8_add128 (type is byte >> 2, rotation is byte & 3), u8_sub128 packed `((b >> 4) & 7) + zoneX` and `(b & 7) + zoneZ` | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:429` |
| s2c | 30 | 3 | u8, u8, u8 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1375` |
| s2c | 35 | 2 | u16be_sub128 root interface | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:846` |
| s2c | 37 | -2 | u32be, u16be (if the u32 is < -70000 the u16 adds 32768), then while position < size: usmart, u16be, and if that u16 is not 0 a u8 (255 is replaced by u32be) | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:476` |
| s2c | 39 | 4 | u16le, u8 (type >> 2, rotation & 3), u8_sub128 packed `((b >> 4) & 7) + zoneX` and `(b & 7) + zoneZ` | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:479` |
| s2c | 45 | -2 | `(size / 8)` times u64be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:356` |
| s2c | 46 | 5 | u24le, u16be (65535 becomes -1) | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:827` |
| s2c | 47 | -2 | u64be; 0 clears the list. Else u64be, i8, u8 count (255 returns). Else count groups of u64be, u16be, i8 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:888` |
| s2c | 50 | 6 | u32_me, u16le | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:802` |
| s2c | 57 | 0 | | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1151` |
| s2c | 61 | 7 | u16be, u8_add128 packed `((b >> 4) & 7) + zoneX` and `(b & 7) + zoneZ`, u16le_sub128 player index (skipped when it equals the local index), u16le | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:510` |
| s2c | 63 | 7 | u8 packed `((b >> 4) & 7) + zoneX` and `(b & 7) + zoneZ`, u16be, u16be, u16be | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:335` |
| s2c | 65 | 15 | u8 packed `((b >> 4) & 7) + zoneX` and `(b & 7) + zoneZ`, i8, i8, i16be, u16be (65535 skips the spawn), u8 times 4, u8 times 4, u16be, u16be, u8, u8 | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:311` |
| s2c | 68 | 2 | u8neg zone z, u8_sub128 zone x, then clear the 8 by 8 ground-item square | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1403` |
| s2c | 77 | 10 | u32_ime, u16be (65535 becomes -1), u32_me. All three are read before the branch | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:132` |
| s2c | 84 | -2 | u8 count, u32be, then per entry a u8 type. Types 0 and 2: two jstr. Type 1: two jstr, u32be. Types 3 and 4: two jstr, u8 count, count jstr, and type 3 also per argument u32be and bytes(n) | `UnderwaterMaterialRenderer.opUnderwaterMaterialRenderer3` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/UnderwaterMaterialRenderer.java:18` |
| s2c | 86 | 12 | u16le_sub128 (65535 becomes -1), u32_me, u16be_sub128 (65535 becomes -1), u32be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:393` |
| s2c | 88 | 6 | u8 packed `((113 & b) >> 4) + zoneX` and `(b & 7) + zoneZ`, u16be, u8, u16be | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:465` |
| s2c | 90 | 6 | u32le, i16le_sub128 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:717` |
| s2c | 98 | 6 | u16be, u32le | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1389` |
| s2c | 99 | 6 | u8, u8, u8, u8, u16be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:87` |
| s2c | 103 | -1 | u64be, u16be, u24be, u8, compressed string | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:557` |
| s2c | 105 | -2 | u8neg zone z, u8_add128 zone x, then while position < size: u8 sub-opcode and `opkind2843` | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1079` |
| s2c | 108 | 5 | u32le, u8neg (byte == 1) | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:949` |
| s2c | 114 | -2 | NPC update. See [sync.md](sync.md) | `Standort.opkind3012` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1043` |
| s2c | 115 | 1 | u8 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:732` |
| s2c | 120 | -1 | u64be, i8, u64be, u16be, u24be, u8, compressed string | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:984` |
| s2c | 123 | 6 | u8, u8, u16be, u8, u8. The last u8 below 100 snaps the camera with atan2 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1049` |
| s2c | 126 | -2 | static rebuild. See [region.md](region.md) | `SieBefindenSichEinemMitgliederGebiet.opkind2783` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1247` |
| s2c | 127 | 3 | u8_sub128 packed `((115 & b) >> 4) + zoneX` and `(b & 7) + zoneZ`, u16be | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:531` |
| s2c | 130 | 8 | u16be, u16be, u32_ime | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:102` |
| s2c | 135 | 2 | u16be_sub128, stored times 30 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:74` |
| s2c | 137 | 20 | u8 index, u8. If the second byte is not 0 the position rewinds one byte and `new Ca17abb5a(buffer)` runs. That constructor was not read | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1092` |
| s2c | 141 | 4 | u16be_sub128, u8_add128 packed `((116 & b) >> 4) + zoneX` and `(b & 7) + zoneZ`, u8_sub128 (type >> 2, rotation & 3) | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:246` |
| s2c | 142 | 4 | u32be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:369` |
| s2c | 146 | 11 | u64be, u16be, i8. The high bit of the long selects remove versus insert | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:600` |
| s2c | 148 | 2 | u16le (65535 becomes -1) | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:816` |
| s2c | 149 | -2 | u32be, jstr | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1234` |
| s2c | 152 | 4 | u32le | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:781` |
| s2c | 159 | 5 | u8 packed `((118 & b) >> 4) + zoneX` and `(b & 7) + zoneZ`, u16be, u8 (low 3 bits and `((250 & b) >> 4)` radius), u8 | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:442` |
| s2c | 162 | 2 | i16be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1160` |
| s2c | 165 | 1 | u8 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:379` |
| s2c | 166 | 6 | u16le, u32_ime | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:738` |
| s2c | 172 | 4 | u32be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1137` |
| s2c | 174 | 14 | u16be_sub128, i8, u16le, i8neg, u16be, i8 of (128 - b), u16le_sub128 player index, u8neg (type >> 2, rotation & 3), u8_sub128 packed `((b >> 4) & 7) + zoneX` and `(b & 7) + zoneZ`, i8 | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:357` |
| s2c | 179 | 6 | u32be, u16le | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1029` |
| s2c | 184 | -2 | jstr template. For each character other than `s` (code 115) a u32be, otherwise a jstr. Then a leading u32be script id. The array is filled from the end | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:112` |
| s2c | 186 | 1 | u8 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:697` |
| s2c | 187 | -2 | dynamic rebuild. See [region.md](region.md) | `SieBefindenSichEinemMitgliederGebiet.opkind2783` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1383` |
| s2c | 188 | 5 | u16be, u8, u16be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:868` |
| s2c | 192 | 0 | | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1106` |
| s2c | 198 | 5 | u16le_sub128, u8 packed `((117 & b) >> 4) + zoneX` and `(b & 7) + zoneZ`, u16le | `C63d74d54.opkind2843` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:492` |
| s2c | 202 | 0 | | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:877` |
| s2c | 206 | 11 | u64be, u16be, u8 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:289` |
| s2c | 208 | 6 | | none. Falls through to T1 | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1426` |
| s2c | 209 | 6 | u8, u8, u16be, u8, u8. The last u8 below 100 snaps the camera position | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1168` |
| s2c | 212 | -1 | one jstr. Later tests use the string. They do not read more buffer bytes. The prefix constants were not opened | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1253` |
| s2c | 213 | 3 | u16be, i8neg | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:767` |
| s2c | 216 | -1 | u64be, compressed string | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:860` |
| s2c | 218 | 2 | u16be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1112` |
| s2c | 221 | 0 | | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:704` |
| s2c | 223 | 0 | | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:520` |
| s2c | 230 | 10 | u16le, u32_ime, u16le, u16be_sub128 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1120` |
| s2c | 231 | 6 | u8 type. Type 1 reads u16be. Types 2 through 6 read u16be, u16be, u8, then the type is stored as 2. Type 10 reads u16be. Bytes left inside the fixed 6 are not read | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:199` |
| s2c | 236 | 24 | bytes(24) copied by `C1be0fc13.opSoftwareFont14` and the position advances 24. No integer decode | `C1be0fc13.opSoftwareFont14` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C1be0fc13.java:30` |
| s2c | 242 | 0 | | `Ca17abb5a.opkind2335` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:193` |
| s2c | 243 | 4 | u32be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:968` |
| s2c | 245 | -2 | u16be root interface, u16be group count, that many groups of u32be, u16be, u8, then until the position saved before the first u16be: u32be, u16be start, u16be end, u32be | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:420` |
| s2c | 252 | 6 | u32_ime, u8_sub128, u8 | `SpotAnim.call72b59b7f` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:537` |
| s2c | 254 | -2 | player update. See [sync.md](sync.md) | `UnerwarteteAntwortVomAnmeldeServer.opkind2683` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:761` |

Every other opcode in the 256-entry table has size 0, has no branch, and takes the T1 path. Those rows are not repeated here.

## C2S read

Steps are capped at 25. The first stored waypoint is the clicked tile. The packet writes `waypoint[count - 1]` first (`Cd4b59fc7.valuekind2702` is the value added to base x, `C1b9239bd.valuekind2671` is the value added to base z). Each earlier step is `u8_add128` dx and `u8_add128` dz from that first written tile. The length byte is payload, written with `opkind33446`. For `n` waypoints the bytes after that length are `2 * n + 3`.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 29 | none | u16le, u32be | `SpotAnim.opSpotAnim7` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1462` |
| c2s | 30 | none | u8 1 or 0 | `C55f3f558.opkind3064` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C55f3f558.java:485` |
| c2s | 53 | none | u64be | `Sprite.call5fde21bf` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/Sprite.java:98` |
| c2s | 61 | none | u32be of `(button << 19) + (time << 20) + y * 765 + x`. Button 2 sets the high bit of that 19-bit field. Time is clamped to 4095. x is clamped to 0..764 and y to 0..502 | `C55f3f558.opkind3064` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C55f3f558.java:463` |
| c2s | 95 | none | u8 length `2 * steps + 3`, u16le_add128 of waypoint z plus `C1be0fc13.valuekind3182`, u16le of waypoint x plus `BitteVersuchenSie.valuekind2721`, then per extra step u8_add128 dx and u8_add128 dz, then u8 run (`TitleMute.valuekind3487[82]` is 1, else 0) | `GameShell.opGameShell27` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/GameShell.java:301` |
| c2s | 146 | none | no payload. Sent when both idle counters are above 4500 | `C55f3f558.opkind3064` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C55f3f558.java:677` |
| c2s | 198 | none | same body as opcode 95. Length byte is `2 * steps + 3` | `GameShell.opGameShell27` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/GameShell.java:292` |
| c2s | 213 | none | u16le of `SorryInvitedPlayersOnly.valuekind23053`, write-u16be_sub128 of `C63d74d54.valuekind28410`. Keys 96 through 99 set the send flag | `C55f3f558.opkind3064` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C55f3f558.java:478` |
| c2s | 219 | none | no payload when `TextureOp17.valueTextureOp175` is above 50. `WallDecor.java:44` also writes opcode 219. That method was not read | `C55f3f558.opkind3064` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C55f3f558.java:752` |
| c2s | 229 | none | u16be, u8neg, u16be, u32be widget | `C55f3f558.opkind3064` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C55f3f558.java:568` |
| c2s | 248 | none | u8 length back-patched with `opkind33421`. Per sample, a small delta with history below 8 is u16be `(history << 12) + ((dx + 32) << 6) + (dy + 32)`. History below 8 otherwise is u24be of the packed coordinate plus `(history << 19) + 8388608`. Otherwise u32be with the `-1073741824` flag. Click coordinates are clamped to 0..764 and 0..502 and packed as `y * 765 + x`. The pair `-1, -1` becomes 524287. The loop stops once 240 bytes have been added after the length placeholder | `C55f3f558.opkind3064` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C55f3f558.java:362` |
| c2s | 249 | none | length byte is `2 * steps + 3 + 14`, then the same walk body as opcode 95. After `opGameShell27` returns, `Sprite.opSprite34` writes u8, u8, u16be yaw, u8 57, u8, u8, u8 89, u16be fine x, u16be fine z, u8, u8 63 | `GameShell.opGameShell27` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/GameShell.java:297` |

RSLib `walk_encode_481` does not write the length byte (`rslib/src/both/move/rev_481.c:313`). Its comment calls that byte framing and says the 14 minimap bytes are not written (`rslib/src/both/move/rev_481.c:20`). This client writes both inside the payload.

## C2S not read

The writers below call `opkind3405` and the bytes after that call were not read. The field list is empty.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:471` |
| c2s | 2 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:121` |
| c2s | 8 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:262` |
| c2s | 11 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:72` |
| c2s | 12 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:655` |
| c2s | 13 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:351` |
| c2s | 21 | none | | `SoftwareFont.calld58221ea` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:974` |
| c2s | 26 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:457` |
| c2s | 28 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:292` |
| c2s | 32 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:319` |
| c2s | 40 | none | | `Cd13a8822` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/Cd13a8822.java:101` |
| c2s | 42 | none | | `PathingEntity` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/PathingEntity.java:73` |
| c2s | 46 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:696` |
| c2s | 47 | none | | `PleaseEnterUsername` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/PleaseEnterUsername.java:51` |
| c2s | 48 | none | | `SoundBank` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoundBank.java:314` |
| c2s | 54 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:494` |
| c2s | 60 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:573` |
| c2s | 64 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:102` |
| c2s | 65 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:535` |
| c2s | 66 | none | | `SoftwareFont.calld58221ea` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:1397` |
| c2s | 69 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:501` |
| c2s | 70 | none | | `ImageProducerFrameBuffer` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ImageProducerFrameBuffer.java:107` |
| c2s | 74 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:587` |
| c2s | 77 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:90` |
| c2s | 79 | none | | `SelectWorld` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SelectWorld.java:348` |
| c2s | 85 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:200` |
| c2s | 87 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:134` |
| c2s | 90 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:187` |
| c2s | 91 | none | | `SekundeXnUbertragen` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SekundeXnUbertragen.java:44` |
| c2s | 92 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:60` |
| c2s | 100 | none | | `SoftwareFont.calld58221ea` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:1173` |
| c2s | 101 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:302` |
| c2s | 104 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:334` |
| c2s | 105 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:594` |
| c2s | 106 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:554` |
| c2s | 107 | none | | `SoftwareFont.calld58221ea` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:1382` |
| c2s | 110 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:638` |
| c2s | 130 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:78` |
| c2s | 132 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:96` |
| c2s | 133 | none | | `SoftwareFont.calld58221ea` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:1019` |
| c2s | 134 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:357` |
| c2s | 140 | none | | `WallDecor.opWallDecor18` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/WallDecor.java:59` |
| c2s | 147 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:54` |
| c2s | 148 | none | | `C5afc506d` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C5afc506d.java:59` |
| c2s | 153 | none | | `SoftwareFont.calld58221ea` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:1186` |
| c2s | 154 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:48` |
| c2s | 155 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:521` |
| c2s | 158 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:163` |
| c2s | 159 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:401` |
| c2s | 161 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:431` |
| c2s | 163 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:487` |
| c2s | 167 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:234` |
| c2s | 168 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:74` |
| c2s | 170 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:723` |
| c2s | 171 | none | | `UnderwaterMaterialRenderer` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/UnderwaterMaterialRenderer.java:126` |
| c2s | 172 | none | | `ChangeLocRequest` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ChangeLocRequest.java:44` |
| c2s | 173 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:84` |
| c2s | 176 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:247` |
| c2s | 179 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:631` |
| c2s | 180 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:615` |
| c2s | 183 | none | | `SelectWorld` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SelectWorld.java:325` |
| c2s | 185 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:421` |
| c2s | 190 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:704` |
| c2s | 191 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:206` |
| c2s | 193 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:99` |
| c2s | 195 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:381` |
| c2s | 196 | none | | `SoftwareFont.calld58221ea` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:982` |
| c2s | 201 | none | | `TextureOpCurve` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/TextureOpCurve.java:42` |
| c2s | 202 | none | | `Cd9f578c5` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/Cd9f578c5.java:81` |
| c2s | 203 | none | | `PlayerEntity` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/PlayerEntity.java:137` |
| c2s | 214 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:515` |
| c2s | 216 | none | | `MalformedLoginPacket.opkind2286` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MalformedLoginPacket.java:66` |
| c2s | 221 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:682` |
| c2s | 223 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:608` |
| c2s | 231 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:148` |
| c2s | 232 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:661` |
| c2s | 236 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:170` |
| c2s | 241 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:57` |
| c2s | 242 | none | | `SoftwareFont.calld58221ea` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:966` |
| c2s | 246 | none | | `ByteArray.opByteArray13` | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:83` |

`LangDe.java:169` also writes opcode 140. `UnderwaterMaterialRenderer.java:129`, `:133`, and `:137` also write opcodes 163, 74, and 2. `ByteArray.java` writes opcode 90 again at lines 216 and 541, and opcode 231 again at line 257.
