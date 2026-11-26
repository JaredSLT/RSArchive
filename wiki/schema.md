# Client spec schema

Each revision lives in `wiki/<family>/<rev>/`. Pages describe that revision's Java client only. A claim names the file and line it was read from, relative to the RSArchive repo. Revision 742 cites paths inside `builds/rs2/742-2012-11-07/src/` and records that nested git SHA.

A field list copied from another revision is not a spec. When a method cannot be read, write `unread` and the method signature. Leave the field list empty.

`wiki/index.md` is written by the orchestrator after merges. A revision agent does not edit it.

## Required pages

| Page | What it holds |
| --- | --- |
| `index.md` | Revision, date, family, main class, canvas size, cache kind (`jag`, `ondemand`, or `js5`), tick order, a link to every other page, and one status line per subsystem. |
| `files.tsv` | Every `.java` file under that revision's source root. |
| `login.md` | Bytes the client writes, and the success and failure bytes it consumes. Width and endianness of each field. |
| `framing.md` | C2S and S2C size tables copied from the arrays or constructors. ISAAC direction. The variable-length rule. |
| `packets.md` | One row per opcode the client reads or writes. |
| `sync.md` | Local player, other players, and NPCs. Mask bit width, order, and the empty-list terminator. |
| `region.md` | Rebuild packet, zone and scene origin, XTEA keys, map file ids. |
| `cache.md` | Config and interface archive ids, and the decode opcodes that client implements. |
| `input.md` | Mouse, keys, camera, walk, run, minimap, and widget ops, and the packet each one sends. |
| `scene.md` | Camera, terrain, models, sprites, and what the software path draws. HD entry points are named and marked platform. |
| `interfaces.md` | Component types, scripts, and the top-level and sidebar ids the client opens. |
| `sound.md` | Synth or vorbis path, and the packet that starts a sound. |
| `gaps.md` | Every unread file and every behavior too thin to implement. |

## files.tsv

Tab-separated, with a header:

```text
path	lines	role	status
```

`path` is relative to the revision source root. `role` is a short noun such as `login`, `packet-switch`, `player-sync`, `region`, `cache-config`, `input`, `scene`, `interface`, `sound`, `data-struct`, `platform-gl`, `platform-dx`, `platform-audio`, or `unused`. `status` is one of `read`, `classified`, `platform-native`, `unread`.

`read` means the behavior pages cite this file. `classified` means the public methods were identified and the file is not on the behavior path. `platform-native` is `jaggl`, `jagdx`, or `jagtheora`. `unread` means the file was not read. Every `unread` row also appears in `gaps.md`.

## packets.md rows

```text
| direction | opcode | size | fields | handler | citation |
```

`direction` is `c2s` or `s2c`. `size` is the table value (`-1` and `-2` keep their meaning from that client). `fields` is the order of reads or writes with widths (`u8`, `u16le`, `u32`, `jstr`, `bytes(n)`). `handler` is the Java method. `citation` is `path:line`.

## gaps.md rows

```text
| item | source | why it blocked | class |
```

`class` is `implementable` or `blocked`. `implementable` means a later pass can code it from the pages as they stand. `blocked` means the source was unread, the cache is absent, there is no gamepack, or the code is a platform binding. Use `blocked` together with a reason of `unread`, `no-cache`, `no-source`, or `platform-native` in the why column.

## Status line

`index.md` gives each subsystem one of `specified`, `partial`, or `gap`. `specified` means the page has field-level citations for that subsystem. `partial` means some opcodes or fields are cited and the rest are in `gaps.md`. `gap` means the page explains why it could not be read.
