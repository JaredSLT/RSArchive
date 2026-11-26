# Sound

`MidiNote.opMidiNote10` opens two lines with `ReflectionCheck.opReflectionCheck13`: 22050 Hz at index 0 (`builds/rs2/474-2007-10-19/src/src/main/java/rt4/MidiNote.java:113`) and 2048 Hz at index 1 (`rt4/MidiNote.java:116`). `opReflectionCheck13` itself was not read (`rt4/ReflectionCheck.java:152`).

`Bzip2DState.opBzip2DState12` stores four objects and returns true (`builds/rs2/474-2007-10-19/src/src/main/java/rt4/Bzip2DState.java:69-74`):

| Argument | Archive |
| --- | --- |
| 1 | index 4, `QuickChatPhrase.valueQuickChatPhrase23`, stored as `SpielerBitteHlenSieEineAndereWelt.valuekind23022` |
| 2 | index 14, `LadeFreundeListe.valuekind23311`, stored as `MaterialManager_Class106_Sub1_Sub1.valueMaterialManager_Class106_Sub1_Sub13` |
| 3 | the synth object passed by the caller, stored as `SynthEnvelope.valueSynthEnvelope17` |
| 4 | index 15, `MaterialManager_Class106_Sub1_Sub1.valueMaterialManager_Class106_Sub1_Sub16`, stored as `JavaMouseWheel.valueJavaMouseWheel77` |

`VorbisFloor`, `VorbisResidue`, and `VorbisSound` are the vorbis types. Their decode and playback methods were not read.

The zone packet that starts an area sound is opcode 205. The fields are u8 coord, u16be sound id, u8 packed radius and loops, and u8 delay (`rt4/ShapedTile.java:119-137`). It is queued when the local player is inside the radius, `MidiNote.valueMidiNote22` is not 0, the loop byte is positive, and `PlayerEntity.valuePlayerEntity73` is below 50.

`ClientScript.call273a5669` also queues area sounds from animation frames. That method was not read.
