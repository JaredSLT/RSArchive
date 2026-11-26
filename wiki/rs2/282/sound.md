# Sound

There is no vorbis path. Synth comes from `sounds.dat` in archive 8. Midis and jingles are ondemand archive 2.

`Wave.opkind611` (`builds/rs2/282-2004-12-21/src/src/main/java/deob/Wave.java:18`) reads `u16be` ids until 65535, builds up to 1000 `Wave` objects, and stores a delay in `Wave.valuekind616[id]`.

`Wave.opkind613` (`Wave.java:58`) reads up to 10 tones. A `u8` of 0 skips that slot. A non-zero byte is rewound and `Tone.opkind44` follows. Then two `u16be` values.

`Tone.opkind44` (`Tone.java:272`):

- two mandatory `Envelope.opkind211`
- three optional pairs: a `u8` of 0 skips the pair; a non-zero byte is rewound and two envelopes follow
- up to 10 steps: `opkind2727` (0 ends the list), `opkind2726`, `opkind2727`
- two more `opkind2727`
- two `u16be`
- `Filter.opkind475`

`Envelope.opkind211` (`Envelope.java:23`): `u8` form, `i32be` start, `i32be` end, then `opkind212`: `u8` phase count, and per phase `u16be` delta and `u16be` amplitude.

`Filter.opkind475` (`Filter.java:106`): `u8` whose high nibble is pair count 0 and whose low nibble is pair count 1. A zero byte stores both counts as 0 and stops. Otherwise: `u16be`, `u16be`, `u8` bitfield, then for each of the two bands and each pair two `u16be`. When the bit for that pair is set, two further `u16be` replace the second copy; otherwise the first pair is copied. If the bitfield is non-zero or the two leading `u16be` differ, one more `Envelope.opkind212` is read.

Packets that start audio, all in `Client.read`:

| opcode | size | fields | effect |
| --- | --- | --- | --- |
| 34 | 5 | `u16be` wave id, `u8`, `u16be` delay | queues the wave when sound is on, the client is not lowmem, and fewer than 50 waves are queued. The delay is added to `Wave.valuekind616[id]`. `Client.java:7486` |
| 23 | 2 | `u16be` midi, 65535 becomes -1 | sets the current song and, when it changes, requests ondemand archive 2. `Client.java:6502` |
| 15 | 4 | `u16be` jingle, `u16be` delay | requests archive 2 and does not loop. `Client.java:6519` |

`CouldNotOpenSocket.wavesave` (`sign/CouldNotOpenSocket.java:273`) stores a byte buffer as `soundN.wav` when the length is at most 2000000 and no save is pending. `wavereplay` (`:288`) replays the current wave name with a null buffer. `midisave` (`:299`) stores a buffer as `jingleN.mid` under the same length cap. `Wave.opkind614` and `Wave.opkind615` mix samples; those bodies were not line-read. `updateAudio` (`Client.java:4080`) is the per-tick consumer and was not line-read past the wave-id compare.
