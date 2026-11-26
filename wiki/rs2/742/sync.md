# Sync

Player packet opcode 90 `PLAYER_INFO` size -2 calls `MapElement.update` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7888, builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:85). The initial placement is not that packet. Login calls `MapElement.opMapElement41` (builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:41) before the rebuild half.

There are 2048 player indexes. There is no 2047 empty-list terminator. A skip run is `gBit(2)`: 0 means 0, 1 means `gBit(5)`, 2 means `gBit(8)`, 3 means `gBit(11)` ({cite('MapElement.java', 199)}).

## Login placement

`opMapElement41` is bit-packed. `gBit(30)` is the local player: plane = bits 28..29, x = bits 14..27, z = bits 0..13, then subtract the world base. For every other index from 1 to 2047, `gBit(18)` is stored as a low-res coordinate: plane = bits 16..17, zone x = bits 8..15, zone z = bits 0..7. The high-res list starts as the local player only. No appearance mask is read here. `bytes()` ends the bit section.

## Later player packet

`opMapElement39` (builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:95) is four bit sections, each wrapped in `bits()` … `bytes()`:

1. high-res indexes whose `nsn` bit 0 is clear
2. high-res indexes whose `nsn` bit 0 is set
3. low-res indexes whose `nsn` bit 0 is set
4. low-res indexes whose `nsn` bit 0 is clear

In each section, `gBit(1) == 0` starts a skip count and sets `nsn` bit 1. `gBit(1) == 1` runs the high-res step (`opMapElement45`) or the low-res step (`opMapElement44`). After the four sections, `nsn` is shifted right by 1 and the high and low lists are rebuilt from which `players[index]` is non-null. Indexes run from 1.

High-res `opMapElement45` (builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:215): `gBit(1)` extended-info flag, then `gBit(2)` kind.

- 0: remove from the high-res list (illegal for the local player). A further `gBit(1)` may recurse into the low-res update.
- 1: one step, `gBit(3)`, added to `routeWaypointX` / `routeWaypointZ`.
- 2: two-tile step, `gBit(4)`.
- 3: `gBit(1) == 0` reads `gBit(12)` (plane 2 bits, dx 5 bits signed, dz 5 bits signed). `gBit(1) == 1` reads `gBit(30)` and adds a wrapped plane/x/z to the current tile.

One-step `gBit(3)`, with +x and +z on the route arrays:

| code | dx | dz |
| --- | --- | --- |
| 0 | -1 | -1 |
| 1 | 0 | -1 |
| 2 | +1 | -1 |
| 3 | -1 | 0 |
| 4 | +1 | 0 |
| 5 | -1 | +1 |
| 6 | 0 | +1 |
| 7 | +1 | +1 |

Two-tile `gBit(4)` ({cite('MapElement.java', 279)}): 0 is (-2,-2), then (-1,-2), (0,-2), (+1,-2), (+2,-2), (-2,-1), (+2,-1), (-2,0), (+2,0), (-2,+1), (+2,+1), (-2,+2), (-1,+2), (0,+2), (+1,+2), (+2,+2).

Low-res `opMapElement44` (builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:390): `gBit(2)`.

- 0: optional recurse, then `gBit(6)` local x, `gBit(6)` local z, `gBit(1)` extended flag. The player is created and teleported to `(zoneX<<6)+x6 - base.x`, `(zoneZ<<6)+z6 - base.z`.
- 1: `gBit(2)` added to the plane nibble. Stays low-res.
- 2: `gBit(5)` is a 2-bit plane delta and a 3-bit zone step. The 3-bit step uses the same dx/dz table as the high-res one-step.
- 3: `gBit(18)` added to the stored zone coordinate (plane, zone x, zone z).

## Player masks

After movement, `opMapElement43` (builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:489) reads one `u8` per extended index. Bit `0x8` adds another `u8` shifted by 8. Bit `0x2000` adds another `u8` shifted by 16. Those two bits are the extra mask bytes. Payload bits are then tested in this order (builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:505). `if:` means the read is inside a branch. Appearance bit `0x20` stores `u8` length and `bytes_rev_sub128` of that length, then `PlayerEntity.getAppearance` reads that buffer from the start.

| bit | fields | citation |
| --- | --- | --- |
| 0x1000 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:507 |
| 0x8000 | i8_neg | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:510 |
| 0x4 | u8_sub128, if:smart1or2, if:smart1or2, if:smart1or2, if:smart1or2, if:smart1or2, if:u8_128sub, if:smart1or2, if:smart1or2, u8_neg, if:smart1or2, if:smart1or2, if:smart1or2, if:u8_neg, if:u8_sub128 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:513 |
| 0x40000 | i8_neg, i8_128sub, i8_128sub, u8_neg, u16be_sub128lo, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:553 |
| 0x100000 | u8, if:u16le_sub128lo, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:561 |
| 0x10 | u16le | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:569 |
| 0x40 | if:smart2or4null, u8_neg | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:576 |
| 0x80000 | u8, if:u16le, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:584 |
| 0x2 | u16le_sub128lo | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:593 |
| 0x1 | u16le_sub128lo, u32_alt2, u8_neg | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:600 |
| 0x20 | u8, bytes_rev_sub128(var38) | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:615 |
| 0x100 | u16be_sub128lo, u32_alt3, u8_sub128 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:623 |
| 0x20000 | u8, if:u16be_sub128lo, if:u16be, if:u16le_sub128lo | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:638 |
| 0x10000 | jstr, u8_128sub | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:654 |
| 0x80 | i8_sub128 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:662 |
| 0x200 | i8, i8, i8_sub128, i8, u16le_sub128lo, u16le_sub128lo, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:665 |
| 0x4000 | u8_neg, if:smart2or4null, if:u8_128sub, if:u16be_sub128lo | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:688 |
| 0x800000 | u8_128sub | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:700 |
| 0x400 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:703 |
| 0x200000 | u16be_sub128lo, u32le, u8_neg | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:710 |
| 0x400000 | u16le, u32be, u8_sub128 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:725 |

`getAppearance` (builds/rs2/742-2012-11-07/src/client/src/main/java/PlayerEntity.java:105) starts at buffer position 0. `u8` flags: bit 0 gender, bit 1 `valuePlayerEntity135`, bit 2 combat-variant, bits 3..5 size-1, bit 6 title `jstr2`, bit 7 extra `jstr2`. Then three `i8`. Wear slots come from `WearposDefaults` opcode 1 (cache group id 6). A slot whose flag is 1 is skipped. Otherwise `u8`, and 0 means empty, else another `u8` and the id is `(b0<<8)+b1`. Slot 0 id 65535 is an NPC appearance: `smart2or4null` plus `u8`, and the wear loop stops. An id at least 16384 is an object id `(id-16384)` with bit `0x40000000`. A smaller id is a model `(id-256)` with the sign bit set. When the appearance is not an NPC, `u16be` is a customisation bitset and each set bit calls `ObjTypeCustomisation.opObjTypeCustomisation29`, which was not read. Then ten `u8` colours, `u16be` bas id, `jstr` name, `u8` combat. Flag bit 2 reads `u16be` (65535 means -1). Otherwise two `u8` (255 means -1). Then `u8` bgsound range, and when it is non-zero four `u16be` and one `u8`.

`MoveSpeed` ids: STATIONARY -1, CRAWL 0, WALK 1, RUN 2 ({cite('MoveSpeed.java', 7)}). `values()` order is STATIONARY, WALK, CRAWL, RUN. The appearance-adjacent animation mask loops `values().length` times (4).

## NPCs

Opcode 72 `NPC_INFO` calls `receiveNpcInfo(false)`. Opcode 65 `NPC_INFO_LARGE` calls `receiveNpcInfo(true)` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8514). Both run `updateLocalNpcs`, `updateNpcsAddChange`, `updateNpcsExtendedInfo`, and require the buffer to be consumed (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9194).

`updateLocalNpcs` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9250) calls `bits()` and `gBit(8)` for how many existing local NPCs to visit. The count must be at most `npcSlotCount`. Each entry is `gBit(1)`. 0 keeps the NPC and reads no mask. 1 reads `gBit(2)`: 0 keep and mark extended; 1 one `gBit(3)` compass step at `MoveSpeed.WALK` then `gBit(1)` extended; 2 if `gBit(1)` is 1 then two compass steps at `RUN`, else one step at `CRAWL`, then `gBit(1)` extended; 3 remove. This method does not call `bytes()`. `updateNpcsAddChange` keeps reading bits.

The compass id is `CompassPoint.getId()`, the second constructor argument, looked up by `MonochromeImageCacheEntry.decode` ({cite('CompassPoint.java', 7)}, {cite('MonochromeImageCacheEntry.java', 15)}). `NpcEntity.move` switches on the first constructor argument ({cite('NpcEntity.java', 221)}). Wire id to dx, dz:

| wire id | dx | dz |
| --- | --- | --- |
| 0 | 0 | +1 |
| 1 | +1 | +1 |
| 2 | +1 | 0 |
| 3 | +1 | -1 |
| 4 | 0 | -1 |
| 5 | -1 | -1 |
| 6 | -1 | 0 |
| 7 | -1 | +1 |

This is not the player step order.

`updateNpcsAddChange` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9309) loops while at least 15 bits remain. `gBit(15)` index 32767 ends the list. Then `gBit(15)` type id, `gBit(1)` extended, `gBit(3)` angle `((angle+4)<<11) & 0x3FFF`, `gBit(1)` jump. Large packets use `gBit(8)` dx and, after `gBit(2)` plane, `gBit(8)` dz, each biased by 256 when the value is above 127. Small packets use `gBit(5)` with bias 32 when the value is above 15. The NPC is moved to the local player's route tile plus those deltas. `bytes()` follows.

Extended info (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9378): `u8`, plus `u8<<8` when bit `0x4` is set, plus `u8<<16` when bit `0x400` is set, plus `u8<<24` when bit `0x20000` is set. Those three bits are only the extra mask bytes. Payload bits, in source order:

| bit | fields | citation |
| --- | --- | --- |
| 0x800000 | u16le_sub128lo | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9394 |
| 0x8000 | i8, i8, i8_128sub, i8_sub128, u16le_sub128lo, u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9400 |
| 0x100 | u16le | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9415 |
| 0x400000 | u8, if:u16be_sub128lo, if:u32le | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9421 |
| 0x80 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9430 |
| 0x2000 | u8_128sub, if:smart2or4null, if:u16le_sub128lo, if:u16le | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9441 |
| 0x20 | u16be_sub128lo | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9480 |
| 0x4000 | u8_sub128, if:u16be_sub128lo, if:u16le_sub128lo, if:u16be_sub128lo | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9486 |
| 0x1000 | u8, if:smart2or4null, if:u8_128sub, if:u16le | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9502 |
| 0x40000 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9515 |
| 0x1000000 | u16be_sub128lo, u32_alt2, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9521 |
| 0x200000 | u8, if:u8_128sub, if:u32_alt3, if:u24_alt3 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9536 |
| 0x80000 | i8_neg, i8_128sub, i8_neg, u8_sub128, u16le, u16be_sub128lo | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9545 |
| 0x2 | u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9553 |
| 0x200 | u16le, u32le, u8_128sub | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9557 |
| 0x100000 | u8, if:u16le, if:u32le | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9572 |
| 0x1 | if:smart2or4null, u8_128sub | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9580 |
| 0x40 | u16le_sub128lo, u32be, u8_sub128 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9588 |
| 0x10 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9603 |
| 0x2000000 | u16be, u32be, u8_neg | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9606 |
| 0x8 | u8_neg, if:smart1or2, if:smart1or2, if:smart1or2, if:smart1or2, if:smart1or2, if:u8_sub128, if:smart1or2, if:smart1or2, u8, if:smart1or2, if:smart1or2, if:smart1or2, if:u8_sub128, if:u8_128sub | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9621 |
| 0x10000 | u8_neg, if:smart2or4null, if:u16le, if:u16be_sub128lo | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9661 |

