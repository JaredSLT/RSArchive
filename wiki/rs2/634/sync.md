# Sync

Player packet opcode 105, size -2, `C2f8bd213.valuekind13907` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C2f8bd213.java:10). The switch calls `AudioThread.opAudioThread46` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C4d106383.java:766, builds/rs2/634-2010-12-10/src/src/main/java/deob/AudioThread.java:11).

NPC packet opcode 118, size -2, `Ca56efff22.valuekind13011` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Ca56efff22.java:5). The switch calls `GnpMis.call59622295` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C4d106383.java:1934, builds/rs2/634-2010-12-10/src/src/main/java/deob/GnpMis.java:14).

Bit order is high-bit-first inside the current byte. See [framing.md](framing.md).

Local player index is `Cf1a80554.valuekind15747`, initial value -1 (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf1a80554.java:5). The game login success block does not write it. The lobby success block writes it as `u16be` (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:348).

## Players

`opAudioThread46` calls `VarnBitType.callf97538dc` then `C5de4f69d.opkind12931`. If the buffer position then differs from the packet size it throws `gpi1` (builds/rs2/634-2010-12-10/src/src/main/java/deob/AudioThread.java:15).

There is no single empty-list terminator byte. Movement is four bit-passes, then the high and low index lists are rebuilt from indices 1 through 2047. Index 0 is not scanned (builds/rs2/634-2010-12-10/src/src/main/java/deob/VarnBitType.java:112).

Each pass starts the bit cursor, walks one index list, byte-aligns, and throws `nsn0`, `nsn1`, `nsn2`, or `nsn3` if a skip count remains (builds/rs2/634-2010-12-10/src/src/main/java/deob/VarnBitType.java:10).

| Pass | Index list | Selected when flag bit 0 is |
| --- | --- | --- |
| 1 | `Cb85a2d97.valuekind17716` length `C549db7f3.valuekind12371` | clear |
| 2 | same high list | set |
| 3 | `C02cf8d8f.valuekind11243` length `Cb2e62127.valuekind15123` | set |
| 4 | same low list | clear |

Flag array is `C084abe8f.valuekind12643`. For each index, a remaining skip count marks the player skipped (flag bit 1) and decrements the count. Otherwise one bit:

- 0: skip length from `C91aa4cea.opkind15084`, then mark skipped.
- 1: movement. Passes 1 and 2 call `Cfe056fdd.opkind11624`. Passes 3 and 4 call `Ccc8df238.opkind12181` and mark skipped when that call returns true.

Skip length (builds/rs2/634-2010-12-10/src/src/main/java/deob/C91aa4cea.java:79): 2 bits select the extra width.

| Selector | Extra bits |
| --- | --- |
| 0 | 0 |
| 1 | 5 |
| 2 | 8 |
| 3 | 11 |

After the four passes every flag byte is shifted right by one. A non-null player goes on the high list. A null slot goes on the low list.

### Movement `Cfe056fdd.opkind11624`

(builds/rs2/634-2010-12-10/src/src/main/java/deob/Cfe056fdd.java:23). Tile x is `valuekind140398`, tile z is `valuekind140395`.

1. One bit. Set means this index is pushed for masks (`Ca7665bfd.valuekind11503`).
2. Two bits, type:

Type 0, no further large move. If the mask bit is set, `valuekind160514` is cleared. If the mask bit is clear and the index is the local player, throw `s:lr`. Otherwise save a low-resolution record in `C7a346eec.valuekind123511` and null the player. Then one more bit: when it is set, call `Ccc8df238.opkind12181`.

Type 1, walk, 3-bit direction added to the current tile:

| Dir | dx | dz |
| --- | --- | --- |
| 0 | -1 | -1 |
| 1 | 0 | -1 |
| 2 | +1 | -1 |
| 3 | -1 | 0 |
| 4 | +1 | 0 |
| 5 | -1 | +1 |
| 6 | 0 | +1 |
| 7 | +1 | +1 |

Type 2, run, 4-bit direction, steps of 1 or 2 (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cfe056fdd.java:102):

| Dir | dx | dz | Dir | dx | dz |
| --- | --- | --- | --- | --- | --- |
| 0 | -2 | -2 | 8 | +2 | 0 |
| 1 | -1 | -2 | 9 | -2 | +1 |
| 2 | 0 | -2 | 10 | +2 | +1 |
| 3 | +1 | -2 | 11 | -2 | +2 |
| 4 | +2 | -2 | 12 | -1 | +2 |
| 5 | -2 | -1 | 13 | 0 | +2 |
| 6 | +2 | -1 | 14 | +1 | +2 |
| 7 | -2 | 0 | 15 | +2 | +2 |

Type 3, teleport, one subtype bit.

- Subtype 0: 12 bits. Plane delta is `bits >> 10` (2 bits). dx is 5 bits, signed (`value - 32` when `value > 15`). dz is the low 5 bits, same sign rule. Both are added to the current tile. Plane becomes `(plane + delta) & 3`. `C764d1a72.opkind13489` may increment the render plane. The local player's plane is copied to `Cbf3f03c1.valuekind13958`.
- Subtype 1: 30 bits. Plane delta is `bits >> 28`. World x is 14 bits at `>> 14`. World z is the low 14 bits. The tile is `(world + currentTile + base) & 16383`, then minus the base. Base x is `C7d96227f.valuekind12381`, base z is `Ca5bf8f3f.valuekind13813`.

When the mask bit is set, the new tile is stored in `valuekind160518` / `valuekind16059` and `valuekind160514` is set, instead of calling `opkind16058` immediately.

### External player `Ccc8df238.opkind12181`

(builds/rs2/634-2010-12-10/src/src/main/java/deob/Ccc8df238.java:18). Two type bits on the low-resolution record `C0710a1a8.valuekind17571`.

| Type | Bits | Effect |
| --- | --- | --- |
| 0 | 1, then 6, 6, 1 | Create the local player. The 1-bit recurses into this method when set. The 6-bit values are local x and z. The last bit requests a mask. Place at `(region << 6) + local` relative to the base. If `C10c06ec7.valuekind16329[index]` is already stored, apply it with `opkind16055`. |
| 1 | 2 | Add the 2-bit plane delta onto the external record. Return false. |
| 2 | 5 | 3 bits of plane delta in the high part, 3 bits of one-tile move. Directions 0..7 match the walk table above (0 is dx-1 dz-1 through 7 dx+1 dz+1). Return false. |
| 3 | 18 | `>> 16` plane delta, next 8 bits added to region x, low 8 bits added to region z. Return false. |

The packed record is plane in the top bits (`>> 28`), region x at `>> 14`, region z in the low 8 bits.

### Player masks

`C5de4f69d.opkind12931` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C5de4f69d.java:10) reads, per masked index:

1. `u8` mask.
2. If bit 6 (value 64) is set, another `u8` shifted 8.
3. If bit 14 (value 16384) of the accumulated mask is set, another `u8` shifted 16.

`LoadingRelated2.opLoadingRelated23` then consumes bits in this order (builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingRelated2.java:49):

| Bit | Widths |
| --- | --- |
| 1 | u16be, 65535 becomes -1, stored as `valuekind140361` |
| 65536 | u8 count. Each entry: u16le_add128; when `(value & 0xC000) == 0xC000`, also u16le; then u16le |
| 1024 | u16le, u8_sub128, u8_sub128. Low 15 bits of the short are kept; bit 15 is a flag |
| 4096 | cstr. A leading `~` is stripped. Chat duration 150 |
| 4 | u8_neg count, then per hit the smart sequence below, then smart, then u8_add128 |
| 2 | u16be face. Cleared after apply when the queue is empty |
| 32768 | u8_add128 == 1 stored as a boolean |
| 256 | u8_neg count. Each: u16be (65535 becomes -1), u8_neg, u16be |
| 8192 | i8, i8_add128, i8_neg, i8_neg, u16le_add128, u16be_add128, u8. Offsets are added to the current tile or to the pending exact tile |
| 512 | u16le_add128 (65535 becomes -1), u32le, u8_sub128 split into low 3 bits and `(value >> 3) & 15` (15 becomes -1) |
| 262144 | i8_neg, i8, i8, u8, u16le_add128, u16be_add128 |
| 128 | i8_add128 stored in `C74d5a544.valuekind13322[index]` |
| 32 | u16le_add128 (65535 becomes -1), u32_228, u8_add128 split into low 3 bits and `(120 & value) >> 3` (15 becomes -1) |
| 8 | four u16be_add128 (65535 becomes -1), then u8_add128 |
| 16 | u8 length, then `opLoadingScreenRelated229` copies that many bytes in reverse. The copy is kept on `C10c06ec7.valuekind16329[index]` and applied by `opkind16055` |
| 2048 | i8_neg. 127 means place on the exact tile; -1 means use `C74d5a544.valuekind13322`; any other value is the step override |

Hit block, bit 4, for each of the count (builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingRelated2.java:106): one smart. 32767 reads three further smarts. 32766 becomes -1 and reads no further type smarts. Any other value reads one more smart. Then one more smart and one `u8_add128` always follow.

### Appearance `C5b4806ed.opkind16055`

(builds/rs2/634-2010-12-10/src/src/main/java/deob/C5b4806ed.java:170). The buffer position is reset to 0. The mask calls it with argument 84, which passes the gates inside the reads.

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | u8 | bit 0 gender; bit 1 a boolean; bit 2 selects the extended block; `((value & 63) >> 3) + 1` is the size passed to `opkind140312`; `>> 6 & 3` |
| 2 | i8 ×4 | the fourth byte equals 1 for a boolean |
| 3 | 12 slots | u8 0 means empty. Otherwise u8 high and u8 low. Slot 0 value 65535 stops the loop: next u16be is an npc id and the next u8 is stored, and the remaining slots are skipped. A value at least 32768 is an obj id `value - 32768`. A smaller value is a kit `value - 256` |
| 4 | u8 ×5 | colours. Out-of-range values become 0 |
| 5 | u16be | render/animation id |
| 6 | cstr | name, also copied to the display name |
| 7 | u8 | combat level |
| 8 | u16be if bit 2 is set, else u8 and u8 | when bit 2 is set the short becomes -1 at 65535 and the other level field is the combat u8 already read. When bit 2 is clear the extended field is 0, the next u8 is a level, and the following u8 becomes -1 at 255 |
| 9 | u8 | extra count. 0 clears the extras. Non-zero reads four u16be and one u8 |

## NPCs

`GnpMis.call59622295` calls `C09c2ddcc.opkind12781`, then `Build.opkind16332`, then `C078a9aea.opkind177816`. The position must then match the packet size (`gnp1`). List checks throw `gnp2`, `gnp3`, and `gnp4` (builds/rs2/634-2010-12-10/src/src/main/java/deob/GnpMis.java:47).

### Existing `C09c2ddcc.opkind12781`

(builds/rs2/634-2010-12-10/src/src/main/java/deob/C09c2ddcc.java:11). Starts the bit cursor and does not byte-align before the add loop.

1. 8-bit count. A count above the current NPC count throws `gnpov1`. A shorter count queues the tail indices for removal.
2. For each of the count, one bit:
   - 0: keep, no mask.
   - 1: two more bits.
     - 0: keep and queue a mask.
     - 1: 3-bit direction passed to `Cfbaee89a.opkind17738(dir, 1)`, then one mask bit.
     - 2: one bit. When it is 1, two 3-bit directions are each passed to `opkind17738` with step argument 2. When it is 0, one 3-bit direction is passed with step argument 0. Then one mask bit.
     - 3: remove.

`opkind17738` was not read, so the NPC direction-to-delta table is unread. See [gaps.md](gaps.md).

### Add `Build.opkind16332`

(builds/rs2/634-2010-12-10/src/src/main/java/deob/Build.java:66). While at least 15 bits remain:

1. 15-bit index. 32767 stops the loop. That is the empty-add terminator.
2. Otherwise: 2-bit plane, 1 bit, 5-bit signed dx (`value - 32` when `value > 15`), 5-bit signed dz with the same rule, 3 bits used as `(15617 & ((4 + bits) << 11))` for the facing of a newly created NPC, 1 mask bit, 14-bit type id.

dx is added to the local player's tile x `C16d7fbde.valuekind15903.valuekind140398[0]`. dz is added to tile z `valuekind140395[0]`. The 1-bit is passed through as a boolean (`value == 1`). The type id is resolved with `Ce41dbe0e.valuekind15247.opkind12018`.

Then byte-align (builds/rs2/634-2010-12-10/src/src/main/java/deob/Build.java:126).

### NPC masks `C078a9aea.opkind177816`

(builds/rs2/634-2010-12-10/src/src/main/java/deob/C078a9aea.java:90). Per masked index: `u8`, and if bit 4 (value 16) is set, another `u8` shifted 8. There is no third mask byte. Bits are consumed in this order:

| Bit | Widths |
| --- | --- |
| 2 | u16be_add128 type id, then the type is reloaded |
| 8 | four u16be (65535 becomes -1), then u8_sub128 |
| 32 | u16le (65535 becomes -1), u32_225, u8. Low 3 bits and `(value & 121) >> 3` (15 becomes -1) |
| 16384 | u8_neg count. Each: u16be (65535 becomes -1), u8_neg, u16le_add128 |
| 512 | u8_sub128 count. Each: u16be_add128; when `(value & 0xC000) == 0xC000`, also u16be; then u16be |
| 4 | u16be_add128, u16le |
| 4096 | i8_add128, i8_add128, i8, i8_add128, u16le, u16be_add128, u8_sub128 |
| 8192 | i8_neg, i8_add128, i8_neg, u8, u16be, u16be |
| 64 | u8_sub128 count, then the same smart hit pattern as player bit 4, except the final byte is `u8` rather than `u8_add128` |
| 128 | u16be, 65535 becomes -1 |
| 1 | cstr. Chat duration 100 |
| 2048 | u16be, u8_sub128, u8_sub128. Low 15 bits kept; bit 15 is a flag |
| 1024 | u16le (65535 becomes -1), u32le, u8_sub128 split into low 3 bits and `(value & 126) >> 3` (15 becomes -1) |

Hit smarts for bit 64 (builds/rs2/634-2010-12-10/src/src/main/java/deob/C078a9aea.java:217): one smart. 32767 reads three further smarts. 32766 becomes -1. Any other value reads one more smart. Then one smart and one `u8`.
