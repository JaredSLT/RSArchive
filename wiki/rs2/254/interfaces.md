# Interfaces

`Component.unpack` (`Component.java:83`) reads `data` from the interface jag. The media jag supplies sprite names. The file starts with a u16be count. Each record is a u16be id. An id of 65535 means the next u16be is the parent layer and the u16be after that is the real id. Then, for every component: u8 type, u8 button type, u16be client code, u16be width, u16be height, u8 transparency, and u8 overlayer. A non-zero overlayer reads a second u8 and stores `(first - 1) << 8 + second`. Otherwise the overlayer is -1. Then a u8 comparator count and that many pairs of u8 comparator and u16be operand, then a u8 script count and, for each script, a u16be length and that many u16be instructions.

| Type | Extra fields |
| --- | --- |
| 0 | u16be scroll, u8 hidden, u16be child count, then per child u16be id, i16be x, i16be y |
| 1 | u16be, u8 flag, then the type 4 text header below, then a u32be colour |
| 2 | four u8 flags (draggable, interactable, usable, swappable), u8 margin x, u8 margin y, 20 slots of u8 present and, when present is 1, i16be x, i16be y, jstr sprite, then five jstr inventory ops |
| 3 | u8 fill, then a u32be colour and three more u32be colours |
| 4 | u8 center, u8 font, u8 shadow, two jstr, one u32be colour, three more u32be colours |
| 5 | two jstr graphics |
| 6 | four pairs of u8. A non-zero first byte of a pair reads another u8 and stores `(first - 1) << 8 + second`, or -1 for the anim slots when the first byte is 0. Then u16be zoom, u16be x angle, u16be y angle. |
| 7 | u8 center, u8 font, u8 shadow, u32be colour, i16be margin x, i16be margin y, u8 interactable, five jstr |

If `buttonType` is 2 or type is 2, the record adds jstr verb, jstr text, and u16be target mask. If `buttonType` is 1, 4, 5, or 6, it adds a jstr option. An empty option becomes `Ok`, `Select`, `Select`, or `Continue` for those four button types.

## Scripts

`getIfVar` (`Client.java:11335`) walks `scripts[slot]`. A missing script returns -2. An exception returns -1. Opcode 0 returns the accumulator. The default operator is add. Opcodes 15, 16, and 17 set the next operator to subtract, divide, and multiply.

| Opcode | Value |
| --- | --- |
| 1 | `statEffectiveLevel[next]` |
| 2 | `statBaseLevel[next]` |
| 3 | `statXP[next]` |
| 4 | Sum of `linkObjCount` on the component id where `linkObjType` equals the obj id plus 1, skipping members objs on a free world |
| 5 | `varps[id]` |
| 6 | `levelExperience[statBaseLevel[id] - 1]` |
| 7 | `varps[id] * 100 / 46875`. The divisor is the character at `Client.java:11385`. |
| 8 | `localPlayer.combatLevel` |
| 9 | Sum of enabled `statBaseLevel`. `Stats.ENABLED` is in `Stats.java:7`. `Stats.COUNT` is 25. |
| 10 | 999999999 when the component contains obj id plus 1 |
| 11 | `runenergy` |
| 12 | `runweight` |
| 13 | 1 when `varps[id]` has bit `bit` set |
| 14 | Varp bitfield through `VarbitType` base, start, and end (`Client.java:11427`) |
| 18 | `(localPlayer.x >> 7) + sceneBaseTileX` |
| 19 | `(localPlayer.z >> 7) + sceneBaseTileZ` |
| 20 | The next instruction word, as a literal |

`isFriend` (`Client.java:4044`) checks every comparator. Comparator 2 fails when the value is greater than or equal to the operand. Comparator 3 fails when it is less than or equal. Comparator 4 fails when it is equal. Any other comparator, including 1, fails when it is not equal.

## Which interfaces open

No numeric main, side, or chat interface id is hardcoded. These fields start at -1 (`Client.java:749` and the login reset at `Client.java:4225`) and are replaced by packets:

| Packet | Field |
| --- | --- |
| 197 | `mainLayerId` |
| 187 | `sideLayerId` |
| 141 | `chatLayerId` |
| 249 | `mainLayerId` and `sideLayerId` |
| 85 | `mainOverlayLayerId` from an i16be. Negative skips the animation reset. |
| 239 | `tutLayerId` from an i16be |
| 174 | clears main, side, and chat |
| 91 | `tabInterfaceId[tab]`. A component id of 65535 is stored as -1. |

The sidebar array has 15 entries (`Client.java:733`). Click rectangles for tabs 0..13 are `handleTabInput` (`Client.java:5138`). The default selected tab is 3 (`Client.java:4228`).

Client codes used by the login and social UI, not as fixed interface ids: report abuse 600 (`Client.java:966`), design confirm 326, gender 324 and 325, colours 314..323, kits 300..313 (`Client.java:1940`), abuse reasons 601..612 (`Client.java:1977`). Welcome screen codes 650 and 655 are selected from opcode 146 (`Client.java:2983`). Logout, friend, and ignore actions are menu actions in `useMenuOption`, not fixed component ids.
