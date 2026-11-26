# Interfaces

Widgets live in js5 index 3. `SoundBank.opSoundBank2` loads group = interface id and one file per component (`builds/rs2/443-2006-12-11/src/src/main/java/deob/SoundBank.java:187`). The stored id is `file + (group << 16)` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/SoundBank.java:207`). A file that starts with byte -1 is decoded by `PacketBit.opPacketBit8`. Every other file is decoded by `PacketBit.opPacketBit6`.

## Old format

`opPacketBit6` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PacketBit.java:258`) reads: u8 type, u8, u16be, i16be, i16be, u16be, u16be, u8, u16be parent, u16be. A parent of 65535 becomes -1 and otherwise keeps the high half of the component id. Then a u8 count of pairs `(u8, u16be)`, then a u8 count of arrays whose length is a u16be and whose entries are u16be, with 65535 stored as -1. Type 0 then reads a u16be and a u8. Later types continue at `PacketBit.java:313` and were not transcribed.

## New format

`opPacketBit8` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PacketBit.java:519`) discards one u8, then reads u8 type, u16be, i16be, i16be, u16be. Type 9 reads the next width as i16be. Every other type reads it as u16be. Then u16be parent, with the same 65535 rule, and a u8 hidden flag. Type 0 reads two u16be. Type 5 reads a u32be, a u16be, and a u8. The remaining type bodies, and `opPacketBit17`, were not transcribed. See [gaps.md](gaps.md).

## Scripts

Client scripts are index 12, group = script id, file 0. The decode is in [cache.md](cache.md) (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PlayerEntity.java:62`).

## Ids the client opens

Startup `NameDesGegenstandsEingeben.opkind1874` closes `NpcType.valueNpcType63`, `HierKlickenUmFortzufahren.valuekind12723`, `Million.valuekind12829`, `Mitteilung.valuekind1868`, `C98f22294.valuekind16625`, `GlModel.valueGlModel19`, and `Keyboard.valueKeyboard14`, and sets the selected tab `NichtHergestelltWerden.valuekind1402` to 3 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/NameDesGegenstandsEingeben.java:221`). That tab field is initialised to 3 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/NichtHergestelltWerden.java:5`). The numeric interface ids in those fields are supplied by the server. S2C opcodes 221, 146, 219, 149, 160, and 97 carry the interface shorts. See [packets.md](packets.md).
