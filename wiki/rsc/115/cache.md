# Cache

Cache kind is jag. There is no ondemand index and no js5. There is no interface archive and no client script archive.

`MudClient.load` fetches, with prefix `menuOptionTarget` (`../release/` from `main`, `MudClient.java:733`):

| file | progress label | citation |
| --- | --- | --- |
| `config48.jag` | Configuration | `deob/MudClient.java:823` |
| `media28.jag` | 2d graphics | `MudClient.java:836` |
| `entity10.jag` | people and monsters | `MudClient.java:890` |
| `textures8.jag` | Textures | `MudClient.java:941` |
| `models12.jag` | 3d models, applet path | `MudClient.java:992` |
| `maps27.jag` | map, applet path | `MudClient.java:1022` |

`GameShell.loadJagex` loads `jagex.jag` and pulls `logo.tga` plus the `h11p.jf`, `h12b.jf`, `h12p.jf`, `h13b.jf`, `h14b.jf`, and `h16b.jf` fonts (`jagex/client/GameShell.java:569`).

## Jag bytes

`GameShell.loadJag` (`GameShell.java:699`) reads a 6-byte header: u24be uncompressed size, u24be packed size, then the body. When the sizes differ, `BZip2.call11f2fbbd` inflates the body (`GameShell.java:741`).

`Tools.readJag` (`jagex/Tools.java:320`) reads u16be count, then 10-byte entries: hash u32be, uncompressed u24be, compressed u24be. The name is uppercased and `hash = hash * 61 + char - 32`. Payload follows the table. Unequal sizes call `BZip2.call11f2fbbd` (`deob/BZip2.java:41`), which constructs `jagex/BZipState.java` and calls `BZip2.call585d4622`. `BZipState` is the field bag for that inflater. The bit model inside `call585d4622` was not transcribed. See `gaps.md`.

## Config

`ClientConfig.call8fd2f9f5` (`deob/ClientConfig.java:242`) pulls `string.dat` and `integer.dat` from the config archive. Strings are `gjstr`: bytes until a 0, then the cursor steps past the 0 (`ClientConfig.java:232`). Integers use `g1` u8, `g2` u16be, and `callb5b4491c` u32be, and a value above 99999999 becomes `99999999 - value` (`ClientConfig.java:221`). Sections are sequential counts, not opcodes.

| count | per record, in order | citation |
| --- | --- | --- |
| `number433` items | three strings (name, examine, command), u16be sprite, u16be price, u8 `numberArray440`, u8 `numberArray441`, u16be `numberArray442`, special int `numberArray443` | `ClientConfig.java:247` |
| `number444` npcs | two strings, five u8, twelve u8 appearance (255 stored as -1), four special ints, two u16be, three u8 | `ClientConfig.java:297` |
| `number462` textures | two strings | `ClientConfig.java:389` |
| `number465` animations | string, special int, four u8 | `ClientConfig.java:401` |
| `number472` objects | four strings, model name through `getModelId`, four u8 (`numberArray478` width, `479` height, `480`, `481`) | `ClientConfig.java:433` |
| `number482` walls | four strings, u16be height, two special ints, two u8 | `ClientConfig.java:480` |
| `number492` | two u8 | `ClientConfig.java:527` |
| `number495` | special int, two u8 | `ClientConfig.java:539` |
| `number499` | count only | `ClientConfig.java:556` |
| `number500` spells | two strings, three u8, u8 count plus that many u16be, u8 count plus that many u8 | `ClientConfig.java:557` |
| `number508` prayers | two strings, two u8 | `ClientConfig.java:604` |

`numberArray440 == 0` is the case where inventory and bank packets read a `gsmart` count (`packets.md`).

`words.txt` lines end at CR, an optional LF is skipped, and `-EOF-` stops the file (`ClientConfig.java:635`). The strings become `Tools.hello`. `badwords.txt` uses the same line rule and then expands vowel variants into `Tools.textArray` (`ClientConfig.java:661`).

## Chat dictionary

`Tools.charValueArray2` is `A`..`Z`, `0`..`9`, space, `! ? . , : ; ( ) - & * \ '` (`jagex/Tools.java:32`).

`Tools.call18c46ebf` maps `a`..`z` to 0..25, `0`..`9` to 26..35, space 36, and the same punctuation to 37..49. Anything else is 36 (`Tools.java:473`).

`Tools.call3cc33792` (`Tools.java:358`) cuts the string to 80, lowercases it, and appends a space. A leading colour tag writes byte 255 and a code 0..9 for `@red@` through `@mag@` in the order red, gre, blu, cya, ran, whi, bla, ora, yel, mag, then removes the five-character tag. A run of letters, digits, or `'` is matched against `hello`. A match whose next symbol is space and whose index is below 165 writes one byte `index + 90`. A match with space and a larger index writes `index / 256 + 50` and `index & 255`. A match whose next symbol is not space writes `index / 256 + 70` and `index & 255`. An unmatched run is written as symbol bytes.

`Tools.call85aa3cc8` (`Tools.java:510`) reverses that. Bytes 0..49 append `charValueArray2`. Bytes 50..69 consume the next byte and append `hello[(b - 50) * 256 + next]` plus a space. Bytes 70..89 do the same without the space. Bytes 90..254 append `hello[b - 90]` plus a space. Byte 255 consumes a colour code and stores `@red@` through `@mag@` for 0..9. When the filter flag is true, `call7c884ded` runs up to twice. The result is cut to 80, lowercased, capitalised after the start and after `.`, `!`, or `?`, and the colour prefix is put back. An exception returns `eep!`.

Server opcode 8 is a raw string, not this dictionary (`packets.md`).
