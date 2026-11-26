# Revision 377

Family `rs2`, date 2006-05-02, revision 377. Main class `jagex2.client.Client`. `Client.main` prints `RS2 user client - release #377` and builds the shell at 765 by 503 (`builds/rs2/377-2006-05-02/src/src/main/java/jagex2/client/Client.java:589`, `Client.java:622`). `GameShell.opGameShell8` sets the same canvas (`jagex2/client/GameShell.java:83`). The game pixmap is `PixMap(503, ..., 765)` (`Client.java:13528`).

Cache kind: `ondemand`. Startup loads jagfile archives, then `OnDemand` for models, anims, midi, and maps (`Client.java:6001`, `jagex2/io/OnDemand.java:539`). Disk cache files for this revision are present under `builds/rs2/377-2006-05-02/cache/`. This tree has no nested source git SHA. `builds/rs2/377-2006-05-02/meta.json` records openrs2 client 31589 and sha256 `9671d16d0aaa87cdbf115717880612bbeb5e115222609c65ed5dd6a701324596`.

There is no `jaggl`, `jagdx`, or `jagtheora` type. Drawing is the software path in `Pix2D` and `Pix3D`.

## Tick

`GameShell.run` sleeps, then calls `update()` and `draw()` (`GameShell.java:175`, `GameShell.java:186`). `Client.update` increments `valueClient1444`, calls `updateTitle` or `updateGame`, then `handleOnDemandRequests` (`Client.java:8772`). `Client.draw` calls `drawTitleScreen` or `drawGame` (`Client.java:12030`).

`updateGame` (`Client.java:1583`), in order:

1. Drain up to five packets with `read()`.
2. Mouse samples, opcode 171, when the login flag `valueClient1094` is set.
3. Click, opcode 19.
4. Arrow-key camera, opcode 140, at most once per 20 ticks.
5. Focus gain or loss, opcode 187.
6. `updateSceneState`, `opClient136`, then synth playback `opClient127`. Reconnect when `valueClient1003` passes 750.
7. Entity maintenance (`opClient112`, `opClient149`, `opClient106`), inventory drag opcode 123, and `useMenuOption`.
8. Scene click walk from `World3D.valueWorld3D119`, then `opClient52` (menu), minimap `opClient125`, `opClient130`, and chat settings.
9. Orbit camera when scene state is 2. Keys via `handleInputKey`.
10. Idle logout opcode 202 after `valueGameShell57` passes 4500. Keepalive opcode 40 after `valueClient1004` passes 50. Flush the outbound buffer.

## Pages

- [login.md](login.md)
- [framing.md](framing.md)
- [packets.md](packets.md)
- [sync.md](sync.md)
- [region.md](region.md)
- [cache.md](cache.md)
- [input.md](input.md)
- [interfaces.md](interfaces.md)
- [scene.md](scene.md)
- [sound.md](sound.md)
- [gaps.md](gaps.md)
- [files.tsv](files.tsv)

## Status

- login: specified
- framing: specified
- packets: specified
- sync: specified
- region: specified
- cache: partial
- input: specified
- interfaces: partial
- scene: partial
- sound: partial
