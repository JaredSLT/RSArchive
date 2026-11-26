#ifndef RSLIB_NET_H
#define RSLIB_NET_H

#include "rslib/both/build.h"
#include "rslib/both/cache.h"
#include "rslib/both/err.h"
#include "rslib/both/packets.h"
#include "rslib/proto.h"
#include "rslib/server/packets.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Default RS2 cycle. Your program may change it with rs_server_set_tick_ms. */
#define RS_TICK_MS 600

typedef struct rs_server rs_server;
typedef struct rs_conn rs_conn;

typedef struct rs_server_hooks {
    void *user;
    void (*on_accept)(rs_conn *c);
    void (*on_handshake)(rs_conn *c, const rs_handshake *hs);
    void (*on_login)(rs_conn *c, const rs_login_request *login);
    void (*on_packet)(rs_conn *c, rs_pkt id, int opcode, rs_buf *payload);
    /* On-demand and JS5 both arrive here; encoding follows the ctx/session build. */
    void (*on_update)(rs_conn *c, const rs_update_req *req);
    void (*on_tick)(rs_server *srv, uint64_t tick);
    void (*on_close)(rs_conn *c);
} rs_server_hooks;

rs_server *rs_server_create(rs_ctx *ctx, const rs_server_hooks *hooks);
void rs_server_destroy(rs_server *s);

/*
 * Attach caches used for on-demand / JS5 serving.
 * `set_cache` is the default (session build). `add_cache` registers extra
 * revisions so one process can serve every snapshot; handshake revision
 * selects the matching cache.
 * The server does not take ownership; close the caches after destroy.
 */
rs_err rs_server_set_cache(rs_server *s, rs_cache *cache);
rs_err rs_server_add_cache(rs_server *s, rs_cache *cache);
rs_cache *rs_server_cache(rs_server *s, const rs_build_info *build);

/* Optional RSA private key used to unwrap the login block. */
void rs_server_set_rsa(rs_server *s, const rs_rsa_key *key);

/* Private key for one revision. Up to 32. NULL clears that revision.
 * Login uses the key for the session build after retarget, else this default. */
void rs_server_set_rsa_for(rs_server *s, int revision, const rs_rsa_key *key);

/*
 * Classic on this port. Default is off.
 *
 * An RSC client waits for the 4-byte seed and does not send first. An RS2 or
 * JS5 client sends opcode 14 or 15 first. One port can serve both only while
 * classic mode is on and the RS2 client sends that byte before the seed.
 * The seed is not written at accept and the read is not blocked: it is sent
 * from the handshake handler after the connection has waited through one poll
 * timeout with an empty buffer. Opcode 14 or 15 before that is RS2/JS5.
 * With classic off, an RSC ctx build still sends the seed at accept.
 */
void rs_server_set_classic(rs_server *s, int enable);

/* Open `{archive_root}/{ctx-build-id}/cache` and attach it. */
rs_err rs_server_open_archive(rs_server *s, const char *archive_root);
/* Open every `{archive_root}/{id}/cache` that exists for a known build. */
rs_err rs_server_load_archive(rs_server *s, const char *archive_root);

rs_err rs_server_listen(rs_server *s, const char *host, int port);
int rs_server_port(const rs_server *s);
void rs_server_set_tick_ms(rs_server *s, int ms);
int rs_server_tick_ms(const rs_server *s);
uint64_t rs_server_tick(const rs_server *s);
rs_ctx *rs_server_ctx(rs_server *s);

/* One poll + optional tick. timeout_ms < 0 waits forever. */
rs_err rs_server_poll(rs_server *s, int timeout_ms);
/* Loop until rs_server_stop. */
rs_err rs_server_run(rs_server *s);
void rs_server_stop(rs_server *s);

int rs_conn_fd(const rs_conn *c);
rs_session *rs_conn_session(rs_conn *c);
const rs_build_info *rs_conn_build(const rs_conn *c);
rs_server *rs_conn_server(rs_conn *c);
const char *rs_conn_addr(const rs_conn *c);
rs_link rs_conn_link(const rs_conn *c);
const rs_login_request *rs_conn_login(const rs_conn *c);

void rs_conn_set_user(rs_conn *c, void *user);
void *rs_conn_user(const rs_conn *c);

rs_err rs_conn_send(rs_conn *c, const void *data, size_t n);
rs_err rs_conn_accept_login(rs_conn *c, int rights, int flagged);
/* Reply to an update request using the cache for this connection's build. */
rs_err rs_conn_serve_update(rs_conn *c, const rs_update_req *req);
void rs_conn_close(rs_conn *c);

#ifdef __cplusplus
}
#endif

#endif
