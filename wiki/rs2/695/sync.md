# Sync

## Movement step bits

`C6bfd9e17.opkind29292` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C6bfd9e17.java:116`) runs only when its int argument is `-111270673`. It reads bits with `C37f4edaa.opkind21483` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C37f4edaa.java:70`), which could not be decompiled, so the bit order inside the reader is unread. The widths passed to that call are in this method. The guard byte is `PkuuBuUpZmoXq.valuekind248624[74]`, which is `-18` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/PkuuBuUpZmoXq.java:37`).

| 2-bit value | next read | why |
| --- | --- | --- |
| 0 | return 0 | `var3 == 0` |
| 1 | 5 bits | `valuekind248624[48] == ~var3` and index 48 is `-2`, so `~var3 == -2` |
| 2 | 8 bits | index 53 is `-3`, so `~var3 == -3`, and the width is index 31, which is 8 |
| other (the remaining 2-bit value) | 11 bits | width is index 11, which is 11 |

No decompiled caller of `opkind29292` was found. Mask bit order, the player block, the NPC block, and an empty-list terminator were not read. `rslib/src/both/move/rev_695.c` leaves `opcode_player` and `opcode_player_index` at `-1`. This client does not show those opcodes either.

## Local player and other players

Unread. The update methods that would consume a player packet are not decompiled in the files that were opened for this page.

## NPCs

Unread. No NPC update handler was read.
