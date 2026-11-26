# Interfaces

`DataBlock2471.doDataBlock2471(PositionedSound)` (`deob/DataBlock2471.java:497`) decodes one component.

1. `u8`. 255 is stored as -1. This is the version gate `var2`.
2. `u8` `tint20`. Bit `0x80` means a following `jstr` name, and the low 7 bits are kept.
3. `u16be`, `i16be`, `i16be`, `u16be`, `u16be`.
4. Four `i8`.
5. `u16be` parent. 65535 becomes -1. Any other value is combined with `tint18 & 0xFFFF0000`.
6. `u8` flags. Bit 0 is stored. When the version gate is at least 0, bit 1 is stored.

Type 0 (`tint20 == 0`, `DataBlock2471.java:531`): two `u16be`. When the version gate is below 0, one more `u8`.

Type 5 (`DataBlock2471.java:539`): `u32be`, `u16be`, `u8` flag, two `u8`, `u32be`, two `u8` booleans, `u32be`. When the version gate is at least 3, another `u8`.

Type 6 (`DataBlock2471.java:556`): `tint47` is set to 1, `smart32-1`, then `u8` flags. Bit 0 selects two `i16be`, three `u16be`. Bits 1, 2, and 3 are booleans. The rest of the type switch was not read.

Widgets are loaded from archive 3 with models archive 7, sprites archive 8, and fonts archive 13 (`deob/Statics.java:3737`). See [cache.md](cache.md).

`Client.tint86` starts at -1 (`rs2/client/Client.java:991`). On the logged-in interface build it is set from `cls89f44c11.tint6` (`Client.java:2087`) or `cls89f44c11.tint7` (`Client.java:2137`). Those ints are in the defaults blob. There is no disk cache, so the numeric top-interface id is in [gaps.md](gaps.md). A server packet also assigns `tint86` (`Client.java:8651`). That handler was not read as a field list.
