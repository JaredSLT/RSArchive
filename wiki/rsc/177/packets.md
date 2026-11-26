# Packets

Every row is an opcode this client reads or writes. Size is `variable` because this client has no opcode size array. The framed length is the rule in [framing](framing.md). Widths are defined there.

A c2s citation is the `p1spooky(opcode, threat)` line. The second integer is the cipher threat, not a payload byte and not a size. Payload fields follow the opcode byte.

Social opcodes 8, 9, 10, 23, 24, 26, 27, and 28 are handled in `Client.call7e894e6a` before `size`. Every other decoded opcode goes to `MudClient.call5fac7c41`. An opcode with no branch there is ignored. This page does not invent rows for those.

Menu action ids are the `menuAction` values in `calld81ac5fe` (`deob/MudClient.java:14797`). Walk here is action 920 and sends opcode 194 through the pathfinder, not a packet of its own.

## Client to server

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | variable | u16be version, u16be referid, u64be username, rsa(password), u32be uid | Client.call25f5d914 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:136 |
| c2s | 1 | variable | empty | Client.calleade55a5 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:216 |
| c2s | 2 | variable | u16be version, u64be username, u16be referid, rsa(password), u32be uid | Client.call9e187c8f | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:282 |
| c2s | 3 | variable | pdata(wordpack bytes) | Client.call8d16abda | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:611 |
| c2s | 4 | variable | u64be username | MudClient login screen | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:2542 |
| c2s | 5 | variable | empty | Client.callb6b49066 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:338 |
| c2s | 6 | variable | empty | MudClient.callde2a6a6a | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:1574 |
| c2s | 7 | variable | rawstr | Client.callce3f88fa | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:618 |
| c2s | 8 | variable | u64be username, u32be uid, rsa(old answer + new password), then 5 times (u8 answerLen, rsa(answer)) | MudClient recovery | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:2625 |
| c2s | 17 | variable | rawstr | MudClient.call5fac7c41 catch | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7908 |
| c2s | 19 | variable | same payload as opcode 0 | Client.call25f5d914 reconnect | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:130 |
| c2s | 25 | variable | rsa(username + password) | Client.call9bc8e4cb | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:484 |
| c2s | 26 | variable | u64be name | Client.callff5c2039 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:556 |
| c2s | 27 | variable | u64be name | Client.call5664398d | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:564 |
| c2s | 28 | variable | u64be name, pdata(wordpack) | Client.call501d167b | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:603 |
| c2s | 29 | variable | u64be name | Client.calld0f9f609 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:502 |
| c2s | 30 | variable | u64be name | Client.callabc0a5c9 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:530 |
| c2s | 31 | variable | u8 chat, u8 private, u8 trade, u8 duel | Client.callcab44984 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:491 |
| c2s | 51 | variable | u64be name, u8 reason | MudClient report | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:10209 |
| c2s | 193 | variable | rawstr sleep word, or the literal `-null-` | MudClient sleep input | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:3113 |
| c2s | 194 | variable | u16be x+origin, u16be y+origin, then at most 25 of (u8 dx, u8 dy) | MudClient.call4534748f | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:9647 |
| c2s | 195 | variable | u16be npc index. Menu 725 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15011 |
| c2s | 196 | variable | empty | MudClient welcome cancel | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:10051 |
| c2s | 197 | variable | empty | MudClient options panel | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:13907 |
| c2s | 198 | variable | empty. Sets flag86 on the duel confirm | MudClient duel confirm | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:12239 |
| c2s | 199 | variable | empty | MudClient duel UI | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:12480 |
| c2s | 200 | variable | u8 flag81, u8 flag82, u8 flag83, u8 flag84 | MudClient duel UI | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:12468 |
| c2s | 201 | variable | u8 count, count times (u16be item, u32be amount) | MudClient duel offer | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:12291 |
| c2s | 202 | variable | empty. Sets flag91 on the trade confirm | MudClient trade confirm | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:11727 |
| c2s | 203 | variable | empty | MudClient decline | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:12245 |
| c2s | 204 | variable | u16be player index. Menu 2806 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15061 |
| c2s | 205 | variable | u16be item, u16be amount | MudClient bank withdraw | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:10701 |
| c2s | 206 | variable | u16be item, u16be amount | MudClient bank deposit | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:10659 |
| c2s | 207 | variable | empty. Closes the bank flag | MudClient bank close | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:11251 |
| c2s | 208 | variable | 5 times (u8 questionLen, rawstr question, u8 answerLen, rsa(answer)) | MudClient recovery questions | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:1784 |
| c2s | 211 | variable | u8 prayer index | MudClient prayer off | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:13466 |
| c2s | 212 | variable | u8 prayer index | MudClient prayer on | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:13476 |
| c2s | 213 | variable | u8 setting id, u8 value. Id 0 camera, id 2 one-button mouse, id 3 sound. No id 1 at these call sites | MudClient options | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:13873 |
| c2s | 214 | variable | u16be player index. Menu 2820 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15073 |
| c2s | 215 | variable | same body as opcode 194 | MudClient.call4534748f | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:9641 |
| c2s | 216 | variable | u16be item, u32be amount | MudClient shop sell | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:11310 |
| c2s | 217 | variable | u16be item, u32be amount | MudClient shop buy | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:11297 |
| c2s | 218 | variable | empty. Clears the shop flag | MudClient shop close | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:11637 |
| c2s | 219 | variable | u16be, u16be. Menu 810 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15044 |
| c2s | 220 | variable | u16be, u16be. Menu 600 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14924 |
| c2s | 221 | variable | u16be x+origin, u16be y+origin, u16be. Menu 900 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15080 |
| c2s | 222 | variable | u16be x+origin, u16be y+origin, u16be loc id. Menu 400 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14885 |
| c2s | 223 | variable | u16be x+origin, u16be y+origin, u8 direction, u16be. Menu 300 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14841 |
| c2s | 224 | variable | u16be x+origin, u16be y+origin, u16be, u16be. Menu 200 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14806 |
| c2s | 225 | variable | u16be, u16be. Menu 700 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14980 |
| c2s | 226 | variable | u16be, u16be. Menu 800 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15033 |
| c2s | 227 | variable | u16be. Menu 1000 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15096 |
| c2s | 228 | variable | u16be player index. Menu 805 and 2805 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15055 |
| c2s | 229 | variable | u16be x+origin, u16be y+origin, u8 direction. Menu 2300 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14872 |
| c2s | 230 | variable | u16be x+origin, u16be y+origin. Menu 2400 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14913 |
| c2s | 231 | variable | u8 style | MudClient combat style | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:9922 |
| c2s | 232 | variable | empty | MudClient trade accept | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:11936 |
| c2s | 233 | variable | empty. Clears flag90 on the trade confirm. Also sent from other trade and duel clicks | MudClient trade confirm | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:11721 |
| c2s | 234 | variable | u8 count, count times (u16be item, u32be amount) | MudClient trade offer | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:11779 |
| c2s | 235 | variable | u16be player index. Menu 2810 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15067 |
| c2s | 236 | variable | 8 times u8: gender, head, top, bottom, hair color, top color, bottom color, skin color | MudClient appearance | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:2141 |
| c2s | 237 | variable | u8 row | MudClient choice list | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:9878 |
| c2s | 238 | variable | u16be x+origin, u16be y+origin, u8 direction. Menu 320 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14863 |
| c2s | 239 | variable | u16be x+origin, u16be y+origin, u8 direction, u16be. Menu 310 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14852 |
| c2s | 240 | variable | u16be, u16be. Menu 610 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14932 |
| c2s | 241 | variable | u16be x+origin, u16be y+origin, u16be loc id. Menu 410 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14895 |
| c2s | 242 | variable | u16be x+origin, u16be y+origin. Menu 420 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14905 |
| c2s | 243 | variable | u16be, u16be. Menu 710 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14991 |
| c2s | 244 | variable | u16be npc index. Menu 715 and 2715 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15020 |
| c2s | 245 | variable | u16be npc index. Menu 720 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15002 |
| c2s | 246 | variable | u16be slot. Menu 640 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14952 |
| c2s | 247 | variable | empty | MudClient options panel | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:13913 |
| c2s | 248 | variable | u16be slot. Menu 620 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14940 |
| c2s | 249 | variable | u16be slot. Menu 630 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14946 |
| c2s | 250 | variable | u16be x+origin, u16be y+origin, u16be, u16be. Menu 210 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14817 |
| c2s | 251 | variable | u16be slot. Menu 660 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14964 |
| c2s | 252 | variable | u16be x+origin, u16be y+origin, u16be item. Menu 220 | MudClient.calld81ac5fe | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14828 |
| c2s | 253 | variable | 4 times (u8 len, rawstr) | MudClient contact | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:1972 |
| c2s | 254 | variable | u16be count, count times (u16be server index, u16be appearance generation) | MudClient opcode 255 tail | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7881 |

Menu actions with no packet: 650, 920, 3200, 3300, 3400, 3600, 3700, 4000 (`deob/MudClient.java:14957`, `deob/MudClient.java:15088`).

## Server to client

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 8 | variable | bytes(length-1) from offset 1 | Client.call7e894e6a | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:362 |
| s2c | 9 | variable | empty. Sends c2s opcode 1 | Client.call7e894e6a | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:367 |
| s2c | 10 | variable | empty. MudClient.tryLogout only prints that logout is refused | Client.call7e894e6a | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:371 |
| s2c | 23 | variable | u8 count, count times (u64be name, u8 world) at stride 9 from offset 2 | Client.call7e894e6a | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:436 |
| s2c | 24 | variable | u64be name at offset 1, u8 world at offset 9 | Client.call7e894e6a | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:400 |
| s2c | 26 | variable | u8 count at offset 1, count times u64be from offset 2 stride 8 | Client.call7e894e6a | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:389 |
| s2c | 27 | variable | u8 chat, u8 private, u8 trade, u8 duel at offsets 1..4 | Client.call7e894e6a | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:376 |
| s2c | 28 | variable | u64be name at offset 1, WordPack bytes from offset 9 length length-9 | Client.call7e894e6a | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:381 |
| s2c | 198 | variable | empty. Sets the sleep failure text | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7569 |
| s2c | 199 | variable | empty. Clears flag104 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7564 |
| s2c | 200 | variable | u16be fatigue `number448` | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7559 |
| s2c | 201 | variable | bytes of the framed buffer, including the opcode byte, passed to drawArea.call378. The image codec inside call378 is unread | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7546 |
| s2c | 202 | variable | u16be `number447` | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7541 |
| s2c | 203 | variable | bytes(length-1) from offset 1. Dialog, continue style | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7534 |
| s2c | 204 | variable | bytes(length-1) from offset 1. Dialog | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7527 |
| s2c | 205 | variable | u32be, u32be, u32be. Read only when flag95 is false | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7515 |
| s2c | 206 | variable | u8 sprite, u8 dx, u8 dy added to the region. Read only when number571 is below 50 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7503 |
| s2c | 207 | variable | bytes(length-1) from offset 1, a sound name | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7497 |
| s2c | 208 | variable | u64be, u8 count, that many (u16be, u32be), u8 count, that many (u16be, u32be), four u8 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7447 |
| s2c | 209 | variable | u8 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7436 |
| s2c | 210 | variable | u8 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7425 |
| s2c | 211 | variable | u8 skill, u8 current, u8 base, u32be xp | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7415 |
| s2c | 212 | variable | u8 slot. Shifts the inventory arrays down | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7397 |
| s2c | 213 | variable | u8 slot, u16be id. Worn is id/32768 and the stored id is id&32767. gsmart count when objStackable of that id is 0, else count 1 and no count bytes | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7576 |
| s2c | 214 | variable | u8 slot, u16be id, gsmart amount. Amount 0 removes the slot and shifts | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7348 |
| s2c | 215 | variable | four u8, each compared to 1 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7298 |
| s2c | 216 | variable | u8 count, count times (u16be, u32be) | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7275 |
| s2c | 217 | variable | u64be, u8 count, that many (u16be, u32be), u8 count, that many (u16be, u32be) | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7233 |
| s2c | 218 | variable | empty | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7227 |
| s2c | 219 | variable | u16be player index | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7209 |
| s2c | 220 | variable | u8 skill, u32be xp | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7203 |
| s2c | 221 | variable | empty | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7198 |
| s2c | 222 | variable | u8 count, u8 max, count times (u16be id, gsmart amount) | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7167 |
| s2c | 224 | variable | empty. Shows five recovery questions from the client table | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7149 |
| s2c | 226 | variable | 50 bytes. A byte equal to 1 sets the quest flag. 50 is `value50` | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7135 |
| s2c | 227 | variable | (length-1) bytes from offset 1. A byte equal to 1 means the prayer is on | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7112 |
| s2c | 228 | variable | u8 camera, u8 mouse, u8 sound. 1 means true | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7105 |
| s2c | 229 | variable | u8 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7094 |
| s2c | 232 | variable | empty. Shows the contact form | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7085 |
| s2c | 233 | variable | empty | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7080 |
| s2c | 234 | variable | u8 count, u8 flag, u8, u8, count times (u16be id, u16be, u8). Flag 1 also copies inventory ids other than 10 into the remaining 40 slots | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6994 |
| s2c | 235 | variable | u8 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6983 |
| s2c | 236 | variable | u8 count, count times (u16be, u32be) | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6960 |
| s2c | 237 | variable | empty | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6954 |
| s2c | 238 | variable | u16be player index | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6940 |
| s2c | 239 | variable | empty | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6935 |
| s2c | 240 | variable | ((length-1)/4) times (i16be, i16be) | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6764 |
| s2c | 241 | variable | empty. Sets value250 to 250 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6759 |
| s2c | 242 | variable | 5 times u8 | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6745 |
| s2c | 243 | variable | 18 u8 current, 18 u8 base, 18 u32be xp, u8 `number446` | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6704 |
| s2c | 244 | variable | five u16be, then the client subtracts the product of the fourth and fifth from the third | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6693 |
| s2c | 245 | variable | empty | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6688 |
| s2c | 246 | variable | u8 count, count times (u8 len, bytes(len)) | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6667 |
| s2c | 247 | variable | u16be count, then the records in [sync](sync.md) | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6611 |
| s2c | 248 | variable | bit payload in [sync](sync.md). No separate size table | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6409 |
| s2c | 249 | variable | repeated until the cursor reaches length. u8 255 plus two i8 clears a sector. Else u16be id, i8 dx, i8 dy, i8 direction. Id 65535 deletes | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6302 |
| s2c | 250 | variable | u16be count, then the records in [sync](sync.md) | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6105 |
| s2c | 252 | variable | u8 count, then count times (u16be id, worn = id/32768, id &= 32767, gsmart count only when objStackable[id] is 0, else count 1) | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6059 |
| s2c | 253 | variable | same loop shape as opcode 249. Record is u16be id, i8 dx, i8 dy. Id 60000 deletes. No direction byte | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:5879 |
| s2c | 254 | variable | same loop shape. Record is u16be id, i8 dx, i8 dy. High bit set removes id&32767. High bit clear adds | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:5770 |
| s2c | 255 | variable | bit payload in [sync](sync.md). This is the else of `var1 != 255` | MudClient.call5fac7c41 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:5769 |

Server opcodes 223, 225, 230, 231, and 251 are not branched. They are ignored.
