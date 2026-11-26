# Region

`REBUILD_NORMAL` (opcode 89, size -2) and `REBUILD_REGION` (opcode 153, size -2) copy the payload and call `World.opWorld131`. The rebuild methods read the fields.

## REBUILD_NORMAL

`World.rebuildNormalMap` (builds/rs2/742-2012-11-07/src/client/src/main/java/World.java:452):

1. `u8_sub128` build-area size (`g1_alt1`), passed to `BuildAreaSize.forId`
2. `u16be` zone origin X (`g2`)
3. `u8_sub128` force-rebuild flag, true when the value is 1
4. `u16be_sub128lo` zone origin Z (`g2_alt2`)
5. `(remaining bytes) / 16` groups of four `u32be` XTEA keys (`g4s`)

Map squares run from `(originX - (mapSizeX>>4)) / 8` through `(originX + (mapSizeX>>4)) / 8`, and the same for Z. `mapSizeX` is `BuildAreaSize.valueBuildAreaSize202` from `forId`. This client does not hardcode 104 or the constant 6. File names from `opWorld141` (builds/rs2/742-2012-11-07/src/client/src/main/java/World.java:684), archive `Js5Archive.MAPS` id 5 (`Statics.valueStatics385`): `m{x}_{z}` when the first argument is true, otherwise `l{x}_{z}`. A true second argument prefixes `u`.

`opWorld137` (builds/rs2/742-2012-11-07/src/client/src/main/java/World.java:730) sets the scene base to `CoordGrid(0, (originX - (mapSizeX>>4)) * 8, (originZ - (mapSizeZ>>4)) * 8)`.

## REBUILD_REGION

`World.rebuildRegionMap` (builds/rs2/742-2012-11-07/src/client/src/main/java/World.java:494):

1. `u16be_sub128lo` origin Z
2. `u8` info. Bit 0 is force-rebuild
3. `u16be` origin X
4. `u8` build-area size
5. `u8_neg` type (`g1_alt2`): 1 `REBUILD_REGION`, 2 `valueRebuildType10`, 3 `valueRebuildType13`, 4 `valueRebuildType14`
6. `bits()`. For level 0..3, for x in `mapSizeX>>3`, for z in `mapSizeZ>>3`: `gBit(1)`, and if that bit is 1, `gBit(26)`, else the chunk is -1. Then `bytes()`
7. the same `(remaining)/16` groups of four `u32be` keys

A 26-bit chunk is `mapzoneX = bits >> 14 & 0x3FF`, `mapzoneZ = bits >> 3 & 0x7FF`. The square id is `(mapzoneX/8<<8) + (mapzoneZ/8)`. The same `m` / `l` names are requested for that square.

The four `u32be` keys are stored on `valueWorld237`. The call that decrypts the map file with those keys was not read. Terrain and loc bytes inside the map files were not read. `MapLoader` / `ClientMapLoader` stay off this page for that reason.
