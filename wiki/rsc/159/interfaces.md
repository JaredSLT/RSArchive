# Interfaces

There is no interface archive and no client script. Components are `Gui` objects built in `MudClient`. `number508` is 0 on the login screens and 1 in the game (`MudClient.call6b2975af` sets 1, `MudClient.call81ea3e37` sets 0). `number320` is the logged-out page: 0 welcome, 1 create account, 2 login, 3 recovery.

## Component types

`Gui.numberArray723` stores the type code. Constructors:

| Code | Method | Role |
| --- | --- | --- |
| 0 | `call236` | label (`Gui.java:649`) |
| 1 | `call237` | text (`Gui.java:662`) |
| 2 | `call238` | box (`Gui.java:675`) |
| 4 | `call241` | scroll list (`Gui.java:714`) |
| 5 | `call242` | text input (`Gui.java:732`) |
| 6 | `call243` | text input (`Gui.java:749`) |
| 9 | `call244` | list (`Gui.java:766`) |
| 10 | `call245` | button (`Gui.java:786`) |
| 11 | `call239` | box (`Gui.java:687`) |
| 12 | `calla8c08a6a` | sprite button (`Gui.java:699`) |
| 14 | `call35f29e05` | checkbox (`Gui.java:798`) |

`call255` focuses an input (`Gui.java:869`). No other type code is assigned by these constructors.

## Screens the client builds

| Screen | Builder | Notes |
| --- | --- | --- |
| chat box | `callc7d11387` | lists at y 269, size 502 by 56, 20 lines; input at (7, 324), size 498 by 14, max 80 (`MudClient.java:1205-1211`) |
| create account | `Gui` `gui324` | `number320` 1 |
| login | `gui333` | `number320` 2. Submit calls `Client.call25f5d914` |
| recovery | `gui374` | five question labels, five inputs, old password, new password, confirm. Submit is opcode 8 |
| security questions | `gui363` | opened by s2c 224 (`flag362`). Submit is opcode 208 |
| contact details | `gui388` | opened by s2c 232 (`flag387`). Four inputs, max 80. Submit is opcode 253 |
| design a character | `gui346` | opened by s2c 239 (`flag416`) and also prepared when s2c 224 is not the only designer path. Accept is opcode 236 |

Sidebar page `number161` is not a widget id. The values are in [input.md](input.md): 0 closed, 1 inventory, 2 map, 3 stats, 4 magic and prayer, 5 friends and ignore, 6 options.

Bank, shop, trade, duel, welcome, quest menu, and combat style are drawn by `PixMap` calls inside `MudClient`, not by a `Gui` type list. Their packets are in [packets.md](packets.md).

## Appearance defaults

The constructor (`MudClient.java:9442-9448`) sets `flag416` false, `number418` 1, `number419` 2, `number420` 2, `number421` 8, `number422` 14, `number424` 1. `number4172` and `number423` have no constructor write, so they stay 0. Opcode 236 writes those eight bytes in the order gender, head, body, legs, hair colour, top colour, bottom colour, skin (`MudClient.java:1955-1963`). Gender toggles through `3 - number424`, so the stored gender is 1 or 2 (`MudClient.java:1922`).

Colour tables at `MudClient.java:9449-9451`: `numberArray3` has 15 entries, `numberArray4` has 10, `numberArray5` has 5.

Quest names are the 39 strings in `treeGnomeVillageMembers` (`MudClient.java:9335-9337`). `value39` is 39. S2c opcode 226 writes one byte per quest.

`number293`, the bank page size, starts at 48 (`MudClient.java:9402`). S2c opcode 222 overwrites it.
