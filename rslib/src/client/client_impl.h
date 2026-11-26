#ifndef RSLIB_CLIENT_INTERNAL_H
#define RSLIB_CLIENT_INTERNAL_H

#include "internal.h"
#include "both/sock_impl.h"

#define RS_IN_CAP 65536

struct rs_client {
    rs_ctx *ctx;
    rs_client_hooks hooks;
    int fd;
    int running;
    int tick_ms;
    uint64_t tick;
    uint64_t next_tick_ms;
    rs_session session;
    rs_buf in;
    rs_buf out;
    rs_handshake hs;
    int got_hs_reply;
    int got_login_reply;
    int closing;
    rs_rsa_key rsa;
    int has_rsa;
    uint32_t isaac_seed[4];
    uint64_t server_seed;
    int login_status;
    int rights;
    int flagged;
    rs_client_on_group on_group;
    struct rs_cache *cache;
    rs_js5_reply js5;
};

int rs_set_nonblock(int fd);
rs_err rs_client_flush(rs_client *c);
rs_err rs_client_handle_in(rs_client *c);

#endif
