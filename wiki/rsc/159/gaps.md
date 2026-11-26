# Gaps

No file under the revision source root was left unread. `platform-native` does not apply: this tree has no `jaggl`, `jagdx`, or `jagtheora`.

Rows are `implementable` when the Java that was read is enough to code the behavior, and the other client disagrees or a called method body was not opened. There is no `blocked` row.

| item | source | why it blocked | class |
| --- | --- | --- | --- |
| BZip2.call11f2fbbd bitstream | builds/rsc/159-2002-11-25/src/src/main/java/deob/BZip2.java:41 | The jag caller and the signature were read. The decompressor body was not. | implementable |
| World3D.call4e47afec | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/World3D.java:596 | callc3e902a1 calls it for every model. The method body was not read. | implementable |
| PixMap.calle1658783 scaler | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/PixMap.java:993 | The sprite dispatch and the start of the scaler were read. The rest of the blit was not. | implementable |
| Object3D merge constructors | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Object3D.java:625 | Object3D(Object3D[]) and the boolean variant call call7bac3bfb. That body was not read. | implementable |
| Walk opcode | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:5437 | Java walk-here is 194 and approach is 215. rslib/src/client/packets/159.c binds RS_C2S_WALK to 221. Java 221 is cast-on-ground at MudClient.java:9236. rslib/src/both/move/rev_rsc.c describes the 194 and 215 body and leaves opcode_walk at -1. | implementable |
| Player update opcode | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3167 | Java opcode is 255, bits 11 then 13 then 4 then 8, cursor at 8. rslib/src/server/packets/159.c binds RS_S2C_PLAYER_UPDATE to 191. rev_rsc.c matches the 11 and 13 widths and writes a count of 0. | implementable |
| NPC update opcode | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3693 | Java opcode is 248. rslib/src/server/packets/159.c binds RS_S2C_NPC_UPDATE to 79. | implementable |
| Region opcode | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:5459 | There is no region opcode. The load is call8390e4c9 from opcode 255. rslib/src/server/packets/159.c binds RS_S2C_REGION to 25. Java opcode 25 is c2s password change at Client.java:452. | implementable |
| Opcode 244 offsets | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3853 | Five u16be values are stored and added inside call8390e4c9 at MudClient.java:5465. rev_rsc.c says the offsets stay 0. zone = origin / 8 + 6 is not a Java field. | implementable |
| Inventory opcode | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3468 | Java inventory is s2c 252. rslib/src/server/packets/159.c binds RS_S2C_INVENTORY to 53. | implementable |
| Command opcode | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:550 | Java command is opcode 7. rslib/src/client/packets/159.c binds RS_C2S_COMMAND to 253. Java 253 is the contact-details submit at MudClient.java:1797. | implementable |
| Attack and drop opcodes | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:9175 | Java attack npc is 244, attack player is 228, drop is 251, unequip is 248, item action is 246. rslib/src/client/packets/159.c binds attack npc 247, attack player 241, drop 246, and ground item 248. Java 247 is the empty contact-details request at MudClient.java:8344. Java 241 is use-item-on-loc at MudClient.java:9048. | implementable |
| Friend and ignore opcodes | builds/rsc/159-2002-11-25/src/src/main/java/jagex/client/Client.java:514 | Java delete friend is 27 and add ignore is 29. rslib/src/client/packets/159.c swaps those two. Delete ignore 30, add friend 26, and private message 28 match. | implementable |
| Appearance opcode | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:1955 | Java appearance is c2s 236, eight u8. rslib/src/client/packets/159.c binds RS_C2S_APPEARANCE to 218. Java 218 is shop close at MudClient.java:6673. S2c 239 opens the designer. | implementable |
| Trade opcode | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6937 | Java trade offer is 234 and trade accept is 232. rslib/src/client/packets/159.c binds RS_C2S_TRADE to 232. | implementable |
| Close opcode | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:6866 | Java 233 declines trade and the duel confirm. Shop close is 218. S2c 233 closes the shop. rslib/src/client/packets/159.c binds RS_C2S_CLOSE_INTERFACE to 233. | implementable |
| Spawn and appearance defaults | builds/rsc/159-2002-11-25/src/src/main/java/deob/MudClient.java:3175 | number211 and number98 are assigned only from opcode 255. The constructor sets appearance number418=1, number419=2, number420=2, number421=8, number422=14, number424=1, and leaves number4172 and number423 at 0 (MudClient.java:9442). The C host spawn 100,200 and the appearance list 0,10,18,26,33,36,42 are not these assignments. | implementable |

Opcodes 8 and 9 in rslib/src/server/packets/159.c match Java message and logout. Opcode 5 keepalive, opcode 1 logout, opcode 3 chat, opcode 214 follow, and opcode 249 wield match the Java writers. Those matches are not gaps.
