# Cache

Kind js5. `meta.json` records engine `js5` and `cache_downloaded: true`. The cache directory for this revision is present, so this page is not blocked on `no-cache`.

`Client.opClient228` opens a reference store constructed with index 255 and 37 data index files in a loop `0 .. 36` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:1276).

## Archive ids

`Lobbyscreen` opens archives with `Ce0d86851.opkind16131(xtea, id, -23, ...)` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:58 and builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:129).

| Index | Field | XTEA argument | Notes |
| --- | --- | --- | --- |
| 0 | `C4151cdd1.valuekind17721` | false | |
| 1 | `Cb6ae220e.valuekind14781` | false | |
| 2 | `Cdbaf00b8.valuekind14862` | false | config archive passed to the type lists below |
| 3 | `C924bebd1.valuekind11315` | false | interfaces. Names `loginscreen` and `lobbyscreen` are looked up here (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:290) |
| 4 | `Caefad3d0.valuekind14285` | false | |
| 5 | `C7a61cd25.valuekind11891` | true | maps |
| 6 | `Ce8c79a4d.valuekind17641` | true | music. Group name `scape main` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:186) |
| 7 | `C0b9281af.valuekind13146` | false | models, passed into npc, loc, and obj lists |
| 8 | `Ccc85a833.valuekind15791` | false | sprites |
| 9 | `Ce0d86851.valuekind16134` | false | |
| 10 | `Cbcbd7a77.valuekind13272` | false | file name `huffman` builds `SoftwareFont` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:251) |
| 11 | `C10f1efbe.valuekind172619` | false | |
| 12 | `C748e389c.valuekind15513` | false | |
| 13 | `C67b514c1.valuekind142712` | false | opened earlier, with the native-library choice (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:61) |
| 14 | `Cd689a375.valuekind11253` | false | |
| 15 | `C40c2cda8.valuekind17506` | false | |
| 16 | `C9dc28953.valuekind16662` | false | |
| 17 | `C384b991b.valuekind13543` | false | |
| 18 | `C99b3e320.valuekind12122` | false | |
| 19 | `Ce9659f6b2.valuekind157015` | false | |
| 20 | `C0ec93d07.valuekind12941` | false | passed into the sound config list |
| 21 | `Cdc5a159d.valuekind119810` | false | |
| 22 | `Unused8.valueUnused863` | false | |
| 23 | `C023ef89a.valuekind176320` | true | |
| 24 | `Cc9b39119.valuekind15053` | false | |
| 25 | `GpuTextureRelated.valueGpuTextureRelated11` | false | |
| 26 | `C1dd03222.valuekind16705` | true | |
| 27 | `VideoOggTheoraRelated.valueVideoOggTheoraRelated15` | false | |
| 28 | `C66af5d01.valuekind17655` | true | |
| 29 | `C65ceb103.valuekind11851` | false | |
| 30 | `Ca8453007.valuekind16514` | true | |
| 31 | `C37939422.valuekind14308` | true | |
| 32 or 34 | `Cb85a2d97.valuekind17711` | false | 34 when `C79bd777c.opkind14351` is false, else 32 (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:59) |
| 33 | `C29d8de52.valuekind12821` | false | |
| 36 | `C42ca1bc2.valuekind15112` | true | last argument is 2, not 1 |

`C7c30cd9c.valuekind14495` is a 37-int progress weight table. It is not an archive-id table.

Login writes one `u32be` CRC per call in `C0a1835bb4.call58b83e6b` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C0a1835bb4.java:11).

## Config group ids that are literals

These lists are constructed at builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:215 and take archive 2 unless noted.

| List | Group | Line |
| --- | --- | --- |
| `C3f50ccfe` (passed to the obj list as `RcaInit.valueRcaInit15`) | 11 | builds/rs2/634-2010-12-10/src/src/main/java/deob/C3f50ccfe.java:12 |
| `GpuTextureRelated` (`GnpMis.valuekind14433`) | 15 | builds/rs2/634-2010-12-10/src/src/main/java/deob/GpuTextureRelated.java:863 |
| `Cb4abf122` objs | last group of its bindings (`fileCount - 1`) | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cb4abf122.java:153 |
| `Cb64e35a6` | last group of its bindings | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cb64e35a6.java:229 |
| `C2df99355` | last group of its bindings | builds/rs2/634-2010-12-10/src/src/main/java/deob/C2df99355.java:33 |

`Cbf3f03c1` and `C8e648c5c` are built from archives 24 and 25, not from archive 2 (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:243). Other lists at lines 215–241 take archive 2, and their group ids are inside constructors that were not opened. Those group ids are unread.

## Obj decode

`ObjType.java` has no opcode switch. The decode method is `C42c2fcc3.opkind136613` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C42c2fcc3.java:638). The caller must pass 4 as the first argument; that value makes the `u16be` gates equal 842397944. An opcode with no branch reads nothing. Implemented opcodes:

| Opcode | Payload |
| --- | --- |
| 1 | u16be |
| 2 | cstr |
| 4, 5, 6 | u16be |
| 7 | u16be, subtract 65536 when the value is at least 32768 |
| 8 | u16be, same signed adjust |
| 11 | no bytes; sets a field to 1 |
| 12 | u32be |
| 16 | no bytes; sets a boolean |
| 18, 23, 24, 25, 26 | u16be |
| 30..34 | cstr, stored at `opcode - 30` |
| 35..39 | cstr, stored at `opcode - 35` |
| 40 | u8 count, then count pairs of u16be |
| 41 | u8 count, then count pairs of u16be |
| 42 | u8 count, then count raw i8 |
| 65 | no bytes; sets a boolean |
| 78, 79, 90, 91, 92, 93, 95, 97, 98 | u16be |
| 96 | u8 |
| 100..109 | two u16be, stored at `opcode - 100` |
| 110, 111, 112 | u16be |
| 113 | i8 |
| 114 | i8, then multiplied by 5 |
| 115 | u8 |
| 121, 122 | u16be |
| 125, 126 | three i8, each shifted left by 2 |
| 127, 128, 129, 130 | u8, u16be |
| 132 | u8 count, then count u16be |
| 134 | u8 |
| 249 | u8 count. Each entry: u8 (1 means a string), u24be key, then cstr or u32be |

The method ends at builds/rs2/634-2010-12-10/src/src/main/java/deob/C42c2fcc3.java:840.
