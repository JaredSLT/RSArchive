# Revision 902

| | |
| --- | --- |
| Revision | 902 |
| Date | 2018-12-14 |
| Family | rs3 |
| Main class | `client` in `builds/rs3/902-2018-12-14/meta.json`. The game class is `rs2.client.Client`. `AppletViewer.main` constructs it (`AppletViewer.java:60`, `AppletViewer.java:77`). |
| Canvas | Default 765 by 553 (`Client.tint73`, `Client.tint74` at `builds/rs3/902-2018-12-14/src/src/main/java/rs2/client/Client.java:989`). Passed to `GameShell.startApplet` as `DataBlock185` (`Client.java:1441`, `GameShell.java:106`). A GWC record can replace both (`deob/DataBlock374.java:329`). |
| Cache | js5. This archive has no disk cache. |
| Source | `builds/rs3/902-2018-12-14/src` has no nested git. `meta.json` sha256 of `original/gamepack.jar` is `49dc0968db9d24af777d0851c7dbbe70c6a2496a3a56ff401d1d19483f876ee9`. |

## Tick

`Client.doClient2` (`Client.java:1540`) is the logic tick:

1. Return when `state == 13`.
2. Optional buss volume, then the audio wait flag.
3. Increment `tint10`. Every 1000 cycles, reseed from the clock.
4. Flush both sockets (`clsb20f177f2`, `clsb20f177f`), then the JS5 TCP client and the JS5 provider.
5. Buss tick, keyboard `OccluderPart`, native mouse, toolkit dt.
6. Drain up to 75 key events and the mouse queue.
7. Login states call `DefaultSprites.doDefaultSprites2222b` and `SecureRandomTask.blankWs`. State 16 calls `blankWs` only. The logged-in state calls `doClient2222b242`. States 4 and 14 retry or log out.
8. Image decode, then the mouse queue is drained again.

`doClient2222b242` (`Client.java:3231`) reads up to 100 server packets through `doClient2(DataBlock335)`, which calls `doClient22` (`Client.java:7254`). While state is 19 it may send the reflection packet and the GC packet, then calls the camera and the second input writer. The method continues after line 3350 (unread). `Client.doClient22()` at line 1716 is a second tick; only its dt sample was read. `Client.doClient2222` at line 1837 was not read.

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

## Status

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
