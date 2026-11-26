# Login

Source: `Client.login` at `builds/rs2/254-2004-09-07/src/src/main/java/jagex2/client/Client.java:4081`. The socket is `openSocket(43594 + portOffset)` (`Client.java:4091`; the character is U+AA4A). `openSocket` connects to the codebase host (`Client.java:2133`). Bytes below are what that method writes and reads. Multi-byte integers are big-endian (`Packet.p2` at `Packet.java:138`, `Packet.p4` at `Packet.java:159`, `Packet.g2` at `Packet.java:229`, `Packet.g8` at `Packet.java:254`). A `jstr` is raw bytes plus a trailing byte 10 (`Packet.pjstr` at `Packet.java:194`).

## Request

The client writes two bytes, then reads (`Client.java:4095`):

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | u8 | 14 |
| 2 | u8 | `(username base37 >> 16) & 31` (`Client.java:4093`) |

Then it discards 8 bytes (`Client.java:4099`) and reads one status byte (`Client.java:4103`). The 8 bytes are not interpreted.

If that status is 0, it reads 8 bytes as one u64be server seed (`Client.java:4105`).

It then builds an RSA plaintext in `out` (`Client.java:4109`):

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | u8 | 10 |
| 2 | u32be | random session word 0 |
| 3 | u32be | random session word 1 |
| 4 | u32be | seed high 32 bits |
| 5 | u32be | seed low 32 bits |
| 6 | u32be | `CouldNotOpenSocket.uid` |
| 7 | jstr | username |
| 8 | jstr | password |

`Packet.rsaenc` (`Packet.java:361`) replaces that buffer with `u8` length plus the big-endian RSA result of `modPow(LOGIN_RSAE, LOGIN_RSAN)`. The exponent is `58778699976184461502525193738213253649000149147835990136706041084440742975821` (`Client.java:540`). The modulus is `7162900525229798032761816791230527296329313291232324290237849263501208207972894053929065636522363163621000728841182238772712427862772219676577293600221789` (`Client.java:115`).

The bytes actually written (`Client.java:4119`) are:

| Order | Width | Meaning |
| --- | --- | --- |
| 1 | u8 | 16 for a normal login, 18 for a reconnect (`var3` true, `tryReconnect` at `Client.java:12463`) |
| 2 | u8 | `rsaBlobLength + 38` (`out.pos + 36 + 1 + 1`) |
| 3 | u8 | revision 254 |
| 4 | u8 | 1 if `lowMem`, else 0 |
| 5 | 9 times u32be | `jagChecksum[0]` through `jagChecksum[8]` |
| 6 | bytes | the RSA blob, including its own length byte |

## ISAAC seeds

Outbound ISAAC is seeded with the four session words (`Client.java:4135`). Each word is then increased by 50 (`Client.java:4137`) and that array seeds inbound ISAAC (`Client.java:4141`).

## Status byte

One u8 after the request, or after the login block when the first status was 0 (`Client.java:4143`).

| Value | What the client does | Citation |
| --- | --- | --- |
| 0 | Read the seed and send the login block. Not a final result. | `Client.java:4104` |
| 1 | Sleep 2 seconds and call `login` again. | `Client.java:4146` |
| 2 | Success. Next u8 is `staffmodlevel`. Next u8 is mouse-tracking enable (`== 1`). Sets `ingame`. | `Client.java:4153` |
| 3 | "Invalid username or password." | `Client.java:4257` |
| 4 | Account disabled. | `Client.java:4260` |
| 5 | Already logged in. | `Client.java:4263` |
| 6 | Client out of date. | `Client.java:4266` |
| 7 | World full. | `Client.java:4269` |
| 8 | Login server offline. | `Client.java:4272` |
| 9 | Login limit exceeded. | `Client.java:4275` |
| 10 | Bad session id. | `Client.java:4278` |
| 11 | Session rejected. The first message string is overwritten by the second. | `Client.java:4281` |
| 12 | Members account required. | `Client.java:4284` |
| 13 | Could not complete login. | `Client.java:4287` |
| 14 | Server updating. | `Client.java:4290` |
| 15 | Sets `ingame` without the success reset. Used while a profile transfers. | `Client.java:4292` |
| 16 | Login attempts exceeded. | `Client.java:4307` |
| 17 | Standing in a members area. | `Client.java:4310` |
| 18 | Not handled. Falls through to the unexpected-response text. | `Client.java:4316` |
| 19 | Not handled. Same fall-through. | `Client.java:4316` |
| 20 | Invalid login server. | `Client.java:4313` |
| 21 | Next u8 is a second count. The client waits that many seconds, then calls `login` again. | `Client.java:4326` |
| -1 | Read as a signed `stream.read()` of a closed stream: "No response from server". | `Client.java:4317` |
| other | "Unexpected server response". | `Client.java:4321` |

`ClientStream.read` returns one signed byte from the socket (`ClientStream.java:72`). Status 2's two follow-up bytes use the same call.
