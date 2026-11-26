# Scene

## Toolkit

`Toolkit.create` (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/graphics/Toolkit.java:78`) selects the renderer from the mode argument:

| mode | factory |
| --- | --- |
| 0 | `PureJavaToolkitFactory` |
| 1 | `GlToolkitFactory` |
| 3 | `DxToolkitFactory` |
| 5 | `GlxRendererFactory` |

Mode 0 is the software path. Modes 1, 3, and 5 are hardware. The Java wrappers under `com/jagex/graphics/gl`, `com/jagex/graphics/glx`, and `com/jagex/graphics/dx` are classified in `files.tsv`. The JNI surfaces are `jaggl` and `jagdx`, status `platform-native`. There is no `jagtheora` package.

## Software drawing

`PureJavaToolkit` (`PureJavaToolkit.java:50`) extends `Toolkit`. Methods named on that class and not read past their signatures:

- `drawRectangle` at `PureJavaToolkit.java:711`, which calls `drawHorizontalLine` and `drawVerticalLine`
- `drawHorizontalLine` at `PureJavaToolkit.java:1050`
- `drawVerticalLine` at `PureJavaToolkit.java:1154`
- `drawLine` at `PureJavaToolkit.java:1262`
- `drawParticles` at `PureJavaToolkit.java:2290`
- `drawBillboard` at `PureJavaToolkit.java:2388`

## Camera and tick

`Client.orbitCamera` is declared at `Client.java:8451`. Its body was not read. The in-game camera packet is `EVENT_CAMERA_POSITION` in `input.md`. `CAM_LOOKAT` is in `packets.md`. `CAMERA_UPDATE` calls `cam2.decode`, which was not opened.

The stored frame is 765 by 553 (`Client.java:1521`). The draw size follows the canvas. The logic rate is 50 Hz (`GameShell.java:44`). `Client.mainloop` (`Client.java:2074`) is the per-tick order: JS5, audio, keyboard, mouse, then login, rebuild, or `updateGame2`. Scene rebuild runs from `world.rebuildScene` on the rebuild state (`Client.java:2186`). Map loading and the scene origin are in `region.md`.
