# Region

Opcode 126, size `-2`, calls `SieBefindenSichEinemMitgliederGebiet.opkind2783(false)` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1247`). Opcode 187, size `-2`, calls `opkind2783(true)` (line 1383). Login stage 10 also calls `opkind2783(false)` on the first payload (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SoundBank.java:221`).

Map archive is JS5 index 5, `VarpType.valueVarpType88` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SieBefindenSichEinemMitgliederGebiet.java:65`). Terrain names start with `m` (`C1c7813d8.valuekind3506`, `builds/rs2/481-2007-12-11/src/src/main/java/deob/C1c7813d8.java:10`). Loc names start with `l` (`Ca17abb5a.valuekind23317`, `builds/rs2/481-2007-12-11/src/src/main/java/deob/Ca17abb5a.java:5`). The separator is `_` (`ServerBeingUpdated.valuekind14063`, `builds/rs2/481-2007-12-11/src/src/main/java/deob/ServerBeingUpdated.java:8`). The group id looked up by that name is `(mapX << 8) + mapZ` only after the name hash; the client asks the archive for the hash of `m` + x + `_` + z and `l` + x + `_` + z.

## Static rebuild

`opkind2783` false branch (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SieBefindenSichEinemMitgliederGebiet.java:412`):

| Order | Width | Use |
| --- | --- | --- |
| 1 | `u16le` | Local x, passed as the third argument of `opDirectByteArray22`. |
| 2 | `keyCount` groups of 4 `u32le` | `keyCount = (packetSize - position) / 16`, computed before the keys. The tail is 7 bytes, so it does not add a key group. Stored in `Ca45137e8.valuekind2963`. |
| 3 | `u8neg` | Level. |
| 4 | `u16le_sub128` | Zone x. |
| 5 | `u16le_sub128` | Local z. |
| 6 | `u16be` | Zone z. |

Then `DieseWeltIstVoll.opkind2622(localX, zoneX, localZ, level, zoneZ)`.

RSLib `region_decode_481` matches this static order. Its comment says opcode 187 is not this packet (`rslib/src/both/move/rev_481.c:21`).

## Dynamic rebuild

True branch (line 343):

| Order | Width | Use |
| --- | --- | --- |
| 1 | `u16le_sub128` | Local z. |
| 2 | `u16le` | Zone x. |
| 3 | `u16le_sub128` | Zone z. |
| 4 | `u8` | Level. This byte is not negated. |
| 5 | bits | 4 planes, 13 by 13. Each cell is 1 bit. If the bit is 1, the next 26 bits are stored in `LangDe.valuekind30912[plane][x][z]`. If 0, the cell is -1. Then `opkind3409`. |
| 6 | `keyCount` groups of 4 `u32_ime` | Same remaining-size division. These keys are not `u32le`. |
| 7 | `u16be_sub128` | Local x. |

Then `opkind2622(localX, zoneX, localZ, level, zoneZ)`.

## 26-bit value

The same stored integer is masked three different ways. Do not replace one literal with another.

| Site | Expression |
| --- | --- |
| Packet reader, map-square id (`SieBefindenSichEinemMitgliederGebiet.java:387`) | `(value & 16760938) >> 14`, then `(value >> 3) & 2047`. Map id is `(first / 8 << 8) + (second / 8)`. |
| Terrain build (`SelectWorld.java:237`) | rotation `(value >> 1) & 3`; `(value & 16762147) >> 14`; `(value >> 3) & 2047`; `(value & 56223166) >> 24` |
| Loc build (`SelectWorld.java:278`) | `(value & 62542787) >> 24`; rotation `(value >> 1) & 3`; `(value >> 14) & 1023`; `(value & 16380) >> 3` |

## Scene origin

`DieseWeltIstVoll.opkind2622` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/DieseWeltIstVoll.java:57`):

- Base x `BitteVersuchenSie.valuekind2721 = zoneX * 8 - 48`, which is `(zoneX - 6) * 8` (line 71).
- Base z `C1be0fc13.valuekind3182 = (zoneZ - 6) * 8` (line 70).
- Existing players and NPCs have field 67 and fine x shifted by the base-x delta, and field 74 and fine z shifted by the base-z delta (lines 75-102).
- The local player is placed with `opDirectByteArray22(localZ, false, localX)` only inside this method, and the method returns immediately when the zone and the remembered level did not change (line 58).
- Low detail (`GameShell.valueGameShell110` false) forces the remembered level `Ca18a37b5.valuekind2611` to 0 (line 60). `Wall.valueWall30` is still set from the packet level (line 106).

Static map squares run x from `(zoneX - 6) / 8` through `(zoneX + 6) / 8` and z from `(zoneZ - 6) / 8` through `(zoneZ + 6) / 8` (line 443). The whole set is marked skipped when `(zoneX / 8 == 48 \|\| zoneX / 8 == 49) && zoneZ / 8 == 48`, or when `zoneX / 8 == 48 && zoneZ / 8 == 148` (lines 432-438). While that flag is set, a square is also skipped when its z is 49, 149, or 147, or its x is 50, or its x is 49 and its z is 47 (line 446).

## Files

`SelectWorld.opkind32111` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SelectWorld.java:121`) loads terrain with `VarpType.valueVarpType88.opkind34710(group, 0)` and locs with `opkind34722(keys, 0, group)` (lines 128 and 136). `opkind34722` decrypts the group through `opkind34719` when the four key ints are not the trivial pattern (`builds/rs2/481-2007-12-11/src/src/main/java/deob/UngUltigesAnmeldePaket.java:418` and line 327). The decrypt is `SlArrows.opkind3341`. Terrain is not passed keys.

Static scene origin of a map square is `(mapId & 255) * 64 - baseZ` for one axis and `(mapId >> 8) * 64 - baseX` for the other (`SelectWorld.java:221`). Dynamic chunks are placed at `8 * chunk` (lines 246 and 286). The terrain and loc byte decoders `ByteArrayNodeSecondary.call099838cc` and `BenutzenSieDieWpasswortNdernOption.call9b06a231` were not read field by field.

Zone packets use the bases set by opcode 6: `IntHashTable.valueIntHashTable24` and `TextureOpRange.valueTextureOpRange31`, both `u8_sub128` (`SpotAnim.java:839`). Opcode 105 sets those bases with `u8neg` and `u8_add128` and then reads a run of zone opcodes (`SpotAnim.java:1079`). Opcode 68 clears an 8 by 8 square of ground items on the current level (`SpotAnim.java:1403`).
