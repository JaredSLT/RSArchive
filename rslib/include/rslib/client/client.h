#ifndef RSLIB_CLIENT_H
#define RSLIB_CLIENT_H

#include "rslib/both/build.h"
#include "rslib/both/err.h"
#include "rslib/both/packets.h"
#include "rslib/client/packets.h"
#include "rslib/proto.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rs_client rs_client;
struct rs_cache;

typedef void (*rs_client_on_group)(rs_client *c, int archive, int group, const uint8_t *container,
                                   size_t n);

typedef struct rs_client_hooks {
    void *user;
    void (*on_connect)(rs_client *c);
    void (*on_handshake)(rs_client *c, const rs_handshake *hs);
    void (*on_login)(rs_client *c, int status, int rights, int flagged);
    void (*on_packet)(rs_client *c, rs_pkt id, int opcode, rs_buf *payload);
    void (*on_update)(rs_client *c, const uint8_t *data, size_t n);
    void (*on_tick)(rs_client *c, uint64_t tick);
    void (*on_close)(rs_client *c);
} rs_client_hooks;

rs_client *rs_client_create(rs_ctx *ctx, const rs_client_hooks *hooks);
void rs_client_destroy(rs_client *c);

void rs_client_set_rsa(rs_client *c, const rs_rsa_key *key);
void rs_client_set_tick_ms(rs_client *c, int ms);
int rs_client_tick_ms(const rs_client *c);
uint64_t rs_client_tick(const rs_client *c);
rs_ctx *rs_client_ctx(rs_client *c);
rs_session *rs_client_session(rs_client *c);
const rs_build_info *rs_client_build(const rs_client *c);
int rs_client_fd(const rs_client *c);

rs_err rs_client_connect(rs_client *c, const char *host, int port);
rs_err rs_client_handshake(rs_client *c, rs_link link, int name_hash);
rs_err rs_client_login(rs_client *c, const char *username, const char *password, int reconnect);
rs_err rs_client_login_request(rs_client *c, const rs_login_request *req);
void rs_client_set_on_group(rs_client *c, rs_client_on_group fn);
rs_err rs_client_set_cache(rs_client *c, struct rs_cache *cache); /* not owned */
rs_err rs_client_send(rs_client *c, const void *data, size_t n);
rs_err rs_client_request_update(rs_client *c, const rs_update_req *req);

rs_err rs_client_poll(rs_client *c, int timeout_ms);
rs_err rs_client_run(rs_client *c);
void rs_client_stop(rs_client *c);
void rs_client_close(rs_client *c);

#ifdef __cplusplus
}
#endif

#endif
