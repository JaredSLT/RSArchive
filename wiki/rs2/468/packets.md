# Packets

Server sizes are `RawModel.valueRawModel23` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/RawModel.java:43`). Client packets have no size array in this client; the size column says `none`. An empty field list means the handler body was not read. Width names are the buffer methods in [framing.md](framing.md).

The same opcode number can appear in both directions. Server 119 is 24 raw bytes. Client 119 is the write in `TextureOp19`. Server 89 was not read. Client 89 is the chat write in [input.md](input.md).

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 6 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:651` |
| c2s | 9 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:344` |
| c2s | 10 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:361` |
| c2s | 13 | none |  | TextureOp19 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp19.java:397` |
| c2s | 23 | none |  | TextureOp38 and SoftReferenceNode | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp38.java:78` |
| c2s | 24 | none | see input.md | MonochromeImageCache.opMonochromeImageCache13 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonochromeImageCache.java:360` |
| c2s | 27 | none |  | MaterialManager_Class106_Sub1 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialManager_Class106_Sub1.java:1609` |
| c2s | 28 | none |  | JString | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/JString.java:93` |
| c2s | 29 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:177` |
| c2s | 31 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:151` |
| c2s | 32 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:273` |
| c2s | 35 | none |  | TextureOp38 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp38.java:51` |
| c2s | 36 | none |  | MaterialManager_Class106_Sub1 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialManager_Class106_Sub1.java:2056` |
| c2s | 37 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:285` |
| c2s | 38 | none |  | DelayedStateChange | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/DelayedStateChange.java:86` |
| c2s | 39 | none |  | EmpfNger | `builds/rs2/468-2007-08-08/src/src/main/java/deob/EmpfNger.java:317` |
| c2s | 43 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:339` |
| c2s | 44 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:205` |
| c2s | 45 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:604` |
| c2s | 50 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:460` |
| c2s | 56 | none |  | TextureOp19 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp19.java:291` |
| c2s | 67 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:691` |
| c2s | 69 | none |  | MaterialManager_Class106_Sub1 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialManager_Class106_Sub1.java:1410` |
| c2s | 73 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:428` |
| c2s | 75 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:485` |
| c2s | 78 | none |  | Client | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:145` |
| c2s | 80 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:224` |
| c2s | 81 | none |  | TextureOp38 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp38.java:64` |
| c2s | 86 | none |  | MaterialManager_Class106_Sub1 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialManager_Class106_Sub1.java:2064` |
| c2s | 89 | none | u8 (`opScenery6()-1`), jstr (`opScenery34(2)`) | ServerBeingUpdated | `builds/rs2/468-2007-08-08/src/src/main/java/deob/ServerBeingUpdated.java:148` |
| c2s | 96 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:467` |
| c2s | 97 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:547` |
| c2s | 98 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:139` |
| c2s | 103 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:145` |
| c2s | 108 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:103` |
| c2s | 110 | none | see input.md | MonochromeImageCache.opMonochromeImageCache13 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonochromeImageCache.java:365` |
| c2s | 112 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:753` |
| c2s | 113 | none |  | ColorImageCache | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ColorImageCache.java:61` |
| c2s | 115 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:716` |
| c2s | 117 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:475` |
| c2s | 119 | none |  | TextureOp19 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp19.java:86` |
| c2s | 120 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:404` |
| c2s | 122 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:735` |
| c2s | 130 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:610` |
| c2s | 135 | none |  | TextureOp16 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp16.java:193` |
| c2s | 137 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:709` |
| c2s | 138 | none |  | TextureOp38 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp38.java:56` |
| c2s | 143 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:115` |
| c2s | 144 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:637` |
| c2s | 149 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:800` |
| c2s | 150 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:151` |
| c2s | 153 | none |  | EmpfNger | `builds/rs2/468-2007-08-08/src/src/main/java/deob/EmpfNger.java:294` |
| c2s | 158 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:664` |
| c2s | 162 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:133` |
| c2s | 164 | none |  | MaterialManager_Class106_Sub1 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialManager_Class106_Sub1.java:1624` |
| c2s | 167 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:121` |
| c2s | 170 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:761` |
| c2s | 172 | none |  | MouseWheel | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MouseWheel.java:26` |
| c2s | 174 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:441` |
| c2s | 175 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:127` |
| c2s | 177 | none |  | TextureOp19 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp19.java:206` |
| c2s | 178 | none |  | JString | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/JString.java:189` |
| c2s | 181 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:109` |
| c2s | 182 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:505` |
| c2s | 183 | none | u8 0 placeholder, u32be; the rest of opJavaMouseWheel10 was not read | JavaMouseWheel.opJavaMouseWheel10 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/JavaMouseWheel.java:118` |
| c2s | 184 | none | see input.md | MonochromeImageCache.opMonochromeImageCache13 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonochromeImageCache.java:355` |
| c2s | 185 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:164` |
| c2s | 192 | none |  | MaterialManager_Class106_Sub1 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialManager_Class106_Sub1.java:1423` |
| c2s | 193 | none |  | TextureOp38 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp38.java:60` |
| c2s | 194 | none | u64be | JString | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/JString.java:258` |
| c2s | 195 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:587` |
| c2s | 204 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:231` |
| c2s | 205 | none | u8 stored as `128-value`, u32le high, u32le low | MonochromeImageCache.opMonochromeImageCache10 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MonochromeImageCache.java:84` |
| c2s | 208 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:495` |
| c2s | 211 | none |  | TextureOp19 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp19.java:199` |
| c2s | 214 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:783` |
| c2s | 215 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:385` |
| c2s | 216 | none |  | MaterialManager_Class106_Sub1 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/MaterialManager_Class106_Sub1.java:2048` |
| c2s | 218 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:311` |
| c2s | 221 | none |  | MapElement | `builds/rs2/468-2007-08-08/src/src/main/java/deob/MapElement.java:97` |
| c2s | 229 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:807` |
| c2s | 231 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:322` |
| c2s | 232 | none |  | EmpfNger | `builds/rs2/468-2007-08-08/src/src/main/java/deob/EmpfNger.java:339` |
| c2s | 238 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:298` |
| c2s | 239 | none |  | GlModel_Class23 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlModel_Class23.java:82` |
| c2s | 241 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:411` |
| c2s | 242 | none |  | TextureOp19 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp19.java:176` |
| c2s | 250 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:629` |
| c2s | 252 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:134` |
| c2s | 253 | none |  | ServerActiveProperties | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/ServerActiveProperties.java:775` |
| s2c | 2 | -2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1320` |
| s2c | 4 | -2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1496` |
| s2c | 5 | 6 | u16be, u32 bytes `>>16`, `>>24`, low, `>>8` | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1744` |
| s2c | 10 | 2 | u16le low+128 (65535 becomes -1) | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1386` |
| s2c | 25 | 6 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1426` |
| s2c | 26 | 5 | u16le, u8, u16le low+128 | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:116` |
| s2c | 27 | 2 | u16be low+128 | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:468` |
| s2c | 32 | 3 | u8 stored as `-value`, u16le | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:197` |
| s2c | 33 | -2 | u8+128 x origin, u8 stored as `-value` z origin, then repeated u8 sub-opcode plus that zone body | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1571` |
| s2c | 36 | -2 | bits, see sync.md | PathingEntity.opPathingEntity14 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:967` |
| s2c | 38 | 8 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:868` |
| s2c | 39 | 2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1129` |
| s2c | 41 | -2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:893` |
| s2c | 42 | -2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:531` |
| s2c | 45 | 3 | u8 stored as `128-value`, u8 stored as `-value`, u8+128 | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1734` |
| s2c | 46 | 11 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1651` |
| s2c | 51 | -2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1380` |
| s2c | 55 | 6 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1063` |
| s2c | 59 | 4 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:906` |
| s2c | 68 | -1 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:809` |
| s2c | 73 | 3 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:885` |
| s2c | 74 | -2 | bits, see sync.md | JString.opJString13 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1374` |
| s2c | 76 | 6 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1592` |
| s2c | 77 | -1 | u64be, then bytes consumed by QuickChatCommandDecoder.opQuickChatCommandDecoder1 | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:922` |
| s2c | 82 | 15 | u8, raw i8, raw i8, signed u16be, u16be, u8, u8, u16be, u16be, u8, u8 | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:302` |
| s2c | 89 | -2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:619` |
| s2c | 92 | 10 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:558` |
| s2c | 94 | -1 | u8 stored as `128-value`, u8 stored as `128-value`, jstr | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1409` |
| s2c | 99 | 5 | u16be, u8, u16be | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1203` |
| s2c | 103 | 7 | u16le, u16be low+128, u8 stored as `128-value`, u16le low+128 | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:247` |
| s2c | 105 | 4 | u8 stored as `-value`, u16be low+128, u8+128 | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:266` |
| s2c | 106 | -2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:482` |
| s2c | 108 | 2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:945` |
| s2c | 110 | 1 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:960` |
| s2c | 111 | 7 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:854` |
| s2c | 112 | 5 | u16be low+128 (65535 becomes -1), u24be | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1397` |
| s2c | 115 | 2 | u8 stored as `-value`, u8+128 | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1767` |
| s2c | 119 | 24 | bytes(24) | ServerActiveProperties.opServerActiveProperties12 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:973` |
| s2c | 120 | 6 | u8, u16be, u8, u16be | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:233` |
| s2c | 122 | 1 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:993` |
| s2c | 124 | 0 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1120` |
| s2c | 130 | 5 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1638` |
| s2c | 134 | 0 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:999` |
| s2c | 137 | 2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:953` |
| s2c | 138 | 7 | u8, u16be, u16be, u16be | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:328` |
| s2c | 142 | 6 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:930` |
| s2c | 147 | 4 | u8, u8 stored as `-value`, u16be low+128 | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:134` |
| s2c | 150 | 2 | u8+128, u8 stored as `-value` | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:221` |
| s2c | 160 | 0 | no payload | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1309` |
| s2c | 161 | 12 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1093` |
| s2c | 164 | 14 | u16le, i8 stored as `-raw`, u16le, u8 stored as `128-value`, i8 stored as `raw-128`, u16be low+128, u16be, i8 stored as `128-raw`, i8 stored as `raw-128`, u8 stored as `128-value` | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:44` |
| s2c | 169 | -2 | see region.md instanced rebuild | TextureOpCurve.opTextureOpCurve14 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1022` |
| s2c | 171 | 10 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1005` |
| s2c | 175 | 0 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:602` |
| s2c | 184 | 11 | u64be, u16be, u8 | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:735` |
| s2c | 188 | 0 | no payload | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:729` |
| s2c | 190 | 0 | no payload | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1080` |
| s2c | 195 | 5 | u8, u16be, u8, u8 | TracingException.opTracingException7 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TracingException.java:280` |
| s2c | 197 | 1 | u8 | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1584` |
| s2c | 201 | 3 | u16be low+128, raw i8 | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1249` |
| s2c | 207 | 2 | u16be low+128 | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1195` |
| s2c | 208 | 6 | u8 stored as `128-value`, u32be, u8 stored as `128-value` | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1043` |
| s2c | 209 | 6 | u16be low+128, u32be | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1226` |
| s2c | 210 | 6 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:544` |
| s2c | 212 | 4 | u32 bytes `>>16`, `>>24`, low, `>>8` | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:802` |
| s2c | 213 | 0 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1028` |
| s2c | 222 | -1 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1152` |
| s2c | 224 | -2 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1440` |
| s2c | 231 | -2 | see region.md non-instanced rebuild | TextureOpCurve.opTextureOpCurve14 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1761` |
| s2c | 232 | 6 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1263` |
| s2c | 237 | 4 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1622` |
| s2c | 238 | -1 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:639` |
| s2c | 239 | 8 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1364` |
| s2c | 244 | 6 | u16le, u32be | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1212` |
| s2c | 247 | 4 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1557` |
| s2c | 254 | 4 |  | GlFont.opGlFont5 | `builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:979` |
