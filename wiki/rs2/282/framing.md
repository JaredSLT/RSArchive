# Framing

Citations are under `builds/rs2/282-2004-12-21/src/src/main/java/deob/` unless noted.

## Widths

`Packet` position is `valuekind2715`. Payload bytes are `valuekind2714`.

| method | width |
| --- | --- |
| `opkind273` / `opkind2713` | `u8` |
| `opkind2714` | `i8` (Java signed byte) |
| `opkind274` / `opkind2715` | `u16be` |
| `opkind2716` | `i16be` (subtract 65536 when the unsigned value is above 32767) |
| `opkind276` / `opkind2717` | `u24be` |
| `opkind277` / `opkind2718` | `u32be` stored big-endian; the read is a Java signed int |
| `opkind278` | `u32le` |
| `opkind279` / `opkind2719` | `u64be` (read as two `u32be`) |
| `opkind2710` / `opkind2720` | `jstr`, raw bytes ended by byte 10 |
| `opkind2721` | bytes until byte 10 |
| `opkind2711` / `opkind2722` | raw `bytes(n)` |
| `opkind2712` | backpatch one length byte at `pos - n - 1` |
| `opkind2723` | bit cursor = byte position times 8 |
| `opkind2724` | that many bits, MSB first inside the byte |
| `opkind2725` | byte-align, position = `(bitpos + 7) / 8` |
| `opkind2726` | if the next byte is below 128, `u8 - 64`; otherwise `u16be - 49152` |
| `opkind2727` | if the next byte is below 128, `u8`; otherwise `u16be - 32768` |

`opkind275` writes a `u8` and then a literal 0. It is not used on the traced game-packet path. `opkind278` is little-endian and is not used by the packets in `packets.md`. Where a packet uses one of the methods above, the width is that row. `Packet.java:125`.

## ISAAC

`Packet.opkind272` writes the opcode as `u8 (opcode + isaac.opkind521())` (`Packet.java:125`). `Isaac.opkind521` walks the result array downward and regenerates when the counter hits 0, then sets the counter to 255 (`Isaac.java:35`).

After login, `valueClient281.valuekind2719` is the C2S cipher, seeded with the four login ints. `valueClient213` is the S2C cipher, seeded with those four ints each plus 50 (`Client.java:10360`).

S2C decode is `(byte - isaac.opkind521()) & 255` (`Client.java:6369`). Login bytes are written with `opkind273` and are not ISAAC-encoded.

The outbound buffer is flushed by `ClientStream.write` when `valueClient281.valuekind2715 > 0`, then the position is cleared (`Client.java:11011`).

## S2C

`Client.read` (`Client.java:6355`):

1. Read one opcode byte and subtract the S2C ISAAC value.
2. Size is `C994ec50d.valuekind182[opcode]`.
3. If the size is `-1`, read one more byte as the payload length.
4. If the size is `-2`, read `u16be` as the payload length.
5. Read that many payload bytes into `valueClient306`. `valueClient248` is that length.
6. An opcode with no branch logs `T1 - ` and logs out (`Client.java:7523`). `IOException` reconnects. Any other exception logs `T2` and logs out.

`valuekind182` has 257 ints. Indices 0 through 255 are the opcode sizes. Index 256 is an extra 0 and is not used as an opcode. Copied from `C994ec50d.java:2`:

```text
0 0 0 -2 0 0 0 0 0 0 3 0 0 0 0 4
2 -1 0 0 0 0 0 2 0 0 0 0 6 0 0 0
2 0 5 0 0 0 0 0 0 0 0 0 -2 0 0 0
4 0 0 0 3 0 4 0 0 0 0 0 0 0 0 0
4 0 0 2 0 0 0 0 0 0 0 0 0 6 0 0
0 5 0 1 0 6 0 0 0 2 1 10 0 0 0 7
0 0 0 0 0 0 0 0 0 6 -2 15 0 0 0 0
0 0 3 0 0 0 0 0 0 0 0 0 0 6 0 0
0 4 2 0 0 3 4 0 0 0 4 0 0 0 4 0
0 0 0 0 0 0 0 0 0 2 0 0 6 0 4 0
0 -1 0 0 0 0 2 -2 0 0 0 0 -2 2 0 0
14 0 0 0 0 0 0 4 0 1 0 0 0 0 0 0
2 0 1 -2 0 -2 0 0 6 0 0 3 0 0 0 1
0 0 0 2 0 0 0 3 0 0 0 7 0 0 0 0
0 0 0 2 0 0 0 4 0 6 0 -1 0 0 0 0
2 1 0 0 0 6 0 9 0 0 0 0 0 0 0 0
0
```

Non-zero sizes, index then value: 3:-2, 10:3, 15:4, 16:2, 17:-1, 23:2, 28:6, 32:2, 34:5, 44:-2, 48:4, 52:3, 54:4, 64:4, 67:2, 77:6, 81:5, 83:1, 85:6, 89:2, 90:1, 91:10, 95:7, 105:6, 106:-2, 107:15, 114:3, 125:6, 129:4, 130:2, 133:3, 134:4, 138:4, 142:4, 153:2, 156:6, 158:4, 161:-1, 166:2, 167:-2, 172:-2, 173:2, 176:14, 183:4, 185:1, 192:2, 194:1, 195:-2, 197:-2, 200:6, 203:3, 207:1, 211:2, 215:3, 219:7, 227:2, 231:4, 233:6, 235:-1, 240:2, 241:1, 245:6, 247:9.

`rslib/src/server/packets/282.c` matches indices 0 through 255. Opcode 125 has size 6 and no `Client.read` branch.

A size of 0 with a handler is an empty payload. A size of 0 with no handler still reads zero payload bytes and then takes the `T1` logout path. Those opcodes are listed once in `gaps.md`.

## C2S

This client has no C2S size array. `C994ec50d.valuekind181` is 257 ints, a permutation of 0 through 255 plus a trailing 0, and nothing reads it. It is not a size table.

Each game packet is `opkind272(opcode)` plus the payload writes in `packets.md`. A one-byte variable length is a placeholder `u8 0` followed later by `opkind2712(written - mark)`, which stores that count in the placeholder. That count is the number of bytes after the placeholder. It is not the S2C `-1` or `-2` rule.

Walk opcodes 207, 86, and 138 write the length with `opkind273` directly. The value is the number of bytes after the length byte. See `input.md`.

`rslib/src/both/move/rev_282.c` `k_c2s_282` is not this array. Its walk encoder omits the length byte and the 14-byte minimap tail. `rslib/src/client/packets/282.c` is absent.
