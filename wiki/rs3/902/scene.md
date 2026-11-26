# Scene

The software toolkit is `deob.PureJava`, which extends `DataBlock60` (`deob/PureJava.java:26`). Raster and model methods on that class include `doPureJava(int, int, int[], float[])` at line 348 and the triangle helpers from line 1074. Their pixel loops were not read. The software model is `com.jagex.graphics.safe.PureJavaModel` (`PureJavaModel.java:34`).

HD classes extend the same toolkit base or `GpuToolkit` and are not the software path:

| Class | Line |
| --- | --- |
| `com.jagex.graphics.GpuToolkit` | `GpuToolkit.java:84` |
| `com.jagex.graphics.gl.GlToolkit` | `GlToolkit.java:86` |
| `com.jagex.graphics.glx.GlxToolkit` | `GlxToolkit.java:64` |
| `com.jagex.graphics.dx.DxToolkit` | `DxToolkit.java:68` |

`jaggl` and `jagdx` are platform bindings (`gaps.md`, `platform-native`). There is no `jagtheora` tree.

The default software view is 765 by 553 (`Client.java:989`), not 765 by 503. A GWC record can replace it (`DataBlock374.java:329`). Terrain is built by the rebuild in [region.md](region.md). The camera step while `Statics.tint110 == 2` runs from `Client.doClient2222b242` (`Client.java:3326`) through `LoginManager`, which moves the camera and does not write the login block.
