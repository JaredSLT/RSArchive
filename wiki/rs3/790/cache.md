# Cache

`builds/rs3/790-2013-12-16/meta.json` has `cache_downloaded` false. This revision has no disk cache in the archive, and none was downloaded. Archive ids below are the ids the client opens. The bytes those archives hold are absent.

## Archive ids

`DataBlock1979` (`deob/DataBlock1979.java:6`) constructs ids 0, 1, 2, 3, 5, 7, 8, 9, 10, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, and 40. Ids 4, 6, 11, and 15 are absent from that list. Id 40 is `cls3aee633537`, and its last constructor boolean is true (`DataBlock1979.java:78`). `doDataBlock1979222` (`DataBlock1979.java:108`) returns one past the highest id, which these constructors put at 41.

`Statics.java:3598` opens them. The CRC list `Statics.doStatics(PreferencesCpuUsage)` (`deob/Statics.java:7453`) writes one `u32be` `doDataBlock19162()` per archive, in this order:

| Order | Field | Archive id |
| --- | --- | --- |
| 1 | `cls2495e59335` | 0 |
| 2 | `cls2495e59326` | 1 |
| 3 | `cls2495e5939` | 2 |
| 4 | `cls2495e59314` | 3 |
| 5 | `cls2495e59327` | 5 |
| 6 | `cls2495e5936` | 7 |
| 7 | `cls2495e5932` | 8 |
| 8 | `cls2495e59341` | 9 |
| 9 | `cls2495e59315` | 10 |
| 10 | `cls2495e593` | 12 |
| 11 | `cls2495e5938` | 13 |
| 12 | `cls2495e59333` | 14, only when `DataBlock2071.tboolean` is false |
| 13 | `cls2495e5937` | 16 |
| 14 | `cls2495e59325` | 17 |
| 15 | `cls2495e59321` | 18 |
| 16 | `cls2495e5935` | 19 |
| 17 | `cls2495e59330` | 20 |
| 18 | `cls2495e59329` | 21 |
| 19 | `cls2495e59336` | 22 |
| 20 | `cls2495e59339` | 23 |
| 21 | `cls2495e59332` | 24 |
| 22 | `cls2495e59319` | 25 |
| 23 | `cls2495e59344` | 26 |
| 24 | `cls2495e59340` | 27 |
| 25 | `cls2495e59328` | 28 |
| 26 | `cls2495e59337` | 29 |
| 27 | `cls2495e59343` | 30 |
| 28 | `cls2495e59331` | 31 |
| 29 | `MapElementType.doMapElementType2222b5()` | return value unread |
| 30 | `MapElementType.doMapElementType2222_dup()` | return value unread |
| 31 | `cls2495e59318` | 35 |
| 32 | `cls2495e59334` | 36 |
| 33 | `cls2495e59342` | 37 |
| 34 | `u32be` 0 | |
| 35 | `u32be` 0 | |
| 36 | `cls2495e59324` | 40, only when `DataBlock2071.tboolean` is false |

Named opens at `Statics.java:3715`:

- Index 5 (`cls2495e59327`) is the map archive the rebuild queries. See [region.md](region.md).
- Index 3 (`cls2495e59314`) is widgets, with models index 7, sprites index 8, and fonts index 13 (`Statics.java:3737`). Fonts are opened at `Statics.java:3536`.
- Index 10 file name `"huffman"` (`Statics.java:3746`) is passed to `DataBlock2443.doDataBlock2443`. The chat codec that consumes it was not read.
- Index 16 (`cls2495e5937`) plus models index 7 constructs `DataBlock2347` (`deob/DataBlock2347.java:10`, `Statics.java:3715`).
- Index 18 (`cls2495e59321`) plus models constructs `DataBlock2084` (`deob/DataBlock2084.java:10`, `Statics.java:3720`).
- Index 2 (`cls2495e5939`) constructs the `Obj` and `DataBlock1835` lists (`Statics.java:3729`).
- Index 28 (`cls2495e59328`) is the defaults archive. It builds `DataBlock2200` and `DataBlock2368` (`Statics.java:3682`, `Statics.java:3690`).

## Loc decode

`DataBlock2103.doDataBlock2103` (`deob/DataBlock2103.java:176`) reads `u8` opcodes until 0. Opcodes read:

| Opcode | Body |
| --- | --- |
| 1 | `u8` count. Per entry: `i8`, then `u8` count of `smart32-1` ids |
| 2 | `jstr` name |
| 14 | `u8` |
| 15 | `u8` |
| 17 | clears `tint4` |
| 18 | no buffer read |
| 19 | `u8` |
| 21 | sets a byte |
| 22 | sets a boolean |
| 23 | sets `tint7` to 1 |
| 24 | `smart32-1` |
| 27 | sets `tint4` to 1 |
| 28 | `u8 << 2` |
| 29 | `i8` |
| 30–34 | `jstr` actions |
| 39 | `i8 * 5` |
| 40 | `u8` count of `u16be` pairs |
| 41 | `u8` count of `u16be` pairs (`DataBlock2103.java:244`) |
| 42 | `u8` count, then that many bytes (`DataBlock2103.java:253`) |

Later opcodes in that method were not read. Npc, obj, seq, and var opcode loops were not read. `NPCType.doNPCType` is a clan decoder on this client, so the npc config loop is the unread `DataBlock2084` path.

## JS5

The handshake and the `DataBlock2372` constructor are in [login.md](login.md). The master-index store writes one `u32be` per `GraphNode` (`rs2/client/Client.java:2039`). The verifier method was not read.
