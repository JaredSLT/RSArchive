# Region

`C50c59075.opkind346722` picks the rebuild (`C50c59075.java:251-260`).

| Mode | Constructor | Packet |
| --- | --- | --- |
| `C16dfbb23.valuekind34686` | `(false, false)` (`C16dfbb23.java:2`) | none. Login calls it with a null buffer (`Client.java:2477`) |
| `valuekind34683` | `(false, false)` (`C16dfbb23.java:4`) | S2C opcode 62, size `-2` (`Client.java:7599-7606`) |
| `valuekind34682` | `(true, false)` (`C16dfbb23.java:3`) | none. Login calls it with a null buffer (`Client.java:2745`) |
| `valuekind34684` and the other modes whose first flag is true | `(true, false)` or `(true, true)` (`C16dfbb23.java:5-8`) | S2C opcode 94, size `-2`, starts as `valuekind34684` (`Client.java:7741-7748`) |

`opkind34681` is the first constructor flag (`C16dfbb23.java:19-21`). Opcode 62 and opcode 94 copy the payload into a fresh buffer before the rebuild and return false from the packet loop (`Client.java:7599-7610`, `7741-7752`).

`rslib/src/both/move/rev_745.c` encodes the static header in this same order and uses the same area-tile table. No field disagreement.

## Area

`C064a5cf22` (`C064a5cf22.java:2-6`):

| Id | Tiles |
| --- | --- |
| 0 | 104 |
| 1 | 120 |
| 2 | 136 |
| 3 | 168 |
| 4 | 72 |

`opkind346727` sets both axes to that tile count (`C50c59075.java:605-607`).

## Static opcode 62

`opkind346717` (`C50c59075.java:335-378`):

| Order | Width |
| --- | --- |
| 1 | u8neg area id |
| 2 | u8sub128, force when it equals 1 |
| 3 | u16be chunk x |
| 4 | u16be_add128 chunk z |
| 5 | `(remaining / 16)` groups of four u32be XTEA keys |

Map squares run x outer, z inner, from `(chunk - (tiles >> 4)) / 8` through `(chunk + (tiles >> 4)) / 8` inclusive (`C50c59075.java:366-368`). For area 0, `tiles >> 4` is 6. Other area ids use their own shift. Keys are stored in `valuekind346739` and are not decrypted in this method (`C50c59075.java:345-351`).

## Dynamic opcode 94

`opkind346729` (`C50c59075.java:381-472`):

| Order | Width |
| --- | --- |
| 1 | u16be_add128 chunk x |
| 2 | u8add128. 1, 2, 3, or 4 selects `valuekind34684`, `valuekind34689`, `valuekind34685`, or `valuekind34681` |
| 3 | u8neg area id |
| 4 | u8sub128. Bit 0 is the force flag |
| 5 | u16le_add128 chunk z |
| 6 | for 4 planes, then `tiles >> 3` by `tiles >> 3`: 1 bit, and 26 bits when that bit is 1, else the zone stays -1 |
| 7 | `(remaining / 16)` groups of four u32be keys |

A 26-bit zone value contributes map square `((value >> 14 & 1023) / 8, (value >> 3 & 2047) / 8)` (`C50c59075.java:446-448`).

## Scene origin

`opkind346712` stores the base as `((chunkX - (sizeX >> 4)) * 8, (chunkZ - (sizeY >> 4)) * 8)` on `valuekind34677` (`C50c59075.java:659`). The static rebuild passes state 5 and the force flag (`C50c59075.java:378`). The dynamic rebuild passes state 5 and bit 0 of the flag byte (`C50c59075.java:472`).

## Map file names

Archive id 5 is `Statics.valueStatics719` (`Loading.java:226`, `C268a19ef.java:7`). `opkind346726` builds `mX_Z`, `lX_Z`, `umX_Z`, and `ulX_Z` (`C50c59075.java:589-601`). The underlay and overlay names prefix `u`. NPC group names are the literal `nX_Z` in the login rebuild only (`C50c59075.java:295`).

## Login map and cutscene

`opkind346723` uses area `Statics.valueStatics604.valuekind39756.opkind40163()`, places the player with `opkind40726(8, 8)`, and loads an 18-square window around the player's chunk (`C50c59075.java:265-289`). The following state is 2, 4, 10, or 12 from `valueClient1760` 17, 3, 18, or 11 (`C50c59075.java:317-329`). There is no packet.

`opkind346725` forces area 0 and fills zones from `CutsceneManager`, using XTEA keys already in `Statics.valueStatics223` (`C50c59075.java:475-586`). There is no packet.

The Js5 read that later decrypts `valuekind346739` was not opened.
