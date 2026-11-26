# Scene

The software path is `Client.drawScene` (`Client.java:11159`). It clears the pixel buffer with `Pix2D.cls` (`Pix2D.java:82`; the call is `Client.java:11232`), calls `World.renderAll(cameraX, cameraZ, cameraPitch, cameraY, roofLevel, cameraYaw)` (`World.java:1130`), removes sprites, draws 2D entity elements, the tile hint, texture anims, and 3D entity elements, then blits the 512 by 334 viewport at (4, 4) (`Client.java:11239`).

Other surfaces created with the viewport (`Client.java:2706`): chat back 479 by 96, map back 172 by 156, sidebar 190 by 261. The applet canvas is 765 by 503.

`World.renderAll` clamps the camera into the world, loads pitch and yaw from `Model.sinTable` / `Model.cosTable`, and sets the visible tile window to 25 tiles around the camera tile (`World.java:1132`). The loops after that window is computed were not read. See [gaps.md](gaps.md).

There is no `jaggl`, `jagdx`, or `jagtheora` class in this tree. No HD entry point is named.

## Terrain and locs

Land and loc bytes are in [region.md](region.md). The world arrays are 104 by 104 by 4. Loc shapes map through `LOC_SHAPE_TO_LAYER` (`Client.java:741`): indexes 0..3 are layer 0, 4..8 are layer 1, 9..21 are layer 2, and 22 is layer 3.

## Models

`Model.unpack` (`Model.java:129`) reads the last 18 bytes as the header: u16be vertex count, u16be face count, u8 textured face count, then five u8 flags and four u16be sizes (vertex x bytes, vertex y bytes, vertex z bytes, face-vertex smart size). Flag 255 on the second flag means per-face priorities. Flag 1 on the other four flags means face info, face alpha, face labels, and vertex labels are present.

`new Model(id)` (`Model.java:289`) reads vertices as a u8 flag plus `gsmart` deltas for x, y, and z when bits 1, 2, and 4 are set. Faces store a u16be colour and optional u8 info, priority, alpha, and label. Face vertices use a u8 opcode 1..4 and `gsmart` deltas (`Model.java:404`). Textured faces are three u16be indexes (`Model.java:450`).

`Pix3D.drawGouraudScanline` (`Pix3D.java:949`) and `Pix3D.drawTexturedScanline` (`Pix3D.java:2278`) were not read. Textures are unpacked from the textures jag by `Pix3D.unpackTextures` (`Client.java:6062`).

## Sprites

Minimap and sidebar sprites come from the media jag (`Client.java:5938`): `invback`, `chatback`, `mapback`, `sideicons` 0..12, `compass`, `mapscene`, `mapfunction`, `hitmarks`, `headicons`, `mapdots` 0..3, and the redstone tab images. Drawing those sprites is `PixMap` and `Sprite` plot calls from `drawGame`. `Sprite` plot bodies were not read; the scene entry that blits the viewport was.
