# Revision 468

| | |
| --- | --- |
| Family | rs2 |
| Date | 2007-08-08 |
| Revision literal | 468, written in the login block and passed to the shell (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/CursorManager.java:159`, `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:225`) |
| Main class | `rt4.Client`, which extends `deob.PleaseSubscribeUseDifferentWorld` (`builds/rs2/468-2007-08-08/src/src/main/java/deob/PleaseSubscribeUseDifferentWorld.java`, `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:224`) |
| Canvas | 765 by 503 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:225`) |
| Cache | js5. Sixteen archives are opened in `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5.java:88` |
| Host | `127.0.0.1` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:223`) |
| Nested git SHA | none. The revision directory is not its own repository |

Names in this tree are scrambled. `MouseRecorder` is the byte buffer, `Cache` is that buffer plus ISAAC, and `MidiPcmStream` is the ISAAC generator. The packet switch is `GlFont.opGlFont5`, not `Client`.

## Tick

`Client.opClient165` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:250`) does this, in order:

1. Increment `DirectByteArray.valueDirectByteArray12`.
2. `opClient170`. That method was not read.
3. `VorbisFloor.opVorbisFloor8`.
4. `MaterialManager_Class106_Sub1.opMaterialManager_Class106_Sub115`.
5. `Timer.opTimer13`.
6. `GameShell.calle0d3a1da`.
7. `MidiNoteStream.opMidiNoteStream5`.
8. If `C99aed688.valuekind20513` is set, `LightType.opLightType3` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/LightType.java`).
9. State in `TextureOpInvert.valueTextureOpInvert2`:
   - 0: `Js5.callf78aac57` and `PcmStream.opPcmStream3`.
   - 5: `DelayedStateChange.opDelayedStateChange1`, then the same cache and pcm calls.
   - 10: `DelayedStateChange.opDelayedStateChange1`.
   - 20: that delayed call, then `CursorManager.call08d5362c` (login).
   - 25: `EmpfNger.opkind1389` (map fetch).
   - 30: `TextureOp19.opTextureOp194`.
   - 40: `CursorManager.call08d5362c`.

`opClient154` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:296`) starts with `opClient166`, `Scenery.opScenery5`, and `PleaseSubscribeUseDifferentWorld.opkind13017`. It is a separate callback. The software draw loop was not read.

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
| sync | specified |
| region | specified |
| cache | partial |
| input | partial |
| scene | partial |
| interfaces | partial |
| sound | partial |
