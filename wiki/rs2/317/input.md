# Input

Widget menus store `menuParamA` as the object id, `menuParamB` as the slot, and `menuParamC` as the component id. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:1829`. Loc and ground-item menus store the bitset, the local x, and the local z. The loc id sent on the wire is `(bitset >> 14) & 32767`.

`useMenuOption` is `Client.java:4585`. Width names are in [framing.md](framing.md).

## Mouse

Opcode 241 is one `i32be`: `(dt << 20) + (right << 19) + (y * 765 + x)`. `y` is clamped to 0..502, `x` to 0..764, and `dt` is the click gap in 50 ms units clamped to 4095. The right bit is 1 when the button is 2. `Client.java:3911`.

Opcode 45 is sent only when `flagged` is set and a click is down or 40 samples are queued. `u8` length, then records until adding another would pass 240 bytes. A repeated point increments a duplicate count up to 2047 and is not written yet. The next moved point writes one of:

- `u16be` `(duplicates << 12) + ((dx + 32) << 6) + (dy + 32)` when duplicates are below 8 and both deltas are in -32..31
- `u24be` `8388608 + (duplicates << 19) + (y * 765 + x)` when duplicates are below 8
- `i32be` `-1073741824 + (duplicates << 19) + (y * 765 + x)` otherwise

A sample of x -1 and y -1 uses position word 524287. `Client.java:3844`.

## Keys, camera, focus

Opcode 3 is `u8` 1 when the client gains focus and `u8` 0 when it loses focus. `Client.java:3959`.

Opcode 86 is `u16be` orbit pitch and `u16be_add` orbit yaw, sent when an arrow key is down and at least 20 ticks have passed. `Client.java:3947`.

Public chat opcode 4: placeholder `u8`, `u8_sub` effect, `u8_sub` colour, then `WordPack.pack` written with `bytes_rev_add`, then `psize1`. Colour prefixes `yellow` through `glow3` set colour 0..11. Effect prefixes `wave`, `wave2`, `shake`, `scroll`, and `slide` set effect 1..5. `Client.java:6104`.

Command opcode 103: `u8` `text.length() - 1`, then `jstr` of the text after the leading `::`. The length counts the terminator that `pjstr` adds. `Client.java:6100`.

Opcode 95 is three `u8` values: public, private, and trade chat settings. `Client.java:2030`.

Opcode 208 is `i32be` of the parsed amount. Opcode 60 is `u64be` of the name. Opcode 126 is placeholder `u8`, `u64be` name, `WordPack.pack` written in order into the same buffer, then `psize1`. `Client.java:5977`.

## Walk and run

`tryMove` at `Client.java:7414`. The first argument selects the opcode: 0 writes 164, 1 writes 248, 2 writes 98. A tile click uses 0 (`World3D.clickTile`). The minimap uses 1. An interaction path uses 2.

The body is `u8` length, `u16le_add` of the first waypoint x plus `sceneBaseTileX`, then for each later waypoint `u8` dx and `u8` dz relative to that first x and z, then `u16le` of the first waypoint z plus `sceneBaseTileZ`, then `u8_neg` of 1 when `actionKey[5]` is 1 and otherwise 0. The length is `2 * steps + 3`, or `2 * steps + 3 + 14` for opcode 248. Steps are capped at 25. The first waypoint is the one nearest the player. Later waypoints walk toward the destination. The flag tile is the destination, `bfsStepX[0]` and `bfsStepZ[0]`.

If the search finds no route, argument 1 returns false and the minimap tail is not written. Any other argument returns true so the interaction packet is still written.

When the walk is accepted, opcode 248 appends `u8` local click x, `u8` local click y, `u16be` yaw, `u8` 57, `u8` minimap angle, `u8` minimap zoom, `u8` 89, `u16be` player x, `u16be` player z, `u8` `tryMoveNearest`, `u8` 63. `Client.java:8393`.

Opcode 36, `i32be` 0, is written before the walk opcode when `oplogic4` reaches 92. `Client.java:7414`.

Idle opcode 202 has no payload and is written when `idleCycles` passes 4500. `Client.java:4120`. Opcode 0 with no payload is the heartbeat. `Client.java:4197`.

## Widget, player, NPC, loc, and ground packets

The write order is the field order. Full rows are in [packets.md](packets.md).

| Opcode | Menu | Fields |
| --- | --- | --- |
| 185 | 315, 646, 169 | `u16be` component |
| 40 | 679 | `u16be` component |
| 145 | 632 | three `u16be_add`: interface, slot, item |
| 117 | 78 | `u16le_add` interface, `u16le_add` item, `u16le` slot |
| 43 | 867 | `u16le` interface, `u16be_add` item, `u16be_add` slot |
| 75 | 493 | `u16le_add` interface, `u16le` slot, `u16be_add` item |
| 16 | 539 | `u16be_add` item, `u16le_add` slot, `u16le_add` interface |
| 41 | 454 | `u16be` item, `u16be_add` slot, `u16be_add` interface |
| 135 | 53 | `u16le` slot, `u16be_add` interface, `u16le` item |
| 122 | 74 | `u16le_add` interface, `u16be_add` slot, `u16le` item |
| 129 | 431 | `u16be_add` slot, `u16be` interface, `u16be_add` item |
| 87 | 847 | `u16be_add` item, `u16be` interface, `u16be_add` slot |
| 237 | 543 | `u16be` slot, `u16be_add` item, `u16be` interface, `u16be_add` spell |
| 53 | 870 | `u16be` slot, `u16be_add` selected slot, `u16le_add` item, `u16be` selected interface, `u16le` selected item, `u16be` interface |
| 214 | drag | `u16le_add` interface, `u8_neg` mode, `u16le_add` from slot, `u16le` to slot |
| 155 | 20 | `u16le` npc |
| 17 | 225 | `u16le_add` npc |
| 21 | 965 | `u16be` npc |
| 18 | 478 | `u16le` npc |
| 72 | 412 | `u16be_add` npc |
| 131 | 413 | `u16le_add` npc, `u16be_add` spell |
| 57 | 582 | `u16be_add` item, `u16be_add` npc, `u16le` slot, `u16be_add` interface |
| 128 | 561 and 6 | `u16be` player |
| 153 | 779 | `u16le` player |
| 39 | 729 | `u16le` player |
| 73 | 27 | `u16le` player |
| 139 | 484 and 577 | `u16le` player |
| 249 | 365 | `u16be_add` player, `u16le` spell |
| 14 | 491 | `u16be_add` selected interface, `u16be` player, `u16be` selected item, `u16le` selected slot |
| 252 | 900 | `u16le_add` loc, `u16le` z, `u16be_add` x |
| 132 | 502 | `u16le_add` x, `u16be` loc, `u16be_add` z |
| 70 | 113 | `u16le` x, `u16be` z, `u16le_add` loc |
| 234 | 872 | `u16le_add` x, `u16be_add` loc, `u16le_add` z |
| 228 | 1062 | `u16be_add` loc, `u16be_add` z, `u16be` x |
| 35 | 956 | `u16le` x, `u16be_add` spell, `u16be_add` z, `u16le` loc |
| 192 | 62 | `u16be` interface, `u16le` loc, `u16le_add` z, `u16le` slot, `u16le_add` x, `u16be` item |
| 236 | 234 | `u16le` z, `u16be` id, `u16le` x |
| 253 | 244 | `u16le` x, `u16le_add` z, `u16be_add` id |
| 23 | 567 | `u16le` z, `u16le` id, `u16le` x |
| 79 | 213 | `u16le` z, `u16be` id, `u16be_add` x |
| 156 | 652 | `u16be_add` x, `u16le` z, `u16le_add` id |
| 181 | 94 | `u16le` z, `u16be` id, `u16le` x, `u16be_add` spell |
| 25 | 511 | `u16le` selected interface, `u16be_add` selected item, `u16be` ground id, `u16be_add` z, `u16le_add` slot, `u16be` x |

Opcode 214 mode is 1 when `bankArrangeMode` is 1 and the component client code is 206, and the destination slot holds an item. Otherwise it is 0. `Client.java:4026`.

Social opcodes: 188 add friend `u64be`, 215 remove friend `u64be`, 133 add ignore `u64be`, 74 remove ignore `u64be`, 218 report `u64be` name, `u8` rule `0..11`, `u8` mute. `Client.java:3174`.

Design opcode 101: `u8` gender 0 male or 1 female, seven `u8` identikit ids, five `u8` colours. `Client.java:3183`. The ids come from `validateCharacterDesign`, which picks the first `IdkType` whose disable flag is clear and whose type equals the slot, or the slot plus 7 for a female. `Client.java:2912`.

Opcode 120 is `u8` selected tab, sent when a flashing tab is cleared. `Client.java:9047`. Opcode 130 closes interfaces and has no payload. `Client.java:13371`. Opcode 121 has no payload and is sent when the scene finishes loading. `Client.java:3537`.

Tab clicks do not send a packet by themselves. `handleTabInput` is `Client.java:6690`.
