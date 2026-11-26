# Framing

This client has no opcode size table and no ISAAC. `Stream` never seeds or applies a cipher. Every opcode uses one length frame. In [packets.md](packets.md) the size column is `frame`, which means the length comes from this rule and is not a constant.

## Byte order

Widths below are the methods that were read.

| Token | Method | Layout |
| --- | --- | --- |
| `u8` | `Stream.p1`, `Tools.call174` | one byte, masked with 255 on read |
| `i8` | a raw `byte` added or stored with no mask | Java sign-extends the byte |
| `u16be` | `Stream.p2`, `Stream.call145`, `Tools.g2` | high byte, then low byte |
| `i16be` | `Tools.g2s` | `u16be`, then subtract 65536 when the value is greater than 32767 |
| `u32be` | `Stream.p4`, `Stream.call146`, `Tools.g4` | four bytes, highest first. `call146` is two `u16be` values |
| `u64be` | `Stream.p8`, `Tools.g8` | high `u32be`, then low `u32be` |
| `rawstr` | `Stream.pjstr` | `getBytes` copied with no trailing 0. The length is a preceding `u8` or the rest of the frame |
| `gsmart` | `Tools.gsmart` | see below |
| `bits(n)` | `Tools.gBit` | MSB first inside the byte. The method does not advance the caller's cursor; the caller adds `n` |
| `packed` | `Tools.call3cc33792` out, `Tools.call85aa3cc8` in | dictionary chat bytes. The frame or a preceding length holds the count |
| `rsa` | `Stream.rsaenc` | one or more length-prefixed RSA blocks |

`Tools.gBit` uses `Stream.BITMASK`, whose entry `n` is the low `n` bits. The bit cursor for opcode 255 and opcode 248 starts at 8, which skips the opcode byte at the front of the frame buffer.

`gsmart` (`Tools.java:99-101`): if the first byte masked with 255 is below 128, the value is that byte and the caller advances 1. Otherwise the value is `((b0 & 255) - 128) << 24`, plus the next three bytes as a big-endian 24-bit tail, and the caller advances 4 when the numeric result is at least 128. A 4-byte form whose numeric result is below 128 advances only 1, because that is the test the callers write.

`Stream.p1` stores `(byte) value`, so a negative step delta is the low 8 bits.

## C2S finish

`Stream.call6c7d81d6` (`Stream.java:235-253`) writes the opcode at `data[number640 + 2]`, writes 0 at the next byte, sets `pos` to `number640 + 3`, and sets `packetAlignment` to 8. If `number640` is past four fifths of `maxPacketLength` (default 5000), it flushes the queued bytes first.

`Stream.call60c8c054` (`Stream.java:256-279`): if `packetAlignment` is not 8, it increments `pos` once. The payload length is `pos - number640 - 2`, which includes the opcode byte and the zero byte written at `number640 + 3`.

- Length at least 160: `data[number640] = 160 + length / 256`, `data[number640 + 1] = length & 255`.
- Length below 160: `data[number640] = length`, then the last payload byte is moved into `data[number640 + 1]` and `pos` shrinks by 1.

`number640` becomes `pos`. `flush` finishes the frame and calls `writePacket(0)`. The keepalive finishes with `call60c8c054` and then `writePacket(20)`, which writes the queued bytes without opening a new opcode.

No C2S packet in this client sets `packetAlignment` away from 8, so the bit-align increment does not run for the writers that were read.

## S2C read

`Stream.readPacket` (`Stream.java:116-154`):

1. If `number639` is positive and `number638` exceeds it, the stream sets `ioError` and returns 0. Login sets `number639` to `Client.number687`, which `MudClient.load` sets to 1000. Each `readPacket` increments `number638`, and a successful read clears it. On the timeout path `number639` is doubled.
2. When `number637` is 0 and at least 2 bytes are available, the first byte is the length. If that byte is at least 160, the length becomes `(byte - 160) * 256` plus the next byte.
3. When `number637` bytes are available: if the length is at least 160, those bytes are copied in order. If the length is below 160, the last byte is read into `buffer[length - 1]` and the preceding `length - 1` bytes are copied to the front. The returned length includes the opcode byte. The caller uses `buffer[0] & 255` as the opcode.

`Client.callb6b49066` reads at most one packet per call, and only when `call216` is true. `Client.call216` returns true and `MudClient` does not override it.

## Socket writer

`ClientStream.write` copies into a 5000-byte ring. The ring throws `IOException("buffer overflow")` when the write cursor meets the read cursor minus 100 bytes, modulo 5000. The first write starts a daemon thread at priority 4. `ClientStream.run` waits while the ring is empty, writes the contiguous span to the socket, and flushes when the ring is empty again. `close` sets `flag673`, closes the socket, and wakes the thread.

## Dispatch

`Client.call7e894e6a` handles opcodes 8, 9, 10, 23, 24, 26, 27, and 28. Every other opcode goes to `size`, which `MudClient.call5fac7c41` (`mudclient.n`) implements. An opcode with no branch there returns without reading fields.

The same numeric opcode is used in both directions. Direction is the `packets.md` column. Examples that both exist: 202, 203, 204, 205, 206, 207, 208, 211, 212, 213, 214, 216, 217, 218, 219, 220, 221, 222, 226, 227, 228, 229, 230, 232, 233, 234, 235, 236, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254.
