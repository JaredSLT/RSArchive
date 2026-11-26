# Region

Opcode 121 calls `Timer.opTimer4(false)`. Opcode 193 calls `Timer.opTimer4(true)` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:827`).

## Static rebuild

`opTimer4` with the flag false (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:70`):

| Order | Width | Use |
| --- | --- | --- |
| 1 | u16le | zone X |
| 2 | u16le | scene X, passed as the third argument of `opkind1872` |
| 3 | u16leA | scene Z, passed as the first argument of `opkind1872` |
| 4 | u8a | plane |
| 5 | `(remaining before the last short) / 16` groups of four u32mix49 | XTEA keys |
| 6 | u16beA | zone Z |

Map squares run from `(zoneX - 6) / 8` through `(zoneX + 6) / 8` and the same window on zone Z. A square is skipped when the X square is 48 or 49 and the Z square is 48, and when the X square is 48 and the Z square is 148, plus the inner exclusions at `Timer.java:91`. Each kept square stores `(mapX << 8) + mapZ` and looks up groups named `m{mapX}_{mapZ}` and `l{mapX}_{mapZ}` on js5 index 5. The name pieces are the literals `m`, `l`, and `_` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cd66098f9.java:22`, `builds/rs2/443-2006-12-11/src/src/main/java/deob/Js5NetQueue.java:27`, `builds/rs2/443-2006-12-11/src/src/main/java/deob/Passwort.java:29`).

## Dynamic rebuild

The flag true reads bits first (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:112`). For each of 4 planes, 13 by 13 tiles: 1 presence bit, then a 26-bit value, otherwise the slot is -1. After `opkind1334`, `(remaining) / 16` groups of four u32be keys follow, then:

| Order | Width | Use |
| --- | --- | --- |
| 1 | u16leA | scene X |
| 2 | u16le | zone Z |
| 3 | u16leA | scene Z |
| 4 | u16le | zone X |
| 5 | u8c | plane |

A present 26-bit word is split with `(word & 16377) >> 3` and `(word & 16774233) >> 14` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:155`).

## Origin

`ChangeLocRequest.opChangeLocRequest2` sets the loading state to 25 when the zone or, unless `SpotAnimType.valueSpotAnimType63` is false, the plane changed. A false flag forces plane 0 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/ChangeLocRequest.java:37`). The bases are `8 * zoneX - 48` and `8 * zoneZ - 48` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/ChangeLocRequest.java:44`). Existing NPC and player path queues shift by the base delta, and the local player is placed with `opkind1872(sceneZ, false, sceneX)`.

## XTEA

`MonotonicClock.opMonotonicClock27` decrypts 8-byte blocks after skipping the first 5 bytes. The round count is 32, the sum starts at `-957401312`, and each round subtracts `-1640531527` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:196`). `AufDerHautpseite` calls it when a file is fetched with a four-int key that is not all zero (`builds/rs2/443-2006-12-11/src/src/main/java/deob/AufDerHautpseite.java:318`). The loading method that pairs each stored key with the `m` and `l` groups is still unread. See [gaps.md](gaps.md).

## Zone packets

Opcode 244 reads u8c into `NichtHergestelltWerden.valuekind14010` and u8 into `NpcType.valueNpcType52`, then reads a raw u8 sub-opcode and calls `DerServerWirdGeradeAktualisiert.opkind1511` until the payload is consumed (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:875`). Opcodes 109, 79, 129, 94, 115, 101, 84, 207, 170, 69, and 122 call the same method with the top-level opcode (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:532`). The packed-tile expressions below are copied from the method.

| Opcode | Fields and tile split |
| --- | --- |
| 79 | u8 `((b >> 4) & 7, b & 7)`; u16be; u16be; u16be (`DerServerWirdGeradeAktualisiert.java:13`) |
| 94 | u16beA; u16beA; u8c `((122 & b) >> 4, b & 7)`; u16beA. The ground item is added when the first short differs from `NameEingeben.valuekind1521` (`:33`) |
| 84 | u16le; u8 `((122 & b) >> 4, b & 7)` (`:52`) |
| 69 | u8 type and rotation `(b >> 2, b & 3)`; u8a `((b >> 4) & 7, b & 7)` (`:75`) |
| 101 | u8 `((124 & b) >> 4, b & 7)`; i8; i8; i16be; u16be; u8; u8; u16be; u16be; u8; u8 (`:87`) |
| 207 | u16beA; u16le; u8 `((122 & b) >> 4, b & 7)` (`:111`) |
| 170 | u16leA; u8a `(b >> 2, b & 3)`; u8s `((126 & b) >> 4, b & 7)` (`:129`) |
| 122 | u8c `((120 & b) >> 4, b & 7)`; u16be; u8a `(b >> 2, b & 3)` (`:182`) |
| 115 | u8 `((b >> 4) & 7, b & 7)`; u16be; u8; u16be (`:195`) |
| 129 | i8neg; i8s; u16beA; i8s; i8a; u16beA; u8c `(b >> 2, b & 3)`; u8s `((119 & b) >> 4, b & 7)`; u16be; u16leA (`:210`) |
| 109 | u8 `((117 & b) >> 4, b & 7)`; u16be; u8 `(high nibble, low 3 bits)`; u8. This branch fills the area-sound queue (`:275`) |
