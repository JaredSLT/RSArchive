# Input

## Mouse

`GameShell.mouseDown` stores x and `y + number952`. Meta sets `number975` and `number976` to 2. Otherwise both are 1. It calls `onMouseClick` (`jagex/client/GameShell.java:349`). `mouseUp` clears `number975` (`GameShell.java:342`). `mouseMove` clears `number975` and `number953` (`GameShell.java:334`). `mouseDrag` keeps the button (`GameShell.java:367`). `MudClient.load` sets `number952 = 0` (`deob/MudClient.java:759`).

`MudClient.onMouseClick` records the point in an 8192-slot trail (`MudClient.java:1243`). The in-game tick copies `number975` into `number55` and then the shell clears `number976` at the end of `update` (`MudClient.java:1056`).

## Keys

`GameShell.keyDown` (`GameShell.java:220`) stores these codes:

| code | field |
| --- | --- |
| 1006 | `flag966` |
| 1007 | `flag967` |
| 1004 | `flag968` |
| 1005 | `flag969` |
| space | `flag970` |
| `n`, `m`, `N`, `M` | `flag971` |
| `{` | `flag964` |
| `}` | `flag965` |
| 1008 | toggles `flag979` |

`MudClient` reads `flag966` and `flag967` to step the camera (`MudClient.java:2576`). The other flags in that table are written and have no reader in `MudClient`. `text980` accepts letters, digits, and space up to length 20. `text982` accepts codes 32..122 up to length 80. Backspace is 8. Enter is 10 or 13 and copies the buffers to `text981` and `text983`. `MudClient.onKeyPress` forwards the code to the active `Gui` (`MudClient.java:1198`).

## Camera

`flag189` starts true (`MudClient.java:8425`). Server opcode 228 and client opcode 213 toggle it (`packets.md`). `number110` starts at 1 (`MudClient.java:8371`). `number112` starts at 128 and is a 0..255 yaw. Auto camera chases `number110 * 32` (`MudClient.java:2460` area and the render at `MudClient.java:4388`). Manual yaw changes `number112` by 2. `value550` starts at 550 (`MudClient.java:8369`). Render distances are in `scene.md`.

## Walk

`MudClient.call4534748f` (`MudClient.java:4954`) asks `World.call3cf8018f` for a step count, decrements it so the last point is the destination, and decrements again before the waypoint loop. `var8` true writes opcode 215. `var8` false writes opcode 255. Both write u16be `destX + number414`, u16be `destY + number184`, then up to 25 earlier points as `p1(pointX - destX)`, `p1(pointY - destY)`. There is no run byte.

`call0311411f` passes its boolean straight through as `var8` (`MudClient.java:4894`). Minimap click uses false, so opcode 255 (`MudClient.java:6906`). Menu 920 uses false (`MudClient.java:8331`). Menu walks that call `calldfe4e065`, `callf6d9a7ee`, or `call5473ac68` pass true, so opcode 215, and then write their own opcode (`MudClient.java:8049`). Menu 900 walks with true and then writes opcode 221 (`MudClient.java:8322`). NPC and player walks convert pixels with `(coord - 64) / 128` before the walk (`MudClient.java:8229`).

## Chat and widgets that send packets

The chat field is `Gui` id `number213` (`MudClient.java:1035`). Enter on text that starts with `::` sends opcode 7 with the substring after `::`, except `::lostcon` closes the socket and `::closecon` sends opcode 1 (`MudClient.java:2512`). Other text is dictionary-compressed and sent as opcode 3.

Sidebar `number157` (`MudClient.call581a7c93`, `MudClient.java:6651`): 1 inventory, 2 minimap, 3 stats and quests, 4 magic and prayers, 5 friends, 6 options. Tabs are 32 pixels wide, stepping left by 33 from `number798 - 35`.

Inventory menu rows (`MudClient.java:8175` through `8222`): 600 opcode 220, 610 opcode 240, 620 opcode 248, 630 opcode 249, 640 opcode 246, 650 selects an item and sends nothing, 660 opcode 251. Friends add, remove, ignore add, ignore remove, and private message use opcodes 26, 27, 29, 30, and 28 (`MudClient.java:5486`, `7319`, `7332`, `5504`). Privacy confirm sends opcode 31 (`MudClient.java:7490`). Bank, shop, trade, and duel opcodes are the c2s rows in `packets.md`. Option menu opcode 237 and combat style opcode 231 are there too. Logout request opcode 6 is `callde2a6a6a` (`MudClient.java:1293`), also fired when `number953` passes 4500 (`MudClient.java:2225`).
