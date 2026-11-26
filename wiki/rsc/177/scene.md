# Scene

Software only. No `jaggl`, `jagdx`, or `jagtheora` class is in this source tree.

The shell is `ViewBox`, an AWT `Frame` (`jagex/client/ViewBox.java:10`). Pixels are `PixMap` (`jagex/client/PixMap.java`). `MudPix` extends it and dispatches sprite ids at 50000, 40000, 20000, and 5000 (`deob/MudPix.java`). The world is `World3D` (`jagex/client/World3D.java`). Models are `Object3D` (`jagex/client/Object3D.java`). Raster helpers are `Polygon3D` and `ScanLine` (`jagex/client/Polygon3D.java`, `jagex/client/ScanLine.java`).

`load` builds `MudPix(512, 334 + 12, 4000, this)` and a `World3D` centered at half of 512 and half of 334 (`deob/MudClient.java:877`, `deob/MudClient.java:895`). `value550` is 550 (`deob/MudClient.java:15249`). `cameraAngleMode` picks the camera. `callade64ba4` snaps `number420` among 8 facings when the tile is blocked (`deob/MudClient.java:8036`). The rest of that camera method was read only through the facing tests.

`draw` when `number384 == 0` paints the login screens. When `number384 == 1` it calls `call34c75df3` (`deob/MudClient.java:1382`). If `value250 != 0` that method draws `Oh dear! You are dead...` and returns (`deob/MudClient.java:8079`). The sleep screen is the next branch (`deob/MudClient.java:8103`). The rest of the world draw, including the 512 by 200 viewport blit, was not enumerated. See [gaps](gaps.md).

`World3D.callc3e902a1` is a fernflower stub (`jagex/client/World3D.java:632`).

Terrain comes from [region](region.md). Loc and bound models are attached by opcodes 253 and 249. Projectile opcode 206 stores a sprite and two tile deltas while fewer than 50 projectiles are active (`deob/MudClient.java:7503`).

Death opcode 241 sets `value250 = 250` and has no further fields (`deob/MudClient.java:6759`).
