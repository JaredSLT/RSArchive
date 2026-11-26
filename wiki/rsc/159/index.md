# Revision 159

- Revision: 159 (`Version.value1592`, written on the login packet as `Client.number686`).
- Date: 2002-11-25.
- Family: rsc.
- Main class: `deob.MudClient`, started from `MudClient.main`.
- Canvas: `initApplication(512, 345, "Runescape by Andrew Gower", false)`. The game area is 512 by 334. The software surface is `MudPix(512, 346, 2700)`. The AWT frame is `ViewBox`, title-bar height 28, or 48 when the last constructor flag is set.
- Cache kind: jag.
- Tick: `GameShell.run` sleeps, then calls `update()` until the frame budget is spent, then calls `draw()` once. `setFramerate(50)` sets `deltime` to `1000/50`. Logged out, `update` is `MudClient.call11091c54`. Logged in, `update` is `MudClient.call0bbcb899`, which starts with `Client.callb6b49066`. Logged-in `draw` is `MudClient.call34c75df3`.

`Version.value159` and `Version.value1592` are both 159. Maps and land use `Version.value50` (50). Config uses `Version.value71` (71). Media uses `Version.value45` (45). Entity archives use `Version.value18` (18). Textures use `Version.value14` (14). Models use `Version.ENTITY` (24). Sounds use `Version.number9` (1).

There is no nested git checkout under this revision tree.

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
- sync: specified
- region: specified
- cache: partial
- input: specified
- scene: partial
- interfaces: specified
- sound: specified

Citations are paths relative to the RSArchive repo. Decompiled override names do not match the parent Java name; the matching method is the `MudClient` method whose `@ObfuscatedName` suffix equals the `Client` suffix (`e.e` is `mudclient.e`, and the same for `f`, `k`, `u`, `n`, `g`, `bb`). `Client.call215` (`e.a`) has no `mudclient.a` method in this tree.
