# Gaps

`class` is `implementable` when the Java method is in this tree and a later pass can read it. No file in this revision is `unread`. There is no `jaggl`, `jagdx`, or `jagtheora` source, so nothing is `platform-native`.

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| C2S opcodes with an empty field list | [packets.md](packets.md), first site `builds/rs2/481-2007-12-11/src/src/main/java/deob/ByteArray.java:57` | `opkind3405` was found and the bytes written after it were not read | implementable |
| S2C opcode 208 | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1426` | Size 6 and no branch. The client logs `T1 -` and logs out. The six bytes are not decoded | implementable |
| S2C opcode 137 object body | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:1096` | `Ca17abb5a(SlArrows)` was not read. The two leading `u8` values are in [packets.md](packets.md) | implementable |
| Chat string | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MapElement.java:58` | `opMapElement13` calls `Ca0f54332.opkind3291`. That method was not read | implementable |
| Username long | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoundBank.java:56` | `ByteArrayNode.opByteArrayNode83` was not read. Login writes the long it returns | implementable |
| Jstr charset | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SlArrows.java` `opkind33415` | The terminator is byte 0. `Texture.call27f7154f` was not read | implementable |
| Login response 15 | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoundBank.java` stage 6 | Calls `AccountLockedAsWeSuspectHasBeenStolen.opkind3054`. That method was not read | implementable |
| JS5 response 0 | `builds/rs2/481-2007-12-11/src/src/main/java/deob/Client.java:384` | Continues into `MalformedLoginPacket.call63b49dee`. That method was not read | implementable |
| ISAAC mix | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C016fbb54.java:230` | `opkind3073` was not read. `opkind3071` and the golden-ratio constant in `opkind3072` are in [framing.md](framing.md) | implementable |
| JS5 archive constructor flags | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C105f7f5d.java` `opkind3426` | The three booleans passed for each index were not read | implementable |
| Appearance flag bits | `builds/rs2/481-2007-12-11/src/src/main/java/deob/MapMarker.java:201` | Java reads flag bit 1 and bit 2. RSLib `appearance_parse` returns `RS_ERR_UNSUPPORTED` when `(flags & ~1) != 0` (`rslib/src/both/move/rev_481.c:216`) | implementable |
| Walk length byte | `builds/rs2/481-2007-12-11/src/src/main/java/deob/GameShell.java:293` | Java writes the length as the first payload byte, and opcode 249 adds 14 bytes at `Sprite.java:55`. RSLib `walk_encode_481` does not write the length (`rslib/src/both/move/rev_481.c:313`). The comment at line 20 calls that byte framing | implementable |
| Dynamic 26-bit masks | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SieBefindenSichEinemMitgliederGebiet.java:387`, `SelectWorld.java:237`, `SelectWorld.java:278` | Three sites mask the same integer with different literals. [region.md](region.md) copies each literal | implementable |
| Config group 6 | `builds/rs2/481-2007-12-11/src/src/main/java/deob/StringInterface.java:434` | `opStringInterface19` was not read to its end | implementable |
| Config group 9 | `builds/rs2/481-2007-12-11/src/src/main/java/deob/ImageProducerFrameBuffer.java:324` | `call8e0819bb` was not read to its end. NPC types load this group | implementable |
| Config group 10 | `builds/rs2/481-2007-12-11/src/src/main/java/deob/PlayerEntity.java:391` | `opPlayerEntity31` was not read to its end | implementable |
| Map terrain and loc bytes | `MalformedLoginPacket.opkind2289`, `ByteArrayNodeSecondary.call099838cc`, `WorldMapFont.opWorldMapFont12`, `BenutzenSieDieWpasswortNdernOption.call9b06a231` | The byte opcodes inside a map file were not read. XTEA and the file ids are in [region.md](region.md) | implementable |
| Interface format with a nonzero first byte | `builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:3372` | `opSoftwareFont48` was not read past the common header | implementable |
| Interface script runner | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C63d74d54.java:558` | `opkind2844` was not read past the call that loads the group | implementable |
| Widget draw | `builds/rs2/481-2007-12-11/src/src/main/java/deob/C725f81a2.java:69` | `opkind2973` was not read. `opGameShell25` calls it with 765 by 503 | implementable |
| Terrain shade tail | `builds/rs2/481-2007-12-11/src/src/main/java/deob/World.java:69` | `opWorld79` continues after the occlusion walk | implementable |
| Music playback | `UnderwaterMaterialRenderer.opUnderwaterMaterialRenderer6` | Opcode 148 calls it with JS5 index 6. The method was not read | implementable |
| Sound effect playback | `Song.opkind3733` | Opcode 46 calls it with JS5 index 11. The method was not read | implementable |
| Area-sound tick | `Cd13a8822.opkind3222` | Called from `C55f3f558.java:498`. The method was not read. Opcode 159 only queues the id | implementable |
| Opcode 188 follow-up | `C218af8f4.opkind3083` | The three fields are in [packets.md](packets.md). The method they are passed to was not read | implementable |
| Second writer of C2S 219 | `builds/rs2/481-2007-12-11/src/src/main/java/deob/WallDecor.java:44` | `opWallDecor17` also writes opcode 219. That body was not read. The keepalive at `C55f3f558.java:752` has no payload | implementable |
| Named GL hooks | `builds/rs2/481-2007-12-11/src/src/main/java/deob/Client.java:243` | `Js5GlTextureProvider.opkind3141` and `opkind3143` were not read. The classes are Java. This tree has no native GL library | implementable |
