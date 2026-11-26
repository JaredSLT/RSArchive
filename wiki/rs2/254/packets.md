# Packets

Widths are the `Packet` methods in [framing.md](framing.md): `u8` is `g1`/`p1`, `i8` is `g1b`, `u16be` is `g2`/`p2`, `i16be` is `g2b`, `u24be` is `g3`/`p3`, `u32be` is `g4`/`p4`, `u64be` is `g8`/`p8`, and `jstr` ends with byte 10. S2C `size` is `Protocol.SERVERPROT_LENGTH`. C2S `size` is `none` because this client has no C2S size array. Paths below are under `builds/rs2/254-2004-09-07/src/src/main/java/`.

An S2C opcode with no row here is not a branch in `Client.read`. The payload is still consumed, then the client logs `T1` and calls `logout` (`jagex2/client/Client.java:4005`). Opcode 127 is the only one of those whose table size is not 0. Its field list is empty.

Zone opcodes use a previous `baseX`/`baseZ` from opcode 61, 159, or 173. The tile u8 is `(x << 4) | z` added to that base (`Client.java:7954`). `readZonePacket` does not read the opcode again: `valueClient36` is 581 (`Client.java:746`), which fails the `var3 <= 0` re-read (`Client.java:7948`).

## Server to client

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 0 | 6 | u8 local x, u8 local z, u16be height, u8 speed, u8 acceleration | `Client.read` camera destination | `jagex2/client/Client.java:3364` |
| s2c | 3 | 4 | u16be component, u16be model. Sets model type 2. | `Client.read` | `jagex2/client/Client.java:3506` |
| s2c | 5 | 0 | | `Client.read` opens the chat number input | `jagex2/client/Client.java:3144` |
| s2c | 8 | 7 | u8 tile, u16be obj id, u16be count, u16be pid. Ignored when pid is `localPid`. | `Client.readZonePacket` | `jagex2/client/Client.java:8084` |
| s2c | 14 | 4 | u16be component, u16be scroll | `Client.read` | `jagex2/client/Client.java:3486` |
| s2c | 21 | 0 | | `Client.read` calls `logout` and returns false | `jagex2/client/Client.java:3580` |
| s2c | 24 | 3 | u8 public, u8 private, u8 trade | `Client.read` | `jagex2/client/Client.java:3808` |
| s2c | 25 | 5 | u16be id, u8 loops, u16be delay | `Client.read` | `jagex2/client/Client.java:3741` |
| s2c | 27 | 6 | u16be component, i16be x, i16be y | `Client.read` | `jagex2/client/Client.java:3028` |
| s2c | 28 | -2 | u16be component, u8 count, then count times u16be obj and u8 count. Count 255 is replaced by a following u32be. Remaining slots are set to 0. | `Client.read` | `jagex2/client/Client.java:3056` |
| s2c | 29 | 0 | | `Client.read` sends C2S 142 if `InputTracking.stop` returns a buffer | `jagex2/client/Client.java:3351` |
| s2c | 30 | 4 | u8 tile, u8 shape and rotation (`>> 2` and `& 3`), u16be sequence | `Client.readZonePacket` | `jagex2/client/Client.java:7952` |
| s2c | 37 | 15 | u8 tile, i8 dx, i8 dz, i16be target, u16be spotanim, u8 start height times 4, u8 end height times 4, u16be start delay, u16be end delay, u8 angle, u8 offset | `Client.readZonePacket` | `jagex2/client/Client.java:8046` |
| s2c | 38 | 4 | u16be component, u16be hsl. Converted with 5-bit channels shifted to 19, 11, and 3. | `Client.read` | `jagex2/client/Client.java:3161` |
| s2c | 41 | -2 | u16be component, jstr | `Client.read` | `jagex2/client/Client.java:3796` |
| s2c | 55 | 6 | u8 local x, u8 local z, u16be height, u8 speed, u8 acceleration | `Client.read` camera source | `jagex2/client/Client.java:3327` |
| s2c | 58 | 1 | u8 tab | `Client.read` | `jagex2/client/Client.java:3418` |
| s2c | 60 | -1 | u64be name, u32be message id, u8 rights, then wordpack of `psize - 13` bytes | `Client.read` | `jagex2/client/Client.java:3434` |
| s2c | 61 | -2 | u8 base x, u8 base z, then u8 sub-opcode plus one `readZonePacket` body until `pos == psize` | `Client.read` | `jagex2/client/Client.java:3308` |
| s2c | 63 | -2 | `psize / 8` times u64be | `Client.read` | `jagex2/client/Client.java:3017` |
| s2c | 64 | 6 | u8 type. Type 1 reads u16be npc and leaves the rest unread. Types 2..6 set a local offset, force type 2, then u16be tile x, u16be tile z, u8 height. Type 10 reads u16be player. The table size stays 6. | `Client.read` | `jagex2/client/Client.java:3172` |
| s2c | 70 | 4 | u8 tile, u8 shape and rotation, u16be loc id | `Client.readZonePacket` shared with 88 | `jagex2/client/Client.java:8192` |
| s2c | 73 | -1 | jstr. Suffix `:tradereq:` or `:duelreq:` is a request. This is not a region rebuild. | `Client.read` | `jagex2/client/Client.java:3756` |
| s2c | 75 | 1 | u8 multizone | `Client.read` | `jagex2/client/Client.java:3321` |
| s2c | 85 | 2 | i16be overlay interface | `Client.read` | `jagex2/client/Client.java:3698` |
| s2c | 87 | -2 | Player update. Bit layout is [sync.md](sync.md). | `Client.readPlayerInfo` | `jagex2/client/Client.java:3709` |
| s2c | 88 | 2 | u8 tile, u8 shape and rotation. Loc id is forced to -1. | `Client.readZonePacket` | `jagex2/client/Client.java:8201` |
| s2c | 91 | 3 | u16be component (65535 becomes -1), u8 tab | `Client.read` | `jagex2/client/Client.java:3566` |
| s2c | 94 | 1 | u8 run energy | `Client.read` | `jagex2/client/Client.java:3408` |
| s2c | 95 | 4 | u16be component, i16be anim. -1 clears the sequence frame. | `Client.read` | `jagex2/client/Client.java:3394` |
| s2c | 98 | 7 | u8 tile, u16be obj id, u16be old count, u16be new count | `Client.readZonePacket` | `jagex2/client/Client.java:8169` |
| s2c | 108 | 0 | | `Client.read` sets `flagSceneTileX` to 0 | `jagex2/client/Client.java:3818` |
| s2c | 111 | 9 | u64be name, u8 world | `Client.read` | `jagex2/client/Client.java:3640` |
| s2c | 114 | 6 | u8 tile, u16be spotanim, u8 height, u16be delay | `Client.readZonePacket` | `jagex2/client/Client.java:8070` |
| s2c | 115 | 3 | u8 tile, u16be obj id | `Client.readZonePacket` | `jagex2/client/Client.java:8023` |
| s2c | 120 | 5 | u8 tile, u16be obj id, u16be count | `Client.readZonePacket` | `jagex2/client/Client.java:8005` |
| s2c | 123 | -2 | NPC update. Bit layout is [sync.md](sync.md). | `Client.readNpcInfo` | `jagex2/client/Client.java:3538` |
| s2c | 127 | 6 | | none | `jagex2/client/Protocol.java:6` |
| s2c | 136 | 6 | u8 skill, u32be xp, u8 effective level. Base level is derived from `levelExperience`. | `Client.read` | `jagex2/client/Client.java:3081` |
| s2c | 138 | 1 | u8 side tab | `Client.read` | `jagex2/client/Client.java:3293` |
| s2c | 140 | 0 | | `Client.read` copies `varCache` back into `varps` | `jagex2/client/Client.java:3553` |
| s2c | 141 | 2 | u16be chat interface | `Client.read` | `jagex2/client/Client.java:2904` |
| s2c | 143 | 2 | u16be ticks. Stored times 30. | `Client.read` | `jagex2/client/Client.java:3480` |
| s2c | 146 | 10 | u32be last address, u16be days since login, u8 days since recovery change, u16be unread messages, u8 members warning | `Client.read` | `jagex2/client/Client.java:2974` |
| s2c | 159 | 2 | u8 base x, u8 base z. Clears the 8 by 8 obj square and ends loc changes there. | `Client.read` | `jagex2/client/Client.java:3218` |
| s2c | 161 | 2 | u16be component. Sets model type 3 from the local player's appearance. | `Client.read` | `jagex2/client/Client.java:3241` |
| s2c | 163 | 2 | u16be song. 65535 becomes -1. | `Client.read` | `jagex2/client/Client.java:3586` |
| s2c | 164 | 2 | i16be weight | `Client.read` | `jagex2/client/Client.java:3515` |
| s2c | 167 | 0 | | `Client.read` clears `cutscene` and the five camera modifiers | `jagex2/client/Client.java:2946` |
| s2c | 168 | 2 | u16be component. Each slot is assigned -1 and then immediately 0. | `Client.read` | `jagex2/client/Client.java:3525` |
| s2c | 170 | -2 | u16be component, then while `pos < psize`: u8 slot, u16be obj, u8 count. Count 255 is replaced by a u32be. | `Client.read` | `jagex2/client/Client.java:3270` |
| s2c | 173 | 2 | u8 base x, u8 base z | `Client.read` | `jagex2/client/Client.java:3344` |
| s2c | 174 | 0 | | `Client.read` closes main, side, and chat interfaces | `jagex2/client/Client.java:3617` |
| s2c | 186 | 3 | u16be varp, i8 value | `Client.read` | `jagex2/client/Client.java:3982` |
| s2c | 187 | 2 | u16be side interface | `Client.read` | `jagex2/client/Client.java:3100` |
| s2c | 196 | 6 | u16be varp, u32be value | `Client.read` | `jagex2/client/Client.java:3039` |
| s2c | 197 | 2 | u16be main interface | `Client.read` | `jagex2/client/Client.java:2921` |
| s2c | 203 | 0 | | `Client.read` sets every player and NPC primary sequence to -1 | `jagex2/client/Client.java:3716` |
| s2c | 204 | -1 | u8 slot 1..5, u8 push-down (0 means push down), jstr. The text `null` clears the option. | `Client.read` | `jagex2/client/Client.java:2957` |
| s2c | 209 | 4 | u16be zone x, u16be zone z | `Client.read` | `jagex2/client/Client.java:3824` |
| s2c | 211 | 4 | u16be component, u16be model. Sets model type 1. | `Client.read` | `jagex2/client/Client.java:3544` |
| s2c | 213 | 3 | u16be local pid, u8 members | `Client.read` | `jagex2/client/Client.java:3263` |
| s2c | 218 | 14 | u8 tile, u8 shape and rotation, u16be loc, u16be start, u16be end, u16be pid, i8, i8, i8, i8 bounds | `Client.readZonePacket` | `jagex2/client/Client.java:8104` |
| s2c | 222 | 6 | u16be component, u16be obj, u16be zoom divisor. Sets model type 4. | `Client.read` | `jagex2/client/Client.java:3003` |
| s2c | 225 | 4 | u8 index, u8 jitter, u8 wobble scale, u8 wobble speed | `Client.read` | `jagex2/client/Client.java:3249` |
| s2c | 227 | 3 | u16be component, u8 hidden | `Client.read` | `jagex2/client/Client.java:3733` |
| s2c | 239 | 2 | i16be tutorial layer | `Client.read` | `jagex2/client/Client.java:3153` |
| s2c | 242 | 4 | u16be song, u16be delay | `Client.read` | `jagex2/client/Client.java:3603` |
| s2c | 249 | 4 | u16be main interface, u16be side interface | `Client.read` | `jagex2/client/Client.java:3122` |
| s2c | 251 | 0 | | `Client.read` calls `InputTracking.activate` | `jagex2/client/Client.java:3976` |
| s2c | 255 | 1 | u8 friend-list status | `Client.read` | `jagex2/client/Client.java:3301` |

## Client to server

Walk bodies are in [input.md](input.md). Loc packets written by `interactWithLoc` are u16be world x, u16be world z, u16be loc id (`Client.java:10145`).

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 4 | none | u8 50 | `Client.minimapBuildBuffer` | `jagex2/client/Client.java:9933` |
| c2s | 6 | none | walk, length `2 * n + 3` | `Client.tryMove` | `jagex2/client/Client.java:6806` |
| c2s | 8 | none | u8 1 on focus, u8 0 on blur | `Client.updateGame` | `jagex2/client/Client.java:10682` |
| c2s | 9 | none | u64be name | `Client.addFriend` | `jagex2/client/Client.java:8324` |
| c2s | 13 | none | u8 gender (0 if `designGender`), seven u8 kits, five u8 colours | `Client.handleInterfaceAction` | `jagex2/client/Client.java:1989` |
| c2s | 17 | none | u16be pid | `Client.useMenuOption` | `jagex2/client/Client.java:1372` |
| c2s | 18 | none | u16be pid | `Client.useMenuOption` | `jagex2/client/Client.java:1354` |
| c2s | 26 | none | three loc u16be, then u16be spell | `Client.interactWithLoc` | `jagex2/client/Client.java:1016` |
| c2s | 28 | none | u32be 0. Written before opcode 213 when a counter trips. | `Client.useMenuOption` | `jagex2/client/Client.java:977` |
| c2s | 33 | none | three loc u16be | `Client.interactWithLoc` | `jagex2/client/Client.java:1391` |
| c2s | 36 | none | u8 62 | `Client.addPlayers` | `jagex2/client/Client.java:9949` |
| c2s | 47 | none | three world u16be. Opcode 206 can precede it. | `Client.useMenuOption` | `jagex2/client/Client.java:1144` |
| c2s | 51 | none | u8 0, then u8 length of the following random bytes: optional u16be 11499, u16be 10548, optional u8 139, optional u8 94, u16be 51693, u8 16, u16be 15036, optional u8 65, u8 random, u16be 22990 | `Client.addProjectiles` | `jagex2/client/Client.java:12412` |
| c2s | 56 | none | u32be 0. Written before opcode 97 when a counter trips. | `Client.useMenuOption` | `jagex2/client/Client.java:1113` |
| c2s | 58 | none | | `Client.closeModal` | `jagex2/client/Client.java:2099` |
| c2s | 59 | none | u16be item, u16be slot, u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:1263` |
| c2s | 62 | none | u16be item, u16be slot, u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:1267` |
| c2s | 67 | none | three world u16be | `Client.useMenuOption` | `jagex2/client/Client.java:1121` |
| c2s | 68 | none | u16be pid, u16be spell | `Client.useMenuOption` | `jagex2/client/Client.java:1403` |
| c2s | 69 | none | u16be npc | `Client.useMenuOption` | `jagex2/client/Client.java:993` |
| c2s | 70 | none | u16be item, u16be slot, u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:1259` |
| c2s | 72 | none | u16be pid. Opcode 233 can precede it. | `Client.useMenuOption` | `jagex2/client/Client.java:1171` |
| c2s | 74 | none | u16be item, u16be slot, u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:886` |
| c2s | 77 | none | u16be 37954. Written before opcode 98 when a counter trips. | `Client.useMenuOption` | `jagex2/client/Client.java:1481` |
| c2s | 80 | none | u16be item, u16be slot, u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:890` |
| c2s | 83 | none | u8 length, u8 colour, u8 effect, wordpack. The length covers everything after itself. | `Client.handleInputKey` | `jagex2/client/Client.java:4990` |
| c2s | 84 | none | u64be name | `Client.delFriend` | `jagex2/client/Client.java:5359` |
| c2s | 86 | none | u8 `length - 1`, jstr of the text after `::` | `Client.handleInputKey` | `jagex2/client/Client.java:4914` |
| c2s | 87 | none | three loc u16be | `Client.interactWithLoc` | `jagex2/client/Client.java:1075` |
| c2s | 91 | none | u16be pitch, u16be yaw | `Client.updateGame` | `jagex2/client/Client.java:10675` |
| c2s | 97 | none | three world u16be. Opcode 56 can precede it. | `Client.useMenuOption` | `jagex2/client/Client.java:1117` |
| c2s | 98 | none | three loc u16be. Opcode 77 can precede it. | `Client.interactWithLoc` | `jagex2/client/Client.java:1485` |
| c2s | 100 | none | | `Client.draw3DEntityElements` | `jagex2/client/Client.java:10505` |
| c2s | 102 | none | u16be item, u16be slot, u16be component, u16be spell | `Client.useMenuOption` | `jagex2/client/Client.java:848` |
| c2s | 113 | none | u16be pid, u16be obj component, u16be slot, u16be interface | `Client.useMenuOption` | `jagex2/client/Client.java:1470` |
| c2s | 118 | none | u16be npc | `Client.useMenuOption` | `jagex2/client/Client.java:1005` |
| c2s | 119 | none | u16be npc, u16be obj component, u16be slot, u16be interface | `Client.useMenuOption` | `jagex2/client/Client.java:1313` |
| c2s | 121 | none | u8 131. Written before opcode 192 when a counter trips. | `Client.useMenuOption` | `jagex2/client/Client.java:1177` |
| c2s | 122 | none | u16be npc | `Client.useMenuOption` | `jagex2/client/Client.java:997` |
| c2s | 127 | none | walk, same body as opcode 6 | `Client.tryMove` | `jagex2/client/Client.java:6816` |
| c2s | 129 | none | u8 public, u8 private, u8 trade | `Client.handleInputKey` | `jagex2/client/Client.java:4841` |
| c2s | 131 | none | u16be 6118. Written before opcode 181 when a counter trips. | `Client.useMenuOption` | `jagex2/client/Client.java:1251` |
| c2s | 134 | none | | `Client.checkScene` | `jagex2/client/Client.java:1629` |
| c2s | 141 | none | three world u16be. Opcode 187 can precede it. | `Client.useMenuOption` | `jagex2/client/Client.java:1134` |
| c2s | 142 | none | u16be length, then that many tracking bytes | `Client.updateGame` | `jagex2/client/Client.java:10697` |
| c2s | 143 | none | u16be npc | `Client.useMenuOption` | `jagex2/client/Client.java:1009` |
| c2s | 144 | none | | `Client.updateGame` | `jagex2/client/Client.java:10856` |
| c2s | 146 | none | u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:1330` |
| c2s | 147 | none | three loc u16be | `Client.interactWithLoc` | `jagex2/client/Client.java:1421` |
| c2s | 160 | none | u16be item, u16be slot, u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:1242` |
| c2s | 161 | none | u32be amount | `Client.handleInputKey` | `jagex2/client/Client.java:4878` |
| c2s | 162 | none | u24be 13018169. Written before opcode 163 when a counter trips. | `Client.useMenuOption` | `jagex2/client/Client.java:870` |
| c2s | 163 | none | u16be item, u16be slot, u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:874` |
| c2s | 176 | none | u16be interface, u16be from slot, u16be to slot, u8 mode | `Client.updateGame` | `jagex2/client/Client.java:10789` |
| c2s | 178 | none | three world u16be | `Client.useMenuOption` | `jagex2/client/Client.java:1107` |
| c2s | 181 | none | u16be item, u16be slot, u16be component. Opcode 131 can precede it. | `Client.useMenuOption` | `jagex2/client/Client.java:1255` |
| c2s | 182 | none | | `Client.updateGame` | `jagex2/client/Client.java:10809` |
| c2s | 187 | none | u32be 0. Written before opcode 141 when a counter trips. | `Client.useMenuOption` | `jagex2/client/Client.java:1130` |
| c2s | 189 | none | u64be name | `Client.addIgnore` | `jagex2/client/Client.java:6505` |
| c2s | 192 | none | u16be pid. Opcode 121 can precede it. | `Client.useMenuOption` | `jagex2/client/Client.java:1181` |
| c2s | 193 | none | u64be name | `Client.delIgnore` | `jagex2/client/Client.java:2559` |
| c2s | 195 | none | u16be npc | `Client.useMenuOption` | `jagex2/client/Client.java:1001` |
| c2s | 200 | none | six u16be: item, slot, component, selected obj, selected slot, selected interface | `Client.useMenuOption` | `jagex2/client/Client.java:910` |
| c2s | 201 | none | u8 side tab | `Client.drawGame` | `jagex2/client/Client.java:8865` |
| c2s | 202 | none | u16be x, u16be z, u16be item, u16be spell | `Client.useMenuOption` | `jagex2/client/Client.java:1089` |
| c2s | 203 | none | u64be name, u8 reason (`clientCode - 601`), u8 mute | `Client.handleInterfaceAction` | `jagex2/client/Client.java:1980` |
| c2s | 206 | none | u8 19. Written before opcode 47 when a counter trips. | `Client.useMenuOption` | `jagex2/client/Client.java:1140` |
| c2s | 213 | none | three loc u16be. Opcode 28 can precede it. | `Client.interactWithLoc` | `jagex2/client/Client.java:981` |
| c2s | 214 | none | u8 length, u64be name, wordpack. The length covers the name and the wordpack. | `Client.handleInputKey` | `jagex2/client/Client.java:4829` |
| c2s | 220 | none | walk, length `2 * n + 3 + 14`, then the 14-byte minimap trailer | `Client.tryMove` | `jagex2/client/Client.java:6812` |
| c2s | 225 | none | u8 0, then u8 length of: optional u16be 16791, u8 254, u16be random, u16be 16128, u16be 52610, u16be random, u16be 55420, optional u16be 35025, u16be 46628, u8 random | `Client.interactWithLoc` | `jagex2/client/Client.java:10097` |
| c2s | 226 | none | u8 232 | `Client.handleInputKey` | `jagex2/client/Client.java:4786` |
| c2s | 228 | none | u16be item, u16be slot, u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:878` |
| c2s | 230 | none | u16be pid | `Client.useMenuOption` | `jagex2/client/Client.java:1368` |
| c2s | 231 | none | u16be npc, u16be spell | `Client.useMenuOption` | `jagex2/client/Client.java:1456` |
| c2s | 232 | none | u8 length, then the sample words in [input.md](input.md) | `Client.updateGame` | `jagex2/client/Client.java:10565` |
| c2s | 233 | none | u8 154. Written before opcode 72 when a counter trips. | `Client.useMenuOption` | `jagex2/client/Client.java:1167` |
| c2s | 234 | none | u32be packed click | `Client.updateGame` | `jagex2/client/Client.java:10660` |
| c2s | 239 | none | | `Client.updateGame` and `Client.buildScene` | `jagex2/client/Client.java:10931` |
| c2s | 240 | none | three loc u16be, then three item u16be | `Client.interactWithLoc` | `jagex2/client/Client.java:1196` |
| c2s | 243 | none | u16be item, u16be slot, u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:882` |
| c2s | 244 | none | u16be component | `Client.useMenuOption` | `jagex2/client/Client.java:938` |
| c2s | 245 | none | u16be x, u16be z, u16be loc, u16be obj component, u16be slot, u16be interface | `Client.useMenuOption` | `jagex2/client/Client.java:1296` |
