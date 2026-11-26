# Login

State machine: `LswprPos.calla978325d` (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:45). Pre-game opcodes are `PathFinderEntitySize` values. The constructor stores only the first int (`valuekind15331`) and drops the second (builds/rs2/634-2010-12-10/src/src/main/java/deob/PathFinderEntitySize.java:8). Those second ints are not a size table.

| Name | Opcode | Declaration |
| --- | --- | --- |
| handshake | 14 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:8 |
| lobby | 16 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:9 |
| | 17 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:18 |
| lobby when client state is 13 | 18 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:7 |
| game login | 19 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:16 |
| | 20 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:17 |
| | 21 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:5 |
| | 22 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:12 |
| | 23 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:4 |
| | 24 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:6 |
| opcode sent from login state 6 | 26 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:13 |
| | 27 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:15 |
| | 28 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:10 |
| | 15 | builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf700075c.java:19 |

RSA modulus `Cad9fc7bc.valuekind15105` hex `81623a54b9f051249834ac97893910ee91109ccd1a87694f9e128982180113d8a7e9e11579269bca981fbc40e666cd1d03a27a32eee39ab94be2f8d8efef02d9` (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cad9fc7bc.java:11). Exponent `C944c25fc.valuekind11182` is hex `10001` (builds/rs2/634-2010-12-10/src/src/main/java/deob/C944c25fc.java:6).

## Codec used here

Widths are the method bodies in `LoadingScreenRelated2` (see [framing.md](framing.md)). `pjstr` is `call75e62fa6`: bytes from `Ca13e5e3b.opkind12054` then `u8` 0 (builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:187). Code points 1..127 and 160..255 are one byte; a fixed punctuation set is remapped; the method returns the input length (builds/rs2/634-2010-12-10/src/src/main/java/deob/Ca13e5e3b.java:73).

## State 3, both modes

Connect, then write plain `u8` opcode 14 (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:101). Read one `u8`. Any value other than 0 fails with that code and drops the socket (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:113).

Inner buffer, capacity 518, then RSA over the whole buffer (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:124):

| Order | Width | Value |
| --- | --- | --- |
| 1 | u8 | 10 |
| 2 | u32be ×4 | random seeds |
| 3 | u64be | 0 |
| 4 | pjstr | username `Cb6ae220e.valuekind14786` |
| 5 | u64be | `Cbff04bcd.valuekind161511` |
| 6 | u64be | `Ce87e0f5c.valuekind13593` |
| 7 | u16be + bytes | RSA `modPow(exponent, modulus)` replaces the buffer (builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:732) |

`PureJavaModelRelated1.valuePureJavaModelRelated19 == 2` selects the lobby outer packet. Any other value selects the game outer packet (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:139).

### Game outer packet

| Order | Width | Value | Line |
| --- | --- | --- | --- |
| 1 | u8 | opcode 19 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:140 |
| 2 | u16be | placeholder 0, later the length | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:141 |
| 3 | u32be | 634 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:143 |
| 4 | bytes | the RSA block | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:144 |
| 5 | pjstr | password `Algorithms.valueAlgorithms45` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:146 |
| 6 | u8 | game id `C4d106383.valuekind15924.valuekind14753` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:147 |
| 7 | u8 | `Cbff04bcd.valuekind161512` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:148 |
| 8 | bytes(24) | machine id from `BASTypeList.opBASTypeList4` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:149, builds/rs2/634-2010-12-10/src/src/main/java/deob/BASTypeList.java:70 |
| 9 | pjstr | settings `PureJavaModelRelated1.valuePureJavaModelRelated18` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:150 |
| 10 | u32be | `Cb0ded762.valuekind160914` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:151 |
| 11 | u32be ×35 | archive CRCs, one per call in `C0a1835bb4.call58b83e6b` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:152, builds/rs2/634-2010-12-10/src/src/main/java/deob/C0a1835bb4.java:11 |

XTEA (`opLoadingScreenRelated232`) runs from the offset after the RSA block to the end, with the four seeds (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:153). `opLoadingScreenRelated258` back-patches the `u16be` length at the placeholder (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:154).

### Lobby outer packet

Opcode is 16, or 18 when `C72191dbb.valuekind12333 == 13` (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:156). The same revision, RSA block, password, 24 machine bytes, settings string, archive CRCs, XTEA range, and length back-patch follow. Between password and the machine bytes the lobby packet also writes:

| Order | Width | Line |
| --- | --- | --- |
| u8 | `Cc4a18eca.valuekind12893` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:168 |
| u8 | `Cb2d622ec.opkind13391` (the callee body was not read) | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:169 |
| u16be | `C16e026a5.valuekind148220` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:170 |
| u16be | `C4d106383.valuekind15923` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:171 |
| u8 | preferences `opkind12973` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:172 |
| bytes(24) | machine id | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:173 |
| pjstr | settings | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:174 |
| u32be | `Cb0ded762.valuekind160914` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:175 |
| u8 + bytes(n) | preferences blob from `opkind13377` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:176 |
| bytes(n) | buffer filled by `Ca0e7b520.valuekind12852.callc0c01646` (producer body unread) | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:181 |
| u16be | `C02cf8d8f.valuekind11247` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:183 |
| u64be | `Cc3b568c6.valuekind16445` | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:184 |
| u8 | 1 when `Cbd77d905.valuekind11322` is non-null, else 0 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:185 |
| pjstr | that string, only when non-null | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:187 |
| u8 | 1 when `jagtheora` loads, else 0 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:190 |

XTEA starts at the post-RSA offset `var11` (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:192).

## Ciphers

After either outer packet: outgoing ISAAC `C10f1efbe.valuekind172616 = new C351e1949(seeds)` (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:198). Each seed is then increased by 50 (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:200). Incoming buffer `C0a1835bb5.valuekind11661.opkind16723` takes the seeds+50 (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:204). Login state becomes 4.

## Response byte

Read one `u8` (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:214).

| Code | Effect | Line |
| --- | --- | --- |
| 21 | state 7; a later `u8` sets delay `(3+byte)*60`, error 21, socket closed | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:251, builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:264 |
| 29 | state 13; a later `u8` is stored in `Cfb0409e7.valuekind17161`, error 29, socket closed | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:216, builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:275 |
| 1 | state 5, error 1, return | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:219 |
| 2 | state 8 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:247 |
| 15 | state 14, length sentinel set to -2 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:244 |
| 23 | while `Cbafceea7.valuekind15851` is below 3, increment it, state 1, close the socket and retry; otherwise fail with 23 | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:227 |
| other | state 0, `C8daec638.opkind67919` with that code, socket closed | builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:236 |

State 6 (reached after code 21's setup, before the extra byte) writes ISAAC opcode 26 and returns to state 4 (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:255).

## Success payload

State 8 reads one `u8` length into `C47e27dee.valuekind14547` and moves to state 9 (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:285).

Game mode (`valuePureJavaModelRelated19 != 2`), in order (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:301):

| Width | Stored |
| --- | --- |
| u8 | `C1b8e0890.valuekind14917` |
| u8 | `C655a4c66.valuekind13386` |
| u8 | members flag, true when the byte is 1 (`C5f11c418.valuekind11931`) |
| u8 | `LoadingRelated2.valueLoadingRelated26`, true when 1 |
| u8 | `C0ffff07a.valuekind16925`, true when the byte is 1 |
| i16be | `C79afb529.valuekind13171` |
| u16be | `C02cf8d8f.valuekind11245` |
| u16be | `C23248673.valuekind13606` |
| u16be | `C50b3ff26.valuekind16481` |
| u32be | `C078a9aea.valuekind17782` |
| u8 | `C99b3e320.valuekind12123` |
| u16be | `Cc5c399ef.valuekind11791` |
| u16be | `HashMap.valueHashMap14` |
| u8 | `C662470cc.valuekind13301`, true when 1 |
| gjstr2 | player name on `C16d7fbde.valuekind15903` (version byte must be 0, then cstr) |
| u8 | `ProjAnim.valueProjAnim89` |
| u32be | `Cc06a5d96.valuekind16755` |
| u16be | world id on `C19c3c19c.valuekind14063`; 65535 becomes -1 |
| gjstr2 | `C19c3c19c.valuekind14063.valuekind16766` |

`gjstr2` is `call76b99ad9` (builds/rs2/634-2010-12-10/src/src/main/java/deob/LoadingScreenRelated2.java:557). This game block does not assign `Cf1a80554.valuekind15747`. That field stays -1 until some other write; the only assignment found is the lobby block below (builds/rs2/634-2010-12-10/src/src/main/java/deob/Cf1a80554.java:5, builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:348).

Lobby mode reads a different block (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:342):

| Width | Stored |
| --- | --- |
| u8 | `C1b8e0890.valuekind14917` |
| u8 | `C655a4c66.valuekind13386` |
| u8 | members, true when 1 |
| u8 | `LoadingRelated2.valueLoadingRelated26` |
| u8 | `C0ffff07a.valuekind16925`, true when 1 |
| u8 | `Cafea72e5.valuekind17535`, true when 1 |
| u16be | `Cf1a80554.valuekind15747` (local player index) |
| u8 | `C50b3ff26.valuekind164810`, true when 1 |
| i24be | `Cc3fe07a2.valuekind14924` (`opLoadingScreenRelated248`) |
| u8 | `Cc9b39119.valuekind15055`, true when 1 |

## Later login states

State 11 reads 3 bytes, then state 12 (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:394). State 12 reads an ISAAC opcode index the same way as the game switch, then always a `u16be` length (`opLoadingScreenRelated214`). It does not use the -1/-2 size rule (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:403). State 10 reads that many bytes, calls `Cd096a612.opkind16103` (body unread), and calls `Cd0f64cd0.opkind14622` unless the packet is `Cc1d4fa95.valuekind12261` (opcode 70, size -2, builds/rs2/634-2010-12-10/src/src/main/java/deob/Cc1d4fa95.java:11). Opcode 70 takes `Ca5bf8f3f.opkind13812` instead (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:419).

State 14 (response 15): the length starts at -2, so the first step reads a `u16be` length, then that many bytes, then `C1ec6e3ef.opkind14974` and `Cd096a612.opkind16103` (builds/rs2/634-2010-12-10/src/src/main/java/deob/LswprPos.java:441). The body of `opkind16103` was not read.
