# Input

State 30 reads server packets, then may write the packets below, then runs `PathingEntity.opPathingEntity36` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/C55f3f558.java:340`).

## Mouse

Opcode 248 carries the sample buffer. Coordinates are clamped to 0..764 by 0..502. The off-screen pair `-1, -1` is packed as 524287. See [packets.md](packets.md).

Opcode 61 is one click. Button 2 is the value that sets bit 19. Time is in 50 ms units, clamped to 4095 (`C55f3f558.java:433`).

## Keys and camera

Keys 96, 97, 98, and 99 set `ConnectionTimedOut.valuekind2991`. The next send is opcode 213: `u16le` of `SorryInvitedPlayersOnly.valuekind23053` and write-`u16be_sub128` of `C63d74d54.valuekind28410` (`C55f3f558.java:467`). Those two fields are the values written. Their camera meaning is not named in that method.

Focus in and out send opcode 30 with `u8` 1 or 0 (`C55f3f558.java:483`).

## Walk, run, minimap

`GameShell.opGameShell27` builds a path on the current level's collision flags `Ca0f54332.valuekind3292` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/GameShell.java:84`). Array slot 0 is the clicked or nearest tile. The walk back toward the player appends turn tiles. The packet sends the tile nearest the player first.

| `var8` | Opcode | When |
| --- | --- | --- |
| 0 | 198 | ordinary walk |
| 1 | 249 | minimap. `Sprite.opSprite34` calls `opGameShell27` with this value when `C55f3f558.valuekind3064` is 0 or 3 (`builds/rs2/481-2007-12-11/src/src/main/java/deob/Sprite.java:41`) |
| 2 | 95 | third walk form |

The run byte is `TitleMute.valuekind3487[82] ? 1 : 0` (`GameShell.java:317`). Minimap opcode 249 then appends the 14 bytes at `Sprite.java:55`. Opcode 115 sets `C55f3f558.valuekind3064`, and the minimap click checks that field for 0 or 3.

## Widget ops

`SoftwareFont.calld58221ea` writes opcodes 242, 21, 196, 133, 100, 153, 107, and 66. `ByteArray.opByteArray13` writes the menu opcodes listed in [packets.md](packets.md). Those bodies were not read, so the field lists are empty.

Friend add is opcode 53, a `u64be`, from `Sprite.call5fde21bf` (`Sprite.java:98`).

Idle opcode 146 and keepalive opcode 219 are written from this same tick, with no payload, at `C55f3f558.java:677` and `:752`.
