# Login

This client does not write a 317 login block. The socket writers are `SecureRandomTask.blankWs` (`com/jagex/core/utils/SecureRandomTask.java:267`) and the earlier lobby handshake `DefaultSprites.doDefaultSprites2222b` (`com/jagex/graphics/DefaultSprites.java:168`).

`Statics.tint83 == 133` is the lobby socket `Client.clsb20f177f` and host `Statics.bufferedFile3` (`SecureRandomTask.java:209`). `tint83 == 290` is the game socket `Client.clsb20f177f2` and host `Statics.bufferedFile7` (`:216`). `CameraManager` posts an HTTP account-appeal form. That is not this socket block.

Login buffers come from `Heap.doTotpPreferences` (`Heap.java:57`), which does not apply ISAAC. RSA uses `Unused1.bigInteger` (exponent `10001` hex) and `Unused1.bigInteger2` (`com/jagex/Unused1.java:9`).

## Early lobby, opcode 28

`DefaultSprites.doDefaultSprites2222b` connects port 40000 and writes:

1. `u8` opcode 28 (`DataBlock351.clsba94f36b9`).
2. `u16be` length placeholder.
3. `u16be` 902, `u16be` 1. These are 16-bit, not the 32-bit revision used later.
4. RSA block from `doDefaultSprites` (`:265`): `u8` 10, four `u32be` random seeds, ten `u32be` random, one `u16be` random, then RSA. The four seeds are kept.
5. `jstr` `Client.string4`, `u16be` `Client.tint`, two `u32be` (`Client.tint4`, `Client.tint3`), `jstr` `Statics.string14`.
6. `u8` language id, `u8` `Client.modegame.game`.
7. `GameShell.doGameShell(buffer)` (body unread).
8. `u8` 1 if `Client.string2` is non-null, and then that `jstr`.
9. `HardwarePlatform.createHardwareBlock` (body unread).
10. Skip 7 bytes (`tint += 7`).
11. XTEA from the mark after the RSA block through the end, using the four seeds. Backpatch the `u16be` length.

The response is one byte, looked up as `SerializableEnums`. Only `serializableEnums6` continues. Then `isaac` is `new Isaac(seeds)` and `isaac2` is `seed[i] + 50`, installed on the receive buffer (`:233`). The client state becomes 12.

## Handshake opcode 14

`blankWs` stage 13 connects port 40000 and writes `u8` opcode 14 only (`SecureRandomTask.java:320`).

Stage 31 reads 9 bytes (`:326`). `u8` must be 0, otherwise the attempt fails. Then `u64be` is stored as `Statics.tlong12`. `tboolean2` selects stage 271, otherwise stage 85.

## Stage 271, opcode 29

Written when `tboolean2` is set (`:352`):

1. `u8` 29, `u16be` length placeholder.
2. `u32be` 902, `u32be` 1.
3. If this is the game socket, `u8` 1 when `Client.state == 4`, else 0.
4. RSA block. `doSecureRandomTask2222b4` (`:1076`) writes into a 518-byte buffer: `u8` 10, four `u32be` secure-random seeds saved as `Statics.intArray`, `u64be` `Statics.tlong12`, and if `Client.state == 4` four more `u32be` from `Statics.intArray6`. The caller then appends, before RSA: the TOTP block, `u8` `tint`, `u8` language id, `u32be` `Client.tint`, five `u32be` random, `u64be` `Client.tlong2`, `u8` `modegame.game`, one more `u32be` random. Then RSA.
5. Copy the ciphertext onto the outer buffer and backpatch the `u16be` length.

TOTP (`doSecureRandomTask(WorldMapElement, long)` `:1101`) writes `u8` `TotpType.getId()`, which is `serialID`, then switches on `tint`:

| tint | serialID | Follow |
| --- | --- | --- |
| 1 `totpType` | 0 | `u32be` stored token |
| 0 `totpType2` | 1 | `u24be` of the 6-digit code, then `pos` advances one more byte |
| 2 `totpType4` | 3 | same as tint 0 |
| 3 `totpType3` | 2 | `pos` advances 4, no bytes written |

Stage 42 reads `u16be` length. Stage 56 reads that payload, XTEA-decrypts with `Statics.intArray`, and reads `jstr0`. Stage 67 reads `u8`; 1 continues to stage 77. Stage 77 reads 16 bytes, XTEA-decrypts, and reads two `u64be` into `tlong` and `tlong2`. Then stage 85.

## Stage 85, opcodes 16, 19, and 30

Game socket: opcode 30 if `tboolean2`, else opcode 16 (`:441`). Lobby socket: opcode 30 if `tboolean2`, else opcode 19 (`:504`). Then `u16be` length placeholder. The XTEA mark is the position after the RSA and username section.

When `tboolean2` is clear the prefix is:

1. `u32be` 902, `u32be` 1.
2. Game only: `u8` 1 when `Client.state == 4`.
3. RSA blob from `doSecureRandomTask2222b6` (`:1132`). That calls `doSecureRandomTask2222b4`, and when `state != 4` appends the username long (`tint` if it is not `-1`, else `CheckNameReply.doCheckNameReply222(string)` if `tlong == -1`, else `tlong`; the hash method was not read), the TOTP block, `u8` 0, `jstr` password `string2`, `u64be` `tlong2`, `u64be` `Client.tlong2`, then RSA.
4. `u8` 1 if `tlong == -1`, and then `jstr` username, else `u64be` `tlong`.

Game tail after that mark (`:469`):

`u8` `Client.doClient2222b34()` (body unread), `u16be` width `Statics.tint151`, `u16be` height `Statics.tint59`, `u8` loading-screen preference, `GameShell.doGameShell` (unread), `jstr` `Client.string`, `u32be` `Client.tint`, `u8` length plus the preferences blob from `doClientWorldMap` (unread), the hardware block (unread), five `u32be` (`tint24`, `tint4`, `tint3`, `tint6`, `tint5`), `jstr` `Statics.string14`, `u8` 1 if `Client.string2` is set and then that `jstr`, `u8` `Client.tboolean`, `u8` `Client.tboolean7`, `u8` `Statics.tint154 & 1`, `u32be` `Client.tint8`, `jstr` `Client.string4`, `u8` 0 when `bufferedFile6` is non-null and its port equals `bufferedFile7`, else 1, `u16be` lobby port `bufferedFile3.tint`, then `doSecureRandomTask2` writes one `u32be` CRC per `FileUtil` except `fileUtil28` (`:1185`). XTEA the slice and backpatch the `u16be` length.

Lobby tail (`:530`) is the same idea with a different field order: `u8` game, `u8` language, `u8` `doClient2222b34`, `u16be` width, `u16be` height, `u8` loading screen, machine info, `jstr` `Client.string`, preferences blob, hardware block, `u32be` `tint24`, `jstr` `string14`, `u32be` `Client.tint`, `u32be` `tint8`, `jstr` `string4`, `u8` `tint154 & 1`, `u8` 0 from `doWorldMapElement(false)`, CRCs, XTEA, length.

ISAAC is installed here as in [framing.md](framing.md) (`:558`).

## Response

Stage 97 reads one status byte (`:577`).

| Status | Next |
| --- | --- |
| 21 | stage 126 reads `u8` and sets the delay to that value times 50, then fails (`:652`) |
| 1 | stage 106, held |
| 52 | stage 228 reads `u16be` length; stage 230 reads that many bytes, ISAAC-adjusts them, and reads `jstr` (`:727`) |
| 2 | game goes to stage 253; lobby goes to stage 141. A `tboolean5` retry returns to stage 13 |
| 15 | stage 202, `u16be` length then a payload consumed by `Statics.doStatics` (body unread) (`:1006`) |
| 23 | up to 3 retries from stage 13 |
| 42 | stage 213 reads `u16be` into `tint13` and returns to stage 97 (`:667`) |
| 49 | lobby only, and only while `Client.state != 16`; stays pending |
| 35 | with `tboolean` and not `tboolean2` and `tint != -1`, sets `tboolean2` and retries |
| 53 | stage 246 reads `u32be` into `tint14` and fails (`:678`) |
| other | fail, stage 3 |

Game success stage 253 reads `u16be` length. Stage 266 (`:776`) reads `u8` flag, then `DBUtils.decodeVarValue` until the length is consumed (that decoder was not read). If the flag is 1 it sends opcode 26 as a single `u8` and goes to stage 132, otherwise it loops to 253. Stage 132 reads `u8`. 2 continues. 29 reads one more `u8` in stage 197. 45 reads `u8` plus `u16be` in stage 197. Anything else fails.

Stage 141 reads `u8` length `tint43`. Stage 151 reads that many bytes. `doSecureRandomTask(WorldMapElementPart)` (`:1058`) reads `u8`. If it is 1, the next four bytes are ISAAC-adjusted (`doWorldMapElementPart`) and stored as a `u32be`.

Game fields after that (`:851`): six `u8` (`tint76`, `tint77`, four booleans), `u16be` player index `Client.tint75`, `u8` boolean, `i24be`, `u8` boolean, `jstr`, `u48be`.

Lobby fields (`:875`): three `u8`, `i24be`, `u8` plane, three `u8` booleans, `u64be`, `u40be`, `u8` flags (bit 0 and bit 1), two `u32be`, four `u16be`, `u32be`, `u8`, two `u16be`, `u8` boolean, `jstr0`, `u8`, `u32be`, `u16be` world id (`65535` means `-1`), `jstr0`, two `u16be`. Lobby then sets state 3 (`:938`).

Game stage 170 reads 3 bytes. Stage 188 decodes an ISAAC opcode, including the extension byte when the peek says so, then `u16be` length (`:959`). Stage 166 reads the payload, lets `Statics.doStatics` consume a prefix (unread), and treats opcode 49 as dynamic rebuild and every other opcode as static rebuild (`:991`). `doSecureRandomTask2222b2` enters the world (`:1208`).
