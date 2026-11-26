# Packets

One row per opcode this client writes with `Packet.p1isaac`, plus the login bytes that are written before ISAAC, plus every server opcode that has a branch in `Client.read`. Logical client opcodes are the integers passed to `p1isaac`. Server opcodes are the values after ISAAC. Width names are in [framing.md](framing.md).

There is no client size array. The size column is `none` for client writes. Server sizes are `Protocol.serverProtSizes`. `-1` is a following `u8` length. `-2` is a following `u16be` length. `Client.read` at `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:11982`.

A server opcode with no branch is still framed, then logged as `T1` and `logout()` runs. Opcode 47 is the only nonzero size in 0..255 with no field reads. Its field list is empty.

Zone opcodes 4, 44, 84, 101, 105, 117, 147, 151, 156, 160, and 215 are also the inner opcodes of packet 60. A coordinate `u8` is `(dx << 4) | dz`, added to `baseX` and `baseZ`. Packet 60 sets that base itself. The other zone packets use the base left by 60, 64, or 85.

Citations are under `builds/rs2/317-2005-06-13/src/src/main/java/`.

## Login, before ISAAC

These are not `p1isaac` opcodes. The full sequence, RSA block, and status bytes are in [login.md](login.md).

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 14 | none | `u8` 14, `u8` `(nameBase37 >> 16) & 31` | `Client.login` | `jagex2/client/Client.java:6963` |
| c2s | 16 | none | `u8` 16, `u8` length, `u8` 255, `u16be` 317, `u8` low-memory, nine `i32be` checksums, RSA block. Normal login. | `Client.login` | `jagex2/client/Client.java:6988` |
| c2s | 18 | none | Same body as 16. Reconnect. | `Client.login` | `jagex2/client/Client.java:6988` |

## Client to server

Menu writes use `menuParamB` as the slot or local x, `menuParamC` as the component or local z, and `menuParamA` as the obj id, loc bitset, or npc or player index. `useMenuOption` at `jagex2/client/Client.java:4593`. A loc id on the wire is `bitset >> 14 & 32767`. Tile coordinates are added to `sceneBaseTileX` or `sceneBaseTileZ` before the write.

Walk opcodes 98, 164, and 248 share one body from `tryMove`. The `u8` length counts the bytes after itself. It is `2 * steps + 3`, and 248 adds 14. Then `u16le_add` of the first x, then one `u8` dx and one `u8` dz per extra step, walking back toward the destination, then `u16le` of the first z, then `u8_neg` run (`actionKey[5] == 1`). Steps are capped at 25. The first waypoint is the one nearest the player. No path writes no packet. `jagex2/client/Client.java:7420`.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | none | no payload. Heartbeat when `heartbeatTimer` passes 50, and empty markers inside `buildScene`. | `Client.updateGame` | `jagex2/client/Client.java:4199` |
| c2s | 3 | none | `u8` 1 when the window gains focus, else 0 | `Client.updateGame` | `jagex2/client/Client.java:3961` |
| c2s | 4 | none | `u8` length, `u8_sub` effect, `u8_sub` colour, `bytes_rev_add` of `WordPack.pack`. Effect is written before colour. Length counts those bytes. | `Client.handleInputKey` | `jagex2/client/Client.java:6163` |
| c2s | 14 | none | `u16be_add` selected interface, `u16be` player, `u16be` selected obj, `u16le` selected slot. Menu 491. | `Client.useMenuOption` | `jagex2/client/Client.java:5324` |
| c2s | 16 | none | `u16be_add` obj, `u16le_add` slot, `u16le_add` interface. Menu 539. | `Client.useMenuOption` | `jagex2/client/Client.java:4827` |
| c2s | 17 | none | `u16le_add` npc. Menu 225. Opcode 230 may be written first. | `Client.useMenuOption` | `jagex2/client/Client.java:5099` |
| c2s | 18 | none | `u16le` npc. Menu 478. Opcode 85 may be written first. | `Client.useMenuOption` | `jagex2/client/Client.java:5394` |
| c2s | 21 | none | `u16be` npc. Menu 965. Opcode 152 may be written first. | `Client.useMenuOption` | `jagex2/client/Client.java:5119` |
| c2s | 23 | none | `u16le` z, `u16le` obj, `u16le` x. Menu 567. | `Client.useMenuOption` | `jagex2/client/Client.java:5242` |
| c2s | 25 | none | `u16le` selected interface, `u16be_add` selected obj, `u16be` ground obj, `u16be_add` z, `u16le_add` slot, `u16be` x. Menu 511. | `Client.useMenuOption` | `jagex2/client/Client.java:4653` |
| c2s | 35 | none | `u16le` x, `u16be_add` spell, `u16be_add` z, `u16le` loc. Menu 956. | `Client.useMenuOption` | `jagex2/client/Client.java:5225` |
| c2s | 36 | none | `i32be` 0. Written when the walk counter reaches 92, before the walk packet. | `Client.tryMove` | `jagex2/client/Client.java:7415` |
| c2s | 39 | none | `u16le` player. Menu 729. | `Client.useMenuOption` | `jagex2/client/Client.java:5206` |
| c2s | 40 | none | `u16be` component. Menu 679. | `Client.useMenuOption` | `jagex2/client/Client.java:4762` |
| c2s | 41 | none | `u16be` obj, `u16be_add` slot, `u16be_add` interface. Menu 454. | `Client.useMenuOption` | `jagex2/client/Client.java:5359` |
| c2s | 43 | none | `u16le` interface, `u16be_add` obj, `u16be_add` slot. Menu 867. Opcode 200 may be written first. | `Client.useMenuOption` | `jagex2/client/Client.java:5259` |
| c2s | 45 | none | `u8` length, then mouse records while `mouseTracking.length` lasts. The loop test is `startPos - out.pos < 240`. `out.pos` only grows, so that test does not stop the loop. A repeated point increments a duplicate count up to 2047 and is emitted with the next different point. x is clamped to 0..764 and y to 0..502. Packed position is `y * 765 + x`, or 524287 when both raw values are -1. Duplicates below 8 and both deltas in -32..31: `u16be` `(duplicates << 12) + ((dx + 32) << 6) + (dy + 32)`. Duplicates below 8 otherwise: `u24be` `8388608 + (duplicates << 19) + packed`. Otherwise `i32be` `-1073741824 + (duplicates << 19) + packed`. | `Client.updateGame` | `jagex2/client/Client.java:3844` |
| c2s | 53 | none | `u16be` slot, `u16be_add` selected slot, `u16le_add` obj, `u16be` selected interface, `u16le` selected obj, `u16be` component. Menu 870. | `Client.useMenuOption` | `jagex2/client/Client.java:4884` |
| c2s | 57 | none | `u16be_add` selected obj, `u16be_add` npc, `u16le` slot, `u16be_add` selected interface. Menu 582. | `Client.useMenuOption` | `jagex2/client/Client.java:4609` |
| c2s | 60 | none | `u64be` name. Name prompt. | `Client.handleInputKey` | `jagex2/client/Client.java:6046` |
| c2s | 70 | none | `u16le` x, `u16be` z, `u16le_add` loc. Menu 113. | `Client.useMenuOption` | `jagex2/client/Client.java:5401` |
| c2s | 72 | none | `u16be_add` npc. Menu 412. | `Client.useMenuOption` | `jagex2/client/Client.java:5179` |
| c2s | 73 | none | `u16le` player. Menu 27. Opcode 189 may be written first. | `Client.useMenuOption` | `jagex2/client/Client.java:4980` |
| c2s | 74 | none | `u64be` name | `Client.removeIgnore` | `jagex2/client/Client.java:10518` |
| c2s | 75 | none | `u16le_add` interface, `u16le` slot, `u16be_add` obj. Menu 493. | `Client.useMenuOption` | `jagex2/client/Client.java:5020` |
| c2s | 77 | none | `u8` length, `u8` random, `u8` 101, `u8` 233, `u16be` 45092, optional `u16be` 35784, `u8` random, `u8` 64, `u8` 38, `u16be` random, `u16be` random | `Client.updateOrbitCamera` | `jagex2/client/Client.java:9907` |
| c2s | 78 | none | no payload | `Client.draw3DEntityElements` | `jagex2/client/Client.java:10008` |
| c2s | 79 | none | `u16le` z, `u16be` obj, `u16be_add` x. Menu 213. | `Client.useMenuOption` | `jagex2/client/Client.java:4995` |
| c2s | 85 | none | `u8` 66. Written before opcode 18 when its counter reaches 53. | `Client.useMenuOption` | `jagex2/client/Client.java:5389` |
| c2s | 86 | none | `u16be` pitch, `u16be_add` yaw. At most every 20 ticks after an arrow key. | `Client.updateGame` | `jagex2/client/Client.java:3954` |
| c2s | 87 | none | `u16be_add` obj, `u16be` interface, `u16be_add` slot. Menu 847. | `Client.useMenuOption` | `jagex2/client/Client.java:4905` |
| c2s | 95 | none | `u8` public, `u8` private, `u8` trade | `Client.handleChatSettingsInput` | `jagex2/client/Client.java:2030` |
| c2s | 98 | none | walk body above. Interaction path, `tryMove` argument 2. | `Client.tryMove` | `jagex2/client/Client.java:7431` |
| c2s | 101 | none | `u8` gender (0 male, 1 female), seven `u8` identikit ids, five `u8` colours | `Client.handleInterfaceAction` | `jagex2/client/Client.java:3183` |
| c2s | 103 | none | `u8` `text.length() - 1`, `jstr` of the text without the leading `::`. The length counts the `jstr` bytes, including terminator 10. | `Client.handleInputKey` | `jagex2/client/Client.java:6100` |
| c2s | 117 | none | `u16le_add` interface, `u16le_add` obj, `u16le` slot. Menu 78. | `Client.useMenuOption` | `jagex2/client/Client.java:4948` |
| c2s | 120 | none | `u8` selected tab. Sent when a flashing tab is cleared. | `Client.drawGame` | `jagex2/client/Client.java:9047` |
| c2s | 121 | none | no payload. Sent when the rebuilt scene is ready. | `Client.checkScene` | `jagex2/client/Client.java:3537` |
| c2s | 122 | none | `u16le_add` interface, `u16be_add` slot, `u16le` obj. Menu 74. | `Client.useMenuOption` | `jagex2/client/Client.java:4663` |
| c2s | 126 | none | `u8` length, `u64be` name, then `WordPack.pack` bytes written in order. Length counts the name and the packed bytes. | `Client.handleInputKey` | `jagex2/client/Client.java:5977` |
| c2s | 128 | none | `u16be` player. Menus 561 and 6. Opcode 136 may be written first. | `Client.useMenuOption` | `jagex2/client/Client.java:4707` |
| c2s | 129 | none | `u16be_add` slot, `u16be` interface, `u16be_add` obj. Menu 431. | `Client.useMenuOption` | `jagex2/client/Client.java:4768` |
| c2s | 130 | none | no payload | `Client.closeInterfaces` | `jagex2/client/Client.java:13371` |
| c2s | 131 | none | `u16le_add` npc, `u16be_add` spell. Menu 413. | `Client.useMenuOption` | `jagex2/client/Client.java:5132` |
| c2s | 132 | none | `u16le_add` x, `u16be` loc, `u16be_add` z. Menu 502. | `Client.useMenuOption` | `jagex2/client/Client.java:5417` |
| c2s | 133 | none | `u64be` name | `Client.addIgnore` | `jagex2/client/Client.java:10104` |
| c2s | 135 | none | `u16le` slot, `u16be_add` interface, `u16le` obj. Menu 53. | `Client.useMenuOption` | `jagex2/client/Client.java:4809` |
| c2s | 136 | none | no payload. Written before opcode 128 when its counter reaches 90. | `Client.useMenuOption` | `jagex2/client/Client.java:4703` |
| c2s | 139 | none | `u16le` player. Menus 484 and 577. | `Client.useMenuOption` | `jagex2/client/Client.java:4857` |
| c2s | 145 | none | `u16be_add` interface, `u16be_add` slot, `u16be_add` obj. Menu 632. | `Client.useMenuOption` | `jagex2/client/Client.java:5002` |
| c2s | 148 | none | no payload | `Client.draw3DEntityElements` | `jagex2/client/Client.java:10069` |
| c2s | 150 | none | no payload | `Client.buildScene` | `jagex2/client/Client.java:1460` |
| c2s | 152 | none | `u8` 88. Written before opcode 21 when its counter reaches 96. | `Client.useMenuOption` | `jagex2/client/Client.java:5114` |
| c2s | 153 | none | `u16le` player. Menu 779 looks up `players[index]`. | `Client.useMenuOption` | `jagex2/client/Client.java:4733` |
| c2s | 155 | none | `u16le` npc. Menu 20. | `Client.useMenuOption` | `jagex2/client/Client.java:4720` |
| c2s | 156 | none | `u16be_add` x, `u16le` z, `u16le_add` obj. Menu 652. | `Client.useMenuOption` | `jagex2/client/Client.java:5047` |
| c2s | 164 | none | walk body above. Tile click, `tryMove` argument 0. | `Client.tryMove` | `jagex2/client/Client.java:7421` |
| c2s | 165 | none | `u8` length, `u8` 139, `u8` 150, `u16be` 32131, `u8` random, `u16be` 3250, `u8` 177, `u16be` 24859, `u8` 119, optional `u16be` 47234, optional `u8` 21 | `Client.handleChatSettingsInput` | `jagex2/client/Client.java:2076` |
| c2s | 181 | none | `u16le` z, `u16be` obj, `u16le` x, `u16be_add` spell. Menu 94. | `Client.useMenuOption` | `jagex2/client/Client.java:5063` |
| c2s | 183 | none | `u24be` 15086193. Written before opcode 228 when its counter reaches 113. | `Client.useMenuOption` | `jagex2/client/Client.java:4749` |
| c2s | 185 | none | `u16be` component. Menus 315, 646, and 169. | `Client.useMenuOption` | `jagex2/client/Client.java:4688` |
| c2s | 188 | none | `u64be` name | `Client.addFriend` | `jagex2/client/Client.java:2844` |
| c2s | 189 | none | `u8` 234. Written before opcode 73 when its counter reaches 54. | `Client.useMenuOption` | `jagex2/client/Client.java:4975` |
| c2s | 192 | none | `u16be` selected interface, `u16le` loc, `u16le_add` z, `u16le` slot, `u16le_add` x, `u16be` selected obj. Menu 62. | `Client.useMenuOption` | `jagex2/client/Client.java:4634` |
| c2s | 200 | none | `u16be` 25501. Written before opcode 43 when its counter reaches 59. | `Client.useMenuOption` | `jagex2/client/Client.java:5254` |
| c2s | 202 | none | no payload. Sent when `idleCycles` passes 4500. | `Client.updateGame` | `jagex2/client/Client.java:4120` |
| c2s | 208 | none | `i32be` parsed amount | `Client.handleInputKey` | `jagex2/client/Client.java:6026` |
| c2s | 210 | none | `i32be` 1057001181. Sent from `buildScene` when `frame` is set. | `Client.buildScene` | `jagex2/client/Client.java:1470` |
| c2s | 214 | none | `u16le_add` interface, `u8_neg` mode, `u16le_add` from slot, `u16le` to slot. Mode is 1 only when `bankArrangeMode` is 1, the component `clientCode` is 206, and the destination slot holds an item. Otherwise 0. | `Client.updateGame` | `jagex2/client/Client.java:4059` |
| c2s | 215 | none | `u64be` name | `Client.removeFriend` | `jagex2/client/Client.java:2501` |
| c2s | 218 | none | `u64be` name, `u8` rule (`clientCode - 601`, buttons 601..612), `u8` mute | `Client.handleInterfaceAction` | `jagex2/client/Client.java:3174` |
| c2s | 226 | none | `u8` length, `u16be` 58722, `u8` 240, `u16be` random, `u8` random, optional `u16be` 51825, `u8` random, `u16be` random, `u16be` 7130, `u16be` random, `u16be` 61657 | `Client.updateTextures` | `jagex2/client/Client.java:2558` |
| c2s | 228 | none | `u16be_add` loc, `u16be_add` z, `u16be` x. Menu 1062. Opcode 183 may be written first. | `Client.useMenuOption` | `jagex2/client/Client.java:4755` |
| c2s | 230 | none | `u8` 239. Written before opcode 17 when its counter reaches 85. | `Client.useMenuOption` | `jagex2/client/Client.java:5094` |
| c2s | 234 | none | `u16le_add` x, `u16be_add` loc, `u16le_add` z. Menu 872. | `Client.useMenuOption` | `jagex2/client/Client.java:5409` |
| c2s | 236 | none | `u16le` z, `u16be` obj, `u16le` x. Menu 234. | `Client.useMenuOption` | `jagex2/client/Client.java:4627` |
| c2s | 237 | none | `u16be` slot, `u16be_add` obj, `u16be` interface, `u16be_add` spell. Menu 543. | `Client.useMenuOption` | `jagex2/client/Client.java:5277` |
| c2s | 238 | none | `u8` 96 | `Client.buildScene` | `jagex2/client/Client.java:1359` |
| c2s | 241 | none | `i32be` `(dt << 20) + (right << 19) + (y * 765 + x)`. y is clamped to 0..502, x to 0..764. `dt` is click delta in 50 ms units, clamped to 4095. The right bit is set when the button is 2. | `Client.updateGame` | `jagex2/client/Client.java:3939` |
| c2s | 246 | none | `u8` length, optional `u8` 101, `u8` 197, `u16be` random, `u8` random, `u8` 67, `u16be` 14214, optional `u16be` 29487, `u16be` random, optional `u8` 220, `u8` 180 | `Client.handleMinimapInput` | `jagex2/client/Client.java:8411` |
| c2s | 248 | none | walk body, with the length already including 14 trailing bytes: `u8` local click x, `u8` local click y, `u16be` yaw, `u8` 57, `u8` minimap angle, `u8` minimap zoom, `u8` 89, `u16be` player x, `u16be` player z, `u8` `tryMoveNearest`, `u8` 63. Those 14 bytes are written only when `tryMove` returns true. | `Client.tryMove` | `jagex2/client/Client.java:7426` |
| c2s | 249 | none | `u16be_add` player, `u16le` spell. Menu 365. | `Client.useMenuOption` | `jagex2/client/Client.java:5192` |
| c2s | 252 | none | `u16le_add` loc, `u16le` z, `u16be_add` x. Menu 900. | `Client.useMenuOption` | `jagex2/client/Client.java:5165` |
| c2s | 253 | none | `u16le` x, `u16le_add` z, `u16be_add` obj. Menu 244. | `Client.useMenuOption` | `jagex2/client/Client.java:5482` |

## Server to client

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 1 | 0 | no payload. Clears `primarySeqId` on every player and npc. | `Client.read` | `jagex2/client/Client.java:12625` |
| s2c | 4 | 6 | `u8` coord, `u16be` spotanim, `u8` height, `u16be` delay | `Client.readZonePacket` | `jagex2/client/Client.java:11556` |
| s2c | 8 | 4 | `u16le_add` component, `u16be` model. Sets model type 1. | `Client.read` | `jagex2/client/Client.java:12965` |
| s2c | 24 | 1 | `u8_sub` flashing tab | `Client.read` | `jagex2/client/Client.java:12867` |
| s2c | 27 | 0 | no payload. Opens the amount prompt. | `Client.read` | `jagex2/client/Client.java:13072` |
| s2c | 34 | -2 | `u16be` component, then while bytes remain: `gsmarts` slot, `u16be` obj, `u8` count, and when that count is 255 an `i32be` replaces it | `Client.read` | `jagex2/client/Client.java:13200` |
| s2c | 35 | 4 | `u8` index, `u8` jitter, `u8` wobble scale, `u8` wobble speed | `Client.read` | `jagex2/client/Client.java:12517` |
| s2c | 36 | 3 | `u16le` varp, `i8` value | `Client.read` | `jagex2/client/Client.java:13140` |
| s2c | 44 | 5 | `u16le_add` obj, `u16be` count, `u8` coord | `Client.readZonePacket` | `jagex2/client/Client.java:11570` |
| s2c | 47 | 6 |  | `Client.read` | `jagex2/client/Client.java:13254` |
| s2c | 50 | 9 | `u64be` name, `u8` world | `Client.read` | `jagex2/client/Client.java:12642` |
| s2c | 53 | -2 | `u16be` component, `u16be` count, then that many slots: `u8` amount, replaced by `u32_alt3` when the byte is 255, then `u16le_add` obj. Slots after the count are set to obj 0 and count 0. | `Client.read` | `jagex2/client/Client.java:12985` |
| s2c | 60 | -2 | `u8` base z, `u8_neg` base x, then `u8` zone opcode and that opcode's payload until `pos` reaches `packetSize` | `Client.read` | `jagex2/client/Client.java:12504` |
| s2c | 61 | 1 | `u8` multizone | `Client.read` | `jagex2/client/Client.java:13157` |
| s2c | 64 | 2 | `u8_neg` base x, `u8_sub` base z. Clears an 8 by 8 obj area on the current level. | `Client.read` | `jagex2/client/Client.java:12071` |
| s2c | 65 | -2 | NPC bit stream. See [sync.md](sync.md). | `Client.readNpcInfo` | `jagex2/client/Client.java:13066` |
| s2c | 68 | 0 | no payload. Copies `varCache` back into `varps`. | `Client.read` | `jagex2/client/Client.java:12798` |
| s2c | 70 | 6 | `i16be` x, `i16le` y, `u16le` component | `Client.read` | `jagex2/client/Client.java:12229` |
| s2c | 71 | 3 | `u16be` interface (65535 becomes -1), `u8_add` tab | `Client.read` | `jagex2/client/Client.java:12178` |
| s2c | 72 | 2 | `u16le` component. Each slot id is set to -1 and then immediately to 0. Counts are left as they were. | `Client.read` | `jagex2/client/Client.java:12118` |
| s2c | 73 | 4 | `u16be_add` zone x, `u16be` zone z. Clears the instanced flag. Rebuild follows in [region.md](region.md). | `Client.read` | `jagex2/client/Client.java:12243` |
| s2c | 74 | 2 | `u16le` song. 65535 becomes -1. Requests ondemand archive 2. | `Client.read` | `jagex2/client/Client.java:12192` |
| s2c | 75 | 4 | `u16le_add` npc type, `u16le_add` component. Sets model type 2. | `Client.read` | `jagex2/client/Client.java:12489` |
| s2c | 78 | 0 | no payload. Clears the tile flag. | `Client.read` | `jagex2/client/Client.java:12563` |
| s2c | 79 | 4 | `u16le` component, `u16be_add` scroll | `Client.read` | `jagex2/client/Client.java:12778` |
| s2c | 81 | -2 | player bit stream. See [sync.md](sync.md). | `Client.readPlayerInfo` | `jagex2/client/Client.java:12035` |
| s2c | 84 | 7 | `u8` coord, `u16be` obj, `u16be` old count, `u16be` new count | `Client.readZonePacket` | `jagex2/client/Client.java:11344` |
| s2c | 85 | 2 | `u8_neg` base z, `u8_neg` base x | `Client.read` | `jagex2/client/Client.java:12860` |
| s2c | 87 | 6 | `u16le` varp, `u32_alt2` value | `Client.read` | `jagex2/client/Client.java:13123` |
| s2c | 97 | 2 | `u16be` viewport interface | `Client.read` | `jagex2/client/Client.java:13090` |
| s2c | 99 | 1 | `u8` minimap state | `Client.read` | `jagex2/client/Client.java:12483` |
| s2c | 101 | 2 | `u8_neg` `(shape << 2) \| rotation`, `u8` coord | `Client.readZonePacket` | `jagex2/client/Client.java:11588` |
| s2c | 104 | -1 | `u8_neg` slot, `u8_add` push-down flag, `jstr` option. Slot is used only for 1..5. The text `null` becomes a null option. Push-down is true when the flag byte is 0. | `Client.read` | `jagex2/client/Client.java:12546` |
| s2c | 105 | 4 | `u8` coord, `u16be` wave, `u8` packed. High nibble is radius. Low 3 bits are loops. Delay comes from `Wave.delays`, not from this packet. | `Client.readZonePacket` | `jagex2/client/Client.java:11366` |
| s2c | 106 | 1 | `u8_neg` selected tab | `Client.read` | `jagex2/client/Client.java:13229` |
| s2c | 107 | 0 | no payload. Clears cutscene and the five camera modifiers. | `Client.read` | `jagex2/client/Client.java:12107` |
| s2c | 109 | 0 | no payload. `logout` returns false from `read`. | `Client.read` | `jagex2/client/Client.java:12223` |
| s2c | 110 | 1 | `u8` run energy | `Client.read` | `jagex2/client/Client.java:12700` |
| s2c | 114 | 2 | `u16le` timer. The stored value is that field times 30. | `Client.read` | `jagex2/client/Client.java:12498` |
| s2c | 117 | 15 | `u8` coord, `i8` destination dx, `i8` destination dz, `i16be` target, `u16be` graphic, `u8` start height, `u8` end height, `u16be` start delay, `u16be` end delay, `u8` peak, `u8` angle. Both heights are multiplied by 4 after the read. Graphic 65535 builds nothing. | `Client.readZonePacket` | `jagex2/client/Client.java:11600` |
| s2c | 121 | 4 | `u16le_add` song, `u16be_add` delay. Requests ondemand archive 2. | `Client.read` | `jagex2/client/Client.java:12209` |
| s2c | 122 | 4 | `u16le_add` component, `u16le_add` rgb555. Stored colour is `(hi5 << 19) + (mid5 << 11) + (lo5 << 3)`. | `Client.read` | `jagex2/client/Client.java:12974` |
| s2c | 126 | -2 | `jstr` text, `u16be_add` component | `Client.read` | `jagex2/client/Client.java:12933` |
| s2c | 134 | 6 | `u8` skill, `u32_alt2` experience, `u8` level. Base level is recomputed from `levelExperience`. | `Client.read` | `jagex2/client/Client.java:12159` |
| s2c | 142 | 2 | `u16le` sidebar interface | `Client.read` | `jagex2/client/Client.java:12911` |
| s2c | 147 | 14 | `u8_sub` coord, `u16be` player, `i8_sub`, `u16le` start cycle, `i8_neg`, `u16be` end cycle, `u8_sub` `(shape << 2) \| rotation`, `i8`, `u16be` loc, `i8_neg`. The two signed x bytes and the two signed z bytes are swapped if they arrive out of order. Cycles are added to `loopCycle`. | `Client.readZonePacket` | `jagex2/client/Client.java:11478` |
| s2c | 151 | 4 | `u8_add` coord, `u16le` loc, `u8_sub` `(shape << 2) \| rotation` | `Client.readZonePacket` | `jagex2/client/Client.java:11543` |
| s2c | 156 | 3 | `u8_add` coord, `u16be` obj. Compared as `id & 32767`. | `Client.readZonePacket` | `jagex2/client/Client.java:11401` |
| s2c | 160 | 4 | `u8_sub` coord, `u8_sub` `(shape << 2) \| rotation`, `u16be_add` sequence. Shape 11 is treated as 10 on the scenery layer. | `Client.readZonePacket` | `jagex2/client/Client.java:11424` |
| s2c | 164 | 2 | `u16le` chat interface | `Client.read` | `jagex2/client/Client.java:13237` |
| s2c | 166 | 6 | `u8` source local x, `u8` source local z, `u16be` height, `u8` speed, `u8` acceleration. Acceleration of 100 or more snaps the camera. | `Client.read` | `jagex2/client/Client.java:12142` |
| s2c | 171 | 3 | `u8` hidden when 1, `u16be` component | `Client.read` | `jagex2/client/Client.java:12903` |
| s2c | 174 | 5 | `u16be` wave, `u8` loops, `u16be` delay. The stored delay adds `Wave.delays[id]`. | `Client.read` | `jagex2/client/Client.java:12531` |
| s2c | 176 | 10 | `u8_neg` days since recoveries, `u16be_add` unread message count, `u8` members warning, `u32_alt3` last address, `u16be` days since last login | `Client.read` | `jagex2/client/Client.java:12042` |
| s2c | 177 | 6 | `u8` destination local x, `u8` destination local z, `u16be` height, `u8` speed, `u8` acceleration. Acceleration of 100 or more aims the camera. | `Client.read` | `jagex2/client/Client.java:13029` |
| s2c | 185 | 2 | `u16le_add` component. Model type 3 is filled from the local player, not from further packet bytes. | `Client.read` | `jagex2/client/Client.java:12094` |
| s2c | 187 | 0 | no payload. Opens the name prompt. | `Client.read` | `jagex2/client/Client.java:13081` |
| s2c | 196 | -1 | `u64be` name, `i32be` message id, `u8` rights, then `WordPack.unpack` of `packetSize - 13` bytes | `Client.read` | `jagex2/client/Client.java:12811` |
| s2c | 200 | 4 | `u16be` component, `i16be` sequence. Sequence -1 clears the anim frame and cycle. | `Client.read` | `jagex2/client/Client.java:13163` |
| s2c | 206 | 3 | `u8` public, `u8` private, `u8` trade | `Client.read` | `jagex2/client/Client.java:12945` |
| s2c | 208 | 2 | `i16le` overlay interface. `resetInterfaceAnimation` runs only when the value is at least 0. | `Client.read` | `jagex2/client/Client.java:12472` |
| s2c | 214 | -2 | `packetSize / 8` times `u64be` ignore name | `Client.read` | `jagex2/client/Client.java:12131` |
| s2c | 215 | 7 | `u16be_add` obj, `u8_sub` coord, `u16be_add` player, `u16be` count. Ignored when the player is the local pid. | `Client.readZonePacket` | `jagex2/client/Client.java:11382` |
| s2c | 218 | 2 | `i16le_add` sticky chat interface | `Client.read` | `jagex2/client/Client.java:13115` |
| s2c | 219 | 0 | no payload. Closes sidebar, chat, and viewport interfaces. | `Client.read` | `jagex2/client/Client.java:13177` |
| s2c | 221 | 1 | `u8` friend-list status | `Client.read` | `jagex2/client/Client.java:13022` |
| s2c | 230 | 8 | `u16be_add` zoom, `u16be` component, `u16be` x angle, `u16le_add` y angle | `Client.read` | `jagex2/client/Client.java:13010` |
| s2c | 240 | 2 | `i16be` weight | `Client.read` | `jagex2/client/Client.java:12955` |
| s2c | 241 | -2 | `u16be_add` zone z, then `gBit(1)` for each of 4 by 13 by 13 cells and `gBit(26)` when that bit is 1, then `u16be` zone x. Sets the instanced flag. See [region.md](region.md). | `Client.read` | `jagex2/client/Client.java:12249` |
| s2c | 246 | 6 | `u16le` component, `u16be` zoom, `u16be` obj. Obj 65535 clears model type to 0. Otherwise model type 4 and zoom is `obj.zoom2d * 100 / packet zoom`. | `Client.read` | `jagex2/client/Client.java:12883` |
| s2c | 248 | 4 | `u16be_add` viewport interface, `u16be` sidebar interface | `Client.read` | `jagex2/client/Client.java:12756` |
| s2c | 249 | 3 | `u8_add` members, `u16le_add` local pid | `Client.read` | `jagex2/client/Client.java:13059` |
| s2c | 253 | -1 | `jstr`. Suffixes `:tradereq:`, `:duelreq:`, and `:chalreq:` are split before display. | `Client.read` | `jagex2/client/Client.java:12569` |
| s2c | 254 | 6 | `u8` type. Type 1 then reads `u16be` npc. Types 2..6 then read `u16be` tile x, `u16be` tile z, `u8` height, and the stored type becomes 2. Type 10 then reads `u16be` player. Any other type reads only the first byte. The frame is still size 6, so the unread tail stays inside the consumed payload. | `Client.read` | `jagex2/client/Client.java:12710` |
