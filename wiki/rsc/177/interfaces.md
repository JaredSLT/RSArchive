# Interfaces

There is no RS2 component archive, no interface id, and no client script. UI is `jagex.client.Gui` immediate-mode widgets plus boolean flags on `MudClient`.

`Gui` holds parallel arrays for widget state (`jagex/client/Gui.java:6`). Call sites use `call90` for a text line, `call94` for a list, `call95` for a text field, `call97` for a scroll list, and `call108` to focus a field (`deob/MudClient.java:882`, `deob/MudClient.java:1298`). The switch inside those `Gui` methods was not read, so the widget type codes are unread. See [gaps](gaps.md).

## Panels

All eleven are `new Gui` in `MudClient`:

| field | line | constructed from |
| --- | --- | --- |
| gui1 | `deob/MudClient.java:882` | `load`, scroll list, friends panel |
| gui2 | `deob/MudClient.java:886` | `load`, scroll list beside gui1 |
| gui3 | `deob/MudClient.java:888` | `load`, scroll list beside gui2 |
| gui4 | `deob/MudClient.java:1299` | chat lists at y 269 and the input line at y 324 |
| gui5 | `deob/MudClient.java:2160` | welcome |
| gui6 | `deob/MudClient.java:2182` | register |
| gui7 | `deob/MudClient.java:2210` | login |
| gui8 | `deob/MudClient.java:1990` | appearance design. Accept sends opcode 236. |
| gui9 | `deob/MudClient.java:1592` | five recovery questions. Accept sends opcode 208. |
| gui10 | `deob/MudClient.java:1851` | recovery answers. Accept sends opcode 8. |
| gui11 | `deob/MudClient.java:1896` | four contact strings. Accept sends opcode 253. |

The titles drawn on gui2 and gui3 were not read.

## Flag windows

These are server opcodes that show or hide a window. Payloads are in [packets](packets.md).

| flag | opened by | closed by |
| --- | --- | --- |
| flag93 bank | 222 | 221, and the client also sends opcode 207 when it closes |
| flag92 shop | 234 | 233, and the client sends opcode 218 when it closes |
| flag87 trade | 238 | 237 |
| flag78 duel | 219 | 218 |
| flag90 trade confirm | 217 | |
| flag85 duel confirm | 208 | |
| flag104 sleep | 201 | 199 |
| flag96 welcome | 205 | |
| flag98 dialog | 204 or 203 | |
| flag103 appearance | 239 | |
| flag102 contact | 232 | |
| flag100 recovery form | 224 | |
| flag94 choice list | 246 | 245 |
| flag99 | 244 | |

The sidebar options block sends opcode 213 for camera, mouse, and sound, opcode 197, and opcode 247 (`deob/MudClient.java:13871`). The option strings for 197 and 247 were not read. Logout on that panel sends opcode 6 (`deob/MudClient.java:13861`).

Prayer toggles send opcode 212 to turn a prayer on and opcode 211 to turn it off (`deob/MudClient.java:13466`).
