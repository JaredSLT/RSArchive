# Cache

There is no disk cache in this archive. `builds/rs3/887-2017-11-01/README.md` says no OpenRS2 disk cache was downloaded. `meta.json` has `cache_downloaded` false and `cache_home` `.jagex_cache_32/runescape`. Engine is js5. Nothing was downloaded for this spec.

JS5 compression types (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/js5/Js5CompressionType.java`): 0 `UNCOMPRESSED`, 1 `BZIP2`, 2 `GZIP`, 3 `LZMA`.

`Client.doClient(ArchiveUtil, ...)` opens an index by `ArchiveUtil.tint2` (`builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:2214`). Disk cache size is 8000000 for archive 30 and 2500000 otherwise (`:2224`). Constructor name strings in `ArchiveUtil` are empty (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/core/utils/ArchiveUtil.java:8`).

## Archive ids

Ids are the first constructor argument. Opens are `CameraTrackableType` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/camera/CameraTrackableType.java`).

| id | field | opened as |
| --- | --- | --- |
| 0 | `archiveUtil` | `quickchatJs5` `:264` |
| 1 | `archiveUtil2` | `js54` `:265` |
| 2 | `archiveUtil3` | `js521` `:266`. Type lists at `:370` take this archive |
| 3 | `archiveUtil4` | `js540` `:268`. Passed to the component loader `:407` |
| 5 | `archiveUtil5` | `js529` `:269`. Map squares |
| 7 | `archiveUtil6` | `js511` `:270` |
| 8 | `archiveUtil7` | `js538` `:263` |
| 10 | `archiveUtil8` | `js518` `:273` |
| 12 | `archiveUtil9` | `js520` `:274`. `DataBlock1037` reads file 0 of a group (`builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock1037.java:27`) |
| 13 | `archiveUtil10` | `js524` `:199` |
| 14 | `archiveUtil11` | `js537` `:275` |
| 16 | `archiveUtil12` | `js56` `:277` |
| 17 | `archiveUtil13` | `js55` `:278` |
| 18 | `archiveUtil14` | `js512` `:279` |
| 19 | `archiveUtil15` | `js523` `:280` |
| 20 | `archiveUtil16` | `js536` `:281` |
| 21 | `archiveUtil17` | `js514` `:282` |
| 22 | `archiveUtil18` | `js539` `:283` |
| 23 | `archiveUtil19` | `js525` `:284` |
| 24 | `archiveUtil20` | `js532` `:286` |
| 25 | `archiveUtil21` | `js59` `:287` |
| 26 | `archiveUtil22` | `js541` `:288` |
| 27 | `archiveUtil23` | `js53` `:289` |
| 28 | `archiveUtil24` | `js57` `:200`. `GWCLocation` is built from this |
| 29 | `archiveUtil25` | `js533` `:290` |
| 30 | `archiveUtil26` | `js530` `:292` |
| 31 | `archiveUtil27` | `js515` `:293` |
| 32 | `archiveUtil28` | skipped by login checksums. `js516` uses it unless `tboolean6`, which uses id 34 (`:197`) |
| 33 | `archiveUtil29` | `js517` `:198` |
| 34 | `archiveUtil30` | alternate of id 32 (`:197`) |
| 35 | `archiveUtil31` | `js531` `:291` |
| 40 | `archiveUtil32` | `js522` `:276` |
| 41 | `archiveUtil33` | `js528` `:285` |
| 43 | `archiveUtil35` | `js534` `:271` |
| 44 | `archiveUtil36` | `js542` `:272` |
| 49 | `archiveUtil41` | `js535` `:267` |

Also constructed, with empty names and no open in that block: 42 (`archiveUtil34`), 45, 46, 47, 48, 50, and 51 (`ArchiveUtil.java:74` through `:92`).

## Config opcodes

`DataBlock523.doHeadbarTypeFactory` loops on u8 until 0 (`builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock523.java:231`). `doDataBlock803` (`:245`) handles these opcodes. Widths are listed only where the body was read.

| opcode | fields |
| --- | --- |
| 1 | smart `doPositionedSound32222222b4b` |
| 2 | jstr |
| 4 | u16be |
| 5 | u16be |
| 6 | u16be |
| 7 | u16be, then subtract 65536 when the value is above 32767 |
| 8 | u16be, same adjustment |
| 11 | no bytes |
| 12 | u32be |
| 13 | u8 |
| 14 | u8 |
| 15 | no bytes |

The same method also compares opcodes 16, 18, 23, 24, 25, 26, 27, 40, 41, 42, 43, 44, 45, 65, 78, 79, 90 through 98, 110 through 115, 121, 122, 125, 126, 132, 134, 139, 140, 157, 161 through 165, 167, and 249. Those field widths were not read. Which archive holds `DataBlock523` was not tied in the lines read.

`GWCLocation.doGWCLocation` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/shared/framework/gwc/GWCLocation.java:102`) loops on u8 until 0. Opcode 1 reads four pairs of i16be. Opcode 2 reads `doPositionedSound32222222b3`. Opcode 3 reads a u8 count. Opcode 4 reads no bytes. Opcode 5 reads u24be into `tint6`. Opcode 6 reads u24be into `tint7`, the top-level interface id. Later opcodes were not read.

The update handshake that fetches groups is opcode 28 in [login.md](login.md). It is not the lobby login.
