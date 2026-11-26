# Input

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`. Client-to-server sizes in this page are the discarded `NoiseGeneratorRelated` constructor literal. The client does not read them. See [framing.md](framing.md). Writes use the codec names from that page. The opcode byte is `LswprPos.opkind9851` and is not counted in the field list.

Logged-in writers are `Cd6e44077.opkind10291` (`deob/Cd6e44077.java:16`), after up to 100 incoming packets.

## Mouse move

Opcode 61, literal -1, `LswprPos.valuekind9852` (`deob/LswprPos.java:18`). Written at `Cd6e44077.java:79` when a queued move differs from the last point, and the packet stays under 241 payload bytes.

- u8 length, backpatched with `opUnused6119` to the number of bytes after it
- for each move, `dt = (timestamp - MonochromeImageCacheEntry.valueMonochromeImageCacheEntry70) / 20`, `dy` and `dx` against the previous y and x
  - `dt < 8` and both deltas in -32..31: u16be `(dt << 12) \| ((dy + 32) << 6) \| (dx + 32)`
  - `dt < 32` and both deltas in -128..127: u8 `128 + dt`, u16be `((dy + 128) << 8) + (dx + 128)`
  - `dt >= 32`: u16be `0xE000 + dt`, then u32be. Off-screen (`y == -1` and `x == -1`) writes `-2147483648`. Otherwise `(y << 16) \| x`, with y and x clamped to 0..65534 before the off-screen test replaces them with -1.
  - otherwise: u8 `192 + dt`, then the same u32be

y is `opkind7943`. x is `opkind7945`.

## Mouse click

Opcode 37, literal 6, `C91bec970.valuekind10259` (`deob/C91bec970.java:22`), at `Cd6e44077.java:152`.

- `dt = (clickTime - C9935070d.valuekind10203) / 50`, clamped to 32767
- button is 1 when `opkind7941() == 2`, else 0
- u16be `(button << 15) \| dt`
- u32le `(y << 16) \| x`, y and x clamped to 0..65535

## Keys

The tick copies at most 128 key records into `Cd24b839a.valuekind8794` (`deob/Client.java:1446`). Opcode 23, literal -1, `Cfdcd577a.valuekind9622` (`deob/Cfdcd577a.java:7`), at `Cd6e44077.java:168`.

- count of records whose `opGlRelated64` is true, capped at 75
- u8 `3 * count`
- per pressed key: u8 `opGlRelated66()`, u16be `(opGlRelated63() - C662956bf.valuekind63512) / 50` clamped to 65535

## Camera

If `Ceea057f0.valuekind10149` is set and `JReZCu.valueJReZCu1 <= 0` (`Cd6e44077.java:195`): clear the flag, set the cooldown to 20, opcode 7, literal 4, `Cdc5a159d.valuekind6894` (`deob/Cdc5a159d.java:11`).

- u16be `cameraYaw >> 3` (`Ced45c6ab.valuekind78812`)
- u16le `C86e345bf.valuekind75915 >> 3`

## Focus

Opcode 44, literal 1, `C83b47d7d.valuekind8934` (`deob/C83b47d7d.java:12`). u8 1 when `C83b47d7d.valuekind8935` becomes true (`Cd6e44077.java:203`). u8 0 when it becomes false (`Cd6e44077.java:209`).

## Client settings

Once, when `Js5GlTextureProvider.valueJs5GlTextureProvider135` is false (`Cd6e44077.java:215`): opcode 42, literal -1, `C234869d1.valuekind7551` (`deob/C234869d1.java:6`).

- u8 0
- the bytes of `Cb453c1d8.valuekind9823.opSceneGraph_Class12079()`, then `opUnused6119` backpatches a u8 length over the byte before those bytes. The u8 0 is before that length byte.

The layout inside `opSceneGraph_Class12079` was not read.

## Keepalive-style u16

Opcode 81, literal 2, `ServerActiveProperties.valueServerActiveProperties49` (`deob/ServerActiveProperties.java:5`). When `Cbded41f3.valuekind8154.valuekind9022 != -1`, write u16be of that field and clear the object (`Cd6e44077.java:41`).

Opcode 34, literal -1, is passed to `Cef613972.opkind78610` on the same tick (`Cd6e44077.java:38`). That method was not read.

## Walk and minimap

`Entity.opEntity19(localX, localZ, kind)` (`deob/Entity.java:30`):

- kind 0 writes opcode 78, literal 5 (`deob/C6defb8da.java:7`)
- kind 1 writes opcode 59, literal 18 (`deob/C96ed0652.java:4`)
- u8_add128: 1 if key 82 is down, else 0
- u16be `localX + TextureOp29SubOp4.valueTextureOp29SubOp4167`
- u16le `localZ + C292109ad.valuekind7854`

`Cf4b043a9.opkind6291` is called after those five bytes and does not add them. The minimap tail is in the caller, menu action 22 (`~var5 == -23`) at `deob/Cdce76b51.java:297`. It calls `opEntity19` with `var6`. When `var6 == 1` it then writes (`Cdce76b51.java:308`):

- u8 255 (`opUnused6106(-1)`)
- u8 255
- u16be `(int) Ced45c6ab.valuekind78812`
- u8 57
- u8 `VarnBitType.valuekind6773`
- u8 `C1659b20b.valuekind9302`
- u8 89
- u16be `Cb4a619ef.valuekind75312.valuekind96510`
- u16be `Cb4a619ef.valuekind75312.valuekind96516`
- u8 63

Five bytes plus these thirteen is 18. A game click passes `var6 != 1` and stops at five bytes.

## Menu actions read in Cdce76b51

| action | opcode | literal | fields | line |
| --- | --- | --- | --- | --- |
| 13 | 6 `C4aacb79a.valuekind6425` | 15 | u16be `Ccce5b0b2.valuekind8464`, u16le_add128 `var6`, u16le `baseX + localX`, u8neg (1 if key 82 else 0), u16le_add128 `C9aec2547.valuekind8831`, u32_mix674 `Ccc85a8332.valuekind7324`, u16le_add128 `baseZ + localZ` | `Cdce76b51.java:259` |
| 23 | 45 `C56f6aa15.valuekind10279` | 3 | u16le player index `var6`, u8_add128 (1 if key 82 else 0) | `Cdce76b51.java:277` |
| 6 | 79 `ClientScript.valueClientScript124` | 7 | u16be `var6`, u8_add128 (1 if key 82 else 0), u16be_add128 `baseX + localX`, u16be `baseZ + localZ` | `Cdce76b51.java:289` |

`C15ab7d97.opkind9292` (`deob/C15ab7d97.java:74`) writes opcode 16, literal -1, `C950f0e92.valuekind6872`: u8 `stringLength + 1` via `Cd18618b0.opkind7513`, pjstr name, u8 1 or 0 for the boolean argument.

Other `LswprPos.opkind9851` call sites were not read. Their field lists are empty in [packets.md](packets.md).
