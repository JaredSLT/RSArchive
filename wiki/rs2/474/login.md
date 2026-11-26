# Login

Login runs in `TextureOp29SubOp.opTextureOp29SubOp8` while the game state is 20 or 40 (`builds/rs2/474-2007-10-19/src/src/main/java/rt4/TextureOp29SubOp.java:97-330`, `rt4/Client.java:309-320`). The stage counter is `Cache.valueCache16`. Bytes below are written on the game socket. The ISAAC ciphers are installed only after the RSA block is sent. Width names are the buffer methods in [framing.md](framing.md).

`Loc.valueLoc42` is the low-detail flag. `Client.main` calls `WirVermutenDassJemandIhrPasswortKennt.opkind2282` for `lowmem`, which sets the flag true (`rt4/Client.java:94-95`, `deob/WirVermutenDassJemandIhrPasswortKennt.java:299-301`). `highmem` calls `MapElementList.opMapElementList7`, which sets it false (`rt4/Client.java:99`, `rt4/MapElementList.java:46-48`). The default is false (`rt4/Loc.java:7`). Membership is a different field, `ReferenceNodeFactory.valueReferenceNodeFactory31` (`rt4/Client.java:102-109`).

## Stage 2, name hash

The username is `GlTexture.valueGlTexture10.opByteArrayNode36()`. That method packs at most 12 characters as base 37: `A-Z` and `a-z` become 1 through 26, digits become 27 through 36, and trailing zeroes are divided out (`rt4/ByteArrayNode.java:282-301`).

Plain bytes, not ISAAC (`rt4/TextureOp29SubOp.java:127-133`):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 14 |
| 2 | u8 | `(usernameLong >> 16) & 31` |

Two bytes are sent.

## Stage 3

One response byte. Zero continues. Any other value goes to `Scenery.opScenery46` (`rt4/TextureOp29SubOp.java:147-158`).

## Stage 4

Eight bytes, read as `u64be` into `EnumStringEntry.valueEnumStringEntry17` (`rt4/TextureOp29SubOp.java:165-182`).

## Stage 5, request

The RSA block is built in `InvType.valueInvType19`, then `opByteArrayNodeSecondary48` replaces it with `u8` length plus the `modPow` bytes (`rt4/ByteArrayNodeSecondary.java:364-374`).

Exponent `LadeFreundeListe.valuekind2336` is `58778699976184461502525193738213253649000149147835990136706041084440742975821` (`deob/LadeFreundeListe.java:15`). Modulus `EnumStringEntry.valueEnumStringEntry12` is `7162900525229798032761816791230527296329313291232324290237849263501208207972894053929065636522363163621000728841182238772712427862772219676577293600221789` (`rt4/EnumStringEntry.java:22`).

RSA plaintext, in order (`rt4/TextureOp29SubOp.java:186-197`):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 10 |
| 2 | u32be | random in `0 .. 99999999` |
| 3 | u32be | random in `0 .. 99999999` |
| 4 | u32be | high 32 bits of the server session key |
| 5 | u32be | low 32 bits of the server session key |
| 6 | u64be | username long |
| 7 | jstr | password `GlTexture.valueGlTexture9`, bytes then `0` (`rt4/ByteArrayNodeSecondary.java:501-503`) |

The outer buffer is `TextureOp35.valueTextureOp353`, still not ISAAC (`rt4/TextureOp29SubOp.java:198-226`):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 16 when the game state is not 40, else 18 |
| 2 | u8 | RSA block length plus 93 |
| 3 | u32be | 474 |
| 4 | u8 | 1 when `Loc.valueLoc42` is true, else 0 |
| 5 | bytes(24) | machine uid from `ObjType.opObjType40`. A missing or all-zero uid is 24 bytes of `0xff` (`rt4/ObjType.java:31-52`) |
| 6 | 16 times u32be | archive CRCs `valueSoftwareFont44` for indexes 0 through 15, in that order (`rt4/TextureOp29SubOp.java:209-224`) |
| 7 | bytes | the RSA block already prefixed by its u8 length |

The 93 in the length byte is 4 plus 1 plus 24 plus 64. The four ISAAC seeds are random0, random1, session high, session low. Outgoing traffic is seeded with those four ints (`rt4/TextureOp29SubOp.java:227`). Each seed is then increased by 50 and that second array seeds the incoming cipher (`rt4/TextureOp29SubOp.java:229-233`).

## Stage 6, response byte

One u8 (`rt4/TextureOp29SubOp.java:237-256`):

| Byte | When | Effect |
| --- | --- | --- |
| 21 | game state is 20 | stage 7 |
| 2 | any | stage 9, success |
| 15 | game state is 40 | `MouseRecorder.call15a13947`, reconnect |
| 23 | `Inv.valueInv11` is still 0 | increment the attempt counter and return to stage 0 |
| other | | `Scenery.opScenery46` |

`MouseRecorder.call15a13947` was not read. The reconnect payload is in [gaps.md](gaps.md).

## Stage 7 and 8

One u8. `MixerListener.valueMixerListener13` becomes `60 * byte + 180`. The client waits that many ticks, then returns to stage 0 (`rt4/TextureOp29SubOp.java:259-268`).

## Stage 9, success header

Eight bytes, then one game packet forced through the static rebuild path (`rt4/TextureOp29SubOp.java:272-295`):

| Order | Width | Field |
| --- | --- | --- |
| 1 | u8 | `IhrCharakterProfilWird.valuekind22516` |
| 2 | u8 | `RunescapeHasBeenUpdated.valuekind2204` is true when the byte is 1 |
| 3 | u8 | high byte of `SpotAnimType.valueSpotAnimType99` |
| 4 | u8 | low byte of that player index |
| 5 | u8 | `C51284405.valuekind2512` |
| 6 | u8 | ISAAC opcode via `opTextureOp272` |
| 7 | u16be | payload size |
| 8 | bytes(size) | passed to `ByteArrayNodeSecondary.opByteArrayNodeSecondary24(false)` after `GlIndexedSprite.opGlIndexedSprite3` |

The opcode byte is discarded. The payload is always the static rebuild in [region.md](region.md). A wait past 2000 ticks swaps the game port with the update port once, then fails with `-3`. An `IOException` fails with `-2` after the same one swap (`rt4/TextureOp29SubOp.java:299-328`).

## Failure codes

`Scenery.opScenery46` displays a code and returns the client to state 10 (`rt4/Scenery.java:35-118`). The three strings passed to `WallDecor.opWallDecor10` were not opened, so this page lists the code and the call only.

Handled codes are `-3`, `-2`, `-1`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `10`, `11`, `12`, `13`, `14`, `16`, `17`, `18`, `19`, `20`, `22`, `23`, `24`, `25`, `26`, `27`, and every other value that reaches the method. Codes `2`, `15`, and `21` are consumed in stage 6 before this method. Code `23` reaches it only after the one retry.
