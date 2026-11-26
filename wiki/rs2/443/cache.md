# Cache

The cache kind is js5. `Cb5815139` opens indices 0 through 13 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cb5815139.java:59`). Login writes a u32be CRC for each index in that same order. See [login.md](login.md).

| Index | Holder |
| --- | --- |
| 0 | `MonotonicClock.valueMonotonicClock6` |
| 1 | `C6e11accc.valuekind1602` |
| 2 | `DelayedStateChange.valueDelayedStateChange5` |
| 3 | `NichtHergestelltWerden.valuekind14012` |
| 4 | `NichtHergestelltWerden.valuekind1406` |
| 5 | `SoundBank.valueSoundBank25` |
| 6 | `WeSuspectSomeoneKnowsPassword.valuekind1708` |
| 7 | `SpotAnimType.valueSpotAnimType66` |
| 8 | `Ignorieren.valuekind17317` |
| 9 | `PacketBit.valuePacketBit67` |
| 10 | `World.valueWorld119` |
| 11 | `WenOchtenSieHinzufUgen.valuekind18021` |
| 12 | `C353edd75.valuekind17810` |
| 13 | `QuickChatCatType.valueQuickChatCatType1` |

Index 3 is the widget archive. `ClientScript.opClientScript4` stores it on `GlSolidColorTexture.valueGlSolidColorTexture4` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cb5815139.java:164`, `builds/rs2/443-2006-12-11/src/src/main/java/deob/ClientScript.java:38`). Index 5 is the map archive. See [region.md](region.md). Index 6 is the archive `SpotAnimType.opSpotAnimType1` passes to the music starter. See [sound.md](sound.md). Index 12 is the clientscript archive. The other holders are opened and checksummed. Their decode methods were not read.

## Reference table

`AufDerHautpseite.opkind15426` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/AufDerHautpseite.java:477`) stores `MidiPcmStream.opMidiPcmStream2` of the raw bytes, then decompresses with `SoftReferenceNode.opSoftReferenceNode4`. Those two helpers were not read.

On the decompressed buffer:

1. u8 format. The rest of the method runs only when this byte is 5.
2. u8 flags.
3. u16be group count.
4. That many u16be values, each added to a running id.
5. When flags are not 0, one u32be name hash per group, then a `ReferenceNode` over those hashes.
6. One u32be per group into `valuekind15417`.
7. One u32be per group into `valuekind15416`.
8. One u16be file count per group.
9. For each group, that many u16be values, each added to a running file id.
10. When flags are not 0, one u32be per file id, then a `ReferenceNode` per group.

## Clientscript

`PlayerEntity.opPlayerEntity8` loads group = script id, file 0, from index 12 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PlayerEntity.java:67`). The last 12 bytes are the header: u32be instruction count, then four u16be. The body starts at position 0 and stops 12 bytes before the end. Each instruction is a u16be opcode. Opcode 3 reads a jstr. An opcode below 100, other than 21, 38, and 39, reads a u32be. Every other opcode reads a u8.

## Widget bytes

`SoundBank.opSoundBank2` loads one interface group from index 3 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/SoundBank.java:187`). A file whose first byte is -1 uses `PacketBit.opPacketBit8`. Any other first byte uses `PacketBit.opPacketBit6`. The component id is `file + (group << 16)`. The type bodies are in [interfaces.md](interfaces.md).

Obj, npc, loc, seq, and spotanim opcode lists were not read. See [gaps.md](gaps.md).
