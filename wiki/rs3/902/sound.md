# Sound

The mix is `com.jagex.audio.api.BussType`. The loading screen builds it from a byte array in archive 28 (`deob/DataBlock374.java:307`) and calls `doBussType(50, 7340032)`. `Client.doClient2` ticks it (`rs2/client/Client.java:1576`).

Server opcode 145, size 10 (`DataBlock457.clsfbe8a099146`, `Client.java:8275`), starts one sound:

| Order | Width |
| --- | --- |
| 1 | `u16be`, and `65535` is stored as `-1` |
| 2 | `u8` |
| 3 | `u16be` |
| 4 | `u8` |
| 5 | `u16be` |
| 6 | `u16be` |

The six values are passed to `BussType.doBussType` and, if that returns a `World`, to a second `doBussType` with the last `u16be` and the third field. Which argument is the sound id, the volume, or the delay is the order above. The method names do not say more.

Vorbis types are `com.jagex.audio.vorbis.VorbisRelated` (`VorbisRelated.java:12`), `deob.VorbisRelatedPart`, and `deob.VorbisRelatedPart2`. `DummySoundBackend` (`com/jagex/audio/backend/DummySoundBackend.java:44`) is the backend stub that was named, not decoded. Sample synthesis and the vorbis packet layout were not read.
