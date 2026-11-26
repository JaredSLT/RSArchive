# Packets

Opcode byte on both directions is ISAAC-adjusted. See [framing.md](framing.md). Width names are defined there. S2C `size` is `C994ec50d.valuekind1192[opcode]` for opcodes 0 through 255. This client has no C2S size array, so a C2S `size` is the payload byte count after the opcode, or `-1` when the writer emits a length byte or `psize1`.

An S2C row with an empty field list has no branch in `Client.read`. The frame reader has already consumed `size` bytes. The fallthrough at `Client.read` logs `T1` and calls `opClient80`.

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 0 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 1 | 4 | s16le, u16le | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4843 |
| s2c | 2 | 7 | u8 pos, u16be id, u16be oldCount, u16be newCount | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6901 |
| s2c | 3 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 4 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 5 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5266 |
| s2c | 6 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 7 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 8 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 9 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 10 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 11 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 12 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 13 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 14 | 2 | u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5035 |
| s2c | 15 | 4 | u16be, u16be+128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4623 |
| s2c | 16 | -2 | u8-128 zoneZ, u8sub zoneX, then u8 subopcode repeated until the payload ends; each subopcode is Client.readZonePacket | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5235 |
| s2c | 17 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 18 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 19 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 20 | -2 | bits: for level 0..3, x 0..12, z 0..12: bits(1), and if set bits(26); then byte-align, u16be zoneX, u16le zoneZ | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4121 |
| s2c | 21 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 22 | 5 | u24mix, u16le | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4030 |
| s2c | 23 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 24 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 25 | 1 | u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4579 |
| s2c | 26 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 27 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 28 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 29 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 30 | 2 | u16be+128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5319 |
| s2c | 31 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 32 | 6 | u16be, u32mixB | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5272 |
| s2c | 33 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 34 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 35 | 4 | u16be+128, u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4541 |
| s2c | 36 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 37 | -2 | see sync.md | Client.readPlayerInfo | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:1084 |
| s2c | 38 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 39 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 40 | 4 | u8 pos, u16be, u8 | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6770 |
| s2c | 41 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 42 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 43 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 44 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 45 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 46 | 4 | u16be, u16le | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4832 |
| s2c | 47 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 48 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 49 | 3 | u8-128, u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4409 |
| s2c | 50 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 51 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 52 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4927 |
| s2c | 53 | 4 | u16le+128, u16le | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4400 |
| s2c | 54 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 55 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 56 | 8 | u16be+128, u16le, u16be, u16le+128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4057 |
| s2c | 57 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 58 | -2 | (size/8) times u64be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4444 |
| s2c | 59 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 60 | 2 | s16be+128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4526 |
| s2c | 61 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 62 | 4 | u16be, u16be+128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4341 |
| s2c | 63 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 64 | 6 | u16be, s16le, s16le+128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4561 |
| s2c | 65 | 7 | u8 pos, u16be+128, u16be, u16be | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6922 |
| s2c | 66 | 2 | u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4674 |
| s2c | 67 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 68 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 69 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 70 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 71 | 2 | u16le | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5100 |
| s2c | 72 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 73 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 74 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 75 | 6 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 76 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 77 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 78 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 79 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 80 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 81 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 82 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 83 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 84 | 4 | u16be+128, u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4687 |
| s2c | 85 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 86 | 4 | u8, u8, u8, u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4512 |
| s2c | 87 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 88 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 89 | -2 | u16be, then while bytes remain: smart, u16be, u8 or u32be when that u8 is 255 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4069 |
| s2c | 90 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 91 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 92 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 93 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 94 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 95 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 96 | 6 | u8, u8, u16be, u8, u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5289 |
| s2c | 97 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 98 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 99 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 100 | 1 | u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4007 |
| s2c | 101 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 102 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 103 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 104 | 5 | u16be, u8, u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4912 |
| s2c | 105 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 106 | 6 | u16le, u16be, u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4603 |
| s2c | 107 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 108 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 109 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 110 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 111 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 112 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 113 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 114 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5357 |
| s2c | 115 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 116 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 117 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 118 | 2 | u16be+128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4942 |
| s2c | 119 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 120 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 121 | 3 | u8sub pos, u16le+128 | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6961 |
| s2c | 122 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 123 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 124 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4933 |
| s2c | 125 | 1 | u8-128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5258 |
| s2c | 126 | 4 | u16be, u8neg, u8-128 | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6702 |
| s2c | 127 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 128 | 6 | u32le, u8-128, u8sub | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4455 |
| s2c | 129 | 1 | u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4884 |
| s2c | 130 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 131 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 132 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 133 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 134 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 135 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 136 | 3 | u8sub, u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4104 |
| s2c | 137 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4474 |
| s2c | 138 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 139 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 140 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 141 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 142 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 143 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 144 | 2 | s16le | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4900 |
| s2c | 145 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 146 | 15 | u8 pos, s8, s8, s16be, u16be, u8, u8, u16be, u16be, u8, u8 | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6877 |
| s2c | 147 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 148 | 3 | u8, u8, u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4485 |
| s2c | 149 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 150 | 3 | s8, u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4427 |
| s2c | 151 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 152 | 4 | u16le+128 zoneZ, u16le+128 zoneX | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4115 |
| s2c | 153 | 6 | u8 type; type 1 then u16be; types 2..6 then u16be, u16be, u8; type 10 then u16be. The frame is fixed at 6 bytes; bytes the branch does not read stay unread in the payload buffer | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5182 |
| s2c | 154 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 155 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 156 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 157 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 158 | 6 | u16be, u16le+128, u16le | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4823 |
| s2c | 159 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 160 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 161 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 162 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 163 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 164 | 2 | u8neg pos, u8 | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6786 |
| s2c | 165 | 1 | u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4874 |
| s2c | 166 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 167 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 168 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4857 |
| s2c | 169 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 170 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 171 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 172 | -2 | see sync.md | Client.readNpcInfo | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:7449 |
| s2c | 173 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 174 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 175 | 6 | u8 pos, u16be, u8, u16be | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6755 |
| s2c | 176 | -1 | u64be, u32be, u8, WordPack bytes of (size-13) | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4696 |
| s2c | 177 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 178 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 179 | 2 | s16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5248 |
| s2c | 180 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 181 | 6 | u8, u8, u16be, u8, u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4383 |
| s2c | 182 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 183 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 184 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 185 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 186 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 187 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 188 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 189 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 190 | 2 | u16le+128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4092 |
| s2c | 191 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 192 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4044 |
| s2c | 193 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 194 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 195 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 196 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 197 | 9 | u64be, u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4745 |
| s2c | 198 | 4 | u8sub pos, u16be, u8neg | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6864 |
| s2c | 199 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 200 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 201 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 202 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 203 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 204 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 205 | 23 | u32le, u16be, u16le, u16be, u16le+128, u8-128, u16le, u16be, u16le+128, u16be+128, u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4586 |
| s2c | 206 | -2 | u16be, u16be count, then count times (u16be, u8-128 or u32be when that u8-128 is 255) | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5332 |
| s2c | 207 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 208 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 209 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 210 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 211 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 212 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 213 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 214 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 215 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 216 | -1 | u8, u8sub, jstr | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4495 |
| s2c | 217 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 218 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 219 | 1 | u8 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4662 |
| s2c | 220 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 221 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 222 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 223 | -2 | jstr, u16le | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4803 |
| s2c | 224 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 225 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 226 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 227 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 228 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 229 | 2 | u16be | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4013 |
| s2c | 230 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 231 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 232 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 233 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 234 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 235 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 236 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 237 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 238 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 239 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 240 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 241 | 2 | u8neg, u8neg | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5077 |
| s2c | 242 | -1 | jstr | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4979 |
| s2c | 243 | 2 | u8, u8-128 | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5228 |
| s2c | 244 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 245 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 246 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 247 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 248 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 249 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 250 | 5 | u8 pos, u16be+128, u16le+128 | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6942 |
| s2c | 251 | 3 | u16le, u8sub | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:4572 |
| s2c | 252 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 253 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5142 |
| s2c | 254 | 0 |  | Client.read | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5366 |
| s2c | 255 | 14 | u16le, s8neg, u16be+128, s8sub, s8sub, u8, u16le+128, u8sub, s8-128, u16le+128 | Client.readZonePacket | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6799 |
| c2s | 246 | -1 | u8 length=n*2+3 with n=min(waypoints,25), u8+128 run, u16le destZ, then (n-1) times (u8+128 dx, u8neg dz), u16le destX. The length byte is payload, not a frame header | Client.opClient92 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:947 |
| c2s | 147 | -1 | same walk body as 246 with length value n*2+3+14, then u8 clickX, u8 clickY, u16be yaw, u8 57, u8 yawJitter, u8 pitchJitter, u8 89, u16be playerFineX, u16be playerFineZ, u8, u8 63 | Client.opClient92 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:953 |
| c2s | 216 | -1 | same walk body as 246 | Client.opClient92 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:958 |
| c2s | 127 | -1 | u8 0, psize1 of a random mix of u16be and u8 | Client.opClient93 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:997 |
| c2s | 35 | 0 |  | Client.opClient102 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:2073 |
| c2s | 58 | 0 |  | Client.buildScene | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:2328 |
| c2s | 173 | 4 | u32be 1057001181 | Client.buildScene | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:2452 |
| c2s | 119 | -1 | u8 0, psize1 of a random mix of u16be and u8 | Client.opClient103 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:2700 |
| c2s | 124 | 3 | u8, u8, u8 | Client.handleChatSettingsInput | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:2747 |
| c2s | 223 | 8 | u64be | Client.opClient59 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:2851 |
| c2s | 146 | 8 | u64be | Client.opClient60 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:3035 |
| c2s | 211 | 0 |  | Client.opClient61 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:3214 |
| c2s | 217 | 6 | u16le+128 x, u16le z, u16le locId | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5407 |
| c2s | 208 | 2 | u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5414 |
| c2s | 76 | 2 | u16le+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5433 |
| c2s | 206 | 6 | u16le+128 x, u16le z, u16be locId | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5454 |
| c2s | 170 | 4 | u16be+128, u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5541 |
| c2s | 41 | 6 | u16be, u16le+128 z, u16be x | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5557 |
| c2s | 199 | 4 | u32be 0 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5576 |
| c2s | 37 | 2 | u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5581 |
| c2s | 162 | 2 | u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5599 |
| c2s | 99 | 2 | u16le+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5604 |
| c2s | 95 | 4 | u32be 0 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5635 |
| c2s | 93 | 6 | u16be+128, u16be+128, u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5640 |
| c2s | 73 | 6 | u16be+128, u16be+128, u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5670 |
| c2s | 17 | 6 | u16be+128, u16be+128, u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5694 |
| c2s | 174 | 6 | u16be+128, u16le+128, u16le | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5712 |
| c2s | 100 | 0 |  | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5784 |
| c2s | 59 | 6 | u16be, u16le z, u16be+128 x | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5789 |
| c2s | 113 | 12 | u16be+128, u16le z, u16be+128, u16le+128, u16be x, u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5805 |
| c2s | 12 | 6 | u16be+128, u16be z, u16le+128 x | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5824 |
| c2s | 182 | 0 |  | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5833 |
| c2s | 255 | 6 | u16be, u16be+128, u16be+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5837 |
| c2s | 102 | 6 | u16le x, u16be+128 z, u16be+128 locId | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5856 |
| c2s | 90 | 8 | u16be z, u16be+128 locId, u16be x, u16le+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5863 |
| c2s | 190 | 2 | u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5891 |
| c2s | 14 | 8 | u16le+128, u16be, u16le, u16be+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5912 |
| c2s | 164 | 3 | u24be 3839761 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5940 |
| c2s | 54 | 2 | u16be+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5945 |
| c2s | 158 | 2 | u16le | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5958 |
| c2s | 189 | 8 | u16be+128, u16be, u16be+128, u16le | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5971 |
| c2s | 143 | 6 | u16le z, u16be+128, u16le x | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5989 |
| c2s | 19 | 6 | u16le, u16le, u16le+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:5996 |
| c2s | 48 | 8 | u16be, u16le, u16le, u16le | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6021 |
| c2s | 163 | 2 | u16be+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6037 |
| c2s | 117 | 6 | u16le+128, u16le, u16le+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6043 |
| c2s | 226 | 6 | u16le+128, u16be, u16le | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6061 |
| c2s | 140 | 6 | u16le, u16be+128, u16le+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6079 |
| c2s | 134 | 2 | u16le | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6104 |
| c2s | 21 | 4 | u16le, u16le | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6117 |
| c2s | 9 | 6 | u16le z, u16be+128 locId, u16le+128 x | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6125 |
| c2s | 154 | 2 | u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6132 |
| c2s | 105 | 6 | u16le+128 z, u16le, u16be+128 x | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6156 |
| c2s | 104 | 8 | u16be+128 x, u16be z, u16le, u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6180 |
| c2s | 30 | 4 | u32be 0 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6199 |
| c2s | 57 | 6 | u16be, u16be x, u16be+128 z | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6204 |
| c2s | 16 | 2 | u16be+128 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6233 |
| c2s | 137 | 12 | u16le, u16be, u16le, u16be+128, u16be+128, u16le | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6239 |
| c2s | 87 | 2 | u16be 37562 | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6265 |
| c2s | 176 | 6 | u16be, u16be, u16be | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6270 |
| c2s | 18 | 12 | u16le+128, u16be+128 locId, u16be, u16le x, u16le+128, u16be z | Client.useMenuOption | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:6288 |
| c2s | 3 | 10 | u64be, u8, u8 | Client.handleInterfaceAction | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:7141 |
| c2s | 205 | 13 | u8, then 7 u8, then 5 u8 | Client.handleInterfaceAction | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:7150 |
| c2s | 168 | -1 | u8 0, psize1 of u64be plus WordPack.opkind772 bytes | Client.handleInputKey | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:8554 |
| c2s | 86 | 4 | u32be | Client.handleInputKey | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:8603 |
| c2s | 200 | 8 | u64be | Client.handleInputKey | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:8623 |
| c2s | 230 | -1 | u8 (text.length-1), jstr of text.substring(2) | Client.handleInputKey | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:8687 |
| c2s | 169 | -1 | u8 0, psize1 of WordPack bytes each +128, u8sub, u8 | Client.handleInputKey | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:8750 |
| c2s | 210 | 0 |  | Client.opClient53 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:8870 |
| c2s | 167 | 8 | u64be | Client.addFriend | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:8910 |
| c2s | 71 | 8 | u64be | Client.addIgnore | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:9600 |
| c2s | 193 | 4 | u32be 0 | Client.opClient82 | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:10565 |
| c2s | 84 | 4 | u32be 0 | Client.drawGame | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:10999 |
| c2s | 225 | 1 | u8 | Client.drawGame | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:11115 |
| c2s | 224 | -1 | u8 0, psize1. Each sample is u16be, u24be, or u32be as input.md describes | Client.updateGame | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:12333 |
| c2s | 150 | 4 | u32be | Client.updateGame | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:12428 |
| c2s | 130 | 4 | u16le+128, u16be | Client.updateGame | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:12443 |
| c2s | 139 | 1 | u8 | Client.updateGame | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:12450 |
| c2s | 109 | 7 | u8, u16be+128, u16be+128, u16le | Client.updateGame | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:12548 |
| c2s | 1 | 2 | u16be 26173 | Client.updateGame | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:12628 |
| c2s | 63 | 0 |  | Client.updateGame | builds/rs2/350-2005-12-30/src/src/main/java/deob/Client.java:12636 |
