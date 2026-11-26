# Gaps

An empty field list in [packets.md](packets.md) is one of these rows. The source file was opened far enough to name the method. The payload bytes were not read. A later pass can fill them from that method.

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| C2S opcode 0 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:169` | `opkind1235` writes the opcode. The following puts were not read | implementable |
| C2S opcode 11 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:584` | same method, also line 932. The two sites were not compared | implementable |
| C2S opcode 21 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/MaterialManager_Class106_Sub1.java:451` | `opMaterialManager_Class106_Sub111` writes the opcode. The puts were not read | implementable |
| C2S opcode 29 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:616` | `opkind1235` payload not read | implementable |
| C2S opcode 36 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:887` | `opkind1235` payload not read | implementable |
| C2S opcode 38 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:338` | `opkind1235` payload not read | implementable |
| C2S opcode 46 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/SoundPcmStream.java:126` | `opSoundPcmStream1` payload not read | implementable |
| C2S opcode 47 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:672` | `opkind1235` payload not read | implementable |
| C2S opcode 48 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:864` | `opkind1235` payload not read | implementable |
| C2S opcode 54 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:535` | also lines 725 and 739. The sites were not compared | implementable |
| C2S opcode 64 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:607` | `opkind1235` payload not read | implementable |
| C2S opcode 69 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:346` | `opkind1235` payload not read | implementable |
| C2S opcode 70 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/QuickChatCatType.java:75` | `opQuickChatCatType6` payload not read | implementable |
| C2S opcode 78 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:99` | `opkind1235` payload not read | implementable |
| C2S opcode 85 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:485` | `opkind1235` payload not read | implementable |
| C2S opcode 87 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:686` | `opkind1235` payload not read | implementable |
| C2S opcode 89 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:699` | `opkind1235` payload not read | implementable |
| C2S opcode 101 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:579` | also line 712. The sites were not compared | implementable |
| C2S opcode 104 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:375` | `opkind1235` payload not read | implementable |
| C2S opcode 113 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:788` | `opkind1235` payload not read | implementable |
| C2S opcode 114 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:282` | `opkind1235` payload not read | implementable |
| C2S opcode 118 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/BitteVersuchenSieEs.java:87` | `opkind1971` payload not read | implementable |
| C2S opcode 119 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/BitteVersuchenSieEs.java:98` | `opkind1971` payload not read | implementable |
| C2S opcode 120 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:92` | `opkind1235` payload not read | implementable |
| C2S opcode 136 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:828` | `opkind1235` payload not read | implementable |
| C2S opcode 141 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/MaterialManager_Class106_Sub1.java:424` | `opMaterialManager_Class106_Sub111` payload not read | implementable |
| C2S opcode 144 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:187` | `opkind1235` payload not read | implementable |
| C2S opcode 145 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:839` | `opkind1235` payload not read | implementable |
| C2S opcode 146 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:251` | `opkind1235` payload not read | implementable |
| C2S opcode 147 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:649` | `opkind1235` payload not read | implementable |
| C2S opcode 149 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:123` | `opkind1235` payload not read | implementable |
| C2S opcode 154 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:238` | `opkind1235` payload not read | implementable |
| C2S opcode 158 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:559` | `opkind1235` payload not read | implementable |
| C2S opcode 159 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/PleaseTryAgain.java:35` | `opkind1621` payload not read | implementable |
| C2S opcode 169 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:782` | `opkind1235` payload not read | implementable |
| C2S opcode 171 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:292` | `opkind1235` payload not read | implementable |
| C2S opcode 178 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:443` | `opkind1235` payload not read | implementable |
| C2S opcode 182 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:73` | `opkind1235` payload not read | implementable |
| C2S opcode 188 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:310` | `opkind1235` payload not read | implementable |
| C2S opcode 195 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:384` | `opkind1235` payload not read | implementable |
| C2S opcode 198 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/SpielFensterGeladen.java:148` | `opkind1475` payload not read | implementable |
| C2S opcode 200 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:360` | `opkind1235` payload not read | implementable |
| C2S opcode 202 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:436` | `opkind1235` payload not read | implementable |
| C2S opcode 206 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:503` | `opkind1235` payload not read | implementable |
| C2S opcode 219 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:398` | also line 757. The sites were not compared | implementable |
| C2S opcode 221 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:417` | `opkind1235` payload not read | implementable |
| C2S opcode 222 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:267` | `opkind1235` payload not read | implementable |
| C2S opcode 224 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:769` | `opkind1235` payload not read | implementable |
| C2S opcode 229 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:919` | `opkind1235` payload not read | implementable |
| C2S opcode 236 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:641` | `opkind1235` payload not read | implementable |
| C2S opcode 243 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:892` | `opkind1235` payload not read | implementable |
| C2S opcode 245 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:732` | `opkind1235` payload not read | implementable |
| C2S opcode 250 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/ShapedTile.java:69` | `opShapedTile1` payload not read | implementable |
| C2S opcode 252 payload | `builds/rs2/443-2006-12-11/src/src/main/java/deob/LadeFreundeListe.java:162` | `opkind1235` payload not read | implementable |
| login failure strings | `builds/rs2/443-2006-12-11/src/src/main/java/deob/WirVermutenDassJemandIhrPasswortKennt.java:61` | codes -3, -2, -1, 4, 5, 6, 7, 8, 10, 11, 12, 13, 16, 17, 18, 19, 20, 22, 25, 26, 27, and the default branch select fields whose literals were not read | implementable |
| config decode opcodes | `builds/rs2/443-2006-12-11/src/src/main/java/deob/AddIgnore.java:1` | obj, npc, loc, seq, and spotanim opcode lists were not read. Group ids were not copied from another revision | implementable |
| widget type bodies | `builds/rs2/443-2006-12-11/src/src/main/java/deob/PacketBit.java:313` | type-specific reads after the shared header, and `opPacketBit17`, were not transcribed. The new-format reads stop at `PacketBit.java:549` | implementable |
| archive decompress | `builds/rs2/443-2006-12-11/src/src/main/java/deob/AufDerHautpseite.java:479` | `SoftReferenceNode.opSoftReferenceNode4` and `MidiPcmStream.opMidiPcmStream2` are called by the reference-table loader and were not read | implementable |
| map key pairing | `builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:455` | state 25 calls `MaterialManager_Class106_Sub1.opMaterialManager_Class106_Sub111`. The method that pairs each stored XTEA key with the `m` and `l` groups was not read | implementable |
| js5 name hash | `builds/rs2/443-2006-12-11/src/src/main/java/deob/AufDerHautpseite.java:469` | `opkind15425` resolves a group name and a file name through `ReferenceNode.opReferenceNode3`. That hash was not read. The huffman names are the literals `huffman` and `""` | implementable |
| MIDI clock body | `builds/rs2/443-2006-12-11/src/src/main/java/deob/Client.java:434` | `TextureOp38.opTextureOp386` is called every tick. Its body was not read | implementable |
| low-memory flag | `builds/rs2/443-2006-12-11/src/src/main/java/deob/ChangeLocRequest.java:37` | region rebuild forces plane 0 when `SpotAnimType.valueSpotAnimType63` is false. Which startup path sets that flag was not read | implementable |
| terrain and model draw | `builds/rs2/443-2006-12-11/src/src/main/java/deob/MaterialManager_Class106_Sub2.java:792` | `callf04f6806` and `Model.java` were not field-listed | implementable |
| RSLib minimap tail | `rslib/src/both/move/rev_443.c:499` | the comment says minimap callers add 14 bytes the encoder does not write. This client writes those bytes at `builds/rs2/443-2006-12-11/src/src/main/java/deob/Cb5815139.java:368` and includes them in the opcode 80 length | implementable |
| RSLib opcode 81 size | `rslib/src/both/move/rev_443.c:506` | `k_c2s_443` stores 8 for opcode 81. This client writes the variable walk layout at `builds/rs2/443-2006-12-11/src/src/main/java/deob/Keyboard.java:332` | implementable |
| RSLib player masks | `rslib/src/both/move/rev_443.c:376` | `player_decode_443` accepts mask bits 0x40 and 0x80. This client reads the full mask set at `builds/rs2/443-2006-12-11/src/src/main/java/deob/Cd66098f9.java:349` | implementable |
