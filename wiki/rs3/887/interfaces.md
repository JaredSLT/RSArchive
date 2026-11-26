# Interfaces

Components are `com.jagex.game.group.PlayerGroupDeltaEncodingKey`. The loader is `doPlayerGroupDeltaEncodingKey(js540, js511, js538, js524)` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/camera/CameraTrackableType.java:407`). `js540` is archive 3.

`Client.doClient2` sets `Client.tint87` from `Statics.gWCLocation.tint7` and opens that interface (`builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:2203`). `tint7` is opcode 6 of `GWCLocation`, a u24be (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/shared/framework/gwc/GWCLocation.java:148`), from archive 28. The number is not in the source. No sidebar id was found in the lines read.

## Component binary

`doPlayerGroupDeltaEncodingKey(PositionedSound)` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/group/PlayerGroupDeltaEncodingKey.java:547`).

| order | width | use |
| --- | --- | --- |
| 1 | u8 | version. 255 becomes -1 |
| 2 | u8 | type. If bit 128 is set, clear it and read jstr name |
| 3 | u16be | `tint23` |
| 4 | i16be | `tint24` |
| 5 | i16be | `tint25` |
| 6 | u16be | `tint26` |
| 7 | u16be | `tint27` |
| 8 | i8, i8, i8, i8 | `tbyte3`, `tbyte4`, `tbyte`, `tbyte2` |
| 9 | u16be | parent. 65535 is -1, else add `tint20 & 0xffff0000` |
| 10 | u8 | bit 0 hidden. If version >= 0, bit 1 is `tboolean3` |

Type-specific, only these types branch:

| type | fields |
| --- | --- |
| 0 | u16be, u16be. If version < 0, an extra u8 |
| 5 | u32be, u16be, u8 flags, u8, u8, u32be, u8, u8, u32be. If version >= 3, an extra u8 |
| 6 | forced model kind 1, smart `b4b`, u8 flags. Bit 1 reads two i16be and four u16be. Bit 2 of that flag reads three i16be, three u16be, and one i16be. Then smart `b4b`. Nonzero `tbyte3` reads u16be. Nonzero `tbyte4` reads u16be |
| 4 | smart `b4b`. If version >= 2, u8. Then jstr, three u8, u8, u32be, u8. If version >= 0, another u8 |
| 3 | u32be, u8, u8 |
| 9 | u8, u32be, u8 |

Common tail from `:671`:

| order | width |
| --- | --- |
| 1 | u24be events |
| 2 | u8, then while nonzero: high nibble selects a slot, low nibble plus the next u8 forms a 12-bit id (4095 is -1), then two i8 |
| 3 | jstr |
| 4 | u8. Low nibble is a string count, high nibble gates two more slots. Each option is a jstr. Each extra slot is u8 index plus u16be |
| 5 | jstr. Empty becomes null |
| 6 | three u8 |
| 7 | jstr |
| 8 | if `DataBlock557.doDataBlock6952(events) != 0`: three u16be, each 65535 becoming -1. The predicate was not read |
| 9 | if version >= 0: one more u16be, 65535 becoming -1 |
| 10 | if version >= 0: u8 count of (u24be, u32be), then u8 count of (u24be, versioned jstr) |

Script hooks are `doPlayerGroupDeltaEncodingKey2` (`:811`), called once per hook field from `:779`. u8 count 0 means null. Each entry is u8 0 and a u32be, or u8 1 and a jstr.

Int arrays are `doPlayerGroupDeltaEncodingKey22` (`:833`): u8 count 0 means null, else that many u32be. Five of those follow the hooks (`:803`).

No branch was present for types other than 0, 3, 4, 5, 6, and 9.

## Scripts

Server opcode 104, size -2, is the script runner (`Statics.java:6587`). It reads a jstr format. For each character from the end, `'s'` reads a jstr and any other character reads a u32be. A final u32be is the script id. `ColourUtils.doColourUtils` runs it (`:6603`). The command switch was not read.

Archive 12 (`js520`) supplies bytes to `DataBlock1037.doDataBlock720` (`DataBlock1037.java:27`). The bytecode opcode table inside that decoder was not read.
