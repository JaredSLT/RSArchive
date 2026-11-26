# Framing

## S2C size table

`Client.valueClient1464` is the 256-entry payload-size table, indexed by the ISAAC-decoded opcode (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:16`). A value of 0 is an empty payload. `-1` and `-2` are variable lengths, decoded below.

```text
0, 0, 0, 6, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 6, -1, 0, 0, 0, -2, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 8, 6, 0, 0, -1, 0, 7, 0, 5, 2, 0, 3, 0, 0, 1, 6, 0, 3, 3, 0, 0, 7, 6, 6, 2, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 5, 6, 6, 0, 0, 0, 6, 0, 2, 0, 0, 0, -2, 4, 0, 0, 0, 0, 0, 0, 14, -1, 0, 0, 2, 0, 0, 0, 10, 0, 0, 2, 0, 0, 0, 0, 0, 4, 10, 1, 2, 0, 0, 0, 0, 4, 5, 4, -1, 0, -2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 10, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, -2, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, -2, 2, 0, 5, 0, 0, 0, 0, 0, -2, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 0, 0, 1, 6, -2, 0, 2, 0, 8, 0, 5, 0, 1, 0, -2, 0, 0, 6, 0, 0, -2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
```

This client has no C2S size array. Outgoing packets are written into `BevorSieDenVorgangWiederholen.valuekind1336`, a 5000-byte buffer (`builds/rs2/443-2006-12-11/src/src/main/java/deob/BevorSieDenVorgangWiederholen.java:25`). The packet rows use `none` in the size column for those opcodes.

## S2C read loop

`PathingEntity.opPathingEntity12` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:169`):

1. If the socket is absent, return false.
2. If the saved opcode is -1, read one raw byte and decode it with `opkind13310`: `(buffer[pos++] - isaacNext) & 255` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/BevorSieDenVorgangWiederholen.java:198`).
3. Look up the size. If it is -1, read one raw byte and use `buffer[0] & 255` as the payload length. That byte is not passed through ISAAC, and the following payload read overwrites it.
4. If the size is -2 (`~size == 1`), read two raw bytes and parse them with `opMonotonicClock20` (u16be).
5. If fewer than `size` bytes are available, return false.
6. Read exactly `size` payload bytes at position 0. The length bytes are outside the handler-visible payload.
7. An opcode with no branch logs `T1` or `T2` and calls `NpcType.opNpcType12` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/PathingEntity.java:1500`).

## C2S write

`opkind1338` stores `(byte)(opcode + isaacNext)` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/BevorSieDenVorgangWiederholen.java:173`). `SoundBank.opSoundBank5` writes the used prefix and the caller sets the position back to 0. Some packets write a placeholder `u8` 0 and later store the following length with `opMonotonicClock36`, which writes that length at `pos - length - 1` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:518`).

## ISAAC

`NpcType` is the cipher. `opNpcType8` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/NpcType.java:174`) mixes eight words that start at `-1640531527` for four iterations, folds the seed in `valueNpcType73` across two 256-word passes into `valueNpcType70`, calls `opNpcType10`, and sets `valueNpcType61` to 256. `opNpcType10` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/NpcType.java:317`) is the result pass. Its index expressions are `(var2 & 1020) >> 2` and `(var3 & 261338) >> 8 >> 2`. `opNpcType7` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/NpcType.java:165`) regenerates only when the counter is 0 before the post-decrement, then sets the counter to 255 and returns `valueNpcType73[counter]`. After `opNpcType8` the counter is 256, so the first output is `valueNpcType73[255]` and does not regenerate.

Login seeds the outgoing cipher with the four random/server words, adds 50 to each word, then seeds the incoming cipher (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:294`). C2S adds the outgoing word. S2C subtracts the incoming word.

## Widths

Gets and puts are on `MonotonicClock`. Multi-byte gets name `buffer[pos - width]` as the first wire byte.

| Method | Width |
| --- | --- |
| `opMonotonicClock4` | u8 |
| `opMonotonicClock8` | u8a, `(b - 128) & 255` |
| `opMonotonicClock24` | u8s, `(128 - b) & 255` |
| `opMonotonicClock35` | u8c, `(-b) & 255` |
| `opMonotonicClock22` | i8 |
| `opMonotonicClock19` | i8s, `128 - b` |
| `opMonotonicClock30` | i8neg, `-b` |
| `opMonotonicClock33` | i8a, `b - 128` |
| `opMonotonicClock20` | u16be |
| `opMonotonicClock29` | i16be, subtract 65536 when the value is greater than 32767 |
| `opMonotonicClock21` | u16le |
| `opMonotonicClock14` | i16le, same 65536 bias |
| `opMonotonicClock38` | u16beA: high byte raw, low byte `(b - 128) & 255` |
| `opMonotonicClock12` | i16beA, same adjustment, then the 65536 bias |
| `opMonotonicClock53` | u16leA: low byte `(b - 128) & 255`, high byte raw |
| `opMonotonicClock40` | i16leA, same adjustment, then the 65536 bias |
| `opMonotonicClock13` | u24be |
| `opMonotonicClock42` | u24mix, `(b0 << 16) \| (b2 << 8) \| b1` |
| `opMonotonicClock31` | u32be |
| `opMonotonicClock41` | u32le |
| `opMonotonicClock49` | u32mix49, `(b2 << 24) \| (b3 << 16) \| (b0 << 8) \| b1` |
| `opMonotonicClock32` | u32mix32, `(b1 << 24) \| (b0 << 16) \| (b3 << 8) \| b2` |
| `opMonotonicClock15` | u64be, two u32be |
| `opMonotonicClock44` | jstr, bytes until 0 |
| `opMonotonicClock46` | smartu: u8 when the peek is below 128, otherwise u16be minus 32768 |
| `opMonotonicClock37` | smarts: byte minus 64 when the peek is below 128, otherwise u16be minus 49152 |
| `opMonotonicClock18` | 7-bit continuation |
| `opkind1339` | `n` bits, high bit first (`builds/rs2/443-2006-12-11/src/src/main/java/deob/BevorSieDenVorgangWiederholen.java:177`) |
| `opMonotonicClock25` | put u8 |
| `opMonotonicClock5` | put u8neg, `(byte)(-v)` |
| `opMonotonicClock17` | put u8s, `(byte)(128 - v)` |
| `opMonotonicClock6` | put u16le |
| `opMonotonicClock16` | put u16leA, first byte `(byte)(v + 128)` |
| `opMonotonicClock51` | put u16be |
| `opMonotonicClock52` | put u16beA, low byte `(byte)(v + 128)` |
| `opMonotonicClock26` | put u24be |
| `opMonotonicClock28` | put u32be |
| `opMonotonicClock9` | put u32le. The second shift is `-242288600`, and that count masked to 5 bits is 8 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:82`) |
| `opMonotonicClock39` | put u32mix39, bytes `(v >> 8), v, (v >> 24), (v >> 16)` |
| `opMonotonicClock47` | put u32mix47, bytes `(v >> 16), (v >> 24), v, (v >> 8)`. The last shift is `-334352184`, masked to 8 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:713`) |
| `opMonotonicClock23` | put u64be |
| `opMonotonicClock54` | put jstr plus a 0 byte |
| `opMonotonicClock50` | put smart: u8 below 128, otherwise u16be of `v + 32768` below 32768 |
| `opMonotonicClock36` | backpatch a u8 length |
| `opMonotonicClock10` / `opMonotonicClock45` | raw bytes |
| `opMonotonicClock2` | read `n` bytes stored as `(byte)(b - 128)` |
| `opMonotonicClock3` | read `n` bytes in reverse |

`opkind1332` sets the bit cursor to `bytePos * 8`. `opkind1334` sets the byte cursor to `(bitPos + 7) / 8`. `opkind1331(packetSize)` is `packetSize * 8 - bitPos`.
