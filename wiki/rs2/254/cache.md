# Cache

Startup reads nine big-endian checksums from `crc` (`Client.load`, `Client.java:5750`). Archives, in checksum order, are loaded by `loadArchive` (`Client.java:5776`):

| Checksum index | Name | Archive id |
| --- | --- | --- |
| 1 | `title` | 1 |
| 2 | `config` | 2 |
| 3 | `interface` | 3 |
| 4 | `media` | 4 |
| 5 | `versionlist` | 5 |
| 6 | `textures` | 6 |
| 7 | `wordenc` | 7 |
| 8 | `sounds` | 8 |

`JagFile.unpack` (`JagFile.java:34`) reads two u24be sizes. When they differ, the payload is BZip2 from offset 6. The directory is u16be file count, then per file a u32be hash, u24be unpacked size, and u24be packed size. Maps, models, anims, and midi are not in these jags. They arrive through `OnDemand` (`OnDemand.java:67`). Archive 0 is models, archive 1 is anims (`Client.java:5052`), archive 2 is midi (`Client.java:5056`), archive 3 is maps (`Client.java:5060`).

Config files are read from the config jag (`Client.java:6066`):

| File | Unpack | Decode |
| --- | --- | --- |
| `seq.dat` | `SeqType.unpack` `SeqType.java:28`, count u16be | `SeqType.java:75` |
| `loc.dat` + `loc.idx` | `LocType.unpack` `LocType.java:58`. Count is the first u16be of the idx. Each idx entry is a u16be length added to a cursor that starts at 2. | `LocType.java:148` |
| `flo.dat` | `FloType.unpack` `FloType.java:23`, count u16be | `FloType.java:49` |
| `obj.dat` + `obj.idx` | `ObjType.unpack` `ObjType.java:68`, same idx shape as locs | `ObjType.java:324` |
| `npc.dat` + `npc.idx` | `NpcType.unpack` `NpcType.java:47`, same idx shape | `NpcType.java:98` |
| `idk.dat` | `IdkType.unpack` `IdkType.java:20`, count u16be | `IdkType.java:46` |
| `spotanim.dat` | `SpotAnimType.unpack` `SpotAnimType.java:26`, count u16be | `SpotAnimType.java:53` |
| `varp.dat` | `VarpType.unpack` `VarpType.java:25`, count u16be | `VarpType.java:59` |
| `varbit.dat` | `VarbitType.unpack` `VarbitType.java:17`, count u16be | `VarbitType.java:46` |

Every decode reads u8 opcodes until 0. An unknown opcode in Flo, Idk, Seq, SpotAnim, Varbit, and Varp is printed and not consumed. Loc has no else branch, so an unknown loc opcode is also not consumed (`LocType.java:279`). Obj ignores an unknown opcode without consuming it (`ObjType.java:432`). Npc ignores an unknown opcode without consuming it (`NpcType.java:176`).

`gsmart` (`Packet.java:351`): next byte below 128 means `g1() - 64`, otherwise `g2() - 49152`. `gsmarts` (`Packet.java:356`): next byte below 128 means `g1()`, otherwise `g2() - 32768`.

## FloType

| Opcode | Fields |
| --- | --- |
| 1 | u24be rgb |
| 2 | u8 texture |
| 3 | overlay flag, no payload |
| 5 | occlude false, no payload |
| 6 | jstr name |

## IdkType

| Opcode | Fields |
| --- | --- |
| 1 | u8 type |
| 2 | u8 count, then that many u16be models |
| 3 | disable, no payload |
| 40..49 | u16be source recolour |
| 50..59 | u16be dest recolour |
| 60..69 | u16be head model |

## SeqType

| Opcode | Fields |
| --- | --- |
| 1 | u8 count, then count times u16be frame, u16be iframe (65535 becomes -1), u16be delay |
| 2 | u16be loops |
| 3 | u8 count, then that many u8 walk-merge values, then a stored sentinel 9999999 |
| 4 | stretches, no payload |
| 5 | u8 priority |
| 6 | u16be replace held left |
| 7 | u16be replace held right |
| 8 | u8 max loops |
| 9 | u8 preanim move |
| 10 | u8 postanim move |
| 11 | u8 duplicate behavior |

## SpotAnimType

There is no opcode 3.

| Opcode | Fields |
| --- | --- |
| 1 | u16be model |
| 2 | u16be anim |
| 4 | u16be resize h |
| 5 | u16be resize v |
| 6 | u16be angle |
| 7 | u8 ambient |
| 8 | u8 contrast |
| 40..49 | u16be source recolour |
| 50..59 | u16be dest recolour |

## VarbitType

| Opcode | Fields |
| --- | --- |
| 1 | u16be base varp, u8 start bit, u8 end bit |
| 10 | jstr debug name |

If `pos != data.length` after the file, it prints `varbit load mismatch` (`VarbitType.java:33`).

## VarpType

| Opcode | Fields |
| --- | --- |
| 1 | u8 |
| 2 | u8 |
| 3 | server-side flag, no payload. The id is recorded. |
| 4 | flag false, no payload |
| 5 | u16be client code |
| 6 | flag, no payload |
| 7 | u32be |
| 8 | two flags, no payload |
| 10 | jstr debug name |
| 11 | flag, no payload |

`updateVarp` (`Client.java:5460`) uses client code 1 (brightness 0.9, 0.8, 0.7, 0.6), 3 (midi volume 0, -400, -800, -1200, or off), 4 (wave volume, same steps), 5 (`oneMouseButton`), 6 (`chatEffects`), 8 (`splitPrivateChat`), and 9 (`bankArrangeMode`).

## LocType

| Opcode | Fields |
| --- | --- |
| 1 | u8 count, then count times u16be model and u8 shape |
| 2 | jstr name |
| 3 | jstr description (`gjstrraw`) |
| 5 | u8 count, then count times u16be model. Shapes are cleared. |
| 14 | u8 width |
| 15 | u8 length |
| 17 | `blockwalk` false |
| 18 | `blockrange` false |
| 19 | u8 active |
| 21 | hillskew |
| 22 | sharelight |
| 23 | occlude |
| 24 | u16be anim, 65535 becomes -1 |
| 28 | u8 wall width |
| 29 | i8 ambient |
| 39 | i8 contrast |
| 30..38 | jstr option. `hidden` becomes null. |
| 40 | u8 count, then count times u16be source and u16be dest |
| 60 | u16be map function |
| 62 | mirror |
| 64 | shadow false |
| 65 | u16be resize x |
| 66 | u16be resize y |
| 67 | u16be resize z |
| 68 | u16be map scene |
| 69 | u8 force approach |
| 70 | i16be offset x |
| 71 | i16be offset y |
| 72 | i16be offset z |
| 73 | force decor |
| 74 | break route finding |
| 75 | u8 raise object |

## NpcType

| Opcode | Fields |
| --- | --- |
| 1 | u8 count, then count times u16be model |
| 2 | jstr name |
| 3 | jstr description |
| 12 | i8 size |
| 13 | u16be run anim |
| 14 | u16be walk anim |
| 17 | four u16be: walk, walk back, walk left, walk right |
| 30..39 | jstr option. `hidden` becomes null. |
| 40 | u8 count, then count times two u16be recolours |
| 60 | u8 count, then count times u16be head model |
| 90 | u16be |
| 91 | u16be |
| 92 | u16be |
| 93 | minimap false |
| 95 | u16be vis level |
| 97 | u16be resize h |
| 98 | u16be resize v |
| 99 | always on top |
| 100 | i8 ambient |
| 101 | i8 contrast, multiplied by 5 |
| 102 | u16be head icon |
| 103 | u16be turn speed |

## ObjType

| Opcode | Fields |
| --- | --- |
| 1 | u16be model |
| 2 | jstr name |
| 3 | jstr description |
| 4 | u16be zoom |
| 5 | u16be x angle |
| 6 | u16be y angle |
| 7 | u16be x offset. If above 32767, subtract 65536. |
| 8 | u16be y offset, same adjustment |
| 10 | u16be |
| 11 | stackable |
| 12 | u32be cost |
| 16 | members |
| 23 | u16be man wear, i8 offset |
| 24 | u16be man wear 2 |
| 25 | u16be woman wear, i8 offset |
| 26 | u16be woman wear 2 |
| 30..34 | jstr option. `hidden` becomes null. |
| 35..39 | jstr inventory option |
| 40 | u8 count, then count times two u16be recolours |
| 78 | u16be man wear 3 |
| 79 | u16be woman wear 3 |
| 90 | u16be man head |
| 91 | u16be woman head |
| 92 | u16be man head 2 |
| 93 | u16be woman head 2 |
| 95 | u16be z angle |
| 97 | u16be cert link |
| 98 | u16be cert template |
| 100..109 | u16be count obj, u16be count |
| 110 | u16be resize x |
| 111 | u16be resize y |
| 112 | u16be resize z |
| 113 | i8 ambient |
| 114 | i8 contrast, multiplied by 5 |

Interface bytes are in [interfaces.md](interfaces.md). Sound bytes are in [sound.md](sound.md).
