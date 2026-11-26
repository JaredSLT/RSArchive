# Cache

Kind is js5 (`builds/rs3/902-2018-12-14/meta.json`, `engine` is `js5`). `cache_downloaded` is false. The README records no OpenRS2 disk cache. Nothing was downloaded. Land and config bytes are absent (`gaps.md`, class `blocked`, reason `no-cache`).

## Master index

`Js5.doJs52` requests archive 255 group 255 (`com/jagex/js5/Js5.java:44`). The HTTP fallback is `http://host:port/ms?m=&a=&g=` (`Js5.java:68`). `DataBlock374` builds `Js5ResourceProvider` with `Unused1.bigInteger3` and `Unused1.bigInteger4` (`deob/DataBlock374.java:166`). The exponent is `10001` hex and the modulus is the literal at `com/jagex/Unused1.java:13` and `:15`.

Compression (`com/jagex/js5/Js5CompressionType.java:9`): 0 uncompressed, 1 bzip2, 2 gzip, 3 lzma.

## Archive ids

`FileUtil` stores the id as the first constructor argument. The name string is empty. Login CRCs are one `u32be` per `FileUtil.doFileUtil()` entry except `fileUtil28` (`SecureRandomTask.java:1185`).

| Field | Id | Opened as |
| --- | --- | --- |
| fileUtil | 0 | `clsbde36bc429` skeletons, passed into BAS (`DataBlock374.java:242`, `:370`) |
| fileUtil2 | 1 | `clsbde36bc442` |
| fileUtil3 | 2 | `clsbde36bc423` config: npc, obj, loc, enum, varbit, param, var domains, sequences, floor overlay |
| fileUtil4 | 3 | `clsbde36bc413` interfaces (`:247`, `ShaderRelated.doShaderRelated` at `:385`) |
| fileUtil5 | 5 | `clsbde36bc422` maps, the rebuild index |
| fileUtil6 | 7 | `clsbde36bc415` sprites |
| fileUtil7 | 8 | `clsbde36bc445` models |
| fileUtil8 | 10 | `clsbde36bc426` |
| fileUtil9 | 12 | `clsbde36bc411` |
| fileUtil10 | 13 | `clsbde36bc416` |
| fileUtil11 | 14 | `clsbde36bc441` |
| fileUtil12 | 16 | `clsbde36bc428` |
| fileUtil13 | 17 | `clsbde36bc44` skybox |
| fileUtil14 | 18 | `clsbde36bc45` sky decor |
| fileUtil15 | 19 | `clsbde36bc43` hitmarks |
| fileUtil16 | 20 | `clsbde36bc438` BAS |
| fileUtil17 | 21 | `clsbde36bc49` floor underlay |
| fileUtil18 | 22 | `clsbde36bc440` |
| fileUtil19 | 23 | `clsbde36bc443` |
| fileUtil20 | 24 | `clsbde36bc427` |
| fileUtil21 | 25 | `clsbde36bc419` |
| fileUtil22 | 26 | `clsbde36bc435` textures |
| fileUtil23 | 27 | `clsbde36bc425` |
| fileUtil24 | 28 | `clsbde36bc46` defaults and the buss blob (`:177`, `:302`) |
| fileUtil25 | 29 | `clsbde36bc420` |
| fileUtil26 | 30 | `clsbde36bc436` hardware platform |
| fileUtil27 | 31 | `clsbde36bc47` |
| fileUtil28 | 32 | sprites when software (`:174`); omitted from the login CRC loop |
| fileUtil29 | 33 | `clsbde36bc432` loading fonts |
| fileUtil30 | 34 | sprites when hardware (`:174`) |
| fileUtil31 | 35 | `clsbde36bc433` |
| fileUtil32 | 40 | `clsbde36bc48` |
| fileUtil33 | 41 | `clsbde36bc42` |
| fileUtil34 | 42 | declared, not opened in the loading list that was read |
| fileUtil35 | 47 | declared, not opened there |
| fileUtil36 | 48 | declared, not opened there |
| fileUtil37 | 49 | `clsbde36bc4` |
| fileUtil38 | 52 | declared, not opened there |
| fileUtil39 | 53 | `clsbde36bc434` surfaces |
| fileUtil40 | 54 | declared, not opened there |
| fileUtil41 | 55 | declared, not opened there |
| fileUtil42 | 56 | `clsbde36bc437` |

Ids present: 0, 1, 2, 3, 5, 7, 8, 10, 12, 13, 14, 16 through 35, 40, 41, 42, 47, 48, 49, 52, 53, 54, 55, 56. `FileUtil.java:8` through `:90`.

Config decode opcode switches were not read. The list constructors are `DataBlock374.java:348` through `:389`.
