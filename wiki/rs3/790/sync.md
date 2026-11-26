# Sync

Local player index is `Client.tint75`. It starts at -1 (`rs2/client/Client.java:962`) and the lobby success block stores the `u16be` there (`deob/Statics.java:3179`). The high-resolution list and the low-resolution list both cover indices 1 through 2047. Index 0 is outside those loops.

`rslib/src/both/move/rev_790.c` uses a different local-player skip and forces the one-step extra bit clear. The Java lines are the spec. That disagreement is in [gaps.md](gaps.md).

## Initial snapshot

`ObjIconFactory.doObjIconFactory` (`com/jagex/game/world/entity/ObjIconFactory.java:49`) runs on the front of the first post-login packet and on the opcode-15 payload (`deob/Statics.java:3309`, `Statics.java:3351`).

Bit mode. 30 bits for the local player: plane is bits 31..28, x is bits 27..14, z is bits 13..0. Scene x is world x minus base `tint2`. Scene z is world z minus base `tint3`.

Then, for every index from 1 through 2047 except `tint75`, 18 bits: plane 2, region x 8, region z 8, stored as `(plane << 28) + (regionX << 14) + regionZ` on `AccountCreationManager`. The loop count is the terminator. Byte-align after the loop.

## Player update

Server opcode 126 (`cls1f645fa6127`, size `-2`) calls `Statics.doStatics(buf, length)` (`Client.java:8294`, `Statics.java:2061`). That calls `doObjIconFactory2` and `doObjIconFactory222`, then throws if the offset differs from the length.

`doObjIconFactory2` (`ObjIconFactory.java:113`) is four bit-aligned passes:

1. High-resolution players whose flag bit 0 is clear. A set bit calls the high-resolution move.
2. High-resolution players whose flag bit 0 is set.
3. Low-resolution players whose flag bit 0 is set. A set bit calls the low-resolution move, and a true return marks the player high-resolution.
4. Low-resolution players whose flag bit 0 is clear.

A clear present bit reads a skip from `doObjIconFactory22` (`ObjIconFactory.java:242`): 2 bits select the width, 0 reads nothing more (skip 0), 1 reads 5 bits, 2 reads 8 bits, and the remaining case reads 11 bits. After the passes, indices 1 through 2047 are rebuilt. A null player slot is low-resolution.

### High-resolution move

`doObjIconFactory(buf, index)` (`ObjIconFactory.java:259`): 1 bit needs-mask, then 2-bit type.

Type 0 with the needs-mask bit clear removes the player to low resolution. The stored region is built from the current scene tile and base. A following 1 bit, when set, reads another low-resolution record. Type 0 with the needs-mask bit set leaves the player in place.

Type 1: 3-bit direction, then 1 bit. When that bit is set, a 2-bit extra step is applied before the 3-bit step (`ObjIconFactory.java:301`, then `ObjIconFactory.java:318`). Directions, as `(dx, dz)`: 0 is (-1,-1), 1 is (0,-1), 2 is (+1,-1), 3 is (-1,0), 4 is (+1,0), 5 is (-1,+1), 6 is (0,+1), 7 is (+1,+1). Extra step: 0 is z+1, 1 is x-1, 2 is x+1, 3 is z-1.

Type 2: 4-bit run. Deltas from the current tile: 0=(-2,-2), 1=(-1,-2), 2=(0,-2), 3=(+1,-2), 4=(+2,-2), 5=(-2,-1), 6=(+2,-1), 7=(-2,0), 8=(+2,0), 9=(-2,+1), 10=(+2,+1), 11=(-2,+2), 12=(-1,+2), 13=(0,+2), 14=(+1,+2), 15=(+2,+2).

Type 3: 1 bit. Clear reads 15 bits: `>> 12 & 7`, `>> 10 & 3` added to the plane, then two signed 5-bit deltas (`>> 5 & 31` and `& 31`, values above 15 subtract 32). Set reads 3 bits plus 30 bits: `>> 28 & 3` plane, `>> 14 & 16383` x, `& 16383` z, wrapped with the current world tile at 16383 (`ObjIconFactory.java:426`).

### Low-resolution move

`doObjIconFactory2(buf, index)` (`ObjIconFactory.java:455`) returns a boolean.

Type 0: optional nested call when a bit is set, then 6-bit local x, 6-bit local z, 1-bit needs-mask. The player is placed at `(regionX << 6) + localX - baseX` and `(regionZ << 6) + localZ - baseZ` and becomes high-resolution.

Type 1: 2-bit plane add. Stays low-resolution.

Type 2: 5 bits. The high 2 bits add to the plane. The low 3 bits use the same 0 through 7 direction table, applied to the stored region bytes. Stays low-resolution.

Type 3: 20 bits. `>> 18 & 3` is stored minus 1, `>> 16 & 3` adds to the plane, `>> 8 & 255` adds to region x, and the low 8 bits add to region z. Stays low-resolution (`ObjIconFactory.java:560`).

## Player masks

`doObjIconFactory222` (`ObjIconFactory.java:580`) walks the players that set the needs-mask bit. Each starts with an ignored `u16be`, then `u8` flags. Bit `0x20` adds another `u8` at bits 8..15. Bit `0x1000` adds another `u8` at bits 16..23. The flag word stops there. Bit `0x200000` is bit 21 of that word.

Wire order of the mask tests (`ObjIconFactory.java:601`): 64, 131072, 16384, 262144, 2097152, 2048, 8, 524288, 32768, 256, 128, 4, 1024, 1048576, 4194304, 65536, 1, 8192, 16, 2.

| Bit | Body |
| --- | --- |
| 64 | `u8` hit count. Per hit, `smart-u`. 32767 reads four more `smart-u`. 32766 reads `u8-128`. Other values read one more `smart-u`. Then one more `smart-u`. Then `neg-u8` headbar count with the same `smart-u` shapes (`ObjIconFactory.java:601`) |
| 131072 | skip 2 bytes, `u8` count, per entry `u8` plus `doDataBlock1874` (`ObjIconFactory.java:644`). The helper was not read |
| 16384 | `u16be-m128` (65535 becomes -1), `u32scr-a`, `u8-128` packed (`ObjIconFactory.java:657`) |
| 262144 | `u16le` (65535 becomes -1), `u32scr-a`, `u8` (`ObjIconFactory.java:675`) |
| 2097152 | `u8-128` compared with 1 (`ObjIconFactory.java:693`) |
| 2048 | `u8` length, then that many bytes each minus 128 (`doPositionedSound232`, `ObjIconFactory.java:697`). Decoded by `IntMathPart.doIntMathPart2` |
| 8 | `128-i8`, `neg-i8`, `neg-i8`, `i8-128`, two `u16be`, one `u16be` (`ObjIconFactory.java:706`) |
| 524288 | `jstr`, then `128-u8` (`ObjIconFactory.java:722`) |
| 32768 | `u8` count, per entry `smart32-1`, `neg-u8`, `u16be` (`ObjIconFactory.java:732`) |
| 256 | `neg-u8` compared with 1 (`ObjIconFactory.java:747`) |
| 128 | `u16be` (`ObjIconFactory.java:751`) |
| 4 | `u16be`, 65535 becomes -1 (`ObjIconFactory.java:759`) |
| 1024 | `jstr` (`ObjIconFactory.java:768`) |
| 1048576 | skip 2, `u8` count, per entry `128-u8` plus `doDataBlock1874` (`ObjIconFactory.java:777`) |
| 4194304 | `128-u8` count. Per entry `u16be`, and when `(id & 0xC000) == 49152` another `u16be-m128`, then `u16le-m128` (`ObjIconFactory.java:789`) |
| 65536 | `i8`, `i8-128`, `i8`, `u8`, `u16le`, `u16be-m128` (`ObjIconFactory.java:809`) |
| 1 | `u16le-m128` (65535 becomes -1), `u32scr-b`, `128-u8` (`ObjIconFactory.java:818`) |
| 8192 | `u16le` (65535 becomes -1), `u32scr-b`, `u8-128` (`ObjIconFactory.java:836`) |
| 16 | appearance, below (`ObjIconFactory.java:854`) |
| 2 | four `smart32-1`, then `neg-u8` (`ObjIconFactory.java:863`) |

### Appearance

Mask value 16 reads `128-u8` length and then that many bytes in reverse (`doPositionedSound22`, `PositionedSound.java:789`). `IntMathPart.doIntMathPart(PositionedSound)` (`deob/IntMathPart.java:181`) reads:

1. `u8` flags. Bit 0 is the gender byte. Bit 2 selects the extended skill block. Bits 3..5 plus 1 are the size. Bit 6 and bit 7 each select a `vstr`.
2. A signed byte must equal 1 (`doPositionedSound23`).
3. `doIntMathPart(buf, gender)` (`IntMathPart.java:78`). The slot count is `Statics.clsc3b697c8.intArray.length`. `DataBlock2368` is constructed from the defaults archive (`Statics.java:3690`). That length was not read. For each slot whose table value is other than 1: `u8`, and 0 means empty. A nonzero `u8` plus a second `u8` makes a `u16be` id. If slot 0's id is 65535, the loop breaks and reads `smart32-1` plus a `u8`. Ids at least 16384 are `(id - 16384) | 0x40000000`. Other ids are `(id - 256) | 0x80000000`.
4. When slot 0 did not break on 65535: `u16be` colour-slot mask. Each set bit calls `ClientScriptHelpers.doClientScriptHelpers`. That helper was not read.
5. Ten colour `u8`s.
6. `u16be` render animation id.

Back in the outer method: `jstr` name, `u8` tint. When bit 2 is set: `u16be` (65535 becomes -1) and the previous tint is copied. Otherwise two `u8`s, and the second 255 becomes -1. Then `u8` skill count. A nonzero count reads five `u16be` and a `u8`.

The second appearance buffer, bit 2048, is `IntMathPart.doIntMathPart2` (`IntMathPart.java:260`): `u8` mask over `this.intArray.length`, and each set bit is a `u8` and a `u16be`. `intArray.length` was not read.

## NPCs

Server opcode 67 (`cls1f645fa668`, size `-2`) calls `Client.doClient23(true)` (`Client.java:9696`). Server opcode 159 (`cls1f645fa6160`, size `-2`) calls `Client.doClient23(false)` (`Client.java:8713`). `doClient23(boolean)` (`Client.java:10436`) calls the existing-move header, `Statics.doStatics23(large)`, and `Statics.doStatics2322b5`. The boolean is the large-coordinate flag: true uses 8-bit deltas and false uses 5-bit deltas.

### Existing NPCs

`doClient2322b122` (`Client.java:10513`): 8-bit count, which must be at most the current count. Per npc, 1 bit kept. Clear leaves the npc with no further bits. Set reads a 2-bit type:

| Type | Bits |
| --- | --- |
| 0 | stay, and the npc needs a mask |
| 1 | one 3-bit `LocType` step with `FacetMode.facetMode3`, then 1-bit mask |
| 2 | a bit selects two 3-bit steps with `facetMode4`, or one 3-bit step with `facetMode2`, then 1-bit mask |
| 3 | remove |

The 3-bit value is looked up in `LocType.doLocType()` (`com/jagex/game/config/loctype/LocType.java:30`). The eight values, as `(tint, tint2)`, are (5,0), (2,1), (0,2), (1,3), (4,4), (7,5), (3,6), (6,7). The tile delta is `IntMathPart2.doIntMathPart2`. That method was not read.

### New NPCs

`Statics.doStatics23` (`Statics.java:8603`): while at least 15 bits remain, read a 15-bit index and stop on 32767. Then a signed delta (`8` bits when large, subtracting 256 above 127; `5` bits otherwise, subtracting 32 above 15), a 1 bit, 2 bits, and the other delta at the same width. The first delta is added to the local player's z (`intArray10`). The second is added to x (`intArray9`) (`Statics.java:8671`). Then 3 bits plus 4, shifted left 11, masked with 16383, a 1-bit needs-mask, and a 15-bit npc type id. Byte-align after the loop.

### NPC masks

`doStatics2322b5` (`Statics.java:8697`): ignored `u16be`, then `u8` flags. Bit `0x8` adds a byte at bits 8..15. Bit `0x1000` adds a byte at bits 16..23. Bit `0x200000` adds a byte at bits 24..31. The per-bit bodies that start at `Statics.java:8717` were not read.
