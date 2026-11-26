# Cache

Cache kind is jag. Versions (`deob/Version.java`):

| archive | version |
| --- | --- |
| config | 85 |
| filter | 2 |
| media | 58 |
| entity | 24 |
| textures | 17 |
| models | 36 |
| maps | 63 |
| land | 63, same `MAPS` constant |
| sounds | 1 |

`load` order (`deob/MudClient.java:866` and the methods it reaches):

1. `config85.jag`, entry `string.dat` and `integer.dat`, decoded by `ClientConfig.call8fd2f9f5` (`deob/MudClient.java:934`).
2. `filter2.jag`: `fragmentsenc.txt`, `badenc.txt`, `hostenc.txt`, `tldlist.txt`, each wrapped in `jagex.Packet` and passed to `WordFilter.call194` (`deob/MudClient.java:940`).
3. `media58.jag` sprites, starting with `index.dat`, `inv1.dat`, `inv2.dat`, `bubble.dat` (`deob/MudClient.java:956`). The rest of the sprite name list in that method was not copied here.
4. `entity24.jag`, and `entity24.mem` when the members flag is set (`deob/MudClient.java:1027`).
5. `textures17.jag` (`deob/MudClient.java:1156`).
6. Applet: `models36.jag`, each `ClientConfig.models[i] + ".ob3"`. Local: `../gamedata/models/name.ob2` (`deob/MudClient.java:1227`). `"na"` is model id 0 and is not stored (`deob/ClientConfig.java:192`).
7. `maps63.jag` and `land63.jag`. Members also loads `.mem` (`deob/MudClient.java:1274`).
8. Members sounds: `sounds1.mem` (`deob/MudClient.java:1288`).

`Tools.readJag` inflates a named entry with `BZip2.decompress` (`jagex/Tools.java:459`, `jagex/Tools.java:492`). `jagex.Packet` reads big-endian u8, u16, u32 (`jagex/Packet.java:28`). It is the filter buffer, not the socket stream.

There is no interface archive and no component-script opcode. Widgets are `jagex.client.Gui`. See [interfaces](interfaces.md).

## integer.dat and string.dat

`call8fd2f9f5` (`deob/ClientConfig.java:261`) is one sequential stream. It is not an opcode switch. The fernflower copy after a `return` is the same blocks for the `number92 != 0` entry. The live path is `number92 == 0`. Counts are u16be. Strings are `gjstr`. `g4` saturates as in [framing](framing.md).

Model ids are not a block. `getModelId` assigns the next id the first time a name is seen (`deob/ClientConfig.java:192`). Loc models are read as `gjstr` and passed through `getModelId`.

| block | per entry, in order |
| --- | --- |
| objects, count u16be | gjstr name, gjstr desc, gjstr option, u16be sprite, u32be cost, u8 stackable, u8 unknown, u16be wear type, u32be sprite mask, u8 untradeable, u8 members. If the client is not members, members objects are renamed. That rename loop is `deob/ClientConfig.java:403`. |
| npcs, count u16be | gjstr name, gjstr desc, u8 attack, u8 strength, u8 hits, u8 defense, u8 attackable, then 12 times u8 sprite (255 stored as -1) (`deob/ClientConfig.java:1170`), u32be hair, u32be top, u32be bottom, u32be skin, u16be width, u16be length, u8 walk speed, u8 combat speed, u8 combat width, gjstr option |
| textures, count u16be | gjstr name, gjstr sub name |
| animations, count u16be | gjstr name, u32be color, u8 gender, u8 has-a, u8 has-f, u8 file id |
| locs, count u16be | gjstr name, gjstr desc, gjstr option1, gjstr option2, gjstr model name via `getModelId`, u8 width, u8 length, u8 type, u8 elevation |
| bounds, count u16be | gjstr name, gjstr desc, gjstr option1, gjstr option2, u16be height, u32be front mask, u32be back mask, u8 block walk, u8 active |
| roofs, count u16be | u8 height, u8 fill |
| decors, count u16be | u32be fill, u8 type, u8 block walk |
| projectiles | one u16be count and no per-entry fields before spells (`deob/ClientConfig.java:938`) |
| spells, count u16be | gjstr name, gjstr desc, u8 level, u8 runes required, u8 type, then u8 rune-id count and that many u16be rune ids, then u8 rune-count count and that many u8 counts |
| prayers, count u16be | gjstr name, gjstr desc, u8 level, u8 drain |

The method then nulls both buffers and returns (`deob/ClientConfig.java:1069` on the zero-spell early path, and `deob/ClientConfig.java:1139` after the spell-rune loop).
