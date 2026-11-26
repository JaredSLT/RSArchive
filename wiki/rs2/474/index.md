# Revision 474

| | |
| --- | --- |
| Family | rs2 |
| Revision | 474 |
| Date | 2007-10-19 |
| Main class | `rt4.Client` |
| Canvas | 765 by 503 |
| Cache | js5 |
| Game port | live 43594, otherwise 40000 plus the world id |
| Update port | live 443, otherwise 50000 plus the world id |

`Client.main` passes height 503, revision 474, and width 765 into `call5dae9383` (`builds/rs2/474-2007-10-19/src/src/main/java/rt4/Client.java:134`). A live world uses char `ꩊ` (43594) for the game socket and 443 for the update socket. Any other mode uses `'鱀' + world` (40000 plus the world id) and `'썐' + world` (50000 plus the world id) (`Client.java:237-238`).

## Tick

`Client.opClient173` (`Client.java:279-325`) then `Client.opClient164` (`Client.java:691-746`).

1. Increment `VarpType.valueVarpType71`. On every 1000th tick, reseed `IntHashTable.valueIntHashTable3` from the clock.
2. `callb527caf9` keeps the JS5 socket up. `Cache.opCache20`, `ReflectionCheck.opReflectionCheck9`, `C82c4f4c4.opkind2322`, `ReferenceNodeFactory.opReferenceNodeFactory11`, and `HashTableIterator.opHashTableIterator11` run, then the mouse wheel is sampled.
3. State 0 runs `MidiNote.opMidiNote10` and `MonotonicClock.opMonotonicClock60`. State 5 adds `MapElement.call8d5bcb84`. State 10 is that login-screen call only. States 20 and 40 run the login-screen call (20 only) and `TextureOp29SubOp.opTextureOp29SubOp8`. State 25 runs `TextureOp35.opTextureOp351`. State 30 runs `InvType.opInvType12`.
4. Draw: state 0 paints the loading string, states 5, 10, and 20 paint the title, state 25 paints the map-load string, state 30 calls `TextureOp19.opTextureOp197`, and state 40 paints the login status string.

Logged-in state 30 reads up to 100 server packets, writes mouse, click, camera, focus, and interface traffic, then flushes `InvType.valueInvType19` on the game socket (`InvType.java:78`, `InvType.java:472-476`).

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

| Subsystem | Status |
| --- | --- |
| login | specified |
| framing | specified |
| packets | partial |
| sync | partial |
| region | partial |
| cache | partial |
| input | partial |
| scene | partial |
| interfaces | partial |
| sound | partial |
