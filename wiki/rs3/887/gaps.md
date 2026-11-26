# Gaps

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| disk cache | builds/rs3/887-2017-11-01/README.md | blocked no-cache. README says no OpenRS2 disk cache was downloaded. meta.json cache_downloaded is false. Numeric archive contents are not in this tree. | blocked |
| RSLib single-step z | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:2293 | Java dirs 0..7 change z by -1,-1,-1,0,0,+1,+1,+1. rslib/src/both/move/rev_887.c k_dz is the opposite sign. k_dx matches. | implementable |
| RSLib walk extra step | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:2275 | Java walk reads bit1 and, when set, bit2 more before the 3-bit step. rslib put_move writes that bit as 0. | implementable |
| RSLib player mask bit | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/network/ServerConnection.java:399 | Java consumes mask bytes. rslib/src/both/move/rev_887.c:246 forces the extend bit clear. | implementable |
| RSLib minimap opcode 119 | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/world/WorldMapRenderer.java:2083 | Kind 1 appends 13 bytes after the 5-byte prefix and the constructor size is 18. rslib walk_encode stops at 5 and rejects nsteps. | implementable |
| RSLib dynamic rebuild | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/scene/World.java:496 | Opcode 128 is a per-tile bit grid and has no XTEA keys. rslib region_decode reads only the 8-byte static header. | implementable |
| player mask bodies from bit 512 | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/network/ServerConnection.java:518 | The method keeps reading masks through line 689. Those bodies were not transcribed. | implementable |
| player appearance helper | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/network/ServerConnection.java:449 | Mask 32768 copies bytes with doPositionedSound2 (reverse order, minus 128) and calls doCutsceneAction_Sub1Part2. That helper was not read. | implementable |
| player var-domain masks | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/network/ServerConnection.java:407 | Masks 131072 and 1048576 call DataBlock721. That decoder was not read. | implementable |
| player low-res readd | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/network/ServerConnection.java:273 | doServerConnection2(PositionedSoundPart, int) bit layout was not read. | implementable |
| teleport opcode constant | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:2385 | The 3-bit teleport opcode is compared to Unused24.unused245.tint. That constant was not read. | implementable |
| NPC direction enum | builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:8108 | Walk and run directions are QuestCommands ids. The enum values were not read. | implementable |
| NPC mask bodies | builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:8227 | The extension bytes were read. Bodies after the bit 1 hitsplat start were not transcribed. | implementable |
| hardware block | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/camera/CameraManager.java:505 | createHardwareBlock writes a buffer whose internal fields were not read. The login packet copies that whole array. | implementable |
| GameShell.doGameShell | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/camera/CameraManager.java:500 | The login block calls GameShell.doGameShell. The bytes it writes were not read. | implementable |
| preferences block | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/camera/CameraManager.java:502 | doDataBlock902 layout was not read. The login packet writes u8 length and then that many bytes. | implementable |
| JS5 seed writer | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock499.java:204 | doDataBlock638 writes the bytes between u16be 1 and the XTEA start, and returns the key. That method was not read. | implementable |
| handshake opcodes 23, 24, 27, 31 | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/Unused8.java:16 | Unused8 constructs them. No writer reference was found. | implementable |
| DataBlock523 opcodes after 15 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock523.java:276 | doDataBlock803 keeps an opcode chain through 249. Field widths after opcode 15 were not read. | implementable |
| GWCLocation opcodes after 6 | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/shared/framework/gwc/GWCLocation.java:150 | Opcode 6 is the top-level interface id. Later opcodes in doGWCLocation were not read. | implementable |
| interface id numbers | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/shared/framework/gwc/GWCLocation.java:148 | blocked no-cache. tint7 is a u24be from archive 28. The number is not in the source. | blocked |
| sidebar interface ids | builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:2203 | blocked no-cache. doClient2 opens gWCLocation.tint7 only. Sidebar ids were not found in source. | blocked |
| script bytecode opcodes | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock1037.java:27 | Archive 12 file 0 is loaded here. The decoder that turns those bytes into opcodes was not read. | implementable |
| script interpreter | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6603 | Opcode 104 calls ColourUtils.doColourUtils. The command switch was not read. | implementable |
| component hook predicate | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/group/PlayerGroupDeltaEncodingKey.java:736 | DataBlock557.doDataBlock6952 decides whether three u16 fields follow the u24be. The predicate was not read. | implementable |
| canvas numeric size | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/client/GameShell.java:108 | The canvas is tint42 by tint87. The initial value is doDataBlock861, which was not read. | implementable |
| tick order | builds/rs3/887-2017-11-01/src/src/main/java/rs2/client/Client.java:1416 | doClient2 is applet startup. The per-frame tick method was not read. | implementable |
| software scene draw | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/graphics/safe/PureJavaToolkit.java:15 | The software toolkit constructor reads u16be and u8. Terrain, model, and sprite draw methods were not read. | implementable |
| sound start packet | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6606 | Opcode 38 writes a volume onto AudioStream2. The packet that starts a synth or vorbis sound was not isolated. | implementable |
| click actions other than 23 and 60 | builds/rs3/887-2017-11-01/src/src/main/java/com/jagex/game/world/WorldMapRenderer.java:2122 | Action 2 starts at this line and was not finished. Other menu actions in the same method were not read. | implementable |
| s2c opcode 185 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6847 | No switch arm. The final else logs the opcode and calls Client.loggedout. Payload bytes are not decoded. | implementable |
| c2s opcode 0 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:6 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 1 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:8 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 2 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:10 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 3 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:12 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 4 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:14 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 5 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:16 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 6 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:18 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 7 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:20 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 8 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:22 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 9 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:24 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 10 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:26 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 11 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:28 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 12 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:30 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 13 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:32 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 14 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:34 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 15 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:36 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 16 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:38 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 17 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:40 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 18 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:42 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 19 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:44 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 21 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:48 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 22 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:50 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 23 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:52 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 24 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:54 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 25 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:56 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 26 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:58 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 27 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:60 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 28 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:62 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 29 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:64 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 30 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:66 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 31 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:68 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 32 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:70 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 33 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:72 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 34 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:74 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 35 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:76 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 36 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:78 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 37 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:80 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 38 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:82 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 39 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:84 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 40 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:86 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 41 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:88 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 42 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:90 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 43 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:92 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 44 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:94 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 45 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:96 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 46 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:98 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 47 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:100 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 48 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:102 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 49 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:104 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 50 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:106 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 51 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:108 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 52 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:110 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 53 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:112 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 54 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:114 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 55 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:116 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 56 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:118 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 57 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:120 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 58 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:122 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 59 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:124 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 60 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:126 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 61 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:128 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 62 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:130 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 63 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:132 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 64 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:134 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 65 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:136 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 66 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:138 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 67 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:140 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 68 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:142 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 69 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:144 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 70 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:146 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 71 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:148 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 72 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:150 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 73 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:152 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 74 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:154 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 75 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:156 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 76 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:158 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 77 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:160 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 78 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:162 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 79 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:164 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 80 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:166 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 81 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:168 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 82 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:170 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 83 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:172 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 84 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:174 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 85 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:176 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 86 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:178 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 87 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:180 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 88 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:182 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 89 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:184 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 90 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:186 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 91 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:188 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 92 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:190 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 93 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:192 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 94 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:194 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 95 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:196 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 96 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:198 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 97 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:200 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 98 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:202 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 99 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:204 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 100 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:206 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 101 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:208 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 102 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:210 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 103 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:212 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 104 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:214 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 106 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:218 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 107 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:220 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 108 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:222 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 109 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:224 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 110 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:226 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 111 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:228 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 112 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:230 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 113 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:232 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 114 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:234 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 115 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:236 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 116 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:238 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 117 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:240 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 118 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:242 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 120 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:246 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 121 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:248 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| c2s opcode 122 | builds/rs3/887-2017-11-01/src/src/main/java/deob/DataBlock552.java:250 | handler not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 0 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6318 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 2 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5138 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 3 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6036 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 4 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5435 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 5 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6619 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 6 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4351 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 7 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6667 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 8 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5344 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 9 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4403 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 10 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4073 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 11 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6525 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 12 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5664 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 13 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5022 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 14 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5772 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 15 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4147 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 16 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6347 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 17 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4824 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 18 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4122 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 19 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4852 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 20 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6755 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 21 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6188 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 22 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5517 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 23 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5750 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 24 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6465 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 25 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5267 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 26 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4108 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 27 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5698 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 28 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4331 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 29 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5383 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 30 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4725 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 31 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6686 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 32 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6473 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 33 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6019 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 34 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4448 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 35 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5487 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 36 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4966 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 37 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4399 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 39 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5322 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 40 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6207 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 41 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4539 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 42 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6312 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 43 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5109 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 44 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4156 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 45 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5000 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 46 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:3996 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 47 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5802 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 48 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4381 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 49 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6431 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 50 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6086 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 51 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6293 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 52 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4324 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 53 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5891 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 54 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5470 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 55 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6616 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 56 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5712 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 57 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5668 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 58 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6005 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 59 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5445 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 60 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5154 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 61 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4140 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 62 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6032 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 63 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6489 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 64 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4346 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 65 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6496 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 66 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6275 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 67 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6726 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 68 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6253 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 69 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4060 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 70 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5620 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 71 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5262 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 72 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6333 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 73 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5684 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 74 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5616 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 75 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6182 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 76 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6444 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 77 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4197 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 78 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5954 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 79 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5073 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 80 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4802 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 81 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6680 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 82 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6570 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 83 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6329 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 84 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5330 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 85 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6410 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 86 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4086 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 87 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5121 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 88 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5232 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 89 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4160 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 90 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6356 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 91 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5441 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 92 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:3979 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 93 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5877 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 94 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4151 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 95 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5656 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 96 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4546 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 97 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5569 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 98 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5503 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 99 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6734 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 100 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4008 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 101 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4127 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 102 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5409 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 103 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6450 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 105 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6417 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 106 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4455 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 107 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5456 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 108 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4001 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 109 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5279 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 110 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5088 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 111 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5745 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 112 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5380 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 113 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6764 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 114 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5215 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 115 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6299 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 116 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4842 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 117 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6100 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 118 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5873 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 119 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5796 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 120 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6258 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 121 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6704 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 122 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6172 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 123 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6574 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 124 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4806 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 125 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6481 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 127 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5390 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 129 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6730 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 130 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5211 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 131 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5660 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 132 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6135 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 133 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6365 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 134 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5734 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 135 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6239 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 136 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6192 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 137 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4793 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 138 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5340 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 139 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6285 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 140 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6744 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 141 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4523 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 142 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5781 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 143 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6405 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 144 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6082 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 145 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6750 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 146 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6709 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 147 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6630 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 148 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4392 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 149 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4042 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 150 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4710 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 151 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6322 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 152 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5719 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 154 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5375 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 155 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6623 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 156 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6690 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 157 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4732 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 158 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5421 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 159 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6426 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 160 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5394 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 161 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4874 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 162 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5273 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 163 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5633 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 164 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4048 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 165 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5201 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 166 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4118 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 167 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5897 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 168 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4498 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 169 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5131 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 170 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5475 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 171 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6719 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 172 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5483 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 173 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5222 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 174 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4516 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 175 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:3992 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 176 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5637 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 177 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5015 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 178 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4185 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 179 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5788 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 180 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5904 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 181 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6579 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 182 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6773 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 183 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5726 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 186 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4864 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 187 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4463 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 188 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6832 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 189 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5416 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 190 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4371 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 191 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:4133 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 192 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:6247 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| s2c opcode 193 | builds/rs3/887-2017-11-01/src/src/main/java/deob/Statics.java:5294 | switch arm not read. Size is the constructor value. Field list is empty. | implementable |
| jagdx/D3DADAPTER_IDENTIFIER.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/D3DADAPTER_IDENTIFIER.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/D3DCAPS.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/D3DCAPS.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/D3DDISPLAYMODE.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/D3DDISPLAYMODE.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/D3DLIGHT.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/D3DLIGHT.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/D3DPRESENT_PARAMETERS.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/D3DPRESENT_PARAMETERS.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/D3DPSHADERCAPS2_0.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/D3DPSHADERCAPS2_0.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/D3DSURFACE_DESC.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/D3DSURFACE_DESC.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/D3DVSHADERCAPS2_0.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/D3DVSHADERCAPS2_0.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/DataBlock1004.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/DataBlock1004.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/DataBlock1010.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/DataBlock1010.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/DataBlock1022.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/DataBlock1022.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/DataBlock492.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/DataBlock492.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/DataBlock501.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/DataBlock501.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/DataBlock656.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/DataBlock656.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3D.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3D.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DBaseTexture.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DBaseTexture.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DCubeTexture.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DCubeTexture.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DDevice.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DDevice.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DEventQuery.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DEventQuery.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DIndexBuffer.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DIndexBuffer.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DSurface.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DSurface.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DSwapChain.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DSwapChain.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DTexture.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DTexture.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DVertexBuffer.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DVertexBuffer.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IDirect3DVolumeTexture.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IDirect3DVolumeTexture.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/IUnknown.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/IUnknown.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/Jagdx2.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/Jagdx2.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jagdx/RuntimeExceptionPart2.java | builds/rs3/887-2017-11-01/src/src/main/java/jagdx/RuntimeExceptionPart2.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jaggl/MapBuffer.java | builds/rs3/887-2017-11-01/src/src/main/java/jaggl/MapBuffer.java | blocked platform-native. jaggl or jagdx binding. | blocked |
| jaggl/OpenGL.java | builds/rs3/887-2017-11-01/src/src/main/java/jaggl/OpenGL.java | blocked platform-native. jaggl or jagdx binding. | blocked |
