# Input

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`. There is no C2S size table. Length bytes below are the writer's own `u8`, not `Protocol.valueProtocol2`.

`MouseTracking.run` stores mouse x and y every 50 ms, up to 500 samples (`jagex2/client/MouseTracking.java:27`). `GameShell` is the mouse, key, and focus listener (`jagex2/client/GameShell.java:24`).

## Movement and camera

`opClient135` (`jagex2/client/Client.java:3888`) pathfinds on `CollisionMap.flags`. Cardinal masks are 19398920, 19399040, 19398914, and 19398944. Diagonals also test 19398926, 19399043, 19398968, and 19399136 (`Client.java:3937`). The approach helpers `opCollisionMap7`, `opCollisionMap8`, and `opCollisionMap9` are unread.

The walk opcode is 28 for a map click (`var6 == 0`), 213 for the minimap (`var6 == 1`), and 247 for an interaction path (`var6 == 2`) (`Client.java:4091`). At most 25 steps are sent. The `u8` length is `steps*2+3`, or that plus 14 for the minimap. Then `p2_alt3` of destination x plus `valueClient1170`, `u8` 1 when key 5 is down, `p2_alt3` of destination z plus `valueClient1171`, and for each earlier step `u8` dx and `p1_alt3` dz.

Minimap click `opClient125` (`Client.java:13461`) accepts x 0..145 and y 0..150 after subtracting 550+25 and 4+5. When the walk returns true it appends, on the same opcode 213 frame: `u8` local minimap x, `u8` local minimap y, `u16be` yaw `valueClient1372`, `u8` 57, `u8` yaw offset, `u8` zoom offset, `u8` 89, `u16be` player fine x, `u16be` player fine z, `u8` `valueClient1253`, `u8` 63. Minimap clicks require S2C `valueClient1179 == 0` (`Client.java:2564`).

A scene tile click uses opcode 28 from `World3D.valueWorld3D119` (`Client.java:1833`).

Arrow keys set a 20-tick timer and send opcode 140: `p2_alt1` `valueClient1371`, `p2_alt1` `valueClient1372` (`Client.java:1708`).

## Other direct input

| opcode | when | payload | citation |
| --- | --- | --- | --- |
| 171 | mouse samples, only if `valueClient1094` | `u8` length via `psize1`; each sample is `u16be` when the gap is under 8 and both deltas are in -32..31: `(gap<<12)\|((dx+32)<<6)\|(dy+32)`; else if gap under 8, `u24be` `0x800000\|(gap<<19)\|y*765+x`; else `u32be` `0xC0000000\|(gap<<19)\|packed`. Sentinel x=y=-1 packs 524287. X is capped at 764 and y at 502 | `Client.java:1601` |
| 19 | mouse press | `u32be` `((dt/50 capped 4095)<<20)\|(right button<<19)\|y*765+x` | `Client.java:1696` |
| 187 | focus | `u8` 1 gained, `u8` 0 lost | `Client.java:1718` |
| 202 | no input for 4500 ticks | empty, then a 250-tick delay | `Client.java:1894` |
| 40 | every 50 ticks, and during scene build | empty | `Client.java:1972` |
| 49 | public chat | `u8` 0 then `psize1` over `p1_alt2` colour, `p1_alt1` effect, and `WordPack` bytes. Text is cut at 80 characters | `Client.java:2352`, `jagex2/wordenc/WordPack.java:67` |
| 56 | command after `::` | `u8` `string.length-1`, then `pjstr` of the text after `::` | `Client.java:2289` |
| 176 | chat mode buttons | three `u8`: public, private, trade | `Client.java:2168` |
| 75 | numeric prompt | `u32be` parsed int, 0 on failure | `Client.java:2205` |
| 206 | name prompt | `u64be` base37 | `Client.java:2225` |
| 227 | private message | `u8` 0, `u64be` friend, `WordPack` text, `psize1` | `Client.java:2156` |
| 110 | close windows | empty | `Client.java:971` |

WordPack nibbles index `valueWordPack4` (`WordPack.java:11`). A nibble below 13 is one character. A nibble of 13 or more starts a two-nibble index minus 195 (`WordPack.java:13`).

Menu areas subtract 4,4 for the viewport, 553,205 for the sidebar, and 17,357 for the chat box (`Client.java:5083`). Spell selection action 70 stores the component in `valueClient1297` (`Client.java:11120`). Item selection action 52 stores slot, component, and item in `valueClient1274`, `valueClient1275`, and `valueClient1276` (`Client.java:11508`). Widget button packets are opcode 79 with one `u16be` component, from actions 352, 890, and 518 (`Client.java:10882`, `Client.java:11246`, `Client.java:11459`).

The per-action C2S layouts are the rows in [packets.md](packets.md).
