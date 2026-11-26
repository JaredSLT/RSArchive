# Revision 525

| | |
| --- | --- |
| Family | rs2 |
| Date | 2008-12-15 |
| Revision | 525 |
| Main class | `deob.Client` (`Client.main` builds the applet; `builds/rs2/525-2008-12-15/meta.json` records `main_class` `client`) |
| Canvas | 1024 by 768 (`Client.java:654` passes those sizes into `ProjAnim.calla0751286`; `ProjAnim.java:503-507` stores width `C09b74607.valuekind50410` and height `Cbb5a2b0d.valuekind4684`; revision `WorldInfo.valueWorldInfo50` is argument 525) |
| Cache | js5 (`meta.json` `engine` is `js5`; cache home `.jagex_cache_32/runescape`) |
| Nested source SHA | not recorded in `meta.json` |

OpenRS2 client 30833, cache 665. Jar sha256 `15c286c88eaf1deec2844cb4ad33d0e0b7e41b26b502ca97f74888af77c1d078`.

Game state `C37c92f88.valuekind49330` in `Client.opClient186` (`Client.java:923`):

1. Bump the frame counter. Every 1000 frames, reseed `C9e146a8e.valuekind5084` from the clock (`Client.java:925-929`).
2. `opClient199` pumps the JS5 socket, or reconnects with `call81a680bc` (`Client.java:989-993`).
3. If present, `ProjAnimNode.valueProjAnimNode42.opkind4673`.
4. `LightType.opLightType25`, `Js5ResourceProvider.opJs5ResourceProvider20`, `Ce6ecfdbe.opkind3801`, `ObjStack.opObjStack16` (mouse snapshot).
5. Mouse-wheel `Cb6bcf595.valuekind5708.opkind5332` into `C98d1be1a.valuekind5218`.
6. State 0 or 5: login-screen pump `call1fede62b` and `C4140f2ca.opkind4553`. State 25 or 28: `Ceea51ef6.opkind4131`. State 10: the same login-screen pump plus `calle2cbab8d`, `C9c57aa59.opkind3985`, `Cafed0342.opkind5355`, and `ObjStack.opObjStack20`. State 30: `ExpiresThuJanGmtMaxAge.opkind4441` (up to 100 server packets, then client packets). State 40: `ObjStack.opObjStack20`. State 1000: the loop returns immediately.

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

| Subsystem | Status |
| --- | --- |
| login | specified |
| framing | specified |
| packets | partial |
| sync | specified |
| region | specified |
| cache | partial |
| input | partial |
| scene | partial |
| interfaces | partial |
| sound | partial |
