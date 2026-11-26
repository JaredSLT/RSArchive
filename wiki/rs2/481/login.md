# Login

Host string in `Client.main` is `127.0.0.1` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/Client.java:97`). The live port is `MalformedLoginPacket.valuekind22832`, copied from `MaterialManager_Class106_Sub1_Sub1.valueMaterialManager_Class106_Sub1_Sub137` (line 194).

When `LocType.valuekind31015` is 0, that port is the character `ꩊ` (U+AA4A, 43594). Otherwise it is `鱀` (U+9C40, 40000) plus `Js5.valueJs536` (line 192). The alternate port `C89ec186e.valuekind3132` is 443 when `valuekind31015` is 0, otherwise `Js5.valueJs536` plus `썐` (U+C350, 50000) (line 193). A failed JS5 connect swaps the two ports (`Client.call81a680bc`, lines 494-498).

## JS5 handshake

`Client.calle88fb0bb` (line 322) connects, then writes a 5-byte `SlArrows` that is not ISAAC: `u8` 15, `u32be` 481 (lines 358-361). It reads one `u8`. 0 continues into `MalformedLoginPacket.call63b49dee` (line 384). Any other value goes to `call81a680bc` (line 375).

`call81a680bc` (line 491): response 6 after at least two attempts sets the error `js5connect_outofdate` and state 1000 (line 503). Responses 7 and 9 (`~code != -8` and `~code != -10`) after at least two attempts, while the client state is still at most 5, set `js5connect_full` (lines 502 and 514). Other failures retry, and the fourth failure sets `js5connect` when the state is already past 5 (line 506).

## Game login bytes written

`SoundBank.opSoundBank15` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SoundBank.java:25`). Stage is `Ccc509c66.valuekind3521`.

| Stage | Bytes |
| --- | --- |
| 0 | Close the socket. Go to stage 1. |
| 1 | Connect to the same host and port. Stage 2 when the socket is up (line 48). |
| 2 | `u8` 14, then `u8` `(int)((nameLong >> 16) & 31)`. Send 2 bytes (lines 56-61). `nameLong` is `ByteArrayNodeSecondary.valueByteArrayNodeSecondary60.opByteArrayNode83()`. That encoder was not read. |
| 5 | Build an inner buffer, RSA-wrap it, then write the outer buffer below. |

Inner buffer `TracingException.valueTracingException72` (lines 117-124), before RSA:

| Order | Width | Value |
| --- | --- | --- |
| 1 | `u8` | 10 |
| 2-5 | `u32be` | four ints: two `Math.random() * 9.9999999E7`, then `(int)(seed >> 32)`, then `(int)seed` |
| 6 | `u64be` | username long from `opByteArrayNode83` |
| 7 | `jstr` | password `ByteArrayNodeSecondary.valueByteArrayNodeSecondary49`, `opkind33435` |

RSA is `SlArrows.call3a4d0eac` (line 124). Modulus `C165b9afc.valuekind2951` is `7162900525229798032761816791230527296329313291232324290237849263501208207972894053929065636522363163621000728841182238772712427862772219676577293600221789` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/C165b9afc.java:6`). Exponent `CursorType.valueCursorType33` is `58778699976184461502525193738213253649000149147835990136706041084440742975821` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/CursorType.java:44`). After `modPow` the inner buffer is `u8` length plus the RSA bytes (`SlArrows.java:221`).

Outer buffer `Entity.valueEntity14` (lines 125-152):

| Order | Width | Value |
| --- | --- | --- |
| 1 | `u8` | 18 if `Node.valueNode43 == 40`, else 16 |
| 2 | `u8` | inner position plus 93 |
| 3 | `u32be` | 481 |
| 4 | `u8` | 1 if `GameShell.valueGameShell110`, else 0 |
| 5 | `bytes(24)` | `TextureOp.calla300d209`: 24 bytes from `Midi_Class162.valueMidi_Class16225`. Missing, all zero, or an IO failure becomes 24 bytes of `-1` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/TextureOp.java:39`). |
| 6-21 | `u32be` | CRC `valuekind34720` of archives 0 through 15, in this order: `QuickChatPhrase.valueQuickChatPhrase25`, `C5afc506d.valuekind3493`, `MouseRecorder.valueMouseRecorder30`, `WorldFull.valuekind29111`, `TextureOp29SubOp2.valueTextureOp29SubOp247`, `VarpType.valueVarpType88`, `VorbisSound.valueVorbisSound72`, `ByteArrayNode.valueByteArrayNode52`, `Js5GlTextureProvider.valueJs5GlTextureProvider13`, `StringInterface.valueStringInterface56`, `BenutzenSieDieWpasswortNdernOption.valuekind31113`, `C78d60e9c.valuekind3383`, `TextureOp29SubOp2.valueTextureOp29SubOp245`, `Sprite.valueSprite92`, `Song.valueSong18`, `SceneGraph_Class120.valueSceneGraph_Class12070` |
| 22 | bytes | the RSA block |

ISAAC seeds are the four ints, then each plus 50 for the decode cipher. See [framing.md](framing.md).

## Bytes consumed

| Stage | Read | Next |
| --- | --- | --- |
| 3 | one `u8` from `opSong11` (line 75) | Nonzero calls `TextureOp17.opTextureOp171` and returns. 0 goes to stage 4. |
| 4 | 8 bytes, then `u64be` server seed `Sprite.valueSprite99` via `opkind33410` (lines 93-109) | Stage 5. |
| 6 | one `u8` (line 165) | 21 while state is 20 goes to stage 7. `~code == -3` means code 2, which goes to stage 9. 15 while state is 40 calls `AccountLockedAsWeSuspectHasBeenStolen.opkind3054` and returns; that method was not read. 23 while `PathingEntity.valuePathingEntity149 < 1` retries from stage 0. Anything else calls `opTextureOp171`. |
| 7 | one `u8` (line 187) | Wait seconds are `byte * 60 + 180` in `SoftLruHashTable.valueSoftLruHashTable25`. Stage 8 counts that down and returns to stage 0. |
| 9 | needs at least 9 bytes available (`~available <= -10`, line 199) | `u8` `TextureOp16.valueTextureOp16159`, `u8` `Ca2c9453c.valuekind31711`, `u8` equal to 1 sets `World.valueWorld159`, `u8<<8` plus `u8` into `C725f81a2.valuekind2971`, `u8` `Cd9f578c5.valuekind31212`, then one ISAAC opcode byte via `opkind3404` into `Cd03d7e86.valuekind29812`, then `u16be` packet size via `opkind3349` into `PathingEntity_Class147.valuePathingEntity_Class14725` (lines 200-212). |
| 10 | the payload of that size (line 219) | `ConnectionTimedOut.opkind2994`, then `SieBefindenSichEinemMitgliederGebiet.opkind2783(false)` unconditionally (lines 220-222). The ISAAC opcode is not dispatched. The first game payload is the static rebuild reader. The opcode is then set to `-1`. |

`ConnectionTimedOut.opkind2994` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/ConnectionTimedOut.java:46`) clears the outgoing position, players, and NPCs, puts the local player in slot 2047 (line 97), zeros varps whose type `valuekind3523` is 0 (lines 113-118), and sets `StringInterface.valueStringInterface61` to `-1` (line 133).

## Failure codes

`TextureOp17.opTextureOp171` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/TextureOp17.java:35`) shows a dialog and returns the client to state 10 (line 114). Codes with their own `SoundPcmStream.opSoundPcmStream22` call: `-3`, `-2`, `-1`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `10`, `11`, `12`, `13`, `14`, `16`, `17`, `18`, `19`, `20`, `22`, `23`, `24`, `25`, `26`, `27`. Every other code, including `0`, `1`, `2`, `15`, and `21`, uses the default at line 76. The dialog arguments are `ByteArrayNode` fields. Their English text was not opened. `opSoundPcmStream22` is the dialog, not audio playback.

A timeout past 2000 attempts while still in login calls `opTextureOp171(-3)` after one retry, or `-2` from the IO catch after one retry (lines 227-255).
