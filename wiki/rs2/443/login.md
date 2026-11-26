# Login

`Timer.opTimer5` is the login state machine (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:183`). The bytes below are raw. ISAAC starts only after the login block has been written.

## Ports

`Client.opClient162` sets the ports from `SecondaryLinkedList.valueSecondaryLinkedList21`, which `main` sets to 0 for `live`, 1 for `office`, and 2 for `local` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:485`). When that value is 0, the primary port is character `ꩊ` (43594) and the secondary port is 443. Otherwise the primary port is `40000 + worldId` and the secondary port is `50000 + worldId` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:542`). The socket opens on `ShapedTile.valueShapedTile9`, which starts as the primary port. A 2000-tick timeout or an `IOException` swaps the two ports once, then fails (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:367`).

## Handshake

State 2 writes two bytes and flushes them (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:216`):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 14 |
| 2 | u8 | `(usernameLong >> 16) & 31` |

State 3 reads one byte. A value other than 0 goes to the failure switch. State 4 reads 8 bytes as one u64be server key (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:251`).

## RSA block

The inner buffer (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:259`):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 10 |
| 2 | u32be | `(int)(Math.random() * 1.0E8)` |
| 3 | u32be | `(int)(Math.random() * 1.0E8)` |
| 4 | u32be | `(int)(serverKey >> 32)` |
| 5 | u32be | `(int)serverKey` |
| 6 | u32be | `ByteArrayNodeSecondary.valueByteArrayNodeSecondary4.valueLoc13` |
| 7 | u64be | username long |
| 8 | jstr | password |

`opMonotonicClock48` replaces that buffer with `u8 encryptedLength` plus the raw `BigInteger.modPow` bytes (`builds/rs2/443-2006-12-11/src/src/main/java/deob/MonotonicClock.java:720`). The modulus is `ReferenceNodeFactory.valueReferenceNodeFactory3` and the exponent is `SoundBank.valueSoundBank5` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/ReferenceNodeFactory.java:8`, `builds/rs2/443-2006-12-11/src/src/main/java/deob/SoundBank.java:15`).

## Outer block

`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:269`:

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 18 when the client state is 40, otherwise 16 |
| 2 | u8 | RSA block length plus 61 |
| 3 | u32be | 443 |
| 4 | u8 | 1 when `SpotAnimType.valueSpotAnimType63` is true, otherwise 0 |
| 5 | 14 × u32be | CRCs of js5 indices 0 through 13, in that order |
| 6 | bytes | the RSA block |

The added 61 is the revision int, the memory byte, and fourteen CRC ints. The four seed words then initialise the ciphers as described in [framing.md](framing.md).

## Response

State 6 reads one byte (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:305`):

| Byte | Result |
| --- | --- |
| 21, and client state 20 | state 7 |
| 2 | state 9 |
| 15, and client state 40 | `ModeratorOptionSpielerUrStundenStummSchalt.opkind1989` returns |
| 23, and `Cd2890cde.valuekind19413 < 1` | increment the retry and return to state 0 |
| any other | `WirVermutenDassJemandIhrPasswortKennt.opJs5CacheRequest1(code)` |

State 7 reads one byte and waits `60 * byte + 180` ticks (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:327`).

State 9, once 8 bytes are available (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:340`):

| Order | Width | Stored as |
| --- | --- | --- |
| 1 | u8 | `EmpfNger.valuekind1386` |
| 2 | u8 | members when the byte is 1 |
| 3 | u16be | `NameEingeben.valuekind1521`, two successive bytes |
| 4 | u8 | `TextureOp4.valueTextureOp410` |
| 5 | u8 ISAAC | first opcode |
| 6 | u16be | first payload size |

State 10 reads that payload, calls `NameDesGegenstandsEingeben.opkind1874`, and calls `Timer.opTimer4(false)` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:355`).

## Failure text

`opJs5CacheRequest1` selects three strings per code (`builds/rs2/443-2006-12-11/src/src/main/java/deob/WirVermutenDassJemandIhrPasswortKennt.java:61`). These literals are tied to a code from the field initialisers read with the switch:

| Code | Literal |
| --- | --- |
| 3 | `Invalid username or password` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/SpielerBitteHlenSieEineAndereWelt.java:20`) |
| 9 | `Too many connections from your address` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:27`) |
| 14 | `Der Server wird gerade aktualisiert` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/DerServerWirdGeradeAktualisiert.java:6`, assigned at `builds/rs2/443-2006-12-11/src/src/main/java/deob/Component.java:194`) |
| 23 | `No reply from loginserver` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:13`) |
| 24 | `Please contact customer support` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:13`) |

Timeout code `-3` and I/O code `-2` enter the same switch (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Timer.java:378`). The remaining codes in the if-chain still point at fields whose string literals were left unread. See [gaps.md](gaps.md).
