# Scene

The software path is `Pix2D` into an `int[]`, `Pix3D` triangle fills, and `World3D.draw`. There is no jaggl, jagdx, or jagtheora class in this client.

`Client.drawGame` calls `drawScene` at `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:8959`. `drawScene` calls `this.scene.draw(cameraX, cameraZ, cameraYaw, cameraY, level, cameraPitch)` at `Client.java:13351`. `World3D.draw` clamps the camera to the scene size in tile units times 128. `World3D.java:1140`.

`Pix2D.bind` installs the pixel buffer and the width and height. `Pix2D.java:23`. `Pix2D.clear` is called before the scene draw. `Pix3D.fillGouraudTriangle` is at `Pix3D.java:407`, `fillTriangle` at `Pix3D.java:1098`, and `fillTexturedTriangle` at `Pix3D.java:1623`. The raster loops inside those methods were not line-read.

`Model.draw` and `Model.drawSimple` project a model. `Model.java:1619`, `Model.java:1670`. Sprites are `jagex2.graphics.Sprite`. The viewport pixmap is blitted at (4, 4) by `areaViewport.draw`. `Client.java:13357`.

## Camera

`updateOrbitCamera` follows the player plus the anticheat offsets. Pitch is clamped to 128..383 while camera modifiers run. `Client.java:13332`. Keys 1..4 set `sendCamera` and produce opcode 86. See [input.md](input.md).

Opcode 107 clears the cutscene and the five camera modifiers. Opcodes 166 and 177 set the cutscene source and destination. Opcode 35 enables one modifier: `u8` index, `u8` jitter, `u8` wobble scale, `u8` wobble speed. `Client.java:12517`.

The minimap is drawn by `drawMinimap`. Packet 99 sets `minimapState`. A state of 0 is the only state in which a click sends a walk. `Client.java:8373`.

## Terrain and models

Land and loc decoding is in [region.md](region.md). `Model.unpack` reads an 18-byte tail: `u16be` vertex count, `u16be` face count, `u8` textured face count, five flag bytes, and four `u16be` lengths, then computes section offsets. `Model.java:130`. The vertex and face byte layout after that header is unread.

`AnimFrame.unpack` reads four `u16be` lengths from the last 8 bytes, splits the file into five packets, reads `u16be` frame count, and for each frame reads `u16be` id, `u8` base id, and `u8` transform count. `AnimFrame.java:29`. The bytes of each transform after that `u8` were not field-read.

`CollisionMap` exposes `addWall`, `addLoc`, `reachedWall`, and `reachedLoc`. The flag bits those methods set were not read. `getDrawLevel` treats tile-flag bit 8 as a bridge and bit 2 on the level above as a drop. `World.java:1403`.

`World.perlin`, used for level-0 heights, was not line-read.
