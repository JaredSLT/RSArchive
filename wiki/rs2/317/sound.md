# Sound

This client synthesizes waves. It does not use vorbis. Midi songs are ondemand archive 2 files.

## Packets

Server opcode 174, size 5: `u16be` wave id, `u8` loops, `u16be` delay. The delay stored is that value plus `Wave.delays[id]`. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:12531`.

Zone opcode 105, size 4: `u8` coordinate, `u16be` wave id, `u8` with radius in the high 4 bits and loops in the low 3 bits. It plays when the player is inside the radius. `Client.java:11366`.

Opcode 74, size 2: `u16le` song, 65535 becomes -1, requested from ondemand archive 2. Opcode 121, size 4: `u16le_add` song and `u16be_add` delay. `Client.java:12192`, `Client.java:12209`.

`updateAudio` calls `Wave.generate(loops, id)` and `CouldNotOpenSocket.wavesave`. `Client.java:7809`, `CouldNotOpenSocket.java:273`. Midi bytes go to `CouldNotOpenSocket.midisave`. `CouldNotOpenSocket.java:299`. Varp client code 3 sets midi volume and client code 4 sets wave volume. `Client.java:2132`.

Low memory skips `sounds.dat`. `Client.java:8247`.

## Wave file

`Wave.unpack` reads `u16be` id until 65535. Each wave is `Wave.read`: up to 10 tones. A nonzero `u8` is rewound and `Tone.read` consumes it. Then `u16be` loop begin and `u16be` loop end. `Wave.delays[id]` is `trim()`, the minimum of `tone.start / 20` and `loopBegin / 20` when the loop range is ordered. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/sound/Wave.java:20`, `Wave.java:60`, `Wave.java:79`.

`Tone.read` at `Tone.java:275` reads two envelopes, frequency and amplitude. A following `u8` of zero skips a pair. A nonzero byte is rewound and two more envelopes are read. That pattern is used for frequency modulation, amplitude modulation, and release plus attack. Then up to 10 harmonics: `gsmarts` volume, stop on 0, `gsmart` semitone, `gsmarts` delay. Then `gsmarts` reverb delay, `gsmarts` reverb volume, `u16be` length, `u16be` start, and `Filter.read`.

`Envelope.read` at `Envelope.java:28` is `u8` form, `i32be` start, `i32be` end, then `readShape`: `u8` length and that many pairs of `u16be` delta and `u16be` peak.

`Filter.read` at `Filter.java:111` is `u8` whose high nibble and low nibble are pair counts. A zero byte sets both unities to 0 and stops. Otherwise `u16be` unity 0, `u16be` unity 1, `u8` flags. For each of two bands and each pair, `u16be` frequency and `u16be` range are stored in slot 0. Slot 1 is another pair when flag bit `(1 << (band * 4 + pair))` is set, otherwise it copies slot 0. If the flag byte is nonzero or the unities differ, the envelope's `readShape` is called.

`Tone.generate` at `Tone.java:59` and `Wave.getWave` at `Wave.java:116` produce the PCM samples. Those loops were not field-read. See `gaps.md`.
