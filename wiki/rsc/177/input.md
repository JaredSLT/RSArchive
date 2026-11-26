# Input

`GameShell` is the AWT shell. `run` calls `update` then `draw` (`jagex/client/GameShell.java:415`). `MudClient.update` clears `mouseClickButton` and `keyHeld` after the login or game step (`deob/MudClient.java:1324`).

`onKeyPress` is `deob/MudClient.java:1494`. `onMouseClick(int, int, int)` is a fernflower stub (`deob/MudClient.java:1544`). Click coordinates used by the UI are `mouseX`, `mouseY`, and `mouseClickButton` / `number374` after that stub. The stub body was not disassembled. See [gaps](gaps.md).

There is no run byte and no minimap opcode. Camera rotation mode is settings opcode 213 id 0. One-button mouse is opcode 213 id 2. See [packets](packets.md).

## Keys

Chat input is `gui4` component `number470` (`deob/MudClient.java:1298`). While the sleep flag `flag104` is set, the word in `text26` is opcode 193, or the literal `-null-` when the click is in the sleep band. That path is `deob/MudClient.java:3096`. Opcode 193 is not public chat.

Public chat is `WordPack.call353` into a 100-byte buffer, then `Client.call8d16abda`, opcode 3, `pdata` of that buffer (`deob/MudClient.java:3169`, `jagex/client/Client.java:611`). WordPack stores 4-bit dictionary indexes (`jagex/WordPack.java:15`). A nibble below 13 is `charValueArray4[nibble]`. Any other nibble is the high half of an index completed by the next nibble: `(high << 4) + low - 195`.

A line that starts with `::` is opcode 7, `pjstr` of the text after the two colons (`Client.callce3f88fa`, `jagex/client/Client.java:618`). `::lostcon` closes the stream when not an applet. `::closecon` calls session logout opcode 1.

Idle above 4500, with `value5004 == 0` and `number509 == 0`, calls `callde2a6a6a`, which sends opcode 6 (`deob/MudClient.java:2836`, `deob/MudClient.java:1574`).

## Walk

Menu action 920 is Walk here. It calls `call0311411f` with the action flag false and writes no opcode of its own (`deob/MudClient.java:15088`). `call0311411f` calls `call4534748f` with that flag (`deob/MudClient.java:9563`).

`call4534748f` pathfinds with `World.call3cf8018f` (`deob/MudClient.java:9629`). On success it decrements the returned index, takes that waypoint as the base, decrements again, and writes:

- opcode 215 threat 592 when the action flag is true, else opcode 194 threat 770
- u16be `baseX + number410`
- u16be `baseY + number411`
- then, walking the index downward while it stays at least 0 and greater than `index - 25`: u8 `pathX - baseX`, u8 `pathY - baseY`

`calldfe4e065` retries the pathfind with the other boolean if the first attempt fails (`deob/MudClient.java:9568`). Loc approach uses the action flag true, so opcode 215, and swaps loc width and length unless the loc direction is 0 or 4 (`deob/MudClient.java:9575`).

NPC and player clicks convert pixel `(n - 64) / 128` to a tile, walk with the action flag true, and send the target's server index in the action packet. They do not send the tile as the action payload. Menu ids are in [packets](packets.md).

## Right click

`calld81ac5fe` (`deob/MudClient.java:14797`) is the menu sender. Actions that only set local state and write no packet: 650 select item, 920 walk, 3200 examine ground item, 3300 examine bound, 3400 examine loc, 3600 examine inventory, 3700 examine npc, 4000 cancel.

Combat style clicks send opcode 231, u8 style, after the UI subtracts 1 (`deob/MudClient.java:9922`). Choice-list rows send opcode 237, u8 row (`deob/MudClient.java:9878`).

Privacy toggles call `Client.callcab44984`, opcode 31, four u8 (`deob/MudClient.java:13920`, `jagex/client/Client.java:490`).
