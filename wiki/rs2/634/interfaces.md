# Interfaces

Interface archive is JS5 index 3, `C924bebd1.valuekind11315` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:133). Group names `loginscreen` and `lobbyscreen` are resolved with `opBindings62` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:290). The numeric group ids are the return values of that call. No decimal sidebar ids were read in the files opened for this page.

Root id `Cc9dc9486.valuekind12025` is set by:

- S2C opcode 102, size 3, `CacheKey.valueCacheKey7` (builds/rs2/634-2010-12-10/src/src/main/java/deob/CacheKey.java:6, builds/rs2/634-2010-12-10/src/src/main/java/deob/C4d106383.java:1754). Payload: `u8_neg` mode, `u16be_add128` interface id. Mode 2 calls `Cc5c399ef.opkind11793`. The id is stored, `C07252b48.opkind15561` runs, and `Unused3.call57c9b54a` runs.
- `C42efb241` assigns the `loginscreen` group id (builds/rs2/634-2010-12-10/src/src/main/java/deob/C42efb241.java:56).

Widget class is `C2a1fbff4`. Decode is `opkind139210` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:536).

## Header

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | u8 | 255 is stored as -1. This value is tested again as the version |
| 2 | u8 | bit 7 set: clear it and read a cstr name |
| 3 | u16be | |
| 4 | i16be | |
| 5 | i16be | |
| 6 | u16be | |
| 7 | u16be | |
| 8 | i8 ×4 | |
| 9 | u16be | parent. 65535 becomes -1. Otherwise `value \| (thisId & 0xFFFF0000)` (`-65536 & valuekind1392141`) |
| 10 | u8 | bit 0 and bit 1 are booleans. When the version byte is at least 0, bit 1 also forces a hidden flag |

## Type body

Type is `valuekind139292` after the bit-7 strip.

| Type | Payload | Line |
| --- | --- | --- |
| 0 | u16be, u16be. When the version byte is below 0, one more u8 (1 means hidden) | builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:570 |
| 5 | u32be, u16be, u8 (bit 0 and bit 1), u8, u8, u32be, u8 (1 means true), u8 (1 means true), u32be | builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:578 |
| 6 | The code sets a count field to 1, then u16be model (65535 becomes -1). u8 flags: bit 0, bit 1, bit 2, bit 3. Bit 0 clear and bit 1 clear: no further model-pose bytes. Bit 0 clear and bit 1 set: i16be, i16be, i16be, u16be, u16be, u16be, i16be. Bit 0 set: i16be, i16be, u16be, u16be, u16be, u16be. Then u16be animation (65535 becomes -1). If the header width byte is non-zero, another u16be. If the header height byte is non-zero, another u16be | builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:592 |
| 4 | u16be (65535 becomes -1), cstr, u8, u8, u8, u8 (1 means true), u32be, u8. When the version byte is at least 0, one more u8 | builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:637 |
| 3 | u32be, u8 (1 means true), u8 | builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:655 |
| 9 | u8, u32be, u8 (1 means true) | builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:661 |

Types other than 0, 3, 4, 5, 6, and 9 have no extra bytes before the shared tail.

## Shared tail

Starts at builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:667 with `u24be`, then a `u8`. While that byte is non-zero the method reads a nibble index, another `u8`, and a 12-bit value (4095 becomes -1). The loop was read through line 679. `opkind13929` and `opkind139218` continue later in the same method. `opkind13929` reads a `u8` count and then, per entry, a `u8` type (0 is `u32be`, otherwise cstr) (builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:509). `opkind139218` reads a `u8` count of `u32be` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C2a1fbff4.java:927). The order of those calls after line 679 was not transcribed.
