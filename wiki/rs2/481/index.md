# Revision 481

| | |
| --- | --- |
| Family | rs2 |
| Date | 2007-12-11 |
| Main class | `deob.Client` in `builds/rs2/481-2007-12-11/src/src/main/java/deob/Client.java`. `src/MAIN_CLASS.txt` says `deob.client`. |
| Canvas | 765 by 503. `Client.main` passes those sizes and the literal 481 to `LongNode.call5dae9383` at `builds/rs2/481-2007-12-11/src/src/main/java/deob/Client.java:99`. |
| Cache | js5. Sixteen archives are created in `SieBefindenSichEinemMitgliederGebiet.opkind2782` at `builds/rs2/481-2007-12-11/src/src/main/java/deob/SieBefindenSichEinemMitgliederGebiet.java:60`. |
| Source pin | OpenRS2 client 31097, jar sha256 `14c7f3aad905903535ccdf6924c3f2cef787e501d69ca422aa92571b35cb4d32` (`builds/rs2/481-2007-12-11/meta.json`). This tree has no nested git SHA. |

## Tick

`Client.opClient181` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/Client.java:398`) increments the cycle, then branches on `Node.valueNode43`:

| State | Work |
| --- | --- |
| 0 | `SieBefindenSichEinemMitgliederGebiet.opkind2782` and `DieseWeltIstVoll.opkind2625` |
| 5 | `TextureOp27.opTextureOp2713`, then the state-0 loaders |
| 10 | `TextureOp27.opTextureOp2713` |
| 20 | `TextureOp27.opTextureOp2713` and `SoundBank.opSoundBank15` |
| 25 | `SelectWorld.opkind32111` |
| 30 | `C55f3f558.opkind3064` |
| 40 | `SoundBank.opSoundBank15` |

`Client.opClient180` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/Client.java:234`) is the draw pass. State 30 calls `GameShell.opGameShell25` at line 287. JS5 reconnect is `Client.calle88fb0bb` at line 322.

Inside state 30, `C55f3f558.opkind3064` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/C55f3f558.java:340`) reads up to 100 server packets, then may write mouse, click, camera, and focus packets before the entity tick.

## Pages

- [login.md](login.md)
- [framing.md](framing.md)
- [packets.md](packets.md)
- [sync.md](sync.md)
- [region.md](region.md)
- [cache.md](cache.md)
- [input.md](input.md)
- [scene.md](scene.md)
- [interfaces.md](interfaces.md)
- [sound.md](sound.md)
- [gaps.md](gaps.md)
- [files.tsv](files.tsv)

## Status

- login: specified
- framing: specified
- packets: partial
- sync: specified
- region: specified
- cache: partial
- input: partial
- scene: partial
- interfaces: partial
- sound: partial
