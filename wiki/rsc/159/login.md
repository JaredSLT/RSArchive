# Login

Host defaults are `Client.worldHost` `127.0.0.1` and `Client.worldPort` 43594. `MudClient.main` overrides the host from argument 1 and the port from argument 2. Argument 0 `members` sets `flag182`. Application mode sets `flag54` false and `menuOptionTarget` to `../release/`. Applet mode keeps `flag54` true and, in `load`, forces port 43594 and then adds the applet parameter `poff` when that parameter parses.

`load` calls `setRsaKey` with `CLASSIC_RSA_EXPONENT` `18439792161837834709` and `CLASSIC_RSA_MODULUS` `192956484481579778191558061814292671521` (`MudClient.java:25-27`, `MudClient.java:844`). It sets `Client.number687` to 1000 and `Client.number686` to `Version.value1592` (159) (`MudClient.java:868-869`).

A login socket is a `ClientStream`. The constructor connects `worldHost:worldPort`, sets a 30000 ms socket timeout, and sets `TCP_NODELAY`. The stream's `number639` is set to 1000 before the session read.

## Session id

`Stream.call146` reads one `u32be`. `Client.call25f5d914` stores it in `number707` and prints it. Recovery opcode 4 reads it and discards the value. The RSA blocks use the stored id.

## Username, password, uid

`Tools.formatAuthString` keeps `a-z`, `A-Z`, and `0-9`, replaces every other character with `_`, and pads or truncates to the requested length with spaces. It does not lowercase (`Tools.java:124` onward).

`Tools.toBase37` (`Tools.java:172-206`) keeps letters and digits, lowercases `A-Z`, turns every other character into a space, trims, and cuts at 12 characters. Each character multiplies the accumulator by 37. `a-z` add `1 + ch - 97` (1 through 26). `0-9` add `27 + ch - 48` (27 through 36). A space adds nothing, so it is a zero digit. `fromBase37` (`Tools.java:209-232`) walks the long the other way: remainder 0 is a space, 1 through 26 are `A-Z` when the remaining value's next digit is 0 and `a-z` otherwise, and 27 through 36 are `0-9`. A negative long returns `invalid_name`.

`Client.callfa2171fd` (`Client.java:70-84`) reads applet parameter `ranseed`, parses the first 10 characters, and if that int is `987654321` replaces it with `u32be` from `uid.dat`. Any exception returns 0.

## Opcode 0 and opcode 19

`Client.call25f5d914` (`Client.java:88-207`). If `number708` is already above 0, the client shows the server-full strings and writes nothing. An empty trimmed username shows `loginResponses[0]` and `[1]` and writes nothing.

Otherwise it opens the socket, reads the session id, and reads applet `referid` or uses 0. Reconnect (`var3` true) writes opcode 19. A fresh login writes opcode 0. Then:

| Order | Field | Width |
| --- | --- | --- |
| 1 | `Client.number686` (159) | `u16be` |
| 2 | referid | `u16be` |
| 3 | `toBase37(formatAuthString(username, 20))` | `u64be` |
| 4 | password, already passed through `formatAuthString(..., 20)` | `rsa` |
| 5 | uid | `u32be` |

`flush` follows. The client discards one `call139` byte. The next `call139` byte is the response.

`rsaenc` (`Stream.java:202-232`) splits the password bytes into 7-byte groups. Each group is a 15-byte block: `[0]` random 1 through 127, `[1..3]` random bytes, `Tools.p4` of the session id at offset 4, `[8..14]` seven password bytes padded with 32. `BigInteger(1, block).modPow(exponent, modulus).toByteArray()` is written as `u8` length plus those bytes. A password longer than 7 bytes writes more than one block.

## Login response

| Byte | Result |
| --- | --- |
| 0 | `number694 = 0`, then `call2975af`. The override is `MudClient.call6b2975af` (`mudclient.e`, `MudClient.java:2459`), which sets `number508` to 1 |
| 1 | `number694 = 0`, then `Client.call215` (`Client.java:560`). No `mudclient.a` method exists, so the body stays empty |
| 3 | invalid username or password (`loginResponses[10]`, `[11]`) |
| 4 | username already in use (`[4]`, `[5]`) |
| 5 | client updated (`[16]`, `[17]`) |
| 6 | one character, ip already in use (`[18]`, `[19]`) |
| 7 | login attempts exceeded (`[20]`, `[21]`) |
| 11 | temporarily disabled (`[22]`, `[23]`) |
| 12 | permanently disabled (`[24]`, `[25]`) |
| 13 | username taken (`[14]`, `[15]`) |
| 14 | server full (`[8]`, `[9]`), and `number708 = 1500` |
| 15 | members account needed (`[26]`, `[27]`) |
| 16 | login to a members server (`[28]`, `[29]`) |
| other | unable to connect (`[12]`, `[13]`) |

A reconnect (`var3` true) that is not response 0 or 1 clears the stored username and password and calls `callea3e37`. The override is `MudClient.call81ea3e37` (`mudclient.f`, `MudClient.java:2452`), which sets `number320`, `number508`, and `number315` to 0. An exception while `number694` is still positive sleeps 5000 ms, decrements `number694`, and calls `call25f5d914` again. `tryLogout` sets `number694` to 10 and calls `call25f5d914` with the reconnect flag.

## Opcode 2, new account

`Client.call9e187c8f` (`Client.java:246-324`), called from the create-account submit in `MudClient.call11091c54` after the local checks. The same server-full guard applies. Fields:

| Order | Field | Width |
| --- | --- | --- |
| 1 | version 159 | `u16be` |
| 2 | `toBase37(formatAuthString(username, 20))` | `u64be` |
| 3 | referid | `u16be` |
| 4 | password | `rsa` |
| 5 | uid | `u32be` |

`flush`, discard one byte, then one response byte, then `close`. Response 2 calls `enterUsernamePassword`. The override is `MudClient.callc0bdeda4` (`mudclient.u`, `MudClient.java:2513`), which sets `number320` to 2 and copies the new username and password into the login fields. Responses 3 through 7 and 11 through 16 use the same strings as login. Any other byte uses the unable-to-connect strings.

## Opcode 4, recovery questions

From the login screen, `MudClient.java:2294-2320`. New socket, `number639 = 1000`, session id discarded. Opcode 4, `u64be` of `toBase37` of the formatted username, `flush`. Discard two `call139` bytes. The third byte is the question count signal. 0 shows that questions were not set and returns. Otherwise the client reads five questions: `u8` length, then that many bytes as a string (`gdata` into a 5000-byte array). `flag373` true blocks a second attempt with a one-hour message. Success moves `number320` to 3.

## Opcode 8, recovery submit

`MudClient.java:2361-2403`. New socket, session id kept. Opcode 8, then `u64be` username, `u32be` uid, `rsa` of `formatAuthString(old password, 20) + formatAuthString(new password, 20)`. Then five times: `u8` of `formatAlphaOnly(answer, 50).length()`, then `rsa` of that answer. `Tools.formatAlphaOnly` (`Tools.java:148-163`) lowercases and keeps `a-z` and `0-9`, stopping at the requested length. It does not pad.

`flush`, discard one byte, one response byte.

| Byte | Result |
| --- | --- |
| 0 | back to `number320` 2, recovery failed, `flag373` set |
| 1 | back to `number320` 2, password reset |
| other | back to `number320` 2, attempts exceeded |

## Opcode 1

`Client.calleade55a5` (`Client.java:210-222`) writes opcode 1 with an empty payload and `flush`, clears the stored username and password, and calls `callea3e37` (`MudClient.call81ea3e37`), which returns to the logged-out screen. `MudClient.callde2a6a6a` writes opcode 6, not opcode 1, for the idle logout. See [packets.md](packets.md).
