# Sound

## Music

S2C opcode 205 is one u16le. 65535 is stored as -1. The value is passed to `SpotAnimType.opSpotAnimType1` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:219`). A non-negative id calls `SieHabenGeradeEineAndereWeltVerlassen.callabfb936d` with archive `WeSuspectSomeoneKnowsPassword.valuekind1708`, which is js5 index 6 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/SpotAnimType.java:42`, `builds/rs2/443-2006-12-11/src/src/main/java/deob/Cb5815139.java:65`). That call sets `TextureOp38.valueTextureOp3815` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/SieHabenGeradeEineAndereWeltVerlassen.java:76`). `opkind1822` sets the same flag for a group id of 0 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/SieHabenGeradeEineAndereWeltVerlassen.java:40`).

Each logic tick calls `opkind1825` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:438`). When the flag is set it fetches the bytes with `AufDerHautpseite.opkind15418` and dispatches `Keyboard.opKeyboard4`, `Cd2890cde.opkind1941`, or `Whi.opkind1813` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/SieHabenGeradeEineAndereWeltVerlassen.java:83`). The MIDI clock is `TextureOp38.opTextureOp386`, called from the same tick. Its body was not read.

## Area sound

S2C opcode 109, and the same sub-opcode inside opcode 244, reads u8, u16be, u8, u8 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:275`). The first u8 splits into tile X `((117 & b) >> 4)` and tile Z `(b & 7)`. The third byte's high nibble and low 3 bits are a radius and a minimum distance. The queue is capped at 50. `VorbisSound.opVorbisSound4` is the reflection request, not a sound packet. See [packets.md](packets.md).

## Huffman

`TextureOp4.opTextureOp47` is `GlModel.opGlModel6` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/TextureOp4.java:82`, `builds/rs2/443-2006-12-11/src/src/main/java/deob/GlModel.java:197`). The smartu from `opMonotonicClock46` is the decoded character count, capped at 32767. The following bits are walked by `ServerActiveProperties.opServerActiveProperties5` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/ServerActiveProperties.java:173`).

Each source byte is tested from bit `0x80` down to bit `0x01`. A clear bit increments the node index. A set bit replaces the node with `valueServerActiveProperties18[node]`. When that table entry is negative, the decoder emits `(byte)(~entry)` and resets the node to 0. The return is `1 + end - start`, which counts the byte in progress because the loop breaks before incrementing the source position.

C2S chat uses `MidiNote.opMidiNote3` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MidiNote.java:76`). It writes a smart of the string length, then `ServerActiveProperties.callb37bcaf7` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/ServerActiveProperties.java:318`). `valueServerActiveProperties1[byte]` is the code length and `valueServerActiveProperties16[byte]` is the codeword. A length of 0 throws `No codeword for data value`. Bits are stored from the high end of each byte. The return is `(bitCursor + 7 >> 3) - start`.

`ServerActiveProperties` is constructed from a byte array of code lengths (`builds/rs2/443-2006-12-11/src/src/main/java/deob/ServerActiveProperties.java:407`). `valueServerActiveProperties1` is that array. For each non-zero length the constructor stores the codeword in `valueServerActiveProperties16` and inserts a leaf `~symbol` into `valueServerActiveProperties18`. A set bit follows the child. A clear bit increments the node index. The bit test starts at `-2147483648 >>> bit`.

The bytes come from js5 index 10, `World.valueWorld119.opkind15425`, with the name `huffman` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Component.java:16`) and the file name `""` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/World.java:29`). The call is `Cb5815139` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cb5815139.java:204`). `DirectByteArray.opDirectByteArray5` stores the object on `Timer.valueTimer16` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/DirectByteArray.java:499`). The name hash inside `opkind15425` was not read. See [gaps.md](gaps.md).
