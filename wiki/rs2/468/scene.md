# Scene

The shell is constructed with width 765, height 503, and revision 468 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:225`). Startup mode `lowmem` calls `UnlitMaterialRenderer.opUnlitMaterialRenderer9`. `highmem` calls `TextureOp29SubOp4.opTextureOp29SubOp46` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:183`).

`Js5.callf78aac57` builds `MaterialManager_Class106_Sub2(4, 104, 104, IntNode.valueIntNode14)` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialManager_Class106_Sub2.java`) and four `Sprite(104, 104)` collision maps (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5.java:63`). `MaterialManager_Class106` holds `valueMaterialManager_Class10611` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialManager_Class106.java`), which zone updates query for walls, decor, scenery, and ground decor. See [region.md](region.md).

`CursorManager` implements `ImageProducer` (`imageUpdate`, `isConsumer`). `ObjType` is the software raster used as `MouseWheel.valueMouseWheel8 = new ObjType(512, 512)` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5.java:69`). `DisplayMode` extends `SoftwareFont` and is the model object returned by loc and player draw methods. `C88f97d27` also extends `SoftwareFont`.

No `jaggl`, `jagdx`, or `jagtheora` type exists in this tree. `Js5NetQueue.call14f88b04` tries `Class.forName("md")` and, on failure, constructs `GlSprite` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5NetQueue.java:77`). That is an audio callback class, not a GL scene. The class `md` is not in this source tree.

## Camera packets that were read

Opcode 160 reads nothing. It sets `DelayedStateChange.valueDelayedStateChange37` false and `ReflectionCheck.valueReflectionCheck11[0..4]` false (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1309`).

Opcode 45 places the local player. See [sync.md](sync.md).

Opcode 55 starts at `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1063` and opcode 76 starts at `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1592`. Their field lists were not read. Opcode 247, 39, and the terrain draw after `GameShell.opGameShell11` line 72 were not read.

Widget content id 1337 is the viewport branch and 1338 is the minimap branch in `GlFont.opGlFont4` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:121`). What those branches draw was not read.
