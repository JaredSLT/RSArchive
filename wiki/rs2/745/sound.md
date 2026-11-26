# Sound

`C2a31b481.opkind35109` opens `AudioChannel` at 22050 Hz and 2 channels, then a stream of 22050 and a second stream of 2048 (`C2a31b481.java:15-22`). `Loading.java:283` calls that during startup. The per-tick call is `opkind351022` from `Client.opClient490` (`Client.java:893`, `C2a31b481.java:60-64`).

Music is S2C opcode 147, size 4 (`Client.java:7725-7733`): u8sub128, u8add128, u16le. 65535 becomes -1. Those three values are passed to `C2a31b481.opkind351010` as volume, a second integer, and the archive group. The group is loaded from archive id 6, `Statics.valueStatics627` (`C2a31b481.java:274`). When the id is -1 the player stops. Starting a track can also send game opcode 36, a u32be `-1` (`C2a31b481.java:266-268`).

Jingles are S2C opcode 53, size 6 (`Client.java:8174-8182`): u24be, u8neg, u16le_add128. 65535 becomes -1. `opkind35105` loads the group from archive id 11, `Statics.valueStatics1075`, then calls `MidiPlayer.opkind32304(-1, 255)` (`C2a31b481.java:298-309`).

`PositionedSound.opkind39553` runs from NPC spawn and from the player appearance decoder (`Client.java:9764`, `PlayerEntity.java:176`). It is not a packet. Synth, MIDI event, and vorbis classes were not decoded. `jagtheora` is platform-native. The login block writes one u8 from `HardwarePlatformLoader.opHardwarePlatformLoader16("jagtheora")` (`Statics.java:2313`).
