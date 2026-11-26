# Sound

## Packet

S2C opcode 103, size 8, `C66af5d01.valuekind17656` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C66af5d01.java:7). Handler at builds/rs2/634-2010-12-10/src/src/main/java/deob/C4d106383.java:1501:

| Order | Width | Use |
| --- | --- | --- |
| 1 | u16be | sound id. 65535 becomes -1 |
| 2 | u8 | passed as the third int to `opkind17223` |
| 3 | u16be | passed as the fourth int |
| 4 | u8 | passed as the first int |
| 5 | u16be | passed as the fifth int |

`C1f1db4b8.opkind17223` queues a `C0d93e06d` on `Cc8a65b4b.valuekind14412` when preferences volume `opkind17696` is non-zero, the third int is non-zero, the id is not -1, and the queue length is below 50 (builds/rs2/634-2010-12-10/src/src/main/java/deob/C1f1db4b8.java:23).

S2C opcode 64, size 2, `C788c6509.valuekind16622` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C788c6509.java:5). The handler reads `u8_sub128` then `u8_add128` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C4d106383.java:1916). When the first byte is 255 both values become -1. `C1866ac0c.opkind11521` stores those two ints on a `C934624a0` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C1866ac0c.java:9). It does not queue a sound.

`AudioThread` mixes audio and also owns the player-update entry `opAudioThread46`. The mix methods were not read. Player update is in [sync.md](sync.md).

## Archives and native code

Music archive is JS5 index 6. The client requests group name `scape main` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:186).

Sound config list `C0d93e06d.valuekind147016` is `Cf21da29e2` over archive 20 plus archives 0 and 1 (builds/rs2/634-2010-12-10/src/src/main/java/deob/Lobbyscreen.java:232). Its decode method was not read.

Named synth and vorbis classes in this client include `VorbisSound`, `VorbisCodebook`, `VorbisFloor`, `Sound`, `SoundBank`, `SynthEnvelope`, and `MidiDecoder`. Their decode methods were not read.

`jagtheora` is platform-native. The lobby login packet writes `u8` 1 when `C6e51e196.opkind13281` loads `jagtheora`, else 0 (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:190).
