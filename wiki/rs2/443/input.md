# Input

Keys are copied by `Whi.opkind1815` and the mouse is snapshotted by `VertexNormal.opVertexNormal1` at the start of each logic tick (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:434`). The packets below are written from the in-game loop `Cc317a406.opkind1793` and from `MonotonicClock.opMonotonicClock34`.

## Walk

`Keyboard.opKeyboard5` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Keyboard.java:322`) caps the waypoint count at 25, including the destination. Argument `var3` selects the opcode: 0 writes opcode 99, 1 writes opcode 80, 2 writes opcode 81. Each writes a u8 length of `3 + 2 * n`, and opcode 80 adds 14 to that length. The shared tail is:

1. u16leA of destination scene X plus base X (`SpielFensterGeladen.valuekind14714`).
2. u8neg of 1 when `MidiPcmStream.valueMidiPcmStream4[82]` is set, otherwise 0. A held run key is the byte `0xFF`.
3. u16le of destination scene Z plus base Z (`HardReferenceNode.valueHardReferenceNode3`).
4. Each earlier waypoint, walking backward from the destination: u8 of scene X minus destination X, then u8s of scene Z minus destination Z.

Scene X is `SceneGraph_Class120.valueSceneGraph_Class12010`. Scene Z is `PacketBit.valuePacketBit119`.

## Minimap

`Cb5815139.opkind1433` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cb5815139.java:349`) ignores the click when `C27064abc.valuekind1831` is not 0. S2C opcode 87 stores that flag (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:651`). A click inside the 146 by 151 area, with offsets 73 and 75, calls `opKeyboard5` with `var3` 1. When that returns true, the same buffer receives the 14 bytes the length already reserved: u8 click X offset, u8 click Y offset, u16be `Cc317a406.valuekind1791`, u8 57, u8 `Cd2890cde.valuekind1942`, u8 `OverlayMultiway.valuekind18816`, u8 89, u16be player fine X, u16be player fine Z, u8 `EmpfNger.valuekind1388`, u8 63 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cb5815139.java:368`).

`rslib/src/both/move/rev_443.c` says the minimap caller adds 14 bytes its encoder does not write, and its C2S size for opcode 81 is 8. This client writes the 14 bytes, and opcode 81 uses the variable walk layout above. See [gaps.md](gaps.md).

## Mouse and camera

Opcode 133 is the mouse recorder (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:194`). It is sent only when `ServerActiveProperties.valueServerActiveProperties21` is set, and only when the button is down or at least 40 samples are queued. The sample layout is in [packets.md](packets.md).

Opcode 162 is one click (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:291`). Opcode 66 is u16beA of `SoundBank.valueSoundBank14` and u16be of `Cc317a406.valuekind1791`, written when keys 96 through 99 were down (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:306`). Opcode 207 is a single u8, 1 when `SoftwareFont.valueSoftwareFont6` becomes set and 0 when it clears (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:313`). Opcode 192 has no payload and is written when both idle counters pass 4500 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:548`).

Opcode 86 has no payload. `Js5CacheRequest.opJs5CacheRequest5` writes it when the counter reaches 50, or when its flag is set, and flushes immediately (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Js5CacheRequest.java:139`). The in-game loop also writes it, without flushing by itself, when the same counter passes 50 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:623`).

## Chat and window

Opcode 4 is public chat: a placeholder u8, the chat type, the effect byte, then `MidiNote.opMidiNote3`, then a backpatched length (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:426`). Opcode 50 is a private message: a placeholder u8, a u64be recipient, the same encoder, then the backpatch (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:483`). Opcode 174 is the `::` branch of the same key handler: u8 of length minus 1 and a jstr of the text from index 2 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:357`). Opcode 22 is a u64be of the entered name (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:304`). Opcode 74 is a u32be (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:284`). Opcode 90 is a u64be friend name (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MidiNote.java:40`).

Opcode 76 is three u8 values: `PathingEntity.valuePathingEntity71`, `ReferenceNode.valueReferenceNode2`, and `Component.valueComponent182` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:436`). `TextureOp38.opTextureOp382` writes the same three bytes (`builds/rs2/443-2006-12-11/src/src/main/java/deob/TextureOp38.java:35`). Opcode 65 is one u8 of `NichtHergestelltWerden.valuekind1402` when a tab closes (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:673`).

## Widget ops

`Cc317a406.opkind1791` writes opcodes 13, 217, 161, 3, 40, 73, 57, 167, 28, and 8. Each is a u32be and a u16be (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cc317a406.java:29`). `LadeFreundeListe.opkind1235` writes the remaining widget opcodes. Those payloads were not read. See [gaps.md](gaps.md).
