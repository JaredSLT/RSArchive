# Framing

Buffer type is `com.jagex.game.world.entity.PositionedSound`. Bit and ISAAC access is `deob.PositionedSoundPart4`. The cipher is `com.jagex.game.client.PrivateChatFilter`. `com.jagex.encryption.Isaac` is not this cipher.

## Size tables

Client to server opcodes 0 through 122 and sizes are the `(opcode, size)` pairs in `deob/DataBlock1675.java`. `tint` is the opcode and `tint2` is the size (`DataBlock1675.java:256`).

Server to client opcodes 0 through 192 are the pairs in `deob/DataBlock1193.java` (`DataBlock1193.java:392`). `doDataBlock1328` returns that table in opcode order (`DataBlock1193.java:397`). The reader indexes it with the decoded opcode (`deob/Statics.java:4108`).

There is no constructor past those ranges. A later pass must not fill the rest with `-3`.

`size == -1` reads one more unsigned byte and uses that as the payload length (`deob/Statics.java:4118`). `size == -2` reads a big-endian unsigned 16-bit length (`doPositionedSound322`, `Statics.java:4129`). `size == 0` is an empty payload. A positive size is that many payload bytes (`Statics.java:4141`). The opcode bytes are not included in `size`.

Client to server, the queue copies `cls4fafe547.tint` bytes and does not add a length prefix (`com/jagex/graphics/safe/PureJavaToolkitContext.java:88` and `:100`). A `-1` or `-2` client packet is as long as its writer left the cursor. Allocation before the write: `-1` gets 260 bytes, `-2` gets 10000, `size <= 18` gets 20, `size <= 98` gets 100, otherwise 260 (`DynamicGroundDecorEntity.java:34`).

Login opcodes in `deob/DataBlock1802` store only the first constructor argument (`DataBlock1802.java:49`). The second argument is discarded. Login writes are plain bytes until the cipher is installed. They do not use the `-1` and `-2` rules above.

## ISAAC

`PrivateChatFilter` is a 256-word ISAAC. Init mixes with `-1640531527` (`PrivateChatFilter.java:78`). `doPrivateChatFilter` returns the next int and consumes it (`PrivateChatFilter.java:31`). `doPrivateChatFilter2` peeks (`PrivateChatFilter.java:41`).

Client to server opcode byte: `value + nextInt`, one byte (`PositionedSoundPart4.java:25`), called with the opcode at `DynamicGroundDecorEntity.java:47`.

Server to client: subtract `nextInt`. If the peeked byte is `>= 128`, the opcode is two bytes: `(first - 128) << 8` plus a second subtracted byte (`PositionedSoundPart4.java:35` and `:41`). The peek uses `doPrivateChatFilter2` and does not consume.

After a successful login packet the outgoing cipher is built from the four seed ints. Those seeds are then each increased by 50 and the incoming cipher is a new `PrivateChatFilter` of the adjusted seeds. The read buffer is bound to the incoming cipher (`deob/BlankWs.java:509` and `deob/Statics.java:2641`).

## Widths

Writes and reads below are `PositionedSound` unless named otherwise. Byte 0 is the first byte on the wire.

| Name | Write | Read |
| --- | --- | --- |
| u8 | `doPositionedSound(int)` `:99` | `doPositionedSound3` `:310` |
| i8 | | `doPositionedSound32` `:315` |
| u8add128 | `doPositionedSound3222` `:669` stores `v+128` | `doPositionedSound32222222b8` `:684` is `(byte-128)&255` |
| u8neg | `doPositionedSound32222` `:674` stores `0-v` | `doPositionedSound32222222b15` `:689` is `(0-byte)&255` |
| u8sub128 | `doPositionedSound322222` `:679` stores `128-v` | `doPositionedSound32222222b17` `:694` is `(128-byte)&255` |
| u16be | `doPositionedSound2(int)` `:104` | `doPositionedSound322` `:320` |
| i16be | | `doPositionedSound33` `:326` |
| u16le | `doPositionedSound22(int)` `:110` and `doPositionedSound22b` `:714` | `doPositionedSound32222222b11` `:732` |
| i16le | | `doPositionedSound32222222` `:750` |
| u16be+128 | `doPositionedSound32222222b4` `:720` high byte, then low+128 | `doPositionedSound32222222b12` `:738` |
| u16le+128 | `doPositionedSound32222222b5` `:726` low+128, then high | `doPositionedSound32222222b13` `:744` |
| u24be | `doPositionedSound222` `:116` | `doPositionedSound3222` `:337` |
| i24be | | `doPositionedSound2322` `:343` |
| u24le | `doPositionedSound2223` `:772` | `doPositionedSound32222222b14` `:793` |
| u32be | `doPositionedSound2222` `:123` | `doPositionedSound322222` `:354` |
| u32le | `doPositionedSound223` `:131` | `doPositionedSound322222b` `:360` and `doPositionedSound322222b3` `:823` (same shifts) |
| u32m-32222222 | `doPositionedSound32222222` `:807`: `>>8`, low, `>>24`, `>>16` | |
| u32m-32222222b3 | `doPositionedSound32222222b3` `:815`: `>>16`, `>>24`, low, `>>8` | |
| u32m-32222222b16 | | `doPositionedSound32222222b16` `:829`: b1 + (b0<<8) + (b3<<16) + (b2<<24) |
| u32m-3322b6 | | `doPositionedSound3322b6` `:835`: b2 + (b3<<8) + (b0<<16) + (b1<<24) |
| u40be | `doPositionedSound(long)` `:139` writes bits 32..0 | |
| u48be | | `doPositionedSound3322b3` `:373` is u16be then u32be |
| u64be | `doPositionedSound22(long)` `:158` | `doPositionedSound3322b8` `:380` is two u32be |
| jstr | `doPositionedSound3(String)` `:188` then u8 0 | `doPositionedSound3322b4` `:419` reads until 0 |
| jstr0 | `doPositionedSound2(String)` `:199` is u8 0, jstr, u8 0 | `doPositionedSound3222222` `:430` requires a leading 0, then a jstr |
| bytes(n) | `doPositionedSound3(byte[], off, len)` `:219` | `doPositionedSound2` `:463`. `doPositionedSound22` `:841` copies in reverse |
| length patch u16be | `doPositionedSound3(int)` `:232` writes the length at `cursor-length-2` | |
| length patch u8 | `doPositionedSound32(int)` `:242` writes at `cursor-length-1` | |

`jstr` bytes are produced by `EffectAnimTypeFactory.doEffectAnimTypeFactory`. That encoder was not read (`gaps.md`).

Smart reads, using the character values in the source:

- `doPositionedSound222` `:471`: if the next byte is `< 128`, u8 minus 64, else u16be minus U+C000 (49152).
- `doPositionedSound32222222b4` `:477`: if `< 128`, u8, else u16be minus U+8000 (32768).
- `doPositionedSound2222` `:483`: if `< 128`, u8 minus 1, else u16be minus U+8001 (32769).
- `doPositionedSound32222222b4b` `:507`: if the next byte is negative, u32be with the top bit cleared, else u16be, and 32767 means -1.
- `doPositionedSound22222` `:517`: 7-bit groups while the byte is negative.

Bit cursor: `doPositionedSoundPart4222` sets the cursor to `byteCursor * 8` (`PositionedSoundPart4.java:55`). `doPositionedSoundPart42(n)` reads `n` bits from the high side of the current byte (`PositionedSoundPart4.java:60`). `doPositionedSoundPart42222` advances to the next byte (`:81`). `doPositionedSoundPart422(byteLength)` is `byteLength*8 - bitCursor` (`:86`).

XTEA encrypt is `doPositionedSound(int[], start, end)` (`PositionedSound.java:590`), delta `-1640531527`, sum starts at 0, 32 rounds, blocks of two u32be. Decrypt is `doPositionedSound2(int[], start, end)` (`:615`), sum starts at `-957401312`. RSA is `doPositionedSound(BigInteger, BigInteger)` (`:640`): `modPow`, then u16be length of `toByteArray()`, then those bytes, replacing the buffer.

Login exponent is `E9516d7340d9343f6ec83c91874488906e42bc63f03971c0.bigInteger`, constructed as hex `10001`. The modulus used by `BlankWs` is `bigInteger2` in that class (`deob/E9516d7340d9343f6ec83c91874488906e42bc63f03971c0.java:8`).
