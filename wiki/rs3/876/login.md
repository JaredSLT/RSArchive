# Login

This client writes an RS3 lobby and world login. It does not write a 317 login block.

`LoginManager.notPermittedSocialNetwork` (`rs2/client/login/LoginManager.java:31`) posts an account-appeal HTTP body and uses a different decimal RSA modulus. It is not the game login.

Two sockets:

- `BlankWs.doBlankWs` sets `Statics.tint88` to 142 and logs in on `Client.pureJavaToolkitContext` (`deob/BlankWs.java:201`). Stage 80 then writes opcode 19, or opcode 30 when `tboolean2` is set (`BlankWs.java:456`).
- `BlankWs.doBlankWs2` sets `Statics.tint88` to 267 and logs in on `Client.pureJavaToolkitContext2` (`BlankWs.java:208`). Stage 80 writes opcode 16, or opcode 30 when `tboolean2` is set (`BlankWs.java:393`). This is the lobby login.
- `Statics.doStatics3222b9` writes opcode 28 on `Client.pureJavaToolkitContext` while `clsf8976dad` equals `DataBlock1785.clsf8976dad` (`deob/Statics.java:2591`). `Client.doClient22` calls that method on states 16, 7, 1, 3, 11, 13, and 5 (`rs2/client/Client.java:1604`).

Widths are in [framing.md](framing.md). Login opcode constructors discard their second argument (`deob/DataBlock1802.java:49`).

## Stage 11 handshake

`BlankWs.blankWs` at stage 11 (`BlankWs.java:267`).

When `tboolean2` is set the packet is opcode 29 (`BlankWs.java:277`):

1. u8 opcode 29.
2. u16be length placeholder.
3. u32be 876.
4. u32be 1.
5. If `tint88 == 267`, u8 `1` when `Client.tint72 == 17`, else u8 `0` (`BlankWs.java:282`).
6. RSA block, then the u16be length is patched over the bytes after the placeholder.

The RSA plaintext, before `doPositionedSound(exponent, modulus)` (`BlankWs.java:286`):

1. u8 `10`, then four u32be seeds stored in `Statics.intArray8` (`BlankWs.java:1027`).
2. u8 auth-type id from `Fullscreen.getId()` (`BlankWs.java:1057`).
3. If that type's `tint` is 0, u32be from `cls76850f50.doDataBlock12832`. If it is 1 or 2, u24be `Integer.parseInt(string3)` and then the cursor advances one byte with no put. If it is 3, the cursor advances four bytes with no put (`BlankWs.java:1058`).
4. u8 `BlankWs.tint`, u8 language id, u32be `Client.tint`, five u32be random values, u64be `Client.tlong2`, u8 modegame id, u8 random (`BlankWs.java:289`).
5. RSA with exponent hex `10001` and `bigInteger2`. The transform replaces the plaintext with u16be length plus the big-endian magnitude (`PositionedSound.java:640`).

When `tboolean2` is clear, stage 11 writes only u8 opcode 14 (`BlankWs.java:304`).

The next read is one u8. Any value other than 0 fails (`BlankWs.java:319`). On success, reconnect goes to stage 40 and a normal login goes to stage 80.

## Stage 80 login packet

Built at `BlankWs.java:388` on a buffer with no cipher. Opcode 16 or 30 on the lobby path (`tint88 == 267`). Opcode 19 or 30 on the world path.

Both paths write u8 opcode, u16be length placeholder, and, when `tboolean2` is clear:

1. u32be 876, u32be 1.
2. Lobby only: u8 `1` when `Client.tint72 == 17`, else `0` (`BlankWs.java:408`). The world path does not write that byte (`BlankWs.java:468`).
3. RSA block from `doBlankWs2222b` (`BlankWs.java:1074`): the same u8 `10` and four seed u32be, the same auth tail, jstr `string2`, u64be `tlong2`, u64be `Client.tlong2`, then RSA with the same exponent and `bigInteger2`.
4. u8 `1` when `tlong == -1`, else u8 `0`. If `tlong == -1`, jstr `string`, else u64be `tlong`.

XTEA with `Statics.intArray8` starts at the first byte after the RSA block on a normal login, and at the first byte after the length placeholder when `tboolean2` is set (`BlankWs.java:411` and `:454`). The u16be length is patched after XTEA.

Lobby tail after the username, including on reconnect (`BlankWs.java:421`):

1. u8 `Client.doClient3222b29()`. That method body was not read.
2. u16be `Statics.tint39`, u16be `Statics.tint53`.
3. u8 `preferencesCharacterShadows2.canMod()`.
4. 24 machine bytes from `GameShell.doGameShell` (`GameShell.java:477`). On failure every byte is `0xFF`.
5. jstr `Client.string`.
6. u32be `Client.tint`.
7. u8 length of `clsf87db028.doDataBlock1501()`, then those bytes. The blob interior was not read.
8. `HardwarePlatform.createHardwareBlock` bytes, length `calculateHardwarePlatformSize()`. The block interior was not read.
9. u32be `Client.tint24`, u32be `Client.tint4`, u32be `Client.tint3`, u32be `Client.tint6`, u32be `Client.tint5`.
10. jstr `Statics.string`.
11. u8 `1` when `Client.string2` is non-null, and then that jstr.
12. u8 `Client.tboolean`, u8 `Client.tboolean7`, u8 `Statics.tint199 & 1`.
13. u32be `Client.tint8`, jstr `Client.string5`.
14. u8 `0` when `cls6529df855.tint` equals `cls6529df85.tint`, else u8 `1`.
15. u16be `cls6529df856.tint`.
16. One u32be checksum per `RawCpuInfo` except `rawCpuInfo28`, or u32be `0` when that archive is absent (`BlankWs.java:1123`).

World tail (`BlankWs.java:482`):

1. u8 modegame id, u8 language id, u8 `Client.doClient3222b29()`.
2. u16be width, u16be height, u8 `canMod()`, the same 24 machine bytes, jstr `Client.string`.
3. The same u8-length preferences blob and the same hardware block.
4. u32be `Client.tint24`, jstr `Statics.string`, u32be `Client.tint`, u32be `Client.tint8`, jstr `Client.string5`, u8 `Statics.tint199 & 1`.
5. The same checksum loop.

After the packet is queued, `privateChatFilter` is `new PrivateChatFilter(seeds)` and `privateChatFilter2` is `new PrivateChatFilter(seeds[i] + 50)`. The read buffer uses `privateChatFilter2` (`BlankWs.java:509`).

## Opcode 28

`Statics.doStatics3222b9` (`Statics.java:2595`):

1. u8 opcode 28, u16be length placeholder.
2. u16be 876, u16be 1. These are `doPositionedSound2`, not the u32be revision used by opcodes 16, 19, and 29.
3. `intArray9 = doStatics(packet)`. That helper was not read.
4. jstr `Client.string5`, u16be `Client.tint`, u32be `Client.tint4`, u32be `Client.tint3`, jstr `Statics.string`, u8 language, u8 modegame, 24 machine bytes.
5. u8 `1` and jstr `Client.string2` when that string is non-null, else u8 `0`.
6. `createHardwareBlock` written straight onto this buffer.
7. The cursor advances 7 bytes with no put (`Statics.java:2617`).
8. XTEA of `intArray9` from the byte after the two u16be header fields through the cursor, then the u16be length is patched.

The response is one u8, looked up as `ByteArrayPool`. Only `byteArrayPool6` continues. The ciphers are then installed the same way: outgoing seeds, incoming seeds plus 50 (`Statics.java:2641`). `Client.doClient2222b(11)` follows.

## Response byte

Stage 95 reads one u8 (`BlankWs.java:529`).

| Byte | What this client does |
| --- | --- |
| 21 | Stage 124 reads another u8 and stores `value * 50`, then fails with code 21 (`BlankWs.java:603`). |
| 1 | Fails with code 1 (`BlankWs.java:534`). |
| 52 | Stage 224 reads u16be length, then that many ISAAC-subtracted bytes, decoded as a jstr (`BlankWs.java:678`). |
| 2 | Success. Lobby (`tint88 == 267`) goes to stage 256. World goes to stage 148 (`BlankWs.java:543`). |
| 15 | Stage 205 reads a u16be length and that many bytes, then runs the player snapshot on them (`BlankWs.java:957`). |
| 23 | Retries stage 11 up to three times (`BlankWs.java:560`). |
| 42 | Stage 210 reads a u16be into `tint13` and returns to stage 95 (`BlankWs.java:618`). |
| 49 | On `tint88 == 142` and `Client.tint72 != 4`, it returns without the normal failure path when `tint5` is already 52 (`BlankWs.java:574`). |
| 53 | Stage 244 reads a u32be into `tint14` and disconnects (`BlankWs.java:629`). |
| 35 | When `tboolean` is set, `tboolean2` is not set, and `tint != -1`, it sets `tboolean2` and returns to stage 11 (`BlankWs.java:582`). |
| other | `doBlankWs2(code)` and disconnect (`BlankWs.java:590`). Codes 29 and 45 are handled later, at stage 135, not in this first byte. |

## After success

Lobby stage 256 reads u16be length. Stage 265 reads that many bytes: u8 `1` means the block is finished, then zero or more `doPublicKeysPart` records until the length is consumed (`BlankWs.java:734`). The record layout was not read. If the flag is 1 the client writes u8 opcode 26 and goes to stage 135 (`BlankWs.java:744`). Stage 135 reads one u8. Value 2 goes to stage 148. Values 29 and 45 go to stage 198, which reads one byte for 29 and three bytes for 45 (`BlankWs.java:644`). Any other value fails.

Stage 148 reads u8 `Statics.tint127`, then that many bytes (`BlankWs.java:782`).

Both paths start with `doBlankWs(PositionedSoundPart4)` (`BlankWs.java:1009`): u8 `1` means four ISAAC-subtracted bytes form a u32 stored by `cls76850f50.doDataBlock1283`. u8 `0` means those four bytes are absent.

Lobby payload after that (`BlankWs.java:802`):

1. u8 `Client.tint77`, u8 `Client.tint78`.
2. u8 `1` flags for `tboolean31`, `tboolean32`, `tboolean33`, `tboolean34`.
3. u16be `Client.tint76`, the local player index. The world branch below does not assign `Client.tint76`.
4. u8 `1` flag `tboolean29`, i24be `Statics.tint60`, u8 `1` flag `tboolean30`, jstr `Statics.string6`.
5. u48be (`doPositionedSound3322b3`) minus `DataBlock1456.doDataBlock1009()`, stored in `Statics.tlong13`.

World payload after the optional uid (`BlankWs.java:826`):

1. u8 `Client.tint77`, u8 `Client.tint78`, u8 `1` flag `tboolean31`.
2. i24be `Statics.tint60`, u8 copied to `cls91f0c927.tbyte`.
3. u8 `1` flag `tboolean32`, u8 `1` flag `tboolean33`.
4. u64be `Statics.tlong9`. `tlong13` is that value minus `doDataBlock1009()` minus a further `doPositionedSound3322b()` (u8 then u32be, five bytes).
5. u8 flags: bit 0 is `Client.tboolean29`, bit 1 is `Statics.tboolean6`.
6. u32be `Statics.tint87`, u32be `Statics.tint84`, u16be `Statics.tint5`, u16be `Statics.tint46`, u16be `Statics.tint93`, u32be `Statics.tint70` passed to `CreateConnectStage`.
7. u8 `Statics.tint7`, u16be `Statics.tint172`, u16be `Statics.tint52`, u8 `1` flag `tboolean7`.
8. jstr0 into the local player's name and `Statics.string2`.
9. u8 `Statics.tint42`, u32be `Statics.tint`.
10. u16be host id (`65535` means `-1`), jstr0 hostname, u16be `tint2`, u16be `tint3` on `cls6529df855`.

The world path then returns (`BlankWs.java:889`). It does not read the player snapshot or the rebuild inside `blankWs`.

The lobby path continues. Stage 188 reads the first game packet header with the ISAAC opcode (`doPositionedSoundPart422`) and then a u16be length, not the game size table (`BlankWs.java:910`). Stage 165 runs `DeveloperConsole.doDeveloperConsole` on the front of that payload (the 30-bit and 18-bit snapshot in [sync.md](sync.md)). The unread tail is the rebuild: opcode 173 calls `World.doWorld` with `RebuildStage.rebuildStage4`, and any other opcode uses `rebuildStage3` (`BlankWs.java:943`).
