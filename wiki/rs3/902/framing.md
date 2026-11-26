# Framing

Sizes are copied from the constructors in this client. `DataBlock420` is game c2s, opcodes 0 through 122 (`deob/DataBlock420.java`). `DataBlock457` is game s2c, opcodes 0 through 194 (`deob/DataBlock457.java`). Negative sizes stay `-1` and `-2`. The numeric tables are the rows in [packets.md](packets.md).

`DataBlock351` is the login opcode list. Its constructor stores only the first argument (`deob/DataBlock351.java:48`). The second argument is not read at runtime. The login writers still emit a `u16be` length when that unused argument is `-2`, and a bare opcode when it is `0`.

## Buffer

`com.jagex.game.world.WorldMapElement` is the byte buffer. `deob.WorldMapElementPart` adds ISAAC and bits. Names below are the widths, not the obfuscated method names.

| Name | Write | Read |
| --- | --- | --- |
| u8 | `doWorldMapElement(int)` `WorldMapElement.java:98` | `doWorldMapElement2` `:319` |
| i8 | | `doWorldMapElement22` `:324` |
| u8+128 | `doWorldMapElement2222b16` `:678` stores `v + 128` | `doWorldMapElement2222b17` `:693` returns `b - 128` |
| u8(0-b) | `doWorldMapElement2222b7` `:683` stores `0 - v` | `doWorldMapElement2222b18` `:698` returns `0 - b` |
| u8(128-b) | `doWorldMapElement2222b8` `:688` stores `128 - v` | `doWorldMapElement2222b19` `:703` returns `128 - b` |
| u16be | `doWorldMapElement2` `:103` | `doWorldMapElement222` `:329` |
| i16be | | `doWorldMapElement2222b30` `:335` |
| u16le | `doWorldMapElement2222b17` `:109` and `doWorldMapElement2222b9` `:723` | `doWorldMapElement2222b23` `:741` |
| u16be_low+128 | `doWorldMapElement2222b15` `:729` high, then `low + 128` | `doWorldMapElement2222b24` `:747` |
| u16le_low+128 | `doWorldMapElement2222b23` `:735` `low + 128`, then high | `doWorldMapElement2222b37` `:753` |
| u24be | `doWorldMapElement222` `:115` | `doWorldMapElement2222b3` `:346` |
| i24be | | `doWorldMapElement2222b4` `:352` |
| u24le | `doWorldMapElement2222b10` `:792` | `doWorldMapElement2222b26` `:813` is a different permutation: `b0<<8 + b1<<16 + b2` |
| u32be | `doWorldMapElement2222` `:122` | `doWorldMapElement2222b6` `:363` |
| u32le | `doWorldMapElement2222b18` `:130` and `doWorldMapElement2222b12` `:819` | `doWorldMapElement2222b27` `:843` and `doWorldMapElement2222b31` `:369` |
| u32_alt13 | `doWorldMapElement2222b13` `:827`: `>>8`, low, `>>24`, `>>16` | `doWorldMapElement2222b35` `:849` |
| u32_alt14 | `doWorldMapElement2222b14` `:835`: `>>16`, `>>24`, low, `>>8` | `doWorldMapElement2222b28` `:855` |
| u64be | `doWorldMapElement22(long)` `:157` | `doWorldMapElement2222b5` `:389` as two `u32be` |
| u48be | | `doWorldMapElement2222b36` `:382`: `u16be` then `u32be` |
| u40be | | `doWorldMapElement2222b33` `:375`: `u8` then `u32be` |
| jstr | `doWorldMapElement2(String)` `:192`: `DataBlock200.doDataBlock143` then `0` | `doWorldMapElement2222b9` `:428` stops at `0` |
| jstr0 | `doWorldMapElement222(String)` `:208`: `0`, bytes, `0` | `doWorldMapElement2222b10` `:439` requires a leading `0` |

`DataBlock200.doDataBlock143` (`deob/DataBlock200.java:271`) maps bytes `0..127` to that code point, bytes `128..159` through `charArray` at line 6 (the Windows-1252 `0x80..0x9F` row), and any other byte to that code point. A `0` ends the string and is not part of it.

`smartu` is `doWorldMapElement2222b38` (`:486`): if the next byte is below 128, read `u8`, otherwise `u16be - 0x8000`. The source spells `0x8000` as the character U+8000. `smarts` is `doWorldMapElement2222b34` (`:480`): `u8 - 64`, or `u16be - 0xC000`. `smart2or4` is the no-argument `doWorldMapElement2222b` (`:516`): if the next byte is negative, `u32be & 0x7fffffff`, otherwise `u16be`, and `32767` means `-1`.

A `u16be` length is backpatched by `doWorldMapElement2222b(int)` (`:241`). A `u8` length is backpatched by `doWorldMapElement2222b3` (`:251`).

XTEA is `doWorldMapElement(int[])` (`:553`), 32 rounds, delta `-1640531527`. Decrypt is `doWorldMapElement2(int[])` (`:576`). RSA is `doWorldMapElement(BigInteger, BigInteger)` (`:649`): `u8` length, then the ciphertext bytes.

## ISAAC

`WorldMapElementPart.doWorldMapElementPart(int)` (`deob/WorldMapElementPart.java:25`) writes `(opcode + isaac.nextInt())` as a byte. That is the c2s opcode.

`doWorldMapElementPart()` (`:30`) reads `(byte - isaac.nextInt()) & 255`.

`doWorldMapElementPart22` (`:41`) is the s2c opcode. If the first decoded byte is below 128 it is the opcode. Otherwise the opcode is `((first - 128) << 8) + second`, and both bytes are ISAAC-adjusted.

`doWorldMapElementPart2()` (`:35`) peeks with `Isaac.doIsaac` (`com/jagex/encryption/Isaac.java:41`), which returns `rsl[count - 1]` and does not decrement `count`. `nextInt` (`Isaac.java:31`) decrements `count`. The peek and the consume are different methods.

Bit access: `doWorldMapElementPart222` sets the bit cursor to `pos * 8` (`:55`). `doWorldMapElementPart2(n)` reads `n` bits, MSB first from the current byte (`:60`). `doWorldMapElementPart2222` byte-aligns with `(bitpos + 7) / 8` (`:81`). `doWorldMapElementPart22(packetLength)` is `packetLength * 8 - bitpos` (`:86`).

## Game packet

`Client.doClient22(DataBlock335)` (`rs2/client/Client.java:7280`):

1. Read one byte into the receive buffer. If `doWorldMapElementPart2()` says the opcode is extended, read a second byte.
2. Decode the opcode with `doWorldMapElementPart22` and index `DataBlock457.doDataBlock350()`.
3. The table size is `DataBlock457.tint2`.
4. If the size is `-1`, read one byte and use it as the payload length (`:7320`).
5. If the size is `-2`, read two bytes and take `u16be` (`:7331`).
6. If the length is greater than 0, read that many bytes over the length bytes (`:7343`). The handler sees only the payload. Size `0` has an empty payload.

`Heap.doHeap(DataBlock420, Isaac)` (`com/jagex/core/datastruct/Heap.java:34`) allocates 260 bytes for `-1`, 10000 for `-2`, 20 when the size is at most 18, 100 when it is at most 98, otherwise 260. It then writes the ISAAC opcode. `DataBlock335.doDataBlock329` (`deob/DataBlock335.java:95`) queues `buffer.tint` bytes. It does not insert a length. A `-1` writer places a `u8` and backpatches it. A `-2` writer places a `u16be` and backpatches it.

## Seeds

Login builds `isaac` from the four `int` seeds and `isaac2` from `seed[i] + 50` (`SecureRandomTask.java:558`). The receive buffer is given `isaac2` (`:567`). The early lobby handshake does the same with its four random seeds (`DefaultSprites.java:233`). Outgoing game packets use `Client.clsb20f177f2.isaac`. Lobby traffic before the world uses `Client.clsb20f177f`.
