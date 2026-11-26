# Interfaces

Archive 3, file `data`, decoded by `Component.unpack` (`builds/rs2/282-2004-12-21/src/src/main/java/deob/Component.java:73`). Sprites come from the media jag. Widths are the `Packet` widths in `framing.md`.

Header: `u16be` component array length. Then, while bytes remain:

- `u16be` id. If it is 65535, the next `u16be` is the parent and the following `u16be` is the real id.
- `u8` type, `u8` button type, `u16be` content id, `u16be` width, `u16be` height, `u8` transparency, `u8` linked value. If that last byte is non-zero, one more `u8` is read and the value is `((first - 1) << 8) + second`; otherwise it is -1.
- `u8` condition count, then that many (`u8` operator, `u16be` value).
- `u8` script count, then that many (`u16be` argc, then argc `u16be` arguments).

Type tails:

| type | fields |
| --- | --- |
| 0 layer | `u16be` scroll height, `u8` hidden, `u16be` child count, then per child `u16be` id, `i16be` x, `i16be` y |
| 1 | `u16be`, `u8` flag, then the text block shared with type 4, then `u32be` colour |
| 2 inventory | `u8` flags for drag, usable, swappable, delete, `u8` x margin, `u8` y margin, then 20 optional sprites (`u8` 1 means `i16be` x, `i16be` y, `jstr` sprite name), then 5 `jstr` options |
| 3 rectangle | `u8` filled, then colour block shared with type 4 |
| 4 text | centred flag, `u8` font, shadow flag, `jstr`, `jstr`, `u32be` colour, and the hover colours shared with type 3 |
| 5 sprite | two `jstr` sprite names |
| 6 model | four optional ids packed as `u8` then `u8` when the first byte is non-zero, two sequence ids in that same form (0 becomes -1), `u16be` zoom, `u16be` x angle, `u16be` y angle |
| 7 inventory grid | centred flag, `u8` font, shadow flag, `u32be` colour, `i16be` x margin, `i16be` y margin, `u8` flag, 5 `jstr` |

Types 1, 3, and 4 share `u32be` colour. Types 3 and 4 then read three more `u32be` colours. Button type 2, or component type 2, appends two `jstr` and a `u16be`. Button types 1, 4, 5, and 6 append a `jstr`, defaulting to `Ok`, `Select`, `Select`, and `Continue` when empty.

The client does not hardcode a root interface id. Servers send the ids. `valueClient179` is 15 sidebar slots, each starting at -1 (`Client.java:663`). The selected tab `valueClient275` starts at 3 (`Client.java:10453`). Opcode 215 writes `valueClient179[tab]`. Opcode 16 and the second `u16be` of opcode 158 write the sidebar override `valueClient432`. Opcode 211 and the first `u16be` of opcode 158 write the modal `valueClient450`. Opcode 166 writes the chat interface `valueClient434`. Opcode 130 writes `valueClient128`. Opcode 240 writes `valueClient236`.

`drawSidebar` draws `valueClient432` when it is not -1, otherwise `valueClient179[valueClient275]` (`Client.java:3884`). `drawChat` draws `valueClient434`, otherwise `valueClient128`, otherwise the built-in chat log (`Client.java:10162`). `draw3DEntityElements` draws `valueClient236` (`Client.java:9106`).

Welcome packet 91 searches components whose content id is 650, or 655 when the third `u8` is not 201 or the fifth `u8` is 1, and opens that component's parent (`Client.java:7273`). Report action 524 opens the parent of content id 600 (`Client.java:11452`).

Scripts are `int[][]`, not a named CS2 VM. `Client.opClient20` (`Client.java:9987`) evaluates one script. Opcode 0 returns the accumulator. 1, 2, and 3 read a skill index and push current level, base level, or experience. 4 sums an inventory count of an item. 5 reads a varp. 6 reads the experience table for a level. 7 reads a varp as a percentage of 46875. 8 reads the local combat level. 9 sums enabled skills. 10 tests whether an inventory contains an item. 11 reads `valueClient216`. 12 reads run energy. 13 tests a varp bit. 14 reads a varbit. 15, 16, and 17 set the next operator to subtract, divide, or multiply. 18 and 19 read the local world x and z. 20 pushes an immediate. The operator is applied after each value opcode.

`Client.opClient24` (`Client.java:1115`) fails a component when any comparison fails. Operator 2 fails when the script result is greater than or equal to the value. Operator 3 fails when it is less than or equal. Operator 4 fails when it is equal. Any other operator fails when it is not equal.

`Client.drawInterface` (`Client.java:12346`) recurses for type 0 and starts an inventory grid for type 2. The draw bodies for types 1, 3, 4, 5, 6, and 7, and the rest of type 2, were not line-read.
