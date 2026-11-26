# Input

## Keys

`WoMklAcaw` implements `KeyListener` and `FocusListener` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/WoMklAcaw.java:8`). `keyPressed` forwards the event to `C6bed1507.opkind20991` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/WoMklAcaw.java:49`). That method was not read, so the key packet is unread.

## Mouse

`DootArraWtgXrcYtx` implements `MouseListener`, `MouseMotionListener`, and `MouseWheelListener` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/DootArraWtgXrcYtx.java:10`). `mousePressed` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/DootArraWtgXrcYtx.java:373`) calls `opkind24735` with button code 0, 1, or 2. It does not call `C54dbe422.opkind28982`. `mouseWheelMoved` is at `builds/rs2/695-2011-12-22/src/src/main/java/deob/DootArraWtgXrcYtx.java:433` and was not read. The camera class `CameraControlMode` exists (`builds/rs2/695-2011-12-22/src/src/main/java/deob/CameraControlMode.java:3`). Its control values were not read.

## Walk and minimap

Opcode 1, size 5, constructor `builds/rs2/695-2011-12-22/src/src/main/java/deob/Zqw.java:86`. Opcode 56, size 18, constructor `builds/rs2/695-2011-12-22/src/src/main/java/deob/Cfa8219d8.java:207`. No decompiled method writes either prototype. `KeBz.opkind25137(Object[], int, int, byte)` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/KeBz.java:55`) could not be decompiled. Field lists are empty in `packets.md`.

## Widget and other c2s sends that were read

The handlers in `packets.md` with a nonempty or `(none)` field list are the decompiled sends. Opcode 57 (`Wal.opkind27478`, `builds/rs2/695-2011-12-22/src/src/main/java/deob/Wal.java:25`) writes two `jstr` values and a flag byte (bit 0, bit 1) when its int argument is at least 102. Opcode 10 (`CqFz.opkind23921`, `builds/rs2/695-2011-12-22/src/src/main/java/deob/CqFz.java:84`) writes two `jstr` values and two u8 values under a u8 length of `String.length()` of each string plus 4. Opcode 14 and opcode 44 send a size-0 prototype and write no payload bytes.
