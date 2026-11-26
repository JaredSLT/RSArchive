# Player and NPC sync

Opcode 29 calls `ReferenceNodeFactory.opReferenceNodeFactory3` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:432`). The order is local movement, the existing-player list, the new-player list, then masks (`builds/rs2/443-2006-12-11/src/src/main/java/deob/ReferenceNodeFactory.java:103`). After the masks, players queued for removal whose cycle stamp was not refreshed are cleared. A position mismatch throws `gpp1`. A missing active player throws `gpp2`.

Opcode 238 calls `SpielerBitteHlenSieEineAndereWelt.opkind1532` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:833`): existing NPCs, new NPCs, then masks. The matching checks throw `gnp1` and `gnp2` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/SpielerBitteHlenSieEineAndereWelt.java:64`).

`opkind1872(sceneZ, teleport, sceneX)` stores `sceneX` in `valuekind18765` and `sceneZ` in `valuekind1876` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/NameDesGegenstandsEingeben.java:97`). Those axes match the region base: rebuilding subtracts the base-X delta from `valuekind18765` and the base-Z delta from `valuekind1876` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/ChangeLocRequest.java:65`).

## Local player

`GlModel.callde1332aa` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/GlModel.java:108`), after `opkind1332`:

| Bits | Meaning |
| --- | --- |
| 1 | update. Zero ends the local block. |
| 2 | type |
| type 0 | queue index 2047 for masks |
| type 1 | 3-bit direction, walk once, then 1 bit that queues 2047 |
| type 2 | two 3-bit directions, both run steps, then 1 bit that queues 2047 |
| type 3 | 7-bit scene X, 1 mask bit, 2-bit plane, 7-bit scene Z, 1 teleport bit. `opkind1872(sceneZ, teleport == 1, sceneX)` |

Direction steps in `opkind18710` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/NameDesGegenstandsEingeben.java:1682`):

| Direction | Scene X | Scene Z |
| --- | --- | --- |
| 0 | -1 | +1 |
| 1 | 0 | +1 |
| 2 | +1 | +1 |
| 3 | -1 | 0 |
| 4 | +1 | 0 |
| 5 | -1 | -1 |
| 6 | 0 | -1 |
| 7 | +1 | -1 |

The second argument of `opkind18710` is stored as the run flag on the step.

## Other players

`AufDerHautpseite.opkind15420` reads an 8-bit count (`builds/rs2/443-2006-12-11/src/src/main/java/deob/AufDerHautpseite.java:386`). A count above the previous list throws `gppov1`. Players past the count are removed. Each kept slot is:

| Bits | Meaning |
| --- | --- |
| 1 | zero keeps the player with no further bits |
| 2 | 0 stay and queue masks; 1 walk one 3-bit direction plus a mask bit; 2 run two 3-bit directions plus a mask bit; 3 remove |

`ModeratorOptionSpielerUrStundenStummSchalt.opkind19811` continues while at least 12 bits remain (`builds/rs2/443-2006-12-11/src/src/main/java/deob/ModeratorOptionSpielerUrStundenStummSchalt.java:201`). An 11-bit index of 2047 ends the list. Otherwise: 1 mask bit, 5-bit scene-Z delta, 1 teleport bit, 3-bit facing, 5-bit scene-X delta. A 5-bit value above 15 subtracts 32. The facing value indexes `Client.valueClient1465`, which is `{768, 1024, 1280, 512, 1536, 256, 0, 1792}` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:17`), and is applied only when the slot was just created. Placement is `opkind1872(localSceneZ + firstDelta, teleport == 1, localSceneX + secondDelta)`. `opkind1334` returns to bytes.

## Player masks

`RegelverstoMelden.opkind1954` reads a u8 and, when bit `0x80` is set, another u8 shifted up by 8 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/RegelverstoMelden.java:89`). `Cd66098f9.opkind1308` then tests bits in this order (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cd66098f9.java:339`):

| Bit | Fields |
| --- | --- |
| 0x40 | u8a length, then that many bytes stored as `(byte)(b - 128)`, then `opkind1842` |
| 0x100 | u8s, u8c, u8s, u8s, u16leA, u16leA, u8c |
| 0x200 | u16be, with 65535 stored as -1, then u32mix32 |
| 0x1 | jstr. The first character 126 strips one character before the chat is shown |
| 0x10 | u16leA, with 65535 stored as -1 |
| 0x2 | u16leA, u16le |
| 0x8 | u16leA, with 65535 stored as -1, then u8c |
| 0x4 | u16be, u8, u8a length, then that many bytes in reverse. The position advances by the length even when the chat is skipped |
| 0x400 | u8, u8s, u8a, u8 |
| 0x20 | u8c, u8, u8c, u8c |

### Appearance

`UndHabenEsDeaktiviertBenutzenSieDie.opkind1842` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/UndHabenEsDeaktiviertBenutzenSieDie.java:45`):

| Order | Width |
| --- | --- |
| gender | u8. The value 1 is passed on as the gender flag |
| head icons | i8, i8 |
| 12 equipment slots | u8. Zero stores 0. Otherwise another u8, and the slot is `(first << 8) \| second`. If slot 0 is 65535, a u16be npc id replaces the equipment and the loop stops |
| 5 colours | u8 each. A colour past the end of the client palette becomes 0 |
| 7 animations | u16be each. 65535 becomes -1. The second value is also copied into the third animation field, so the wire still has seven shorts |
| name | u64be |
| combat | u8 |
| skill | u16be |

## NPCs

Existing NPCs use the same 8-bit shape as other players (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Js5CacheRequest.java:78`). A count above the previous list throws `gnpov1`.

New NPCs continue while at least 27 bits remain (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MaterialManager_Class106_Sub1.java:74`). A 15-bit index of 32767 ends the list. Otherwise: 1 mask bit, 13-bit type id, 5-bit scene-X delta, 3-bit facing, 5-bit scene-Z delta, 1 teleport bit. Deltas above 15 subtract 32. Placement is `opkind1872(localSceneZ + secondDelta, teleport == 1, localSceneX + firstDelta)`. The facing table is the same `valueClient1465`, applied only for a new slot.

NPC masks are one u8, tested in this order by `PacketBit.opPacketBit14` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PacketBit.java:810`):

| Bit | Fields |
| --- | --- |
| 0x2 | u16be, u16le |
| 0x20 | u16leA, with 65535 stored as -1, then u8a |
| 0x40 | u8a, u8a, u8, u8c |
| 0x4 | u16leA, with 65535 stored as -1, then u32mix32 |
| 0x10 | u16le, with 65535 stored as -1 |
| 0x80 | u8s, u8s, u8, u8s |
| 0x1 | u16le type id |
| 0x8 | jstr |

## RSLib

`rslib/src/both/move/rev_443.c` `player_decode_443` accepts an other-player count of 0, a new-player index of 2047, and mask bits `0x40` and `0x80`. The Java mask reader consumes the additional bits listed above (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cd66098f9.java:349`). That limit is recorded in [gaps.md](gaps.md). The local teleport bit order in that decoder matches `GlModel.callde1332aa`.
