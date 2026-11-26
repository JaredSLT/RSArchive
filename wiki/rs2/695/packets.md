# Packets

One row per c2s opcode stored by `Bwz.opkind25975`. Direction `c2s`. Size is that constructor's size argument (`framing.md`). Fields are payload writes after `C54dbe422.opkind28982`. The opcode byte itself is inside that stub and is unread. There is no s2c opcode row: no s2c handler was read.

`jstr` and `len` are defined in `framing.md`. An empty field cell means the sender was not read. `(none)` means the sender was read and the bytes between packet-start and `C02bd0dd5.opkind28916` contain no buffer write.

Opcodes 1 and 56 have constructors and no decompiled sender. `Zqw.valuekind23663` is only stored (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Zqw.java:86`). `Cfa8219d8.valuekind27061` is stored at `builds/rs2/695-2011-12-22/src/src/main/java/deob/Cfa8219d8.java:207` and is only touched again on guard paths that pass a bad argument (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Cfa8219d8.java:113`, `builds/rs2/695-2011-12-22/src/src/main/java/deob/Cfa8219d8.java:176`). `KeBz.opkind25137` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/KeBz.java:55`) could not be decompiled. `rslib/src/both/move/rev_695.c` encodes opcode 1 as `p2_le_add128`, `p2`, `p1_sub128`. That field order is not in a Java method that was read.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 0 | 12 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/CkRo.java:148 |
| c2s | 1 | 5 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Zqw.java:86 |
| c2s | 2 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C40ad719b.java:92 |
| c2s | 3 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5d01d1b2.java:104 |
| c2s | 4 | 1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2d8699d9.java:52 |
| c2s | 5 | 4 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5b9599bf.java:81 |
| c2s | 6 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ogb.java:97 |
| c2s | 7 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/NpNa.java:101 |
| c2s | 8 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf6bc0332.java:86 |
| c2s | 9 | -1 | u8(String.length()+1), jstr | Ca0a70a56.opkind25002 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ca0a70a56.java:19 |
| c2s | 10 | -1 | u8(2+(len(a)+1)+(len(b)+1)), jstr, u8, u8, jstr | CqFz.opkind23921 | builds/rs2/695-2011-12-22/src/src/main/java/deob/CqFz.java:84 |
| c2s | 11 | -1 | Cac85f8eb: u8(len+1), jstr; Cf025e7de: u8(0) | Cac85f8eb.opkind21194 (deob/Cac85f8eb.java:24); Cf025e7de.opkind27172 (deob/Cf025e7de.java:90) | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cac85f8eb.java:24 |
| c2s | 12 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C3f8bc05e.java:96 |
| c2s | 13 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc4abf44b.java:245 |
| c2s | 14 | 0 | (none) | Client.opClient275 (deob/Client.java:1646); Vkx.opkind26913 (deob/Vkx.java:19) | builds/rs2/695-2011-12-22/src/src/main/java/deob/Vkx.java:19 |
| c2s | 15 | 4 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C30e83732.java:47 |
| c2s | 16 | 11 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Fif.java:75 |
| c2s | 17 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ccf8b1408.java:66 |
| c2s | 18 | 7 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C37f4edaa.java:67 |
| c2s | 19 | 9 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd146f731.java:39 |
| c2s | 20 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/BsNt.java:197 |
| c2s | 21 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5d38035b.java:106 |
| c2s | 22 | 7 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C397ef5ae.java:43 |
| c2s | 23 | 6 | u8, u16be, u16be, u8 | OlCc.opkind29853 | builds/rs2/695-2011-12-22/src/src/main/java/deob/OlCc.java:25 |
| c2s | 24 | 7 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/BngBv.java:86 |
| c2s | 25 | 6 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C24181c4e.java:136 |
| c2s | 26 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf194086e.java:243 |
| c2s | 27 | 3 | u8, u8, u8 | C8bba036b.opkind27698 | builds/rs2/695-2011-12-22/src/src/main/java/deob/C8bba036b.java:59 |
| c2s | 28 | 15 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/KhTms.java:46 |
| c2s | 29 | 16 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cb783430a.java:158 |
| c2s | 30 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C59384afc.java:118 |
| c2s | 31 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/ZiCc.java:93 |
| c2s | 32 | -2 | u16be((len(a)+1)+(len(b)+1)), jstr if the boolean is true, jstr if the boolean is true | Ca6605093.opkind22094 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ca6605093.java:62 |
| c2s | 33 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Alz.java:111 |
| c2s | 34 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/BdEi.java:37 |
| c2s | 35 | 7 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C1a99908e.java:57 |
| c2s | 36 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Hbm.java:84 |
| c2s | 37 | 9 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cff2dbaf3.java:121 |
| c2s | 38 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2ba3dcee.java:103 |
| c2s | 39 | -2 | u16be(backpatched length), jstr, smart length (u8 if encoded length < 128, else u16be(32768+length) if length < 32768); encoded bytes unread | Cf8e807c7.opkind235310 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf8e807c7.java:83 |
| c2s | 40 | 9 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd18f18ec.java:128 |
| c2s | 41 | -1 | u8(String.length()+2), jstr, u8 | C433c77a6.opkind25205 | builds/rs2/695-2011-12-22/src/src/main/java/deob/C433c77a6.java:10 |
| c2s | 42 | -1 | u8(String.length()), jstr | Ctj.opkind28885 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ctj.java:25 |
| c2s | 43 | -1 | u8(backpatched length), u8(2 in HoKp when the byte arg is -33; 3 in Ce5a85176), u16be, unread C98edb2f9.opkind30717 | HoKp.opkind21302 (deob/HoKp.java:19); Ce5a85176.opkind27245 (deob/Ce5a85176.java:38) | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ce5a85176.java:38 |
| c2s | 44 | 0 | (none) | NgFz.opkind24882 | builds/rs2/695-2011-12-22/src/src/main/java/deob/NgFz.java:49 |
| c2s | 45 | 2 | u16be | Ceb5c69c5.opkind25106 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ceb5c69c5.java:113 |
| c2s | 46 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C57a13226.java:90 |
| c2s | 47 | 4 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ca4620b48.java:43 |
| c2s | 48 | 16 | u16be, u8(high), u8(low+128 when the method int is 0), bytes((v>>8), v, (v>>24), (v>>16)), bytes((v>>16), (v>>24), v, (v>>8)), u8(v+128), u8(v>>8), u8(high), u8(low+128) | C61fc9e68.opkind24831 | builds/rs2/695-2011-12-22/src/src/main/java/deob/C61fc9e68.java:10 |
| c2s | 49 | 4 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C6140e91a.java:98 |
| c2s | 50 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5a7cf2fc.java:94 |
| c2s | 51 | 11 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Bsa.java:83 |
| c2s | 52 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cb4103721.java:46 |
| c2s | 53 | 1 | u8 | CfBg.opkind247040 | builds/rs2/695-2011-12-22/src/src/main/java/deob/CfBg.java:75 |
| c2s | 54 | 6 | bytes((v>>8), v, (v>>24), (v>>16)), u16be | C0902c09f.opkind22184 | builds/rs2/695-2011-12-22/src/src/main/java/deob/C0902c09f.java:27 |
| c2s | 55 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C0c1143e3.java:59 |
| c2s | 56 | 18 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cfa8219d8.java:207 |
| c2s | 57 | -1 | u8((len(a)+1)+(len(b)+1)+1), jstr, jstr, u8(bit0 if the first boolean, bit1 if the second) | Wal.opkind27478 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Wal.java:25 |
| c2s | 58 | 7 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C30611ec0.java:75 |
| c2s | 59 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ays.java:149 |
| c2s | 60 | -1 | u8(String.length()+1), jstr | Cab539144.opkind29252 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cab539144.java:51 |
| c2s | 61 | 4 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Qof.java:212 |
| c2s | 62 | 0 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Oze.java:68 |
| c2s | 63 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cbdb4518c.java:18 |
| c2s | 64 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/MrEr.java:158 |
| c2s | 65 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ngs.java:102 |
| c2s | 66 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C12b53b72.java:108 |
| c2s | 67 | -1 | u8(len+1), jstr | CrvHh.opkind29591 | builds/rs2/695-2011-12-22/src/src/main/java/deob/CrvHh.java:47 |
| c2s | 68 | 2 | u16be | C4c82c6fc.opkind30056 | builds/rs2/695-2011-12-22/src/src/main/java/deob/C4c82c6fc.java:22 |
| c2s | 69 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C068e880b.java:95 |
| c2s | 70 | -1 | u8(len+1), jstr | Ce69281a9.opkind26888 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ce69281a9.java:182 |
| c2s | 71 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C46d7ee71.java:89 |
| c2s | 72 | 4 | u32be | OrcNp.opkind27375 | builds/rs2/695-2011-12-22/src/src/main/java/deob/OrcNp.java:16 |
| c2s | 73 | 7 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C51fa93c1.java:94 |
| c2s | 74 | 0 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/CqFz.java:71 |
| c2s | 75 | 9 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc65edb01.java:150 |
| c2s | 76 | 17 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/EzdvuOhcy.java:193 |
| c2s | 77 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc65edb01.java:147 |
| c2s | 78 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd98f8263.java:66 |
| c2s | 79 | 4 | u32be | Cpcg.opkind30093 (deob/Cpcg.java:8); Pyr.opkind22387 (deob/Pyr.java:40) | builds/rs2/695-2011-12-22/src/src/main/java/deob/Pyr.java:40 |
| c2s | 80 | 4 | u32be | QsIy.opkind24104 | builds/rs2/695-2011-12-22/src/src/main/java/deob/QsIy.java:31 |
| c2s | 81 | 9 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C9041a3a4.java:84 |
| c2s | 82 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C003d5dcc.java:93 |
| c2s | 83 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/ApAp.java:175 |
| c2s | 84 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C59e7e25b.java:72 |
| c2s | 85 | 3 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C634a31ab.java:113 |
| c2s | 86 | 2 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2e1922c0.java:65 |
| c2s | 87 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C79297f7a.java:224 |
| c2s | 88 | -1 | u8(len+1), jstr | C2f91c425.opkind27993 | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2f91c425.java:11 |
| c2s | 89 | 2 | u16be | EzdvuOhcy.opkind21151 | builds/rs2/695-2011-12-22/src/src/main/java/deob/EzdvuOhcy.java:68 |
| c2s | 90 | -1 | u8(String.length()+4), u8(0 or 1), u16be, jstr | Buildlive.opkind23553 | builds/rs2/695-2011-12-22/src/src/main/java/deob/Buildlive.java:33 |
| c2s | 91 | 9 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Fvf.java:147 |
| c2s | 92 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/C50b7ab2f.java:110 |
| c2s | 93 | 4 | u32be | QrEsRgFg.opkind21899 | builds/rs2/695-2011-12-22/src/src/main/java/deob/QrEsRgFg.java:50 |
| c2s | 94 | 8 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd8fcd301.java:71 |
| c2s | 95 | -1 | u8(len+1), jstr when the boolean is true | C5d38035b.opkind241310 | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5d38035b.java:11 |
| c2s | 96 | -1 |  | unread | builds/rs2/695-2011-12-22/src/src/main/java/deob/AvFi.java:56 |
