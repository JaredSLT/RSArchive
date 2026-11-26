# Scene

Software path only. The tree has no `jaggl`, `jagdx`, or `jagtheora`.

`GameShell.initApplication` creates `ViewBox`, an AWT `Frame`, with title-bar inset `value28 = 28` (`jagex/client/GameShell.java:137`, `jagex/client/ViewBox.java:25`). The game pixmap is `MudPix` (`deob/MudPix.java:11`).

## Draw order

`MudClient.call34c75df3` (`deob/MudClient.java:4185`): a nonzero `value250` draws the death string and returns. Appearance and security screens return before the world. When `world.flag427` is set, roof models are removed and, on plane 0, the upstairs models too. A cell whose collision bit 128 is clear is indoors (`flag107`, `MudClient.java:4206`).

NPCs are submitted with `World3D.call25c5add9`, sprite id `20000 + index`, sizes `ClientConfig.numberArray457` and `numberArray458` (`MudClient.java:4344`). `number17` 8 tilts -30 and `number17` 9 tilts 30. Ground items use id `40000 + item id` (`'鱀'` is U+9C40, 40000) at height offset `numberArray139` (`MudClient.java:4358`).

`MudPix.call3f614d` (`MudPix.java:21`) sends ids at or above 40000 to `MudClient.call74b02490` as `id - 40000`, ids at or above 20000 to `call7fec4788` as `id - 20000`, ids at or above 5000 to `calld8dc823a` as `id - 5000`, and every smaller id to `PixMap.calle1658783` (`jagex/client/PixMap.java:983`).

## Camera

`World3D.callf90da9a8(x, y, z, pitch, yaw, roll, distance)` masks pitch, yaw, and roll with 1023 (`jagex/client/World3D.java:2811`). The call sites pass pitch 912, yaw `number112 * 4`, roll 0 (`MudClient.java:4391` and `4411`).

| mode | clip fields | distance |
| --- | --- | --- |
| `flag188` | `value1000 = 3000`, `value10002 = 3000`, `value20 = 1`, `value10 = 2800` | 2000 |
| ordinary, `flag979` false | 2400, 2400, 1, 2300 | `value550 * 2` |
| `flag979` true | 2200, 2200, 1, 2100 | `value550 * 2` |

`load` also sets the ordinary clips and calls `call19579efa(-50, -10, -50)` (`MudClient.java:797`). Plane 3 adds `call3b18d590` with a random first argument 40..42 and a random second argument 40..46 (`MudClient.java:4365`). The raster after the camera set is `World3D.callc3e902a1` (`World3D.java:564`, called at `MudClient.java:4414`). That body and the `PixMap` plot loops were not transcribed. See `gaps.md`.

`Polygon3D` is the face record (`jagex/client/Polygon3D.java:6`): a boolean, an object id defaulting to -1, sixteen ints, and an `Object3D`. `ScanLine` is four ints (`jagex/client/ScanLine.java:6`).

## Models and terrain

Applet models come from `models12.jag` as `.ob3` bytes and `new Object3D(bytes, offset, true)` (`MudClient.java:1000`). That constructor (`jagex/client/Object3D.java:365`) reads u16be vertex count, u16be face count, then three `g2s` per vertex, then per face a u8 vertex count, two `g2s` (32767 replaced by `number755`, which is 12345678), a u8 that stores 0 or `number755`, then index lists of u8 when the vertex count is below 256 and u16be otherwise.

A local client loads `../gamedata/models/<name>.ob2` (`MudClient.java:1010`). `Object3D(String)` (`Object3D.java:528`) reads three bytes, decodes a length with `call129abeec`, and reads that many body bytes. `call129abeec` (`Object3D.java:1437`) skips bytes 10 and 13 and decodes three alphabet bytes: `0-9` are 0..9, `A-Z` are 10..35, `a-z` are 36..61, byte 163 is 62, `$` is 63 (`Object3D.java:1476`). The integer is `c0 * 4096 + c1 * 64 + c2 - 131072`. The value 123456 is replaced by `number755`. The body then uses that reader for vertex count, face count, three ints per vertex, and per face the counts consumed by `Object3D.java:594`.

Terrain construction starts in `World.call9329ebf4` (`region.md`). The login backdrop and the in-game mesh after the sector load were not line-traced. See `gaps.md`.
