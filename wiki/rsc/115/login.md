# Login

Host `Client.worldHost` starts as `"127.0.0.1"` and `text643` is `"66.28.11.53"` (`jagex/client/Client.java:15`). Port is `43594` (`Client.java:19`, assigned again in `MudClient.load` at `deob/MudClient.java:758`). An applet parameter `poff` is added to the port (`MudClient.java:764`). On a failed login that is not running as an applet, `worldHost` becomes `text643` (`Client.java:174`).

`Client.number640` is `Version.value1152` (115) (`MudClient.java:761`). `Client.number641` is 1000 and is copied onto `stream.number593` before the session read (`MudClient.java:760`, `Client.java:121`).

RSA keys set by `setRsaKey` in `load` (`MudClient.java:748`):

- exponent `18439792161837834709` (`MudClient.java:23`)
- modulus `192956484481579778191558061814292671521` (`MudClient.java:25`)

`formatAuthString` keeps `A-Z`, `a-z`, and `0-9`, replaces every other character with `_`, and pads with spaces to the requested length (`jagex/Tools.java:136`). `toBase37` lowercases, turns other characters into spaces, trims, keeps 12 characters, and encodes `a-z` as 1..26 and `0-9` as 27..36, multiplying by 37 for every character including spaces (`Tools.java:179`).

The uid is `callfa2171fd` (`Client.java:72`): applet parameter `ranseed`, first 10 characters parsed as an int. If that int is 987654321, four bytes are read from `uid.dat` with `Tools.g4` (u32be). Any exception returns 0.

## Session and frame

`ClientStream` opens a TCP socket, `setSoTimeout(30000)`, `setTcpNoDelay(true)` (`jagex/client/ClientStream.java:33`). `call130` is one `InputStream.read()` byte (`ClientStream.java:85`). `call135` is two of those, high byte first (`Stream.java:92`). `call136` is two `call135` values, high half first, so the session id is u32be (`Stream.java:99`).

The session id is read before the login frame is written (`Client.java:122`). Login responses are raw socket bytes. They are not `readPacket` frames.

`Stream.rsaenc` splits the password into chunks of 7 bytes (`Stream.java:216`). Each plaintext block is 15 bytes:

| offset | bytes |
| --- | --- |
| 0 | random 1..127 |
| 1..3 | random 0..255 |
| 4..7 | session id, `Tools.p4`, u32be (`Tools.java:61`) |
| 8..14 | seven password bytes, or `0x20` when the password is shorter |

`BigInteger(1, block).modPow(exponent, modulus).toByteArray()` is written as `u8` length plus those bytes. `toByteArray` can include a sign byte, so the length is the array length.

## Opcode 0 and opcode 19

`Client.call25f5d914` (`Client.java:90`). Reconnect (`var3` true) writes opcode 19 (`Client.java:126`). A fresh login writes opcode 0 (`Client.java:128`).

| order | field |
| --- | --- |
| 1 | opcode |
| 2 | `p2(number640)` u16be version 115 |
| 3 | `p8(toBase37(username))` u64be |
| 4 | `rsaenc(password, sessionId)` |
| 5 | `p4(uid)` u32be |

`flush` sends the frame (`Client.java:135`). The client then discards one `call130` byte (`Client.java:136`) and reads the response as one `call130` byte (`Client.java:137`). The discarded byte has no branch in this method. See `gaps.md`.

If `number663 > 0` the method shows the server-full strings and returns before connecting (`Client.java:91`).

| response | effect | citation |
| --- | --- | --- |
| 0 | `number649 = 0`, `call2975af` | `Client.java:139` |
| 1 | `number649 = 0`, `call207` | `Client.java:142` |
| reconnect and any other code | clear the names and `callea3e37` | `Client.java:145` |
| 3 | invalid username or password | `Client.java:149`, strings `Client.java:582` |
| 4 | username already in use | `Client.java:151`, strings `Client.java:576` |
| 5 | client updated | `Client.java:153`, strings `Client.java:589` |
| 6 | one character / IP already in use | `Client.java:155`, strings `Client.java:591` |
| 7 | login attempts exceeded | `Client.java:157`, strings `Client.java:594` |
| 11 | temporarily disabled | `Client.java:159`, strings `Client.java:596` |
| 12 | permanently disabled | `Client.java:161`, strings `Client.java:598` |
| 13 | username already taken | `Client.java:163`, strings `Client.java:587` |
| 14 | server full, `number663 = 1500` | `Client.java:165`, strings `Client.java:580` |
| other | unable to connect | `Client.java:168`, strings `Client.java:585` |

`MudClient` overrides `call2975af` as `call6b2975af` (`mudclient.g`, `MudClient.java:2151`): `menuOptionType = 1`, entities cleared, no start tile stored. `Client.call207` (`jagex/client/e.c`, `Client.java:536`) is empty and `MudClient` does not override it. Reconnect failure calls `callea3e37`, overridden as `call81ea3e37` (`mudclient.h`, `MudClient.java:2144`): `number298 = 0`, `menuOptionType = 0`, `number293 = 0`. An exception retries while `number649 > 0` (`Client.java:178`).

`Client.tryLogout` sets `number649 = 10` and calls `call25f5d914` with reconnect true, which writes opcode 19 (`Client.java:216`). `MudClient.tryLogout` calls that only when `number293 == 0`; otherwise it calls `call81ea3e37` (`MudClient.java:2135`).

## Opcode 2 new account

`Client.call9e187c8f` (`Client.java:237`). Same session read, then opcode 2, `p2` version, `p8` username, `rsaenc` password, `p4` uid, `flush` (`Client.java:261`). One `call130` byte is discarded (`Client.java:267`) and one response byte is read (`Client.java:268`). The stream is closed (`Client.java:269`).

Response 2 calls `enterUsernamePassword`, overridden as `MudClient.callc0bdeda4` (`MudClient.java:2205`), which moves to the login screen and calls `call25f5d914(..., false)`. Codes 3, 4, 5, 6, 7, 11, 12, 13, and 14 use the same string table as login. Code 14 also sets `number663 = 1500`.

## Opcode 4 and opcode 8 recovery

Opcode 4, `MudClient.java:1987`: connect, copy `number593`, discard `call136()`, opcode 4, `p8(toBase37(username))`, `flush`, discard `call135()` (u16be), read one response byte.

Response 0 shows that recovery questions are unset. Any other value reads five questions: `u8` length, `gdata` of that many bytes, `new String`. A question that starts with `~:` is an index into `whatFirstComputerGame` (`MudClient.java:1998`, question table `MudClient.java:8532`).

Opcode 8, `MudClient.java:2068`: session `call136` kept, opcode 8, `p8` username, `p4` uid, `rsaenc` of the old password and the new password each passed through `formatAuthString` to 20 and concatenated, then five `Stream.call146` blocks of `Tools.call174` hashes (`Tools.java:160`, block layout in `framing.md`), `flush`, discard one byte, read one byte. 0 fails and sets `flag351`. 1 succeeds. Any other value is "attempts exceeded".

## Opcode 25 password change

`Client.call9bc8e4cb` (`Client.java:425`): opcode 25, `rsaenc(formatAuthString(user, 20) + formatAuthString(pass, 20), number662)`, then `call60c8c054` with no trailing response read in this method.
