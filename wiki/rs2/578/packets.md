# Packets

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`.

Server-to-client `size` is `CanAddYourselfOwnFriendList.valuekind9353`, indexed by opcode in `Cdba04b48.opkind7258`. `-1` is an extra u8 length. `-2` is an extra u16be length. See [framing.md](framing.md).

Client-to-server `size` is the second argument of `new NoiseGeneratorRelated(opcode, size)`. That argument is discarded (`deob/NoiseGeneratorRelated.java:57`). It is not a table the client consults. Rows still show the literal. Empty `fields` means that write or read body was not read; the row is in [gaps.md](gaps.md).

A server field list is the direct `C7502a728.valuekind8306` reads in that branch, in source order. A loop lists the call once per textual occurrence. Codec names are in [framing.md](framing.md).

## Server to client

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 0 | 10 | u16be_m128, u16be_m128, u32be, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOpCurve.java:18; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:184 |
| s2c | 1 | 6 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C312500b3.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:637 |
| s2c | 2 | 2 | u8_m128, u8_128m | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOpCurve.java:17; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1193 |
| s2c | 3 | 3 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd09638cd.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1536 |
| s2c | 4 | 1 | u8 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C8f1543b2.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1222 |
| s2c | 5 | 8 | u16be, u8, u8, u16be, u8, u8 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C4aacb79a.java:11; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:613 |
| s2c | 6 | -2 | u8, jstr, jstr, jstr, jstr | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/BZip2State.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:990 |
| s2c | 7 | 2 | i16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C3c9f52c8.java:21; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:537 |
| s2c | 8 | 2 | u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd0aca380.java:19; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:666 |
| s2c | 9 | 8 | u32be, u16le, i16le_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C1659b20b.java:39; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1775 |
| s2c | 10 | -1 | u16be, u16be, u16be, u16be, u24be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SkillDefaults.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:815 |
| s2c | 11 | -1 | jstr | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/TitelbildGeFfnet.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1590 |
| s2c | 12 | 3 | u16be_m128, u8neg | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/FriendListFirst.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:341 |
| s2c | 13 | 5 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/UnableSendMessagePlayerNotFriendlist.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1629 |
| s2c | 14 | 6 | u16be, u16be_m128, u16be_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6e44077.java:10; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1033 |
| s2c | 15 | 10 | u16le_m128, u32_mix2, u16be, u16le_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SuaListaDeAmigosEstCheiaLimiteParaOsUsuRio.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:701 |
| s2c | 16 | 7 | u8_m128, u32_mix, u16le_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C408bee69.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1633 |
| s2c | 17 | 2 | u8neg, u8_128m | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C98dff673.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:626 |
| s2c | 18 | -2 | u8 count, u32be, then one record per count. Record starts with u8 kind. Kind 0 or 2: two jstr. Kind 1: two jstr, u32be. Kind 3: two jstr, u8 n, n jstr, then n times u32be length and bytes(length). Kind 4: two jstr, u8 n, n jstr. Other kinds are caught and stored as an error code. deob/C00d852fc.java:20 | C00d852fc.opkind6741 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Overlay.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:81 |
| s2c | 19 | 0 | (none) | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ColFfff.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:951 |
| s2c | 20 | 6 | u32be, u8neg, u8_128m | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C870b26fe.java:24; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:413 |
| s2c | 21 | 3 | u8neg, u16le_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ccf074931.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1444 |
| s2c | 22 | 8 | u16be, u32be, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C9a01f63b.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1580 |
| s2c | 23 | 8 | u16be, u8, u8, u8, u8, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdba04b48.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:843 |
| s2c | 24 | 12 | u32be, u32_mix2, u16be_m128, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C05450a40.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:712 |
| s2c | 25 | 8 | u16be, u8, u8, u16be, u8, u8 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc7b9a9fb.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:431 |
| s2c | 26 | 28 | byte offset += 28, then opUnused677 steps back 4 and compares a checksum to a u32be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/BZip2State.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:794 |
| s2c | 27 | 8 | u16le, u32_mix, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C60e4b017.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1204 |
| s2c | 28 | 6 | u16be, u32be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6fe1c24.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1754 |
| s2c | 29 | 3 | i8_neg, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C3524d4b6.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1466 |
| s2c | 30 | 2 | u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd414b330.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:170 |
| s2c | 31 | 8 | u32be, u16le, u16be_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SuaListaDeAmigosEstCheiaLimiteParaOsUsuRio.java:16; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:683 |
| s2c | 32 | -2 | see sync.md, small coordinate deltas | VousTesPasAutorisRejoindreLeCanalDeClanDeC.call59622295 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cee94503e.java:36; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1421 |
| s2c | 33 | 15 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SoundBank.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1603 |
| s2c | 34 | 15 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/BasType.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1184 |
| s2c | 35 | -2 | jstr, u8, jstr, u64be, i8, u8, jstr, u8, jstr, u16be, i8, jstr | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cadb73c12.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:723 |
| s2c | 36 | 8 | u16le_m128, u32_mix2, u16be_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cf532c0ef.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:331 |
| s2c | 37 | -2 | see sync.md, large coordinate deltas | VousTesPasAutorisRejoindreLeCanalDeClanDeC.call59622295 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C60b05013.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1061 |
| s2c | 38 | 14 | u32_mix2, u16le, u32_mix, u16be_m128, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ce5d3f87a.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:956 |
| s2c | 39 | 17 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ModelParticleEffector.java:28; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:693 |
| s2c | 40 | 1 | u8 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C0ca90caa.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:839 |
| s2c | 41 | 3 | u8_m128, u8neg, u8_128m | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C897c0956.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:945 |
| s2c | 42 | 3 | u8, u8neg, u8_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C00d852fc.java:16; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:875 |
| s2c | 43 | -1 | jstr, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C78938ee3.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:596 |
| s2c | 44 | 8 | u32be, u16le_m128, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C6c80930e.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1227 |
| s2c | 45 | 1 | u8 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdc377551.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1440 |
| s2c | 46 | -1 | jstr, u16be_m128, u8_128m, u8_128m | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C3c9f52c8.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1012 |
| s2c | 47 | 6 | u32le, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C1160dfa5.java:29; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1237 |
| s2c | 48 | 5 | i8_m128, u16be_m128, u16le_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C97555c89.java:22; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:111 |
| s2c | 49 | -1 | jstr | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ce564a28a.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1472 |
| s2c | 50 | -1 | u8, jstr, jstr, jstr, jstr | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C9935070d.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1243 |
| s2c | 51 | -1 | u8, jstr, jstr, u64be, u16be, u24be, u8, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca955aed5.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1488 |
| s2c | 52 | 9 | u16be_m128, u8_128m, u32le, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ce564a28a.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:397 |
| s2c | 53 | -1 | u16be, jstr, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/MaterialManager_Class106_Sub1_Sub1.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1607 |
| s2c | 54 | 12 | u16le_m128, u16le, u32_mix, u32_mix2 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C92fd9bf3.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1043 |
| s2c | 55 | -1 | u8, jstr, jstr, u16be, u24be, u8 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C079962ec.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:542 |
| s2c | 56 | -1 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cadb73c12.java:10; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:860 |
| s2c | 57 | 12 | u32_mix, u16le_m128, u16be_m128, u16be_m128, u16be_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/GlRenderList.java:20; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1617 |
| s2c | 58 | 0 | (none) | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C2356a3f8.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:603 |
| s2c | 59 | -2 | u8_128m, u8, u8neg, then while bytes remain: u8 selector and C34efe850.opkind8064 of Cb55a2183.opkind6385()[selector]. Selector bodies were not read | C34efe850.opkind8064 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C0922b514.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1652 |
| s2c | 60 | -2 | jstr, u16le, u32_mix2 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SuaListaDeIgnoradosEstCheia.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1407 |
| s2c | 61 | 4 | u8_128m, u16le, u8_128m | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C23d8b466.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1290 |
| s2c | 62 | 6 | u8_m128, u24be, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C2c56d79f.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1301 |
| s2c | 63 | -1 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOp17.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1417 |
| s2c | 64 | 3 | u8, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ChargementDeMechscapeEnCoursVeuillezPatien.java:89; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1198 |
| s2c | 65 | 5 | u16le, u8, u16be_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/MitgliederNnenFreundeHinzufGenFreieSpieler.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1569 |
| s2c | 66 | 10 | u32_mix, u32_mix, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C7e0331f9.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:129 |
| s2c | 67 | -1 | jstr, u8, jstr, u16be, i8, jstr | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C6402f7a6.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1065 |
| s2c | 68 | 4 | u32be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C4112a865.java:16; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1188 |
| s2c | 69 | -2 | u16be, u8, u16be, u16le, u8, u32_mix | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/NpcType.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1549 |
| s2c | 70 | -2 | u8, jstr, jstr, u16be, u8, jstr, u8 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C9a01f63b.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:444 |
| s2c | 71 | 20 | u8 index, then u8. If that second byte is 0 the other 18 bytes of this fixed packet are not read. Otherwise the offset rewinds one and Ced45c6ab(Unused6) reads i8, u16be, u32be, u32be, u32be, u32be. deob/Ced45c6ab.java:150 | Ced45c6ab.<init> | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cbded41f3.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:803 |
| s2c | 72 | -2 | see sync.md | DiesesSystemDarfNichtMissbrauchtWerden.opkind7151 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc7b9a9fb.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:350 |
| s2c | 73 | 6 | u16le_m128, u32le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca1086244.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:607 |
| s2c | 74 | 4 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C7c8a7e8e.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1403 |
| s2c | 75 | 0 | (none) | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C707c400e.java:29; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:85 |
| s2c | 76 | -1 | jstr | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C86e345bf.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:178 |
| s2c | 77 | 12 | u16be, u16le, u16be_m128, u16be, u32_mix2 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C1579fb1e.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:354 |
| s2c | 78 | 6 | u16be_m128, u32_mix2 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOpCurve.java:16; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1540 |
| s2c | 79 | 7 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOp29SubOp4.java:24; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1728 |
| s2c | 80 | 2 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C587d6f55.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1218 |
| s2c | 81 | 8 | u32_mix2, u16le_m128, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/NoiseGeneratorRelated.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1389 |
| s2c | 82 | 0 | (none) | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C008c004b.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:163 |
| s2c | 83 | 7 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SoftwareFont.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:835 |
| s2c | 84 | -1 | u8, jstr, jstr, u64be, u16be, u24be, u8 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C337f49ad.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1312 |
| s2c | 85 | -2 | jstr, u16le_m128, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ZugewiesenerSpeicher.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1280 |
| s2c | 86 | 10 | u32le, u16le, i16le, i16be_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOp29SubOp4.java:28; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1425 |
| s2c | 87 | -2 | u16be, jstr, jstr, u32be, u32be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C49e80b76.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1732 |
| s2c | 88 | -2 | see region.md, dynamic | Cc4cace49.opkind9583 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C408bee69.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1599 |
| s2c | 89 | 6 | u32le, u16le_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SoundBank.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:388 |
| s2c | 90 | 12 | u16be, u16be, u16be, u16le_m128, u32le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C798f1741.java:11; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1476 |
| s2c | 91 | 2 | u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca51ca532.java:22; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:121 |
| s2c | 92 | 6 | u16be (65535 becomes -1), u8, u16be, u8 | Cf4ef65ea.opkind10014 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc4e911ab.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:671 |
| s2c | 93 | 5 | u8, u16le, u16le | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C6ada6b6d.java:17; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:90 |
| s2c | 94 | 3 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cde6fd388.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:871 |
| s2c | 95 | 8 | u32_mix2, u16be_m128, u16le_m128 | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/PureJavaToolkitContext.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1452 |
| s2c | 96 | 6 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Unused8.java:14; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1771 |
| s2c | 97 | 7 | u16le_m128, u32le, u8_128m | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/VarnBitType.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:641 |
| s2c | 98 | -2 | see region.md, static | Cc4cace49.opkind9583 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C2517e207.java:20; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1436 |
| s2c | 99 | -1 | smart, u32be, u8, jstr, jstr, jstr | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc7b9a9fb.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1155 |
| s2c | 100 | -2 | u16be, u8, smart, u16be, u8, u32be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/DeinChatraumIstJetztAusgeschaltet.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1367 |
| s2c | 101 | 14 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd4e86b82.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:697 |
| s2c | 102 | 12 | u8, u8, u8, u16be, u16be, u8, u16be, u16be, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/LesModificationsSerontApportEsVotreClanDan.java:18; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:1664 |
| s2c | 103 | 0 | (none) | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ProjAnimNode.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:651 |
| s2c | 104 | 4 |  | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C475dcb13.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:831 |
| s2c | 105 | -1 | u8, jstr, jstr, u16be, u24be, u8, u16be | C268455a9.opkind9551 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C82719fbc.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C268455a9.java:898 |

## Client to server

The opcode byte from `opkind8134` is not repeated in `fields`.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | 2 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOpCurve.java:15; builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOpCurve.java:15 |
| c2s | 1 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ClientScript.java:21; builds/rs2/578-2009-12-16/src/src/main/java/deob/ClientScript.java:21 |
| c2s | 2 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd09638cd.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd09638cd.java:6 |
| c2s | 3 | 7 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C04f53c60.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/C04f53c60.java:7 |
| c2s | 4 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc7b9a9fb.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc7b9a9fb.java:4 |
| c2s | 5 | 7 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/NoMomentoVocEstEmUmCanalDeCl.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/NoMomentoVocEstEmUmCanalDeCl.java:4 |
| c2s | 6 | 15 | u16be, u16le_add128, u16le, u8neg, u16le_add128, u32_mix674, u16le_add128 | Cdce76b51 menu action 13 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C4aacb79a.java:13; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdce76b51.java:259 |
| c2s | 7 | 4 | u16be, u16le | Cd6e44077.opkind10291 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdc5a159d.java:11; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6e44077.java:198 |
| c2s | 8 | 6 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca7df0199.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca7df0199.java:8 |
| c2s | 9 | 15 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cb987fd13.java:14; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cb987fd13.java:14 |
| c2s | 10 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ccf074931.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/Ccf074931.java:5 |
| c2s | 11 | 16 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Quaternion.java:13; builds/rs2/578-2009-12-16/src/src/main/java/deob/Quaternion.java:13 |
| c2s | 12 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/CanAddYourselfOwnFriendList.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/CanAddYourselfOwnFriendList.java:5 |
| c2s | 13 | 16 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C870b26fe.java:23; builds/rs2/578-2009-12-16/src/src/main/java/deob/C870b26fe.java:23 |
| c2s | 14 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SuaListaDeIgnoradosEstCheia.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/SuaListaDeIgnoradosEstCheia.java:12 |
| c2s | 15 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C97555c89.java:25; builds/rs2/578-2009-12-16/src/src/main/java/deob/C97555c89.java:25 |
| c2s | 16 | -1 | u8 strlen+1, pjstr, u8 | C15ab7d97.opkind9292 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C950f0e92.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C15ab7d97.java:74 |
| c2s | 17 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/MouseRecorder.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/MouseRecorder.java:4 |
| c2s | 18 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C292109ad.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C292109ad.java:8 |
| c2s | 19 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ImageProducerFrameBuffer.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/ImageProducerFrameBuffer.java:9 |
| c2s | 20 | 4 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C7ab33351.java:25; builds/rs2/578-2009-12-16/src/src/main/java/deob/C7ab33351.java:25 |
| c2s | 21 | 6 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C90803bf8.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C90803bf8.java:5 |
| c2s | 22 | 7 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/MoireEnCoursAttribution.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/MoireEnCoursAttribution.java:7 |
| c2s | 23 | -1 | u8 3*count, then count times u8, u16be | Cd6e44077.opkind10291 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cfdcd577a.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6e44077.java:168 |
| c2s | 24 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc5ae4471.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc5ae4471.java:12 |
| c2s | 25 | 2 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cadb73c12.java:11; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cadb73c12.java:11 |
| c2s | 26 | 7 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/UniversoDeJogoCriado.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/UniversoDeJogoCriado.java:8 |
| c2s | 27 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Component.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/Component.java:8 |
| c2s | 28 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C384b991b.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C384b991b.java:8 |
| c2s | 29 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SoftwareToolkitFactory.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/SoftwareToolkitFactory.java:7 |
| c2s | 30 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C6e1ca670.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C6e1ca670.java:5 |
| c2s | 31 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ByteArrayPool.java:16; builds/rs2/578-2009-12-16/src/src/main/java/deob/ByteArrayPool.java:16 |
| c2s | 32 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C2c1ee577.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/C2c1ee577.java:9 |
| c2s | 33 | 4 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C408bee69.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/C408bee69.java:9 |
| c2s | 34 | -1 |  | Cef613972.opkind78610 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc2fb57c0.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6e44077.java:38 |
| c2s | 35 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/SoftwareToolkitFactory.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/SoftwareToolkitFactory.java:4 |
| c2s | 36 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca3541dc7.java:11; builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca3541dc7.java:11 |
| c2s | 37 | 6 | u16be, u32le | Cd6e44077.opkind10291 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C91bec970.java:22; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6e44077.java:152 |
| c2s | 38 | 4 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ParticleSystem.java:22; builds/rs2/578-2009-12-16/src/src/main/java/deob/ParticleSystem.java:22 |
| c2s | 39 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C798f1741.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C798f1741.java:5 |
| c2s | 40 | 7 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdba04b48.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdba04b48.java:4 |
| c2s | 41 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cef613972.java:16; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cef613972.java:16 |
| c2s | 42 | -1 | u8 0, then u8 length and bytes from opSceneGraph_Class12079 (blob unread) | Cd6e44077.opkind10291 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C234869d1.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6e44077.java:216 |
| c2s | 43 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdf90cca7.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdf90cca7.java:7 |
| c2s | 44 | 1 | u8 | Cd6e44077.opkind10291 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C83b47d7d.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6e44077.java:205 |
| c2s | 45 | 3 | u16le, u8_add128 | Cdce76b51 menu action 23 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C56f6aa15.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdce76b51.java:277 |
| c2s | 46 | 2 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ce564a28a.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/Ce564a28a.java:6 |
| c2s | 47 | 7 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ClientScript.java:20; builds/rs2/578-2009-12-16/src/src/main/java/deob/ClientScript.java:20 |
| c2s | 48 | 11 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Overlay.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/Overlay.java:8 |
| c2s | 49 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ClientScript.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/ClientScript.java:9 |
| c2s | 50 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ClientScript.java:10; builds/rs2/578-2009-12-16/src/src/main/java/deob/ClientScript.java:10 |
| c2s | 51 | 0 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cff30c109.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cff30c109.java:8 |
| c2s | 52 | 12 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ccce5b0b2.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/Ccce5b0b2.java:7 |
| c2s | 53 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cf5403b04.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cf5403b04.java:7 |
| c2s | 54 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C3a1ccbff.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C3a1ccbff.java:6 |
| c2s | 55 | 0 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C2c56d79f.java:13; builds/rs2/578-2009-12-16/src/src/main/java/deob/C2c56d79f.java:13 |
| c2s | 56 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C16f98a91.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C16f98a91.java:6 |
| c2s | 57 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc4cace49.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cc4cace49.java:5 |
| c2s | 58 | 2 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOp19.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/TextureOp19.java:7 |
| c2s | 59 | 18 | u8_add128, u16be, u16le; minimap kind adds u8 255, u8 255, u16be, u8 57, u8, u8, u8 89, u16be, u16be, u8 63 | Entity.opEntity19 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C96ed0652.java:4; builds/rs2/578-2009-12-16/src/src/main/java/deob/Entity.java:30 |
| c2s | 60 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C1ffccd0f.java:12; builds/rs2/578-2009-12-16/src/src/main/java/deob/C1ffccd0f.java:12 |
| c2s | 61 | -1 | u8 length, then per sample one of u16be, or u8 plus u16be, or u16be plus u32be, or u8 plus u32be. See input.md | Cd6e44077.opkind10291 | builds/rs2/578-2009-12-16/src/src/main/java/deob/LswprPos.java:18; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6e44077.java:79 |
| c2s | 62 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cee0dc575.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cee0dc575.java:5 |
| c2s | 63 | 8 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C897c0956.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C897c0956.java:6 |
| c2s | 64 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C30d052dc.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C30d052dc.java:6 |
| c2s | 65 | 4 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C4aacb79a.java:14; builds/rs2/578-2009-12-16/src/src/main/java/deob/C4aacb79a.java:14 |
| c2s | 66 | 2 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C8cb2fbe0.java:10; builds/rs2/578-2009-12-16/src/src/main/java/deob/C8cb2fbe0.java:10 |
| c2s | 67 | 4 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C91bec970.java:20; builds/rs2/578-2009-12-16/src/src/main/java/deob/C91bec970.java:20 |
| c2s | 68 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca51ca532.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca51ca532.java:6 |
| c2s | 69 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C312500b3.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/C312500b3.java:5 |
| c2s | 70 | 3 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C81d8950f.java:8; builds/rs2/578-2009-12-16/src/src/main/java/deob/C81d8950f.java:8 |
| c2s | 71 | 0 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca760d128.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/Ca760d128.java:7 |
| c2s | 72 | 7 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C565d1093.java:17; builds/rs2/578-2009-12-16/src/src/main/java/deob/C565d1093.java:17 |
| c2s | 73 | 7 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Cf2a3eab0.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cf2a3eab0.java:6 |
| c2s | 74 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ChargementDeMechscapeEnCoursVeuillezPatien.java:86; builds/rs2/578-2009-12-16/src/src/main/java/deob/ChargementDeMechscapeEnCoursVeuillezPatien.java:86 |
| c2s | 75 | 11 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/GlRelated6.java:14; builds/rs2/578-2009-12-16/src/src/main/java/deob/GlRelated6.java:14 |
| c2s | 76 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Ce0a28d2a.java:16; builds/rs2/578-2009-12-16/src/src/main/java/deob/Ce0a28d2a.java:16 |
| c2s | 77 | -1 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/Js5ResourceProvider.java:9; builds/rs2/578-2009-12-16/src/src/main/java/deob/Js5ResourceProvider.java:9 |
| c2s | 78 | 5 | u8_add128, u16be, u16le | Entity.opEntity19 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C6defb8da.java:7; builds/rs2/578-2009-12-16/src/src/main/java/deob/Entity.java:30 |
| c2s | 79 | 7 | u16be, u8_add128, u16be_add128, u16be | Cdce76b51 menu action 6 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ClientScript.java:22; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cdce76b51.java:289 |
| c2s | 80 | 7 |  | LswprPos.opkind9851 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C0ca90caa.java:6; builds/rs2/578-2009-12-16/src/src/main/java/deob/C0ca90caa.java:6 |
| c2s | 81 | 2 | u16be | Cd6e44077.opkind10291 | builds/rs2/578-2009-12-16/src/src/main/java/deob/ServerActiveProperties.java:5; builds/rs2/578-2009-12-16/src/src/main/java/deob/Cd6e44077.java:41 |

## Login and JS5

These opcodes share numbers with the game tables and are not looked up in `opkind7258`.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 14 | 2 | u8 opcode 14, u8 (usernameLong >> 16) & 31 | LswprPos.calla978325d | builds/rs2/578-2009-12-16/src/src/main/java/deob/LswprPos.java:90 |
| c2s | 15 | 5 | u8 opcode 15, u32be 578 | Client | builds/rs2/578-2009-12-16/src/src/main/java/deob/Client.java:1576 |
| c2s | 16 | -2 | see login.md. Constructor size is discarded | LswprPos.calla978325d | builds/rs2/578-2009-12-16/src/src/main/java/deob/LswprPos.java:143 |
| c2s | 17 | 1 | isaac opcode only | LswprPos.calla978325d | builds/rs2/578-2009-12-16/src/src/main/java/deob/LswprPos.java:251 |
| c2s | 18 | -2 | same bytes as opcode 16, sent when login state is 40 | LswprPos.calla978325d | builds/rs2/578-2009-12-16/src/src/main/java/deob/LswprPos.java:140 |
| c2s | 22 | -2 |  | Cf3727c90.opkind9792 | builds/rs2/578-2009-12-16/src/src/main/java/deob/BasType.java:30 |
| c2s | 24 | -1 | u8 10, u32be, u64be, u32be, pjstr, u32be, RSA, then u8 opcode 24, u8 rsaLen+2, u16be 578, RSA bytes | C1659b20b.callc660d548 | builds/rs2/578-2009-12-16/src/src/main/java/deob/C1659b20b.java:80 |

