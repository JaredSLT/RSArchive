# Gaps

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| Discarded byte before the login and new-player response | `builds/rsc/115-2001-12-24/src/src/main/java/jagex/client/Client.java:136` and `Client.java:267` | The method reads one `call130` byte and never branches on it. | implementable |
| Player and NPC kind-1 update reads 4 bits without advancing, then either consumes 2 bits or rereads 4 | `deob/MudClient.java:2899` and `MudClient.java:3394` | The first `Tools.gBit(..., 4)` does not add to the bit cursor. The following branch is what the decompiled method does. | implementable |
| New-player loop tests 24 bits and then reads 26 | `deob/MudClient.java:2915` | The while condition is `bitpos + 24 < length * 8`. The body reads 11+5+5+4+1 bits. | implementable |
| New-NPC loop tests 31 bits and then reads 33 | `deob/MudClient.java:3413` | The while condition is `bitpos + 31 < length * 8`. The body reads 11+5+5+4+8 bits. | implementable |
| Player opcode 250 type other than 0..6 | `deob/MudClient.java:3225` | The branch consumes no bytes, so the next record starts at the same offset. | implementable |
| Player opcode 250 type 5 when the entity is absent | `deob/MudClient.java:3240` | The skip is a raw 14 bytes plus a u8 length. The present-entity layout is specified in `sync.md`. | implementable |
| NPC opcode 247 type other than 1 or 2 | `deob/MudClient.java:3452` | The method advances only for types 1 and 2. | implementable |
| Terrain mesh after the collision clear | `deob/World.java:918` | `call9329ebf4` loads the four `.jm` sectors and clears `numberArrayArray425`. The vertex loop after that line was not transcribed. | implementable |
| Path search after the grid clear | `deob/World.java:111` | `call3cf8018f` is the step source for opcodes 215 and 255. The search body after the clear at line 112 was not transcribed. | implementable |
| Bank slot merge | `deob/MudClient.java:4073` | Opcode 222's fields are specified. `callb8c761e1` was opened only at its start. | implementable |
| BZip2 decode loop | `deob/BZip2.java:56` | `call11f2fbbd` builds a `jagex.BZipState` and calls `call585d4622`. The Huffman loop was not transcribed. | implementable |
| Software raster | `jagex/client/World3D.java:564` | `callc3e902a1` is the call after the camera set. The scanline body, and the `PixMap` plot loops it uses, were not transcribed. `Polygon3D` and `ScanLine` are the records those loops store. | implementable |
| No start tile in the client | `deob/MudClient.java:2151` and `MudClient.java:2841` | Login success clears entities. The first x and y are the 10-bit and 12-bit fields of opcode 255. | implementable |
| RSLib server opcode 191 | `rslib/src/server/packets/115.c:4` | This client reads player movement as opcode 255 at `deob/MudClient.java:2833`. | implementable |
| RSLib server opcode 79 | `rslib/src/server/packets/115.c:5` | This client reads NPC movement as opcode 248 at `deob/MudClient.java:3347`. | implementable |
| RSLib server opcode 53 | `rslib/src/server/packets/115.c:6` | This client reads inventory as opcode 252 at `deob/MudClient.java:3124`. | implementable |
| RSLib server opcode 25 | `rslib/src/server/packets/115.c:7` | This client loads the map inside opcode 255 through `call8390e4c9` at `deob/MudClient.java:2847`. | implementable |
| RSLib client opcode 221 bound as walk | `rslib/src/client/packets/115.c:9` and `rslib/src/both/move/rev_rsc.c:12` | Walk-here is opcode 255 and approach is opcode 215 at `deob/MudClient.java:4964`. Opcode 221 is menu 900 after that walk, at `MudClient.java:8323`. | implementable |
| RSLib `opcode_walk` and `opcode_player` left unset | `rslib/src/both/move/rev_rsc.c:13` and `rev_rsc.c:26` | The Java walk opcodes are 255 and 215. The Java player packet is 255. The 10-bit x, 12-bit y, 4-bit facing, and sector formula in that file match `MudClient.java:2841` and `MudClient.java:5003` when the opcode 244 offsets are 0. | implementable |
| RSLib says opcode 244 offsets are absent | `rslib/src/both/move/rev_rsc.c:29` | Opcode 244 stores the offsets at `deob/MudClient.java:3507`, and `call8390e4c9` adds them at `MudClient.java:4992`. | implementable |
| RSLib friend and ignore opcodes | `rslib/src/client/packets/115.c:11` | Java add-friend is 26 (`jagex/client/Client.java:483`), remove-friend is 27 (`Client.java:490`), add-ignore is 29 (`Client.java:446`), remove-ignore is 30 (`Client.java:463`). | implementable |
| RSLib appearance opcode 218 | `rslib/src/client/packets/115.c:16` | Appearance is opcode 236 at `deob/MudClient.java:1654`. Opcode 218 closes the shop at `MudClient.java:5853`. | implementable |
| RSLib drop opcode 246 | `rslib/src/client/packets/115.c:19` | Menu 660 writes opcode 251 at `deob/MudClient.java:8216`. Opcode 246 is menu 640 at `MudClient.java:8204`. | implementable |
| RSLib attack-player opcode 241 | `rslib/src/client/packets/115.c:26` | Attack player is opcode 228 at `deob/MudClient.java:8298`. Opcode 241 is menu 410 at `MudClient.java:8147`. | implementable |
| RSLib ground-item opcode 248 | `rslib/src/client/packets/115.c:28` | Opcode 248 is menu 620, one u16be, at `deob/MudClient.java:8192`. | implementable |
| Sound | `builds/rsc/115-2001-12-24/src/src/main/java` | no-source. The tree has no synth, vorbis, midi, wave, or sound opcode. | blocked |
