# RSLib

Pure C (C99) library, built with CMake, for writing RuneScape **servers and clients** that speak to the snapshots in this archive.

This is a library, not a game. Your program links `librslib` and decides policy (login, world, which caches to open).

**Set the build once on `rs_ctx`.** Every later call — cache, login, packets, update protocol — uses that build. Login length, CRC count, RSA, and success extras follow that build.

## Layout

Sources and headers are split by role, then by function, then by build:

```
src/server/packets.c            # server packet API
src/server/packets/317.c        # 317 s2c opcodes + sizes
src/server/login.c              # login handling
src/server/update.c             # on-demand / JS5 serving
src/both/isaac.c                # ISAAC
src/both/xtea.c                 # XTEA
src/client/connect.c            # outbound TCP
```

Public headers live under `rslib/both/`, `rslib/client/`, `rslib/server/`, and `rslib/proto.h`.

```c
rs_ctx *ctx = rs_ctx_create();
rs_ctx_set_build_id(ctx, "317");

rs_server *srv = rs_server_create(ctx, &server_hooks);
rs_client *cli = rs_client_create(ctx, &client_hooks);
```

`rs_proto_for(build)` returns the vtable for that revision (`rs_proto_rsc`, `rs_proto_rs2`, `rs_proto_js5`, or `rs_proto_js5_lobby` on every RS3 row). Handshake, login, game frames, and the update protocol all go through the same `rs_session_*` calls. A session role (`RS_ROLE_SERVER` or `RS_ROLE_CLIENT`) flips ISAAC and size tables so a client write is a server read. `rs_codec_for(build)` returns the codec for the whole game payload on that row (`rs_codec_plain`, `rs_codec_rs2`, `rs_codec_js5_474`, `rs_codec_js5_508`, `rs_codec_js5_530`, or `rs_codec_rs3`), not only region and walk. Chat input differs: the RS2 object writes the 317 colour/effect header, the JS5 objects write wordpack of the text only, and RSC returns `RS_ERR_UNSUPPORTED`. Typed sends are `rs_conn_send_*` and `rs_client_send_*`. A JS5 handshake with a known JS5 revision selects that build before the reply, while a login revision selects the build only when `rs_proto_for` matches (so 474 can become 530, and 474 does not become 910).

## Builds

| Id | Rev | Era | Cache | Game protocol extras |
|----|-----|-----|-------|----------------------|
| 2001-115 | 115 | RSC | `.jag` | |
| 2002-159 | 159 | RSC | `.jag` | |
| 2003-177 | 177 | RSC | `.jag` | |
| 2004-254 | 254 | RS2 | `dat` + `idx0-4` | ISAAC, on-demand, s2c + c2s sizes |
| 2004-282 | 282 | RS2 | `dat` + `idx0-4` | ISAAC, on-demand, s2c sizes |
| 2005-317 | 317 | RS2 | `dat` + `idx0-4` | ISAAC, on-demand, s2c + c2s sizes |
| 2005-350 | 350 | RS2 | `dat` + `idx0-4` | ISAAC, on-demand, s2c sizes |
| 2006-377 | 377 | RS2 | `dat` + `idx0-4` | ISAAC, on-demand, s2c + c2s sizes |
| 2006-443 | 443 | JS5 | `dat2` + `idx255` | JS5 index format 5, s2c sizes |
| 2007-468 | 468 | JS5 | `dat2` + `idx255` | JS5, XTEA maps, s2c + c2s sizes |
| 2007-474 | 474 | JS5 | `dat2` + `idx255` | JS5, XTEA maps, s2c + c2s sizes |
| 2007-481 | 481 | JS5 | `dat2` + `idx255` | JS5 index format 6, s2c sizes |
| 2008-508 | 508 | JS5 | `dat2` + `idx255` | JS5, XTEA, s2c + c2s sizes |
| 2008-525 | 525 | JS5 | `dat2` + `idx255` | JS5, s2c sizes |
| 2009-530 | 530 | JS5 | `dat2` + `idx255` | JS5, XTEA, s2c + c2s sizes |
| 2009-578 | 578 | JS5 | `dat2` + `idx255` | JS5, no size table in the gamepack |
| 2010-614 | 614 | JS5 | `dat2` + `idx255` | JS5, large file ids, s2c + c2s sizes |
| 2010-634 | 634 | JS5 | `dat2` + `idx255` | JS5, large file ids, no size table |
| 2011-667 | 667 | JS5 | `dat2` + `idx255` | JS5, large file ids, s2c + c2s sizes |
| 2011-695 | 695 | JS5 | no cache in the archive | registered; no size table |
| 2012-742 | 742 | JS5 | `dat2` + `idx255` | JS5 index format 7, named opcodes |
| 2012-745 | 745 | JS5 | no cache in the archive | s2c + c2s sizes |
| 2013-790 | 790 | RS3 / JS5 | no cache in the archive | s2c + c2s sizes |
| 2014-831 | 831 | RS3 / JS5 | no cache in the archive | sources exist in the archive; RS3 disk caches are not archived |
| 2015-860 | 860 | RS3 / JS5 | no cache in the archive | sources exist in the archive; RS3 disk caches are not archived |
| 2016-876 | 876 | RS3 / JS5 | no cache in the archive | s2c + c2s sizes |
| 2017-887 | 887 | RS3 / JS5 | no cache in the archive | s2c + c2s sizes |
| 2018-902 | 902 | RS3 / JS5 | no cache in the archive | s2c + c2s sizes |
| 2019-910 | 910 | RS3 / JS5 | no cache in the archive | named opcodes; login CRC count 41 |

`builds/osrs` is a README slot, not a client snapshot. RS3 rows use JS5 framing and the packet sizes from that client. Their login writer is the lobby delegate (`rs_ops_js5_lobby`): opcode 19 for a lobby login and opcode 16 for a game login, with an explicit RSA length and an XTEA tail keyed by the four ISAAC seeds. 695 store and CRC counts are carried from 667. 831 carries 790. 860 carries 876.

Register anything else at runtime with `rs_build_register` (later revisions, private opcode tables, different ports). A registered build may set `ops` and `codec`. A NULL `ops` selects RSC, RS2, or JS5 from the era. A NULL `codec` selects the same delegate `rs_codec_for` would pick from the revision.

## Select a build

```c
rs_ctx *ctx = rs_ctx_create();
rs_ctx_set_build_id(ctx, "317");   /* also "2005-317" or rs_ctx_set_build_revision(ctx, 317) */

rs_session session;
rs_session_init(&session, rs_ctx_build(ctx));                 /* server role */
rs_session_init_role(&session, rs_ctx_build(ctx), RS_ROLE_CLIENT);
```

To retarget the same program, change that one call. Connections inherit the ctx build.

## Cache

```c
rs_cache *cache = rs_cache_open_archive(ctx, "builds"); /* builds/<version>/<rev>-<date>/cache */
rs_blob data;
rs_cache_get(cache, 0, 1, -1, &data);   /* same call on 115, 317, or 474 */
rs_cache_read_named(cache, "obj.dat", &data);
```

`keys.json` (OpenRS2 XTEA) is loaded automatically from JS5 cache directories.

## Protocol

The same session calls work on every archived build:

- `rs_session_handshake_write` / `parse` / `reply`
- `rs_session_login_write` / `parse` / `success`
- `rs_session_read` / `write` (role-aware game frames)
- `rs_session_update_write` / `parse` / `reply` (on-demand or JS5)

Also:

- Packet buffer with RS encodings (`g1`/`p1` … `gsmart`, alt I/O, bits)
- ISAAC (Jagex stream cipher)
- Size tables: incoming server sizes when the client defines them, and c2s sizes when the client defines that direction (plus the historical 254–667 c2s tables). 282, 350, 443, 481, and 525 are s2c only. 578, 634, and 695 have no table in the gamepack. 831 and 860 have sources in the archive; RS3 disk caches are not archived. Unused constructor-era opcodes are invalid (`-3`). A zero in an old-engine table is an empty packet. A missing table fails the write with `RS_ERR_PROTO`. RSC uses length-prefixed frames.
- Opcode maps: RSC 115/159/177, RS2 254/317/377, JS5 468/474/508/530/742, and RS3 910 (named ClientProt/ServerProt). 614, 667, and the other constructor revisions frame by size table; bind names with `rs_pkt_bind` or `rs_pkt_copy`.
- Typed codecs (`rs_codec_*`) for walk, appearance, skill, varp, interfaces, inventory, region, player/NPC update, zone loc/obj, spotanim, projectile, sound, player index, player option, friend status, energy, and weight, plus interface widgets, inventory update/clear, song/jingle, camera, zone full/partial, sidebar/tab, friends/ignores, hint, instanced region, and the c2s chat/command/private/design/chat-mode codecs. Every call goes through the build's codec delegate. Walk, region, and the player block differ by delegate. The other payloads share the 317 client layout (skill is `g4_alt2`, varp id is `g2_alt1`, interface text is the string then `g2_alt2`, inventory is the amount then `g2_alt3` id) until that era's own function is plugged into the delegate. 317 region is `g2_alt2`+`g2`. Other RS2 rows and revisions without a decoded rebuild (443, 525, 578–745) write two zone u16s. Revisions 468–507 encode rebuild-normal (local tile, plane, zone, LE XTEA, then local/zone z). 508 is `g2_alt2(zone_x)`, `g2_le_add128(local_x)`, `g2_alt2(local_z)`, BE XTEA, `g1_alt2(plane)`, `g2(zone_z)`. 530 is `g2_alt2(local_x)`, alt3 XTEA, `g1_alt3(plane)`, `g2(zone_x)`, `g2_alt2(zone_z)`, `g2_alt2(local_z)`. RS3 (790+) is the 910 rebuild: zone, npc view, mapsquare count, build area, zone z, force-reload, and no XTEA. Player and NPC updates stay on the 317 mask layout. `rs_cache_find_key_mapsquare` looks up `keys.json` by mapsquare.
- JS5 requests from revision 790 are 6 bytes. Opcode 4 stores the XOR key in byte 1. Earlier control packets stay 4 bytes before 508 and 5 bytes from 508 through 745, with the key in the last byte. A group whose index carries a whirlpool digest is checked against the stored container before XTEA. A mismatch is `RS_ERR_FORMAT`.
- WordPack chat (RS2) and Huffman (JS5, from cache bits).
- Config (`obj`/`npc`/`loc`) and map loc readers; world-list encode/decode.

## Packet registry

Logical names stay stable; opcodes are per revision. 254, 317, 377, 468, 474, 508, 530, 742, and 910 ship bound (742 and 910 from named ClientProt/ServerProt). Copy a table onto a nearby revision with `rs_pkt_copy`, or fill gaps with `rs_pkt_bind`. RSA login uses `rs_rsa_set_private_decimal` on the server.

```c
int walk = rs_session_pkt_opcode(&session, RS_C2S_WALK);
rs_session_write_pkt(&session, &out, RS_S2C_MESSAGE, payload, n);
rs_conn_send_pkt(c, RS_S2C_MESSAGE, payload, n);
```

## Server

`rs_server` is an optional poll loop + 600ms tick **your** program drives. It accepts TCP, classifies login vs update vs game, and delivers hooks. Encoding follows the ctx build.

```c
static void on_login(rs_conn *c, const rs_login_request *login) {
    (void)login;
    rs_conn_accept_login(c, 0, 0); /* your auth policy */
}

static void on_update(rs_conn *c, const rs_update_req *req) {
    rs_conn_serve_update(c, req); /* on-demand or JS5, from the attached cache */
}

static void on_packet(rs_conn *c, rs_pkt id, int opcode, rs_buf *payload) {
    (void)c;
    (void)opcode;
    (void)payload;
    if (id == RS_C2S_WALK) {
        /* your world logic */
    }
}

int main(void) {
    rs_ctx *ctx = rs_ctx_create();
    rs_server_hooks hooks = {0};
    rs_server *srv;

    rs_ctx_set_build_id(ctx, "317");
    hooks.on_login = on_login;
    hooks.on_update = on_update;
    hooks.on_packet = on_packet;
    srv = rs_server_create(ctx, &hooks);
    rs_server_open_archive(srv, "builds");
    rs_server_listen(srv, "0.0.0.0", 43594);
    return rs_server_run(srv);
}
```

Change `rs_ctx_set_build_id` to `"474"` (or `"2007-474"`) and the same `main` speaks JS5, different opcodes, and the 474 cache.

`rslib-world` is a tiny example of that loop: login, Lumbridge (zone 402,402 / tile 54,54), a player update, and echoed chat. JS5 474 attaches archive XTEA keys to the rebuild packet.

```bash
cmake --build rslib/build --target rslib-world
./rslib/build/rslib-world --build 317 --archive builds --port 43594
./rslib/build/rslib-world --build 474 --archive builds --port 43594
```

## Client

`rs_client` is the matching outbound reactor. Same ctx build, same packet names:

```c
rs_ctx *ctx = rs_ctx_create();
rs_client_hooks hooks = {0};
rs_client *cli;

rs_ctx_set_build_id(ctx, "317");
cli = rs_client_create(ctx, &hooks);
rs_client_connect(cli, "127.0.0.1", 43594);
rs_client_handshake(cli, RS_LINK_LOGIN, 'p');
rs_client_login(cli, "player1", "pass", 0);
rs_client_send_pkt(cli, RS_C2S_WALK, payload, n);
return rs_client_run(cli);
```

## Build

```bash
cmake -S rslib -B rslib/build
cmake --build rslib/build
ctest --test-dir rslib/build --output-on-failure
# in your emulator: target_link_libraries(myserver PRIVATE rslib)
```

Depends on zlib and libbz2. Jagex cache streams start at the bzip2 pi magic; the library prepends `BZh1` before decompressing.
