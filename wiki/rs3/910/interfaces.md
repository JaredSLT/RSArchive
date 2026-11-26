# Interfaces

## Component header

`Component.decode` (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/config/iftype/Component.java:573`) starts:

1. `g1` version. 255 is stored as -1.
2. `g1` type. If bit 7 is set it is cleared and a `gjstr` name follows.
3. `g2` clientcode, `g2s` x, `g2s` y, `g2` width, `g2` height.
4. Four signed bytes. When either of the first two is 4, two more `g2` aspect values follow.
5. `g2` layer. 65535 means -1, otherwise the high half of `parentlayer` is added.
6. `g1` flags. Bit 0 hides the component. When the version is at least 0, bit 1 is noclickthrough.

Types whose bodies were opened:

| type | what follows in the opened slice |
| --- | --- |
| 0 | `g2` scroll width, `g2` scroll height, and a `g1` when the version is negative |
| 5 | `g4s` graphic, `g2` angle, then several `g1` flags |
| 6 | model block starting at `Component.java:637` |
| 4 | text flags, font bytes, colour bytes |
| 3 | `g1` fill, `g1` transparency |
| 9 | `g1` line width, `g1` direction |

The method continues after `Component.java:703`. Later type ids were not read.

## What the client opens

`openedTopInterface` starts at -1 (`Client.java:1556`). On the login screen it is set to `graphicsDefaults.login_interface` (`Client.java:2730`). In the lobby it is set to `graphicsDefaults.lobby_interface` (`Client.java:2782`). Those two values are `g3` fields of defaults opcodes 5 and 6 (`GraphicsDefaults.java:145`). They are not integer literals in the client. There is no disk cache, so the numeric ids are not in this archive.

`IF_OPENTOP` (`Client.java:10006`) reads four 32-bit key values (`g4_alt2`, `g4_alt1`, `g4_alt2`, `g4s`), discards `g1`, and reads `g2_alt3` as the interface id. Size 19. `IF_OPENSUB` was not read. `IF_OPENSUB_ACTIVE_OBJ` was read and is listed in `packets.md`.

## Scripts

Scripts live in JS5 archive 12. `ClientScript` (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/script/ClientScript.java:44`) reads the trailer first. The last `g2` is the trailer length. The header sits at `length - 2 - trailer - 16` and holds `g4s` instruction count, three `g2` local counts (int, object, long), three `g2` argument counts, `g1` switch-table count, and for each table a `g2` size plus pairs of `g4s` key and value. Position 0 then reads `fastgstr` as the script name.

Each instruction is `g2` into `ClientScriptCommand.values()` (`ClientScript.java:59`). That index is the opcode. The constructor's first integer is not the opcode. Operand width is `g4s` when `isLargeOperand` is set, otherwise `g1`, except `PUSH_VAR` and `POP_VAR` (`g1` domain, `g2` id, `g1`), `PUSH_CONSTANT_STRING` (`g1` type, then `gjstr` or `g4s` or `g8`), and `PUSH_VARBIT` / `POP_VARBIT` (`g2` plus `g1`) (`ClientScript.java:105`).

`values()` is the array at `ClientScriptCommand.java:2876`. It was not copied into this spec. `ScriptRunner.executeScript` (`ScriptRunner.java:415`) walks `instructions` until `_RETURN` at frame depth 0. `intOperands[pc] == 1` selects the secondary form. `executeCommand` was not read, except `movescripted` which sends `MOVE_SCRIPTED`.
