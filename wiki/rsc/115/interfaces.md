# Interfaces

This client has no component archive and no script interpreter. `Gui` is an immediate-mode list. The type stored in `numberArray678` is the integer in each factory (`jagex/client/Gui.java:649`):

| type | factory | what it builds |
| --- | --- | --- |
| 0 | `call228` | text |
| 1 | `call229` | centered text |
| 2 | `call230` | box |
| 4 | `call233` | scroll list |
| 5 | `call234` | text field |
| 6 | `call235` | text field, same storage as type 5 |
| 8 | `call236` | option list |
| 9 | `call237` | list |
| 10 | `call238` | button |
| 11 | `call231` | inset box |
| 12 | `calla8c08a6a` | sprite |
| 14 | `call35f29e05` | square |

There are no top-level interface ids. Screens are fields on `MudClient`.

Login `number298` (`MudClient.call9fbda8f4` starts at `deob/MudClient.java:1672`, and the recovery screens follow in that method): 0 welcome, 1 create account, 2 username and password, 3 recovery. In-game `menuOptionType` is 0 on those screens and 1 after login success (`MudClient.java:2156`).

Sidebar `number157` (`MudClient.java:6651`):

| value | panel | sprite | close test |
| --- | --- | --- | --- |
| 0 | closed | | |
| 1 | inventory, `call5523c487` | `value2000 + 1` | x left of `number798 - 248`, or y below `36 + value30 / 5 * 34` |
| 2 | minimap, `call42a9d5e8` | `value2000 + 2` | x left of `number798 - 199`, or y below 240 |
| 3 | stats and quests, `call9c2cde54` | `value2000 + 3` | x left of `number798 - 199`, or y below 284 |
| 4 | magic and prayers, `call00106137` | `value2000 + 4` | same x, y below 240 |
| 5 | friends | | same x, y below 240 |
| 6 | options | | same x, y below 252 |

`value30` is 30 (`MudClient.java:8406`). `value2000` is 2000 (`MudClient.java:772`). Inventory cells are 49 by 34 in five columns (`MudClient.java:6723`). The minimap panel is 156 by 152, scale 192, and the compass uses `number112` (`MudClient.java:6888`). Stats and magic each have a two-way selector, `number183` and `number442`.

Chat `number216` is 0 all, 1 history, 2 quest, 3 private. The lists sit at y 269, width 502, height 56, and the input sits at y 324 (`MudClient.java:1032`). Tab hits require `number974 > value334 - 4` and the x ranges at `MudClient.java:2483`.

Server opcode 239 shows the appearance designer (`flag383`). Server opcode 224 shows the security-question screen (`flag340`). Server opcodes 246 and 245 open and close the five-string option menu. Those screens are built in Java, not loaded as interface ids. The packets they send are in `packets.md` and `input.md`.
