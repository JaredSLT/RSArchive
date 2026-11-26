# Revision 350

- Revision: 350
- Date: 2005-12-30
- Family: rs2
- Main class: `deob.Client` (`Client.main` at `builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:603`)
- Canvas: 765 by 503. `Client.main` calls `GameShell.initApplication(503, false, 765)` (`Client.java:636`). `GameShell.initApplication` stores width 765 and height 503 (`builds/rs2/350-2005-12-30/src/src/main/java/deob/GameShell.java:63`). The game pixmap is created as `new PixMap(765, 503, ...)` (`Client.java:10547`).
- Cache kind: ondemand. Startup archives are jagfiles fetched by CRC. Runtime models, anims, midi, and maps use `deob.OnDemand`.
- Source: decompiled in this tree under `builds/rs2/350-2005-12-30/src/src/main/java`. This revision page does not record a nested git SHA.

## Tick

`GameShell.run` sleeps, then while the cycle accumulator is below 256 calls `opGameShell2` and afterwards calls `draw` (`GameShell.java:169`). `Client.opGameShell2` increments `valueClient833`, then calls `updateTitle` or `updateGame`, then `opClient71` (`Client.java:2284`).

`Client.updateGame` (`Client.java:12315`) order:

1. Up to five `Client.read` calls (`Client.java:12325`).
2. Mouse-sample packet 224, click packet 150, camera packet 130, focus packet 139 (`Client.java:12333`, `12428`, `12443`, `12450`).
3. `updateSceneState`, zone anticheat `opClient103`, `updateAudio` (`Client.java:12460`).
4. If `valueClient500` passes 750, `tryReconnect` (`Client.java:12464`).
5. NPC and player movement (`opClient100`, `opClient82`), then `updateEntityChats` (`Client.java:12469`).
6. Drag, menu, and scene click, including walk opcode 246 when the world is clicked (`Client.java:12548` and `useMenuOption`).
7. Orbit camera or cutscene when `valueClient880 == 2` (`Client.java:12612`).
8. `handleInputKey`, anticheat opcode 1, idle opcode 63 when `valueGameShell24` passes 4500, anticheat opcode 58 every 50 cycles (`Client.java:12624`, `12628`, `12636`, `12711`).
9. Flush `valueClient739` when `valuekind8722 > 0` (`Client.java:12715`).

`Client.draw` calls `drawTitleScreen` or `drawGame` (`Client.java:2794`). `drawGame` calls `drawScene` when `valueClient880 == 2` (`Client.java:11005`).

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
- region: partial
- cache: specified
- input: specified
- scene: partial
- interfaces: partial
- sound: partial
