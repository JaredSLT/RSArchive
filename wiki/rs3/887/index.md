# RS3 887

| | |
| --- | --- |
| Family | rs3 |
| Revision | 887 |
| Date | 2017-11-01 |
| Engine | js5 |
| Cache home | `.jagex_cache_32/runescape` |
| Cache downloaded | false |
| Gamepack SHA-256 | `0f00099d8e15b8005cbc631b46d0368a057751b50192191bbc600877ae9d18d8` |
| Source | `builds/rs3/887-2017-11-01/src/src/main/java` |
| Nested git | none. The tree is in this repo. Worktree base `962888d5`. |
| Main class | `MAIN_CLASS.txt` says `AppletViewer`. `meta.json` says `client`. `Client` passes 887 into `startApplet` at `builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:1411`. |
| Canvas | `GameShell` calls `canvas.setSize(Statics.tint42, Statics.tint87)` at `builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/client/GameShell.java:520` and `:688`. The numeric default is unread. |
| Tick order | unread. `Client.doClient2` at `builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:1416` is applet startup. |

`com.jagex.encryption.Isaac` is a font loader. The cipher is `deob.DataBlock482`. The buffer is `com.jagex.game.world.entity.PositionedSound`. The bit buffer is `deob.PositionedSoundPart`.

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

- login: partial
- framing: specified
- packets: partial
- sync: partial
- region: partial
- cache: partial
- input: partial
- scene: partial
- interfaces: partial
- sound: partial
