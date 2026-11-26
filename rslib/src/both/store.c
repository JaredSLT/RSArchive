#include "internal.h"

#define RS_SECTOR 520
#define RS_MAX_FILE (16 * 1024 * 1024)

typedef struct rs_store {
    FILE *dat;
    FILE *idx;
    int archive_id;
    int large_ids;
} rs_store;

static rs_err store_open(rs_store *s, const char *dat_path, const char *idx_path, int archive_id,
                         int large_ids) {
    memset(s, 0, sizeof(*s));
    s->archive_id = archive_id;
    s->large_ids = large_ids;
    s->dat = fopen(dat_path, "rb+");
    if (!s->dat) {
        s->dat = fopen(dat_path, "rb");
    }
    if (!s->dat) {
        return RS_ERR_IO;
    }
    s->idx = fopen(idx_path, "rb+");
    if (!s->idx) {
        s->idx = fopen(idx_path, "rb");
    }
    if (!s->idx) {
        fclose(s->dat);
        s->dat = NULL;
        return RS_ERR_IO;
    }
    return RS_OK;
}

static void store_close(rs_store *s) {
    if (!s) {
        return;
    }
    if (s->dat) {
        fclose(s->dat);
    }
    if (s->idx) {
        fclose(s->idx);
    }
    memset(s, 0, sizeof(*s));
}

static long file_len(FILE *f) {
    long n;
    if (fseek(f, 0, SEEK_END) != 0) {
        return -1;
    }
    n = ftell(f);
    return n;
}

static rs_err store_read(rs_store *s, int file, rs_blob *out) {
    uint8_t idx[6];
    uint8_t sec[RS_SECTOR];
    int size, sector, got, part, header;
    long dat_len;

    memset(out, 0, sizeof(*out));
    if (!s || !s->idx || !s->dat) {
        return RS_ERR_IO;
    }
    if (fseek(s->idx, (long)file * 6, SEEK_SET) != 0) {
        return RS_ERR_NOT_FOUND;
    }
    if (fread(idx, 1, 6, s->idx) != 6) {
        return RS_ERR_NOT_FOUND;
    }
    size = rs_be24(idx);
    sector = rs_be24(idx + 3);
    if (size <= 0 || size > RS_MAX_FILE || sector <= 0) {
        return RS_ERR_NOT_FOUND;
    }
    dat_len = file_len(s->dat);
    if (dat_len < 0 || sector > dat_len / RS_SECTOR) {
        return RS_ERR_FORMAT;
    }
    out->data = rs_malloc((size_t)size);
    if (!out->data) {
        return RS_ERR_NOMEM;
    }
    out->size = (size_t)size;
    got = 0;
    part = 0;
    while (got < size) {
        int remain, take, fid, fpart, next, arch;
        if (sector == 0) {
            rs_blob_free(out);
            return RS_ERR_FORMAT;
        }
        if (fseek(s->dat, (long)sector * RS_SECTOR, SEEK_SET) != 0) {
            rs_blob_free(out);
            return RS_ERR_IO;
        }
        remain = size - got;
        if (s->large_ids && file > 65535) {
            header = 10;
            if (remain > 510) {
                remain = 510;
            }
        } else {
            header = 8;
            if (remain > 512) {
                remain = 512;
            }
        }
        take = remain + header;
        if (fread(sec, 1, (size_t)take, s->dat) != (size_t)take) {
            rs_blob_free(out);
            return RS_ERR_IO;
        }
        if (header == 10) {
            fid = (int)rs_be32u(sec);
            fpart = rs_be16(sec + 4);
            next = rs_be24(sec + 6);
            arch = sec[9] & 0xff;
        } else {
            fid = rs_be16(sec);
            fpart = rs_be16(sec + 2);
            next = rs_be24(sec + 4);
            arch = sec[7] & 0xff;
        }
        if (fid != file || fpart != part || arch != s->archive_id) {
            rs_blob_free(out);
            return RS_ERR_FORMAT;
        }
        memcpy(out->data + got, sec + header, (size_t)remain);
        got += remain;
        sector = next;
        part++;
    }
    return RS_OK;
}

/* used by cache.c */
rs_err rs_store_open(void **out, const char *dat, const char *idx, int archive_id, int large_ids);
void rs_store_close(void *s);
rs_err rs_store_read(void *s, int file, rs_blob *out);

rs_err rs_store_open(void **out, const char *dat, const char *idx, int archive_id, int large_ids) {
    rs_store *s = rs_calloc(1, sizeof(*s));
    rs_err e;
    if (!s) {
        return RS_ERR_NOMEM;
    }
    e = store_open(s, dat, idx, archive_id, large_ids);
    if (e != RS_OK) {
        free(s);
        return e;
    }
    *out = s;
    return RS_OK;
}

void rs_store_close(void *s) {
    if (!s) {
        return;
    }
    store_close((rs_store *)s);
    free(s);
}

rs_err rs_store_read(void *s, int file, rs_blob *out) {
    return store_read((rs_store *)s, file, out);
}
