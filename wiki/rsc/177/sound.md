# Sound

Members only. `call48dc0195` loads `sounds1.mem` and constructs `PcmPlayer` (`deob/MudClient.java:1287`). A failure is caught and printed. The free client leaves `aPcmPlayer` null.

`PcmPlayer` extends `InputStream`. Its constructor calls `AudioPlayer.player.start(this)` (`jagex/client/PcmPlayer.java:16`). `call70` points the stream at a byte range. `read` copies those bytes and then writes `-1` (`jagex/client/PcmPlayer.java:33`).

`sun.audio.AudioPlayer` in this tree is a compile stub. `start` and `stop` are empty (`sun/audio/AudioPlayer.java:13`). No samples leave this process until that stub is replaced. See [gaps](gaps.md).

`MudClient.call711f4ca5(name)` plays `name + ".pcm"` from the sounds jag via `Tools.getJagOffset` and `Tools.getJagSize` (`deob/MudClient.java:1582`). It does nothing when `soundEffectsMode` is true. That flag means sound is off. Server opcode 228 sets it from one u8 (`deob/MudClient.java:7105`). The options panel sends opcode 213, id 3, with the new flag (`deob/MudClient.java:13891`).

Server opcode 207 is a sound name: the bytes from offset 1 through `length - 1`, passed to `call711f4ca5` (`deob/MudClient.java:7497`). Prayer opcode 227 plays `prayeron` or `prayeroff` when a byte flips (`deob/MudClient.java:7112`).

Client opcode 207 is the bank-close packet. It is not a sound. Direction distinguishes them. See [packets](packets.md).
