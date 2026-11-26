# Scene

This client draws in software. The source has no `jaggl`, `jagdx`, or `jagtheora` type. `files.tsv` has no `platform-native` row. `sun/audio/AudioPlayer.java` is an 18-line stub and is classified `platform-audio`.

State 30 draws through `MonotonicClock.opMonotonicClock43`, which calls `C27064abc.opkind1836` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:112`). `opkind1836` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/C27064abc.java:126`) projects the local player and the other players. When `Ignorieren.valuekind1731` is clear it builds the camera from `SoundBank.valueSoundBank14`, clamped by `Component.valueComponent166 / 256`, and from yaw `Mitteilung.valuekind1864 + Cc317a406.valuekind1791` masked with 2047, then calls `Cd66098f9.opkind1309`. A cutscene takes the camera level from `WallDecor.opWallDecor4`. The normal path uses `C353edd75.opkind1783`.

Five slots in `AddFriend.valuekind1897` add a sine shake to camera X, Z, height, yaw, and pitch. Pitch is then clamped to 128..383 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/C27064abc.java:161`).

`PathingEntity_Class147.callef6815b5` is the software raster (`builds/rs2/443-2006-12-11/src/src/main/java/deob/C27064abc.java:198`). The world is `AddFriend.valuekind1896`, constructed as `new MaterialManager_Class106_Sub2(4, 104, 104, ...)` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cb5815139.java:34`). `MaterialManager_Class106_Sub2` is a plain Java class (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MaterialManager_Class106_Sub2.java:3`). Its draw method is `callf04f6806` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MaterialManager_Class106_Sub2.java:792`), called with the shaken camera and the level from the branch above (`builds/rs2/443-2006-12-11/src/src/main/java/deob/C27064abc.java:199`).

`Model.java` is the model class. Its face and vertex layout was not read. `SoftwareModel.java` is a 13-line abstract stub. Terrain tile heights and the model draw loop are in [gaps.md](gaps.md).
