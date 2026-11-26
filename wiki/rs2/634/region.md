# Region

Rebuild packet opcode 19, size -2, `C8d2c3755.valuekind16673` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C8d2c3755.java:5). In-game handler `Cd0f64cd0.opkind14622` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C4d106383.java:1549, builds/rs2/634-2010-12-10/src/src/main/java/deob/Cd0f64cd0.java:24). The same reader runs from lobby login state 10 when the packet is not opcode 70 (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:429).

## Header

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | u8_sub128 | map-size index |
| 2 | u16le_add128 | zone z |
| 3 | u16le | zone x |
| 4 | u8_neg | force rebuild when the value is 1 (`~read == -2`) |

`C8bcadc03.opkind14733` applies the size index when it differs from `C1b4ca309.valuekind17072` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C8bcadc03.java:34). Both map width `C2f8bd213.valuekind13905` and map height `C5f11c418.valuekind11934` become `Caeac0dcd.valuekind12105[index]`. The table is `{104, 120, 136, 168}` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Caeac0dcd.java:7). The fields start at 104 (builds/rs2/634-2010-12-10/src/src/main/java/deob/C2f8bd213.java:9, builds/rs2/634-2010-12-10/src/src/main/java/deob/C5f11c418.java:4).

## XTEA

Key count is `(payloadSize - bytesConsumed) / 16` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cd0f64cd0.java:32). Each group is four `u32be` keys in `C196f937d.valuekind16281[n][4]` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cd0f64cd0.java:35).

Squares are visited x outer, z inner:

- x from `(-(mapWidth >> 4) + zoneX) / 8` through `(zoneX + (mapWidth >> 4)) / 8`
- z from `(zoneZ - (mapHeight >> 4)) / 8` through `(zoneZ + (mapHeight >> 4)) / 8`

(builds/rs2/634-2010-12-10/src/src/main/java/deob/Cd0f64cd0.java:58). For the default 104, `map >> 4` is 6.

Map archive is JS5 index 5, `C7a61cd25.valuekind11891` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:135). File ids, in order, are the group ids of these names:

| Name | Slot |
| --- | --- |
| `m{x}_{z}` | `Cabbfa20d.valuekind16912` |
| `l{x}_{z}` | `ProjAnim.valueProjAnim90` |
| `um{x}_{z}` | `Cb41e23d2.valuekind16776` |
| `ul{x}_{z}` | `Cc9dc9486.valuekind12028` |

Square key `C56ca4e51.valuekind12172[i] = z + (x << 8)` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cd0f64cd0.java:60).

## Scene origin

`C88025b40.opkind14454(zoneZ, 123, zoneX, 11, force)` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cd0f64cd0.java:69). When the zone changed, or the force flag is set, or the build area differs from the stored one unless the preference forces area 0:

```text
baseX = (zoneX - (mapWidth >> 4)) * 8
baseZ = (zoneZ - (mapHeight >> 4)) * 8
```

Stored as `C7d96227f.valuekind12381` and `Ca5bf8f3f.valuekind13813` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C88025b40.java:92). When the fourth argument is 11, NPC tile coordinates and fine coordinates are shifted by the change in base (builds/rs2/634-2010-12-10/src/src/main/java/deob/C88025b40.java:100). The in-game rebuild passes 11.
