#define _POSIX_C_SOURCE 200809L

#include "server/server_impl.h"

static rs_cache *cache_for_conn(rs_conn *c) {
    return rs_server_cache(c->srv, c->session.build);
}

static rs_err send_buf(rs_conn *c, rs_buf *b) {
    return rs_conn_send(c, b->data, b->pos);
}

static rs_err serve_ondemand(rs_conn *c, const rs_ondemand_req *req) {
    rs_cache *cache;
    rs_blob raw;
    rs_buf out;
    rs_err e;
    int archive, chunks, i;
    if (!c || !req) {
        return RS_ERR_ARG;
    }
    cache = cache_for_conn(c);
    if (!cache) {
        return RS_OK;
    }
    archive = req->archive;
    if (c->session.build && c->session.build->cache_kind == RS_CACHE_RS2) {
        archive = req->archive + 1;
    }
    e = rs_cache_read(cache, archive, req->file, &raw);
    if (e != RS_OK) {
        if (rs_buf_alloc(&out, 16) != RS_OK) {
            return RS_ERR_NOMEM;
        }
        rs_ondemand_write_reject(&out, req->archive, req->file);
        e = send_buf(c, &out);
        rs_buf_free(&out);
        return e;
    }
    chunks = (int)((raw.size + 499) / 500);
    if (chunks < 1) {
        chunks = 1;
    }
    if (rs_buf_alloc(&out, 520) != RS_OK) {
        rs_blob_free(&raw);
        return RS_ERR_NOMEM;
    }
    for (i = 0; i < chunks; i++) {
        out.pos = 0;
        e = rs_ondemand_write_chunk(&out, req->archive, req->file, raw.data, raw.size, i);
        if (e == RS_OK) {
            e = send_buf(c, &out);
        }
        if (e != RS_OK) {
            break;
        }
    }
    rs_buf_free(&out);
    rs_blob_free(&raw);
    return e;
}

static rs_err js5_checksums(rs_cache *cache, const rs_build_info *build, rs_buf *dst) {
    rs_buf inner;
    rs_err e;
    int i, n;
    uint8_t hdr[5];
    if (rs_buf_alloc(&inner, 256) != RS_OK) {
        return RS_ERR_NOMEM;
    }
    n = rs_cache_archive_count(cache);
    for (i = 0; i < n; i++) {
        rs_blob idx;
        uint32_t crc = 0;
        if (rs_cache_read(cache, 255, i, &idx) == RS_OK) {
            crc = rs_crc32(idx.data, idx.size);
            rs_blob_free(&idx);
        }
        e = rs_p4(&inner, (int)crc);
        if (e != RS_OK) {
            rs_buf_free(&inner);
            return e;
        }
        if (build && build->js5_index_format >= 6) {
            int ver = 0;
            rs_js5_index idx;
            if (rs_cache_index(cache, i, &idx) == RS_OK) {
                ver = idx.version;
                rs_js5_index_free(&idx);
            }
            e = rs_p4(&inner, ver);
            if (e != RS_OK) {
                rs_buf_free(&inner);
                return e;
            }
        }
    }
    hdr[0] = 0;
    rs_put_be32(hdr + 1, (uint32_t)inner.pos);
    e = rs_pdata(dst, hdr, 5);
    if (e == RS_OK) {
        e = rs_pdata(dst, inner.data, inner.pos);
    }
    rs_buf_free(&inner);
    (void)build;
    return e;
}

static rs_err serve_js5(rs_conn *c, const rs_js5_req *req) {
    rs_cache *cache;
    rs_blob raw;
    rs_buf out;
    rs_err e;
    if (!c || !req) {
        return RS_ERR_ARG;
    }
    cache = cache_for_conn(c);
    if (!cache) {
        return RS_OK;
    }
    if (req->opcode != RS_JS5_PREFETCH && req->opcode != RS_JS5_URGENT) {
        return RS_OK;
    }
    if (rs_buf_alloc(&out, 1024) != RS_OK) {
        return RS_ERR_NOMEM;
    }
    if (req->archive == 255 && req->group == 255) {
        rs_buf container;
        if (rs_cache_read(cache, 255, 255, &raw) == RS_OK) {
            e = rs_js5_write_group(c->session.build, &out, 255, 255, raw.data, raw.size, req->opcode == RS_JS5_URGENT);
            rs_blob_free(&raw);
        } else {
            if (rs_buf_alloc(&container, 256) != RS_OK) {
                rs_buf_free(&out);
                return RS_ERR_NOMEM;
            }
            e = js5_checksums(cache, c->session.build, &container);
            if (e == RS_OK) {
                e = rs_js5_write_group(c->session.build, &out, 255, 255, container.data, container.pos,
                                       req->opcode == RS_JS5_URGENT);
            }
            rs_buf_free(&container);
        }
        if (e == RS_OK) {
            e = send_buf(c, &out);
        }
        rs_buf_free(&out);
        return e;
    }
    e = rs_cache_read(cache, req->archive, req->group, &raw);
    if (e != RS_OK) {
        rs_buf_free(&out);
        return RS_ERR_NOT_FOUND;
    }
    e = rs_js5_write_group(c->session.build, &out, req->archive, req->group, raw.data, raw.size,
                           req->opcode == RS_JS5_URGENT);
    rs_blob_free(&raw);
    if (e == RS_OK) {
        e = send_buf(c, &out);
    }
    rs_buf_free(&out);
    return e;
}

rs_err rs_server_handle_update(rs_conn *c) {
    for (;;) {
        rs_update_req req;
        size_t consumed = 0;
        rs_err e = rs_session_update_parse(&c->session, c->in.data, c->in.pos, &req, &consumed);
        if (e == RS_ERR_NEED_MORE) {
            return RS_OK;
        }
        if (e != RS_OK) {
            return e;
        }
        if (!consumed || consumed > c->in.pos) {
            return RS_ERR_PROTO;
        }
        memmove(c->in.data, c->in.data + consumed, c->in.pos - consumed);
        c->in.pos -= consumed;
        if (c->srv->hooks.on_update) {
            c->srv->hooks.on_update(c, &req);
        }
    }
}

rs_err rs_conn_serve_update(rs_conn *c, const rs_update_req *req) {
    if (!c || !req) {
        return RS_ERR_ARG;
    }
    if (c->session.build && c->session.build->js5) {
        rs_js5_req js5;
        js5.opcode = req->opcode;
        js5.archive = req->archive;
        js5.group = req->id;
        return serve_js5(c, &js5);
    }
    {
        rs_ondemand_req od;
        od.archive = req->archive;
        od.file = req->id;
        od.priority = req->priority;
        return serve_ondemand(c, &od);
    }
}
