# Cache

This revision has no disk cache in the archive. `builds/rs3/910-2019-10-16/meta.json` sets `cache_downloaded` to false and `openrs2_cache_id` to null. The engine string is `js5`. The cache home string is `.jagex_cache_32/runescape`. Nothing was downloaded for this spec.

## Archive ids

`Js5Archive` stores `archiveId` (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/config/Js5Archive.java:8`).

| id | name |
| --- | --- |
| 0 | ANIMS |
| 1 | BASES |
| 2 | CONFIG |
| 3 | INTERFACES |
| 5 | MAPS |
| 7 | MODELS |
| 8 | SPRITES |
| 10 | BINARY |
| 12 | CLIENTSCRIPTS |
| 13 | FONTMETRICS |
| 14 | VORBIS |
| 16 | CONFIG_LOC |
| 17 | CONFIG_ENUM |
| 18 | CONFIG_NPC |
| 19 | CONFIG_OBJ |
| 20 | CONFIG_SEQ |
| 21 | CONFIG_SPOT |
| 22 | CONFIG_STRUCT |
| 23 | WORLDMAP |
| 24 | QUICKCHAT |
| 25 | QUICKCHAT_GLOBAL |
| 26 | MATERIALS |
| 27 | CONFIG_PARTICLE |
| 28 | DEFAULTS |
| 29 | CONFIG_BILLBOARD |
| 30 | DLLS |
| 31 | SHADERS |
| 32 | LOADING_SPRITES |
| 33 | LOADING_SCREENS |
| 34 | LOADING_SPRITES_RAW |
| 35 | CUTSCENES |
| 40 | AUDIOSTREAMS |
| 41 | WORLDMAPAREADATA |
| 42 | WORLDMAPLABELS |
| 47 | MODELSRT7 |
| 48 | ANIMSRT7 |
| 49 | DBTABLEINDEX |
| 52 | TEXTURES_DXT |
| 53 | TEXTURES_PNG |
| 54 | TEXTURES_PNG_MIPPED |
| 55 | TEXTURES_ETC |
| 56 | ANIMS_KEYFRAMES |

`LOADING_SPRITES` is omitted from the login CRC list (`LoginManager.java:1232`). Ids that do not appear above are absent from `values()`.

## Config groups

`Js5ConfigGroup` ids (`Js5ConfigGroup.java:8`). A second constructor argument is `groupSizeInBits`. Zero means the group is the id and the file is 0. `fileId` is the low bits and `groupId` is the id shifted by those bits (`Js5ConfigGroup.java:178`).

| id | name | bits |
| --- | --- | --- |
| 1 | FLUTYPE | 0 |
| 2 | HUNTTYPE | 0 |
| 3 | IDKTYPE | 0 |
| 4 | FLOTYPE | 0 |
| 5 | INVTYPE | 0 |
| 6 | LOCTYPE | 8 |
| 7 | MESANIMTYPE | 0 |
| 8 | ENUMTYPE | 8 |
| 9 | NPCTYPE | 7 |
| 10 | OBJTYPE | 8 |
| 11 | PARAMTYPE | 0 |
| 12 | SEQTYPE | 7 |
| 13 | SPOTTYPE | 8 |
| 14 | js5ConfigGroup | 10 |
| 15..17 | js5ConfigGroup2..4 | 0 |
| 18 | AREATYPE | 0 |
| 19..25 | js5ConfigGroup5..11 | 0 |
| 26 | STRUCTTYPE | 5 |
| 27 | CHATPHRASETYPE | 0 |
| 28 | CHATCATTYPE | 0 |
| 29 | SKYBOXTYPE | 0 |
| 30 | SKYDECORTYPE | 0 |
| 31 | LIGHTTYPE | 0 |
| 32 | BASTYPE | 0 |
| 33 | CURSORTYPE | 0 |
| 34 | MSITYPE | 0 |
| 35 | QUESTTYPE | 0 |
| 36 | MELTYPE | 0 |
| 37..39 | js5ConfigGroup12..14 | 0 |
| 40 | DBTABLETYPE | 0 |
| 41 | DBROWTYPE | 0 |
| 42 | CONTROLLERTYPE | 0 |
| 43..45 | js5ConfigGroup15..17 | 0 |
| 46 | HITMARKTYPE | 0 |
| 47 | VARCLAN | 0 |
| 48 | ITEMCODETYPE | 0 |
| 49 | CATEGORYTYPE | 0 |
| 50, 51, 53, 54 | js5ConfigGroup18..21 | 0 |
| 60 | VAR_PLAYER | 0 |
| 61 | VAR_NPC | 0 |
| 62 | VAR_CLIENT | 0 |
| 63 | VAR_WORLD | 0 |
| 64 | VAR_REGION | 0 |
| 65 | VAR_OBJECT | 0 |
| 66 | VAR_CLAN | 0 |
| 67 | VAR_CLAN_SETTING | 0 |
| 68 | VAR_CONTROLLER | 0 |
| 69 | VAR_BIT | 0 |
| 70 | GAMELOGEVENT | 0 |
| 72 | HEADBARTYPE | 0 |
| 73, 74 | js5ConfigGroup22, js5ConfigGroup23 | 0 |
| 75 | VAR_GLOBAL | 0 |
| 76 | WATERTYPE | 0 |
| 77 | SEQGROUPTYPE | 0 |
| 78, 79 | js5ConfigGroup24, js5ConfigGroup25 | 0 |
| 80 | VAR_PLAYER_GROUP | 0 |
| 81, 82 | js5ConfigGroup26, js5ConfigGroup27 | 0 |
| 83 | WORLDAREATYPE | 0 |
| 84 | js5ConfigGroup28 | 0 |

There is no group id 52 in this list.

## Decode opcodes that were read

`ObjType.decode` (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/config/objtype/ObjType.java:238`) reads `g1` opcodes until 0. Bodies opened through the start of opcode 42 (`ObjType.java:317`):

| opcode | body |
| --- | --- |
| 1 | `gSmart2or4s` model |
| 2 | `gjstr` name |
| 4, 5, 6 | `g2` zoom, xan, yan |
| 7, 8 | `g2`, subtract 65536 when above 32767 |
| 11 | stackable, no extra bytes |
| 12 | `g4s` cost |
| 13, 14 | `g1` wear positions |
| 15, 16 | flags, no extra bytes |
| 23, 24, 25, 26 | `gSmart2or4s` wear models |
| 27 | `g1` wearpos3 |
| 30..34 | `gjstr` ground ops |
| 35..39 | `gjstr` inventory ops |
| 40 | `g1` count, then count times two `g2` recolors |
| 41 | `g1` count, then count times two `g2` retextures |
| 42 | `g1` count, body continues unread |

`GraphicsDefaults.decode` (`GraphicsDefaults.java:101`) was read through opcode 11: opcode 1 is count times two `g2s`, 2 is `gSmart2or4s`, 3 is `g1` and reallocates arrays, 4 and 8 and 10 are flags, 5 and 6 are `g3` login and lobby interface ids, 7 is a 10 by 4 table of `g2` values, 9 and 11 are `g1`. Later opcodes in that file were not read. The interface ids are cache data, not literals.

Other config `decode` methods were not opened. Their field widths are in `gaps.md`.
