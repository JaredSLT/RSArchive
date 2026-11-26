# Sync

Opcode 254, size `-2`, calls `UnerwarteteAntwortVomAnmeldeServer.opkind2683` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:761`). Opcode 114, size `-2`, calls `Standort.opkind3012` (line 1043).

Local player slot is 2047 (`ConnectionTimedOut.java:97`). Players are `SelectWorld.valuekind3218`. NPCs are `C4d4e728c.valuekind27615`, length 32768, class `C78d60e9c`.

Scene X is `DirectByteArray.valueDirectByteArray67`. Scene Z is `valueDirectByteArray74`. Fine X `valueDirectByteArray51` is built from field 67. Fine Z `valueDirectByteArray42` is built from field 74 (`builds/rs2/481-2007-12-11/src/src/main/java/deob/DirectByteArray.java:201`). Level is `Wall.valueWall30`.

`opDirectByteArray22(z, jump, x)` stores `x` in field 67 and `z` in field 74 (lines 170-202). `opDirectByteArray26` (lines 221-277), with field 67 as x and field 74 as z:

| dir | dx | dz |
| --- | --- | --- |
| 0 | -1 | +1 |
| 1 | 0 | +1 |
| 2 | +1 | +1 |
| 3 | -1 | 0 |
| 4 | +1 | 0 |
| 5 | -1 | -1 |
| 6 | 0 | -1 |
| 7 | +1 | -1 |

The second argument `true` marks a run step (`valueDirectByteArray45[0]`).

Facing values are `NoReplyLoginserver.valuekind247131` = `{768, 1024, 1280, 512, 1536, 256, 0, 1792}` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/NoReplyLoginserver.java:4`).

## Local player

`C105f7f5d.opkind3425` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/C105f7f5d.java:181`) starts the bit cursor, then:

| Bits | Meaning |
| --- | --- |
| 1 | Present. 0 ends the block. |
| 2 | Type, only if present. |

| Type | Following bits |
| --- | --- |
| 0 | No movement. Push index 2047 for a mask. |
| 1 | 3-bit direction, `opDirectByteArray26(dir, false)`, then 1 bit. If that bit is 1, push 2047. |
| 2 | 3-bit direction with `true`, another 3-bit direction with `true`, then 1 mask bit. |
| 3 | 7-bit local x, 1 mask bit, 7-bit local z, 2-bit level into `Wall.valueWall30`, 1-bit jump, then `opDirectByteArray22(z, jump == 1, x)` (lines 206-216). |

## Other players

`LongNode.opLongNode23` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/LongNode.java:29`), still in bits:

| Bits | Meaning |
| --- | --- |
| 8 | Count. If the count is below `TextureOp19.valueTextureOp1947`, the extras go on the remove list `Ca18a37b5.valuekind2615`. If the count is above the previous size, throw `gppov1`. |

Per previous player: 1 bit skip. 0 keeps the player and refreshes the cycle. 1 is followed by 2 type bits:

| Type | Meaning |
| --- | --- |
| 0 | Keep, and push the index for a mask. |
| 1 | Keep, 3-bit walk, 1 mask bit. |
| 2 | Keep, two 3-bit run steps, 1 mask bit. |
| 3 | Remove. |

## New players

`Ccc509c66.opkind3527` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/Ccc509c66.java:130`). While `opkind3402(packetSize) >= 11`:

| Bits | Meaning |
| --- | --- |
| 11 | Index. 2047 (`~index != -2048` is false) ends the list. |
| 5 | dz. If the value is greater than 15, subtract 32. Added to local z. |
| 1 | Jump. |
| 3 | Facing, indexed into `valuekind247131`. Applied only when the player object is new. |
| 1 | Mask. 1 pushes the index. |
| 5 | dx, signed the same way. Added to local x. |

Place with `opDirectByteArray22(localZ + dz, jump == 1, localX + dx)`. If `Node.valueNode37[index]` is already set, `opMapMarker20` runs on it. The block is 26 bits. The loop test only requires 11. Then `opkind3409` byte-aligns (line 173).

## Player masks

`World.opWorld78` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/World.java:35`). For each index in `Wall.valueWall13`: `u8` mask. If bit `0x2` is set, add another `u8 << 8`. Bit 2 is the extension, not a payload by itself. Then `TextureOp29.opkind3024` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/TextureOp29.java:532`). The check order is the wire order.

| Mask | Bytes |
| --- | --- |
| `0x200` | `u8neg`, `u8neg`, `u8_sub128`, `u8_add128`, `u16be_sub128`, `u16le`, `u8_sub128` (lines 533-540) |
| `0x400` | `u8`, `u8_sub128` |
| `0x40` | `u16le` (65535 becomes -1), `u8neg` |
| `0x20` | `u16le_sub128` (65535 becomes -1) |
| `0x10` | `u8`, `u8neg`, `u8_add128` |
| `0x80` | `jstr`. If the first character is 126, one character is stripped and the text is also shown as public chat. |
| `0x1` | `u16be`, `u16le_sub128` |
| `0x100` | `u16le_sub128` (65535 becomes -1), `u32le`. Low 16 is the delay, high 16 is separate. |
| `0x8` | `u16be`, `u8_sub128`, `u8_add128` length, then `opkind33448` of that many bytes. The position is restored to the start plus the length (line 649), so skipped chat still consumes the bytes. The bytes are reversed and each is minus 128. The text decode is `MapElement.opMapElement13`. |
| `0x4` | `u8_sub128` length, then `call56371ce8` of that many bytes into a new buffer stored at `Node.valueNode37[index]`, then `MapMarker.opMapMarker20`. |

## Appearance

`MapMarker.opMapMarker20` resets the buffer to 0 (`builds/rs2/481-2007-12-11/src/src/main/java/deob/MapMarker.java:198`). The wire bytes were stored in reverse, so this order is the reversed block.

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | `u8` flags | bit 0 gender; bit 1 (`(flags & 2) == 2`) reads two more `u8`, each stored `<< 2`; bit 2 (`(flags & 4) != 0`) reads a trailing `u16be` |
| 2 | `i8`, `i8` | |
| 3 | 12 slots | `u8`. 0 means empty. Otherwise a second `u8`, id = `(first << 8) \| second`. Slot 0 and id 65535 reads `u16be` npc id and stops the slots. id `>= 32768` is an object (`ClientScript.valueClientScript80[id - 32768]`, stored with `0x40000000`). Otherwise an identity kit (`id - 256`) with `0x80000000`. |
| 4 | 5 times `u8` | Colour, clamped to the palette length, else 0. |
| 5 | 7 times `u16be` | 65535 becomes -1. |
| 6 | `u64be` | Name. |
| 7 | `u8` | Combat level. |
| 8 | `u16be` | Only when flag bit 2 was set. |

No skill level is read. RSLib `appearance_parse` returns `RS_ERR_UNSUPPORTED` when `(flags & ~1) != 0` (`rslib/src/both/move/rev_481.c:216`). This client reads bit 1 and bit 2. See [gaps.md](gaps.md).

## NPC movement

`ClickSwitch.call50826e2b` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/ClickSwitch.java:388`). 8-bit count. A short count removes the tail. A long count throws `gnpov1`. Per NPC: 1 bit skip, else 2 type bits. Type 0 keeps and masks. Type 1 is a 3-bit walk plus a mask bit. Type 2 is two 3-bit run steps plus a mask bit. Type 3 (`~type == -4`) removes. The player type-3 test is `var6 == 3`; the NPC test is the same value.

## New NPCs

`MsiType.opMsiType22` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/MsiType.java:189`). While bits remaining are at least 27 (`~remaining <= -28`):

| Bits | Meaning |
| --- | --- |
| 15 | Index. 32767 ends the list. |
| 14 | Type id, passed to `Js5.opJs512`. |
| 5 | dx, later signed if greater than 15. |
| 5 | dz, signed if greater than 15. |
| 3 | Facing. |
| 1 | Mask. |
| 1 | Jump. |

Place with `opDirectByteArray22(localZ + dz, jump == 1, localX + dx)`. The sign fix for dx is applied in code after the facing bits are read; it does not read more bits. The full block is 44 bits. The loop test only requires 27. Then byte-align.

If `valueDirectByteArray66` is 0, facing is forced to 0 (line 233).

## NPC masks

`GlBoundingBox.opGlBoundingBox10` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/GlBoundingBox.java:162`). One `u8` only. There is no extension byte. Wire order:

| Mask | Bytes |
| --- | --- |
| `0x1` | `u16be` (65535 becomes -1), `u8` |
| `0x80` | `u8`, `u8_sub128` |
| `0x2` | `u16be` type id, then the type fields are copied |
| `0x20` | `jstr`. Duration is set to 100. |
| `0x10` | `u16be_sub128` (65535 becomes -1), `u32be`. High 16 and low 16 are split. This is not the player spotanim width. |
| `0x40` | `u16be_sub128`, `u16be_sub128` |
| `0x8` | `u16be` (65535 becomes -1) |
| `0x4` | `u8`, `u8_add128`, `u8` |

After masks, player update throws `gpp1` if the byte cursor does not equal the packet size, and `gpp2` if a listed player is null (`UnerwarteteAntwortVomAnmeldeServer.java:51`). NPC update throws `gnp1` and `gnp2` the same way (`Standort.java:31`).
