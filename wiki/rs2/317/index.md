# Revision 317

| | |
| --- | --- |
| Revision | 317 |
| Date | 2005-06-13 |
| Family | rs2 |
| Main class | `jagex2.client.Client` |
| Canvas | 765 by 503 |
| Cache | ondemand, with jag archives loaded at startup |
| Tick | `GameShell.run` sleeps, calls `Client.update` while a 256-step accumulator fills, then `Client.draw`. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/GameShell.java:93` |

`Client.main` prints `RS2 user client - release #317` and calls `GameShell.initApplication(503, false, 765)`. The applet path sets the same 765 by 503 canvas. `Client.update` increments `loopCycle`, runs `updateTitle` or `updateGame`, then `handleOnDemandRequests`. `updateGame` reads at most five packets, then writes mouse, camera, and focus traffic, and flushes the outbound buffer at the end of the method. `Client.draw` calls `drawTitleScreen` or `drawGame`.

There is no XTEA in this client. Map files come from ondemand archive 3. There is no `rev_317` move layout; walking is built in `Client.tryMove`.

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
- input: specified
- scene: partial
- interfaces: specified
- sound: partial
