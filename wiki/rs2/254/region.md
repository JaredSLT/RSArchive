# Region

Rebuild is server opcode 209, size 4 (`Client.java:3824`). The payload is u16be zone x, then u16be zone z. If both zones match `sceneCenterZoneX` / `sceneCenterZoneZ` and `sceneState` is already 2, the handler returns. Otherwise:

- `sceneBaseTileX = (zoneX - 6) * 8` and `sceneBaseTileZ = (zoneZ - 6) * 8` (`Client.java:3834`).
- Tutorial island is set when `zoneX / 8` is 48 or 49 and `zoneZ / 8` is 48, or when `zoneX / 8` is 48 and `zoneZ / 8` is 148 (`Client.java:3837`).
- Map squares run from `(zone - 6) / 8` through `(zone + 6) / 8` on both axes (`Client.java:3853`).
- The packed index is `(mapX << 8) + mapZ` (`Client.java:3868`).

`OnDemand.getMapFile` (`builds/rs2/254-2004-09-07/src/src/main/java/jagex2/io/OnDemand.java:167`) looks that index up in `mapIndex`. Kind 0 returns `mapLand`. Kind 1 returns `mapLoc`. The call passes `valueClient19`, which is -62 (`Client.java:113`), so a missing square returns -1. Requests use archive 3 (`Client.java:3872`).

Squares inside the tutorial-island exclusion (`Client.java:3869`) store file id -1 for both land and locs. After the file list is built, NPC and player routes and coordinates are shifted by the change in scene base, and obj stacks are copied across the 104 by 104 by 4 world (`Client.java:3894`). `awaitingSync` is set. The rebuild does not finish the scene until a later player update clears that flag (`checkScene` returns -4 while it is set, `Client.java:1622`).

No method in this tree reads XTEA keys. Opcode 241 is size 4 in `Protocol.SERVERPROT_LENGTH` and has no handler.

## Land

`ClientBuild.loadGround` (`ClientBuild.java:461`) reads 4 levels of 64 by 64 tiles. Each tile is a run of u8 opcodes until 0 or 1:

| Opcode | Effect |
| --- | --- |
| 0 | Stop. Height is Perlin noise on level 0, otherwise the level below minus 240. |
| 1 | Stop. Next u8 is the height. A value of 1 is treated as 0. Level 0 stores `-value * 8`. A higher level stores the level below minus `value * 8`. |
| 2..49 | Next byte is a signed overlay id (`g1b`). Shape is `(opcode - 2) / 4`. Rotation is `(opcode - 2) & 3`. |
| 50..81 | Collision flag `opcode - 49`. No extra byte. |
| 82 and above | Underlay id `opcode - 81`. No extra byte. |

Tiles outside 0..103 still consume the same opcodes so the stream stays aligned.

## Locs

`ClientBuild.loadLocations` (`ClientBuild.java:535`): `gsmarts` id delta until 0, then for that id `gsmarts` position delta until 0. The position is `level << 12 | x << 6 | z` after adding the delta minus 1. Then one u8: shape is `>> 2`, rotation is `& 3`. `gsmarts` is `g1` when the next byte is below 128, otherwise `g2() - 32768` (`Packet.java:356`).

## Scene ready

When every requested land and loc buffer is present, locations pass `ClientBuild.checkLocations`, and `awaitingSync` is clear, `sceneState` becomes 2 and the client writes C2S opcode 134 with no body (`Client.java:1625`). `buildScene` (`Client.java:11840`) writes C2S opcode 239 with no body four times while it loads land, loads locs, finishes the build, and sorts obj stacks.
