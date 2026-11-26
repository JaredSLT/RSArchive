#include "internal.h"

#include <bzlib.h>
#include <zlib.h>

rs_err rs_bzip2_jagex(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len) {
    char *in;
    unsigned int dlen;
    int rc;
    if (!src_len) {
        return RS_ERR_FORMAT;
    }
    /* Cache streams start at the pi magic (1AY&SY). libbz2 wants BZh + blocksize. */
    in = rs_malloc(src_len + 4);
    if (!in) {
        return RS_ERR_NOMEM;
    }
    in[0] = 'B';
    in[1] = 'Z';
    in[2] = 'h';
    in[3] = '1';
    memcpy(in + 4, src, src_len);
    dlen = (unsigned int)dst_len;
    rc = BZ2_bzBuffToBuffDecompress((char *)dst, &dlen, in, (unsigned int)(src_len + 4), 0, 0);
    free(in);
    if (rc != BZ_OK) {
        return RS_ERR_FORMAT;
    }
    if ((size_t)dlen != dst_len) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

/* OnDemand.poll reads until the gzip member ends. Idx files keep two trailing bytes. */
rs_err rs_gzip_member(const uint8_t *src, size_t src_len, rs_blob *out) {
    z_stream zs;
    uint8_t *dst;
    size_t cap;
    int rc;
    int done = 0;
    if (!out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    if (!src || src_len < 10 || src_len > 16u * 1024u * 1024u || src[0] != 0x1f || src[1] != 0x8b) {
        return RS_ERR_FORMAT;
    }
    cap = 4096;
    dst = rs_malloc(cap);
    if (!dst) {
        return RS_ERR_NOMEM;
    }
    memset(&zs, 0, sizeof(zs));
    zs.next_in = (Bytef *)src;
    zs.avail_in = (uInt)src_len;
    rc = inflateInit2(&zs, 16 + MAX_WBITS);
    if (rc != Z_OK) {
        free(dst);
        return RS_ERR_FORMAT;
    }
    for (;;) {
        if (zs.total_out > cap) {
            rc = Z_DATA_ERROR;
            break;
        }
        zs.next_out = dst + zs.total_out;
        zs.avail_out = (uInt)(cap - zs.total_out);
        rc = inflate(&zs, Z_NO_FLUSH);
        if (rc == Z_STREAM_END) {
            done = 1;
            break;
        }
        if ((rc == Z_OK || rc == Z_BUF_ERROR) && zs.avail_out == 0) {
            size_t next;
            uint8_t *grown;
            if (cap >= 8u * 1024u * 1024u) {
                rc = Z_DATA_ERROR;
                break;
            }
            next = cap * 2;
            grown = rs_realloc(dst, next);
            if (!grown) {
                rc = Z_MEM_ERROR;
                break;
            }
            dst = grown;
            cap = next;
            continue;
        }
        break;
    }
    inflateEnd(&zs);
    if (!done) {
        free(dst);
        return rc == Z_MEM_ERROR ? RS_ERR_NOMEM : RS_ERR_FORMAT;
    }
    out->data = dst;
    out->size = zs.total_out;
    return RS_OK;
}

rs_err rs_gzip_inflate(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len) {
    z_stream zs;
    int rc;
    memset(&zs, 0, sizeof(zs));
    zs.next_in = (Bytef *)src;
    zs.avail_in = (uInt)src_len;
    zs.next_out = dst;
    zs.avail_out = (uInt)dst_len;
    rc = inflateInit2(&zs, 16 + MAX_WBITS);
    if (rc != Z_OK) {
        return RS_ERR_FORMAT;
    }
    rc = inflate(&zs, Z_FINISH);
    inflateEnd(&zs);
    if (rc != Z_STREAM_END) {
        return RS_ERR_FORMAT;
    }
    if (zs.total_out != dst_len) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}
