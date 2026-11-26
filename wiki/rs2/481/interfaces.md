# Interfaces

JS5 index 3 is `WorldFull.valuekind29111`, stored as `C1fd292b8.valuekind2605` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/C1e98a145.java:17`). `C1e98a145.opkind3303` loads every file in the group. If `data[0]` is not 0 it calls `SoftwareFont.opSoftwareFont48`. Otherwise it calls `opSoftwareFont41` (`C1e98a145.java:47`).

The root id is `StringInterface.valueStringInterface61`. Login sets it to -1 (`ConnectionTimedOut.java:133`). Opcode 35 and the first `u16be` of opcode 245 replace it and call `C63d74d54.opkind2844` plus `ClickSwitch.opkind3312`. Sidebar hashes are `WorldFull.opkind2914`, fed by opcodes 245, 18, and 243. No numeric sidebar id is hardcoded in those methods.

## `opSoftwareFont41`

`builds/rs2/481-2007-12-11/src/src/main/java/deob/SoftwareFont.java:3113`. One discarded `u8`, then `valueSoftwareFont181 = true`, type `u8`, content `u16be`, x `i16be`, y `i16be`, width `u16be`, height `u16be` (type 9 uses `i16be` for the height), parent `u16be` (65535 becomes -1, otherwise packed with the interface id), hidden `u8 == 1`.

| Type | Body |
| --- | --- |
| 0 | two `u16be` |
| 3 | `u32be`, `u8 == 1`, `u8` |
| 4 | `u16be` font (65535 becomes -1), `jstr`, four `u8` with the fourth tested `== 1`, `u32be` |
| 5 | `u32be`, `u16be`, `u8 == 1`, `u8`, `u8`, `u32be`, `u8 == 1`, `u8 == 1` |
| 6 | model mode forced to 1, `u16be` (65535 becomes -1), two `i16be`, five `u16be` (one 65535 becomes -1), `u8 == 1` |
| 9 | `u8`, `u32be` |

After the type body, every component reads `u24be`, `jstr`, a `u8` count of `jstr`, three `u8`, one `jstr`, eighteen `opSoftwareFont51` scripts, and three `opSoftwareFont37` int arrays (lines 3197-3232).

`opSoftwareFont51` (`SoftwareFont.java:3638`) is `u8` count, then per entry `u8` kind. Kind 0 is `u32be`. Kind 1 is `jstr`. Count 0 returns null.

`opSoftwareFont37` (`SoftwareFont.java:3019`) is `u8` count, then that many `u32be`. Count 0 returns null.

`opSoftwareFont48` starts at line 3372. The bytes read there are type `u8`, `u8`, content `u16be`, x `i16be`, y `i16be`, width `u16be`, height `u16be`, `u8`, parent `u16be` (65535 becomes -1). The method continues and was not read to its end.

`C63d74d54.opkind2844` starts at line 558 and was not read past the call to `opkind3303`.
