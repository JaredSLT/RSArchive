# Login

`Client.opClient284` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2414`) is the JS5 connect state machine. The state is `C107b28c0.valuekind21861 * -1817801821`. The game-login RSA block and the login response codes other than the one handshake byte below were not read. `Cf1c47083.opkind27025` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf1c47083.java:32`) is the RSA method and could not be decompiled. `HdEb.valuekind25851` is `new BigInteger("10001", 16)` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/HdEb.java:47`). Which modulus is applied on the wire was not read. `C6cf5b211.opkind25941` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C6cf5b211.java:106`) and `FuLb.opkind27968` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/FuLb.java:39`) have no callers in this source tree.

## State 0

Opens a socket with `UdciEtqgtUxyt.opkind24642` on `C188edb50.valuekind30811` and advances the state (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2464`). The open method was not read.

## State 1, bytes written

`Eir.opkind27462` is called with 25000 and `-886` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2470`). That method was not read. The buffer is `C3cae535d.opkind21655` with capacity `hostname.length() + 9 + 2`.

| order | width | value | citation |
| --- | --- | --- | --- |
| 1 | u8 | 15. `Pgo.opkind24522` of `C2ba3dcee.valuekind24696`, built by `C6140e91a.opkind27072(15, -18606, ..., -1)` and decoded with `* -1673233645`. | builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2473, builds/rs2/695-2011-12-22/src/src/main/java/deob/C2ba3dcee.java:106 |
| 2 | u8 | `SipRx.valuekind27223.length() + 9` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2471 |
| 3 | u32be | 695 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2475 |
| 4 | u32be | 1 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2476 |
| 5 | jstr | `SipRx.valuekind27223`. Charset unread (`Awd.opkind30402`). A trailing u8 0 is written. | builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2477 |

`Cc9967dcd.opkind27496` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc9967dcd.java:10`) is then asked to send `var2 + 2` bytes and could not be decompiled. The length byte equals `8 + (String.length()+1)`, which is the two u32be fields plus one byte per Java char plus the terminator, if the unread encoder writes one byte per char.

This u8 15 is a JS5 id object. It is not c2s opcode 15.

## State 2, one response byte

`Wal.opkind27476` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Wal.java:144`) is supposed to wait for 1 byte and could not be decompiled. It returns false, so the decompiled branch does not enter the read. The read that follows the true branch copies 1 byte through `Dir.opkind23327` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Dir.java:38`) into `byte[1]`. `Dir.opkind23327` forwards to `Ays.opkind30634`, which was not read. When `var4[0] != 0`, `Client.opClient278(24282, var5)` runs (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2496`). `var5` is the return value of the copy, and `opClient278` was not read. Byte 0 advances the state (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2501`).

## State 3, 27 CRCs

The slot list has 27 entries (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2506`). The client waits for `4 * 27` bytes, copies them, and for each slot calls `Xejo.opkind24355` with `Cf9661093.opkind20932` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2517`). Each value is a u32be. Archive index numbers are not literals in that list (`cache.md`).

On earlier failure paths in the same method the client stores `Eak.valuekind25162 = -1212340779` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2429`). The decoded login state of that constant was not computed here.

`Client.init` passes the revision literal 695 into `opkind248617` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:598`).
