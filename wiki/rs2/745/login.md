# Login

Two sockets. `Statics.opStatics142` drives `Statics.valueStatics810` (`Statics.java:2097`). `C82f912d1.opkind35371` drives `Client.valueClient1828` while `valueClient1760` is 11 (`C82f912d1.java:74-91`). Game packets after login use `Client.valueClient1829`. Handshake buffers come from `ClientMessage.opkind39493` and are not ISAAC-framed, except opcode 26 below.

`Cb5e68c59` stores only the opcode. The second constructor argument is ignored (`Cb5e68c59.java:29-31`). These opcodes are not the game-table rows of the same number in [packets.md](packets.md).

Revision constants in the blocks below are `745` and sub-revision `1`.

## Opcode 15, separate socket

`Client.java:1441-1450` writes, before the game login:

| Order | Width |
| --- | --- |
| 1 | u8 opcode 15 (`valuekind33717`) |
| 2 | u8 length `9 + valueClient1777.length()` |
| 3 | u32be `745` |
| 4 | u32be `1` |
| 5 | jstr `valueClient1777` |

The next server byte must be 0 or the client calls `opClient492` (`Client.java:1456-1461`). The rest of that state machine was not read.

## First byte on the login socket

`Statics.java:2135-2172`, state 12. Timeout is 500 attempts, or 2000 after a retry, or 6000 when `C0de22efb.valuekind34293` and the state is at least 65 (`Statics.java:2100-2108`). Three failures call `opStatics129(-5)` and state 8 (`Statics.java:2117-2120`).

If `C0de22efb.valuekind34293`:

| Order | Width |
| --- | --- |
| 1 | u8 opcode 29 (`valuekind337110`) |
| 2 | u16be length placeholder, later backpatched |
| 3 | u32be `745` |
| 4 | u32be `1` |
| 5 | u8 `valueClient1760 == 7`, only when `valueStatics809 == 221` |
| 6 | bytes from `C0de22efb.opkind342926` after that method appends its own fields |

`opkind342926` itself was not opened. Before it returns, the block it builds writes u8 `valuekind34294`, u16be random, u8 language, u32be `valueClient1946`, six u32be random, u64be `valueClient1797`, u8 `valueClient1911.valuekind36636`, u8 random, then RSA `opkind397442` (`Statics.java:2154-2167`). The numeric modulus and exponent are `Cc3f7f61f2.valuekind36153` and `valuekind36152`. That class was not opened.

Otherwise the client writes a single u8 opcode 14 (`valuekind33716`) and nothing else (`Statics.java:2171`).

## Server check and 16-byte block

State 30 reads one byte. `1` moves to state 77 (`Statics.java:2238-2240`). The other values were not read in this stretch.

State 77 reads 16 bytes, runs `opkind397440` over the buffer with `valueStatics879`, then reads two u64be into `valuekind34291` and `valuekind34297` (`Statics.java:2244-2255`).

## Login block, state 81

`valueStatics809 == 221` selects opcode 30 when `valuekind34293` is set, otherwise opcode 16 (`Statics.java:2263-2271`). The other branch selects opcode 30 or opcode 19 the same way (`Statics.java:2323-2328`).

Both branches write u8 opcode, u16be length placeholder, then the body, XTEA `opkind397441` over the body, then `opkind397415` (`Statics.java:2271-2322` and `2326-2366`).

Opcode 16 body, only when `valuekind34293` is false (`Statics.java:2275-2321`):

| Order | Width |
| --- | --- |
| 1 | u32be `745` |
| 2 | u32be `1` |
| 3 | u8 `valueClient1760 == 7` |
| 4 | bytes from `C0de22efb.opkind342922` |
| 5 | u8 `valuekind34291 == -1` |
| 6 | jstr `valuekind34295` if that flag is 1, else u64be `valuekind34291` |
| 7 | u8 `Client.opClient465()` |
| 8 | u16be `valueStatics1056` |
| 9 | u16be `valueStatics692` |
| 10 | u8 `valueStatics604.valuekind397539.opkind40203()` |
| 11 | 24 bytes from `GameShell.opGameShell106` |
| 12 | jstr `valueClient1763` |
| 13 | u32be `valueClient1946` |
| 14 | u8 length plus that many bytes from `valueStatics604.opkind39754` |
| 15 | bytes from `HardwarePlatform.opHardwarePlatform8`, length `opHardwarePlatform10` |
| 16 | u32be `valueClient1831` |
| 17 | u64be `valueClient1768` |
| 18 | jstr `valueStatics1041` |
| 19 | u8 `valueClient1818 == null`, and a jstr of that string when it is not null |
| 20 | u8 jagtheora loader flag |
| 21 | u8 `valueClient1761` |
| 22 | u8 `valueClient1807` |
| 23 | u8 `valueStatics840` |
| 24 | u32be `valueClient1954` |
| 25 | jstr `valueClient1777` |
| 26 | u8 world-host compare |
| 27 | bytes from `C0de22efb.opkind342923` |

`opkind342922`, `opkind342923`, `opClient465`, `opkind39754`, and `opHardwarePlatform8` were not opened. `GameShell.opGameShell106` writes 24 bytes from a random file, or 24 bytes of `-1` if that read fails (`GameShell.java:442-462`).

Opcode 19 body, when `valuekind34293` is false (`Statics.java:2335-2364`), is the same shape with these differences: no `opClient465`, no display u16 pair, no settings u8, no u64be `valueClient1768`, no jagtheora or `valueClient1761` / `valueClient1807` / `valueStatics840` bytes, and `valueClient1954` is a u32be placed before the username. The RSA block is still `opkind342922`. The `valuekind34293` true path skips the revision pair and that RSA block (`Statics.java:2335`).

Opcode 30 is the `valuekind34293` side of both branches. Its bytes after the shared header were not separated from the false path beyond the `if (!valuekind34293)` guard.

## ISAAC and the response byte

On flush the client builds the outbound Isaac from `valueStatics879`, adds 50 to each of the four seeds, builds the inbound Isaac from the mutated seeds, discards a third Isaac, and binds the inbound buffer (`Statics.java:2371-2380`).

State 93 reads one u8 (`Statics.java:2385-2391`):

| Code | What was read |
| --- | --- |
| 21 | state 126, then one more u8 times 50 (`Statics.java:2475-2482`) |
| 29 or 45 | branch not read past the condition (`Statics.java:2395`) |
| 1 | state 100 (`Statics.java:2396-2399`) |
| 52 | state 213 (`Statics.java:2402-2404`) |
| 2 | state 137, or state 12 when `valuekind342922` was set (`Statics.java:2405-2412`) |
| 15 | sets the packet size field to `-2` and state 194 (`Statics.java:2413-2415`) |
| 23 | up to three retries, then the failure path (`Statics.java:2416-2422`) |
| 42 | state 206 (`Statics.java:2425-2428`) |
| 35 | one guarded branch was not read (`Statics.java:2439`) |
| other | failure state 8 for the codes that fall through in the stretch that was read |

The body of state 137 / 143, which is the success payload after code 2, was not read.

## Opcode 26

State 118 writes one ISAAC opcode 26 (`valuekind33718`) and no further payload (`Statics.java:2463-2471`).

## Lobby connection, opcode 28

`C82f912d1.opkind35371` (`C82f912d1.java:91-117`):

| Order | Width |
| --- | --- |
| 1 | u8 opcode 28 (`valuekind33719`) |
| 2 | u16be length placeholder |
| 3 | u16be `745` |
| 4 | u16be `1` |
| 5 | RSA block from `opkind35378` |
| 6 | jstr `valueClient1777` |
| 7 | u16be `valueClient1946` |
| 8 | u64be `valueClient1768` |
| 9 | jstr `valueStatics1041` |
| 10 | u8 language |
| 11 | u8 `valueClient1911.valuekind36636` |
| 12 | 24 bytes from `GameShell.opGameShell106` |
| 13 | u8 `valueClient1818 == null`, plus that jstr when it is not null |
| 14 | bytes from `HardwarePlatform.opHardwarePlatform8` |
| 15 | 7 unused bytes (`valuekind39742 += 7`) |
| 16 | XTEA of the body after the length placeholder, then u16be backpatch |

`opkind35378` (`C82f912d1.java:167-188`): u8 `10`, four u32be seeds, ten u32be random, u16be random, RSA, then those bytes are appended. The seeds are the Isaac seeds.

The server replies with one byte mapped by `C9f5485f94.opkind36531`, which was not opened, onto `C89d2df1b`. Success is `valuekind36686`, whose stored code is 2 (`C89d2df1b.java:6`). On success the outbound Isaac is the seeds and the inbound Isaac is the seeds plus 50 (`C82f912d1.java:135-147`). Any other mapped value closes the socket. The enum also stores -1, -2, -3, -4, -5, 3, 7, 9, and 37 (`C89d2df1b.java:2-10`). Which raw byte maps to which code was not read. Two timeouts move to code -5 (`valuekind36687`) and state 17 (`C82f912d1.java:191-201`).

While state is 11, `opkind353710` can also send game opcode 11. That row is in [packets.md](packets.md). `Statics.opStatics98` sends game opcode 34, one u8, on `valueClient1828` in the same state (`Statics.java:1179-1183`).
