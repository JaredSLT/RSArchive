# Cache

Kind is js5. `meta.json` has `"engine": "js5"` and `"cache_downloaded": false`. This archive has no disk cache. Do not download one. The client still opens `main_file_cache.dat2`, `main_file_cache.idx255`, and `main_file_cache.idxN` under the cache directory (`GameShell.java:171-176`).

`C268a19ef.opkind32373` returns one more than the highest archive id (`C268a19ef.java:55-69`). The highest id in the constructors is 37, so the index count is 38. There is no id 22. Constructors are `C268a19ef.java:2-38`.

## Archives opened by Loading

`Client.opClient611` is the Js5 factory (`Client.java:1580`). Bindings:

| Id | Field | Where | Later use that was read |
| --- | --- | --- | --- |
| 0 | `valuekind323723` | `Loading.java:221` `valueStatics908` | passed into a list at `Loading.java:338` |
| 1 | `valuekind32372` | `Loading.java:222` `valueStatics858` | same list |
| 2 | `valuekind32373` | `Loading.java:223` `valueStatics1046` | config archive argument of the type lists |
| 3 | `valuekind32374` | `Loading.java:224` `valueStatics991` | interfaces (`Loading.java:351`) |
| 4 | `valuekind32375` | `Loading.java:225` `valueStatics748` | |
| 5 | `valuekind32376` | `Loading.java:226` `valueStatics719` | maps |
| 6 | `valuekind323717` | `Loading.java:227` `valueStatics627` | MIDI (`C2a31b481.java:274`) |
| 7 | `valuekind32378` | `Loading.java:228` `valueStatics902` | models |
| 8 | `valuekind32379` | `Loading.java:220` `valueStatics954` | sprites |
| 9 | `valuekind323710` | `Loading.java:229` `valueStatics1054` | |
| 10 | `valuekind323726` | `Loading.java:231` `valueStatics987` | huffman group name at `Loading.java:360` |
| 11 | `valuekind323712` | `Loading.java:232` `valueStatics1075` | jingle MIDI (`C2a31b481.java:308`) |
| 12 | `valuekind323713` | `Loading.java:233` `valueStatics880` | |
| 13 | `valuekind323737` | `Loading.java:158` `valueStatics750` | fonts, passed to the interface loader |
| 14 | `valuekind323711` | `Loading.java:234` `valueStatics986` | |
| 15 | `valuekind323733` | `Loading.java:235` `valueStatics929` | |
| 16 | `valuekind323727` | `Loading.java:236` `valueStatics803` | locations (`Loading.java:330`) |
| 17 | `valuekind323718` | `Loading.java:237` `valueStatics837` | |
| 18 | `valuekind323715` | `Loading.java:238` `valueStatics567` | NPCs (`Loading.java:335`) |
| 19 | `valuekind323720` | `Loading.java:239` `valueStatics802` | objects (`Loading.java:336`) |
| 20 | `valuekind323739` | `Loading.java:240` `valueStatics1067` | |
| 21 | `valuekind32377` | `Loading.java:241` `valueStatics975` | |
| 23 | `valuekind323719` | `Loading.java:242` `valueStatics720` | |
| 24 | `valuekind323724` | `Loading.java:243` `valueStatics1053` | |
| 25 | `valuekind323725` | `Loading.java:244` `valueStatics1074` | |
| 26 | `valuekind323735` | `Loading.java:245` `valueStatics829` | |
| 27 | `valuekind323722` | `Loading.java:246` `valueStatics641` | |
| 28 | `valuekind323728` | `Loading.java:247` `valueStatics1065` | graphics defaults (`Loading.java:299`) |
| 29 | `valuekind323729` | `Loading.java:248` `valueStatics807` | |
| 30 | `valuekind323730` | `Loading.java:250` `valueStatics1060` | |
| 31 | `valuekind323731` | `Loading.java:251` `valueStatics935` | |
| 32 | `valuekind323732` | `Loading.java:156` | one of the two early archives, chosen with id 34 |
| 33 | `valuekind323716` | `Loading.java:157` `valueStatics672` | |
| 34 | `valuekind323734` | `Loading.java:156` | the other early archive |
| 35 | `valuekind323721` | `Loading.java:249` `valueStatics1099` | |
| 36 | `valuekind323736` | `Loading.java:252` `valueStatics701` | reference table count argument is 2 |
| 37 | `valuekind32371` | `Loading.java:230` `valueStatics952` | |

Group names inside those archives were not read, except map names in [region.md](region.md), the huffman group `"huffman"`, and the graphics-defaults file name `C92f8cbca.valuekind35948.valuekind35943` (`GraphicsDefaults.java:23`). That name field was not opened.

## Graphics defaults

`GraphicsDefaults.opkind35951` loops on u8 opcodes until 0 (`GraphicsDefaults.java:27-47`). An opcode that is not listed below is not consumed, so the loop would desynchronise.

| Opcode | Reads |
| --- | --- |
| 1 | one i16be pair per current array length (`GraphicsDefaults.java:49-60`) |
| 2 | bigsmart (`62-63`) |
| 3 | u8 new array length. The values are filled by opcode 1 (`64-67`) |
| 4 | no bytes. Clears a flag (`68-69`) |
| 5 | u24be (`70-71`) |
| 6 | u24be. This is the root interface id (`72-73`, opened at `Client.java:1571`) |
| 7 | 10 by 4 blocks. Each cell is a u16be (65535 becomes -1), a u16be count, and that many u16be (65535 becomes -1) (`93-117`) |
| 8 | no bytes. Clears a flag (`75-76`) |
| 9 | u8 (`77-78`) |
| 10 | no bytes. Clears a flag (`79-80`) |
| 11 | u8 (`81-82`) |
| 12 | u16be width, u16be height (`83-85`) |
| 13 | u8 (`86-87`) |
| 14 | u8 (`88-89`) |
| 15 | u8 (`90-91`) |

## ObjType

`ObjType.opkind37292` reads u8 opcodes until 0 (`ObjType.java:78-86`). `opkind372919` (`ObjType.java:89-305`) implements:

| Opcode | Reads |
| --- | --- |
| 1 | bigsmart |
| 2 | jstr |
| 4, 5, 6 | u16be |
| 7, 8 | u16be, then subtract 65536 when the value is above 32767 |
| 11 | no bytes |
| 12 | u32be |
| 13, 14 | u8 |
| 16 | no bytes |
| 18 | u16be |
| 23, 24, 25, 26 | bigsmart |
| 27 | u8 |
| 30..34 | jstr |
| 35..39 | jstr |
| 40 | u8 count, then that many u16be pairs |
| 41 | u8 count, then that many u16be pairs |
| 42 | u8 count, then that many i8 |
| 43 | u32be |
| 44 | u16be, consumed as a bit mask. No further bytes |
| 45 | u16be, same |
| 65 | no bytes |
| 78, 79, 90, 91, 92, 93 | bigsmart |
| 95 | u16be |
| 96 | u8 |
| 97, 98 | u16be |
| 100..109 | u16be, u16be |
| 110, 111, 112 | u16be |
| 113, 114 | i8. Opcode 114 multiplies by 5 |
| 115 | u8 |
| 121, 122 | u16be |
| 125, 126 | three i8 |
| 127, 128, 129, 130 | u8, u16be |
| 132 | u8 count, then that many u16be |
| 134 | u8 |
| 139, 140 | u16be |
| 142..146 | u16be |
| 150..154 | u16be |
| 249 | u8 count, then that many of: u8 `== 1`, u24be, and either a jstr or a u32be |

Any other opcode falls out of the chain without reading. NPC, location, sequence, and the other config decoders were not opened.
