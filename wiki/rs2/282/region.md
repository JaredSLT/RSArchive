# Region

Citations are `builds/rs2/282-2004-12-21/src/src/main/java/deob/`.

## Rebuild packet

S2C opcode 231, size 4. The only packet bytes are `u16be` zone x and `u16be` zone z (`Client.java:6655`). If both match the current zone and `valueClient328` is already 2, the handler returns. There is no XTEA key in this packet.

Scene origin:

- `valueClient197 = (zoneX - 6) * 8`
- `valueClient198 = (zoneZ - 6) * 8`

Map squares run from `(zone - 6) / 8` through `(zone + 6) / 8` inclusive, on both axes. For each square the client requests terrain `OnDemand.opOnDemand3(0, rx, rz)` and locs `opOnDemand3(1, rx, rz)`, both from ondemand archive 3 (`Client.java:6701`).

Blackout `valueClient435` is set when `(zoneX / 8` is 48 or 49 and `zoneZ / 8` is 48) or (`zoneX / 8` is 48 and `zoneZ / 8` is 148). A square is skipped, with both file ids set to -1, when blackout is set and (`rz` is 49, 149, or 147, or `rx` is 50, or `rx` is 49 and `rz` is 47).

The scene is 104 by 104 tiles and 4 planes (`Client.buildScene` allocates `ClientBuild(104, 104)` at `Client.java:11893`). Heights live in `groundh[4][105][105]`. Collision flags live in `levelTileFlags[4][104][104]`.

`opClient42` waits until every terrain and loc buffer is present, checks models with `ClientBuild.opkind573`, then calls `buildScene` and writes C2S opcode 214 with an empty payload (`Client.java:4250`).

Per-square placement origin is `(packed >> 8) * 64 - valueClient197` and `(packed & 255) * 64 - valueClient198` (`Client.java:4236`).

## Map index

`OnDemand.load` reads `map_index` from the versionlist jag (`OnDemand.java:103`). Row count is `length / 7`. Each row is `u16be` packed coordinate (`x << 8 | z`), `u16be` terrain file id (type 0), `u16be` loc file id (type 1), `u8` members flag.

## Terrain file

`ClientBuild.opkind572` (`ClientBuild.java:482`) walks planes 0..3 and local 64 by 64. Each tile is a run of `u8` opcodes until 0 or 1:

| opcode | bytes after it | meaning |
| --- | --- | --- |
| 0 | none | plane 0 height from noise, else previous plane minus 240 |
| 1 | `u8` height | a height of 1 is treated as 0; plane 0 stores `-height * 8`, else previous minus `height * 8` |
| 2..49 | `i8` overlay id | shape `(opcode - 2) / 4`, rotation `(opcode - 2) & 3` |
| 50..81 | none | underlay byte is `opcode - 49` |
| 82.. | none | settings byte is `opcode - 81` |

Tiles outside 0..103 still consume the same opcode bytes and discard them.

## Loc file

`ClientBuild.opkind575` (`ClientBuild.java:556`) and the prefetch `opkind573` (`ClientBuild.java:35`) share the stream:

- `opkind2727` delta loc id, 0 ends the file
- `opkind2727` delta packed coordinate, 0 ends that loc's instances
- `u8`: high 6 bits are the type, low 2 bits are the rotation

Packed coordinate: plane is `>> 12`, local x is `>> 6 & 63`, local z is `& 63`. `opkind2727` is the smart in `framing.md`. `opkind576` places one loc and was not read past the lowmem plane skip at `ClientBuild.java:609`.

Zone packets that add and remove locs after the rebuild are in `packets.md`. Their base tile is set by opcodes 153, 195, and 32.
