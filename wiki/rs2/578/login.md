# Login

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`. Login opcodes are `C8cb2fbe0` values. The constructor stores the first int and drops the second (`deob/C8cb2fbe0.java:48`).

| name | opcode | constructor |
| --- | --- | --- |
| handshake | 14 | `deob/BasType.java:23` |
| js5 | 15 | `BasType.java:24` |
| game login | 16 | `BasType.java:25` |
| isaac resume | 17 | `BasType.java:26` |
| login while state 40 | 18 | `BasType.java:27` |
| 20 | 20 | `BasType.java:28` |
| 21 | 21 | `BasType.java:29` |
| create | 22 | `BasType.java:30` |
| 23 | 23 | `BasType.java:31` |
| world login | 24 | `BasType.java:32` |

Opcodes 20, 21, and 23 are constructed and not written by the methods read here. Opcode 22 is the create-account writer `Cf3727c90.opkind9792`, which was not read.

## Ports

Live mode (`SchriftartenGeladen.valuekind8814 == Cca3bb49b.valuekind7962`): host from the codebase, game port 43594, second port 443 (`deob/Client.java:1805`). Local mode (`C14b434f1.valuekind8787`): host `127.0.0.1`, game port `worldId + 40000`, second port `worldId + 50000` (`Client.java:1810`). The other non-live branch uses the codebase host and the same `worldId + 40000` game port (`Client.java:1815`).

## JS5 connect

`Client` state `C15ab7d97.valuekind9292 == 2` writes 5 bytes on a separate socket: u8 opcode 15, u32be 578 (`Client.java:1575`). It reads one raw byte. Nonzero fails with that code. Zero hands the socket to `Ca9d0edad.valuekind97119.opkind7648` (`Client.java:1601`).

## Game handshake and login block

`LswprPos.calla978325d` (`deob/LswprPos.java:46`). Outgoing buffer is `C5c7b4c41.valuekind7264`, a `ParticleEmitterRelated` of 5000 bytes (`deob/C5c7b4c41.java:5`). Incoming is `C7502a728.valuekind8306`, also 5000 (`deob/C7502a728.java:6`). Socket wrapper is `Cd24b839a`. Raw read is `opkind8799`.

State 1 opens `host C759ca1fe.valuekind6933`, port `C36f4c4c6.valuekind7842`.

State 2, after connect (`LswprPos.java:87`):

- u8 opcode 14
- u8 `(usernameLong >> 16) & 31`. The long is `MitgliederNnenFreundeHinzufGenFreieSpieler.opkind5905(C8cb2fbe0.valuekind10125)`.
- flush 2 bytes
- read one u8. Nonzero stores that code in `SpotAnimType.valueSpotAnimType135`, closes the socket, and returns to state 0 (`LswprPos.java:110`).

State 3 waits for 8 bytes, then (`LswprPos.java:126`):

- u64be server seed via `opUnused669` into `UnableSendMessagePlayerNotFriendlist.valuekind7634`
- RSA block on a fresh `Unused6(70)` (`LswprPos.java:129`):
  - u8 10
  - u32be random
  - u32be random
  - u32be seed high
  - u32be seed low
  - u64be username long (`opUnused696`)
  - pjstr password `VocTemPermissParaConversarNesteCanalDeCl.valuekind10265` (`call75e62fa6`)
  - `opUnused698(C6ada6b6d.valuekind6972, GlxRelated12.valueGlxRelated123)` (`LswprPos.java:138`)
- The modulus is the decimal in `deob/C6ada6b6d.java:6`. The exponent is the decimal in `deob/GlxRelated12.java:12`. `opUnused698` (`deob/Unused6.java:333`) rewinds the block, treats those bytes as a BigInteger, `modPow(exponent, modulus)`, then writes u8 ciphertext length and the ciphertext bytes. No private key is in this client.
- Outer packet (`LswprPos.java:139`):
  - u8 opcode 18 if `Ca4c92d3c.valuekind7069 == 40`, otherwise u8 opcode 16
  - u16be length placeholder 0 (`opUnused6107`)
  - u32be 578
  - u8 `Cd6fe1c24.valuekind8632`
  - u8 `Ce5d3f87a.opkind9964()`
  - u16be `C9dc28953.valuekind9251`
  - u16be `MapElement.valueMapElement85`
  - u8 `Cb453c1d8.valuekind9823.valuekind97915`
  - 24 raw bytes from `Cd18618b0.opkind7515` (`deob/Cd18618b0.java:35`). A failed read fills 24 bytes of 0xFF.
  - pjstr `C51676708.valuekind9413`
  - u32be `C6c80930e.valuekind9465`
  - u8 length plus the bytes of `Cb453c1d8.valuekind9823.opSceneGraph_Class12079()`
  - u16be `C18a6fbcb.valuekind9472`
  - twenty-nine u32be CRCs, `opkind64013()`, in this order (`LswprPos.java:162`): indices 0 `C97d5c431.valuekind7027`, 1 `Unused6.valueUnused68`, 2 `C67a82e1d.valuekind7141`, 3 `GlRenderList.valueGlRenderList28`, 4 `C36f4c4c6.valuekind7844`, 5 `C381a95c3.valuekind101912`, 6 `Ca7df0199.valuekind8424`, 7 `C889f53b6.valuekind8626`, 8 `C408bee69.valuekind9673`, 9 `Cb55a2183.valuekind63810`, 10 `C34de5186.valuekind8287`, 11 `C759ca1fe.valuekind6936`, 12 `Cee767227.valuekind7813`, 13 `C4743b1d7.valuekind6984`, 14 `C23d8b466.valuekind8481`, 15 `FriendListFirst.valuekind8418`, 16 `NpcType.valueNpcType166`, 17 `ListaDeAtualizaEsCarregada.valuekind8504`, 18 `C759ca1fe.valuekind6931`, 19 `TextureOp29SubOp2.valueTextureOp29SubOp294`, 20 `C0922b514.valuekind59423`, 21 `WorldMapFont.valueWorldMapFont98`, 22 `MaterialManager_Class106_Sub2_Sub1.valueMaterialManager_Class106_Sub2_Sub166`, 23 `Cf3727c90.valuekind97936`, 24 `Cbded41f3.valuekind8155`, 25 `C0922b514.valuekind59421`, 26 `C00d852fc.valuekind6744`, 27 `TextureOp29SubOp2.valueTextureOp29SubOp293`, 28 `C36f4c4c6.valuekind7843`
  - the RSA ciphertext bytes
  - backpatch the u16be length with `opUnused6113` so the length counts from after the placeholder (`LswprPos.java:192`)
- ISAAC (`LswprPos.java:194`): outgoing cipher from the four seeds, then each seed `+= 50`, then the incoming cipher from those adjusted seeds. `ParticleEmitterRelated.call32929251` (`deob/ParticleEmitterRelated.java:160`) installs `new Cb55a2183(seeds)`.

State 4 reads one raw response byte (`LswprPos.java:209`):

| byte | next |
| --- | --- |
| 21 | state 7 |
| 29 | state 11 |
| 1 | state 5, status 1 |
| 2 | state 8 |
| 15 | state 12, and the following size is forced to -2 |
| 23, and this is the first attempt | reconnect, state 1 |
| other | status = that byte, socket closed, state 0 |

State 7: one u8, status 21, delay `byte * 60 + 180`, disconnect (`LswprPos.java:255`).

State 11: one u8 stored in `C5afcc012.valuekind10051`, status 29, disconnect (`LswprPos.java:263`).

State 6 writes isaac opcode 17 and returns to state 4 (`LswprPos.java:249`). Nothing in the read path of `calla978325d` assigns state 6.

State 8 reads 13 bytes (`LswprPos.java:276`):

- u8 `C81d8950f.valuekind85510`
- u8 `C3c9f52c8.valuekind102222`
- u8 `== 1` into `Cfcdb42e4.valuekind95412`
- u8 `== 1` into `Ca7df0199.valuekind8425`
- u8 `== 1` into `Ca4c92d3c.valuekind7066`
- u8 `== 1` into `UniversoDeJogoCriado.valuekind6556`
- u16be local player index `FullscreenMode.valueFullscreenMode4`
- u8 `== 1` into `C6b1ceb6e.valuekind6914`
- u8 `== 1` into `Cc7df166d.valuekind6851`

Then state 10.

State 10 reads the first game packet with isaac (`LswprPos.java:318`). If `opkind8137` says the opcode byte is at least 128, it pulls one more byte. Opcode is `opkind8131`. The size is always `opUnused694` (u16be), not the descriptor size. The payload is passed to `GlVertexBufferObject.opGlVertexBufferObject13`, which was not read, and then `Cc4cace49.opkind9583(false)`. The consumed length must equal the u16 size.

State 12 (response 15): if the size is still -2, read u16be. Then read that payload, call `Ce760822b.opkind6443` and `opGlVertexBufferObject13`. Both callees were not read. Status becomes 15.

## World login opcode 24

`C1659b20b.callc660d548` (`deob/C1659b20b.java:68`) when `FriendListFirst.valuekind8414 == 0`:

- RSA block: u8 10, u32be random, u64be username, u32be random, pjstr password, u32be random, then `opUnused698` with the same modulus and exponent
- outer: u8 opcode 24, u8 `rsaLength + 2`, u16be 578, RSA bytes

This method does not read a response. The later state machine is `LesModificationsSerontApportEsVotreClanDan.opkind7163` when `valuekind8414 != 0`, and that method was not read.
