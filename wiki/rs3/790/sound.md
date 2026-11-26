# Sound

`PositionedSound` (`com/jagex/game/world/entity/PositionedSound.java:1`) is the packet buffer. Its read and write methods are the codecs in [framing.md](framing.md).

`Client.doClient` allocates `VorbisRelatedPart3[]` with length `DataBlock1979.doDataBlock1979222()` and, per archive, calls `doDataBlock2372` and `doVorbisRelatedPart32222b` (`rs2/client/Client.java:2151`). That is the per-archive vorbis setup during cache open. The method that starts one synth or vorbis sound from a server packet was not read.

`jagtheora` is platform-native. The login block writes one `u8` for whether that library is present (`deob/Statics.java:2828`). `Statics.jagtheora` is the login state machine, and the name is the decompiler's.

Server opcode 118 reads one `jstr` and passes it to `WorldMapRenderer.doWorldMapRenderer22` (`rs2/client/Client.java:8429`, `com/jagex/game/world/WorldMapRenderer.java:179`), which stores the string. That handler is a string assignment.
