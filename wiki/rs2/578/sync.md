# Player and NPC sync

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`. Bit reads are `ParticleEmitterRelated.opkind8135`. `opkind8136` sets the bit cursor to `byteOffset * 8`. `opkind8132` sets the byte offset to `(bitCursor + 7) / 8`.

## Players

Opcode 72, size -2, `Cc7b9a9fb.valuekind9575` (`deob/Cc7b9a9fb.java:5`). The branch is `deob/C268455a9.java:350`. It calls `DiesesSystemDarfNichtMissbrauchtWerden.opkind7151(buffer, payloadLength)` (`deob/DiesesSystemDarfNichtMissbrauchtWerden.java:13`), which calls `C0783558e.callf97538dc` and then `C565d1093.opkind7472`. The byte offset must equal the payload length or it throws `gpi1`.

Local player index is the u16be from login, `FullscreenMode.valueFullscreenMode4`. The rebuild loop uses indices 1 through 2047 (`deob/C0783558e.java:206`).

`callf97538dc` (`C0783558e.java:105`) runs four blocks. Each block starts with `opkind8136` and ends with `opkind8132`. A leftover skip count throws `nsn0`, `nsn1`, `nsn2`, or `nsn3`.

The active list is `C36f4c4c6.valuekind7841`, length `C86e345bf.valuekind75916`. The inactive list is `C6defb8da.valuekind7821`, length `C950f0e92.valuekind6875`. Per-index flags are `Cd09638cd.valuekind6415`.

| block | who | test |
| --- | --- | --- |
| 1 | active list | `(flags & 1) == 0` |
| 2 | active list | `(flags & 1) != 0` |
| 3 | inactive list | `(flags & 1) != 0` |
| 4 | inactive list | `(flags & 1) == 0` |

Inside a block, one bit. Zero starts a skip: `Ceea057f0.opkind10142` (`deob/Ceea057f0.java:111`) reads 2 bits, then 0, 5, 8, or 11 bits for selectors 0, 1, 2, and 3. Each skipped index gets `flags \|= 2` and does not read a movement. One means movement: active indices call `C798f1741.opkind9443`; inactive indices call `C6402f7a6.opkind7664`, and a true return also sets `flags \|= 2`.

After the blocks, every index from 1 to 2047 shifts `flags >>= 1`. A non-null `C7a346f71.valuekind9151[index]` is appended to the new active list. A null slot is appended to the new inactive list.

### Existing player

`C798f1741.opkind9443` (`deob/C798f1741.java:142`):

- 1 bit. If set, the index is pushed for masks.
- 2 bits:
  - 0: remove. The local player throws `s:lr`. Otherwise the client stores `(plane << 28) \| ((baseX + tileX) >> 6 << 14) \| ((baseZ + tileZ) >> 6)` in `C5afcc012.valuekind10056`, saves facing and the interaction word, clears the player, then reads 1 bit. If that bit is set, `opkind7664` runs immediately.
  - 1: 3-bit walk. Directions from the source: 0 decreases x and z, 1 decreases z, 2 increases x and decreases z, 3 decreases x, 4 increases x, 5 decreases x and increases z, 6 increases z, 7 increases both. If the mask bit was set, the destination is stored and applied after masks. Otherwise `opkind9817` steps now.
  - 2: 4-bit run (`C798f1741.java:215`). x is `valuekind101869` and z is `valuekind101873`. The 16 values change them by: 0 (-2,-2), 1 (-1,-2), 2 (0,-2), 3 (+1,-2), 4 (+2,-2), 5 (-2,-1), 6 (+2,-1), 7 (-2,0), 8 (+2,0), 9 (-2,+1), 10 (+2,+1), 11 (-2,+2), 12 (-1,+2), 13 (0,+2), 14 (+1,+2), 15 (+2,+2).
  - 3: 1 bit. If clear, 12 bits: plane delta in bits 10..11, x delta in bits 5..9, z delta in bits 0..4. Deltas above 15 subtract 32. x and z add to the current tile. Plane is `(plane + delta) & 3`. If the bit is set, 30 bits: plane `>> 28`, x `(>> 14) & 16383`, z `& 16383`, added to base plus current tile, then masked with 16383 and the base subtracted again (`C798f1741.java:325`).

### New or relocated player

`C6402f7a6.opkind7664` (`deob/C6402f7a6.java:52`) reads 2 bits:

- 0: 1 bit. If set, the method calls itself before the rest. Then 6-bit x, 6-bit z, 1-bit mask flag. A set mask flag queues the index. The region word `C5afcc012.valuekind10056` splits as plane `>> 28`, region-x `(>> 14) & 255`, region-z `& 255`. World tile x is `(regionX << 6) + dx - baseX`. World tile z is `(regionZ << 6) + dz - baseZ`. A player already in the slot throws `hr:lr`. A saved appearance buffer, if any, is applied with `opkind9815`.
- 1: 2-bit plane delta added to the stored region word. Returns false, so the inactive flag is not marked.
- 2: 5 bits. The top 2 add to the plane. The low 3 are a one-tile step of the region-x and region-z bytes. Returns false.
- other: 18 bits. Top 2 add to the plane, next 8 add to region-x, low 8 add to region-z. Returns false.

### Masks

`C565d1093.opkind7472` (`deob/C565d1093.java:54`) reads u8. If bit `0x2` is set, it reads another u8 and adds it as the high byte. Bodies are `C3f2a4ef6.opkind7775` (`deob/C3f2a4ef6.java:79`), in this test order:

| bit | fields |
| --- | --- |
| 0x20 | i8_m128, stored as the step direction |
| 0x8 | u16be_m128, u8neg, u8 length, `bytes_rev_m128(length)`. Bit 15 of the u16 marks a quick-chat payload. The client rewinds to `offset + length` even when it skips the text. |
| 0x400 | u8_128m count, then that many of u16le (65535 becomes -1), u8neg, u16le |
| 0x1 | u8_128m length, `bytes_rev(length)`, then `C057d85be.opkind9815` |
| 0x10 | u16be_m128. 65535 becomes -1. Stored as `valuekind101836`. |
| 0x4 | u16le_m128 (65535 becomes -1), u8_m128 |
| 0x100 | i8_m128, i8_m128, i8_128m, i8, u16be_m128, u16be_m128, u8neg |
| 0x1000 | u16be, u8_m128, u8_128m. The low 15 bits of the u16 are an id. Bit 15 is a flag. |
| 0x80 | u16be, stored as `valuekind9819` and applied as a facing when the player is not mid-step |
| 0x200 | jstr. A leading byte 126 (`~charAt(0) == -127`) drops that character. |
| 0x2000 | u16le_m128 (65535 becomes -1), u32be. The high 16 bits of the u32 are stored separately from the low 16, which is added to the cycle. |
| 0x40 | smart, u8_128m, then later u8neg |
| 0x800 | smart, u8_128m |
| 0x4000 | i8_128m. This byte is not applied until every other bit has been read. 127 uses the pre-move coordinate pair directly. |

### Appearance

`C057d85be.opkind9815` (`deob/C057d85be.java:217`) resets the offset to 0. The bytes were stored reversed, so this walk is the server order:

- u8 flags. Bit 0 is `valuekind98110`. Bit 1 selects a second name. Bit 2 selects the combat-word form below. Bits 3..5 are `((flags >> 3) & 7) + 1`, passed to `opkind10183`. Bits 6..7 are stored in `valuekind98126`.
- three i8
- 12 slots. Each slot is u8. Zero stores 0. Nonzero reads another u8 and forms `(first << 8) + second`. Slot 0 equal to 65535 is an npc transform: u16be id, u8 extra, and the remaining slots are skipped. A value at least 32768 looks up `C7a346f71.valuekind9157[value - 32768]` and stores `0x40000000 | that`. A smaller value stores `value - 256` with the high bit `0x80000000` set.
- 5 times u8 colour index
- u16be `valuekind98120`
- jstr name. If bit 1 is clear, that string is also `valuekind9812`. If bit 1 is set, another jstr is `valuekind9812`.
- u8 `valuekind98111`
- If bit 2 is set: u16be into `valuekind9818`, and `valuekind98118 = -1`. If clear: u8 into `valuekind9817`, u8 into `valuekind98118` (255 becomes -1).
- u8 `valuekind98113`. If nonzero: four u16be and one u8 (`valuekind9814`, `valuekind98114`, `valuekind9815`, `valuekind98121`, `valuekind98128`).

## NPCs

Opcode 37, size -2, calls `VousTesPasAutorisRejoindreLeCanalDeClanDeC.call59622295(true)` (`deob/C268455a9.java:1061`, descriptor `deob/C60b05013.java:4`). Opcode 32, size -2, calls it with false (`C268455a9.java:1421`, descriptor `deob/Cee94503e.java:36`). The boolean is the large-delta flag. After the three steps, the byte offset must equal the payload length or it throws `gnp1` (`deob/VousTesPasAutorisRejoindreLeCanalDeClanDeC.java:29`).

### Existing

`C094282f0.opkind6811` (`deob/C094282f0.java:97`):

- 8-bit count. A count above the current npc list throws `gnpov1`. A short count appends the missing indices to the remove list.
- For each counted npc, 1 bit. Zero keeps the npc and sets its cycle, with no mask. One reads 2 bits:
  - 0: keep, and queue a mask
  - 1: 3-bit direction passed to `opDxGraphicsDeletable4(1, dir)`, then 1 bit mask
  - 2: 1 bit. Clear reads one 3-bit direction with speed argument 0. Set reads two 3-bit directions, each with speed argument 2. Then 1 bit mask.
  - 3: remove

### Added

`Cb4a619ef.opkind7532` (`deob/Cb4a619ef.java:42`) loops while `opkind8138(payloadLength) >= 15`. The next 15 bits are the index. 32767 ends the loop. Otherwise:

- first signed delta, 5 bits when the packet flag is false and 8 bits when it is true. A 5-bit value above 15 subtracts 32. An 8-bit value above 127 subtracts 256.
- second signed delta, 8 bits when the flag is true and 5 bits when it is false, with the same sign fold
- 1 bit mask queue
- 14-bit type id, loaded through `VarnBitType.valuekind6775.opJs5ResourceProvider30`
- 3-bit angle. The client stores `(angle + 4) << 11`, masked with 14633, on first sight.
- 2 bits passed as the third argument of `opDxGraphicsDeletable5`
- 1 bit passed as the teleport flag

`opDxGraphicsDeletable5` is called as `(size, twoBits, localPlayer.valuekind101869[0] + secondDelta, teleport, localPlayer.valuekind101873[0] + firstDelta)` (`Cb4a619ef.java:104`).

### Masks

`Js5GlTextureProvider.opJs5GlTextureProvider17` (`deob/Js5GlTextureProvider.java:166`) reads u8. If bit `0x40` is set, it reads another u8 and adds it as the high byte. Test order:

| bit | fields |
| --- | --- |
| 0x400 | u16be_m128, u8_128m, u8_128m. Low 15 bits are an id. Bit 15 is a flag. |
| 0x2 | smart, u8neg, u8 |
| 0x8 | u16be (65535 becomes -1), u8_128m |
| 0x80 | jstr |
| 0x4 | u16le_m128 (65535 becomes -1), u32le. High 16 bits of the u32 are stored apart from the low 16, which is added to the cycle. |
| 0x20 | u16be. 65535 becomes -1. Stored as `valuekind101836`. |
| 0x1 | smart, u8neg |
| 0x100 | u8_128m count, then that many of u16be_m128 (65535 becomes -1), u8neg, u16be |
| 0x200 | u16be, u16le |
| 0x10 | u16le type id, reloaded through `opJs5ResourceProvider30` |
