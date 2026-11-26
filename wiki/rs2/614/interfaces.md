# Interfaces

A component is `C1f967ea4`. `opkind10344` decodes one component from an `Unused6` (builds/rs2/614-2010-07-13/src/src/main/java/deob/C1f967ea4.java:295).

Common header, in order:

| order | width | meaning |
| --- | --- | --- |
| 1 | u8 | 255 is stored as -1. This is not the type |
| 2 | u8 | type in the low 7 bits. Bit 128 is cleared and a jstr follows |
| 3 | u16be | |
| 4 | s16be | `opUnused6142(4)` |
| 5 | s16be | |
| 6 | u16be | |
| 7 | u16be | |
| 8 | s8 | four times |
| 9 | u16be | parent. 65535 means -1. Otherwise combined with `valuekind1034130` |
| 10 | u8 | bit 0 and bit 1 are flags. When the first byte was not -1 and bit 1 is set, another flag is stored |

Type branches that follow, compared on the type byte:

| type | fields read in that branch |
| --- | --- |
| 0 | u16be, u16be, and if the first header byte was -1 another u8 flag |
| 5 | u32be, u16be, u8, u8, u8, u32be, u8, u8, u32be |
| 6 | u16be (65535 = -1), s16be, s16be, u16be, u16be, u16be, u16be, u16be (65535 = -1), u8, s16be, s16be, u8, and two optional u16be when two earlier signed bytes are nonzero |
| 4 | u16be (65535 = -1), jstr, four u8, u32be, u8, and one more u8 when the first header byte is at least 0 |
| 3 | u32be, u8, u8 |
| 9 | u8, u32be, u8 |

Other type values have the common header only. After the type branch the client reads `u24be`, `u8`, and while that u8 is nonzero an inventory triple (`u8` index nibble, `u8`, `s8`, `s8`). Then a jstr, a u8 whose low 4 bits are a string count and whose high 4 bits gate two optional u16be slots, another jstr, four u8, and a jstr. When a helper says the u24be is an action container, two u16be values follow (65535 = -1) and another u16be. A negative first header byte also reads one u16be. A non-negative first header byte then reads a count of `u24be + u32be` pairs and a count of `u24be + gjstr2` pairs.

The rest of the method is a series of calls to `opkind103417` and `opkind10346` (builds/rs2/614-2010-07-13/src/src/main/java/deob/C1f967ea4.java:520). Those two methods were not read. They are the client-script hooks on the component. The script VM (`ClientScript`, `ClientScriptCommand`) was not read.

The game root interface is not a client constant. S2C opcode 82, size 5, reads `u16le`, `u16be(lo-128)`, `u8`. The u16be(lo-128) is stored as `IntTreeMap.valueIntTreeMap3`. Mode 2 calls `Font.opFont6` (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1615). The login group name is `loginscreen` and the lobby group name is `lobbyscreen`, both on JS5 index 3 (builds/rs2/614-2010-07-13/src/src/main/java/deob/IDKTypeList.java:515). A numeric sidebar id was not found in the files read for this page.
