# Login

`Client.login` opens a socket to `'ꩊ' + portOffset`. That character is U+AA4A, decimal 43594. The socket is a `ClientStream`: 30 second so-timeout, TCP no delay. `read()` returns the raw `InputStream.read()` value, 0..255 or -1. Multi-byte reads loop until the count is filled or throw `EOF`.

Citations: `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:6949`, `Client.java:6959`, `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/io/ClientStream.java:26`, `ClientStream.java:69`.

## Name hash

`JString.toBase37` keeps at most 12 characters. Each step multiplies by 37 and adds 1..26 for letters or 27..36 for digits. Trailing factors of 37 are divided off. The charset is in `JString.base37_charset`.

Citation: `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/jstring/JString.java:10`, `JString.java:13`.

## Bytes written before the status

1. `u8` 14, `u8` `(nameBase37 >> 16) & 31`. Two bytes are written. `Client.java:6963`.
2. Eight `read()` results are discarded. `Client.java:6967`.
3. One `u8` status. If it is 0, eight more bytes are read and `g8()` takes the server seed as two big-endian `i32` halves. `Client.java:6971`, `Packet.java:258`.
4. The ISAAC seed is four `i32` values: two `Math.random` draws in `0 .. 99999999`, then `(int)(seed >> 32)`, then `(int)seed`. `Client.java:6977`.
5. The RSA block, before encryption: `u8` 10, four `i32be` seeds, `i32be` `CouldNotOpenSocket.uid`, `jstr` username, `jstr` password. `jstr` is raw bytes plus terminator 10. `Client.java:6979`, `Packet.java:198`.
6. `Packet.rsaenc` replaces that block with `u8` ciphertext length and the ciphertext bytes of `block.modPow(exponent, modulus)`. `Packet.java:373`.
7. Public exponent, from the static initializer: `58778699976184461502525193738213253649000149147835990136706041084440742975821`. `Client.java:546`.
8. Public modulus: `7162900525229798032761816791230527296329313291232324290237849263501208207972894053929065636522363163621000728841182238772712427862772219676577293600221789`. `Client.java:71`.
9. The outer buffer: `u8` 18 on reconnect, otherwise `u8` 16. Then `u8` `rsaBlock.pos + 36 + 1 + 1 + 2`. Then `u8` 255, `u16be` 317, `u8` 1 when `lowMemory` else 0, nine `i32be` `archiveChecksum[0]` through `[8]`, then the RSA block bytes. That whole buffer is written. `Client.java:6988`.

The length byte is the count of every byte after it. `rsaenc` has already stored the RSA block length in `out.pos`, and `36 + 1 + 1 + 2` is the checksums, the 255 byte, the low-memory byte, and the revision.

## ISAAC

The outbound generator is `new Isaac` on the four seed ints. The inbound generator is a second `Isaac` on those ints after each has had 50 added. `Client.java:7005`. `Isaac.nextInt` returns `rsl[count]` and decrements; at zero it runs `isaac()` and sets `count` to 255. `Isaac.java:34`. The `init` mixing that fills `mem` and `rsl` from the seed is unread; see `gaps.md`.

## Status byte

The same `u8` is consumed again after the login block when the earlier status was 0. `Client.java:7013`.

| Value | What the client does |
| --- | --- |
| 1 | Sleep 2000 ms and call `login` again. |
| 2 | Read `u8` rights and `u8` flagged (`== 1`). Enter the game. Selected tab becomes 3. `validateCharacterDesign` runs. |
| 3 | Invalid username or password. |
| 4 | Account disabled. |
| 5 | Already logged in. |
| 6 | Client out of date. |
| 7 | World full. |
| 8 | Login server offline. |
| 9 | Login limit exceeded. |
| 10 | Bad session id. |
| 11 | Rejected session. The second message string replaces the first. |
| 12 | Members account required. |
| 13 | Could not complete login. |
| 14 | Server updating. |
| 15 | Reconnect success. `ingame` is set. No rights byte is read. |
| 16 | Attempts exceeded. |
| 17 | Standing in a members-only area. |
| 20 | Invalid loginserver. |
| 21 | Read `u8` seconds and count it down, then call `login` again. |
| -1 | If the pre-block status was 0 and `loginAttempts` is below 2, sleep 2000 ms and retry. Otherwise no response. |
| other | Unexpected server response. |

An `IOException` sets the message `Error connecting to server.` `Client.java:7016` through `Client.java:7225`.
