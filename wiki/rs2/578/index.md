# Revision 578

| | |
| --- | --- |
| Family | rs2 |
| Date | 2009-12-16 |
| Main class | `deob.Client` |
| Cache | js5. `Client.opClient183` opens disk indices 0..28 after the JS5 handshake (`builds/rs2/578-2009-12-16/src/src/main/java/deob/Client.java:1933`). |
| Canvas | `Client.main` builds the applet at 1024 by 768 and passes revision 578 (`Client.java:79`). Static defaults are width 765 and height 503 (`deob/Ca0b9e3f8.java:13`, `deob/QuickChatPhrase.java:4`). If the toolkit mode is `C98dff673.valuekind7622`, both become 640 and 480; if it is `C0d93e06d.valuekind9062`, they are set to 765 and 503 (`Client.java:1394`). |

## Tick

`Client.opClient154` (`Client.java:1409`) runs while login state `Ca4c92d3c.valuekind7069` is not 1000. It increments `C040791af.valuekind10044`, then:

1. `opClient224`
2. JS5 poll `Cc625958e.valuekind7195.opkind8377` when the client exists
3. `Cfdcd577a.opkind9622`
4. `C3f19e1f2.opkind7091`
5. mouse `PreLoadingScreen.valuePreLoadingScreen3.opkind8948` and wheel `ListaDeAtualizaEsCarregada.valuekind8507.opWorldMapFont27`
6. optional camera device `Ccc85a8332.valuekind7323.opkind6453`
7. toolkit `TextureOp19.valueTextureOp1989.opVorbisCodebook9`
8. `Cfcdb42e4.opkind9544`
9. keyboard records into `Cd24b839a.valuekind8794`, at most 128 (`Client.java:1446`)
10. mouse records; a pressed record is kept in `MsiType.valueMsiType150`, a move record is queued
11. state 0 or 5: `opClient183` and `Cd0aca380.opkind8867`
12. state 25 or 28: `SuaListaDeIgnoradosEstCheia.opkind8193`
13. state 10: `opClient221`, `Cd0aca380.opkind8868`, `TextureOp29SubOp4.opDxStencilBuffer1`, `LswprPos.calla978325d`
14. state 40: `LswprPos.calla978325d`
15. state 30: `Cd6e44077.opkind10291`, which calls `C00d852fc.opkind6743` up to 100 times (`Cd6e44077.java:34`). That method is `C268455a9.opkind9551`.
16. `C244815a8.opStringNode1`

`~x == -N` in this decompile means `x == N - 1`. State 30 is the branch `~valuekind7069 != -31` falling through to the else at `Client.java:1499`. State 40 is `~valuekind7069 == -41`.

## Pages

- [files.tsv](files.tsv)
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

## Status

- login: specified
- framing: partial
- packets: partial
- sync: specified
- region: partial
- cache: partial
- input: partial
- scene: partial
- interfaces: partial
- sound: partial
