# Sound

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`.

Loading stage 45 (`Client.java:1188-1201`):

- `McranTitreCharg.opkind5102` opens a channel at 22050 Hz on `MouseWheel.valueMouseWheel51`.
- `SignLinkAudioChannel.opSignLinkAudioChannel2` takes index 14 (`ObjStackNode.valueObjStackNode68`), the software instrument, index 15 (`ChargementDesPolices.valuekind57610`), and index 4 (`C747a18c6.valuekind5021`).
- A second `opkind5102` uses 2048 Hz and a `TextureOp29SubOp3`.
- `WwwWtqa.valuekind4344` is a `TextureOpBinary` at 22050 Hz with `C45440f3e.valuekind4806`.
- `AreaSound.valueAreaSound9` is a group id from index 6 (`ChargementDesPolices.valuekind57612`) named by `SynthInstrument.valueSynthInstrument14`. That string assignment was not opened.

S2C zone opcode 115, size 5, in `C0bfaa0f7.opkind4571` (`C0bfaa0f7.java:266-288`): `u8` coordinate, `u16be` sound id (65535 means -1), `u8` whose `(byte & 244) >> 4` is the distance field and whose low 3 bits are stored as `Cdb4daca5.valuekind5255`, `u8` stored as `RunescapeCom.valuekind4955`. The distance check uses that field plus one. The packed position is `(localZ << 8) + (localX << 16) + distanceField` (`C0bfaa0f7.java:286`). The client queues it only when the local player is inside that distance, `TextureOp29SubOp4.valueTextureOp29SubOp4153 != 0`, the low 3 bits are non-zero, fewer than 50 sounds are queued, and the id is non-zero.

S2C 138, size 2 (`DelayedStateChange.java:236-243`): `u16le_sub128lo` (65535 means -1) passed to `C9c57aa59.opkind39811`.

Vorbis types in this tree are `VorbisCodebook`, `VorbisMapping`, `VorbisSound`, and `PacketBit` (residue, not the bit buffer). Synth types are `SynthInstrument`, `SynthEnvelope`, `SoundPcmStream`, and `JavaAudioChannel`. Their decode methods were not transcribed. The huffman bit tree is `BrokenOutputStream.valueBrokenOutputStream53.opMaterialManager_Class106_Sub2_Sub112`, called from `Cd2e07e2c.call7d8dba53` (`Cd2e07e2c.java:32`). The tree itself was not read.
