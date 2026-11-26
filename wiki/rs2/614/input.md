# Input

Mouse and key records are drained at the start of the client tick into two queues (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:1991). A menu click reaches `ParticleEmitter.opParticleEmitter4` (builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:30) from `VorbisMapping` and `TriggerType` and `CutsceneSpline`, always with the boolean argument false. That boolean is the guard for several `opUnused6157` writes, so those writes do happen on the calls that were found.

Walk and minimap share `KeyHeldBinding.opKeyHeldBinding4` (builds/rs2/614-2010-07-13/src/src/main/java/deob/KeyHeldBinding.java:47), reached when the menu action is 21 (builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:248). The third argument selects the opcode. 0 writes C2S opcode 26, size 5. 1 writes C2S opcode 7, size 18. Both then write:

| order | width | value |
| --- | --- | --- |
| 1 | u8(v+128) | 1 when wearpos key 82 is set, else 0. That key is the run toggle read by `opWearposDefaults9` |
| 2 | u16le | first coordinate plus baseZ (`HashMap.valueHashMap10`) |
| 3 | u16, high byte then low byte plus 128 | baseX (`Vector3i.valueVector3i1`) plus the second coordinate |

Opcode 7 then appends `u8, u8, u16be, u8 57, u8, u8, u8 89, u16be, u16be, u8 63` (builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:254). The two u16be values are the local player's fine coordinates. Opcode 26 does not append them.

Other menu actions whose bodies were read in the same method:

| menu action | C2S opcode | size | body |
| --- | --- | --- | --- |
| 4 | 29 | 7 | u16le, u16le, u8(128-v) run, u16(hi, lo+128) |
| 1008 | 80 | 7 | u8(128-v) run, u16be, u16(hi, lo+128), u16le |
| 8 | 52 | 7 | u16be, u16(lo+128, hi), u8(v+128) run, u16(lo+128, hi) |
| 9 | 78 | 7 | u16(lo+128, hi), u16le, u16le, u8neg run |
| 1009 | 2 | 2 | u16be |
| 13 | 35 | 7 | u16le, u16(lo+128, hi), u8(128-v) run, u16be |
| 45 | 1 | 7 | u16(lo+128, hi), u16(lo+128, hi), u8(v+128) run, u16(hi, lo+128) |
| 3 | 9 | 15 | u16(lo+128, hi), u16(hi, lo+128), u16be, u16be, u8 run, u32be, u16be |
| 1007 | 20 | 2 | u16be |
| 50 | 48 | 6 | u16(lo+128, hi), then four bytes `(v>>16), (v>>24), v, (v>>8)` (`FullscreenMode.opFullscreenMode8`, builds/rs2/614-2010-07-13/src/src/main/java/deob/FullscreenMode.java:36) |
| 59 | 44 | 3 | u16(hi, lo+128), u8(v+128) run |
| 46 | 65 | 3 | u16(lo+128, hi), u8 |
| 49 | 62 | 3 | u16le, u8(v+128) run |

The opcode byte is not part of these sizes. Camera movement, raw key packets, and widget transmit packets outside this menu method were not read. See `gaps.md`.
