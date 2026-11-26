# Interfaces

Component decode is `Component.opComponent53` (builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:652).

Common header:

- `u8` version. 255 is stored as -1
- `u8` type. Bit 7 clears the type to 7 bits and reads `jstr` name
- `u16be` clientcode
- `i16be` xpos, `i16be` ypos, `u16be` width, `u16be` `valueComponent274`
- four `i8` (two extras, then x mode and y mode)
- `u16be` parent. 65535 means -1. Any other value is combined with `id & 0xFFFF0000`
- `u8` flags. Bit 0 is hide. When version is at least 0, bit 1 is `valueComponent283`

Type-specific bytes. These are the only `this.type ==` tests in the file:

| type | bytes | citation |
| --- | --- | --- |
| 0 layer | u16be scroll width, u16be scroll height; if version < 0, one more u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:682 |
| 5 graphic | u32be graphic, u16be, u8 flags, u8, u8 outline, u32be shadow, u8, u8, u32be colour; version >= 3 adds u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:689 |
| 6 model | smart2or4null, u8 flags. If bit 0: i16be, i16be, u16be, u16be, u16be, u16be. Else if bit 1: i16be, i16be, i16be, u16be, u16be, u16be, i16be. Then smart2or4null. If valueComponent324 is non-zero, u16be. If valueComponent272 is non-zero, u16be | builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:705 |
| 4 text | smart2or4null font, optional u8 when version >= 2, jstr, four u8, u32be colour, u8; version >= 0 adds u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:737 |
| 3 rectangle | u32be colour, u8, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:753 |
| 9 line | u8, u32be colour, u8 | builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:758 |

Any other type has no extra bytes and falls through. The shared tail (builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:763) is `u24be` events, then hooks: a `u8` selector, and while it is non-zero another `u8`, `i8`, `i8`, then the next selector. The selector's high nibble and that following `u8` form a 12-bit id (4095 is stored as -1). Then `jstr` opbase and a `u8` whose low nibble is the option-string count and whose high nibble gates the next pairs. The low nibble is that many `jstr`. If the high nibble is above 0, the next bytes are `u8`, `u16be`. If it is above 1, another `u8`, `u16be`. Then `jstr` pause text, three `u8`, `jstr`, and, when `ServerKeyProperties.unpackTargetMask` of the event word is non-zero, three `u16be` (65535 is stored as -1). Version >= 0 adds another `u16be`, then a `u8` count of (`u24be`, `u32be`) and a `u8` count of (`u24be`, `jstr2`). Hooks are `opComponent69` (builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:885): `u8` count, or null when 0; each entry is `u8` 0 then `u32be`, or `u8` 1 then `jstr`. `opComponent54` (builds/rs2/742-2012-11-07/src/client/src/main/java/Component.java:904) is `u8` count then that many `u32be`. Twenty `opComponent69` calls follow, plus one more when version >= 0, then five `opComponent54` calls.

Client scripts live in `Js5Archive.CLIENTSCRIPTS` id 12. Interfaces live in `Js5Archive.INTERFACES` id 3. `ClientScriptCommand` opcode bodies were not read.

## Which interface opens

The top-level interface id is not a Java constant. `GraphicsDefaults` is loaded from defaults group id 3. Decode opcode 5 is `u24be` `valueGraphicsDefaults28`, used as the game top interface (builds/rs2/742-2012-11-07/src/client/src/main/java/GraphicsDefaults.java:104). Opcode 6 is `u24be` `lobby_interface`. The cache file is not in this tree, so the numeric ids are absent. Opcode 1's `i16be` pairs are hitmark sprite offsets, not sidebar ids (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:5733).

`IF_OPENTOP` (opcode 45, size 19, builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:8638): `u32le`, `u16le_sub128lo` interface id, `u32_alt2`, `u8_128sub` type, `u32be`, `u32be`. Type 2 closes the world map. `topLevelInterface` becomes the id.

`IF_OPENSUB` (opcode 47, size 23, builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6993): `u32le`, `u32le`, `u32_alt2` combined id, `u8_128sub` type, `u16le_sub128lo` child, `u32_alt2`, `u32_alt2`.

`IF_CLOSESUB` is opcode 148. `RUNCLIENTSCRIPT` (opcode 29, size -2) reads a `jstr` of `s` versus other argument types, the values in reverse, then `u32be` script id (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:6541).
