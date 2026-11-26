# Interfaces

Components are `ShaderRelated`, not the six-value `Interface` enum (`com/jagex/game/config/iftype/Interface.java`). The archive is `FileUtil.fileUtil4`, id 3, opened as `clsbde36bc413` and passed to `ShaderRelated.doShaderRelated` (`deob/DataBlock374.java:385`).

`ShaderRelated.doShaderRelated(WorldMapElement)` (`com/jagex/graphics/ShaderRelated.java:544`) starts with a common header:

1. `u8` version, and `255` means `-1`.
2. `u8` type. If bit `0x80` is set, clear it and read `jstr`.
3. `u16be`, `i16be`, `i16be`, `u16be`, `u16be`.
4. Four `i8`.
5. `u16be` parent. `65535` means `-1` after adding `this.tint20 & 0xFFFF0000`. Otherwise the raw value is kept and that sum is stored.
6. `u8` flags. Bit 0 is `tboolean2`. If the version is at least 0, bit 1 is `tboolean3`.

Type 0 (`:578`): two `u16be`. If the version is below 0, one more `u8` is `tboolean3`.

Type 5 (`:586`): `u32be`, `u16be`, `u8` flags (bits 0 and 1), two `u8`, `u32be`, two `u8` booleans, `u32be`. If the version is at least 3, one more `u8` boolean.

Type 6 starts at line 603 (`u8` 1, `smart2or4`, `u8` flags) and the rest of that type was not read. Later tests exist for types 4 (`:638`), 3 (`:656`), and 9 (`:662`). Their bodies were not read. Further types in the same method were not read.

The top-level interface id and the sidebar ids were not read as a table. `Client.tint86` is used as an open-interface field in code that was not given a field-level trace here.

## Scripts

`ClientScriptCommand` (`com/jagex/game/script/ClientScriptCommand.java:8`) is the opcode enum. The first constant is `add = new ClientScriptCommand(289)`. Later constants pass an integer and sometimes a boolean. Those integers are the command opcodes. The interpreter methods start at `VarClanSettingsDomain.doVarClanSettingsDomain` (`com/jagex/game/config/vartype/VarClanSettingsDomain.java:200`) and were not read.

Server opcode 28, size `-2` (`DataBlock457.clsfbe8a09929`, `Client.java:9215`), runs one script. It reads `jstr`. Walking that string from the end, `s` reads another `jstr` and any other character reads `u32be`. A final `u32be` is stored at argument 0. `VarClanSettingsDomain.doVarClanSettingsDomain` then runs it.
