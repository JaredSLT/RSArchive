# Gaps

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| s2c size table | no array in the Java client; rslib packets/695.c absent; rev_695.c sizes_s2c is NULL | no-source. This client has no s2c size table. | blocked |
| ISAAC direction and the meaning of c2s size -1 and -2 | builds/rs2/695-2011-12-22/src/src/main/java/deob/C54dbe422.java:107 | unread. opkind28982 could not be decompiled. | blocked |
| jstr charset and encoded length | builds/rs2/695-2011-12-22/src/src/main/java/deob/Awd.java:80 | unread. opkind30402 could not be decompiled. Callers still write a trailing u8 0. | blocked |
| socket send | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc9967dcd.java:10 | unread. opkind27496 could not be decompiled. | blocked |
| JS5 byte-available wait | builds/rs2/695-2011-12-22/src/src/main/java/deob/Wal.java:144 | unread. opkind27476 could not be decompiled and returns false. | blocked |
| JS5 response codes other than the one compared byte | builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:2496 | unread. Nonzero byte calls opClient278, which was not read. Byte 0 advances state. | blocked |
| game login RSA block and modulus | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf1c47083.java:32 | unread. opkind27025 could not be decompiled. Exponent 10001 is at HdEb.java:47. opkind25941 and opkind27968 have no callers. | blocked |
| c2s opcode 1 walk body | builds/rs2/695-2011-12-22/src/src/main/java/deob/KeBz.java:55; builds/rs2/695-2011-12-22/src/src/main/java/deob/Zqw.java:86 | unread handler. RSLib rev_695.c walk_encode writes p2_le_add128, p2, p1_sub128. That order is not in the Java method. Size 5 is the constructor. | implementable |
| c2s opcode 56 minimap body | builds/rs2/695-2011-12-22/src/src/main/java/deob/KeBz.java:55; builds/rs2/695-2011-12-22/src/src/main/java/deob/Cfa8219d8.java:207 | unread handler. Size 18 is the constructor. RSLib says the extra minimap bytes were not written. The Java method is a stub, so this page leaves the field list empty. | implementable |
| c2s opcode 39 encoded string bytes | builds/rs2/695-2011-12-22/src/src/main/java/deob/NbFd.java:84; builds/rs2/695-2011-12-22/src/src/main/java/deob/XvAp.java:18 | unread. The length prefix rule is in C59fcb984.opkind24403. The encoder and the copy are stubs. | blocked |
| c2s opcode 43 bytes after the u16be | builds/rs2/695-2011-12-22/src/src/main/java/deob/C98edb2f9.java:26 | unread. opkind30717 could not be decompiled. The leading u8, subtype u8, and u16be are in packets.md. | blocked |
| player update, NPC update, mask order, empty-list terminator | builds/rs2/695-2011-12-22/src/src/main/java/deob/C37f4edaa.java:70 | unread. The bit reader is a stub. Step widths are in sync.md. No update packet was read. | blocked |
| which packet calls C6bfd9e17.opkind29292 | builds/rs2/695-2011-12-22/src/src/main/java/deob/C6bfd9e17.java:116 | unread. No decompiled caller. | blocked |
| region rebuild body, zone, scene origin, map x versus z | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5adf0e0a.java:41 | unread. opkind28764 and opkind28769 could not be decompiled. RSLib describes g2, g1_sub128, g2, g1_add128 and a tail, and leaves region_encode null. Map names are in Biy.opkind280313. | implementable |
| which packet calls XTEA | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ca53ed1b5.java:57 | unread caller. The 32-round mix is in region.md. No Java caller of opkind28634. | blocked |
| JS5 archive index ids | builds/rs2/695-2011-12-22/src/src/main/java/deob/Xejo.java:256 | unread. The 27 slots are mode objects, not numeric index literals. | blocked |
| config and interface decode opcodes | builds/rs2/695-2011-12-22/src/src/main/java/deob/FlUsSy.java:31 | unread. opkind25026 could not be decompiled. The u8-until-0 loop is in Cc24bb2d2.opkind26523. | blocked |
| disk cache | builds/rs2/695-2011-12-22/README.md; meta.json cache_downloaded false | no-cache. No disk cache is in this archive. | blocked |
| interface component types, scripts, top-level id, sidebar id | interfaces.md | unread. No method that opens those ids was read. | blocked |
| packet that starts a sound | sound.md | unread. Vorbis mix is cited. No start packet was found in the decompiled senders. | blocked |
| tick order | builds/rs2/695-2011-12-22/src/src/main/java/deob/Client.java:115 | unread. opkind248642 calls opClient258. The body was not read. | blocked |
| key packet | builds/rs2/695-2011-12-22/src/src/main/java/deob/WoMklAcaw.java:51 | unread. keyPressed forwards to C6bed1507.opkind20991, which was not read. | blocked |
| mouse wheel and camera values | builds/rs2/695-2011-12-22/src/src/main/java/deob/DootArraWtgXrcYtx.java:433 | unread. mousePressed button codes 0, 1, and 2 are in input.md. The wheel method and CameraControlMode values were not read. | blocked |
| software scene draw | builds/rs2/695-2011-12-22/src/src/main/java/deob/PureJavaModel.java:3 | unread. The class is the software model. Draw methods were not read. | blocked |
| jaggl | jaggl/ | platform-native. OpenGL binding. | blocked |
| jagdx | jagdx/ | platform-native. Direct3D binding. | blocked |
| jagtheora | jagtheora/ | platform-native. Ogg, Theora, and Vorbis binding. | blocked |
| c2s opcode 0 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/CkRo.java:148 | unread handler. Size 12 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 2 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C40ad719b.java:92 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 3 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5d01d1b2.java:104 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 4 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2d8699d9.java:52 | unread handler. Size 1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 5 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5b9599bf.java:81 | unread handler. Size 4 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 6 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ogb.java:97 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 7 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/NpNa.java:101 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 8 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf6bc0332.java:86 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 12 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C3f8bc05e.java:96 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 13 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc4abf44b.java:245 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 15 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C30e83732.java:47 | unread handler. Size 4 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 16 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Fif.java:75 | unread handler. Size 11 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 17 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ccf8b1408.java:66 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 18 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C37f4edaa.java:67 | unread handler. Size 7 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 19 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd146f731.java:39 | unread handler. Size 9 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 20 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/BsNt.java:197 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 21 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5d38035b.java:106 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 22 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C397ef5ae.java:43 | unread handler. Size 7 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 24 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/BngBv.java:86 | unread handler. Size 7 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 25 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C24181c4e.java:136 | unread handler. Size 6 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 26 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf194086e.java:243 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 28 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/KhTms.java:46 | unread handler. Size 15 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 29 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cb783430a.java:158 | unread handler. Size 16 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 30 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C59384afc.java:118 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 31 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/ZiCc.java:93 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 33 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Alz.java:111 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 34 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/BdEi.java:37 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 35 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C1a99908e.java:57 | unread handler. Size 7 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 36 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Hbm.java:84 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 37 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cff2dbaf3.java:121 | unread handler. Size 9 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 38 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2ba3dcee.java:103 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 40 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd18f18ec.java:128 | unread handler. Size 9 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 46 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C57a13226.java:90 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 47 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ca4620b48.java:43 | unread handler. Size 4 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 49 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C6140e91a.java:98 | unread handler. Size 4 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 50 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5a7cf2fc.java:94 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 51 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Bsa.java:83 | unread handler. Size 11 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 52 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cb4103721.java:46 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 55 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C0c1143e3.java:59 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 58 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C30611ec0.java:75 | unread handler. Size 7 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 59 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ays.java:149 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 61 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Qof.java:212 | unread handler. Size 4 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 62 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Oze.java:68 | unread handler. Size 0 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 63 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cbdb4518c.java:18 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 64 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/MrEr.java:158 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 65 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ngs.java:102 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 66 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C12b53b72.java:108 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 69 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C068e880b.java:95 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 71 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C46d7ee71.java:89 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 73 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C51fa93c1.java:94 | unread handler. Size 7 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 74 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/CqFz.java:71 | unread handler. Size 0 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 75 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc65edb01.java:150 | unread handler. Size 9 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 76 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/EzdvuOhcy.java:193 | unread handler. Size 17 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 77 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc65edb01.java:147 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 78 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd98f8263.java:66 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 81 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C9041a3a4.java:84 | unread handler. Size 9 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 82 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C003d5dcc.java:93 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 83 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/ApAp.java:175 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 84 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C59e7e25b.java:72 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 85 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C634a31ab.java:113 | unread handler. Size 3 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 86 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2e1922c0.java:65 | unread handler. Size 2 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 87 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C79297f7a.java:224 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 91 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Fvf.java:147 | unread handler. Size 9 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 92 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/C50b7ab2f.java:110 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 94 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd8fcd301.java:71 | unread handler. Size 8 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
| c2s opcode 96 payload | builds/rs2/695-2011-12-22/src/src/main/java/deob/AvFi.java:56 | unread handler. Size -1 is the Bwz.opkind25975 argument. Field list is empty. | implementable |
