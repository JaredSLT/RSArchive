# Framing

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`.

## S2C size table

`Client.read` loads the payload length from `Protocol.valueProtocol2[opcode]` (`jagex2/client/Client.java:2440`). The array is copied from `jagex2/io/Protocol.java:7`:

```text
public static final int[] valueProtocol2 = new int[]{0, 0, 4, 6, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 6, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 0, 6, 2, 0, 0, -2, 0, 0, 0, 0, 0, 6, 0, 0, 0, -1, 0, 0, 0, 4, 0, 0, 0, -2, 0, 0, 0, 2, 23, 0, 9, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 0, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 5, 0, 2, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 7, 0, 0, 0, 1, 3, 0, 4, 0, 0, 0, 0, 0, -2, -1, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, -1, 2, 2, 0, 0, 4, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 15, 3, -2, 0, 0, 8, 6, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 6, 4, 3, 0, 14, 0, 0, -2, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 2, 2, 0, 4, 0, 0, 0, -2, 0, 0, 0, 0, 0, -2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 5, 0, 1, 1, 4, 0, 2, 0};
```

The array has 257 ints, indexes 0 through 256. A frame opcode is one byte, so index 256 is never selected. `-1` means the next byte is an unsigned length (`Client.java:2444`). `-2` means the next `u16be` (`g2`) is the length (`Client.java:2454`). The client then reads that many payload bytes (`Client.java:2470`).

`Protocol.valueProtocol1` (`Protocol.java:6`) is a 256-int permutation. No method in this client reads it. It is not a size table.

```text
public static final int[] valueProtocol1 = new int[]{98, 169, 38, 190, 164, 57, 241, 179, 104, 56, 186, 107, 118, 102, 13, 5, 55, 235, 18, 31, 236, 23, 75, 220, 161, 127, 2, 134, 255, 225, 9, 148, 130, 113, 198, 212, 168, 116, 28, 114, 6, 173, 103, 132, 154, 144, 40, 76, 92, 254, 122, 46, 87, 187, 83, 124, 253, 109, 95, 196, 175, 208, 215, 34, 199, 50, 152, 101, 151, 67, 59, 82, 48, 97, 136, 203, 17, 80, 36, 200, 37, 99, 219, 88, 224, 207, 24, 139, 230, 93, 195, 192, 39, 119, 11, 60, 214, 12, 165, 7, 81, 239, 216, 147, 108, 43, 182, 110, 96, 27, 202, 16, 100, 170, 3, 15, 143, 188, 1, 204, 252, 125, 211, 245, 153, 47, 64, 178, 174, 137, 68, 240, 86, 210, 221, 8, 123, 228, 94, 85, 35, 251, 49, 149, 79, 234, 105, 65, 58, 171, 185, 21, 129, 226, 189, 206, 138, 62, 194, 117, 246, 162, 91, 248, 167, 61, 237, 156, 71, 26, 51, 32, 66, 73, 172, 0, 249, 191, 150, 233, 157, 120, 54, 159, 243, 14, 180, 33, 106, 90, 177, 20, 176, 133, 142, 209, 19, 72, 10, 238, 41, 155, 30, 160, 115, 146, 205, 42, 111, 217, 128, 89, 181, 244, 223, 131, 69, 247, 231, 184, 213, 227, 63, 166, 4, 135, 201, 250, 163, 78, 84, 193, 183, 141, 197, 229, 53, 45, 52, 222, 77, 121, 44, 29, 70, 140, 25, 242, 74, 218, 158, 232, 126, 112, 22, 145, 0};
```

An opcode with no `if (valueClient1002 == …)` branch logs `T1` and calls `logout` (`Client.java:3841`). The only nonzero table entry with no branch is opcode 187, size 6. Opcode 0 is size 0 and also has no branch, so it takes the same logout path.

## C2S size table

This client has no C2S size array. Outbound frames are `p1isaac(opcode)` plus the writes in that call. Do not copy `rslib/src/client/packets/377.c`. Variable writes use an explicit `u8` length or `psize1`, which stores the length at `data[pos - len - 1]` so the length byte is not included in the count (`jagex2/io/Packet.java:218`).

## ISAAC

`p1isaac` adds `random.nextInt()` to the opcode byte (`Packet.java:131`). The S2C opcode byte subtracts `valueClient1031.nextInt()` and masks with 255 (`Client.java:2437`). Seeds are the four login ints. The incoming cipher uses each seed plus 50 (`Client.java:7688`). `nextInt` consumes `valueIsaac9` downward and calls `isaac()` after 256 values (`jagex2/io/Isaac.java:36`). The mix is `Isaac.isaac` (`Isaac.java:45`).

## Widths

Reads and writes below are the methods in `Packet.java`.

| method | width |
| --- | --- |
| `g1` / `p1` | `u8` |
| `g1b` | `i8` |
| `g2` / `p2` | `u16be` |
| `g2b` | `i16be` |
| `g3` / `p3` | `u24be` |
| `g4` / `p4` | `u32be` |
| `ip4` | `u32le` |
| `g8` / `p8` | `u64be` as two `g4` / eight big-endian bytes |
| `gjstr` / `pjstr` | bytes until 10; the 10 is not part of the string |
| `gsmart` | if next `u8 < 128` then `g1() - 64`, else `g2() - 0xC000` (`Packet.java:360`) |
| `gsmarts` | if next `u8 < 128` then `g1()`, else `g2() - 0x8000` (`Packet.java:365`) |
| `g1_alt1` / `p1_alt1` | `u8`, wire byte is value+128 |
| `g1_alt2` / `p1_alt2` | `u8`, wire byte is `-value` |
| `g1_alt3` / `p1_alt3` | `u8`, wire byte is `128-value` |
| `g1b_alt1` | `i8`, wire byte minus 128 |
| `g1b_alt2` | `i8`, negation of the wire byte |
| `g1b_alt3` | `i8`, 128 minus the wire byte |
| `g2_alt1` / `p2_alt1` | `u16le` |
| `g2_alt2` / `p2_alt2` | `u16be` with the low byte stored as `low+128` |
| `g2_alt3` / `p2_alt3` | `u16le` with the first byte stored as `low+128` |
| `g2b_alt1` | `i16le` |
| `g2b_alt2` | `i16be` with the low byte minus 128 |
| `g3_alt3` | second byte `<< 16`, plus first byte `<< 8`, plus third byte (`Packet.java:571`) |
| `g4_alt1` | `u32le` |
| `g4_alt2` | `(b2<<24)+(b3<<16)+(b0<<8)+b1` after the four wire bytes `b0..b3` (`Packet.java:591`) |
| `g4_alt3` | `(b2<<24)+(b0<<16)+(b3<<8)+b1` (`Packet.java:601`) |
| `gdata` / `pdata` | raw bytes |
| `gdata_alt1` | `bytes(n)` reversed into the destination |
| `gdata_alt2` | each byte minus 128 |
| `ip2` | low byte, then a literal 0 (`Packet.java:149`) |
| `gBit` | MSB first from the bit cursor set by `accessBits` (`Packet.java:312`, `Packet.java:322`) |
| `accessBytes` | `pos = (bitpos + 7) / 8` (`Packet.java:347`) |

The same numeric opcode is used in both directions for 6, 26, 40, 50, 71, 75, 88, 100, 158, 181, 187, 199, 203, 206, 222, and 226. Direction decides which layout applies.
