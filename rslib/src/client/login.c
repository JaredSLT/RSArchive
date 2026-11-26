#define _POSIX_C_SOURCE 200809L

#include "client/client_impl.h"

static void login_fill_crcs(rs_client *c, rs_login_request *req) {
    const rs_build_info *b;
    int i, n;
    if (!c->cache || req->ncrc != 0) {
        return;
    }
    b = c->session.build;
    if (!b || b->login_crc_count <= 0) {
        return;
    }
    n = rs_login_crc_count(b);
    if (n > RS_LOGIN_CRC_MAX) {
        n = RS_LOGIN_CRC_MAX;
    }
    for (i = 0; i < n; i++) {
        rs_blob idx;
        if (rs_cache_read(c->cache, 255, i, &idx) == RS_OK) {
            req->archive_crc[i] = rs_crc32(idx.data, idx.size);
            rs_blob_free(&idx);
        }
    }
    req->ncrc = n;
}

rs_err rs_client_login_request(rs_client *c, const rs_login_request *req) {
    rs_login_request copy;
    rs_buf tmp;
    rs_err e;
    int i;
    if (!c || !req) {
        return RS_ERR_ARG;
    }
    copy = *req;
    if (copy.revision == 0 && c->session.build) {
        copy.revision = c->session.build->revision;
    }
    for (i = 0; i < 4; i++) {
        c->isaac_seed[i] = copy.isaac_seed[i];
    }
    login_fill_crcs(c, &copy);
    if (copy.ncrc == 0 && c->session.build && c->session.build->login_crc_count > 0) {
        copy.ncrc = rs_login_crc_count(c->session.build);
    }
    e = rs_buf_alloc(&tmp, 1024);
    if (e != RS_OK) {
        return e;
    }
    e = rs_session_login_write(&c->session, &tmp, &copy, c->has_rsa ? &c->rsa : NULL);
    if (e == RS_OK) {
        e = rs_client_send(c, tmp.data, tmp.pos);
    }
    rs_buf_free(&tmp);
    c->got_login_reply = 0;
    return e;
}

static int login_all_digits(const char *s) {
    if (!s || !s[0]) {
        return 0;
    }
    for (; *s; s++) {
        if (*s < '0' || *s > '9') {
            return 0;
        }
    }
    return 1;
}

static int login_is_rs3(const rs_build_info *b) {
    if (!b) {
        return 0;
    }
    if (b->ops == &rs_ops_js5_lobby) {
        return 1;
    }
    return b->version && strcmp(b->version, "rs3") == 0;
}

rs_err rs_client_login(rs_client *c, const char *username, const char *password, int reconnect) {
    rs_login_request req;
    const rs_build_info *b;
    int i;
    if (!c || !username) {
        return RS_ERR_ARG;
    }
    memset(&req, 0, sizeof(req));
    b = c->session.build;
    req.reconnect = reconnect;
    req.revision = b ? b->revision : 0;
    req.lobby = login_is_rs3(b) ? 1 : 0;
    for (i = 0; i < 4; i++) {
        req.isaac_seed[i] = (uint32_t)(rs_now_ms() * 1103515245u + (uint32_t)(i + 1) * 12345u);
    }
    strncpy(req.username, username, sizeof(req.username) - 1);
    if (password) {
        strncpy(req.password, password, sizeof(req.password) - 1);
    }
    if (b && b->login_user37) {
        if (login_all_digits(req.username)) {
            req.username37 = strtoull(req.username, NULL, 10);
        } else {
            req.username37 = rs_base37_encode(req.username);
        }
    }
    /* 910 lobby_tail_write already treats a NULL ext as a zero tail.
     * Leave ext unset so no hardware bytes are invented. */
    return rs_client_login_request(c, &req);
}
