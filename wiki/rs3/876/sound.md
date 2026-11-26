# Sound

Server opcode 11, size 8 (`DataBlock1193.cls2789b62712` at `builds/rs3/876-2016-11-28/src/src/main/java/deob/DataBlock1193.java:28`), is read in `Statics.doStatics3` (`deob/Statics.java:5320`):

| Order | Width | Use |
| --- | --- | --- |
| 1 | u16be | `65535` becomes `-1` |
| 2 | u8 | |
| 3 | u16be | |
| 4 | u8 | |
| 5 | u16be | |

Those five values are passed to `cls5dfec39a.doDataBlock1350` with `DataBlock1490.cls8c81b66d10`. `2 + 1 + 2 + 1 + 2 = 8`.

Three more arms call the same object:

| Opcode | Size | Fields | Line |
| --- | --- | --- | --- |
| 127 | 2 | u16be into `doDataBlock135022` | `Statics.java:5362` |
| 31 | 3 | u8neg, u16be (`65535` becomes `-1`) into `doDataBlock13502222` | `Statics.java:6922` |
| 70 | 3 | u8, u16le+128 (`65535` becomes `-1`) into `doDataBlock1350222` | `Statics.java:6941` |

`VorbisRelated.doVorbisRelated` (`com/jagex/audio/vorbis/VorbisRelated.java:40`) reads a header from a `PositionedSound`: u8 index into `DataBlock1750.doDataBlock1307()`, u8 index into `Statics.doStatics()`, i16be, i16be, u32le (`doPositionedSound322222b3`), u32be, u8. The method that chooses this header instead of another audio class was not read. `com.jagex.audio` classes other than this one were not opened. That selection is the synth-versus-vorbis row in [gaps.md](gaps.md).
