# Login

Host defaults to `127.0.0.1` and port `43594` (`jagex/client/Client.java:16`, `jagex/client/Client.java:18`). `main` may replace both from args (`deob/MudClient.java:800`). An applet forces port 43594, then adds applet param `poff` when that param parses (`deob/MudClient.java:850`). `Client.number360` is set to `Version.CLIENT` (177) in `load` (`deob/MudClient.java:856`, `deob/Version.java:6`).

RSA keys set in the `MudClient` constructor and copied in `load` via `setRsaKey` (`deob/MudClient.java:15225`, `deob/MudClient.java:822`):

- exponent `18439792161837834709`
- modulus `192956484481579778191558061814292671521`

`MudClient.getUid` returns `Link.uid` (`deob/MudClient.java:15145`, `deob/Link.java:13`). `Client.getUid` returns 0 (`jagex/client/Client.java:73`). Login calls `this.getUid()`, so the override is the one used. No writer of `Link.uid` appears in `Link.java`. The field stays at the Java default 0 unless another class assigns it.

User and password are passed through `Tools.formatAuthString` with length 20 before they are written (`jagex/client/Client.java:91`).

## Session id

After the socket opens, the server sends 4 raw bytes. `Stream.g4` reads them big-endian (`jagex/Stream.java:117`, `jagex/client/Client.java:111`). This is not inside `readPacket` framing. The value is `sessionId`.

## Opcode 0 login and opcode 19 reconnect

`Client.call25f5d914` (`jagex/client/Client.java:78`). Reconnect (`var3` true) writes opcode 19. A fresh login writes opcode 0.

| order | width | source |
| --- | --- | --- |
| opcode | framed u8, threat 625 for opcode 0, threat 712 for opcode 19 | `jagex/client/Client.java:130`, `jagex/client/Client.java:136` |
| version | u16be `number360` | `jagex/client/Client.java:139` |
| referid | u16be. Applet param `referid`, plus 50 when applet param `limit30` is `"1"`. Otherwise 0. | `jagex/client/Client.java:117`, `jagex/client/Client.java:140` |
| username | u64be base37 | `jagex/client/Client.java:141` |
| password | `rsaenc(password, sessionId)` | `jagex/client/Client.java:142` |
| uid | u32be `getUid()` | `jagex/client/Client.java:143` |

Then `flush`. Then one discarded `read()`, then one response `read()`, then `g1spooky` with `SERVERPROT_ENCRYPTION` (`jagex/client/Client.java:145`). The response is not a `readPacket` frame.

`rsaenc` builds 15-byte blocks, 7 password bytes each (`jagex/Stream.java:218`). Each block is `u8` nonzero-biased random, 3 random bytes, session id as big-endian u32 at offset 4 (`Tools.p4`), then 7 password bytes padded with 32. `BigInteger(1, block).modPow(exponent, modulus).toByteArray()` is written as `u8` length plus that many bytes. The length is whatever `toByteArray` returns.

## Response codes

Strings are `Client.loginResponses` (`jagex/client/Client.java:665`).

| code | result |
| --- | --- |
| 0 | `call488`. `MudClient` override `call6b2975af` sets `number384 = 1` and enters the game (`deob/MudClient.java:2722`). |
| 1 | `call487`. The `Client` method is empty (`jagex/client/Client.java:628`). |
| 3 | invalid username or password |
| 4 | username already in use |
| 5 | client updated |
| 6 | one character / IP already in use |
| 7 | login attempts exceeded |
| 11 | account temporarily disabled |
| 12 | account permanently disabled |
| 13 | username already taken |
| 14 | server full. Sets `number371 = 1500`. |
| 15 | members account needed |
| 16 | log in to a members server |
| other | unable to connect |

A reconnect that fails clears the credentials and calls `call489` (`jagex/client/Client.java:155`).

## Opcode 2 new player

`Client.call9e187c8f` (`jagex/client/Client.java:249`). Same raw `g4` session id. Field order differs from login:

| order | width |
| --- | --- |
| opcode 2, threat 129 | framed |
| version | u16be |
| username | u64be base37 |
| referid | u16be, same applet rule |
| password | `rsaenc` |
| uid | u32be |

`flush`, one discarded `read()`, one response `read()`, `close`, then `g1spooky` (`jagex/client/Client.java:288`). Code 2 calls `enterUsernamePassword`. Codes 3 through 16 match the login strings above. Other codes are unable to connect.

## Opcode 4 password questions

`MudClient` login screen `number513 == 2` (`deob/MudClient.java:2538`). Connect, discard one `g4`, opcode 4 threat 848, u64be base37 username, `flush`. Two discarded `read()` calls, then one response byte, `g1spooky` (`deob/MudClient.java:2541`).

Code 0 means no questions. Any other code reads 5 questions: `u8` length, then that many raw bytes (`deob/MudClient.java:2565`).

## Opcode 8 recover

`number513 == 3` (`deob/MudClient.java:2620`). Raw `g4` session id. Opcode 8 threat 121. u64be username, u32be uid, `rsaenc` of the old-answer candidate concatenated with the new password, then 5 times `u8` answer length and `rsaenc` of that answer (`deob/MudClient.java:2625`). `flush`, one discarded `read()`, one response byte, `g1spooky`.

| code | result |
| --- | --- |
| 0 | fail, `flag101`, retry in 1 hour |
| 1 | password reset |
| other | attempts exceeded |

## Opcode 1 logout

`Client.calleade55a5` writes opcode 1 threat 325 and `flush`, then clears credentials (`jagex/client/Client.java:213`). In-game logout request is opcode 6, not opcode 1. See [packets](packets.md).
