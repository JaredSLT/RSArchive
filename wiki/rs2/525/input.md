# Input

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`.

`Client.opClient186` copies the mouse in `ObjStack.opObjStack16` and stores the wheel delta in `C98d1be1a.valuekind5218` (`Client.java:940-944`). Key collection is `Client.calle2cbab8d`, called in state 10 (`Client.java:981`). That method was not read. The key array used by the packets below is `PathingEntity.valuePathingEntity170`.

On state 30, `ExpiresThuJanGmtMaxAge.opkind4441` (`ExpiresThuJanGmtMaxAge.java:18`) drains up to 100 server packets, then writes the client packets in this order when their conditions hold.

## Mouse samples, opcode 208

ISAAC (`ExpiresThuJanGmtMaxAge.java:41`). `u8` placeholder, then samples while fewer than 240 payload bytes have been written (`ExpiresThuJanGmtMaxAge.java:46`). The placeholder is later `backpatch_u8` with the payload length (`opObjStackNode79`, `ExpiresThuJanGmtMaxAge.java:115`).

Coordinates are clamped to 0..65534, and a `-1, -1` sample is kept as a flag. A sample that matches the previous point only increments a repeat count, up to 2047. Otherwise the repeat is flushed:

| Repeat and delta | Bytes |
| --- | --- |
| repeat `< 8` and both deltas in -32..31 | `u16be`: `(repeat << 12) \| ((dx + 32) << 6) \| (dy + 32)` |
| repeat `< 32` and both deltas in -128..127 | `u8` `128 + repeat`, then `u16be` `((dx + 128) << 8) \| (dy + 128)` |
| repeat `< 32` and a real point | `u8` `192 + repeat`, then `u32be` `x \| (y << 16)` |
| repeat `< 32` and the `-1, -1` flag | `u8` `192 + repeat`, then `u32be` `0x80000000` |
| repeat `>= 32` and a real point | `u16be` `0xE000 + repeat`, then `u32be` `x \| (y << 16)` |
| repeat `>= 32` and the flag | `u16be` `0xE000 + repeat`, then `u32be` `0x80000000` |

## Click, opcode 53

ISAAC, only when `ClanMember.valueClanMember1 != 0` (`ExpiresThuJanGmtMaxAge.java:130-162`).

`u16le_add128lo` of `(button << 15) \| deltaTicks`, where button is 1 when the click value is 2 and 0 otherwise, and `deltaTicks` is `(PlainTile.valuePlainTile16 - ProjAnimNode.valueProjAnimNode47) / 50` clamped to 32767. Then `u32le` of `(y << 16) \| x` with both coordinates clamped to 0..65535. `x` is `Cc997e18d.valuekind4422`. `y` is `C4832e8d5.valuekind4071`.

## Camera keys, opcode 254

Keys 96, 97, 98, and 99 set `C0b9281af.valuekind4622` (`ExpiresThuJanGmtMaxAge.java:168-179`). When that flag is set and `WestBconnect.valuekind4855 <= 0`, the client sends ISAAC 254 with two `u16be_add128lo` values, `Clanreq.valuekind5052` and `C9c57aa59.valuekind3982` (`ExpiresThuJanGmtMaxAge.java:182-187`), and holds the flag off for 20 frames. S2C 124 writes those two fields (`DelayedStateChange.java:228-231`): `u16le_sub128lo`, then `u16be`.

## Focus, opcode 97

ISAAC `u8` 1 when `C22edd1ce.valuekind4095` becomes true, and `u8` 0 when it becomes false (`ExpiresThuJanGmtMaxAge.java:190-199`).

## Cache hash, opcode 142

Once, ISAAC `u32le` of `Cd50b3810.opkind5644()` (`ExpiresThuJanGmtMaxAge.java:202-205`).

## Walk

`MouseWheel.opMouseWheel20(steps, kind)` (`MouseWheel.java:34-70`). `steps` is clamped to 25. ISAAC opcode is 249 for kind 0, 98 for kind 1, and 218 for kind 2.

| Order | Write |
| --- | --- |
| 1 | `u8` length. Kind 0 or 2: `2 * steps + 3`. Kind 1: `2 * steps + 17` |
| 2 | `u16le_add128lo` of `VorbisMapping.valueVorbisMapping10 + waypointX[steps - 1]` |
| 3 | for each earlier waypoint, from `steps - 2` down to 0: `u8` `(waypointZ - baseZ)` and `u8_neg` `(waypointX - baseX)` |
| 4 | `u8_add128` of 1 when key 82 is down, else 0 |
| 5 | `u16be_add128lo` of `Cf05f8883.valuekind4308 + waypointZ[steps - 1]` |

`baseX` in that expression is the X of the last waypoint, and `baseZ` is its Z. The arrays are `SoundPcmStream.valueSoundPcmStream107` (Z) and `Ce9619f9b.valuekind4181` (X).

Call sites, each passing its own kind argument: `Cf46facc5.java:223`, `C9ce5ebca.java:206`, `Ca157093a.java:309`. Which UI control uses which kind was not traced.

## Ignore, opcode 31

`C0bfaa0f7.opkind4573` (`C0bfaa0f7.java:328-356`) sends ISAAC 31 and `u64be` of the name when the ignore list accepts it.

The rest of `opkind4441` after line 208, and the other `opkind5031` sites, were not transcribed. See [gaps.md](gaps.md) and [packets.md](packets.md).
