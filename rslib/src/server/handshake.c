#define _POSIX_C_SOURCE 200809L

#include "server/server_impl.h"

/* Classify from the buffered bytes before the session ops run.
 * Opcode 14 has no revision: keep an RS2/JS5 build, otherwise 317.
 * Opcode 15 is a big-endian revision; a JS5 hit replaces 317 or RSC.
 * Opcode 16/18 is a login block. RSC ops never look at it, so leave RSC
 * for 317 and let rs_login_sniff_build retarget from the block. */
static int rs_era_speaks_first(const rs_build_info *b) {
    return b && (b->era == RS_ERA_RS2 || b->era == RS_ERA_JS5 || b->js5);
}

static void rs_handshake_classify(rs_conn *c) {
    const uint8_t *d;
    const rs_build_info *cur;
    const rs_build_info *pick;
    size_t n;
    int op;

    if (!c || !c->in.data || c->in.pos < 1) {
        return;
    }
    d = c->in.data;
    n = c->in.pos;
    cur = c->session.build;
    op = d[0] & 0xff;
    if (op == 14 && n >= 2) {
        pick = rs_era_speaks_first(cur) ? cur : rs_build_lookup(317);
        if (pick) {
            rs_session_set_build(&c->session, pick);
        }
        return;
    }
    if (op == 15 && n >= 5) {
        int rev = (int)rs_be32u(d + 1);
        const rs_build_info *found = rs_build_lookup(rev);
        if (found && (found->js5 || found->era == RS_ERA_JS5)) {
            rs_session_set_build(&c->session, found);
        }
        return;
    }
    if ((op == 16 || op == 18) && cur && cur->era == RS_ERA_RSC) {
        pick = rs_build_lookup(317);
        if (pick) {
            rs_session_set_build(&c->session, pick);
        }
    }
}

/* RSC speaks second. The seed is only sent after one empty poll timeout,
 * and never when opcode 14 or 15 is already buffered. */
static rs_err rs_handshake_classic_seed(rs_conn *c) {
    rs_handshake hs;
    rs_buf reply;
    uint8_t tmp[8];
    rs_err e;
    const rs_build_info *b;

    if (!c->srv || !c->srv->classic || c->empty_polls < 1) {
        return RS_ERR_NEED_MORE;
    }
    b = c->session.build;
    if (!b || b->era != RS_ERA_RSC) {
        b = rs_build_lookup(177);
        if (!b) {
            return RS_ERR_VERSION;
        }
        rs_session_set_build(&c->session, b);
    }
    memset(&hs, 0, sizeof(hs));
    hs.link = RS_LINK_LOGIN;
    hs.revision = b->revision;
    c->server_seed = rs_rand_seed();
    rs_buf_wrap(&reply, tmp, sizeof(tmp));
    e = rs_session_handshake_reply(&c->session, &reply, &hs, c->server_seed);
    if (e != RS_OK) {
        return e;
    }
    if (reply.pos > 0) {
        e = rs_conn_send(c, tmp, reply.pos);
        if (e != RS_OK) {
            return e;
        }
    }
    c->hs = hs;
    c->got_hs = 1;
    c->session.link = RS_LINK_LOGIN;
    return RS_OK;
}

rs_err rs_server_handle_handshake(rs_conn *c) {
    rs_handshake hs;
    rs_err e;
    rs_buf reply;
    uint8_t tmp[32];

    if (!c) {
        return RS_ERR_ARG;
    }
    if (c->in.pos == 0) {
        return rs_handshake_classic_seed(c);
    }
    rs_handshake_classify(c);
    e = rs_session_handshake_parse(&c->session, c->in.data, c->in.pos, &hs);
    if (e != RS_OK) {
        return e;
    }
    if (hs.consumed > c->in.pos) {
        return RS_ERR_PROTO;
    }
    memmove(c->in.data, c->in.data + hs.consumed, c->in.pos - hs.consumed);
    c->in.pos -= hs.consumed;
    c->hs = hs;
    c->got_hs = 1;
    if (hs.link == RS_LINK_JS5 && hs.revision > 0) {
        const rs_build_info *found = rs_build_lookup(hs.revision);
        if (found && (found->js5 || found->era == RS_ERA_JS5)) {
            rs_session_set_build(&c->session, found);
        }
    }
    c->session.link = hs.link;
    if (hs.link == RS_LINK_LOGIN) {
        c->server_seed = rs_rand_seed();
    }
    rs_buf_wrap(&reply, tmp, sizeof(tmp));
    e = rs_session_handshake_reply(&c->session, &reply, &hs, c->server_seed);
    if (e != RS_OK) {
        return e;
    }
    if (reply.pos > 0) {
        e = rs_conn_send(c, tmp, reply.pos);
        if (e != RS_OK) {
            return e;
        }
    }
    if (c->srv->hooks.on_handshake) {
        c->srv->hooks.on_handshake(c, &hs);
    }
    return RS_OK;
}
