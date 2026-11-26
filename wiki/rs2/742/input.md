# Input

Walk and minimap are destination coordinates, not a step list. `MiniMenu.createMoveMessage` (builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1982) writes `MOVE_GAMECLICK` (opcode 74, size 5) when the third argument is 0, and `MOVE_MINIMAPCLICK` (opcode 41, size 18) when it is 1.

Both start with:

1. `u16be` `base.z + destZ` (`p2`)
2. `u16le_add128lo` `base.x + destX` (`p2_alt3`: low byte plus 128, then high byte)
3. `u8` 1 when control is held

The destination is the scene-local click plus the world base. `mapFlagX` / `mapFlagZ` store the local destination. Minimap (`action == 23` and `index == 1`, builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1800) appends 13 bytes: `u8` 0xFF, `u8` 0xFF, `u16be` `valueClient1619`, `u8` 57, `u8` `valueClient1653`, `u8` `valueClient1578`, `u8` 89, `u16be` player x, `u16be` player z, `u8` 63. 5 + 13 = 18.

`CLICKWORLDMAP` (opcode 66, size 4) is one `u32le` (`p4_alt1`) of `valueStatics344<<28 | valueStatics474<<14 | valueStatics341` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:3109).

`WINDOW_STATUS` (opcode 31, size 6, builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2371): `u8` window mode, `u16be` canvas width, `u16be` canvas height, `u8` antialiasing. Window mode is 3 when fullscreen, 2 when `valueClient1640`, otherwise 1.

`EVENT_CAMERA_POSITION` (opcode 33, size 4, builds/rs2/742-2012-11-07/src/client/src/main/java/ClientWatch.java:134): two `u16le_add128lo`, each of a camera value shifted right by 3.

`EVENT_APPLET_FOCUS` (opcode 94, size 1): `u8` 1 when focused.

`EVENT_KEYBOARD` (opcode 24, size -2, builds/rs2/742-2012-11-07/src/client/src/main/java/ClientWatch.java:114): the leading `u16be` is the -2 length and equals `count * 4`. Each event is `u8` key plus `u24be` delta, clamped to 16777215.

`CLIENT_DETAILOPTIONS_STATUS`: `u8` length via `psize1`, then `options.createPreferencesBlock()` bytes. The block body is unread.

`PING_STATISTICS` (opcode 93, size 4, builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2939): `u8`, `u8` gc percent, `u16be_add128lo` (`p2_alt2`) ping.

`NO_TIMEOUT` (opcode 30, size 0) has an empty payload.

`REFLECTION_CHECK_REPLY` (opcode 25, size -1) writes a `u8` length and then `ReflectionCheck.performCheck`. That method was not read.

Mouse click, applet (`EVENT_MOUSE_CLICK` opcode 90, size 6, builds/rs2/742-2012-11-07/src/client/src/main/java/AppletMouseLogger.java:29): `u32le` `x | y<<16`, `u16be` packed with the button in bit 15.

Mouse click, native (`EVENT_NATIVE_MOUSE_CLICK` opcode 61, size 7, builds/rs2/742-2012-11-07/src/client/src/main/java/NativeMouseLogger.java:25): `u8_add128`, `u16be_add128lo`, `u32_alt3` of `x | y<<16`.

Mouse move (`EVENT_MOUSE_MOVE` opcode 67, size -1, builds/rs2/742-2012-11-07/src/client/src/main/java/MouseLogger.java:24): the first `u8` is the -1 length. The payload starts with two bytes written at the reserved slot: `u8` `(timeSum / count)` and `u8` `(timeSum % count)`. Each sample is one of the branches at builds/rs2/742-2012-11-07/src/client/src/main/java/MouseLogger.java:80: a single `u16be` when the deltas fit in 6 bits; `u8` plus `u16be` when they fit in 8 bits; or `u8` plus `u32be`, or `u16be` plus `u32be`, for the long forms. The applet hook writes no extra byte. The native hook writes one more `u8` per sample (builds/rs2/742-2012-11-07/src/client/src/main/java/NativeMouseLogger.java:44), on `EVENT_NATIVE_MOUSE_MOVE`.

Widget and world ops from `MiniMenu` (builds/rs2/742-2012-11-07/src/client/src/main/java/MiniMenu.java:1772 and the following branches):

| packet | fields |
| --- | --- |
| OPLOC1..6 | u8_neg ctrl, u16le_add128lo z+base, u32le loc id, u16le_add128lo x+base |
| OPNPC* | u16le index, u8 ctrl |
| OPNPCT | see the `packets.md` row written at the `OPNPCT` create site |
| OPPLAYER* | u16le_add128lo index, u8 ctrl |
| FACE_SQUARE | u16le_add128lo, u16le_add128lo |
| IF_BUTTON1..10 | u16le_add128lo, u16le_add128lo, u32_alt3 (`opClient319`, builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:9987) |

`MESSAGE_PUBLIC` (builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:10009): length `u8`, then `u8`, `u8`, then `WordPack.opWordPack11`. The word pack was not read. `MESSAGE_PRIVATE` (builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:10028): length `u16be`, `jstr`, then the same word pack.

`CHAT_SETMODE` writes a single `u8` (builds/rs2/742-2012-11-07/src/client/src/main/java/ScriptRunner.java:9885).
