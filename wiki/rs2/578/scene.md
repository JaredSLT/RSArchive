# Scene

Paths are under `builds/rs2/578-2009-12-16/src/src/main/java/`.

## Toolkit

`StarteSoftwarebibliothek.opkind9842` (`deob/StarteSoftwarebibliothek.java:17`) stores the live toolkit in `TextureOp19.valueTextureOp1989` and the requested kind in `C7bdaa717.valuekind7415`. A failure falls back to kind 0.

`C870b26fe.opkind8692` (`deob/C870b26fe.java:27`):

- kind 0: `Cfdcd577a.opkind9621`, which returns `new VorbisCodebook(canvas, ...)` (`deob/Cfdcd577a.java:12`). `VorbisCodebook` extends `C870b26fe` (`deob/VorbisCodebook.java:7`). The class name is not the audio decoder.
- kind 1: `Ce7f0d2ee.opkind6573` loads `Class.forName("bf")` and calls its constructor `(Canvas, C7cbf08df7, int, int, AnimationNode)` (`deob/Ce7f0d2ee.java:120`). Class `bf` is not in this tree.
- any other kind throws `IllegalArgumentException("UM")`.

`deob.MidiDecoder` extends `C870b26fe`, imports `jaggl.opengl` and `jaggl.context`, and calls `System.loadLibrary("jaggl")` (`deob/MidiDecoder.java:186`). Nothing read in this pass executes `new MidiDecoder`. The native files are `jaggl/opengl.java`, `jaggl/context.java`, and `jaggl/X11/UsrLibModulesDri.java`. There is no `jagdx` or `jagtheora` tree.

## What the software path draws

`VorbisCodebook.opVorbisCodebook23` (`VorbisCodebook.java:227`) clears through `Unused28`: it sets the raster flags and calls `Unused28.call551471d5(10, 50, 90, 5, 75, 15, -65536, -65536, -65536)`. Model drawing goes through `opMidiDecoder11`, which casts to `LoginProt` (`VorbisCodebook` model path, cited from the class body around the `LoginProt` cast). `LoginProt` is a model, not the login writer. The scene-graph walk that decides which models and sprites are submitted was not read.

Height used by the camera follow is `C6402f7a6.opkind7663`. The follow method `C8d92094d.opkind9865` (`deob/C8d92094d.java:59`) was opened only at its first lines: it samples that height at a tile center. The rest of the camera lerp was not read.

Tile culling and the per-pass draw lists were not read.
