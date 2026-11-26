# Scene

The software draw for state 30 is `GameShell.opGameShell25` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/GameShell.java:30`), called from `Client.opClient180` at `builds/rs2/481-2007-12-11/src/src/main/java/deob/Client.java:287`.

That method:

1. If the root interface `StringInterface.valueStringInterface61` is set, calls `DieseWeltIstVoll.opkind2621` (line 39).
2. Draws that interface with `C725f81a2.opkind2973(503, root, -1, 0, 0, 0, 0, 765)` (line 57). The 765 and 503 match the canvas.
3. Resets menu bookkeeping in `PlayerAppearance.calle0b912d2` (line 60) and sorts menu rows in `TextureOpRange.opTextureOpRange23` (line 61).
4. Calls `SorryInvitedPlayersOnly.opkind23013` with the local fine x, level, and fine z (line 80).

`Client.opClient180` then paints `Song.valueSong33` with `opkind2932` (line 299). `C725f81a2.opkind2973` was not read, so the viewport's model and terrain calls inside the interface draw are unread.

## Camera

Opcode 123 stores five bytes and, when the last byte is below 100, sets `LongNode.valueLongNode33` and `AudioChannel.valueAudioChannel19` from `atan2` (`SpotAnim.java:1049`). Opcode 209 stores five bytes and, on the same test, sets the camera tile from the first two bytes (`SpotAnim.java:1168`). Opcode 202 clears `SceneGraph_Class120.valueSceneGraph_Class12074` and five camera-slot flags. Opcode 99 fills those slots.

## Terrain and tiles

`Ca2c9453c` allocates `TextureOpInvert.valueTextureOpInvert34` as `SceneGraph_Class120[4][104][104]` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/Ca2c9453c.java:85`).

`World.opWorld79` walks four planes of 104 by 104. Where flag bit 1 is set it calls `opTextureOp1627` (`builds/rs2/481-2007-12-11/src/src/main/java/deob/World.java:49`). The method continues past line 69 and that tail was not read. Height samples used by scenery are `C4148662f.valuekind2587` (`C63d74d54.java:395`). Map origins are in [region.md](region.md).

## Models and sprites

Model bytes are JS5 index 7. The model classes named by the decoders are `C88f97d27` and `C1555760a`. Sprite archives are index 8 (`Js5GlTextureProvider.valueJs5GlTextureProvider13`) and index 13 (`Sprite.valueSprite92`), loaded at `SieBefindenSichEinemMitgliederGebiet.java:121`.

## Platform

This tree has no `jaggl`, `jagdx`, or `jagtheora` source. `GlModel`, `GlTile`, `GlBoundingBox`, and `Js5GlTextureProvider` are Java classes. `GlBoundingBox.opGlBoundingBox10` is the NPC mask reader, not a GL call. When `GlBoundingBox.valueGlBoundingBox17` is set, `Client.opClient180` calls `Js5GlTextureProvider.valueJs5GlTextureProvider12.opkind3141` and `opkind3143` (`Client.java:240`). Those methods were not read.
