# Cache

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`.

## Jagfile archives

`Client.load` (`jagex2/client/Client.java:5930`) loads CRCs into `valueClient970` and then these archives. The index is the on-disk jag index, and the CRC slot is `valueClient970[index]`.

| CRC slot | index | name | citation |
| --- | --- | --- | --- |
| 1 | 1 | title | `Client.java:6001` |
| 2 | 2 | config | `Client.java:6008` |
| 3 | 3 | interface | `Client.java:6009` |
| 4 | 4 | media | `Client.java:6010` |
| 5 | 5 | versionlist | `Client.java:6023` |
| 6 | 6 | textures | `Client.java:6011` |
| 7 | 7 | wordenc | `Client.java:6012` |
| 8 | 8 | sounds | `Client.java:6013` |

`Jagfile.unpack` reads `u24be` uncompressed size and `u24be` compressed size. When they differ, `BZip2.read` starts at offset 6 (`jagex2/io/Jagfile.java:28`). The entry table is `u16be` count, then per file `u32be` hash, `u24be` uncompressed, `u24be` compressed. `BZip2.read` itself is unread (`jagex2/io/BZip2.java:8`).

Config files from the config jag, decoded until opcode `u8` 0. Indexed types also read `name.idx` as `u16be` count and `u16be` sizes.

| file | decoder | opcodes |
| --- | --- | --- |
| `flo.dat` | `FloType.decodeClient` `jagex2/config/FloType.java:52` | 1 `u24be` rgb; 2 `u8` texture; 3 flag; 5 flag false; 6 `jstr`; 7 `u24be` |
| `idk.dat` | `IdkType.decodeClient` `jagex2/config/IdkType.java:48` | 1 `u8` part; 2 `u8` count then `u16be` models; 3 disable flag; 40..49 `u16be` src; 50..59 `u16be` dst; 60..69 `u16be` heads |
| `seq.dat` | `SeqType.decodeClient` `jagex2/config/SeqType.java:77` | 1 `u8` count then per frame `u16be`, `u16be` (65535 is -1), `u16be` delay; 2 `u16be`; 3 `u8` count, `u8` values, then sentinel 9999999; 4 flag; 5 `u8`; 6 `u16be`; 7 `u16be`; 8 `u8`; 9 `u8`; 10 `u8`; 11 `u8`; 12 `u32be` |
| `spotanim.dat` | `SpotAnimType.decodeClient` `jagex2/config/SpotAnimType.java:56` | 1 `u16be` model; 2 `u16be` seq; 4 `u16be`; 5 `u16be`; 6 `u16be`; 7 `u8`; 8 `u8`; 40..49 `u16be`; 50..59 `u16be` |
| `varp.dat` | `VarpType.decodeClient` `jagex2/config/VarpType.java:60` | 1 `u8`; 2 `u8`; 3 client var; 4 flag; 5 `u16be`; 6 flag; 7 `u32be`; 8 special; 10 `jstr`; 11 flag; 12 `u32be`; 13 special; 14 flag |
| `varbit.dat` | `VarbitType.decodeClient` `jagex2/config/VarbitType.java:53` | 1 `u16be` varp, `u8` low bit, `u8` high bit; 2 flag; 3 `u32be`; 4 `u32be`; 5 flag; 10 `jstr` |
| `loc.dat` + `loc.idx` | `LocType.decodeClient` `jagex2/config/LocType.java:358` | 1 `u8` count, then `u16be` model and `u8` type; 2 `jstr` name; 3 `gjstrraw` desc; 5 `u8` count then `u16be` models; 14 `u8` width; 15 `u8` length; 17 solid off; 18 flag; 19 `u8`; 21 flag; 22 flag; 23 flag; 24 `u16be` seq, 65535 is -1; 28 `u8`; 29 `i8`; 39 `i8`; 30..38 `jstr` ops; 40 `u8` count then pairs of `u16be`; 60 `u16be`; 62 flag; 64 flag; 65..68 `u16be`; 69 `u8`; 70..72 `i16be`; 73 flag; 74 flag; 75 `u8`; 77 `u16be` varbit, `u16be` varp, `u8` count, `u16be` variants (65535 is -1) |
| `npc.dat` + `npc.idx` | `NpcType.decodeClient` `jagex2/config/NpcType.java:108` | 1 models; 2 `jstr`; 3 `gjstrraw`; 12 `i8` size; 13 `u16be` stand; 14 `u16be` walk; 17 four `u16be` walks; 30..39 `jstr` ops; 40 recolour; 60 head models; 90..92 `u16be`; 93 flag; 95 `u16be`; 97 `u16be`; 98 `u16be`; 99 flag; 100 `i8`; 101 `i8` times 5; 102 `u16be`; 103 `u16be`; 106 variants; 107 clickable off |
| `obj.dat` + `obj.idx` | `ObjType.decodeClient` `jagex2/config/ObjType.java:467` | 1 `u16be` model; 2 `jstr`; 3 `gjstrraw`; 4 `u16be` zoom; 5 `u16be` xan; 6 `u16be` yan; 7 `u16be` zan, then signed adjust; 8 `u16be` xof, signed adjust; 10 `u16be`; 11 stackable; 12 `u32be` cost; 16 members; 23 `u16be` male model, `i8` offset; 24 `u16be`; 25 `u16be` female, `i8`; 26 `u16be`; 30..34 `jstr` ops; 35..39 `jstr` iops; 40 recolour; 78 `u16be`; 79 `u16be`; 90..93 `u16be`; 95 `u16be`; 97 `u16be` note; 98 `u16be` note template; 100..109 `u16be` count and `u16be` stack; 110..112 `u16be` scales; 113 `i8`; 114 `i8` times 5; 115 `u8` team |

`UnkType` has a constructor and no `decodeClient` (`jagex2/config/UnkType.java:18`). There is no enum, struct, or separate client-script archive. Interface scripts are int arrays inside `Component`.

## Version list and ondemand

`OnDemand.load` (`jagex2/io/OnDemand.java:539`) reads, in archive order 0 models, 1 anims, 2 midi, 3 maps:

| file | layout |
| --- | --- |
| `model_version`, `anim_version`, `midi_version`, `map_version` | `u16be` per file |
| `model_crc`, `anim_crc`, `midi_crc`, `map_crc` | `u32be` per file |
| `model_index` | one byte per model, 0 past the file |
| `map_index` | records of 7 bytes: `u16be` map square, `u16be` land, `u16be` loc, `u8` |
| `anim_index` | `u16be` per anim |
| `midi_index` | `u8` per midi |

The update socket writes `u8` 15, reads 8 ignored bytes, then 4 bytes: archive `u8`, file `u16be`, priority `u8` (2 urgent, 1 when not logged in, 0 when logged in) (`OnDemand.java:741`). The response header is 6 bytes: archive `u8`, file `u16be`, length `u16be`, chunk `u8`. Body chunks are 500 bytes. Length 0 rejects the file (`OnDemand.java:70`).

Map file ids are in [region.md](region.md). Model bytes are only framed through the 18-byte trailer in [scene.md](scene.md). Anim frame bytes are unread (`jagex2/dash3d/AnimFrame.java:20`).
