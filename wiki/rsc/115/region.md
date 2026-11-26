# Region

There is no region opcode and no XTEA. The map load is `MudClient.call8390e4c9`, called from server opcode 255 after the local 10-bit x and 12-bit y (`deob/MudClient.java:2847`, method at `MudClient.java:4986`).

Opcode 244 stores five u16be values and then `number93 -= menuAction * number94` (`MudClient.java:3507`). The next position packet adds `number92` to x and `number93` to y before the sector math (`MudClient.java:4992`). After the load, both origins are reduced by those same offsets (`MudClient.java:5013`). `menuAction` is copied to `errorLoading` and is the plane passed to `World.call4fc32bcd`.

If `value250 != 0` the method sets `world.flag427 = false` and returns (`MudClient.java:4987`). If the plane is unchanged and the coordinate sits inside the stored window, it sets `flag427 = true` and returns (`MudClient.java:4994`).

## Sector

`sector = (coord + 24) / 48` (`MudClient.java:5003`).

| value | formula |
| --- | --- |
| `number414`, `number184` | `sector * 48 - 48` |
| window low `number484`, `number97` | `sector * 48 - 32` |
| window high `number226`, `number243` | `sector * 48 + 32` |

Scene local is the absolute tile minus that origin. Tile pixels are 128 (`MudClient.java:8360`). The scene grid is 96 by 96 (`deob/World.java:40`).

## Files

`World.call4fc32bcd(x, y, plane)` (`World.java:834`) clears models and calls `call9329ebf4`. When the plane is 0 it also calls `call9329ebf4` for planes 1 and 2 with the build flag false, reloads plane 0's four sectors, and calls `call36d34db0` (`World.java:839`).

`call17103888` (`World.java:736`) names the file `m` + plane + `sx / 10` + `sx % 10` + `sy / 10` + `sy % 10` + `.jm`, where `sx` and `sy` are sector indexes. Applet loads use `maps27.jag` through `Tools.readJag`. A local client sets `flag399 = false` (`MudClient.java:1027`) and reads `../gamedata/maps/` into a 20736-byte buffer.

The four chunks are `(sx - 1, sy - 1)`, `(sx, sy - 1)`, `(sx - 1, sy)`, `(sx, sy)` (`World.java:899`).

Each `.jm` has 2304 cells (`48 * 48`):

| layer | encoding |
| --- | --- |
| `byteValueArrayArray412` | running sum, masked to 8 bits |
| `byteValueArrayArray413` | running sum, masked to 8 bits |
| `byteValueArrayArray414` | raw byte |
| `byteValueArrayArray415` | raw byte |
| `numberArrayArray` | u16be |
| `byteValueArrayArray416` | raw byte |
| `byteValueArrayArray417` | raw byte |
| `byteValueArrayArray418` | raw byte |

2304 * 9 bytes is 20736. A missing file zeros the chunk. Plane 0 sets layer 417 to -6. Plane 3 sets layer 417 to 8 (`World.java:800`).

`call36d34db0` (`World.java:852`) walks the 96 by 96 grid. A cell whose plane-0 sample is 250 becomes type 9 on the edge case at coordinate 47 and type 2 otherwise.

`call9329ebf4` (`World.java:895`) loads those four sectors, and when its flag is true clears the 96 by 96 collision grid `numberArrayArray425` and starts the terrain mesh. The mesh body after the clear was not transcribed. See `gaps.md`.

## Path

`World.call3cf8018f` (`World.java:111`) returns the step count consumed by `MudClient.call4534748f`. The walk writer is in `input.md`. The search after the grid clear at `World.java:112` was not transcribed as a step list. See `gaps.md`.
