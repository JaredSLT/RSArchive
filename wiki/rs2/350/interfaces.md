# Interfaces

Archive file `data` in the interface jag. `Component.call9dc50ca7` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/Component.java:350`) reads `u16be` count, allocates `Component[count]`, then reads records until the buffer ends.

A record starts with `u16be` id. If that id is 65535, the next `u16be` replaces the parent group and another `u16be` is the component id. `Component.unpack` (`Component.java:102`) then reads the component. The parent group is `valueComponent68` and the id is `valueComponent43`.

## Common header

1. `u8` type `valueComponent28`.
2. `u8` button type `valueComponent35`.
3. `u16be` `valueComponent81`.
4. `u16be` width `valueComponent66`.
5. `u16be` height `valueComponent52`.
6. `u8` `valueComponent22`.
7. `u8` parent link. If it is not 0, the link becomes `(link-1)<<8` plus another `u8`. Otherwise it is -1.
8. `u8` compare count, then that many pairs of `u8` opcode and `u16be` operand (`valueComponent74`, `valueComponent46`).
9. `u8` script count, then that many scripts. Each script is `u16be` length and that many `u16be` values.

`valueComponent81` of 600 stores the group as `valueComponent41`. 650 stores `valueComponent53`. 655 stores `valueComponent39` (`Component.java:120`). The report-abuse click sets `valueClient544` to `Component.valueComponent41` (`Client.java:2778`). No other numeric top-level interface id is hardcoded. Sidebar slots are `valueClient917`, fifteen entries initialized to -1 (`Client.java:735`) and filled by S2C opcode 49.

Opened groups, all server-driven:

| field | packet |
| --- | --- |
| `valueClient544` viewport | 14, and also 62 |
| `valueClient852` sidebar overlay | 62 and 71 |
| `valueClient915` chat | 118 |
| `valueClient703` and `valueClient554` | 15 |
| `valueClient945` | 144 |
| `valueClient829` | 60 |
| `valueClient917[tab]` | 49 |

## Type bodies

| type | fields after the header |
| --- | --- |
| 0 | `u16be` scroll height, `u8` hidden flag, `u16be` child count, then per child `u16be` id, `s16be` x, `s16be` y |
| 1 | `u16be`, `u8` flag, and the text block shared with type 4 |
| 2 | five `u8` flags, `u8`, `u8`, then 20 slots of `u8` present plus `s16be`, `s16be`, `jstr` sprite when present, then five `jstr` options |
| 3 | `u8` filled flag, then the color block shared with type 4 |
| 4 | center and font `u8`s shared with type 1, `jstr`, `jstr`, then `u32be` color and, with type 3, three more `u32be` |
| 5 | `jstr` graphic, `jstr` active graphic. A comma splits the sprite name from an index |
| 6 | four model refs, each `u8` then another `u8` when the first is not 0, then three `u16be` |
| 7 | inventory grid: center `u8`, font `u8`, shadow `u8`, `u32be`, `s16be`, `s16be`, `u8`, five `jstr` |
| 8 | `jstr` |

Types 1 and 4 share the center, font, and shadow bytes (`Component.java:217`). Types 1, 3, and 4 read `u32be` color (`Component.java:232`). Types 3 and 4 read three more `u32be` (`Component.java:236`).

Button types 2, and also type 2 components, read two `jstr` and one `u16be` (`Component.java:316`). Button types 1, 4, 5, and 6 read a `jstr`. An empty string becomes `Ok`, `Select`, `Select`, or `Continue` for those four button types (`Component.java:322`).

`Client.useMenuOption` treats script opcode 5 as a client varp toggle (`Client.java:5417` checks `scripts[0][0] == 5`). The rest of the script interpreter inside `drawInterface` and `handleInterfaceInput` was not read. See [gaps.md](gaps.md).

Thirteen side icons are loaded from the media jag as `sideicons` (`Client.java:13523`).
