# Scene

The software toolkit is `com.jagex.graphics.safe.PureJavaToolkit` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/safe/PureJavaToolkit.java:9`). Its constructor reads u16be and u8 from a buffer (`:17`). Terrain, model, sprite, and camera draw methods were not read.

HD enters through `com.jagex.graphics.gl.GlToolkit`, which loads `jaclib` and `jaggl` and constructs `jaggl.OpenGL` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/gl/GlToolkit.java:469`). `jaggl` and `jagdx` are platform-native. There is no `jagtheora` tree. `jaclib` is not in that set. Its files are classified in `files.tsv`.

Region rebuild and the scene origin are in [region.md](region.md). The per-frame draw order was not read.
