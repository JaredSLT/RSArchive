# Sound

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`.

## Startup

`Client.java:1981` builds two mixers with `C6b1ceb6e.opkind6913` (`deob/C6b1ceb6e.java:11`):

- slot 0, sample rate 22050, stored on `C3f2a4ef6.valuekind7771`
- slot 1, sample rate 2048, stored on `TextureOp19.valueTextureOp1991`

`opkind6913` tries `Class.forName("vb").newInstance()`. Class `vb` is not in this tree. On failure it builds `TextureOpFlip`. On a second failure it returns an empty `LocTint`. The buffer length is the sample rate rounded up to a multiple of 1024 and capped at 16384 on the `vb` path, and forced to 16384 on the `TextureOpFlip` path. The mix array is 256 samples, or 512 when `GlRenderList.valueGlRenderList24` is true.

`C783261f2.opkind6562` is then called with vorbis archive index 14 (`C23d8b466.valuekind8481`), bases index 15 (`FriendListFirst.valuekind8418`), archive index 4 (`C36f4c4c6.valuekind7844`), and the first mixer (`Client.java:1983`). Its body was not read. `scape main` is the named file on archive 6 (`Client.java:1988`).

Sound configs are group 36 on the config archive (`deob/Cc4cace49.java:242`). `Sound.opSound39` was not read.

## Packet

Opcode 92, size 6, `Cc4e911ab.valuekind9694` (`deob/C268455a9.java:671`):

- u16be, 65535 becomes -1
- u8
- u16be
- u8

The call is `Cf4ef65ea.opkind10014(firstU8, secondU8, secondU16, id)` (`C268455a9.java:680`). `opkind10014` was not read, so the four arguments are not named.

## Animation sounds

Player mask bit `0x2000` and npc mask bit `0x4` call `C0783558e.opkind9043` when the sequence's sound table exists (`deob/C3f2a4ef6.java:302`, `deob/Js5GlTextureProvider.java:259`). `opkind9043` (`deob/C0783558e.java:71`) reads the table slot, not the packet:

- id is `word >> 8`. If the slot has more than one id, one is picked with `Math.random`.
- volume is `(word & 246) >> 5`
- loops is `word & 31`
- loops 0 and the local player: `Cf4ef65ea.opkind10014(volume, 255, 0, id)`
- otherwise, if client setting `Cb453c1d8.valuekind9823.valuekind9798 != 0` and the queue length is under 50: store id, volume, delay 0, radius 255, and `(plane << 24) \| (((x - 64) / 128) << 16) \| (((z - 64) / 128) << 8) \| loops`

The mix loop and the vorbis decoder were not read.
