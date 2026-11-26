# Framing

Logical client opcodes are the integers passed to `Packet.p1isaac` before ISAAC is added. The wire byte is `(opcode + random.nextInt())` as a Java byte. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/io/Packet.java:129`.

Server opcodes are one raw byte, then `(raw - randomIn.nextInt()) & 255` when `randomIn` is set. The size is `Protocol.serverProtSizes[opcode]`. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:11993`.

Outbound and inbound generators are separate. Login seeds the outbound generator with the four ints and the inbound generator with those ints plus 50 each. See [login.md](login.md).

## Variable length

`serverProtSizes` value `-1` means the payload length is the next `u8`. Value `-2` means the payload length is the next `u16be` (`g2`). The client then reads that many payload bytes. `Client.java:12004`, `Client.java:12014`.

There is no client-to-server size array. Packet rows in [packets.md](packets.md) use size `none` for client writes. A variable client packet writes a placeholder `u8` and later `psize1`, which stores the following byte count in the byte immediately before those bytes. `Packet.java:216`.

`Protocol.clientProtScrambled` is a 257-int table. Nothing in this client reads it. It is not a size table. `Protocol.java:6`.

`serverProtSizes` has 257 ints. Index 256 is a trailing 0. The opcode is masked to 8 bits, so index 256 is unused. The table below is indexes 0..255, copied from `Protocol.java:7`.

```text
0, 0, 0, 0, 6, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0
0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
0, 0, -2, 4, 3, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 6
0, 0, 9, 0, 0, -2, 0, 0, 0, 0, 0, 0, -2, 1, 0, 0
2, -2, 0, 0, 0, 0, 6, 3, 2, 4, 2, 4, 0, 0, 0, 4
0, -2, 0, 0, 7, 2, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0
0, 2, 0, 1, 0, 2, 0, 0, -1, 4, 1, 0, 0, 0, 1, 0
0, 0, 2, 0, 0, 15, 0, 0, 0, 4, 4, 0, 0, 0, -2, 0
0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 2, 0
0, 0, 0, 14, 0, 0, 0, 4, 0, 0, 0, 0, 3, 0, 0, 0
4, 0, 0, 0, 2, 0, 6, 0, 0, 0, 0, 3, 0, 0, 5, 0
10, 6, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0
0, 0, 0, 0, -1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 3, 0
2, 0, 0, 0, 0, 0, -2, 7, 0, 0, 2, 0, 0, 1, 0, 0
0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0
2, -2, 0, 0, 0, 0, 6, 0, 4, 3, 0, 0, 0, -1, 6, 0
```

Any opcode with no branch in `Client.read` is still framed by this table. The payload is then logged as `T1` and `logout()` runs. The only nonzero size in 0..255 with no field reads is opcode 47, size 6. `Client.java:13254`.

## Widths

| Name | Method | Layout |
| --- | --- | --- |
| u8 | `p1` / `g1` | one byte |
| i8 | `g1b` | one signed byte |
| u16be | `p2` / `g2` | high byte, then low byte |
| i16be | `g2b` | `u16be`, then subtract 65536 when the value is above 32767 |
| u24be | `p3` / `g3` | three big-endian bytes |
| i32be | `p4` / `g4` | four big-endian bytes, returned as a Java int |
| u64be | `p8` / `g8` | two `i32be` halves, high half first |
| jstr | `pjstr` / `gjstr` | raw bytes, terminator 10 |
| u8_neg | `p1_alt2` / `g1_alt2` | wire byte is `-value` |
| u8_sub | `p1_alt3` / `g1_alt3` | wire byte is `128 - value`. `p1_alt3` ignores its first argument |
| u8_add | `g1_alt1` | wire byte minus 128 |
| i8_neg | `g1b_alt2` | signed `-byte`. The method returns only when `_flowObfuscator10` is -57, which is its initializer |
| i8_sub | `g1b_alt3` | signed `128 - byte` |
| u16le | `p2_alt1` / `g2_alt1` | low byte, then high byte. `g2_alt1` returns 3 only when `_flowObfuscator11` is not 108; the field is initialized to 108 |
| u16be_add | `p2_alt2` / `g2_alt2` | high byte, then low byte plus 128 |
| u16le_add | `p2_alt3` / `g2_alt3` | low byte plus 128, then high byte |
| i16le | `g2b_alt1` | little-endian, subtract 65536 above 32767 |
| i16le_add | `g2b_alt3` | `(second << 8) + (first - 128)`, then the same sign adjust |
| u32_alt2 | `g4_alt2` | bytes `b0 b1 b2 b3` become `(b2 << 24) + (b3 << 16) + (b0 << 8) + b1` |
| u32_alt3 | `g4_alt3` | bytes `b0 b1 b2 b3` become `(b1 << 24) + (b0 << 16) + (b3 << 8) + b2` |
| gsmart | `gsmart` | if the first byte is below 128, `u8 - 64`; otherwise `u16be` minus the char U+C000 (49152) |
| gsmarts | `gsmarts` | if the first byte is below 128, `u8`; otherwise `u16be` minus the char U+8000 (32768) |
| bytes(n) | `pdata` / `gdata` | `n` bytes in order |
| bytes_rev(n) | `gdata_alt1` | `n` bytes copied into the destination from the end |
| bytes_rev_add(n) | `pdata_alt2` | `n` source bytes written from the end, each plus 128 |

`ip2` writes the low byte and then a literal 0. It is not a little-endian `u16`. `Packet.java:147`. `ip4` is little-endian 4 bytes. `Packet.java:170`.

Bit reads start at `accessBits`, which sets `bitPos = pos * 8`. `gBit` takes the high bits of each byte first. `accessBytes` sets `pos` to `(bitPos + 7) / 8`. `Packet.java:315`, `Packet.java:329`, `Packet.java:354`.

When `out.pos > 0`, `updateGame` writes the buffer and clears `heartbeatTimer`. If `heartbeatTimer` passes 50, opcode 0 is written with no payload first. `Client.java:4197`.
