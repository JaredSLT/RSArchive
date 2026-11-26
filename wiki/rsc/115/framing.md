# Framing

This client has no `int[256]` opcode size table and no ISAAC cipher. Every game packet is variable length. The `size` column in `packets.md` is `variable` for that reason. `-1` and `-2` are not values in this client.

`Stream.maxPacketLength` is 5000 (`jagex/Stream.java:10`). Multi-byte integers are big-endian:

| write | read | width | citation |
| --- | --- | --- | --- |
| `p1` | one byte | u8 | `Stream.java:152` |
| `p2` | `Tools.g2` | u16be | `Stream.java:157`, `Tools.java:86` |
| `p4` | `Tools.g4` | u32be | `Stream.java:169`, `Tools.java:91` |
| `p8` | `Tools.g8` | u64be, high `p4` then low `p4` | `Stream.java:177`, `Tools.java:96` |
| | `Tools.g2s` | i16be, subtract 65536 when the u16be is greater than 32767 | `Tools.java:101` |
| `pjstr` | | raw `getBytes` into the buffer, no terminator byte | `Stream.java:183` |

`Tools.gsmart` (`Tools.java:111`): if `(data[offset] & 255) < 128`, the result is the signed Java byte at that offset (0..127). Otherwise the result is `((data[offset] & 255) - 128) << 24`, plus the next three bytes as a big-endian 24-bit value. Subtraction happens before the shift. Callers advance 1 byte when the decoded integer is below 128 and 4 bytes otherwise (`MudClient.java:3134`). The advance uses the decoded integer. A first byte of 128 with a small continuation can decode below 128 and then advance one byte.

`Tools.gBit(data, bitOffset, n)` (`Tools.java:116`) reads `n` bits MSB-first inside each byte. The byte index is `bitOffset >> 3`. `Stream.BITMASK` matches the masks used there (`Stream.java:348`).

## Writer

`call6c7d81d6(opcode)` (`Stream.java:249`): if `number594` is past `maxPacketLength * 4 / 5`, it calls `writePacket(0)` first. It stores the opcode at `data[number594 + 2]`, sets `pos = number594 + 3`, and sets `packetAlignment = 8`.

`call60c8c054` (`Stream.java:270`): if `packetAlignment != 8`, `pos` increases by 1. No bit writer in the methods read leaves alignment at anything other than 8, so that pad is unused by the packets in `packets.md`. Payload length `var1 = pos - number594 - 2` includes the opcode byte.

- `var1 >= 160`: `data[number594] = 160 + var1 / 256`, `data[number594 + 1] = var1 & 255`. The payload stays where it was written, starting at the opcode.
- `var1 < 160`: `data[number594] = var1`, `pos` decreases by 1, and `data[number594 + 1]` becomes the byte that was at the old `pos`. The short header is one length byte plus the last payload byte moved into header byte 1.

`number594` becomes `pos`. `flush` calls `call60c8c054` then `writePacket(0)` (`Stream.java:296`). `writePacket(n)` sends when `number577 >= n` and `number594 > 0`, then sets `pos = 3` (`Stream.java:302`). `writePacket(0)` sends immediately when anything is queued. The idle path calls `call60c8c054` and later `writePacket(20)` (`Client.java:311`).

## Reader

`readPacket` (`Stream.java:111`) waits until `available >= 2`, reads one length byte, and if that byte is `>= 160` reads a second byte: length `(first - 160) * 256 + second`. When `available >= length`:

- length `>= 160`: read `length` bytes straight into the buffer
- length `< 160`: put the next socket byte at `buf[length - 1]`, then read `length - 1` bytes at `buf[0]`

The returned length includes the opcode. After reassembly the opcode is `buf[0]`. Login session ids and login response bytes bypass this reader. See `login.md`.

`Client.callb6b49066` (`Client.java:303`) sends opcode 5 with an empty payload when `hasPacket()` is false and 5000 ms have passed, then `writePacket(20)`, then `readPacket`. A positive length dispatches `in[0] & 255`.

## RSA block used by questions

`Stream.call146` (`Stream.java:197`) writes one 15-byte plaintext: random byte 1..127, two random bytes, session id u32be at offset 3, hash u64be at offset 7 (`Tools.call165`, `Tools.java:69`), then `u8` length and `modPow(...).toByteArray()`.
