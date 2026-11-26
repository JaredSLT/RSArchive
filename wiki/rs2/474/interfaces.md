# Interfaces

The widget class is `deob.NoReplyLoginserver`. Index 3 is the interface archive. `deob.World.opkind2385` stores that archive and allocates `NoReplyLoginserver[fileCount()][]` (`builds/rs2/474-2007-10-19/src/src/main/java/deob/World.java:93-98`). Components are fetched with `HashTableIterator.call071bd8ce`. The open root is `OchteSichMitIhnenDuellieren.valuekind2524`. The logged-in tick walks it with `Node.opNode5(0, root, 0, 0, 765, 0, 503)` (`rt4/InvType.java:319`).

Opcode 241 closes that root when it is not -1, via `EnumStringEntry.opEnumStringEntry8` (`rt4/JavaMouseWheel.java:204-207`). Opcode 251 sets the root from a u16le and rebuilds it (`rt4/JavaMouseWheel.java:885-889`). Opcode 128 does the same and then writes container and inventory data (`rt4/JavaMouseWheel.java:444-494`). Sidebar ids are not a constant table in the methods read here.

## IF3 decode

`NoReplyLoginserver.opkind24713` (`deob/NoReplyLoginserver.java:818-938`) sets `valuekind247117` true. The first u8 is discarded. The type is the next u8, `valuekind24799`.

Common header:

| Order | Width | Use |
| --- | --- | --- |
| 1 | u8 | discarded |
| 2 | u8 | type |
| 3 | u16be | |
| 4 | i16be | x |
| 5 | i16be | y |
| 6 | u16be | one dimension |
| 7 | u16be, or i16be when type is 9 | the other dimension |
| 8 | u16be | parent. 65535 stores -1. Any other value is combined with `fileId & 0xffff0000` |
| 9 | u8 | hidden when the byte is 1 |

Type bodies:

| Type | Fields |
| --- | --- |
| 0 | u16be, u16be |
| 3 | u32be, u8, u8 |
| 4 | u16be font (65535 becomes -1), jstr, u8, u8, u8, u8, u32be |
| 5 | u32be, u16be, u8 flag, u8, u8, u32be, u8 flag, u8 flag |
| 6 | model type forced to 1, u16be model (65535 becomes -1), i16be, i16be, five u16be with the last 65535 becoming -1, u8 flag |
| 9 | u8, u32be |

After the type body, every component reads u24be, jstr, u8 count plus that many jstr, three u8 values, jstr, then eighteen `opkind2479` arrays and three `opkind24711` arrays (`deob/NoReplyLoginserver.java:902-937`). Those two methods were not read.

`opkind2478` is a second decoder and was not read. Older component types may live there.

## Scripts

Server packet 237 builds an `Object[]` and calls `MidiNote.opMidiNote12` (`rt4/JavaMouseWheel.java:219-234`). The descriptor is a jstr. From the last character backward, `'s'` (115) reads a jstr and any other character reads a u32be. One more u32be is stored at index 0.

`TextureOpCurve.opTextureOpCurve18` is the client script interpreter (`deob/TextureOpCurve.java:113`). It was not read. It also writes the C2S opcodes listed in [packets.md](packets.md). The archive those scripts are loaded from was not identified beyond index 2 being stored by `ClientScript.opClientScript21`.
