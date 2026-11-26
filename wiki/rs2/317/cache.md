# Cache

Startup caches are jag archives. Runtime models, anims, midi, and maps are ondemand. This is not a js5 cache.

`loadArchiveChecksums` opens `crc` plus a random integer plus `-317`, reads 40 bytes, and stores nine `i32be` checksums. A tenth `i32be` must equal 1234 folded with `(check << 1) + checksum` for each of the nine. A mismatch clears checksum 8 and retries. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/client/Client.java:875`.

Login sends all nine checksums, including index 0. `Client.load` fetches archives 1 through 8 and does not document what checksum index 0 names. `Client.java:7930`.

| Index | Label | Jag name |
| --- | --- | --- |
| 1 | title screen | title |
| 2 | config | config |
| 3 | interface | interface |
| 4 | 2d graphics | media |
| 5 | update list | versionlist |
| 6 | textures | textures |
| 7 | chat system | wordenc |
| 8 | sound effects | sounds |

## Jagfile

`Jagfile.unpack` reads `u24be` unpacked size and `u24be` packed size. If they differ, `BZip2.read` decompresses from offset 6. Then `u16be` file count. Each file is `i32be` hash, `u24be` unpacked size, `u24be` packed size. Payload starts after the `2 + count * 10` header. The name hash uppercases the string and does `hash = hash * 61 + char - 32` in a Java int. `builds/rs2/317-2005-06-13/src/src/main/java/jagex2/io/Jagfile.java:29`. `JString.hashCode` uses the same 61 multiply and then folds the high byte back into 56 bits. `JString.java:63`. The BZip2 bitstream itself is unread.

## Ondemand

`OnDemand.load` reads `map_index`, `anim_index`, and `midi_index` from the versionlist jag. Map records are described in [region.md](region.md). `anim_index` is `length / 2` big-endian unsigned shorts. `midi_index` is one `u8` per byte of the file. `OnDemand.java:242`.

The client requests archive 0 for models, 1 for anims, 2 for midi, and 3 for maps. `OnDemand.read` socket framing and `FileStream` sector layout were not field-read.

## Config files

Flat files are `u16be` count, then that many `decodeClient` records. Indexed files are `name.dat` plus `name.idx`: the index is `u16be` count and then count `u16be` sizes. Offsets start at 2 and accumulate. Loc, npc, and obj use indexes. Flo, idk, seq, spotanim, varp, and varbit do not.

Decode loops are `u8` opcodes until 0.

### flo.dat

`FloType.decodeClient` at `FloType.java:51`. 1 `u24be` rgb. 2 `u8` texture. 3 sets overlay. 5 clears occlude. 6 `jstr` debug name. 7 `u24be` colour, restoring hue fields around the call.

### idk.dat

`IdkType.java:46`. 1 `u8` type. 2 `u8` count, then count `u16be` models. 3 sets disable. 40..49 `u16be` recolour source. 50..59 `u16be` destination. 60..69 `u16be` heads.

### seq.dat

`SeqType.java:77`. 1 `u8` count, then per frame `u16be` frame, `u16be` iframe (65535 becomes -1), `u16be` delay. 2 `u16be` replay offset. 3 `u8` count, that many `u8` walk-merge values, then a stored sentinel 9999999. 4 sets stretches. 5 `u8` priority. 6 `u16be` right hand. 7 `u16be` left hand. 8 `u8` replay count. 9 `u8` preanim move. 10 `u8` postanim move. 11 `u8` replace mode. 12 `i32be`.

### spotanim.dat

`SpotAnimType.java:55`. 1 `u16be` model. 2 `u16be` anim. 4 `u16be` resize h. 5 `u16be` resize v. 6 `u16be` angle. 7 `u8` ambient. 8 `u8` contrast. 40..49 and 50..59 are `u16be` recolour pairs.

### varp.dat

`VarpType.java:61`. 1 `u8`. 2 `u8`. 3 sets a flag and records the id. 4 clears a flag. 5 `u16be` client code. 6 sets a flag. 7 `i32be`. 8 sets a constant 1. 10 `jstr`. 11 sets a flag. 12 `i32be`. 13 sets a constant 2. Client code 3 is midi volume and client code 4 is wave volume. `Client.java:2106`.

### varbit.dat

`VarbitType.java:52`. 1 `u16be` base varp, `u8` start bit, `u8` end bit. 2 sets a flag. 3 `i32be`. 4 `i32be`. 10 `jstr`.

### npc.dat

`NpcType.java:249`. 1 `u8` count, count `u16be` models. 2 `jstr` name. 3 `gjstrraw` description. 12 `i8` size. 13 `u16be` ready anim. 14 `u16be` walk. 17 four `u16be` walk directions. 30..39 `jstr` options, `hidden` becomes null. 40 `u8` count, then pairs of `u16be`. 60 `u8` count, count `u16be` heads. 90, 91, 92 `u16be`. 93 clears minimap. 95 `u16be` vis level. 97 and 98 `u16be` resize. 99 sets always on top. 100 `i8` ambient. 101 `i8` times 5 contrast. 102 `u16be` head icon. 103 `u16be` turn speed. 106 `u16be` multivarbit, `u16be` multivarp, `u8` n, then `n + 1` `u16be` (65535 becomes -1). 107 clears active.

### loc.dat

`LocType.java:416`. 1 `u8` n. If models are already set and this is not low memory, skip `3 * n` bytes. Otherwise n pairs of `u16be` model and `u8` shape. 2 `jstr` name. 3 `gjstrraw` description. 5 `u8` n. The same low-memory skip is `2 * n` bytes, otherwise n `u16be` models and shapes are cleared. 14 `u8` width. 15 `u8` length. 17 clears blockwalk. 18 clears blockrange. 19 `u8` active when the byte is 1. 21 hillskew. 22 sharelight. 23 occlude. 24 `u16be` anim, 65535 becomes -1. 28 `u8` wall width. 29 `i8` ambient. 30..38 `jstr` options. 39 `i8` contrast. 40 `u8` n, n pairs of `u16be`. 60 `u16be` map function. 62 mirror. 64 clears shadow. 65, 66, 67 `u16be` resize. 68 `u16be` map scene. 69 `u8` force approach. 70, 71, 72 `i16be` offsets. 73 forced decor. 74 break route finding. 75 `u8` raise object. 77 `u16be` multivarbit, `u16be` multivarp, `u8` n, `n + 1` `u16be`.

### obj.dat

`ObjType.java:578`. 1 `u16be` model. 2 `jstr` name. 3 `gjstrraw` description. 4 `u16be` zoom. 5 `u16be` x angle. 6 `u16be` y angle. 7 `u16be` x offset, minus 65536 when above 32767. 8 the same for y. 10 `u16be`. 11 stackable. 12 `i32be` cost. 16 members. 23 `u16be` man wear and `i8` offset. 24 `u16be` man wear 2. 25 `u16be` woman wear and `i8` offset. 26 `u16be` woman wear 2. 30..34 `jstr` options. 35..39 `jstr` inventory options. 40 recolour pairs. 78 `u16be` man wear 3. 79 `u16be` woman wear 3. 90..93 `u16be` heads. 95 `u16be` z angle. 97 `u16be` cert link. 98 `u16be` cert template. 100..109 `u16be` object and `u16be` count. 110, 111, 112 `u16be` resize. 113 `i8` ambient. 114 `i8` times 5 contrast. 115 `u8` team.

There are no enum, struct, or clientscript config files. Interface scripts live inside `Component`. See [interfaces.md](interfaces.md).

## Word filter

`WordFilter.read` takes four packets: bad words, domains, fragments, and tlds. `WordFilter.java:39`. `readTld` is `i32be` count, then for each entry `u8` type, `u8` length, and that many `u8` characters. `WordFilter.java:46`. The other dictionary layouts were not field-read. Chat display calls `WordFilter.filter`.

## Word pack

`WordPack` uses 4-bit tokens and the character table at `WordPack.java:11`. A nibble below 13 indexes the table directly. A nibble of 13 or more is saved and combined with the next nibble as `(first << 4) + second - 195`. `unpack` capitalizes after `.`, `!`, and `?`. `pack` lowercases and stops at 80 characters. `WordPack.java:15`.
