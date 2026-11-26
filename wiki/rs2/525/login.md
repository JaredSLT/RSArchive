# Login

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`.

The writer is `ObjStack.opObjStack20` (`deob/ObjStack.java:158`). State is `Wall.valueWall41`. The game socket is `Cd6f54da9.valuekind4523.opSound24(GlTile.valueGlTile48, PolicesChargBes.valuekind51410)` (`ObjStack.java:184`).

## Ports

`Client.opClient195` (`Client.java:692`):

- Live (`ExpiresThuJanGmtMaxAge.valuekind4445 == 0`): game port 43594, JS5 port 443, host `getCodeBase().getHost()` (`Client.java:701-704`).
- Local (`== 2`): host `127.0.0.1`, game port `40000 + Broulement.valuekind5402`, JS5 port `50000 + Broulement.valuekind5402`. The source chars are U+9C40 and U+C350 (`Client.java:706-709`).
- The other non-zero mode uses the codebase host, the same `+ 40000` game port and `+ 50000` JS5 port (`Client.java:711-714`).

`PolicesChargBes.valuekind51410` starts as the game port and flips to the JS5 port when a connect fails (`ObjStack.java:176-179`, `Client.java:1008-1011`).

## JS5 handshake

`Client.call81a680bc` (`Client.java:1005`), before ISAAC. Five raw bytes: `u8` 15, `u32be` 525 (`Client.java:1069-1072`). Response is one raw `u8`. 0 continues. Any other value is passed to `opClient201` (`Client.java:1084-1087`). Two or more failures whose stored code is 6 raise `js5connect_outofdate` (`Client.java:1018-1020`). Four or more failures, and only while the game state is 0 or 5, raise `js5connect_full` for code 7 or 9, `js5connect` for any other code above 0, and `js5io` otherwise (`Client.java:1030-1038`). Four or more failures whose stored code is -1 raise `js5crc` (`Client.java:1024-1026`). `Ce4af6bb6.opkind4125` stores that -1 when a JS5 container check throws (`Ce4af6bb6.java:140-141`, from `JavaMouseWheel.java:126` and `JavaMouseWheel.java:408`).

## Game login, state 2

Raw, on `SoftReferenceNode.valueSoftReferenceNode170` (`ObjStack.java:200-204`):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 14 |
| 2 | u8 | `(usernameLong >>> 16) & 31` |

`usernameLong` is `SpielweltErstellt.valuekind4549.opkind36756()`. The server answers one raw `u8` via `opkind5517`. Non-zero stores that byte as `C90133226.valuekind4463` and drops the socket (`ObjStack.java:213-227`).

## State 3, server seed and RSA block

Need 8 bytes. Read as `u64be` (`opObjStackNode78`) into `C37c92f88.valuekind4933` (`ObjStack.java:233-241`).

ISAAC seed `int[4]` (`ObjStack.java:238-245`):

| Index | Value |
| --- | --- |
| 0 | `(int)(Math.random() * 1.0E8)` |
| 1 | `(int)(Math.random() * 1.0E8)` |
| 2 | `(int)(serverSeed >>> 32)` |
| 3 | `(int)serverSeed` |

Inner buffer, then `opObjStackNode32(Ce8ab9076.valuekind4981, Loc.valueLoc55)` (`ObjStack.java:246-254`). `opObjStackNode32` (`ObjStackNode.java:93-103`) replaces the buffer with `u8` ciphertext length plus the `modPow` bytes.

| Order | Write | Value |
| --- | --- | --- |
| 1 | u8 | 10 |
| 2 | u32be | seed[0] |
| 3 | u32be | seed[1] |
| 4 | u32be | seed[2] |
| 5 | u32be | seed[3] |
| 6 | u64be | username long |
| 7 | jstr | password `SpielweltErstellt.valuekind45410` |
| 8 | rsa | modulus `Ce8ab9076.valuekind4981`, exponent `Loc.valueLoc55` |

## Outer login packet

`TitelbildGeOffnet.valuekind5322` (`ObjStack.java:255-306`). Opcode `u8` is 18 when `C37c92f88.valuekind49330 == 40`, otherwise 16.

The following `u16be` is `159 + rsaBlockLength + C05b1c6f1.opkind5453(settingsString)`. `opkind5453` (`C05b1c6f1.java:59-61`) is `1 + string.opkind36761()`, the jstr byte count including the terminator. `rsaBlockLength` is the post-RSA inner position (length byte plus ciphertext).

Bytes after that `u16be`, which sum to 157 plus the jstr plus the RSA block (157 + 2 = 159, so the `u16be` counts itself):

| Order | Write | Value |
| --- | --- | --- |
| 1 | u32be | 525 |
| 2 | u8 | `C5b2564ca.valuekind55912` |
| 3 | u8 | 1 if `ConnexionPerdue.valuekind5348`, else 0 |
| 4 | u8 | 0 |
| 5 | u8 | `Ced8d3620.opkind4221()` |
| 6 | u16be | canvas width `C09b74607.valuekind50410` |
| 7 | u16be | canvas height `Cbb5a2b0d.valuekind4684` |
| 8 | u8 | `Cc0d52e40.valuekind5631` |
| 9 | bytes(24) | `MouseRecorder.opMouseRecorder65` (`MouseRecorder.java:17-38`): 24 bytes from the uid file, or 24 bytes of `-1` on failure |
| 10 | jstr | `C7c1906f4.valuekind57214` |
| 11 | u32be | `VorbisMapping.valueVorbisMapping9` |
| 12 | u32be | `Cd50b3810.opkind5644()` |
| 13 | u16be | `MapElementList.valueMapElementList61` |
| 14 | u32be × 28 | CRC `opkind4004()` of JS5 indexes 0 through 27, in that order (`ObjStack.java:277-304`). Index 6 is `ChargementDesPolices.valuekind57612` (`Client.java:1161`, CRC at `ObjStack.java:283`) |
| 15 | bytes | the RSA block |

Outgoing cipher: `SoftReferenceNode.valueSoftReferenceNode170.opkind5032(seed)` (`ObjStack.java:307`). Then each seed `+= 50` (`ObjStack.java:309-311`). Incoming cipher: `TexturesChargBes.valuekind4792.opkind5032(seed)` (`ObjStack.java:313`).

`Cd2e07e2c.opkind5031` adds the next ISAAC word to the opcode (`Cd2e07e2c.java:80-81`). `opkind5033` subtracts it (`Cd2e07e2c.java:88-89`). `LinkedList.opLinkedList4` is the generator (`LinkedList.java:58-64`), seeded by `LinkedList(int[])` (`LinkedList.java:46-55`) with golden-ratio mix `opLinkedList3` (`LinkedList.java:67`).

## State 4 response

One raw `u8` (`ObjStack.java:317-358`):

| Byte | Next state |
| --- | --- |
| 21 | 7 |
| 29 | 10 |
| 1 | status 1, state 5, return |
| 2 | state 8 |
| 15 | status 15, state 0, return (socket stays up) |
| 23 | if `C6bf8523c.valuekind55114 < 1`, increment attempts and reconnect from state 1; otherwise failure |
| other | status = that byte, disconnect, state 0 |

## State 6

ISAAC opcode 17 and no further payload (`ObjStack.java:361-365`), then state 4.

## State 7

One raw `u8`. `Packet.valuePacket143 = (byte + 3) * 60`. Status 21. Disconnect (`ObjStack.java:366-372`).

## State 10

One raw `u8` stored in `Client.valueClient1499`. Status 29. Disconnect (`ObjStack.java:374-380`).

## State 8 success

14 raw bytes (`ObjStack.java:383-399`), then one ISAAC opcode and a `u16be` length (`ObjStack.java:416-417`):

| Order | Read | Stored |
| --- | --- | --- |
| 1 | u8 | `Cb4aa1ea4.valuekind35713` |
| 2 | u8 | `Cc997e18d.valuekind4425` |
| 3 | u8 | `C74548ea9.valuekind5788` is true when the byte is 1 |
| 4 | u8 | `MapElementList.valueMapElementList59` is true when the byte is 1 |
| 5 | u8 | `CollisionMap.valueCollisionMap23` is true when the byte is 1 |
| 6 | u8 | `GlIndexedSprite.valueGlIndexedSprite19` is true when the byte is 1 |
| 7 | u8 | `C9ce5ebca.valuekind5674` is true when the byte is 1 |
| 8 | u16be | `Ceea51ef6.valuekind4136` (local player index) |
| 9 | u8 | `Ce9619f9b.valuekind4183` is true when the byte is 1 |
| 10 | u8 | `InvType.valueInvType58` is true when the byte is 1 |
| 11 | isaac opcode | `C45440f3e.valuekind4804` |
| 12 | u16be | payload length `PlainTile.valuePlainTile19` |

State 9 reads that many payload bytes and calls `C8be0107e.opkind3609(false)` (`ObjStack.java:421-430`). Status becomes 2. The opcode slot is cleared to `-1` without a second dispatch.

## Other pre-ISAAC writers

`Light.opLight6` (`Light.java:23-47`), opcode raw 69. Inner block, then RSA, then `u8` 69, `u8` block length, block bytes:

`u8` 10, `u16be` random, `u16be` 525, `u64be` username, `u32be` random, jstr password, `u32be` random, RSA.

`TextureOp35.opTextureOp3518` (`TextureOp35.java:13-33`), opcode raw 122. Same outer length framing. Inner:

`u8` 10, `u16be` random, `u16be` 525, `u64be`, `u32be` random, jstr, `u32be` random, `u16be` `VorbisMapping.valueVorbisMapping9`, `u8`, `u8`, `u32be` random, `u16be`, `u16be`, `u32be` random, RSA.

`ObjStack.opObjStack19` (`ObjStack.java:145-151`), raw, no RSA: `u8` 62, `u8`, `u8`, `u16be`, `u16be`.
