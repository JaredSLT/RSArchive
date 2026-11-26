# Login

The lobby path is `LoginManager.requestLobbyLoginWithUsername` (`builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/login/LoginManager.java:250`). It sets `requestState` to 132 and `connection` to `Client.lobbyConnection`. Game login sets `requestState` to 211 and uses `Client.gameConnection` (`LoginManager.java:257`). This client does not write a revision-317 login block.

Both paths share the earlier steps in `LoginManager.update` (`LoginManager.java:309`).

## Handshake

Step 14 opens the lobby socket when `requestState` is not 211, clears the write queue, and writes one byte, `LoginProt.INIT_GAME_CONNECTION.id`, which is 14 (`LoginManager.java:362`). There is no payload.

Step 35 reads 9 bytes. The first is `g1` and must be 0. The next is `g8`, a big-endian 64-bit server seed stored in `tlong` (`LoginManager.java:375`). Any other first byte is a failure code.

When `isSocialLogin` is set, step 276 writes `INIT_SOCIAL_NETWORK_CONNECTION` (id 29) instead of jumping to the lobby block (`LoginManager.java:396`): `u16be` length placeholder, `u32be` 910, `u32be` 1, an extra `u8` only when `requestState == 211`, then an RSA packet from `startRSAPAcket` plus `packLoginTOTPDetails`, closed with `psize2`. Step 40 reads a `u16be` length. Step 58 XTEA-decrypts that buffer with `outKey` and reads `gjstr2`. Step 64 requires the next byte to be 1. Step 70 reads 16 bytes, decrypts them, and reads two `g8` values, `socialKey` and `socialname`.

## RSA block

`startRSAPAcket` (`LoginManager.java:1118`) writes `u8` 10, four `u32be` values from `secureRandom` into `outKey`, and `u64be` `tlong`. When `Client.state == 14` it also writes the previous four `outKey2` ints. `createLoginRSAPacket` (`LoginManager.java:1174`), used when `state != 14`, then writes the TOTP block, `pbool(false)`, `pjstr(password)`, `u64be` `socialname`, and `u64be` `Client.tlong2`, and calls `rsaenc(PublicKeys.LOGIN_RSAE, PublicKeys.LOGIN_RSAN)`. The modulus bytes were not opened.

`packLoginTOTPDetails` (`LoginManager.java:1143`) writes `u8` `TotpType.getId()`. Index 0 writes `u32be`. Indexes 1 and 3 write `p3` of the six-digit string and then increment `pos` by 1. Index 2 adds 4 to `pos` and writes nothing.

## Lobby block

When `requestState != 211`, step 84 writes this block (`LoginManager.java:546`). The opcode is 19 `LOBBYLOGIN`, or 30 `SOCIAL_NETWORK_LOGIN` when `isSocialLogin` is set. Then:

1. `u16be` 0, later backpatched by `psize2` over the bytes that follow.
2. If this is not a social login: `u32be` 910, `u32be` 1, the RSA packet bytes, `u8` 1 when `socialKey == -1` else 0, then `pjstr(username)` or `u64be` `socialKey`.
3. `u8` `Client.modegame.game`.
4. `u8` `Client.language.getId()`.
5. `u8` `Client.getWindowMode()`.
6. `u16be` `GameShell.canvasWid`.
7. `u16be` `GameShell.canvasHei`.
8. `u8` anti-alias preference.
9. 24 raw bytes from `GameShell.pushUID192` (`GameShell.java:545`). On failure those 24 bytes are `0xFF`.
10. `pjstr(Client.siteSettings)`.
11. `u8` length of `preferences.createPreferencesBlock`, then those bytes. The block internals were not opened.
12. A hardware block from `HardwarePlatform.createHardwareBlock`. Its writer was not opened.
13. `u32be` `Client.currentIncrementVerifyId`.
14. `pjstr(Client.string2)`.
15. `u32be` `Client.playerIsAffiliate`.
16. `u32be` `Client.tint4`.
17. `pjstr(Client.gamepack)`.
18. `u8` `Client.clientType & 1`.
19. `pbool(false)`.
20. `pushJS5CRCs` (`LoginManager.java:1226`): one `u32be` checksum for every `Js5Archive.values()` entry except `LOADING_SPRITES`, or `u32be` 0 when that archive is absent.
21. `tinyenc(outKey, start, pos)` over the bytes from the social-key section through the CRCs.
22. `psize2` of the length after the placeholder.

`outKey` is then the outbound ISAAC seed. The inbound seed is each of those four ints plus 50 (`LoginManager.java:600`).

## Game block

`requestState == 211` writes opcode 16 or 30 at `LoginManager.java:483`. It is a different block from the lobby block: it inserts `u8` (`state == 14`), places the preferences and hardware blocks earlier, writes several extra `u32be` values (`userFlow2`, `userFlow1`, `tint3`, `tint`, `tint4`), an optional `pjstr` additional-info string, javascript and chrome flags, a world-node flag, and `u16be` current lobby node. It does not write `pbool(false)` or `modegame.game` or the language id in the same places. It is still not a 317 block.

## Reply byte

Step 98 reads one `u8` (`LoginManager.java:620`).

| code | effect |
| --- | --- |
| 21 | one more `u8`, multiplied by 50, then failure 21 |
| 1 | reply 1, stop at step 103 |
| 52 | `u16be` length, then that many ISAAC-buffered bytes, then `gjstr` opened as a URL |
| 2 | success. Game (`requestState == 211`) goes to step 256. Lobby goes to step 141 |
| 15 | length is `-2` (`u16be`), then `receivePlayerPositions` only |
| 23 | retry from step 14 when attempts are below 3 |
| 42 | `u16be` queue position, then back to step 98 |
| 49 | lobby-only special case when `requestState == 132` |
| 35 | SSO retry when enabled |
| 53 | `g4s` ban duration |
| other | `setReply(code)` and close |

Step 256 reads `u16be` length, then `u8` and var values from `decodeVarValue` (that decoder was not opened). When the flag is 1 the client writes opcode 26 `GAMELOGIN_CONTINUE` and requires the next byte to be 2 before continuing to step 141.

## Lobby success body

Step 141 reads one `u8`, `replyPacketSize`. Step 157 reads that many bytes (`LoginManager.java:909`).

`getAuthPreferences` (`LoginManager.java:1100`): `u8` equal to 1, then four `gIsaac1` bytes assembled big-endian.

Then, for `requestState != 211`:

| field | read |
| --- | --- |
| staffModLevel | `g1` |
| playerModLevel | `g1` |
| tboolean20 | `g1` equals 1 |
| lobbyDOB | `g3s` |
| gender | `g1` stored on `localPlayerEntity` |
| playerIsQuickChat | `g1` equals 1 |
| tboolean21 | `g1` equals 1 |
| lobbyMembership | `g8` |
| offset subtracted from membership | `g5`, one `u8` then one big-endian `u32` (`Packet.java:375`) |
| flags | `g1`, bit 0 members, bit 1 tboolean22 |
| lobbyJCoinsBalance | `g4s` |
| lobbyLoyaltyBalance | `g4s` |
| lobbyRecoveryDay | `g2` |
| lobbyUnreadMessages | `g2` |
| lobbyLastLoginDay | `g2` |
| playerHostName | `g4s` |
| lobbyEmailStatus | `g1` |
| lobbyCCExpiry | `g2` |
| lobbyGraceExpiry | `g2` |
| lobbyDOBRequested | `g1` equals 1 |
| name | `gjstr2` |
| lobbyMembersStats | `g1` |
| lobbyPlayAge | `g4s` |
| world node | `g2`, 65535 becomes -1 |
| host | `gjstr2` |
| port | `g2` |
| port2 | `g2` |

The client then `setReply(2)`, `setState(13)`, and clears `packetType` (`LoginManager.java:980`).

When `requestState == 211`, step 157 uses a different body (`LoginManager.java:885`): the same auth-preferences prefix, then `g1` staffModLevel, `g1` playerModLevel, `g1` tboolean20, `g1` playerIsQuickChat, `g1` tboolean21, `g1` loggedInQuickChat, `g2` currentPlayerUid, `g1` playerIsMembers, `g3s` lobbyDOB, `g1` loggedInMembers, `gjstr` owner, and `g6` minus `MonotonicTime.get()` stored in `tlong5`. `g6` is `g2` then `g4s` (`Packet.java:382`).

Step 170 then reads 3 bytes, an ISAAC opcode through `ServerProt.values()[gIsaac1or2()]`, `u16be` size, `receivePlayerPositions`, and the remainder as `REBUILD_REGION` or else `REBUILD_NORMAL` (`LoginManager.java:992`). That first placement is not opcode 122.
