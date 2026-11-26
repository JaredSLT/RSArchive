# Gaps

Every unread `files.tsv` row is listed here. This pass opened every `.java` file, so the unread count is 0 and there is no unread-file row. `blocked` uses `unread`, `no-cache`, `no-source`, or `platform-native` in the why column.

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| s2c 2 MESSAGE_GAME branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7217 | field list is source order; some reads are inside if | implementable |
| s2c 23 valueServerProt13 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8686 | field list is source order; some reads are inside if | implementable |
| s2c 29 RUNCLIENTSCRIPT branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6541 | field list is source order; some reads are inside if | implementable |
| s2c 31 valueServerProt110 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8390 | field list is source order; some reads are inside if | implementable |
| s2c 44 valueServerProt30 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8001 | field list is source order; some reads are inside if | implementable |
| s2c 67 valueServerProt47 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6929 | field list is source order; some reads are inside if | implementable |
| s2c 77 valueServerProt115 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6724 | field list is source order; some reads are inside if | implementable |
| s2c 80 valueServerProt57 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6848 | field list is source order; some reads are inside if | implementable |
| s2c 86 valueServerProt62 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7250 | field list is source order; some reads are inside if | implementable |
| s2c 94 valueServerProt68 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6966 | field list is source order; some reads are inside if | implementable |
| s2c 98 valueServerProt101 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7168 | field list is source order; some reads are inside if | implementable |
| s2c 121 valueServerProt89 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7064 | field list is source order; some reads are inside if | implementable |
| s2c 136 valueServerProt84 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8470 | field list is source order; some reads are inside if | implementable |
| s2c 139 valueServerProt106 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8138 | field list is source order; some reads are inside if | implementable |
| s2c 140 valueServerProt107 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6742 | field list is source order; some reads are inside if | implementable |
| s2c 144 valueServerProt42 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:7756 | field list is source order; some reads are inside if | implementable |
| s2c 147 UPDATE_REBOOT_TIMER branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8542 | field list is source order; some reads are inside if | implementable |
| s2c 162 valueServerProt124 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8116 | field list is source order; some reads are inside if | implementable |
| c2s 25 REFLECTION_CHECK_REPLY fields | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2927 | writer was found and its direct buf writes are empty or delegated; remaining payload unread | implementable |
| c2s 84 CLAN_JOINCHAT_LEAVECHAT writers differ | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:11960 | more than one field list; row joins them with OR | implementable |
| zone 11 valueZoneProt30 branches | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8811 | field list is source order; some reads are inside if | implementable |
| BASType opcode 27 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:178 | branch contains nested reads; list is source order | implementable |
| BASType opcode 28 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:187 | branch contains nested reads; list is source order | implementable |
| BASType opcode 52 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:242 | branch contains nested reads; list is source order | implementable |
| BASType opcode 56 | builds/rs2/742-2012-11-07/src/client/src/main/java/BASType.java:263 | branch contains nested reads; list is source order | implementable |
| EffectAnimType opcode 40 | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:107 | branch contains nested reads; list is source order | implementable |
| EffectAnimType opcode 41 | builds/rs2/742-2012-11-07/src/client/src/main/java/EffectAnimType.java:115 | branch contains nested reads; list is source order | implementable |
| IDKType opcode 2 | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:54 | branch contains nested reads; list is source order | implementable |
| IDKType opcode 3 | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:60 | branch contains nested reads; list is source order | implementable |
| IDKType opcode 40 | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:61 | branch contains nested reads; list is source order | implementable |
| IDKType opcode 41 | builds/rs2/742-2012-11-07/src/client/src/main/java/IDKType.java:69 | branch contains nested reads; list is source order | implementable |
| InvType opcode 4 | builds/rs2/742-2012-11-07/src/client/src/main/java/InvType.java:33 | branch contains nested reads; list is source order | implementable |
| LocType opcode 1 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:255 | branch contains nested reads; list is source order | implementable |
| LocType opcode 40 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:301 | branch contains nested reads; list is source order | implementable |
| LocType opcode 41 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:309 | branch contains nested reads; list is source order | implementable |
| LocType opcode 42 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:317 | branch contains nested reads; list is source order | implementable |
| LocType opcode 79 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:399 | branch contains nested reads; list is source order | implementable |
| LocType opcode 106 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:444 | branch contains nested reads; list is source order | implementable |
| LocType opcode 160 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:463 | branch contains nested reads; list is source order | implementable |
| LocType opcode 249 | builds/rs2/742-2012-11-07/src/client/src/main/java/LocType.java:508 | branch contains nested reads; list is source order | implementable |
| MapElementType opcode 15 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:174 | branch contains nested reads; list is source order | implementable |
| MapElementType opcode 249 | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElementType.java:220 | branch contains nested reads; list is source order | implementable |
| NPCType opcode 1 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:216 | branch contains nested reads; list is source order | implementable |
| NPCType opcode 40 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:228 | branch contains nested reads; list is source order | implementable |
| NPCType opcode 41 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:236 | branch contains nested reads; list is source order | implementable |
| NPCType opcode 42 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:244 | branch contains nested reads; list is source order | implementable |
| NPCType opcode 60 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:280 | branch contains nested reads; list is source order | implementable |
| NPCType opcode 121 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:343 | branch contains nested reads; list is source order | implementable |
| NPCType opcode 160 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:408 | branch contains nested reads; list is source order | implementable |
| NPCType opcode 249 | builds/rs2/742-2012-11-07/src/client/src/main/java/NPCType.java:437 | branch contains nested reads; list is source order | implementable |
| ObjType opcode 40 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:269 | branch contains nested reads; list is source order | implementable |
| ObjType opcode 41 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:277 | branch contains nested reads; list is source order | implementable |
| ObjType opcode 42 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:285 | branch contains nested reads; list is source order | implementable |
| ObjType opcode 132 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:380 | branch contains nested reads; list is source order | implementable |
| ObjType opcode 249 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:404 | branch contains nested reads; list is source order | implementable |
| ObjType opcode 251 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:438 | branch contains nested reads; list is source order | implementable |
| ObjType opcode 252 | builds/rs2/742-2012-11-07/src/client/src/main/java/ObjType.java:444 | branch contains nested reads; list is source order | implementable |
| ParticleEmitterType opcode 9 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:270 | branch contains nested reads; list is source order | implementable |
| ParticleEmitterType opcode 10 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:276 | branch contains nested reads; list is source order | implementable |
| ParticleEmitterType opcode 25 | builds/rs2/742-2012-11-07/src/client/src/main/java/ParticleEmitterType.java:311 | branch contains nested reads; list is source order | implementable |
| QuestType opcode 3 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:92 | branch contains nested reads; list is source order | implementable |
| QuestType opcode 4 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:100 | branch contains nested reads; list is source order | implementable |
| QuestType opcode 10 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:118 | branch contains nested reads; list is source order | implementable |
| QuestType opcode 13 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:126 | branch contains nested reads; list is source order | implementable |
| QuestType opcode 14 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:132 | branch contains nested reads; list is source order | implementable |
| QuestType opcode 18 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:143 | branch contains nested reads; list is source order | implementable |
| QuestType opcode 19 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:155 | branch contains nested reads; list is source order | implementable |
| QuestType opcode 249 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuestType.java:167 | branch contains nested reads; list is source order | implementable |
| QuickChatCatType opcode 2 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatCatType.java:36 | branch contains nested reads; list is source order | implementable |
| QuickChatCatType opcode 3 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatCatType.java:45 | branch contains nested reads; list is source order | implementable |
| QuickChatPhraseType opcode 2 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatPhraseType.java:39 | branch contains nested reads; list is source order | implementable |
| QuickChatPhraseType opcode 3 | builds/rs2/742-2012-11-07/src/client/src/main/java/QuickChatPhraseType.java:45 | branch contains nested reads; list is source order | implementable |
| SeqType opcode 1 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:91 | branch contains nested reads; list is source order | implementable |
| SeqType opcode 3 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:106 | branch contains nested reads; list is source order | implementable |
| SeqType opcode 12 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:126 | branch contains nested reads; list is source order | implementable |
| SeqType opcode 13 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:135 | branch contains nested reads; list is source order | implementable |
| SeqType opcode 16 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:152 | branch contains nested reads; list is source order | implementable |
| SeqType opcode 249 | builds/rs2/742-2012-11-07/src/client/src/main/java/SeqType.java:177 | branch contains nested reads; list is source order | implementable |
| SkyBoxType opcode 2 | builds/rs2/742-2012-11-07/src/client/src/main/java/SkyBoxType.java:36 | branch contains nested reads; list is source order | implementable |
| BillboardType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/BillboardType.java:1 | opcode switch was not extracted from this file | implementable |
| BindingType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/BindingType.java:1 | opcode switch was not extracted from this file | implementable |
| CameraEffectType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/CameraEffectType.java:1 | opcode switch was not extracted from this file | implementable |
| CameraTrackableType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/CameraTrackableType.java:1 | opcode switch was not extracted from this file | implementable |
| ClientTriggerType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/ClientTriggerType.java:1 | opcode switch was not extracted from this file | implementable |
| DataType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/DataType.java:1 | opcode switch was not extracted from this file | implementable |
| LoadableResourceType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/LoadableResourceType.java:1 | opcode switch was not extracted from this file | implementable |
| LoadingScreenElementType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/LoadingScreenElementType.java:1 | opcode switch was not extracted from this file | implementable |
| LoadingScreenType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/LoadingScreenType.java:1 | opcode switch was not extracted from this file | implementable |
| PrimitiveType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/PrimitiveType.java:1 | opcode switch was not extracted from this file | implementable |
| RebuildType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/RebuildType.java:1 | opcode switch was not extracted from this file | implementable |
| SpotAnimType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/SpotAnimType.java:1 | opcode switch was not extracted from this file | implementable |
| TextureTramsformType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/TextureTramsformType.java:1 | opcode switch was not extracted from this file | implementable |
| ToolkitType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/ToolkitType.java:1 | opcode switch was not extracted from this file | implementable |
| TriggerType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/TriggerType.java:1 | opcode switch was not extracted from this file | implementable |
| UniformType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/UniformType.java:1 | opcode switch was not extracted from this file | implementable |
| VideoType decode | builds/rs2/742-2012-11-07/src/client/src/main/java/VideoType.java:1 | opcode switch was not extracted from this file | implementable |
| RSLib CAMERA_MOVE and CAM_SHAKE | builds/rs2/742-2012-11-07/src/client/src/main/java/ServerProt.java:82 | rslib/src/server/packets/742.c binds both RS_S2C_CAMERA_MOVE and RS_S2C_CAM_SHAKE to opcode 91. Java CAM_MOVETO is opcode 25 size 6 and CAM_SHAKE is opcode 91 size 6. Sizes for 0..163 otherwise match. | implementable |
| RSLib player placement comment | builds/rs2/742-2012-11-07/src/client/src/main/java/MapElement.java:41 | rslib/src/both/move/rev_742.c describes the 30-bit placement as PLAYER_INFO. Java login consumes opMapElement41 before the rebuild remainder. Later PLAYER_INFO can also read gBit(30) in the high-res kind-3 branch. | implementable |
| RSLib unused opcode pad | builds/rs2/742-2012-11-07/src/client/src/main/java/ServerProt.java:7 | RSLib size arrays pad past the last Java opcode with -3. Java ServerProt ends at 163 and ClientProt ends at 107. | implementable |
| preferences block body | builds/rs2/742-2012-11-07/src/client/src/main/java/ClientWatch.java:146 | createPreferencesBlock bytes are copied. The field layout inside the block was not read. | implementable |
| hardware block length | builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:467 | login copies the array from opHardwarePlatform7. That length and createHardwareBlock were not read. | implementable |
| WordPack.opWordPack11 | builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:10014 | public and private chat append this buffer. The method was not read. | implementable |
| ObjTypeCustomisation.opObjTypeCustomisation29 | builds/rs2/742-2012-11-07/src/client/src/main/java/PlayerEntity.java:175 | appearance reads this when a wear slot's customisation bit is set. The method was not read. | implementable |
| map file bytes and XTEA apply | builds/rs2/742-2012-11-07/src/client/src/main/java/World.java:465 | rebuild stores four u32be keys per square and requests m/l/u file names. The terrain and loc decode, and the XTEA call site, were not read. | implementable |
| BuildAreaSize.forId | builds/rs2/742-2012-11-07/src/client/src/main/java/World.java:460 | the size byte is passed through. The id-to-tile-count table was not read, so mapSizeX is not a hardcoded 104. | implementable |
| PureJava raster | builds/rs2/742-2012-11-07/src/client/src/main/java/PureJavaToolkitFactory.java:13 | software create id 0 returns PureJavaToolkit. Raster, floor, model, and sprite draw bodies were not read. | implementable |
| camera spline tail | builds/rs2/742-2012-11-07/src/client/src/main/java/BasicCamera.java:124 | opLookat_Sub13 and opPositionSpline6 consume the rest of valueServerProt93 when bits 5 and 6 of the first u8 are set. Those methods were not read. | implementable |
| ReflectionCheck.performCheck | builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2930 | c2s REFLECTION_CHECK_REPLY payload after the length byte was not read. | implementable |
| client script bodies | builds/rs2/742-2012-11-07/src/client/src/main/java/Js5Archive.java:43 | CLIENTSCRIPTS is archive 12. ClientScriptCommand opcode implementations were not read. | implementable |
| top-level interface id | builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:104 | no-cache. Opcode 5 and 6 are u24be interface ids in defaults group 3. The cache is not in this tree. | blocked |
| jaggl | builds/rs2/742-2012-11-07/src/client/src/main/java/RendererToolkit.java:54 | platform-native. Java GlToolkitFactory is the entry. The jaggl sources are JNI bindings. | blocked |
| jagdx | builds/rs2/742-2012-11-07/src/client/src/main/java/RendererToolkit.java:58 | platform-native. Java DxToolkitFactory is the entry. The jagdx sources are JNI bindings. | blocked |
| jagtheora | builds/rs2/742-2012-11-07/src/client/src/main/java/LoginManager.java:477 | platform-native. Login sends whether the library loaded. The jagtheora sources are JNI bindings. | blocked |
| jagdx/C7cbf08df.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/C7cbf08df.java:1 | platform-native | blocked |
| jagdx/C7cbf08df37.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/C7cbf08df37.java:1 | platform-native | blocked |
| jagdx/C7cbf08df38.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/C7cbf08df38.java:1 | platform-native | blocked |
| jagdx/C7cbf08df39.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/C7cbf08df39.java:1 | platform-native | blocked |
| jagdx/C7cbf08df40.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/C7cbf08df40.java:1 | platform-native | blocked |
| jagdx/Cfaba9b39.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/Cfaba9b39.java:1 | platform-native | blocked |
| jagdx/D3DADAPTER_IDENTIFIER.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/D3DADAPTER_IDENTIFIER.java:1 | platform-native | blocked |
| jagdx/D3DCAPS.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/D3DCAPS.java:1 | platform-native | blocked |
| jagdx/D3DDISPLAYMODE.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/D3DDISPLAYMODE.java:1 | platform-native | blocked |
| jagdx/D3DLIGHT.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/D3DLIGHT.java:1 | platform-native | blocked |
| jagdx/D3DPRESENT_PARAMETERS.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/D3DPRESENT_PARAMETERS.java:1 | platform-native | blocked |
| jagdx/D3DPSHADERCAPS2_0.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/D3DPSHADERCAPS2_0.java:1 | platform-native | blocked |
| jagdx/D3DVSHADERCAPS2_0.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/D3DVSHADERCAPS2_0.java:1 | platform-native | blocked |
| jagdx/HRESULT.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/HRESULT.java:1 | platform-native | blocked |
| jagdx/IDirect3D.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3D.java:1 | platform-native | blocked |
| jagdx/IDirect3DBaseTexture.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DBaseTexture.java:1 | platform-native | blocked |
| jagdx/IDirect3DCubeTexture.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DCubeTexture.java:1 | platform-native | blocked |
| jagdx/IDirect3DDevice.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DDevice.java:1 | platform-native | blocked |
| jagdx/IDirect3DEventQuery.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DEventQuery.java:1 | platform-native | blocked |
| jagdx/IDirect3DIndexBuffer.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DIndexBuffer.java:1 | platform-native | blocked |
| jagdx/IDirect3DSurface.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DSurface.java:1 | platform-native | blocked |
| jagdx/IDirect3DSwapChain.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DSwapChain.java:1 | platform-native | blocked |
| jagdx/IDirect3DTexture.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DTexture.java:1 | platform-native | blocked |
| jagdx/IDirect3DVertexBuffer.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DVertexBuffer.java:1 | platform-native | blocked |
| jagdx/IDirect3DVolumeTexture.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IDirect3DVolumeTexture.java:1 | platform-native | blocked |
| jagdx/IUnknown.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/IUnknown.java:1 | platform-native | blocked |
| jagdx/JagdxCccc.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagdx/JagdxCccc.java:1 | platform-native | blocked |
| jaggl/MapBuffer.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jaggl/MapBuffer.java:1 | platform-native | blocked |
| jaggl/OpenGL.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jaggl/OpenGL.java:1 | platform-native | blocked |
| jagtheora/misc/SimplePeer.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/misc/SimplePeer.java:1 | platform-native | blocked |
| jagtheora/ogg/OggPacket.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/ogg/OggPacket.java:1 | platform-native | blocked |
| jagtheora/ogg/OggPage.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/ogg/OggPage.java:1 | platform-native | blocked |
| jagtheora/ogg/OggStreamState.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/ogg/OggStreamState.java:1 | platform-native | blocked |
| jagtheora/ogg/OggSyncState.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/ogg/OggSyncState.java:1 | platform-native | blocked |
| jagtheora/theora/DecoderContext.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/theora/DecoderContext.java:1 | platform-native | blocked |
| jagtheora/theora/Frame.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/theora/Frame.java:1 | platform-native | blocked |
| jagtheora/theora/GranulePos.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/theora/GranulePos.java:1 | platform-native | blocked |
| jagtheora/theora/SetupInfo.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/theora/SetupInfo.java:1 | platform-native | blocked |
| jagtheora/theora/TheoraComment.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/theora/TheoraComment.java:1 | platform-native | blocked |
| jagtheora/theora/TheoraInfo.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/theora/TheoraInfo.java:1 | platform-native | blocked |
| jagtheora/vorbis/DSPState.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/vorbis/DSPState.java:1 | platform-native | blocked |
| jagtheora/vorbis/VorbisBlock.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/vorbis/VorbisBlock.java:1 | platform-native | blocked |
| jagtheora/vorbis/VorbisComment.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/vorbis/VorbisComment.java:1 | platform-native | blocked |
| jagtheora/vorbis/VorbisInfo.java | builds/rs2/742-2012-11-07/src/client/src/main/java/jagtheora/vorbis/VorbisInfo.java:1 | platform-native | blocked |
