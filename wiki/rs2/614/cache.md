# Cache

Kind js5. `AnimationNode` opens `random.dat`, `main_file_cache.dat2` (length 209715200), `main_file_cache.idx255`, and `main_file_cache.idx0` upward (builds/rs2/614-2010-07-13/src/src/main/java/deob/AnimationNode.java:152). `Client.opClient228` wraps dat2 as the master provider (5200), each idx 0..33 as a provider (6000), and idx255 as the reference table (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:2110). `Bindings` rejects a discard-unpacked value outside 0..2 with `js5: Invalid value ... supplied for discardunpacked` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Bindings.java:24).

`IDKTypeList` opens the archives (builds/rs2/614-2010-07-13/src/src/main/java/deob/IDKTypeList.java:216 and builds/rs2/614-2010-07-13/src/src/main/java/deob/IDKTypeList.java:271):

| index | holder |
| --- | --- |
| 0 | `C1b0f82ac.valuekind10989` |
| 1 | `ShaderCompileFailed.valueShaderCompileFailed5` |
| 2 | `KeaInit.valueKeaInit3` |
| 3 | `PrivateChatFilter.valuePrivateChatFilter2` |
| 4 | `Lookat.valueLookat7` |
| 5 | `DaaInit.valueDaaInit2` |
| 6 | `KcaInit.valueKcaInit6` |
| 7 | `FrameSetLoader.valueFrameSetLoader1` |
| 8 | `C10c7aac2.valuekind107911` |
| 9 | `LoadingScreenClear.valueLoadingScreenClear7` |
| 10 | `C0917b442.valuekind11072` |
| 11 | `C8b0fd7d6.valuekind10367` |
| 12 | `C8b0fd7d6.valuekind10364` |
| 13 | `Unused23.valueUnused232` |
| 14 | `MsiType.valueMsiType153` |
| 15 | `SkillDefaults.valueSkillDefaults13` |
| 16 | `Quaternion.valueQuaternion7` |
| 17 | `Font.valueFont13` |
| 18 | `CutsceneRoute.valueCutsceneRoute29` |
| 19 | `VarcStrTypeList.valueVarcStrTypeList5` |
| 20 | `FileUtil.valueFileUtil5` |
| 21 | `OcclusionManager.valueOcclusionManager10` |
| 22 | `DxGraphicsDeletable.valueDxGraphicsDeletable7` |
| 23 | `Unused15.valueUnused152` |
| 24 | `Unused13.valueUnused132` |
| 25 | `StockMarketOffer.valueStockMarketOffer56` |
| 26 | `ObjTypeCustomisation.valueObjTypeCustomisation4` |
| 27 | `PreLoadingScreen.valuePreLoadingScreen12` |
| 28 | `Graphic.valueGraphic9` |
| 29 | `RcaInit.valueRcaInit6` |
| 30 | `VarnBitTypeList.valueVarnBitTypeList4` |
| 31 | `Timer.valueTimer111` |
| 32 | `LoadingScreenAlignmentX.valueLoadingScreenAlignmentX4` |
| 33 | `MaterialAlphaMode.valueMaterialAlphaMode10` |

Index 3 is the interface archive. The client looks up `loginscreen` and `lobbyscreen` with `opBindings10` (builds/rs2/614-2010-07-13/src/src/main/java/deob/IDKTypeList.java:515). Index 5 is the map archive used by both rebuilds. Index 18 is passed to the NPC config loader `Js5ConfigGroup.valueJs5ConfigGroup2` together with index 7 (builds/rs2/614-2010-07-13/src/src/main/java/deob/IDKTypeList.java:444). Index 19 is passed to the item loader `Js5ConfigGroup.valueJs5ConfigGroup1` with index 7.

## NPC config opcodes

`ProgramData.opProgramData4` reads `u8` opcodes until 0 and calls `opProgramData7` (builds/rs2/614-2010-07-13/src/src/main/java/deob/ProgramData.java:502). Opcode 1 is excluded by `~var1 != -2` and has no payload. These opcodes were read in `opProgramData7` (builds/rs2/614-2010-07-13/src/src/main/java/deob/ProgramData.java:547):

| opcode | fields |
| --- | --- |
| 1 | no payload |
| 2 | jstr |
| 12 | no payload |
| 30 | jstr |
| 31 | jstr |
| 32 | jstr |
| 33 | jstr |
| 34 | jstr |
| 40 | no payload |
| 41 | u8 count, then count times (u16be, u16be) |
| 42 | no payload |
| 60 | no payload |
| 93 | no payload |
| 95 | no payload |
| 97 | no payload |
| 98 | no payload |
| 99 | no payload |
| 100 | s8 |
| 101 | s8 |
| 102 | u16be |
| 103 | u16be |
| 106 | no payload |
| 107 | no payload |
| 109 | no payload |
| 111 | no payload |
| 113 | u16be, u16be |
| 114 | s8, s8 |
| 118 | no payload |
| 119 | s8 |
| 121 | u8 count, then per entry u8 and three s8 |
| 122 | u16be |
| 123 | u16be |
| 125 | s8 |
| 127 | no payload |
| 128 | u8 |

The method continues through builds/rs2/614-2010-07-13/src/src/main/java/deob/ProgramData.java:811. Opcodes present in that method and not in the table above are 134, 135, 137, 139, 140, 141, 155, 159, 160, 161, 249. Their payloads were not read.

Other classes contain a `while (true)` loop that reads `opUnused6139` and takes an `Unused6`. Their opcode bodies were not read. See `gaps.md`.
