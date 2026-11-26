# Framing

There is one S2C size table and no C2S size table. Game packets after login use ISAAC. The JS5 socket and the login stages before the seed do not.

## Buffers

The incoming game buffer is `MaterialManager_Class106_Sub2.valueMaterialManager_Class106_Sub2117`, a `TextureOp27` of 5000 bytes (`rt4/MaterialManager_Class106_Sub2.java:6`). The outgoing game buffer is `InvType.valueInvType19`, also a `TextureOp27`. `TextureOp27` extends `ByteArrayNodeSecondary` and holds the ISAAC cipher plus the bit offset.

`World` is the ISAAC generator. `opWorld74` returns the next int, regenerating the 256-word state when the counter hits zero (`rt4/World.java:58-64`). The mix starts from eight copies of `-1640531527` (`rt4/World.java:67-76`).

Opcode direction (`rt4/TextureOp27.java:99-108`):

| Direction | Operation |
| --- | --- |
| S2C | `(buffer[pos++] - isaac.next()) & 255` |
| C2S | `buffer[pos++] = (byte)(opcode + isaac.next())` |

Login copies the four seed ints into the outgoing cipher, adds 50 to each, and copies that into the incoming cipher (`rt4/TextureOp29SubOp.java:227-233`).

## S2C sizes

`TextureOpCurve.valueTextureOpCurve65` has 256 entries. Index is the opcode (`deob/TextureOpCurve.java:107`). The only reader is `JavaMouseWheel.call72b59b7f` (`rt4/JavaMouseWheel.java:170`).

```text
0 0 0 0 20 1 0 5 0 0 6 0 0 0 0 3
0 7 14 0 0 0 4 0 0 0 0 0 3 0 1 0
11 6 0 -1 10 0 15 0 0 0 1 0 6 0 0 0
6 6 0 0 0 0 24 4 -2 -1 0 0 0 -2 0 0
0 0 2 0 0 0 0 5 0 0 0 0 8 0 0 0
0 0 0 0 0 0 2 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 6 0 0 0 0
0 0 6 0 0 -1 0 -2 0 2 0 0 0 0 0 6
-2 0 0 0 0 12 0 0 0 0 0 0 0 0 0 0
0 0 -2 0 0 0 -2 6 0 4 0 0 0 0 2 0
0 0 0 -2 0 0 0 6 2 2 0 0 0 -2 4 -2
0 0 8 6 0 0 0 0 5 0 0 -2 4 0 0 7
0 0 0 0 6 0 0 0 11 0 0 0 0 5 0 2
0 -1 0 0 0 0 0 0 0 0 0 0 7 0 0 0
0 -1 0 0 0 0 3 -2 0 0 0 0 4 -2 -2 0
0 0 0 5 0 0 0 0 0 10 0 2 0 3 0 6
```

`-1` means the next byte is a u8 length. `-2` (`~size == 1`) means the next two bytes are a u16be length via `opByteArrayNodeSecondary69`. The length bytes occupy the front of the buffer and are overwritten when the payload of that many bytes is read. The opcode byte is not part of `size`. If `size` is still greater than the bytes available, the read waits (`rt4/JavaMouseWheel.java:165-199`).

An opcode with no `if` in `call72b59b7f` throws `T1` and calls `ConnectingServer.opkind2372` (`rt4/JavaMouseWheel.java:1524-1536`). Opcode 151 is in the size table as 6 and has no `if`.

## C2S

There is no C2S size array in this client. A client packet is the ISAAC opcode byte plus the writes that follow it. Two writers back-patch a length: `opByteArrayNodeSecondary38` stores a u8 at `pos - 1 - length` (`rt4/ByteArrayNodeSecondary.java:304-306`), and `opByteArrayNodeSecondary50` stores a u32be four bytes before the payload. The u8 form is used by mouse recorder opcode 121 and reflection opcode 149. The u32be form was not found on a C2S write in the methods read here.

`rslib/src/client/packets/474.c` contains a C2S size table. This client has nowhere to copy that table from. See [gaps.md](gaps.md).

The game socket flushes when `InvType.valueInvType19` has a positive offset (`rt4/InvType.java:472-476`).

## Bits

`opTextureOp279` sets the bit position to eight times the byte position. `opTextureOp2711(n)` reads `n` bits, most significant bit first inside each byte, using `SlArrows.valuekind2223` as the width mask (`rt4/TextureOp27.java:119-140`). `opTextureOp274` byte-aligns with `(bitPosition + 7) / 8`. `opTextureOp277(size)` is the number of bits left in a packet of that size.

## Update socket

JS5 is a separate socket. The handshake is plaintext: u8 `15`, u32be `474`, five bytes (`rt4/Client.java:417-422`). One response byte must be 0 (`rt4/Client.java:434-438`). While a priority is pending, the update socket also writes u8 `4`, u8 priority, u16be `0` (`deob/BitteVersuchenSieEs.java:64-68`).

## Integer layouts

Reads and writes below are `ByteArrayNodeSecondary` unless noted. Multi-byte values are listed in stream order.

| Method | Layout |
| --- | --- |
| 67 read, 54 write | u8 |
| 71 read | i8 |
| 22 read | u8neg, `(-byte) & 255` |
| 42 read | u8m128, `(byte - 128) & 255` |
| 57 read | u8n128, `(128 - byte) & 255` |
| 28 read | i8neg, `(byte)(-byte)` |
| 30 read | i8_128sub, `(byte)(128 - byte)` |
| 31 read | i8m128, `(byte)(byte - 128)` |
| 62 write | u8 as `(byte)(-value)` |
| 36 write | u8 as `(byte)(128 - value)` |
| 69 read, 55 write | u16be |
| 65 read, 52 write | u16le |
| 21 read | i16be. A value above 32767 loses 65536 (`rt4/ByteArrayNodeSecondary.java:210-216`) |
| 32 read | i16le, same sign rule (`rt4/ByteArrayNodeSecondary.java:262-268`) |
| 46 read | u16le_lo_m128. First byte is `(low - 128) & 255`, second byte is the high byte (`rt4/ByteArrayNodeSecondary.java:354-356`) |
| 73 read | u16be_lo_m128. High byte, then `(low - 128) & 255` (`rt4/ByteArrayNodeSecondary.java:546-549`) |
| 60 read | i16be_lo_m128. Same bytes as 73, then the sign rule (`rt4/ByteArrayNodeSecondary.java:438-444`) |
| 35 write | u16be with the low byte stored as `(byte)(value + 128)` |
| 47 write | u16le with the low byte stored as `(byte)(value + 128)` |
| 23 read, 37 write | u32be |
| 68 read, 63 write | u32le |
| 26 read, 33 write | u32me. Stream bytes `b0 b1 b2 b3` rebuild as `(b2 << 24) \| (b3 << 16) \| (b0 << 8) \| b1` (`rt4/ByteArrayNodeSecondary.java:234-236`, write at `272-276`) |
| 59 read, 56 write | u32me2. Stream bytes rebuild as `(b1 << 24) \| (b0 << 16) \| (b3 << 8) \| b2` (`rt4/ByteArrayNodeSecondary.java:433-435`, write at `415-419`) |
| 25 read, 20 write | u24be |
| 29 read | u24me, `(b0 << 16) \| (b2 << 8) \| b1` (`rt4/ByteArrayNodeSecondary.java:249-251`) |
| 61 read, 39 write | u64be, high u32be then low u32be |
| 43 write | two u32me writes, high then low |
| 34 read | jstr. Bytes until a 0, exclusive (`rt4/ByteArrayNodeSecondary.java:279-285`) |
| 45 read | nullable jstr. A leading 0 returns null |
| 51 read | unsigned smart. A first byte at least 128 selects `u16be - 32768`, otherwise u8 |
| 49 read | A first byte at least 128 selects `u16be - 49152`, otherwise `u8 - 64` |
| 64 read, 66 write | 7-bit groups with the high bit set on every byte except the last |
| 53 read | `bytes(n)` stored in reverse |
| 58 read, 44 write | `bytes(n)` in order |
| 27 | CRC of the current payload via `SoftwareModel.opSoftwareModel2`, then u32be. The CRC method body was not read |

XTEA for map loc bytes is `opByteArrayNodeSecondary70` (`rt4/ByteArrayNodeSecondary.java:520-540`). It starts at offset 5, takes `(length - 5) / 8` blocks, and runs 32 rounds. The sum starts at `-957401312` and the delta is `-1640531527`. One key index is `sum & 3`. The other is `(sum >>> 11) & 1268776963`.
