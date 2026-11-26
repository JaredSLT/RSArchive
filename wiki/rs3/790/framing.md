# Framing

## Size tables

`DataBlock1959.tint` is the client-to-server opcode and `tint2` is the size. The constructor stores both (`deob/DataBlock1959.java:248`). Opcodes 0 through 118, in order:

```text
-1,-2,4,-1,-1,8,6,-1,-1,11,3,-2,4,8,3,8,0,-1,-2,4,-1,0,8,8,9,2,4,-1,9,-1,15,8,-1,5,9,-1,-1,-1,-1,15,4,9,9,3,17,-2,-1,8,-1,3,3,3,11,-1,-2,6,7,0,1,6,1,9,18,12,7,4,3,-1,7,8,2,1,9,3,-2,3,-1,-1,-1,4,3,3,-1,3,7,-1,0,3,-1,4,3,8,1,4,16,16,4,-1,15,-2,3,2,-1,-1,3,0,0,1,-2,3,-2,8,9,-2,2,7,7,-1,7
```

`DataBlock1901.doDataBlock1901` returns the server-to-client values in opcode order, and the array index is the opcode (`deob/DataBlock1901.java:361`). The constructor stores opcode and size (`DataBlock1901.java:365`). Opcodes 0 through 174, in order:

```text
-2,4,4,0,28,10,8,3,-2,-1,-2,-2,6,3,2,-1,4,2,25,0,0,8,-1,32,1,1,6,10,1,21,10,7,5,25,-1,2,-2,18,1,-2,11,2,2,1,-2,-1,10,10,-1,6,3,-1,-2,20,2,6,4,1,0,4,6,9,-2,-1,-1,22,-2,-2,12,0,0,2,8,8,2,1,-1,8,0,14,0,19,0,5,-2,3,3,3,1,6,8,4,-2,6,6,5,8,-2,8,0,12,10,2,-2,2,8,-2,3,-2,-2,2,5,6,0,-1,7,6,6,-1,0,29,8,6,-1,2,-1,-2,6,4,6,-1,2,0,7,3,3,3,-2,10,19,-2,5,-2,3,8,4,-2,4,-1,-1,23,0,0,-2,1,10,-1,-2,-1,-2,-2,-2,-2,9,-1,-2,2,10,1,-1,9,4,6,1,-1
```

A size of `-1` is one raw unsigned length byte. A size of `-2` is a raw unsigned 16-bit big-endian length. The length counts the payload and leaves out the length field. Compared with `rslib/src/client/packets/790.c` and `rslib/src/server/packets/790.c`, every opcode 0 through 255 agrees: present Java sizes match, and opcodes with no constructor are `-3` in RSLib.

`DataBlock2081` is the pre-ISAAC login and JS5 opcode list. Its constructor stores only the opcode (`deob/DataBlock2081.java:46`). Those numbers also appear in `DataBlock1959`. [packets.md](packets.md) keeps both rows and distinguishes them by the handler column.

## Outbound buffer

`DataBlock2435.doDataBlock2435(DataBlock1959, SoundBackendConfig)` (`deob/DataBlock2435.java:31`) picks the buffer from `tint2`: `-1` allocates 260, `-2` allocates 10000, a size at most 18 allocates 20, a size at most 98 allocates 100, and any other size allocates 260. It then writes the ISAAC opcode with `preferencesCpuUsage.doPreferencesCpuUsage(opcode)` (`DataBlock2435.java:48`).

`doDataBlock24352` (`DataBlock2435.java:54`) builds a 5000-byte buffer with `cls34becc0c` left null. Login writes use that method, so the login opcode byte is raw.

`PureJavaToolkitFactory.doPureJavaToolkitFactory(DataBlock2435)` (`com/jagex/graphics/safe/PureJavaToolkitFactory.java:99`) queues the packet, sets `tint` to the bytes written, and resets the stream offset. `doPureJavaToolkitFactory2` (`PureJavaToolkitFactory.java:75`) copies `preferencesCpuUsage.byteArray` from 0 through `tint` onto the socket. That copy adds no extra header.

`Client.pureJavaToolkitFactory2` is the socket walk and the in-game pump use. `Client.pureJavaToolkitFactory` is the other socket. Lobby entry sets `Statics.pureJavaToolkitFactory` to `pureJavaToolkitFactory2` with `tint70` 285 (`deob/DataBlock1937.java:199`). Game entry sets it to `pureJavaToolkitFactory` with `tint70` 179 (`DataBlock1937.java:174`).

## ISAAC

`SoundBackendConfig` (`com/jagex/audio/backend/SoundBackendConfig.java:20`) is the cipher. `doSoundBackendConfig` consumes the next result (`SoundBackendConfig.java:31`). `doSoundBackendConfig2` peeks it (`SoundBackendConfig.java:41`).

`PreferencesCpuUsage` extends `PositionedSound` and is the stream that adds or subtracts those results.

- Outbound opcode byte: `value + doSoundBackendConfig()` (`com/jagex/game/clientoptions/options/PreferencesCpuUsage.java:27`).
- Inbound opcode: one byte minus `doSoundBackendConfig()`. When that value is at least 128, a second byte is subtracted and the opcode is `((first - 128) << 8) + second` (`PreferencesCpuUsage.java:38`).
- The peek `doPreferencesCpuUsage` (`PreferencesCpuUsage.java:31`) uses `doSoundBackendConfig2`.

Length bytes are copied raw (`rs2/client/Client.java:7496`, `Client.java:7507`). After the login block, `Statics.jagtheora` installs `soundBackendConfig` from the four seeds and `soundBackendConfig2` from those seeds plus 50, and the stream uses `soundBackendConfig2` (`deob/Statics.java:2891`).

## Inbound loop

`Client.doClient23(PureJavaToolkitFactory)` (`Client.java:7451`) reads one packet.

1. If the peek says the opcode needs a second byte, read one more byte (`Client.java:7470`).
2. `doPreferencesCpuUsage2` consumes the opcode and indexes `DataBlock1901.doDataBlock1901()` (`Client.java:7481`).
3. Size `-1`: one raw `u8` (`Client.java:7491`).
4. Size `-2`: raw `u16be` through `doPositionedSound232` (`Client.java:7502`).
5. When the payload length is greater than 0, read that many raw bytes (`Client.java:7514`).

`doClient2222b192` calls this up to 100 times on `pureJavaToolkitFactory2` (`Client.java:3209`). The first packet after login forces a `u16be` length in the login state machine. That rule is in [login.md](login.md).

## Variable length on the way out

A size of `-1` writes a placeholder byte and `PositionedSound.doPositionedSound232(int)` patches the byte immediately before the body with the body length (`com/jagex/game/world/entity/PositionedSound.java:238`). A size of `-2` writes a placeholder `u16be` with `doPositionedSound2(0)` and `doPositionedSound23(int)` patches those two bytes as the big-endian body length (`PositionedSound.java:232`). Chat opcode 17 and private-message opcode 11 use those patches (`com/jagex/game/client/MouseBinding.java:6793`, `MouseBinding.java:6811`).

## Integers

Writes on `PositionedSound`:

| Name | Method | Line | Bytes |
| --- | --- | --- | --- |
| `u8` | `doPositionedSound(int)` | 94 | one byte |
| `u16be` | `doPositionedSound2(int)` | 99 | high, low |
| `u16le` | `doPositionedSound2322b12(int)` | 105 | low, high |
| `u24be` | `doPositionedSound222(int)` | 111 | high, mid, low |
| `u32be` | `doPositionedSound2222(int)` | 118 | big-endian |
| `u32le` | `doPositionedSound2222b(int)` | 126 | little-endian |
| `u40be` | `doPositionedSound(long)` | 134 | five big-endian bytes |
| `u48be` | `doPositionedSound2(long)` | 143 | six big-endian bytes |
| `u64be` | `doPositionedSound22b(long)` | 153 | eight big-endian bytes |
| `u8+128` | `doPositionedSound2322b3(int)` | 619 | `value + 128` |
| `neg-u8` write | `doPositionedSound2322b11(int)` | 624 | `0 - value` |
| `128-u8` write | `doPositionedSound2322b10(int)` | 629 | `128 - value` |
| `u16le` write | `doPositionedSound2322b4(int)` | 664 | low, high |
| `u16be` low+128 | `doPositionedSound2322b5(int)` | 670 | high, low+128 |
| `u16le` low+128 | `doPositionedSound2322b8(int)` | 676 | low+128, high |
| `u32w-b` | `doPositionedSound2322b6(int)` | 755 | `>>8`, low, `>>24`, `>>16` |
| `u32w-d` | `doPositionedSound2322b9(int)` | 763 | `>>16`, `>>24`, low, `>>8` |

Reads:

| Name | Method | Line | Assembly |
| --- | --- | --- | --- |
| `u8` | `doPositionedSound2` | 275 | one byte `& 255` |
| `i8` | `doPositionedSound23` | 280 | one signed byte |
| `u16be` | `doPositionedSound232` | 285 | high, low |
| `i16be` | `doPositionedSound2322b5` | 291 | `u16be`, subtract 65536 above 32767 |
| `u24be` | `doPositionedSound2322b4` | 302 | three big-endian bytes |
| `i24be` | `doPositionedSound2322` | 308 | `u24be`, subtract 16777216 above 8388607 |
| `u32be` | `doPositionedSound2322b6` | 319 | four big-endian bytes |
| `u32le` | `doPositionedSound2322b28` | 325 | four little-endian bytes |
| `u8+u32be` | `doPositionedSound2322b8` | 331 | one `u8` then `u32be`, `u8` in bits 32..39 |
| `u64be` | `doPositionedSound2322b9` | 338 | two `u32be`, first is high |
| `u64le` | `doPositionedSound2322b10` | 345 | two `u32le`, second is high |
| `u8-128` | `doPositionedSound2322b17` | 634 | `byte - 128` |
| `neg-u8` | `doPositionedSound2322b18` | 639 | `0 - byte` |
| `128-u8` | `doPositionedSound2322b29` | 644 | `128 - byte` |
| `i8-128` | `doPositionedSound2322b30` | 649 | signed `byte - 128` |
| `neg-i8` | `doPositionedSound2322b21` | 654 | signed `0 - byte` |
| `128-i8` | `doPositionedSound2322b20` | 659 | signed `128 - byte` |
| `u16le` | `doPositionedSound2322b7` | 682 | low, high |
| `u16be-m128` | `doPositionedSound2322b22` | 688 | high, low-128 |
| `u16le-m128` | `doPositionedSound2322b32` | 694 | low-128, high |
| `i16le` | `doPositionedSound2322b23` | 700 | little-endian, signed |
| `i16be-m128` | `doPositionedSound2322b24` | 711 | high, low-128, signed |
| `i16le-m128` | `doPositionedSound2322b25` | 722 | low-128, high, signed |
| `u32scr-a` | `doPositionedSound2322b26` | 771 | `b0 + (b1<<8) + (b2<<16) + (b3<<24)` |
| `u32scr-b` | `doPositionedSound2322b3` | 777 | `b1 + (b0<<8) + (b3<<16) + (b2<<24)` |
| `u32scr-c` | `doPositionedSound2322b` | 783 | `b2 + (b3<<8) + (b0<<16) + (b1<<24)` |
| `smart-i` | `doPositionedSound222` | 436 | next byte below 128: `u8 - 64`, else `u16be - 49152` |
| `smart-u` | `doPositionedSound2322b27` | 442 | next byte below 128: `u8`, else `u16be - 32768` |
| `smart-2` | `doPositionedSound2222` | 448 | next byte below 128: `u8 - 1`, else `u16be - 32768` |
| `smart-x` | `doPositionedSound2322b13` | 454 | sum of `smart-u` values while each is 32767 |
| `smart32` | `doPositionedSound2322b14` | 467 | next byte negative: `u32be & 0x7fffffff`, else `u16be` |
| `smart32-1` | `doPositionedSound2322b15` | 472 | same, and a `u16be` of 32767 becomes -1 |
| `f32` | `doPositionedSound22` | 369 | `u32be` bits as a float |

`b0` is the first byte on the wire. `u32scr-a` has the same assembly as `u32le`.

## Bits

`doPreferencesCpuUsage22` sets the bit cursor to `byteOffset * 8` (`PreferencesCpuUsage.java:51`). `doPreferencesCpuUsage2(n)` reads `n` bits, most significant bit first within each byte (`PreferencesCpuUsage.java:56`). `doPreferencesCpuUsage222` byte-aligns with `(bitCursor + 7) / 8` (`PreferencesCpuUsage.java:77`). `doPreferencesCpuUsage22(int)` returns the bits remaining against a byte length (`PreferencesCpuUsage.java:82`).

## Strings

`CoordGrid.doCoordGrid` encodes and decodes modified Cp1252 (`com/jagex/game/shared/movement/CoordGrid.java:206`, `CoordGrid.java:283`). Codes 0 through 127 and 160 through 255 pass through. The encoder maps a fixed set of characters in the 128 through 159 range (euro sign, quotes, and the rest of that table) and writes 63 for every other character. The decoder skips 0 bytes.

| Name | Write | Read |
| --- | --- | --- |
| `jstr` | `doPositionedSound2(String)` at `PositionedSound.java:183`: encoded bytes, then 0 | `doPositionedSound2322b12` at `PositionedSound.java:384`: bytes until 0 |
| `vstr` | `doPositionedSound232(String)` at `PositionedSound.java:199`: 0, `jstr`, 0 | `doPositionedSound2322b16` at `PositionedSound.java:395`: one byte must be 0, then a `jstr` |
| `optjstr` | | `doPositionedSound2322b11` at `PositionedSound.java:374`: a leading 0 means null, otherwise a `jstr` |
| `smartstr` | `doPositionedSound(CharSequence)` at `PositionedSound.java:211`: leading 0, smart length, then `Statics.doStatics` | `doPositionedSound2322b19` at `PositionedSound.java:411`: leading 0, 7-bit varint length `doPositionedSound22222` (`PositionedSound.java:482`), then `DataBlock2513.doDataBlock2513`. That decoder was not read. |

## XTEA and RSA

`doPositionedSound(int[])` (`PositionedSound.java:494`) encrypts the whole buffer: 32 rounds, sum starts at 0, delta `-1640531527`.

`doPositionedSound2(int[])` (`PositionedSound.java:516`) decrypts the whole buffer: 32 rounds, sum starts at `-957401312`, same delta, decrypt direction. The login response uses this form.

`doPositionedSound(int[], int start, int end)` (`PositionedSound.java:540`) encrypts `(end - start) / 8` blocks in that window, with the same 32-round encrypt, then restores the write cursor. Lobby and game login call it (`Statics.java:2836`). Opcode 28 reconnect calls it (`com/jagex/game/world/WorldMapRenderer.java:228`).

`doPositionedSound2(int[], int, int)` (`PositionedSound.java:565`) is the same window with the decrypt sum.

`doPositionedSound(BigInteger, BigInteger)` (`PositionedSound.java:590`) replaces the buffer with `modPow` bytes from `BigInteger.toByteArray` and prefixes a `u16be` length.
