#define _POSIX_C_SOURCE 200809L

#include "server/server_impl.h"

rs_err rs_conn_accept_login(rs_conn *c, int rights, int flagged) {
    uint8_t buf[16];
    rs_buf b;
    rs_err e;
    if (!c || !c->got_login) {
        return RS_ERR_PROTO;
    }
    rs_buf_wrap(&b, buf, sizeof(buf));
    e = rs_session_login_success(&c->session, &b, rights, flagged);
    if (e != RS_OK) {
        return e;
    }
    e = rs_conn_send(c, buf, b.pos);
    if (e != RS_OK) {
        return e;
    }
    rs_session_seed_isaac(&c->session, c->login.isaac_seed);
    c->session.link = RS_LINK_GAME;
    return RS_OK;
}

/* Opcode 19 is the lobby login and stays on the current parser.
 * Opcode 16 and 18: an RS2 listener (u8 length, marker 255, u16 revision)
 * whose byte 2 is not 255 reads a u32 revision at offset 3 and switches to
 * that JS5 build when rs_build_lookup knows it. A JS5 listener whose byte 2
 * is 255 and whose byte 1 is a u8 length of at least 8 reads a u16 revision
 * at offset 3 and switches to that RS2 build. A miss keeps the current build.
 * The ctx revision does not have to match the block. */
static void rs_login_sniff_build(rs_conn *c) {
    const uint8_t *d;
    const rs_build_info *cur;
    const rs_build_info *found;
    size_t n;
    int op;
    if (!c || !c->session.build || c->in.pos < 1) {
        return;
    }
    d = c->in.data;
    n = c->in.pos;
    cur = c->session.build;
    op = d[0] & 0xff;
    if (op != 16 && op != 18) {
        return;
    }
    if (cur->login_size_wide == 0 && cur->era == RS_ERA_RS2 && n >= 7 && d[2] != 255) {
        int rev = (int)rs_be32u(d + 3);
        found = rs_build_lookup(rev);
        if (found && found->revision == rev && found->login_rev_bytes == 4 &&
            (found->era == RS_ERA_JS5 || found->js5 || found->login_size_wide)) {
            rs_session_set_build(&c->session, found);
            c->session.link = RS_LINK_LOGIN;
        }
        return;
    }
    if ((cur->era == RS_ERA_JS5 || cur->js5 || cur->login_size_wide) && n >= 5 && d[2] == 255 &&
        d[1] >= 8) {
        int rev = rs_be16(d + 3);
        found = rs_build_lookup(rev);
        if (found && found->revision == rev && found->era == RS_ERA_RS2 && found->login_size_wide == 0 &&
            found->login_rev_bytes == 2) {
            rs_session_set_build(&c->session, found);
            c->session.link = RS_LINK_LOGIN;
        }
    }
}

static void rs_login_retarget_same(rs_conn *c, int revision) {
    const rs_build_info *found;

    if (!c || !c->session.build || revision <= 0) {
        return;
    }
    found = rs_build_lookup(revision);
    if (found && found != c->session.build && rs_proto_for(found) == rs_proto_for(c->session.build)) {
        rs_session_set_build(&c->session, found);
        c->session.link = RS_LINK_LOGIN;
    }
}

/* Revision sits in the clear before the RSA block. Same-proto retarget
 * still runs after parse; this copy runs first so the RSA key matches. */
static int rs_login_peek_rev(const rs_conn *c) {
    const uint8_t *d;
    const rs_build_info *b;
    size_t n;
    int op;

    if (!c || !c->session.build || c->in.pos < 5) {
        return 0;
    }
    d = c->in.data;
    n = c->in.pos;
    b = c->session.build;
    op = d[0] & 0xff;
    if (op != 16 && op != 18 && !(op == 19 && b->ops == &rs_ops_js5_lobby)) {
        return 0;
    }
    if (!b->login_size_wide) {
        if (d[2] != 255) {
            return 0;
        }
        return rs_be16(d + 3);
    }
    if (n < 7) {
        return 0;
    }
    if (b->login_rev_bytes >= 4) {
        return (int)rs_be32u(d + 3);
    }
    return rs_be16(d + 3);
}

static const rs_rsa_key *rs_login_rsa(const rs_conn *c) {
    const rs_build_info *b;
    int i;

    if (!c || !c->srv) {
        return NULL;
    }
    b = c->session.build;
    if (b) {
        for (i = 0; i < RS_RSA_FOR_MAX; i++) {
            if (c->srv->rsa_for[i].present && c->srv->rsa_for[i].revision == b->revision) {
                return &c->srv->rsa_for[i].key;
            }
        }
    }
    return c->srv->has_rsa ? &c->srv->rsa : NULL;
}

rs_err rs_server_handle_login(rs_conn *c) {
    rs_buf src;
    rs_err e;
    size_t used;

    rs_login_sniff_build(c);
    rs_login_retarget_same(c, rs_login_peek_rev(c));
    rs_buf_wrap(&src, c->in.data, c->in.pos);
    e = rs_session_login_parse(&c->session, &src, rs_login_rsa(c), &c->login);
    if (e != RS_OK) {
        return e;
    }
    used = src.pos;
    memmove(c->in.data, c->in.data + used, c->in.pos - used);
    c->in.pos -= used;
    c->got_login = 1;
    rs_login_retarget_same(c, c->login.revision);
    if (c->srv->hooks.on_login) {
        c->srv->hooks.on_login(c, &c->login);
    }
    return RS_OK;
}
