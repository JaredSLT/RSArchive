# Player and NPC sync

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`.

S2C opcode 187, size `-2`, calls `Cafed0342.call00d41322` (`DelayedStateChange.java:1595`). S2C opcode 201, size `-2`, calls `PathingEntity_Class147.opPathingEntity_Class14749` (`DelayedStateChange.java:1440`).

Both start the bit reader with `opAbstractWorld1` inside the movement methods and finish on `opkind5038` after the add-loop. The packet is finished only when `valueObjStackNode65 == PlainTile.valuePlainTile19`. A mismatch throws `gpp1` (players, `Cafed0342.java:56-57`) or `gnp1` (NPCs, `PathingEntity_Class147.java:85-86`). A null slot in the kept list throws `gpp2` (`Cafed0342.java:59-62`) or `gnp2` (`PathingEntity_Class147.java:88-91`).

## Local player

`C046d0af1.opGlModel_Class235` (`C046d0af1.java:34-71`).

`bits(1)` update. If 0, stop. Else `bits(2)` type:

| Type | Bits |
| --- | --- |
| 0 | no more movement bits. Flag slot 2047 for a mask |
| 1 | `bits(3)` walk direction, `bits(1)` mask flag. Walk is `opkind40611(false, dir)` |
| 2 | `bits(3)`, `bits(3)` run directions, `bits(1)` mask flag. Both steps use `opkind40611(true, dir)` |
| 3 | `bits(7)`, `bits(1)` mask flag, `bits(7)`, `bits(2)` plane stored in `C0236c255.valuekind49771`, `bits(1)` teleport flag. `opEnvironment6(second7, first7, teleport)` (`C046d0af1.java:60-69`) |

## Other players

`Cb4aa1ea4.opkind35712` (`Cb4aa1ea4.java:45-98`).

`bits(8)` count. If the count is greater than the previous list, throw `gppov1` (`Cb4aa1ea4.java:53-54`). Players past the count are removed. For each kept index: `bits(1)` update. If 0, keep the player and do not flag a mask. If 1, `bits(2)`:

| Type | Effect |
| --- | --- |
| 0 | keep, flag a mask |
| 1 | `bits(3)` walk, `bits(1)` mask |
| 2 | `bits(3)`, `bits(3)` run, `bits(1)` mask |
| 3 | remove |

`World.opWorld87` (`World.java:32-78`) while `opkind5034(payloadSize) >= 11` (`World.java:34`):

`bits(11)` index. 2047 ends the loop. Create `Environment` if the slot is null, and if `SignLinkAudioChannel.valueSignLinkAudioChannel21[index]` is already set, run `call8cb28dd2` on it. Then `bits(1)` mask flag, `bits(5)` added to `valuekind40647` (subtract 32 if `> 15`), `bits(5)` added to `valuekind40617` (subtract 32 if `> 15`), `bits(1)` flag passed as the third argument of `opEnvironment6`, `bits(3)` facing looked up in `Cf4d6a7f2.valuekind5531`. `opEnvironment6` is called as `(local40617 + second5, local40647 + first5, flag == 1)` (`World.java:54-71`). `opkind5038` aligns.

The active index is 11 bits. `C3c7e15a6.valuekind5482` is a separate NPC array of length 32768 (`C3c7e15a6.java:13`).

## Player masks

`WorldInfo.opWaterMaterialRenderer3` (`WorldInfo.java:20-30`). For each flagged index: `u8` flags. If bit 32 is set, another `u8` is shifted up by 8. Then `C3c7e15a6.call1b833e03` (`C3c7e15a6.java:28-178`). Test order:

| Mask | Body |
| --- | --- |
| 512 | `u16le_sub128lo` graphic (65535 means -1). `u32be`: high 16 is one delay, low 16 is the other |
| 2 | `u16le` chat word. Bit 15 (`0x8000`) selects quick chat. `u8_sub128` effect. `u8_128minus` text length. `bytes(length)` into a side buffer (`opObjStackNode63`), then pos jumps to `start + length` (`C3c7e15a6.java:79-106`). On that side buffer, quick chat is `Tile.opTile8`: `u16be` id (`Tile.java:78-82`), then `TextureOp11.opTextureOp1112` reads one big-endian integer per parameter, width `C046d0af1.valuekind4716[type]` (`C046d0af1.java:10`, `ObjStackNode.java:446`). Otherwise `huffstr`. Ignored players skip the chat |
| 16 | `u16le_sub128lo` face entity. 65535 means -1 |
| 4 | `u16be_sub128lo` animation (65535 means -1), `u8_neg` delay |
| 1 | `smart` damage, `u8_neg` type, `u8_128minus` soak |
| 128 | `jstr` overhead. If the first character is 126, strip it and force a public chat line |
| 1024 | `smart` damage, `u8` type |
| 8 | `u8` length, then `bytes_sub128(length)` into a new `ObjStackNode`, stored at `SignLinkAudioChannel.valueSignLinkAudioChannel21[index]`, then `Environment.call8cb28dd2` |
| 256 | `u8_neg`, `u8_neg`, `u8_128minus`, `u8_neg`, `u16le_sub128lo` delay, `u16be_sub128lo` delay, `u8_sub128` |
| 64 | `u16be_sub128lo` and `u16be` face coordinates |

## Appearance

`Environment.call8cb28dd2` (`Environment.java:297-439`) reads the bytes after the `-128` transform, so each original wire byte had 128 added.

| Order | Decoded read |
| --- | --- |
| 1 | `u8` flags |
| 2 | if bit 1 is set: two `u8`, each shifted left 2. Otherwise both are 0 (`Environment.java:300-306`) |
| 3 | size is `1 + ((flags & 57) >> 3)` via `opkind40610` (`Environment.java:311`) |
| 4 | gender is bit 0. Prayer icon is bits 6 and 7 (`Environment.java:309-312`) |
| 5 | `i8`, `i8` |
| 6 | 12 slots. Each starts with `u8`. 0 means empty. Otherwise another `u8`, and the word is `(first << 8) \| second`. Slot 0 word 65535 means the next `u16be` is an npc id and the slot loop stops. A word `>= 32768` is an item id `word - 32768` with bit `0x40000000`. A smaller word is `word - 256` with bit `0x80000000` |
| 7 | 5 × `u8` colours. A value outside the colour table becomes 0 |
| 8 | 7 × `u16be` animations. 65535 means -1. Order of fields: `valuekind4068`, `valuekind40627` (also copied to `valuekind40610`), `valuekind40660`, `valuekind40639`, `valuekind40619`, `valuekind40624`, `valuekind4065` |
| 9 | `u64be` name |
| 10 | `u8` combat |
| 11 | if flags bit 2: `u16be` skill, and the second level reuses combat, and the third level is -1. Else two `u8`, and 255 on the second means -1 |
| 12 | `u8`. If non-zero, three `u16be` (`valueEnvironment28`, `valueEnvironment12`, `valueEnvironment33`) (`Environment.java:409-417`) |

## NPCs

Movement `VeuillezPatienterTentativeDeBtablissement.opkind4023` (`VeuillezPatienterTentativeDeBtablissement.java:34-87`) matches other players, with `bits(8)` count and throw `gnpov1` if the count grew (`VeuillezPatienterTentativeDeBtablissement.java:43-44`).

Add loop `C835aa718.opkind4881` (`C835aa718.java:20-83`). The loop condition is `~opkind5034(payloadSize) <= -28` (`C835aa718.java:22`), which is true when at least 27 bits remain.

`bits(15)` index. 32767 stops. `bits(14)` type id via `Cba52aa90.opkind4313`. `bits(5)` added to `valuekind40647` (subtract 32 if `> 15`). `bits(1)` mask flag. `bits(1)` move flag. `bits(3)` facing through `Cf4d6a7f2.valuekind5531`. `bits(5)` added to `valuekind40617` (subtract 32 if `> 15`). `opkind4069(local40647 + first5, moveFlag, step, local40617 + second5)` (`C835aa718.java:38-73`). `opkind5038` aligns.

## NPC masks

`C0b9281af.opkind4621` (`C0b9281af.java:17-108`). One `u8`. Bit 128 is not a second flag byte. Order:

| Mask | Body |
| --- | --- |
| 128 | `jstr` overhead |
| 64 | `u16be_sub128lo` face entity. 65535 means -1 |
| 32 | `u16le_sub128lo` new type id |
| 1 | `u16be_sub128lo` graphic (65535 means -1), `u32_b2b3b0b1` packed delays |
| 8 | `u8_neg` damage, `u8_128minus` type |
| 2 | `u16be_sub128lo` animation (65535 means -1), `u8_128minus` delay |
| 16 | `u8` damage, `u8_sub128` type, `u8` soak |
| 4 | `u16be_sub128lo` and `u16be` face coordinates |
