# Cache

The cache kind is jag. There is no ondemand index, no js5, and no interface archive. Config is not an opcode switch. `ClientConfig.call8fd2f9f5` walks `string.dat` and `integer.dat` as two sequential streams.

## Jag container

`Tools.readJag` (`Tools.java:339-374`) reads `u16be` file count, then one 10-byte entry per file:

| Bytes | Field |
| --- | --- |
| 0..3 | name hash, `u32be` |
| 4..6 | decompressed size, 24-bit big-endian |
| 7..9 | compressed size, 24-bit big-endian |

The payload starts at `2 + count * 10`. The name is uppercased. The hash is `hash * 61 + (char - 32)` for each character. When the two sizes differ, `BZip2.call11f2fbbd(dest, decompressedSize, archive, compressedSize, payloadOffset)` runs (`Tools.java:360`, declaration `BZip2.java:41`). When they match, the payload is copied raw. The bzip bitstream inside `call11f2fbbd` was not read. See [gaps.md](gaps.md).

`Tools.call2e3198b4` is the single-file form: a 6-byte header of two 24-bit sizes, then the body, bzip when the sizes differ (`Tools.java:236-279`).

## Archives loaded by `MudClient.load`

`menuOptionTarget` is `../release/` from `main` (`MudClient.java:825`).

| File | Version field | Use |
| --- | --- | --- |
| `config` + 71 + `.jag` | `Version.value71` | `ClientConfig.call8fd2f9f5` |
| `media` + 45 + `.jag` | `Version.value45` | 2d graphics |
| `entity` + 18 + `.jag` and `.mem` | `Version.value18` | people and monsters. The `.mem` load is the members path |
| `textures` + 14 + `.jag` | `Version.value14` | textures |
| `models` + 24 + `.jag` | `Version.ENTITY` | applet models. The application path loads `../gamedata/models/name.ob2` instead (`MudClient.java:1144-1166`) |
| `maps` + 50 + `.jag` and `.mem` | `Version.value50` | scenery `.dat` and `.loc` |
| `land` + 50 + `.jag` and `.mem` | `Version.value50` | height `.hei` |
| `sounds` + 1 + `.mem` | `Version.number9` | PCM names. See [sound.md](sound.md) |

`GameShell.loadJagex` reads `jagex.jag` through `Tools.call2e3198b4`, then `logo.tga` and `h11p.jf` from that archive (`GameShell.java:542-545`). The progress names in `load` are the strings passed to `loadJag`, including `Configuration`, `2d graphics`, `people and monsters`, `Textures`, `3d models`, `map`, `landscape`, and `Sound effects`.

## Config decode

`ClientConfig.call8fd2f9f5(byte[] archive, boolean members)` (`ClientConfig.java:248`). `string.dat` is a stream of bytes ending at 0, returned by `gjstr`. `integer.dat` is read by `g1` (`u8`), `g2` (`u16be`), `g4` (`u32be`), and `callb5b4491c`. `callb5b4491c` reads `g4` and, when the value is above 99999999, replaces it with `99999999 - value`.

Counts are one `u16be` (or two, for spells) and then parallel arrays. The sections, in order:

| Section | Count field | Per entry, in stream order |
| --- | --- | --- |
| items | `number476 = g2` | three `gjstr` (`textArray478`, `479`, `480`); `g2` sprite (`numberArray481`); `callb5b4491c` price (`482`); `g1` stackable (`483`, 0 means the inventory packet carries `gsmart`); `g1` (`484`); `g2` wield (`485`); `callb5b4491c` (`486`); `g1` untradable (`487`); `g1` members (`488`). A non-member client with `488 == 1` replaces the name and examine text and writes `numberArray484[0] = 0` (`ClientConfig.java:313-318`) |
| npcs | `number489 = g2` | two `gjstr` (`490`, `491`); five `g1` (`493` through `497`); 12 `g1` appearance, and 255 becomes -1 (`numberArrayArray498`); four `callb5b4491c` (`499` through `502`); two `g2` (`503`, `504`); three `g1` (`505` through `507`); one `gjstr` (`492`) (`ClientConfig.java:345-420`) |
| textures | `number508 = g2` | two `gjstr` (`509`, `510`) |
| entities / animations | `number511 = g2` | `gjstr` (`512`); `callb5b4491c` (`513`); four `g1` (`514` through `517`) |
| locs | `number518 = g2` | four `gjstr` (`519` through `522`); `getModelId` of one `gjstr` (`523`); four `g1` (`524` width, `525` height, `526`, `527`) |
| boundaries | `number528 = g2` | four `gjstr` (`529` through `532`); `g2` (`533`); two `callb5b4491c` (`534`, `535`); two `g1` (`536`, `537`) |
| tiles | `number538 = g2` | two `g1` arrays (`539`, `540`) |
| overlays | `number541 = g2` | `g4` (`542`), `g1` (`543`), `g1` (`544`) |
| spells | `number545 = g2`, then `number546 = g2` | two `gjstr`; three `g1` (`549` level, `550` reagent count, `551` target type); `g1` count then that many `g2` reagent ids; `g1` count then that many `g1` amounts |
| prayers | `number554 = g2` | two `gjstr`; two `g1` (`557` level, `558`) |

`number551` is the spell target used by the menu: 2 npc, 3 inventory, 4 boundary, 5 loc, 6 ground. Values at or below 1 add the self cast (`MudClient.java:8776`).

After the prayers, `words.txt` and `badwords.txt` are read from the same jag. Each is CRLF lines until the line `-EOF-`, stored for the chat dictionary (`Tools.hello`) and the bad-word list (`Tools.textArray`). `eOF` and `eOF2` consume those files (`ClientConfig.java:659-662`).

No component script is decoded. The GUI is built in code. See [interfaces.md](interfaces.md).
