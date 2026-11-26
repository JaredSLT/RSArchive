# Gaps

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/` unless the source is RSLib. Every `.java` file under the source root is `read` or `classified` in [files.tsv](files.tsv). There is no unread file, no missing cache, and no `jaggl`, `jagdx`, or `jagtheora` type.

`implementable` rows are disagreements or size quirks already fixed by the Java citations. A later pass codes the Java side. `blocked` rows are method bodies that were not read.

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| S2C logout is opcode 5, size 0, and calls `logout` | `jagex2/client/Client.java:2781`. RSLib `RS_S2C_LOGOUT` is 86 in `rslib/src/server/packets/377.c:24` | Java opcode 5. 86 is not a handler | implementable |
| S2C 158 is `g2b_alt1` into `valueClient1316`. Interface text is opcode 232, `g2_alt3` component then `jstr` | `jagex2/client/Client.java:2646` and `Client.java:3808`. RSLib `RS_S2C_IF_TEXT` is 158 in `rslib/src/server/packets/377.c:29` | Java text packet is 232 | implementable |
| S2C 10 is a sidebar tab. Large varp is opcode 115, `g4_alt3` value then `g2_alt1` id | `jagex2/client/Client.java:3661` and `Client.java:2787`. RSLib `RS_S2C_VARP_LARGE` is 10 in `rslib/src/server/packets/377.c:31` | Java large varp is 115 | implementable |
| S2C 53 is the instanced rebuild. Inventory is opcode 134 and opcode 206 | `jagex2/client/Client.java:3351`, `Client.java:3018`, and `Client.java:3317`. RSLib `RS_S2C_INVENTORY` is 53 in `rslib/src/server/packets/377.c:32` | Java inventory opcodes are 134 and 206 | implementable |
| RSLib `RS_S2C_IF_OPEN` 97 is not a branch | `rslib/src/server/packets/377.c:33`. Opens are 109, 128, 159, 246, and 253 at `Client.java:2578`, `Client.java:2962`, `Client.java:3214`, `Client.java:3256`, and `Client.java:3733` | no handler for 97 | implementable |
| RSLib `RS_S2C_ENERGY` 110 is not a branch. The `u8` script opcode 11 reads is S2C 125. C2S 110 closes windows | `rslib/src/server/packets/377.c:34`. `Client.java:3583` and `Client.java:973` | no S2C handler for 110 | implementable |
| RSLib `RS_S2C_WEIGHT` 240 is not a branch. The `i16be` script opcode 12 reads is S2C 174 | `rslib/src/server/packets/377.c:35`. `Client.java:2940` | no handler for 240 | implementable |
| C2S add friend is opcode 120. Opcode 217 is add ignore | `jagex2/client/Client.java:9411` and `Client.java:8761`. RSLib `RS_C2S_ADD_FRIEND` is 217 in `rslib/src/client/packets/377.c:40` | Java add friend is 120 | implementable |
| C2S remove ignore is opcode 160. Opcode 217 is add ignore | `jagex2/client/Client.java:9226` and `Client.java:8761`. RSLib `RS_C2S_ADD_IGNORE` is 160 in `rslib/src/client/packets/377.c:42` | Java 160 removes an ignore | implementable |
| C2S add friend is opcode 120, not remove ignore | `jagex2/client/Client.java:9411`. RSLib `RS_C2S_DEL_IGNORE` is 120 in `rslib/src/client/packets/377.c:43` | Java 120 adds a friend | implementable |
| C2S spell on a player is opcode 31, action 918. Spell on an NPC is opcode 104, action 67 | `jagex2/client/Client.java:11007` and `Client.java:11021`. RSLib `RS_C2S_MAGIC_ON_NPC` is 31 and `RS_C2S_MAGIC_ON_PLAYER` is 104 in `rslib/src/client/packets/377.c:64` | Java 31 is the player and 104 is the NPC | implementable |
| C2S item on a player is opcode 143, action 596: `p2_alt1` `valueClient1276`, `p2_alt3` `valueClient1274`, `u16be` `valueClient1275`, `p2_alt2` player index. Opcode 1 is widget action 903 | `jagex2/client/Client.java:11303` and `Client.java:10915`. RSLib `RS_C2S_ITEM_ON_PLAYER` is 1 in `rslib/src/client/packets/377.c:63` | Java item-on-player is 143 | implementable |
| C2S item on a loc is opcode 152, action 467. Opcode 210 is action 376 and writes the spell component `valueClient1297` first | `jagex2/client/Client.java:10755` and `Client.java:10975`. RSLib `RS_C2S_ITEM_ON_OBJECT` is 210 in `rslib/src/client/packets/377.c:62` | Java item-on-loc is 152 | implementable |
| No C2S size array. `Protocol.valueProtocol1` is unused. Outbound length is the writer's `u8` or `psize1` | `jagex2/io/Protocol.java:6` and `jagex2/io/Packet.java:218`. RSLib `rs_sizes_c2s_377` is `rslib/src/client/packets/377.c:3` | do not copy the RSLib C2S sizes | implementable |
| S2C 187 is table size 6 and has no branch. `read` logs `T1` and logs out. The payload is not parsed | `jagex2/io/Protocol.java:7` index 187 and `jagex2/client/Client.java:3841` | empty field list. Do not invent a payload | implementable |
| S2C 203 table size is 14. `readZonePacket` reads 16 bytes. Nested under opcode 183 the sub-opcode is not checked against 14. A standalone 203 frame is 14 bytes | `jagex2/client/Client.java:12402` and `Client.java:2440` | use the 16-byte read inside a 183 batch and the table length for a top-level frame | implementable |
| S2C 199 table size stays 6. Type 1 reads `u16be`. Types 2..6 read `u16be`, `u16be`, `u8`. Type 10 reads `u16be`. Short branches leave the tail unread | `jagex2/client/Client.java:2705` | keep the table size | implementable |
| New NPC loop tests `bitpos + 21 < size * 8` while the body is 14+1+5+5+1+13 bits | `jagex2/client/Client.java:12348` | keep the 21-bit test | implementable |
| `BZip2.read` body | `jagex2/io/BZip2.java:8` | unread | blocked |
| `FileStream` sector read and write | `jagex2/io/FileStream.java:32` and `FileStream.java:131`. The scratch array is 520 bytes at `FileStream.java:9` | unread | blocked |
| `AnimFrame.opAnimFrame1` | `jagex2/dash3d/AnimFrame.java:20` | unread | blocked |
| `Model.opModel5` vertex walk after the 18-byte trailer | `jagex2/dash3d/Model.java:220` | unread | blocked |
| `Pix3D.fillGouraudTriangle`, `fillTriangle`, `fillTexturedTriangle`, and `unpackTextures` | `jagex2/graphics/Pix3D.java:402`, `Pix3D.java:1093`, `Pix3D.java:1618`, and `Pix3D.java:147` | unread | blocked |
| `World.perlin` both overloads, `opWorld55`, `opWorld58`, `opWorld59`, and `stitchHeightmap` | `jagex2/dash3d/World.java:45`, `World.java:423`, `World.java:918`, `World.java:949`, `World.java:1002`, and `World.java:1472` | unread | blocked |
| `CollisionMap.opCollisionMap7`, `opCollisionMap8`, and `opCollisionMap9` | `jagex2/dash3d/CollisionMap.java:458`, `CollisionMap.java:613`, and `CollisionMap.java:688` | unread | blocked |
| `Client.drawInterface` per-type paint | `jagex2/client/Client.java:12967` | unread | blocked |
| `Tone.read`, `Envelope.readShape`, and `Filter.opFilter5` | `jagex2/sound/Tone.java:276`, `jagex2/sound/Envelope.java:25`, and `jagex2/sound/Filter.java:109` | unread | blocked |
| `WordFilter.read` and `opWordFilter60` | `jagex2/wordenc/WordFilter.java:37` and `WordFilter.java:185`. Call sites `Client.java:2163`, `Client.java:2362`, `Client.java:3180`, and `Client.java:5850` | unread | blocked |
