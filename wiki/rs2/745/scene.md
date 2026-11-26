# Scene

The software toolkit is `PureJavaToolkit`. `Statics.opStatics225` returns `new PureJavaToolkit(canvas, settings, width, height)` (`Statics.java:638-639`). `PureJavaToolkit` itself was not opened, so the draw order, terrain raster, and model raster are unread.

The canvas starts at 765 by 553 (`Client.java:408-409`). Graphics defaults opcode 12 replaces `valueClient1822` and `valueClient1909` when both loaded values are not -1 (`Loading.java:302-304`). `PLAN-CLIENT.md` describes a 765 by 503 software view for later eras. This client uses 765 by 553.

HD classes in the default package (`Gl*`, `Dx*`, `Gpu*`) are the Java side of the platform bindings. `jaggl` and `jagdx` are platform-native. Their draw paths were not specified.

Scene origin and map squares are in [region.md](region.md). The camera spline `Client.opClient444` was not opened. Yaw and pitch packets were not found in the methods that were read. Game opcodes 74 and 100 stay empty until `ClientWatch` is read.
