# Sound

Startup builds two mixers (`builds/rs2/481-2007-12-11/src/src/main/java/deob/SieBefindenSichEinemMitgliederGebiet.java:108`).

| Mixer | Rate | Archive |
| --- | --- | --- |
| `SekundeXnUbertragen.valuekind22632` is a `Cb94a47aa` | 22050 | `Sprite.callffb0c548` gets index 15 `SceneGraph_Class120.valueSceneGraph_Class12070`, index 4 `TextureOp29SubOp2.valueTextureOp29SubOp247`, and index 14 `Song.valueSong18` (line 113) |
| `Ca0f54332.valuekind3293` is a `VertexNormal` | 2048 | none in that call |

The output class is loaded by name `"rd"` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/PleaseEnterUsername.java:97`). That class is not in this tree.

## Packets

Opcode 148, size 2, is `u16le` (65535 becomes -1) into `C1b9239bd.opkind2672` (`SpotAnim.java:816`). Id -1 and the mute flag clear stops playback with `SekundeXnUbertragen.opVorbisResidue5`. Any other id plays `UnderwaterMaterialRenderer.opUnderwaterMaterialRenderer6(0, id, VorbisSound.valueVorbisSound72, C63d74d54.valuekind28411)` (`C1b9239bd.java:14`). `valueVorbisSound72` is JS5 index 6. `opUnderwaterMaterialRenderer6` was not read.

Opcode 46, size 5, is `u24le` then `u16be` (65535 becomes -1) into `MaterialManager_Class106_Sub1.opMaterialManager_Class106_Sub126` (`SpotAnim.java:827`). A non-zero volume and an id other than -1 call `Song.opkind3733(id, 0, C78d60e9c.valuekind3383, volume)` and set `C78d60e9c.valuekind3381` (`MaterialManager_Class106_Sub1.java:203`). `valuekind3383` is JS5 index 11. `Song.opkind3733` was not read.

Opcode 159 queues an area sound id, a radius, and a delay into `IntHashTable.valueIntHashTable6` and the parallel arrays (`C63d74d54.java:442`) when the local player is inside the radius and `C68c7fcfe.valuekind28812` is not 0. `Cd13a8822.opkind3222` is called from the in-game tick (`C55f3f558.java:498`) and was not read.

Opcode 188 reads `u16be`, `u8`, `u16be` and calls `C218af8f4.opkind3083` (`SpotAnim.java:868`). That method was not read.

`Client.opClient180` calls `opkind22117` on the 22050 mixer when `C78d60e9c.valuekind3381` is set (`Client.java:236`).
