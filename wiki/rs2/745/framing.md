# Framing

Game packet sizes are constructor pairs, not a later array. C2S is `C10c700cf(opcode, size)` with fields `valuekind3370108` and `valuekind337051` (`C10c700cf.java:112-118`). S2C is `C8db7d40a(opcode, size)` with fields `valuekind337449` and `valuekind3374162` (`C8db7d40a.java:169-172`). `C8db7d40a.opkind33741` returns the constants in opcode order 0 through 159 (`C8db7d40a.java:165`).

`rslib/src/client/packets/745.c` and `rslib/src/server/packets/745.c` repeat those constructor sizes and use `-3` past the last opcode. The sizes agree. Those files also bind names. The names are not field lists.

## Size meaning

`-1` is a following unsigned byte. `-2` is a following unsigned big-endian short (`Client.opClient457`, `Client.java:6663-6683`). After the length is known, if it is greater than 0 the client sets the buffer offset to 0 and copies that many payload bytes over the start of the buffer (`Client.java:6686-6692`). The length prefix is not part of the handler payload. A size of 0 has no payload bytes.

## ISAAC

Outbound game opcode: one byte, `opcode + Isaac.opkind36991()` (`C30153e1d.java:14-16`). `ClientMessage.opkind39491` writes that byte before the payload (`ClientMessage.java:33-34`). Inbound opcode: one byte minus `Isaac.opkind36991()`. If that byte is at least 128, a second byte is subtracted from the next cipher word and the opcode is `((first - 128) << 8) + second` (`C30153e1d.java:18-25`). Peek uses `opkind36992`, which does not consume (`Isaac.java:28-34`, `C30153e1d.java:18-20`).

`Isaac.opkind36991` returns the next word from a 256-word block (`Isaac.java:19-26`). The mixer `opkind36993` was not transcribed.

After a successful login the outbound cipher is `new Isaac(seeds)` and the inbound cipher is `new Isaac` of those seeds each plus 50. A third `new Isaac` of the mutated seeds is constructed and discarded. The inbound buffer is then bound to the inbound cipher (`Statics.java:2371-2380`, `C82f912d1.java:138-147`).

`opkind41388` copies `n` bytes each minus the next cipher word (`C30153e1d.java:28-32`). Opcode 0 uses that on its whole payload.

Login handshake buffers from `ClientMessage.opkind39493` do not write an ISAAC opcode (`ClientMessage.java:39-44`). Opcode 26 on the login socket is the exception: it installs the outbound cipher and writes `opkind41381` (`Statics.java:2467-2468`).

## Bit access

`opkind41387` sets the bit cursor to `offset * 8` (`C30153e1d.java:35-37`). `opkind41383(n)` reads `n` bits, most significant bit first inside the current byte: the first bit available in a byte is `8 - (bitpos & 7)` (`C30153e1d.java:39-56`). `opkind41384` byte-aligns with `(bitpos + 7) / 8` (`C30153e1d.java:59-61`). `opkind41385(n)` is `n * 8 - bitpos` (`C30153e1d.java:63-65`). The mask table is the usual `2^n - 1` through 32 bits (`C30153e1d.java:2`).

## C2S flush

`ClientMessage.opkind39491` picks a buffer of 260 bytes when the size is `-1`, 10000 when `-2`, 20 when the size is at most 18, 100 when at most 98, otherwise 260 (`ClientMessage.java:21-31`). The caller writes any length prefix. `opkind397415` backpatches a big-endian short at `offset - length - 2` (`C8613fa98.java:192-195`). `opkind397416` backpatches one byte at `offset - length - 1` (`C8613fa98.java:197-199`). `C2c0360c3.opkind37672` records `valuekind39493` as the write offset and queues the message (`C2c0360c3.java:61-66`). `opkind37671` concatenates those bytes and writes them (`C2c0360c3.java:38-56`).

`Client.valueClient1829` is the game connection and `Client.valueClient1828` is the other connection. `opClient600` returns the lobby buffer when `opClient448(valueClient1760)` is true, otherwise the game buffer. That picker was not opened; the two fields are the connections used by the writers below.

## Widths

All of these are `C8613fa98`.

| Method | Width |
| --- | --- |
| `opkind39744` / `opkind397418` | u8 write / u8 read (`77-79`, `231-233`) |
| `opkind397419` | i8 (`235-237`) |
| `opkind397445` / `opkind397446` | u8neg write `0 - v` / read (`525-527`, `537-539`) |
| `opkind397470` / `opkind397467` | u8sub128 write `128 - v` / read (`529-531`, `541-543`) |
| `opkind397444` / `opkind397471` | u8add128 write `v + 128` / read `byte - 128` (`521-523`, `533-535`) |
| `opkind397448` | i8neg `0 - byte` (`549-551`) |
| `opkind397447` | i8add128 `byte - 128` (`545-547`) |
| `opkind397449` | i8sub128 `128 - byte` (`553-555`) |
| `opkind39745` / `opkind397420` | u16be (`81-84`, `239-242`) |
| `opkind397463` | i16be (`244-252`) |
| `opkind397477` / `opkind397450` / `opkind397453` | u16le write high-second / read (`86-89`, `557-560`, `572-575`) |
| `opkind397451` / `opkind397473` | u16be_add128: high byte, then low+128 (`562-565`, `577-580`) |
| `opkind397452` / `opkind397474` | u16le_add128: low+128, then high byte (`567-570`, `582-585`) |
| `opkind39747` / `opkind397422` | u24be (`91-95`, `254-257`) |
| `opkind397432` | i24be (`259-267`) |
| `opkind397458` | u24 as `b1 + (b0 << 16) + (b2 << 8)` (`619-622`) |
| `opkind39748` / `opkind397423` | u32be (`97-102`, `269-272`) |
| `opkind39749` / `opkind397459` | u32le (`104-109`, `624-629`) |
| `opkind397476` | u32 write order `>>8, low, >>24, >>16` (`631-636`) |
| `opkind397460` | u32 write order `>>16, >>24, low, >>8` (`638-643`) |
| `opkind397430` and `opkind397424` | u32 read `b0 + (b1 << 8) + (b3 << 24) + (b2 << 16)` (`645-648`, `274-277`) |
| `opkind397461` | u32 read `b1 + (b0 << 8) + (b3 << 16) + (b2 << 24)` (`650-653`) |
| `opkind397421` | u32 read `b2 + (b3 << 8) + (b0 << 16) + (b1 << 24)` (`655-658`) |
| `opkind397472` / `opkind397469` | u64be. The read is two `opkind397423` (`119-128`, `285`) |
| `opkind397412` / `opkind397464` | jstr: Cp1252 bytes and a `0` terminator (`146-154`, `326-334`) |
| `opkind397431` | vjstr: a leading `0`, then the same jstr. Any other lead byte throws (`336-348`) |
| `opkind397414` / `opkind397433` | raw bytes (`178-182`, `367-370`) |
| `opkind397462` | bytes each minus 128 (`660-664`) |
| `opkind397434` | smart: u8 if the next byte is below 128, otherwise u16be minus U+8000 (`379-382`). The source literal is `耀` |
| `opkind397438` | bigsmart: if the next byte is negative, u32be with the high bit cleared, else u16be, and 32767 means -1 (`405-412`) |
| `opkind397435` | u8 minus 1, or u16be minus U+8001, chosen the same way as smart (`384-387`). The source literal is `老` |
| `opkind397466` | u8 minus 64, or u16be minus U+C000 (`374-377`). The source literal is `쀀`, and `MapElement.java:486` compares that literal with `49152` |
| `opkind397441` | XTEA encrypt of `[start, end)` in 8-byte blocks, sum starts at 0, delta `-1640531527`, 32 rounds (`447-469`) |
| `opkind397440` | XTEA decrypt of the whole buffer, sum starts at `-957401312`, same delta (`425-444`) |
| `opkind39746` | XTEA decrypt of a range, same start sum (`471-492`) |
| `opkind397442` | RSA `modPow`, then rewrite as u16be length plus the byte array (`495-506`) |

`opkind397410` writes five bytes and casts `value >> 32` through `int` before the byte store (`111-117`). That is not a full 40-bit big-endian store. No packet row below uses it.

## Game size tables

C2S `opcode:size` from `C10c700cf.java:2-111`:

```text
0:16 1:12 2:-1 3:7 4:1 5:4 6:2 7:-1 8:-1 9:6 10:3 11:-2 12:3 13:16 14:8 15:3
16:8 17:0 18:0 19:4 20:0 21:3 22:-2 23:1 24:9 25:9 26:-1 27:7 28:7 29:7 30:8
31:-1 32:-1 33:-1 34:1 35:15 36:4 37:-1 38:8 39:8 40:9 41:8 42:-1 43:15 44:6
45:-1 46:8 47:9 48:2 49:4 50:6 51:17 52:-1 53:-1 54:-1 55:3 56:-2 57:8 58:4
59:7 60:3 61:-1 62:3 63:2 64:3 65:1 66:-1 67:4 68:9 69:3 70:-1 71:-1 72:5
73:9 74:4 75:-1 76:11 77:3 78:18 79:-1 80:3 81:15 82:3 83:-1 84:-2 85:11
86:-1 87:3 88:7 89:3 90:-1 91:2 92:-1 93:-2 94:9 95:0 96:3 97:7 98:8 99:3
100:1 101:-2 102:8 103:4 104:4 105:-2 106:4 107:-1 108:3 109:-2
```

S2C `opcode:size` from `C8db7d40a.java:2-161`:

```text
0:-2 1:-2 2:-2 3:28 4:-1 5:-1 6:-2 7:0 8:10 9:6 10:8 11:1 12:18 13:-1 14:2
15:8 16:19 17:0 18:6 19:4 20:-2 21:10 22:-1 23:6 24:-2 25:1 26:5 27:7 28:0
29:6 30:2 31:-2 32:-2 33:8 34:2 35:4 36:-1 37:3 38:-1 39:4 40:-1 41:2 42:2
43:-1 44:1 45:6 46:-2 47:6 48:0 49:6 50:-1 51:-1 52:-1 53:6 54:14 55:-2 56:-2
57:6 58:2 59:-2 60:4 61:12 62:-2 63:11 64:1 65:0 66:0 67:8 68:3 69:0 70:-2
71:3 72:-2 73:8 74:8 75:4 76:9 77:5 78:-2 79:8 80:29 81:19 82:1 83:0 84:-1
85:10 86:6 87:-1 88:3 89:23 90:1 91:-2 92:5 93:9 94:-2 95:6 96:0 97:6 98:-1
99:-2 100:2 101:0 102:6 103:4 104:0 105:0 106:3 107:10 108:4 109:5 110:10
111:9 112:10 113:5 114:-1 115:4 116:6 117:-2 118:-1 119:-2 120:-2 121:8 122:-2
123:-2 124:2 125:2 126:32 127:25 128:2 129:-2 130:1 131:-1 132:10 133:8 134:20
135:-1 136:3 137:1 138:-2 139:-1 140:5 141:22 142:8 143:-2 144:-2 145:-1
146:21 147:4 148:1 149:0 150:8 151:6 152:-1 153:7 154:3 155:3 156:12 157:2
158:25 159:1
```
