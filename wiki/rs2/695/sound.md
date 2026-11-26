# Sound

`VorbisSound` extends `C2b2c3dbe` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/VorbisSound.java:3`). `VorbisSound.call012f556d` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/VorbisSound.java:24`) mixes samples into an `int[]`: it steps a byte sample by `var4 >> 8`, interpolates with the next byte, and adds `(sample * var6) >> 6`.

Package `jagtheora` (ogg, theora, vorbis, misc) is platform-native. `Cfa8219d8.opkind27068` constructs `VorbisInfo` and `VorbisComment` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Cfa8219d8.java:165`). That method is not the c2s opcode 56 sender.

The packet that starts a sound was not found among the decompiled senders in `packets.md`.
