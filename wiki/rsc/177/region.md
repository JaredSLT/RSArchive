# Region

There is no region packet and no XTEA. The server opcode 255 coordinates are world tiles. The client loads jag chunks itself when those tiles leave the loaded window.

`MudClient.call8390e4c9` in the Java source returns false and has no body (`deob/MudClient.java:9670`). The body below was read from `deob.MudClient.fn8390e4c9` in `builds/rsc/177-2003-10-31/deob/gamepack-deob.jar` (javap of that method). Opcode 255 calls it with the 11-bit and 13-bit coordinates before subtracting the scene origin (`deob/MudClient.java:7624`).

## When the map reloads

1. If `anInt511 != 0`, set `World.flag55` false and return false. No load.
2. Clear `flag99`.
3. Add `number406` to x and `number407` to y. Opcode 244 writes those fields (`deob/MudClient.java:6693`).
4. If `number409 == number412` and x is strictly inside `number413..number415` and y is strictly inside `number414..number416`, set `flag55` true and return false.
5. Otherwise draw `Loading... Please wait`, then:
   - `sectorX = (x + 24) / 48`
   - `sectorY = (y + 24) / 48`
   - `number409 = number412` (plane)
   - origin `number410 = sectorX * 48 - 48`
   - origin `number411 = sectorY * 48 - 48`
   - window `number413 = sectorX * 48 - 32`, `number415 = sectorX * 48 + 32`
   - window `number414 = sectorY * 48 - 32`, `number416 = sectorY * 48 + 32`
6. `World.call4fc32bcd(x, y, plane)` (`deob/World.java:1275`).
7. Subtract `number406` from `number410` and `number407` from `number411`.
8. Shift loc, wall, and ground-item tiles by `newOrigin - oldOrigin`. Shift players and NPCs and their waypoints by that delta times 128.
9. Set `flag55` true and return true.

The login scene, before a live opcode 255, loads sector 50, 50, plane 0: `call4fc32bcd(50 * 48 + 23, 50 * 48 + 23, 0)` (`deob/MudClient.java:2297`). The client stores no spawn tile of 100, 200.

## Terrain entry

`call4fc32bcd` (`deob/World.java:1275`):

- `sector = (x + 24) / 48`, `(y + 24) / 48`
- `call9329ebf4(x, y, plane, true)` builds the 96 by 96 mesh. That method starts at `deob/World.java:1359`. Its per-tile type cases were not enumerated. See [gaps](gaps.md).
- If plane is 0, it also calls `call9329ebf4` for planes 1 and 2 with the flag false, then four neighbor chunks, then `call36d34db0` (`deob/World.java:1293`).

The four chunk calls are `call17103888(sectorX - 1, sectorY - 1, plane, 0)` through `(sectorX, sectorY, plane, 3)`. The Java method is empty (`deob/World.java:1231`). The filename and height decode below are from `deob.World.fn17103888` in the same deob jar.

## Chunk files

Name `m` + plane + `(x / 10)` + `(x % 10)` + `(y / 10)` + `(y % 10)`.

When the land jag `byteValueArray20` is non-null:

- Read `name.hei` from `land{MAPS}.jag` (`byteValueArray20`). If missing, try `land{MAPS}.mem` (`byteValueArray22`).
- `.hei` expands to 2304 bytes (48 by 48). A byte below 128 is stored and becomes the repeat value. A byte at least 128 repeats the previous value `(byte - 128)` times.
- Then a 48 by 48 pass: running sum starts at 64, each cell becomes `(sum + cell) & 127`, and the stored byte is `sum * 2`.
- A second RLE fills the other height layer. Its blur seed is 35, not 64.
- If there is no `.hei`, both height layers are left zero. The bytecode path for that zero fill was not walked line by line. See [gaps](gaps.md).
- `name.dat` is required from `maps{MAPS}.jag` (`byteValueArray21`), else members `maps{MAPS}.mem` (`byteValueArray23`). Missing `.dat` throws. The byte layout inside `.dat` was not read. Widths are unread.
- `name.loc` is optional, from the maps jag. Its record layout was not read. Widths are unread.

When the land archive is null, the bytecode loads `../gamedata/maps/name.jm` (`fn17103888` string `.jm`). That decode was not read. Widths are unread.

Archive names and version numbers are in [cache](cache.md). `Version.MAPS` is 63 (`deob/Version.java:9`).

## Opcode 240 clear

`(payloadLength - 1) / 4` records of two i16be, added to `number428` and `number429`, then shifted right 3. Items, locs, and bounds in that sector are removed (`deob/MudClient.java:6764`).

## Ground objects

Opcodes 254, 253, and 249 loop until the cursor reaches the framed length. A leading u8 of 255 clears one sector: the next two bytes are added to the region coordinates and the sum is shifted right 3 (`deob/MudClient.java:5777`, `deob/MudClient.java:5886`, `deob/MudClient.java:6308`).

Otherwise:

| opcode | record | delete id |
| --- | --- | --- |
| 254 items | u16be id, i8 dx, i8 dy. Tile is region plus the byte. High bit clear adds the id. High bit set (`& 0x8000`) clears that bit and removes the matching tile and id (`deob/MudClient.java:5809`). | high bit |
| 253 locs | u16be id, i8 dx, i8 dy. Rotation comes from `World.call433`, not from the packet (`deob/MudClient.java:5925`). | 60000 |
| 249 bounds | u16be id, i8 dx, i8 dy, i8 direction (`deob/MudClient.java:6348`). | 65535 |
