# Revision 282

- Revision: 282
- Date: 2004-12-21
- Family: rs2
- Main class: `deob.Client` (`Client.main` prints `RS2 user client - release #282`)
- Canvas: 765 by 503. `Client.main` calls `GameShell.opkind11(503, 765, 461)`, which stores width 765 and height 503.
- Cache kind: ondemand. Jag archives supply title, config, interface, media, versionlist, textures, wordenc, and sounds. Models, anims, midis, and maps are requested on a second socket. This client has no js5 and no jaggl, jagdx, or jagtheora.
- Tick: `GameShell.run` sleeps, then calls `loop` while a 256-step accumulator is below 256. `valuekind131` defaults to 20 and `valuekind132` defaults to 1. `Client.loop` increments `valueClient169`, calls `updateTitle` or `updateGame`, then `onDemandLoop`. `updateGame` decrements timers, reads up to five server packets, then, while logged in, writes mouse tracking, the click packet, the camera packet, and the focus packet, runs `updateSceneState`, `updateTemporaryLocs`, and `updateAudio`, reconnects after 750 idle reads, runs `opClient29`, `opClient31`, and `opClient19`, handles drag and tab flash, walks from a world click, handles mouse, minimap, tabs, chat mode, and keys, sends idle and keepalive, and flushes the outbound buffer.

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
- cache: specified
- input: specified
- scene: partial
- interfaces: partial
- sound: specified
