# Framing

Buffer class: `deob.C9cd2e2e1` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/C9cd2e2e1.java`).

## ISAAC

`Isaac.opkind1041` returns the next cipher word (`builds/rs2/350-2005-12-30/src/src/main/java/deob/Isaac.java:32`).

Inbound opcode: one byte minus `this.valueClient872.opkind1041()`, masked with 255 (`Client.java:3966`). `valueClient872` is the inbound cipher built in `Client.login` after adding 50 to each seed word (`Client.java:11404`).

Outbound opcode: `opkind872` writes `(opcode + isaac.opkind1041())` as one byte (`C9cd2e2e1.java:130`). Game packets use `valueClient739` and pass 132 as the unused second argument. The method assigns that argument to 0 before writing.

## S2C length

`Client.read` (`Client.java:3953`):

1. If `valueClient499` is -1, read one byte and subtract the inbound ISAAC word.
2. `valueClient498 = C994ec50d.valuekind1192[opcode]` (`Client.java:3972`).
3. Size `-1`: read one more `u8` length (`Client.java:3976`).
4. Size `-2`: read `u16be` (`opkind8715`) length (`Client.java:3986`).
5. Read that many payload bytes into `valueClient514` (`Client.java:4002`).

`valuekind1192` is indexed only by an opcode masked with 255. The Java array has 257 ints. Index 256 is 0 and is never used as an opcode. Copied from `builds/rs2/350-2005-12-30/src/src/main/java/deob/C994ec50d.java:5`, sixteen values per row, opcode 0 at the start. The last lone 0 is index 256.

```text
0, 4, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4
-2, 0, 0, 0, -2, 0, 5, 0, 0, 1, 0, 0, 0, 0, 2, 0
6, 0, 0, 4, 0, -2, 0, 0, 4, 0, 0, 0, 0, 0, 4, 0
0, 3, 0, 0, 0, 4, 0, 0, 8, 0, -2, 0, 2, 0, 4, 0
6, 7, 2, 0, 0, 0, 0, 2, 0, 0, 0, 6, 0, 0, 0, 0
0, 0, 0, 0, 4, 0, 4, 0, 0, -2, 0, 0, 0, 0, 0, 0
6, 0, 0, 0, 1, 0, 0, 0, 5, 0, 6, 0, 0, 0, 0, 0
0, 0, 0, 0, 0, 0, 2, 0, 0, 3, 0, 0, 0, 1, 4, 0
6, 1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0
2, 0, 15, 0, 3, 0, 3, 0, 4, 6, 0, 0, 0, 0, 6, 0
0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, -2, 0, 0, 6
-1, 0, 0, 2, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0
0, 0, 0, 0, 0, 9, 4, 0, 0, 0, 0, 0, 0, 23, -2, 0
0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, -2
0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
0, 2, -1, 2, 0, 0, 0, 0, 0, 0, 5, 3, 0, 0, 0, 14
0
```

`valuekind1191` is a second 257-int array in the same file. No method in this client reads it. Its purpose is unread. See [gaps.md](gaps.md).

There is no C2S size array in this client. `packets.md` counts C2S payload bytes from the writes after `opkind872`.

## Variable writes

`opkind8712` stores a `u8` length at `position - length - 1` (`C9cd2e2e1.java:222`). Callers write `u8` 0, remember the position, write the body, then call `opkind8712(position - start)`. That length byte is inside the payload.

Walk opcodes 246, 147, and 216 do not use `opkind8712`. They write an explicit `u8` with `opkind873` (`Client.java:949`). That byte is also payload. Its value is `n*2+3` for 246 and 216, and `n*2+3+14` for 147. It does not count itself.

## Widths

Writes:

| name | method | bytes |
| --- | --- | --- |
| u8 | `opkind873` | one byte (`C9cd2e2e1.java:140`) |
| u8+128 | `opkind8729` | byte plus 128 (`C9cd2e2e1.java:392`) |
| u8neg | `opkind8730` | negation of the byte (`C9cd2e2e1.java:401`) |
| u8sub | `opkind8731` | 128 minus the value (`C9cd2e2e1.java:410`) |
| u16be | `opkind874` | high byte then low byte (`C9cd2e2e1.java:144`) |
| u16le | `opkind8738` | low byte then high byte (`C9cd2e2e1.java:477`) |
| u16be+128 | `opkind8739` | high byte, then low byte plus 128 (`C9cd2e2e1.java:487`) |
| u16le+128 | `opkind8740` | low byte plus 128, then high byte (`C9cd2e2e1.java:497`) |
| u24be | `opkind876` | three bytes, high first (`C9cd2e2e1.java:159`) |
| u32be | `opkind877` | four bytes, high first (`C9cd2e2e1.java:165`) |
| u32le write | `opkind878` | four bytes, low first (`C9cd2e2e1.java:172`) |
| u64be | `opkind879` | eight bytes, high first (`C9cd2e2e1.java:184`) |
| jstr | `opkind8710` | raw bytes then byte 10 (`C9cd2e2e1.java:204`) |
| bytes | `opkind8711` | raw copy (`C9cd2e2e1.java:210`) |
| bytes+128 | `opkind8751` | each byte plus 128 (`C9cd2e2e1.java:625`) |
| psize1 | `opkind8712` | patch the preceding length byte |

Reads:

| name | method | layout |
| --- | --- | --- |
| u8 | `opkind8713` | one byte (`C9cd2e2e1.java:231`) |
| s8 | `opkind8714` | signed byte (`C9cd2e2e1.java:235`) |
| u16be | `opkind8715` | high then low (`C9cd2e2e1.java:239`) |
| s16be | `opkind8716` | u16be, subtract 65536 when above 32767 (`C9cd2e2e1.java:244`) |
| u24be | `opkind8717` | three bytes, high first (`C9cd2e2e1.java:254`) |
| u32be | `opkind8718` | four bytes, high first (`C9cd2e2e1.java:259`) |
| u64be | `opkind8719` | two `u32be` (`C9cd2e2e1.java:264`) |
| jstr | `opkind8720` | bytes until 10 (`C9cd2e2e1.java:276`) |
| bytes-until-10 | `opkind8721` | same terminator, returned as a byte array (`C9cd2e2e1.java:285`) |
| bytes(n) | `opkind8722` | n raw bytes (`C9cd2e2e1.java:305`) |
| bits | `opkind8723`, `opkind8724`, `opkind8725` | bit cursor starts at `position*8`; `opkind8724` takes the high bits of each byte first (`C9cd2e2e1.java:321`) |
| smart | `opkind8727` | if the next byte is below 128, `u8`, otherwise `u16be` minus 32768 (`C9cd2e2e1.java:369`) |
| u8-128 | `opkind8732` | `(byte - 128) & 255` (`C9cd2e2e1.java:419`) |
| u8neg | `opkind8733` | `(-byte) & 255` (`C9cd2e2e1.java:428`) |
| u8sub | `opkind8734` | `(128 - byte) & 255` (`C9cd2e2e1.java:437`) |
| s8-128 | `opkind8735` | signed `(byte - 128)` (`C9cd2e2e1.java:446`) |
| s8neg | `opkind8736` | signed negation (`C9cd2e2e1.java:459`) |
| s8sub | `opkind8737` | signed `(128 - byte)` (`C9cd2e2e1.java:468`) |
| u16le | `opkind8741` | low byte then high byte (`C9cd2e2e1.java:510`) |
| u16be+128 | `opkind8742` | high byte, then low byte minus 128 (`C9cd2e2e1.java:520`) |
| u16le+128 | `opkind8743` | when the argument is negative: low byte minus 128, then high byte. When the argument is not negative the method returns the constant 3 (`C9cd2e2e1.java:530`). Packet callers pass `valueClient615`, which is `-151` (`Client.java:528`). `getCombatLevelColorTag` can set it to `-303` (`Client.java:576`), which is still negative. |
| s16le | `opkind8744` | u16le, subtract 65536 when above 32767 (`C9cd2e2e1.java:540`) |
| s16be+128 | `opkind8745` | same bytes as `u16be+128`, then the signed adjustment (`C9cd2e2e1.java:555`) |
| s16le+128 | `opkind8746` | same bytes as `u16le+128`, then the signed adjustment (`C9cd2e2e1.java:570`) |
| u24mix | `opkind8747` | bytes b0, b1, b2 in read order become `(b1<<16)\|(b0<<8)\|b2` (`C9cd2e2e1.java:585`) |
| u32le | `opkind8748` | four bytes, low first (`C9cd2e2e1.java:595`) |
| u32mixA | `opkind8749` | bytes b0, b1, b2, b3 in read order become `(b2<<24)\|(b3<<16)\|(b0<<8)\|b1` (`C9cd2e2e1.java:605`) |
| u32mixB | `opkind8750` | bytes b0, b1, b2, b3 become `(b1<<24)\|(b0<<16)\|(b3<<8)\|b2` (`C9cd2e2e1.java:615`) |
| bytes-128 reversed | `opkind8752` | reads n bytes and stores them last-to-first, each minus 128 (`C9cd2e2e1.java:637`) |

`opkind8726` exists (`C9cd2e2e1.java:364`) and is not used by the packet handlers read for this revision.
