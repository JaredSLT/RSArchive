# Interfaces

Components are `C1c6243ba`. The archive is id 3, `Statics.valueStatics991`, passed to `opkind32414` with the model archive id 7, the sprite archive id 8, and the font archive id 13 (`Loading.java:351`, `C1c6243ba.java:221-223`). Lookup `opkind324131` uses the high 16 bits as the group and the low 16 bits as the child (`C1c6243ba.java:229-238`).

The root id is not a source constant. Graphics defaults opcode 6 stores it on `valuekind359518` (`GraphicsDefaults.java:72-73`). `Client.java:1571` copies that field to `valueClient1957` and `ScriptRunner.opkind3634896` opens it (`Client.java:1574`). An earlier loading stage uses `valuekind359511` the same way (`Client.java:1524`). No separate sidebar id was read. `valuekind35952` is a model id from opcode 2, not an interface id (`GraphicsDefaults.java:62-63`).

`ScriptRunner` was not read. The component file stores script hooks. It does not execute them.

## Decode

`opkind324122` (`C1c6243ba.java:309-570`).

Common header:

| Order | Width |
| --- | --- |
| 1 | u8 version. 255 becomes -1 |
| 2 | u8 type. If bit 7 is set, the type keeps the low 7 bits and a jstr follows |
| 3 | u16be, i16be, i16be, u16be, u16be, i8, i8, i8, i8 |
| 4 | u16be parent. 65535 becomes -1. Otherwise the value is added to `valuekind3241128 & 0xFFFF0000` |
| 5 | u8. Bit 0 is stored. If the version is at least 0, bit 1 is stored |

Type 0 then reads u16be, u16be, and, when the version is below 0, a u8 `== 1` (`C1c6243ba.java:343-348`).

Type 5 reads u32be, u16be, a u8 whose bits 0 and 1 are flags, u8, u8, u32be, u8 `== 1`, u8 `== 1`, u32be, and, when the version is at least 3, another u8 `== 1` (`C1c6243ba.java:351-365`).

Type 6 reads a bigsmart, a u8 of flags, then either two i16be and three u16be, or three i16be, two u16be, and an i16be. Then a bigsmart. A non-zero earlier i8 reads a u16be, and the other i8 does the same (`C1c6243ba.java:368-400`).

Type 4 reads a bigsmart, an optional u8 `== 1` when the version is at least 2, a jstr, three u8, a u8 `== 1`, a u32be, a u8, and an optional u8 when the version is at least 0 (`C1c6243ba.java:403-418`).

Type 3 reads a u32be, a u8 `== 1`, and a u8 (`C1c6243ba.java:421-424`).

Type 9 reads a u8, a u32be, and a u8 `== 1` (`C1c6243ba.java:427-430`).

No other type reads a body. The type field is `valuekind324126`.

After the body: u24be, then a u8 loop. The loop key's high nibble minus 1 is an index and the low 12 bits of `(key << 8 | next u8)` are a value, with 4095 meaning -1, followed by two i8 (`C1c6243ba.java:433-457`). Then a jstr (`C1c6243ba.java:460`). A u8 splits into a low nibble count of jstrs and a high nibble. The high nibble reads one or two `(u8 index, u16be)` pairs into an int array (`C1c6243ba.java:461-486`). Then a jstr, three u8, and a jstr (`C1c6243ba.java:488-496`).

If `Ca788d8cd.opkind39421` of the u24be is not 0, three u16be follow, and 65535 becomes -1 (`C1c6243ba.java:498-512`). That predicate was not opened. When the version is at least 0, another u16be follows, then a u8 count of `(u24be, u32be)` pairs and a u8 count of `(u24be, vjstr)` pairs (`C1c6243ba.java:515-538`).

Script hooks are `opkind32419`: a u8 count, then that many u8 tags. Tag 0 is a u32be boxed as an integer. Tag 1 is a jstr. Count 0 is null (`C1c6243ba.java:572-589`). The decode calls that helper 18 times, plus one more when the version is at least 0 (`C1c6243ba.java:541-563`). Five int arrays follow from `opkind324110`: a u8 count and that many u32be, or null when the count is 0 (`C1c6243ba.java:565-569`, `593-605`).

Drawing calls `opClient562` and `opClient526` with `valueClient1957` (`Client.java:10511`, `Client.java:11343`). Those methods were not read.
