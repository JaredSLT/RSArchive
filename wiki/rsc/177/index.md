# RuneScape Classic 177

| | |
| --- | --- |
| Family | rsc |
| Game | classic |
| Revision | 177 |
| Date | 2003-10-31 |
| Main class | `deob.MudClient` |
| Canvas | 512 by 334. `initApplication` uses height 334+11. `MudPix` is 512 by 334+12. Error fill is 512 by 356. |
| Cache | jag |
| Source | openrs2 client 33805, sha256 `139beb51db766d91315b6a6762a1c55777ec1ae08889cb1cac742a746e416188` |

`ClientConfig.number92` and `Stream.number640` / `Tools.number643` are fernflower loop flags. `number92` is written only when `Tools.number643 != 0`, and `number643` advances only when `number92 != 0`. The live path is `number92 == 0` and `number640 == 0`. Loops still run. The flag only picks the while or do-while entry. Pages below describe the `number92 == 0` path.

Tick: `GameShell.run` calls `update` then `draw` (`jagex/client/GameShell.java:415`). `load` sets the frame rate to 50 and `main` sets `mindel` to 10 (`deob/MudClient.java:809`, `deob/MudClient.java:876`). While `number384 == 0`, `update` runs the login UI. While `number384 == 1`, it increments idle cycles and calls `call0bbcb899`, which calls `Client.callb6b49066` first (`deob/MudClient.java:1308`, `deob/MudClient.java:2831`). That reads one framed server packet and may send heartbeat opcode 5. The mouse click is cleared at the end of `update`.

There is no ISAAC cipher and no opcode size table. Integers on the socket and in jag integer streams are big-endian. See [framing](framing.md).

## Pages

- [login](login.md)
- [framing](framing.md)
- [packets](packets.md)
- [sync](sync.md)
- [region](region.md)
- [cache](cache.md)
- [input](input.md)
- [scene](scene.md)
- [interfaces](interfaces.md)
- [sound](sound.md)
- [gaps](gaps.md)
- [files.tsv](files.tsv)

## Status

- login: specified
- framing: specified
- packets: specified
- sync: specified
- region: partial
- cache: specified
- input: partial
- scene: partial
- interfaces: partial
- sound: specified
