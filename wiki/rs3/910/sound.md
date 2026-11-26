# Sound

Vorbis data is JS5 archive 14 (`Js5Archive.VORBIS`). The client also has a separate synth packet. Both call `audioApi.playSound` with `SoundType.soundType10`.

## Packets that start a sound

| packet | opcode | size | body | source |
| --- | --- | --- | --- | --- |
| SYNTH_SOUND | 182 | 8 | `g2` id (65535 means -1), `g1`, `g2`, `g1`, `g2`, then `playSound` on `SFX_SUB` | `Client.java:9294` |
| VORBIS_SOUND | 37 | 8 | the same five fields, then `playSound` on `SFX_SUB` | `Client.java:11390` |
| VORBIS_SPEECH_SOUND | see packets.md | | `g2` id (65535 means -1), `g1`, `g2`, `g1`, then `playSound` on `DIALOG_SUB` | `Client.java:9613` |
| MIDI_SONG | 17 | 3 | `g2_alt3` id (65535 means -1), `g1_alt2` volume, then `playSong` | `Client.java:10407` |
| MIDI_SONG_LOCATION | see packets.md | | `g4_alt3` packed coordinate, `g4s` song, `g1_alt1`, `g1`, `g1_alt2`, then a positioned `playSong` | `Client.java:10026` |
| MIDI_SONG_STOP | see packets.md | 0 | `audioApi.stopSong` | `Client.java:10436` |
| VORBIS_SOUND_GROUP_START | see packets.md | | `g2`, then `audioApi.startGroup` | `Client.java:11376` |
| VORBIS_PRELOAD_SOUNDS | see packets.md | | `g2`, then `preloadSounds` | `Client.java:9578` |

Zone opcode 14 `SOUND_AREA` (`Client.java:11523`) plays a positioned effect on `SFX_SUB` when the local player is inside the radius. The tile byte, id, range, and two further bytes are in `region.md`.

The vorbis decoder class was not opened. Synth versus vorbis is selected by which server packet arrived, not by a flag inside one shared packet.
