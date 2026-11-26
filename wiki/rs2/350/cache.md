# Cache

Cache kind is ondemand plus startup jagfiles. `Client.load` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:13342`) loads:

| CRC index | name | progress label |
| --- | --- | --- |
| 1 | title | title screen |
| 2 | config | config |
| 3 | interface | interface |
| 4 | media | 2d graphics |
| 5 | versionlist | update list |
| 6 | textures | textures |
| 7 | wordenc | chat system |
| 8 | sounds | sound effects |

`loadArchive` checks a local file CRC and otherwise opens `name + crc` over HTTP (`Client.java:9239`). The HTTP body starts with 6 bytes. The reader sets the cursor to 3 and reads `u24be` plus 6 as the total length (`Client.java:9274`).

## CRC blob

`Client.loadArchiveChecksums` (`Client.java:11861`) opens `crc` + random + `-350` and reads 40 bytes. Nine `u32be` values fill `valueClient914[0]` through `[8]`. A tenth `u32be` is the checksum. The client starts at 1234 and, for each of the nine CRCs, sets `checksum = (checksum<<1) + crc`. A mismatch retries.

## Ondemand

`OnDemand.send` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/OnDemand.java:525`) connects with `openSocket(43594 + valueClient685)`, writes `u8` 15, and discards 8 bytes. Each request is 4 bytes: archive `u8`, file `u16be`, priority `u8` (2 when urgent, 1 when not logged in, otherwise 0).

`OnDemand.read` (`OnDemand.java:584`) waits for 6 header bytes: archive `u8`, file `u16be`, length `u16be`, chunk `u8`. The body arrives in 500-byte pieces, shorter on the last piece. `OnDemand.poll` gunzips the finished body with `GZIPInputStream` (`OnDemand.java:715`). A length of 0 is a reject.

While logged in, after 500 idle loops the socket writes four bytes `0, 0, 0, 10` (`OnDemand.java:437`).

Versionlist jag, `OnDemand.load` (`OnDemand.java:255`):

| file | layout |
| --- | --- |
| model_version, anim_version, midi_version, map_version | `u16be` per file. Archive order is 0 models, 1 anims, 2 midi, 3 maps |
| model_crc, anim_crc, midi_crc, map_crc | `u32be` per file |
| model_index | one raw byte per model, 0 when the file is shorter |
| map_index | `length/7` rows of `u16be` key, `u16be` land id, `u16be` loc id, `u8` |
| anim_index | `u16be` per entry (`length/2`) |
| midi_index | one `u8` per midi |

## Config opcodes

All decoders read `u8` opcodes until 0. Files are inside the config jag. Indexed types use `u16be` count, then that many `u16be` sizes, and the data cursor starts at 2 (`LocType.java:59`, `NpcType.java:61`, `ObjType.java:225`).

Unknown opcodes in seq, spotanim, flo, idk, varp, and varbit print an error and do not consume more bytes. Unknown opcodes in npc, loc, and obj are skipped with no extra read and no error print.

### seq.dat

`SeqType.decodeClient` (`SeqType.java:74`).

| opcode | fields |
| --- | --- |
| 1 | `u8` count, then count times three `u16be`. The second `u16be` of 65535 is stored as -1 |
| 2 | `u16be` |
| 3 | `u8` count, then that many `u8`, then a trailing slot set to 9999999 |
| 4 | flag |
| 5 | `u8` |
| 6 | `u16be` |
| 7 | `u16be` |
| 8 | `u8` |
| 9 | `u8` |
| 10 | `u8` |
| 11 | `u8` |
| 12 | `u32be` |

### spotanim.dat

`SpotAnimType.decodeClient` (`SpotAnimType.java:50`).

| opcode | fields |
| --- | --- |
| 1 | `u16be` model |
| 2 | `u16be` sequence |
| 4 | `u16be` |
| 5 | `u16be` |
| 6 | `u16be` |
| 7 | `u8` |
| 8 | `u8` |
| 40..49 | `u16be` recolor source |
| 50..59 | `u16be` recolor destination |

### flo.dat

`FloType.opkind1072` (`FloType.java:46`). There is no opcode 4.

| opcode | fields |
| --- | --- |
| 1 | `u24be` |
| 2 | `u8` |
| 3 | flag |
| 5 | flag |
| 6 | `jstr` |
| 7 | `u24be` |

### idk.dat

`IdkType.opkind892` (`IdkType.java:45`).

| opcode | fields |
| --- | --- |
| 1 | `u8` part |
| 2 | `u8` count, then count `u16be` models |
| 3 | flag |
| 40..49 | `u16be` |
| 50..59 | `u16be` |
| 60..69 | `u16be` heads |

### varbit.dat

`VarbitType.decodeClient` (`VarbitType.java:50`).

| opcode | fields |
| --- | --- |
| 1 | `u16be` varp, `u8` lsb, `u8` msb |
| 2 | flag |
| 3 | `u32be` |
| 4 | `u32be` |
| 5 | flag |
| 10 | `jstr` |

### varp.dat

`VarpType.decodeClient` (`VarpType.java:58`).

| opcode | fields |
| --- | --- |
| 1 | `u8` |
| 2 | `u8` |
| 3 | flag, and the varp id is appended to the client-varp list |
| 4 | flag |
| 5 | `u16be` |
| 6 | flag |
| 7 | `u32be` |
| 8 | type stored as 1 |
| 10 | `jstr` |
| 11 | flag |
| 12 | `u32be` |
| 13 | type stored as 2 |
| 14 | flag |

### npc.dat

`NpcType.opkind1121` (`NpcType.java:100`).

| opcode | fields |
| --- | --- |
| 1 | `u8` n, n times `u16be` models |
| 2 | `jstr` name |
| 3 | bytes until 10 |
| 12 | `s8` size |
| 13 | `u16be` |
| 14 | `u16be` |
| 17 | four `u16be` |
| 30..39 | `jstr`. The text `hidden` is stored as null |
| 40 | `u8` n, n times two `u16be` |
| 60 | `u8` n, n times `u16be` |
| 90, 91, 92 | `u16be` |
| 93 | flag |
| 95 | `u16be` |
| 97, 98 | `u16be` |
| 99 | flag |
| 100 | `s8` |
| 101 | `s8` multiplied by 5 |
| 102 | `u16be` |
| 103 | `u16be` |
| 106 | `u16be` (65535 becomes -1), `u16be` (65535 becomes -1), `u8` n, then n+1 times `u16be` (65535 becomes -1) |
| 107 | flag |

### loc.dat

`LocType.opkind842` (`LocType.java:369`).

| opcode | fields |
| --- | --- |
| 1 | `u8` n, then n times (`u16be` model, `u8` type). If models are already loaded and `valuekind8450` is clear, the client skips `n*3` bytes |
| 2 | `jstr` |
| 3 | bytes until 10 |
| 5 | `u8` n, then n times `u16be`. The same skip is `n*2` bytes when models are already loaded |
| 14 | `u8` width |
| 15 | `u8` length |
| 17, 18 | flags |
| 19 | `u8`. A value of 1 sets the interactive flag |
| 21, 22, 23 | flags |
| 24 | `u16be`, 65535 becomes -1 |
| 28 | `u8` |
| 29 | `s8` |
| 39 | `s8` |
| 30..38 | `jstr`. `hidden` becomes null. The array length is 5 |
| 40 | `u8` n, n times two `u16be` |
| 60 | `u16be` |
| 62 | flag |
| 64 | flag |
| 65, 66, 67, 68 | `u16be` |
| 69 | `u8` |
| 70, 71, 72 | `s16be` |
| 73, 74 | flags |
| 75 | `u8` |
| 77 | `u16be` (65535 becomes -1), `u16be` (65535 becomes -1), `u8` n, then n+1 times `u16be` (65535 becomes -1) |

### obj.dat

`ObjType.opObjType7` (`ObjType.java:586`).

| opcode | fields |
| --- | --- |
| 1 | `u16be` model |
| 2 | `jstr` name |
| 3 | bytes until 10 |
| 4, 5, 6 | `u16be` |
| 7, 8 | `u16be`. Values above 32767 subtract 65536 |
| 10 | `u16be` |
| 11 | flag |
| 12 | `u32be` |
| 16 | flag |
| 23 | `u16be`, `s8` |
| 24 | `u16be` |
| 25 | `u16be`, `s8` |
| 26 | `u16be` |
| 30..34 | `jstr`. `hidden` becomes null |
| 35..39 | `jstr` |
| 40 | `u8` n, n times two `u16be` |
| 78, 79, 90, 91, 92, 93, 95, 97, 98 | `u16be` |
| 100..109 | `u16be`, `u16be` |
| 110, 111, 112 | `u16be` |
| 113 | `s8` |
| 114 | `s8` multiplied by 5 |
| 115 | `u8` |

## Interfaces

The interface jag file `data` is decoded by `Component.call9dc50ca7` (`Component.java:350`). See [interfaces.md](interfaces.md).
