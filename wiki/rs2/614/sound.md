# Sound

S2C opcode 44, size 6, `Binding.valueBinding1`, is the packet that queues a sound (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1962).

| order | width | use |
| --- | --- | --- |
| 1 | u16be | id. 65535 is stored as -1 |
| 2 | u8 | passed as the volume byte |
| 3 | u16be | passed as the delay |
| 4 | u8 | passed through |

`LoadableResourceManager.opLoadableResourceManager2` (builds/rs2/614-2010-07-13/src/src/main/java/deob/LoadableResourceManager.java:16) queues `new JcaInit((byte)1, id, volumeByte, delay, lastByte, 0)` when `valuekind111223` is not 0, the volume byte is not 0, the queue has fewer than 50 entries, and the id is not -1. `valuekind111223` defaults to 127 (builds/rs2/614-2010-07-13/src/src/main/java/deob/VaaInit.java:39, builds/rs2/614-2010-07-13/src/src/main/java/deob/Ccd79e102.java:29) and is the volume multiplier at builds/rs2/614-2010-07-13/src/src/main/java/deob/Occluder.java:211.

S2C opcode 6, size 3, reads `u8`, `s8(128-b) << 3`, `s8 << 3` and clears sounds in an 8-tile window (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1868). It does not start a sound.

`JcaInit`, the synth classes, and the vorbis classes (`VorbisMapping`, `VorbisCodebook`, `VorbisResidue`) were not read as a decode. No packet was identified as the vorbis start. There is no `jagtheora` package. `deob/VideoOggTheoraRelated.java` and `deob/OggTheoraStreamRelated.java` are ordinary Java files. See `gaps.md`.
