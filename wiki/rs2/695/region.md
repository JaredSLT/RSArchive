# Region

## Rebuild packet

Unread. `C5adf0e0a.opkind28764` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C5adf0e0a.java:41`) and `C5adf0e0a.opkind28769` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C5adf0e0a.java:45`, `throws IOException`) could not be decompiled. `opkind28767` in the same file stores scaled ints (`1754962601 * var3`, `1202647543 * var1`, `846122345 * var4`) and is a setter, not a map packet reader (`builds/rs2/695-2011-12-22/src/src/main/java/deob/C5adf0e0a.java:23`). Zone coordinates, the scene origin, and the key list inside the rebuild were not read.

`rslib/src/both/move/rev_695.c` describes a rebuild body (two g2 values, a g1_sub128, a g1_add128, then a tail) and leaves `region_encode` null. That body is not in a Java method that was read. The disagreement is in `gaps.md`.

## Map file names

`Biy.opkind280313` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Biy.java:152)` returns null unless its int guard is 95. Otherwise the name is `"m" + var2 + "_" + var3` when the second boolean is true, and `"l" + var2 + "_" + var3` when it is false. When the first boolean is true, the name gains a leading `u`. The method does not say which integer is x and which is z.

## XTEA

`Ca53ed1b5.opkind28634` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Ca53ed1b5.java:57`) has no caller in this source tree. The algorithm it implements:

- Block count `(end - start) / 8`.
- Each block reads two u32be values with `Cf9661093.opkind20932`.
- 32 rounds. The delta literal is `-1640531527`, which is the unsigned 32-bit value `0x9E3779B9`.
- First mix uses `key[sum & 3]`.
- Sum then increases by the delta.
- Second mix uses `key[(sum & 6915) >>> 11]`. `6915` is `0x1B03`. The only bits at or above bit 11 are bits 11 and 12 (`0x1800`), so `(sum & 6915) >>> 11` equals `(sum >>> 11) & 3`.
- The mix is `v += key + sum ^ (other + (other >>> 5 ^ other << 4))`, with `+` binding tighter than `^`.
- The stored offset then goes back eight bytes (`-= 42532456`) and both ints are written with `C2c7eccd5.opkind21945` (u32be).

The int parameter between start and the key array is unused. Which packet carries the four keys was not read.
