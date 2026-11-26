# Input

Walk and click packets are written by `Friend.doFriend` on `Client.pureJavaToolkitContext2`. Width names are in [framing.md](framing.md). `Friend.doFriend()` with no arguments calls `doMouse` on `Statics.gWCLocation.mouse2` (`rs2/client/logic/friendchat/Friend.java:121` and `:141`). The body of `doMouse` was not read.

## Walk and minimap

`Friend.doFriend(int x, int y, int kind)` (`Friend.java:2352`) writes the shared prefix and returns the packet. The caller queues it.

| Kind | Opcode | Size | Bytes |
| --- | --- | --- | --- |
| 0 | 11 `clscdcb044e12` | 5 | u16be `baseX + x`, u16le `baseZ + y`, u8sub128 of `doFriend() ? 1 : 0` |
| 1 | 44 `clscdcb044e45` | 18 | the same 5 bytes |

`getBase()` is the region origin (`com/jagex/game/scene/World.java:176`). Menu action 23 calls this method (`Friend.java:2213`). Kind 0 queues only those 5 bytes. Kind 1 appends u8 `255`, u8 `255`, u16be `(int) Client.tfloat2`, u8 `57`, u8 `Client.tint44`, u8 `Client.tint46`, u8 `89`, u16be of the local player's x float, u16be of the local player's z float, and u8 `63` (`Friend.java:2218`). `5 + 13 = 18`. There is no step list.

`rslib/src/both/move/rev_876.c` uses the same three prefix widths. There is no disagreement row.

## Mouse

`NativeMouse.doNativeMouse` (`com/jagex/game/client/NativeMouse.java:19`) writes opcode 73, size 6 (`clscdcb044e74`): u16le of `doUnused23(event, 32767) | (button == 2 ? 1 : 0) << 15`, then `u32m-32222222` of `doKeyFrameSet222() | (doKeyFrameSet22222() << 16)`. Both coordinates are clamped to `0..65535` before that mix. `doNativeMouse22` constructs opcode 116, size `-1`, and writes no fields (`NativeMouse.java:61`).

While `Client.tint72 == 9`, `doClient3222b122` also writes opcode 105, size `-1`: u8 `0`, then `AbstractSocket.doAbstractSocket` appends bytes, then a u8 length patch (`rs2/client/Client.java:3212`). The appended method was not read. The same method starts opcode 8 when `Statics.cursorTypeFactory.tint != -1` (`Client.java:3222`). That write was not finished.

## Menu clicks

`Friend.doFriend(DataBlock1140, int, int, boolean)` (`Friend.java:2019`) sends these. `baseX` and `baseZ` come from `getBase()`.

| Action | Opcode | Size | Fields | Line |
| --- | --- | --- | --- | --- |
| 2 | 107 | 17 | u16be+128 `Client.tint86`, u32be `Statics.tint51`, u8sub128 of `doFriend() ? 1 : 0`, u32be of the menu long's high half masked with `0x7fffffff`, u16le `baseX + x`, u16be `Client.tint85`, u16be `baseZ + z` | `:2069` |
| 3, 4, 5, 6, 1001, 1002 | 36, 39, 40, 41, 90, 29 | 9 | u8 of `doFriend() ? 1 : 0`, u16be+128 `baseZ + z`, `u32m-32222222b3` of the same masked high half, u16le+128 `baseX + x` | `:2331` |
| 8 | 32 | 11 | `u32m-32222222b3` of `Statics.tint51`, u8 of `doFriend() ? 1 : 0`, u16be+128 `Client.tint86`, u16be+128 npc index, u16le `Client.tint85` | `:2296` |
| 9, 10, 11, 12, 13, 1003 | 59, 65, 52, 61, 88, 38 | 3 | u8neg of `doFriend() ? 1 : 0`, u16le npc index | `:2105` |
| 15 | 85 | 11 | u32be `Statics.tint51`, u16le `Client.tint85`, u16be player index, u16be `Client.tint86`, u8 of `doFriend() ? 1 : 0` | `:2195` |
| 16 | 85 | 11 | the same, with the local player's `tint` instead of the menu index | `:2051` |
| 17 | 64 | 15 | u16be+128 index, u16be `baseX + x`, u8neg of `doFriend() ? 1 : 0`, u16be+128 `Client.tint85`, u16le `Client.tint86`, u16le+128 `baseZ + z`, `u32m-32222222b3` of `Statics.tint51` | `:2176` |
| 18, 19, 20, 21, 22, 1004 | 24, 70, 26, 94, 28, 0 | 7 | u16le+128 index, u16be `baseX + x`, u8 `((booleanArg ? 2 : 0) \| (doFriend() ? 1 : 0))`, u16be `baseZ + z` | `:2148` |
| 44..53 | 100, 68, 43, 17, 49, 67, 7, 54, 78, 101 | 3 | u8sub128 of `doFriend() ? 1 : 0`, u16le player index | `:2272` |
| 59 | 47 | 12 | u16le+128 `Client.tint86`, u16be+128 `Client.tint85`, u16le+128 `baseX + x`, u32be `Statics.tint51`, u16be+128 `baseZ + z` | `:2117` |
| 60 | 63 | 4 | u16le+128 `baseZ + z`, u16be+128 `baseX + x` | `:2164` |

`booleanArg` is the fourth parameter of that `doFriend` method. Keyboard writers and `Client.cameraMoveTo` (`Client.java:4073`) were not read. Their opcodes stay empty in [packets.md](packets.md).
