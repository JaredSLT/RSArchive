# Scene

Software only. This client has no jaggl, jagdx, or jagtheora class.

`PixMap` stores the second constructor argument as the width and the first as the height (`PixMap.java:23`). `Pix3D.opkind153(width, height)` builds a scanline table of `width * y` (`Pix3D.java:93`).

| surface | PixMap | blit |
| --- | --- | --- |
| viewport `valueClient344` | 512 by 334 (`Client.java:5336`) | `(4, 4)` (`Client.java:9662`) |
| minimap `valueClient343` | 172 by 156 (`Client.java:5332`) | `(550, 4)` (`Client.java:3311`) |
| sidebar `valueClient342` | 190 by 261 (`Client.java:5335`) | `(553, 205)` (`Client.java:3894`) |
| chat `valueClient345` | 479 by 96 (`Client.java:5331`) | `(17, 357)` (`Client.java:10296`) |

`Pix3D.opkind153` is called for 479 by 96, 190 by 261, and 512 by 334 (`Client.java:4846`). `Pix2D.opkind126` fills a rectangle (`Pix2D.java:136`).

`Client.drawScene` (`Client.java:9656`) calls `World.opWorld41(pitch, yaw, cameraY, cameraX, cameraZ, plane)`. `World.opWorld41` (`World.java:1116`) clamps camera x and z to the map in 128-unit steps, builds sine and cosine from pitch and yaw, takes a visibility map, and walks a tile window of ±25 before `opWorld47`. The rest of `opWorld47` and the plane walk were not line-read.

`Client.opClient9` (`Client.java:8658`) starts the orbit camera from yaw and pitch. `updateOrbitCamera` and `cinemaCamera` are called from `updateGame` (`Client.java:10916`). Their bodies were not line-read.

Terrain heights and the land file are in `region.md`. Loc and NPC models come from `LocType.getModel` and `NpcType.getModel`. `Model.opkind55` is the model-file loader (`Model.java` method `opkind55`). Its byte layout was not line-read. Sprite drawing is `Sprite.opkind171` through `opkind1713`. Those bodies were not line-read.

There is no HD entry point to mark platform.
