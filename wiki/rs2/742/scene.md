# Scene

Toolkit create id is `RendererToolkit.opRendererToolkit7` (builds/rs2/742-2012-11-07/src/client/src/main/java/RendererToolkit.java:49):

| create id | factory | native |
| --- | --- | --- |
| 0 | `PureJavaToolkitFactory` returns `PureJavaToolkit` (builds/rs2/742-2012-11-07/src/client/src/main/java/PureJavaToolkitFactory.java:13) | no |
| 1 | `GlToolkitFactory` | yes (`opRendererToolkit10`) |
| 2 | `SoftwareToolkitFactory` returns `JaAkLjavaAwtCanvasIiLya` (builds/rs2/742-2012-11-07/src/client/src/main/java/SoftwareToolkitFactory.java:1) | no |
| 3 | `DxToolkitFactory` | yes |
| 5 | `GlxToolkitFactory` | yes |

`opRendererToolkit10` (builds/rs2/742-2012-11-07/src/client/src/main/java/RendererToolkit.java:65) treats ids 1, 3, and 5 as native. `ToolkitType` ids 0..10 are a separate enum (builds/rs2/742-2012-11-07/src/client/src/main/java/ToolkitType.java:7). The preference value checked in `opClient398` (`options.toolkit.getValue() == 2` takes the try/catch path, builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:1461) is that preference, not the create-id switch above.

In-game draw is `opClient349`, which calls `world.getScene().opScene21` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:4640, builds/rs2/742-2012-11-07/src/client/src/main/java/Scene.java:939). The method walks visible tiles from the camera. Server camera packet 149 is `BasicCamera.opBasicCamera2` (builds/rs2/742-2012-11-07/src/client/src/main/java/BasicCamera.java:25). Its optional vectors are three `gFloat` values in `Vector3.opVector314` (builds/rs2/742-2012-11-07/src/client/src/main/java/Vector3.java:101). `gFloat` is `Float.intBitsToFloat(g4s())` (builds/rs2/742-2012-11-07/src/client/src/main/java/Packet.java:350). The `u16be` mask bits are `MouseRecorder` constructor arguments 0 through 14 (builds/rs2/742-2012-11-07/src/client/src/main/java/MouseRecorder.java:7). The `PureJavaToolkit` raster, floor, model, and sprite bodies were not read past the factory. HD entry points are the Gl, Glx, and Dx factories above, plus the platform bindings `jaggl`, `jagdx`. Those bindings are platform-native. `jagtheora` is the audio binding named in [sound.md](sound.md).
