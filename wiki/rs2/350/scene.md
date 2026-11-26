# Scene

Software drawing. This client has no `jaggl`, `jagdx`, or `jagtheora` classes. There is no HD entry point.

## Buffers

`Client.load` builds four software viewports with `Pix3D.opkind963` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:13700`, `builds/rs2/350-2005-12-30/src/src/main/java/deob/Pix3D.java:98`):

| call | stored |
| --- | --- |
| `opkind963(503, 765)` | `valueClient964` |
| `opkind963(96, 479)` | `valueClient961` |
| `opkind963(261, 190)` | `valueClient962` |
| `opkind963(334, 512)` | `valueClient963` |

The game canvas pixmap is 765 by 503 (`Client.java:10547`).

## Draw order

`drawGame` calls `drawScene` when `valueClient880 == 2` (`Client.java:11005`). `drawScene` (`Client.java:1826`):

1. `opClient102(true)` players, `opClient68(true)` NPCs, then both again with false.
2. `opClient47` projectiles and `opClient46` spotanims.
3. Orbit camera unless a cutscene is active (`valueClient497`).
4. `Pix2D.opkind1104` clears the software pixels (`Client.java:1899`).
5. `World3D.opWorld3D41` draws the world (`Client.java:1900`, `builds/rs2/350-2005-12-30/src/src/main/java/deob/World3D.java:1138`).
6. `World3D.opWorld3D16`, `draw2DEntityElements`, `drawTileHint`, `opClient45`, `draw3DEntityElements`.
7. The viewport pixmap is blitted at (4, 4) (`Client.java:1911`).

`drawMinimap` (`Client.java:1526`) and `createMinimap` (`Client.java:9445`) paint the 2D minimap from `World3D.opWorld3D37` and loc mapfunction sprites. Minimap click handling is in [input.md](input.md).

## Terrain

Land heights and overlays are the tile opcodes in [region.md](region.md) (`World.opkind671`). Loc files are the smart-delta stream in the same page. Placement of each loc shape is `World.opkind6710`, which was not read. Mesh construction is `Model`, which was classified by its method list and not read as a rasterizer. `Pix3D` triangle routines and `World3D.opWorld3D41` internals are the same gap. See [gaps.md](gaps.md).
