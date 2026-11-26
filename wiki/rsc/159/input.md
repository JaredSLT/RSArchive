# Input

## Mouse

`GameShell.mouseMove` stores x in `number1018` and `y + number996` in `number1019`, and clears `number1020` (`GameShell.java:313-318`). `mouseDown` sets `number1020` to 2 when `metaDown` is set and to 1 otherwise, and copies that into `number1021` (`GameShell.java:328-338`). `mouseUp` clears `number1020`. `mouseDrag` updates the same button bit without latching `number1021`.

`MudClient.update` clears `number1021` and `number1023` at the end of the tick (`MudClient.java:1231-1232`). A logged-in right click (`number1021 == 2`) sets `number57` to 2 (`MudClient.java:2888-2889`). Menu activation uses `number57 == 1`.

## Keys

`GameShell.keyDown` (`GameShell.java:190-265`):

| Code | Effect |
| --- | --- |
| 1006 | `flag1011` true until keyUp |
| 1007 | `flag1012` true until keyUp |
| 1004 | `flag1013` true. No reader in `MudClient` |
| 1005 | `flag1014` true. No reader in `MudClient` |
| space | `flag1015`. No reader in `MudClient` |
| `n`, `m`, `N`, `M` | `flag1016`. No reader in `MudClient` |
| `{` | `flag1009`. No reader in `MudClient` |
| `}` | `flag1010`. No reader in `MudClient` |
| 1008 | toggles `flag1024`. Logged-in draw copies it to `drawArea.flag867` (`MudClient.java:4815`) |
| backspace 8 | deletes one character from `text1025` and `text1027` |
| 10 or 13 | copies `text1025` to `text1026` and `text1027` to `text1028` |

`text1025` stops at 20 characters. `text1027` stops at 80. Any key also clears `number997`, the idle counter.

## Camera

`flag196` starts true (`MudClient.java:9340`). While it is true, key 1006 adds 1 to `number114` modulo 8, and key 1007 adds 7 modulo 8. The client then skips odd values and calls `call77329c68` so the step is not blocked (`MudClient.java:2894-2922`). Yaw `number116` animates toward `number114 * 32` (`MudClient.java:4839` is one assignment site; the interpolate is in the same tick). While `flag196` is false, key 1006 adds 2 to `number116` and key 1007 subtracts 2, both modulo 256 (`MudClient.java:2924-2927`).

`number116` starts at 128 (`MudClient.java:9285`). The draw passes `number116 * 4` as the yaw argument of `World3D.callf90da9a8`, with pitch 912 and roll 0 (`MudClient.java:4862`).

Zoom `value550` starts at 550 (`MudClient.java:9282`). While `flag111` is set and `value550` is above 550 it decreases by 4. Otherwise, while it is below 750, it increases by 4 (`MudClient.java:2930-2933`). There is no run toggle and no run packet.

## Walk

`MudClient.call4534748f` (`MudClient.java:5427-5455`). `World.call3cf8018f` fills `numberArray59` and `numberArray60`. A return of -1 sends nothing. Otherwise the client decrements the length twice, so the `u16be` pair is the point before the last path point, in path space, and the loop writes at most 25 earlier points.

`var8` true writes opcode 215. `var8` false writes opcode 194. Both then write:

| Order | Field |
| --- | --- |
| 1 | `u16be` of `destX + menuAction` |
| 2 | `u16be` of `destY + number484` |
| 3 | up to 25 pairs of `u8` `pathX - destX` and `u8` `pathY - destY`, walking backward from the point before the destination |

`call0311411f` forwards its last boolean as `var8` (`MudClient.java:5367-5368`). Menu 920 "Walk here" calls it with false, so opcode 194 (`MudClient.java:9244-9245`). The minimap click also calls it with false (`MudClient.java:7736-7737`). The click point is the scene pixel divided by 128.

`calldfe4e065` tries the path with the approach flag false inside the pathfinder, and if that fails tries again with that flag true. The packet flag stays the caller's boolean (`MudClient.java:5372-5375`). Object and boundary menus pass true, so they send opcode 215 before the action opcode.

Menu 900 "Cast on ground" sends opcode 215 and then opcode 221. Opcode 221 is not the walk packet. `rslib/src/client/packets/159.c` binds `RS_C2S_WALK` to 221. See [gaps.md](gaps.md).

## Sidebar and chat

`number161` (`MudClient.call581a7c93`, `MudClient.java:7482-7543`): 0 closed, 1 inventory, 2 map, 3 stats, 4 magic or prayer, 5 friends or ignore, 6 options. The icons sit on the right, 33 pixels apart, from `number843 - 35` leftward.

`number224`: 0 all, 1 chat history, 2 quest history, 3 private history (`MudClient.java:2802-2816`). Report abuse sets `number300` to 1.

Public chat is the Gui input `number221`. On submit, a string that starts with `::` is opcode 7 (`Client.callce3f88fa`, `rawstr` of the text after `::`) except `::lostcon` closes the socket and `::closecon` sends opcode 1, and those two only when `flag54` is false (`MudClient.java:2828-2840`). Any other line is `Tools.call3cc33792` then c2s opcode 3. Private messages are opcode 28. Friends, ignore, privacy, prayers, options, trade, duel, bank, and shop clicks are the opcodes in [packets.md](packets.md).

Combat style, when the local `number18` is 8 or 9, sends opcode 231 with `p1(styleIndex - 1)` (`MudClient.java:5755-5756`). The labels are Controlled, Aggressive, Accurate, and Defensive (`MudClient.java:5774-5778`).

Options opcode 213 (`MudClient.java:8304-8324`): `u8` 0 and `u8` 0 or 1 toggles `flag196`; `u8` 2 toggles `flag197`; `u8` 3 toggles `flag198` on a members client. Privacy opcode 31 sends `number701`, `number702`, `number703`, `number704` (`MudClient.java:8376`).
