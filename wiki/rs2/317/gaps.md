# Gaps

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| S2C opcode 47, size 6, has no field reads | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:13254` | `Client.read` logs `T1` and calls `logout`. The payload bytes are framed and discarded. | implementable |
| S2C opcode 254 is size 6, and types other than 2..6 do not read 6 bytes | `Client.java:12710` | Type 1 reads `u8` plus `u16be`. Type 10 reads `u8` plus `u16be`. Any other type reads only the `u8`. The extra bytes stay inside the consumed frame. | implementable |
| RSLib `rs_sizes_c2s_317` | `rslib/src/client/packets/317.c:3` | `Protocol.java` has no client size array. Client packets are sized by the writes in `Client.java`, including `psize1`. | implementable |
| RSLib server bind list omits opcodes this client handles | `rslib/src/server/packets/317.c:40` | Java handles 24, 68, 78, 84, 147, 164, 187, 215, and 218. The first 256 size values match `Protocol.serverProtSizes`. This is a missing bind, not a size mismatch. | implementable |
| RSLib names opcode 153 `RS_C2S_NPC5` | `rslib/src/client/packets/317.c:54` | Java writes opcode 153 from player menu action 779. `Client.java:4733`. | implementable |
| `Model.unpack` vertex and face bytes | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/graphics/Model.java:130` | The 18-byte tail and section offsets were read. The coordinate and face bytes after those offsets were not. | implementable |
| `AnimFrame.unpack` transform payload | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/graphics/AnimFrame.java:67` | Header counts, frame id, base id, and transform count were read. The bytes selected by each transform `u8` were not. | implementable |
| `CollisionMap` flag bits | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/dash3d/CollisionMap.java:50` | `addWall`, `addLoc`, and the reach tests were identified. The bit values they store were not read. | implementable |
| `Tone.generate` and `Wave.getWave` sample loop | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/sound/Tone.java:59` | The wave, tone, envelope, and filter read formats were read. The PCM loop was not. | implementable |
| `BZip2.read` bitstream | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/io/BZip2.java:8` | `Jagfile.unpack` calls it. The block format was not field-read. | implementable |
| `OnDemand.read` socket framing and `FileStream` sectors | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/io/OnDemand.java:94` | `map_index`, `getMapFile`, and the archive numbers the client requests were read. The prefetch socket bytes were not. | implementable |
| `WordFilter` dictionaries other than tlds | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/wordenc/WordFilter.java:69` | `read` and `readTld` were read. `readBadWords`, `readDomains`, `readFragments`, and `readBadCombinations` were not. | implementable |
| `Isaac.init` seed mixing | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/io/Isaac.java:66` | `nextInt` and the start of `isaac` were read. The golden-ratio mix in `init` was not copied. | implementable |
| `World.perlin` | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/dash3d/World.java:54` | Land opcode 0 calls `perlin(932731 + x + originX, 556238 + z + originZ)`. The noise body was not read. | implementable |
| `Pix3D` triangle fill bodies | `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/graphics/Pix3D.java:407` | `fillGouraudTriangle`, `fillTriangle`, and `fillTexturedTriangle` were located. Their edge walks were not line-read. | implementable |
| Design identikit defaults are data, not the constants 0, 10, 18, 26, 33, 36, 42 | `Client.java:2912` | `validateCharacterDesign` stores the first `IdkType` whose disable flag is clear and whose type matches the slot. | implementable |
