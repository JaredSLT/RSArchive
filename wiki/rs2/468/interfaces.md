# Interfaces

The interface archive is index 3, `MouseRecorder.valueMouseRecorder2` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5.java:93`). The component class is `TextureOp16`. `HashTable.opHashTable5` returns one by id. The top interface id is `Component.valueComponent194` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Component.java`), not a constant in the sources that were read.

## Open

Server opcode 27 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:468`):

| Order | Width | Use |
| --- | --- | --- |
| 1 | u16be, low byte stored as `value + 128` | `Component.valueComponent194` |

Then `Client.opClient171` zeros `valueTextureOp1623` and `valueTextureOp1695` on each child (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:231`). `Js5NetQueue.opJs5NetQueue16` runs each child's `valueTextureOp16121` script through `MaterialManager_Class106_Sub1.call1cd035e5` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/Js5NetQueue.java:58`). It does not hardcode an interface id.

Opcode 224, 124, 161, 130, 209, and 237 also touch components. Opcode 209 was read: u16be with low `+ 128`, then u32be component id, applied only when `valueTextureOp1618 == 0` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1226`). The other handlers were not read. Sidebar ids are not constants in the methods that were read.

`Client.opClient168` handles design buttons. The method's button ids were not read past the opcode 78 site at `builds/rs2/468-2007-08-08/src/src/main/java/rt4/Client.java:145`.

## Component decode

`TextureOp16.opTextureOp1610` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp16.java:486`) starts:

| Order | Width | Field |
| --- | --- | --- |
| 1 | u8 | `valueTextureOp1618`, the type |
| 2 | u8 | `valueTextureOp1649` |
| 3 | u16be | `valueTextureOp16101`, the content id |
| 4 | signed u16be | `valueTextureOp1656` and `valueTextureOp1685` |
| 5 | signed u16be | `valueTextureOp1629` and `valueTextureOp16111` |
| 6 | u16be | `valueTextureOp1667` |
| 7 | u16be | `valueTextureOp1633` |
| 8 | u8 | `valueTextureOp16115` |
| 9 | u16be | `valueTextureOp16118`. 65535 becomes -1. Otherwise `value + (valueTextureOp166 & -65536)` |
| 10 | u16be | `valueTextureOp1615`. 65535 becomes -1 |
| 11 | u8 | count, then that many (u8, u16be) pairs |

The method continues at `builds/rs2/468-2007-08-08/src/src/main/java/rt4/TextureOp16.java:519` with another u8 count. That tail, and `opTextureOp1613`, were not read. Draw code branches on content id 1337 and 1338 (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:121`). A full type enum was not read.

`MaterialManager_Class106_Sub1.call1cd035e5` is the script runner. Its opcode list was not read.

Server opcode 5 sets a component colour from a u16be of 5-bit channels and a u32 with wire bytes `>>16`, `>>24`, low, `>>8` (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1744`). The packed colour is `(mid << 11) + (high << 19) + (low << 3)`.

Server opcode 94 writes a player option into slots 1..8: u8 stored as `128 - value`, another such u8, then a jstr (`builds/rs2/468-2007-08-08/src/src/main/java/rt4/GlFont.java:1409`). The jstr is cleared when it compares equal to `JString.valueJString26`. The second byte equal to 0 sets `ParticleSystem.valueParticleSystem22`. The literal of `valueJString26` was not read.
