# Revision 742

- Family: rs2
- Date: 2012-11-07
- Main class: `Client` (`Client extends GameShell`)
- Build: `Client.valueClient1529 = 742`, `SUBREVISION = 1`. `main` passes 742 and 1 into `opGameShell76` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:15, builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:1240)
- Canvas: `frameWidth = 765`, `frameHeight = 553` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:558, builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:561)
- Cache: js5 (`Js5`, `Js5Archive`, `ClientJs5TcpClient`)
- Nested sources: `a6b67f4f03dead8b26d30ad8c444b40001749c75` (short `a6b67f4`)

Feature flags in this tree, all read at builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:18: `ENABLE_HOST_CHECK` false, `ENABLE_JS5_RSA` false, `ENABLE_LOBBY` false, `ENABLE_RSA` false, `ENABLE_ISAAC` false, `CUSTOM_SEND_MAC_ADDRESS` true. The false flags change the bytes that are actually written. `rsaenc` still length-prefixes its block. ISAAC methods still add on the way out and subtract on the way in, and the addend is 0.

## Tick

`Client.mainloop_inner` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:1332) increments `loopCycle`, pumps js5, polls the keyboard and the mouse, then:

- loading states call `Loading` and `GameShell`
- rebuild states call `world.opWorld125`
- title states call the title screen, `AccountCreationManager`, and `LoginManager.login`
- lobby states call the title screen and `LoginManager.login`
- state 4 calls `LoginManager.login`
- game states call `updateGame`
- states 9 and 0 call `LoginManager.login` and then logout unless the reply is still in progress

State predicates (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:1715): game is 1, 16, or 4; lobby is 15, 19, 11, or 7; title is 6, 5, 3, 18, 14, 8, or 17; rebuild is 5, 19, 16, 7, or 17. `updateGame` returns before the entity pass unless `state == 1` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:2923). While state is 1 it reads up to 100 server packets, may write `REFLECTION_CHECK_REPLY` and `PING_STATISTICS`, calls `updatePlayers` and `updateNpcs` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:3043), `updateInterfaces` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:3083), may write `CLICKWORLDMAP`, runs interface hooks, writes `NO_TIMEOUT` when `numConnections > 50`, and flushes (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:3223). Draw is `opClient341` from `opClient398`, outside `updateGame` (builds/rs2/742-2012-11-07/src/client/src/main/java/Client.java:1461).

## Pages

- [login.md](login.md)
- [framing.md](framing.md)
- [packets.md](packets.md)
- [sync.md](sync.md)
- [region.md](region.md)
- [cache.md](cache.md)
- [input.md](input.md)
- [interfaces.md](interfaces.md)
- [scene.md](scene.md)
- [sound.md](sound.md)
- [gaps.md](gaps.md)
- [files.tsv](files.tsv)

## Status

- login: partial
- framing: specified
- packets: partial
- sync: partial
- region: partial
- cache: partial
- input: partial
- interfaces: partial
- scene: partial
- sound: partial
