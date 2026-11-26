# Gaps

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| Map reload formula is not the Java stub. The stub returns false. Sector math is on [region](region.md), read from `deob.MudClient.fn8390e4c9` in the deob jar | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:9670 | fernflower stub. Implement the bytecode formula, not `return false` | implementable |
| Chunk `.hei` name and RLE are on [region](region.md). `World.call17103888` itself is empty | builds/rsc/177-2003-10-31/src/src/main/java/deob/World.java:1231 | fernflower stub. Filename and `.hei` were read from `deob.World.fn17103888` | implementable |
| `.dat`, `.loc`, and `.jm` byte layouts | builds/rsc/177-2003-10-31/deob/gamepack-deob.jar `deob.World.fn17103888` | unread. The bytecode reaches the names `.dat`, `.loc`, and `.jm`. The per-byte records were not read | blocked |
| Missing `.hei` zero-fill path | builds/rsc/177-2003-10-31/deob/gamepack-deob.jar `deob.World.fn17103888` | unread past the null `.hei` branch | blocked |
| Terrain tile cases inside `World.call9329ebf4` | builds/rsc/177-2003-10-31/src/src/main/java/deob/World.java:1359 | unread. The method starts the 96 by 96 mesh. Tile-type arms were not read | blocked |
| Player type 5 skip when the entity is missing adds 14, then one u8 `n`, then `n+1`. The present-player body is 17+n bytes after the index and type | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6232 | both cursors are in the decompile and they differ by one byte. Keep each path as written | implementable |
| Sleep image codec `drawArea.call378` | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:7554 | unread. Opcode 201 passes the framed buffer. The image decoder was not read | blocked |
| `onMouseClick` | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:1544 | no-source. Fernflower stub. Click fields used later are `mouseX`, `mouseY`, and `number374` | blocked |
| `ClientStream.run` writer thread | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/ClientStream.java:130 | no-source. Fernflower stub. The ring write is in `ClientStream.write` | blocked |
| `World3D.callc3e902a1` | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/World3D.java:632 | no-source. Fernflower stub | blocked |
| World draw after the death and sleep branches | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:8079 | unread. Death and sleep text were read. The rest of `call34c75df3` was not | blocked |
| `Gui` widget type codes inside `call90`, `call94`, `call95`, `call97` | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Gui.java:6 | unread. Call sites are cited from `MudClient`. The constructors' type switches were not read | blocked |
| Option-panel labels for c2s opcodes 197 and 247 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:13907 | unread. Both packets are empty. The `drawString` above each click was not read | blocked |
| `AudioPlayer.start` and `stop` do nothing | builds/rsc/177-2003-10-31/src/src/main/java/sun/audio/AudioPlayer.java:13 | compile stub. PCM bytes are still selected by `call711f4ca5` | implementable |
| C client spawn claim 100, 200 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:2302 | Java login scene loads sector 50, 50 plane 0. Live tiles arrive on opcode 255. The client stores no 100, 200 start tile | implementable |
| RSLib s2c `PLAYER_UPDATE` 191 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:5769 | Java player positions are opcode 255. Opcode 191 is not handled | implementable |
| RSLib s2c `NPC_UPDATE` 79 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6409 | Java NPC positions are opcode 248 | implementable |
| RSLib s2c `INVENTORY` 53 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:6059 | Java inventory is opcode 252 | implementable |
| RSLib s2c `REGION` 25 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:484 | Opcode 25 is c2s change-password. There is no region packet. Map load is client-side from opcode 255 | implementable |
| RSLib c2s `CHAT` 193 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:611 | Public chat is opcode 3. Opcode 193 is the sleep word | implementable |
| RSLib c2s `COMMAND` 253 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:618 | Commands are opcode 7. Opcode 253 is four contact strings | implementable |
| RSLib c2s `WALK` 221 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:9647 | Walk here is opcode 194. Action walk is opcode 215. Opcode 221 is menu 900 | implementable |
| RSLib c2s `DEL_FRIEND` 29 and `ADD_IGNORE` 27 | builds/rsc/177-2003-10-31/src/src/main/java/jagex/client/Client.java:502 | Opcode 29 adds an ignore. Opcode 27 removes a friend. Add friend is 26. Remove ignore is 30 | implementable |
| RSLib c2s `IDLE_LOGOUT` 1 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:1574 | Opcode 1 is session logout. The in-game idle logout request is opcode 6. Heartbeat is opcode 5 | implementable |
| RSLib c2s `APPEARANCE` 218 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:2141 | Appearance design is opcode 236. Opcode 218 is an empty shop-close packet | implementable |
| RSLib c2s `DROP` 246 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14964 | Drop is menu 660, opcode 251. Opcode 246 is menu 640 | implementable |
| RSLib c2s `ATTACK_NPC` 247 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15020 | Attack npc is menu 715, opcode 244. Opcode 247 is an empty options click | implementable |
| RSLib c2s `ATTACK_PLAYER` 241 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15055 | Attack player is menu 805, opcode 228. Opcode 241 is menu 410 | implementable |
| RSLib c2s `GROUND_ITEM` 248 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14828 | Take is menu 220, opcode 252. Opcode 248 is menu 620 | implementable |
| RSLib c2s `OBJECT1` 224, `OBJECT2` 250, `OBJECT3` 252 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:14806 | Opcode 224 is menu 200, opcode 250 is menu 210, opcode 252 is menu 220. Loc commands are opcodes 242 and 230 | implementable |
| RSLib c2s `NPC1` 245 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:15002 | Opcode 245 is menu 720 talk-to. Npc option is opcode 195 | implementable |
| RSLib c2s `CLOSE_INTERFACE` 233 and `TRADE` 232 | builds/rsc/177-2003-10-31/src/src/main/java/deob/MudClient.java:11936 | Opcode 232 is one empty trade-accept click. Opcode 233 is an empty close from the trade UI. They are not a generic close or a trade-open | implementable |

`rslib/src/both/move/rev_rsc.c` describes walk opcodes 194 and 215 and the 11/13/4 player prefix. Those comments match this client. The same file's packet binds do not: `RS_S2C_PLAYER_UPDATE` is 191 and `RS_S2C_REGION` is 25, so those codecs are disabled. The move codec also omits the known-player and new-player loops that opcode 255 reads (`deob/MudClient.java:7717`, `deob/MudClient.java:7878`).

No `.java` file under the source root was left unread. `files.tsv` has no `unread` row and no `platform-native` row. `jaggl`, `jagdx`, and `jagtheora` are absent.
