# Region

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`.

Opcodes 222 and 53 share one branch (`jagex2/client/Client.java:3342`). Neither packet contains XTEA keys. A search of this client finds no XTEA.

## Opcode 222

Table size 4. `g2` zone z into `valueClient1022`, `g2_alt3` zone x into `valueClient1021`, `valueClient1289 = false` (`Client.java:3345`).

Scene origin:

`valueClient1170 = (zoneX - 6) * 8`

`valueClient1171 = (zoneZ - 6) * 8`

(`Client.java:3380`).

Map squares run from `(zone-6)/8` through `(zone+6)/8` inclusive on both axes (`Client.java:3410`). `valueClient1196` is set when `(zoneX/8` is 48 or 49 and `zoneZ/8` is 48) or (`zoneX/8` is 48 and `zoneZ/8` is 148) (`Client.java:3383`). A square is left unloaded when that flag is set and `z` is 49, 149, or 147, or `x` is 50, or (`x` is 49 and `z` is 47) (`Client.java:3413`).

`OnDemand.getMapFile(x, z, 0)` returns the land file id. Kind 1 returns the loc file id. Otherwise -1 (`jagex2/io/OnDemand.java:793`). The client requests archive 3 (`Client.java:3416`).

## Opcode 53

Table size `-2`. `g2_alt2` zone x, `accessBits`, then levels 0..3, x 0..12, z 0..12: `gBit(1)` and, when set, `gBit(26)`, else -1. Then `accessBytes`, `g2_alt2` zone z, and `valueClient1289 = true` (`Client.java:3351`). The same `(zone-6)*8` origin is used.

The 26-bit value is split in `buildScene` (`Client.java:8967`):

| field | shift |
| --- | --- |
| level | `>> 24 & 3` |
| rotation | `>> 1 & 3` |
| land x | `>> 14 & 1023` |
| land z | `>> 3 & 2047` |

Unique map squares are `(landX/8 << 8) + (landZ/8)` (`Client.java:3443`). Land bytes go through `World.opWorld55`. Loc bytes go through `World.opWorld58`.

## Zone base

Opcode 75, size 2: `g1_alt2` base x, `g1_alt1` base z (`Client.java:3726`). Opcode 40, size 2: `g1_alt3` base z, `g1_alt2` base x (`Client.java:3116`). Opcode 183, size `-2`, sets `g1` base x and `g1_alt1` base z, then reads `u8` sub-opcodes while `pos < size` and calls `readZonePacket` (`Client.java:3201`). The same sub-opcodes are also legal as top-level S2C opcodes (`Client.java:3577`). A top-level zone packet uses the base already stored. It does not read a new base.

Loc shape class `valueClient1162` is `{0,0,0,0,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,3}` (`Client.java:792`).

## Terrain and locs

`buildScene` (`Client.java:8905`) clears the scene, calls `Pix3D.init2D`, resets collision, and builds `World(104, 104)`. It sends empty C2S 40 before the load and between the land and loc passes (`Client.java:8930`, `Client.java:8950`, `Client.java:9000`, `Client.java:9025`, `Client.java:9032`).

Non-instanced land uses `World.opWorld60` (`jagex2/dash3d/World.java:1372`): four levels of 64 by 64 tiles, clearing collision bit `0x1000000`, then `readLandInner` per tile. When land bytes are missing and zone z is below 800, `stitchHeightmap` fills 64 by 64 (`Client.java:8945`). Loc bytes use `World.readLoc` (`World.java:1422`).

`readLandInner` (`World.java:1527`) reads `u8` opcodes until 0:

| opcode | meaning |
| --- | --- |
| 0 | height from `perlin`, or 240 below the level under it |
| 1 | `u8` height, 1 becomes 0, times -8 relative to the level under it |
| 2..49 | `i8` overlay shape; rotation is `(opcode-2)/4`; extra rotation is `(opcode-2+origin) & 3` |
| 50..81 | underlay flag `opcode-49` |
| 82 and above | floor overlay `opcode-81` |

`readLoc` (`World.java:1426`): `gsmarts` loc-id delta until 0, then `gsmarts` position delta until 0. The position is `level = pos>>12`, `x = pos>>6 & 63`, `z = pos & 63`, then `u8` with type `>> 2` and rotation `& 3`.

When the map finishes loading, the client sends empty C2S 6 (`Client.java:13419`).

`opWorld55`, `opWorld59`, `perlin`, and `stitchHeightmap` are named from their signatures and are not expanded. See [gaps.md](gaps.md).
