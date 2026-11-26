# Framing

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`.

## Buffers

`ObjStackNode` is the byte buffer. `valueObjStackNode66` is the array. `valueObjStackNode65` is the position.

`Cd2e07e2c` extends it and holds the ISAAC `LinkedList` plus a bit position `valuekind50310`.

Incoming game buffer: `TexturesChargBes.valuekind4792`. Outgoing game buffer: `SoftReferenceNode.valueSoftReferenceNode170`. Login outer buffer: `TitelbildGeOffnet.valuekind5322`.

## ISAAC

Server opcode: `opkind5033` is `(byte - nextIsaac) & 255` (`Cd2e07e2c.java:88-89`).

Client opcode: `opkind5031` stores `(opcode + nextIsaac)` as a byte (`Cd2e07e2c.java:80-81`).

Payload writes and reads are plain, except `opkind5035`, which subtracts an ISAAC word from each payload byte (`Cd2e07e2c.java:100-103`).

Seeds are built in login state 3. The outgoing cipher is seeded with the four ints. Each int then increases by 50, and that array seeds the incoming cipher (`ObjStack.java:307-313`). See [login.md](login.md).

XTEA is `ObjStackNode.opObjStackNode42` (`ObjStackNode.java:188-208`), delta `-1640531527`, used when opening a JS5 container, not on the game opcode. RSA is `opObjStackNode32` (`ObjStackNode.java:93-103`).

## S2C sizes

`C9389b5a8.valuekind5393` is `int[256]`, indexed by the ISAAC opcode (`C9389b5a8.java:5`, used at `DelayedStateChange.java:99`). There is no C2S `int[256]` in this client.

`0` means the payload is empty. `-1` (`~size == 0`) means the next raw `u8` is the payload length (`DelayedStateChange.java:102-109`). `-2` (`~size == 1`) means the next raw `u16be` (`opObjStackNode29`) is the payload length (`DelayedStateChange.java:112-120`). Those length bytes are not ISAAC and are not part of the counted payload. The handler then reads exactly `size` payload bytes (`DelayedStateChange.java:123-127`). The opcode byte is not included.

| opcode | size | opcode | size | opcode | size | opcode | size |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | 0 | 1 | 0 | 2 | -1 | 3 | 0 |
| 4 | 0 | 5 | 0 | 6 | 0 | 7 | 0 |
| 8 | 0 | 9 | 14 | 10 | 0 | 11 | 0 |
| 12 | 0 | 13 | 0 | 14 | 7 | 15 | 6 |
| 16 | 0 | 17 | -1 | 18 | 0 | 19 | 0 |
| 20 | 0 | 21 | 6 | 22 | 4 | 23 | 24 |
| 24 | 0 | 25 | 8 | 26 | 1 | 27 | 0 |
| 28 | 0 | 29 | 0 | 30 | 0 | 31 | 15 |
| 32 | 0 | 33 | 0 | 34 | 6 | 35 | -1 |
| 36 | 0 | 37 | 0 | 38 | 0 | 39 | 0 |
| 40 | 0 | 41 | 7 | 42 | 0 | 43 | -1 |
| 44 | 5 | 45 | 0 | 46 | 0 | 47 | 0 |
| 48 | 0 | 49 | 0 | 50 | 0 | 51 | 0 |
| 52 | 0 | 53 | 0 | 54 | 0 | 55 | 0 |
| 56 | 0 | 57 | 6 | 58 | 0 | 59 | 0 |
| 60 | 15 | 61 | 0 | 62 | 0 | 63 | 0 |
| 64 | 0 | 65 | 0 | 66 | 10 | 67 | 0 |
| 68 | 0 | 69 | 2 | 70 | -1 | 71 | 0 |
| 72 | 3 | 73 | 7 | 74 | 0 | 75 | -1 |
| 76 | 20 | 77 | 0 | 78 | 0 | 79 | 4 |
| 80 | 1 | 81 | 0 | 82 | 0 | 83 | 0 |
| 84 | 10 | 85 | 3 | 86 | 0 | 87 | 0 |
| 88 | 2 | 89 | 6 | 90 | 8 | 91 | 0 |
| 92 | 0 | 93 | -2 | 94 | 0 | 95 | 0 |
| 96 | 0 | 97 | 0 | 98 | 0 | 99 | 12 |
| 100 | 0 | 101 | 0 | 102 | 0 | 103 | 9 |
| 104 | 0 | 105 | 0 | 106 | 5 | 107 | 0 |
| 108 | 0 | 109 | 0 | 110 | 3 | 111 | 0 |
| 112 | 0 | 113 | 0 | 114 | 0 | 115 | 5 |
| 116 | 0 | 117 | 0 | 118 | 0 | 119 | -1 |
| 120 | -2 | 121 | 0 | 122 | 0 | 123 | 0 |
| 124 | 4 | 125 | -2 | 126 | 0 | 127 | 0 |
| 128 | -1 | 129 | 0 | 130 | 0 | 131 | 1 |
| 132 | 0 | 133 | 0 | 134 | 0 | 135 | 0 |
| 136 | 0 | 137 | 0 | 138 | 2 | 139 | 0 |
| 140 | 10 | 141 | 9 | 142 | 0 | 143 | -2 |
| 144 | -2 | 145 | 0 | 146 | 0 | 147 | 0 |
| 148 | 6 | 149 | 0 | 150 | 0 | 151 | -1 |
| 152 | 1 | 153 | 0 | 154 | -2 | 155 | 0 |
| 156 | 7 | 157 | 0 | 158 | 1 | 159 | 0 |
| 160 | 0 | 161 | 0 | 162 | 5 | 163 | 0 |
| 164 | 0 | 165 | 0 | 166 | 0 | 167 | 0 |
| 168 | -1 | 169 | 0 | 170 | 0 | 171 | 0 |
| 172 | 0 | 173 | 6 | 174 | 0 | 175 | 0 |
| 176 | 0 | 177 | 6 | 178 | 0 | 179 | 0 |
| 180 | 0 | 181 | 8 | 182 | 0 | 183 | 0 |
| 184 | 0 | 185 | -2 | 186 | 0 | 187 | -2 |
| 188 | 6 | 189 | 5 | 190 | 8 | 191 | 14 |
| 192 | 4 | 193 | -1 | 194 | 0 | 195 | 0 |
| 196 | 0 | 197 | 2 | 198 | 0 | 199 | 0 |
| 200 | 0 | 201 | -2 | 202 | 2 | 203 | -2 |
| 204 | 8 | 205 | 4 | 206 | -2 | 207 | 0 |
| 208 | 0 | 209 | 6 | 210 | 0 | 211 | 0 |
| 212 | 0 | 213 | 0 | 214 | 0 | 215 | 2 |
| 216 | 0 | 217 | 0 | 218 | 0 | 219 | 0 |
| 220 | -1 | 221 | 10 | 222 | 0 | 223 | 0 |
| 224 | -1 | 225 | 10 | 226 | 0 | 227 | -2 |
| 228 | 0 | 229 | 0 | 230 | 5 | 231 | 0 |
| 232 | 0 | 233 | 0 | 234 | 0 | 235 | 0 |
| 236 | -2 | 237 | 3 | 238 | 8 | 239 | 0 |
| 240 | 0 | 241 | -1 | 242 | 2 | 243 | 0 |
| 244 | 0 | 245 | 0 | 246 | 0 | 247 | 3 |
| 248 | 0 | 249 | -1 | 250 | 5 | 251 | 0 |
| 252 | 0 | 253 | 12 | 254 | 0 | 255 | 0 |

## Field tokens

Widths below are the `ObjStackNode` methods. A later page uses the token.

| Token | Method | Width |
| --- | --- | --- |
| u8 | `opObjStackNode52` / write `opObjStackNode75` | 1, unsigned |
| i8 | `opObjStackNode53` | 1, signed byte |
| i8_sub128 | `opObjStackNode54` | 1, `byte - 128` |
| u8_neg | read `opObjStackNode35` (`(-byte) & 255`); write `opObjStackNode71` | 1 |
| u8_sub128 | `opObjStackNode39` (`(byte - 128) & 255`) | 1 |
| u8_128minus | read `opObjStackNode61` (`(128 - byte) & 255`); write `opObjStackNode64` | 1 |
| u8_add128 | write `opObjStackNode30` | 1, stored `128 + value` |
| u16be | read `opObjStackNode29`; write `opObjStackNode77` | 2, big-endian |
| i16be | `opObjStackNode59` | 2, big-endian, subtract 65536 if `> 32767` |
| u16le | read `opObjStackNode65`; write `opObjStackNode74` | 2, little-endian |
| i16le | `opObjStackNode41` | 2, little-endian, subtract 65536 if `> 32767` |
| u16be_sub128lo | `opObjStackNode47` | 2, high byte then `(low - 128)` |
| u16le_sub128lo | `opObjStackNode69` | 2, `(first - 128)` then high byte |
| u16le_add128lo | write `opObjStackNode46` | 2, `(128 + low)` then high byte |
| u16be_add128lo | write `opObjStackNode49` | 2, high byte then `(128 + low)` |
| u24be | read `opObjStackNode33`; write `opObjStackNode57` | 3, big-endian |
| u24le | `opObjStackNode70` | 3, little-endian |
| u32be | read `opObjStackNode73`; write `opObjStackNode56` | 4, big-endian |
| u32le | read `opObjStackNode44`; write `opObjStackNode82` | 4, little-endian |
| u32_b1b0b3b2 | `opObjStackNode55` | 4, wire order of the int is byte1, byte0, byte3, byte2 (`ObjStackNode.java:286-288`) |
| u32_b2b3b0b1 | `opObjStackNode60` | 4, wire order byte2, byte3, byte0, byte1 (`ObjStackNode.java:321-323`) |
| u32_w45 | write `opObjStackNode45` | 4, bytes `(v>>8), v, (v>>24), (v>>16)` (`ObjStackNode.java:223-227`) |
| u32_w76 | write `opObjStackNode76` | 4, bytes `(v>>16), (v>>24), v, (v>>8)` (`ObjStackNode.java:411-415`) |
| u64be | read `opObjStackNode78`; write `opObjStackNode40` | 8, two big-endian ints |
| jstr | read `opObjStackNode38`; write `opObjStackNode72` | bytes up to a 0 terminator |
| gjstr2 | `call76b99ad9` | version `u8` must be 0, then jstr (`ObjStackNode.java:149-160`) |
| jstr_or_null | `opObjStackNode37` | if the current byte is 0, skip it and return null, else jstr |
| smart | `opObjStackNode31` | `u8` if `< 128`, else `u16be - 32768` |
| smart_small | `opObjStackNode34` | `u8 - 64` if `< 128`, else `u16be - 49152` |
| varint | read `opObjStackNode36`; write `opObjStackNode66` | 7-bit groups, high bit continues |
| bigsmart | `opObjStackNode51` | smart values, add 32767 while the smart is 32767 |
| bytes(n) | read `opObjStackNode63`; write `opObjStackNode43` | n raw bytes |
| bytes_sub128(n) | `opObjStackNode48` | n bytes, each minus 128 |
| bits(n) | `opkind5037` | n bits, MSB within the byte (`Cd2e07e2c.java:107-125`) |
| bit_start | `opAbstractWorld1` | bit position = `8 * pos` |
| bit_align | `opkind5038` | pos = `(bitpos + 7) / 8` |
| huffstr | `Cd2e07e2c.call7d8dba53` | smart length (capped at 32767), then `BrokenOutputStream.valueBrokenOutputStream53.opMaterialManager_Class106_Sub2_Sub112` consumes the compressed bytes (`Cd2e07e2c.java:23-32`) |

`opkind5034(n)` is remaining bits: `n * 8 - bitpos` (`Cd2e07e2c.java:92-93`).

A `~expr == -N` test in this decompiler means the value is `N - 1`.
