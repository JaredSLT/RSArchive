# Framing

## ISAAC

`MidiPcmStream` is the generator. `opMidiPcmStream12` returns `valueMidiPcmStream28[valueMidiPcmStream33]` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/MidiPcmStream.java:261`).

`Cache.opCache8` writes one opcode byte as `(opcode + nextResult) & 255` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Cache.java:119`). That is the client-to-server direction.

`Cache.opCache2` reads one opcode byte as `(raw - nextResult) & 255` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Cache.java:83`). That is the server-to-client direction.

`opCache11` installs the seed (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Cache.java:123`). Login uses the four ints in the RSA block for the outbound generator and those ints plus 50 for the inbound generator. See [login.md](login.md).

Bit mode is on the same `Cache`:

| Method | Behavior | Citation |
| --- | --- | --- |
| `opCache3` | bit position = buffer position × 8 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Cache.java:87` |
| `opCache4(n)` | next `n` bits, MSB within the current byte, masked by `WirVermutenIhrSpielkontoWurdeGestohlen.valuekind1551` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/WirVermutenIhrSpielkontoWurdeGestohlen.java`) | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Cache.java:91` |
| `opCache6(packetLen)` | `packetLen * 8 - bitPosition` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Cache.java:111` |
| `opCache7` | byte position = `(bitPosition + 7) / 8` | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Cache.java:115` |

## Server-to-client sizes

There is one size array, `RawModel.valueRawModel23`, copied from `builds/rs2/468-2007-08-08/src/src/main/java/rt4/RawModel.java:43`. Index is the ISAAC opcode. It is read only in `GlFont.opGlFont5` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:433`).

```text
0, 0, -2, 0, -2, 6, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 5, 2, 0, 0, 0, 0, 3, -2, 0, 0, -2, 0, 8, 2, 0, -2, -2, 0, 0, 3, 11, 0, 0, 0, 0, -2, 0, 0, 0, 6, 0, 6, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 3, -2, 0, 6, -1, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, -2, 0, 0, 10, 0, -1, 0, 0, 0, 0, 5, 0, 0, 0, 7, 0, 4, -2, 0, 2, 0, 1, 7, 5, 0, 0, 2, 0, 0, 0, 24, 6, 0, 1, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 2, 7, 0, 0, 0, 6, 0, 0, 0, 0, 4, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 14, 0, 0, 0, 0, -2, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 6, 6, 6, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -2, 0, 0, 0, 0, 0, 0, -2, 6, 0, 0, 0, 0, 4, -1, 8, 0, 0, 0, 0, 6, 0, 0, 4, 0, 0, 0, 0, 0, 0, 4, 0
```

`GlFont.opGlFont5` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:419`):

1. Read one socket byte and decode the opcode with `opCache2`.
2. `size = RawModel.valueRawModel23[opcode]`.
3. If `size == -1`, read one raw socket byte and use it as the length (`& 255`).
4. If `size == -2`, read two socket bytes and decode them with `opMouseRecorder30` (u16be).
5. Wait until the socket has `size` bytes, then read that body.

The value `-1` at `TextureOp4.valueTextureOp420` means "no opcode yet" (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:429`). It is not a packet.

Opcodes in the array with no branch in `opGlFont5` are listed in [gaps.md](gaps.md). This client has no client-to-server size array. Each outbound packet is just the bytes written after `opCache8`.

The first packet inside a successful login is the exception in [login.md](login.md): an ISAAC opcode and a u16be length, without this array.

## Buffer widths

Reads and writes below are `MouseRecorder` unless noted. The position field is `valueMouseRecorder6` and the array is `valueMouseRecorder11`.

| Method | Direction | Width |
| --- | --- | --- |
| `opMouseRecorder31` | read | u8 |
| `opMouseRecorder14` | write | u8 |
| `opMouseRecorder30` | read | u16be |
| `opMouseRecorder13` | write | u16be |
| `opMouseRecorder23` | read | u16le |
| `opMouseRecorder44` | write | u16le |
| `opMouseRecorder37` | read | u32be |
| `opMouseRecorder29` | write | u32be |
| `opMouseRecorder24` | read | u32le |
| `opMouseRecorder25` | write | u32le |
| `opMouseRecorder17` | read | u24be |
| `opMouseRecorder48` | write | u24be |
| `opMouseRecorder26` | read | u64be, two `opMouseRecorder37` |
| `opMouseRecorder39` | write | u64be |
| `opMouseRecorder12` | write | u32le of the high half, then u32le of the low half |
| `opMouseRecorder9` | read | u32 with wire bytes `>>16`, `>>24`, low, `>>8` |
| `opMouseRecorder51` | write | the same u32 order |
| `opMouseRecorder47` | read | u32 with wire bytes `>>8`, low, `>>24`, `>>16` |
| `opMouseRecorder54` | write | the same u32 order |
| `opMouseRecorder18` | read | u16be, low byte stored as `value + 128` |
| `opMouseRecorder46` | write | that u16 |
| `opMouseRecorder19` | read | u16le, low byte stored as `value + 128` |
| `opMouseRecorder4` | write | that u16 |
| `opMouseRecorder27` | read | signed u16be with low byte `+ 128`; values above 32767 subtract 65536 |
| `opMouseRecorder49` | read | signed u16be; values above 32767 subtract 65536 |
| `opMouseRecorder20` | read | u8 stored as `value + 128` |
| `opMouseRecorder6` | write | that u8 |
| `opMouseRecorder28` | read | u8 stored as `-value` |
| `opMouseRecorder32` | write | that u8 |
| `opMouseRecorder34` | read | u8 stored as `128 - value` |
| `opMouseRecorder45` | write | that u8 |
| `opMouseRecorder42` | read | raw signed byte |
| `opMouseRecorder33` | read | signed `-raw` |
| `opMouseRecorder35` | read | signed `raw - 128` |
| `opMouseRecorder11` | read | signed `128 - raw` |
| `opMouseRecorder53` | read | jstr, bytes until 0 |
| `opMouseRecorder21` | write | jstr plus a 0 byte |
| `opMouseRecorder7` | read | a 0 byte means null, otherwise `opMouseRecorder53` |
| `opMouseRecorder55` | read | `n` bytes in order |
| `opMouseRecorder41` | write | `n` bytes in order |
| `opMouseRecorder38` | read | `n` bytes stored from the last index down to 0 |
| `opMouseRecorder5` | read | if the first byte is below 128, that u8; otherwise u16be minus 32768 |
| `opMouseRecorder43` | write | u8 when the value is 0..127, otherwise u16be with bit 15 set |
| `opMouseRecorder16` | read | if the first byte is at least 128, u16be minus 49152; otherwise u8 minus 64 |
| `opMouseRecorder10` | read | 7-bit groups while the signed byte is negative |
| `opMouseRecorder52` | write | those 7-bit groups |
| `opMouseRecorder15` | write | stores `length` at `position - length - 1` |
| `opMouseRecorder50` | write | RSA, then u8 length and the encoded bytes |

Citations for that table are the method bodies in `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MouseRecorder.java:380`.
