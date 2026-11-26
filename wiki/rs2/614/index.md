# Revision 614

| | |
| --- | --- |
| family | rs2 |
| revision | 614 |
| date | 2010-07-13 |
| main class | `deob.Client` |
| canvas | 1024 x 768 |
| cache | js5 |
| frame title | Jagex |

`Client.main` is builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:402. The canvas and revision are stored by `GlRenderList.call618a22eb` (builds/rs2/614-2010-07-13/src/src/main/java/deob/GlRenderList.java:366).

Tick, when the toolkit mode is in game and the client is not inside a loading gate (builds/rs2/614-2010-07-13/src/src/main/java/deob/Client.java:2012): drain input, then `callad91983c` (up to 100 S2C packets when the world state is 7 and login state is 0), then on one branch `KeyHeldBinding.opKeyHeldBinding8`, then `MapCoordUtil.calla978325d(3)`. World states 12 and 13 call the login method without the packet poll. Other world states draw the login and lobby screens.

## Pages

- [Login](login.md)
- [Framing](framing.md)
- [Packets](packets.md)
- [Sync](sync.md)
- [Region](region.md)
- [Cache](cache.md)
- [Input](input.md)
- [Scene](scene.md)
- [Interfaces](interfaces.md)
- [Sound](sound.md)
- [Gaps](gaps.md)
- [Files](files.tsv)

## Status

| subsystem | status |
| --- | --- |
| login | partial |
| framing | specified |
| packets | partial |
| sync | specified |
| region | specified |
| cache | partial |
| input | partial |
| scene | partial |
| interfaces | partial |
| sound | partial |

`files.tsv` counts: read 208, classified 525, platform-native 4, unread 0.
