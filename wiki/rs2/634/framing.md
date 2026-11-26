# Framing

This client has no `int[256]` size array. Each game packet is a constructor:

- C2S `Cc6a8f147(opcode, size)` stores opcode in `valuekind11422` and size in `valuekind11421` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cc6a8f147.java:42).
- S2C `C454c974c(opcode, size)` stores size in `valuekind13162` and opcode in `valuekind13161` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C454c974c.java:16).

`rslib/src/client/packets/634.c` and `rslib/src/server/packets/634.c` are absent. The sizes below are copied from these constructors. `rslib/src/both/move/rev_634.c` `k_sizes_c2s_634` and `k_sizes_s2c_634` match every constructor size. RSLib fills opcodes that have no constructor with -3. That -3 is not a value stored by this client.

Opcodes 0..84 each have one C2S constructor. Opcodes 0..120 each have one S2C constructor. No constructor uses an opcode outside those ranges.

## S2C dispatch index

`C42b26f21.opkind14182` returns the 121 S2C objects in opcode order (builds/rs2/634-2010-12-10/src/src/main/java/deob/C42b26f21.java:55). For every slot, the array index equals `valuekind13161`. The wire opcode is that index.

## ISAAC

`C351e1949` is a 256-int ISAAC. The mix constant is `-1640531527` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C351e1949.java:56). `opkind13916` consumes the next result. `opkind13913` returns the pending result and leaves the counter in place (builds/rs2/634-2010-12-10/src/src/main/java/deob/C351e1949.java:189, builds/rs2/634-2010-12-10/src/src/main/java/deob/C351e1949.java:226).

C2S opcode byte: `value + isaac.next` in `Cc8b81df5.opkind16725` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cc8b81df5.java:103). The send helper `ClientOptionsLightingDetail.opClientOptionsLightingDetail7` allocates a `Cc8b81df5` from the constructor size and writes that opcode (builds/rs2/634-2010-12-10/src/src/main/java/deob/ClientOptionsLightingDetail.java:17). Size -1 allocates 260 bytes. Size -2 allocates 10000. Any other size at most 18, including 0, allocates 20. Size at most 98 allocates 100. A larger size allocates 260.

S2C opcode: `byte - isaac.next` in `opkind16724` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cc8b81df5.java:90). One byte when the result is below 128. Otherwise a second byte: `((first - 128) << 8) + second`. `opkind16721` peeks the same subtraction with `opkind13913` so the peek does not consume the cipher (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cc8b81df5.java:74).

Outgoing cipher seeds are the four login seeds. Incoming seeds are those values plus 50. See [login.md](login.md).

## In-game variable length

`C4d106383.opkind15924` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C4d106383.java:21), called from `C802d90122.opkind13153`, up to 100 times a tick (builds/rs2/634-2010-12-10/src/src/main/java/deob/Client.java:2098):

1. Read the ISAAC opcode index described above.
2. Reject the index when it is below 0 or at least 121 (`invo:`).
3. `size = packet.valuekind13162`.
4. Size -1: read one `u8` length.
5. Size -2: read `u16be` (`opLoadingScreenRelated214`).
6. Size above 0: read that many payload bytes.
7. Compare the packet object to the static fields and dispatch.

Lobby login state 12 uses the same opcode index, then always a `u16be` length, including when the constructor size is -1 or a fixed number (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:414).

## Bit buffer

`Cc8b81df5.opkind167213` sets the bit cursor to `bytePos * 8` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cc8b81df5.java:163). `opkind16727` reads `n` bits, high bits of the current byte first (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cc8b81df5.java:119). `opkind16729` byte-aligns to `(bitPos + 7) / 8` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cc8b81df5.java:144). `opkind167212` returns `byteLength*8 - bitPos` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cc8b81df5.java:151).

## Integer codecs

Gate constants shown are the values callers in this spec pass, so the body runs.

| Token | Method | Layout | Line |
| --- | --- | --- | --- |
| u8 | 261 read, 254 write | one byte | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:706, builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:631 |
| u8_add128 | 269 write, 267 read | wire byte is value+128 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:784, builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:771 |
| u8_sub128 | 249 write, 224 read | wire byte is 128-value | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:553, builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:266 |
| u8_neg | 213 read | `(-byte) & 255` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:164 |
| i8 | 262 read | raw signed byte | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:714 |
| i8_neg | 242 read | `(-byte)` as a signed byte | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:433 |
| i8_add128 | 227 read | signed `(128 - byte)` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:289 |
| u16be | 214 read, 220 write | high byte then low | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:168, builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:228 |
| u16le | 251 read, 234 write | low byte then high | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:590, builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:367 |
| i16be | 250 read | u16be, subtract 65536 when the value is at least 32768 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:576 |
| u16be_add128 | 218 write, 236 read | high byte plain, low byte is value+128 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:213, builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:376 |
| u16le_add128 | 231 write (only when the first arg is 4325), 239 read | low byte is value+128, then high byte | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:316, builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:404 |
| u32be | 259 read, 265 write | big-endian | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:686, builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:749 |
| u32le | 245 read | little-endian | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:459 |
| u32_225 | 225 read | stream b0 b1 b2 b3 becomes `b2<<24 \| b3<<16 \| b0<<8 \| b1` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:270 |
| u32_228 | 228 read | stream b0 b1 b2 b3 becomes `b1<<24 \| b0<<16 \| b3<<8 \| b2` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:297 |
| u24be | 233 read | three big-endian bytes | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:351 |
| i24be | 248 read | u24be, subtract 16777216 when the value is at least 8388608 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:539 |
| u64be | 266 write | eight big-endian bytes | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:760 |
| u16be length back-patch | 258 | writes the length at `pos - length - 2` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:664 |
| smart | 257 read | if the next byte is below 128, a `u8`; otherwise `u16be - 32768` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:655 |
| cstr | `opUnused666` | bytes until 0, then `C66af5d01.opkind17651` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:621 |
| bytes(n) | `calld61bdcd3` write, 229 read reversed, 263 read forward | raw copy | builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:638, builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:302 |

The cstr decoder `C66af5d01.opkind17651` was not read. The terminator is in `opUnused666`.

## C2S sizes

Opcode, then constructor size. Range 0..84.

```text
0:0 1:-1 2:6 3:-1 4:0 5:3 6:-1 7:6 8:1 9:8 10:-1 11:8 12:3 13:2 14:-1 15:8
16:7 17:8 18:3 19:-1 20:-1 21:2 22:7 23:8 24:7 25:3 26:7 27:7 28:2 29:3 30:4 31:1
32:4 33:8 34:15 35:5 36:7 37:2 38:7 39:8 40:12 41:-1 42:8 43:-1 44:3 45:7 46:2 47:4
48:11 49:6 50:-1 51:3 52:4 53:-1 54:15 55:4 56:7 57:3 58:4 59:8 60:8 61:11 62:3 63:3
64:-1 65:3 66:-1 67:-1 68:2 69:-1 70:0 71:2 72:16 73:-1 74:-1 75:3 76:4 77:-1 78:16 79:3
80:7 81:3 82:18 83:-1 84:-1
```

## S2C sizes

Opcode, then constructor size. The same number is the index in `opkind14182`. Range 0..120. `-1` and `-2` are the rules above.

```text
0:-1 1:2 2:6 3:3 4:1 5:6 6:2 7:3 8:-2 9:-1 10:-2 11:6 12:0 13:-2 14:2 15:16
16:6 17:4 18:0 19:-2 20:-2 21:-1 22:0 23:6 24:6 25:6 26:11 27:3 28:-2 29:7 30:-1 31:-1
32:6 33:3 34:12 35:0 36:-2 37:7 38:-1 39:8 40:-1 41:3 42:-1 43:4 44:6 45:8 46:-1 47:6
48:-2 49:4 50:-1 51:17 52:-1 53:-2 54:4 55:0 56:2 57:10 58:2 59:3 60:0 61:-1 62:4 63:5
64:2 65:-1 66:6 67:3 68:6 69:12 70:-2 71:7 72:0 73:3 74:28 75:6 76:6 77:4 78:-1 79:3
80:0 81:6 82:4 83:2 84:7 85:4 86:6 87:1 88:10 89:0 90:10 91:2 92:-1 93:-2 94:-1 95:10
96:8 97:0 98:-2 99:-1 100:-1 101:20 102:3 103:8 104:1 105:-2 106:8 107:6 108:5 109:7 110:10 111:-1
112:4 113:1 114:4 115:11 116:6 117:1 118:-2 119:6 120:-2
```
