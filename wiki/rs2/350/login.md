# Login

`Client.login` (`builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:11346`) writes the handshake and the login block. The socket is `openSocket` of code point U+AA4A (43594) plus `valueClient685` (`Client.java:11356`). Widths are `C9cd2e2e1` methods cited in [framing.md](framing.md).

## Handshake

1. `u8` 14 (`Client.java:11360`).
2. `u8` name hash: `(JString.opkind1141(username) >> 16) & 31` (`Client.java:11357`).
3. Those two bytes are written (`Client.java:11362`).
4. Eight response bytes are read and discarded (`Client.java:11364`).
5. One `u8` response (`Client.java:11368`).

On response 0 the client reads `u64be` (`opkind8719`) into `valueClient840` (`Client.java:11371`).

## RSA block

Built in `valueClient739` (`Client.java:11375`):

1. `u8` 10.
2. `u32be` random.
3. `u32be` random.
4. `u32be` high half of the server seed.
5. `u32be` low half of the server seed.
6. `u32be` `CouldNotOpenSocket.uid`.
7. `jstr` username.
8. `jstr` password.
9. `opkind8728(valueClient726, valueClient515)` (`Client.java:11384`). The first argument is the exponent and the second is the modulus (`C9cd2e2e1.java:374`). The method rewinds, encrypts with `modPow`, then writes `u8` length and that many bytes.

Exponent `valueClient726` is `58778699976184461502525193738213253649000149147835990136706041084440742975821` (`Client.java:532`). Modulus `valueClient515` is `7162900525229798032761816791230527296329313291232324290237849263501208207972894053929065636522363163621000728841182238772712427862772219676577293600221789` (`Client.java:525`).

## Outer block

Written to `valueClient775` (`Client.java:11385`):

1. `u8` 18 on reconnect, otherwise `u8` 16.
2. `u8` equal to the RSA buffer length plus 36 plus 1 plus 1 plus 2.
3. `u8` 255.
4. `u16be` 350.
5. `u8` 1 when `valueClient687` (low memory) is set, otherwise 0.
6. Nine `u32be` values `valueClient914[0]` through `valueClient914[8]`.
7. The RSA bytes.

The four seed integers seed the outbound ISAAC. Each word is then increased by 50 and those words seed the inbound ISAAC (`Client.java:11402`).

## Response bytes

One `u8` after the outer block (`Client.java:11410`). Response 0 from the handshake can fall through into this same `u8` when the server did not return 0 on the first byte.

| byte | client text |
| --- | --- |
| 1 | retry after 2000 ms (`Client.java:11413`) |
| 2 | success: next `u8` is `valueClient891`, next `u8` equal to 1 sets `valueClient781` (`Client.java:11421`) |
| 3 | invalid username or password (`Client.java:11532`) |
| 4 | account disabled (`Client.java:11534`) |
| 5 | already logged in (`Client.java:11537`) |
| 6 | RuneScape updated (`Client.java:11540`) |
| 7 | world full (`Client.java:11543`) |
| 8 | login server offline (`Client.java:11546`) |
| 9 | login limit exceeded (`Client.java:11549`) |
| 10 | bad session id (`Client.java:11552`) |
| 11 | login server rejected session (`Client.java:11555`) |
| 12 | members account required (`Client.java:11558`) |
| 13 | could not complete login (`Client.java:11561`) |
| 14 | server being updated (`Client.java:11564`) |
| 15 | reconnect accepted; packet state is reset and `valueClient654` is set without the success init (`Client.java:11567`) |
| 16 | login attempts exceeded (`Client.java:11581`) |
| 17 | members-only area (`Client.java:11584`) |
| 20 | invalid loginserver (`Client.java:11587`) |
| 21 | one more `u8` of seconds; the client counts down and logs in again (`Client.java:11618`) |
| 22 | malformed login packet (`Client.java:11591`) |
| -1 | no response; the first handshake retry path differs from a later failure (`Client.java:11594`) |

Any other byte prints `response:` and shows an unexpected-server-response string (`Client.java:11613`).
