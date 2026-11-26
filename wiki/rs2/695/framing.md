# Framing

Source tree `builds/rs2/695-2011-12-22/src/src/main/java`, base `962888d50a69a95e6011e2018a39fe668ed4982a`. Sizes below are the opcode and size arguments of `Bwz.opkind25975` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Bwz.java:8`). The guard byte must be greater than 98 or the method returns a fresh array and stores nothing. On the live path it writes `opcode * -1474902435` into `((int[])obj[0])[1]` and `size * -1272501511` into `((int[])obj[0])[0]`.

## C2S sizes

97 constructors, opcodes 0 through 96, each opcode once. Opcodes 97 through 255 have no constructor. `rslib/src/client/packets/695.c` and `rslib/src/server/packets/695.c` are absent. `rslib/src/both/move/rev_695.c` uses the same 0..96 sizes and `-3` for 97..255. That size list matches these constructors, so there is no size disagreement to record. The walk and region field lists in that file are a separate gap (`gaps.md`).

| opcode | size | static | constructor |
| --- | --- | --- | --- |
| 0 | 12 | `valuekind27203` | builds/rs2/695-2011-12-22/src/src/main/java/deob/CkRo.java:148 |
| 1 | 5 | `valuekind23663` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Zqw.java:86 |
| 2 | 3 | `valuekind29153` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C40ad719b.java:92 |
| 3 | -1 | `valuekind22762` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5d01d1b2.java:104 |
| 4 | 1 | `valuekind21095` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2d8699d9.java:52 |
| 5 | 4 | `valuekind22321` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5b9599bf.java:81 |
| 6 | 8 | `valuekind25143` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ogb.java:97 |
| 7 | 8 | `valuekind25241` | builds/rs2/695-2011-12-22/src/src/main/java/deob/NpNa.java:101 |
| 8 | 3 | `valuekind29161` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf6bc0332.java:86 |
| 9 | -1 | `valuekind21643` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C55da887c.java:267 |
| 10 | -1 | `valuekind27213` | builds/rs2/695-2011-12-22/src/src/main/java/deob/WaDe.java:99 |
| 11 | -1 | `valuekind26413` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2f4ed000.java:170 |
| 12 | 3 | `valuekind25843` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C3f8bc05e.java:96 |
| 13 | 3 | `valuekind30333` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc4abf44b.java:245 |
| 14 | 0 | `valuekind27501` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C6c864a94.java:99 |
| 15 | 4 | `valuekind25234` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C30e83732.java:47 |
| 16 | 11 | `valuekind28641` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Fif.java:75 |
| 17 | -1 | `valuekind21351` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ccf8b1408.java:66 |
| 18 | 7 | `valuekind21487` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C37f4edaa.java:67 |
| 19 | 9 | `valuekind25702` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd146f731.java:39 |
| 20 | 3 | `valuekind29641` | builds/rs2/695-2011-12-22/src/src/main/java/deob/BsNt.java:197 |
| 21 | -1 | `valuekind24131` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5d38035b.java:106 |
| 22 | 7 | `valuekind294510` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C397ef5ae.java:43 |
| 23 | 6 | `valuekind30912` | builds/rs2/695-2011-12-22/src/src/main/java/deob/PksCc.java:106 |
| 24 | 7 | `valuekind24021` | builds/rs2/695-2011-12-22/src/src/main/java/deob/BngBv.java:86 |
| 25 | 6 | `valuekind22421` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C24181c4e.java:136 |
| 26 | 3 | `valuekind23305` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf194086e.java:243 |
| 27 | 3 | `valuekind22631` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C76308b3a.java:73 |
| 28 | 15 | `valuekind27391` | builds/rs2/695-2011-12-22/src/src/main/java/deob/KhTms.java:46 |
| 29 | 16 | `valuekind27981` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cb783430a.java:158 |
| 30 | -1 | `valuekind20943` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C59384afc.java:118 |
| 31 | -1 | `valuekind25111` | builds/rs2/695-2011-12-22/src/src/main/java/deob/ZiCc.java:93 |
| 32 | -2 | `valuekind20774` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Egp.java:281 |
| 33 | 8 | `valuekind21126` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Alz.java:111 |
| 34 | 3 | `valuekind25185` | builds/rs2/695-2011-12-22/src/src/main/java/deob/BdEi.java:37 |
| 35 | 7 | `valuekind22212` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C1a99908e.java:57 |
| 36 | 3 | `valuekind24374` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Hbm.java:84 |
| 37 | 9 | `valuekind21311` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cff2dbaf3.java:121 |
| 38 | 3 | `valuekind24692` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2ba3dcee.java:103 |
| 39 | -2 | `valuekind25852` | builds/rs2/695-2011-12-22/src/src/main/java/deob/HdEb.java:48 |
| 40 | 9 | `valuekind28822` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd18f18ec.java:128 |
| 41 | -1 | `valuekind29121` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Eusb.java:151 |
| 42 | -1 | `valuekind22091` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ca6605093.java:93 |
| 43 | -1 | `valuekind21221` | builds/rs2/695-2011-12-22/src/src/main/java/deob/RmFhRuFvDgBgThHfFcnTx.java:208 |
| 44 | 0 | `valuekind27733` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C0684bb0b.java:140 |
| 45 | 2 | `valuekind29025` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd6c57779.java:188 |
| 46 | 8 | `valuekind29563` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C57a13226.java:90 |
| 47 | 4 | `valuekind26613` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ca4620b48.java:43 |
| 48 | 16 | `valuekind25564` | builds/rs2/695-2011-12-22/src/src/main/java/deob/BvRr.java:77 |
| 49 | 4 | `valuekind27074` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C6140e91a.java:98 |
| 50 | 3 | `valuekind30391` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C5a7cf2fc.java:94 |
| 51 | 11 | `valuekind27587` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Bsa.java:83 |
| 52 | -1 | `valuekind20971` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cb4103721.java:46 |
| 53 | 1 | `valuekind22831` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ggg.java:94 |
| 54 | 6 | `valuekind22584` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C34e3e756.java:111 |
| 55 | 3 | `valuekind28673` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C0c1143e3.java:59 |
| 56 | 18 | `valuekind27061` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cfa8219d8.java:207 |
| 57 | -1 | `valuekind30923` | builds/rs2/695-2011-12-22/src/src/main/java/deob/EewWb.java:140 |
| 58 | 7 | `valuekind21066` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C30611ec0.java:75 |
| 59 | 3 | `valuekind30631` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ays.java:149 |
| 60 | -1 | `valuekind29214` | builds/rs2/695-2011-12-22/src/src/main/java/deob/KoBr.java:753 |
| 61 | 4 | `valuekind21341` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Qof.java:212 |
| 62 | 0 | `valuekind30241` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Oze.java:68 |
| 63 | 8 | `valuekind26941` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cbdb4518c.java:18 |
| 64 | 8 | `valuekind25507` | builds/rs2/695-2011-12-22/src/src/main/java/deob/MrEr.java:158 |
| 65 | -1 | `valuekind20741` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Ngs.java:102 |
| 66 | -1 | `valuekind30821` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C12b53b72.java:108 |
| 67 | -1 | `valuekind24515` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C4aec345e.java:93 |
| 68 | 2 | `valuekind25674` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cae00e614.java:83 |
| 69 | 3 | `valuekind23712` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C068e880b.java:95 |
| 70 | -1 | `valuekind27145` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C178d60a3.java:51 |
| 71 | 3 | `valuekind24811` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C46d7ee71.java:89 |
| 72 | 4 | `valuekind21287` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C318cbec0.java:115 |
| 73 | 7 | `valuekind22542` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C51fa93c1.java:94 |
| 74 | 0 | `valuekind23921` | builds/rs2/695-2011-12-22/src/src/main/java/deob/CqFz.java:71 |
| 75 | 9 | `valuekind21115` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc65edb01.java:150 |
| 76 | 17 | `valuekind21151` | builds/rs2/695-2011-12-22/src/src/main/java/deob/EzdvuOhcy.java:193 |
| 77 | 8 | `valuekind21112` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc65edb01.java:147 |
| 78 | 8 | `valuekind27336` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd98f8263.java:66 |
| 79 | 4 | `valuekind25042` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Kvr.java:91 |
| 80 | 4 | `valuekind20723` | builds/rs2/695-2011-12-22/src/src/main/java/deob/XvAp.java:73 |
| 81 | 9 | `valuekind29243` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C9041a3a4.java:84 |
| 82 | 8 | `valuekind20714` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C003d5dcc.java:93 |
| 83 | 3 | `valuekind21791` | builds/rs2/695-2011-12-22/src/src/main/java/deob/ApAp.java:175 |
| 84 | -1 | `valuekind22042` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C59e7e25b.java:72 |
| 85 | 3 | `valuekind26124` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C634a31ab.java:113 |
| 86 | 2 | `valuekind29581` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2e1922c0.java:65 |
| 87 | -1 | `valuekind28543` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C79297f7a.java:224 |
| 88 | -1 | `valuekind21391` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2cc60217.java:45 |
| 89 | 2 | `valuekind27333` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd98f8263.java:64 |
| 90 | -1 | `valuekind27882` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C0ab50657.java:78 |
| 91 | 9 | `valuekind24801` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Fvf.java:147 |
| 92 | 8 | `valuekind27342` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C50b7ab2f.java:110 |
| 93 | 4 | `valuekind29671` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C6fbd0173.java:38 |
| 94 | 8 | `valuekind23982` | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cd8fcd301.java:71 |
| 95 | -1 | `valuekind21464` | builds/rs2/695-2011-12-22/src/src/main/java/deob/C0cc47847.java:77 |
| 96 | -1 | `valuekind26361` | builds/rs2/695-2011-12-22/src/src/main/java/deob/AvFi.java:56 |

## S2C sizes

Absent. No `int[256]` in this client is a packet-size table. The `int[256]` objects that were opened are a CRC table (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Wwif.java:71`, polynomial `-306674912`), glyph widths (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Cac04599f.java:82`), a sine and cosine table (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C63a46d96.java:11`), model histograms (`builds/rs2/695-2011-12-22/src/src/main/java/deob/GlModel.java:2077`, `builds/rs2/695-2011-12-22/src/src/main/java/deob/GpuModel.java:1971`, `builds/rs2/695-2011-12-22/src/src/main/java/deob/C37689905.java:641`), and a 0..255 clamp (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C69c6784f.java:115`, filled in `builds/rs2/695-2011-12-22/src/src/main/java/deob/C4a688b2f.java:17`).

`C6140e91a.opkind27072` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C6140e91a.java:62`) stores `346064667 * opcode` and does not store its fourth int. The call sites in `builds/rs2/695-2011-12-22/src/src/main/java/deob/C2ba3dcee.java:105` pass fourth ints `0`, `-1`, `-2`, and `4` for numeric ids 14, 15, 16, 19, 22, 23, 24, 26, 27, 28, 29, and 30. Those fourth ints are not sizes. Id 15 decodes with `* -1673233645` to 15 and is the JS5 handshake byte in `login.md`, not c2s opcode 15 (size 4, `builds/rs2/695-2011-12-22/src/src/main/java/deob/C30e83732.java:47`).

## Variable length and ISAAC

Unread. `C54dbe422.opkind28982(byte, Object[] prototype, Object[] stream)` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C54dbe422.java:107`) is the packet-start call every decompiled sender makes, and the method could not be decompiled. It returns null in the stub. The opcode byte, any ISAAC mix, and the meaning of size `-1` and size `-2` are inside that method. This page does not assign a meaning to `-1` or `-2`.

After that call, `C02bd0dd5.opkind28916` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C02bd0dd5.java:13`) queues the node when its int argument is 14576. The queue lines still follow the guard. The queued length is the buffer offset saved before the offset is reset. That queue is not an ISAAC cipher.

## Buffer

A packet buffer from `C3cae535d.opkind21655` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C3cae535d.java:69`) keeps the stored offset at `((int[])buf[0])[0]` and the bytes at `(byte[])buf[2]`. The real index is `stored * 953738501` (32-bit). Adding `-1068425267` to the stored offset advances one byte. `Cf9661093.opkind20932` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf9661093.java:76`) adds `21266228` to the stored offset, which is four bytes, then assembles a big-endian int. `Ca53ed1b5.opkind28634` subtracts `42532456` from the stored offset, which is eight bytes (`region.md`).

| method | width | citation |
| --- | --- | --- |
| `Pgo.opkind24522` | u8. The byte is written, then a static is cleared when the guard is not 37. | builds/rs2/695-2011-12-22/src/src/main/java/deob/Pgo.java:38 |
| `ZvEx.opkind25622` | u16be, high then low. Both bytes are written. The guard 1 sits between them. | builds/rs2/695-2011-12-22/src/src/main/java/deob/ZvEx.java:69 |
| `C2c7eccd5.opkind21945` | u32be, `>>24`, `>>16`, `>>8`, low. Guard `-16807` calls a stub first when it fails. The four writes follow the guard. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2c7eccd5.java:40 |
| `Efd.opkind28926` | u24be (`>>16`, `>>8`, low) when the boolean is true. | builds/rs2/695-2011-12-22/src/src/main/java/deob/Efd.java:30 |
| `C50b7ab2f.opkind27345` | five bytes of a long (`>>32`, `>>24`, `>>16`, `>>8`, low) when the guard is `-19151`. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C50b7ab2f.java:64 |
| `C031e5805.opkind27928` | four bytes `(v>>8)`, `v`, `(v>>24)`, `(v>>16)`. The third parameter is unused. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C031e5805.java:15 |
| `C947dd79c.opkind25605` | four bytes `(v>>16)`, `(v>>24)`, `v`, `(v>>8)`. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C947dd79c.java:48 |
| `C39055ec1.opkind25281` | `(byte)(v+128)` then `(byte)(v>>8)`. Both bytes are written. Guard `-14` sits between them. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C39055ec1.java:126 |
| `C2e1922c0.opkind29587` | `(byte)(v>>8)` always. `(byte)(v+128)` only when the guard is `-15922`. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C2e1922c0.java:15 |
| `C183a0c8c.opkind25368` | one byte `(byte)(128-v)` at relative index `var2`, after the offset advances. The walk assembler does not call it in decompiled Java. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C183a0c8c.java:12 |
| `ZiCc.opkind25114` | read `255 & (128 - byte)`. | builds/rs2/695-2011-12-22/src/src/main/java/deob/ZiCc.java:10 |
| `NgFz.opkind248815` | read u8 when the guard is `-7`. | builds/rs2/695-2011-12-22/src/src/main/java/deob/NgFz.java:95 |
| `C743384ae.opkind20987` | when the boolean is true and the string has no NUL: `Awd.opkind30402` then u8 0. `Awd.opkind30402` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Awd.java:80`) could not be decompiled, so the charset and the byte count are unread. When the boolean is false, this method writes nothing. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C743384ae.java:146 |
| `Cf287b9a5.opkind25447(s, false)` | returns `s.length()+1`. It does not write. | builds/rs2/695-2011-12-22/src/src/main/java/deob/Cf287b9a5.java:7 |
| `C59fcb984.opkind24403` | takes the length of `NbFd.opkind30721` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/NbFd.java:84`, stub). Writes u8 length when that length is below 128, otherwise u16be of `32768+length` when the length is below 32768. Then `XvAp.opkind20728` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/XvAp.java:18`, stub) copies bytes. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C59fcb984.java:26 |
| `FhFh.opkind26384` | backpatches a u16be at `offset - length - 2`. It does not append. Guard `-31735`. | builds/rs2/695-2011-12-22/src/src/main/java/deob/FhFh.java:9 |
| `C4c84121b.opkind27432` | when the byte guard is `<= -114`, backpatches one byte at `offset - length - 1`. | builds/rs2/695-2011-12-22/src/src/main/java/deob/C4c84121b.java:20 |

`jstr` in `packets.md` means `C743384ae.opkind20987` with the boolean true: unread `Awd.opkind30402` bytes followed by a written u8 0. `len` means `String.length()`. A u8 length taken from `Cf287b9a5.opkind25447(s, false)` is `s.length()+1`, which is the Java char count plus one, computed before the encoder runs.
