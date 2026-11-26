# Framing

Buffer class: `com.jagex.game.world.entity.PositionedSound` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/world/entity/PositionedSound.java`). Bit and Isaac wrapper: `deob.PositionedSoundPart`. Cipher: `deob.DataBlock482`.

`jstr` means a NUL-terminated string written or read through `ScriptFrame.doScriptFrame`. The charset method itself was not opened.

## Widths

Writes in `PositionedSound`:

| method | line | width |
| --- | --- | --- |
| `doPositionedSound(int)` | 99 | u8 |
| `doPositionedSound2(int)` | 104 | u16be |
| `doPositionedSound22(int)` | 110 | u16le |
| `doPositionedSound222(int)` | 116 | u24be |
| `doPositionedSound2222(int)` | 123 | u32be |
| `doPositionedSound223(int)` | 131 | u32le |
| `doPositionedSound(long)` | 139 | u40be |
| `doPositionedSound2(long)` | 148 | u48be |
| `doPositionedSound22(long)` | 158 | u64be |
| `doPositionedSound3(String)` | 188 | jstr |
| `doPositionedSound2(String)` | 204 | u8 0, then jstr |
| `doPositionedSound(CharSequence)` | 216 | u8 0, varint length, utf8 |
| `doPositionedSound3(byte[], int, int)` | 224 | raw bytes |
| `doPositionedSound32(int)` | 247 | backpatch u8 at `tint - n - 1` |
| `doPositionedSound3(int)` | 237 | backpatch u16be at `tint - n - 2` |
| `doPositionedSound3222(int)` | 674 | u8_add128 |
| `doPositionedSound32222(int)` | 679 | u8 stored as `0 - value` |
| `doPositionedSound322222(int)` | 684 | u8 stored as `128 - value` |
| `doPositionedSound32222222b3(int)` | 725 | u16be, low byte + 128 (u16be_low128) |
| `doPositionedSound32222222b5(int)` | 731 | u16le, low byte + 128 (u16le_low128) |
| `doPositionedSound22b(int)` | 719 | u16le |
| `doPositionedSound2223(int)` | 777 | u24le |
| `doPositionedSound223b(int)` | 797 | u32le |
| `doPositionedSound2(int[], int, int)` | 595 | XTEA encrypt, delta `-1640531527`, 32 rounds, big-endian blocks |
| `doPositionedSound(int[], int, int)` | 620 | XTEA decrypt, same delta, 32 rounds |
| `doPositionedSound(BigInteger, BigInteger)` | 645 | replaces the buffer with u16be cipher length and the cipher bytes |

Reads:

| method | line | width |
| --- | --- | --- |
| `doPositionedSound3()` | 315 | u8 |
| `doPositionedSound32()` | 320 | i8 |
| `doPositionedSound322()` | 325 | u16be |
| `doPositionedSound4()` | 331 | i16be |
| `doPositionedSound3222()` | 342 | u24be |
| `doPositionedSound2322()` | 348 | i24be |
| `doPositionedSound42()` | 359 | u32be |
| `doPositionedSound422()` | 365 | u32le |
| `doPositionedSound4222b()` | 371 | u40be (u8 then u32be) |
| `doPositionedSound4222b3()` | 378 | u48be (u16be then u32be) |
| `doPositionedSound4222b8()` | 385 | u64be |
| `doPositionedSound32222()` | 424 | jstr |
| `doPositionedSound3222222()` | 435 | u8 must be 0, then jstr |
| `doPositionedSound4222b9()` | 414 | u8 0 means null, else jstr |
| `doPositionedSound2222()` | 476 | signed smart: u8-64, or u16be-49152 |
| `doPositionedSound32222222b4()` | 482 | unsigned smart: u8, or u16be-32768 |
| `doPositionedSound32222222b4b()` | 512 | if next byte < 0 then u32be masked with `0x7fffffff`, else u16be, and 32767 becomes -1 |
| `doPositionedSound32222222b3()` | 507 | if next byte < 0 then u32be masked with `0x7fffffff`, else u16be |
| `doPositionedSound22222()` | 522 | varint |
| `doPositionedSound32222222b8()` | 689 | u8_add128 |
| `doPositionedSound32222222b9()` | 694 | u8_neg |
| `doPositionedSound32222222b10()` | 699 | u8_sub128 |
| `doPositionedSound32222222b16()` | 737 | u16le |
| `doPositionedSound32222222b17()` | 743 | u16be, low byte - 128 |
| `doPositionedSound32222222b15()` | 749 | u16le, low byte - 128 |
| `doPositionedSound322222()` | 821 | u32le |
| `doPositionedSound32222222b14()` | 827 | u32, (b1) + (b0<<8) + (b3<<16) + (b2<<24) |
| `doPositionedSound4222b7()` | 833 | u32, (b2) + (b3<<8) + (b0<<16) + (b1<<24) |
| `doPositionedSound2(byte[], int, int)` | 839 | `len` bytes, stored in reverse, each minus 128 |

`doPositionedSound3222(int)` is the u8_add128 write. `doPositionedSound3222()` with no arguments is the u24be read. Call sites in this spec name which one.

## Bits

`PositionedSoundPart` (`builds/rs3/887-2017-11-01/src/src/main/java/deob/PositionedSoundPart.java`):

- `doPositionedSoundPart222` at line 54 sets the bit index to `bytePos * 8`.
- `doPositionedSoundPart2(n)` at line 59 reads `n` bits, MSB first within the current byte.
- `doPositionedSoundPart2222` at line 80 aligns to `(bitPos + 7) / 8`.
- `doPositionedSoundPart22(payloadLen)` at line 85 returns `payloadLen * 8 - bitPos`.

## Isaac

`DataBlock482.doDataBlock525` consumes one Isaac result (`builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock482.java:29`). `doDataBlock5252` peeks (`:39`). Init mixes with `-1640531527` (`:76`).

Client to server opcode: `PositionedSoundPart.doPositionedSoundPart(int)` writes `(byte)(opcode + isaac)` (`PositionedSoundPart.java:24`). Game opcodes stop at 122, so this is one byte.

Server to client opcode: `doPositionedSoundPart22` subtracts Isaac (`PositionedSoundPart.java:40`). If that byte is >= 128, a second Isaac byte is added as the low 8 bits. `doPositionedSoundPart2` peeks the first byte without consuming it (`:34`).

`ClientInvCache.doClientInvCache` writes that opcode immediately (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/client/ClientInvCache.java:34`). `RuneScapeSetupStatus.doRuneScapeSetupStatus` stores `buffer.tint` into the packet length and resets the buffer (`RuneScapeSetupStatus.java:98`). `doRuneScapeSetupStatus2` concatenates queued packets and writes the socket (`:74`).

Login builds two ciphers from the same four seeds. The outgoing cipher keeps the seeds. Each seed is then increased by 50 and the incoming cipher is built from those (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/camera/CameraManager.java:521`). The incoming cipher is bound onto the connection buffer (`:530`). The client adds Isaac on the way out and subtracts it on the way in.

## Variable length

`Statics.doStatics(RuneScapeSetupStatus)` at `builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:3902`:

- No current opcode: read one Isaac byte, and a second if the peek is >= 128. Index `DataBlock832.doDataBlock825()`. Store `tint2` as the size (`:3932`).
- Size `-1`: read one raw u8. That value replaces the size (`:3942`). It is not Isaac.
- Size `-2`: read u16be with `doPositionedSound322` (`:3953`). It is not Isaac.
- Size `> 0`: read that many payload bytes (`:3965`).
- Size `0`: the payload step is skipped.

The length byte or short is not part of the counted payload. Previous opcodes are kept on `cls9a1483c84`, `cls9a1483c83`, and `cls9a1483c82` (`:3976`).

An opcode with no switch arm hits the final else, which logs the opcode and calls `Client.loggedout` (`Statics.java:6847`). Opcode 185 is in that set.

## Size tables

Copied from `DataBlock552` and `DataBlock832` constructors. Compared with `rslib/src/client/packets/887.c` and `rslib/src/server/packets/887.c`: every opcode this client constructs has the same size. Opcodes past the end of each Java table are absent here. RSLib stores `-3` for those. That is not a field layout.

`DataBlock832.doDataBlock825` lists the server constants in constructor order (`DataBlock832.java:399`), so the array index is the opcode.

### C2S `DataBlock552`

| opcode | size |
| --- | --- |
| 0 | 0 |
| 1 | 3 |
| 2 | -1 |
| 3 | 11 |
| 4 | 8 |
| 5 | -1 |
| 6 | 1 |
| 7 | 4 |
| 8 | 4 |
| 9 | 7 |
| 10 | -2 |
| 11 | -1 |
| 12 | -1 |
| 13 | 3 |
| 14 | -2 |
| 15 | 3 |
| 16 | 1 |
| 17 | -1 |
| 18 | -1 |
| 19 | 1 |
| 20 | 4 |
| 21 | -1 |
| 22 | 8 |
| 23 | 2 |
| 24 | 8 |
| 25 | 9 |
| 26 | 3 |
| 27 | -1 |
| 28 | 0 |
| 29 | 2 |
| 30 | 1 |
| 31 | 3 |
| 32 | 7 |
| 33 | 6 |
| 34 | -1 |
| 35 | -2 |
| 36 | 3 |
| 37 | 7 |
| 38 | 8 |
| 39 | -1 |
| 40 | 9 |
| 41 | 18 |
| 42 | 9 |
| 43 | 3 |
| 44 | 4 |
| 45 | 16 |
| 46 | 9 |
| 47 | 3 |
| 48 | -1 |
| 49 | 9 |
| 50 | 3 |
| 51 | 9 |
| 52 | -1 |
| 53 | 0 |
| 54 | -1 |
| 55 | 8 |
| 56 | 3 |
| 57 | 3 |
| 58 | 3 |
| 59 | 4 |
| 60 | 0 |
| 61 | -1 |
| 62 | -1 |
| 63 | -2 |
| 64 | -2 |
| 65 | 2 |
| 66 | 4 |
| 67 | -1 |
| 68 | -1 |
| 69 | 15 |
| 70 | -2 |
| 71 | 3 |
| 72 | 15 |
| 73 | 17 |
| 74 | 3 |
| 75 | -1 |
| 76 | -2 |
| 77 | -1 |
| 78 | 8 |
| 79 | 3 |
| 80 | -1 |
| 81 | 0 |
| 82 | 11 |
| 83 | -1 |
| 84 | 4 |
| 85 | -1 |
| 86 | 16 |
| 87 | 0 |
| 88 | 8 |
| 89 | 5 |
| 90 | 4 |
| 91 | 6 |
| 92 | 12 |
| 93 | 7 |
| 94 | -2 |
| 95 | 8 |
| 96 | 1 |
| 97 | 4 |
| 98 | 8 |
| 99 | -1 |
| 100 | 4 |
| 101 | 9 |
| 102 | -1 |
| 103 | -1 |
| 104 | 9 |
| 105 | 5 |
| 106 | 8 |
| 107 | -1 |
| 108 | 4 |
| 109 | 7 |
| 110 | 6 |
| 111 | -1 |
| 112 | 3 |
| 113 | -2 |
| 114 | 3 |
| 115 | -1 |
| 116 | -2 |
| 117 | 7 |
| 118 | 7 |
| 119 | 18 |
| 120 | -2 |
| 121 | -2 |
| 122 | 1 |

### S2C `DataBlock832`

| opcode | size |
| --- | --- |
| 0 | 5 |
| 1 | -2 |
| 2 | 19 |
| 3 | -1 |
| 4 | -2 |
| 5 | 10 |
| 6 | 19 |
| 7 | -1 |
| 8 | -1 |
| 9 | -1 |
| 10 | 8 |
| 11 | -1 |
| 12 | 0 |
| 13 | -2 |
| 14 | 10 |
| 15 | 0 |
| 16 | 4 |
| 17 | 10 |
| 18 | 0 |
| 19 | 11 |
| 20 | 9 |
| 21 | 7 |
| 22 | -1 |
| 23 | -2 |
| 24 | 3 |
| 25 | -2 |
| 26 | 6 |
| 27 | 6 |
| 28 | 0 |
| 29 | 6 |
| 30 | -1 |
| 31 | 21 |
| 32 | 1 |
| 33 | 21 |
| 34 | -2 |
| 35 | 2 |
| 36 | 3 |
| 37 | 5 |
| 38 | 6 |
| 39 | 4 |
| 40 | 8 |
| 41 | -2 |
| 42 | 1 |
| 43 | -2 |
| 44 | 6 |
| 45 | 10 |
| 46 | 2 |
| 47 | -2 |
| 48 | -1 |
| 49 | 1 |
| 50 | 2 |
| 51 | 6 |
| 52 | 3 |
| 53 | 3 |
| 54 | 0 |
| 55 | 0 |
| 56 | 6 |
| 57 | -2 |
| 58 | 29 |
| 59 | 2 |
| 60 | -1 |
| 61 | 8 |
| 62 | 0 |
| 63 | 8 |
| 64 | 4 |
| 65 | 32 |
| 66 | 1 |
| 67 | 0 |
| 68 | 0 |
| 69 | 25 |
| 70 | 8 |
| 71 | 1 |
| 72 | -2 |
| 73 | -2 |
| 74 | 0 |
| 75 | 1 |
| 76 | 4 |
| 77 | -2 |
| 78 | -1 |
| 79 | 3 |
| 80 | 1 |
| 81 | -1 |
| 82 | 18 |
| 83 | 1 |
| 84 | 1 |
| 85 | 0 |
| 86 | -2 |
| 87 | 3 |
| 88 | 4 |
| 89 | -2 |
| 90 | 9 |
| 91 | -1 |
| 92 | -1 |
| 93 | 2 |
| 94 | -2 |
| 95 | 2 |
| 96 | 12 |
| 97 | 22 |
| 98 | 5 |
| 99 | 2 |
| 100 | -2 |
| 101 | 6 |
| 102 | 8 |
| 103 | 3 |
| 104 | -2 |
| 105 | 2 |
| 106 | 10 |
| 107 | -2 |
| 108 | 5 |
| 109 | 6 |
| 110 | -1 |
| 111 | 2 |
| 112 | -2 |
| 113 | 10 |
| 114 | -2 |
| 115 | 25 |
| 116 | -1 |
| 117 | 12 |
| 118 | 6 |
| 119 | 4 |
| 120 | 10 |
| 121 | -1 |
| 122 | 3 |
| 123 | 2 |
| 124 | -2 |
| 125 | 28 |
| 126 | -2 |
| 127 | 7 |
| 128 | -2 |
| 129 | 0 |
| 130 | -1 |
| 131 | -2 |
| 132 | -2 |
| 133 | -2 |
| 134 | -2 |
| 135 | 1 |
| 136 | 3 |
| 137 | 4 |
| 138 | 3 |
| 139 | 8 |
| 140 | 1 |
| 141 | -1 |
| 142 | 4 |
| 143 | -1 |
| 144 | 2 |
| 145 | 2 |
| 146 | 1 |
| 147 | -2 |
| 148 | -1 |
| 149 | 3 |
| 150 | 3 |
| 151 | 6 |
| 152 | 5 |
| 153 | -2 |
| 154 | -2 |
| 155 | 6 |
| 156 | 1 |
| 157 | 14 |
| 158 | -2 |
| 159 | 2 |
| 160 | 4 |
| 161 | -1 |
| 162 | 4 |
| 163 | 0 |
| 164 | 6 |
| 165 | 8 |
| 166 | -1 |
| 167 | 3 |
| 168 | 9 |
| 169 | 8 |
| 170 | 0 |
| 171 | 8 |
| 172 | 7 |
| 173 | -2 |
| 174 | 8 |
| 175 | 0 |
| 176 | -2 |
| 177 | 6 |
| 178 | 23 |
| 179 | 5 |
| 180 | -1 |
| 181 | 10 |
| 182 | -1 |
| 183 | 8 |
| 184 | 2 |
| 185 | -2 |
| 186 | 1 |
| 187 | 10 |
| 188 | 6 |
| 189 | 0 |
| 190 | 6 |
| 191 | 5 |
| 192 | 3 |
| 193 | -1 |
