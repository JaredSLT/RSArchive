# Revision 443

| | |
| --- | --- |
| Revision | 443 |
| Date | 2006-12-11 |
| Family | rs2 |
| Main class | `deob.Client`, started from `deob.client` (`builds/rs2/443-2006-12-11/src/MAIN_CLASS.txt`). `Client.main` requires six arguments and constructs `Cd66098f9` at 765 by 503 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:532`). |
| Canvas | 765 by 503 (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cd66098f9.java:151`) |
| Cache | js5. Indices 0 through 13 are opened in `Cb5815139` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cb5815139.java:59`). |
| Source | OpenRS2 client 32110, cache 1191. No nested git SHA is recorded. SHA-256 of the original gamepack is `78ad870efd06e3748fc3ad75304899d75f587cc95f0662dfc180c07f35b6f1d4` (`builds/rs2/443-2006-12-11/meta.json`). |

## Tick

`Cd66098f9.run` asks `opkind1411` how many logic ticks are due, calls `opkind1305` that many times, then calls `opkind13011` once (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cd66098f9.java:101`). `opkind1305` calls `Client.opClient161` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cd66098f9.java:137`). `opkind13011` calls `Client.opClient158` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Cd66098f9.java:659`).

`opClient161` increments the cycle, pumps the cache, drains `GlModel.opGlModel4`, applies the queued sound in `SieHabenGeradeEineAndereWeltVerlassen.opkind1825`, advances the MIDI clock in `TextureOp38.opTextureOp386`, copies keys in `Whi.opkind1815`, snapshots the mouse in `VertexNormal.opVertexNormal1`, then dispatches on `VorbisSound.valueVorbisSound15` (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:434`). States 0 and 5 run the startup loaders. State 10 runs login-screen input. State 20 runs that input and `Timer.opTimer5`. State 25 runs the cache load. States 30 and 35 run `Cc317a406.opkind1793`, which reads up to 100 server packets. State 40 runs `Timer.opTimer5` again (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:448`).

The draw switch is in `Client.opClient158`. State 30 calls `MonotonicClock.opMonotonicClock43`. State 35 calls `Cd2890cde.opkind1942`. States 5, 10, and 20 draw the login screen (`builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:112`).

## Pages

- [files.tsv](files.tsv)
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

## Status

| Subsystem | Status |
| --- | --- |
| login | partial |
| framing | specified |
| packets | partial |
| sync | specified |
| region | partial |
| cache | partial |
| input | partial |
| scene | partial |
| interfaces | partial |
| sound | partial |
