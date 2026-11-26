# Region

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`.

Opcode 98, size -2, `C2517e207.valuekind9998` (`deob/C2517e207.java:20`) calls `Cc4cace49.opkind9583(false)` and returns false, so the read loop may take another packet in the same tick (`deob/C268455a9.java:1436`). Opcode 88, size -2, `C408bee69.valuekind9671` (`deob/C408bee69.java:7`) calls `opkind9583(true)` and also returns false (`C268455a9.java:1599`). Login state 10 calls `opkind9583(false)` on the first game payload (`deob/LswprPos.java:341`).

Scene width `Cdf90cca7.valuekind8128` defaults to 104 (`deob/Cdf90cca7.java:8`). Scene height `C83b47d7d.valuekind8933` defaults to 104 (`deob/C83b47d7d.java:9`).

Map archive is index 5, `C381a95c3.valuekind101912` (`deob/Client.java:1938`). File ids come from `opkind64010(name)` on that archive. The terrain bytes those ids point at are loaded later by `SuaListaDeIgnoradosEstCheia` (`deob/SuaListaDeIgnoradosEstCheia.java:50`). The decoder of those bytes was not read.

## Static rebuild

`opkind9583(false)` (`deob/Cc4cace49.java:99`):

- u16le base X, stored as the first argument of `Cdf90cca7.opkind8121`
- u16be base Z, stored as the fourth argument
- u8 passed to `C91e6498b.opkind7453`. That method was not read.
- u8neg. The value `== 1` is the force flag (`~opUnused673() == -2`).
- `(remaining) / 16` groups of four u32be keys, stored in `Ced45c6ab.valuekind7887`

Map squares, for x from `(baseX - (width >> 4)) / 8` through `(baseX + (width >> 4)) / 8`, and the same for z using height (`Cc4cace49.java:127`):

- id `(x << 8) + z`
- names `m{x}_{z}`, `l{x}_{z}`, `um{x}_{z}`, `ul{x}_{z}`

## Dynamic rebuild

`opkind9583(true)` (`Cc4cace49.java:21`):

- u16be, passed as the base-X argument of `opkind8121`
- u16le, passed as the base-Z argument
- u8 to `opkind7453`
- u8neg `== 1` force flag
- bit cursor aligned with `opkind8136`
- for plane 0..3, zone x `0 .. (width >> 3) - 1`, zone z `0 .. (height >> 3) - 1`: 1 bit. If set, 26 bits stored in `C96ed0652.valuekind6644`. If clear, that slot is -1.
- `opkind8132`
- `(remaining) / 16` groups of four u32be keys

A zone value that is not -1 becomes a map square (`Cc4cace49.java:70`):

- `region = (value >> 14) & 1023`
- `zpart = (value & 16377) >> 3`
- map id `(region / 8 << 8) + (zpart / 8)`
- the same four names, with `region / 8` and `zpart & 255` as the two numbers. `255 & mapId` is the low byte (`Cc4cace49.java:86`).

## Scene origin

`Cdf90cca7.opkind8121(false, baseX, force, baseZ)` (`deob/Cdf90cca7.java:13`):

- `TextureOp29SubOp4.valueTextureOp29SubOp4167 = (baseX - (width >> 4)) * 8`
- `C292109ad.valuekind7854 = (baseZ - (height >> 4)) * 8`

Those two statics are the base added to local tiles in the walk writer. The rest of `opkind8121` shifts already spawned npc and player coordinates by the origin delta and sets load state 25, or 28 when the first argument is true. The packet always passes false for that argument. State 25 and 28 call `SuaListaDeIgnoradosEstCheia.opkind8193` from the tick (`deob/Client.java:1483`). The body of `opkind8193` was not read.
