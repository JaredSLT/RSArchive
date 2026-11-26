# Scene

`Cb90b1f11.opkind11045` selects the toolkit (builds/rs2/614-2010-07-13/src/src/main/java/deob/Cb90b1f11.java:27).

| id | result | kind |
| --- | --- | --- |
| 0 | `ObaInit.opObaInit11(..., false)` returns `VertexDeclarationElementComponent` (builds/rs2/614-2010-07-13/src/src/main/java/deob/ObaInit.java:57) | software |
| 1 | `SceneDebugging.opSceneDebugging1` returns `MidiDecoder` (builds/rs2/614-2010-07-13/src/src/main/java/deob/SceneDebugging.java:11). `MidiDecoder` imports `jaggl.OpenGL` (builds/rs2/614-2010-07-13/src/src/main/java/deob/MidiDecoder.java:5) | platform |
| 2 | `SpriteShader.opSpriteShader4` | unread |
| 3 | `Unused24.call8e8bc5c7` | unread |
| 4 | `ObaInit.opObaInit11(..., true)` returns `Cp1252` | software |
| 5 | `DiskStore.opDiskStore4` loads native library `jaggl` and constructs `jaggl.OpenGL` (builds/rs2/614-2010-07-13/src/src/main/java/deob/DiskStore.java:42) | platform |

The default build area is 104 by 104 (builds/rs2/614-2010-07-13/src/src/main/java/deob/SoftwareToolkitFactory.java:14). Rebuild replaces it from the map-size index. The software pixel loops inside `VertexDeclarationElementComponent` and `Cp1252`, and the camera and terrain draw methods, were not read. HD entry points are the toolkit ids 1 and 5 above, plus the packages `jaggl` and `jagdx`. See `gaps.md`.
