# Region

There is no region opcode, no XTEA, and no zone field. The map reload is `MudClient.call8390e4c9`, called from s2c opcode 255 with the absolute tiles before those tiles are rebased (`MudClient.java:3181`, `MudClient.java:5459`).

## Opcode 244 offsets

`MudClient.java:3853-3861` sets `flag319` and reads five `u16be` values:

| Offset | Field |
| --- | --- |
| 1 | `number227`, the local player server index later passed to `calla598fe3b` |
| 3 | `number96`, added to the x tile before the sector split |
| 5 | `errorLoading`, added to the y tile, then reduced by `number97 * number414` |
| 7 | `number97`, stored as the plane `number184` |
| 9 | `number414` |

`errorLoading -= number97 * number414` is the last statement of the handler. These are not left at 0. `rslib/src/both/move/rev_rsc.c` says the offsets stay 0. The Java assignments are the ones above. See [gaps.md](gaps.md).

## Origin

`call8390e4c9` (`MudClient.java:5459-5487`):

1. If `value2502` is not 0, the method returns false and sets `world.flag470` false. `value2502` is a Java int with no constructor write, so it starts at 0. S2c opcode 241 sets it to 250 (`MudClient.java:3893`). The logged-in tick decrements it (`MudClient.java:2637-2638`). While it is nonzero, opcode 255 does not reload.
2. `tileX += number96` and `tileY += errorLoading`.
3. If `number184 == number97` and the tile is inside `(number226, number105)` by `(number243, number106)`, the method returns false.
4. Otherwise `sx = (tileX + 24) / 48` and `sy = (tileY + 24) / 48`. `number184 = number97`. `menuAction = sx * 48 - 48`. `number484 = sy * 48 - 48`. The loaded window is `sx * 48 - 32` through `sx * 48 + 32`, and the same for `sy`.
5. `World.call4fc32bcd(tileX, tileY, plane)` loads the sectors.
6. After the load, `menuAction -= number96` and `number484 -= errorLoading`.

When both offsets are 0 the origin is `((tile + 24) / 48) * 48 - 48`. The client does not store `origin / 8 + 6`. Scene-local tiles are the opcode 255 tiles minus `menuAction` and `number484`. Walk packets add those origins back. The terrain mesh is 96 by 96 tiles (`MudClient.java:5515`).

## Sector files

`World.call4fc32bcd` (`World.java:1018-1031`) computes the same `(tile + 24) / 48` pair and calls `call9329ebf4`. When the plane is 0 it also builds planes 1 and 2 without the object mesh, then loads four plane-0 sectors and calls `call36d34db0`. The four sectors are `(sx - 1, sy - 1)`, `(sx, sy - 1)`, `(sx - 1, sy)`, and `(sx, sy)`, stored in slots 0 through 3.

`World.call17103888` (`World.java:742`) names a sector `"m" + plane + (sx / 10) + (sx % 10) + (sy / 10) + (sy % 10)`. There is no numeric map file id.

Jag bytes are loaded in `MudClient.load` (`MudClient.java:1178-1185`):

| Archive | Field |
| --- | --- |
| `maps` + `Version.value50` + `.jag` | `world.byteValueArray452` |
| `maps` + 50 + `.mem` | `world.byteValueArray454`, members fallback |
| `land` + 50 + `.jag` | `world.byteValueArray451` |
| `land` + 50 + `.mem` | `world.byteValueArray453`, members fallback |

Each sector is 48 by 48, 2304 cells. The sector arrays `byteValueArrayArray455` through `458` are `[4][2304]` (`World.java` class header).

Height file `name.hei`, from land jag, then members land (`World.java:747-804`):

- One layer into `byteValueArrayArray455`. A byte below 128 is a literal and becomes the repeat value. A byte at least 128 repeats the previous value `byte - 128` times.
- The layer is then walked column-major: `acc = (acc + cell) & 127`, and the cell is replaced with `acc * 2`. The first accumulator is 64.
- A second layer into `byteValueArrayArray456` uses the same RLE. Its accumulator starts at 35 and the same `* 2` pass runs.

Scenery file `name.dat` (`World.java:812-911`), four `< 192` layers and three `< 128` layers, in order:

| Layer | Store |
| --- | --- |
| byte `< 192` literal, else skip `byte - 192` zeros | `byteValueArrayArray457` |
| same `< 192` rule | `byteValueArrayArray458` |
| `< 192` stores the byte as an int, else skip | `numberArrayArray` |
| `< 192` stores `byte + 12000`, else skip that many cells without writing | `numberArrayArray` |
| `< 128` literal else skip zeros | `byteValueArrayArray459` |
| `< 128` literal else repeat the previous byte | `byteValueArrayArray460` |
| `< 128` literal else skip zeros | `byteValueArrayArray461` |

Optional `name.loc`: a byte below 128 stores `byte + 48000` (`'뮀'`) into `numberArrayArray`. A byte at least 128 skips `byte - 128` cells (`World.java:913-925`).

If the land jag is missing, the method reads `../gamedata/maps/name.jm` as 20736 raw bytes and fills the same arrays without the RLE (`World.java:930-970`). A thrown `IOException` zeros the sector.

Tile size on the mesh is `World.value128` (128), the same scale as the entity pixel. While the terrain mesh is built, an overlay whose `ClientConfig.numberArray544` entry is nonzero sets collision bit 64, and an overlay whose `numberArray543` entry is 2 sets bit 128 (`World.java:1188-1196`). A loc whose `numberArray526` entry is 1 sets bit 64 on its footprint (`World.java:343-345`). `MudClient.call77329c68` rejects a camera step when the tested cell has bit 128 set (`MudClient.java:4518-4540`).
