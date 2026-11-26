# Input

Walk, minimap, and the menu ops below are written by `AccountAppealLogin.doAccountAppealLogin`. The run byte is `doAccountAppealLogin()` (`rs2/client/login/AccountAppealLogin.java:121`), a key-binding check through `LocTypeCustomisation`, not a hard-coded control key. Coordinates are `Bindings.tint2 + x` and `Bindings.tint3 + z` from `RebuildStage.doRebuildStage2222b24`.

## Walk and minimap

`doAccountAppealLogin(int x, int y, int kind)` (`AccountAppealLogin.java:2427`):

| kind | Opcode | Size | Body |
| --- | --- | --- | --- |
| 0 | 41 `clse7da79ad42` | 5 | `u8+128` run, `u16le` x, `u16be_low+128` z |
| 1 | 78 `clse7da79ad79` | 18 | the same 5 bytes, then the caller appends the 13 bytes below |

Kind 1 append (`:2251`): `u8 -1`, `u8 -1`, `u16be` of `(int) Client.tfloat2`, `u8 57`, `u8 Client.tint44`, `u8 Client.tint46`, `u8 89`, `u16be` player fine x, `u16be` player fine z, `u8 63`.

Menu op 60, when it does not take the forced-walk path, sends opcode 118 `clse7da79ad119` size 4: two `u16be_low+128` coordinates and no run byte (`:2396`).

`rslib/src/both/move/rev_902.c` writes opcode 41 as `p1_add128`, `p2_le`, `p2_add128`. Those are `u8+128`, `u16le`, and `u16be_low+128`. The walk body matches.

## Menu ops

The dispatcher is `doAccountAppealLogin(DataBlock190, int, int, boolean)` (`:2093`). `var6` is the menu opcode after subtracting 2000 when it is at least 2000.

| Menu | Packet | Size | Fields |
| --- | --- | --- | --- |
| 9, 10, 11, 12, 13, 1003 | 53, 113, 80, 34, 83, 87 | 3 | `u16be` npc index, `u8+128` run |
| 8 | 60 | 11 | `u16le_low+128` index, `u32be` `Statics.tint4`, two `u16be_low+128` (`tint85`, `tint84`), `u8(128-run)` |
| 2 | 57 | 17 | `u16le` `tint84`, `u32_alt13` of `(id >>> 32)`, `u32be` `tint4`, `u16le_low+128` x, `u16be` `tint85`, `u8+128` run, `u16be` z |
| 3, 4, 5, 6, 1001, 1002 | 32, 19, 16, 103, 64, 120 | 9 | `u16le` z, `u16le_low+128` x, `u32_alt13` id, `u8(0-run)` |
| 59 | 52 | 12 | `u16le` x, `u16le_low+128` z, `u16be_low+128` `tint84`, `u16be` `tint85`, `u32be` `tint4` |
| 15 and 16 | 26 | 11 | `u32_alt14` `tint4`, `u16le_low+128` `tint85`, `u8` run, `u16le` index, `u16le_low+128` `tint84`. Menu 15 uses the clicked index; menu 16 uses the local player index |
| 17 | 76 | 15 | `u16le` z, `u16be_low+128` x, `u8(128-run)`, `u32_alt14` `tint4`, `u16be_low+128` id, `u16le` `tint84`, `u16be` `tint85` |
| 44..53 | 45, 5, 102, 58, 110, 40, 29, 112, 7, 22 | 3 | `u16be_low+128` player index, `u8+128` run |
| 18..22 and 1004 | 24, 23, 91, 27, 84, 15 | 7 | `u16le` x, `u16le` z, `u8` `((flag ? 2 : 0) \| (run ? 1 : 0))`, `u16be` id |
| 23 | 41 or 78 | | `doAccountAppealLogin(x, z, kind)` |

`Client.opplayer` (`Client.java:11568`) writes the same player opcodes 45, 5, 102, 58, 110, 40, 29, 112, 7, 22.

## Other c2s on the tick

While state is 19, `Client.doClient2222b242` (`Client.java:3249`) sends opcode 79 `clse7da79ad80` size `-1`: `u8 0`, then `ConfigTypeListPreload.doConfigTypeListPreload` (body unread), then a `u8` length backpatch. Opcode 106 `clse7da79ad107` size 4 (`:3290`): `u8+128` of the GC percent or `-1`, `u16le` `questCommands.tint`, `u8` `Client.tint`.

`CursorTypeFactory.doCursorTypeFactory2222` and `DataBlock316.doDataBlock311` run next (`:3301`). Their packets were not read. Keyboard and mouse are drained in `doClient2` (`:1580`, `:1626`) and the packet bodies were not read.
