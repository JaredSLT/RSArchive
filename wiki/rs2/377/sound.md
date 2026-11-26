# Sound

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`.

There is no vorbis path and no `jagtheora`. Synth is `Wave` plus `Tone`. Midi is an ondemand file, archive 2, played through the host strings on `CouldNotOpenSocket` (`deob/CouldNotOpenSocket.java:31`).

## Synth archive

`sounds.dat` from the sounds jag is `Wave.unpack` (`jagex2/sound/Wave.java:20`). It loops `u16be` id until 65535, with at most 5000 tracks. Each track is `opWave2` (`Wave.java:60`): up to 10 tones, each a nonzero `u8` which is rewound and passed to `Tone.read`, then `u16be` and `u16be`. `trim` stores `Wave.delays[id]`.

`Tone.read` (`jagex2/sound/Tone.java:276`), `Envelope.readShape` (`jagex2/sound/Envelope.java:25`), and `Filter.opFilter5` (`jagex2/sound/Filter.java:109`) were not read.

## Packets that start sound

S2C 26, size 5 (`jagex2/client/Client.java:2507`): `u16be` id, `u8` volume, `u16be` delay. Delay 65535 queues the sound with delay 0. Otherwise the delay is `delay + Wave.delays[id]`, and only when sound is enabled, the client is not lowmem, and fewer than 50 sounds are queued.

Zone opcode 41, size 4 (`Client.java:12602`): `u8` tile offset from the zone base, `u16be` wave id, `u8` with radius `>> 4` and loop `& 7`. It queues when the player is inside the radius, sound is enabled, the client is not lowmem, and the queue is under 50. The delay is `Wave.delays[id]`.

Playback is `opClient127` (`Client.java:13909`). It calls `Wave.opWave1(volume, id)` and submits the PCM. On failure, if `CouldNotOpenSocket.reporterror` is set it sends C2S 80 `u16be` `id & 32767`. Otherwise C2S 80 `u16be` `-1` (`Client.java:13935`). `reporterror` is initialized true (`CouldNotOpenSocket.java:33`).

## Midi

S2C 220, size 2: `g2_alt3` song id. 65535 means -1 and stops the request. Otherwise `OnDemand.request(2, id)` (`Client.java:2615`).

S2C 249, size 5: `g2_alt1` song id, `g3_alt3` delay (`Client.java:2632`). The delay is stored in `valueClient1255` and the same archive 2 request is made.

`CouldNotOpenSocket.midisave` writes the midi bytes when `savereq` is null, cycles `midipos` through 0..4, and sets `savereq` to `jingle` plus that index plus `.mid` (`deob/CouldNotOpenSocket.java:299`). The signlink loop then sets `midi` to the cache directory plus `savereq` (`CouldNotOpenSocket.java:146`). Wave replay uses `sound` plus `wavepos` plus `.wav` and sets `wave` the same way (`CouldNotOpenSocket.java:283`, `CouldNotOpenSocket.java:141`). `midivol` and `wavevol` are host fields (`CouldNotOpenSocket.java:38`, `CouldNotOpenSocket.java:44`).
