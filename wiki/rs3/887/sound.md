# Sound

Server opcode 38, size 6 (`builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock832.java:82`), is handled at `builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6606`.

| order | width | use |
| --- | --- | --- |
| 1 | u8_neg | index |
| 2 | u32be | passed to `AudioStream2.doAudioStream2` |
| 3 | u8 | passed to `AudioStream2.doAudioStream22` |

The stream array is `objIconFactory.audioStream2Array`. This packet sets values on an existing stream. It was not shown to start a sound.

`com.jagex.audio.stream.AudioStream2`, `com.jagex.audio.vorbis.VorbisRelated`, and `com.jagex.audio.backend.AudioMixer` are the named synth and vorbis types. Their decode and mix methods were not read. No `jagtheora` sources are in this tree. The packet that starts a synth or a vorbis sound was not isolated.
