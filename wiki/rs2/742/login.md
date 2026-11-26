# Login

`LoginManager.login` (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:280) is the byte stream. World kind `Statics.valueStatics256 == 223` is the game connection (`WorldSwitcher.content`). Any other value in this method uses the lobby address `valueStatics360`. Outbound login messages are built with `ClientMessage.opClientMessage8`, which does not write an ISAAC opcode. Integers below are the `Packet` methods named in [framing.md](framing.md).

## State 12, connect

Game opens `WorldSwitcher.content`. Lobby opens `valueStatics360`. The buffer is cleared.

SSO (`valueLoginManager49`):

1. `u8` `LoginProt.INIT_SOCIAL_NETWORK_CONNECTION` (29)
2. `u16be` length, back-patched with `psize2` over the following bytes
3. `u32be` revision `valueClient1529` (742)
4. `u32be` `SUBREVISION` (1)
5. if game, `u8` 1 when `Client.state == 9`, else 0
6. RSA block from the inline writes at builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:331, then `rsaenc` (builds/rs2/742-2012-11-07/src/client/src/main/java/Packet.java:530). `ENABLE_RSA` is false, so `modPow` is skipped and the block is still `u16be` length plus the raw bytes. Inside, before `rsaenc`: the four XTEA seeds from `opLoginManager36` (`u8` 10, four `u32be`), then `u8` `ssoKey`, `u16be` random, `u8` language id, `u32be` `valueClient1630`, six `u32be` random, `u64be` `valueClient1689`, `u8` `modeGame.game`, `u8` random

Non-SSO: one `u8`, `LoginProt.INIT_GAME_CONNECTION` (14).

## State 35

Read `u8`. 0 continues. Any other value is a failure (`opLoginManager35`, close). SSO continues at state 40. Non-SSO continues at state 82.

## SSO states 40, 55, 61, 76

- State 40: `u16be` length (`g2`)
- State 55: that many bytes, `tinydec` with the four seeds `valueStatics279`, then `gjstr2` (leading 0, then nul-terminated CP1252). The string is opened as a URL
- State 61: `u8`. Value 1 moves to state 76
- State 76: 16 bytes, `tinydec`, then `u64be` `socialKey` and `u64be` `socialName` (`g8` twice)

`tinydec` is XTEA, delta `0x9e3779b9`, 32 rounds (builds/rs2/742-2012-11-07/src/client/src/main/java/Packet.java:463).

## State 82, login block

Opcode is `GAMELOGIN` (16) or, for SSO, `SOCIAL_NETWORK_LOGIN` (30) on a game world. Lobby uses `LOBBYLOGIN` (19) or `SOCIAL_NETWORK_LOGIN`. Then `u16be` length (`psize2` over the rest).

Game, non-SSO, before the shared tail (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:435):

- `u32be` revision, `u32be` subrevision
- `CUSTOM_SEND_MAC_ADDRESS` is true, so `pjstr("userMacAddress")` is written here
- `u8` 1 when `state == 9`, else 0
- `pjstr("userMacAddress")` again
- `opLoginManager33` (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:924): `opLoginManager36` (`u8` 10, four `u32be` seeds), `u64be` 0, `jstr` password, `u64be` `socialName`, `u64be` `valueClient1689`, then `rsaenc` as above
- `u8` 1 when `socialKey == -1`, else 0
- if `socialKey == -1`, `jstr` username, else `u64be` socialKey

Game tail, SSO and non-SSO:

- `u8` `getWindowMode()` (3 fullscreen, 2 when `valueClient1640`, else 1; builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2269)
- `u16be` canvas width, `u16be` canvas height
- `u8` antialiasing preference
- `GameShell.opGameShell60`: `bytes(24)` (builds/rs2/742-2012-11-07/src/client/src/main/java/GameShell.java:470). A failed read fills the 24 bytes with `0xFF`
- `jstr` `valueClient1567`
- `u32be` `valueClient1630`
- `u8` length of `options.createPreferencesBlock()`, then that many plain bytes. The block's own fields are unread
- hardware bytes: `pdata` of the whole array allocated by `opHardwarePlatform7()`. That length is unread. `createHardwareBlock` fills the array
- `u32be` `currentIncrementVerifyId`
- `u64be` `valueClient1538`
- `jstr` `valueStatics366`
- `u8` 0 or 1, and a `jstr` when `createAdditionalInfo` is set
- `u8` 1 when the native library name `jagtheora` is loaded
- `u8` `valueClient1533`, `u8` `valueClient1588`, `u8` `valueStatics401`
- `u32be` `valueClient1613`
- `jstr` `gamepack`
- `u8` 0 when the previous world address matches the current one, else 1
- `opLoginManager32` (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:960): 37 `u32be` values. 35 are `Js5.opJs534()` and two are `Loading.opLoading27()` and `Loading.opLoading25()`

`tinyenc` then runs from the byte after the RSA block through the end, and `psize2` patches the outer `u16be`.

Lobby tail (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:487) uses the same revision, RSA block, and username-or-socialKey prefix, then `u8` game id, `u8` language, the 24-byte shell block, `jstr` `valueClient1567`, the preferences length and bytes, the hardware array, `jstr` `valueStatics366`, two `u32be` (`valueClient1630`, `valueClient1613`), `jstr` gamepack, and the same 37 `u32be` checksums. It does not write the canvas sizes.

After the flush, `randomOut = new Isaac(seed)`, each of the four seed ints is increased by 50, and `randomIn = new Isaac(seed)` (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:531). Later game opcodes use those ciphers. This login block does not.

## State 96 reply

One `u8`:

| value | next |
| --- | --- |
| 21 | state 120, then one `u8` times 50 stored as the wait, failure 21 |
| 29 or 45 | state 183 |
| 1 | state 106 and return. `continueLogin` moves 106 to 113 (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:127). State 113 writes `GAMELOGIN_CONTINUE` (26) with `pIsaac1` and returns to state 96 (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:602) |
| 52 | state 213 |
| 2 | state 136, unless `valueLoginManager61` is set, in which case state 12 runs again |
| 15 | `packetSize = -2`, state 197 |
| 23 | state 12 when fewer than 3 attempts have run |
| 42 | state 208. Two bytes are stored as `(b0<<8)+b1` in `valueLoginManager58`, then state 96 (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:626) |
| 49 | lobby reply, recorded when the world kind is 154 and state is not 4 |
| 35 | SSO retry (state 12) when SSO is allowed and `ssoKey` is set |
| 53 | state 235, then `u32be` (`g4s`) into `valueLoginManager50`, failure 53 |
| other | failure, `loginState = 7`, close |

State 183 (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:648): reply 29 reads one `u8` into `valueLoginManager51`. Reply 45 reads that `u8` and a `u16be` (`(b1<<8)+b2`, not `g2` on a reset cursor) into `valueLoginManager57`. Then failure.

State 213: `u16be` length. State 225: that many bytes, `gIsaacArrayBuffer` into a buffer, then `gjstr`. The string is opened in the browser.

## Success, state 136 then 145

`u8` length, then that many bytes (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:704).

Game (`valueStatics256 == 223`):

- `u8` staff mod level
- `u8` `valueClient1543`
- four `u8` flags (`valueClient1636`, `valueClient1645`, `valueClient1646`, `valueClient1649`)
- `u16be` `currentPlayerUid`
- `u8` flag `valueClient1598`
- `i24be` `valueStatics445` (`g3s`)
- `u8` members flag `valueClient1692`, applied to loc, obj, and npc type lists
- `jstr` `valueStatics233`

Lobby uses a different list at builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:736: three `u8`, `i24be`, `u8` gender, two `u8` flags, `u64be`, `u40be` (`g5`) subtracted from a timestamp, `u8` bitfield, `u32be`, `u8`, `u32be`, three `u16be`, `u32be`, `u8`, `u16be`, `u16be`, `u8`, `jstr2` name, `u8`, `u32be`, `u8`, `u16be` world id (65535 becomes -1), `jstr2` host. Lobby then sets state 15 and does not place the player.

## Player and map packet

Game success continues at state 166. Three bytes are read. If `isIsaac1or2` is set, a fourth byte is read. `gIsaac1or2` selects `ServerProt.values()[opcode]`, and `g2` (`u16be`) is the payload length even when the opcode's table size is `-2` (builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:814). The payload is one buffer. `MapElement.opMapElement41` consumes the player half. The remainder is `REBUILD_REGION` when that opcode was selected, and `REBUILD_NORMAL` otherwise. `pos` must equal the length.

State 197 (reply 15) reads a `u16be` length and the whole buffer is `opMapElement41` only.
