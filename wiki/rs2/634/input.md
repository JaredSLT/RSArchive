# Input

Keys are polled by `Cdba8e8ea.valuekind11355.opkind16781` and the mouse by `Cf5ea55c9.valuekind12991.opkind16197` from `Client.opClient241` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2298). The bodies of those poll methods were not read. Widget transmit queues are drained into `Unused3.opUnused319` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2271). That method was not read.

Camera update during the in-game tick calls `Cbd77d905.opkind11323` or `C2a2f4a67.opkind17151` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2160). Those bodies were not read.

## Packets that were read

All four are C2S. The opcode byte is ISAAC-added by `ClientOptionsLightingDetail.opClientOptionsLightingDetail7` (builds/rs2/634-2010-12-10/src/src/main/java/deob/ClientOptionsLightingDetail.java:40). The fields below are the payload after that byte.

| Opcode | Size | When | Payload | Citation |
| --- | --- | --- | --- | --- |
| 0 | 0 | in-game, after 50 cycles with no send (`Caa093cd9.valuekind11171`) | empty | builds/rs2/634-2010-12-10/src/src/main/java/deob/C0e62e5e5.java:4, builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2222 |
| 35 | 5 | menu action 19 when the packed int is not 1 | see walk | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cb6ae220e.java:10, builds/rs2/634-2010-12-10/src/src/main/java/deob/C79bd777c.java:32 |
| 82 | 18 | menu action 19 when the packed int is 1 | walk body plus 13 bytes | builds/rs2/634-2010-12-10/src/src/main/java/deob/C7643b519.java:4, builds/rs2/634-2010-12-10/src/src/main/java/deob/C8aa72153.java:549 |
| 36 | 7 | menu action 4 | see click | builds/rs2/634-2010-12-10/src/src/main/java/deob/C42efb241.java:7, builds/rs2/634-2010-12-10/src/src/main/java/deob/C8aa72153.java:571 |

Walk and minimap share `C79bd777c.opkind14354` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C79bd777c.java:32), called from menu action 19 (builds/rs2/634-2010-12-10/src/src/main/java/deob/C8aa72153.java:543):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u16le | destination z tile plus base z `Ca5bf8f3f.valuekind13813` |
| 2 | u8_add128 | 1 when key 82 is down, else 0 |
| 3 | u16be_add128 | base x `C7d96227f.valuekind12381` plus destination x |

Minimap (`var3 == 1`) appends these 13 bytes, which makes 18 (builds/rs2/634-2010-12-10/src/src/main/java/deob/C8aa72153.java:549):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | -1 |
| 2 | u8 | -1 |
| 3 | u16be | `(int) C29762178.valuekind14651` |
| 4 | u8 | 57 |
| 5 | u8 | `C0ec93d07.valuekind12943` |
| 6 | u8 | `C64814a3b3.valuekind15636` |
| 7 | u8 | 89 |
| 8 | u16be | local player fine x `valuekind14162` |
| 9 | u16be | local player fine z `valuekind141610` |
| 10 | u8 | 63 |

`rslib/src/both/move/rev_634.c` `walk_encode` writes only the 5-byte body and rejects `nsteps != 0` (rslib/src/both/move/rev_634.c:32). `walk_decode` requires the buffer length to be 5 (rslib/src/both/move/rev_634.c:57). The Java minimap packet is 18 bytes. That disagreement is in [gaps.md](gaps.md). The size tables themselves match.

Click action 4, opcode 36 (builds/rs2/634-2010-12-10/src/src/main/java/deob/C8aa72153.java:576):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u16le_add128 | base z plus clicked z. The write runs because the gate argument is 4325 |
| 2 | u16be_add128 | base x plus clicked x |
| 3 | u8_sub128 | 1 when key 82 is down, else 0 |
| 4 | u16le_add128 | high 32 bits of the entity long, `(int)(var8 >>> 32)` |

Other C2S opcodes are listed in [packets.md](packets.md) with an empty field list.
