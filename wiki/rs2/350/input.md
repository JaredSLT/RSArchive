# Input

Packets below are the C2S rows in [packets.md](packets.md). Width names are in [framing.md](framing.md).

## Mouse samples

`MouseTracking` records coordinates. `Client.updateGame` sends opcode 224 when tracking is enabled (`valueClient781`, set from the login success flag) and the button is down or at least 40 samples are queued (`Client.java:12330`).

Payload: `u8` 0, then samples, then `psize1`. The loop stops once 240 payload bytes have been written past the length position (`Client.java:12338`). Y is clamped to 0..502 and X to 0..764. Packed coordinate is `y*765+x`. An off-screen sample (`-1, -1`) uses packed value 524287.

| case | write |
| --- | --- |
| same pixel as the previous sample | no bytes; `valueClient638` increments, capped at 2047 |
| idle below 8 and both deltas inside -32..31 | `u16be` of `(idle<<12)\|(dx+32<<6)\|(dz+32)` |
| idle below 8 | `u24be` of `8388608 + (idle<<19) + packed` |
| otherwise | `u32be` of `-1073741824 + (idle<<19) + packed` |

## Click

Opcode 150 (`Client.java:12428`): one `u32be` of `(dt<<20) + (right<<19) + (y*765+x)`. `dt` is `(now - last) / 50`, capped at 4095. Right click is 1 when `valueGameShell32 == 2`. Y is clamped to 0..502 and X to 0..764.

## Camera

Opcode 130 every 20 cycles while a camera-drag flag is set (`Client.java:12440`): `u16le+128` of `valueClient549`, `u16be` of `valueClient550`.

Opcode 139 (`Client.java:12448`): `u8` 1 when the applet gains focus, `u8` 0 when it loses focus.

## Keys

`Client.handleInputKey` (`Client.java:8554` and the following branches):

| input | opcode | payload |
| --- | --- | --- |
| private message | 168 | `u8` 0, `psize1` of `u64be` recipient plus `WordPack.opkind772` of the text. The encoder body is unread |
| amount dialog | 86 | `u32be` |
| name dialog | 200 | `u64be` of the name hash |
| command starting with `::` | 230 | `u8` of `text.length()-1`, then `jstr` of `text.substring(2)`. The length counts the terminator byte 10 that `jstr` appends |
| public chat | 169 | `u8` 0, `psize1` of WordPack bytes each plus 128, then `u8sub` effect and `u8` color |

Public, private, and trade chat modes are opcode 124: three `u8` values (`Client.java:2747`). The same packet is also written from `handleInputKey` when a mode change follows a private or public send.

## Walk, run, minimap

`Client.opClient92` (`Client.java:947`) writes the walk after pathing. `n` is the waypoint count capped at 25. The first payload byte is a length, and it is not a frame header.

| `var1` | opcode | length value | tail |
| --- | --- | --- | --- |
| 0 | 246 | `n*2+3` | none |
| 1 | 147 | `n*2+3+14` | 14 bytes from `opClient101` |
| 2 | 216 | `n*2+3` | none |

Shared body: `u8+128` run flag (`valueGameShell36[5]==1`), `u16le` destination Z (`last waypoint z + valueClient593`), then for each earlier waypoint `u8+128` dx and `u8neg` dz, then `u16le` destination X (`first waypoint x + valueClient592`).

A one-waypoint map walk has length value 5 and total payload 6, because the length byte is extra. `rslib/src/both/move/rev_350.c` `walk_encode_350` omits that byte and the minimap tail. See [gaps.md](gaps.md).

Minimap tail, only after opcode 147 (`Client.java:2026`): `u8` local click x, `u8` local click y, `u16be` camera yaw, `u8` 57, `u8` `valueClient659`, `u8` `valueClient546`, `u8` 89, `u16be` player fine X, `u16be` player fine Z, `u8` `valueClient937`, `u8` 63.

Scene clicks call `opClient92` with `var1==2` before the interaction opcode. Minimap clicks call it with `var1==1`. A plain tile click uses `var1==0`.

Opcode 35 is an empty packet from `opClient102` when the local player is already on the clicked tile (`Client.java:2073`).

## Widget and world ops

`Client.useMenuOption` (`Client.java:5407`) writes one interaction opcode after the optional walk. The field order is the C2S rows in [packets.md](packets.md). Drag-and-drop is opcode 109 from `updateGame` (`Client.java:12548`): `u8` insert mode, `u16be+128` component, `u16be+128` destination slot, `u16le` source slot.

## Other timed writes

| opcode | when | payload |
| --- | --- | --- |
| 1 | `valueClient779` passes 153 | `u16be` 26173 (`Client.java:12628`) |
| 63 | `valueGameShell24` passes 4500 | none (`Client.java:12636`) |
| 58 | scene build, and every 50 cycles in `updateGame` | none (`Client.java:2328`, `Client.java:12711`) |
| 84 | `drawGame` | `u32be` 0 (`Client.java:10999`) |
| 193 | `opClient82` | `u32be` 0 (`Client.java:10565`) |
| 225 | tab redraw | `u8` selected tab (`Client.java:11115`) |
| 210 | scene finished | none (`Client.java:8870`) |
| 173 | `buildScene` when a frame is active | `u32be` 1057001181 (`Client.java:2452`) |
| 127 | loc menu anticheat after 1184 hits | `u8` 0 plus `psize1` of random `u16be` and `u8` (`Client.java:997`) |
| 119 | zone anticheat after 1896 hits | same length pattern (`Client.java:2700`) |
