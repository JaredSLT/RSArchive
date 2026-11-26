# Packets

One row per opcode this client constructs. S2C sizes come from `CameraEffectType`. C2S sizes come from the `FloorUnderlayType` constructor argument, which the object does not store. An empty field list means that handler was not read. Width names are the `Unused6` methods cited in `framing.md` and `login.md`.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 0 | 8 | u16be(lo-128), u16be(lo-128), u32be | GpuRelated14.opGpuRelated144 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:383 |
| s2c | 1 | 0 |  | valuekind10418.opRendererInfo4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:995 |
| s2c | 2 | -1 |  | unread SynthFilter.opSynthFilter7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:348 |
| s2c | 3 | 3 | u8(128-b), u16be | valuekind10418.opRendererInfo11 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1974 |
| s2c | 4 | -1 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:216 |
| s2c | 5 | -2 | u8, bytes(len-1) | ObjIconFactory.opObjIconFactory2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:407 |
| s2c | 6 | 3 | u8, s8(128-b), s8 | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1868 |
| s2c | 7 | 8 | u16be, u8, u8, u8, u8, u16be | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:587 |
| s2c | 8 | -2 | u16be, u16(lo-128,hi), jstr | PureJavaDepthBuffer.opPureJavaDepthBuffer4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2141 |
| s2c | 9 | 14 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:583 |
| s2c | 10 | -2 | u16(lo-128,hi) zoneZ, u8(b-128) mapSize, u8 force==1, u16le zoneX, then 4*u32be XTEA per map square. See region.md | C0bd9f077.opkind11111 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C0bd9f077.java:8 |
| s2c | 11 | 2 | s16be | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1043 |
| s2c | 12 | 0 |  | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:695 |
| s2c | 13 | 0 |  | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1030 |
| s2c | 14 | 8 | u16be, u8, u8, u16be, u8, u8 | DaaInit.opDaaInit1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:370 |
| s2c | 15 | 3 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1644 |
| s2c | 16 | 12 | u16be, u16be, u16(lo-128,hi), u16le, u32_badc | Whirlpool.opWhirlpool2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1340 |
| s2c | 17 | 2 | u8(128-b), u8 | Cd0dffacd.opkind10967 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1477 |
| s2c | 18 | 3 | u8(b-128), u16be(lo-128) | C67590275.opkind11103 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:489 |
| s2c | 19 | 14 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1292 |
| s2c | 20 | -1 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1048 |
| s2c | 21 | 7 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1064 |
| s2c | 22 | -1 |  | unread LoadingScreenNewsManager.opLoadingScreenNewsManager3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1903 |
| s2c | 23 | 1 | u8 | PlayerVarProvider.opPlayerVarProvider3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1178 |
| s2c | 24 | -1 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:254 |
| s2c | 25 | 6 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1864 |
| s2c | 26 | -2 | u8(128-b) mapSize, u8neg force==1, u16be zoneZ, u16be zoneX, u8neg, then a 1-bit grid of 26-bit region ids, then 4*u32be XTEA. See region.md | GpuRelated13.opGpuRelated133 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuRelated13.java:55 |
| s2c | 27 | 8 |  | unread TextureOp29SubOp2.opTextureOp29SubOp222 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:950 |
| s2c | 28 | 10 | u16be(lo-128), u16le, u32_badc, u16(lo-128,hi) | ClientDynamicProvider.opClientDynamicProvider5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1604 |
| s2c | 29 | -1 |  | unread LoadingScreenNewsManager.opLoadingScreenNewsManager3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1411 |
| s2c | 30 | -1 |  | unread valuekind10988.opUtf82 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:359 |
| s2c | 31 | -1 |  | unread Matrix4x4.opMatrix4x43 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2112 |
| s2c | 32 | 8 | u16(lo-128,hi), u32_cdab, u16be(lo-128) | Whirlpool.opWhirlpool2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2028 |
| s2c | 33 | 1 | u8 | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1407 |
| s2c | 34 | -1 |  | unread LoadingScreenNewsManager.opLoadingScreenNewsManager3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1000 |
| s2c | 35 | 7 | u16le, u32le, u8 | DxIndexBuffer.opDxIndexBuffer3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:657 |
| s2c | 36 | 20 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:964 |
| s2c | 37 | -2 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:497 |
| s2c | 38 | 4 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1361 |
| s2c | 39 | 6 | u32_badc, u16le | Whirlpool.opWhirlpool2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:941 |
| s2c | 40 | 10 | u32_badc, u16be(lo-128), s16le, s16be | DbaInit.opDbaInit2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2004 |
| s2c | 41 | 0 |  | Browser.opBrowser2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1958 |
| s2c | 42 | -2 | existing NPCs, new NPCs with 8-bit signed dx/dz until bits(15)==32767, then masks. See sync.md | C67590275.call59622295 | builds/rs2/614-2010-07-13/src/src/main/java/deob/C67590275.java:8 |
| s2c | 43 | 4 | u8(b-128), u8(128-b), u16le | SkyDecorTypeList.opSkyDecorTypeList6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:467 |
| s2c | 44 | 6 | u16be id (65535=-1), u8, u16be, u8 | LoadableResourceManager.opLoadableResourceManager2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1962 |
| s2c | 45 | -2 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2086 |
| s2c | 46 | -2 | u32be, jstr, u16be | BuildAreaSize.opBuildAreaSize12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1145 |
| s2c | 47 | 4 | s24be, u8 | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1072 |
| s2c | 48 | -2 |  | unread Occluder.opOccluder3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1516 |
| s2c | 49 | -1 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2155 |
| s2c | 50 | -1 |  | unread LoadingScreenNewsManager.opLoadingScreenNewsManager3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1087 |
| s2c | 51 | 11 | u16be, u16le, u8(128-b), u32_cdab, u16be(lo-128) | valueUnused92.opPrimaryLayerEntityPredicate6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1648 |
| s2c | 52 | 8 | u32_cdab, u16be(lo-128), u16(lo-128,hi) | BZip2State.opBZip2State19 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1994 |
| s2c | 53 | 3 | s8, u8(128-b), s8neg | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:667 |
| s2c | 54 | 3 |  | unread valuekind10418.opRendererInfo7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:651 |
| s2c | 55 | 28 |  | unread GpuRendererRelated6.opGpuRendererRelated64 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1352 |
| s2c | 56 | 6 | u16le, u16(lo-128,hi), u16be | Js5Index.opJs5Index21 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1980 |
| s2c | 57 | 3 | u8(128-b), u16be | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2019 |
| s2c | 58 | -1 |  | unread LoadingScreenNewsManager.opLoadingScreenNewsManager3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:699 |
| s2c | 59 | 12 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:519 |
| s2c | 60 | 6 |  | unread VaaInit.opVaaInit12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1488 |
| s2c | 61 | 6 | u16le, u32_cdab | Whirlpool.opWhirlpool2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:619 |
| s2c | 62 | 5 | u16le, u16be, u8 | valueUnused92.opPrimaryLayerEntityPredicate6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:451 |
| s2c | 63 | -2 |  | unread CameraTrackableType.opCameraTrackableType3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1504 |
| s2c | 64 | -2 | player presence bits, movement, then a mask of u8 extended by bit 32 and bit 32768. See sync.md | KeaInit.opKeaInit2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/KeaInit.java:17 |
| s2c | 65 | 3 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1174 |
| s2c | 66 | 10 | u32_badc, u16(lo-128,hi), u32le | valueClanSettings2.opPrimaryLayerEntityPredicate6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1258 |
| s2c | 67 | -2 |  | unread Stream.opStream2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:855 |
| s2c | 68 | 12 |  | unread SampleRateConverter.opSampleRateConverter5 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1586 |
| s2c | 69 | 2 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1990 |
| s2c | 70 | 6 | u16be, u32le | valuekind10418.opRendererInfo11 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:604 |
| s2c | 71 | 12 | u32_badc, u16(lo-128,hi), u32le, u16(lo-128,hi) | Whirlpool.opWhirlpool2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:930 |
| s2c | 72 | 7 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:926 |
| s2c | 73 | 0 |  | MapElementTypeList.opMapElementTypeList2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1857 |
| s2c | 74 | 0 |  | Browser.opBrowser2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:610 |
| s2c | 75 | 8 | u16be, u8, u8, u16be, u8, u8 | MidiDecoder.opMidiDecoder120 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:231 |
| s2c | 76 | 17 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:628 |
| s2c | 77 | 7 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2108 |
| s2c | 78 | -2 | existing NPCs, new NPCs with 5-bit signed dx/dz until bits(15)==32767, then masks. See sync.md | C67590275.call59622295 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:344 |
| s2c | 79 | 6 |  | unread VaaInit.opVaaInit12 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1246 |
| s2c | 80 | 5 | u16be(lo-128), u16be, s8(b-128) | BZip2State.opBZip2State19 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2176 |
| s2c | 81 | -2 |  | unread GpuRendererRelated6.opGpuRendererRelated65 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1369 |
| s2c | 82 | 5 | u16le, u16be(lo-128) interfaceId, u8 mode | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1615 |
| s2c | 83 | 6 | u16be, u32be | valueClanSettings2.opPrimaryLayerEntityPredicate6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:634 |
| s2c | 84 | 16 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:366 |
| s2c | 85 | 9 |  | unread valueClanSettings2.opPrimaryLayerEntityPredicate6 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1155 |
| s2c | 86 | 6 |  | unread valuekind10418.opRendererInfo7 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1334 |
| s2c | 87 | 6 | u8neg, u8neg, u32be | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1389 |
| s2c | 88 | 8 |  | unread PositionMode.opPositionMode2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1135 |
| s2c | 89 | 4 | u32be | valueLoadingScreenElementFactory2.opAnimationNode43 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:614 |
| s2c | 90 | 8 |  | unread ClientScriptState.opClientScriptState2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:244 |
| s2c | 91 | 4 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:393 |
| s2c | 92 | 5 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1365 |
| s2c | 93 | 8 | u32be, u16be, u16(lo-128,hi) | C0bd9f077.opkind11112 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1182 |
| s2c | 94 | 2 | u8neg, u8 | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:462 |
| s2c | 95 | -2 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:673 |
| s2c | 96 | 12 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1210 |
| s2c | 97 | 1 | u8 | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:354 |
| s2c | 98 | 6 |  | unread EntityChatLine.opEntityChatLine1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:478 |
| s2c | 99 | 7 | u8(128-b), u32_cdab, u16be(lo-128) | MainLoadingScreen.opMainLoadingScreen1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:976 |
| s2c | 100 | -1 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:414 |
| s2c | 101 | -1 | jstr, u16be | valueBuildAreaSize70.call254c0adc | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2077 |
| s2c | 102 | 8 |  | unread Whirlpool.opWhirlpool2 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1196 |
| s2c | 103 | 2 | u16be | Cda67a3af.opkind10861 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1895 |
| s2c | 104 | 12 |  | unread JdaInit.opJdaInit3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1574 |
| s2c | 105 | 2 |  | unread GameShellEnvironment.opGameShellEnvironment3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:397 |
| s2c | 106 | 6 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2151 |
| s2c | 107 | -1 | isaac_bytes(len) | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:226 |
| s2c | 108 | 19 |  | unread SampleRateConverter.opSampleRateConverter1 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1500 |
| s2c | 109 | 0 |  | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:850 |
| s2c | 110 | -1 | jstr | JReZCu.callca3846ee | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1068 |
| s2c | 111 | -1 |  | unread LoadingScreenNewsManager.opLoadingScreenNewsManager3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1520 |
| s2c | 112 | -1 |  | unread LoadingScreenNewsManager.opLoadingScreenNewsManager3 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:2042 |
| s2c | 113 | -2 |  | unread GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:746 |
| s2c | 114 | -1 | jstr, u16(lo-128,hi), u16be(lo-128) | PureJavaDepthBuffer.opPureJavaDepthBuffer4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1077 |
| s2c | 115 | 2 | u16be | GlFloorModelRelated5.opGlFloorModelRelated53 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1636 |
| s2c | 116 | 0 |  | GlModelRelated5.opGlModelRelated52 | builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelRelated5.java:1171 |
| c2s | 0 | 0 |  | FullscreenMode.call0d0697f8 | builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:2310 |
| c2s | 1 | 7 | u16(lo+128, hi), u16(lo+128, hi), u8(v+128), u16(hi, lo+128) | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:180 |
| c2s | 2 | 2 | u16be | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:158 |
| c2s | 3 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/OggTheoraStreamRelated.java:6 |
| c2s | 4 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/BgaInit.java:9 |
| c2s | 5 | 4 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/SpotAnimType.java:6 |
| c2s | 6 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/LookatMode.java:9 |
| c2s | 7 | 18 | the opcode 26 body, then u8, u8, u16be, u8, u8, u8, u8, u16be, u16be, u8 | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:252 |
| c2s | 8 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/Entity.java:4 |
| c2s | 9 | 15 | u16(lo+128, hi), u16(hi, lo+128), u16be, u16be, u8, u32be, u16be | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:193 |
| c2s | 10 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/QaaToString.java:6 |
| c2s | 11 | 7 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/ShaderRelated.java:4 |
| c2s | 12 | 7 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/CutsceneRoute.java:7 |
| c2s | 13 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5FileResourceLoader.java:7 |
| c2s | 14 | 4 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraTrackableType.java:7 |
| c2s | 15 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cf9df8543.java:5 |
| c2s | 16 | 16 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/HbaFinalize.java:4 |
| c2s | 17 | 0 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/ColourUtils.java:8 |
| c2s | 18 | 7 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/ClientOptionsParticles.java:7 |
| c2s | 19 | 2 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/FdaInit.java:11 |
| c2s | 20 | 2 | u16be | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:209 |
| c2s | 21 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/GraphicsRelated.java:7 |
| c2s | 22 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/Cf951faab.java:6 |
| c2s | 23 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/RawCpuInfo.java:6 |
| c2s | 24 | 4 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/MiniMenu.java:4 |
| c2s | 25 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/CacheKey.java:10 |
| c2s | 26 | 5 | u8(v+128) run, u16le (firstCoord+baseZ), u16(hi, lo+128) (baseX+secondCoord) | KeyHeldBinding.opKeyHeldBinding4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/KeyHeldBinding.java:47 |
| c2s | 27 | 16 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/ClientOptionsLightingDetail.java:15 |
| c2s | 28 | 4 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/GraphicsDeletable.java:4 |
| c2s | 29 | 7 | u16le, u16le, u8(128-v), u16(hi, lo+128) | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:106 |
| c2s | 30 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/StaticPointLight.java:23 |
| c2s | 31 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/SpriteShader.java:10 |
| c2s | 32 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/AnimFrame.java:5 |
| c2s | 33 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoadableResourceManager.java:6 |
| c2s | 34 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/OcclusionSomething.java:4 |
| c2s | 35 | 7 | u16le, u16(lo+128, hi), u8(128-v), u16be | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:167 |
| c2s | 36 | 6 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoadingScreen.java:6 |
| c2s | 37 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraTrackableType.java:5 |
| c2s | 38 | 2 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/Js5ArchiveResourceLoader.java:4 |
| c2s | 39 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused17.java:6 |
| c2s | 40 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/QfaInit.java:5 |
| c2s | 41 | 4 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/C7ef06aa1.java:7 |
| c2s | 42 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/RayTracing.java:8 |
| c2s | 43 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/C59796a6b.java:5 |
| c2s | 44 | 3 | u16(hi, lo+128), u8(v+128) | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:227 |
| c2s | 45 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/RendererToolkit.java:4 |
| c2s | 46 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/C234869d1.java:7 |
| c2s | 47 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/MsiType.java:4 |
| c2s | 48 | 6 | u16(lo+128, hi), u32(b0=v>>16, b1=v>>24, b2=v, b3=v>>8) | FullscreenMode.opFullscreenMode8 | builds/rs2/614-2010-07-13/src/src/main/java/deob/FullscreenMode.java:36 |
| c2s | 49 | 0 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/PathingEntity_Class147.java:7 |
| c2s | 50 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/UnblendedTileData2.java:88 |
| c2s | 51 | 4 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/BASType.java:11 |
| c2s | 52 | 7 | u16be, u16(lo+128, hi), u8(v+128), u16(lo+128, hi) | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:132 |
| c2s | 53 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitterRelated.java:6 |
| c2s | 54 | 12 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/ModelParticleEffector.java:15 |
| c2s | 55 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/KbaFinalize.java:5 |
| c2s | 56 | 4 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/SoundPcmStream.java:4 |
| c2s | 57 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/Interface.java:5 |
| c2s | 58 | 1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/SimpleToolkitChange.java:10 |
| c2s | 59 | 2 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/Base64.java:7 |
| c2s | 60 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/CreateAccountReply.java:13 |
| c2s | 61 | 2 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraTrackableType.java:8 |
| c2s | 62 | 3 | u16le, u8(v+128) | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:296 |
| c2s | 63 | 15 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/OcclusionManager.java:10 |
| c2s | 64 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/C0e4ebe77.java:7 |
| c2s | 65 | 3 | u16(lo+128, hi), u8 | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:282 |
| c2s | 66 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/GraphicsRelated.java:11 |
| c2s | 67 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/PureJavaModelRelated1.java:4 |
| c2s | 68 | 11 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/AudioChannel.java:6 |
| c2s | 69 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/Mouse.java:4 |
| c2s | 70 | 1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/MapElement.java:5 |
| c2s | 71 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/CgaFinalize.java:18 |
| c2s | 72 | 11 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/PingRequest.java:9 |
| c2s | 73 | 6 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/JcaInit.java:5 |
| c2s | 74 | 7 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/LoginManager.java:19 |
| c2s | 75 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/C9e211cac.java:4 |
| c2s | 76 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/NpcType.java:6 |
| c2s | 77 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/GpuBuffer.java:16 |
| c2s | 78 | 7 | u16(lo+128, hi), u16le, u16le, u8neg | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:145 |
| c2s | 79 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/WfaInit.java:10 |
| c2s | 80 | 7 | u8(128-v), u16be, u16(hi, lo+128), u16le | ParticleEmitter.opParticleEmitter4 | builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitter.java:119 |
| c2s | 81 | -1 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/KdaToString.java:4 |
| c2s | 82 | 8 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/PingRequest.java:10 |
| c2s | 83 | 3 |  | unread | builds/rs2/614-2010-07-13/src/src/main/java/deob/C1b0f82ac.java:13 |
