# Revision 745

| | |
| --- | --- |
| Family | rs2 |
| Date | 2012-12-07 |
| Revision | 745 |
| Game class | `Client` extends `GameShell` (`builds/rs2/745-2012-12-07/src/src/main/java/Client.java:23`) |
| Canvas | 765 by 553 (`Client.java:408-409`). GraphicsDefaults opcode 12 can replace both (`GraphicsDefaults.java:83-85`, applied at `Loading.java:302-304`) |
| Cache | js5. Disk files `main_file_cache.dat2`, `main_file_cache.idx255`, and `main_file_cache.idxN` (`GameShell.java:171-176`). No disk cache is in this archive |
| Source tree | Archive commit `962888d50a69a95e6011e2018a39fe668ed4982a`. The build directory is not a nested git repo |

`meta.json` names the pack class `client`. The applet entry in this tree is `Client`.

Tick: `GameShell.valueGameShell175` is `20000000` ns (`GameShell.java:23`). The shell loop asks how many of those slices elapsed, runs `opGameShell96` once per slice, then runs `opGameShell97` once (`GameShell.java:610-617`). `opGameShell96` calls `Client.opClient589` (`GameShell.java:651`). `opGameShell97` calls `Client.opClient588` (`GameShell.java:677`). `opClient589` calls `opClient490` (`Client.java:844-847`). `opClient490` services both connections, then `opClient500`, an optional login helper, the sound tick `C2a31b481.opkind351022`, the keyboard queue, and the mouse queue, then dispatches on `valueClient1760` (`Client.java:886-977`). Logged-in state 19 calls `opClient579`, which drains up to 100 server packets through `opClient593` before the rest of that method (`Client.java:976-977`, `Client.java:2608`).

## Pages

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
- [files.tsv](files.tsv)

## Status

| Subsystem | Status |
| --- | --- |
| login | partial |
| framing | specified |
| packets | partial |
| sync | partial |
| region | specified |
| cache | partial |
| input | partial |
| interfaces | partial |
| scene | partial |
| sound | partial |
