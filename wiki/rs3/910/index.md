# Revision 910

- Family: rs3
- Date: 2019-10-16
- Source commit: `962888d50a69a95e6011e2018a39fe668ed4982a`. The Java sources are in this archive tree. There is no nested git under the revision.
- Game class: `rs2.client.Client` extends `GameShell` (`builds/rs3/910-2019-10-16/src/src/main/java/rs2/client/Client.java:339`).
- `builds/rs3/910-2019-10-16/src/MAIN_CLASS.txt` names `AppletViewer`. `builds/rs3/910-2019-10-16/meta.json` sets `main_class` to `client`.
- Stored frame size: 765 by 553 (`Client.java:1521`). `WINDOW_STATUS` sends the live `GameShell.canvasWid` and `GameShell.canvasHei`.
- Cache kind: js5 (`meta.json` field `engine`). `cache_downloaded` is false. `openrs2_cache_id` is null. Cache home string is `.jagex_cache_32/runescape`.
- Tick: `GameShell.logicUpdateInterval` is 20000000 ns (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/client/GameShell.java:44`), so `getLogicRate` returns 50 (`GameShell.java:856`). `Client.mainloop` (`Client.java:2074`) increments `loopCycle`, updates JS5, audio, keyboard, and mouse, then selects a state. Title and lobby call `LoginManager.update`. Rebuild calls `world.rebuildScene`. The in-game branch calls `updateGame2` (`Client.java:2198`). `updateGame2` (`Client.java:3936`) reads up to 100 packets while `state == 18`.

## Pages

- [files.tsv](files.tsv)
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

## Subsystem status

- login: partial
- framing: specified
- packets: partial
- sync: partial
- region: partial
- cache: partial
- input: partial
- interfaces: partial
- scene: partial
- sound: partial
