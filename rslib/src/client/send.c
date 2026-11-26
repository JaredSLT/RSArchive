#define _POSIX_C_SOURCE 200809L

#include "client/client_impl.h"

rs_err rs_client_flush(rs_client *c) {
    while (c->out.pos > 0) {
        ssize_t n = rs_sock_send((rs_sock)c->fd, c->out.data, c->out.pos);
        if (n < 0) {
            if (rs_sock_errno == RS_SOCK_AGAIN) {
                return RS_OK;
            }
            return RS_ERR_IO;
        }
        if (n == 0) {
            return RS_ERR_IO;
        }
        if ((size_t)n < c->out.pos) {
            memmove(c->out.data, c->out.data + n, c->out.pos - (size_t)n);
        }
        c->out.pos -= (size_t)n;
    }
    return RS_OK;
}

rs_err rs_client_send(rs_client *c, const void *data, size_t n) {
    rs_err e;
    if (!c || c->fd < 0) {
        return RS_ERR_ARG;
    }
    e = rs_pdata(&c->out, data, n);
    if (e != RS_OK) {
        return e;
    }
    return rs_client_flush(c);
}

rs_err rs_client_request_update(rs_client *c, const rs_update_req *req) {
    rs_buf tmp;
    rs_err e;
    if (!c || !req) {
        return RS_ERR_ARG;
    }
    e = rs_buf_alloc(&tmp, 16);
    if (e != RS_OK) {
        return e;
    }
    e = rs_session_update_write(&c->session, &tmp, req);
    if (e == RS_OK) {
        e = rs_client_send(c, tmp.data, tmp.pos);
    }
    rs_buf_free(&tmp);
    return e;
}
