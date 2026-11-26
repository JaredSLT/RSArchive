# Framing

## S2C size table

`Protocol.SERVERPROT_LENGTH` (`builds/rs2/254-2004-09-07/src/src/main/java/jagex2/client/Protocol.java:6`) is the only size table. The initializer has 257 ints. Index 256 is 0. The opcode is masked with 255 (`Client.java:2864`), so index 256 is never used. Values for opcodes 0..255, 16 per row:

```text
  6   0   0   4   0   0   0   0   7   0   0   0   0   0   4   0
  0   0   0   0   0   0   0   0   3   5   0   6  -2   0   4   0
  0   0   0   0   0  15   4   0   0  -2   0   0   0   0   0   0
  0   0   0   0   0   0   0   6   0   0   1   0  -1  -2   0  -2
  6   0   0   0   0   0   4   0   0  -1   0   1   0   0   0   0
  0   0   0   0   0   2   0  -2   2   0   0   3   0   0   1   4
  0   0   7   0   0   0   0   0   0   0   0   0   0   0   0   9
  0   0   6   3   0   0   0   0   5   0   0  -2   0   0   0   6
  0   0   0   0   0   0   0   0   6   0   1   0   0   2   0   2
  0   0  10   0   0   0   0   0   0   0   0   0   0   0   0   2
  0   2   0   2   2   0   0   0   2   0  -2   0   0   2   0   0
  0   0   0   0   0   0   0   0   0   0   3   2   0   0   0   0
  0   0   0   0   6   2   0   0   0   0   0   0  -1   0   0   0
  0   4   0   4   0   3   0   0   0   0  14   0   0   0   6   0
  0   4   0   3   0   0   0   0   0   0   0   0   0   0   0   2
  0   0   4   0   0   0   0   0   0   4   0   0   0   0   0   1
```

`Client.read` (`Client.java:2862`):

1. Read one byte. If `randomIn` is set, `opcode = (byte - isaac.nextInt()) & 255`.
2. `psize = SERVERPROT_LENGTH[opcode]`.
3. If `psize == -1`, read one more u8 and use that as the length.
4. If `psize == -2`, read one u16be and use that as the length.
5. Read exactly `psize` payload bytes.
6. A size of 0 is a legal empty payload. The handler still runs.
7. An opcode with no `if (this.ptype == ...)` branch logs `T1` and calls `logout` (`Client.java:4005`). Opcode 127 is size 6 and has no branch, so those 6 bytes are consumed and then the client logs out. See [gaps.md](gaps.md).

There is no separate C2S size array. `Protocol.CLIENTPROT_LOOKUP` (`Protocol.java:6`) is never read. Do not treat it as opcodes or sizes.

## C2S framing

Game packets are appended to `out` and flushed when `out.pos > 0` at the end of `updateGame` (`Client.java:10935`). The only header is the ISAAC opcode from `Packet.pIsaac` (`Packet.java:125`): the stored byte is `(opcode + isaac.nextInt())` truncated to 8 bits.

A C2S packet has no outer length unless the writer inserts one. `psize1` patches an earlier placeholder with a u8 length of the bytes after that placeholder (`Packet.java:212`). Walk, chat, private message, mouse movement, and two anticheat packets do that. Those lengths are payload, not a second framing layer.

## Endianness

Read from `Packet.java`. Do not infer a width that is not in the method.

| Method | Line | Width |
| --- | --- | --- |
| `p1` / `g1` | 134 / 221 | u8 |
| `g1b` | 225 | i8 |
| `p2` / `g2` | 138 / 229 | u16be |
| `g2b` | 234 | i16be (subtract 65536 if above 32767) |
| `p3` / `g3` | 153 / 244 | u24be |
| `p4` / `g4` | 159 / 249 | 4 bytes, big-endian, stored as a signed Java int |
| `ip4` | 166 | u32le |
| `p8` / `g8` | 178 / 254 | u64be, as two `g4` values |
| `pjstr` / `gjstr` | 194 / 265 | bytes ended by 10 |
| `gsmart` | 351 | if the next byte is below 128, `g1() - 64`; otherwise `g2() - 49152` |
| `gsmarts` | 356 | if the next byte is below 128, `g1()`; otherwise `g2() - 32768` |
| `gBit` | 317 | MSB first within the current byte, then the next byte |
| `bits` / `bytes` | 308 / 342 | bit cursor starts at `pos * 8`; `bytes` advances `pos` to `(bitPos + 7) / 8` |

`ip2` (`Packet.java:143`) writes the low byte and then a literal 0. No login or game packet in this revision calls it.

## ISAAC direction

Outbound: add `Isaac.nextInt()` to the opcode (`Packet.java:127`). Inbound: subtract `Isaac.nextInt()` from the opcode (`Client.java:2866`). `nextInt` (`Isaac.java:34`) returns `rsl[count]` after the usual decrement-and-regenerate step. Seeds are in [login.md](login.md). The inbound generator is the outbound seed with 50 added to each of the four words (`Client.java:4137`).
