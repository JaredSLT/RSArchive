# Interfaces

Citations are under `builds/rs2/525-2008-12-15/src/src/main/java/`. Component class `C0236c255`.

## Ids the client opens

`Client.java:1379` sets `C1f64ef12.valuekind5555` to the group id of the name `loginscreen` (`Ced8d3620.java:16`) inside index 3 (`Ca157093a.valuekind36713`). `Ca3682c6a.java:38` copies that id into the root `C0db1dd64.valuekind4949`.

S2C 230, size 5 (`DelayedStateChange.java:1660-1669`): `u16be` checked with `C4d4e728c.opkind4584`, `u16be` stored as the root `C0db1dd64.valuekind4949`, `u8_neg`. If the third byte is 2 the client calls `MouseRecorder.opMouseRecorder64`. No numeric sidebar id is assigned in this client. Content type `valuekind497101` is special at 1337 (3d view, `Client.java:196`), 1338 (`Client.java:202`, `LightType.java:122`), and 1400 (`Client.java:210`, `LightType.java:186`).

## If3 decode `opkind49712`

`C0236c255.opkind49712` (`C0236c255.java:607-858`). `valuekind49760` is set false.

Common header: `u8` type, `u8` button type, `u16be` content type, `i16be` x, `i16be` y, `u16be` width, `u16be` height, `u8`, `u16be` parent (65535 means -1, otherwise add `valuekind497133 & 0xFFFF0000`), `u16be` (65535 means -1), `u8` count of (`u8`, `u16be`) pairs, `u8` count of arrays of `u16be` (65535 means -1).

| Type | Further reads |
| --- | --- |
| 0 | `u16be`, `u8` |
| 1 | `u16be`, `u8`, and the type-4 font block below |
| 2 | four `u8` flags, two `u8`, 20 slots of `u8` plus (`i16be`, `i16be`, `u32be`) when that `u8` is 1, five `jstr` |
| 3 | `u8`, then the colour block |
| 4 | font block, two `jstr`, colour block |
| 5 | `u32be`, `u32be` |
| 6 | seven `u16be`. The first, second, third, and fourth become -1 when they are 65535 (`valuekind49720`, `valuekind49753`, `valuekind497106`, `valuekind497131`). The last three are stored as read (`C0236c255.java:760-785`) |
| 7 | inventory arrays sized `width * height`, `u8`, `u16be` font, `u8`, `u32be`, `i16be`, `i16be`, `u8`, five `jstr` |
| 8 | `jstr` |

The type-4 font block, shared with type 1 (`C0236c255.java:728-737`): `u8`, `u8`, `u8`, `u16be` (65535 means -1), `u8`. Types 1, 3, and 4 then take a `u32be` colour (`C0236c255.java:745-746`). Types 3 and 4 take three more `u32be` (`C0236c255.java:749-752`).

If button type is 2 or the component type is 2: two `jstr` and a `u16be` whose low 6 bits are shifted into the flags (`C0236c255.java:823-827`). Button types 1, 4, 5, and 6 read another `jstr` (`C0236c255.java:830-847`).

## Older decode `opkind4979`

`C0236c255.opkind4979` (`C0236c255.java:435-590`). `valuekind49760` is set true. Skip one byte, then `u8` type, `u16be` content, `i16be` x, `i16be` y, `u16be` width, `u16be` height, four `i8`, `u16be` parent with the same 65535 rule, `u8`.

| Type | Further reads |
| --- | --- |
| 0 | `u16be`, `u16be`, `u8` |
| 5 | `u32be`, `u16be`, `u8` (bit 1 and bit 0), `u8`, `u8`, `u32be`, `u8`, `u8` |
| 6 | `u16be` model (65535 means -1), `i16be`, `i16be`, `u16be`, `u16be`, `u16be`, `u16be`, `u16be` (65535 means -1), `u8`, then two skipped `u16be` and one skipped `u8`, then a `u16be` if the first `i8` is non-zero and a `u16be` if the second `i8` is non-zero |
| 4 | `u16be` font (65535 means -1), `jstr`, `u8`, `u8`, `u8`, `u8`, `u32be` |
| 3 | `u32be`, `u8`, `u8` |
| 9 | `u8`, `u32be`, `u8` |

Then `u24be` flags, `u8` count of `i8`, an optional second `u8` count of `i8`, `jstr`, `u8` count of `jstr`, three `u8`, `jstr`, 21 script blocks from `call334d9080`, and five int arrays from `opkind49710`.

## Scripts

`call334d9080` (`C0236c255.java:972-992`): `u8` count. 0 means null. Each entry is `u8` kind. Kind 0 is `u32be`. Kind 1 is `jstr`. Any other kind is left unread by this method.

`opkind49710` (`C0236c255.java:592-604`): `u8` count. 0 means null. Else that many `u32be`.

S2C 144, size `-2` (`DelayedStateChange.java:1599-1617`): `u16be` checked with `opkind4584`, `jstr` whose characters select the following values (`s`, byte 115, selects `jstr`, anything else selects `u32be`), then one `u32be`. The object array is run by `LightType.opLightType23`. That runner was not transcribed.

S2C 206, size `-2`, is the reflection payload in `ComponentPointer.opComponentPointer5` (`ComponentPointer.java:20-88`): `u8` count, `u32be`, then per entry a `u8` kind. Kinds 0, 1, and 2 are two `jstr`, plus a `u32be` when the kind is 1. Kinds 3 and 4 are two `jstr`, a `u8` argument count, that many `jstr`, and, for kind 3, that many (`u32be` length, `bytes(length)`).
