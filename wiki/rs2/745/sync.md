# Player and NPC update

S2C opcode 1, size `-2`, calls `MapElement.opkind36705` (`Client.java:9014-9015`). The handler checks that the buffer offset equals the packet size (`MapElement.java:69-70`). S2C opcode 99, size `-2`, calls `Statics.opStatics118(false)` (`Client.java:8082-8083`). S2C opcode 59, size `-2`, calls `Statics.opStatics118(true)` (`Client.java:8922-8923`). Both NPC packets return from the packet loop with true. `opStatics118` bumps `valueClient1825`, then calls `opClient551`, `opClient618`, and `opClient606` (`Statics.java:4723-4729`).

Bit order is MSB first. See [framing.md](framing.md).

There is no zero-index terminator on the player list. Skip runs use a 2-bit code over prebuilt index lists of 2048 slots (`MapElement.java:1-8`). NPCs being added stop on index 32767.

## Local player during login

`MapElement.opkind36703` is not the in-game packet. It reads a 30-bit value: plane is `>> 28`, x is `>> 14 & 16383`, z is `& 16383`, then subtracts the scene base (`MapElement.java:18-29`). For every other index from 1 to 2047 it reads 18 bits: plane `>> 16`, region x `>> 8 & 255`, region z `& 255`, packed as `(plane << 28) + (rx << 14) + rz` (`MapElement.java:45-53`). The call sites are inside the login flow at `Statics.java:2741` and `Statics.java:2782`. Those surrounding bytes were not read.

## In-game players

`Statics.opStatics149` walks four lists (`Statics.java:1046-1158`):

1. Existing high-res indices whose flag bit 0 is clear (`MapElement.valuekind36706`, count `valuekind367010`).
2. Existing high-res indices whose flag bit 0 is set.
3. Outside indices whose flag bit 0 is set (`valuekind36705`, count `valuekind36707`).
4. Outside indices whose flag bit 0 is clear.

Each slot is one bit. `0` starts a skip: 2 bits, then 0, 5, 8, or 11 further bits for skip counts 0, that 5-bit value, that 8-bit value, or that 11-bit value (`MapElement.opkind36707`, `MapElement.java:74-85`). `1` updates the slot. A skip that does not land on zero throws (`Statics.java:1071-1072`). After the four passes the counts are cleared and indices 1 through 2047 have their flag byte shifted right by one. A non-null player is appended to the high-res list. A null slot is appended to the outside list (`Statics.java:1154-1165`). Index 0 is not in that loop.

### Movement of an existing player

`MapElement.opkind36704` (`MapElement.java:90-277`). One bit queues a later mask. Then 2 bits:

| Code | Bits |
| --- | --- |
| 0 | no step. If the mask bit was clear, one more bit: 1 calls the add routine, 0 removes the player. Removing the local index throws (`MapElement.java:99-123`) |
| 1 | 3-bit walk. Deltas on `(x, z)` for codes 0..7 are `(-1,-1), (0,-1), (1,-1), (-1,0), (1,0), (-1,1), (0,1), (1,1)` (`MapElement.java:130-150`) |
| 2 | 4-bit run. Deltas are two tiles, codes 0..15, the first five along z-2 from x-2 through x+2, then the same pattern one row lower, then the same pattern two rows lower (`MapElement.java:164-207`) |
| 3 | 1 bit. `0` is 12 bits: plane delta 2 bits, then 5-bit signed x and 5-bit signed z (`MapElement.java:219-231`). `1` is 30 bits: plane `>> 28`, absolute x `>> 14 & 16383`, absolute z `& 16383`, then rebased (`MapElement.java:253-258`) |

The 3-bit and 4-bit codes are the deltas above. `C42f50a33` is not used for players.

### Adding a player that is not on screen

`MapElement.opkind36708` (`MapElement.java:280-388`). 2 bits:

| Code | Effect |
| --- | --- |
| 0 | optional recursive call if the next bit is 1, then 6-bit x, 6-bit z, and 1 mask bit. The stored region coordinate supplies plane and the 8-bit region x/z. Scene tile is `(rx << 6) + x - baseX` and the same for z (`MapElement.java:282-321`) |
| 1 | 2-bit plane delta added to the stored plane (`MapElement.java:326-329`) |
| 2 | 5 bits: 2-bit plane delta and a 3-bit neighbour step on the stored region x/z (`MapElement.java:331-376`) |
| 3 | 18 bits, same layout as the login 18-bit record, added to the stored coordinate (`MapElement.java:378-386`) |

Only code 0 creates the `PlayerEntity`. The other codes return false and stay off the high-res list.

### Player masks

After movement, `opkind36702` reads a mask for each queued index (`MapElement.java:391-404`). The header is a u8. If bit 7 (128) is set, another u8 is added at `<< 8`. If bit 14 (16384) is set, another u8 is added at `<< 16`. The check order in `opkind36701` (`MapElement.java:409-688`) is not bit-number order:

| Bit | Reads |
| --- | --- |
| 512 | u8neg, stored as `== 1` (`411-413`) |
| 1048576 | u8 count, then that many times u8add128 plus `Ca0827813.opkind37734` (`415-423`). The inner method was not opened |
| 8 | u16le, u32_421, u8sub128 (`426-441`) |
| 1 | u16le_add128 (`444`) |
| 32 | u16be_add128, 65535 becomes -1 (`452-458`) |
| 131072 | u16le_add128, u32be, u8 (`461-476`) |
| 65536 | u8add128 count, then that many u16le, an extra u16le when `(word & U+C000) == 49152`, then another u16le (`479-496`) |
| 262144 | jstr, u8 (`499-506`) |
| 1024 | i8add128, i8, i8neg, i8add128, u16le_add128, u16le_add128, u16le (`509-516`) |
| 16 | one bigsmart per `C33282969.opkind34741()` entry, then u8add128 (`534-542`). The array length was not opened |
| 64 | u8neg length, then that many bytes each minus 128, decoded by `PlayerEntity.opkind40722` (`545-551`) |
| 256 | i8neg, replaces the default move speed (`554-556`) |
| 2048 | jstr (`558`) |
| 4096 | u8neg count, then that many bigsmart, u8neg, u16le (`567-579`) |
| 32768 | the source test is `(var3 & '耀')`. u16le_add128, u32_461, u8add128 (`582-597`) |
| 2097152 | u16be, u32_430, u8 (`600-615`) |
| 8388608 | u8sub128 `== 1` (`618-620`) |
| 4194304 | u8 count, then that many u8neg plus `opkind37734` (`622-631`) |
| 524288 | i8neg, i8add128, i8add128, u8, u16be, u16le_add128 (`634-640`) |
| 2 | u8neg count of hits, then smart values. 32767 reads four further smarts. 32766 reads one u8neg instead of a second smart. Then one more smart. A second count is u8sub128 and uses smarts, with 32767 meaning remove (`643-683`) |
| 4 | i8sub128 stored as the per-player move speed (`686-688`) |

The default speed byte is `C33282969.valuekind34745.valuekind34743` (`MapElement.java:410`). That constant was not opened. If a mask bit queued a coordinate and this speed byte is 127, the player is placed immediately (`MapElement.java:690-692`).

### Appearance, bit 64

`PlayerEntity.opkind40722` (`PlayerEntity.java:40-179`):

| Order | Width |
| --- | --- |
| 1 | u8 flags. Bit 0 and bit 1 are stored. Bit 2 selects the extra block below. Bits 3..5 plus 1 are the size. Bit 6 is a vjstr. Bit 7 is a vjstr |
| 2 | i8, i8, i8 `== 1` |
| 3 | one slot per `Statics.valueStatics590.valuekind35971`. Slots whose table value is not 1 read a u8. 0 means empty. Any other value reads a second u8 and forms `(first << 8) + second`. Slot 0 equal to 65535 reads a bigsmart and a u8 and stops the loop. A value at least 16384 is `value - 16384` with bit `0x40000000`. A smaller value is `value - 256` with bit `0x80000000` |
| 4 | if the 65535 branch was not taken, a u16be selects which slots with table value 0 call `ObjTypeCustomisation.opkind37301`. That method was not opened |
| 5 | 10 times u8 |
| 6 | u16be, jstr, u8 |
| 7 | if flag bit 2 is set: u16be, and 65535 becomes -1. Otherwise two u8, and a second byte of 255 becomes -1 |
| 8 | u8. If it is not 0: u16be, u16be, u16be, u16be, u8 |

`valuekind35971.length` was not read. `WearposDefaults` is constructed at `Loading.java:307` and was not opened.

## NPCs

`opClient551` (`Client.java:9631-9691`): u8 bit count of NPCs to keep. A larger count than the current list throws. Indices past the count are removed. For each kept index, one bit: `0` refreshes the cycle only. `1` then 2 bits:

| Code | Bits |
| --- | --- |
| 0 | queue a mask |
| 1 | 3-bit direction through `C42f50a33.opkind34721`, walked at `C33282969.valuekind34741`, plus one mask bit. The direction enum was not opened |
| 2 | one bit. `1` is two 3-bit steps at `valuekind34744`. `0` is one 3-bit step at `valuekind34742`. Then one mask bit |
| 3 | remove |

`opClient618` (`Client.java:9694-9768`) loops while `opkind41385(packetSize) >= 15`. The index is 15 bits. 32767 breaks. A new slot is allocated when the index is unknown. Then 2-bit plane. If the packet argument is true (opcode 59), dx and dz are 8-bit signed (`> 127` subtracts 256). If false (opcode 99), they are 5-bit signed (`> 15` subtracts 32). Then one bit, one mask bit, a 3-bit angle stored as `(angle + 4) << 11`, a 15-bit type id, and the second coordinate. The NPC is placed at the local player's tile plus those deltas.

`opClient606` (`Client.java:9771-10154`) reads the mask header as a u8, plus another u8 at `<< 8` when bit 6 (64) is set, at `<< 16` when bit 11 (2048) is set, and at `<< 24` when bit 18 (262144) is set. Check order:

| Bit | Reads |
| --- | --- |
| 4 | jstr (`9790-9791`) |
| 128 | u16le_add128, 65535 becomes -1 (`9794-9798`) |
| 16777216 | u16le, u32_430, u8neg (`9801-9816`) |
| 33554432 | u16be, u32_461, u8neg (`9819-9834`) |
| 4194304 | u8add128 flags. Bit 0 skips the rest. Bit 1 is one bigsmart per model. Bit 2 is one u16be_add128 per recolour. Bit 3 is one u16le per retexture. The third count stays 0 when `valuekind374915` is non-null, because that assignment overwrites the second count (`9837-9880`) |
| 1 | u16le_add128, u16be_add128 (`9883-9885`) |
| 2 | bigsmart per `C33282969.opkind34741()` entry, then u8add128 (`9888-9896`) |
| 65536 | u16le, 65535 copies the type's value (`9899-9903`) |
| 16384 | u8neg flags, same bit layout as 4194304, but the three counts are the type arrays and bit 2 uses u16le_add128 and bit 3 uses u16be (`9906-9951`) |
| 1048576 | u8 count, then that many u8, u32_461, u24_458 (`9954-9962`) |
| 8 | u16be, u32_430, u8neg (`9965-9980`) |
| 4096 | u16le_add128, 65535 becomes -1 (`9983-9987`) |
| 1024 | u8neg count, then u16be, an extra u16be_add128 when `(word & U+C000) == 49152`, then another u16be_add128 (`9990-10007`) |
| 512 | i8sub128, i8sub128, i8sub128, i8, u16le_add128, u16le_add128, u16le_add128 (`10010-10017`) |
| 32768 | `(var4 & '耀')`. u16le, u32_430, u8neg (`10026-10041`) |
| 32 | bigsmart type id (`10044-10049`) |
| 131072 | u8 count, then u8sub128 plus `Statics.valueStatics960.opkind37734` (`10057-10065`). The inner method was not opened |
| 524288 | i8neg, i8sub128, i8add128, u8, u16le, u16be (`10068-10075`) |
| 8388608 | jstr (`10077-10081`) |
| 8192 | u8neg count, then bigsmart, u8neg, u16le_add128 (`10084-10097`) |
| 16 | u8sub128 hit count, smarts with the same 32767 / 32766 split as the player mask, then u8neg and another smart group (`10100-10140`) |
| 2097152 | u8 count, then u8 plus `opkind37734` (`10143-10152`) |

An NPC whose cycle does not match `valueClient1825` is removed (`Statics.java:4736-4746`).
