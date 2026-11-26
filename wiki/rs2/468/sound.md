# Sound

`SoundPcmStream` is the socket. `MidiPcmStream` is ISAAC. Neither class is the synth.

The synth queue is `ProjAnimNode.valueProjAnimNode18`, capped at 50. `TextureOpCurve.opTextureOpCurve15` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/TextureOpCurve.java:211`) pushes an entry when `IntNode.valueIntNode13 != 0` and the third argument is not 0:

| Slot field | Value |
| --- | --- |
| `VertexNormal.valueVertexNormal34` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/VertexNormal.java`) | first argument, the id |
| `TextureOp14.valueTextureOp148` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp14.java`) | third argument |
| `JavaAudioChannel.valueJavaAudioChannel25` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/JavaAudioChannel.java`) | second argument |
| `VorbisSound.valueVorbisSound35` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/VorbisSound.java`) | null |
| `ClientScript.valueClientScript45` | 0 |

Server opcode 99 calls that with u16be id, u8, u16be (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1203`). The call is `opTextureOpCurve15(u16be, u16be, u8)`, so the u8 is the third argument and the second u16be is the second argument.

Zone opcode 195 pushes the same arrays from inside the zone decoder when the local player is within the radius (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:280`). `ClientScript.valueClientScript45` is packed as `(x << 16) + (z << 8) + radiusNibble`. `VorbisSound.valueVorbisSound35` is set null there as well. The radius test uses `valueSoftReferenceNode130` as x and `valueSoftReferenceNode108` as z.

Server opcode 10 is one u16le with low byte `+ 128` (65535 becomes -1) passed to `C99aed688.opkind2056` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1386`). That callee was not read.

Server opcode 112 is u16be with low `+ 128` (65535 becomes -1) and a u24be, passed to `SpotAnimType.opSpotAnimType17` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1397`). That callee was not read.

`VorbisSound`, `VorbisFloor`, `VorbisResidue`, `deob.PacketBit`, and `MapMarker.opMapMarker11` are the vorbis types in this tree. Their decode opcodes and the packet that starts a song were not read. `Song` is archive 6 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5.java:95`). `PcmStream.opPcmStream3` runs from the cache tick states. Its body was not read.
