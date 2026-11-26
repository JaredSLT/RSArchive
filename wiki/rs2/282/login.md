# Login

Game packets use ISAAC. The login exchange does not. Multi-byte integers in this exchange are big-endian. Strings are `jstr`: raw bytes followed by terminator byte 10 (`Packet.opkind2710`).

Port is `43594 + valueClient147`. The character `'ꩊ'` is U+AA4A. `valueClient147` is `main` argument 1, the port offset. The socket is a `ClientStream` (`ClientStream.write` at `builds/rs2/282-2004-12-21/src/src/main/java/deob/ClientStream.java:94`).

Citations are `builds/rs2/282-2004-12-21/src/src/main/java/deob/Client.java` unless another file is named.

## Bytes written

1. Connect, then write two raw bytes, not ISAAC: `u8 14`, `u8 ((JString.opkind561(username) >> 16) & 31)`. `Client.java:10315` and `:10319`.
2. The name hash is base-37, at most 12 characters. `A`/`a` is 1 through `Z`/`z` is 26, `0` through `9` is 27 through 36. Trailing factors of 37 are divided out. `JString.java:12`.
3. After the server accepts the handshake, build the RSA block in `valueClient281` (`Client.java:10333`):
   - `u8 10`
   - `u32be` random seed 0
   - `u32be` random seed 1
   - `u32be` server seed high (`valueClient271 >> 32`)
   - `u32be` server seed low
   - `u32be` `CouldNotOpenSocket.uid`
   - `jstr` username
   - `jstr` password
   - `Packet.opkind2728(valueClient58, valueClient51)` replaces that buffer with `u8` length plus the unsigned big-endian `modPow` bytes. The first argument is the exponent and the second is the modulus (`Packet.java:363`).
4. Exponent `valueClient58` is `58778699976184461502525193738213253649000149147835990136706041084440742975821`. Modulus `valueClient51` is `7162900525229798032761816791230527296329313291232324290237849263501208207972894053929065636522363163621000728841182238772712427862772219676577293600221789`. `Client.java:67`.
5. Outer buffer `valueClient412` (`Client.java:10343`):
   - `u8 18` on reconnect, otherwise `u8 16`
   - `u8` (`rsaBlockLength + 36 + 1 + 1 + 2`)
   - `u8 255`
   - `u16be 282`
   - `u8` 1 when `valueClient149` (lowmem), else 0
   - nine `u32be` archive checksums `valueClient460[0]` through `valueClient460[8]`
   - the RSA bytes
6. Seed the C2S Isaac with the four ints. Seed the S2C Isaac with each of those ints plus 50. Then write the outer buffer (`Client.java:10360`).

## Bytes consumed

1. Discard 8 bytes (`Client.java:10323`).
2. Read `u8` response. If it is not 0, skip the seed and the login block.
3. If it is 0, read 8 bytes as `u64be` server seed (`Packet.opkind2719`, two `u32be`) into `valueClient271` (`Client.java:10329`).
4. After the outer buffer is written, read one `u8` status (`Client.java:10368`).

| status | bytes after the status | result |
| --- | --- | --- |
| 1 | none | sleep 2 seconds and call `login` again |
| 2 | `u8` rights into `valueClient295`, `u8` that sets `valueClient183` when it equals 1 | in game |
| 3 | none | invalid username or password |
| 4 | none | account disabled |
| 5 | none | already logged in |
| 6 | none | client updated |
| 7 | none | world full |
| 8 | none | login server offline |
| 9 | none | login limit |
| 10 | none | bad session id |
| 11 | none | `valueClient172` is assigned `"Login server rejected session."` and then overwritten with `"Please try again."` |
| 12 | none | members account required |
| 13 | none | could not complete login |
| 14 | none | server updating |
| 15 | none | sets `valueClient411` and clears packet state; does not read the two success bytes of status 2 |
| 16 | none | attempts exceeded |
| 17 | none | members-only area |
| 20 | none | invalid loginserver |
| 21 | `u8` seconds | countdown, one second per remaining count, then `login` again |
| -1 | none | no response |
| other | none | unexpected response |

Status 2 is the normal success path (`Client.java:10378`). Status 15 is the reconnect success path (`Client.java:10518`).
