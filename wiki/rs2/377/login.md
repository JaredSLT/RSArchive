# Login

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`.

`Client.login` (`jagex2/client/Client.java:7630`) writes the handshake on `ClientStream` and consumes one response byte at a time with `opClientStream5` (`jagex2/io/ClientStream.java:74`). The socket is `openSocket('ꩊ' + valueClient1056)` (`Client.java:7640`). The character literal is U+AA4A. `valueClient1056` is argv port-offset (`Client.java:597`).

## Handshake

| step | bytes | citation |
| --- | --- | --- |
| client | `u8 14`, `u8 ((base37(name) >> 16) & 31)` | `Client.java:7644` |
| server | 8 bytes ignored | `Client.java:7648` |
| server | `u8` response | `Client.java:7652` |

`JString.toBase37` packs at most 12 characters (`jagex2/jstring/JString.java:13`).

When that response is 0, the client reads `u64be` server seed with `g8` (`Client.java:7655`, `jagex2/io/Packet.java:260`).

## Inner block

Built on `valueClient1096`, then replaced by `rsaenc` (`Client.java:7659`):

| order | width | value |
| --- | --- | --- |
| 1 | `u8` | 10 |
| 2 | `u32be` | random seed 0 |
| 3 | `u32be` | random seed 1 |
| 4 | `u32be` | high 32 bits of the server seed |
| 5 | `u32be` | low 32 bits of the server seed |
| 6 | `u32be` | `CouldNotOpenSocket.uid` (`deob/CouldNotOpenSocket.java:42`) |
| 7 | `jstr` | username, terminator byte 10 |
| 8 | `jstr` | password, terminator byte 10 |

`pjstr` writes platform bytes then 10 (`Packet.java:200`).

`rsaenc` runs only when its first argument is 0 (`Packet.java:379`). `valueClient1054` is declared at `Client.java:316` and is never assigned, so the call at `Client.java:7668` passes the Java int default 0 and the block is encrypted. Modulus is `valueClient973` (`Client.java:71`):

`7162900525229798032761816791230527296329313291232324290237849263501208207972894053929065636522363163621000728841182238772712427862772219676577293600221789`

Exponent is `valueClient1435` (`Client.java:586`):

`58778699976184461502525193738213253649000149147835990136706041084440742975821`

`modPow(exponent, modulus)` replaces the buffer with `u8` ciphertext length plus the ciphertext bytes (`Packet.java:381`). This revision has no RSA private key in the client.

## Outer block

`valueClient1061` (`Client.java:7669`):

| order | width | value |
| --- | --- | --- |
| 1 | `u8` | 18 on reconnect, else 16 |
| 2 | `u8` | `inner.pos + 36 + 1 + 1 + 2` (bytes after this length byte) |
| 3 | `u8` | 255 |
| 4 | `u16be` | 377 |
| 5 | `u8` | 1 when `valueClient1058` (lowmem), else 0 |
| 6 | nine `u32be` | `valueClient970[0..8]` CRCs |
| 7 | bytes | the RSA inner buffer |

ISAAC is created after this write. Outgoing seeds are the four ints above. Incoming seeds are those four plus 50 each (`Client.java:7686`). The constructor argument `valueClient1300` is `-89` (`Client.java:849`). `Isaac` throws when that argument is not negative (`jagex2/io/Isaac.java:25`).

## Response byte

Read after the outer write, or the handshake byte when it was not 0 (`Client.java:7694`).

| code | client behaviour | citation |
| --- | --- | --- |
| 0 | only as the handshake status above; the next byte is the login status | `Client.java:7654` |
| 1 | sleep 2000 ms and call `login` again | `Client.java:7697` |
| 2 | `u8` staff/mod into `valueClient999`, `u8` where 1 sets `valueClient1094`, then enter the game | `Client.java:7704` |
| 3 | invalid username or password | `Client.java:7813` |
| 4 | account disabled | `Client.java:7816` |
| 5 | already logged in | `Client.java:7819` |
| 6 | client updated | `Client.java:7822` |
| 7 | world full | `Client.java:7825` |
| 8 | login server offline | `Client.java:7828` |
| 9 | login limit | `Client.java:7831` |
| 10 | bad session id | `Client.java:7834` |
| 12 | members account required | `Client.java:7837` |
| 13 | could not complete login | `Client.java:7840` |
| 14 | server updating | `Client.java:7843` |
| 15 | reconnect: set `valueClient1264` and reset packet cursors, without the full state reset of code 2 | `Client.java:7846` |
| 16 | login attempts exceeded | `Client.java:7860` |
| 17 | members-only area | `Client.java:7863` |
| 18 | account locked | `Client.java:7866` |
| 20 | invalid loginserver | `Client.java:7869` |
| 21 | `u8` seconds, countdown text, sleep 1200 ms per step, then `login` again | `Client.java:7912` |
| 22 | malformed login packet | `Client.java:7873` |
| 23 | no reply from loginserver | `Client.java:7876` |
| 24 | profile load error | `Client.java:7879` |
| 25 | unexpected loginserver response | `Client.java:7882` |
| 26 | address blocked | `Client.java:7885` |
| -1 | if the handshake byte was 0 and `valueClient982 < 2`, sleep 2000 ms and retry; otherwise "no response" | `Client.java:7888` |
| other | "Unexpected server response" | `Client.java:7906` |

Code 11 and code 19 are not named. They fall through to the unexpected branch.

On code 2 the local player slot is `valueClient1101` (2047) and gender starts male (`valueClient1271 = true`) with five colours cleared (`Client.java:771`, `Client.java:7758`, `Client.java:7795`).
