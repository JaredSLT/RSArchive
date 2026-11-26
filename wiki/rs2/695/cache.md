# Cache

Kind: js5. The archive metadata `builds/rs2/695-2011-12-22/meta.json` sets `"engine": "js5"` and `"cache_downloaded": false`. `builds/rs2/695-2011-12-22/README.md` records that no OpenRS2 disk cache was downloaded. This spec does not add a cache. The string `.jagex_cache_32/runescape` is that metadata's `cache_home`. A search of the Java sources found no `jagex_cache` literal.

## JS5 handshake and CRC slots

The connect bytes and the 27 u32be CRCs are in `login.md`. The 27 objects are created in `builds/rs2/695-2011-12-22/src/src/main/java/deob/Xejo.java:256` through `builds/rs2/695-2011-12-22/src/src/main/java/deob/Xejo.java:282` by `opkind243510(3, Jgw.valuekind23561 or valuekind23564 or valuekind23562 or valuekind23563, new Object[3])`. Those `Jgw` values are mode objects. The numeric JS5 index id is not a literal in this block.

## Non-game socket writes

These are not `Bwz.opkind25975` opcodes.

`Ca16555f3.opkind30586` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Ca16555f3.java:87`), when connection slot `[10]` is set, resets the offset, writes u8 `2` if the boolean is true and u8 `3` if it is false, writes five bytes of long 0 through `C50b7ab2f.opkind27345(0L, -19151, ...)`, and sends the whole buffer. The send method is the stub `Cc9967dcd.opkind27496`.

`FiuFvFbjsNsDzFyqLzTdhuFyk.opkind247215` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/FiuFvFbjsNsDzFyqLzTdhuFyk.java:195`) writes u8 6 (`Pgo`, guard argument `var0 + 35`), u24be of 3 (`Efd.opkind28926(3, true, ...)`), and u16be of 0, then sends. The send length expression indexes the buffer object by `var0`. The caller that picks `var0` was not read.

## Config decode

`Cc24bb2d2.opkind26523` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/Cc24bb2d2.java:24`) reads a u8 with `NgFz.opkind248815` until the byte is 0. Each nonzero byte is passed to `FlUsSy.opkind25026` (`builds/rs2/695-2011-12-22/src/src/main/java/deob/FlUsSy.java:31`), which could not be decompiled. Config opcodes, interface archive ids, and per-archive decode switches are unread.
