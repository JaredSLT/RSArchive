# Sound

Synth, not vorbis. The sounds jag file `sounds.dat` is passed to `Wave.opkind641` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:13652`).

`Wave.opkind641` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/Wave.java:17`) loops on `u16be` id until 65535. Each wave calls `opkind643`. That method reads up to 10 tones: `u8` 0 skips a slot, otherwise the byte is pushed back and `Tone.opkind994` decodes one tone (`Wave.java:59`). Two `u16be` values follow (`valuekind643`, `valuekind644`). `Tone.opkind994` (`Tone.java:275`) was not read. See [gaps.md](gaps.md).

Playback: `Client.updateAudio` (`Client.java:3851`) calls `Wave.opkind642` and then `opClient56`, which plays bytes through `CouldNotOpenSocket`. `opClient87` calls `CouldNotOpenSocket.wavereplay` (`Client.java:11851`).

## Packets that start a sound

S2C opcode 104, size 5 (`Client.java:4912`): `u16be` id, `u8` loops or volume stored in `valueClient911`, `u16be` delay added to `Wave.valuekind649[id]`. The queue is skipped when sound is off, low memory is on, or 50 sounds are already queued.

S2C zone opcode 40, size 4 (`Client.java:6770`): `u8` position, `u16be` id, `u8` whose high nibble is a radius and low 3 bits are the type. The sound is queued only when the local player tile is inside the radius.

## Midi

S2C opcode 229, size 2 (`Client.java:4013`): `u16be` music id. 65535 is stored as -1. A new id requests ondemand archive 2 when music is enabled, low memory is off, and `valueClient787` is 0.

S2C opcode 22, size 5 (`Client.java:4030`): `u24mix` delay, `u16le` music id. The client requests archive 2 and stores the delay in `valueClient787` when music is enabled and low memory is off.

Archive 2 is midi in the versionlist. See [cache.md](cache.md). There is no vorbis decoder in this client.
