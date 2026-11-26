# Cache

Cache kind is js5. Archive ids are the first argument of each `Js5Archive` constructor (builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:7). Group ids inside `CONFIG` are `Js5ConfigGroup.valueJs5ConfigGroup28`. The two-argument constructor stores the second int in `valueJs5ConfigGroup33`. `opJs5ConfigGroup6` is `id >>> shift` and `opJs5ConfigGroup7` is `id & ((1<<shift)-1)` (builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:187). That shift is how a group spreads across files. Group 52 is absent. Group 53 is `valueJs5ConfigGroup31`.

Defaults live in archive `CONFIG` group `DefaultsGroup`. `GraphicsDefaults` loads group id 3 (`valueDefaultsGroup22`). `WearposDefaults` loads group id 6 (`valueDefaultsGroup23`). Numeric interface ids are the cache values `GraphicsDefaults` opcode 5 and 6, not Java constants. The cache is not in this tree.

## Js5Archive

| name | id | citation |
| --- | --- | --- |
| ANIMS | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:7 |
| BASES | 1 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:10 |
| CONFIG | 2 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:13 |
| INTERFACES | 3 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:16 |
| SYNTH_SOUNDS | 4 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:19 |
| MAPS | 5 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:22 |
| MIDI_SONGS | 6 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:25 |
| MODELS | 7 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:28 |
| SPRITES | 8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:31 |
| valueJs5Archive11 | 9 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:34 |
| BINARY | 10 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:37 |
| MIDI_JINGLES | 11 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:40 |
| CLIENTSCRIPTS | 12 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:43 |
| FONTMETRICS | 13 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:46 |
| VORBIS | 14 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:49 |
| MIDI_INSTRUMENTS | 15 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:52 |
| CONFIG_LOC | 16 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:55 |
| CONFIG_ENUM | 17 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:58 |
| CONFIG_NPC | 18 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:61 |
| CONFIG_OBJ | 19 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:64 |
| CONFIG_SEQ | 20 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:67 |
| CONFIG_SPOT | 21 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:70 |
| CONFIG_VARBIT | 22 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:73 |
| WORLDMAPDATA | 23 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:76 |
| QUICKCHAT | 24 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:79 |
| QUICKCHAT_GLOBAL | 25 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:82 |
| MATERIALS | 26 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:85 |
| CONFIG_PARTICLE | 27 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:88 |
| valueJs5Archive10 | 28 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:91 |
| CONFIG_BILLBOARD | 29 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:94 |
| DLLS | 30 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:97 |
| SHADERS | 31 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:100 |
| LOADING_SPRITES | 32 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:103 |
| LOADING_SCREEN | 33 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:106 |
| LOADING_SPRITES_RAW | 34 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:109 |
| CUTSCENES | 35 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:112 |
| valueJs5Archive13 | 36 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:115 |
| valueJs5Archive14 | 37 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:118 |

## Js5ConfigGroup

| name | group | shift | citation |
| --- | --- | --- | --- |
| FLUTYPE | 1 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:7 |
| valueJs5ConfigGroup20 | 2 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:10 |
| IDKTYPE | 3 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:13 |
| FLOTYPE | 4 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:16 |
| INVTYPE | 5 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:19 |
| LOCTYPE | 6 | 8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:22 |
| MESANIMTYPE | 7 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:25 |
| valueJs5ConfigGroup19 | 8 | 8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:28 |
| NPCTYPE | 9 | 7 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:31 |
| OBJTYPE | 10 | 8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:34 |
| PARAMTYPE | 11 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:37 |
| SEQTYPE | 12 | 7 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:40 |
| valueJs5ConfigGroup21 | 13 | 8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:43 |
| VARBIT | 14 | 10 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:46 |
| VARCSTR | 15 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:49 |
| VARP | 16 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:52 |
| valueJs5ConfigGroup18 | 17 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:55 |
| valueJs5ConfigGroup17 | 18 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:58 |
| VARCINT | 19 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:61 |
| VAROBJ | 20 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:64 |
| VARPSTR | 21 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:67 |
| VARS | 22 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:70 |
| VARSSTR | 23 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:73 |
| VARN | 24 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:76 |
| VARNBIT | 25 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:79 |
| STRUCTTYPE | 26 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:82 |
| CHATPHRASETYPE | 27 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:85 |
| CHATCATTYPE | 28 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:88 |
| SKYBOXTYPE | 29 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:91 |
| SKYDECORTYPE | 30 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:94 |
| LIGHTTYPE | 31 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:97 |
| BASTYPE | 32 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:100 |
| CURSORTYPE | 33 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:103 |
| MSITYPE | 34 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:106 |
| QUESTTYPE | 35 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:109 |
| MELTYPE | 36 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:112 |
| valueJs5ConfigGroup25 | 37 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:115 |
| valueJs5ConfigGroup26 | 38 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:118 |
| valueJs5ConfigGroup27 | 39 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:121 |
| DBTABLETYPE | 40 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:124 |
| DBROWTYPE | 41 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:127 |
| CONTROLLERTYPE | 42 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:130 |
| valueJs5ConfigGroup24 | 43 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:133 |
| valueJs5ConfigGroup29 | 44 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:136 |
| valueJs5ConfigGroup30 | 45 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:139 |
| HITMARKTYPE | 46 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:142 |
| VARCLAN | 47 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:145 |
| ITEMCODETYPE | 48 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:148 |
| CATEGORYTYPE | 49 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:151 |
| VARREGION | 50 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:154 |
| valueJs5ConfigGroup23 | 51 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:157 |
| valueJs5ConfigGroup31 | 53 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:160 |
| VARCLANSETTING | 54 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:163 |
| GAMELOGEVENT | 70 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:166 |
| HEADBARTYPE | 72 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:169 |
| valueJs5ConfigGroup32 | 73 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:172 |
| valueJs5ConfigGroup22 | 74 | 0 | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5ConfigGroup.java:175 |

## DefaultsGroup

| name | id | citation |
| --- | --- | --- |
| valueDefaultsGroup25 | 1 | builds/rs2/742-2012-11-07/src/client/src/main/java/DefaultsGroup.java:7 |
| valueDefaultsGroup19 | 2 | builds/rs2/742-2012-11-07/src/client/src/main/java/DefaultsGroup.java:10 |
| valueDefaultsGroup22 | 3 | builds/rs2/742-2012-11-07/src/client/src/main/java/DefaultsGroup.java:13 |
| valueDefaultsGroup24 | 4 | builds/rs2/742-2012-11-07/src/client/src/main/java/DefaultsGroup.java:16 |
| valueDefaultsGroup21 | 5 | builds/rs2/742-2012-11-07/src/client/src/main/java/DefaultsGroup.java:19 |
| valueDefaultsGroup23 | 6 | builds/rs2/742-2012-11-07/src/client/src/main/java/DefaultsGroup.java:22 |
| valueDefaultsGroup18 | 7 | builds/rs2/742-2012-11-07/src/client/src/main/java/DefaultsGroup.java:25 |
| valueDefaultsGroup20 | 8 | builds/rs2/742-2012-11-07/src/client/src/main/java/DefaultsGroup.java:28 |

## GraphicsDefaults opcodes

Terminator is opcode 0. Opcode 1 is pairs of `i16be` into the hitmark offset arrays, not sidebar ids. Opcode 3 resizes those arrays (`u8` count) and must be read before opcode 1 if the count is not 4. Opcode 5 is `u24be` `valueGraphicsDefaults28`, the game top-level interface id. Opcode 6 is `u24be` `lobby_interface`. Opcode 7 is ten groups of four sprite tables. Opcodes 2, 4, 8, 9, 10, 11, 12, 13, 14, 15 are in builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:66.

| opcode | fields | citation |
| --- | --- | --- |
| 0 | end | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:71 |
| 1 | (i16be, i16be) * array length | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:85 |
| 2 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:96 |
| 3 | u8 count, then resize | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:98 |
| 4 | flag, no bytes | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:102 |
| 5 | u24be game top interface | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:104 |
| 6 | u24be lobby interface | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:106 |
| 7 | 10*4*(u16be, u16be count, u16be*count) | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:108 |
| 8 | flag, no bytes | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:129 |
| 9 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:131 |
| 10 | flag, no bytes | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:133 |
| 11 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:135 |
| 12 | u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:137 |
| 13 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:140 |
| 14 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:142 |
| 15 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:144 |

## WearposDefaults opcodes

The appearance slot count is opcode 1's `u8` length, then that many `u8` flags. A flag of 1 means the appearance packet skips that slot. Opcode 3 and 4 are one `u8`. Opcode 5 and 6 are `u8` count then that many `u8`. Terminator is 0. builds/rs2/742-2012-11-07/src/client/src/main/java/WearposDefaults.java:30.


## Config type decode opcodes

Each `decode(Packet, int code)` below was read. Fields are the `Packet` calls in that opcode's branch, source order, with `if:` on nested branches. Loops are not unrolled. A later pass still has to honour the count that precedes a repeated read.

### BASType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:155.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | smart2or4null, smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:156 |
| 2 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:159 |
| 3 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:161 |
| 4 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:163 |
| 5 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:165 |
| 6 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:167 |
| 7 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:169 |
| 8 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:171 |
| 9 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:173 |
| 26 | u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:175 |
| 27 | u8, if:i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:178 |
| 28 | u8, if:u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:187 |
| 29 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:196 |
| 30 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:198 |
| 31 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:200 |
| 32 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:202 |
| 33 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:204 |
| 34 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:206 |
| 35 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:208 |
| 36 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:210 |
| 37 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:212 |
| 38 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:214 |
| 39 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:216 |
| 40 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:218 |
| 41 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:220 |
| 42 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:222 |
| 43 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:224 |
| 44 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:226 |
| 45 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:228 |
| 46 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:230 |
| 47 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:232 |
| 48 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:234 |
| 49 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:236 |
| 50 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:238 |
| 51 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:240 |
| 52 | u8, if:smart2or4null, if:u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:242 |
| 53 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:252 |
| 54 | u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:254 |
| 55 | u8, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:257 |
| 56 | u8, if:i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:263 |

### CursorType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/CursorType.java:30.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/CursorType.java:31 |
| 2 | u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/CursorType.java:33 |

### EffectAnimType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:72.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:73 |
| 2 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:75 |
| 4 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:77 |
| 5 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:79 |
| 6 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:81 |
| 7 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:83 |
| 8 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:85 |
| 9 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:87 |
| 10 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:90 |
| 11 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:92 |
| 12 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:94 |
| 13 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:96 |
| 14 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:98 |
| 15 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:101 |
| 16 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:104 |
| 40 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:107 |
| 41 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:115 |
| 44 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:123 |
| 45 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:138 |

### EnumType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/EnumType.java:45.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/EnumType.java:46 |
| 2 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/EnumType.java:48 |
| 3 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/EnumType.java:50 |
| 4 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/EnumType.java:52 |
| 5 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/EnumType.java:60 |
| 7 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/EnumType.java:73 |

### FloorOverlayType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:66.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u24be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:67 |
| 2 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:69 |
| 3 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:71 |
| 5 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:76 |
| 7 | u24be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:78 |
| 8 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:80 |
| 9 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:82 |
| 10 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:84 |
| 11 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:86 |
| 12 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:88 |
| 13 | u24be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:90 |
| 14 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:92 |
| 16 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:94 |
| 20 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:96 |
| 21 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:98 |
| 22 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorOverlayType.java:100 |

### FloorUnderlayType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/FloorUnderlayType.java:45.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u24be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorUnderlayType.java:46 |
| 2 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorUnderlayType.java:49 |
| 3 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorUnderlayType.java:54 |
| 4 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorUnderlayType.java:56 |
| 5 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/FloorUnderlayType.java:58 |

### HitmarkType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:63.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:64 |
| 2 | u24be | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:66 |
| 3 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:69 |
| 4 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:71 |
| 5 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:73 |
| 6 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:75 |
| 7 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:77 |
| 8 | jstr2 | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:79 |
| 9 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:81 |
| 10 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:83 |
| 11 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:85 |
| 12 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:87 |
| 13 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:89 |
| 14 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/HitmarkType.java:91 |

### IDKType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:51.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:52 |
| 2 | u8, if:smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:54 |
| 3 | if:u8, if:u16be, if:u16be, if:u8, if:u16be, if:u16be, if:u16be, if:u16be, if:smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:60 |
| 40 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:61 |
| 41 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:69 |
| 44 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:77 |
| 45 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:92 |
| 60-69 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:107 |

### InvType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/InvType.java:30.

| opcode | fields | citation |
| --- | --- | --- |
| 2 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/InvType.java:31 |
| 4 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/InvType.java:33 |

### LightType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/LightType.java:30.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LightType.java:31 |
| 2 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LightType.java:33 |
| 3 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LightType.java:35 |
| 4 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LightType.java:37 |

### LocType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:254.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u8, if:i8, if:u8, if:smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:255 |
| 2 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:267 |
| 14 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:269 |
| 15 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:271 |
| 17 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:273 |
| 18 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:276 |
| 19 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:278 |
| 21 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:280 |
| 22 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:282 |
| 23 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:284 |
| 24 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:286 |
| 27 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:291 |
| 28 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:293 |
| 29 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:295 |
| 39 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:297 |
| 30-34 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:299 |
| 40 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:301 |
| 41 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:309 |
| 42 | u8, if:i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:317 |
| 44 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:323 |
| 45 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:338 |
| 62 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:353 |
| 64 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:355 |
| 65 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:357 |
| 66 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:359 |
| 67 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:361 |
| 69 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:363 |
| 70 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:365 |
| 71 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:367 |
| 72 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:369 |
| 73 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:371 |
| 74 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:373 |
| 75 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:375 |
| 92 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:387 |
| 78 | u16be, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:396 |
| 79 | u16be, u16be, u8, u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:399 |
| 81 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:408 |
| 82 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:411 |
| 88 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:413 |
| 89 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:415 |
| 91 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:417 |
| 93 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:419 |
| 94 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:422 |
| 95 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:424 |
| 97 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:427 |
| 98 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:429 |
| 101 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:434 |
| 102 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:436 |
| 103 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:438 |
| 104 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:440 |
| 105 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:442 |
| 106 | u8, if:smart2or4null, if:u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:444 |
| 107 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:456 |
| 150-154 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:458 |
| 160 | u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:463 |
| 162 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:469 |
| 163 | i8, i8, i8, i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:472 |
| 164 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:477 |
| 165 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:479 |
| 166 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:481 |
| 167 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:483 |
| 168 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:485 |
| 169 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:487 |
| 170 | smart1or2 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:489 |
| 171 | smart1or2 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:491 |
| 173 | u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:493 |
| 177 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:496 |
| 178 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:498 |
| 189 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:500 |
| 190-195 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:502 |
| 249 | u8, if:u8, if:u24be, if:jstr, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:508 |

### MSIType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/MSIType.java:30.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/MSIType.java:31 |
| 2 | u24be | builds/rs2/742-2012-11-07/src/client/src/main/java/MSIType.java:33 |
| 3 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/MSIType.java:35 |
| 4 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/MSIType.java:37 |

### MapElementType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:138.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:139 |
| 2 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:141 |
| 3 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:143 |
| 4 | u24be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:145 |
| 5 | u24be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:147 |
| 6 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:149 |
| 7 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:151 |
| 8 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:159 |
| 9 | u16be, u16be, u32be, u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:161 |
| 15 | u8, if:i16be, u32be, u8, if:u32be, if:i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:174 |
| 16 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:190 |
| 17 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:192 |
| 18 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:194 |
| 19 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:196 |
| 20 | u16be, u16be, u32be, u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:198 |
| 21 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:209 |
| 22 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:211 |
| 23 | u8, u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:213 |
| 24 | i16be, i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:217 |
| 249 | u8, if:u8, if:u24be, if:jstr, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:220 |

### NPCType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:215.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u8, if:smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:216 |
| 2 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:222 |
| 12 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:224 |
| 30-34 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:226 |
| 40 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:228 |
| 41 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:236 |
| 42 | u8, if:i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:244 |
| 44 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:250 |
| 45 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:265 |
| 60 | u8, if:smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:280 |
| 93 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:286 |
| 95 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:288 |
| 97 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:290 |
| 98 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:292 |
| 99 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:294 |
| 100 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:296 |
| 101 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:298 |
| 102 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:300 |
| 103 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:302 |
| 118 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:314 |
| 107 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:329 |
| 109 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:331 |
| 111 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:333 |
| 113 | u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:335 |
| 114 | i8, i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:338 |
| 119 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:341 |
| 121 | u8, if:u8, if:i8, if:i8, if:i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:343 |
| 123 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:353 |
| 125 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:355 |
| 127 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:357 |
| 128 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:359 |
| 134 | u16be, u16be, u16be, u16be, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:361 |
| 137 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:382 |
| 138 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:384 |
| 140 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:386 |
| 141 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:388 |
| 142 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:390 |
| 143 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:392 |
| 150-154 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:394 |
| 155 | i8, i8, i8, i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:399 |
| 158 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:404 |
| 159 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:406 |
| 160 | u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:408 |
| 162 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:414 |
| 163 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:416 |
| 164 | u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:418 |
| 165 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:421 |
| 168 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:423 |
| 169 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:425 |
| 170-175 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:427 |
| 249 | u8, if:u8, if:u24be, if:jstr, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:437 |

### ObjType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:222.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:223 |
| 2 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:225 |
| 4 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:227 |
| 5 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:229 |
| 6 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:231 |
| 7 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:233 |
| 8 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:238 |
| 11 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:243 |
| 12 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:245 |
| 13 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:247 |
| 14 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:249 |
| 16 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:251 |
| 18 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:253 |
| 23 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:255 |
| 24 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:257 |
| 25 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:259 |
| 26 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:261 |
| 27 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:263 |
| 30-34 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:265 |
| 35-39 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:267 |
| 40 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:269 |
| 41 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:277 |
| 42 | u8, if:i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:285 |
| 43 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:291 |
| 44 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:294 |
| 45 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:309 |
| 65 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:324 |
| 78 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:326 |
| 79 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:328 |
| 90 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:330 |
| 91 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:332 |
| 92 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:334 |
| 93 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:336 |
| 95 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:338 |
| 96 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:340 |
| 97 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:342 |
| 98 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:344 |
| 100-109 | u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:346 |
| 110 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:353 |
| 111 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:355 |
| 112 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:357 |
| 113 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:359 |
| 114 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:361 |
| 115 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:363 |
| 121 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:365 |
| 122 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:367 |
| 125 | i8, i8, i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:369 |
| 126 | i8, i8, i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:373 |
| 132 | u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:380 |
| 134 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:386 |
| 139 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:388 |
| 140 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:390 |
| 142-146 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:392 |
| 150-154 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:398 |
| 249 | u8, if:u8, if:u24be, if:jstr, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:404 |
| 242 | smart2or4, smart2or4 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:421 |
| 243 | smart2or4 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:424 |
| 244 | smart2or4 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:426 |
| 245 | smart2or4 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:428 |
| 246 | smart2or4 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:430 |
| 247 | smart2or4 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:432 |
| 248 | smart2or4 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:434 |
| 250 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:436 |
| 251 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:438 |
| 252 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:444 |
| 253 | u16be, u16be, u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:450 |

### ParamType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/ParamType.java:30.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParamType.java:31 |
| 2 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParamType.java:33 |
| 4 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParamType.java:35 |
| 5 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/ParamType.java:37 |

### ParticleEffectorType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEffectorType.java:98.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEffectorType.java:99 |
| 2 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEffectorType.java:101 |
| 3 | u32be, u32be, u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEffectorType.java:103 |
| 4 | u8, u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEffectorType.java:107 |
| 6 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEffectorType.java:110 |
| 8 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEffectorType.java:112 |
| 9 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEffectorType.java:114 |
| 10 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEffectorType.java:116 |

### ParticleEmitterType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:240.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u16be, u16be, u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:241 |
| 2 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:251 |
| 3 | u32be, u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:253 |
| 4 | u8, i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:256 |
| 5 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:259 |
| 6 | u32be, u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:261 |
| 7 | u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:264 |
| 8 | u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:267 |
| 9 | u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:270 |
| 10 | u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:276 |
| 12 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:282 |
| 13 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:284 |
| 14 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:286 |
| 15 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:288 |
| 16 | u8, u16be, u16be, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:290 |
| 17 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:295 |
| 18 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:297 |
| 19 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:299 |
| 20 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:301 |
| 21 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:303 |
| 22 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:305 |
| 23 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:307 |
| 24 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:309 |
| 25 | u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:311 |
| 26 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:317 |
| 27 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:319 |
| 28 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:321 |
| 29 | i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:323 |
| 30 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:325 |
| 31 | u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:327 |
| 32 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:330 |
| 33 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:332 |
| 34 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:334 |

### QuestType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:87.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | jstr2 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:88 |
| 2 | jstr2 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:90 |
| 3 | u8, if:u16be, if:u32be, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:92 |
| 4 | u8, if:u16be, if:u32be, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:100 |
| 5 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:108 |
| 6 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:110 |
| 7 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:112 |
| 8 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:114 |
| 9 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:116 |
| 10 | u8, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:118 |
| 12 | u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:124 |
| 13 | u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:126 |
| 14 | u8, if:u8, if:u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:132 |
| 15 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:139 |
| 17 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:141 |
| 18 | u8, if:u32be, if:u32be, if:u32be, if:jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:143 |
| 19 | u8, if:u32be, if:u32be, if:u32be, if:jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:155 |
| 249 | u8, if:u8, if:u24be, if:jstr, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:167 |

### QuickChatCatType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatCatType.java:33.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatCatType.java:34 |
| 2 | u8, if:u16be, if:i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatCatType.java:36 |
| 3 | u8, if:u16be, if:i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatCatType.java:45 |
| 4 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatCatType.java:54 |

### QuickChatPhraseType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatPhraseType.java:36.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | jstr | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatPhraseType.java:37 |
| 2 | u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatPhraseType.java:39 |
| 3 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatPhraseType.java:45 |
| 4 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatPhraseType.java:60 |

### SeqType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:90.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u16be, if:u16be, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:91 |
| 2 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:104 |
| 3 | u8, if:u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:106 |
| 5 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:112 |
| 6 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:114 |
| 7 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:116 |
| 8 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:118 |
| 9 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:120 |
| 10 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:122 |
| 11 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:124 |
| 12 | u8, if:u16be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:126 |
| 13 | u16be, if:u8, if:u24be, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:135 |
| 14 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:148 |
| 15 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:150 |
| 16 | if:u8, if:u8, if:u8, if:u16be, if:u16be, if:u8, if:u8, if:u8, if:u24be, if:jstr, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:152 |
| 18 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:153 |
| 19 | u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:155 |
| 20 | u8, u16be, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:163 |
| 22 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:175 |
| 249 | u8, if:u8, if:u24be, if:jstr, if:u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:177 |

### SkyBoxType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/SkyBoxType.java:33.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyBoxType.java:34 |
| 2 | u8, if:u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyBoxType.java:36 |
| 3 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyBoxType.java:41 |
| 4 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyBoxType.java:43 |
| 5 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyBoxType.java:45 |

### SkyDecorType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/SkyDecorType.java:51.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyDecorType.java:52 |
| 2 |  | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyDecorType.java:54 |
| 3 | i16be, i16be, i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyDecorType.java:56 |
| 4 | u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyDecorType.java:60 |
| 5 | smart2or4null | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyDecorType.java:62 |
| 6 | u24be | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyDecorType.java:64 |
| 7 | i16be, i16be, i16be | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyDecorType.java:66 |

### VarClanSettingType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/VarClanSettingType.java:30.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/VarClanSettingType.java:31 |
| 2 | u16be, u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/VarClanSettingType.java:33 |

### VarClanType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/VarClanType.java:30.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/VarClanType.java:31 |
| 3 | u16be, u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/VarClanType.java:33 |

### VarPlayerType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/VarPlayerType.java:24.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/VarPlayerType.java:25 |
| 5 | u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/VarPlayerType.java:27 |

### VarbitType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/VarbitType.java:27.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u16be, u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/VarbitType.java:28 |

### VarnBitType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/VarnBitType.java:27.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | u16be, u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/VarnBitType.java:28 |

### VarnType

Dispatch builds/rs2/742-2012-11-07/src/client/src/main/java/VarnType.java:21.

| opcode | fields | citation |
| --- | --- | --- |
| 1 | i8 | builds/rs2/742-2012-11-07/src/client/src/main/java/VarnType.java:22 |

### StructType

`opStructType19` reads `u8` opcodes until 0 (builds/rs2/742-2012-11-07/src/client/src/main/java/StructType.java:12). `opStructType20` ignores every opcode other than 249 (builds/rs2/742-2012-11-07/src/client/src/main/java/StructType.java:22).

| opcode | fields | citation |
| --- | --- | --- |
| 249 | u8 count, then per entry u8 flag, u24be key, and jstr when the flag is 1, otherwise u32be | builds/rs2/742-2012-11-07/src/client/src/main/java/StructType.java:25 |

## Type classes with no extracted opcode switch

- `BillboardType` builds/rs2/742-2012-11-07/src/client/src/main/java/BillboardType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `BindingType` builds/rs2/742-2012-11-07/src/client/src/main/java/BindingType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `CameraEffectType` builds/rs2/742-2012-11-07/src/client/src/main/java/CameraEffectType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `CameraTrackableType` builds/rs2/742-2012-11-07/src/client/src/main/java/CameraTrackableType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `ClientTriggerType` builds/rs2/742-2012-11-07/src/client/src/main/java/ClientTriggerType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `DataType` builds/rs2/742-2012-11-07/src/client/src/main/java/DataType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `LoadableResourceType` builds/rs2/742-2012-11-07/src/client/src/main/java/LoadableResourceType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `LoadingScreenElementType` builds/rs2/742-2012-11-07/src/client/src/main/java/LoadingScreenElementType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `LoadingScreenType` builds/rs2/742-2012-11-07/src/client/src/main/java/LoadingScreenType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `PrimitiveType` builds/rs2/742-2012-11-07/src/client/src/main/java/PrimitiveType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `RebuildType` builds/rs2/742-2012-11-07/src/client/src/main/java/RebuildType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `SpotAnimType` builds/rs2/742-2012-11-07/src/client/src/main/java/SpotAnimType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `TextureTramsformType` builds/rs2/742-2012-11-07/src/client/src/main/java/TextureTramsformType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `ToolkitType` builds/rs2/742-2012-11-07/src/client/src/main/java/ToolkitType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `TriggerType` builds/rs2/742-2012-11-07/src/client/src/main/java/TriggerType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `UniformType` builds/rs2/742-2012-11-07/src/client/src/main/java/UniformType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.
- `VideoType` builds/rs2/742-2012-11-07/src/client/src/main/java/VideoType.java:1 has no `decode(Packet, int)` opcode switch in the form this pass extracted.

