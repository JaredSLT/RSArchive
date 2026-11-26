# Scene

## Loop

Draw is `Client.calle71babc1` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2409), from `ProjAnim.opProjAnim29` (builds/rs2/634-2010-12-10/src/src/main/java/deob/ProjAnim.java:291).

`C09c2ddcc.opkind12782` is true for client states 0, 1, and 2 and then calls `C078a9aea.opkind17786` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C09c2ddcc.java:80, builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2477). States 7 and 9 (`C42efb241.opkind12293`, builds/rs2/634-2010-12-10/src/src/main/java/deob/C42efb241.java:27) call `C0e62e5e5.opkind15212` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2506). That method body was not read.

`opkind17786` ensures `C77f20e7e.valuekind178027` exists and, when the redraw flag is set, calls `opkind126812` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C078a9aea.java:22).

## Scene thread

`C4b39fa30.opkind12702` constructs `C253d3d13`, calls `opkind12682`, and starts it as a thread (builds/rs2/634-2010-12-10/src/src/main/java/deob/C4b39fa30.java:25). The same class fills 16384-entry sine and cosine tables. `opkind126812` sets `valuekind12683` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C253d3d13.java:198).

`C253d3d13.run` loops until `valuekind12684` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C253d3d13.java:61). The part that was read calls `valuekind12682.opkind16439` when that object is an `Offsetpertencycles`, and otherwise builds a `FloorModel` through toolkit `Cced21ec0.valuekind12871.opMidiDecoder333` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C253d3d13.java:67). The rest of `run` was not read.

## Toolkit

The live toolkit is `Cced21ec0.valuekind12871`. It is created by `C3b816c24.opkind17381` during startup (builds/rs2/634-2010-12-10/src/src/main/java/deob/ProjAnim.java:484). That factory was not read.

`MidiDecoder.opMidiDecoder257` returns true (builds/rs2/634-2010-12-10/src/src/main/java/deob/MidiDecoder.java:1476). Overrides exist on `CutsceneLoadingStage`, `GpuWaterRelated`, and `HashMapKey`. Those bodies were not read. `C31334af7.opMidiDecoder257` is abstract (builds/rs2/634-2010-12-10/src/src/main/java/deob/C31334af7.java:207).

The draw path calls `opMidiDecoder257` when deciding whether to resize (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2447) and whether to draw debug rectangles (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2473).

## Platform

HD bindings are platform code:

- `jaggl.OpenGL` (builds/rs2/634-2010-12-10/src/src/main/java/jaggl/OpenGL.java:7) and `jaggl.MapBuffer`.
- package `jagdx` (Direct3D structures and buffers).

`ClientOptionsLightingDetail` imports `jaggl.OpenGL` and also contains the C2S send helper. The class is in `deob`, so it is not marked platform-native. Its GL methods were not read.
