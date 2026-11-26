# Region

Static rebuild is S2C opcode 10, size -2, `StaticPointLight.valueStaticPointLight15`. The switch calls `C0bd9f077.opkind11111(false)` and returns false from the packet poll (builds/rs2/614-2010-07-13/src/src/main/java/deob/C0bd9f077.java:8).

| order | width | meaning |
| --- | --- | --- |
| 1 | u16(lo-128, hi) | zoneZ (`calldb3c5e12`) |
| 2 | u8(b-128) | map-size index |
| 3 | u8 | 1 means force rebuild |
| 4 | u16le | zoneX (`opUnused6164`) |
| 5 | u32be x 4, repeated | XTEA key. Group count is `(payloadLength - bytesRead) / 16` |

`PingProvider.opPingProvider2` sets both scene width and scene height from `PureJavaVertexNormal.valuePureJavaVertexNormal10[index]` (builds/rs2/614-2010-07-13/src/src/main/java/deob/PingProvider.java:15). The default build length is 104 (builds/rs2/614-2010-07-13/src/src/main/java/deob/SoftwareToolkitFactory.java:14).

Map squares run x from `(zoneX - (sceneWidth >> 4)) / 8` through `(zoneX + (sceneWidth >> 4)) / 8`, and z from `(zoneZ - (sceneHeight >> 4)) / 8` through `(zoneZ + (sceneHeight >> 4)) / 8`. Names on JS5 index 5 (`DaaInit.valueDaaInit2`) are `m{x}_{z}`, `l{x}_{z}`, `um{x}_{z}`, and `ul{x}_{z}` (builds/rs2/614-2010-07-13/src/src/main/java/deob/C0bd9f077.java:38).

`QuickChatDynamicProvider.opQuickChatDynamicProvider4(zoneZ, force, 1, zoneX, 11)` (builds/rs2/614-2010-07-13/src/src/main/java/deob/QuickChatDynamicProvider.java:34) sets the scene origin:

`baseX = 8 * (zoneX - (sceneWidth >> 4))`

`baseZ = 8 * (zoneZ - (sceneHeight >> 4))`

`baseX` is `Vector3i.valueVector3i1`. `baseZ` is `HashMap.valueHashMap10`. With a build length of 104, `>> 4` is 6. The client uses the shift, not a hardcoded 6.

Dynamic rebuild is S2C opcode 26, size -2, `RayTracing.valueRayTracing9`. The switch calls `GpuRelated13.opGpuRelated133(false)` and returns false (builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuRelated13.java:55). The login path uses the same method when the first world opcode is 26 (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:489).

| order | width | meaning |
| --- | --- | --- |
| 1 | u8(128-b) | map-size index |
| 2 | u8neg | 1 means force rebuild |
| 3 | u16be | zoneZ |
| 4 | u16be | zoneX |
| 5 | u8neg | stored in `ProgramData.valueProgramData55` |
| 6 | bits | for 4 planes times `(sceneWidth >> 3)` times `(sceneHeight >> 3)`: 1 bit, and if that bit is 1 a 26-bit region id, otherwise -1 |
| 7 | u32be x 4, repeated | XTEA for `(remaining bytes) / 16` groups |

A region id becomes a map square `(id >> 14 & 1023) / 8` and `(id >> 3 & 2047) / 8`, with the same four names on index 5. The origin call is `opQuickChatDynamicProvider4(zoneZ, force, 1, zoneX, 11)`.

XTEA decrypt of a buffer is `Unused6.call9320132b` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:562): two u32be words, 32 rounds, delta -1640531527, sum starting at -957401312.
