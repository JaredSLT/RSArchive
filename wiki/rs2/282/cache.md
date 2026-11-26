# Cache

Citations are `builds/rs2/282-2004-12-21/src/src/main/java/deob/`. The tree has no downloaded cache bytes (`builds/rs2/282-2004-12-21/meta.json` has `cache_downloaded` false and `cache_home` `.file_store_32`). The formats below are what this client reads.

## Archive ids

`Client.load` (`Client.java:4480`) loads jag archives by CRC index:

| index | name | progress label |
| --- | --- | --- |
| 1 | title | title screen |
| 2 | config | config |
| 3 | interface | interface |
| 4 | media | 2d graphics |
| 5 | versionlist | update list |
| 6 | textures | textures |
| 7 | wordenc | chat system |
| 8 | sounds | sound effects |

`loadArchiveChecksums` (`Client.java:3903`) fetches `crc` + random + `-282`, 40 bytes: nine `u32be` checksums and one `u32be` check. The check starts at 1234 and, for each checksum, becomes `(check << 1) + crc`. `valueClient460[8]` must become non-zero. On failure the client toggles `valueClient144` and retries.

When `valueClient144` is true, `openUrl` uses a jaggrab socket on port 43595 and writes ASCII `JAGGRAB /` + path + `\n\n` (`Client.java:1568`).

## JagFile

`JagFile.opkind541` (`JagFile.java:27`): `u24be` uncompressed size, `u24be` compressed size. Equal sizes are stored. Unequal sizes call `BZip2.read` from offset 6. Then `u16be` file count. Each entry is `u32be` name hash, `u24be` uncompressed, `u24be` compressed. The data offset starts after the table (`position + count * 10`) and advances by the compressed size. The name hash is uppercase `* 61 + (char - 32)` (`JagFile.java:64`). The BZip2 decompressor body is not cited; the call site is.

## Disk cache

`CouldNotOpenSocket.storeid` is `main` argument 4 (meta run args use 32). `FileStream` (`FileStream.java:16`) caps a file at 500000 bytes. An index record is at `fileId * 6`: `u24be` length, `u24be` sector. Data sectors are 520 bytes: `u16be` file id, `u16be` chunk, `u24be` next sector, `u8` index id, then up to 512 data bytes (`FileStream.java:43`). The sector's index id must equal `valuekind506`.

A completed ondemand file is checked by `OnDemand.opOnDemand11` (`OnDemand.java:720`): the last `u16be` is the version, and the CRC32 of the preceding bytes must match the versionlist CRC.

## Ondemand

Versionlist files (`OnDemand.java:90`):

| file | layout |
| --- | --- |
| `model_version`, `anim_version`, `midi_version`, `map_version` | `u16be` per file |
| `model_crc`, `anim_crc`, `midi_crc`, `map_crc` | `u32be` per file |
| `model_index` | raw bytes, 0 where the file is short |
| `map_index` | 7 bytes per row, see `region.md` |
| `anim_index` | `u16be` per row |
| `midi_index` | one `u8` per file |

Archive index 0 is models, 1 anims, 2 midis, 3 maps. A finished map (archive 3) is rewritten to index 93 before the game consumes it (`OnDemand.java:687`).

`OnDemand.send` (`OnDemand.java:742`) connects to `43594 + portOffset`, writes `u8 15`, discards 8 input bytes, then writes 4-byte requests: `u8` archive, `u16be` file, `u8` priority (2 urgent, 1 when not in game, 0 in game). Idle keepalive is four bytes `0, 0, 0, 10` (`OnDemand.java:451`).

`OnDemand.read` (`OnDemand.java:612`) waits for 6 header bytes: `u8` archive, `u16be` file, `u16be` total length, `u8` chunk. A total length of 0 rejects the request. Each following chunk is at most 500 bytes, and chunk `n` occupies offset `n * 500`. A chunk other than 0 requires the buffer to exist already.

## Config files

All decoders read `u8` opcodes until 0. Multi-byte values are the `Packet` widths in `framing.md`. `jstr` ends with byte 10. `opkind2721` is bytes until byte 10. Counts for flo, seq, spotanim, varp, varbit, and idk are `u16be` at the start of the dat. Loc, npc, and obj use an idx: `u16be` count, then one `u16be` length per id, with the first data offset at 2 (`LocType.java:52`, `NpcType.java:41`, `ObjType.java:58`).

An unknown opcode in flo, seq, spotanim, varp, varbit, and idk is printed and does not consume further bytes, so the rest of that definition desynchronizes. Loc and npc unknown opcodes fall out of the `if` chain the same way. Obj unknown opcodes do the same through opcode 115.

### flo.dat — `FloType.opkind252` (`FloType.java:47`)

| opcode | fields |
| --- | --- |
| 1 | `u24be` rgb |
| 2 | `u8` texture |
| 3 | flag true |
| 5 | flag false |
| 6 | `jstr` |

There is no opcode 4 or 7.

### seq.dat — `SeqType.decodeClient` (`SeqType.java:73`)

| opcode | fields |
| --- | --- |
| 1 | `u8` frame count, then that many (`u16be` frame, `u16be` delay with 65535 becoming -1, `u16be`) |
| 2 | `u16be` |
| 3 | `u8` count, that many `u8`, then a trailing stored 9999999 that is not read from the buffer |
| 4 | flag |
| 5 | `u8` |
| 6 | `u16be` |
| 7 | `u16be` |
| 8 | `u8` |
| 9 | `u8` |
| 10 | `u8` |
| 11 | `u8` |
| 12 | `u32be` |

### spotanim.dat — `SpotAnimType.decodeClient` (`SpotAnimType.java:49`)

| opcode | fields |
| --- | --- |
| 1 | `u16be` model |
| 2 | `u16be` sequence |
| 4 | `u16be` scale x |
| 5 | `u16be` scale y |
| 6 | `u16be` rotation |
| 7 | `u8` |
| 8 | `u8` |
| 40..49 | `u16be` recolour source |
| 50..59 | `u16be` recolour destination |

There is no opcode 3.

### varp.dat — `VarpType.opVarpType1` (`VarpType.java:57`)

| opcode | fields |
| --- | --- |
| 1 | `u8` |
| 2 | `u8` |
| 3 | flag, records this varp as a client varp |
| 4 | flag false |
| 5 | `u16be` |
| 6 | flag |
| 7 | `u32be` |
| 8 | type 1 |
| 10 | `jstr` |
| 11 | flag |
| 12 | `u32be` |
| 13 | type 2 |

### varbit.dat — `VarbitType.opVarbitType1` (`VarbitType.java:49`)

| opcode | fields |
| --- | --- |
| 1 | `u16be` base varp, `u8` start bit, `u8` end bit |
| 2 | flag |
| 3 | `u32be` |
| 4 | `u32be` |
| 10 | `jstr` |

### idk.dat — `IdkType.opkind282` (`IdkType.java:43`)

| opcode | fields |
| --- | --- |
| 1 | `u8` type |
| 2 | `u8` count, then that many `u16be` models |
| 3 | disable flag |
| 40..49 | `u16be` recolour source |
| 50..59 | `u16be` recolour destination |
| 60..69 | `u16be` head model |

### loc.dat — `LocType.opkind61` (`LocType.java:141`)

| opcode | fields |
| --- | --- |
| 1 | `u8` count, then count times (`u16be` model, `u8` type) |
| 2 | `jstr` name |
| 3 | bytes until 10 |
| 5 | `u8` count, then count `u16be` models, and the type array is cleared |
| 14 | `u8` width |
| 15 | `u8` length |
| 17 | solid false |
| 18 | flag |
| 19 | `u8` (1 sets interact) |
| 21 | flag |
| 22 | flag |
| 23 | flag |
| 24 | `u16be` sequence, 65535 becomes -1 |
| 28 | `u8` |
| 29 | `i8` |
| 30..38 | `jstr` into a 5-string array at index `opcode - 30`; `"hidden"` becomes null |
| 39 | `i8` |
| 40 | `u8` count, then count times (`u16be`, `u16be`) |
| 60 | `u16be` mapscene |
| 62 | flag |
| 64 | flag false |
| 65 | `u16be` |
| 66 | `u16be` |
| 67 | `u16be` |
| 68 | `u16be` mapfunction |
| 69 | `u8` |
| 70 | `i16be` |
| 71 | `i16be` |
| 72 | `i16be` |
| 73 | flag |
| 74 | flag |
| 75 | `u8` |

### npc.dat — `NpcType.opkind131` (`NpcType.java:92`)

| opcode | fields |
| --- | --- |
| 1 | `u8` count, then count `u16be` models |
| 2 | `jstr` name |
| 3 | bytes until 10 |
| 12 | `i8` size |
| 13 | `u16be` |
| 14 | `u16be` |
| 17 | four `u16be` anims |
| 30..39 | `jstr` into a 5-string array at `opcode - 30`; `"hidden"` becomes null |
| 40 | `u8` count, then count times (`u16be`, `u16be`) |
| 60 | `u8` count, then count `u16be` heads |
| 90 | `u16be` |
| 91 | `u16be` |
| 92 | `u16be` |
| 93 | visibility false |
| 95 | `u16be` combat |
| 97 | `u16be` |
| 98 | `u16be` |
| 99 | flag |
| 100 | `i8` |
| 101 | `i8` times 5 |
| 102 | `u16be` |
| 103 | `u16be` |

### obj.dat — `ObjType.opObjType1` (`ObjType.java:314`)

| opcode | fields |
| --- | --- |
| 1 | `u16be` model |
| 2 | `jstr` |
| 3 | bytes until 10 |
| 4 | `u16be` zoom |
| 5 | `u16be` x angle |
| 6 | `u16be` y angle |
| 7 | `u16be` x offset, subtract 65536 when above 32767 |
| 8 | `u16be` y offset, same adjustment |
| 10 | `u16be` |
| 11 | stackable |
| 12 | `u32be` cost |
| 16 | members |
| 23 | `u16be` male model, `i8` offset |
| 24 | `u16be` female model |
| 25 | `u16be` male model 2, `i8` offset |
| 26 | `u16be` female model 2 |
| 30..34 | `jstr` ground actions; `"hidden"` becomes null |
| 35..39 | `jstr` inventory actions |
| 40 | `u8` count, then count times (`u16be`, `u16be`) |
| 78 | `u16be` |
| 79 | `u16be` |
| 90..93 | `u16be` head models |
| 95 | `u16be` |
| 97 | `u16be` certificate link |
| 98 | `u16be` certificate template |
| 100..109 | `u16be` stacked model, `u16be` count |
| 110 | `u16be` |
| 111 | `u16be` |
| 112 | `u16be` scale |
| 113 | `i8` |
| 114 | `i8` times 5 |
| 115 | `u8` team |

### interface

Archive 3, file `data`, plus media sprites. See `interfaces.md`.
