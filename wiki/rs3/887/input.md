# Input

Walk and minimap clicks are `WorldMapRenderer.doWorldMapRenderer(int x, int z, int kind)` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/world/WorldMapRenderer.java:2288`). The packet is queued on the game socket `Client.runeScapeSetupStatus2` (`:2102`).

Kind 0 uses `DataBlock552.cls1e74bdac106`, opcode 105, size 5 (`builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:216`). Kind 1 uses `cls1e74bdac120`, opcode 119, size 18 (`DataBlock552.java:244`).

Both write the same 5 bytes (`WorldMapRenderer.java:2299`):

| order | width | value |
| --- | --- | --- |
| 1 | u8_add128 | 1 if `Statics.doStatics2322b15()` else 0. That method was not read |
| 2 | u16be_low128 | `base.tint3 + z` |
| 3 | u16le_low128 | `base.tint2 + x` |

`base` is `Client.world.getBaseb()`. The caller passes tile x then tile z (`:2082`). The method stores those in `ReceivePlayerPositions.tint3` and `tint4`.

When the caller kind is 1, it appends 13 bytes before send (`:2083`):

| order | width | value |
| --- | --- | --- |
| 1 | u8 | `doPositionedSound(-1)`, wire 0xff |
| 2 | u8 | 0xff |
| 3 | u16be | `(int) Client.tfloat2` |
| 4 | u8 | 57 |
| 5 | u8 | `Client.tint45` |
| 6 | u8 | `Client.tint47` |
| 7 | u8 | 89 |
| 8 | u16be | player fine x, `(int) tfloat2` |
| 9 | u16be | player fine z, `(int) tfloat3` |
| 10 | u8 | 63 |

5 + 13 = 18, which matches the constructor. Kind 0 does not append those bytes. It sets `Client.tint66`, `tint67`, `tint69`, and `tint68` and still sends the size-5 packet (`:2095`).

`rslib/src/both/move/rev_887.c` `walk_encode` writes the 5-byte prefix and rejects `nsteps != 0`. It does not write the opcode 119 tail.

## Other clicks in the same method

Action 60 (`:2107`) sends opcode 20, size 4 (`DataBlock552.java:46`): u16be `base.tint3 + z`, u16le_low128 `base.tint2 + x`.

Action 2 starts opcode `cls1e74bdac74` at `:2126`. The method was not finished, so those fields are not listed.

Mouse, key, camera, and the other menu actions in this method were not read.
