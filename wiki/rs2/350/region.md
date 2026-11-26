# Region

Rebuild is S2C opcode 152, size 4, and instanced rebuild is opcode 20, size `-2`. Both share the block at `Client.java:4112` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java`).

## Opcode 152

Two `u16le+128` values (`opkind8743(valueClient615)`):

1. Zone Z, stored in `valueClient518`.
2. Zone X, stored in `valueClient517`.

`valueClient655` is set false. There is no XTEA and no scene-local pair in this packet. If the zone is unchanged and `valueClient880` is already 2, the handler returns (`Client.java:4143`).

Scene origin:

- `valueClient592 = (zoneX - 6) * 8`
- `valueClient593 = (zoneZ - 6) * 8`

Map squares requested for opcode 152 run x and z from `(center-6)/8` through `(center+6)/8` inclusive (`Client.java:4180`). For each square the client calls `OnDemand.getMapFile(z, x, 0, 949)` and `getMapFile(z, x, 1, 949)`, then `request(3, id)` when the id is not -1. Archive 3 is maps. `getMapFile` builds key `(x<<8)+z`. Type 0 returns the land id. Any other type returns the loc id. A missing key returns -1 (`OnDemand.java:342`).

`valueClient637` is set when the zone square is 48 or 49 on x and 48 on z, or 48 on x and 148 on z (`Client.java:4153`). Those tutorial squares skip the request when z is 49, 149, or 147, or x is 50, or x is 49 and z is 47 (`Client.java:4183`).

## Opcode 20

Bit block (`Client.java:4121`): for level 0..3, x 0..12, z 0..12, `bits(1)`. If that bit is 1, `bits(26)` is stored in `valueClient675[level][x][z]`. Otherwise the cell is -1. Then byte-align, `u16be` zone X, `u16le` zone Z. `valueClient655` is set true.

A packed cell is split as region X `(word>>14)&1023`, region Z `(word>>3)&2047` (`Client.java:4213`). The file key is `(regionX/8<<8)+(regionZ/8)`. The same land and loc requests are made. The client does not read an XTEA key here either.

## Zone base without a rebuild

Opcode 243, size 2: `u8` stored in `valueClient895`, `u8-128` stored in `valueClient894` (`Client.java:5228`).

Opcode 241, size 2: `u8neg` stored in `valueClient894`, `u8neg` stored in `valueClient895` (`Client.java:5077`). It clears object stacks and loc-add records on an 8 by 8 square.

Opcode 16, size `-2`: `u8-128` into `valueClient895`, `u8sub` into `valueClient894`, then `u8` sub-opcodes dispatched to `readZonePacket` until the payload ends (`Client.java:5235`).

Zone packets add the high nibble of the position byte to `valueClient894` and the low three bits to `valueClient895` (`Client.readZonePacket`, `Client.java:6700`).

## Map files

`World.opkind671` reads one land tile (`builds/rs2/350-2005-12-30/src/src/main/java/deob/World.java:453`). Each tile is a loop of `u8` opcodes:

| opcode | payload |
| --- | --- |
| 0 | end. Level 0 height is `-opkind6711(...) * 8`. A higher level copies the level below and subtracts 240 |
| 1 | `u8` height. A height byte of 1 is treated as 0. Level 0 stores `-height*8`. A higher level subtracts `height*8` from the level below |
| 2..49 | `s8` overlay id. Shape is `(opcode-2)/4`. Rotation is `(opcode-2+rotation)&3` |
| 50..81 | underlay flag `opcode-49`. No extra byte |
| 82.. | overlay type `opcode-81`. No extra byte |

Tiles outside 0..103 still consume the same opcodes (`World.java:496`).

`World.opkind675` reads a loc file (`World.java:539`):

1. `smart` id delta until 0. The id accumulates.
2. Inner `smart` position delta until 0. The position accumulates after subtracting 1 from each delta.
3. Position: z is `word&63`, x is `(word>>6)&63`, level is `word>>12`.
4. `u8`: shape is `byte>>2`, rotation is `byte&3`.
5. If land flag bit 2 is set on level 1, the loc level drops by one.
6. `opkind6710` places the loc. That method body was not read. See [gaps.md](gaps.md).

`World.opkind676` is the same loc stream for an 8 by 8 instance chunk (`World.java:589`). Coordinates go through `C9bbf6bf7.opkind833` and `opkind834` (`World.java:619`). Those two helper bodies were not read.

`Client.buildScene` (`Client.java:2303`) is the caller that consumes the loaded land and loc bytes after `valueClient880` advances. Opcode 210, with no payload, is written from `opClient53` once the scene state reaches 2 (`Client.java:8870`).
