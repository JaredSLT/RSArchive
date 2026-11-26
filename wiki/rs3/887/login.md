# Login

RS3 login in this client is the lobby path. `CameraManager.doCameraManager(String, String)` sets `Statics.tint76 = 153` and points `Statics.runeScapeSetupStatus` at `Client.runeScapeSetupStatus` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/camera/CameraManager.java:178`). `Client.drawDebug` labels that socket `Lobby:` and `Client.runeScapeSetupStatus2` as `Game:` (`builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:9668` and `:9679`).

`doCameraManager2(String, String)` sets `tint76 = 270` and uses the game socket (`CameraManager.java:185`). That path writes a different stage-80 block. It is not a 317 login block. The lobby block is the one below.

The state machine is `CameraManager` from the `tint2 == 10` test (`CameraManager.java:275`). Hosts: lobby `Statics.cls37d26e716`, game `Statics.cls37d26e717` (`:276`). Opcode constants are `Unused8` (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/Unused8.java`).

`jstr` is `PositionedSound.doPositionedSound3(String)`.

## Stage 10, opcode 14

Connect the lobby host when `tint76` is not 270 (`:278`). Write one u8, `Unused8.unused8.tint75`, which is 14, size 0 (`:284`, `Unused8.java:8`).

## Stage 35 response

Read 9 bytes (`:290`).

| order | width | meaning |
| --- | --- | --- |
| 1 | u8 | status. Nonzero fails. `doCameraManager(int)` at `:1035` is empty. `doCameraManager2(int)` stores the code in `tint5` when `tint76` is 153 (`:1108`). |
| 2 | u64be | server seed `Statics.tlong7`, only when status is 0 (`:307`) |

Then stage 273 if `tboolean2`, else stage 80 (`:309`).

## Stage 273, opcode 29

`Unused8.unused810` is opcode 29, size -2 (`Unused8.java:26`). Written at `CameraManager.java:318`.

| order | width |
| --- | --- |
| 1 | u8 opcode 29 |
| 2 | u16be length placeholder, backpatched with `doPositionedSound3(int)` |
| 3 | u32be 887 |
| 4 | u32be 1 |
| 5 | only when `tint76 == 270`: u8, 1 if `Client.state == 9` else 0 |
| 6 | RSA block from `doCameraManager2222b` plus `doCameraManager(buf, long)` plus the fields through the `modPow` at `:341` |

Inner buffer before RSA (`:1039` and `:327`):

| order | width |
| --- | --- |
| 1 | u8 10 |
| 2 | u32be random seed 0 |
| 3 | u32be random seed 1 |
| 4 | u32be high half of `tlong7` |
| 5 | u32be low half of `tlong7` |
| 6 | u8 auth type id |
| 7 | auth body, below |
| 8 | u8 `tint` |
| 9 | u8 language id |
| 10 | u32be `Client.tint` |
| 11 | five u32be random |
| 12 | u64be `Client.tlong2` |
| 13 | u8 `Client.modegame.tint` |
| 14 | u8 random |

Auth (`:1055`): a 6-digit `string3` uses one `LinkListIterator`, a default-sprite uid uses another, otherwise a third. Case `tint` 0 or 3 writes u24be `parseInt(string3)` and then increments the buffer offset by one. Case 1 writes u32be. Case 2 skips 4 bytes. The iterator ids were not opened.

RSA uses `TotpPreferences.bigInteger` exponent `10001` hex and `bigInteger2` modulus (`builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/core/utils/TotpPreferences.java:9`). `doPositionedSound(BigInteger, BigInteger)` writes u16be cipher length and the cipher (`PositionedSound.java:645`).

Stage 40 reads a u16be length and stage 53 reads that many bytes. Those bodies were not read.

## Lobby stage 80, opcode 19

Taken when `tint76 != 270` (`CameraManager.java:468`). Opcode is 30 (`unused811`) if `tboolean2`, else 19 (`unused84`, `Unused8.java:14`).

| order | width | notes |
| --- | --- | --- |
| 1 | u8 | 19, or 30 when `tboolean2` |
| 2 | u16be | length placeholder at `var19` |
| 3 | u32be 887 | skipped when `tboolean2` |
| 4 | u32be 1 | skipped when `tboolean2` |
| 5 | bytes | RSA block from `doCameraManager2222b3`, copied raw. Skipped when `tboolean2` |
| 6 | u8 | 1 if `tlong == -1`, else 0. Skipped when `tboolean2` |
| 7 | jstr or u64be | jstr `string` if `tlong == -1`, else u64be `tlong`. Skipped when `tboolean2` |
| 8 | u8 | `Client.modegame.tint` |
| 9 | u8 | language id |
| 10 | u8 | `Client.doClient2222b26()`. That method was not read |
| 11 | u16be | `Statics.tint42` |
| 12 | u16be | `Statics.tint87` |
| 13 | u8 | `preferencesParticles2.doPreferencesParticles22()` |
| 14 | unread | `GameShell.doGameShell` |
| 15 | jstr | `Client.string` |
| 16 | u8 | prefs length |
| 17 | bytes(n) | `doDataBlock902` bytes. Layout unread |
| 18 | bytes(n) | hardware block. `calculateHardwarePlatformSize` bytes, layout unread |
| 19 | u32be | `Client.tint25` |
| 20 | jstr | `Statics.string14` |
| 21 | u32be | `Client.tint` |
| 22 | u32be | `Client.tint8` |
| 23 | jstr | `Client.string4` |
| 24 | u8 | `Statics.tint155 & 1` |
| 25 | u32be each | one checksum per `ArchiveUtil` except `archiveUtil28` (id 32). Missing archive writes 0 (`:1135`) |
| 26 | XTEA | `doPositionedSound2(intArray21, var20, tint)` from the offset after the RSA copy, or from just after the length when `tboolean2` |
| 27 | backpatch | u16be length of the bytes after the length field |

RSA inner block `doCameraManager2222b3` (`:1086`): the four-seed block from `doCameraManager2222b` (`:1039`), then the auth writer, then jstr password `string2`, u64be `tlong2`, u64be `Client.tlong2`, then `modPow`.

Seeds in `Statics.intArray21`: two random u32, then the high and low halves of `tlong7` (`:1041`).

## After stage 80

Outgoing Isaac from the four seeds (`:521`). Add 50 to each seed (`:523`). Incoming Isaac from the adjusted seeds (`:528`), bound on the buffer (`:530`). Seeds cleared. Next stage is 98.

## Stage 98 status

One u8 (`:541`). Branches read here:

| code | next |
| --- | --- |
| 21 | stage 125 |
| 1 | `doCameraManager2(1)` and return (`:546`) |
| 52 | stage 227 |
| 2 | game (`tint76 == 270`) goes to 257; lobby goes to 145 (`:562`) |
| 15 | `tint2 = -2`, stage 208 (`:568`) |
| 23 | retry from stage 10, up to 3 (`:572`) |

Later codes in the same method were not all read. `doCameraManager(int)` does not print a message (`:1035`).

## Lobby success, stages 145 and 154

Stage 145: one u8 length into `Statics.tint116` (`:800`).

Stage 154, lobby branch (`:830`):

| order | width | stored |
| --- | --- | --- |
| 1 | u8 | if 1, four Isaac bytes follow (`doCameraManager` at `:1021`) |
| 2 | u8 | `Client.tint77` |
| 3 | u8 | `Client.tint78` |
| 4 | u8 | flag `tboolean31` |
| 5 | i24be | `Statics.tint14` |
| 6 | u8 | local player `tbyte` |
| 7 | u8 | flag `tboolean32` |
| 8 | u8 | flag `tboolean33` |
| 9 | u64be | `Statics.tlong11` |
| 10 | u40be | subtracted into `tlong10` |
| 11 | u8 | bit 0 `Client.tboolean29`, bit 1 `Statics.tboolean` |
| 12 | u32be | `Statics.tint176` |
| 13 | u32be | `Statics.tint79` |
| 14 | u16be | `Statics.tint55` |
| 15 | u16be | `Statics.tint158` |
| 16 | u16be | `Statics.tint48` |
| 17 | u32be | `Statics.tint113`, then a `SecureRandomTask` thread |
| 18 | u8 | `Statics.tint166` |
| 19 | u16be | `Statics.tint154` |
| 20 | u16be | `Statics.tint159` |
| 21 | u8 | staff flag `tboolean5` |
| 22 | versioned jstr | username (`doPositionedSound3222222`) |
| 23 | u8 | `Statics.tint175` |
| 24 | u32be | `Statics.tint46` |
| 25 | u16be | world id, 65535 becomes -1 |
| 26 | versioned jstr | world host |
| 27 | u16be | port `tint2` |
| 28 | u16be | port `tint3` |

This branch does not assign `Client.tint76`. The player snapshot reader uses `Client.tint76` (`ServerConnection.java:55`). The game-socket stage 154 does assign it (`CameraManager.java:820`).

## Game stage 80, opcode 16

Written when `tint76 == 270` (`:405`). Opcode 30 if `tboolean2`, else 16 (`unused83`, `Unused8.java:12`). Same length and XTEA shape as the lobby block, with extra u32 fields and a host compare. Field order is `CameraManager.java:413` through `:467`. It is the world login this client also writes. It is not the lobby path.

## Opcode 26

On the game connection, stage 268 may write opcode 26, size 0 (`:756`, `Unused8.java:20`), then go to stage 138.

## Opcode 15

`Client` writes opcode 15 on `Statics.cls37d26e715`, not on the lobby socket (`Client.java:2069`). Size from `Unused8` is -1 (`Unused8.java:10`). Bytes: u8 15, u8 length, u32be 887, u32be 1, jstr `string4`, u8 `language.serialID`. The length counts the bytes after itself.

## JS5 opcode 28

`DataBlock499.doDataBlock6382222b4` writes opcode 28 on the lobby socket (`builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock499.java:199`). That is the update handshake, not the lobby login. Bytes after u16be 887 and u16be 1, up to the XTEA start, are `doDataBlock638` and were not read (`:204`).
