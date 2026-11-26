# Gaps

`implementable` means a later pass can finish the row from this client. `blocked` means the source is missing. No file in [files.tsv](files.tsv) is `unread`. Every `.java` file under the source root was opened for its class and method names, and the behavior files were read for the fields cited in the other pages.

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| appearance bytes inside player mask `0x10` | `rt4/JavaAudioChannel.java:170` `opJavaAudioChannel15` | method body unread | implementable |
| Huffman tail of S2C 35, 57, 117 and player mask `0x80` | `deob/PleaseEnterUsername.java:68` `opkind2424` | method body unread | implementable |
| S2C 238 reflection request | `deob/SieBefindenSichEinemMitgliederGebiet.java` `opkind2261` | called at `rt4/JavaMouseWheel.java:214`. Body unread | implementable |
| S2C 241 close | `rt4/EnumStringEntry.java` `opEnumStringEntry8` | called at `rt4/JavaMouseWheel.java:206`. Body unread | implementable |
| S2C 151 | `deob/TextureOpCurve.java:107` | size 6 and no switch arm. The handler throws `T1` at `rt4/JavaMouseWheel.java:1524` | implementable |
| C2S opcodes with an empty field list | `deob/HierWechseln.java:177` `opkind2465`, `deob/TextureOpCurve.java:113` `opTextureOpCurve18`, and the other write sites named in packets.md | opcode byte was read. The following writes were not | implementable |
| widget script arrays | `deob/NoReplyLoginserver.java:914` `opkind2479` and `opkind24711` | methods unread | implementable |
| second widget decoder | `deob/NoReplyLoginserver.java` `opkind2478` | method unread | implementable |
| client script interpreter | `deob/TextureOpCurve.java:113` `opTextureOpCurve18` | method unread | implementable |
| config decode opcodes | index 2 group readers listed in cache.md | each decoder's opcode loop was not read | implementable |
| terrain tile decoder and XTEA apply | `rt4/TextureOp35.java:55` passes keys to `SoftwareFont.call4bdb2339`. `opByteArrayNodeSecondary70` is the cipher | the method that calls the cipher, and the per-tile terrain walk, were not read | implementable |
| reconnect code 15 | `rt4/MouseRecorder.java` `call15a13947` | called at `rt4/TextureOp29SubOp.java:243`. Body unread | implementable |
| login failure strings | `rt4/Scenery.java:35` | codes are listed. The three string fields per code were not opened | implementable |
| audio line constructor and vorbis playback | `rt4/ReflectionCheck.java:152`, `rt4/VorbisSound.java`, `rt4/ClientScript.java` `call273a5669` | call sites are in sound.md. Bodies unread | implementable |
| software raster internals | `rt4/TextureOp19.java` `opTextureOp197` | only the state-30 call at `rt4/Client.java:740` was read | implementable |
| RSLib C2S sizes | `rslib/src/client/packets/474.c` | this client has no C2S size array. Do not copy that table | implementable |
| RSLib region encoder | `rslib/src/both/move/rev_474.c:26` | the comment says `rs_region_encode_474` swaps the two locals. Java reads local z first at `rt4/ByteArrayNodeSecondary.java:107` and local x at line 119 | implementable |
| RSLib appearance layout | `rslib/src/both/move/rev_474.c:13` | the comment describes `opJavaAudioChannel15`. That method was not read, so the comment is not a field list | implementable |
| HD classes `uc`, `je`, `mc`, `jb`, `hf` | `rt4/VertexNormal.java:53`, `rt4/ReflectionCheck.java:157`, `rt4/QuickChatPhrase.java:47`, `rt4/MaterialManager_Class106_Sub1.java:52`, `rt4/Scenery.java:139` | `Class.forName` targets are not in this source tree. No `jaggl`, `jagdx`, or `jagtheora` files are present | blocked no-source |
