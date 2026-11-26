# Revision 254

| | |
| --- | --- |
| Family | rs2 |
| Date | 2004-09-07 |
| Source | `builds/rs2/254-2004-09-07/src/src/main/java` |
| Source commit | `962888d50a69a95e6011e2018a39fe668ed4982a` (this archive; the build is not a nested git repo) |
| Main class | `jagex2.client.Client` (`src/MAIN_CLASS.txt`, `Client.main` at `Client.java:592`) |
| Canvas | 765 by 503 (`Client.main` calls `initApplication(765, 503)` at `Client.java:625`; applet `GameShell.initApplet` sets the same at `GameShell.java:84`) |
| Cache kind | `ondemand` (title and config are jag archives; maps, models, and midi use `OnDemand`) |
| Tick | `GameShell.run` sleeps on `deltime` (20 ms, `GameShell.java:32`), then `refresh` then `draw` (`GameShell.java:180` and `GameShell.java:190`). `Client.refresh` bumps `loopCycle` and calls `updateTitle` or `updateGame`, then `onDemandLoop` (`Client.java:1547`). `Client.draw` calls `drawTitleScreen` or `drawGame` (`Client.java:9760`). |

`Client.updateGame` (`Client.java:10547`) reads up to five server packets, writes mouse, click, camera, and focus packets, runs scene and audio, steps players and NPCs, then flushes `out` if it is non-empty (`Client.java:10935`).

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

## Subsystems

| Subsystem | Status |
| --- | --- |
| login | specified |
| framing | specified |
| packets | partial |
| sync | specified |
| region | specified |
| cache | specified |
| input | specified |
| scene | partial |
| interfaces | specified |
| sound | specified |
