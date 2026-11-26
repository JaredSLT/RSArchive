# Scene

The software toolkit `PureJavaToolkit.doPureJavaToolkit(int, int, int, int)` (`com/jagex/graphics/safe/PureJavaToolkit.java:26`) stores four ints. The constructor does the same (`PureJavaToolkit.java:17`). The raster draw path was not read.

`DataBlock2480` holds the canvas width, height, and the two insets the applet receives (`deob/DataBlock2480.java:16`). The compiled default is 765 by 553. See [index.md](index.md).

`clse1a06052.staticelements()` is the in-tick rebuild call (`rs2/client/Client.java:1540`). The region bytes it consumes are in [region.md](region.md). The method that turns those bytes into a software framebuffer was not read.

HD entry points, Java classes that call the platform bindings:

- `com/jagex/graphics/gl/GlToolkit.java:81` extends `SetIgnoreRepaint` and holds an `OpenGL`. The `jaggl` package is platform-native.
- `com/jagex/graphics/glx/GlxToolkit.java:58` extends `GpuToolkit` and holds an `OpenGL`.
- `com/jagex/graphics/dx/DxToolkit.java:70` extends `GpuToolkit` and imports `jagdx`. The `jagdx` package is platform-native.

Camera position bytes that the minimap click writes are in [input.md](input.md). The camera update that produces `Client.tint44`, `Client.tint46`, and `Client.tfloat2` was not read.
