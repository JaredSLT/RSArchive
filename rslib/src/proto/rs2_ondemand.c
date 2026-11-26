#include "proto/ops.h"

rs_err rs_ondemand_parse(const uint8_t *data, size_t len, rs_ondemand_req *out) {
    if (!data || !out) {
        return RS_ERR_ARG;
    }
    if (len < 4) {
        return RS_ERR_NEED_MORE;
    }
    out->archive = data[0] & 0xff;
    out->file = (data[1] << 8) | (data[2] & 0xff);
    out->priority = data[3] & 0xff;
    return RS_OK;
}

rs_err rs_ondemand_write_chunk(rs_buf *dst, int archive, int file, const uint8_t *data, size_t size,
                               int chunk) {
    size_t off = (size_t)chunk * 500;
    size_t n;
    rs_err e;
    if (off > size) {
        return RS_ERR_RANGE;
    }
    n = size - off;
    if (n > 500) {
        n = 500;
    }
    e = rs_p1(dst, archive);
    if (e) {
        return e;
    }
    e = rs_p2(dst, file);
    if (e) {
        return e;
    }
    e = rs_p2(dst, (int)size);
    if (e) {
        return e;
    }
    e = rs_p1(dst, chunk);
    if (e) {
        return e;
    }
    return rs_pdata(dst, data + off, n);
}

rs_err rs_ondemand_write_reject(rs_buf *dst, int archive, int file) {
    rs_err e = rs_p1(dst, archive);
    if (e) {
        return e;
    }
    e = rs_p2(dst, file);
    if (e) {
        return e;
    }
    e = rs_p2(dst, 0);
    if (e) {
        return e;
    }
    return rs_p1(dst, 0);
}

rs_err rs2_update_parse(rs_session *s, const uint8_t *data, size_t len, rs_update_req *out,
                               size_t *consumed) {
    rs_ondemand_req od;
    rs_err e;
    (void)s;
    if (!out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = rs_ondemand_parse(data, len, &od);
    if (e != RS_OK) {
        return e;
    }
    out->opcode = od.priority >= 2 ? 1 : 0;
    out->archive = od.archive;
    out->id = od.file;
    out->priority = od.priority;
    if (consumed) {
        *consumed = 4;
    }
    return RS_OK;
}

rs_err rs2_update_write(rs_session *s, rs_buf *dst, const rs_update_req *req) {
    rs_err e;
    (void)s;
    if (!req) {
        return RS_ERR_ARG;
    }
    e = rs_p1(dst, req->archive);
    if (e) {
        return e;
    }
    e = rs_p2(dst, req->id);
    if (e) {
        return e;
    }
    return rs_p1(dst, req->priority);
}

rs_err rs2_update_reply(rs_session *s, rs_buf *dst, const rs_update_req *req,
                               const uint8_t *data, size_t len) {
    int chunks, i;
    rs_err e;
    (void)s;
    if (!dst || !req) {
        return RS_ERR_ARG;
    }
    if (!data || !len) {
        return rs_ondemand_write_reject(dst, req->archive, req->id);
    }
    chunks = (int)((len + 499) / 500);
    if (chunks < 1) {
        chunks = 1;
    }
    for (i = 0; i < chunks; i++) {
        e = rs_ondemand_write_chunk(dst, req->archive, req->id, data, len, i);
        if (e != RS_OK) {
            return e;
        }
    }
    return RS_OK;
}
