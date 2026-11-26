# Framing

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`.

## Buffers

Game bytes sit on `deob.Unused6` (`valueUnused65` is the array, `valueUnused69` is the offset). The isaac packet buffer is `deob.ParticleEmitterRelated`, which extends `Unused6`. Outgoing `C5c7b4c41.valuekind7264` and incoming `C7502a728.valuekind8306` are each `new ParticleEmitterRelated(5000)`.

`C2S` opcodes are `deob.NoiseGeneratorRelated`. The constructor keeps `valueNoiseGeneratorRelated1` and ignores the second argument (`deob/NoiseGeneratorRelated.java:57`). `opNoiseGeneratorRelated2` returns that opcode. `LswprPos.opkind9851` writes it with `opkind8134` (`deob/LswprPos.java:42`).

There is no `int[]` client-to-server size table in this source. The second constructor argument is not stored. `rslib/src/client/packets/578.c` and `rslib/src/server/packets/578.c` are absent. `rslib/src/both/move/rev_578.c` has `k_sizes_c2s_578` and `k_sizes_s2c_578`. Those numbers match the constructor literals in this tree. Java consults only the server-to-client sizes. The missing client-to-server array is a blocked gap in [gaps.md](gaps.md). Packet rows still list the discarded literal so the source number is visible. That literal is not a size the client reads.

## Server-to-client sizes

`deob.CanAddYourselfOwnFriendList` stores both arguments: `valuekind9353` is the size, `valuekind9352` is the opcode (`deob/CanAddYourselfOwnFriendList.java:85`). `Cdba04b48.opkind7258` returns a 106-element array (`deob/Cdba04b48.java:24`). The slot equals the opcode stored on that object for every opcode 0 through 105. Framing indexes that array by the isaac opcode (`deob/C268455a9.java:38`).

Size values, opcode order:

`10, 6, 2, 3, 1, 8, -2, 2, 2, 8, -1, -1, 3, 5, 6, 10, 7, 2, -2, 0, 6, 3, 8, 8, 12, 8, 28, 8, 6, 3, 2, 8, -2, 15, 15, -2, 8, -2, 14, 17, 1, 3, 3, -1, 8, 1, -1, 6, 5, -1, -1, -1, 9, -1, 12, -1, -1, 12, 0, -2, -2, 4, 6, -1, 3, 5, 10, -1, 4, -2, -2, 20, -2, 6, 4, 0, -1, 12, 6, 7, 2, 8, 0, 7, -1, -2, 10, -2, -2, 6, 12, 2, 6, 5, 3, 8, 6, 7, -2, -1, -2, 14, 12, 0, 4, -1`

## Read loop

`C268455a9.opkind9551` (`deob/C268455a9.java:10`):

- If no packet is current and one raw byte is available, read it. If `ParticleEmitterRelated.opkind8137` is true, read a second byte. `opkind8137` (`ParticleEmitterRelated.java:155`) is `(bufferByte - isaacPeek) & 255 >= 128`. Peek uses `Cb55a2183.opkind6381`, which does not decrement (`deob/Cb55a2183.java:102`).
- Opcode is `opkind8131` (`ParticleEmitterRelated.java:110`): one byte minus `opkind6382`. `opkind6382` decrements (`Cb55a2183.java:111`). If that result is below 128, it is the opcode. Otherwise a second byte minus the next isaac value is combined as `((first - 128) << 8) | second`.
- An opcode outside `0 .. length-1` throws `IOException("invo:"+opcode)`.
- Size is `valuekind9353`.
- Size `-1`: read one raw unsigned byte and use it as the length (`C268455a9.java:48`).
- Size `-2`: read u16be with `opUnused694` and use it as the length (`C268455a9.java:59`).
- Wait until that many payload bytes are buffered, then copy them and dispatch.
- An opcode with no branch logs through `C565d1093.call49c1f9c0` and calls `C611b2b12.opkind9706` (`C268455a9.java:1785`).

The first packet after login success does not use this size field. State 10 always reads a u16be length (`deob/LswprPos.java:327`).

Client-to-server opcodes are one isaac byte: `opkind8134` stores `isaac + opcode` (`ParticleEmitterRelated.java:126`). No second-byte writer was read.

ISAAC mix constant `-1640531527` is in `Cb55a2183.callaaa6bb1d` (`Cb55a2183.java:121`).

## Codec

Names used in [packets.md](packets.md), [sync.md](sync.md), and [input.md](input.md). Reads and writes are `deob/Unused6.java` unless noted.

| name | method | line | meaning |
| --- | --- | --- | --- |
| u8 | `call08b1455d` | 204 | unsigned byte |
| i8 | `opUnused690` | 285 | signed byte |
| u8neg | `opUnused673` | 133 | `(-byte) & 255` |
| u8_128m | `opUnused680` | 178 | `(128 - byte) & 255` |
| u8_m128 | `opUnused6121` | 556 | `(byte - 128) & 255` |
| i8_m128 | `opUnused6115` | 468 | `(byte)(byte - 128)` |
| i8_neg | `call26c54ac0` | 118 | `(byte)(-byte)` |
| i8_128m | `opUnused6105` | 387 | `(byte)(128 - byte)` |
| u16be | `opUnused694` | 300 | big-endian unsigned |
| u16le | `opUnused672` | 128 | little-endian unsigned |
| i16be | `opUnused688` | 265 | big-endian, subtract 65536 if above 32767 |
| i16le | `opUnused6112` | 443 | little-endian, same sign fold |
| u16be_m128 | `opUnused675` | 144 | big-endian, low byte minus 128, unsigned |
| i16be_m128 | `opUnused671` | 108 | big-endian, low byte minus 128, signed fold |
| u16le_m128 | `opUnused6114` | 463 | low byte minus 128, high byte second, unsigned |
| i16le_m128 | `opUnused683` | 208 | low byte minus 128, high byte second, signed fold |
| u24be | `opUnused697` | 329 | three bytes, big-endian |
| u32be | `opUnused6100` | 351 | four bytes, big-endian |
| u32le | `call89703553` | 400 | four bytes, little-endian |
| u32_mix | `opUnused699` | 346 | `(b1<<24)\|(b0<<16)\|(b3<<8)\|b2` |
| u32_mix2 | `call1cda1d8d` | 458 | `(b2<<24)\|(b3<<16)\|(b0<<8)\|b1` |
| u64be | `opUnused669` | 95 | two u32be, high then low |
| jstr | `opUnused678` | 168 | bytes until 0, no version byte |
| gjstr2 | `call76b99ad9` | 477 | version byte must be 0, then jstr |
| jstr_null | `opUnused666` | 506 | a leading 0 returns null, otherwise jstr |
| smart | `opUnused684` | 218 | byte under 128 is u8, else u16be minus 32768 |
| smart_s | `opUnused685` | 223 | byte under 128 is `u8 - 64`, else `u16be - 49152` |
| smart_ext | `opUnused6103` | 368 | sum of smart while the value is 32767 |
| varint7 | `call8cc08f3e` | 182 | 7-bit groups, high bit continues |
| bytes | `opUnused670` | 101 | `n` bytes forward |
| bytes_rev | `calld61bdcd3` | 310 | `n` bytes stored last-to-first |
| bytes_rev_m128 | `calld67f8fbe` | 499 | reversed, each byte minus 128 |
| bits(n) | `opkind8135` | `ParticleEmitterRelated.java:130` | `n` bits, MSB within the byte |
| u8 write | `opUnused6106` | 391 | low 8 bits |
| u8neg write | `opUnused682` | 200 | `(-value) & 255` |
| u8_128m write | `opUnused686` | 228 | `(128 - value) & 255` |
| u8_add128 write | `opUnused692` | 296 | `(value + 128) & 255` |
| u16be write | `opUnused6107` | 395 | big-endian |
| u16le write | `opUnused695` | 305 | little-endian |
| u16le_add128 write | `opUnused6102` | 363 | low byte plus 128, then high byte |
| u16be_add128 write | `opUnused6116` | 472 | high byte, then low byte plus 128 |
| u32be write | `opUnused681` | 193 | big-endian |
| u32le write | `opUnused6110` | 416 | little-endian |
| u32_mix674 write | `opUnused674` | 137 | bytes `(value>>16), (value>>24), value, (value>>8)` |
| u24be write | `opUnused6109` | 410 | big-endian |
| u64be write | `opUnused696` | 317 | big-endian |
| pjstr | `call75e62fa6` | 255 | bytes plus 0; a NUL inside the string throws |
| backpatch u8 | `opUnused6119` | 531 | writes the length over the byte just before the counted span |
| backpatch u16be | `opUnused6113` | 453 | writes the length over the two bytes just before the counted span |

`opUnused677` (`Unused6.java:161`) steps the offset back 4, compares a checksum of the bytes before that to a following u32be, and leaves the offset after that u32be.
