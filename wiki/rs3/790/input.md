# Input

Menu actions are `LowResPlayerInfo.doLowResPlayerInfo` (`com/jagex/game/client/LowResPlayerInfo.java:1655`). Every packet below is queued on `Client.pureJavaToolkitFactory2`. The run byte is `doLowResPlayerInfo() ? 1 : 0`. `var6` is `(int) menu.tlong` (`LowResPlayerInfo.java:1660`). Base coordinates come from `DataBlock1971` (`tint2` is x, `tint3` is z).

The keyboard-like queue is `DataBlock2188` and the mouse queue is `DataBlock2413`, drained at the start of `Client.doClient2` (`rs2/client/Client.java:1479`, `Client.java:1514`). The packets those queues emit, beyond the menu actions below, were not read. Camera bytes appear in the minimap tail.

## Walk and minimap

`doLowResPlayerInfo(int x, int z, int kind)` (`LowResPlayerInfo.java:1989`). Kind 0 writes opcode 33 (`cls34becc0c34`, size 5). Kind 1 writes opcode 62 (`cls34becc0c63`, size 18). Both bodies are:

1. `u16be` with low+128 of `baseZ + z` (`doPositionedSound2322b5`).
2. `u8+128` of the run bit (`doPositionedSound2322b3`).
3. `u16be` with low+128 of `baseX + x`.

Menu action 23 calls that method with `var6` as the kind (`LowResPlayerInfo.java:1854`). When `var6` is 1, the caller appends 13 bytes (`LowResPlayerInfo.java:1855`): `u8` 0xFF, `u8` 0xFF, `u16be` `(int) Client.tfloat2`, `u8` 57, `u8` `Client.tint44`, `u8` `Client.tint46`, `u8` 89, `u16be` camera x, `u16be` camera z, `u8` 63. Five plus 13 is 18, the constructor size of opcode 62. Opcode 33 stays 5 bytes.

`rslib/src/both/move/rev_790.c` `walk_encode` writes only the 5-byte body for both opcodes. The Java tail is the spec. That row is in [gaps.md](gaps.md).

## Menu actions

| Action | Opcode | Size | Fields | Line |
| --- | --- | --- | --- | --- |
| 8 | 9 | 11 | `u16be-m128` `Client.tint84`, `u16le` index, `u16be-m128` `Client.tint85`, `u8` run, `u32w-b` `Statics.tint153` | 1675 |
| 60 | 96 | 4 | `u16be` `baseZ+z`, `u16le` `baseX+x` | 1694 |
| 44 | 66 | 3 | `u8` run, `u16le` player index | 1735 |
| 45 | 81 | 3 | same | 1735 |
| 46 | 75 | 3 | same | 1735 |
| 47 | 109 | 3 | same | 1735 |
| 48 | 87 | 3 | same | 1735 |
| 49 | 50 | 3 | same | 1735 |
| 50 | 51 | 3 | same | 1735 |
| 51 | 49 | 3 | same | 1735 |
| 52 | 14 | 3 | same | 1735 |
| 53 | 100 | 3 | same | 1735 |
| 3 | 34 | 9 | `u32be` `(id & 0x7fffffff)`, `u16be` low+128 z, `u16be` low+128 x, `u8+128` run | 1764 |
| 4 | 24 | 9 | same | 1764 |
| 5 | 28 | 9 | same | 1764 |
| 6 | 41 | 9 | same | 1764 |
| 1001 | 42 | 9 | same | 1764 |
| 1002 | 72 | 9 | same | 1764 |
| 15 | 52 | 11 | `u8` run, `u16be` `tint85`, `u16be` `tint84`, `u32w-d` `Statics.tint153`, `u16le` low+128 index | 1779 |
| 16 | 52 | 11 | same, and the index is the local player's `tint` | 1812 |
| 17 | 30 | 15 | `u16be` `baseX+x`, `u16be` low+128 `tint84`, `u16be` `tint85`, `u32w-b`, `u16be` low+128 z, `u8` run, `u16le` low+128 index | 1795 |
| 18 | 115 | 7 | `u16le` index, `128-u8` run, `u16le` x, `u16be` z | 1841 |
| 19 | 118 | 7 | same | 1841 |
| 20 | 68 | 7 | same | 1841 |
| 21 | 116 | 7 | same | 1841 |
| 22 | 84 | 7 | same | 1841 |
| 1004 | 56 | 7 | same | 1841 |

Game opcode 14 is the action-52 click. Login opcode 14 is the bare pre-ISAAC request. Game opcode 30 is action 17. Login opcode 30 is the `tboolean2` login block. Both rows are in [packets.md](packets.md).

Actions 1008 through 1012 call `Staticelements.doStaticelements` and were not opened (`LowResPlayerInfo.java:1701`).

## Chat

Opcode 17 (`cls34becc0c18`, size `-1`) at `com/jagex/game/client/MouseBinding.java:6792`: placeholder `u8`, `u8` colour, `u8` effect, `DataBlock2443.doDataBlock2443` unread, then the length byte is patched.

Opcode 11 (`cls34becc0c12`, size `-2`) at `MouseBinding.doMouseBinding2222b495` (`MouseBinding.java:6810`): placeholder `u16be`, `jstr`, `DataBlock2443.doDataBlock2443` unread, then the `u16be` length is patched.

While `tint72` is 9, `doClient2222b192` also writes opcode 67 (`cls34becc0c68`, size `-1`) with a placeholder `u8` 0, `DataBlock2472.doDataBlock2472`, and a patched length (`Client.java:3214`). Opcode 40 (`cls34becc0c41`, size 4) is started in the same method when `Statics.cls8190f414.tint` is other than -1 (`Client.java:3224`). The rest of that writer was not read.
