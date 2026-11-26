# Interfaces

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`.

Archive index 3 is `GlRenderList.valueGlRenderList28`, held as `C9eb3d527.valuekind9635`. `C90803bf8.opkind9771` binds fonts (index 13), models (index 7), that interface archive, and sprites (index 8). The binder body was not re-read line by line. The load path that was read is `C8d92094d.opkind9864` (`deob/C8d92094d.java:23`).

`opkind9864(interfaceId)` asks the archive for the file count. An empty archive is marked loaded. Each file is `opkind64023(fileId, interfaceId)`. Byte 0 must be -1. Any other value throws `IllegalStateException("if1")` (`C8d92094d.java:44`). This client accepts that format only. The widget id is `(interfaceId << 16) | componentIndex`.

There is no hardcoded root or sidebar id in the methods read. Server packet 93, size 5, reads u8, u16le, u16le (`deob/C268455a9.java:90`). When `Packet.opPacket59` accepts the second u16, a u8 of 2 calls `Cbc0ee57f.opkind8252`, then `C0922b514.valuekind59424` becomes the first u16le and `Ce7f0d2ee.opkind6575` resets that interface. `Packet.opPacket59` was not read.

`C15ab7d97.call8eeb32b9` (`deob/C15ab7d97.java:16`) draws that root when it is not -1, through `C5c7b4c41.opkind7261`, passing canvas width `C9dc28953.valuekind9251` and height `MapElement.valueMapElement85`. Per-type painting and the child-interface slot lookup were not read.

## Component decode

`Js5GlTextureProvider.opJs5GlTextureProvider12` (`deob/Js5GlTextureProvider.java:508`) skips one byte (the marker already checked), then:

- u8 type. If bit `0x80` is set, the type is masked with `0x7f` and a jstr name follows.
- u16be, i16be, i16be, u16be, u16be
- four i8
- u16be parent. 65535 means -1. Any other value is combined with the high 16 bits of this widget id.
- u8 hidden, stored as `== 1`

Type bodies that follow, before the shared tail:

| type | test in source | fields |
| --- | --- | --- |
| 0 | `type == 0` | u16be, u16be, u8 `== 1` |
| 5 | `~type == -6` | u32be, u16be, u8 (bit 1 and bit 0 kept separately), u8, u8, u32be, u8 `== 1`, u8 `== 1`, u32be |
| 6 | `~type == -7` | model kind forced to 1, u16be (65535 becomes -1), i16be, i16be, five u16be (the fifth, 65535 becomes -1), u8 `== 1`, u16be stored as a short, u16be stored as a short, u8 `== 1`. If the third of the four header i8 values is nonzero, another u16be. If the second header i8 is nonzero, another u16be. |
| 4 | `type == 4` | u16be font (65535 becomes -1), jstr, four u8 (the fourth is `== 1`), u32be |
| 3 | `~type == -4` | u32be, u8 `== 1`, u8 |
| 9 | `type == 9` | u8, u32be, u8 `== 1` |

Types 1, 2, 7, and 8 have no extra body in this method. The shared tail is `Js5GlTextureProvider.java:610`:

- u24be bitfield
- u8 values until 0. For each nonzero byte, the high nibble minus 1 selects a slot in a 10-long array, the low nibble is combined with the next u8, and the low 12 bits equal to 4095 store -1. Then two i8.
- jstr
- u8. The low nibble is an option-string count. The high nibble is an extra count. That many option jstr follow.
- If the high nibble is nonzero: u8 index, u16be into an int array. If the high nibble is greater than 1, another u8 index and u16be.
- jstr. An empty string becomes null.
- three u8
- jstr
- If `C30d052dc.opkind6992(bitfield)` is nonzero: three u16be, each 65535 becoming -1. `opkind6992` was not read.
- `new C7bdaa717(bitfield, lastId)`
- 20 script arrays from `call3fbdf91c` (`Js5GlTextureProvider.java:718`): u8 count, 0 means null; each entry is u8 0 then u32be, or u8 1 then jstr
- 5 int arrays from `opJs5GlTextureProvider13` (`Js5GlTextureProvider.java:739`): u8 count, 0 means null; otherwise that many u32be

Client-script opcodes and the click dispatcher were not read.
