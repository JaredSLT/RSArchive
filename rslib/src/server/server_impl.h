#ifndef RSLIB_SERVER_INTERNAL_H
#define RSLIB_SERVER_INTERNAL_H

#include "internal.h"
#include "both/sock_impl.h"

#define RS_IN_CAP 65536
#define RS_MAX_CONNS 1024
#define RS_MAX_CACHES 32
#define RS_RSA_FOR_MAX 32

struct rs_conn {
    rs_server *srv;
    int fd;
    rs_session session;
    rs_buf in;
    rs_buf out;
    rs_handshake hs;
    rs_login_request login;
    uint64_t server_seed;
    int got_hs;
    int got_login;
    int closing;
    /* Set after one poll timeout with an empty read buffer. */
    int empty_polls;
    char addr[64];
    void *user;
};

struct rs_server {
    rs_ctx *ctx;
    rs_server_hooks hooks;
    int listen_fd;
    int port;
    int running;
    int tick_ms;
    uint64_t tick;
    uint64_t next_tick_ms;
    rs_conn **conns;
    int nconns;
    int cap;
    rs_cache *caches[RS_MAX_CACHES];
    int ncaches;
    rs_cache *owned[RS_MAX_CACHES];
    int nowned;
    rs_rsa_key rsa;
    int has_rsa;
    struct {
        int revision;
        int present;
        rs_rsa_key key;
    } rsa_for[RS_RSA_FOR_MAX];
    /* 1 = emit the RSC seed after one empty poll timeout. Default 0. */
    int classic;
};

int rs_set_nonblock(int fd);
uint64_t rs_rand_seed(void);
void rs_conn_free(rs_conn *c);
void rs_conn_drop(rs_server *s, int idx);
rs_err rs_conn_flush(rs_conn *c);
rs_err rs_conn_read(rs_conn *c);
rs_err rs_server_accept(rs_server *s);
rs_err rs_server_handle_handshake(rs_conn *c);
rs_err rs_server_handle_login(rs_conn *c);
rs_err rs_server_handle_game(rs_conn *c);
rs_err rs_server_handle_update(rs_conn *c);

#endif
