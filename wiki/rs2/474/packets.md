# Packets

S2C size is `TextureOpCurve.valueTextureOpCurve65[opcode]` (`builds/rs2/474-2007-10-19/src/src/main/java/deob/TextureOpCurve.java:107`). C2S has no size table, so those rows use `none`. Width names are in [framing.md](framing.md). The handler column is the method that reads or writes the payload. A row with an empty field list was not fully read. Those rows are in [gaps.md](gaps.md).

Zone opcodes 7, 18, 28, 38, 49, 55, 86, 188, 191, 205, and 220 are decoded by `ShapedTile.opShapedTile20` (`rt4/ShapedTile.java:118`). Opcode 163 repeats that decoder after a zone base. Player opcode 150 and NPC opcode 146 are specified in [sync.md](sync.md). Rebuild opcodes 61 and 175 are specified in [region.md](region.md).

## Server to client

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 4 | 20 | u8 index, u8 flag. When the flag is 0 the rest is ignored. Otherwise the flag byte is reread as i8, then u16be, u32be, u32be, u32be, u32be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1260` |
| s2c | 5 | 1 | u8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:520` |
| s2c | 7 | 5 | u8 coord, u16be id, u16le_lo_m128 amount | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:331` |
| s2c | 10 | 6 | u32le, u16be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:500` |
| s2c | 15 | 3 | u8, u8, u8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:609` |
| s2c | 17 | 7 | u8n128, u16le, u32me | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:377` |
| s2c | 18 | 14 | u8 shape and rotation, i8_128sub, i8, u8m128 coord, u16be_lo_m128, u16be loc id, u16be_lo_m128 player index, u16le, i8m128, i8neg | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:349` |
| s2c | 22 | 4 | u32be component | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:367` |
| s2c | 28 | 3 | u16be_lo_m128 id, u8m128 coord | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:141` |
| s2c | 30 | 1 | u8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:250` |
| s2c | 32 | 11 | u64be, u16be, i8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1274` |
| s2c | 33 | 6 | u16le_lo_m128, u32me | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1167` |
| s2c | 35 | -1 | u64be, i8, u64be, u16be, u24be, u8, then `PleaseEnterUsername.opkind2424` | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:399` |
| s2c | 36 | 10 | u16be, u16le, u32be, u16le_lo_m128 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1230` |
| s2c | 38 | 15 | u8 coord, i8, i8, i16be, u16be, u8, u8, u16be, u16be, u8, u8 | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:227` |
| s2c | 42 | 1 | u8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:391` |
| s2c | 44 | 6 | u8, u8, u8, u8, u16be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:834` |
| s2c | 48 | 6 | u8 type. Type 1 then reads u16be. Types 2 through 6 then read u16be, u16be, u8. Type 10 then reads u16be. The socket still delivers 6 bytes | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:732` |
| s2c | 49 | 6 | u8 coord, u16be id, u8 height, u16be delay | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:251` |
| s2c | 54 | 24 | bytes(24) copied onto the uid file when it is open | `SoundBank.opSoundBank10` | `rt4/SoundBank.java:59` |
| s2c | 55 | 4 | u8m128 shape and rotation, u8 coord, u16be animation | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:164` |
| s2c | 56 | -2 | u64be. Zero clears the clan list and stops. Otherwise another u64be, i8, u8 count. Count 255 stops. Otherwise that many records of u64be, u16be, i8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:909` |
| s2c | 57 | -1 | u64be, then `PleaseEnterUsername.opkind2424` | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1472` |
| s2c | 61 | -2 | static rebuild in region.md | `ByteArrayNodeSecondary.opByteArrayNodeSecondary24` | `rt4/JavaMouseWheel.java:1054` |
| s2c | 66 | 2 | i16be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1015` |
| s2c | 71 | 5 | u24me, u16le. 65535 becomes -1 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:576` |
| s2c | 76 | 8 | i16le, u32le, i16be_lo_m128 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1455` |
| s2c | 82 | 0 | | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:661` |
| s2c | 84 | 0 | | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:784` |
| s2c | 86 | 2 | u8neg shape and rotation, u8neg coord | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:318` |
| s2c | 100 | 0 | | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1000` |
| s2c | 107 | 6 | u32me2, i16le | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:674` |
| s2c | 114 | 6 | u16be, u32me | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:623` |
| s2c | 117 | -1 | u64be, u16be, u24be, u8, then `PleaseEnterUsername.opkind2424` | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:689` |
| s2c | 119 | -2 | u32be component, u16be container. A negative component selects no widget. A component below -70000 adds 32768 to the container. Then u16be count, and that many records of u8neg amount (255 is replaced by u32le) and u16le_lo_m128 item. The record index is the slot | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1181` |
| s2c | 121 | 2 | u16le_lo_m128. 65535 becomes -1 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:565` |
| s2c | 127 | 6 | u16le_lo_m128, u32be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:647` |
| s2c | 128 | -2 | u16be root, u16be count, then that many u32be, u16be, u8. Then while bytes remain: u32be, u16be start, u16be end, u32be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:444` |
| s2c | 133 | 12 | u32le, u16le_lo_m128. 65535 becomes -1. u32be, u16be_lo_m128. 65535 becomes -1 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1375` |
| s2c | 138 | 0 | | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1023` |
| s2c | 146 | -2 | NPC update in sync.md | `SlStars.opkind2503` | `deob/SlStars.java:35` |
| s2c | 150 | -2 | player update in sync.md | `LocType.opLocType9` | `rt4/LocType.java:23` |
| s2c | 151 | 6 | | none. `call72b59b7f` throws `T1` | `deob/TextureOpCurve.java:107` |
| s2c | 153 | 4 | u32me | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1127` |
| s2c | 156 | 0 | | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:239` |
| s2c | 158 | 2 | u16be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1046` |
| s2c | 163 | -2 | u8 zone x, u8n128 zone z, then repeated u8 opcode plus one zone body until the packet ends | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:264` |
| s2c | 166 | 0 | | `ConnectingServer.opkind2372` | `rt4/JavaMouseWheel.java:543` |
| s2c | 167 | 6 | u8, u8, u16be, u8, u8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:970` |
| s2c | 168 | 2 | u8 zone x, u8m128 zone z | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:602` |
| s2c | 169 | 2 | u8n128 zone z, u8m128 zone x, then clears that 8 by 8 area | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1419` |
| s2c | 173 | -2 | `(size / 8)` times u64be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:849` |
| s2c | 174 | 4 | u32be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:549` |
| s2c | 175 | -2 | dynamic rebuild in region.md | `ByteArrayNodeSecondary.opByteArrayNodeSecondary24` | `rt4/JavaMouseWheel.java:778` |
| s2c | 178 | 8 | u16le_lo_m128, u16le_lo_m128, u32le | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1365` |
| s2c | 179 | 6 | u8, u8, u16be, u8, u8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:526` |
| s2c | 184 | 5 | u8m128, u32be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1247` |
| s2c | 187 | -2 | u32be component, u16be container. A component below 0 selects no widget. A component below -70000 adds 32768 to the container. Then while bytes remain: unsigned smart slot, u16be id, and when the id is not 0 a u8 amount or, when that u8 is 255, a u32be amount | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:790` |
| s2c | 188 | 4 | u8 shape and rotation, u16be_lo_m128 id, u8n128 coord | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:265` |
| s2c | 191 | 7 | u8 coord, u16be id, u16be, u16be_lo_m128 | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:278` |
| s2c | 196 | 6 | u8, u32me2, u8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1141` |
| s2c | 200 | 11 | u64be, u16be, u8 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1060` |
| s2c | 205 | 5 | u8 coord, u16be sound, u8 radius and loops, u8 delay | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:119` |
| s2c | 207 | 2 | u16le | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:640` |
| s2c | 209 | -1 | jstr, then branched on prefixes inside that string | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:277` |
| s2c | 220 | 7 | u8 coord, u16be id, u16be, u16be | `ShapedTile.opShapedTile20` | `rt4/ShapedTile.java:297` |
| s2c | 225 | -1 | u8n128, jstr, u8neg | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1402` |
| s2c | 230 | 3 | u8neg, u8n128, u8n128 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:899` |
| s2c | 231 | -2 | u32me2, jstr | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1442` |
| s2c | 236 | 4 | u32be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:257` |
| s2c | 237 | -2 | jstr descriptor, then for each character from the end a jstr when the character is `s` and otherwise a u32be, then one u32be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:219` |
| s2c | 238 | -2 | | `SieBefindenSichEinemMitgliederGebiet.opkind2261` | `rt4/JavaMouseWheel.java:213` |
| s2c | 241 | 0 | | `EnumStringEntry.opEnumStringEntry8` | `rt4/JavaMouseWheel.java:204` |
| s2c | 243 | 5 | u16be, u8, u16be | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1356` |
| s2c | 249 | 10 | u32me, u32be, u16be. 65535 becomes -1 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:1480` |
| s2c | 251 | 2 | u16le | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:885` |
| s2c | 253 | 3 | u16be, i8m128 | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:588` |
| s2c | 255 | 6 | u16le_lo_m128, u32le | `JavaMouseWheel.call72b59b7f` | `rt4/JavaMouseWheel.java:862` |

`PleaseEnterUsername.opkind2424` (`deob/PleaseEnterUsername.java:68`) is the unread tail of opcodes 35, 57, and 117, and of player mask `0x80`. `EnumStringEntry.opEnumStringEntry8` and `SieBefindenSichEinemMitgliederGebiet.opkind2261` were not read.

## Client to server

Login opcodes 14, 16, and 18 are plaintext on the game socket. Their bytes are in [login.md](login.md). The rows below are the ISAAC game packets unless the citation says otherwise.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 1 | none | u64be | `TextureOp29SubOp4.opTextureOp29SubOp410` | `rt4/TextureOp29SubOp4.java:55` |
| c2s | 3 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:86` |
| c2s | 4 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:422` |
| c2s | 8 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:725` |
| c2s | 11 | none | walk in input.md, including the 14 extra bytes | `QuickChatPhrase.opQuickChatPhrase7` | `rt4/QuickChatPhrase.java:266` |
| c2s | 13 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:762` |
| c2s | 21 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:807` |
| c2s | 22 | none | | `BrokenOutputStream` | `rt4/BrokenOutputStream.java:37` |
| c2s | 27 | none | | `MaterialManager_Class106` | `rt4/MaterialManager_Class106.java:59` |
| c2s | 29 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:98` |
| c2s | 31 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:616` |
| c2s | 34 | none | u8 1 or 0 | `InvType.opInvType12` | `rt4/InvType.java:205` |
| c2s | 35 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:348` |
| c2s | 37 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:80` |
| c2s | 41 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:474` |
| c2s | 46 | none | walk in input.md | `QuickChatPhrase.opQuickChatPhrase7` | `rt4/QuickChatPhrase.java:262` |
| c2s | 47 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:215` |
| c2s | 52 | none | | `TextureOpCurve.opTextureOpCurve18` | `deob/TextureOpCurve.java:1760` |
| c2s | 55 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:235` |
| c2s | 56 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:657` |
| c2s | 59 | none | walk in input.md | `QuickChatPhrase.opQuickChatPhrase7` | `rt4/QuickChatPhrase.java:272` |
| c2s | 64 | none | | `IntNode` | `rt4/IntNode.java:75` |
| c2s | 65 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:116` |
| c2s | 70 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:92` |
| c2s | 71 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:356` |
| c2s | 78 | none | | `SoftwareFont` | `rt4/SoftwareFont.java:90` |
| c2s | 82 | none | | `TextureOpCurve.opTextureOpCurve18` | `deob/TextureOpCurve.java:812` |
| c2s | 84 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:280` |
| c2s | 87 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:74` |
| c2s | 88 | none | | `TextureOpCurve.opTextureOpCurve18` | `deob/TextureOpCurve.java:820` |
| c2s | 89 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:286` |
| c2s | 96 | none | | `GlBoundingBox` and `HierWechseln.opkind2465` | `rt4/GlBoundingBox.java:44` |
| c2s | 102 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:623` |
| c2s | 105 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:318` |
| c2s | 107 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:569` |
| c2s | 116 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:455` |
| c2s | 121 | none | mouse samples in input.md | `InvType.opInvType12` | `rt4/InvType.java:86` |
| c2s | 122 | none | | `C82c4f4c4` | `deob/C82c4f4c4.java:57` |
| c2s | 124 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:484` |
| c2s | 125 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:385` |
| c2s | 131 | none | | `GlTile` | `rt4/GlTile.java:107` |
| c2s | 134 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:225` |
| c2s | 141 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:201` |
| c2s | 145 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:514` |
| c2s | 149 | none | u8 length backpatch, u32be id, per-check status and payload, CRC | `DirectByteArray` | `rt4/DirectByteArray.java:72` |
| c2s | 150 | none | | `TextureOp35` | `rt4/TextureOp35.java:244` |
| c2s | 151 | none | | `TextureOpVerticalGradient.opTextureOpVerticalGradient4` | `rt4/TextureOpVerticalGradient.java:29` |
| c2s | 152 | none | u64be | `Mouse` | `rt4/Mouse.java:44` |
| c2s | 154 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:110` |
| c2s | 156 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:122` |
| c2s | 157 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:252` |
| c2s | 160 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:195` |
| c2s | 163 | none | | `TextureOp12` and `MonochromeImageCacheEntry` | `rt4/TextureOp12.java:450` |
| c2s | 166 | none | u16le low+128, u8 `(128 - flag)`, u32me2, u16le | `InvType.opInvType12` | `rt4/InvType.java:288` |
| c2s | 167 | none | u32le packed click | `InvType.opInvType12` | `rt4/InvType.java:183` |
| c2s | 169 | none | u16le, u16le | `InvType.opInvType12` | `rt4/InvType.java:198` |
| c2s | 171 | none | | `TextureOpCurve.opTextureOpCurve18` | `deob/TextureOpCurve.java:857` |
| c2s | 174 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:843` |
| c2s | 175 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:822` |
| c2s | 176 | none | | `TextureOp35` | `rt4/TextureOp35.java:267` |
| c2s | 178 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:441` |
| c2s | 185 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:468` |
| c2s | 186 | none | | `InvType.opInvType12` | `rt4/InvType.java:469` |
| c2s | 190 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:859` |
| c2s | 192 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:104` |
| c2s | 196 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:609` |
| c2s | 198 | none | | `TextureOpCurve.opTextureOpCurve18` | `deob/TextureOpCurve.java:1570` |
| c2s | 202 | none | | `SceneGraph_Class120` | `rt4/SceneGraph_Class120.java:224` |
| c2s | 203 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:634` |
| c2s | 204 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:791` |
| c2s | 205 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:641` |
| c2s | 206 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:395` |
| c2s | 208 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:503` |
| c2s | 211 | none | | `GlBoundingBox` and `HierWechseln.opkind2465` | `rt4/GlBoundingBox.java:36` |
| c2s | 214 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:303` |
| c2s | 215 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:554` |
| c2s | 226 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:736` |
| c2s | 230 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:334` |
| c2s | 234 | none | u32be, u16be | `ChangeLocRequest` | `rt4/ChangeLocRequest.java:68` |
| c2s | 236 | none | | `InvType.opInvType12` | `rt4/InvType.java:394` |
| c2s | 238 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:663` |
| c2s | 241 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:592` |
| c2s | 242 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:688` |
| c2s | 244 | none | | `GlBoundingBox` and `HierWechseln.opkind2465` | `rt4/GlBoundingBox.java:40` |
| c2s | 247 | none | | `GlBoundingBox` and `HierWechseln.opkind2465` | `rt4/GlBoundingBox.java:32` |
| c2s | 249 | none | | `TextureOpCurve.opTextureOpCurve18` | `deob/TextureOpCurve.java:804` |
| c2s | 250 | none | u8neg, u32me, u32me | `ServerBeingUpdated.opkind14025` | `deob/ServerBeingUpdated.java:136` |
| c2s | 251 | none | | `TextureOpCurve.opTextureOpCurve18` | `deob/TextureOpCurve.java:1557` |
| c2s | 252 | none | | `TextureOpCurve.opTextureOpCurve18` | `deob/TextureOpCurve.java:1775` |
| c2s | 254 | none | | `HierWechseln.opkind2465` | `deob/HierWechseln.java:705` |

Opcode 186 is also written at `rt4/SoftwareFont.java:128`. Opcode 163 is also written at `rt4/MonochromeImageCacheEntry.java:32`. The update socket writes plaintext u8 `15` and u8 `4` and is not part of this ISAAC table (`rt4/Client.java:420`, `deob/BitteVersuchenSieEs.java:65`).
