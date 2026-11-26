# Packets

Citations are under `builds/rs2/377-2006-05-02/src/src/main/java/`. Width names are the `Packet` methods in [framing.md](framing.md).

S2C `size` is `Protocol.valueProtocol2`. There is no C2S size table, so every C2S size cell is `no-table`. The same opcode number can exist in both directions.

Widget menu values in `useMenuOption` (`jagex2/client/Client.java:10645`): `var2` is `valueClient1111`, `var3` is `valueClient1112`, `var5` is `valueClient1114`. On the scene, `var2` is local x and `var3` is local z, added to `valueClient1170` and `valueClient1171`. On a widget, `var3` is the component id and `var2` is the slot. `valueClient1274` is the selected slot, `valueClient1275` the selected component, `valueClient1276` the selected item (`Client.java:11508`). `valueClient1297` is the selected spell component (`Client.java:11123`). A loc id taken from a scene bitset is `var5 >> 14 & 32767`.

## S2C

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| s2c | 2 | 4 | `g2_alt3` component, `g2b_alt2` seq | `read` | `jagex2/client/Client.java:3630` |
| s2c | 3 | 6 | `u8` x, `u8` z, `u16be` height, `u8`, `u8`. The last `u8` at least 100 snaps the camera | `read` | `jagex2/client/Client.java:3613` |
| s2c | 5 | 0 | none. Calls `logout` and returns false | `read` | `jagex2/client/Client.java:2781` |
| s2c | 6 | 0 | none. Opens the text prompt (`valueClient1366 = 2`) | `read` | `jagex2/client/Client.java:2686` |
| s2c | 10 | 3 | `g1_alt3` tab, `g2_alt2` interface. 65535 stores -1 | `read` | `jagex2/client/Client.java:3661` |
| s2c | 13 | 0 | none. Clears entity animations | `read` | `jagex2/client/Client.java:2546` |
| s2c | 18 | 6 | `u16be`, `g2_alt2` component, `g2_alt1`. Stored as `(first<<16)+third` | `read` | `jagex2/client/Client.java:3779` |
| s2c | 21 | 6 | `u16be` zoom, `g2_alt1` item (65535 clears type 0), `g2_alt3` component. Sets model type 4 | `read` | `jagex2/client/Client.java:3593` |
| s2c | 26 | 5 | `u16be` wave, `u8` volume, `u16be` delay. See [sound.md](sound.md) | `read` | `jagex2/client/Client.java:2507` |
| s2c | 29 | 0 | none. Closes open interfaces | `read` | `jagex2/client/Client.java:2804` |
| s2c | 40 | 2 | `g1_alt3` zone z, `g1_alt2` zone x | `read` | `jagex2/client/Client.java:3116` |
| s2c | 41 | 4 | zone sound. See [sound.md](sound.md) | `readZonePacket` | `jagex2/client/Client.java:12602` |
| s2c | 49 | 6 | `g1_alt2` skill, `u8` into `valueClient1159`, `u32be` xp into `valueClient975`. Level `valueClient1183` is derived from `valueClient1084`, not sent | `read` | `jagex2/client/Client.java:3298` |
| s2c | 50 | 2 | `i16be` into `valueClient1399` | `read` | `jagex2/client/Client.java:2917` |
| s2c | 53 | -2 | instanced rebuild. See [region.md](region.md) | `read` | `jagex2/client/Client.java:3351` |
| s2c | 58 | 0 | none. Opens the numeric prompt (`valueClient1366 = 1`) | `read` | `jagex2/client/Client.java:3099` |
| s2c | 59 | 6 | `u8` offset, `u16be` spotanim, `u8` height, `u16be` delay | `readZonePacket` | `jagex2/client/Client.java:12618` |
| s2c | 61 | 0 | none. Clears the cross destination | `read` | `jagex2/client/Client.java:2956` |
| s2c | 63 | -1 | `jstr`. Suffixes `:tradereq:`, `:duelreq:`, and `:chalreq:` are special-cased | `read` | `jagex2/client/Client.java:2861` |
| s2c | 67 | 4 | four `u8` camera-shake values | `read` | `jagex2/client/Client.java:3004` |
| s2c | 71 | -2 | NPC update. See [sync.md](sync.md) | `readNpcInfo` | `jagex2/client/Client.java:3644` |
| s2c | 75 | 2 | `g1_alt2` zone x, `g1_alt1` zone z | `read` | `jagex2/client/Client.java:3726` |
| s2c | 76 | 23 | `g2_alt1`, `g2_alt3`, `u16be` ignored, `u16be`, `g2_alt1`, `g2_alt2`, `g2_alt2`, `u16be`, `g4_alt1` host, `g2_alt3`, `g1_alt1`. Then a DNS lookup | `read` | `jagex2/client/Client.java:2844` |
| s2c | 78 | 9 | `u64be` name, `u8` world | `read` | `jagex2/client/Client.java:3041` |
| s2c | 82 | 3 | `u8` hide when 1, `u16be` component | `read` | `jagex2/client/Client.java:2932` |
| s2c | 88 | 2 | `g1_alt3` offset, `g1_alt3` type and rotation. Loc id is forced to -1 | `readZonePacket` | `jagex2/client/Client.java:12646` |
| s2c | 90 | -2 | player update. See [sync.md](sync.md) | `readPlayerInfo` | `jagex2/client/Client.java:3788` |
| s2c | 106 | 7 | `g1_alt1` offset, `g2_alt3`, `g2_alt2`, `g2_alt2` player. Skipped when the player is self | `readZonePacket` | `jagex2/client/Client.java:12466` |
| s2c | 107 | 5 | `u16be` obj, `g1_alt2` offset, `g2_alt2` count | `readZonePacket` | `jagex2/client/Client.java:12538` |
| s2c | 109 | 2 | `u16be` into `valueClient1120` | `read` | `jagex2/client/Client.java:2578` |
| s2c | 113 | 0 | none. Copies server varps from `valueClient1137` into `valueClient1169` | `read` | `jagex2/client/Client.java:3795` |
| s2c | 115 | 6 | `g4_alt3` value, `g2_alt1` varp id | `read` | `jagex2/client/Client.java:2787` |
| s2c | 121 | 7 | `u8` offset, `u16be` obj, `u16be` old count, `u16be` new count | `readZonePacket` | `jagex2/client/Client.java:12556` |
| s2c | 125 | 1 | `u8` into `valueClient1443`. Redraws when tab 12 is open. Script opcode 11 reads it | `read` | `jagex2/client/Client.java:3583` |
| s2c | 126 | 3 | `u8` members into `valueClient1197`, `g2_alt1` local player index `valueClient1093` | `read` | `jagex2/client/Client.java:3719` |
| s2c | 128 | 4 | `g2_alt2` into `valueClient1294`, `g2_alt3` into `valueClient1217` | `read` | `jagex2/client/Client.java:2962` |
| s2c | 134 | -2 | `u16be` component, then while `pos < size`: `gsmarts` slot, `u16be` item, `u8` count, or 255 then `u32be` count | `read` | `jagex2/client/Client.java:3018` |
| s2c | 135 | -1 | `u64be` name, `u32be` message id, `u8` rights, then `WordPack.opWordPack1` on `size-13` bytes. The last 100 ids are deduped | `read` | `jagex2/client/Client.java:3152` |
| s2c | 142 | 4 | `u16be` seq, `g1_alt1` type and rotation, `u8` offset | `readZonePacket` | `jagex2/client/Client.java:12485` |
| s2c | 148 | 0 | none. Cancels the camera move and clears five shakes | `read` | `jagex2/client/Client.java:3708` |
| s2c | 152 | 4 | `g1_alt2` type and rotation, `g2_alt3` loc, `g1_alt1` offset | `readZonePacket` | `jagex2/client/Client.java:12632` |
| s2c | 156 | 1 | `u8` into `valueClient1179`. 0 allows minimap clicks | `read` | `jagex2/client/Client.java:2563` |
| s2c | 157 | -1 | `g1_alt2` slot, `jstr` (the text `null` stores null), `u8` priority | `read` | `jagex2/client/Client.java:2669` |
| s2c | 158 | 2 | `g2b_alt1` into `valueClient1316` | `read` | `jagex2/client/Client.java:2646` |
| s2c | 159 | 2 | `g2_alt3` into `valueClient1294` | `read` | `jagex2/client/Client.java:3214` |
| s2c | 162 | 4 | `g2_alt2`, `g2_alt1` component. Sets model type 2 | `read` | `jagex2/client/Client.java:2569` |
| s2c | 166 | 6 | `g2b_alt1` into `valueComponent137`, `g2b_alt1` into `valueComponent107`, `u16be` component | `read` | `jagex2/client/Client.java:2475` |
| s2c | 167 | 6 | `u8` x, `u8` z, `u16be` height, `u8`, `u8`. The last `u8` at least 100 snaps the look-at angles | `read` | `jagex2/client/Client.java:2751` |
| s2c | 174 | 2 | `i16be` into `valueClient1160`. Redraws when tab 12 is open. Script opcode 12 reads it | `read` | `jagex2/client/Client.java:2940` |
| s2c | 181 | 15 | `u8` offset, `i8` dx, `i8` dz, `i16be`, `u16be` spotanim, `u8` times 4, `u8` times 4, `u16be`, `u16be`, `u8`, `u8`. Spotanim 65535 is skipped | `readZonePacket` | `jagex2/client/Client.java:12577` |
| s2c | 182 | 3 | `g2_alt2` varp, `g1b_alt3` value | `read` | `jagex2/client/Client.java:2529` |
| s2c | 183 | -2 | `u8` base x, `g1_alt1` base z, then while `pos < size`: `u8` sub-opcode and `readZonePacket` | `read` | `jagex2/client/Client.java:3201` |
| s2c | 186 | 8 | `g2_alt2`, `g2_alt3` component, `g2_alt2`, `g2_alt1` | `read` | `jagex2/client/Client.java:2486` |
| s2c | 187 | 6 |  | `read` logs `T1` and logs out. The 6 bytes are not parsed | `jagex2/client/Client.java:3841` |
| s2c | 190 | 2 | `g2_alt1` times 30 into `valueClient1186` | `read` | `jagex2/client/Client.java:3571` |
| s2c | 199 | 6 | `u8` type. Type 1 then reads `u16be`. Types 2..6 set an origin and read `u16be`, `u16be`, `u8`. Type 10 reads `u16be`. The frame length stays 6 | `read` | `jagex2/client/Client.java:2705` |
| s2c | 200 | 4 | `u16be` component, `g2_alt3` scroll, clamped for type 0 | `read` | `jagex2/client/Client.java:3821` |
| s2c | 201 | 3 | three `u8`: public, private, trade | `read` | `jagex2/client/Client.java:2695` |
| s2c | 203 | 14 | `readZonePacket` reads `u16be` loc, `u8` type and rotation, `g1b_alt2`, `g1_alt1` offset, `g1b_alt1`, `g2_alt2`, `g2_alt1` player, `i8`, `g1b_alt1`, `u16be`, `u16be` (16 bytes). The table size is 14. See [gaps.md](gaps.md) | `readZonePacket` | `jagex2/client/Client.java:12402` |
| s2c | 206 | -2 | `u16be` component, `u16be` count, then that many `g2_alt3` item and `g1_alt2` count, or 255 then `g4_alt1` | `read` | `jagex2/client/Client.java:3317` |
| s2c | 208 | 3 | `g2_alt2` obj, `g1_alt1` offset | `readZonePacket` | `jagex2/client/Client.java:12660` |
| s2c | 216 | 4 | `g2_alt3` model, `g2_alt3` component. Sets model type 1 | `read` | `jagex2/client/Client.java:2498` |
| s2c | 218 | 4 | `u16be` component, `g2_alt2` colour | `read` | `jagex2/client/Client.java:2658` |
| s2c | 219 | 2 | `g2_alt1` component. The inventory slots are written -1 and then 0 | `read` | `jagex2/client/Client.java:3679` |
| s2c | 220 | 2 | `g2_alt3` midi. 65535 means -1 | `read` | `jagex2/client/Client.java:2615` |
| s2c | 222 | 4 | `u16be` zone z, `g2_alt3` zone x. See [region.md](region.md) | `read` | `jagex2/client/Client.java:3345` |
| s2c | 226 | -2 | `size/8` times `u64be` ignore names | `read` | `jagex2/client/Client.java:3650` |
| s2c | 232 | -2 | `g2_alt3` component, `jstr` text | `read` | `jagex2/client/Client.java:3808` |
| s2c | 233 | 1 | `u8` into `valueClient1438` | `read` | `jagex2/client/Client.java:2950` |
| s2c | 238 | 1 | `u8` into `valueClient1338` | `read` | `jagex2/client/Client.java:3692` |
| s2c | 246 | 2 | `g2_alt3` into `valueClient1217` | `read` | `jagex2/client/Client.java:3256` |
| s2c | 249 | 5 | `g2_alt1` midi, `g3_alt3` delay | `read` | `jagex2/client/Client.java:2632` |
| s2c | 251 | 1 | `u8` into `valueClient992` | `read` | `jagex2/client/Client.java:3772` |
| s2c | 252 | 1 | `g1_alt2` selected tab `valueClient1405` | `read` | `jagex2/client/Client.java:3108` |
| s2c | 253 | 4 | `g2_alt1` into `valueClient1092`, `g2_alt2` into `valueClient1182` | `read` | `jagex2/client/Client.java:3733` |
| s2c | 255 | 2 | `g2_alt3` component. Sets model type 3 from the local appearance | `read` | `jagex2/client/Client.java:3139` |

## C2S

| direction | opcode | size | fields | handler | citation |
| --- | --- | --- | --- | --- | --- |
| c2s | 1 | no-table | `u16be` id, `p2_alt1` selected slot, `p2_alt1` selected item, `p2_alt3` selected component, `p2_alt2` slot, `p2_alt2` component | `useMenuOption` action 903 | `jagex2/client/Client.java:10915` |
| c2s | 3 | no-table | `p2_alt2` id, `u16be` component, `u16be` slot | `useMenuOption` action 9 | `jagex2/client/Client.java:10765` |
| c2s | 4 | no-table | `p2_alt1` slot, `p2_alt3` id, `p2_alt3` component | `useMenuOption` action 891 | `jagex2/client/Client.java:11146` |
| c2s | 6 | no-table | empty. Sent when the map has finished loading | `opClient53` | `jagex2/client/Client.java:13419` |
| c2s | 8 | no-table | `p2_alt1` index | `useMenuOption` action 432 | `jagex2/client/Client.java:10990` |
| c2s | 13 | no-table | `p2_alt3` npc index. Action 118 may first send opcode 157 | `useMenuOption` action 118 | `jagex2/client/Client.java:10969` |
| c2s | 19 | no-table | `u32be` click. See [input.md](input.md) | `updateGame` | `jagex2/client/Client.java:1696` |
| c2s | 22 | no-table | `u16be` 38304 | `handleChatSettingsInput` | `jagex2/client/Client.java:4314` |
| c2s | 24 | no-table | `p2_alt1` component, `p2_alt1` id, `p2_alt2` slot | `useMenuOption` action 399 | `jagex2/client/Client.java:10848` |
| c2s | 28 | no-table | map walk. See [input.md](input.md) | `opClient135` | `jagex2/client/Client.java:4091` |
| c2s | 31 | no-table | `u16be` player index, `p2_alt1` spell component | `useMenuOption` action 918 | `jagex2/client/Client.java:11007` |
| c2s | 36 | no-table | `u16be` spell component, `p2_alt2` component, `p2_alt2` slot, `p2_alt2` id | `useMenuOption` action 361 | `jagex2/client/Client.java:10936` |
| c2s | 40 | no-table | empty keepalive, also during `buildScene` | `updateGame` | `jagex2/client/Client.java:1972` |
| c2s | 42 | no-table | `p2_alt1` npc index | `useMenuOption` action 553 | `jagex2/client/Client.java:10790` |
| c2s | 45 | no-table | `p2_alt2` player index | `useMenuOption` action 876 | `jagex2/client/Client.java:10711` |
| c2s | 49 | no-table | chat. See [input.md](input.md) | `handleInputKey` | `jagex2/client/Client.java:2352` |
| c2s | 50 | no-table | `p2_alt2` z plus base, `p2_alt1` loc id, `p2_alt3` x plus base | `useMenuOption` action 888 | `jagex2/client/Client.java:11199` |
| c2s | 54 | no-table | `p2_alt2` id, `p2_alt1` z plus base, `u16be` x plus base | `useMenuOption` action 930 | `jagex2/client/Client.java:10841` |
| c2s | 55 | no-table | `p2_alt1` loc id, `p2_alt1` z plus base, `u16be` x plus base | `useMenuOption` action 1280 | `jagex2/client/Client.java:11183` |
| c2s | 56 | no-table | command. See [input.md](input.md) | `handleInputKey` | `jagex2/client/Client.java:2289` |
| c2s | 57 | no-table | `u16be` npc index, `p2_alt1` selected item, `p2_alt3` selected component, `u16be` selected slot | `useMenuOption` action 347 | `jagex2/client/Client.java:10873` |
| c2s | 67 | no-table | `p2_alt2` npc index | `useMenuOption` action 921 | `jagex2/client/Client.java:10724` |
| c2s | 71 | no-table | `p2_alt3` id, `p2_alt3` x plus base, `p2_alt2` z plus base. Action 684 may first send opcode 222 | `useMenuOption` action 684 | `jagex2/client/Client.java:11063` |
| c2s | 75 | no-table | `u32be` amount | `handleInputKey` | `jagex2/client/Client.java:2205` |
| c2s | 77 | no-table | `p2_alt2` x plus base, `u16be` z plus base, `p2_alt3` id | `useMenuOption` action 68 | `jagex2/client/Client.java:11037` |
| c2s | 78 | no-table | `u32be` 1057001181, when the game frame is open | `buildScene` | `jagex2/client/Client.java:9060` |
| c2s | 79 | no-table | `u16be` component | `useMenuOption` actions 890, 352, 518 | `jagex2/client/Client.java:10882` |
| c2s | 80 | no-table | `u16be` wave id masked with 32767, or `u16be` -1 | `opClient127` | `jagex2/client/Client.java:13935` |
| c2s | 83 | no-table | `p2_alt1` id, `u16be` z plus base, `p2_alt1` spell, `p2_alt3` x plus base | `useMenuOption` action 199 | `jagex2/client/Client.java:11495` |
| c2s | 91 | no-table | `p2_alt1` id, `p2_alt3` slot, `u16be` component | `useMenuOption` action 444 | `jagex2/client/Client.java:11374` |
| c2s | 95 | no-table | `u32be` 0, before opcode 100 on action 26 | `useMenuOption` | `jagex2/client/Client.java:11362` |
| c2s | 100 | no-table | `u16be` x plus base, `p2_alt2` z plus base, `p2_alt3` id | `useMenuOption` action 26 | `jagex2/client/Client.java:11367` |
| c2s | 104 | no-table | `p2_alt2` spell component, `p2_alt1` npc index | `useMenuOption` action 67 | `jagex2/client/Client.java:11021` |
| c2s | 110 | no-table | empty | `opClient128` | `jagex2/client/Client.java:973` |
| c2s | 112 | no-table | `p2_alt1` npc index | `useMenuOption` action 318 | `jagex2/client/Client.java:11480` |
| c2s | 116 | no-table | `p2_alt1` player index. Action 677 uses `var5`. Action 544 uses `valueClient1104[var34]` | `useMenuOption` | `jagex2/client/Client.java:10803` |
| c2s | 119 | no-table | `u8` tab | `drawGame` | `jagex2/client/Client.java:7213` |
| c2s | 120 | no-table | `u64be` name | `addFriend` | `jagex2/client/Client.java:9411` |
| c2s | 123 | no-table | `p2_alt3` destination slot, `p1_alt1` mode, `p2_alt2` component, `p2_alt1` source slot | `updateGame` | `jagex2/client/Client.java:1816` |
| c2s | 126 | no-table | `u8` 125, before opcode 203 on action 961 | `useMenuOption` | `jagex2/client/Client.java:10732` |
| c2s | 136 | no-table | `u16be` x plus base, `p2_alt1` z plus base, `u16be` loc id | `useMenuOption` action 892 | `jagex2/client/Client.java:11272` |
| c2s | 140 | no-table | `p2_alt1` `valueClient1371`, `p2_alt1` `valueClient1372` | `updateGame` | `jagex2/client/Client.java:1711` |
| c2s | 141 | no-table | `u64be` name | `opClient144` | `jagex2/client/Client.java:5057` |
| c2s | 143 | no-table | `p2_alt1` selected item `valueClient1276`, `p2_alt3` selected slot `valueClient1274`, `u16be` selected component `valueClient1275`, `p2_alt2` player index | `useMenuOption` action 596 | `jagex2/client/Client.java:11303` |
| c2s | 152 | no-table | `p2_alt1` loc id, `p2_alt1` selected component, `p2_alt1` selected item, `p2_alt1` z plus base, `u16be` selected slot, `p2_alt3` x plus base | `useMenuOption` action 467 | `jagex2/client/Client.java:10755` |
| c2s | 157 | no-table | `u32be` 0, before opcode 13 on action 118 | `useMenuOption` | `jagex2/client/Client.java:10964` |
| c2s | 158 | no-table | `p2_alt3` slot, `p2_alt3` id, `p2_alt1` component | `useMenuOption` action 894 | `jagex2/client/Client.java:11164` |
| c2s | 160 | no-table | `u64be` name | `removeIgnore` | `jagex2/client/Client.java:9226` |
| c2s | 161 | no-table | `p2_alt3` slot, `p2_alt3` id, `p2_alt1` component | `useMenuOption` action 324 | `jagex2/client/Client.java:11206` |
| c2s | 163 | no-table | `u8` gender, seven `u8` kits, five `u8` colours | `handleInterfaceAction` | `jagex2/client/Client.java:5444` |
| c2s | 165 | no-table | `u8` 206, before opcode 228 on action 227 | `useMenuOption` | `jagex2/client/Client.java:10681` |
| c2s | 168 | no-table | empty, every 85 redraws | `drawGame` | `jagex2/client/Client.java:7098` |
| c2s | 171 | no-table | mouse samples. See [input.md](input.md) | `updateGame` | `jagex2/client/Client.java:1601` |
| c2s | 173 | no-table | `u24be` 2657152 | `createMinimap` | `jagex2/client/Client.java:10377` |
| c2s | 176 | no-table | three `u8` chat modes | `handleInputKey` | `jagex2/client/Client.java:2168` |
| c2s | 177 | no-table | `p2_alt2` slot, `p2_alt1` id, `p2_alt1` component | `useMenuOption` action 225 | `jagex2/client/Client.java:11103` |
| c2s | 181 | no-table | `p2_alt2` x plus base, `p2_alt1` z plus base, `p2_alt1` loc id | `useMenuOption` action 35 | `jagex2/client/Client.java:11191` |
| c2s | 184 | no-table | `u64be` name, `u8` reason-601, `u8` mute | `handleInterfaceAction` | `jagex2/client/Client.java:5435` |
| c2s | 187 | no-table | `u8` 1 on focus gain, `u8` 0 on focus loss | `updateGame` | `jagex2/client/Client.java:1718` |
| c2s | 194 | no-table | `p2_alt1` player index | `useMenuOption` action 408 | `jagex2/client/Client.java:11537` |
| c2s | 197 | no-table | `u32be` 0 | `Client.java:10034` | `jagex2/client/Client.java:10034` |
| c2s | 202 | no-table | empty | `updateGame` | `jagex2/client/Client.java:1897` |
| c2s | 203 | no-table | `p2_alt2` component, `p2_alt1` slot, `p2_alt1` id | `useMenuOption` action 961 | `jagex2/client/Client.java:10737` |
| c2s | 206 | no-table | `u64be` base37 name | `handleInputKey` | `jagex2/client/Client.java:2225` |
| c2s | 210 | no-table | `u16be` spell component, `p2_alt1` loc id, `p2_alt2` x plus base, `p2_alt1` z plus base | `useMenuOption` action 376 | `jagex2/client/Client.java:10975` |
| c2s | 211 | no-table | `p2_alt3` selected slot, `p2_alt2` selected item, `p2_alt3` z plus base, `p2_alt3` x plus base, `p2_alt1` selected component, `p2_alt1` ground id | `useMenuOption` action 100 | `jagex2/client/Client.java:11320` |
| c2s | 213 | no-table | minimap walk plus 14 bytes. See [input.md](input.md) | `opClient135` and `opClient125` | `jagex2/client/Client.java:4097` |
| c2s | 217 | no-table | `u64be` name | `addIgnore` | `jagex2/client/Client.java:8761` |
| c2s | 222 | no-table | `u24be` 11257922, before opcode 71 on action 684 | `useMenuOption` | `jagex2/client/Client.java:11058` |
| c2s | 226 | no-table | `u16be` component | `useMenuOption` action 575 | `jagex2/client/Client.java:11265` |
| c2s | 227 | no-table | private message. See [input.md](input.md) | `handleInputKey` | `jagex2/client/Client.java:2156` |
| c2s | 228 | no-table | `p2_alt1` slot, `p2_alt2` id, `u16be` component | `useMenuOption` action 227 | `jagex2/client/Client.java:10686` |
| c2s | 230 | no-table | `p2_alt1` id, `p2_alt2` x plus base, `u16be` z plus base | `useMenuOption` action 270 | `jagex2/client/Client.java:11288` |
| c2s | 231 | no-table | `p2_alt3` component, `p2_alt1` slot, `u16be` id | `useMenuOption` action 564 | `jagex2/client/Client.java:11415` |
| c2s | 233 | no-table | `p2_alt2` player index | `useMenuOption` action 493 | `jagex2/client/Client.java:10901` |
| c2s | 241 | no-table | `u16be` loc id, `u16be` x plus base, `p2_alt2` z plus base | `useMenuOption` action 389 | `jagex2/client/Client.java:11408` |
| c2s | 244 | no-table | `u8` 0 then `psize1` over `u8` 219, `u16be` 37745, `u8` 61, `u16be` 43756, `u16be` random, `u8` random, `u16be` 51171, optional `u16be` 15808, `u8` 97, `u8` random | `opClient116` | `jagex2/client/Client.java:10452` |
| c2s | 245 | no-table | `p2_alt3` player index. Action 200 uses `var5`. Action 695 uses `valueClient1104[var34]` | `useMenuOption` | `jagex2/client/Client.java:10673` |
| c2s | 247 | no-table | interaction walk. See [input.md](input.md) | `opClient135` | `jagex2/client/Client.java:4102` |
| c2s | 248 | no-table | empty | `opClient47` | `jagex2/client/Client.java:4942` |
