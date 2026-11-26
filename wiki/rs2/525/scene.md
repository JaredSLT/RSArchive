# Scene

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`.

No source file mentions `jaggl`, `jagdx`, `jagtheora`, OpenGL, or Direct3D. There is no platform-native file in this tree.

The toolkit factory is `PathingEntity_Class147.opkind6041` (`PathingEntity_Class147.java:53-63`), called from `ProjAnim.java:591`. It does `Class.forName("rj")` and `newInstance`. On any throwable it builds `Ced8d3620`. `rj` is not a class in this source.

`Ced8d3620` extends `DeVotreListeWamis` and implements `ImageProducer` and `ImageObserver` (`Ced8d3620.java:14`). `DeVotreListeWamis` is the abstract toolkit (`DeVotreListeWamis.java:7`). `FluType` extends `VeuillezPatienterTentativeDeBtablissement` (`FluType.java:3`). `TextureOpTiledSprite` and `AnimFrame` (which extends `CollisionMap`) are the other large software classes. Their draw methods were not read.

Camera angles that the client sends are the pair stored by S2C 124 and written by C2S 254. See [input.md](input.md). Plane during player sync is the 2-bit field in local movement type 3, stored in `C0236c255.valuekind49771`. See [sync.md](sync.md). Terrain tiles and locs come from the region rebuild. See [region.md](region.md).

The draw loop inside `FluType` and the model renderer was not transcribed. See [gaps.md](gaps.md).
