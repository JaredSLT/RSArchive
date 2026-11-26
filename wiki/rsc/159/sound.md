# Sound

Sound is PCM bytes inside `sounds` + `Version.number9` + `.mem`. `Version.number9` is 1 (`Version.java:21`). `MudClient.load` stores the jag in `soundEffects` (`MudClient.java:1197`). There is no synth and no vorbis.

`MudClient.call711f4ca5` (`MudClient.java:1517-1522`) looks up `name + ".pcm"` with `Tools.call8b8e7e32` (offset) and `Tools.call188` (decompressed length) and passes those to `PcmPlayer.call413`. Playback is skipped when `flag198` is true. `flag198` starts false (`MudClient.java:9342`). Options opcode 213 with setting 3 toggles it on a members client. S2c opcode 228 sets `flag198` from its third `u8` (`MudClient.java:4050-4053`).

`PcmPlayer` extends `InputStream` (`PcmPlayer.java:8`). The constructor calls `AudioPlayer.player.start(this)`. `call413` sets the byte span. `read` copies that span and then writes `-1` for any further requested byte. `call412` calls `AudioPlayer.player.stop`.

`sun.audio.AudioPlayer` in this tree is an 18-line stub. `start` and `stop` are empty (`sun/audio/AudioPlayer.java`). It is not `jaggl`, `jagdx`, or `jagtheora`.

## Packets

S2c opcode 207 is `new String(payload, 1, length - 1)` and that string is passed to `call711f4ca5` (`MudClient.java:4356-4358`).

S2c opcode 227 plays `"prayeron"` when a prayer byte changes from off to on, and `"prayeroff"` when it changes from on to off (`MudClient.java:4057-4068`). Those two names are local. The prayer clicks do the same: opcode 212 plays `"prayeron"`, opcode 211 plays `"prayeroff"` (`MudClient.java:8021-8031`).

Members-only loading is the `.mem` path guarded by `flag182` for entity art. The sound jag load at line 1197 is not inside that members test. `call711f4ca5` still no-ops when `aPcmPlayer` is null.
