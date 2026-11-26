# Input

Walk, minimap, camera, keys, mouse, and widget ops are client packets. Sizes are the `ClientProt` constructor values. Field order is the order of the writes.

## Walk

`MiniMenu.doMiniMenu(int,int,int)` (`builds/rs3/910-2019-10-16/src/src/main/java/com/jagex/game/MiniMenu.java:2501`) writes the shared prefix for opcode 33 `MOVE_GAMECLICK` and opcode 78 `MOVE_MINIMAPCLICK`:

1. `p2` of `base.z + localZ` (`u16be`).
2. `p1` of 1 when control is held, else 0.
3. `p2_alt3` of `base.x + localX` (little-endian short with the low byte plus 128).

Opcode 33 stops there. Size 5. When the menu action passes `var7 == 1`, the caller appends (`MiniMenu.java:2280`): `p1(-1)`, `p1(-1)`, `p2` of `(int) orbitCameraYaw`, `p1(57)`, `p1(minimapAnticheatAngle)`, `p1(minimapZoom)`, `p1(89)`, `p2` of local player x, `p2` of local player z, `p1(63)`. That is 13 more bytes. Opcode 78 has size 18.

`rslib/src/both/move/rev_910.c` writes only the 5-byte prefix for opcode 78. See `gaps.md`.

`MOVE_SCRIPTED` is written by `ScriptRunner.movescripted` (`ScriptRunner.java:17450`): `p1_alt1` of `MoveSpeed.getId()` (`serialID`, not the enum index), `p2_alt2` of z, `p2_alt1` of x. Size 5. Crawl, walk, and run are the only accepted speeds.

`CLICKWORLDMAP` (`Client.java:4139`) writes `p4_alt3` of `tint65 << 28` plus `tint64 << 14` plus `tint66`. Size 4.

`FACE_SQUARE` (`MiniMenu.java:2398`) writes `p2_alt2` of world z and `p2_alt3` of world x.

## Widget and world ops

`IF_BUTTON1` through `IF_BUTTON10` share `Client.doClient` (`Client.java:13075`): `p2_alt3(invobject)`, `p2_alt2(component id)`, `p4(parentlayer)`. Size 8. `rslib/src/both/action.c` writes that same order for revision 910.

`WINDOW_STATUS` (`Client.java:3234`): `p1` window mode, `p2` canvas width, `p2` canvas height, `p1` anti-alias. Size 6.

The other menu writers that were read are in `packets.md`: `OPPLAYER1`..`10`, `OPLOC1`..`6`, `OPNPC1`..`6`, `OPOBJ1`..`6`, `OPOBJT`, `OPNPCT`, `OPLOCT`, `OPPLAYERT`, `APCOORDT`, `IF_BUTTOND`, `IF_BUTTONT`, `RESUME_PAUSEBUTTON`.

## Mouse, keys, camera

`EVENT_MOUSE_CLICK` (`AppletMouseLogger.java:39`): `p4` of x plus y shifted left 16, after clamping each coordinate to 0..65535, then `p2_alt3` of `MouseLogger.doMouseLogger` with bit 15 set when `buttonAction == 2`. The parent `doMouseLogger` was not opened. Size 6.

`EVENT_MOUSE_MOVE` is created at `AppletMouseLogger.java:59`. `doAppletMouseLogger(Packet, MouseEvent)` is empty (`AppletMouseLogger.java:63`) and the length helper returns 0. The parent move writer was not opened.

`EVENT_KEYBOARD` (`ClientWatch.java:130`): `p2` of `count * 4`, then each event is `p1` key code and `p3` delta clamped to 16777215. The opcode size is `-2`, and this `p2` is that length prefix.

`EVENT_CAMERA_POSITION` (`ClientWatch.java:172`): `p2_alt3` of yaw shifted right 3, `p2` of pitch shifted right 3. `cameraState == 3` uses the quaternion helpers. Otherwise the values are `orbitCameraYaw` and `orbitCameraPitch`.

`EVENT_APPLET_FOCUS` (`ClientWatch.java:180`): `p1` of 1 when `GameShell.focus` is set.

`CLIENT_DETAILOPTIONS_STATUS` (`ClientWatch.java:186`): placeholder `p1(0)`, the preferences block, `psize1`. The block internals were not opened. Size `-1`.

`CLIENT_COMPRESSEDTEXTUREFORMAT_SUPPORT` (`ClientWatch.java:198`): placeholder `p2(0)`, `p1` 0 or 1, `SerializableEnums.encodeSet` (unread), `pSmart1or2` count, `pSmart2or4` minimum id and deltas, `psize2`. Size `-2`.
