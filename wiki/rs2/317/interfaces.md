# Interfaces

`Component.unpack` reads `data` from the interface jag. Sprites come from the media jag. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/config/Component.java:83`. The call is `Client.java:8256`.

The file starts with `u16be` count. Each component is `u16be` id. If that id is 65535, the next `u16be` is the parent and the `u16be` after that is the id. Then `u8` type, `u8` button type, `u16be` client code, `u16be` width, `u16be` height, `u8` alpha, `u8` overlayer. A nonzero overlayer is `(value - 1 << 8) +` another `u8`. Zero means -1.

`u8` comparator count, then that many `u8` comparator and `u16be` operand. `u8` script count, then that many `u16be` length and that many `u16be` script ints.

| Type | Extra fields |
| --- | --- |
| 0 layer | `u16be` scroll, `u8` hide, `u16be` child count, each child `u16be` id, `i16be` x, `i16be` y |
| 1 | `u16be`, `u8` flag, then the text block shared with type 4, then `i32be` colour |
| 2 inventory | four `u8` flags (draggable, interactable, usable, swappable), `u8` margin x, `u8` margin y, 20 slots of `u8` present then `i16be`, `i16be`, `jstr`, then five `jstr` options |
| 3 rectangle | `u8` fill, `i32be` colour, three more `i32be` colours |
| 4 text | `u8` center, `u8` font, `u8` shadowed, two `jstr`, `i32be` colour, three more `i32be` |
| 5 sprite | two `jstr` sprite names |
| 6 model | four optional `(u8, u8)` pairs for model, active model, anim, and active anim, then three `u16be` zoom, x angle, y angle |
| 7 inventory text | `u8` center, `u8` font, `u8` shadow, `i32be` colour, `i16be` margin x, `i16be` margin y, `u8` interactable, five `jstr` |

If `buttonType` is 2 or type is 2: two `jstr` and `u16be` target mask. If `buttonType` is 1, 4, 5, or 6: `jstr` option, defaulting to Ok, Select, Select, or Continue.

`Component.getModel` model types are 1 model, 2 npc head, 3 local player head, 4 object, 5 null. `Component.java:383`.

## Scripts

`Client.executeClientscript1` at `Client.java:10548` walks an int array. Opcode 0 returns the accumulator. The next opcodes load a value, and 15, 16, and 17 set the operator to subtract, divide, or multiply for the following value.

| Opcode | Value |
| --- | --- |
| 1 | current skill level |
| 2 | base skill level |
| 3 | experience |
| 4 | count of an object in an inventory. The stored id is compared with `obj + 1` |
| 5 | varp |
| 6 | experience for a base level |
| 7 | `varp * 100 / '뜛'`. That character is U+B71B, code point 46875 |
| 8 | local player combat level |
| 9 | sum of base levels whose `Stats.enabled` entry is true. `Stats.count` is 25. `Stats.java:6` |
| 10 | 999999999 when an inventory contains the object |
| 11 | energy |
| 12 | weight |
| 13 | one varp bit |
| 14 | varbit from `basevar`, `startbit`, and `endbit` |
| 18 | player tile x plus `sceneBaseTileX` |
| 19 | player tile z plus `sceneBaseTileZ` |
| 20 | the next script int, as a constant |

Comparators are a separate array and are not this interpreter.

## What the server opens

Sidebar slots are `tabInterfaceId[0..13]`, filled by packet 71. The constructor fills them with -1 and login sets the selected tab to 3. `Client.java:806`, `Client.java:7098`. There is no hardcoded sidebar interface id.

| Packet | What it opens |
| --- | --- |
| 97 | viewport interface, `u16be` |
| 248 | viewport `u16be_add` and sidebar `u16be` |
| 142 | sidebar interface, `u16le` |
| 164 | chat interface, `u16le` |
| 208 | viewport overlay, `i16le` |
| 218 | sticky chat, `i16le_add` |
| 219 | closes sidebar, chat, and viewport |

Client code 600 selects the report-abuse interface. Codes 650 and 655 select the last-login interface from packet 176. Code 206 is the bank arrange mode used by opcode 214. Design buttons use client codes 324, 325, 326, and 601..613. Those codes are searched on the loaded components. They are not constant interface ids.
