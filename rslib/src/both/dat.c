#include "internal.h"

#include "rslib/both/dat.h"

#include <stdint.h>
#include <string.h>

#define TRY(expr)                    \
    do {                             \
        rs_err _rs_try_err = (expr); \
        if (_rs_try_err) {           \
            return _rs_try_err;      \
        }                            \
    } while (0)

#define TRY_F(freefn, obj, expr)     \
    do {                             \
        rs_err _rs_try_err = (expr); \
        if (_rs_try_err) {           \
            freefn(obj);             \
            return _rs_try_err;      \
        }                            \
    } while (0)

static const uint8_t cs742_wide[131] = {
    0x08, 0x02, 0x00, 0x40, 0x01, 0x02, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x08, 0x00, 0x40, 0x04,
    0x00, 0x24, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x00, 0x00, 0x24, 0x00, 0x00, 0x40, 0x00, 0x49, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x80, 0x00, 0x01, 0x00, 0x02, 0x00,
    0x00, 0x84, 0x00, 0x20, 0x00, 0x50, 0x00, 0x21, 0x00, 0x10, 0x00, 0x40, 0x08, 0x80, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x10, 0x80, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08,
    0x00, 0x00, 0x10
};

static const uint8_t cs910_wide[179] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x10, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x20, 0x08, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x0a, 0x80, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x08,
    0x00, 0x08, 0x00
};

/* Index is the cp1252 byte minus 128. A zero entry is decoded as '?'. */
static const int cp1252_high[32] = {
    0x20AC, 0, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
    0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0, 0x017D, 0,
    0, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0, 0x017E, 0x0178
};

static rs_err need(const rs_buf *b, size_t n) {
    if (!b || rs_buf_remaining(b) < n) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err g1(rs_buf *b, int *o) {
    TRY(need(b, 1));
    *o = rs_g1(b);
    return RS_OK;
}

static rs_err g1b(rs_buf *b, int *o) {
    TRY(need(b, 1));
    *o = rs_g1b(b);
    return RS_OK;
}

static rs_err g2(rs_buf *b, int *o) {
    TRY(need(b, 2));
    *o = rs_g2(b);
    return RS_OK;
}

static rs_err g3(rs_buf *b, int *o) {
    TRY(need(b, 3));
    *o = rs_g3(b);
    return RS_OK;
}

static rs_err g4s(rs_buf *b, int *o) {
    TRY(need(b, 4));
    *o = rs_g4s(b);
    return RS_OK;
}

static rs_err g8(rs_buf *b, uint64_t *o) {
    TRY(need(b, 8));
    *o = rs_g8(b);
    return RS_OK;
}

static rs_err skipn(rs_buf *b, size_t n) {
    TRY(need(b, n));
    b->pos += n;
    return RS_OK;
}

static rs_err smart2or4(rs_buf *b, int *o) {
    int v;
    TRY(need(b, 1));
    if ((int8_t)b->data[b->pos] < 0) {
        TRY(g4s(b, &v));
        *o = v & 0x7fffffff;
        return RS_OK;
    }
    TRY(g2(b, &v));
    *o = v == 32767 ? -1 : v;
    return RS_OK;
}

static rs_err smart1or2(rs_buf *b, int *o) {
    int hi;
    TRY(need(b, 1));
    hi = b->data[b->pos] & 0xff;
    if (hi < 128) {
        return g1(b, o);
    }
    TRY(g2(b, o));
    *o -= 0x8000;
    return RS_OK;
}

static rs_err gstr_term(rs_buf *b, int term, char **out) {
    size_t start = b->pos;
    size_t n;
    char *s;
    if (!b->data && b->pos < b->cap) {
        return RS_ERR_FORMAT;
    }
    while (b->pos < b->cap && b->data[b->pos] != (uint8_t)term) {
        b->pos++;
    }
    if (b->pos >= b->cap) {
        return RS_ERR_FORMAT;
    }
    n = b->pos - start;
    b->pos++;
    s = rs_malloc(n + 1);
    if (!s) {
        return RS_ERR_NOMEM;
    }
    if (n) {
        memcpy(s, b->data + start, n);
    }
    s[n] = 0;
    *out = s;
    return RS_OK;
}

static rs_err gstr_nl(rs_buf *b, char **out) {
    return gstr_term(b, 10, out);
}

static rs_err gstr_nul(rs_buf *b, char **out) {
    return gstr_term(b, 0, out);
}

static rs_err fastgstr(rs_buf *b, char **out, int *setflag) {
    TRY(need(b, 1));
    if (b->data[b->pos] == 0) {
        b->pos++;
        *out = NULL;
        if (setflag) {
            *setflag = 0;
        }
        return RS_OK;
    }
    TRY(gstr_nul(b, out));
    if (setflag) {
        *setflag = 1;
    }
    return RS_OK;
}

static rs_err cp1252_char(rs_buf *b, int *cp) {
    int v;
    int mapped;
    TRY(g1(b, &v));
    if (v == 0) {
        return RS_ERR_FORMAT;
    }
    if (v >= 128 && v < 160) {
        mapped = cp1252_high[v - 128];
        *cp = mapped ? mapped : '?';
        return RS_OK;
    }
    *cp = v;
    return RS_OK;
}

static rs_err alloc_ints(int **dst, int n) {
    if (n < 0) {
        return RS_ERR_FORMAT;
    }
    if (n == 0) {
        *dst = NULL;
        return RS_OK;
    }
    *dst = rs_calloc((size_t)n, sizeof(int));
    if (!*dst) {
        return RS_ERR_NOMEM;
    }
    return RS_OK;
}

static int fits(const rs_buf *b, int count, int stride) {
    size_t rem;
    if (count < 0 || stride <= 0) {
        return 0;
    }
    rem = rs_buf_remaining(b);
    return (size_t)count <= rem / (size_t)stride;
}

static rs_err dim_ok(int w, int h, size_t *n) {
    size_t prod;
    if (w < 0 || h < 0) {
        return RS_ERR_FORMAT;
    }
    prod = (size_t)w * (size_t)h;
    if (w && prod / (size_t)w != (size_t)h) {
        return RS_ERR_FORMAT;
    }
    if (prod > (size_t)INT32_MAX) {
        return RS_ERR_FORMAT;
    }
    *n = prod;
    return RS_OK;
}

static void free_params(rs_dat_param *p, int n) {
    int i;
    if (!p) {
        return;
    }
    for (i = 0; i < n; i++) {
        free(p[i].str_value);
    }
    free(p);
}

static rs_err read_params(rs_buf *b, rs_dat_param **dst, int *count) {
    int n;
    int i;
    int is_str;
    int key;
    rs_dat_param *p;
    TRY(g1(b, &n));
    if (!fits(b, n, 4)) {
        return RS_ERR_FORMAT;
    }
    p = n ? rs_calloc((size_t)n, sizeof(*p)) : NULL;
    if (n && !p) {
        return RS_ERR_NOMEM;
    }
    for (i = 0; i < n; i++) {
        TRY(g1(b, &is_str));
        TRY(g3(b, &key));
        p[i].key = key;
        p[i].is_string = is_str == 1;
        if (is_str == 1) {
            if (gstr_nul(b, &p[i].str_value)) {
                free_params(p, i + 1);
                return RS_ERR_FORMAT;
            }
        } else {
            if (g4s(b, &p[i].int_value)) {
                free_params(p, i + 1);
                return RS_ERR_FORMAT;
            }
        }
    }
    free_params(*dst, *count);
    *dst = p;
    *count = n;
    return RS_OK;
}

static rs_err read_bitmask(rs_buf *b, int8_t **dst, int *len) {
    int mask;
    int n;
    int i;
    int next;
    int8_t *bytes;
    TRY(g2(b, &mask));
    n = 0;
    for (i = mask; i > 0; i >>= 1) {
        n++;
    }
    bytes = n ? rs_malloc((size_t)n) : NULL;
    if (n && !bytes) {
        return RS_ERR_NOMEM;
    }
    next = 0;
    for (i = 0; i < n; i++) {
        if ((mask & (1 << i)) > 0) {
            bytes[i] = (int8_t)next;
            next++;
        } else {
            bytes[i] = -1;
        }
    }
    free(*dst);
    *dst = bytes;
    *len = n;
    return RS_OK;
}

static int wide_bit(const uint8_t *bits, int nbits, int op) {
    if (op < 0 || op >= nbits) {
        return 0;
    }
    return (bits[op >> 3] >> (op & 7)) & 1;
}

static int domain_ok(int serial) {
    return serial >= 0 && serial <= 10;
}

void rs_varbit_free(rs_varbit *out) {
    if (!out) {
        return;
    }
    free(out->name);
    memset(out, 0, sizeof(*out));
}

static rs_err varbit_317(rs_buf *b, rs_varbit *o) {
    int op;
    for (;;) {
        TRY_F(rs_varbit_free, o, g1(b, &op));
        if (op == 0) {
            return RS_OK;
        }
        if (op == 1) {
            TRY_F(rs_varbit_free, o, g2(b, &o->basevar));
            TRY_F(rs_varbit_free, o, g1(b, &o->startbit));
            TRY_F(rs_varbit_free, o, g1(b, &o->endbit));
        } else if (op == 10) {
            free(o->name);
            o->name = NULL;
            TRY_F(rs_varbit_free, o, gstr_nl(b, &o->name));
        } else if (op == 2) {
            o->op2 = 1;
        } else if (op == 3) {
            TRY_F(rs_varbit_free, o, g4s(b, &o->op3));
        } else if (op == 4) {
            TRY_F(rs_varbit_free, o, g4s(b, &o->op4));
        } else {
            rs_varbit_free(o);
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err varbit_742(rs_buf *b, rs_varbit *o) {
    int op;
    for (;;) {
        TRY_F(rs_varbit_free, o, g1(b, &op));
        if (op == 0) {
            return RS_OK;
        }
        if (op == 1) {
            TRY_F(rs_varbit_free, o, g2(b, &o->basevar));
            TRY_F(rs_varbit_free, o, g1(b, &o->startbit));
            TRY_F(rs_varbit_free, o, g1(b, &o->endbit));
        } else {
            rs_varbit_free(o);
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err varbit_910(rs_buf *b, rs_varbit *o) {
    int op;
    int index;
    /* serialID -> index. Unknown serials are not in the encoding key. */
    static const int8_t index_of[16] = {
        -1, 14, 7, 3, 8, 11, 1, 5, 13, 2, 4, 12, 6, 9, 10, 0
    };
    for (;;) {
        TRY_F(rs_varbit_free, o, g1(b, &op));
        if (op == 0) {
            return RS_OK;
        }
        if (op < 1 || op > 15) {
            rs_varbit_free(o);
            return RS_ERR_FORMAT;
        }
        index = index_of[op];
        if (index == 7) {
            TRY_F(rs_varbit_free, o, g1(b, &o->startbit));
            TRY_F(rs_varbit_free, o, g1(b, &o->endbit));
        } else if (index == 14) {
            TRY_F(rs_varbit_free, o, g1(b, &o->domain));
            if (!domain_ok(o->domain)) {
                rs_varbit_free(o);
                return RS_ERR_FORMAT;
            }
            o->domain_set = 1;
            TRY_F(rs_varbit_free, o, smart2or4(b, &o->basevar));
        }
    }
}

rs_err rs_varbit_decode(int revision, rs_buf *src, rs_varbit *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    if (revision == 317) {
        out->op3 = -1;
        return varbit_317(src, out);
    }
    if (revision == 742) {
        return varbit_742(src, out);
    }
    if (revision == 910) {
        out->basevar = -1;
        return varbit_910(src, out);
    }
    return RS_ERR_UNSUPPORTED;
}

static void enum_clear_entries(rs_enum *o) {
    int i;
    if (!o->entries) {
        return;
    }
    for (i = 0; i < o->count; i++) {
        free(o->entries[i].str_value);
    }
    free(o->entries);
    o->entries = NULL;
    o->count = 0;
}

void rs_enum_free(rs_enum *out) {
    if (!out) {
        return;
    }
    enum_clear_entries(out);
    free(out->default_str);
    memset(out, 0, sizeof(*out));
}

static rs_err enum_default_null(rs_enum *o) {
    free(o->default_str);
    o->default_str = rs_malloc(5);
    if (!o->default_str) {
        return RS_ERR_NOMEM;
    }
    memcpy(o->default_str, "null", 5);
    return RS_OK;
}

static rs_err enum_entries(rs_buf *b, rs_enum *o, int n, int as_string, int sparse) {
    int i;
    int key;
    rs_enum_entry *e;
    if (n < 0 || !fits(b, n, sparse ? 2 : 4)) {
        return RS_ERR_FORMAT;
    }
    e = n ? rs_calloc((size_t)n, sizeof(*e)) : NULL;
    if (n && !e) {
        return RS_ERR_NOMEM;
    }
    for (i = 0; i < n; i++) {
        if (sparse) {
            if (g2(b, &key)) {
                o->entries = e;
                o->count = i;
                rs_enum_free(o);
                return RS_ERR_FORMAT;
            }
        } else if (g4s(b, &key)) {
            o->entries = e;
            o->count = i;
            rs_enum_free(o);
            return RS_ERR_FORMAT;
        }
        e[i].key = key;
        if (as_string) {
            if (gstr_nul(b, &e[i].str_value)) {
                o->entries = e;
                o->count = i + 1;
                rs_enum_free(o);
                return RS_ERR_FORMAT;
            }
            e[i].is_string = 1;
        } else if (g4s(b, &e[i].int_value)) {
            o->entries = e;
            o->count = i;
            rs_enum_free(o);
            return RS_ERR_FORMAT;
        }
        if (sparse && (key < 0 || key >= o->capacity)) {
            o->entries = e;
            o->count = i + 1;
            rs_enum_free(o);
            return RS_ERR_FORMAT;
        }
    }
    enum_clear_entries(o);
    o->entries = e;
    o->count = n;
    o->sparse = sparse;
    return RS_OK;
}

static rs_err enum_late(rs_buf *b, rs_enum *o, int allow_smart) {
    int op;
    int n;
    int cap;
    TRY_F(rs_enum_free, o, enum_default_null(o));
    for (;;) {
        TRY_F(rs_enum_free, o, g1(b, &op));
        if (op == 0) {
            return RS_OK;
        }
        if (op == 1) {
            TRY_F(rs_enum_free, o, cp1252_char(b, &o->input_char));
            o->input_char_set = 1;
        } else if (op == 2) {
            TRY_F(rs_enum_free, o, cp1252_char(b, &o->output_char));
            o->output_char_set = 1;
        } else if (op == 3) {
            free(o->default_str);
            o->default_str = NULL;
            TRY_F(rs_enum_free, o, gstr_nul(b, &o->default_str));
        } else if (op == 4) {
            TRY_F(rs_enum_free, o, g4s(b, &o->default_int));
        } else if (op == 5 || op == 6) {
            TRY_F(rs_enum_free, o, g2(b, &n));
            o->capacity = 0;
            TRY_F(rs_enum_free, o, enum_entries(b, o, n, op == 5, 0));
        } else if (op == 7 || op == 8) {
            TRY_F(rs_enum_free, o, g2(b, &cap));
            TRY_F(rs_enum_free, o, g2(b, &n));
            o->capacity = cap;
            TRY_F(rs_enum_free, o, enum_entries(b, o, n, op == 7, 1));
        } else if (allow_smart && (op == 101 || op == 102)) {
            int id;
            TRY_F(rs_enum_free, o, smart1or2(b, &id));
            if (op == 101) {
                o->input_type = id;
                o->input_type_set = 1;
            } else {
                o->output_type = id;
                o->output_type_set = 1;
            }
        } else {
            rs_enum_free(o);
            return RS_ERR_FORMAT;
        }
    }
}

rs_err rs_enum_decode(int revision, rs_buf *src, rs_enum *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    if (revision == 742) {
        return enum_late(src, out, 0);
    }
    if (revision == 910) {
        return enum_late(src, out, 1);
    }
    return RS_ERR_UNSUPPORTED;
}

static void seq_free_sounds(rs_seq *o) {
    int i;
    if (o->sounds) {
        for (i = 0; i < o->sound_count; i++) {
            free(o->sounds[i]);
        }
        free(o->sounds);
    }
    free(o->sound_len);
    o->sounds = NULL;
    o->sound_len = NULL;
    o->sound_count = 0;
}

void rs_seq_free(rs_seq *out) {
    if (!out) {
        return;
    }
    free(out->frames);
    free(out->iframes);
    free(out->delays);
    free(out->walkmerge);
    free(out->walk_flags);
    free(out->frames2);
    seq_free_sounds(out);
    free(out->sound_bytes);
    free(out->sound_a);
    free(out->sound_b);
    free_params(out->params, out->param_count);
    memset(out, 0, sizeof(*out));
}

static void seq_defaults(rs_seq *o, int revision) {
    o->replayoff = -1;
    o->priority = 5;
    o->righthand = -1;
    o->lefthand = -1;
    o->replaycount = 99;
    o->preanim_move = -1;
    o->postanim_move = -1;
    o->replacemode = 2;
    o->op22 = -1;
    if (revision == 910) {
        o->tint = -1;
        o->tint2 = -1;
    }
}

static rs_err seq_dummy_317(rs_seq *o) {
    free(o->frames);
    free(o->iframes);
    free(o->delays);
    o->frames = o->iframes = o->delays = NULL;
    o->frame_count = 1;
    TRY(alloc_ints(&o->frames, 1));
    TRY(alloc_ints(&o->iframes, 1));
    TRY(alloc_ints(&o->delays, 1));
    o->frames[0] = -1;
    o->iframes[0] = -1;
    o->delays[0] = -1;
    return RS_OK;
}

static void seq_finish_moves(rs_seq *o, int walk) {
    if (o->preanim_move == -1) {
        o->preanim_move = walk ? 2 : 0;
    }
    if (o->postanim_move == -1) {
        o->postanim_move = walk ? 2 : 0;
    }
}

static rs_err seq_packed(rs_buf *b, int count, int **packed, int **delays) {
    int i;
    int v;
    int *frame;
    int *delay;
    if (!fits(b, count, 6)) {
        return RS_ERR_FORMAT;
    }
    TRY(alloc_ints(&delay, count));
    if (alloc_ints(&frame, count)) {
        free(delay);
        return RS_ERR_NOMEM;
    }
    for (i = 0; i < count; i++) {
        if (g2(b, &delay[i])) {
            free(delay);
            free(frame);
            return RS_ERR_FORMAT;
        }
    }
    for (i = 0; i < count; i++) {
        if (g2(b, &frame[i])) {
            free(delay);
            free(frame);
            return RS_ERR_FORMAT;
        }
    }
    for (i = 0; i < count; i++) {
        if (g2(b, &v)) {
            free(delay);
            free(frame);
            return RS_ERR_FORMAT;
        }
        frame[i] = (int)((uint32_t)frame[i] + ((uint32_t)v << 16));
    }
    free(*packed);
    free(*delays);
    *packed = frame;
    *delays = delay;
    return RS_OK;
}

static rs_err seq_frames2(rs_buf *b, rs_seq *o, int count) {
    int i;
    int v;
    int *frame;
    if (!fits(b, count, 4)) {
        return RS_ERR_FORMAT;
    }
    TRY(alloc_ints(&frame, count));
    for (i = 0; i < count; i++) {
        if (g2(b, &frame[i])) {
            free(frame);
            return RS_ERR_FORMAT;
        }
    }
    for (i = 0; i < count; i++) {
        if (g2(b, &v)) {
            free(frame);
            return RS_ERR_FORMAT;
        }
        frame[i] = (int)((uint32_t)frame[i] + ((uint32_t)v << 16));
    }
    free(o->frames2);
    o->frames2 = frame;
    o->frames2_count = count;
    return RS_OK;
}

static rs_err seq_sounds(rs_buf *b, rs_seq *o) {
    int n;
    int i;
    int inner;
    int j;
    TRY(g2(b, &n));
    if (n < 0 || !fits(b, n, 1)) {
        return RS_ERR_FORMAT;
    }
    seq_free_sounds(o);
    free(o->sound_bytes);
    free(o->sound_a);
    free(o->sound_b);
    o->sound_bytes = o->sound_a = o->sound_b = NULL;
    o->sound_count = n;
    if (n == 0) {
        o->sounds = NULL;
        o->sound_len = NULL;
        return RS_OK;
    }
    o->sounds = rs_calloc((size_t)n, sizeof(int *));
    o->sound_len = rs_calloc((size_t)n, sizeof(int));
    if (!o->sounds || !o->sound_len) {
        seq_free_sounds(o);
        return RS_ERR_NOMEM;
    }
    for (i = 0; i < n; i++) {
        if (g1(b, &inner)) {
            seq_free_sounds(o);
            return RS_ERR_FORMAT;
        }
        if (inner <= 0) {
            continue;
        }
        if (!fits(b, inner, 2)) {
            seq_free_sounds(o);
            return RS_ERR_FORMAT;
        }
        o->sounds[i] = rs_calloc((size_t)inner, sizeof(int));
        if (!o->sounds[i]) {
            seq_free_sounds(o);
            return RS_ERR_NOMEM;
        }
        o->sound_len[i] = inner;
        if (g3(b, &o->sounds[i][0])) {
            seq_free_sounds(o);
            return RS_ERR_FORMAT;
        }
        for (j = 1; j < inner; j++) {
            if (g2(b, &o->sounds[i][j])) {
                seq_free_sounds(o);
                return RS_ERR_FORMAT;
            }
        }
    }
    return RS_OK;
}

static rs_err seq_fill(int **dst, int n, int value) {
    int i;
    if (*dst || n <= 0) {
        return RS_OK;
    }
    TRY(alloc_ints(dst, n));
    for (i = 0; i < n; i++) {
        (*dst)[i] = value;
    }
    return RS_OK;
}

static rs_err seq_317(rs_buf *b, rs_seq *o) {
    int op;
    int n;
    int i;
    for (;;) {
        TRY_F(rs_seq_free, o, g1(b, &op));
        if (op == 0) {
            if (o->frame_count == 0) {
                TRY_F(rs_seq_free, o, seq_dummy_317(o));
            }
            seq_finish_moves(o, o->walkmerge != NULL);
            return RS_OK;
        }
        if (op == 1) {
            TRY_F(rs_seq_free, o, g1(b, &n));
            if (!fits(b, n, 6)) {
                rs_seq_free(o);
                return RS_ERR_FORMAT;
            }
            free(o->frames);
            free(o->iframes);
            free(o->delays);
            o->frames = o->iframes = o->delays = NULL;
            o->frame_count = n;
            TRY_F(rs_seq_free, o, alloc_ints(&o->frames, n));
            TRY_F(rs_seq_free, o, alloc_ints(&o->iframes, n));
            TRY_F(rs_seq_free, o, alloc_ints(&o->delays, n));
            for (i = 0; i < n; i++) {
                TRY_F(rs_seq_free, o, g2(b, &o->frames[i]));
                TRY_F(rs_seq_free, o, g2(b, &o->iframes[i]));
                if (o->iframes[i] == 65535) {
                    o->iframes[i] = -1;
                }
                TRY_F(rs_seq_free, o, g2(b, &o->delays[i]));
            }
        } else if (op == 2) {
            TRY_F(rs_seq_free, o, g2(b, &o->replayoff));
        } else if (op == 3) {
            TRY_F(rs_seq_free, o, g1(b, &n));
            if (n < 0 || !fits(b, n, 1)) {
                rs_seq_free(o);
                return RS_ERR_FORMAT;
            }
            free(o->walkmerge);
            o->walkmerge = NULL;
            o->walkmerge_count = n + 1;
            TRY_F(rs_seq_free, o, alloc_ints(&o->walkmerge, n + 1));
            for (i = 0; i < n; i++) {
                TRY_F(rs_seq_free, o, g1(b, &o->walkmerge[i]));
            }
            o->walkmerge[n] = 9999999;
        } else if (op == 4) {
            o->stretches = 1;
        } else if (op == 5) {
            TRY_F(rs_seq_free, o, g1(b, &o->priority));
        } else if (op == 6) {
            TRY_F(rs_seq_free, o, g2(b, &o->righthand));
        } else if (op == 7) {
            TRY_F(rs_seq_free, o, g2(b, &o->lefthand));
        } else if (op == 8) {
            TRY_F(rs_seq_free, o, g1(b, &o->replaycount));
        } else if (op == 9) {
            TRY_F(rs_seq_free, o, g1(b, &o->preanim_move));
        } else if (op == 10) {
            TRY_F(rs_seq_free, o, g1(b, &o->postanim_move));
        } else if (op == 11) {
            TRY_F(rs_seq_free, o, g1(b, &o->replacemode));
        } else if (op == 12) {
            TRY_F(rs_seq_free, o, g4s(b, &o->op12));
        } else {
            rs_seq_free(o);
            return RS_ERR_FORMAT;
        }
    }
}

/* A missing opcode 13 leaves the sound array null and throws before the index. */
static rs_err seq_apply_mod(rs_buf *b, rs_seq *o, int op) {
    int idx;
    int value;
    int a;
    int c;
    int wide = op == 119 || op == 120;
    int is_byte = op == 19 || op == 119;
    if (o->sound_count < 0) {
        return RS_ERR_FORMAT;
    }
    if (is_byte) {
        TRY(seq_fill(&o->sound_bytes, o->sound_count, 255));
        if (wide) {
            TRY(g2(b, &idx));
        } else {
            TRY(g1(b, &idx));
        }
        TRY(g1(b, &value));
        if (idx < 0 || idx >= o->sound_count) {
            return RS_ERR_FORMAT;
        }
        o->sound_bytes[idx] = value;
        return RS_OK;
    }
    TRY(seq_fill(&o->sound_a, o->sound_count, 256));
    TRY(seq_fill(&o->sound_b, o->sound_count, 256));
    if (wide) {
        TRY(g2(b, &idx));
    } else {
        TRY(g1(b, &idx));
    }
    TRY(g2(b, &a));
    if (idx < 0 || idx >= o->sound_count) {
        return RS_ERR_FORMAT;
    }
    TRY(g2(b, &c));
    o->sound_a[idx] = a;
    o->sound_b[idx] = c;
    return RS_OK;
}

static rs_err seq_late(rs_buf *b, rs_seq *o, int revision) {
    int op;
    int n;
    int i;
    uint32_t sum;
    o->sound_count = -1;
    for (;;) {
        TRY_F(rs_seq_free, o, g1(b, &op));
        if (op == 0) {
            break;
        }
        if (op == 1) {
            TRY_F(rs_seq_free, o, g2(b, &n));
            free(o->frames);
            free(o->delays);
            o->frames = NULL;
            o->delays = NULL;
            TRY_F(rs_seq_free, o, seq_packed(b, n, &o->frames, &o->delays));
            o->frame_count = n;
        } else if (op == 2) {
            TRY_F(rs_seq_free, o, g2(b, &o->replayoff));
        } else if (op == 3 && revision == 742) {
            TRY_F(rs_seq_free, o, g1(b, &n));
            if (n < 0 || !fits(b, n, 1)) {
                rs_seq_free(o);
                return RS_ERR_FORMAT;
            }
            free(o->walk_flags);
            o->walk_flags = rs_calloc(256, 1);
            if (!o->walk_flags) {
                rs_seq_free(o);
                return RS_ERR_NOMEM;
            }
            for (i = 0; i < n; i++) {
                int bit;
                TRY_F(rs_seq_free, o, g1(b, &bit));
                if (bit < 0 || bit > 255) {
                    rs_seq_free(o);
                    return RS_ERR_FORMAT;
                }
                o->walk_flags[bit] = 1;
            }
        } else if (op == 5) {
            TRY_F(rs_seq_free, o, g1(b, &o->priority));
        } else if (op == 6) {
            TRY_F(rs_seq_free, o, g2(b, &o->righthand));
        } else if (op == 7) {
            TRY_F(rs_seq_free, o, g2(b, &o->lefthand));
        } else if (op == 8) {
            TRY_F(rs_seq_free, o, g1(b, &o->replaycount));
        } else if (op == 9) {
            TRY_F(rs_seq_free, o, g1(b, &o->preanim_move));
        } else if (op == 10) {
            TRY_F(rs_seq_free, o, g1(b, &o->postanim_move));
        } else if (op == 11) {
            TRY_F(rs_seq_free, o, g1(b, &o->replacemode));
        } else if (op == 12 || (revision == 910 && op == 112)) {
            if (op == 12) {
                TRY_F(rs_seq_free, o, g1(b, &n));
            } else {
                TRY_F(rs_seq_free, o, g2(b, &n));
            }
            TRY_F(rs_seq_free, o, seq_frames2(b, o, n));
        } else if (op == 13) {
            TRY_F(rs_seq_free, o, seq_sounds(b, o));
        } else if (op == 14) {
            o->flag14 = 1;
        } else if (op == 15) {
            o->flag15 = 1;
        } else if (op == 16 || (revision == 910 && op == 18)) {
            /* Explicit no-op. 742 opcode 18 is a flag; 910 opcode 18 is not. */
        } else if (op == 18 && revision == 742) {
            o->flag18 = 1;
        } else if (op == 19 || op == 20 || (revision == 910 && (op == 119 || op == 120))) {
            TRY_F(rs_seq_free, o, seq_apply_mod(b, o, op));
        } else if (op == 22) {
            TRY_F(rs_seq_free, o, g1(b, &o->op22));
        } else if (revision == 910 && op == 23) {
            int discard;
            TRY_F(rs_seq_free, o, g2(b, &discard));
        } else if (revision == 910 && op == 24) {
            TRY_F(rs_seq_free, o, g2(b, &o->tint2));
        } else if (revision == 910 && op == 25) {
            TRY_F(rs_seq_free, o, g2(b, &o->tint));
        } else if (op == 249) {
            if (read_params(b, &o->params, &o->param_count)) {
                rs_seq_free(o);
                return RS_ERR_FORMAT;
            }
        } else {
            rs_seq_free(o);
            return RS_ERR_FORMAT;
        }
    }
    if (revision == 742) {
        seq_finish_moves(o, o->walk_flags != NULL);
        return RS_OK;
    }
    seq_finish_moves(o, 0);
    if (o->delays) {
        sum = 0;
        for (i = 0; i < o->frame_count; i++) {
            sum += (uint32_t)o->delays[i];
        }
        o->length = (int)sum;
    }
    return RS_OK;
}

rs_err rs_seq_decode(int revision, rs_buf *src, rs_seq *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    if (revision == 317) {
        seq_defaults(out, 317);
        return seq_317(src, out);
    }
    if (revision == 742 || revision == 910) {
        seq_defaults(out, revision);
        return seq_late(src, out, revision);
    }
    return RS_ERR_UNSUPPORTED;
}

void rs_spot_type_free(rs_spot_type *out) {
    if (!out) {
        return;
    }
    free(out->recol_s);
    free(out->recol_d);
    free(out->retex_s);
    free(out->retex_d);
    free(out->recol_idx);
    free(out->retex_idx);
    memset(out, 0, sizeof(*out));
}

static void spot_defaults(rs_spot_type *o) {
    o->anim = -1;
    o->resizeh = 128;
    o->resizev = 128;
    o->hillskew_value = -1;
}

static rs_err spot_pairs(rs_buf *b, int **src, int **dst, int *count) {
    int n;
    int i;
    int *a;
    int *c;
    TRY(g1(b, &n));
    if (n < 0 || !fits(b, n, 4)) {
        return RS_ERR_FORMAT;
    }
    TRY(alloc_ints(&a, n));
    if (alloc_ints(&c, n)) {
        free(a);
        return RS_ERR_NOMEM;
    }
    for (i = 0; i < n; i++) {
        int s;
        int d;
        if (g2(b, &s) || g2(b, &d)) {
            free(a);
            free(c);
            return RS_ERR_FORMAT;
        }
        a[i] = (int16_t)s;
        c[i] = (int16_t)d;
    }
    free(*src);
    free(*dst);
    *src = a;
    *dst = c;
    *count = n;
    return RS_OK;
}

static rs_err spot_317(rs_buf *b, rs_spot_type *o) {
    int op;
    TRY_F(rs_spot_type_free, o, alloc_ints(&o->recol_s, 6));
    TRY_F(rs_spot_type_free, o, alloc_ints(&o->recol_d, 6));
    o->recol_count = 6;
    for (;;) {
        TRY_F(rs_spot_type_free, o, g1(b, &op));
        if (op == 0) {
            return RS_OK;
        }
        if (op == 1) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->model));
        } else if (op == 2) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->anim));
        } else if (op == 4) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->resizeh));
        } else if (op == 5) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->resizev));
        } else if (op == 6) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->angle));
        } else if (op == 7) {
            TRY_F(rs_spot_type_free, o, g1(b, &o->ambient));
        } else if (op == 8) {
            TRY_F(rs_spot_type_free, o, g1(b, &o->contrast));
        } else if (op >= 40 && op < 46) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->recol_s[op - 40]));
        } else if (op >= 50 && op < 56) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->recol_d[op - 50]));
        } else {
            rs_spot_type_free(o);
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err spot_late(rs_buf *b, rs_spot_type *o, int revision) {
    int op;
    for (;;) {
        TRY_F(rs_spot_type_free, o, g1(b, &op));
        if (op == 0) {
            return RS_OK;
        }
        if (op == 1) {
            TRY_F(rs_spot_type_free, o, smart2or4(b, &o->model));
        } else if (op == 2) {
            TRY_F(rs_spot_type_free, o, smart2or4(b, &o->anim));
        } else if (op == 4) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->resizeh));
        } else if (op == 5) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->resizev));
        } else if (op == 6) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->angle));
        } else if (op == 7) {
            TRY_F(rs_spot_type_free, o, g1(b, &o->ambient));
        } else if (op == 8) {
            TRY_F(rs_spot_type_free, o, g1(b, &o->contrast));
        } else if (op == 9) {
            o->hillskew_mode = 3;
            o->hillskew_value = 8224;
        } else if (op == 10) {
            o->tboolean = 1;
        } else if (revision == 742 && op == 11) {
            o->hillskew_mode = 1;
        } else if (revision == 742 && op == 12) {
            o->hillskew_mode = 4;
        } else if (revision == 742 && op == 13) {
            o->hillskew_mode = 5;
        } else if (revision == 742 && op == 14) {
            int v;
            TRY_F(rs_spot_type_free, o, g1(b, &v));
            o->hillskew_mode = 2;
            o->hillskew_value = v * 256;
        } else if (op == 15) {
            TRY_F(rs_spot_type_free, o, g2(b, &o->hillskew_value));
            o->hillskew_mode = 3;
        } else if (op == 16) {
            TRY_F(rs_spot_type_free, o, g4s(b, &o->hillskew_value));
            o->hillskew_mode = 3;
        } else if (op == 40) {
            TRY_F(rs_spot_type_free, o, spot_pairs(b, &o->recol_s, &o->recol_d, &o->recol_count));
        } else if (op == 41) {
            TRY_F(rs_spot_type_free, o, spot_pairs(b, &o->retex_s, &o->retex_d, &o->retex_count));
        } else if (op == 44) {
            TRY_F(rs_spot_type_free, o, read_bitmask(b, &o->recol_idx, &o->recol_idx_len));
        } else if (op == 45) {
            TRY_F(rs_spot_type_free, o, read_bitmask(b, &o->retex_idx, &o->retex_idx_len));
        } else if (revision == 910 && op == 46) {
        } else {
            rs_spot_type_free(o);
            return RS_ERR_FORMAT;
        }
    }
}

rs_err rs_spot_type_decode(int revision, rs_buf *src, rs_spot_type *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    spot_defaults(out);
    if (revision == 317) {
        return spot_317(src, out);
    }
    if (revision == 742 || revision == 910) {
        return spot_late(src, out, revision);
    }
    return RS_ERR_UNSUPPORTED;
}

void rs_identkit_free(rs_identkit *out) {
    if (!out) {
        return;
    }
    free(out->models);
    free(out->recol_s);
    free(out->recol_d);
    free(out->retex_s);
    free(out->retex_d);
    free(out->recol_idx);
    free(out->retex_idx);
    memset(out, 0, sizeof(*out));
}

static void idk_heads(rs_identkit *o) {
    int i;
    for (i = 0; i < 5; i++) {
        o->heads[i] = -1;
    }
}

static rs_err idk_317(rs_buf *b, rs_identkit *o) {
    int op;
    int n;
    int i;
    o->type = -1;
    idk_heads(o);
    TRY_F(rs_identkit_free, o, alloc_ints(&o->recol_s, 6));
    TRY_F(rs_identkit_free, o, alloc_ints(&o->recol_d, 6));
    o->recol_count = 6;
    for (;;) {
        TRY_F(rs_identkit_free, o, g1(b, &op));
        if (op == 0) {
            return RS_OK;
        }
        if (op == 1) {
            TRY_F(rs_identkit_free, o, g1(b, &o->type));
        } else if (op == 2) {
            TRY_F(rs_identkit_free, o, g1(b, &n));
            if (!fits(b, n, 2)) {
                rs_identkit_free(o);
                return RS_ERR_FORMAT;
            }
            free(o->models);
            o->models = NULL;
            o->model_count = n;
            TRY_F(rs_identkit_free, o, alloc_ints(&o->models, n));
            for (i = 0; i < n; i++) {
                TRY_F(rs_identkit_free, o, g2(b, &o->models[i]));
            }
        } else if (op == 3) {
            o->disable = 1;
        } else if (op >= 40 && op < 46) {
            TRY_F(rs_identkit_free, o, g2(b, &o->recol_s[op - 40]));
        } else if (op >= 50 && op < 56) {
            TRY_F(rs_identkit_free, o, g2(b, &o->recol_d[op - 50]));
        } else if (op >= 60 && op < 65) {
            TRY_F(rs_identkit_free, o, g2(b, &o->heads[op - 60]));
        } else {
            rs_identkit_free(o);
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err idk_late(rs_buf *b, rs_identkit *o) {
    int op;
    int n;
    int i;
    int discard;
    idk_heads(o);
    o->type = -1;
    for (;;) {
        TRY_F(rs_identkit_free, o, g1(b, &op));
        if (op == 0) {
            return RS_OK;
        }
        if (op == 1) {
            TRY_F(rs_identkit_free, o, g1(b, &discard));
        } else if (op == 2) {
            TRY_F(rs_identkit_free, o, g1(b, &n));
            if (n < 0 || !fits(b, n, 2)) {
                rs_identkit_free(o);
                return RS_ERR_FORMAT;
            }
            free(o->models);
            o->models = NULL;
            o->model_count = n;
            TRY_F(rs_identkit_free, o, alloc_ints(&o->models, n));
            for (i = 0; i < n; i++) {
                TRY_F(rs_identkit_free, o, smart2or4(b, &o->models[i]));
            }
        } else if (op == 3) {
        } else if (op == 40) {
            TRY_F(rs_identkit_free, o, spot_pairs(b, &o->recol_s, &o->recol_d, &o->recol_count));
        } else if (op == 41) {
            TRY_F(rs_identkit_free, o, spot_pairs(b, &o->retex_s, &o->retex_d, &o->retex_count));
        } else if (op == 44) {
            TRY_F(rs_identkit_free, o, read_bitmask(b, &o->recol_idx, &o->recol_idx_len));
        } else if (op == 45) {
            TRY_F(rs_identkit_free, o, read_bitmask(b, &o->retex_idx, &o->retex_idx_len));
        } else if (op >= 60 && op < 65) {
            TRY_F(rs_identkit_free, o, smart2or4(b, &o->heads[op - 60]));
        } else {
            rs_identkit_free(o);
            return RS_ERR_FORMAT;
        }
    }
}

rs_err rs_identkit_decode(int revision, rs_buf *src, rs_identkit *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    if (revision == 317) {
        return idk_317(src, out);
    }
    if (revision == 742 || revision == 910) {
        return idk_late(src, out);
    }
    return RS_ERR_UNSUPPORTED;
}

static int be_u16(const uint8_t *d) {
    return (d[0] << 8) | d[1];
}

static int be_u24(const uint8_t *d) {
    return (d[0] << 16) | (d[1] << 8) | d[2];
}

void rs_sprite_free(rs_sprite *out) {
    int i;
    if (!out) {
        return;
    }
    if (out->frames) {
        for (i = 0; i < out->count; i++) {
            free(out->frames[i].pixels);
            free(out->frames[i].indices);
            free(out->frames[i].alpha);
        }
        free(out->frames);
    }
    free(out->palette);
    memset(out, 0, sizeof(*out));
}

static rs_err sprite_317(rs_buf *b, rs_sprite *o) {
    int pal_count;
    int i;
    int order;
    int w;
    int h;
    int idx;
    int x;
    int y;
    size_t n;
    rs_sprite_frame *f;
    TRY_F(rs_sprite_free, o, g2(b, &o->sheet_w));
    TRY_F(rs_sprite_free, o, g2(b, &o->sheet_h));
    TRY_F(rs_sprite_free, o, g1(b, &pal_count));
    o->palette_count = pal_count;
    if (pal_count > 0) {
        o->palette = rs_calloc((size_t)pal_count, sizeof(int));
        if (!o->palette) {
            rs_sprite_free(o);
            return RS_ERR_NOMEM;
        }
        for (i = 1; i < pal_count; i++) {
            TRY_F(rs_sprite_free, o, g3(b, &o->palette[i]));
            if (o->palette[i] == 0) {
                o->palette[i] = 1;
            }
        }
    }
    f = rs_calloc(1, sizeof(*f));
    if (!f) {
        rs_sprite_free(o);
        return RS_ERR_NOMEM;
    }
    o->frames = f;
    o->count = 1;
    TRY_F(rs_sprite_free, o, g1(b, &f->offset_x));
    TRY_F(rs_sprite_free, o, g1(b, &f->offset_y));
    TRY_F(rs_sprite_free, o, g2(b, &w));
    TRY_F(rs_sprite_free, o, g2(b, &h));
    TRY_F(rs_sprite_free, o, g1(b, &order));
    if (dim_ok(w, h, &n)) {
        rs_sprite_free(o);
        return RS_ERR_FORMAT;
    }
    f->width = w;
    f->height = h;
    f->sheet_w = o->sheet_w;
    f->sheet_h = o->sheet_h;
    f->flags = order;
    f->pixel_count = (int)n;
    if (n) {
        f->pixels = rs_calloc(n, sizeof(int));
        if (!f->pixels) {
            rs_sprite_free(o);
            return RS_ERR_NOMEM;
        }
    }
    if ((order == 0 || order == 1) && n) {
        if (order == 0) {
            for (i = 0; i < (int)n; i++) {
                TRY_F(rs_sprite_free, o, g1(b, &idx));
                if (idx < 0 || idx >= pal_count) {
                    rs_sprite_free(o);
                    return RS_ERR_FORMAT;
                }
                f->pixels[i] = o->palette[idx];
            }
        } else {
            for (x = 0; x < w; x++) {
                for (y = 0; y < h; y++) {
                    TRY_F(rs_sprite_free, o, g1(b, &idx));
                    if (idx < 0 || idx >= pal_count) {
                        rs_sprite_free(o);
                        return RS_ERR_FORMAT;
                    }
                    f->pixels[x + y * w] = o->palette[idx];
                }
            }
        }
    }
    return RS_OK;
}

static rs_err read_plane(const uint8_t *d, size_t len, size_t *p, int8_t *dst, int w, int h, int column) {
    size_t n = (size_t)w * (size_t)h;
    size_t i;
    int x;
    int y;
    if (*p + n > len) {
        return RS_ERR_FORMAT;
    }
    if (!column) {
        for (i = 0; i < n; i++) {
            dst[i] = (int8_t)d[(*p)++];
        }
        return RS_OK;
    }
    for (x = 0; x < w; x++) {
        for (y = 0; y < h; y++) {
            dst[x + y * w] = (int8_t)d[(*p)++];
        }
    }
    return RS_OK;
}

static int plane_keeps(const int8_t *p, size_t n) {
    size_t i;
    for (i = 0; i < n; i++) {
        if (p[i] != -1) {
            return 1;
        }
    }
    return 0;
}

/* 742 uses trailer 65535 for one raw image. 910 uses bit 15 and the low 15 bits as the frame count. */
static rs_err sprite_bytes(const uint8_t *d, size_t len, rs_sprite *o, int revision) {
    int trailer;
    int full;
    int count;
    int format;
    int alpha;
    int w;
    int h;
    int i;
    size_t n;
    size_t total;
    size_t need;
    size_t p;
    if (!d || len < 2) {
        return RS_ERR_FORMAT;
    }
    trailer = be_u16(d + len - 2);
    if (revision == 742) {
        full = trailer == 65535;
        count = full ? 1 : trailer;
    } else if ((trailer >> 15) != 0) {
        full = 1;
        count = trailer & 32767;
    } else {
        full = 0;
        count = trailer & 32767;
    }
    if (!full) {
        int pal_count;
        size_t dim_at;
        size_t pal_at;
        size_t pal_bytes;
        if (len < 7 || (size_t)count > (len - 7) / 8) {
            return RS_ERR_FORMAT;
        }
        dim_at = len - 7 - (size_t)count * 8;
        o->sheet_w = be_u16(d + dim_at);
        o->sheet_h = be_u16(d + dim_at + 2);
        pal_count = (d[dim_at + 4] & 0xff) + 1;
        pal_bytes = (size_t)(pal_count - 1) * 3;
        if (pal_bytes > dim_at) {
            return RS_ERR_FORMAT;
        }
        pal_at = dim_at - pal_bytes;
        o->palette = rs_calloc((size_t)pal_count, sizeof(int));
        if (!o->palette) {
            return RS_ERR_NOMEM;
        }
        o->palette_count = pal_count;
        for (i = 1; i < pal_count; i++) {
            int color = be_u24(d + pal_at + (size_t)(i - 1) * 3);
            o->palette[i] = color == 0 ? 1 : color;
        }
        if (count) {
            o->frames = rs_calloc((size_t)count, sizeof(*o->frames));
            if (!o->frames) {
                return RS_ERR_NOMEM;
            }
            o->count = count;
            for (i = 0; i < count; i++) {
                o->frames[i].offset_x = be_u16(d + dim_at + 5 + (size_t)i * 2);
            }
            for (i = 0; i < count; i++) {
                o->frames[i].offset_y = be_u16(d + dim_at + 5 + (size_t)count * 2 + (size_t)i * 2);
            }
            for (i = 0; i < count; i++) {
                o->frames[i].width = be_u16(d + dim_at + 5 + (size_t)count * 4 + (size_t)i * 2);
            }
            for (i = 0; i < count; i++) {
                o->frames[i].height = be_u16(d + dim_at + 5 + (size_t)count * 6 + (size_t)i * 2);
            }
            for (i = 0; i < count; i++) {
                rs_sprite_frame *f = &o->frames[i];
                f->sheet_w = o->sheet_w;
                f->sheet_h = o->sheet_h;
                f->crop_x = o->sheet_w - f->width - f->offset_x;
                f->crop_y = o->sheet_h - f->height - f->offset_y;
            }
        }
        p = 0;
        for (i = 0; i < count; i++) {
            rs_sprite_frame *f = &o->frames[i];
            int flag;
            int column;
            if (p >= len) {
                return RS_ERR_FORMAT;
            }
            flag = d[p++] & 0xff;
            f->flags = flag;
            column = flag & 1;
            if (dim_ok(f->width, f->height, &n)) {
                return RS_ERR_FORMAT;
            }
            f->pixel_count = (int)n;
            if (!n) {
                continue;
            }
            f->indices = rs_malloc(n);
            if (!f->indices) {
                return RS_ERR_NOMEM;
            }
            TRY(read_plane(d, len, &p, f->indices, f->width, f->height, column));
            if (flag & 2) {
                f->alpha = rs_malloc(n);
                if (!f->alpha) {
                    return RS_ERR_NOMEM;
                }
                TRY(read_plane(d, len, &p, f->alpha, f->width, f->height, column));
                if (!plane_keeps(f->alpha, n)) {
                    free(f->alpha);
                    f->alpha = NULL;
                }
            }
        }
        return RS_OK;
    }
    if (!d || len < 1) {
        return RS_ERR_FORMAT;
    }
    format = d[0] & 0xff;
    if (format == 1) {
        return RS_ERR_UNSUPPORTED;
    }
    if (format != 0 || len < 6) {
        return RS_ERR_FORMAT;
    }
    alpha = (d[1] & 0xff) == 1;
    w = be_u16(d + 2);
    h = be_u16(d + 4);
    if (dim_ok(w, h, &n)) {
        return RS_ERR_FORMAT;
    }
    if (n && (size_t)count > SIZE_MAX / n) {
        return RS_ERR_FORMAT;
    }
    total = (size_t)count * n;
    if (total > (SIZE_MAX - 6) / 3) {
        return RS_ERR_FORMAT;
    }
    need = 6 + total * 3;
    if (alpha) {
        if (need > SIZE_MAX - total) {
            return RS_ERR_FORMAT;
        }
        need += total;
    }
    if (need > len) {
        return RS_ERR_FORMAT;
    }
    o->full = 1;
    o->sheet_w = w;
    o->sheet_h = h;
    if (count) {
        o->frames = rs_calloc((size_t)count, sizeof(*o->frames));
        if (!o->frames) {
            return RS_ERR_NOMEM;
        }
        o->count = count;
    }
    p = 6;
    for (i = 0; i < count; i++) {
        size_t k;
        rs_sprite_frame *f = &o->frames[i];
        f->width = w;
        f->height = h;
        f->sheet_w = w;
        f->sheet_h = h;
        f->flags = alpha;
        f->pixel_count = (int)n;
        if (!n) {
            continue;
        }
        f->pixels = rs_calloc(n, sizeof(int));
        if (!f->pixels) {
            return RS_ERR_NOMEM;
        }
        for (k = 0; k < n; k++) {
            uint32_t px = (uint32_t)be_u24(d + p) | 0xff000000u;
            p += 3;
            if (revision == 910 && (int32_t)px == (int32_t)0xffff00ffu) {
                px = 0;
            }
            f->pixels[k] = (int)(int32_t)px;
        }
        if (alpha) {
            for (k = 0; k < n; k++) {
                uint32_t a = d[p++] & 0xffu;
                uint32_t px = ((uint32_t)f->pixels[k] & 0xffffffu) | (a << 24);
                f->pixels[k] = (int)(int32_t)px;
            }
        }
    }
    return RS_OK;
}

static rs_err sprite_late(rs_buf *b, rs_sprite *o, int revision) {
    rs_err e;
    size_t len = rs_buf_remaining(b);
    const uint8_t *d = b->data ? b->data + b->pos : NULL;
    e = sprite_bytes(d, len, o, revision);
    if (e) {
        rs_sprite_free(o);
        return e;
    }
    b->pos = b->cap;
    return RS_OK;
}

rs_err rs_sprite_decode(int revision, rs_buf *src, rs_sprite *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    if (revision == 317) {
        return sprite_317(src, out);
    }
    if (revision == 742 || revision == 910) {
        return sprite_late(src, out, revision);
    }
    return RS_ERR_UNSUPPORTED;
}

void rs_font_free(rs_font *out) {
    int i;
    if (!out) {
        return;
    }
    for (i = 0; i < 256; i++) {
        free(out->glyph[i].pixels);
    }
    free(out->kerning);
    memset(out, 0, sizeof(*out));
}

static void font_trim(rs_font_glyph *g) {
    int y;
    int sum;
    int y0;
    int h = g->height;
    int w = g->width;
    g->offset_x = 1;
    g->advance = w + 2;
    y0 = h / 7;
    sum = 0;
    for (y = y0; y < h; y++) {
        sum += g->pixels[y * w];
    }
    if (sum <= h / 7) {
        g->advance--;
        g->offset_x = 0;
    }
    sum = 0;
    for (y = y0; y < h; y++) {
        sum += g->pixels[w - 1 + y * w];
    }
    if (sum <= h / 7) {
        g->advance--;
    }
}

static rs_err font_317(rs_buf *b, rs_font *o) {
    int count;
    int i;
    int x;
    int y;
    int fmt;
    int w;
    int h;
    int v;
    size_t n;
    TRY_F(rs_font_free, o, skipn(b, 4));
    TRY_F(rs_font_free, o, g1(b, &count));
    if (count > 0) {
        TRY_F(rs_font_free, o, skipn(b, (size_t)(count - 1) * 3));
    }
    for (i = 0; i < 256; i++) {
        rs_font_glyph *g = &o->glyph[i];
        TRY_F(rs_font_free, o, g1(b, &v));
        TRY_F(rs_font_free, o, g1(b, &g->offset_y));
        TRY_F(rs_font_free, o, g2(b, &w));
        TRY_F(rs_font_free, o, g2(b, &h));
        TRY_F(rs_font_free, o, g1(b, &fmt));
        if (dim_ok(w, h, &n)) {
            rs_font_free(o);
            return RS_ERR_FORMAT;
        }
        if (w == 0 && h > 0) {
            rs_font_free(o);
            return RS_ERR_FORMAT;
        }
        g->width = w;
        g->height = h;
        if (n) {
            g->pixels = rs_calloc(n, 1);
            if (!g->pixels) {
                rs_font_free(o);
                return RS_ERR_NOMEM;
            }
        }
        if (n && fmt == 0) {
            for (y = 0; y < (int)n; y++) {
                TRY_F(rs_font_free, o, g1b(b, &v));
                g->pixels[y] = (int8_t)v;
            }
        } else if (n && fmt == 1) {
            for (x = 0; x < w; x++) {
                for (y = 0; y < h; y++) {
                    TRY_F(rs_font_free, o, g1b(b, &v));
                    g->pixels[x + y * w] = (int8_t)v;
                }
            }
        }
        if (h > o->height && i < 128) {
            o->height = h;
        }
        font_trim(g);
    }
    o->glyph[32].advance = o->glyph[105].advance;
    o->space_advance = o->glyph[32].advance;
    o->has_glyphs = 1;
    return RS_OK;
}

static void free_runs(int8_t **runs) {
    int i;
    for (i = 0; i < 256; i++) {
        free(runs[i]);
        runs[i] = NULL;
    }
}

static rs_err read_delta(rs_buf *b, int8_t **dst, int n) {
    int i;
    int v;
    int8_t acc = 0;
    if (n < 0) {
        return RS_ERR_FORMAT;
    }
    if (n == 0) {
        *dst = NULL;
        return RS_OK;
    }
    *dst = rs_malloc((size_t)n);
    if (!*dst) {
        return RS_ERR_NOMEM;
    }
    for (i = 0; i < n; i++) {
        if (g1b(b, &v)) {
            free(*dst);
            *dst = NULL;
            return RS_ERR_FORMAT;
        }
        acc = (int8_t)(uint8_t)((int)acc + v);
        (*dst)[i] = acc;
    }
    return RS_OK;
}

static rs_err kern_pair(const int8_t *sec, int sec_n, const int8_t *fst, int fst_n, int off_l, int end_l, int off_r, int end_r, int adv_l, int adv_r, int *out) {
    int lo = off_l > off_r ? off_l : off_r;
    int hi = end_l < end_r ? end_l : end_r;
    int best = adv_l < adv_r ? adv_l : adv_r;
    int i0 = lo - off_l;
    int i1 = lo - off_r;
    int x;
    for (x = lo; x < hi; x++) {
        int s;
        if (i0 < 0 || i1 < 0 || i0 >= sec_n || i1 >= fst_n) {
            return RS_ERR_FORMAT;
        }
        s = (int)sec[i0++] + (int)fst[i1++];
        if (s < best) {
            best = s;
        }
    }
    *out = best;
    return RS_OK;
}

static int8_t idiv_i8(int v, int scale) {
    return (int8_t)(uint8_t)(v / scale);
}

static rs_err font_metrics(rs_buf *b, rs_font *o, int revision) {
    int version;
    int kern;
    int i;
    int j;
    int v;
    int widths[256];
    int offsets[256];
    int8_t *first[256];
    int8_t *second[256];
    memset(first, 0, sizeof(first));
    memset(second, 0, sizeof(second));
    memset(widths, 0, sizeof(widths));
    memset(offsets, 0, sizeof(offsets));
    TRY(g1(b, &version));
    if (version != 0) {
        return RS_ERR_FORMAT;
    }
    TRY(g1(b, &kern));
    kern = kern == 1;
    if (revision == 742) {
        TRY(need(b, 256));
        memcpy(o->advance, b->data + b->pos, 256);
        b->pos += 256;
        if (kern) {
            for (i = 0; i < 256; i++) {
                TRY(g1(b, &widths[i]));
            }
            for (i = 0; i < 256; i++) {
                TRY(g1(b, &offsets[i]));
            }
            for (i = 0; i < 256; i++) {
                rs_err de = read_delta(b, &first[i], widths[i]);
                if (de) {
                    free_runs(first);
                    free_runs(second);
                    return de;
                }
            }
            for (i = 0; i < 256; i++) {
                rs_err de = read_delta(b, &second[i], widths[i]);
                if (de) {
                    free_runs(first);
                    free_runs(second);
                    return de;
                }
            }
            o->kerning = rs_calloc(256 * 256, 1);
            if (!o->kerning) {
                free_runs(first);
                free_runs(second);
                return RS_ERR_NOMEM;
            }
            for (i = 0; i < 256; i++) {
                if (i == 32 || i == 160) {
                    continue;
                }
                for (j = 0; j < 256; j++) {
                    int best;
                    if (j == 32 || j == 160) {
                        continue;
                    }
                    if (kern_pair(second[i], widths[i], first[j], widths[j], offsets[i], widths[i] + offsets[i], offsets[j], widths[j] + offsets[j], o->advance[i], o->advance[j], &best)) {
                        free_runs(first);
                        free_runs(second);
                        return RS_ERR_FORMAT;
                    }
                    o->kerning[i * 256 + j] = (int8_t)(uint8_t)(-best);
                }
            }
            o->space_advance = widths[32] + offsets[32];
            free_runs(first);
            free_runs(second);
        } else {
            TRY(g1(b, &o->space_advance));
        }
        TRY(g1(b, &v));
        TRY(g1(b, &v));
        TRY(g1(b, &o->ascent));
        TRY(g1(b, &o->descent));
        o->has_metrics = 1;
        return RS_OK;
    }
    TRY(need(b, 768));
    memcpy(o->top, b->data + b->pos, 256);
    memcpy(o->bearing, b->data + b->pos + 256, 256);
    memcpy(o->left, b->data + b->pos + 512, 256);
    b->pos += 768;
    TRY(g2(b, &o->pad_a));
    TRY(g2(b, &o->pad_b));
    for (i = 0; i < 256; i++) {
        TRY(g2(b, &v));
        o->box[i][0] = (int16_t)(uint16_t)v;
    }
    for (i = 0; i < 256; i++) {
        TRY(g2(b, &v));
        o->box[i][1] = (int16_t)(uint16_t)v;
    }
    for (i = 0; i < 256; i++) {
        o->box[i][2] = (int16_t)o->top[i];
        o->box[i][3] = (int16_t)o->bearing[i];
    }
    if (kern) {
        for (i = 0; i < 256; i++) {
            if (o->bearing[i] < 0) {
                free_runs(first);
                return RS_ERR_FORMAT;
            }
            {
                rs_err de = read_delta(b, &first[i], o->bearing[i]);
                if (de) {
                    free_runs(first);
                    free_runs(second);
                    return de;
                }
            }
        }
        for (i = 0; i < 256; i++) {
            rs_err de = read_delta(b, &second[i], o->bearing[i]);
            if (de) {
                free_runs(first);
                free_runs(second);
                return de;
            }
        }
        o->kerning = rs_calloc(256 * 256, 1);
        if (!o->kerning) {
            free_runs(first);
            free_runs(second);
            return RS_ERR_NOMEM;
        }
        for (i = 0; i < 256; i++) {
            if (i == 32 || i == 160) {
                continue;
            }
            for (j = 0; j < 256; j++) {
                int best;
                int adv_l = (uint8_t)o->top[i];
                int adv_r = (uint8_t)o->top[j];
                if (j == 32 || j == 160) {
                    continue;
                }
                if (kern_pair(second[i], o->bearing[i], first[j], o->bearing[j], o->left[i], (int)o->bearing[i] + (int)o->left[i], o->left[j], (int)o->bearing[j] + (int)o->left[j], adv_l, adv_r, &best)) {
                    free_runs(first);
                    free_runs(second);
                    return RS_ERR_FORMAT;
                }
                o->kerning[i * 256 + j] = (int8_t)(uint8_t)(-best);
            }
        }
        o->space_advance = (int)o->left[32] + (int)o->bearing[32];
        free_runs(first);
        free_runs(second);
    } else {
        TRY(g1(b, &o->space_advance));
    }
    TRY(g1(b, &o->metric_a));
    TRY(g1(b, &o->metric_b));
    TRY(g1(b, &o->metric_c));
    TRY(g1(b, &o->metric_d));
    TRY(g1(b, &o->scale));
    if (o->scale == 0) {
        return RS_ERR_FORMAT;
    }
    if (o->scale != 1) {
        o->space_advance /= o->scale;
        o->metric_a /= o->scale;
        o->metric_b /= o->scale;
        o->metric_c /= o->scale;
        o->metric_d /= o->scale;
        for (i = 0; i < 256; i++) {
            o->top[i] = idiv_i8(o->top[i], o->scale);
            o->bearing[i] = idiv_i8(o->bearing[i], o->scale);
            o->left[i] = idiv_i8(o->left[i], o->scale);
        }
        if (o->kerning) {
            for (i = 0; i < 256 * 256; i++) {
                o->kerning[i] = idiv_i8(o->kerning[i], o->scale);
            }
        }
    }
    o->has_metrics = 1;
    return RS_OK;
}

static rs_err font_late(rs_buf *b, rs_font *o, int revision) {
    rs_err e = font_metrics(b, o, revision);
    if (e) {
        rs_font_free(o);
        return e;
    }
    return RS_OK;
}

rs_err rs_font_decode(int revision, rs_buf *src, rs_font *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    if (revision == 317) {
        return font_317(src, out);
    }
    if (revision == 742 || revision == 910) {
        return font_late(src, out, revision);
    }
    return RS_ERR_UNSUPPORTED;
}

void rs_clientscript_free(rs_clientscript *out) {
    int i;
    if (!out) {
        return;
    }
    free(out->name);
    if (out->ops) {
        for (i = 0; i < out->op_count; i++) {
            free(out->ops[i].str_operand);
        }
        free(out->ops);
    }
    if (out->switches) {
        for (i = 0; i < out->switch_count; i++) {
            free(out->switches[i].cases);
        }
        free(out->switches);
    }
    memset(out, 0, sizeof(*out));
}

static void script_op_clear(rs_script_op *op) {
    memset(op, 0, sizeof(*op));
    op->opcode = -1;
    op->domain = -1;
    op->var_id = -1;
}

static rs_err script_op(rs_buf *b, rs_script_op *op, int revision) {
    int id;
    TRY(g2(b, &id));
    if (revision == 742) {
        if (id >= 1046) {
            return RS_ERR_FORMAT;
        }
        if (id == 145) {
            op->opcode = 145;
            return gstr_nul(b, &op->str_operand);
        }
        if (id == 63) {
            op->opcode = 63;
            op->long_set = 1;
            return g8(b, &op->long_operand);
        }
        if (wide_bit(cs742_wide, 1046, id)) {
            TRY(g4s(b, &op->int_operand));
        } else {
            TRY(g1(b, &op->int_operand));
        }
        op->int_set = 1;
        op->opcode = id;
        return RS_OK;
    }
    if (id >= 1432) {
        return RS_ERR_FORMAT;
    }
    if (id == 524 || id == 482) {
        TRY(g1(b, &op->domain));
        TRY(g2(b, &op->var_id));
        TRY(g1(b, &op->int_operand));
        op->int_set = 1;
        op->opcode = id;
        return RS_OK;
    }
    if (id == 1376) {
        int serial;
        TRY(g1(b, &serial));
        if (serial == 2) {
            op->opcode = 1376;
            return gstr_nul(b, &op->str_operand);
        }
        if (serial == 0) {
            op->opcode = 204;
            TRY(g4s(b, &op->int_operand));
            op->int_set = 1;
            return RS_OK;
        }
        if (serial == 1) {
            op->opcode = 761;
            op->long_set = 1;
            return g8(b, &op->long_operand);
        }
        return RS_ERR_FORMAT;
    }
    if (id == 1351 || id == 239) {
        TRY(g2(b, &op->var_id));
        TRY(g1(b, &op->int_operand));
        op->int_set = 1;
        op->opcode = id;
        return RS_OK;
    }
    if (wide_bit(cs910_wide, 1432, id)) {
        TRY(g4s(b, &op->int_operand));
    } else {
        TRY(g1(b, &op->int_operand));
    }
    op->int_set = 1;
    op->opcode = id;
    return RS_OK;
}

static rs_err script_body(rs_buf *b, rs_clientscript *o, int revision) {
    size_t base = b->pos;
    size_t len = rs_buf_remaining(b);
    size_t header;
    int trailer;
    int count;
    int switches;
    int i;
    int filled;
    const uint8_t *d;
    if (!b->data || len < 2) {
        return RS_ERR_FORMAT;
    }
    d = b->data + base;
    trailer = be_u16(d + len - 2);
    if ((size_t)trailer > len - 2 || len - 2 - (size_t)trailer < 16) {
        return RS_ERR_FORMAT;
    }
    header = len - 2 - (size_t)trailer - 16;
    b->pos = base + header;
    TRY(g4s(b, &count));
    if (count < 0) {
        return RS_ERR_FORMAT;
    }
    TRY(g2(b, &o->int_local));
    TRY(g2(b, &o->obj_local));
    TRY(g2(b, &o->long_local));
    TRY(g2(b, &o->int_arg));
    TRY(g2(b, &o->obj_arg));
    TRY(g2(b, &o->long_arg));
    TRY(g1(b, &switches));
    if (switches > 0) {
        o->switches = rs_calloc((size_t)switches, sizeof(*o->switches));
        if (!o->switches) {
            return RS_ERR_NOMEM;
        }
        o->switch_count = switches;
        for (i = 0; i < switches; i++) {
            int n;
            int c;
            TRY(g2(b, &n));
            if (n < 0 || !fits(b, n, 8)) {
                return RS_ERR_FORMAT;
            }
            o->switches[i].count = n;
            if (n) {
                o->switches[i].cases = rs_calloc((size_t)n, sizeof(rs_script_case));
                if (!o->switches[i].cases) {
                    return RS_ERR_NOMEM;
                }
            }
            for (c = 0; c < n; c++) {
                TRY(g4s(b, &o->switches[i].cases[c].key));
                TRY(g4s(b, &o->switches[i].cases[c].value));
            }
        }
    }
    b->pos = base;
    TRY(fastgstr(b, &o->name, &o->name_set));
    if (count) {
        o->ops = rs_calloc((size_t)count, sizeof(*o->ops));
        if (!o->ops) {
            return RS_ERR_NOMEM;
        }
        for (i = 0; i < count; i++) {
            script_op_clear(&o->ops[i]);
        }
    }
    o->op_count = count;
    filled = 0;
    while (b->pos < base + header) {
        rs_script_op tmp;
        script_op_clear(&tmp);
        TRY(script_op(b, &tmp, revision));
        if (filled >= count) {
            free(tmp.str_operand);
            return RS_ERR_FORMAT;
        }
        o->ops[filled++] = tmp;
    }
    b->pos = b->cap;
    return RS_OK;
}

static rs_err script_decode(rs_buf *b, rs_clientscript *o, int revision) {
    rs_err e = script_body(b, o, revision);
    if (e) {
        rs_clientscript_free(o);
        return e;
    }
    return RS_OK;
}

rs_err rs_clientscript_decode(int revision, rs_buf *src, rs_clientscript *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    if (revision == 742 || revision == 910) {
        return script_decode(src, out, revision);
    }
    return RS_ERR_UNSUPPORTED;
}
