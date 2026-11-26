# Scene

## Canvas

`Client.init` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:568`) stores width and height multipliers, then `PkuuBuUpZmoXq.opkind248617` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/PkuuBuUpZmoXq.java:386`) reads them.

- Worlds `SipRx.valuekind27221`, `C397ef5ae.valuekind29453`, and `Ce5cb4be1.valuekind30345` set `Aqn.valuekind25992 = -715556227` and `ZiCc.valuekind25113 = 1451317229` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:574`). Those are also the static defaults (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Aqn.java:126`, `builds/rs2/695-2011-12-22/src/src/main/java/deob/ZiCc.java:92`).
- World `Fic.valuekind28293` sets `Aqn.valuekind25992 = 1731314304` and `ZiCc.valuekind25113 = 684781216` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:570`).
- Any other world leaves the defaults.

`var35 = Aqn.valuekind25992 * 2108068481` and `var36 = ZiCc.valuekind25113 * 223755507` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:583`). Those are stored as `var35 * -277205841` and `var36 * 2036125945`. `C1dafaa2f.opkind23395` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C1dafaa2f.java:77`) returns `-1436505521 * ((int[])obj[1])[0]` when its guard is 0. That chain is 765 on the default path and 640 on the Fic path. Height `((int[])obj[1])[3] * 62845257` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/PkuuBuUpZmoXq.java:391`) is 503 on the default path and 480 on the Fic path.

Default canvas: 765 by 503. Fic canvas: 640 by 480.

## Software path

`PureJavaModel` extends `Model` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/PureJavaModel.java:3`). `PureJavaTexture` extends `C94e4a28a` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/PureJavaTexture.java:3`). `Font` is `builds/rs2/695-2011-12-22/src/src/main/java/deob/Font.java:3`. The methods that draw terrain, models, and sprites were not read.

## HD entry points (platform)

`GlModel` extends `Model` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/GlModel.java:6`). `GpuModel` extends `Model` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/GpuModel.java:6`). Package `jaggl` is the OpenGL binding. Package `jagdx` is the Direct3D binding. Both are platform-native in `files.tsv`. `C0350f28b` extends `SxTd` and implements `VarRelated1` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C0350f28b.java:5`). It lives under `deob`, so it is classified rather than marked platform-native. Its native calls were not read.

## Tick

`PkuuBuUpZmoXq.opkind248642` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/PkuuBuUpZmoXq.java:450`) stores the current time in a 32-slot ring (`Egp.valuekind20775` masked with 31) and calls `Client.opClient258` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:115`). The body of `opClient258` was not read. Tick order is unread.
