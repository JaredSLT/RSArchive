# Scene

The software path is `World3D` projected into `MudPix`, which extends `PixMap`. There is no `jaggl`, `jagdx`, or `jagtheora` type in this revision. HD entry points: none.

## Camera and draw

`MudClient.load` builds `World3D` with 15000, 15000, and 1000, then `calle1ef7591(256, 167, 256, 167, 512, 9)` (`MudClient.java:904`). Logged-in `call34c75df3` clears the surface with `PixMap.call15913791` (`PixMap.java:257`), submits entities and projectiles, then calls `World3D.callf90da9a8` and `World3D.callc3e902a1`.

`callf90da9a8(x, y, z, pitch, yaw, roll, zoom)` (`World3D.java:2817`) masks pitch, yaw, and roll with 1023, stores `1024 - angle`, and applies the sine table `numberArray`. The in-game call is pitch 912, yaw `number116 * 4`, roll 0, zoom `value550 * 2` (`MudClient.java:4862`). An alternate call uses zoom 2000 (`MudClient.java:4842`). Login preview calls use the same pitch and roll (`MudClient.java:2105`).

`callc3e902a1` (`World3D.java:568-599`) builds the frustum and calls `Object3D.call4e47afec` on each submitted model, including the terrain model. The body of `call4e47afec` was not read. See [gaps.md](gaps.md).

`flag1024` from key 1008 is copied to `drawArea.flag867` before the clear (`MudClient.java:4815`). `call15913791` zeros every pixel when `flag867` is false, and zeros every other row when it is true (`PixMap.java:257-268`).

## Sprites

`MudPix.call3f614d` (`MudPix.java:21-33`) dispatches a sprite id:

| Id | Call |
| --- | --- |
| at least 50000 | `MudClient.callca501252` with `id - 50000` (`'썐'`) |
| at least 40000 | `MudClient.call74b02490` with `id - 40000` (`'鱀'`) |
| at least 20000 | `MudClient.call7fec4788` with `id - 20000` |
| at least 5000 | `MudClient.calld8dc823a` with `id - 5000` |
| otherwise | `PixMap.calle1658783` |

Ground items are submitted as `'鱀' + itemId` (40000 plus the id) at `MudClient.java:4794`. Projectiles use `'썐' + index` (`MudClient.java:4803`). The scaler body of `calle1658783` after its width read (`PixMap.java:993`) was not read.

## Terrain

The mesh is `World.call9329ebf4`, invoked from `World.call4fc32bcd` (`World.java:1018`). Sector bytes are decoded in [region.md](region.md). The walkable grid is 96 by 96. Tile pitch is 128.

## Models

Application `main` calls `initApplication`, so `GameShell.flag993` is false and models load through `Object3D(String)` from `../gamedata/models/name.ob2` (`MudClient.java:1166`). The applet path uses `Object3D(byte[], int, boolean)` on the models jag.

`Object3D(byte[], int, boolean)` (`Object3D.java:365-455`):

| Order | Field |
| --- | --- |
| 1 | `u16be` vertex count |
| 2 | `u16be` face count |
| 3 | vertex count times `i16be` x, then the same for y, then for z |
| 4 | per face: `u8` vertex count |
| 5 | per face: `i16be`, and 32767 becomes `number800` (12345678) |
| 6 | per face: a second `i16be` with the same 32767 rule |
| 7 | per face: `u8`, 0 stores 0 and any other value stores `number800` |
| 8 | per face: that many vertex indices, `u8` when the vertex count is below 256, otherwise `u16be` |

`Object3D(String)` (`Object3D.java:528-623`) reads the file, takes the first three alphabet bytes as a length through `call129abeec`, reads that many bytes, then reads vertex count, face count, three ints per vertex, and per face a vertex count, two ints, `number837`, `number838`, a flag, the vertex indices, and a second index list. Every integer is `call129abeec`.

`call129abeec` (`Object3D.java:1438-1451`) skips bytes 10 and 13, then reads three alphabet bytes. The value is `c0 * 4096 + c1 * 64 + c2 - 131072`. The value 123456 is replaced with `number800` (12345678).

`numberArray3` (`Object3D.java:1480-1492`): `'0'` through `'9'` map to 0 through 9, `'A'` through `'Z'` map to 10 through 35, `'a'` through `'z'` map to 36 through 61, byte 163 maps to 62, and `'$'` (36) maps to 63. The writer table `byteValueArray` stores the same symbols in that order, with index 62 holding byte `-93` (unsigned 163) and index 63 holding `'$'`.

`Object3D(Object3D[], ...)` merge constructors at lines 625 and 654 were not read.
