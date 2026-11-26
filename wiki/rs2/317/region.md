# Region

There is no XTEA in this source tree. `rslib/src/both/move/rev_317.c` is absent. This client builds the scene from land and loc files, not from a move layout.

## Rebuild packets

Opcode 73, size 4, `Client.read` at `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:12243`.

`u16be_add` zoneX, `u16be` zoneZ. `sceneInstanced` is false.

Opcode 241, size `-2`, at `Client.java:12249`.

`u16be_add` zoneZ. Then `accessBits`. For each of 4 levels, 13 by 13 tiles: `gBit(1)`. If that bit is 1, `gBit(26)` is stored, otherwise the cell is -1. Then `accessBytes`, `u16be` zoneX. `sceneInstanced` is true.

If the center zones are unchanged and `sceneState` is already 2, the packet is ignored. Otherwise:

`sceneBaseTileX = (zoneX - 6) * 8` and `sceneBaseTileZ = (zoneZ - 6) * 8`. `Client.java:12278`.

`withinTutorialIsland` is set when `zoneX / 8` is 48 or 49 and `zoneZ / 8` is 48, or when `zoneX / 8` is 48 and `zoneZ / 8` is 148.

`sceneState` becomes 1. NPCs, players, object stacks, temporary locs, and the destination flag are shifted by the change in base tiles. `awaitingSync` becomes true. `Client.java:12385`.

## Map file ids

`versionlist` member `map_index` is `length / 7` records of `u16be` map square, `u16be` land file, `u16be` loc file, `u8` prefetch. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/io/OnDemand.java:242`.

`getMapFile(kind, mapZ, mapX)` looks up `(mapX << 8) + mapZ`. Kind 0 returns the land file. Any other kind returns the loc file. `OnDemand.java:580`.

A normal rebuild requests squares from `(center - 6) / 8` through `(center + 6) / 8` on both axes. The stored index is `(mapX << 8) + mapZ`. Tutorial squares with `mapZ` 49, 149, or 147, or `mapX` 50, or `mapX` 49 and `mapZ` 47, are skipped. Land is `getMapFile(0, mapZ, mapX)` and loc is `getMapFile(1, mapZ, mapX)`, both requested from ondemand archive 3. `Client.java:12311`.

An instanced rebuild collects unique `(srcMapX / 8 << 8) + (srcMapZ / 8)` from the 26-bit cells and requests the same way. The 26-bit value is later split in `buildScene` as level `(v >> 24) & 3`, rotation `(v >> 1) & 3`, mapX `(v >> 14) & 1023`, mapZ `(v >> 3) & 2047`. `Client.java:1414`.

`checkScene` waits until every requested land and loc byte array is present, `World.validateLocs` accepts the loc bytes, and `awaitingSync` is false. It then sets `sceneState` to 2, calls `buildScene`, and writes client opcode 121 with no payload. `Client.java:3501`.

## Land file

`World.readLand` walks 4 levels of 64 by 64 and calls `readLandInner` for tiles inside the 104-wide scene. `World.java:1308`. `readLandInner` at `World.java:1337` reads `u8` opcodes until the tile ends.

| Opcode | Meaning |
| --- | --- |
| 0 | End. Level 0 height is `-perlin(932731 + x + originX, 556238 + z + originZ) * 8`. A higher level copies the level below and subtracts 240. |
| 1 | `u8` height. A value of 1 is treated as 0. Level 0 stores `-byte * 8`. A higher level subtracts `byte * 8` from the level below. End. |
| 2..49 | `i8` overlay id. Shape is `(opcode - 2) / 4`. Rotation is `(opcode - 2 + mapRotation) & 3`. |
| 50..81 | Tile flag is `opcode - 49`. |
| 82..255 | Underlay id is `opcode - 81`. |

Tiles outside 0..103 still consume the same stream and discard the extra bytes. The body of `perlin` is unread; see `gaps.md`.

## Loc file

`World.readLoc` at `World.java:1493` and `readLocRegion` at `World.java:1416` share the stream:

`gsmarts` id delta. Zero ends the file. The running id starts at -1 and adds each delta. Then, repeatedly, `gsmarts` position delta. Zero ends that loc. The running position starts at 0 and adds `delta - 1`. The low 6 bits are local z, the next 6 bits are local x, and the high bits are the level. Then `u8` packs shape in the top 6 bits and rotation in the low 2 bits.

`readLoc` places the loc when the tile is inside 1..102. If level-1 tile flags contain bit 2, the collision level is one lower. `readLocRegion` keeps only locs whose level equals the requested level and whose local tile sits in an 8 by 8 window. Placement uses `WorldRegion.rotateLocX` and `rotateLocZ`, and the rotation is `(packetRotation + regionRotation) & 3`.

`rotateLocX(rotation, length, x, z, width)` and `rotateLocZ(z, length, rotation, width, x)` are at `WorldRegion.java:42` and `WorldRegion.java:58`. Rotation 0 returns the original axis. Rotation 1 swaps axes. Rotation 2 mirrors both. Rotation 3 mirrors the other axis and accounts for width and length.

## Zone base

Opcode 85, size 2: `u8_neg` baseZ, then `u8_neg` baseX. `Client.java:12860`.

Opcode 64, size 2: `u8_neg` baseX, then `u8_sub` baseZ. It clears object stacks in that 8 by 8 area. `Client.java:12071`.

Opcode 60, size `-2`: `u8` baseZ, `u8_neg` baseX, then `u8` zone opcodes until the payload ends. Each opcode is passed to `readZonePacket`. `Client.java:12504`.

The same zone opcodes also arrive as their own packets and are dispatched at `Client.java:13223`. The coordinate byte is `(dx << 4) | dz`, added to `baseX` and `baseZ`.

`LOC_LAYER` is `{0,0,0,0, 1,1,1,1,1, 2,2,2,2,2,2,2,2,2,2,2,2,2, 3}`. Shapes 0..3 are walls, 4..8 are wall decor, 9..21 are scenery, and 22 is ground decor. A loc animation with shape 11 is treated as shape 10. `Client.java:823`, `Client.java:11460`.

Zone payload fields are in [packets.md](packets.md).
