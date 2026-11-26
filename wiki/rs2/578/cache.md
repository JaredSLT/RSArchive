# Cache

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`.

Kind is js5. `Client.opClient183` opens indices 0..28 with `Cc2fb57c0.opkind10022` (`deob/Client.java:1933`). `C83985cd8.opkind64023(file, group)` returns `valuekind64011[group][file]` (`deob/C83985cd8.java:391` and `C83985cd8.java:511`). `opkind64013` is the CRC written at login (`C83985cd8.java:375`). Reference table 255 is not opened by the lines cited here.

| index | static | use read in this pass |
| --- | --- | --- |
| 0 | `C97d5c431.valuekind7027` | opened, CRC only |
| 1 | `Unused6.valueUnused68` | opened, CRC only |
| 2 | `C67a82e1d.valuekind7141` | configs. Loaders below take this archive unless noted. |
| 3 | `GlRenderList.valueGlRenderList28` | interfaces. Stored as `C9eb3d527.valuekind9635` by `C90803bf8.opkind9771`. |
| 4 | `C36f4c4c6.valuekind7844` | passed into sound init `C783261f2.opkind6562` |
| 5 | `C381a95c3.valuekind101912` | maps, by name. See [region.md](region.md). |
| 6 | `Ca7df0199.valuekind8424` | `scape main` via `opkind64010` (`Client.java:1988`) |
| 7 | `C889f53b6.valuekind8626` | model archive argument of several loaders |
| 8 | `C408bee69.valuekind9673` | sprites passed to the interface loader |
| 9 | `Cb55a2183.valuekind63810` | opened, CRC only |
| 10 | `C34de5186.valuekind8287` | opened, CRC only |
| 11 | `C759ca1fe.valuekind6936` | opened, CRC only |
| 12 | `Cee767227.valuekind7813` | opened, CRC only |
| 13 | `C4743b1d7.valuekind6984` | fonts, first argument of the interface loader |
| 14 | `C23d8b466.valuekind8481` | vorbis archive in `C783261f2.opkind6562` |
| 15 | `FriendListFirst.valuekind8418` | sound bases in that same call |
| 16 | `NpcType.valueNpcType166` | loc models |
| 17 | `ListaDeAtualizaEsCarregada.valuekind8504` | opened, CRC only |
| 18 | `C759ca1fe.valuekind6931` | npc models |
| 19 | `TextureOp29SubOp2.valueTextureOp29SubOp294` | obj models |
| 20 | `C0922b514.valuekind59423` | seq frames |
| 21 | `WorldMapFont.valueWorldMapFont98` | spotanim models |
| 22 | `MaterialManager_Class106_Sub2_Sub1.valueMaterialManager_Class106_Sub2_Sub166` | varbit-style loader `ColFfffff` |
| 23 | `Cf3727c90.valuekind97936` | opened, CRC only |
| 24 | `Cbded41f3.valuekind8155` | opened, CRC only |
| 25 | `C0922b514.valuekind59421` | opened, CRC only |
| 26 | `C00d852fc.valuekind6744` | opened, CRC only |
| 27 | `TextureOp29SubOp2.valueTextureOp29SubOp293` | opened, CRC only |
| 28 | `C36f4c4c6.valuekind7843` | opened, CRC only. Also the archive waited on at startup state 40 (`Client.java:1968`). |

## Id split

`opkind64023` arguments below are `(file, group)`.

| kind | file | group | loader |
| --- | --- | --- | --- |
| npc | `id & 127` `deob/C6cd79f09.java:28` | `id >>> 7` `deob/Entity.java:11` | `Js5ResourceProvider` on config index 2 (`deob/Js5ResourceProvider.java:384`). Type `C1659b20b`. |
| obj | `id & 255` `deob/Sound.java:40` | `id >>> 8` `deob/ImpossibleDeParticiperUneDiscussionDeClanP.java:41` | `C49e80b76` |
| loc | `id & 255` `deob/C6b1ceb6e.java:71` | `id >>> 8` `deob/Cb55a2183.java:61` | `C0b1b24fc` |
| seq | `id & 127` `deob/LinkedList.java:28` | `id >>> 7` `deob/Ce564a28a.java:16` | `BuildAreaSize` returns `Ce0a28d2a`. Frame archive is index 20. |
| spotanim | `id & 255` `deob/C87221f27.java:59` | `id >>> 8` `deob/BuildAreaSize.java:165` | `ByteArrayPool`, type `C261e8130`. Model archive is index 21. |
| index 22 | `id & 1023` `deob/C34de5186.java:34` | `id >>> 10` `deob/Cee94503e.java:158` | `ColFfffff` (`deob/ColFfffff.java:134`) |

## Config groups on index 2

The group is the second argument of `opkind64023`. The file argument is the id, or the split above.

| group | call |
| --- | --- |
| 1 | `Cdf90cca7.java:181` |
| 3 | `C97555c89.java:137` |
| 4 | `Ce564a28a.java:53` |
| 5 | `Ccc85a833.java:57` |
| 11 | `Component.java:45`. This is not the widget class. Widgets are archive 3. |
| 26 | `Cca3bb49b.java:24` |
| 30 | `Ccd2eafaa.java:55` |
| 32 | `PureJavaToolkitContext.java:47` |
| 33 | `C7502a728.java:146` |
| 34 | `Cf2a3eab0.java:91` |
| 36 | `Cc4cace49.java:242`, decode entry `Sound.opSound39` |

Decode opcode switches for npc (`C1659b20b.opkind9307`), obj (`Cd0aaa64c.opkind9433`), loc (`ModelParticleEffector.call56df7df9`), seq, and spotanim were not read. No opcode list is claimed for them.
