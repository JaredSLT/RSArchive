# Sound

Archives: `SYNTH_SOUNDS` 4, `VORBIS` 14, `MIDI_SONGS` 6, `MIDI_JINGLES` 11, `MIDI_INSTRUMENTS` 15 (builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:19).

`Sound.valueSound173` is the type byte (builds/rs2/742-2012-11-07/src/client/src/main/java/Sound.java:45). `opSound52` is true for types 2 and 3. Playback (builds/rs2/742-2012-11-07/src/client/src/main/java/TextureOp19.java:103): type 1 loads `SynthSound` from `Statics.synthSounds`. Types 2 and 3 load `VorbisSound` from `Statics.vorbis`. The synth and vorbis file decoders were not read.

Packets that start a sound:

| opcode | name | size | fields | starts | citation |
| --- | --- | --- | --- | --- | --- |
| 15 | valueServerProt9 | 8 | u16be id (65535 means -1), u8, u16be, u8, u16be | `opTextureOp1960`, type 1 synth | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7548 |
| 26 | valueServerProt76 | 6 | u16be, u8, u16be, u8 | `opTextureOp1965(..., true, 256)`, type 3 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7838 |
| 39 | valueServerProt67 | 4 | u8, u16be (65535 means -1), u8_128sub | `opTextureOp1955`, midi | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8559 |
| 92 | valueServerProt66 | 2 | u16be (65535 means -1) | `opTextureOp1975`, midi request | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7560 |
| 100 | valueServerProt79 | 8 | u16be (65535 means -1), u8, u16be, u8, u16be | `opTextureOp1965(..., false, ...)`, type 2 | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6888 |
| 119 | valueServerProt88 | 6 | u16le_sub128lo id (65535 means -1), u8_128sub, u24_alt3 | `opTextureOp1974`, midi song | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6531 |
| 138 | valueServerProt105 | 0 | empty | `opTextureOp1977` stops type-3 sounds | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7834 |

Opcode 145 `valueServerProt112` is `u16be` into `DeveloperConsole.opDeveloperConsole17`. It does not reset sound (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8629).

Zone index 0 `valueZoneProt21` calls `opTextureOp1957`. Zone index 8 `valueZoneProt27` calls `opTextureOp1959`. Their field lists are on the zone rows in [packets.md](packets.md).

The client answers some midi packets with c2s `valueClientProt24` (`p4` of an id). That writer is the row for opcode 60 in [packets.md](packets.md).
