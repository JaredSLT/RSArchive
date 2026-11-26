# Sound

Waves are software synth. There is no vorbis decoder and no `jagtheora` class.

S2C opcode 25, size 5 (`Client.java:3741`), starts a wave: u16be id, u8 loops, u16be delay. The delay stored on the client is that u16be plus `Wave.delay[id]`. Playback goes through `updateAudio` (`Client.java:5395`) and `Wave.generate`. `CouldNotOpenSocket.wavesave` is the platform write. The call from this client is `saveWave` (`Client.java:8335`).

`sounds.dat` is unpacked by `Wave.unpack` (`Wave.java:17`) when `lowMem` is false (`Client.java:6076`). The file is a sequence of u16be ids until 65535. Each wave is `Wave.read` (`Wave.java:57`): up to 10 tones. A u8 of 0 skips that tone. A non-zero u8 is rewound and `Tone.read` consumes the tone. After the ten slots, u16be loop begin and u16be loop end.

`Tone.read` (`Tone.java:197`) reads two envelopes (frequency base, amplitude base). A following u8 of 0 skips a pair. A non-zero u8 is rewound and two more envelopes are read. That pattern is repeated for amplitude modulation and for release/attack. Then up to 10 harmonics: `gsmarts` volume, stop on 0, `gsmart` semitone, `gsmarts` delay. Then `gsmarts` reverb delay, `gsmarts` reverb volume, u16be length, u16be start.

`Envelope.unpack` (`Envelope.java:26`): u8 form, u32be start, u32be end, u8 length, then that many pairs of u16be shape delta and u16be shape peak.

Midi uses ondemand archive 2. S2C 163 is a u16be song id, and 65535 means stop (`Client.java:3586`). S2C 242 is u16be song id and u16be delay (`Client.java:3603`). `CouldNotOpenSocket.midisave` and `midifade` are the platform calls. Varp client code 3 sets the midi volume (`Client.java:5486`). Varp client code 4 sets the wave volume (`Client.java:5525`).
