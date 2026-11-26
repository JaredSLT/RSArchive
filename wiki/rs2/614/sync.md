# Sync

Player packet is S2C opcode 64, size -2, `KbaFinalize.valueKbaFinalize6`. The switch calls `KeaInit.opKeaInit2` (builds/rs2/614-2010-07-13/src/src/main/java/deob/KeaInit.java:17). That calls `RendererToolkit.callf97538dc` and then `Js5ArchiveResourceLoader.opJs5ArchiveResourceLoader3`, and throws `gpi1` if the cursor does not equal the payload length.

## Player presence

`RendererToolkit.callf97538dc` (builds/rs2/614-2010-07-13/src/src/main/java/deob/RendererToolkit.java:9) runs four bit-packed passes. Each pass starts with `opParticleEmitterRelated9` and ends with `opParticleEmitterRelated6`. Slots are 1 through 2047 (builds/rs2/614-2010-07-13/src/src/main/java/deob/RendererToolkit.java:110). There is no 32767 terminator. A skip run covers the slots the server does not update.

For each slot in the pass, if a skip budget remains the client marks bit 1 and decrements the budget. Otherwise it reads 1 bit. 0 starts a skip via `C0e0d8b98.opkind11082` and marks bit 1. 1 calls `NativeLibraryLoader.opNativeLibraryLoader3` for an active slot, or `AudioChannel.opAudioChannel21` for an inactive slot.

The passes are:

1. Active slots whose activity bit 0 is clear (`WorldMapRelated1.valueWorldMapRelated16`, count `C0917b442.valuekind11073`).
2. Active slots whose activity bit 0 is set.
3. Inactive slots whose activity bit 0 is set (`ObjReference.valueObjReference5`, count `Cc61b8c1a.valuekind10933`).
4. Inactive slots whose activity bit 0 is clear.

`C0e0d8b98.opkind11082` (builds/rs2/614-2010-07-13/src/src/main/java/deob/C0e0d8b98.java:18), guard 11: 2 bits, then 0 extra bits when the value is 0, 5 bits when it is 1, 8 bits when it is 2, and 11 bits when it is 3.

After the four passes every slot's activity byte is shifted right by 1. Slots with `StructTypeList.valueStructTypeList6[slot] != null` are active. The others are inactive.

## Player movement

`NativeLibraryLoader.opNativeLibraryLoader3` (builds/rs2/614-2010-07-13/src/src/main/java/deob/NativeLibraryLoader.java:98). One bit selects a mask update and appends the slot to `PreciseSleep.valuePreciseSleep3`. Then 2 bits select the step.

| type | bits | meaning |
| --- | --- | --- |
| 0 | none | no step. The local player with no mask throws `s:lr`. Another player is removed and one further bit may call `opAudioChannel21` |
| 1 | 3 | one-tile step |
| 2 | 4 | two-tile step |
| 3 | 1 | 0 = 12-bit relative teleport, 1 = 30-bit absolute teleport |

One-tile directions, x = `valueLoadableResourceType86`, z = `valueLoadableResourceType80`: 0 (--x, --z), 1 (--z), 2 (++x, --z), 3 (--x), 4 (++x), 5 (--x, ++z), 6 (++z), 7 (++x, ++z).

Two-tile directions use the same compass in steps of 2: 0 (x-2, z-2), 1 (x-1, z-2), 2 (z-2), 3 (x+1, z-2), 4 (x+2, z-2), 5 (x-2, z-1), 6 (x+2, z-1), 7 (x-2), 8 (x+2), 9 (x-2, z+1), 10 (x+2, z+1), 11 (x-2, z+2), 12 (x-1, z+2), 13 (z+2), 14 (x+1, z+2), 15 (x+2, z+2).

Relative teleport is 12 bits: plane in the top 2 bits added to the current plane, then dx 5 bits and dz 5 bits, each subtracted by 32 when greater than 15. Absolute teleport is 30 bits: plane `>> 28`, x `>> 14 & 16383`, z `& 16383`, then wrapped against the scene base with 16383. The local player's plane is copied to `VarBitTypeList.valueVarBitTypeList2`.

## Adding a player

`AudioChannel.opAudioChannel21` (builds/rs2/614-2010-07-13/src/src/main/java/deob/AudioChannel.java:20) reads 2 bits.

| type | next bits | effect |
| --- | --- | --- |
| 0 | 1 optional recurse bit, 6-bit x, 6-bit z, 1 mask bit | create the player from the external record. An existing player throws `hr:lr`. Returns true |
| 1 | 2-bit plane delta | updates the external packed coordinate. Returns false |
| 2 | 5 bits = 3-bit direction plus 2-bit plane | updates the external packed coordinate. Returns false |
| 3 | 18 bits = plane byte, dx byte, dz byte | updates the external packed coordinate. Returns false |

## Player masks

`Js5ArchiveResourceLoader.opJs5ArchiveResourceLoader3` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5ArchiveResourceLoader.java:11) reads one u8 per masked slot. Bit 32 reads another u8 shifted left 8. Bit 32768 (`'耀'`) reads another u8 shifted left 16. `TextureOp14.opTextureOp1427` (builds/rs2/614-2010-07-13/src/src/main/java/deob/TextureOp14.java:7) then tests bits in this order:

| bit | fields |
| --- | --- |
| 64 | u8neg length, then that many bytes reversed and each subtracted by 128 (`opUnused6148`). The side buffer is the appearance block below |
| 2 | u16be (65535 = -1), u32_cdab (`call1cda1d8d`), u8(128-b) |
| 4 | smart (`opUnused6151`), u8neg, u8neg |
| 1024 | s8(128-b). Applied after the step when the player was flagged to move. 127 is a separate case |
| 8 | u16be (65535 = -1), u8(128-b) |
| 16384 | jstr. A first character of 126 drops that character |
| 256 | u16le (65535 = -1), u32le (`call89703553`), u8 |
| 4096 | u16le, u8(b-128), u8neg. Low 15 bits of the u16le are the id. Bit 32768 of that u16le is a flag |
| 2048 | s8, s8, s8(b-128), s8(128-b), u16(lo-128, hi), u16(lo-128, hi), u8(128-b) |
| 512 | u8(b-128) count, then count times (u16(lo-128, hi) with 65535 = -1, u8(128-b), u16be) |
| 128 | u16le (65535 = -1) |
| 16 | u16(lo-128, hi). Applied immediately when the player is not mid-step |
| 131072 | smart, u8neg |
| 8192 | u8. The stored flag is that byte == 1 |
| 65536 | s8neg, s8, s8, u8(128-b), u16be(lo-128), u16be(lo-128) |
| 1 | s8 |

`u32_cdab` is `call1cda1d8d` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:128). Bytes in buffer order are A, B, C, D and the value is `(C<<24) + (D<<16) + (A<<8) + B`.

## Appearance

`C0dc71316.opkind105817` (builds/rs2/614-2010-07-13/src/src/main/java/deob/C0dc71316.java:463) resets the side buffer and reads:

`u8` flags, three `s8`, then up to 12 slots. A slot u8 of 0 is empty. Otherwise a second u8 forms a 16-bit value. Slot 0 equal to 65535 is an npc transform: `u16be` id, `u8` extra, and the slot loop stops. A value at least 32768 is an object. A smaller value is a kit.

Then five colour `u8`, `u16be`, `jstr` display name, `u8`. If flag bit 2 is set: `u16be` (65535 = -1). Otherwise two `u8`. Then `u8`. If that last byte is nonzero: five `u16be` and one `u8`.

Flag bit 0 is a gender bit. Bits 3 through 5, masked with 59 before the shift, change the size. Bits 6 and 7 are stored separately.

## Local player on login

This block is not opcode 64. `NPCTypeCustomisation.opNPCTypeCustomisation2` (builds/rs2/614-2010-07-13/src/src/main/java/deob/NPCTypeCustomisation.java:29) bit-aligns, reads 30 bits (plane `>> 28`, x `>> 14 & 16383`, z `& 16383`) placed relative to the scene base, then for every other slot 1..2047 reads 18 bits (plane `>> 16`, x byte, z byte) into an external record, then byte-aligns.

## NPCs

S2C opcode 42, size -2, calls `C67590275.call59622295(true, false)` (builds/rs2/614-2010-07-13/src/src/main/java/deob/C67590275.java:8). A second packet calls `call59622295(false, false)` (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:345). The boolean is the coordinate width for new NPCs. Both throw `gnp1` or `gnp2` when the cursor or the npc table disagrees.

Existing NPCs, `KbaFinalize.opKbaFinalize2` (builds/rs2/614-2010-07-13/src/src/main/java/deob/KbaFinalize.java:11): bit-align, 8-bit count. The count must be at most the previous count or the client throws `gnpov1`. For each index: 1 bit. 0 keeps the npc with no mask. 1 reads 2 more bits:

| subtype | bits | effect |
| --- | --- | --- |
| 0 | none | keep, queue a mask |
| 1 | 3-bit walk, then 1 mask bit | keep |
| 2 | 1 bit, then either two 3-bit directions or one 3-bit direction, then 1 mask bit | keep |
| 3 | none | remove |

New NPCs, `Language.opLanguage2` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Language.java:14): while at least 16 bits remain, read 15 bits. 32767 ends the list and is the only terminator. Otherwise the client creates the npc. When the boolean is true, dx and dz are 8-bit signed (subtract 256 above 127). When it is false they are 5-bit signed (subtract 32 above 15). Then 3 bits, 2 bits, 1 mask bit, 1 bit, and a 14-bit type id. Positions are relative to the local player.

NPC masks, `C5904e851.opkind10904` (builds/rs2/614-2010-07-13/src/src/main/java/deob/C5904e851.java:22): one u8, and if bit 64 is set another u8 shifted left 8. There is no third mask byte. Wire order:

| bit | fields |
| --- | --- |
| 8 | smart, u8, u8(b-128) |
| 512 | u16be, u8(b-128), u8. Bit 32768 of the u16be is a flag. The low 15 bits are kept |
| 2 | u16le (65535 = -1), u8neg |
| 1 | u16(lo-128, hi) (65535 = -1) |
| 8192 | smart, u8neg |
| 32 | u16be(lo-128), u16be(lo-128) |
| 2048 | s8, s8(128-b), s8neg, u8(b-128), u16(lo-128, hi), u16be(lo-128) |
| 4 | u16be(lo-128) type id |
| 16 | jstr |
| 128 | u16be(lo-128) (65535 = -1), u32_badc (`opUnused6127`), u8(128-b) |
| 4096 | u8 count, then count times (u16be(lo-128) with 65535 = -1, u8neg, u16(lo-128, hi)) |
| 1024 | s8(128-b), s8neg, s8neg, s8neg, u16(lo-128, hi), u16be(lo-128), u8(128-b) |
| 256 | u16le (65535 = -1), u32_badc, u8(128-b) |

`opUnused6127` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:21) reads bytes A, B, C, D and returns `(B<<24) + (A<<16) + (D<<8) + C`. That layout is `u32_badc`.
