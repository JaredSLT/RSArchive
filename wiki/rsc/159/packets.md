# Packets

`size` is `frame` for every row. The length rule is in [framing.md](framing.md). This client has no `-1` or `-2` size table. Field tokens are defined there. The same opcode number is a different packet in each direction.

A handler that reads nothing has an empty field list. Opcodes with no branch in `Client.call7e894e6a` or `MudClient.call5fac7c41` are not rows.

## Client to server

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | frame | u16be version 159, u16be referid, u64be base37 username, rsa password, u32be uid | Client.call25f5d914 | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:136 |
| c2s | 1 | frame | | Client.calleade55a5 | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:213 |
| c2s | 2 | frame | u16be version 159, u64be base37 username, u16be referid, rsa password, u32be uid | Client.call9e187c8f | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:280 |
| c2s | 3 | frame | packed chat bytes | Client.call8d16abda | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:543 |
| c2s | 4 | frame | u64be base37 username | MudClient recovery | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:2302 |
| c2s | 5 | frame | | Client.callb6b49066 | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:335 |
| c2s | 6 | frame | | MudClient.callde2a6a6a | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:1509 |
| c2s | 7 | frame | rawstr of the text after :: | Client.callce3f88fa | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:550 |
| c2s | 8 | frame | u64be username, u32be uid, rsa of old password plus new password, then five times u8 answer length and rsa answer | MudClient recovery submit | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:2372 |
| c2s | 17 | frame | rawstr exception text | MudClient.call5fac7c41 catch | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4464 |
| c2s | 19 | frame | same fields as opcode 0 | Client.call25f5d914 | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:134 |
| c2s | 25 | frame | rsa of formatAuthString username concatenated with formatAuthString password | Client.call9bc8e4cb | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:452 |
| c2s | 26 | frame | u64be friend name | Client.callff5c2039 | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:507 |
| c2s | 27 | frame | u64be friend name | Client.call5664398d | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:514 |
| c2s | 28 | frame | u64be target, packed message | Client.call501d167b | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:535 |
| c2s | 29 | frame | u64be ignore name | Client.calld0f9f609 | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:470 |
| c2s | 30 | frame | u64be ignore name | Client.callabc0a5c9 | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:487 |
| c2s | 31 | frame | u8 chat, u8 private, u8 trade, u8 duel | Client.callcab44984 | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:459 |
| c2s | 51 | frame | u64be reported name, u8 reason | MudClient.callb20c47cc | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:5997 |
| c2s | 194 | frame | u16be world x, u16be world y, then at most 25 pairs of u8 dx and u8 dy | MudClient.call4534748f | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:5439 |
| c2s | 195 | frame | u16be npc index | MudClient.calld81ac5fe menu 725 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9166 |
| c2s | 196 | frame | | MudClient.call5f89248d | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:5849 |
| c2s | 197 | frame | | MudClient options | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:8338 |
| c2s | 198 | frame | | MudClient duel confirm accept | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:7184 |
| c2s | 199 | frame | | MudClient duel accept | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:7332 |
| c2s | 200 | frame | u8 retreat, u8 magic, u8 prayer, u8 weapons | MudClient duel rules | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:7320 |
| c2s | 201 | frame | u8 count, then count times u16be id and u32be amount | MudClient duel offer | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:7249 |
| c2s | 202 | frame | | MudClient trade confirm accept | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6872 |
| c2s | 203 | frame | | MudClient duel decline | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:7190 |
| c2s | 204 | frame | u16be player index | MudClient.calld81ac5fe menu 2806 Duel with | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9217 |
| c2s | 205 | frame | u16be item id, u16be amount | MudClient bank deposit | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6409 |
| c2s | 206 | frame | u16be item id, u16be amount | MudClient bank withdraw | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6367 |
| c2s | 207 | frame | | MudClient bank close | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6456 |
| c2s | 208 | frame | five times: u8 question length, rawstr question, u8 answer length, rsa answer | MudClient security submit | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:1628 |
| c2s | 211 | frame | u8 prayer index | MudClient prayer off | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:8021 |
| c2s | 212 | frame | u8 prayer index | MudClient prayer on | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:8027 |
| c2s | 213 | frame | u8 setting, u8 0 or 1. Setting 0 is flag196, 2 is flag197, 3 is flag198 | MudClient options | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:8304 |
| c2s | 214 | frame | u16be player index | MudClient.calld81ac5fe menu 2820 Follow | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9229 |
| c2s | 215 | frame | same body as opcode 194 | MudClient.call4534748f | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:5437 |
| c2s | 216 | frame | u16be item id, u32be price | MudClient shop sell | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6717 |
| c2s | 217 | frame | u16be item id, u32be price | MudClient shop buy | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6704 |
| c2s | 218 | frame | | MudClient shop close | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6673 |
| c2s | 219 | frame | u16be player index, u16be inventory slot | MudClient.calld81ac5fe menu 810 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9200 |
| c2s | 220 | frame | u16be inventory slot, u16be spell | MudClient.calld81ac5fe menu 600 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9079 |
| c2s | 221 | frame | u16be world x, u16be world y, u16be spell | MudClient.calld81ac5fe menu 900 Cast on ground | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9236 |
| c2s | 222 | frame | u16be world x, u16be world y, u16be loc id | MudClient.calld81ac5fe menu 400 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9038 |
| c2s | 223 | frame | u16be world x, u16be world y, u8 direction, u16be spell | MudClient.calld81ac5fe menu 300 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:8994 |
| c2s | 224 | frame | u16be world x, u16be world y, u16be ground item id, u16be spell | MudClient.calld81ac5fe menu 200 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:8959 |
| c2s | 225 | frame | u16be npc index, u16be spell | MudClient.calld81ac5fe menu 700 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9135 |
| c2s | 226 | frame | u16be player index, u16be spell | MudClient.calld81ac5fe menu 800 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9189 |
| c2s | 227 | frame | u16be spell | MudClient.calld81ac5fe menu 1000 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9252 |
| c2s | 228 | frame | u16be player index | MudClient.calld81ac5fe menu 805 or 2805 Attack | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9211 |
| c2s | 229 | frame | u16be world x, u16be world y, u8 direction | MudClient.calld81ac5fe menu 2300 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9025 |
| c2s | 230 | frame | u16be world x, u16be world y | MudClient.calld81ac5fe menu 2400 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9067 |
| c2s | 231 | frame | u8 style. The writer stores index minus 1, so the first row is byte 255 | MudClient.call409d996b | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:5755 |
| c2s | 232 | frame | | MudClient trade accept | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6988 |
| c2s | 233 | frame | | MudClient trade decline and duel-confirm decline | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6866 |
| c2s | 234 | frame | u8 count, then count times u16be id and u32be amount | MudClient trade offer | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6937 |
| c2s | 235 | frame | u16be player index | MudClient.calld81ac5fe menu 2810 Trade with | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9223 |
| c2s | 236 | frame | eight u8: gender, head, body, legs, hair colour, top colour, bottom colour, skin | MudClient.calla735ea81 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:1955 |
| c2s | 237 | frame | u8 choice index | MudClient.call6cfb0655 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:5733 |
| c2s | 238 | frame | u16be world x, u16be world y, u8 direction | MudClient.calld81ac5fe menu 320 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9016 |
| c2s | 239 | frame | u16be world x, u16be world y, u8 direction, u16be inventory slot | MudClient.calld81ac5fe menu 310 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9005 |
| c2s | 240 | frame | u16be slot, u16be other slot | MudClient.calld81ac5fe menu 610 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9087 |
| c2s | 241 | frame | u16be world x, u16be world y, u16be loc id | MudClient.calld81ac5fe menu 410 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9048 |
| c2s | 242 | frame | u16be world x, u16be world y | MudClient.calld81ac5fe menu 420 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9058 |
| c2s | 243 | frame | u16be npc index, u16be inventory slot | MudClient.calld81ac5fe menu 710 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9146 |
| c2s | 244 | frame | u16be npc index | MudClient.calld81ac5fe menu 715 or 2715 Attack | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9175 |
| c2s | 245 | frame | u16be npc index | MudClient.calld81ac5fe menu 720 Talk-to | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9157 |
| c2s | 246 | frame | u16be inventory slot | MudClient.calld81ac5fe menu 640 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9107 |
| c2s | 247 | frame | | MudClient contact-details request | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:8344 |
| c2s | 248 | frame | u16be inventory slot | MudClient.calld81ac5fe menu 620 Remove | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9095 |
| c2s | 249 | frame | u16be inventory slot | MudClient.calld81ac5fe menu 630 Wield or Wear | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9101 |
| c2s | 250 | frame | u16be world x, u16be world y, u16be ground item id, u16be inventory slot | MudClient.calld81ac5fe menu 210 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:8970 |
| c2s | 251 | frame | u16be inventory slot | MudClient.calld81ac5fe menu 660 Drop | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9119 |
| c2s | 252 | frame | u16be world x, u16be world y, u16be ground item id | MudClient.calld81ac5fe menu 220 Take | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:8981 |
| c2s | 253 | frame | four times u8 length and rawstr: name, postcode, country, email | MudClient.call56734aec | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:1797 |
| c2s | 254 | frame | u16be count, then count times two u16be. A negative list entry writes 1600 plus number55 masked with 1023, then 1 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3282 |

World x and world y on the action packets are the scene-local tile plus `menuAction` or `number484`. Walk opcodes 194 and 215 use that same pair for the destination. Menu 650 Use, 3200 Examine ground item, 3300 Examine boundary, 3400 Examine loc, 3600 Examine inventory, 3700 Examine npc, 4000 Cancel, and 920 Walk here write no second opcode. Walk here is opcode 194 only.

## Server to client

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 8 | frame | rawstr from offset 1 for length minus 1 | Client.call7e894e6a | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:357 |
| s2c | 9 | frame | | Client.call7e894e6a calls calleade55a5 | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:362 |
| s2c | 10 | frame | | Client.tryLogout2, override MudClient.tryLogoutb | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:2437 |
| s2c | 23 | frame | u8 count, then count times u64be name and u8 world | Client.call7e894e6a | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:368 |
| s2c | 24 | frame | u64be name, u8 world | Client.call7e894e6a | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:377 |
| s2c | 26 | frame | u8 count, then count times u64be name | Client.call7e894e6a | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:416 |
| s2c | 27 | frame | i8, i8, i8, i8 stored in number701 through number704 | Client.call7e894e6a | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:404 |
| s2c | 28 | frame | u64be sender, packed from offset 9 for length minus 9 | Client.call7e894e6a | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:409 |
| s2c | 202 | frame | u16be stored in number417. The stats pane draws number417 times 100 divided by 750 as fatigue | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4404 |
| s2c | 203 | frame | rawstr from offset 1, flag312 set | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4393 |
| s2c | 204 | frame | rawstr from offset 1, flag312 cleared | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4386 |
| s2c | 205 | frame | if flag305 is already true, no fields. Otherwise three u32be: last-login days, recovery-change days, last IP | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4374 |
| s2c | 206 | frame | if number433 is already 50, no fields. Otherwise u8 type, i8 x offset, i8 y offset | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4362 |
| s2c | 207 | frame | rawstr sound name | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4356 |
| s2c | 208 | frame | u64be name, u8 their count, that many u16be id and u32be amount, u8 your count, that many u16be id and u32be amount, four u8 rules | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4324 |
| s2c | 209 | frame | i8, 1 sets flag237 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4313 |
| s2c | 210 | frame | i8, 1 sets flag236 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4302 |
| s2c | 211 | frame | u8 skill, u8 current, u8 base, u32be xp | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4292 |
| s2c | 212 | frame | u8 slot. Later slots shift down | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4279 |
| s2c | 213 | frame | u8 slot, u16be id. gsmart count when numberArray483 of the low 15 bits is 0, otherwise count 1. Low 15 bits are the id. The value divided by 32768 is the stack flag | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4256 |
| s2c | 214 | frame | u8 slot, u16be id, gsmart amount. Amount 0 deletes the slot and shifts | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4225 |
| s2c | 215 | frame | four u8: retreat, magic, prayer, weapons | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4195 |
| s2c | 216 | frame | u8 count, then count times u16be id and u32be amount | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4179 |
| s2c | 217 | frame | u64be name, u8 their count, that many u16be and u32be, u8 your count, that many u16be and u32be | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4151 |
| s2c | 218 | frame | | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4145 |
| s2c | 219 | frame | u16be player index | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4127 |
| s2c | 220 | frame | u8 skill, u32be xp | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4121 |
| s2c | 221 | frame | | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4116 |
| s2c | 222 | frame | u8 count, u8 max slots, then count times u16be id and gsmart amount | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4095 |
| s2c | 224 | frame | | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4082 |
| s2c | 226 | frame | value39 bytes. value39 is 39. A byte equal to 1 marks that quest done | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4074 |
| s2c | 227 | frame | one byte per remaining payload byte. A byte equal to 1 marks that prayer on | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4057 |
| s2c | 228 | frame | three u8. 1 sets flag196, flag197, flag198 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4050 |
| s2c | 229 | frame | i8, 1 sets flag264 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4039 |
| s2c | 232 | frame | | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4030 |
| s2c | 233 | frame | | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4025 |
| s2c | 234 | frame | u8 count, i8 general-store flag, u8 number277, u8 number278, then count times u16be id, u16be stock, i8 extra. Flag 1 fills unsold inventory from slot 39 downward and skips item 10 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4407 |
| s2c | 235 | frame | i8, 1 sets flag263 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:4013 |
| s2c | 236 | frame | u8 count, then count times u16be id and u32be amount | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3997 |
| s2c | 237 | frame | | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3991 |
| s2c | 238 | frame | u16be player index | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3977 |
| s2c | 239 | frame | | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3972 |
| s2c | 240 | frame | (length minus 1) divided by 4 pairs of i16be x and i16be y. Each pair is a sector clear of ground items, walls, and bounds | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3897 |
| s2c | 241 | frame | | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3892 |
| s2c | 242 | frame | five u8 into numberArray174 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3884 |
| s2c | 243 | frame | 18 u8 current, 18 u8 base, 18 u32be xp, one u8 into number373 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3864 |
| s2c | 244 | frame | five u16be: number227, number96, errorLoading, number97, number414. Then errorLoading minus number97 times number414 | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3853 |
| s2c | 245 | frame | | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3848 |
| s2c | 246 | frame | u8 count, then count times u8 length and that many raw bytes | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3832 |
| s2c | 247 | frame | u16be count, then per record u16be index and u8 type. Type 1 is u16be speaker, u8 length, packed bytes. Type 2 is three u8. Other types advance nothing | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3788 |
| s2c | 248 | frame | bits, see sync.md | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3693 |
| s2c | 249 | frame | repeated records until the length. u8 255 then two i8 clears one sector of bounds. Otherwise u16be id, i8 x, i8 y, i8 direction. Id 65535 removes only | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3620 |
| s2c | 250 | frame | u16be count, then the type records in sync.md | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3492 |
| s2c | 252 | frame | u8 count, then per slot u16be id and, when the item is unstackable, gsmart count, else count 1. Low 15 bits are the id. Division by 32768 is the stack flag | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3468 |
| s2c | 253 | frame | repeated records. u8 255 then two i8 clears one sector of walls. Otherwise u16be id, i8 x, i8 y. High bit 0x8000 removes. Id 60000 removes and does not add | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3373 |
| s2c | 254 | frame | repeated records. u8 255 then two i8 clears one sector of ground items. Otherwise u16be id, i8 x, i8 y. High bit 0x8000 removes that id | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3301 |
| s2c | 255 | frame | bits, see sync.md. The same handler calls the region load | MudClient.call5fac7c41 | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3167 |

Skill order for opcodes 211, 220, and 243 is the constructor list: Attack, Defense, Strength, Hits, Ranged, Prayer, Magic, Cooking, Woodcut, Fletching, Fishing, Firemaking, Crafting, Smithing, Mining, Herblaw, Carpentry, Thieving (`MudClient.java:9331`). The long names at line 9332 use Woodcutting for the ninth name. `value18` is 18.

Opcode 241 writes `value2502 = 250` and has no further payload bytes. The death message is local (`MudClient.java:4601`).

Ground, wall, and bound coordinates are `number211` or `number98` plus the signed byte, in scene tiles. The 255 record shifts that sum right by 3 and treats it as a sector.
