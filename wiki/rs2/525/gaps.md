# Gaps

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| appearance bytes | builds/rs2/525-2008-12-15/src/src/main/java/deob/C3c7e15a6.java:158 | Java mask bit 8 reads bytes_sub128 before Environment.call8cb28dd2. RSLib look_write in rslib/src/both/move/rev_525.c writes plain bytes and always ends with a 0, so it never writes the three u16be Environment.java:415 reads when the last u8 is non-zero | implementable |
| dynamic region opcode 125 | builds/rs2/525-2008-12-15/src/src/main/java/deob/DelayedStateChange.java:453 | Java rebuilds with opcode 125 and four u32_b1b0b3b2 keys per zone (C8be0107e.java:113). RSLib rslib/src/both/move/rev_525.c comment only names opcode 93 | implementable |
| Client.opClient154 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Client.java:837 | blocked no-source: the method body is the comment Couldn't be decompiled. ProjAnim.java:405 and ProjAnim.java:646 call it | blocked |
| toolkit class rj | builds/rs2/525-2008-12-15/src/src/main/java/deob/PathingEntity_Class147.java:55 | blocked no-source: Class.forName(rj) is not a file in this tree. The catch builds Ced8d3620 | blocked |
| huffman bit tree | builds/rs2/525-2008-12-15/src/src/main/java/deob/Cd2e07e2c.java:32 | opMaterialManager_Class106_Sub2_Sub112 was not read. The call consumes smart-length compressed bytes | implementable |
| key collector calle2cbab8d | builds/rs2/525-2008-12-15/src/src/main/java/deob/Client.java:981 | unread method. The key array PathingEntity.valuePathingEntity170 is what the walk and camera packets test | implementable |
| walk kind callers | builds/rs2/525-2008-12-15/src/src/main/java/deob/MouseWheel.java:44 | Opcodes 249, 98, and 218 are kinds 0, 1, and 2. Cf46facc5.java:223, C9ce5ebca.java:206, and Ca157093a.java:309 pass the kind through. Which control uses which kind was not traced | implementable |
| in-game tick after opcode 142 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:208 | opkind4441 continues past the mouse, click, camera, focus, and hash packets. Those later writes were not transcribed | implementable |
| script runner | builds/rs2/525-2008-12-15/src/src/main/java/deob/LightType.java | LightType.opLightType23 runs the array built by S2C 144. The opcode meanings inside the runner were not read | implementable |
| software draw | builds/rs2/525-2008-12-15/src/src/main/java/deob/FluType.java:3 | FluType, TextureOpTiledSprite, and AnimFrame draw methods were not read | implementable |
| sprite name strings | builds/rs2/525-2008-12-15/src/src/main/java/deob/Light.java:54 | call6c18fa80 resolves names to group ids. The string fields passed in were not opened | implementable |
| synth and vorbis decode | builds/rs2/525-2008-12-15/src/src/main/java/deob/Client.java:1192 | The 22050 Hz startup and sound packets 115 and 138 are cited. Vorbis and synth decode methods were not read | implementable |
| config opcodes C8be0107e.opkind36019 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C8be0107e.java:615 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes C3c656823.opkind50017 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C3c656823.java:778 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes FichiersConfigChargBs.opkind4693 | builds/rs2/525-2008-12-15/src/src/main/java/deob/FichiersConfigChargBs.java:326 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes MapElementList.opMapElementList25 | builds/rs2/525-2008-12-15/src/src/main/java/deob/MapElementList.java:161 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes C4ef674d8.opkind5223 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C4ef674d8.java:433 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes C9e146a8e.opkind5083 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C9e146a8e.java:474 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes C9c57aa59.opkind3988 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C9c57aa59.java:321 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes C90b6a93f.opkind4213 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C90b6a93f.java:46 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes Ca3682c6a.opkind4561 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Ca3682c6a.java:147 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes C1b399fc7.opkind4593 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C1b399fc7.java:130 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes C4832e8d5.opkind4074 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C4832e8d5.java:175 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes C98d1be1a.opkind5216 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:835 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes Cc2a456cb.opkind5493 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Cc2a456cb.java:83 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes WallDecor.calldd94acee | builds/rs2/525-2008-12-15/src/src/main/java/deob/WallDecor.java:140 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes WwwWtqa.opkind4345 | builds/rs2/525-2008-12-15/src/src/main/java/deob/WwwWtqa.java:116 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes TextureOp11.opTextureOp1111 | builds/rs2/525-2008-12-15/src/src/main/java/deob/TextureOp11.java:89 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes FrameBuffer.opFrameBuffer8 | builds/rs2/525-2008-12-15/src/src/main/java/deob/FrameBuffer.java:27 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes Cfd4a3c76.opkind4145 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Cfd4a3c76.java:49 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| config opcodes ChargementDesPolices.opkind5767 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ChargementDesPolices.java:149 | the u8-until-0 loop is located. Opcode field widths were not transcribed | implementable |
| c2s opcode 2 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:68 | writer not transcribed | implementable |
| c2s opcode 9 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:259 | writer not transcribed | implementable |
| c2s opcode 10 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:162 | writer not transcribed | implementable |
| c2s opcode 23 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:104 | writer not transcribed | implementable |
| c2s opcode 24 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1543 | writer not transcribed | implementable |
| c2s opcode 25 | builds/rs2/525-2008-12-15/src/src/main/java/deob/RunescapeCom.java:56 | writer not transcribed | implementable |
| c2s opcode 26 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Cee767227.java:63 | writer not transcribed | implementable |
| c2s opcode 29 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:503 | writer not transcribed (2 sites) | implementable |
| c2s opcode 32 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Ceea51ef6.java:174 | writer not transcribed | implementable |
| c2s opcode 34 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:706 | writer not transcribed | implementable |
| c2s opcode 37 | builds/rs2/525-2008-12-15/src/src/main/java/deob/WallDecor.java:124 | writer not transcribed | implementable |
| c2s opcode 40 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:621 | writer not transcribed | implementable |
| c2s opcode 45 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1558 | writer not transcribed | implementable |
| c2s opcode 50 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ConnexionPerdue.java:184 | writer not transcribed | implementable |
| c2s opcode 51 | builds/rs2/525-2008-12-15/src/src/main/java/deob/WallDecor.java:132 | writer not transcribed | implementable |
| c2s opcode 56 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:414 | writer not transcribed | implementable |
| c2s opcode 59 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:602 | writer not transcribed (2 sites) | implementable |
| c2s opcode 60 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:486 | writer not transcribed | implementable |
| c2s opcode 70 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:277 | writer not transcribed | implementable |
| c2s opcode 75 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:93 | writer not transcribed | implementable |
| c2s opcode 78 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:686 | writer not transcribed | implementable |
| c2s opcode 85 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:184 | writer not transcribed | implementable |
| c2s opcode 89 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C5a9c80ea.java:18 | writer not transcribed | implementable |
| c2s opcode 90 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:496 | writer not transcribed | implementable |
| c2s opcode 91 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C68af3879.java:84 | writer not transcribed | implementable |
| c2s opcode 93 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:589 | writer not transcribed (2 sites) | implementable |
| c2s opcode 96 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SynthInstrument.java:47 | writer not transcribed | implementable |
| c2s opcode 99 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:657 | writer not transcribed | implementable |
| c2s opcode 102 | builds/rs2/525-2008-12-15/src/src/main/java/deob/FrameBuffer.java:247 | writer not transcribed | implementable |
| c2s opcode 103 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:61 | writer not transcribed | implementable |
| c2s opcode 107 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:86 | writer not transcribed | implementable |
| c2s opcode 114 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:798 | writer not transcribed | implementable |
| c2s opcode 115 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1349 | writer not transcribed | implementable |
| c2s opcode 116 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:562 | writer not transcribed | implementable |
| c2s opcode 117 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:110 | writer not transcribed | implementable |
| c2s opcode 118 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:391 | writer not transcribed | implementable |
| c2s opcode 119 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:134 | writer not transcribed (2 sites) | implementable |
| c2s opcode 120 | builds/rs2/525-2008-12-15/src/src/main/java/deob/TextureOp25.java:10 | writer not transcribed | implementable |
| c2s opcode 126 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:743 | writer not transcribed | implementable |
| c2s opcode 130 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:428 | writer not transcribed | implementable |
| c2s opcode 132 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:80 | writer not transcribed | implementable |
| c2s opcode 135 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:640 | writer not transcribed (2 sites) | implementable |
| c2s opcode 143 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:2746 | writer not transcribed | implementable |
| c2s opcode 144 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:856 | writer not transcribed | implementable |
| c2s opcode 147 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:86 | writer not transcribed | implementable |
| c2s opcode 149 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:249 | writer not transcribed | implementable |
| c2s opcode 150 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:331 | writer not transcribed | implementable |
| c2s opcode 151 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:543 | writer not transcribed | implementable |
| c2s opcode 158 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:864 | writer not transcribed | implementable |
| c2s opcode 161 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:98 | writer not transcribed | implementable |
| c2s opcode 164 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ObjType.java:49 | writer not transcribed (2 sites) | implementable |
| c2s opcode 165 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:62 | writer not transcribed | implementable |
| c2s opcode 166 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:92 | writer not transcribed | implementable |
| c2s opcode 167 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C90133226.java:27 | writer not transcribed | implementable |
| c2s opcode 170 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:347 | writer not transcribed | implementable |
| c2s opcode 171 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:775 | writer not transcribed | implementable |
| c2s opcode 175 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:294 | writer not transcribed | implementable |
| c2s opcode 177 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:147 | writer not transcribed | implementable |
| c2s opcode 178 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:74 | writer not transcribed | implementable |
| c2s opcode 179 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C8876ac7c.java:89 | writer not transcribed | implementable |
| c2s opcode 184 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:513 | writer not transcribed (2 sites) | implementable |
| c2s opcode 187 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:765 | writer not transcribed | implementable |
| c2s opcode 191 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:168 | writer not transcribed | implementable |
| c2s opcode 196 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:528 | writer not transcribed | implementable |
| c2s opcode 199 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:909 | writer not transcribed | implementable |
| c2s opcode 200 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:470 | writer not transcribed | implementable |
| c2s opcode 202 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Cc33039b3.java:155 | writer not transcribed | implementable |
| c2s opcode 203 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:615 | writer not transcribed | implementable |
| c2s opcode 204 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:494 | writer not transcribed (2 sites) | implementable |
| c2s opcode 205 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:75 | writer not transcribed (2 sites) | implementable |
| c2s opcode 207 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1336 | writer not transcribed | implementable |
| c2s opcode 209 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:576 | writer not transcribed | implementable |
| c2s opcode 211 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:570 | writer not transcribed | implementable |
| c2s opcode 213 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1720 | writer not transcribed (2 sites) | implementable |
| c2s opcode 219 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:1733 | writer not transcribed | implementable |
| c2s opcode 221 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Ceea51ef6.java:149 | writer not transcribed | implementable |
| c2s opcode 223 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C0fa1de35.java:116 | writer not transcribed | implementable |
| c2s opcode 228 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:646 | writer not transcribed | implementable |
| c2s opcode 230 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:753 | writer not transcribed | implementable |
| c2s opcode 231 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:363 | writer not transcribed | implementable |
| c2s opcode 232 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:220 | writer not transcribed | implementable |
| c2s opcode 236 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:234 | writer not transcribed | implementable |
| c2s opcode 237 | builds/rs2/525-2008-12-15/src/src/main/java/deob/Caadef800.java:10 | writer not transcribed (2 sites) | implementable |
| c2s opcode 239 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:197 | writer not transcribed (3 sites) | implementable |
| c2s opcode 240 | builds/rs2/525-2008-12-15/src/src/main/java/deob/SoftwareFont.java:872 | writer not transcribed | implementable |
| c2s opcode 242 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:476 | writer not transcribed | implementable |
| c2s opcode 244 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:270 | writer not transcribed | implementable |
| c2s opcode 246 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:713 | writer not transcribed | implementable |
| c2s opcode 248 | builds/rs2/525-2008-12-15/src/src/main/java/deob/C98d1be1a.java:678 | writer not transcribed | implementable |
| c2s opcode 255 | builds/rs2/525-2008-12-15/src/src/main/java/deob/ExpiresThuJanGmtMaxAge.java:413 | writer not transcribed | implementable |
