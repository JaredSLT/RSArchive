# Interfaces

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`.

The interface jag is index 3. `Component.call9dc50ca7` reads file `"data"` (`jagex2/config/Component.java:384`). The file starts with `u16be` count. Each component is `u16be` id. Id 65535 means the next `u16be` is the parent group and the `u16be` after that is the id (`Component.java:390`). `unpack` then reads the body (`Component.java:130`).

## Common header

`u8` type, `u8` button type, `u16be` content type, `u16be` width, `u16be` height, `u8` transparency, `u8` parent link. A nonzero link is `(n-1<<8)` plus another `u8`. A zero link is -1.

Content type 600 records `valueComponent124`, 650 records `valueComponent133`, and 655 records `valueComponent154` (`Component.java:148`). Report-abuse uses content type 600. The client does not hardcode the chat, sidebar, or viewport interface ids. Those fields are filled by packets:

| field | set by |
| --- | --- |
| `valueClient1120` | S2C 109 `u16be` |
| `valueClient1217` | S2C 246 `g2_alt3`, and the second id of S2C 128 |
| `valueClient1294` | S2C 159 `g2_alt3`, and the first id of S2C 128 |
| `valueClient1092`, `valueClient1182` | S2C 253 `g2_alt1`, `g2_alt2` |
| `valueClient1210[tab]` | S2C 10 `g1_alt3` tab, `g2_alt2` interface, 65535 means -1 |
| `valueClient1316` | S2C 158 `g2b_alt1` |

## Type bodies

Scripts follow the header for every type: `u8` compare count, each `u8` and `u16be`; then `u8` script count, each `u16be` length and that many `u16be` values (`Component.java:160`).

| type | extra fields | citation |
| --- | --- | --- |
| 0 layer | `u16be` scroll, `u8` hide, `u16be` child count, each child `u16be` id, `i16be` x, `i16be` y | `Component.java:185` |
| 1 | `u16be`, `u8` flag; also the text-font block shared with type 4 | `Component.java:200` |
| 2 inventory grid | four `u8` flags, `u8` margin, `u8` margin, 20 optional sprites (`u8` 1 then `i16be`, `i16be`, `jstr`), five `jstr` ops | `Component.java:205` |
| 3 rectangle | `u8` filled; then four `u32be` colours shared with type 4 | `Component.java:241` |
| 4 text | `u8` center, `u8` font, `u8` shadowed, `jstr`, `jstr`, four `u32be` colours | `Component.java:245` |
| 5 sprite | two `jstr` sprite names | `Component.java:270` |
| 6 model | two optional model ids, two optional seq ids, three `u16be` zoom and rotations | `Component.java:284` |
| 7 grid | `u8` center, `u8` font, `u8` shadowed, `u32be` colour, `i16be`, `i16be`, `u8` flag, five `jstr` | `Component.java:316` |
| 8 | `jstr` | `Component.java:340` |

Button type 2, or component type 2, adds two `jstr` and one `u16be` (`Component.java:344`). Button types 1, 4, 5, and 6 add a `jstr`, defaulting to Ok, Select, Select, or Continue (`Component.java:350`).

`drawInterface` (`Client.java:12967`) was not read. Per-type painting is in [gaps.md](gaps.md).

## Scripts

`opClient51` (`Client.java:12093`) returns the accumulator on opcode 0. An exception returns -1. A missing script returns -2. Opcodes 15, 16, and 17 set the next operator to subtract, divide, or multiply. The default operator is add.

| opcode | value |
| --- | --- |
| 1 | `valueClient1159[index]`, the skill byte from S2C 49 |
| 2 | `valueClient1183[index]`, level derived from the xp table |
| 3 | `valueClient975[index]`, xp from S2C 49 |
| 4 | inventory count of obj+1 on a component |
| 5 | `valueClient1169[index]` varp |
| 6 | `valueClient1084[level-1]` |
| 7 | varp times 100 divided by the char literal U+B71B (`Client.java:12143`) |
| 8 | local combat level |
| 9 | sum of enabled skills. `Stats` has 25 names; the last four are disabled (`jagex2/client/Stats.java:6`) |
| 10 | 999999999 when the inventory contains the item |
| 11 | `valueClient1443` from S2C 125 |
| 12 | `valueClient1160` from S2C 174 |
| 13 | varp bit test |
| 14 | varbit extract |
| 18 | local fine x `>> 7` plus scene base x |
| 19 | local fine z `>> 7` plus scene base z |
| 20 | immediate script operand |

## Actions that send packets

`handleInterfaceAction` (`Client.java:5324`): 201 and 202 open friend add and delete prompts, 501 and 502 open ignore prompts, 300..313 cycle identity kits, 314..323 cycle colours from `valueClient1140`, 324 and 325 set gender and call `opClient132`, 326 sends appearance opcode 163, 620 toggles the report mute, and 601..613 send opcode 184.

Appearance opcode 163 is `u8` gender (0 male, 1 female), seven `u8` kit ids, five `u8` colours (`Client.java:5444`). Report opcode 184 is `u64be` name, `u8` reason minus 601, `u8` mute (`Client.java:5435`).
