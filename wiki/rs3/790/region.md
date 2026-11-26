# Region

Rebuild dispatch is `Staticelements2.doStaticelements2(DataBlock2124)` (`deob/Staticelements2.java:341`).

`DataBlock1914` (`deob/DataBlock1914.java:26`) stores two booleans. `doDataBlock1914` returns the first, which selects the dynamic body.

| Value | First bool | Use |
| --- | --- | --- |
| `cls23ed1d88` | false | `doStaticelements22222b19`, no packet body (`Staticelements2.java:356`) |
| `cls23ed1d883` | false | static rebuild |
| `cls23ed1d882` | true | `doStaticelements22222b20`, no packet body (`Staticelements2.java:524`) |
| `cls23ed1d884`, `cls23ed1d885` | true, second false | dynamic |
| `cls23ed1d886`, `cls23ed1d887` | true, second true | dynamic |

Server opcode 103 (`cls1f645fa6104`, size `-2`) copies the payload and calls the static rebuild with `cls23ed1d883` (`rs2/client/Client.java:7686`). Server opcode 161 (`cls1f645fa6162`, size `-2`) does the same with `cls23ed1d884` (`Client.java:9914`). The login state's first packet uses the same two values (`deob/Statics.java:3315`).

## Build area

`PlayerGroupDeltaEncodingKey` (`com/jagex/game/group/PlayerGroupDeltaEncodingKey.java:8`) maps the build-area id to a tile count: 0 is 104, 1 is 120, 2 is 136, 3 is 168, 4 is 72. `doStaticelements2(PlayerGroupDeltaEncodingKey)` (`Staticelements2.java:638`) sets `tint3` and `tint4` to that count.

## Static body

`doStaticelements2(PreferencesCpuUsage)` (`Staticelements2.java:406`):

1. `u16le` zone X (`doPositionedSound2322b7`).
2. `u8` build-area id.
3. `neg-u8`. A value of 1 forces reload.
4. `u16be-m128` zone Z (`doPositionedSound2322b22`).
5. `u8` count. That count sizes the arrays.

The method then computes squares itself. For map X from `(zoneX - (tiles >> 4)) / 8` through `(zoneX + (tiles >> 4)) / 8`, and the same for Z, it keeps a square when archive index 5 answers `doDataBlock1916(fileId, 3)`. `ParticleEmitterType.particleEmitterType4.tint` is 3 (`com/jagex/game/config/ParticleEmitterType.java:14`). The filled count is `tint12`, which can differ from the `u8` that sized the arrays. Map file id `doStaticelements2(int, int)` (`Staticelements2.java:633`) is `x | (z << 7)`.

This method contains no XTEA call. `rslib/src/both/move/rev_790.c` `region_encode` matches this header. The dynamic body is the gap in [gaps.md](gaps.md).

## Dynamic body

`doStaticelements22` (`Staticelements2.java:441`):

1. `u8-128` build-area id.
2. `u16be`. This becomes the Z argument of the origin setter.
3. `u8` flags. Bit 0 is force reload.
4. `u16be-m128`. This becomes the X argument of the origin setter.
5. `u8-128` instance type. 1 selects `cls23ed1d884`, 2 selects `cls23ed1d885`, 3 selects `cls23ed1d886`, 4 selects `cls23ed1d887`.
6. Bit buffer: 4 planes by `(tiles >> 3)` by `(tiles >> 3)`. Each cell is a 1-bit present flag and, when set, 26 bits. The 26-bit word is split as `>> 14 & 1023` and `>> 3 & 2047`. The low 3 bits remain in the stored word. The client turns those fields into map squares and keeps the ones archive 5 has.
7. Byte align, then `u8` unique-count. That count sizes the arrays. `tint12` is the count the client computed from the 26-bit words.

This method contains no XTEA call.

## Origin

`doStaticelements2(int x, int z, int stage, boolean force)` (`Staticelements2.java:670`) stores the base as `DataBlock1971(0, (x - (tiles >> 4)) * 8, (z - (tiles >> 4)) * 8)` (`Staticelements2.java:694`). `DataBlock1971` stores plane, x, z (`deob/DataBlock1971.java:19`). For 104 tiles, `tiles >> 4` is 6, so the base is `(zone - 6) * 8`. Static rebuild passes zone X then zone Z and stage 17. Dynamic rebuild passes the `u16be-m128` as X, the `u16be` as Z, and stage 17.

Scene-local coordinates later subtract this base. The player snapshot uses `tint2` as base X and `tint3` as base Z (`com/jagex/game/world/entity/ObjIconFactory.java:59`).
