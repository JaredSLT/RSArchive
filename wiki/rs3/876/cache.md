# Cache

`builds/rs3/876-2016-11-28/meta.json` records engine `js5`, cache home `.jagex_cache_32/runescape`, and `cache_downloaded` false. This revision has no disk cache in the archive. Do not download one. Values that exist only as archive bytes are the blocked row in [gaps.md](gaps.md).

The archive id is the first argument of each `RawCpuInfo` constructor (`com/jagex/game/client/RawCpuInfo.java:105`). The client opens them in `CameraSettingType` (`com/jagex/graphics/camera/CameraSettingType.java:265`).

| Id | Enum | Static | Cited use |
| --- | --- | --- | --- |
| 0 | `rawCpuInfo` | `js512` | opened at `:332` |
| 1 | `rawCpuInfo2` | `js523` | opened at `:333` |
| 2 | `rawCpuInfo3` | `js58` | config lists from `:438` |
| 3 | `rawCpuInfo4` | `js525` | interfaces, `DataBlock1540.doDataBlock1200` at `:475` |
| 5 | `rawCpuInfo5` | `js5` | maps, `:337` |
| 7 | `rawCpuInfo6` | `js520` | models, `:338` |
| 8 | `rawCpuInfo7` | `js52` | sprites, `:331` |
| 10 | `rawCpuInfo8` | `js528` | group name `huffman`, `:486` |
| 12 | `rawCpuInfo9` | `js529` | `:342` |
| 13 | `rawCpuInfo10` | `js511` | third extra archive of the interface loader, `:267` |
| 14 | `rawCpuInfo11` | `js515` | `:343` |
| 16 | `rawCpuInfo12` | `js56` | `PreferencesUnknown7`, passed to `Client.world.doWorld` at `:451` |
| 17 | `rawCpuInfo13` | `js535` | `:346` |
| 18 | `rawCpuInfo14` | `js536` | `Lookat_Sub2` at `:456` |
| 19 | `rawCpuInfo15` | `js516` | `:348` |
| 20 | `rawCpuInfo16` | `js542` | `:349` |
| 21 | `rawCpuInfo17` | `js519` | `:350` |
| 22 | `rawCpuInfo18` | `js54` | `SeqTypeList` at `:464` |
| 23 | `rawCpuInfo19` | `js524` | `:352` |
| 24 | `rawCpuInfo20` | `js514` | `:354` |
| 25 | `rawCpuInfo21` | `js59` | `:355` |
| 26 | `rawCpuInfo22` | `js531` | `BasicTextureListInner` at `:436` |
| 27 | `rawCpuInfo23` | `js53` | `DataBlock1596` and `DataBlock1186` at `:477` |
| 28 | `rawCpuInfo24` | `js526` | `DataBlock1793` and the colour tables at `:414` |
| 29 | `rawCpuInfo25` | `js539` | `:358` |
| 30 | `rawCpuInfo26` | `js522` | `:360` |
| 31 | `rawCpuInfo27` | `js532` | `:361` |
| 32 or 34 | `rawCpuInfo28` or `rawCpuInfo30` | `js518` | `Statics.tboolean` selects id 34, otherwise id 32 (`:265`) |
| 33 | `rawCpuInfo29` | `js527` | `:266` |
| 35 | `rawCpuInfo31` | `js513` | `:359` |
| 40 | `rawCpuInfo32` | `js537` | `:344` |
| 41 | `rawCpuInfo33` | `js543` | `:353` |
| 43 | `rawCpuInfo35` | `js57` | `:339` |
| 44 | `rawCpuInfo36` | `js55` | `:340` |
| 49 | `rawCpuInfo41` | `js540` | `:335` |

Ids 42, 45, 46, 47, 48, 50, and 51 are on the enum (`RawCpuInfo.java:77`) and are not opened in that method. The login CRC loop skips id 32 only (`deob/BlankWs.java:1128`).

## NPC config

`Lookat_Sub2` is constructed on `js536` (`CameraSettingType.java:456`). Its factory `ProjAnimCutsceneAction.doBasicHitmarkTypeFactory` returns `new Emoji` (`com/jagex/game/cutscene/ProjAnimCutsceneAction.java:20`, constructor at `com/jagex/game/camera/Lookat_Sub2.java:15`). `NPCType` is a loading-stage enum, not this decoder.

`Emoji.doHeadbarTypeFactory` reads opcodes until u8 `0` (`rs2/client/clientscript/emoji/Emoji.java:198`). `doEmoji` (`:210` through `:515`) implements:

| Opcode | Fields |
| --- | --- |
| 1 | u8 count, then that many `doPositionedSound32222222b4b` |
| 2 | jstr |
| 12 | u8 |
| 30..34 | jstr into `stringArray[opcode - 30]` |
| 40 | u8 count of u16be, u16be |
| 41 | u8 count of u16be, u16be |
| 42 | u8 count of i8 |
| 44 | u16be bitfield, expanded into a byte per set bit |
| 45 | u16be bitfield, same expansion |
| 60 | u8 count of `doPositionedSound32222222b4b` |
| 93 | no payload, clears a flag |
| 95 | u16be |
| 97 | u16be |
| 98 | u16be |
| 99 | no payload, sets a flag |
| 100 | i8 |
| 101 | i8 times 5 |
| 102 | u8 bitfield; each set bit reads `doPositionedSound32222222b4b` and `doPositionedSound2222` |
| 103 | u16be |
| 106 and 118 | two u16be (`65535` means `-1`); opcode 118 also reads a third u16be with the same sentinel; then u8 count and `count + 1` u16be values with the same sentinel. The extra 118 value is stored after those entries. |
| 107, 109, 111 | no payload, clear flags |
| 113 | two u16be |
| 114 | two i8 |
| 119 | i8 |
| 121 | u8 count; per entry u8 index and three i8 |
| 123 | u16be |
| 125 | i8 looked up in `ClientScriptHelpers` |
| 127 | u16be |
| 128 | u8 looked up in `DataBlock1396` |
| 134 | four u16be (`65535` means `-1`) and one u8 |
| 135 and 136 | u8, u16be |
| 137 | u16be |
| 138 | `doPositionedSound32222222b4b` |
| 140 | u8 |
| 141, 143 | no payload, set flags |
| 142 | u16be |
| 150..154 | jstr into `stringArray[opcode - 150]`, cleared unless `cls9423298a.tboolean` |
| 155 | four i8 |
| 158 | no payload, stores byte 1 |
| 159 | no payload, stores byte 0 |
| 160 | u8 count of u16be |
| 162 and 178 | no payload |
| 163 | u8 |
| 164 | two u16be |
| 165 | u8 |
| 168 | u8 |
| 169 | no payload, clears a flag |
| 170..175 | u16be (`65535` means `-1`) into a 6-long array at `opcode - 170` |
| 179 | six `doPositionedSound222` values |
| 180 | u8 |
| 181 | u16be, u8 |
| 182 | no payload, sets a flag |
| 249 | u8 count; per entry u8 `1` selects a jstr, otherwise a u32be, and the key is a u24be |

Loc, object, sequence, and var decoders were not opened. That remainder is in [gaps.md](gaps.md).
