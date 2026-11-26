# Framing

There is no C2S size array in this client. Outgoing sizes are whatever `TracingException.valueTracingException72` writes after `Cc382a83a.opkind3405`. `C105f7f5d.valuekind3423` is the S2C size table only (`builds/rs2/481-2007-12-11/src/src/main/java/deob/C105f7f5d.java:4`). A zero in that table is a fixed empty payload. `-1` and `-2` are variable lengths.

`rslib/src/server/packets/481.c` publishes the same 256 S2C sizes. `rslib/src/client/packets/481.c` is absent. RSLib's move comment says `valuekind3423` is not a C2S table (`rslib/src/both/move/rev_481.c:24`).

## S2C sizes

Copied from `C105f7f5d.valuekind3423`. The array length is 256. Omitted opcodes are 0.

| opcode | size |
| --- | --- |
| 4 | -2 |
| 6 | 2 |
| 8 | -1 |
| 10 | 8 |
| 17 | 3 |
| 18 | 7 |
| 21 | 6 |
| 29 | 2 |
| 30 | 3 |
| 35 | 2 |
| 37 | -2 |
| 39 | 4 |
| 45 | -2 |
| 46 | 5 |
| 47 | -2 |
| 50 | 6 |
| 61 | 7 |
| 63 | 7 |
| 65 | 15 |
| 68 | 2 |
| 77 | 10 |
| 84 | -2 |
| 86 | 12 |
| 88 | 6 |
| 90 | 6 |
| 98 | 6 |
| 99 | 6 |
| 103 | -1 |
| 105 | -2 |
| 108 | 5 |
| 114 | -2 |
| 115 | 1 |
| 120 | -1 |
| 123 | 6 |
| 126 | -2 |
| 127 | 3 |
| 130 | 8 |
| 135 | 2 |
| 137 | 20 |
| 141 | 4 |
| 142 | 4 |
| 146 | 11 |
| 148 | 2 |
| 149 | -2 |
| 152 | 4 |
| 159 | 5 |
| 162 | 2 |
| 165 | 1 |
| 166 | 6 |
| 172 | 4 |
| 174 | 14 |
| 179 | 6 |
| 184 | -2 |
| 186 | 1 |
| 187 | -2 |
| 188 | 5 |
| 198 | 5 |
| 206 | 11 |
| 208 | 6 |
| 209 | 6 |
| 212 | -1 |
| 213 | 3 |
| 216 | -1 |
| 218 | 2 |
| 230 | 10 |
| 231 | 6 |
| 236 | 24 |
| 243 | 4 |
| 245 | -2 |
| 252 | 6 |
| 254 | -2 |

## Read loop

`SpotAnim.call72b59b7f` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:25`):

1. If `Cd03d7e86.valuekind29812` is `-1` (`~opcode == 0` at line 35), read one byte and set the opcode with `Cc382a83a.opkind3404` (line 39). Size is `valuekind3423[opcode]` (line 40).
2. If that size is `-1` (line 43), read one more byte. The payload length is that unsigned byte (line 50).
3. If that size is `-2` (`~size == 1` at line 53), read two bytes. The payload length is `SlArrows.opkind3349`, a `u16be` (line 61).
4. If the socket has fewer than `size` bytes (`~available > ~size` at line 64), return false and keep the opcode.
5. Read exactly `size` bytes into `C218af8f4.valuekind3083` and set its position to 0 (lines 68-69).
6. Dispatch. An opcode with no branch reaches `CursorType.call49c1f9c0` with the text `T1 - ` and then `Ca17abb5a.opkind2335` (lines 1426-1427). Opcode 208 is in the size table at size 6 and has no branch.

The socket wrapper is `Song`: `opSong10` is available, `opSong11` reads one byte, `call09d88f3c` fills a buffer (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SpotAnim.java:30`).

## ISAAC

`Cc382a83a.opkind3405` writes `(isaac.opkind3071() + opcode)` as one byte (`builds/rs2/481-2007-12-11/src/src/main/java/deob/Cc382a83a.java:61`). `opkind3404` reads `(byte - isaac.opkind3071()) & 255` (line 57). Both directions call the same `C016fbb54.opkind3071`, which returns `results[count]` after decrementing and regenerates when the count hits 0 (`builds/rs2/481-2007-12-11/src/src/main/java/deob/C016fbb54.java:94`).

Seeds are installed in `SoundBank.opSoundBank15`. The outgoing cipher is `opkind3407` on the four ints. Each int is then increased by 50, and the incoming cipher is `opkind3407` on those adjusted ints (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SoundBank.java:154`). The mix in `opkind3072` starts from the golden-ratio constant `-1640531527` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/C016fbb54.java:104`).

## Bits

`Cc382a83a.opkind3406` sets the bit cursor to `bytePosition * 8` (line 65). `opkind3403(n)` reads `n` bits from the high end of each byte (lines 36-54). `opkind3402(packetSize)` returns `8 * packetSize - bitPosition` (line 32). `opkind3409` byte-aligns with `(bitPosition + 7) / 8` (line 73).

## Buffer widths

Position is `SlArrows.valuekind3341`. Bytes are `valuekind3345`. Widths below are the method bodies in `builds/rs2/481-2007-12-11/src/src/main/java/deob/SlArrows.java`.

| Method | Width |
| --- | --- |
| `opkind33442` / `opkind33446` | `u8` read / write |
| `opkind3348` | `i8` |
| `opkind3345` / `opkind3344` | `u8neg` read `(-b) & 255` / write `-v` |
| `opkind33432` / `opkind33422` | `u8_add128` read `(b - 128) & 255` / write `v + 128` |
| `opkind33424` | `u8_sub128` read `(128 - b) & 255` |
| `opkind3347` | `i8` of `(128 - b)` |
| `opkind33427` | `i8` of `-b` |
| `opkind3349` / `opkind33428` | `u16be` |
| `opkind33416` / `opkind33423` | `u16le` |
| `opkind33439` | `i16be` |
| `opkind33445` / `opkind3343` | `u16be_sub128` (low byte is `b - 128`) |
| `opkind3342` | `i16be_sub128` |
| `opkind33425` / `opkind33413` | `u16le_sub128` read / `u16le_add128` write |
| `opkind33417` | `i16le_sub128` |
| `opkind33450` / `calld233f8e8` | `u24be` |
| `opkind33419` | `u24le` |
| `opkind33437` / `opkind33443` | `u32be` |
| `opkind33433` / `opkind33436` | `u32le` |
| `opkind33430` / `opkind33412` | `u32_me`, byte order `(b1, b0, b3, b2)` on the wire for value bytes `(b0, b1, b2, b3)` |
| `opkind33447` / `opkind33414` | `u32_ime`, byte order `(b2, b3, b0, b1)` |
| `opkind33410` / `opkind33438` | `u64be` |
| `opkind33415` | `jstr` ended by byte 0. The charset builder `Texture.call27f7154f` was not read. |
| `opkind33420` | nullable `jstr`; a leading 0 is null |
| `opkind33435` | write `jstr` plus 0 |
| `opkind33441` | `usmart`: peek `< 128` then `u8`, else `u16be - 32768` |
| `opkind33429` | `smart`: peek `< 128` then `u8 - 64`, else `u16be - 49152` |
| `opkind3346` / `opkind33411` | `bytes(n)` |
| `call56371ce8` | `bytes(n)` copied last-to-first |
| `opkind33448` | `bytes(n)` copied last-to-first, each minus 128 |
| `opkind33418` | varint, 7-bit groups while the byte is negative |
| `opkind33421` | back-patch a `u8` length |
| `opkind3341` | XTEA decrypt in place, delta `-1640531527`, 32 rounds (lines 46-67) |

`opkind33431` writes a varint, but its shift constants include the literals `268627493` and `16403` (lines 253-270). Those writes are not used by a packet read for this page.
