# Packets

Opcodes the client reads or writes. C2S size is `none` because this client has no C2S size array. S2C size is `C994ec50d.valuekind182`. `-1` is a following `u8` length. `-2` is a following `u16be` length. Widths are in `framing.md`. `psize1` is a placeholder `u8` later set to the number of bytes after it. `jstr` ends with byte 10. `wordpack(n)` is `WordPack.opkind591` over `n` bytes.

C2S opcodes below are the values passed to `Packet.opkind272` before ISAAC is added. The same number can appear in both directions.

Zone coordinates are one `u8`: bits 6..4 are x and bits 2..0 are z, added to the base from opcode 32, 153, or the start of 195.

Citations are `builds/rs2/282-2004-12-21/src/src/main/java/deob/Client.java` unless noted.

## S2C

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 3 | -2 | `payloadLength / 8` times `u64be` | `Client.read` | `Client.java:7399` |
| s2c | 10 | 3 | `u16be` component, `u8` hidden when 1 | `Client.read` | `Client.java:7100` |
| s2c | 15 | 4 | `u16be` jingle, `u16be` delay | `Client.read` | `Client.java:6519` |
| s2c | 16 | 2 | `u16be` sidebar interface | `Client.read` | `Client.java:7501` |
| s2c | 17 | -1 | `u8` slot 1..5, `u8`, `jstr` (`"null"` becomes null) | `Client.read` | `Client.java:6827` |
| s2c | 23 | 2 | `u16be` midi, 65535 becomes -1 | `Client.read` | `Client.java:6502` |
| s2c | 28 | 6 | `u16be` component, `u16be` object (65535 clears the model), `u16be` zoom | `Client.read` | `Client.java:7343` |
| s2c | 32 | 2 | `u8` zone base x, `u8` zone base z | `Client.read` | `Client.java:7133` |
| s2c | 34 | 5 | `u16be` wave, `u8`, `u16be` delay | `Client.read` | `Client.java:7486` |
| s2c | 44 | -2 | `u16be` component, `jstr` | `Client.read` | `Client.java:7235` |
| s2c | 47 | 0 | empty; clears player and NPC sequences | `Client.read` | `Client.java:7410` |
| s2c | 48 | 4 | zone `u8`, `u8` (type in the high 6 bits, rotation in the low 2), `u16be` sequence | `Client.readZonePacket` | `Client.java:2122` |
| s2c | 52 | 3 | zone `u8`, `u16be` object id | `Client.readZonePacket` | `Client.java:2193` |
| s2c | 54 | 4 | `u16be` component, `u16be` scroll | `Client.read` | `Client.java:7443` |
| s2c | 64 | 4 | `u8` camera slot, `u8`, `u8`, `u8` | `Client.read` | `Client.java:6432` |
| s2c | 67 | 2 | `i16be` run energy | `Client.read` | `Client.java:7427` |
| s2c | 77 | 6 | `u16be` component, `i16be` x, `i16be` y | `Client.read` | `Client.java:6533` |
| s2c | 81 | 5 | zone `u8`, `u16be` object id, `u16be` count | `Client.readZonePacket` | `Client.java:2175` |
| s2c | 83 | 1 | `u8` | `Client.read` | `Client.java:7463` |
| s2c | 85 | 6 | zone `u8`, `u16be` spotanim, `u8` height, `u16be` delay | `Client.readZonePacket` | `Client.java:2240` |
| s2c | 88 | 0 | empty; logout | `Client.read` | `Client.java:7329` |
| s2c | 89 | 2 | `u16be`, stored times 30 | `Client.read` | `Client.java:7114` |
| s2c | 90 | 1 | `u8` tab to flash | `Client.read` | `Client.java:6416` |
| s2c | 91 | 10 | `u32be` host, `u16be`, `u8`, `u16be`, `u8` | `Client.read` | `Client.java:7264` |
| s2c | 95 | 7 | zone `u8`, `u16be` id, `u16be` old count, `u16be` new count | `Client.readZonePacket` | `Client.java:2339` |
| s2c | 101 | 0 | empty; stops five camera shakes | `Client.read` | `Client.java:7388` |
| s2c | 105 | 6 | `u8` skill, `u32be` experience, `u8` level | `Client.read` | `Client.java:7014` |
| s2c | 106 | -2 | `u16be` component, `u8` count, then that many (`u16be` item, `u8` amount, or `u32be` amount when the `u8` is 255); remaining slots are zeroed | `Client.read` | `Client.java:6477` |
| s2c | 107 | 15 | zone `u8`, `i8` dx, `i8` dz, `i16be` target, `u16be`, `u8` start height, `u8` end height, `u16be` delay, `u16be`, `u8`, `u8` | `Client.readZonePacket` | `Client.java:2216` |
| s2c | 114 | 3 | `u8` public filter, `u8` private filter, `u8` trade filter | `Client.read` | `Client.java:7033` |
| s2c | 115 | 0 | empty; clears the minimap flag | `Client.read` | `Client.java:7229` |
| s2c | 125 | 6 |  | none | `Client.java:7523` |
| s2c | 129 | 4 | `u16be` component, `u16be` model id; model type becomes 1 | `Client.read` | `Client.java:6646` |
| s2c | 130 | 2 | `i16be` interface id into `valueClient128` | `Client.read` | `Client.java:6408` |
| s2c | 133 | 3 | `u16be`, `u8` | `Client.read` | `Client.java:7293` |
| s2c | 134 | 4 | `u16be` component, `i16be` animation | `Client.read` | `Client.java:6948` |
| s2c | 138 | 4 | zone `u8`, `u8` (type in the high 6 bits, rotation in the low 2), `u16be` loc id | `Client.readZonePacket` | `Client.java:2362` |
| s2c | 142 | 4 | `u16be` component, `u16be` id; model type becomes 2 | `Client.read` | `Client.java:6446` |
| s2c | 153 | 2 | `u8` zone base x, `u8` zone base z; clears objects in that 8 by 8 | `Client.read` | `Client.java:6544` |
| s2c | 156 | 6 | `u8` type. Type 1 reads `u16be` and leaves 3 bytes unread. Types 2..6 read `u16be` x, `u16be` z, `u8` height, and the type is then stored as 2, so the following type-10 branch does not run. Any other type leaves 5 bytes unread | `Client.read` | `Client.java:6889` |
| s2c | 158 | 4 | `u16be` modal, `u16be` sidebar | `Client.read` | `Client.java:6455` |
| s2c | 161 | -1 | `jstr`. Suffixes `:tradereq:`, `:duelreq:`, and `:chalreq:` are split off the front name | `Client.read` | `Client.java:6590` |
| s2c | 166 | 2 | `u16be` chat interface | `Client.read` | `Client.java:7247` |
| s2c | 167 | -2 | player block in `sync.md` | `Client.readPlayerInfo` | `Client.java:6820` |
| s2c | 171 | 0 | empty; closes modal, sidebar override, and chat interface | `Client.read` | `Client.java:6567` |
| s2c | 172 | -2 | `u16be` component, then while bytes remain: `u8` slot, `u16be` item, `u8` amount, or `u32be` amount when the `u8` is 255 | `Client.read` | `Client.java:7306` |
| s2c | 173 | 2 | zone `u8`, `u8` (type in the high 6 bits, rotation in the low 2); loc id is forced to -1 | `Client.readZonePacket` | `Client.java:2370` |
| s2c | 176 | 14 | zone `u8`, `u8` (type in the high 6 bits, rotation in the low 2), `u16be` loc, `u16be`, `u16be`, `u16be` player, `i8`, `i8`, `i8`, `i8` | `Client.readZonePacket` | `Client.java:2274` |
| s2c | 183 | 4 | `u16be` component, `u16be` colour packed 5-5-5 | `Client.read` | `Client.java:7089` |
| s2c | 185 | 1 | `u8` friend-list state | `Client.read` | `Client.java:7479` |
| s2c | 190 | 0 | empty; copies server varps into the local array | `Client.read` | `Client.java:7120` |
| s2c | 192 | 2 | `u16be` component; model type becomes 3 from the local player | `Client.read` | `Client.java:6807` |
| s2c | 194 | 1 | `u8` weight | `Client.read` | `Client.java:7108` |
| s2c | 195 | -2 | `u8` zone base x, `u8` zone base z, then while bytes remain: `u8` sub-opcode and `readZonePacket` | `Client.read` | `Client.java:7001` |
| s2c | 197 | -2 | NPC block in `sync.md` | `Client.readNpcInfo` | `Client.java:7473` |
| s2c | 200 | 6 | `u8` x, `u8` z, `u16be` height, `u8`, `u8` | `Client.read` | `Client.java:7198` |
| s2c | 203 | 3 | `u16be` varp, `i8` value | `Client.read` | `Client.java:6855` |
| s2c | 207 | 1 | `u8` system-update timer | `Client.read` | `Client.java:7300` |
| s2c | 210 | 0 | empty; opens the amount input | `Client.read` | `Client.java:6992` |
| s2c | 211 | 2 | `u16be` modal | `Client.read` | `Client.java:7363` |
| s2c | 215 | 3 | `u16be` component (65535 becomes -1), `u8` tab | `Client.read` | `Client.java:7215` |
| s2c | 219 | 7 | zone `u8`, `u16be` id, `u16be` count, `u16be` owner; skipped when the owner is the local player id | `Client.readZonePacket` | `Client.java:2254` |
| s2c | 227 | 2 | `u16be` component; each inventory slot is set to -1 and then to 0 | `Client.read` | `Client.java:6935` |
| s2c | 231 | 4 | `u16be` zone x, `u16be` zone z | `Client.read` | `Client.java:6655` |
| s2c | 233 | 6 | `u8` x, `u8` z, `u16be` height, `u8`, `u8` | `Client.read` | `Client.java:6962` |
| s2c | 235 | -1 | `u64be` name, `u32be` message counter, `u8` rights, `wordpack(payloadLength - 13)` | `Client.read` | `Client.java:7043` |
| s2c | 240 | 2 | `i16be` interface into `valueClient236` | `Client.read` | `Client.java:6844` |
| s2c | 241 | 1 | `u8` selected tab | `Client.read` | `Client.java:7335` |
| s2c | 245 | 6 | `u16be` varp, `i32be` value | `Client.read` | `Client.java:6872` |
| s2c | 247 | 9 | `u64be` name, `u8` world | `Client.read` | `Client.java:7140` |

Opcodes whose size is 0 and that have no branch still consume an empty payload and then log `T1` and log out. They are one row in `gaps.md`.

## C2S

Field order for walks, chat, and menus is `input.md`. Each row repeats the payload.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | none | `u8 19` | `Client.useMenuOption` | `Client.java:11251` |
| c2s | 2 | none | `u16be` id, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11630` |
| c2s | 9 | none | `u16be` component | `Client.useMenuOption` | `Client.java:11147` |
| c2s | 12 | none | `psize1` of the random bytes in `input.md` | `Client.addProjectiles` | `Client.java:9034` |
| c2s | 13 | none | `u64be` name | `Client.addFriend` | `Client.java:2419` |
| c2s | 20 | none | `u32be` packed click | `Client.updateGame` | `Client.java:10745` |
| c2s | 24 | none | `u24be 13018169` | `Client.useMenuOption` | `Client.java:11618` |
| c2s | 25 | none | `u32be 0` | `Client.useMenuOption` | `Client.java:11264` |
| c2s | 36 | none | `u16be` player, `u16be` item, `u16be` component, `u16be` slot | `Client.useMenuOption` | `Client.java:11569` |
| c2s | 39 | none | `u16be` x+origin, `u16be` z+origin, `u16be` loc, `u16be` item, `u16be` component, `u16be` slot | `Client.useMenuOption` | `Client.java:11067` |
| c2s | 41 | none | `u32be 0` | `Client.useMenuOption` | `Client.java:11278` |
| c2s | 42 | none | `u16be` id, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11634` |
| c2s | 46 | none | `u16be` item, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11182` |
| c2s | 51 | none | empty | `Client.closeModal` | `Client.java:5229` |
| c2s | 52 | none | `u8 50` | `Client.opClient7` | `Client.java:7934` |
| c2s | 53 | none | `u16be` pitch, `u16be` yaw | `Client.updateGame` | `Client.java:10760` |
| c2s | 60 | none | loc triple, then `u16be` item, `u16be` component, `u16be` slot | `Client.interactWithLoc` | `Client.java:5175` |
| c2s | 62 | none | `u16be` x+origin, `u16be` z+origin, `u16be` id | `Client.useMenuOption` | `Client.java:11255` |
| c2s | 72 | none | `u16be` component | `Client.useMenuOption` | `Client.java:11370` |
| c2s | 73 | none | `u8` 1 on focus gained, 0 on focus lost | `Client.updateGame` | `Client.java:10767` |
| c2s | 74 | none | `u16be` item, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11195` |
| c2s | 80 | none | `u8 131` | `Client.useMenuOption` | `Client.java:11109` |
| c2s | 82 | none | `u16be` item, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11174` |
| c2s | 86 | none | `u8` length (`steps * 2 + 17`), then the opcode-207 walk bytes, then the 14 minimap bytes in `input.md` | `Client.opClient8` | `Client.java:8339` |
| c2s | 89 | none | empty | `Client.updateGame` | `Client.java:10885` |
| c2s | 91 | none | `u16be` x+origin, `u16be` z+origin, `u16be` id, `u16be` selected component | `Client.useMenuOption` | `Client.java:11695` |
| c2s | 93 | none | `u16be` component, `u16be` from slot, `u16be` to slot, `u8` mode | `Client.updateGame` | `Client.java:10865` |
| c2s | 94 | none | `u8` tab | `Client.drawGame` | `Client.java:3321` |
| c2s | 98 | none | `u16be` player index | `Client.useMenuOption` | `Client.java:11103` |
| c2s | 100 | none | empty | `Client.draw3DEntityElements` | `Client.java:9102` |
| c2s | 101 | none | `u64be` name | `Client.opClient16` | `Client.java:9686` |
| c2s | 102 | none | `u32be` amount | `Client.handleInputKey` | `Client.java:1289` |
| c2s | 103 | none | loc triple from `interactWithLoc` | `Client.interactWithLoc` | `Client.java:5175` |
| c2s | 106 | none | `u64be` name | `Client.opClient30` | `Client.java:2794` |
| c2s | 108 | none | `u16be` x+origin, `u16be` z+origin, `u16be` id | `Client.useMenuOption` | `Client.java:11245` |
| c2s | 109 | none | `u16be` player index | `Client.useMenuOption` | `Client.java:11113` |
| c2s | 117 | none | `u16be` x+origin, `u16be` z+origin, `u16be` id | `Client.useMenuOption` | `Client.java:11282` |
| c2s | 120 | none | empty | `Client.updateGame` | `Client.java:11007` |
| c2s | 123 | none | `u16be` id, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11638` |
| c2s | 125 | none | `u8` gender, 7 `u8` kits, 5 `u8` colours | `Client.handleInterfaceAction` | `Client.java:9867` |
| c2s | 127 | none | loc triple from `interactWithLoc` | `Client.interactWithLoc` | `Client.java:5175` |
| c2s | 135 | none | `u16be` npc, `u16be` slot, `u16be` component, `u16be` selected component | `Client.useMenuOption` | `Client.java:11592` |
| c2s | 136 | none | `u16be` item, `u16be` slot, `u16be` component, `u16be` used item, `u16be` used component, `u16be` used slot | `Client.useMenuOption` | `Client.java:11487` |
| c2s | 137 | none | `u64be` name, `u8` reason, `u8` mute | `Client.handleInterfaceAction` | `Client.java:9858` |
| c2s | 138 | none | `u8` length (`steps * 2 + 3`), `u8` run, `u16be` x, `u16be` z, then `(steps - 1)` pairs of `i8` dx, `i8` dz | `Client.opClient8` | `Client.java:8344` |
| c2s | 139 | none | `psize1` of `u64be` name and wordpack | `Client.handleInputKey` | `Client.java:1240` |
| c2s | 147 | none | `u16be` npc index | `Client.useMenuOption` | `Client.java:11541` |
| c2s | 149 | none | `psize1` of the random bytes in `input.md` | `Client.interactWithLoc` | `Client.java:5126` |
| c2s | 150 | none | `u16be` npc, `u16be` item, `u16be` component, `u16be` slot | `Client.useMenuOption` | `Client.java:11383` |
| c2s | 154 | none | `u8` public, `u8` private, `u8` trade | `Client.handleInputKey` | `Client.java:1252` |
| c2s | 157 | none | loc triple from `interactWithLoc` | `Client.interactWithLoc` | `Client.java:5175` |
| c2s | 166 | none | `u16be` player index | `Client.useMenuOption` | `Client.java:11299` |
| c2s | 169 | none | `u16be` x+origin, `u16be` z+origin, `u16be` id | `Client.useMenuOption` | `Client.java:11272` |
| c2s | 174 | none | `u16be` player index | `Client.useMenuOption` | `Client.java:11303` |
| c2s | 179 | none | `u16be` item, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11178` |
| c2s | 181 | none | `u16be` npc, `u16be` selected component | `Client.useMenuOption` | `Client.java:11669` |
| c2s | 185 | none | `u16be` id, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11626` |
| c2s | 187 | none | loc triple from `interactWithLoc` | `Client.interactWithLoc` | `Client.java:5175` |
| c2s | 188 | none | `u8 62` | `Client.addPlayers` | `Client.java:11837` |
| c2s | 189 | none | `u16be` npc index | `Client.useMenuOption` | `Client.java:11533` |
| c2s | 196 | none | `u16be` player index | `Client.useMenuOption` | `Client.java:11307` |
| c2s | 201 | none | `u16be 37954` | `Client.useMenuOption` | `Client.java:11435` |
| c2s | 207 | none | `u8` length (`steps * 2 + 3`), `u8` run, `u16be` x, `u16be` z, then `(steps - 1)` pairs of `i8` dx, `i8` dz | `Client.opClient8` | `Client.java:8334` |
| c2s | 209 | none | empty | `Client.updateGame` | `Client.java:10932` |
| c2s | 213 | none | loc triple, then `u16be` selected component | `Client.interactWithLoc` | `Client.java:5175` |
| c2s | 214 | none | empty | `Client.opClient42` | `Client.java:4250` |
| c2s | 215 | none | loc triple from `interactWithLoc` | `Client.interactWithLoc` | `Client.java:5175` |
| c2s | 216 | none | `u16be` id, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11622` |
| c2s | 219 | none | `u32be 0` | `Client.useMenuOption` | `Client.java:11678` |
| c2s | 222 | none | `psize1` of the mouse samples in `input.md` | `Client.updateGame` | `Client.java:10650` |
| c2s | 223 | none | `u16be` npc index | `Client.useMenuOption` | `Client.java:11545` |
| c2s | 224 | none | `u8` (typed length minus 1), `jstr` of the text after `::` | `Client.handleInputKey` | `Client.java:1325` |
| c2s | 230 | none | `u8 232` | `Client.handleInputKey` | `Client.java:1197` |
| c2s | 233 | none | `u16be` npc index | `Client.useMenuOption` | `Client.java:11537` |
| c2s | 235 | none | `u8 154` | `Client.useMenuOption` | `Client.java:11099` |
| c2s | 236 | none | `u16be` npc index | `Client.useMenuOption` | `Client.java:11529` |
| c2s | 239 | none | `u16be` item, `u16be` slot, `u16be` component | `Client.useMenuOption` | `Client.java:11199` |
| c2s | 240 | none | `u16be` player, `u16be` selected component | `Client.useMenuOption` | `Client.java:11585` |
| c2s | 247 | none | `u16be` x+origin, `u16be` z+origin, `u16be` id | `Client.useMenuOption` | `Client.java:11268` |
| c2s | 250 | none | `u16be 6118` | `Client.useMenuOption` | `Client.java:11191` |
| c2s | 253 | none | `psize1` of `u8` colour, `u8` effect, and wordpack | `Client.handleInputKey` | `Client.java:1401` |
| c2s | 255 | none | `u64be` name | `Client.addIgnore` | `Client.java:10594` |

The loc triple is `u16be` world x, `u16be` world z, `u16be` loc id (`uid >> 14 & 32767`), written at `Client.java:5176`. Opcode 60 and 213 append their extra `u16be` values only when `interactWithLoc` returns true.
