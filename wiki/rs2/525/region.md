# Region

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`.

`C8be0107e.opkind3609(boolean)` (`C8be0107e.java:84`) rebuilds the map. The argument is stored in `SpotAnimType.valueSpotAnimType122`.

| Call | Argument | Opcode |
| --- | --- | --- |
| login success | `false` | the opcode already read in login state 8; the payload is this packet (`ObjStack.java:429`) |
| `DelayedStateChange.java:996` | `false` | 93, size `-2` |
| `DelayedStateChange.java:453` | `true` | 125, size `-2` (`~opcode == -126`) |

Map file names are built on JS5 index 5, `C05b1c6f1.valuekind5452`. The pieces are the string `m` (`Environment.java:21`), `l` (`Cd35c2db1.java:13`), `um` (`DelayedStateChange.java:7`), `ul` (`VeuillezPatienterTentativeDeBtablissement.java:8`), and `_` (`Wall.java:13`). `opTextureOpVerticalGradient8` copies plain ASCII through (`TextureOpVerticalGradient.java:155`). A chunk id is `(regionX << 8) + regionZ`. Group ids:

| Group | Name |
| --- | --- |
| terrain | `m` + regionX + `_` + regionZ |
| locs | `l` + regionX + `_` + regionZ |
| underwater terrain | `um` + regionX + `_` + regionZ |
| underwater locs | `ul` + regionX + `_` + regionZ |

## Static (`false`)

Key count is `(payloadSize - pos) / 16` (`C8be0107e.java:161`). Each key is four `u32be` (`opObjStackNode73`, `C8be0107e.java:164-167`).

Then (`C8be0107e.java:170-174`):

| Order | Read | Use |
| --- | --- | --- |
| 1 | u16le | map base `var27`, passed to `opkind4696` |
| 2 | u16le_sub128lo | map base `var28` |
| 3 | u16be_sub128lo | `var29` |
| 4 | u8 | `var30` |
| 5 | u16be | `var31` |

`opkind4696(var31, var28, var29, var27, var30)` (`C8be0107e.java:216`).

Zone loop: regionX from `(var27 - 6) / 8` through `(var27 + 6) / 8`, regionZ from `(var28 - 6) / 8` through `(var28 + 6) / 8` (`C8be0107e.java:195-196`).

Blackout when `(var27 / 8 == 48 || var27 / 8 == 49) && var28 / 8 == 48`, or `var27 / 8 == 49 && var28 / 8 == 148` (`C8be0107e.java:182-188`). Inside that blackout, a chunk is stored with all four file ids `-1` when `regionZ == 49`, `~regionZ == -150` (regionZ 149), `regionZ == 147`, `regionX == 50`, or `~regionX == -50 && regionZ == 47` (regionX 49 and regionZ 47) (`C8be0107e.java:198`).

## Dynamic (`true`)

Header (`C8be0107e.java:87-91`): `u16be_sub128lo`, `u16le_sub128lo`, `u16le_sub128lo`, `u16be_sub128lo`, `u8_128minus`. The same five values are passed to `opkind4696(var4, var2, var1, var3, var5)` (`C8be0107e.java:159`), so the argument order is fourth, second, first, third, fifth.

Then `bit_start`. For plane 0..3, localX 0..12, localZ 0..12: `bits(1)`. If set, `bits(26)` is stored, otherwise the cell is -1 (`C8be0107e.java:94-104`). `bit_align`.

From a 26-bit value: regionX is `(value >> 14) & 1023`, the low piece is `(value & 16378) >> 3`, and the chunk id is `((regionX / 8) << 8) + (low / 8)` (`C8be0107e.java:133-135`).

Key count is the remaining byte count divided by 16 (`C8be0107e.java:108`). Each key is four `u32_b1b0b3b2` (`opObjStackNode55`, `C8be0107e.java:111-114`), not `u32be`.
