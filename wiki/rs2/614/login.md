# Login

Revision 614, family rs2, date 2010-07-13. Main class `deob.Client` extends `GlRenderList` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:17). `Client.main` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:402) requires six arguments and calls `GlRenderList.call618a22eb(34, 768, title, (byte)66, 1024, false, 614, build)` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:461). That stores height 768, width 1024, and revision 614, and sets the frame title to `Jagex` (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlRenderList.java:366). The first argument 34 is the number of cache index files opened by `AnimationNode` (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlRenderList.java:386, builds/rs2/614-2010-07-13/src/src/main/java/deob/AnimationNode.java:155).

The login state machine is `MapCoordUtil.calla978325d` (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:62). Callers pass 3. The socket port is not 43594. `SeqTypeList.opSeqTypeList1` treats 43594 as a guard and still opens `valueSeqTypeList4` on `valueSeqTypeList6` or `valueSeqTypeList3` (builds/rs2/614-2010-07-13/src/src/main/java/deob/SeqTypeList.java:98). The live client sets `valueSeqTypeList6` to world id plus U+9C40 and `valueSeqTypeList3` to world id plus U+C350 (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:2063). A matching world flag uses host `127.0.0.1` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:2070). The socket wrapper is created with 7500 (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:107).

## Handshake

Stage 2 writes two bytes and flushes them with no ISAAC (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:112):

| order | width | value |
| --- | --- | --- |
| 1 | u8 | 14 (`CameraHelpers.valueCameraHelpers8`) |
| 2 | u8 | `(usernameHash >> 16) & 31` |

Stage 3 reads one u8. Nonzero is a failure code: the client reports it, disconnects, and calls `DxStencilBuffer.opDxStencilBuffer1(5)` (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:124). Zero moves to stage 4.

Stage 4 reads 8 bytes and decodes them with `opUnused6137(true)`, which is two `u32be` values, high word first (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:205, builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:144). That u64 is the server session.

## RSA block

The plaintext block is an `Unused6` of 518 bytes (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:145):

| order | width | value |
| --- | --- | --- |
| 1 | u8 | 10 |
| 2 | u32be | random seed 0, `0 .. 99999999` |
| 3 | u32be | random seed 1 |
| 4 | u32be | `(int)(session >> 32)` |
| 5 | u32be | `(int)session` |
| 6 | u64be | username hash (`opUnused6144` of `BufferedFile.opBufferedFile3`) |
| 7 | jstr | password, modified UTF then NUL (`call75e62fa6`, builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:137) |
| 8 | u64be | `SynthSound.valueSynthSound1` |
| 9 | u64be | `IterableQueueIterator.valueIterableQueueIterator6` |

`opUnused6138((byte)-118, modulus, exponent)` replaces that buffer with `u16be` of `BigInteger.toByteArray().length` plus those bytes (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:215). Modulus is `SkyBoxType.valueSkyBoxType4` (builds/rs2/614-2010-07-13/src/src/main/java/deob/SkyBoxType.java:9). Exponent is `Billboard.valueBillboard4` = 65537 (builds/rs2/614-2010-07-13/src/src/main/java/deob/Billboard.java:18).

## Game login opcode 19

Used when the scene state is not 2 and not 3 (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:159). Bytes after the opcode:

| order | width | value |
| --- | --- | --- |
| 1 | u16be | placeholder 0, later backpatched with `opUnused6143` to the number of bytes written after it |
| 2 | u32be | 614 |
| 3 | u8 | `GpuModelFlags.valueCutsceneManager2` |
| 4 | u8 | `GpuRelated12.valueGpuRelated123` |
| 5 | bytes(24) | `Billboard.call318e7730` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Billboard.java:69) |
| 6 | jstr | `MdaWrite.valueMdaWrite5` |
| 7 | u32be | `MSIType.valueMSIType1` |
| 8 | u32be x 34 | `GlxRelated12.opGlxRelated1214` (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlxRelated12.java:23) |
| 9 | bytes | the RSA block, copied raw |

The 34 integers are `u32be` from `opBindings4` of JS5 indexes 0 through 31, then `C7120902c.call644a0412`, then `ParticleList.opParticleList2`.

## Lobby login opcodes 18 and 16

Scene state 2 or 3. Opcode 18 when `WorldMap.valueWorldMap13 == 13`, otherwise opcode 16 (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:173).

| order | width | value |
| --- | --- | --- |
| 1 | u16be | placeholder, backpatched the same way |
| 2 | u32be | 614 |
| 3 | u8 | `VarnTypeList.valueVarnTypeList1` |
| 4 | u8 | `Loading.opLoading2` |
| 5 | u16be | canvas width `Cd311d774.valuekind10872` |
| 6 | u16be | canvas height `HashMap.valueHashMap4` |
| 7 | u8 | `NonCpCharacter.valuekind10567.valuekind111225` |
| 8 | bytes(24) | machine info, same call |
| 9 | jstr | `MdaWrite.valueMdaWrite5` |
| 10 | u32be | `MSIType.valueMSIType1` |
| 11 | u8, bytes(n) | client preferences. `WearposDefaults.opWearposDefaults1((byte)-120)` returns 46, the buffer capacity (builds/rs2/614-2010-07-13/src/src/main/java/deob/WearposDefaults.java:17). The length byte is the offset after `VaaInit.opkind11126`, whose interior was not read |
| 12 | bytes(19) | `HardwarePlatform.opHardwarePlatform4` (builds/rs2/614-2010-07-13/src/src/main/java/deob/HardwarePlatform.java:242). `WfaInit.opWfaInit4(-127)` returns 19 (builds/rs2/614-2010-07-13/src/src/main/java/deob/WfaInit.java:30) |
| 13 | u16be | `ChatLine.valueChatLine8` |
| 14 | u64be | `GpuRendererRelated2.valueGpuRendererRelated27` |
| 15 | u32be x 34 | same checksum block |
| 16 | bytes | the RSA block |

The 19 preference bytes are: nine u8 (the first is literal 3, two are 0/1 flags), one u16be, one u8, one u24be, one u32be.

## ISAAC

After either login block is flushed, the outgoing buffer is seeded with the four seeds and each seed is then increased by 50. The incoming buffer is seeded with the increased seeds (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:206). ISAAC is `Pix3D`, not the class named `Isaac` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Pix3D.java:26, builds/rs2/614-2010-07-13/src/src/main/java/deob/ParticleEmitterRelated.java:72).

## Response byte

Stage 5 reads one u8 (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:216).

| byte | result |
| --- | --- |
| 21 | stage 8. Another u8 is read. `GlBufferRelated1.valueGlBufferRelated11 = 60 * (3 + byte)`. Reported as 21. Disconnect |
| 29 | stage 14. Another u8 is stored in `Ced1eccca.valuekind10621`. Reported as 29. Disconnect |
| 1 | stage 6 and report 1. Stage 6 writes one ISAAC opcode byte 26 and returns to stage 5 (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:261) |
| 2 | stage 9, then the success block below |
| 15 | payload length preset to -2 and stage 15 |
| 23 | if the reconnect count is below 3, restart at stage 1 |
| other | failure with that code, then disconnect |

## Success block

Stage 9 reads one u8 length and stage 10 reads that many bytes (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:287).

Game path, scene state not 2 and not 3 (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:302):

`u8, u8, u8, u8, u8, u16be, u16be, u16be, u16be, u32be, u8, u16be, u16be, u8, gjstr2, u8, u32be, u16be, gjstr2`

`gjstr2` is `call76b99ad9`: a version byte that must be 0, then a NUL string (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:30). The u16be at the world-id slot stores 65535 as -1.

Lobby path, scene state 2 or 3 (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:343):

`u8, u8, u8, u8, u8, u8, u16be, u8, s24be, u8`

`s24be` is `opUnused6168(1950018080)` (builds/rs2/614-2010-07-13/src/src/main/java/deob/Unused6.java:596). The u16be is stored as the local player slot `FloorUnderlayType.valueFloorUnderlayType1`. When the scene state is 3, one more u8 is read. If it is 1 the client also reads `u64be`, `u8` count, and that many ISAAC-subtracted bytes (`opParticleEmitterRelated7`). `MapElementType.opMapElementType6` turns those bytes into a string and was not read.

## Entering the world

Stage 13, reached from stage 12 after a 3-byte header, reads an ISAAC opcode and then forces a `u16be` length (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:463). It reads the local-player block `NPCTypeCustomisation.opNPCTypeCustomisation2` (builds/rs2/614-2010-07-13/src/src/main/java/deob/NPCTypeCustomisation.java:29). If the opcode is `RayTracing.valueRayTracing9` (opcode 26) it calls dynamic rebuild. Every other opcode on this path calls static rebuild. The cursor must equal the u16be length or the client throws `lswp pos`.

Stage 15 reads a `u16be` length because the length was preset to -2, reads the same local-player block, and requires the cursor to equal that length (`lswpr pos`). It does not call a rebuild in this method (builds/rs2/614-2010-07-13/src/src/main/java/deob/MapCoordUtil.java:501).

## Account creation

`FloorUnderlayType.opFloorUnderlayType1` (builds/rs2/614-2010-07-13/src/src/main/java/deob/FloorUnderlayType.java:71) builds a plaintext `Unused6` of 576 bytes, RSA-wraps it with the same modulus and exponent, then writes opcode 25 or 22.

Plaintext: `u8 10`, `u16be` random, `u64be`, `u16be`, `jstr`, `u64be`, then either four `u32be` random values or two `u64be` values, then one more `u32be` random.

On the wire, after the opcode: `u16be (28 + rsaLength)`, `u16be 614`, `u8` language, `u8` gpu flag, `bytes(24)` machine info, then the raw RSA bytes. Opcode 25 is used when the boolean argument is true. Opcode 22 is used when it is false. Login opcodes are `PathFinderEntitySize`. The constructor stores only the opcode (builds/rs2/614-2010-07-13/src/src/main/java/deob/PathFinderEntitySize.java:7). Declared pairs are in `framing.md`.

## Declared login opcodes whose writers were not read

Opcode 15 size 4, opcode 17 size 0, opcode 20 size 6, opcode 21 size 9, opcode 23 size 4, opcode 24 size -1, and opcode 27 size 0 are constructed in `CameraHelpers` (builds/rs2/614-2010-07-13/src/src/main/java/deob/CameraHelpers.java:5). Opcode 20 is written as a bare opcode byte at builds/rs2/614-2010-07-13/src/src/main/java/deob/JReZCu.java:27 and opcode 21 at builds/rs2/614-2010-07-13/src/src/main/java/deob/GlModelFlags.java:18. Their bodies were not read. See `gaps.md`.
