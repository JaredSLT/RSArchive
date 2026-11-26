# Scene

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`.

There is no HD entry point and no `jaggl`, `jagdx`, or `jagtheora` type. The software types are `Pix2D`, `Pix3D`, `PixMap`, `Sprite`, `Model`, and `World3D`.

## Camera and draw

`drawScene` (`jagex2/client/Client.java:13817`) pushes players and NPCs, projectiles, and spotanims, then orbits or uses the cutscene camera. Five shake channels add to the camera (`Client.java:13853`). Pitch is clamped to 128..383. `Pix2D.resetBounds` clears the pixel buffer (`jagex2/graphics/Pix2D.java:84`). `World3D.opWorld3D80` clamps camera x and z to the map, loads yaw and pitch sine tables, and keeps a 25-tile window (`jagex2/dash3d/World3D.java:1131`). The call is `opWorld3D80(cameraX, level, cameraY, cameraZ, yaw, pitch)` (`Client.java:13891`). `opWorld3D63` (`World3D.java:625`) walks the temporary sprite list and `opWorld3D64` (`World3D.java:642`) unlinks each sprite from its squares. Overlay helpers and `draw3DEntityElements` follow. The viewport pixmap is blitted at 4,4 (`Client.java:13897`).

`Pix3D.init2D` only clears texture scratch (`jagex2/graphics/Pix3D.java:112`). `fillGouraudTriangle` (`Pix3D.java:402`), `fillTriangle` (`Pix3D.java:1093`), `fillTexturedTriangle` (`Pix3D.java:1618`), and `unpackTextures` (`Pix3D.java:147`) were not read.

## Terrain

Land and loc decoding, the zone origin `(zone-6)*8`, and the absence of XTEA are in [region.md](region.md). `World.opWorld59` (`jagex2/dash3d/World.java:1002`), `opWorld55` (`World.java:918`), `perlin` (`World.java:423`), and `stitchHeightmap` (`World.java:1472`) place or fill tiles and were not read.

## Models

`Model.opModel3` (`jagex2/dash3d/Model.java:130`) treats the last 18 bytes as a trailer. `pos = length - 18`, then:

| order | width | stored as |
| --- | --- | --- |
| 1 | `u16be` | vertex count |
| 2 | `u16be` | face count |
| 3 | `u8` | textured-face count |
| 4 | five `u8` | flags `var5` through `var9` |
| 5 | four `u16be` | section lengths `var10` through `var13` |

The front of the file is then an offset map. Flag `var6 == 255` inserts a per-face group. Flag `var8 == 1` inserts face labels. Flag `var5 == 1` inserts another per-face section. Flag `var9 == 1` inserts vertex labels. Flag `var7 == 1` inserts face priorities. Later offsets cover `var13`, face indices of `faceCount * 2`, texture data of `texturedFaceCount * 6`, then `var10` and `var11` (`Model.java:154`).

`Model.opModel5` (`Model.java:220`) walks those offsets and was not read. `AnimFrame.opAnimFrame1` (`jagex2/dash3d/AnimFrame.java:20`) was not read.

Minimap pixels are drawn by `createMinimap` into a 512 by 512 sprite (`Client.java:10328`). Every 177 calls it sends C2S 173 `u24be` 2657152 (`Client.java:10374`).
