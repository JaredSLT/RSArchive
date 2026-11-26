# Input

Citations are `builds/rs2/282-2004-12-21/src/src/main/java/deob/Client.java` unless noted. Every game opcode is ISAAC-added by `Packet.opkind272`. There is no C2S size table. `psize1` means a placeholder `u8 0` whose value is later set to the number of bytes written after it (`Packet.opkind2712`).

`MouseTracking.run` (`MouseTracking.java:26`) stores up to 500 samples of `GameShell` mouse x and y and sleeps 50 ms.

## Mouse, camera, focus

`updateGame` (`Client.java:10632`) sends these while logged in:

| opcode | when | payload |
| --- | --- | --- |
| 222 | `valueClient183` is true (login status 2, second byte equals 1) and a click is down or 40 samples are queued | `psize1` of samples, stopping before the added bytes reach 240. A sample that repeats the previous x and y only increments an idle counter, up to 2047, and writes no bytes. A moved sample writes one of: if idle is below 8 and both deltas are in -32..31, `u16be (idle << 12) + ((dx + 32) << 6) + (dy + 32)`; else if idle is below 8, `u24be` of `8388608 + (idle << 19) + abs`; else `i32be` of `-1073741824 + (idle << 19) + abs`. `abs` is `y * 765 + x` with y clamped 0..502 and x 0..764. A sample of (-1, -1) uses abs 524287. `Client.java:10650` |
| 20 | a click is pending | `u32be (dt << 20) + (right << 19) + (y * 765 + x)`. `dt` is `(now - last) / 50` capped at 4095. Right is mouse button 2. `Client.java:10745` |
| 53 | an arrow key changed and the cooldown elapsed | `u16be` pitch `valueClient421`, `u16be` yaw `valueClient422`. `Client.java:10760` |
| 73 | focus gained | `u8 1`. `Client.java:10767` |
| 73 | focus lost | `u8 0`. `Client.java:10773` |
| 89 | every 62 ticks | empty. `Client.java:10885` |
| 209 | `valuekind111` passed 4500 | empty, and the counter drops by 500. `Client.java:10932` |
| 120 | every 50 ticks, and four times inside `buildScene` | empty. `Client.java:11007` and `:11926` |

## Keys and chat

`handleInputKey` (`Client.java:1192`):

| opcode | payload |
| --- | --- |
| 230 | every 192 keys, `u8 232` |
| 139 | private message. `psize1` of `u64be` name and `WordPack.opkind592`. `Client.java:1240` |
| 154 | `u8` public, `u8` private, `u8` trade. Also sent from chat-mode clicks at `:5669`, `:5679`, and `:5689` |
| 102 | `u32be` amount. `Client.java:1289` |
| 224 | command starting with `::`. `u8 (typedLength - 1)`, then `jstr` of `substring(2)`. The `u8` is not a `psize1` of the string. `Client.java:1325` |
| 253 | public chat. Placeholder `u8`, then `u8` colour, `u8` effect, `WordPack`, then `psize1` of those three. Colour prefixes: yellow 0, red 1, green 2, cyan 3, purple 4, white 5, flash1 6, flash2 7, flash3 8, glow1 9, glow2 10, glow3 11. Effect prefixes: wave 1, scroll 2, otherwise 0. `Client.java:1329` and `:1401` |

Friends and ignores:

| opcode | payload | site |
| --- | --- | --- |
| 13 | `u64be` name | `addFriend` `:2419` |
| 106 | `u64be` name | `opClient30` `:2794` |
| 255 | `u64be` name | `addIgnore` `:10594` |
| 101 | `u64be` name | `opClient16` `:9686` |

## Walk and minimap

`opClient8` (`Client.java:8149`) pathfinds on the current plane. The first written waypoint is the destination. Later waypoints are `i8` dx and `i8` dz from that destination. The step count is capped at 25. Run is `u8 1` when key index 5 is held, else `u8 0`.

| `var11` | opcode | length byte | payload after the length byte |
| --- | --- | --- | --- |
| 0 world click | 207 | `steps * 2 + 3` | `u8` run, `u16be` absolute x (`tile + valueClient197`), `u16be` absolute z, then `(steps - 1)` pairs of `i8` dx, `i8` dz |
| 1 minimap | 86 | `steps * 2 + 3 + 14` | the same walk bytes, then 14 bytes written by `opClient25` |
| 2 menu path | 138 | `steps * 2 + 3` | the same walk bytes as 207, with no 14-byte tail |

`opClient25` (`Client.java:1149`) hits when `mouseX - 575` is in 0..145 and `mouseY - 9` is in 0..150. It then subtracts 73 from that x and 75 from that y, calls `opClient8` with `var11` 1, and on success appends without a new opcode (`Client.java:1169`): `u8` (hit x minus 73), `u8` (hit y minus 75), `u16be` camera yaw, `u8 57`, `u8` minimap rotation, `u8` zoom, `u8 89`, `u16be` player fine x, `u16be` player fine z, `u8 valueClient363`, `u8 63`.

World click uses `var11` 0 from `updateGame` (`Client.java:10888`). Menu actions that walk first use `var11` 2 and then write a separate opcode.

`rslib/src/both/move/rev_282.c` `walk_encode` writes run, destination, and steps and omits the length byte and the 14-byte tail. Its comment at lines 539-541 records that omission. A fixed 5-byte body matches only a one-point walk with the length byte removed.

## Widget and menu opcodes

`useMenuOption` (`Client.java:11028`) subtracts 2000 from actions at or above 2000. Coordinates below are local tiles unless noted. `valueClient197` and `valueClient198` are the scene origin.

Loc packets from `interactWithLoc` (`Client.java:5175`), written only when the loc exists: opcode, `u16be` world x, `u16be` world z, `u16be` loc id (`uid >> 14 & 32767`).

| opcode | action | fields after the shared loc triple |
| --- | --- | --- |
| 157 | 357 | none |
| 215 | 625 | none |
| 187 | 743 | none on this packet; may be preceded by anticheat 201 |
| 127 | 1071 | none |
| 103 | 721 | none on this packet; may be preceded by anticheat 219 |
| 213 | 899 | `u16be` selected component `valueClient207`, only if `interactWithLoc` returns true |
| 60 | 810 | `u16be` item, `u16be` component, `u16be` slot, only if it returns true |

Other menu writes. A walk of opcode 138 may precede the packet when the row says so.

| opcode | action | payload |
| --- | --- | --- |
| 39 | 111 | after walk: `u16be` x+origin, `u16be` z+origin, `u16be` loc id, `u16be` item, `u16be` component, `u16be` slot |
| 98 | 507 and 387 | after walk: `u16be` player index. Action 507 and 387 may first send 235 |
| 109 | 957 and 639 | after walk: `u16be` player index. Those actions may first send 80 |
| 9 | 435, 225, and 231 | `u16be` component. Action 231 sends it only when `handleInterfaceAction` returns true |
| 82 | 113 | `u16be` item, `u16be` slot, `u16be` component |
| 179 | 331 | same three `u16be` |
| 46 | 354 | same three `u16be` |
| 74 | 582 | same three `u16be`. May be preceded by 250 |
| 239 | 555 | same three `u16be` |
| 108 | 617 | after walk: `u16be` x+origin, `u16be` z+origin, `u16be` id |
| 62 | 224 | same tail. May be preceded by 0 |
| 247 | 139 | same tail. May be preceded by 25 |
| 169 | 778 | same tail |
| 117 | 662 | same tail. May be preceded by 41 |
| 166 | 499 | after walk: `u16be` player index |
| 174 | 185 | after walk: `u16be` player index |
| 196 | 27 | after walk: `u16be` player index |
| 72 | 997 | `u16be` component, once per latch |
| 150 | 829 | after walk: `u16be` npc, `u16be` item, `u16be` component, `u16be` slot |
| 136 | 398 | `u16be` item, `u16be` slot, `u16be` component, `u16be` used item, `u16be` used component, `u16be` used slot |
| 236 | 242 | after walk: `u16be` npc index |
| 189 | 793 | after walk: `u16be` npc index |
| 233 | 209 | after walk: `u16be` npc index |
| 147 | 852 | after walk: `u16be` npc index |
| 223 | 309 | after walk: `u16be` npc index |
| 36 | 275 | after walk: `u16be` player, `u16be` item, `u16be` component, `u16be` slot |
| 240 | 131 | after walk: `u16be` player, `u16be` selected component |
| 135 | 563 | `u16be` npc, `u16be` slot, `u16be` component, `u16be` selected component |
| 216 | 681 | `u16be` id, `u16be` slot, `u16be` component. May be preceded by 24 |
| 185 | 694 | same three `u16be` |
| 2 | 962 | same three `u16be` |
| 42 | 100 | same three `u16be` |
| 123 | 795 | same three `u16be` |
| 181 | 240 | after walk to an NPC: `u16be` npc index, `u16be` selected component |
| 91 | 370 | after walk: `u16be` x+origin, `u16be` z+origin, `u16be` id, `u16be` selected component |
| 93 | inventory drag in `updateGame` | `u16be` component, `u16be` from slot, `u16be` to slot, `u8` mode. `Client.java:10865` |
| 94 | sidebar tab redraw | `u8` tab. `Client.java:3321` |
| 137 | report, component ids 601..612 | `u64be` name, `u8` (`contentId - 601`), `u8` mute flag. `Client.java:9858` |
| 125 | design confirm, component 326 | `u8` gender (0 when `valueClient298`, else 1), 7 `u8` identity kits, 5 `u8` colours. `Client.java:9867` |
| 51 | close modal | empty. `Client.java:5229` |
| 214 | scene built | empty. `Client.java:4250` |
| 52 | minimap rebuild anticheat | `u8 50`. `Client.java:7934` |
| 188 | local player reaches the flag tile and a counter passes 122 | `u8 62`. `Client.java:11837` |
| 100 | cross timer passes 57 | empty. `Client.java:9102` |

Anticheat payloads are written exactly as follows. They are not stable gameplay packets.

| opcode | payload |
| --- | --- |
| 235 | `u8 154` when counter `valueClient113` reaches 66, before opcode 98 |
| 80 | `u8 131` when `valueClient370` reaches 52, before opcode 109 |
| 250 | `u16be 6118` when `valueClient228` reaches 133, before opcode 74 |
| 0 | `u8 19` when `valueClient193` reaches 75, before opcode 62 |
| 25 | `u32be 0` when `valueClient63` reaches 123, before opcode 247 |
| 41 | `u32be 0` when `valueClient62` reaches 118, before opcode 117 |
| 201 | `u16be 37954` when `valueClient261` reaches 124, before loc opcode 187 |
| 219 | `u32be 0` when `valueClient120` reaches 139, before loc opcode 103 |
| 24 | `u24be 13018169` when `valueClient99` reaches 116, before opcode 216 |
| 149 | inside `interactWithLoc` when `valueClient305` passes 1086: placeholder `u8`, optional `u16be 16791`, `u8 254`, `u16be` random, `u16be 16128`, `u16be 52610`, `u16be` random, `u16be 55420`, optional `u16be 35025`, `u16be 46628`, `u8` random, `psize1`. `Client.java:5126` |
| 12 | inside `addProjectiles` when `valueClient297` passes 1174: placeholder `u8`, optional `u16be 11499`, `u16be 10548`, optional `u8 139`, optional `u8 94`, `u16be 51693`, `u8 16`, `u16be 15036`, optional `u8 65`, `u8` random, `u16be 22990`, `psize1`. `Client.java:9034` |

Actions that send no packet of their own: 1381 examine loc, 718 walk the ground locally, 1714 examine npc, 1152 examine object, 1328 examine a stack, 902 open the private-message prompt, 274 select a spell locally, 102 select an item locally, 524 open the report interface whose content id is 600, 737 `closeModal` (that method sends 51). Action 605 calls `addFriend` (opcode 13). Action 47 calls `addIgnore` (opcode 255). Action 513 calls `opClient30` (opcode 106). Action 884 calls `opClient16` (opcode 101).
