# Revision 115

| | |
| --- | --- |
| Family | rsc |
| Date | 2001-12-24 |
| Main class | `deob.MudClient` |
| Canvas | shell `512 x 345`, pixmap `512 x 346` |
| Cache | jag |
| Source | `builds/rsc/115-2001-12-24/src/src/main/java` |

`meta.json` records openrs2 client id 34058, sha256 `b7f76f90f3313f8560aa6c4340c06aad9e263aec20db69fca81e3cd5aab6451d`, 110382 bytes, jar, live. `openrs2_cache_id` is null. This worktree has no nested git SHA for the client tree.

`MudClient.main` calls `GameShell.initApplication(512, 334 + 11, "Runescape by Andrew Gower", false)` (`builds/rsc/115-2001-12-24/src/src/main/java/deob/MudClient.java:734`). The constructor sets `value512 = 512`, `value334 = 334`, `value9 = 9` (`MudClient.java:8361`). `load` builds `MudPix(512, 334 + 12, 2700, this)` (`MudClient.java:780`) and `setFramerate(50)`, which sets `deltime = 1000 / 50` (`GameShell.java:203`). The viewport is `calle1ef7591(256, 167, 256, 167, 512, 9)` (`MudClient.java:796`).

Version constants are `Version.value115` and `value1152`, both 115 (`deob/Version.java:6`). The login packet writes `value1152`. Archive names use config 48, maps 27, media 28, models 12, textures 8, entity 10 (`Version.java:10`).

## Tick

`GameShell.run` loads `jagex.jag`, calls `load()`, then sleeps and calls `update()` while a 256-step accumulator is below 256, capped by `value1000`, then `draw()` (`jagex/client/GameShell.java:445`). In game, `MudClient.update` calls `call0bbcb899` (`MudClient.java:1052`). That method reads the socket first (`Client.callb6b49066` at `MudClient.java:2220`), steps the logout timer, entities, chat, and camera, and the matching draw is `call34c75df3` (`MudClient.java:4185`), which draws the world when `world.flag427` is set.

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
- packets: specified
- sync: partial
- region: partial
- cache: partial
- input: specified
- scene: partial
- interfaces: specified
- sound: gap
