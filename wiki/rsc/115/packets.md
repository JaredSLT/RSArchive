# Packets

`size` is `variable` on every row. This client has no opcode size table (`framing.md`). The framed length includes the opcode byte. Widths are the reads and writes in the cited method. `u16be`, `u32be`, and `u64be` are big-endian. `i8` is a Java signed byte. `i16be` is `Tools.g2s`. `gsmart` is `Tools.gsmart` with the 1-or-4 advance in `framing.md`. `pjstr` is raw bytes with no terminator. `rsaenc` and `call146` are the blocks in `login.md` and `framing.md`.

An opcode the switch does not name is ignored after the frame is consumed (`MudClient.call5fac7c41` returns without a field read when the opcode is not 208 and none of the earlier branches match, `deob/MudClient.java:4010`).

The same opcode number can be client-to-server and server-to-client. Those are separate rows.

## Client to server

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | variable | u16be version, u64be base37 username, rsaenc(password), u32be uid | `Client.call25f5d914` | `builds/rsc/115-2001-12-24/src/src/main/java/jagex/client/Client.java:128` |
| c2s | 1 | variable | empty | `Client.calleade55a5` | `jagex/client/Client.java:204` |
| c2s | 2 | variable | u16be version, u64be username, rsaenc(password), u32be uid | `Client.call9e187c8f` | `jagex/client/Client.java:261` |
| c2s | 3 | variable | bytes of `Tools.call3cc33792` (dictionary, `cache.md`) | `Client.call8d16abda` | `jagex/client/Client.java:519` |
| c2s | 4 | variable | u64be username | `MudClient` recovery | `deob/MudClient.java:1987` |
| c2s | 5 | variable | empty | `Client.callb6b49066` | `jagex/client/Client.java:311` |
| c2s | 6 | variable | empty | `MudClient.callde2a6a6a` | `deob/MudClient.java:1300` |
| c2s | 7 | variable | pjstr of the text after `::` | `Client.callce3f88fa` | `jagex/client/Client.java:526` |
| c2s | 8 | variable | u64be username, u32be uid, rsaenc(old20 + new20), five `call146` answer hashes | `MudClient` recovery | `deob/MudClient.java:2068` |
| c2s | 17 | variable | pjstr | `MudClient.call5fac7c41` catch | `deob/MudClient.java:4049` |
| c2s | 19 | variable | same fields as opcode 0 | `Client.call25f5d914` | `jagex/client/Client.java:126` |
| c2s | 25 | variable | rsaenc(user20 + pass20) | `Client.call9bc8e4cb` | `jagex/client/Client.java:428` |
| c2s | 26 | variable | u64be name | `Client.callff5c2039` | `jagex/client/Client.java:483` |
| c2s | 27 | variable | u64be name | `Client.call5664398d` | `jagex/client/Client.java:490` |
| c2s | 28 | variable | u64be target, dictionary bytes | `Client.call501d167b` | `jagex/client/Client.java:511` |
| c2s | 29 | variable | u64be name | `Client.calld0f9f609` | `jagex/client/Client.java:446` |
| c2s | 30 | variable | u64be name | `Client.callabc0a5c9` | `jagex/client/Client.java:463` |
| c2s | 31 | variable | u8 chat, u8 pm, u8 trade, u8 duel | `Client.callcab44984` | `jagex/client/Client.java:435` |
| c2s | 198 | variable | empty | `MudClient` duel confirm | `deob/MudClient.java:6359` |
| c2s | 199 | variable | empty | `MudClient.call49f3495a` | `deob/MudClient.java:6502` |
| c2s | 200 | variable | u8 retreat, u8 magic, u8 prayer, u8 weapons | `MudClient.call49f3495a` | `deob/MudClient.java:6490` |
| c2s | 201 | variable | u8 count, then count times (u16be id, u32be amount) | `MudClient.call49f3495a` | `deob/MudClient.java:6419` |
| c2s | 202 | variable | empty | `MudClient` trade confirm | `deob/MudClient.java:6052` |
| c2s | 203 | variable | empty | `MudClient.call49f3495a` | `deob/MudClient.java:6365` |
| c2s | 204 | variable | u16be | `MudClient.calld81ac5fe` menu 2806 | `deob/MudClient.java:8304` |
| c2s | 205 | variable | u16be id, u16be amount (1, 5, 25, 100, 500, or 2500) | `MudClient.call8bbb8887` | `deob/MudClient.java:5633` |
| c2s | 206 | variable | u16be id, u16be amount (same set) | `MudClient.call8bbb8887` | `deob/MudClient.java:5591` |
| c2s | 207 | variable | empty | `MudClient.call8bbb8887` | `deob/MudClient.java:5554` |
| c2s | 208 | variable | five times (u8 length, pjstr question, `call146` answer hash) | `MudClient.call7e1875fa` | `deob/MudClient.java:1410` |
| c2s | 211 | variable | u8 prayer | `MudClient` prayer list | `deob/MudClient.java:7189` |
| c2s | 212 | variable | u8 prayer | `MudClient` prayer list | `deob/MudClient.java:7194` |
| c2s | 213 | variable | u8 which, u8 value. Camera writes 0 then 0 or 1. Mouse writes 2 then 0 or 1 | `MudClient` options | `deob/MudClient.java:7441` |
| c2s | 214 | variable | u16be | `MudClient.calld81ac5fe` menu 2820 | `deob/MudClient.java:8316` |
| c2s | 215 | variable | u16be destX + origin, u16be destY + origin, then up to 25 times (u8 dx, u8 dy) | `MudClient.call4534748f` | `deob/MudClient.java:4964` |
| c2s | 216 | variable | u16be id, u16be price | `MudClient.callda3855f9` | `deob/MudClient.java:5897` |
| c2s | 217 | variable | u16be id, u16be price | `MudClient.callda3855f9` | `deob/MudClient.java:5884` |
| c2s | 218 | variable | empty | `MudClient.callda3855f9` | `deob/MudClient.java:5853` |
| c2s | 219 | variable | u16be, u16be | `MudClient.calld81ac5fe` menu 810 | `deob/MudClient.java:8287` |
| c2s | 220 | variable | u16be, u16be | `MudClient.calld81ac5fe` menu 600 | `deob/MudClient.java:8176` |
| c2s | 221 | variable | u16be x + origin, u16be y + origin, u16be | `MudClient.calld81ac5fe` menu 900 | `deob/MudClient.java:8323` |
| c2s | 222 | variable | u16be x + origin, u16be y + origin, u16be | `MudClient.calld81ac5fe` menu 400 | `deob/MudClient.java:8137` |
| c2s | 223 | variable | u16be x + origin, u16be y + origin, u8 direction, u16be | `MudClient.calld81ac5fe` menu 300 | `deob/MudClient.java:8093` |
| c2s | 224 | variable | u16be x + origin, u16be y + origin, u16be, u16be | `MudClient.calld81ac5fe` menu 200 | `deob/MudClient.java:8058` |
| c2s | 225 | variable | u16be, u16be | `MudClient.calld81ac5fe` menu 700 | `deob/MudClient.java:8232` |
| c2s | 226 | variable | u16be, u16be | `MudClient.calld81ac5fe` menu 800 | `deob/MudClient.java:8276` |
| c2s | 227 | variable | u16be | `MudClient.calld81ac5fe` menu 1000 | `deob/MudClient.java:8339` |
| c2s | 228 | variable | u16be | `MudClient.calld81ac5fe` menu 805 or 2805 | `deob/MudClient.java:8298` |
| c2s | 229 | variable | u16be x + origin, u16be y + origin, u8 direction | `MudClient.calld81ac5fe` menu 2300 | `deob/MudClient.java:8124` |
| c2s | 230 | variable | u16be x + origin, u16be y + origin | `MudClient.calld81ac5fe` menu 2400 | `deob/MudClient.java:8165` |
| c2s | 231 | variable | u8 style. Rows 1..4 send 0..3. Row 0 sends nothing | `MudClient.call409d996b` | `deob/MudClient.java:5276` |
| c2s | 232 | variable | empty | `MudClient.call9dfb1410` | `deob/MudClient.java:6163` |
| c2s | 233 | variable | empty | trade, trade confirm, duel confirm, and duel decline sites | `deob/MudClient.java:6046` |
| c2s | 234 | variable | u8 count, then count times (u16be id, u32be amount) | `MudClient.call9dfb1410` | `deob/MudClient.java:6112` |
| c2s | 235 | variable | u16be | `MudClient.calld81ac5fe` menu 2810 | `deob/MudClient.java:8310` |
| c2s | 236 | variable | nine u8: `number391`, `number384`, `number385`, `number386`, `number387`, `number388`, `number389`, `number390`, then `Gui.call249(number338)` | `MudClient.calla735ea81` | `deob/MudClient.java:1654` |
| c2s | 237 | variable | u8 index | `MudClient.call6cfb0655` | `deob/MudClient.java:5254` |
| c2s | 238 | variable | u16be x + origin, u16be y + origin, u8 direction | `MudClient.calld81ac5fe` menu 320 | `deob/MudClient.java:8115` |
| c2s | 239 | variable | u16be x + origin, u16be y + origin, u8 direction, u16be | `MudClient.calld81ac5fe` menu 310 | `deob/MudClient.java:8104` |
| c2s | 240 | variable | u16be, u16be | `MudClient.calld81ac5fe` menu 610 | `deob/MudClient.java:8184` |
| c2s | 241 | variable | u16be x + origin, u16be y + origin, u16be | `MudClient.calld81ac5fe` menu 410 | `deob/MudClient.java:8147` |
| c2s | 242 | variable | u16be x + origin, u16be y + origin | `MudClient.calld81ac5fe` menu 420 | `deob/MudClient.java:8157` |
| c2s | 243 | variable | u16be, u16be | `MudClient.calld81ac5fe` menu 710 | `deob/MudClient.java:8243` |
| c2s | 244 | variable | u16be | `MudClient.calld81ac5fe` menu 715 or 2715 | `deob/MudClient.java:8263` |
| c2s | 245 | variable | u16be | `MudClient.calld81ac5fe` menu 720 | `deob/MudClient.java:8254` |
| c2s | 246 | variable | u16be | `MudClient.calld81ac5fe` menu 640 | `deob/MudClient.java:8204` |
| c2s | 248 | variable | u16be | `MudClient.calld81ac5fe` menu 620 | `deob/MudClient.java:8192` |
| c2s | 249 | variable | u16be | `MudClient.calld81ac5fe` menu 630 | `deob/MudClient.java:8198` |
| c2s | 250 | variable | u16be x + origin, u16be y + origin, u16be, u16be | `MudClient.calld81ac5fe` menu 210 | `deob/MudClient.java:8069` |
| c2s | 251 | variable | u16be slot | `MudClient.calld81ac5fe` menu 660 | `deob/MudClient.java:8216` |
| c2s | 252 | variable | u16be x + origin, u16be y + origin, u16be | `MudClient.calld81ac5fe` menu 220 | `deob/MudClient.java:8080` |
| c2s | 254 | variable | u16be count, then count times (u16be `entity.number11`, u16be `entity.number12`) | `MudClient.call5fac7c41` | `deob/MudClient.java:2943` |
| c2s | 255 | variable | same body as opcode 215 | `MudClient.call4534748f` | `deob/MudClient.java:4966` |

Walk dx and dy are `p1(waypoint - dest)`, the low 8 bits of the subtraction (`MudClient.java:4973`). Opcode 215 is `var8` true. Opcode 255 is `var8` false. Menu 900 sends opcode 215 and then opcode 221 (`MudClient.java:8322`).

The constructor assigns `number391 = 1`, `number385 = 1`, `number386 = 2`, `number387 = 2`, `number388 = 8`, `number389 = 14` (`MudClient.java:8520`). `number384` and `number390` stay at the Java 0 default.

## Server to client

`Client.call7e894e6a` handles 8, 9, 10, 23, 24, 26, 27, and 28 (`jagex/client/Client.java:332`). Every other opcode goes to `size`, which `MudClient` overrides as `call5fac7c41` (`deob/MudClient.java:2831`).

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 8 | variable | bytes from offset 1, length `payload - 1`, raw `new String` | `Client.call7e894e6a` | `jagex/client/Client.java:333` |
| s2c | 9 | variable | empty | `Client.calleade55a5` | `jagex/client/Client.java:338` |
| s2c | 10 | variable | empty | `MudClient.tryLogoutb` | `jagex/client/Client.java:342`, `deob/MudClient.java:2129` |
| s2c | 23 | variable | u8 count, then count times (u64be name, u8 world) | `Client.call7e894e6a` | `jagex/client/Client.java:344` |
| s2c | 24 | variable | u64be name, u8 world | `Client.call7e894e6a` | `jagex/client/Client.java:353` |
| s2c | 26 | variable | u8 count, then count times u64be | `Client.call7e894e6a` | `jagex/client/Client.java:392` |
| s2c | 27 | variable | u8 chat, u8 pm, u8 trade, u8 duel | `Client.call7e894e6a` | `jagex/client/Client.java:380` |
| s2c | 28 | variable | u64be sender, dictionary bytes from offset 9, length `payload - 9` | `Client.call7e894e6a` | `jagex/client/Client.java:385` |
| s2c | 208 | variable | u64be name, u8 our count, count times (u16be, u32be), u8 their count, count times (u16be, u32be), four u8 rules | `MudClient.call5fac7c41` | `deob/MudClient.java:4014` |
| s2c | 209 | variable | u8 | `MudClient.call5fac7c41` | `deob/MudClient.java:3999` |
| s2c | 210 | variable | u8 | `MudClient.call5fac7c41` | `deob/MudClient.java:3988` |
| s2c | 211 | variable | u8 skill, u8 current, u8 base, u32be xp | `MudClient.call5fac7c41` | `deob/MudClient.java:3978` |
| s2c | 212 | variable | u8 slot | `MudClient.call5fac7c41` | `deob/MudClient.java:3965` |
| s2c | 213 | variable | u8 slot, u16be id (low 15 bits stored, value / 32768 stored separately), gsmart amount when `ClientConfig.numberArray440[id] == 0`, otherwise amount 1 | `MudClient.call5fac7c41` | `deob/MudClient.java:3942` |
| s2c | 214 | variable | u8 slot, u16be id, gsmart amount. Amount 0 deletes the slot and shifts | `MudClient.call5fac7c41` | `deob/MudClient.java:3911` |
| s2c | 215 | variable | four u8, each compared with 1 | `MudClient.call5fac7c41` | `deob/MudClient.java:3881` |
| s2c | 216 | variable | u8 count, then count times (u16be, u32be) | `MudClient.call5fac7c41` | `deob/MudClient.java:3865` |
| s2c | 217 | variable | u64be name, u8 our count, count times (u16be, u32be), u8 their count, count times (u16be, u32be) | `MudClient.call5fac7c41` | `deob/MudClient.java:3837` |
| s2c | 218 | variable | empty | `MudClient.call5fac7c41` | `deob/MudClient.java:3831` |
| s2c | 219 | variable | u16be index | `MudClient.call5fac7c41` | `deob/MudClient.java:3813` |
| s2c | 220 | variable | u8 skill, u32be xp | `MudClient.call5fac7c41` | `deob/MudClient.java:3807` |
| s2c | 221 | variable | empty | `MudClient.call5fac7c41` | `deob/MudClient.java:3802` |
| s2c | 222 | variable | u8 count, then count times (u16be id, gsmart amount) | `MudClient.call5fac7c41` | `deob/MudClient.java:3782` |
| s2c | 224 | variable | empty | `MudClient.call5fac7c41` | `deob/MudClient.java:3769` |
| s2c | 226 | variable | 17 bytes, one per `value17`, each compared with 1 | `MudClient.call5fac7c41` | `deob/MudClient.java:3761`, `value17` at `MudClient.java:8420` |
| s2c | 227 | variable | one byte per remaining payload byte, compared with 1 | `MudClient.call5fac7c41` | `deob/MudClient.java:3753` |
| s2c | 228 | variable | u8 camera auto, u8 one-button mouse, each compared with 1 | `MudClient.call5fac7c41` | `deob/MudClient.java:3746` |
| s2c | 229 | variable | u8 compared with 1 | `MudClient.call5fac7c41` | `deob/MudClient.java:3735` |
| s2c | 233 | variable | empty | `MudClient.call5fac7c41` | `deob/MudClient.java:3730` |
| s2c | 234 | variable | u8 stock, i8, u8 buy mod, u8 sell mod, then stock times (u16be id, u16be, i8) | `MudClient.call5fac7c41` | `deob/MudClient.java:3678` |
| s2c | 235 | variable | u8 compared with 1 | `MudClient.call5fac7c41` | `deob/MudClient.java:3667` |
| s2c | 236 | variable | u8 count, then count times (u16be id, u32be amount) | `MudClient.call5fac7c41` | `deob/MudClient.java:3651` |
| s2c | 237 | variable | empty | `MudClient.call5fac7c41` | `deob/MudClient.java:3645` |
| s2c | 238 | variable | u16be player index | `MudClient.call5fac7c41` | `deob/MudClient.java:3631` |
| s2c | 239 | variable | empty | `MudClient.call5fac7c41` | `deob/MudClient.java:3626` |
| s2c | 240 | variable | `(length - 1) / 4` times (i16be, i16be) | `MudClient.call5fac7c41` | `deob/MudClient.java:3551` |
| s2c | 241 | variable | empty. Sets `value250 = 250` | `MudClient.call5fac7c41` | `deob/MudClient.java:3546` |
| s2c | 242 | variable | five u8 | `MudClient.call5fac7c41` | `deob/MudClient.java:3538` |
| s2c | 243 | variable | 16 u8 current, 16 u8 base, 16 u32be xp, u8 | `MudClient.call5fac7c41` | `deob/MudClient.java:3518` |
| s2c | 244 | variable | five u16be: index, x offset, y offset, plane, scale. Then `y offset -= plane * scale` | `MudClient.call5fac7c41` | `deob/MudClient.java:3507` |
| s2c | 245 | variable | empty | `MudClient.call5fac7c41` | `deob/MudClient.java:3502` |
| s2c | 246 | variable | u8 count, then count times (u8 length, raw bytes) | `MudClient.call5fac7c41` | `deob/MudClient.java:3486` |
| s2c | 247 | variable | u16be count at offset 1, records from offset 3. Each record: u16be index, u8 type. Type 1: u16be speaker, i8 length, dictionary bytes. Type 2: three u8 | `MudClient.call5fac7c41` | `deob/MudClient.java:3442` |
| s2c | 248 | variable | bit fields in `sync.md` | `MudClient.call5fac7c41` | `deob/MudClient.java:3347` |
| s2c | 249 | variable | repeating from offset 1. u8 255 then two i8, or u16be id, i8 dx, i8 dy, i8 direction. Id 65535 removes only | `MudClient.call5fac7c41` | `deob/MudClient.java:3274` |
| s2c | 250 | variable | u16be count at offset 1, records from offset 3. See `sync.md` | `MudClient.call5fac7c41` | `deob/MudClient.java:3148` |
| s2c | 252 | variable | u8 count, then per slot u16be (low 15 bits id, value / 32768 stored apart) and gsmart when `numberArray440[id] == 0`, else amount 1 | `MudClient.call5fac7c41` | `deob/MudClient.java:3124` |
| s2c | 253 | variable | repeating from offset 1. u8 255 then two i8, or u16be id, i8 dx, i8 dy. Id 60000 removes only | `MudClient.call5fac7c41` | `deob/MudClient.java:3029` |
| s2c | 254 | variable | repeating from offset 1. u8 255 then two i8, or u16be id, i8 dx, i8 dy. Bit 15 set removes `id & 32767` | `MudClient.call5fac7c41` | `deob/MudClient.java:2957` |
| s2c | 255 | variable | bit fields in `sync.md`. Region load is inside this opcode | `MudClient.call5fac7c41` | `deob/MudClient.java:2833` |

Opcode 10 calls `tryLogout2`, overridden as `tryLogoutb` (`mudclient.m`, `MudClient.java:2129`): `number293 = 0` and the message `@cya@Sorry, you can't logout at the moment`.

Shop opcode 234, when the following i8 equals 1, also fills leftover slots from the inventory and skips item 10 (`MudClient.java:3698`). The selected-slot check returns only when it resets the selection (`MudClient.java:3724`). A false check leaves the method at the return after the if/else (`MudClient.java:4045`).

Inventory opcode 213 returns immediately when the slot index is past `number159` after growing the count (`MudClient.java:3960`). A slot already in range updates the arrays and then hits that same trailing return. It does not enter opcode 212.
