# Interfaces

Interface bytes live in JS5 index 3, `Statics.js525` (`com/jagex/graphics/camera/CameraSettingType.java:336` and `:475`). `DataBlock1540.doDataBlock1200(Js5, Js5, Js5, Js5)` stores that archive as `Statics.js510` (`deob/DataBlock1540.java:428`). Loading group `var0` file `var6` calls `doDataBlock1200(PositionedSound)` (`DataBlock1540.java:484`).

## Component header

`doDataBlock1200(PositionedSound)` (`DataBlock1540.java:510`) reads:

1. u8 version. `255` is stored as `-1`.
2. u8 type. If bit 7 is set, the bit is cleared and a jstr name follows.
3. u16be, i16be, i16be, u16be, u16be.
4. four i8.
5. u16be parent. `65535` becomes `-1`. Any other value is combined with `tint20 & -65536`.
6. u8 flags. Bit 0 sets `tboolean2`. Bit 1 sets `tboolean3` only when the version is `>= 0`.

Type-specific bytes:

| Type | Fields |
| --- | --- |
| 0 | two u16be. If version `< 0`, one more u8 compared with 1. |
| 3 | u32be, u8 compared with 1, u8 |
| 4 | `doPositionedSound32222222b4b`. If version `>= 2`, u8 compared with 1. Then jstr, three u8, u8 compared with 1, u32be, u8. If version `>= 0`, one more u8. |
| 5 | u32be, u16be, u8 (bit 0 and bit 1), two u8, u32be, two u8 compared with 1, u32be. If version `>= 3`, one more u8 compared with 1. |
| 6 | `doPositionedSound32222222b4b`, then u8 flags. Bit 0 reads two i16be and four u16be. Bit 1 reads three i16be, three u16be, and one i16be. Bits 2 and 3 are stored with no extra bytes. Another `doPositionedSound32222222b4b` follows. A nonzero third header i8 reads a u16be, and a nonzero fourth header i8 reads a u16be. |
| 9 | u8, u32be, u8 compared with 1 |

No other type id is tested.

The common tail (`DataBlock1540.java:634`):

1. u24be `var7`.
2. u8 values until `0`. Each value's high nibble minus 1 is an index `0..10`. The low nibble is joined with the next u8 as `(first << 8) | second`, and the low 12 bits are kept (`4095` means `-1`). Two i8 follow.
3. jstr `string3`.
4. u8 whose low nibble is a jstr count and whose high nibble controls the next ids. Each low-nibble slot is a jstr. If the high nibble is `> 0`, a u8 index sizes an int array filled with `-1` and one u16be is stored at that index. If the high nibble is `> 1`, another u8 index and u16be are stored.
5. jstr `string4`. An empty string becomes null.
6. three u8, then jstr `string5`.
7. If `(var7 >> 11) & 127` is not 0 (`Statics.doStatics2`, `deob/Statics.java:1594`), three u16be follow, and `65535` means `-1`.
8. If version `>= 0`, one u16be follows, and `65535` means `-1`.
9. If version `>= 0`, a u8 count of pairs `(u24be, u32be)`, then a u8 count of pairs `(u24be, jstr0)`.
10. Twenty-one script arrays from `doDataBlock12002`, plus one more when version `>= 0`, then five int arrays from `doDataBlock120022`.

`doDataBlock12002` (`:774`): u8 count, and `0` means null. Each entry is a u8 type: `0` then u32be, `1` then jstr. `doDataBlock120022` (`:796`): u8 count, and `0` means null, otherwise that many u32be.

## Packets that open interfaces

Server opcode 73, size 19 (`Statics.java:4429`): discarded u8sub128, u32be, u32le, `u32m-3322b6`, u16be stored in `Client.tint87`, `u32m-32222222b16`. The four ints are passed to `Client.doClient` and `PingRequest.doPingRequest`.

Server opcode 71, size 4 (`Statics.java:5505`): one `u32m-3322b6` stored in `Statics.tint87`, which is a different field from `Client.tint87` (`Statics.java:654`).

Server opcode 140, size `-2` (`Statics.java:5471`): u16be id, u8 flags (bit 0 and bit 1), u16be count. Each entry is a u16be, then a u8 that is replaced by a following u32be when the u8 is `255`. If bit 1 is set, a u8 count selects that many `doPublicKeysPart` records. The record interior was not read.

Server opcode 45, size `-2` (`Statics.java:4327`): a jstr descriptor. For each character from the end, `s` reads a jstr and any other character reads a u32be. A final u32be is the script id, stored at argument 0. `PingRequest.doPingRequest(DataBlock1688)` runs it.

The interpreter is `PingRequest.doPingRequest(DataBlock1253, DataBlock1625)` (`com/jagex/core/utils/PingRequest.java:638`). The switch starts at case 0. Case bodies were not read.

`Client.tint87` is also assigned from `Statics.playerSkill.tint6` and `tint7` (`rs2/client/Client.java:2145`). Those numbers are archive contents. With no disk cache they stay blocked in [gaps.md](gaps.md).
