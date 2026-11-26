# Scene

The software toolkit is `deob.PureJava3` (`builds/rs3/876-2016-11-28/src/src/main/java/deob/PureJava3.java:18`). It extends `SetIgnoreRepaint` and imports `com.jagex.graphics.safe.PureJavaToolkitFactory`. Its draw methods were not read. That is the software-scene row in [gaps.md](gaps.md).

HD entry points, marked platform:

| Class | Extends | Native | Line |
| --- | --- | --- | --- |
| `com.jagex.graphics.gl.GlToolkit` | `SetIgnoreRepaint` | `jaggl.OpenGL` | `GlToolkit.java:88` |
| `com.jagex.graphics.glx.GlxToolkit` | `GpuToolkit` | `jaggl.OpenGL` | `GlxToolkit.java:64` |
| `com.jagex.graphics.dx.DxToolkit` | `GpuToolkit` | `jagdx` | `DxToolkit.java:71` |
| `com.jagex.graphics.GpuToolkit` | `SetIgnoreRepaint` | abstract | `GpuToolkit.java:94` |

`jaggl` and `jagdx` are platform-native. This client has no `jagtheora` tree. `jaclib` is a classified Java helper, not one of those three packages.

`PureJavaToolkitContext` is the socket, not a rasteriser. `Client.cameraMoveTo` (`rs2/client/Client.java:4073`) was not read, so camera pitch, yaw, and terrain submission are not specified here.
