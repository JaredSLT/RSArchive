# Login

The lobby writer is `Statics.jagtheora` (`deob/Statics.java:2614`). Lobby entry sets `tint70` to 285 and `Statics.pureJavaToolkitFactory` to `Client.pureJavaToolkitFactory2` (`deob/DataBlock1937.java:199`, `DataBlock1937.java:206`, `DataBlock1937.java:224`). Game-world entry sets `tint70` to 179 and the socket to `Client.pureJavaToolkitFactory` (`DataBlock1937.java:174`, `DataBlock1937.java:181`, `DataBlock1937.java:231`). The state variable is `DataBlock1937.tint2`. Login opcodes are `DataBlock2081` and are written with `DataBlock2435.doDataBlock24352`, so the opcode byte is raw.

The RSA exponent is `bigInteger` (`deob/Cc83cb2fc68818da3e8de0f2965f5972075a95708e46b5ff.java:8`, hex `10001`). The login modulus is `bigInteger2` on the next field (`Cc83cb2fc68818da3e8de0f2965f5972075a95708e46b5ff.java:10`). `bigInteger3` and `bigInteger4` are the JS5 key (`Cc83cb2fc68818da3e8de0f2965f5972075a95708e46b5ff.java:12`).

## State 11

`Statics.java:2652` connects with a 15000 ms socket. When `DataBlock1937.tboolean2` is set, the client writes opcode 29 (`DataBlock2081.cls5db3575f10`):

1. `u8` opcode 29.
2. `u16be` length placeholder.
3. `u32be` 790.
4. `u32be` 1.
5. When `tint70` is 285, one `u8`: 1 when `Client.tint72` is 3, otherwise 0 (`Statics.java:2667`).
6. The RSA block from `DataBlock1937.doDataBlock19372222b12` (`DataBlock1937.java:273`), continued in the state-11 method (`Statics.java:2671`): `u8` 10, four `u32be` seeds stored in `Statics.intArray7`, `u8` `DataBlock1937.tint`, `u32be` random, `u8` `language.getId()`, `u32be` `Client.tint`, six `u32be` random, `u64be` `Client.tlong2`, `u8` `Client.modegame.tint`, one `u32be` random, then RSA with `bigInteger` and `bigInteger2`.
7. The cipher bytes are copied and the placeholder is patched with `doPositionedSound23`.

When `tboolean2` is clear, state 11 writes only opcode 14 (`Statics.java:2688`).

## States 33, 45, 57, 67, 77

State 33 reads one `u8` (`Statics.java:2696`). A nonzero value fails through `doDataBlock1937222`. Zero moves to state 45 when `tboolean2` is set, and to state 85 otherwise.

State 45 reads a `u16be` length (`Statics.java:2721`). State 57 reads that many bytes, decrypts them with `doPositionedSound2(intArray7)`, reads a `vstr`, and opens it (`Statics.java:2732`). State 67 reads one byte and waits until it is 1 (`Statics.java:2747`). State 77 reads 16 bytes, decrypts them, and stores two `u64be` in `DataBlock1937.tlong` and `tlong2` (`Statics.java:2758`). The next state is 85.

## State 85 lobby

When `tint70` is 285 (`Statics.java:2777`), the opcode is 30 if `tboolean2` is set and 16 otherwise. The packet starts with the opcode byte and a `u16be` length placeholder.

When `tboolean2` is clear, the revision section is:

1. `u32be` 790, `u32be` 1, `u8` (`Client.tint72 == 3 ? 1 : 0`).
2. RSA block from `Statics.doStatics2322b` (`Statics.java:7442`): the seed prefix, `u64be` 0, `jstr` `DataBlock1937.string2`, `u64be` `DataBlock1937.tlong2`, `u64be` `Client.tlong2`, RSA with the login key.
3. `u8` 1 when `DataBlock1937.tlong` is -1, otherwise 0. The -1 arm writes `jstr` `DataBlock1937.string`. The other arm writes `u64be` `tlong`.

The common lobby tail (`Statics.java:2805`) is:

1. `u8` `Client.doClient2222b32()`. The method body was not read.
2. `u16be` `Statics.tint47`, `u16be` `Statics.tint60`.
3. `u8` `clsd5c2f94e.clsf0cbf2332.doAudioStream2Part3122()`. The method body was not read.
4. `GameShell.doGameShell(PreferencesCpuUsage)`. The bytes it writes were not read.
5. `jstr` `Client.string`.
6. `u32be` `Client.tint`.
7. `u8` length, then that many bytes from `clsd5c2f94e.doDataBlock2417()`. The producer was not read.
8. `hardwarePlatform.createHardwareBlock` into a buffer of `calculateHardwarePlatformSize`, copied in full. The producer was not read.
9. `u32be` `Client.tint24`, `u32be` `Client.tint4`, `u32be` `Client.tint3`.
10. `jstr` `Statics.string10`.
11. `u8` 0 when `Client.string2` is null, otherwise 1 and `jstr` `string2`.
12. `u8` 1 when the `jagtheora` native library is present.
13. `u8` `Client.tboolean`, `u8` `Client.tboolean7`, `u8` `Statics.tint51`.
14. `u32be` `Client.tint6`, `jstr` `Client.string5`.
15. `u8` 0 when the lobby host equals the current host, otherwise 1.
16. `Statics.doStatics(PreferencesCpuUsage)` (`Statics.java:7453`), the CRC list in [cache.md](cache.md).

`doPositionedSound(intArray7, var12, tint)` XTEA-encrypts from the byte after the RSA block to the end (`Statics.java:2836`). The `u16be` length is then patched. ISAAC is installed from the four seeds and from those seeds plus 50 (`Statics.java:2891`).

When `tboolean2` is set, opcode 30 omits the revision, RSA, and username section and starts at the common tail. The XTEA window then starts at the same cursor as the length placeholder's following byte.

## State 85 game world

When `tint70` is 179 (`Statics.java:2838`), the opcode is 30 if `tboolean2` is set and 19 otherwise. The clear-`tboolean2` section is `u32be` 790, `u32be` 1, the same RSA block, then the username `u8` and `jstr` or `u64be`. There is no `u8` before the RSA. The tail order is `u8` modegame, `u8` language, `u8` `Client.doClient2222b32()`, `u16be` `tint47`, `u16be` `tint60`, `u8` `clsd5c2f94e.clsf0cbf2332.doAudioStream2Part3122()`, `GameShell.doGameShell`, `jstr` `Client.string`, the preferences length and bytes, the hardware block, `u32be` `Client.tint24`, `jstr` `string10`, `u32be` `Client.tint`, `u32be` `Client.tint6`, `jstr` `Client.string5`, `u8` `tint51`, then the CRC list. The `doClient2222b32` and `doAudioStream2Part3122` bodies were not read. XTEA and the length patch follow (`Statics.java:2885`).

## State 90

One raw `u8` (`Statics.java:2905`):

| Code | Next |
| --- | --- |
| 21 | one more byte, delay `byte * 50`, fail 21 (`Statics.java:2985`) |
| 1 | state 106, transient (`Statics.java:2916`) |
| 52 | `u16be` length (state 222), then that many ISAAC-decoded bytes as a `jstr` opened in the browser (state 230, `Statics.java:3068`) |
| 2 | lobby goes to state 254; game world goes to state 145 (`Statics.java:2925`) |
| 15 | `u16be` length (state 204, `Statics.java:3329`), then `ObjIconFactory.doObjIconFactory` on that payload |
| 23 | retry from state 11 while `tint4` is below 3 (`Statics.java:2942`) |
| 42 | `u16be` into `tint13` (state 211, `Statics.java:3000`), then state 90 |
| 49 | with `tint70` 179 and `Client.tint72` other than 13, return (`Statics.java:2956`) |
| 35 | can set `tboolean2` and restart at state 11 (`Statics.java:2964`) |
| 53 | `u32be` (state 242), then fail 53 (`Statics.java:3011`) |
| other | fail with that code (`Statics.java:2972`) |

## Lobby list and state 145

State 254 reads a `u16be` length (`Statics.java:3093`). State 266 reads a leading `u8`. A value of 1 means the list continues. The rest of the buffer is `obj4.doDataBlock1874` records (`Statics.java:3114`). The record layout was not read. When the flag is 1 the client sends opcode 26 with an empty body and waits in state 131 (`Statics.java:3121`). Otherwise it returns to state 254.

State 131 reads one byte (`Statics.java:3130`). 2 continues to state 145. 29 or 45 go to state 192. Any other byte fails.

State 192 (`Statics.java:3026`): code 29 reads one byte; code 45 reads three bytes (`u8` plus `u16be`).

State 145 reads one `u8` into `tint32` (`Statics.java:3154`).

Lobby state 154 (`tint70` 285, `Statics.java:3164`) reads `tint32` bytes:

1. `u8` `Client.tint76`, `u8` `Client.tint77`.
2. Four `u8` flags, each compared with 1: `tboolean31`, `tboolean32`, `tboolean33`, `tboolean35`.
3. `u16be` `Client.tint75`, the local player index.
4. `u8` `tboolean29`.
5. `i24be` `Statics.tint73`.
6. `u8` `tboolean30`.
7. `jstr` `Statics.string`.

That `tboolean30` is passed into the loc, npc, and ping helpers (`Statics.java:3184`).

Game state 154 (`Statics.java:3188`) reads a different layout of `tint32` bytes:

1. `u8` `Client.tint76`, `u8` `Client.tint77`, `u8` `tboolean31`.
2. `i24be` `Statics.tint73`.
3. `u8` stored on `cls349728e6.tbyte`.
4. `u8` `tboolean32`, `u8` `tboolean33`.
5. `u64be` `tlong8`.
6. `u8+u32be` (`doPositionedSound2322b8`), subtracted from `tlong8` together with the current clock.
7. `u8` packed flags: bit 0 is `Client.tboolean29`, bit 1 is `tboolean7`.
8. `u32be` `tint163`, `u32be` `tint170`, `u16be` `tint50`, `u16be` `tint20`, `u16be` `tint144`, `u32be` `tint122`.
9. A thread is started with `tint122`.
10. `u8` `tint196`, `u16be` `tint55`, `u16be` `tint71`, `u8` `tboolean2`.
11. `vstr` display name.
12. `u8` `tint171`, `u32be` `tint19`.
13. `u16be` world id. 65535 becomes -1.
14. `vstr` host.

## First ISAAC packet

State 177 reads 3 bytes (`Statics.java:3273`). State 180 (`Statics.java:3282`) peeks the opcode. When the peek says a second opcode byte is required, it reads one more byte at index 3. `doPreferencesCpuUsage2` consumes the opcode. `doPositionedSound232` then reads a `u16be` length. That length is forced to `u16be` for this first packet. State 160 reads that many bytes (`Statics.java:3298`).

`ObjIconFactory.doObjIconFactory` consumes the local-player snapshot from the front of the payload (`Statics.java:3309`). The remainder is a rebuild. Opcode `cls1f645fa6162` (161) uses `DataBlock1914.cls23ed1d884`. Any other opcode uses `cls23ed1d883`. The offset must equal the length.

## Opcode 28 reconnect

`WorldMapRenderer` writes opcode 28 on the game socket (`com/jagex/game/world/WorldMapRenderer.java:204`):

1. `u8` opcode, `u16be` length placeholder, `u16be` 790, `u16be` 1.
2. RSA block `doWorldMapRenderer` (`WorldMapRenderer.java:282`): `u8` 10, four `u32be` seeds in `Statics.intArray8`, ten `u32be` random, one random written with `doPositionedSound2(int)` so it is a `u16be` of a random int, RSA with the login key.
3. `jstr` `Client.string5`, `u32be` `Client.tint`, `u32be` `Client.tint4`, `u32be` `Client.tint3`, `jstr` `Statics.string10`, `u8` language, `u8` modegame.
4. `GameShell.doGameShell`, unread.
5. `u8` 0 when `Client.string2` is null, otherwise 1 and the `jstr`.
6. `createHardwareBlock` written into the packet, unread.
7. `tint += 7`.
8. Ranged XTEA from after the RSA copy to the end, then the `u16be` length patch.

The response is one byte, looked up as a `GWCLocation`. Success is `GWCLocation.gWCLocation6` (`WorldMapRenderer.java:247`). On success the client installs ISAAC from the seeds and from the seeds plus 50 and calls `Client.doClient2222b6(16)` (`WorldMapRenderer.java:250`).

## JS5 handshake

`Client.java:2002` writes opcode 15 on a separate socket: `u8` opcode, `u8` length `9 + Client.string5.length()`, `u32be` 790, `u32be` 1, `jstr` `Client.string5`. The response byte must be 0. The client then reads `GraphNode.doGraphNode().length * 4` bytes and stores one `u32be` per entry (`Client.java:2032`). `DataBlock2372` is constructed with `bigInteger3` and `bigInteger4` (`Statics.java:3525`). Its verifier was not read.

`DataBlock2081` opcodes 23, 24, and 27 have no writer outside the enum.
