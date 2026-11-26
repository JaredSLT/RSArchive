# Input

Mouse clicks, keys, camera keys, walking, the minimap, and widget ops are collected in `updateGame` (`Client.java:10547`) after the server read. The packets below are C2S. There is no C2S size table. Widths are the `Packet` writes named in [framing.md](framing.md).

## Mouse and keys

`GameShell` copies the click before `refresh`. In game:

| Packet | When | Body |
| --- | --- | --- |
| 234 | Any click (`Client.java:10660`) | u32be `(dt << 20) + (button << 19) + y * 765 + x`. `dt` is 50 ms units, capped at 4095. `button` is 1 when the click button is 2. `y` is clamped to 0..502 and `x` to 0..764. |
| 232 | Mouse samples when tracking is on and the button is down or 40 samples are queued (`Client.java:10565`) | u8 length, then samples. A repeated point only increments a delta, up to 2047. If delta is below 8 and both axis deltas are in -32..31, one u16be is `(delta << 12) + ((dx + 32) << 6) + (dy + 32)`. If delta is below 8 and the delta is larger, one u24be is `8388608 + (delta << 19) + (y * 765 + x)`. Otherwise one u32be is `-1073741824 + (delta << 19) + packed`. A sample of x=-1,y=-1 uses packed value 524287. |
| 8 | Focus gained or lost (`Client.java:10682`) | u8 1 or 0 |
| 91 | Arrow keys, then at most once per 20 loops (`Client.java:10675`) | u16be orbit pitch, u16be orbit yaw |
| 226 | `valueClient18` passes 192 (`Client.java:4786`) | u8 232 |
| 144 | `idleCycles` passes 4500 (`Client.java:10856`) | no body. `pendingLogout` becomes 250. |
| 182 | A counter passes 62 (`Client.java:10809`) | no body |
| 239 | `noTimeoutCycle` passes 50 (`Client.java:10931`), and four times inside `buildScene` | no body |

`InputTracking` (`InputTracking.java:87`) is turned on by S2C 251 and flushed as C2S 142: u16be length plus that many raw bytes (`Client.java:10697` and the S2C 29 reply at `Client.java:3354`). Event codes, each preceded by the tracking buffer's own bookkeeping:

| Code | Body |
| --- | --- |
| 1 or 2 | Mouse pressed, button 1 uses code 1. u8 `dt/10` capped at 250, then u24be `x + (y << 10)`. Only when x is in 0..788 and y is in 0..531. |
| 3 or 4 | Mouse released. u8 dt. |
| 5 | Move, recorded at most every 50 ms, when both deltas are in -8..7. u8 dt, u8 `(dx + 8) + ((dy + 8) << 4)`. |
| 6 | Move when both deltas are in -128..127. u8 dt, u8 `dx + 128`, u8 `dy + 128`. |
| 7 | Other moves. u8 dt, u24be absolute `x + (y << 10)`. |
| 8 / 9 | Key down / key up. Keys 1000, 1001, 1002, 1003 become 11, 12, 14, 15. A key at or above 1008 subtracts 992. Then u8 dt, u8 key. |
| 10 / 11 | Focus gained / lost. u8 dt. |
| 12 / 13 | Mouse entered / exited. u8 dt. |

`flush` returns the completed buffer (`InputTracking.java:44`). `stop` returns the active buffer when it is non-empty (`InputTracking.java:59`).

## Camera

`updateOrbitCamera` (`Client.java:2009`) chases the local player. Keys 1 and 2 change yaw velocity. Keys 3 and 4 change pitch velocity. Pitch is clamped to 128..383. The sent packet is opcode 91 above. Cutscene cameras come from S2C 55 and S2C 0 and are in [packets.md](packets.md).

## Walk

`tryMove` (`Client.java:6622`) writes the walk packet only when a path exists. `var5` 0 writes opcode 6, `var5` 1 writes opcode 220, and `var5` 2 writes opcode 127 (`Client.java:6806`). The path is compressed to at most 25 waypoints. Waypoint 0 of that compression is the click tile (`flagSceneTileX` / `flagSceneTileZ` at `Client.java:6829`). The first coordinate written is the waypoint nearest the player, after the final `--var26`. Later waypoints are u8 deltas from that first coordinate, not from each previous waypoint. A one-waypoint path writes no delta pair.

Body for opcodes 6 and 127:

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | u8 | `2 * n + 3`. `n` includes the destination. |
| 2 | u8 | 1 when action key 5 is down, else 0 |
| 3 | u16be | first waypoint x plus `sceneBaseTileX` |
| 4 | u16be | first waypoint z plus `sceneBaseTileZ` |
| 5 | u8, u8, repeated `n - 1` times | dx and dz from that first waypoint (`p1` of a signed tile difference, `Client.java:6834`) |

Opcode 220 uses length `2 * n + 3 + 14` and the same body. `handleMinimapInput` then appends 14 bytes only when `tryMove` returns true (`Client.java:8238`): u8 click x offset, u8 click y offset, u16be orbit yaw, u8 57, u8 minimap angle, u8 minimap zoom, u8 89, u16be `localPlayer.x`, u16be `localPlayer.z`, u8 `tryMoveNearest`, u8 63.

A ground click calls `tryMove` with `var5` 0 (`Client.java:10815`), so the opcode is 6. Other scene clicks use `var5` 2, opcode 127. The minimap uses `var5` 1, opcode 220.

The minimap click origin is `mouseClickX - 550 - 25` and `mouseClickY - 4 - 5`, accepted inside 146 by 151 (`Client.java:8222`). It then subtracts 73 and 75, rotates by `orbitCameraYaw + macroMinimapAngle`, and scales by `macroMinimapZoom + 256`. The destination tile is `(player.x + dx) >> 7` and `(player.z - dz) >> 7`.

## Widget and world ops

`useMenuOption` (`Client.java:821`) and `interactWithLoc` (`Client.java:10082`) write these bodies. Loc packets from `interactWithLoc` are u16be world x, u16be world z, u16be loc id, using the caller's opcode. Opcode 26 appends one more u16be spell (`Client.java:1016`). Opcode 240 appends three more u16be item fields (`Client.java:1196`).

| Opcode | Body | Site |
| --- | --- | --- |
| 4 | u8 50 | minimap build counter `Client.java:9933` |
| 9 | u64be name | add friend `Client.java:8324` |
| 13 | u8 gender (0 when `designGender` is true), seven u8 kits, five u8 colours | design confirm `Client.java:1989` |
| 17 | u16be pid | player option `Client.java:1372` |
| 18 | u16be pid | `Client.java:1354` |
| 28 | u32be 0 | rare, before loc opcode 213 `Client.java:977` |
| 33 | three world u16be | loc `Client.java:1391` |
| 36 | u8 62 | flag reached `Client.java:9949` |
| 47 | three world u16be | loc `Client.java:1144` |
| 51 | u8 0, then a u8 length of a random tail | `Client.java:12412` |
| 56 | u32be 0 | rare, before loc opcode 97 `Client.java:1113` |
| 58 | no body | close modal `Client.java:2099` |
| 59, 62, 70, 160, 181 | u16be item, u16be slot, u16be component | inventory `Client.java:1240` |
| 67, 97, 141, 178 | three world u16be | loc `Client.java:1107` |
| 68 | u16be pid, u16be spell | `Client.java:1403` |
| 69, 118, 122, 143, 195 | u16be npc id | `Client.java:993` |
| 72 | u16be pid | attack or follow `Client.java:1171` |
| 74, 80, 163, 228, 243 | u16be item, u16be slot, u16be component | `Client.java:866`. Opcode 162 (u24be 13018169) can precede 163. |
| 77 | u16be 37954 | rare, before loc opcode 98 `Client.java:1481` |
| 83 | u8 length, u8 colour, u8 effect, wordpack | public chat `Client.java:4990`. The length covers the colour, the effect, and the wordpack. |
| 84 | u64be name | delete friend `Client.java:5359` |
| 86 | u8 `text.length - 1`, jstr of the text after `::` | command `Client.java:4914` |
| 87, 98, 147, 213 | three world u16be | loc via `interactWithLoc` |
| 100 | no body | cross anticheat `Client.java:10505` |
| 102 | u16be item, u16be slot, u16be component, u16be spell | `Client.java:848` |
| 113 | u16be pid, u16be obj component, u16be slot, u16be interface | `Client.java:1470` |
| 119 | u16be npc, then the same three item u16be | `Client.java:1313` |
| 121 | u8 131 | rare, before player opcode 192 `Client.java:1177` |
| 129 | u8 public, u8 private, u8 trade | `Client.java:4841` |
| 131 | u16be 6118 | rare, before inventory opcode 181 `Client.java:1251` |
| 134 | no body | scene ready `Client.java:1629` |
| 142 | u16be length, then that many bytes | input tracking |
| 146 | u16be component | continue `Client.java:1330` |
| 161 | u32be amount | `Client.java:4878` |
| 176 | u16be interface, u16be from slot, u16be to slot, u8 mode | drag `Client.java:10789` |
| 187 | u32be 0 | rare, before loc opcode 141 `Client.java:1130` |
| 189 | u64be name | add ignore `Client.java:6505` |
| 192, 230 | u16be pid | `Client.java:1181`, `Client.java:1368` |
| 193 | u64be name | delete ignore `Client.java:2559` |
| 200 | six u16be: item, slot, component, selected obj, selected slot, selected interface | `Client.java:910` |
| 201 | u8 side tab | flashing tab finished `Client.java:8865` |
| 202 | u16be x, u16be z, u16be item, u16be spell | `Client.java:1089` |
| 203 | u64be name, u8 `clientCode - 601`, u8 mute | report abuse `Client.java:1980` |
| 206 | u8 19 | rare, before loc opcode 47 `Client.java:1140` |
| 214 | u8 length, u64be name, wordpack | private message `Client.java:4829`. The length covers the name and the wordpack. |
| 225 | u8 0, then a u8 length of a random tail | loc anticheat `Client.java:10097` |
| 231 | u16be npc, u16be spell | `Client.java:1456` |
| 233 | u8 154 | rare, before player opcode 72 `Client.java:1167` |
| 244 | u16be component | button `Client.java:938` |
| 245 | three world u16be, then three item u16be | item on loc `Client.java:1296` |

Tabs are hit-tested in `handleTabInput` (`Client.java:5138`) for indexes 0..13. `tabInterfaceId` is 15 ints, initially -1 (`Client.java:733`). The default `sideTab` after login is 3 (`Client.java:4228`).

`WordPack.pack` (`WordPack.java:65`) lowercases, truncates to 80 characters, and nibble-packs against `TABLE`.

Design defaults are not fixed style ids. `validateCharacterDesign` (`Client.java:2755`) stores the first non-disabled `IdkType` whose type equals the slot, plus 7 when `designGender` is false. `designGender` starts true and the five colours are set to 0 (`Client.java:4235`).
