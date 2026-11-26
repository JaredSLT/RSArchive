# Framing

There is no opcode size table and no ISAAC. `Stream.overallPacketSizes` and `OVERALL_PACKET_COUNTER` are traffic counters, incremented inside `encryptPacket` only when `maxPacketLength <= 10000` (`jagex/Stream.java:56`, `jagex/Stream.java:332`). They are not a decode table. Every game packet size is the framed length.

`Stream.number640` is 0 on the live path, so the `if (var6 != 0)` heads in `gBit` and `pdata` are skipped. The loops under them still run (`jagex/Tools.java:94`, `jagex/Stream.java:205`).

## Widths

| name | meaning | source |
| --- | --- | --- |
| u8 | one byte, unsigned `& 255` | `Stream.g1` / `p1` `jagex/Stream.java:105`, `jagex/Stream.java:175` |
| u16be | big-endian | `g2` / `p2` `jagex/Stream.java:110`, `jagex/Stream.java:180` |
| u32be | big-endian | `g4` / `p4` `jagex/Stream.java:117`, `jagex/Stream.java:185` |
| u64be | two u32be, high word first | `p8` `jagex/Stream.java:194`, `Tools.g8` `jagex/Tools.java:74` |
| i16be | `g2`, then subtract 65536 when the value is greater than 32767 | `Tools.g2s` `jagex/Tools.java:79` |
| i8 | a Java `byte` added to a coordinate | packet handlers in `MudClient.call5fac7c41` |
| gsmart | if `(b0 & 255) < 128`, that byte as a Java `byte` (0..127). Else 4 bytes: `((b0 & 255) - 128) << 24` plus the next three big-endian bytes | `Tools.gsmart` `jagex/Tools.java:89` |
| rawstr | `pjstr` copies the Java string bytes with no length and no terminator | `jagex/Stream.java:200` |
| pdata | raw bytes, caller supplies the length | `jagex/Stream.java:206` |
| rsa | see [login](login.md). Each 7 plaintext chars become `u8` block length plus that many bytes | `jagex/Stream.java:218` |
| bits(n) | `Tools.gBit`. MSB first inside the byte. The call does not advance the caller's bit offset. Masks are `Tools.BITMASK` | `jagex/Tools.java:16`, `jagex/Tools.java:94` |

`ClientConfig.g1`, `g2`, and `g4` read `integer.dat` with the same big-endian `Tools` helpers. `g4` replaces a value above 99999999 with `99999999 - value`. `gjstr` reads `string.dat` until a 0 byte and consumes that 0. On the live path an immediate 0 returns an empty string (`deob/ClientConfig.java:220`).

## C2S

`p1spooky(opcode, threat)` stores the raw opcode at `data[start + 2]`, stores 0 at `start + 3`, and sets `pos` to `start + 3` (`jagex/Stream.java:273`). The initial `pos` is 3 (`jagex/Stream.java:50`). The threat is not a payload byte and not a size. It feeds the cipher below.

`encryptPacket` (`jagex/Stream.java:305`):

1. `data[start + 2] = (byte)(opcode + encodeKey)`.
2. `writeThreatIndex = (writeThreatIndex + threat) % THREAT_LEN`.
3. `encodeKey = (encodeKey * 3 + copyrightChar + threat) & 0xffff`.
4. `encodeKey` starts at 3141592 (`jagex/Stream.java:12`).
5. The copyright character is `charAt(writeThreatIndex)` of the string literal at `jagex/Stream.java:54`. `THREAT_LEN` is that string's length (`jagex/Stream.java:54`).
6. If `packetAlignment != 8`, increment `pos` first. `p1spooky` sets `packetAlignment` to 8, and the writers read here do not change it.
7. `length = pos - start - 2`. That count includes the opcode byte.
8. If `length >= 160`: `data[start] = 160 + length / 256`, `data[start + 1] = length & 255`.
9. Else: `data[start] = length`, decrement `pos`, and move the last payload byte to `data[start + 1]`.

`flush` calls `encryptPacket` then `writePacket(0)` (`jagex/Stream.java:348`). Heartbeat calls `encryptPacket` and later `writePacket(20)` (`jagex/client/Client.java:338`). `writePacket` writes the buffer only after `writeTries` reaches its argument (`jagex/Stream.java:354`).

`ClientStream.write` copies into a 5000-byte ring. It throws `buffer overflow` when `bufPos` meets `(bufLen + 4900) % 5000` (`jagex/client/ClientStream.java:100`). `ClientStream.run` is a fernflower stub (`jagex/client/ClientStream.java:130`). See [gaps](gaps.md).

## S2C

`readPacket` (`jagex/Stream.java:129`):

- If `length == 0` and at least 2 bytes are available, read one length byte. If that byte is `>= 160`, `length = (byte - 160) * 256 + next byte`.
- If `length >= 160`, read `length` bytes straight into the buffer.
- Else read one byte into `buffer[length - 1]`, and if `length > 1` read `length - 1` bytes at the start.
- Return the payload length. Byte 0 of that payload is the still-encoded opcode.

`Client.callb6b49066` passes `in[0] & 255` and that length to `call7e894e6a` (`jagex/client/Client.java:349`). `g1spooky` (`jagex/Stream.java:295`):

```text
decoded = (raw - decodeKey) & 255
threatAdd = SERVERPROT_ENCRYPTION[decoded]
readThreatIndex = (readThreatIndex + threatAdd) % THREAT_LEN
decodeKey = (decodeKey * 3 + copyrightChar + threatAdd) & 0xffff
```

`decodeKey` starts at 3141592 (`jagex/Stream.java:16`). The returned value is `decoded`. The table is the `int[]` at `jagex/client/Client.java:695`. Login, new player, getpq, and recover responses use this same `g1spooky` on a bare byte. Those four are not `readPacket` frames. See [login](login.md).

Game opcodes other than 8, 9, 10, 23, 24, 26, 27, and 28 go to `size`, which `MudClient` overrides as `call5fac7c41` (`jagex/client/Client.java:386`, `deob/MudClient.java:5765`).
