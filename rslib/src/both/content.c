#include "rslib/both/content.h"

#include <math.h>
#include <string.h>

#define TRY(expr)                         \
    do {                                  \
        rs_err _rs_try_err = (expr);      \
        if (_rs_try_err) {                \
            return _rs_try_err;           \
        }                                 \
    } while (0)

enum { HIDE_NONE = 0, HIDE_CI = 1, HIDE_EXACT = 2 };

static int rev_ok(int revision) {
    return revision == 317 || revision == 474 || revision == 530 || revision == 742 || revision == 910;
}

static rs_err need(const rs_buf *b, size_t n) {
    if (rs_buf_remaining(b) < n) {
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

static rs_err g2b(rs_buf *b, int *o) {
    TRY(need(b, 2));
    *o = rs_g2b(b);
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

static int m1_u16(int v) {
    return v == 65535 ? -1 : v;
}

static rs_err gstr(rs_buf *b, char *dst, size_t cap, int *setflag) {
    rs_err err = rs_gjstr(b, dst, cap);
    if (err) {
        return err;
    }
    if (setflag) {
        *setflag = 1;
    }
    return RS_OK;
}

static int eq_ci(const char *a, const char *b) {
    while (*a && *b) {
        unsigned char ca = (unsigned char)*a;
        unsigned char cb = (unsigned char)*b;
        if (ca >= 'A' && ca <= 'Z') {
            ca = (unsigned char)(ca - 'A' + 'a');
        }
        if (cb >= 'A' && cb <= 'Z') {
            cb = (unsigned char)(cb - 'A' + 'a');
        }
        if (ca != cb) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == 0 && *b == 0;
}

static void hide_slot(char *slot, int *set, int hide) {
    int drop = 0;
    if (hide == HIDE_CI) {
        drop = eq_ci(slot, "hidden");
    } else if (hide == HIDE_EXACT) {
        drop = strcmp(slot, "Hidden") == 0;
    }
    if (drop) {
        slot[0] = 0;
        *set = 0;
    }
}

static rs_err read_op(rs_buf *b, char *slot, int *set, int hide) {
    TRY(gstr(b, slot, RS_CONTENT_STR, set));
    hide_slot(slot, set, hide);
    return RS_OK;
}

static rs_err discard_str(rs_buf *b) {
    char tmp[RS_CONTENT_STR];
    return rs_gjstr(b, tmp, sizeof(tmp));
}

static void copy_null(char *dst, int *set) {
    memcpy(dst, "null", 5);
    *set = 1;
}

static rs_err smarts(rs_buf *b, int *o) {
    int hi;
    TRY(need(b, 1));
    hi = b->data[b->pos] & 0xff;
    if (hi < 128) {
        *o = hi;
        b->pos++;
        return RS_OK;
    }
    TRY(need(b, 2));
    *o = rs_g2(b) - 32768;
    return RS_OK;
}

/* nullmap: short form 32767 becomes -1 (gSmart2or4null / gSmart2or4s). */
static rs_err smart2or4(rs_buf *b, int *o, int nullmap) {
    TRY(need(b, 1));
    if ((int8_t)b->data[b->pos] < 0) {
        int v;
        TRY(g4s(b, &v));
        *o = v & 0x7fffffff;
        return RS_OK;
    }
    TRY(g2(b, o));
    if (nullmap && *o == 32767) {
        *o = -1;
    }
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

static rs_err smart1or2s(rs_buf *b, int *o) {
    int hi;
    TRY(need(b, 1));
    hi = b->data[b->pos] & 0xff;
    if (hi < 128) {
        TRY(g1(b, o));
        *o -= 64;
        return RS_OK;
    }
    TRY(g2(b, o));
    *o -= 0xc000;
    return RS_OK;
}

static rs_err smart1or2null(rs_buf *b, int *o) {
    int hi;
    TRY(need(b, 1));
    hi = b->data[b->pos] & 0xff;
    if (hi < 128) {
        TRY(g1(b, o));
        *o -= 1;
        return RS_OK;
    }
    TRY(g2(b, o));
    *o -= 0x8001;
    return RS_OK;
}

static rs_err read_recol(rs_buf *b, int *count, int *src, int *dst) {
    int n;
    int i;
    TRY(g1(b, &n));
    if (n > RS_CONTENT_RECOL) {
        return RS_ERR_RANGE;
    }
    *count = n;
    for (i = 0; i < n; i++) {
        TRY(g2(b, &src[i]));
        TRY(g2(b, &dst[i]));
    }
    return RS_OK;
}

static rs_err read_palette(rs_buf *b, int *count, int *dst) {
    int n;
    int i;
    TRY(g1(b, &n));
    if (n > RS_CONTENT_RECOL) {
        return RS_ERR_RANGE;
    }
    *count = n;
    for (i = 0; i < n; i++) {
        TRY(g1b(b, &dst[i]));
    }
    return RS_OK;
}

static rs_err read_index(rs_buf *b, int *count, int *dst) {
    int mask;
    int n = 0;
    int i;
    int next = 0;
    TRY(g2(b, &mask));
    for (i = mask; i > 0; i >>= 1) {
        n++;
    }
    if (n > RS_CONTENT_INDEX) {
        return RS_ERR_RANGE;
    }
    *count = n;
    for (i = 0; i < n; i++) {
        if ((mask & (1 << i)) > 0) {
            dst[i] = next++;
        } else {
            dst[i] = -1;
        }
    }
    return RS_OK;
}

static rs_err read_params(rs_buf *b, int *count, rs_param *params) {
    int n;
    int i;
    TRY(g1(b, &n));
    if (n > RS_CONTENT_PARAM) {
        return RS_ERR_RANGE;
    }
    *count = n;
    for (i = 0; i < n; i++) {
        int flag;
        TRY(g1(b, &flag));
        params[i].is_string = flag == 1;
        TRY(g3(b, &params[i].key));
        if (params[i].is_string) {
            TRY(gstr(b, params[i].str_value, RS_CONTENT_STR, NULL));
        } else {
            TRY(g4s(b, &params[i].int_value));
        }
    }
    return RS_OK;
}

static rs_err read_models(rs_buf *b, int *count, int *dst, int cap, int mode) {
    int n;
    int i;
    TRY(g1(b, &n));
    if (n > cap) {
        return RS_ERR_RANGE;
    }
    *count = n;
    for (i = 0; i < n; i++) {
        if (mode == 2) {
            TRY(smart2or4(b, &dst[i], 1));
        } else {
            TRY(g2(b, &dst[i]));
            if (mode == 1 && dst[i] == 65535) {
                dst[i] = -1;
            }
        }
    }
    return RS_OK;
}

static rs_err read_quests(rs_buf *b, int *count, int *dst) {
    int n;
    int i;
    TRY(g1(b, &n));
    if (n > RS_CONTENT_QUEST) {
        return RS_ERR_RANGE;
    }
    *count = n;
    for (i = 0; i < n; i++) {
        TRY(g2(b, &dst[i]));
    }
    return RS_OK;
}

static rs_err skip_n(rs_buf *b, size_t n) {
    TRY(need(b, n));
    b->pos += n;
    return RS_OK;
}

static rs_err skip_g1g2(rs_buf *b) {
    return skip_n(b, 3);
}

static rs_err skip_counted_pairs(rs_buf *b) {
    int n;
    TRY(g1(b, &n));
    return skip_n(b, (size_t)n * 4);
}

static void cursor_init(int *set, int *dst, int n) {
    int i;
    if (*set) {
        return;
    }
    *set = 1;
    for (i = 0; i < n; i++) {
        dst[i] = -1;
    }
}

static void wear_m1(rs_obj_type *o) {
    o->manwear = o->manwear2 = o->manwear3 = -1;
    o->womanwear = o->womanwear2 = o->womanwear3 = -1;
    o->manhead = o->manhead2 = o->womanhead = o->womanhead2 = -1;
}

static void obj_reset(rs_obj_type *o, int rev) {
    memset(o, 0, sizeof(*o));
    if (rev == 530) {
        /* Uninitialised Java ints stay 0: zoom, model, most wear, resize, certtemplate. */
        o->cost = 1;
        o->certlink = -1;
        o->womanwear2 = -1;
        o->op127_a = -1;
        return;
    }
    o->zoom2d = 2000;
    o->cost = 1;
    o->resizex = o->resizey = o->resizez = 128;
    wear_m1(o);
    o->certlink = -1;
    o->certtemplate = -1;
    o->dummy10 = -1;
    if (rev == 317 || rev == 474) {
        return;
    }
    copy_null(o->name, &o->name_set);
    o->wearpos = o->wearpos2 = o->wearpos3 = -1;
    o->lentlink = o->lenttemplate = -1;
    o->boughtlink = o->boughttemplate = -1;
    o->aux18 = -1;
    if (rev == 910) {
        o->tradeable = 1;
        o->placeholder = 1;
        o->category = -1;
        o->shardlink = o->shardtemplate = -1;
        copy_null(o->shardname, &o->shardname_set);
    }
}

static rs_err obj_count(rs_buf *b, rs_obj_type *o, int index) {
    TRY(g2(b, &o->countobj[index]));
    TRY(g2(b, &o->countco[index]));
    return RS_OK;
}

static int shl(int v, int shift) {
    if (shift <= 0) {
        return v;
    }
    return (int)((uint32_t)v << (unsigned)shift);
}

static rs_err obj_offsets(rs_buf *b, int *x, int *y, int *z, int shift) {
    int a;
    int c;
    int d;
    TRY(g1b(b, &a));
    TRY(g1b(b, &c));
    TRY(g1b(b, &d));
    *x = shl(a, shift);
    *y = shl(c, shift);
    *z = shl(d, shift);
    return RS_OK;
}

static rs_err obj_decode_317(rs_buf *b, rs_obj_type *o) {
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            return RS_OK;
        } else if (op == 1) {
            TRY(g2(b, &o->model));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 3) {
            TRY(gstr(b, o->desc, RS_CONTENT_DESC, &o->desc_set));
        } else if (op == 4) {
            TRY(g2(b, &o->zoom2d));
        } else if (op == 5) {
            TRY(g2(b, &o->xan2d));
        } else if (op == 6) {
            TRY(g2(b, &o->yan2d));
        } else if (op == 7) {
            TRY(g2b(b, &o->xof2d));
        } else if (op == 8) {
            TRY(g2b(b, &o->yof2d));
        } else if (op == 10) {
            TRY(g2(b, &o->dummy10));
        } else if (op == 11) {
            o->stackable = 1;
        } else if (op == 12) {
            TRY(g4s(b, &o->cost));
        } else if (op == 16) {
            o->members = 1;
        } else if (op == 23) {
            TRY(g2(b, &o->manwear));
            TRY(g1b(b, &o->manwear_y));
        } else if (op == 24) {
            TRY(g2(b, &o->manwear2));
        } else if (op == 25) {
            TRY(g2(b, &o->womanwear));
            TRY(g1b(b, &o->womanwear_y));
        } else if (op == 26) {
            TRY(g2(b, &o->womanwear2));
        } else if (op >= 30 && op < 35) {
            TRY(read_op(b, o->op[op - 30], &o->op_set[op - 30], HIDE_CI));
        } else if (op >= 35 && op < 40) {
            TRY(read_op(b, o->iop[op - 35], &o->iop_set[op - 35], HIDE_NONE));
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 78) {
            TRY(g2(b, &o->manwear3));
        } else if (op == 79) {
            TRY(g2(b, &o->womanwear3));
        } else if (op == 90) {
            TRY(g2(b, &o->manhead));
        } else if (op == 91) {
            TRY(g2(b, &o->womanhead));
        } else if (op == 92) {
            TRY(g2(b, &o->manhead2));
        } else if (op == 93) {
            TRY(g2(b, &o->womanhead2));
        } else if (op == 95) {
            TRY(g2(b, &o->zan2d));
        } else if (op == 97) {
            TRY(g2(b, &o->certlink));
        } else if (op == 98) {
            TRY(g2(b, &o->certtemplate));
        } else if (op >= 100 && op < 110) {
            TRY(obj_count(b, o, op - 100));
        } else if (op == 110) {
            TRY(g2(b, &o->resizex));
        } else if (op == 111) {
            TRY(g2(b, &o->resizey));
        } else if (op == 112) {
            TRY(g2(b, &o->resizez));
        } else if (op == 113) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 114) {
            TRY(g1b(b, &o->contrast));
            o->contrast *= 5;
        } else if (op == 115) {
            TRY(g1(b, &o->team));
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err obj_decode_474(rs_buf *b, rs_obj_type *o) {
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            return RS_OK;
        } else if (op == 1) {
            TRY(g2(b, &o->model));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 4) {
            TRY(g2(b, &o->zoom2d));
        } else if (op == 5) {
            TRY(g2(b, &o->xan2d));
        } else if (op == 6) {
            TRY(g2(b, &o->yan2d));
        } else if (op == 7) {
            TRY(g2b(b, &o->xof2d));
        } else if (op == 8) {
            TRY(g2b(b, &o->yof2d));
        } else if (op == 11) {
            o->stackable = 1;
        } else if (op == 12) {
            TRY(g4s(b, &o->cost));
        } else if (op == 16) {
            o->members = 1;
        } else if (op == 23) {
            TRY(g2(b, &o->manwear));
            TRY(g1(b, &o->manwear_y));
        } else if (op == 24) {
            TRY(g2(b, &o->manwear2));
        } else if (op == 25) {
            TRY(g2(b, &o->womanwear));
            TRY(g1(b, &o->womanwear_y));
        } else if (op == 26) {
            TRY(g2(b, &o->womanwear2));
        } else if (op >= 30 && op < 35) {
            TRY(read_op(b, o->op[op - 30], &o->op_set[op - 30], HIDE_EXACT));
        } else if (op >= 35 && op < 40) {
            TRY(read_op(b, o->iop[op - 35], &o->iop_set[op - 35], HIDE_NONE));
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 41) {
            TRY(read_recol(b, &o->retex_count, o->retex_s, o->retex_d));
        } else if (op == 65) {
            o->stockmarket = 1;
        } else if (op == 78) {
            TRY(g2(b, &o->manwear3));
        } else if (op == 79) {
            TRY(g2(b, &o->womanwear3));
        } else if (op == 90) {
            TRY(g2(b, &o->manhead));
        } else if (op == 91) {
            TRY(g2(b, &o->womanhead));
        } else if (op == 92) {
            TRY(g2(b, &o->manhead2));
        } else if (op == 93) {
            TRY(g2(b, &o->womanhead2));
        } else if (op == 95) {
            TRY(g2(b, &o->zan2d));
        } else if (op == 97) {
            TRY(g2(b, &o->certlink));
        } else if (op == 98) {
            TRY(g2(b, &o->certtemplate));
        } else if (op >= 100 && op < 110) {
            TRY(obj_count(b, o, op - 100));
        } else if (op == 110) {
            TRY(g2(b, &o->resizex));
        } else if (op == 111) {
            TRY(g2(b, &o->resizey));
        } else if (op == 112) {
            TRY(g2(b, &o->resizez));
        } else if (op == 113) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 114) {
            TRY(g1b(b, &o->contrast));
            o->contrast *= 5;
        } else if (op == 115) {
            TRY(g1(b, &o->team));
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err obj_decode_530(rs_buf *b, rs_obj_type *o) {
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            return RS_OK;
        } else if (op == 1) {
            TRY(g2(b, &o->model));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 4) {
            TRY(g2(b, &o->zoom2d));
        } else if (op == 5) {
            TRY(g2(b, &o->xan2d));
        } else if (op == 6) {
            TRY(g2(b, &o->yan2d));
        } else if (op == 7) {
            TRY(g2b(b, &o->xof2d));
        } else if (op == 8) {
            TRY(g2b(b, &o->yof2d));
        } else if (op == 11) {
            o->stackable = 1;
        } else if (op == 12) {
            TRY(g4s(b, &o->cost));
        } else if (op == 16) {
            o->members = 1;
        } else if (op == 23) {
            TRY(g2(b, &o->manwear));
        } else if (op == 24) {
            TRY(g2(b, &o->manwear2));
        } else if (op == 25) {
            TRY(g2(b, &o->womanwear));
        } else if (op == 26) {
            TRY(g2(b, &o->womanwear2));
        } else if (op >= 30 && op < 35) {
            TRY(read_op(b, o->op[op - 30], &o->op_set[op - 30], HIDE_EXACT));
        } else if (op >= 35 && op < 40) {
            TRY(read_op(b, o->iop[op - 35], &o->iop_set[op - 35], HIDE_NONE));
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 41) {
            TRY(read_recol(b, &o->retex_count, o->retex_s, o->retex_d));
        } else if (op == 42) {
            TRY(read_palette(b, &o->palette_count, o->palette));
        } else if (op == 65) {
            o->stockmarket = 1;
        } else if (op == 78) {
            TRY(g2(b, &o->manwear3));
        } else if (op == 79) {
            TRY(g2(b, &o->womanwear3));
        } else if (op == 90) {
            TRY(g2(b, &o->manhead));
        } else if (op == 91) {
            TRY(g2(b, &o->womanhead));
        } else if (op == 92) {
            TRY(g2(b, &o->manhead2));
        } else if (op == 93) {
            TRY(g2(b, &o->womanhead2));
        } else if (op == 95) {
            TRY(g2(b, &o->zan2d));
        } else if (op == 96) {
            TRY(g1(b, &o->dummyitem));
        } else if (op == 97) {
            TRY(g2(b, &o->certlink));
        } else if (op == 98) {
            TRY(g2(b, &o->certtemplate));
        } else if (op >= 100 && op < 110) {
            TRY(obj_count(b, o, op - 100));
        } else if (op == 110) {
            TRY(g2(b, &o->resizex));
        } else if (op == 111) {
            TRY(g2(b, &o->resizey));
        } else if (op == 112) {
            TRY(g2(b, &o->resizez));
        } else if (op == 113) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 114) {
            TRY(g1b(b, &o->contrast));
            o->contrast *= 5;
        } else if (op == 115) {
            TRY(g1(b, &o->team));
        } else if (op == 121) {
            TRY(g2(b, &o->lentlink));
        } else if (op == 122) {
            TRY(g2(b, &o->lenttemplate));
        } else if (op == 125) {
            TRY(obj_offsets(b, &o->manwearxoff, &o->manwearyoff, &o->manwearzoff, 0));
        } else if (op == 126) {
            TRY(obj_offsets(b, &o->womanwearxoff, &o->womanwearyoff, &o->womanwearzoff, 0));
        } else if (op == 127) {
            TRY(g1(b, &o->op127_a));
            TRY(g2(b, &o->op127_b));
        } else if (op == 128) {
            TRY(g1(b, &o->op128_a));
            TRY(g2(b, &o->op128_b));
        } else if (op == 129 || op == 130) {
            TRY(skip_g1g2(b));
        } else if (op == 249) {
            TRY(read_params(b, &o->param_count, o->params));
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err obj_decode_late(rs_buf *b, rs_obj_type *o, int rev) {
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            if (rev == 910 && o->dummyitem != 0) {
                o->tradeable = 0;
            }
            return RS_OK;
        } else if (op == 1) {
            TRY(smart2or4(b, &o->model, 1));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 4) {
            TRY(g2(b, &o->zoom2d));
        } else if (op == 5) {
            TRY(g2(b, &o->xan2d));
        } else if (op == 6) {
            TRY(g2(b, &o->yan2d));
        } else if (op == 7) {
            TRY(g2b(b, &o->xof2d));
        } else if (op == 8) {
            TRY(g2b(b, &o->yof2d));
        } else if (op == 11) {
            o->stackable = 1;
        } else if (op == 12) {
            TRY(g4s(b, &o->cost));
        } else if (op == 13) {
            TRY(g1(b, &o->wearpos));
        } else if (op == 14) {
            TRY(g1(b, &o->wearpos2));
        } else if (op == 15) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            o->tradeable = 0;
        } else if (op == 16) {
            o->members = 1;
        } else if (op == 18) {
            if (rev != 742) {
                return RS_ERR_FORMAT;
            }
            TRY(g2(b, &o->aux18));
        } else if (op == 23) {
            TRY(smart2or4(b, &o->manwear, 1));
        } else if (op == 24) {
            TRY(smart2or4(b, &o->manwear2, 1));
        } else if (op == 25) {
            TRY(smart2or4(b, &o->womanwear, 1));
        } else if (op == 26) {
            TRY(smart2or4(b, &o->womanwear2, 1));
        } else if (op == 27) {
            TRY(g1(b, &o->wearpos3));
        } else if (op >= 30 && op < 35) {
            TRY(read_op(b, o->op[op - 30], &o->op_set[op - 30], HIDE_NONE));
        } else if (op >= 35 && op < 40) {
            TRY(read_op(b, o->iop[op - 35], &o->iop_set[op - 35], HIDE_NONE));
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 41) {
            TRY(read_recol(b, &o->retex_count, o->retex_s, o->retex_d));
        } else if (op == 42) {
            TRY(read_palette(b, &o->palette_count, o->palette));
        } else if (op == 43) {
            TRY(g4s(b, &o->minimenu_colour));
            o->minimenu_colour_set = 1;
        } else if (op == 44) {
            TRY(read_index(b, &o->recolindex_count, o->recolindex));
        } else if (op == 45) {
            TRY(read_index(b, &o->retexindex_count, o->retexindex));
        } else if (op == 65) {
            o->stockmarket = 1;
        } else if (op == 78) {
            TRY(smart2or4(b, &o->manwear3, 1));
        } else if (op == 79) {
            TRY(smart2or4(b, &o->womanwear3, 1));
        } else if (op == 90) {
            TRY(smart2or4(b, &o->manhead, 1));
        } else if (op == 91) {
            TRY(smart2or4(b, &o->womanhead, 1));
        } else if (op == 92) {
            TRY(smart2or4(b, &o->manhead2, 1));
        } else if (op == 93) {
            TRY(smart2or4(b, &o->womanhead2, 1));
        } else if (op == 94) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(g2(b, &o->category));
        } else if (op == 95) {
            TRY(g2(b, &o->zan2d));
        } else if (op == 96) {
            TRY(g1(b, &o->dummyitem));
        } else if (op == 97) {
            TRY(g2(b, &o->certlink));
        } else if (op == 98) {
            TRY(g2(b, &o->certtemplate));
        } else if (op >= 100 && op < 110) {
            TRY(obj_count(b, o, op - 100));
        } else if (op == 110) {
            TRY(g2(b, &o->resizex));
        } else if (op == 111) {
            TRY(g2(b, &o->resizey));
        } else if (op == 112) {
            TRY(g2(b, &o->resizez));
        } else if (op == 113) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 114) {
            TRY(g1b(b, &o->contrast));
            if (rev == 910) {
                o->contrast *= 5;
            }
        } else if (op == 115) {
            TRY(g1(b, &o->team));
        } else if (op == 121) {
            TRY(g2(b, &o->lentlink));
        } else if (op == 122) {
            TRY(g2(b, &o->lenttemplate));
        } else if (op == 125) {
            TRY(obj_offsets(b, &o->manwearxoff, &o->manwearyoff, &o->manwearzoff, 2));
        } else if (op == 126) {
            TRY(obj_offsets(b, &o->womanwearxoff, &o->womanwearyoff, &o->womanwearzoff, 2));
        } else if (op >= 127 && op <= 130) {
            TRY(skip_g1g2(b));
        } else if (op == 132) {
            TRY(read_quests(b, &o->quest_count, o->quests));
        } else if (op == 134) {
            TRY(g1(b, &o->picksizeshift));
        } else if (op == 139) {
            TRY(g2(b, &o->boughtlink));
        } else if (op == 140) {
            TRY(g2(b, &o->boughttemplate));
        } else if (op >= 142 && op < 147) {
            cursor_init(&o->cursor_set, o->cursor, RS_CONTENT_CURSOR);
            TRY(g2(b, &o->cursor[op - 142]));
        } else if (op >= 150 && op < 155) {
            cursor_init(&o->icursor_set, o->icursor, RS_CONTENT_OP);
            TRY(g2(b, &o->icursor[op - 150]));
        } else if (op == 156) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
        } else if (op == 157) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            o->tboolean = 1;
        } else if (op == 161) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(g2(b, &o->shardlink));
        } else if (op == 162) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(g2(b, &o->shardtemplate));
        } else if (op == 163) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(g2(b, &o->shardcount));
        } else if (op == 164) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(gstr(b, o->shardname, RS_CONTENT_NAME, &o->shardname_set));
        } else if (op == 165) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            o->stackable = 2;
        } else if (op == 167) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            o->cert_not_tradeable = 1;
        } else if (op == 168) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            o->placeholder = 0;
        } else if (op == 249) {
            TRY(read_params(b, &o->param_count, o->params));
        } else if (rev == 742 && op == 242) {
            TRY(smart2or4(b, &op, 0));
            TRY(smart2or4(b, &op, 0));
        } else if (rev == 742 && op >= 243 && op <= 248) {
            TRY(smart2or4(b, &op, 0));
        } else if (rev == 742 && op == 250) {
            TRY(skip_n(b, 1));
        } else if (rev == 742 && (op == 251 || op == 252)) {
            TRY(skip_counted_pairs(b));
        } else if (rev == 742 && op == 253) {
            TRY(skip_n(b, 8));
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

rs_err rs_obj_type_decode(int revision, rs_buf *src, rs_obj_type *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    if (!rev_ok(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    obj_reset(out, revision);
    if (revision == 317) {
        return obj_decode_317(src, out);
    }
    if (revision == 474) {
        return obj_decode_474(src, out);
    }
    if (revision == 530) {
        return obj_decode_530(src, out);
    }
    return obj_decode_late(src, out, revision);
}

static void npc_anims_m1(rs_npc_type *o) {
    o->readyanim = o->walkanim = o->walkanim_b = o->walkanim_r = o->walkanim_l = -1;
}

static void npc_reset(rs_npc_type *o, int rev) {
    memset(o, 0, sizeof(*o));
    o->size = 1;
    o->vislevel = -1;
    o->turnspeed = 32;
    o->active = 1;
    o->resizeh = o->resizev = 128;
    o->minimap = 1;
    o->headicon = -1;
    o->multivarbit = o->multivarp = -1;
    o->multi_default = -1;
    if (rev == 317) {
        npc_anims_m1(o);
        o->field90 = o->field91 = o->field92 = -1;
        return;
    }
    if (rev == 474) {
        npc_anims_m1(o);
        o->anim15 = o->anim16 = -1;
        o->walksmoothing = 1;
        return;
    }
    o->walksmoothing = 1;
    o->spotshadow = 1;
    o->spotshadowtrans_1 = -96;
    o->spotshadowtrans_2 = -16;
    o->bgsound = o->bgsound_crawl = o->bgsound_walk = o->bgsound_run = -1;
    o->bas = -1;
    o->overlayheight = -1;
    o->attackcursor = -1;
    o->aux122 = o->aux126 = -1;
    o->aux135_a = o->aux135_b = o->aux136_a = o->aux136_b = -1;
    if (rev == 530) {
        o->respawndir = 7;
        return;
    }
    copy_null(o->name, &o->name_set);
    o->respawndir = 4;
    o->covermarker = -1;
    o->mapelement = -1;
    o->oppriority = -1;
    o->picksize = -1;
    o->bgsound_volume = 255;
    o->bgsound_minrate = o->bgsound_maxrate = 256;
    o->antimacro = 1;
    o->spotshadowtexture = -1;
}

static rs_err npc_multi(rs_buf *b, rs_npc_type *o, int with_default, int smart_count) {
    int n;
    int i;
    int def = -1;
    int len;
    TRY(g2(b, &o->multivarbit));
    o->multivarbit = m1_u16(o->multivarbit);
    TRY(g2(b, &o->multivarp));
    o->multivarp = m1_u16(o->multivarp);
    if (with_default) {
        TRY(g2(b, &def));
        def = m1_u16(def);
    }
    if (smart_count) {
        TRY(smart1or2(b, &n));
    } else {
        TRY(g1(b, &n));
    }
    if (n < 0) {
        return RS_ERR_FORMAT;
    }
    len = n + (with_default ? 2 : 1);
    if (len > RS_CONTENT_MULTI) {
        return RS_ERR_RANGE;
    }
    o->multi_count = len;
    o->multi_default = def;
    for (i = 0; i <= n; i++) {
        TRY(g2(b, &o->multi[i]));
        o->multi[i] = m1_u16(o->multi[i]);
    }
    if (with_default) {
        o->multi[n + 1] = def;
    }
    return RS_OK;
}

static rs_err npc_bgsound(rs_buf *b, rs_npc_type *o) {
    TRY(g2(b, &o->bgsound));
    o->bgsound = m1_u16(o->bgsound);
    TRY(g2(b, &o->bgsound_crawl));
    o->bgsound_crawl = m1_u16(o->bgsound_crawl);
    TRY(g2(b, &o->bgsound_walk));
    o->bgsound_walk = m1_u16(o->bgsound_walk);
    TRY(g2(b, &o->bgsound_run));
    o->bgsound_run = m1_u16(o->bgsound_run);
    TRY(g1(b, &o->bgsound_range));
    return RS_OK;
}

static rs_err npc_model_off(rs_buf *b, rs_npc_type *o) {
    int n;
    int i;
    if (o->model_count <= 0) {
        return RS_ERR_FORMAT;
    }
    TRY(g1(b, &n));
    if (n > RS_CONTENT_MODEL) {
        return RS_ERR_RANGE;
    }
    o->model_off_count = n;
    for (i = 0; i < n; i++) {
        int idx;
        TRY(g1(b, &idx));
        if (idx < 0 || idx >= o->model_count) {
            return RS_ERR_RANGE;
        }
        o->model_off_index[i] = idx;
        TRY(g1b(b, &o->model_off[idx][0]));
        TRY(g1b(b, &o->model_off[idx][1]));
        TRY(g1b(b, &o->model_off[idx][2]));
    }
    return RS_OK;
}

static rs_err npc_headicons_910(rs_buf *b, rs_npc_type *o) {
    int mask;
    int n = 0;
    int i;
    TRY(g1(b, &mask));
    for (i = mask; i != 0; i >>= 1) {
        n++;
    }
    if (n > RS_CONTENT_HEADICON) {
        return RS_ERR_RANGE;
    }
    o->headicon_count = n;
    for (i = 0; i < n; i++) {
        if ((mask & (1 << i)) == 0) {
            o->headicon_group[i] = -1;
            o->headicon_id[i] = -1;
        } else {
            TRY(smart2or4(b, &o->headicon_group[i], 1));
            TRY(smart1or2null(b, &o->headicon_id[i]));
        }
    }
    return RS_OK;
}

static rs_err npc_clickbox(rs_buf *b, rs_npc_type *o) {
    int i;
    for (i = 0; i < 6; i++) {
        TRY(smart1or2s(b, &o->clickbox[i]));
    }
    o->clickbox_set = 1;
    return RS_OK;
}

static rs_err npc_decode_317(rs_buf *b, rs_npc_type *o) {
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            return RS_OK;
        } else if (op == 1) {
            TRY(read_models(b, &o->model_count, o->models, RS_CONTENT_MODEL, 0));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 3) {
            TRY(gstr(b, o->desc, RS_CONTENT_DESC, &o->desc_set));
        } else if (op == 12) {
            TRY(g1b(b, &o->size));
        } else if (op == 13) {
            TRY(g2(b, &o->readyanim));
        } else if (op == 14) {
            TRY(g2(b, &o->walkanim));
        } else if (op == 17) {
            TRY(g2(b, &o->walkanim));
            TRY(g2(b, &o->walkanim_b));
            TRY(g2(b, &o->walkanim_r));
            TRY(g2(b, &o->walkanim_l));
        } else if (op >= 30 && op < 35) {
            TRY(read_op(b, o->op[op - 30], &o->op_set[op - 30], HIDE_CI));
        } else if (op >= 35 && op < 40) {
            TRY(discard_str(b));
            return RS_ERR_RANGE;
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 60) {
            TRY(read_models(b, &o->head_count, o->heads, RS_CONTENT_HEAD, 0));
        } else if (op == 90) {
            TRY(g2(b, &o->field90));
        } else if (op == 91) {
            TRY(g2(b, &o->field91));
        } else if (op == 92) {
            TRY(g2(b, &o->field92));
        } else if (op == 93) {
            o->minimap = 0;
        } else if (op == 95) {
            TRY(g2(b, &o->vislevel));
        } else if (op == 97) {
            TRY(g2(b, &o->resizeh));
        } else if (op == 98) {
            TRY(g2(b, &o->resizev));
        } else if (op == 99) {
            o->alwaysontop = 1;
        } else if (op == 100) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 101) {
            TRY(g1b(b, &o->contrast));
            o->contrast *= 5;
        } else if (op == 102) {
            TRY(g2(b, &o->headicon));
        } else if (op == 103) {
            TRY(g2(b, &o->turnspeed));
        } else if (op == 106) {
            TRY(npc_multi(b, o, 0, 0));
        } else if (op == 107) {
            o->active = 0;
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err npc_decode_474(rs_buf *b, rs_npc_type *o) {
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            return RS_OK;
        } else if (op == 1) {
            TRY(read_models(b, &o->model_count, o->models, RS_CONTENT_MODEL, 0));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 12) {
            TRY(g1(b, &o->size));
        } else if (op == 13) {
            TRY(g2(b, &o->readyanim));
        } else if (op == 14) {
            TRY(g2(b, &o->walkanim));
        } else if (op == 15) {
            TRY(g2(b, &o->anim15));
        } else if (op == 16) {
            TRY(g2(b, &o->anim16));
        } else if (op == 17) {
            TRY(g2(b, &o->walkanim));
            TRY(g2(b, &o->walkanim_b));
            TRY(g2(b, &o->walkanim_r));
            TRY(g2(b, &o->walkanim_l));
        } else if (op >= 30 && op < 35) {
            TRY(read_op(b, o->op[op - 30], &o->op_set[op - 30], HIDE_EXACT));
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 41) {
            TRY(read_recol(b, &o->retex_count, o->retex_s, o->retex_d));
        } else if (op == 60) {
            TRY(read_models(b, &o->head_count, o->heads, RS_CONTENT_HEAD, 0));
        } else if (op == 93) {
            o->minimap = 0;
        } else if (op == 95) {
            TRY(g2(b, &o->vislevel));
        } else if (op == 97) {
            TRY(g2(b, &o->resizeh));
        } else if (op == 98) {
            TRY(g2(b, &o->resizev));
        } else if (op == 99) {
            o->alwaysontop = 1;
        } else if (op == 100) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 101) {
            TRY(g1b(b, &o->contrast));
            o->contrast *= 5;
        } else if (op == 102) {
            TRY(g2(b, &o->headicon));
        } else if (op == 103) {
            TRY(g2(b, &o->turnspeed));
        } else if (op == 106) {
            TRY(npc_multi(b, o, 0, 0));
        } else if (op == 107) {
            o->active = 0;
        } else if (op == 109) {
            o->walksmoothing = 0;
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err npc_decode_530(rs_buf *b, rs_npc_type *o) {
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            return RS_OK;
        } else if (op == 1) {
            TRY(read_models(b, &o->model_count, o->models, RS_CONTENT_MODEL, 1));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 12) {
            TRY(g1(b, &o->size));
        } else if (op >= 30 && op < 35) {
            TRY(read_op(b, o->op[op - 30], &o->op_set[op - 30], HIDE_EXACT));
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 41) {
            TRY(read_recol(b, &o->retex_count, o->retex_s, o->retex_d));
        } else if (op == 42) {
            TRY(read_palette(b, &o->palette_count, o->palette));
        } else if (op == 60) {
            TRY(read_models(b, &o->head_count, o->heads, RS_CONTENT_HEAD, 0));
        } else if (op == 93) {
            o->minimap = 0;
        } else if (op == 95) {
            TRY(g2(b, &o->vislevel));
        } else if (op == 97) {
            TRY(g2(b, &o->resizeh));
        } else if (op == 98) {
            TRY(g2(b, &o->resizev));
        } else if (op == 99) {
            o->alwaysontop = 1;
        } else if (op == 100) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 101) {
            TRY(g1b(b, &o->contrast));
            o->contrast *= 5;
        } else if (op == 102) {
            TRY(g2(b, &o->headicon));
        } else if (op == 103) {
            TRY(g2(b, &o->turnspeed));
        } else if (op == 106 || op == 118) {
            TRY(npc_multi(b, o, op == 118, 0));
        } else if (op == 107) {
            o->active = 0;
        } else if (op == 109) {
            o->walksmoothing = 0;
        } else if (op == 111) {
            o->spotshadow = 0;
        } else if (op == 113) {
            TRY(g2(b, &o->spotshadowcolour_1));
            TRY(g2(b, &o->spotshadowcolour_2));
        } else if (op == 114) {
            TRY(g1b(b, &o->spotshadowtrans_1));
            TRY(g1b(b, &o->spotshadowtrans_2));
        } else if (op == 115) {
            TRY(skip_n(b, 2));
        } else if (op == 119) {
            TRY(g1b(b, &o->walkflags));
        } else if (op == 121) {
            TRY(npc_model_off(b, o));
        } else if (op == 122) {
            TRY(g2(b, &o->aux122));
        } else if (op == 123) {
            TRY(g2(b, &o->overlayheight));
        } else if (op == 125) {
            TRY(g1b(b, &o->respawndir));
        } else if (op == 126) {
            TRY(g2(b, &o->aux126));
        } else if (op == 127) {
            TRY(g2(b, &o->bas));
        } else if (op == 128) {
            TRY(skip_n(b, 1));
        } else if (op == 134) {
            TRY(npc_bgsound(b, o));
        } else if (op == 135) {
            TRY(g1(b, &o->aux135_a));
            TRY(g2(b, &o->aux135_b));
        } else if (op == 136) {
            TRY(g1(b, &o->aux136_a));
            TRY(g2(b, &o->aux136_b));
        } else if (op == 137) {
            TRY(g2(b, &o->attackcursor));
        } else if (op == 249) {
            TRY(read_params(b, &o->param_count, o->params));
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err npc_decode_late(rs_buf *b, rs_npc_type *o, int rev) {
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            return RS_OK;
        } else if (op == 1) {
            TRY(read_models(b, &o->model_count, o->models, RS_CONTENT_MODEL, 2));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 12) {
            TRY(g1(b, &o->size));
        } else if (op >= 30 && op < 35) {
            TRY(read_op(b, o->op[op - 30], &o->op_set[op - 30], HIDE_NONE));
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 41) {
            TRY(read_recol(b, &o->retex_count, o->retex_s, o->retex_d));
        } else if (op == 42) {
            TRY(read_palette(b, &o->palette_count, o->palette));
        } else if (op == 44) {
            TRY(read_index(b, &o->recolindex_count, o->recolindex));
        } else if (op == 45) {
            TRY(read_index(b, &o->retexindex_count, o->retexindex));
        } else if (op == 60) {
            TRY(read_models(b, &o->head_count, o->heads, RS_CONTENT_HEAD, 2));
        } else if (op == 93) {
            o->minimap = 0;
        } else if (op == 95) {
            TRY(g2(b, &o->vislevel));
        } else if (op == 97) {
            TRY(g2(b, &o->resizeh));
        } else if (op == 98) {
            TRY(g2(b, &o->resizev));
        } else if (op == 99) {
            o->alwaysontop = 1;
        } else if (op == 100) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 101) {
            TRY(g1b(b, &o->contrast));
        } else if (op == 102) {
            if (rev == 910) {
                TRY(npc_headicons_910(b, o));
            } else {
                TRY(g2(b, &o->headicon));
            }
        } else if (op == 103) {
            TRY(g2(b, &o->turnspeed));
        } else if (op == 106 || op == 118) {
            TRY(npc_multi(b, o, op == 118, rev == 910));
        } else if (op == 107) {
            o->active = 0;
        } else if (op == 109) {
            o->walksmoothing = 0;
        } else if (op == 111) {
            o->spotshadow = 0;
        } else if (op == 113) {
            TRY(g2(b, &o->spotshadowcolour_1));
            TRY(g2(b, &o->spotshadowcolour_2));
        } else if (op == 114) {
            TRY(g1b(b, &o->spotshadowtrans_1));
            TRY(g1b(b, &o->spotshadowtrans_2));
        } else if (op == 119) {
            TRY(g1b(b, &o->walkflags));
        } else if (op == 121) {
            TRY(npc_model_off(b, o));
        } else if (op == 123) {
            TRY(g2(b, &o->overlayheight));
        } else if (op == 125) {
            TRY(g1b(b, &o->respawndir));
        } else if (op == 127) {
            TRY(g2(b, &o->bas));
        } else if (op == 128) {
            TRY(skip_n(b, 1));
        } else if (op == 134) {
            TRY(npc_bgsound(b, o));
        } else if (op == 135 || op == 136) {
            TRY(skip_g1g2(b));
        } else if (op == 137) {
            TRY(g2(b, &o->attackcursor));
        } else if (op == 138) {
            TRY(smart2or4(b, &o->covermarker, 1));
        } else if (op == 140) {
            TRY(g1(b, &o->bgsound_volume));
        } else if (op == 141) {
            o->follower = 1;
        } else if (op == 142) {
            TRY(g2(b, &o->mapelement));
        } else if (op == 143) {
            o->drawbelow = 1;
        } else if (op >= 150 && op < 155) {
            TRY(read_op(b, o->op[op - 150], &o->op_set[op - 150], HIDE_NONE));
        } else if (op == 155) {
            TRY(g1b(b, &o->tint_hue));
            TRY(g1b(b, &o->tint_saturation));
            TRY(g1b(b, &o->tint_luminence));
            TRY(g1b(b, &o->tint_weight));
        } else if (op == 158) {
            o->oppriority = 1;
        } else if (op == 159) {
            o->oppriority = 0;
        } else if (op == 160) {
            TRY(read_quests(b, &o->quest_count, o->quests));
        } else if (op == 162) {
            if (rev == 742) {
                o->flag162 = 1;
            } else if (rev != 910) {
                return RS_ERR_FORMAT;
            }
        } else if (op == 163) {
            TRY(g1(b, &o->picksize));
        } else if (op == 164) {
            TRY(g2(b, &o->bgsound_minrate));
            TRY(g2(b, &o->bgsound_maxrate));
        } else if (op == 165) {
            TRY(g1(b, &o->picksizeshift));
        } else if (op == 168) {
            TRY(g1(b, &o->bgsound_size));
        } else if (op == 169) {
            o->antimacro = 0;
        } else if (op >= 170 && op < 176) {
            int v;
            cursor_init(&o->cursor_set, o->cursor, RS_CONTENT_CURSOR);
            TRY(g2(b, &v));
            o->cursor[op - 170] = m1_u16(v);
        } else if (op == 178) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
        } else if (op == 179) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(npc_clickbox(b, o));
        } else if (op == 180) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(g1(b, &o->fade_in));
        } else if (op == 181) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(g2(b, &o->spotshadowtexture));
            TRY(g1(b, &o->spotshadowtexture_alpha));
        } else if (op == 182) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            o->transmog = 1;
        } else if (op == 249) {
            TRY(read_params(b, &o->param_count, o->params));
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

rs_err rs_npc_type_decode(int revision, rs_buf *src, rs_npc_type *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    if (!rev_ok(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    npc_reset(out, revision);
    if (revision == 317) {
        return npc_decode_317(src, out);
    }
    if (revision == 474) {
        return npc_decode_474(src, out);
    }
    if (revision == 530) {
        return npc_decode_530(src, out);
    }
    return npc_decode_late(src, out, revision);
}

static int loc_any_op(const rs_loc_type *o) {
    int i;
    for (i = 0; i < RS_CONTENT_OP; i++) {
        if (o->op_set[i]) {
            return 1;
        }
    }
    return 0;
}

static void loc_apply_active_parallel(rs_loc_type *o) {
    int i;
    o->active = 0;
    if (o->model_count > 0 && (!o->shapes_present || o->shapes[0] == 10)) {
        o->active = 1;
    }
    for (i = 0; i < RS_CONTENT_OP; i++) {
        if (o->op_set[i]) {
            o->active = 1;
        }
    }
}

static void loc_apply_active_groups(rs_loc_type *o) {
    int i;
    o->active = 0;
    if (o->group_count == 1 && o->group_shape[0] == 10) {
        o->active = 1;
    }
    for (i = 0; i < RS_CONTENT_OP; i++) {
        if (o->op_set[i]) {
            o->active = 1;
        }
    }
}

/* 317: active (if opcode 19 absent), then breakroute, then raiseobject. */
static void loc_finish_317(rs_loc_type *o, int seen19) {
    if (!seen19) {
        o->active = 0;
        if (o->model_count > 0 && (!o->shapes_present || o->shapes[0] == 10)) {
            o->active = 1;
        }
        if (loc_any_op(o)) {
            o->active = 1;
        }
    }
    if (o->breakroutefinding) {
        o->blockwalk = 0;
        o->blockrange = 0;
    }
    if (o->raiseobject == -1) {
        o->raiseobject = o->blockwalk ? 1 : 0;
    }
}

/* raiseobject from blockwalk, then breakroute clears blockwalk (and blockrange except 910). */
static void loc_finish_later(rs_loc_type *o, int groups, int clear_range, int set_tb3) {
    if (o->active == -1) {
        if (groups) {
            loc_apply_active_groups(o);
        } else {
            loc_apply_active_parallel(o);
        }
    }
    if (o->raiseobject == -1) {
        o->raiseobject = o->blockwalk != 0 ? 1 : 0;
    }
    if (set_tb3 && (o->anim_count > 0 || o->tboolean2 || o->multi_count > 0)) {
        o->tboolean3 = 1;
    }
    if (o->breakroutefinding) {
        o->blockwalk = 0;
        if (clear_range) {
            o->blockrange = 0;
        }
    }
}

static void loc_reset(rs_loc_type *o, int rev) {
    memset(o, 0, sizeof(*o));
    o->width = o->length = 1;
    o->blockrange = 1;
    o->anim = -1;
    o->shadow = 1;
    o->resizex = o->resizey = o->resizez = 128;
    o->mapfunction = o->mapscene = o->mapelement = -1;
    o->multivarbit = o->multivarp = -1;
    o->multi_default = -1;
    o->raiseobject = -1;
    o->bgsound = -1;
    if (rev == 317) {
        o->blockwalk = 1;
        o->walloff = 16;
        return;
    }
    o->blockwalk = 2;
    o->active = -1;
    o->hillchange_value = -1;
    if (rev == 474) {
        o->walloff = 16;
        return;
    }
    o->hardshadow = 1;
    o->randomanim = 1;
    o->walloff = (rev == 530) ? 16 : 64;
    if (rev == 530) {
        o->aux99_a = -1;
        o->aux100_a = o->aux100_b = -1;
        return;
    }
    copy_null(o->name, &o->name_set);
    o->occlude = -1;
    o->tint = 960;
    o->bgsound_volume = 255;
    o->bgsound_minrate = o->bgsound_maxrate = 256;
}

/* lowMemory is false, so a second model list is skipped once models are set. */
static rs_err loc_models_1(rs_buf *b, rs_loc_type *o) {
    int n;
    int i;
    TRY(g1(b, &n));
    if (n <= 0) {
        return RS_OK;
    }
    if (o->model_count > 0) {
        return skip_n(b, (size_t)n * 3);
    }
    if (n > RS_CONTENT_MODEL) {
        return RS_ERR_RANGE;
    }
    o->model_count = n;
    o->shapes_present = 1;
    for (i = 0; i < n; i++) {
        TRY(g2(b, &o->models[i]));
        TRY(g1(b, &o->shapes[i]));
    }
    return RS_OK;
}

static rs_err loc_models_5(rs_buf *b, rs_loc_type *o) {
    int n;
    int i;
    TRY(g1(b, &n));
    if (n <= 0) {
        return RS_OK;
    }
    if (o->model_count > 0) {
        return skip_n(b, (size_t)n * 2);
    }
    if (n > RS_CONTENT_MODEL) {
        return RS_ERR_RANGE;
    }
    o->shapes_present = 0;
    o->model_count = n;
    for (i = 0; i < n; i++) {
        TRY(g2(b, &o->models[i]));
    }
    return RS_OK;
}

static rs_err loc_groups(rs_buf *b, rs_loc_type *o) {
    int n;
    int g;
    TRY(g1(b, &n));
    if (n > RS_CONTENT_GROUP) {
        return RS_ERR_RANGE;
    }
    o->group_count = n;
    o->model_count = 0;
    o->shapes_present = n > 0;
    for (g = 0; g < n; g++) {
        int nm;
        int j;
        TRY(g1b(b, &o->group_shape[g]));
        TRY(g1(b, &nm));
        if (nm > RS_CONTENT_GROUP_MODEL) {
            return RS_ERR_RANGE;
        }
        if (o->model_count + nm > RS_CONTENT_MODEL) {
            return RS_ERR_RANGE;
        }
        o->group_n[g] = nm;
        for (j = 0; j < nm; j++) {
            int m;
            TRY(smart2or4(b, &m, 1));
            o->group_model[g][j] = m;
            o->models[o->model_count] = m;
            o->shapes[o->model_count] = o->group_shape[g];
            o->model_count++;
        }
    }
    return RS_OK;
}

static rs_err loc_multi_g2(rs_buf *b, rs_loc_type *o, int with_default) {
    int n;
    int i;
    int def = -1;
    int len;
    TRY(g2(b, &o->multivarbit));
    o->multivarbit = m1_u16(o->multivarbit);
    TRY(g2(b, &o->multivarp));
    o->multivarp = m1_u16(o->multivarp);
    if (with_default) {
        TRY(g2(b, &def));
        def = m1_u16(def);
    }
    TRY(g1(b, &n));
    len = n + (with_default ? 2 : 1);
    if (len > RS_CONTENT_MULTI) {
        return RS_ERR_RANGE;
    }
    o->multi_count = len;
    o->multi_default = def;
    for (i = 0; i <= n; i++) {
        TRY(g2(b, &o->multi[i]));
        o->multi[i] = m1_u16(o->multi[i]);
    }
    if (with_default) {
        o->multi[n + 1] = def;
    }
    return RS_OK;
}

static rs_err loc_multi_smart(rs_buf *b, rs_loc_type *o, int with_default, int smart_count) {
    int n;
    int i;
    int def = -1;
    int len;
    TRY(g2(b, &o->multivarbit));
    o->multivarbit = m1_u16(o->multivarbit);
    TRY(g2(b, &o->multivarp));
    o->multivarp = m1_u16(o->multivarp);
    if (with_default) {
        TRY(smart2or4(b, &def, 1));
    }
    if (smart_count) {
        TRY(smart1or2(b, &n));
    } else {
        TRY(g1(b, &n));
    }
    if (n < 0) {
        return RS_ERR_FORMAT;
    }
    len = n + 2;
    if (len > RS_CONTENT_MULTI) {
        return RS_ERR_RANGE;
    }
    o->multi_count = len;
    o->multi_default = def;
    for (i = 0; i <= n; i++) {
        TRY(smart2or4(b, &o->multi[i], 1));
    }
    o->multi[n + 1] = def;
    return RS_OK;
}

static rs_err loc_random_sounds(rs_buf *b, rs_loc_type *o) {
    int n;
    int i;
    TRY(g2(b, &o->bgsound_mindelay));
    TRY(g2(b, &o->bgsound_maxdelay));
    TRY(g1(b, &o->bgsound_range));
    TRY(g1(b, &n));
    if (n > RS_CONTENT_SOUND) {
        return RS_ERR_RANGE;
    }
    o->bgsound_random_count = n;
    for (i = 0; i < n; i++) {
        TRY(g2(b, &o->bgsound_random[i]));
    }
    return RS_OK;
}

static rs_err loc_anim_weights(rs_buf *b, rs_loc_type *o) {
    int n;
    int i;
    int sum = 0;
    TRY(g1(b, &n));
    if (n > RS_CONTENT_ANIM) {
        return RS_ERR_RANGE;
    }
    o->anim_count = n;
    for (i = 0; i < n; i++) {
        TRY(smart2or4(b, &o->anim_ids[i], 1));
        TRY(g1(b, &o->anim_weight[i]));
        sum += o->anim_weight[i];
    }
    if (n > 0 && sum == 0) {
        return RS_ERR_FORMAT;
    }
    for (i = 0; i < n; i++) {
        o->anim_weight[i] = o->anim_weight[i] * 65535 / sum;
    }
    if (n > 0) {
        o->anim = o->anim_ids[0];
    }
    return RS_OK;
}

static rs_err loc_one_anim(rs_buf *b, rs_loc_type *o) {
    int id;
    TRY(smart2or4(b, &id, 1));
    if (id != -1) {
        o->anim = id;
        o->anim_count = 1;
        o->anim_ids[0] = id;
    }
    return RS_OK;
}

static rs_err loc_ops_or_range(rs_buf *b, rs_loc_type *o, int op, int hide, int limit) {
    if (op >= 30 && op < 35) {
        return read_op(b, o->op[op - 30], &o->op_set[op - 30], hide);
    }
    if (op >= 35 && op < limit) {
        TRY(discard_str(b));
        return RS_ERR_RANGE;
    }
    return RS_ERR_FORMAT;
}

static rs_err loc_decode_317(rs_buf *b, rs_loc_type *o) {
    int seen19 = 0;
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            loc_finish_317(o, seen19);
            return RS_OK;
        } else if (op == 1) {
            TRY(loc_models_1(b, o));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 3) {
            TRY(gstr(b, o->desc, RS_CONTENT_DESC, &o->desc_set));
        } else if (op == 5) {
            TRY(loc_models_5(b, o));
        } else if (op == 14) {
            TRY(g1(b, &o->width));
        } else if (op == 15) {
            TRY(g1(b, &o->length));
        } else if (op == 17) {
            o->blockwalk = 0;
        } else if (op == 18) {
            o->blockrange = 0;
        } else if (op == 19) {
            int v;
            TRY(g1(b, &v));
            seen19 = 1;
            if (v == 1) {
                o->active = 1;
            }
        } else if (op == 21) {
            o->hillskew = 1;
        } else if (op == 22) {
            o->sharelight = 1;
        } else if (op == 23) {
            o->occlude = 1;
        } else if (op == 24) {
            TRY(g2(b, &o->anim));
            o->anim = m1_u16(o->anim);
        } else if (op == 28) {
            TRY(g1(b, &o->walloff));
        } else if (op == 29) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 39) {
            TRY(g1b(b, &o->contrast));
        } else if (op >= 30 && op < 39) {
            rs_err err = loc_ops_or_range(b, o, op, HIDE_CI, 39);
            if (err) {
                return err;
            }
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 60) {
            TRY(g2(b, &o->mapfunction));
        } else if (op == 62) {
            o->mirror = 1;
        } else if (op == 64) {
            o->shadow = 0;
        } else if (op == 65) {
            TRY(g2(b, &o->resizex));
        } else if (op == 66) {
            TRY(g2(b, &o->resizey));
        } else if (op == 67) {
            TRY(g2(b, &o->resizez));
        } else if (op == 68) {
            TRY(g2(b, &o->mapscene));
        } else if (op == 69) {
            TRY(g1(b, &o->forceapproach));
        } else if (op == 70) {
            TRY(g2b(b, &o->offsetx));
        } else if (op == 71) {
            TRY(g2b(b, &o->offsety));
        } else if (op == 72) {
            TRY(g2b(b, &o->offsetz));
        } else if (op == 73) {
            o->forcedecor = 1;
        } else if (op == 74) {
            o->breakroutefinding = 1;
        } else if (op == 75) {
            TRY(g1(b, &o->raiseobject));
        } else if (op == 77) {
            TRY(loc_multi_g2(b, o, 0));
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

static rs_err loc_shared_mid(rs_buf *b, rs_loc_type *o, int op, int hide, int contrast_mul) {
    if (op == 14) {
        return g1(b, &o->width);
    }
    if (op == 15) {
        return g1(b, &o->length);
    }
    if (op == 19) {
        return g1(b, &o->active);
    }
    if (op == 22) {
        o->sharelight = 1;
        return RS_OK;
    }
    if (op == 23) {
        o->occlude = 1;
        return RS_OK;
    }
    if (op == 24) {
        TRY(g2(b, &o->anim));
        o->anim = m1_u16(o->anim);
        return RS_OK;
    }
    if (op == 27) {
        o->blockwalk = 1;
        return RS_OK;
    }
    if (op == 28) {
        return g1(b, &o->walloff);
    }
    if (op == 29) {
        return g1b(b, &o->ambient);
    }
    if (op == 39) {
        TRY(g1b(b, &o->contrast));
        o->contrast *= contrast_mul;
        return RS_OK;
    }
    if (op >= 30 && op < 35) {
        return read_op(b, o->op[op - 30], &o->op_set[op - 30], hide);
    }
    if (op == 40) {
        return read_recol(b, &o->recol_count, o->recol_s, o->recol_d);
    }
    if (op == 41) {
        return read_recol(b, &o->retex_count, o->retex_s, o->retex_d);
    }
    if (op == 60) {
        return g2(b, &o->mapfunction);
    }
    if (op == 62) {
        o->mirror = 1;
        return RS_OK;
    }
    if (op == 64) {
        o->shadow = 0;
        return RS_OK;
    }
    if (op == 65) {
        return g2(b, &o->resizex);
    }
    if (op == 66) {
        return g2(b, &o->resizey);
    }
    if (op == 67) {
        return g2(b, &o->resizez);
    }
    if (op == 69) {
        return g1(b, &o->forceapproach);
    }
    if (op == 70) {
        return g2b(b, &o->offsetx);
    }
    if (op == 71) {
        return g2b(b, &o->offsety);
    }
    if (op == 72) {
        return g2b(b, &o->offsetz);
    }
    if (op == 73) {
        o->forcedecor = 1;
        return RS_OK;
    }
    if (op == 74) {
        o->breakroutefinding = 1;
        return RS_OK;
    }
    if (op == 75) {
        return g1(b, &o->raiseobject);
    }
    if (op == 78) {
        TRY(g2(b, &o->bgsound));
        return g1(b, &o->bgsound_range);
    }
    if (op == 79) {
        return loc_random_sounds(b, o);
    }
    return RS_ERR_FORMAT;
}

static rs_err loc_decode_474(rs_buf *b, rs_loc_type *o) {
    for (;;) {
        int op;
        rs_err err;
        TRY(g1(b, &op));
        if (op == 0) {
            loc_finish_later(o, 0, 1, 0);
            return RS_OK;
        }
        if (op == 1) {
            TRY(loc_models_1(b, o));
            continue;
        }
        if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
            continue;
        }
        if (op == 5) {
            TRY(loc_models_5(b, o));
            continue;
        }
        if (op == 17) {
            o->blockrange = 0;
            o->blockwalk = 0;
            continue;
        }
        if (op == 18) {
            o->blockrange = 0;
            continue;
        }
        if (op == 21) {
            o->hillskew = 1;
            o->hillchange_value = 0;
            continue;
        }
        if (op == 68) {
            TRY(g2(b, &o->mapscene));
            continue;
        }
        if (op == 77) {
            TRY(loc_multi_g2(b, o, 0));
            continue;
        }
        if (op == 81) {
            int v;
            TRY(g1(b, &v));
            o->hillskew = 1;
            o->hillchange_value = v * 256;
            continue;
        }
        err = loc_shared_mid(b, o, op, HIDE_EXACT, 5);
        if (err == RS_ERR_FORMAT && op != 14 && op != 15 && op != 19 && op != 22 && op != 23 &&
            op != 24 && op != 27 && op != 28 && op != 29 && op != 39 && !(op >= 30 && op < 35) &&
            op != 40 && op != 41 && op != 60 && op != 62 && op != 64 && op != 65 && op != 66 &&
            op != 67 && op != 69 && op != 70 && op != 71 && op != 72 && op != 73 && op != 74 &&
            op != 75 && op != 78 && op != 79) {
            return RS_ERR_FORMAT;
        }
        if (err) {
            return err;
        }
    }
}

static rs_err loc_decode_530(rs_buf *b, rs_loc_type *o) {
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            loc_finish_later(o, 0, 1, 0);
            return RS_OK;
        } else if (op == 1) {
            TRY(loc_models_1(b, o));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 5) {
            TRY(loc_models_5(b, o));
        } else if (op == 17) {
            o->blockwalk = 0;
            o->blockrange = 0;
        } else if (op == 18) {
            o->blockrange = 0;
        } else if (op == 21) {
            o->hillchange = 1;
        } else if (op == 42) {
            TRY(read_palette(b, &o->palette_count, o->palette));
        } else if (op == 77 || op == 92) {
            TRY(loc_multi_g2(b, o, op == 92));
        } else if (op == 81) {
            int v;
            TRY(g1(b, &v));
            o->hillchange = 2;
            o->hillchange_value = v * 256;
        } else if (op == 82) {
            /* explicit empty branch */
        } else if (op == 88) {
            o->hardshadow = 0;
        } else if (op == 89) {
            o->randomanim = 0;
        } else if (op == 90) {
            o->aux90 = 1;
        } else if (op == 91) {
            o->members = 1;
        } else if (op == 93) {
            o->hillchange = 3;
            TRY(g2(b, &o->hillchange_value));
        } else if (op == 94) {
            o->hillchange = 4;
        } else if (op == 95) {
            o->hillchange = 5;
        } else if (op == 96) {
            o->aux96 = 1;
        } else if (op == 97) {
            o->aux97 = 1;
        } else if (op == 98) {
            o->aux98 = 1;
        } else if (op == 99) {
            TRY(g1(b, &o->aux99_a));
            TRY(g2(b, &o->aux99_b));
        } else if (op == 100) {
            TRY(g1(b, &o->aux100_a));
            TRY(g2(b, &o->aux100_b));
        } else if (op == 101) {
            TRY(g1(b, &o->tint3));
        } else if (op == 102) {
            TRY(g2(b, &o->mapscene));
        } else if (op == 249) {
            TRY(read_params(b, &o->param_count, o->params));
        } else {
            rs_err err = loc_shared_mid(b, o, op, HIDE_EXACT, 5);
            if (err) {
                return err;
            }
        }
    }
}

static rs_err loc_decode_late(rs_buf *b, rs_loc_type *o, int rev) {
    int contrast_mul = rev == 910 ? 5 : 1;
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            loc_finish_later(o, 1, rev != 910, 1);
            return RS_OK;
        } else if (op == 1) {
            TRY(loc_groups(b, o));
        } else if (op == 2) {
            TRY(gstr(b, o->name, RS_CONTENT_NAME, &o->name_set));
        } else if (op == 14) {
            TRY(g1(b, &o->width));
        } else if (op == 15) {
            TRY(g1(b, &o->length));
        } else if (op == 17) {
            o->blockwalk = 0;
            if (rev == 742) {
                o->blockrange = 0;
            }
        } else if (op == 18) {
            if (rev == 742) {
                o->blockrange = 0;
            }
        } else if (op == 19) {
            TRY(g1(b, &o->active));
        } else if (op == 21) {
            o->hillchange = 1;
        } else if (op == 22) {
            o->sharelight = 1;
        } else if (op == 23) {
            o->occlude = 1;
        } else if (op == 24) {
            TRY(loc_one_anim(b, o));
        } else if (op == 27) {
            o->blockwalk = 1;
        } else if (op == 28) {
            int v;
            TRY(g1(b, &v));
            o->walloff = v << 2;
        } else if (op == 29) {
            TRY(g1b(b, &o->ambient));
        } else if (op == 39) {
            TRY(g1b(b, &o->contrast));
            o->contrast *= contrast_mul;
        } else if (op >= 30 && op < 35) {
            TRY(read_op(b, o->op[op - 30], &o->op_set[op - 30], HIDE_NONE));
        } else if (op == 40) {
            TRY(read_recol(b, &o->recol_count, o->recol_s, o->recol_d));
        } else if (op == 41) {
            TRY(read_recol(b, &o->retex_count, o->retex_s, o->retex_d));
        } else if (op == 42) {
            TRY(read_palette(b, &o->palette_count, o->palette));
        } else if (op == 44) {
            TRY(read_index(b, &o->recolindex_count, o->recolindex));
        } else if (op == 45) {
            TRY(read_index(b, &o->retexindex_count, o->retexindex));
        } else if (op == 62) {
            o->mirror = 1;
        } else if (op == 64) {
            o->shadow = 0;
        } else if (op == 65) {
            TRY(g2(b, &o->resizex));
        } else if (op == 66) {
            TRY(g2(b, &o->resizey));
        } else if (op == 67) {
            TRY(g2(b, &o->resizez));
        } else if (op == 69) {
            TRY(skip_n(b, 1));
        } else if (op == 70) {
            int v;
            TRY(g2b(b, &v));
            o->offsetx = shl(v, 2);
        } else if (op == 71) {
            int v;
            TRY(g2b(b, &v));
            o->offsety = shl(v, 2);
        } else if (op == 72) {
            int v;
            TRY(g2b(b, &v));
            o->offsetz = shl(v, 2);
        } else if (op == 73) {
            o->forcedecor = 1;
        } else if (op == 74) {
            o->breakroutefinding = 1;
        } else if (op == 75) {
            TRY(g1(b, &o->raiseobject));
        } else if (op == 77 || op == 92) {
            TRY(loc_multi_smart(b, o, op == 92, rev == 910));
        } else if (op == 78) {
            TRY(g2(b, &o->bgsound));
            TRY(g1(b, &o->bgsound_range));
        } else if (op == 79) {
            TRY(loc_random_sounds(b, o));
        } else if (op == 81) {
            int v;
            TRY(g1(b, &v));
            o->hillchange = 2;
            o->hillchange_value = v * 256;
        } else if (op == 82) {
            o->istexture = 1;
        } else if (op == 88) {
            o->hardshadow = 0;
        } else if (op == 89) {
            o->randomanim = 0;
        } else if (op == 91) {
            o->members = 1;
        } else if (op == 93) {
            o->hillchange = 3;
            TRY(g2(b, &o->hillchange_value));
        } else if (op == 94) {
            o->hillchange = 4;
        } else if (op == 95) {
            o->hillchange = 5;
            TRY(g2b(b, &o->hillchange_value));
        } else if (op == 97) {
            o->mapscene_rotate = 1;
        } else if (op == 98) {
            o->tboolean2 = 1;
        } else if (op == 99 || op == 100) {
            TRY(skip_g1g2(b));
        } else if (op == 101) {
            TRY(g1(b, &o->tint3));
        } else if (op == 102) {
            TRY(g2(b, &o->mapscene));
        } else if (op == 103) {
            o->occlude = 0;
        } else if (op == 104) {
            TRY(g1(b, &o->bgsound_volume));
        } else if (op == 105) {
            o->mapscene_mirror = 1;
        } else if (op == 106) {
            TRY(loc_anim_weights(b, o));
        } else if (op == 107) {
            TRY(g2(b, &o->mapelement));
        } else if (op >= 150 && op < 155) {
            TRY(read_op(b, o->op[op - 150], &o->op_set[op - 150], HIDE_NONE));
        } else if (op == 160) {
            TRY(read_quests(b, &o->quest_count, o->quests));
        } else if (op == 162) {
            o->hillchange = 3;
            TRY(g4s(b, &o->hillchange_value));
        } else if (op == 163) {
            TRY(g1b(b, &o->tint_hue));
            TRY(g1b(b, &o->tint_saturation));
            TRY(g1b(b, &o->tint_luminence));
            TRY(g1b(b, &o->tint_weight));
        } else if (op == 164) {
            TRY(g2b(b, &o->post_xoff));
        } else if (op == 165) {
            TRY(g2b(b, &o->post_yoff));
        } else if (op == 166) {
            TRY(g2b(b, &o->post_zoff));
        } else if (op == 167) {
            TRY(g2(b, &o->tint4));
        } else if (op == 168) {
            if (rev == 742) {
                o->flag168 = 1;
            }
        } else if (op == 169) {
            if (rev == 742) {
                o->flag169 = 1;
            }
        } else if (op == 170) {
            TRY(smart1or2(b, &o->tint));
        } else if (op == 171) {
            TRY(smart1or2(b, &o->tint2));
        } else if (op == 173) {
            TRY(g2(b, &o->bgsound_minrate));
            TRY(g2(b, &o->bgsound_maxrate));
        } else if (op == 177) {
            o->tboolean3 = 1;
        } else if (op == 178) {
            TRY(g1(b, &o->bgsound_size));
        } else if (op == 186) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(g1(b, &o->enum186));
        } else if (op == 188) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
        } else if (op == 189) {
            o->antimacro = 1;
        } else if (op >= 190 && op < 196) {
            cursor_init(&o->cursor_set, o->cursor, RS_CONTENT_CURSOR);
            TRY(g2(b, &o->cursor[op - 190]));
        } else if (op == 196) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(g1(b, &o->enum196));
        } else if (op == 197) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            TRY(g1(b, &o->enum197));
        } else if (op == 198 || op == 199) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
        } else if (op == 200) {
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            o->tboolean = 1;
        } else if (op == 201) {
            int i;
            if (rev != 910) {
                return RS_ERR_FORMAT;
            }
            for (i = 0; i < 6; i++) {
                TRY(smart1or2s(b, &o->clickbox[i]));
            }
            o->clickbox_set = 1;
        } else if (op == 249) {
            TRY(read_params(b, &o->param_count, o->params));
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

rs_err rs_loc_type_decode(int revision, rs_buf *src, rs_loc_type *out) {
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    if (!rev_ok(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    loc_reset(out, revision);
    if (revision == 317) {
        return loc_decode_317(src, out);
    }
    if (revision == 474) {
        return loc_decode_474(src, out);
    }
    if (revision == 530) {
        return loc_decode_530(src, out);
    }
    return loc_decode_late(src, out, revision);
}

static int cos_tab[2048];
static int cos_ready;

static void cos_init(void) {
    int i;
    if (cos_ready) {
        return;
    }
    for (i = 0; i < 2048; i++) {
        cos_tab[i] = (int)(65536.0 * cos((double)i * 0.0030679615));
    }
    cos_ready = 1;
}

static int noise(int x, int z) {
    uint32_t n = (uint32_t)x + (uint32_t)z * 57u;
    uint32_t n3 = (n << 13) ^ n;
    uint32_t n2 = n3 * (n3 * n3 * 15731u + 789221u) + 1376312589u;
    n2 &= 0x7fffffffu;
    return (int)((n2 >> 19) & 255u);
}

static int smooth_noise(int x, int z) {
    int corners = noise(x - 1, z - 1) + noise(x + 1, z - 1) + noise(x - 1, z + 1) + noise(x + 1, z + 1);
    int sides = noise(x - 1, z) + noise(x + 1, z) + noise(x, z - 1) + noise(x, z + 1);
    int centre = noise(x, z);
    return corners / 16 + sides / 8 + centre / 4;
}

static int interpolate(int a, int b, int frac, int scale) {
    int c = cos_tab[frac * 1024 / scale];
    int t = (65536 - c) >> 1;
    return ((a * (65536 - t)) >> 16) + ((b * t) >> 16);
}

static int perlin3(int x, int z, int scale) {
    int x0 = x / scale;
    int xf = x & (scale - 1);
    int z0 = z / scale;
    int zf = z & (scale - 1);
    int n00 = smooth_noise(x0, z0);
    int n10 = smooth_noise(x0 + 1, z0);
    int n01 = smooth_noise(x0, z0 + 1);
    int n11 = smooth_noise(x0 + 1, z0 + 1);
    int i0 = interpolate(n00, n10, xf, scale);
    int i1 = interpolate(n01, n11, xf, scale);
    return interpolate(i0, i1, zf, scale);
}

static int perlin2(int x, int z) {
    int v = perlin3(x + 45365, z + 91923, 4) - 128 + ((perlin3(x + 10294, z + 37821, 2) - 128) >> 1) +
            ((perlin3(x, z, 1) - 128) >> 2);
    int h = (int)((double)v * 0.3) + 35;
    if (h < 10) {
        h = 10;
    } else if (h > 60) {
        h = 60;
    }
    return h;
}

typedef void (*col_fn)(rs_land *land, int plane, int x, int z, int bit);

static void col_add(rs_land *land, int plane, int x, int z, int bit) {
    if (!land || plane < 0 || plane >= RS_LAND_PLANES) {
        return;
    }
    if (x < 0 || z < 0 || x >= RS_LAND_SIZE || z >= RS_LAND_SIZE) {
        return;
    }
    land->collision[plane][x][z] |= bit;
}

/* CollisionMap.remove: flags &= 16777215 - bit. This square never sets bit 0x1000000. */
static void col_clear(rs_land *land, int plane, int x, int z, int bit) {
    if (!land || plane < 0 || plane >= RS_LAND_PLANES) {
        return;
    }
    if (x < 0 || z < 0 || x >= RS_LAND_SIZE || z >= RS_LAND_SIZE) {
        return;
    }
    land->collision[plane][x][z] &= ~bit;
}

static void col_block(rs_land *land, int plane, int x, int z) {
    col_add(land, plane, x, z, 2097152);
}

static void col_loc(rs_land *land, int plane, int x, int z, int width, int length, int rot, int blockrange, col_fn put) {
    int flag = blockrange ? 131328 : 256;
    int i;
    int j;
    if (rot == 1 || rot == 3) {
        int tmp = width;
        width = length;
        length = tmp;
    }
    for (i = x; i < x + width; i++) {
        for (j = z; j < z + length; j++) {
            put(land, plane, i, j, flag);
        }
    }
}

/* CollisionMap.addWall / removeWall. Neighbours outside 0..63 are skipped. */
static void col_wall(rs_land *land, int plane, int x, int z, int type, int rot, int blockrange, col_fn put) {
    if (type == 0) {
        if (rot == 0) {
            put(land, plane, x, z, 128);
            put(land, plane, x - 1, z, 8);
        } else if (rot == 1) {
            put(land, plane, x, z, 2);
            put(land, plane, x, z + 1, 32);
        } else if (rot == 2) {
            put(land, plane, x, z, 8);
            put(land, plane, x + 1, z, 128);
        } else if (rot == 3) {
            put(land, plane, x, z, 32);
            put(land, plane, x, z - 1, 2);
        }
    }
    if (type == 1 || type == 3) {
        if (rot == 0) {
            put(land, plane, x, z, 1);
            put(land, plane, x - 1, z + 1, 16);
        } else if (rot == 1) {
            put(land, plane, x, z, 4);
            put(land, plane, x + 1, z + 1, 64);
        } else if (rot == 2) {
            put(land, plane, x, z, 16);
            put(land, plane, x + 1, z - 1, 1);
        } else if (rot == 3) {
            put(land, plane, x, z, 64);
            put(land, plane, x - 1, z - 1, 4);
        }
    }
    if (type == 2) {
        if (rot == 0) {
            put(land, plane, x, z, 130);
            put(land, plane, x - 1, z, 8);
            put(land, plane, x, z + 1, 32);
        } else if (rot == 1) {
            put(land, plane, x, z, 10);
            put(land, plane, x, z + 1, 32);
            put(land, plane, x + 1, z, 128);
        } else if (rot == 2) {
            put(land, plane, x, z, 40);
            put(land, plane, x + 1, z, 128);
            put(land, plane, x, z - 1, 2);
        } else if (rot == 3) {
            put(land, plane, x, z, 160);
            put(land, plane, x, z - 1, 2);
            put(land, plane, x - 1, z, 8);
        }
    }
    if (!blockrange) {
        return;
    }
    if (type == 0) {
        if (rot == 0) {
            put(land, plane, x, z, 65536);
            put(land, plane, x - 1, z, 4096);
        } else if (rot == 1) {
            put(land, plane, x, z, 1024);
            put(land, plane, x, z + 1, 16384);
        } else if (rot == 2) {
            put(land, plane, x, z, 4096);
            put(land, plane, x + 1, z, 65536);
        } else if (rot == 3) {
            put(land, plane, x, z, 16384);
            put(land, plane, x, z - 1, 1024);
        }
    }
    if (type == 1 || type == 3) {
        if (rot == 0) {
            put(land, plane, x, z, 512);
            put(land, plane, x - 1, z + 1, 8192);
        } else if (rot == 1) {
            put(land, plane, x, z, 2048);
            put(land, plane, x + 1, z + 1, 32768);
        } else if (rot == 2) {
            put(land, plane, x, z, 8192);
            put(land, plane, x + 1, z - 1, 512);
        } else if (rot == 3) {
            put(land, plane, x, z, 32768);
            put(land, plane, x - 1, z - 1, 2048);
        }
    }
    if (type == 2) {
        if (rot == 0) {
            put(land, plane, x, z, 66560);
            put(land, plane, x - 1, z, 4096);
            put(land, plane, x, z + 1, 16384);
        } else if (rot == 1) {
            put(land, plane, x, z, 5120);
            put(land, plane, x, z + 1, 16384);
            put(land, plane, x + 1, z, 65536);
        } else if (rot == 2) {
            put(land, plane, x, z, 20480);
            put(land, plane, x + 1, z, 65536);
            put(land, plane, x, z - 1, 1024);
        } else if (rot == 3) {
            put(land, plane, x, z, 81920);
            put(land, plane, x, z - 1, 1024);
            put(land, plane, x - 1, z, 4096);
        }
    }
}

static void apply_loc_collision(rs_land *land, int plane, int x, int z, int shape, int rot,
                                const rs_land_loc_type *ty, int present) {
    col_fn op = present ? col_add : col_clear;
    if (!ty || !ty->blockwalk) {
        return;
    }
    if (shape == 22) {
        if (ty->active) {
            op(land, plane, x, z, 2097152);
        }
        return;
    }
    if (shape == 10 || shape == 11 || shape == 9 || shape >= 12) {
        col_loc(land, plane, x, z, ty->width, ty->length, rot, ty->blockrange, op);
        return;
    }
    if (shape >= 0 && shape <= 3) {
        col_wall(land, plane, x, z, shape, rot, ty->blockrange, op);
    }
}

rs_err rs_land_collision_add(rs_land *land, int revision, int plane, int x, int z, int shape, int rot,
                             const rs_land_loc_type *ty) {
    if (!land || !ty) {
        return RS_ERR_ARG;
    }
    if (revision != 317) {
        return RS_ERR_UNSUPPORTED;
    }
    if (plane < 0 || plane >= RS_LAND_PLANES || x < 0 || z < 0 || x >= RS_LAND_SIZE || z >= RS_LAND_SIZE) {
        return RS_ERR_ARG;
    }
    /* Client.addLoc skips the 104-map border. Square-local coordinates use the same test. */
    if (x <= 0 || z <= 0 || x >= 103 || z >= 103) {
        return RS_OK;
    }
    apply_loc_collision(land, plane, x, z, shape, rot, ty, 1);
    return RS_OK;
}

rs_err rs_land_collision_remove(rs_land *land, int revision, int plane, int x, int z, int shape, int rot,
                                const rs_land_loc_type *ty) {
    if (!land || !ty) {
        return RS_ERR_ARG;
    }
    if (revision != 317) {
        return RS_ERR_UNSUPPORTED;
    }
    if (plane < 0 || plane >= RS_LAND_PLANES || x < 0 || z < 0 || x >= RS_LAND_SIZE || z >= RS_LAND_SIZE) {
        return RS_ERR_ARG;
    }
    if (x <= 0 || z <= 0 || x >= 103 || z >= 103) {
        return RS_OK;
    }
    apply_loc_collision(land, plane, x, z, shape, rot, ty, 0);
    return RS_OK;
}

/*
 * Classic tile loop, rotation 0. scale 8 / step 240 through 578.
 * scale 32 / step 960 from 614 (height * 8 << 2, upper level - 960).
 * Cited from ClientBuild.loadGround, World.readLandInner, HuffmanCodec.opHuffmanCodec2,
 * ClientOptionsHardwareInfo.opClientOptionsHardwareInfo9, and MapLoader.decodeLand.
 */
static rs_err read_land_tile(rs_buf *b, rs_land *out, int level, int x, int z, int origin_x,
                              int origin_z, int scale, int step) {
    out->flags[level][x][z] = 0;
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            if (level == 0) {
                out->height[0][x][z] = -perlin2(932731 + x + origin_x, 556238 + z + origin_z) * scale;
            } else {
                out->height[level][x][z] = out->height[level - 1][x][z] - step;
            }
            return RS_OK;
        }
        if (op == 1) {
            int h;
            TRY(g1(b, &h));
            if (h == 1) {
                h = 0;
            }
            if (level == 0) {
                out->height[0][x][z] = -h * scale;
            } else {
                out->height[level][x][z] = out->height[level - 1][x][z] - h * scale;
            }
            return RS_OK;
        }
        if (op <= 49) {
            int overlay;
            TRY(g1b(b, &overlay));
            out->overlay[level][x][z] = overlay;
            out->overlay_shape[level][x][z] = (op - 2) / 4;
            out->overlay_rot[level][x][z] = (op - 2) & 3;
        } else if (op <= 81) {
            out->flags[level][x][z] = op - 49;
        } else {
            out->underlay[level][x][z] = (int)(int8_t)(op - 81);
        }
    }
}

/* MapLoader.readLandscape / opkind344214, underwater false, square rotation 0. */
static rs_err read_land_bits(rs_buf *b, rs_land *out, int level, int x, int z, int origin_x,
                             int origin_z, int scale, int step) {
    int hdr;
    out->flags[level][x][z] = 0;
    TRY(g1(b, &hdr));
    if ((hdr & 1) != 0) {
        int packed;
        int id;
        TRY(g1(b, &packed));
        TRY(smarts(b, &id));
        out->overlay[level][x][z] = id;
        out->overlay_shape[level][x][z] = packed >> 2;
        out->overlay_rot[level][x][z] = packed & 3;
    }
    if ((hdr & 2) != 0) {
        int flags;
        TRY(g1b(b, &flags));
        out->flags[level][x][z] = flags;
    }
    if ((hdr & 4) != 0) {
        int id;
        TRY(smarts(b, &id));
        out->underlay[level][x][z] = id;
    }
    if ((hdr & 8) != 0) {
        int h;
        TRY(g1(b, &h));
        if (h == 1) {
            h = 0;
        }
        if (level == 0) {
            out->height[0][x][z] = -h * scale;
        } else {
            out->height[level][x][z] = out->height[level - 1][x][z] - h * scale;
        }
    } else if (level == 0) {
        out->height[0][x][z] = -perlin2(932731 + x + origin_x, 556238 + z + origin_z) * scale;
    } else {
        out->height[level][x][z] = out->height[level - 1][x][z] - step;
    }
    return RS_OK;
}

/* finishBuild / build / opMapLoader18 / opkind34428: flag bit 1 blocks, plane-1 flag bit 2 bridges. */
static void stamp_flag_collision(rs_land *out) {
    int level;
    int x;
    int z;
    for (level = 0; level < RS_LAND_PLANES; level++) {
        for (x = 0; x < RS_LAND_SIZE; x++) {
            for (z = 0; z < RS_LAND_SIZE; z++) {
                if ((out->flags[level][x][z] & 1) == 1) {
                    int plane = level;
                    if ((out->flags[1][x][z] & 2) == 2) {
                        plane = level - 1;
                    }
                    if (plane >= 0) {
                        col_block(out, plane, x, z);
                    }
                }
            }
        }
    }
}

/* 0 unread. 1 classic loop. 2 one-byte flag header. stamp is the flag-bit collision pass. */
static int land_layout(int revision, int *scale, int *step, int *bits, int *stamp) {
    *scale = 8;
    *step = 240;
    *bits = 0;
    *stamp = 1;
    switch (revision) {
    case 254:
    case 282:
    case 317:
    case 350:
    case 377:
    case 443:
    case 468:
    case 474:
    case 481:
    case 508:
    case 525:
    case 530:
    case 578:
        return 1;
    case 614:
    case 634:
    case 667:
    case 742:
        *scale = 32;
        *step = 960;
        return 1;
    case 745:
        *scale = 32;
        *step = 960;
        *bits = 1;
        return 1;
    case 790:
    case 876:
    case 887:
    case 902:
    case 910:
        *scale = 32;
        *step = 960;
        *bits = 1;
        *stamp = 0;
        return 1;
    default:
        return 0;
    }
}

rs_err rs_land_decode(int revision, const uint8_t *data, size_t len, rs_land *out) {
    return rs_land_decode_at(revision, data, len, 0, 0, out);
}

rs_err rs_land_decode_at(int revision, const uint8_t *data, size_t len, int origin_x, int origin_z,
                         rs_land *out) {
    rs_buf buf;
    int level;
    int x;
    int z;
    int scale;
    int step;
    int bits;
    int stamp;
    if (!data || !out) {
        return RS_ERR_ARG;
    }
    if (!land_layout(revision, &scale, &step, &bits, &stamp)) {
        return RS_ERR_UNSUPPORTED;
    }
    if (len == 0) {
        return RS_ERR_FORMAT;
    }
    memset(out, 0, sizeof(*out));
    cos_init();
    rs_buf_wrap(&buf, (uint8_t *)data, len);
    /*
     * Rotation is 0. The square starts clear. origin is the world tile of (0,0),
     * added only to perlin heights. 695 has no transcribed reader. 831 and 860
     * have no sources.
     */
    for (level = 0; level < RS_LAND_PLANES; level++) {
        for (x = 0; x < RS_LAND_SIZE; x++) {
            for (z = 0; z < RS_LAND_SIZE; z++) {
                if (bits) {
                    TRY(read_land_bits(&buf, out, level, x, z, origin_x, origin_z, scale, step));
                } else {
                    TRY(read_land_tile(&buf, out, level, x, z, origin_x, origin_z, scale, step));
                }
            }
        }
    }
    if (stamp) {
        stamp_flag_collision(out);
    }
    return RS_OK;
}

rs_err rs_land_locs_decode(int revision, const uint8_t *data, size_t len, rs_land *land,
                           const rs_land_loc_type *types, int ntypes, rs_land_loc *out, int cap,
                           int *count) {
    rs_buf buf;
    int id = -1;
    if (!data || !count || cap < 0 || (cap > 0 && !out)) {
        return RS_ERR_ARG;
    }
    if (types && ntypes < 0) {
        return RS_ERR_ARG;
    }
    if (revision != 317) {
        return RS_ERR_UNSUPPORTED;
    }
    if (len == 0) {
        return RS_ERR_FORMAT;
    }
    *count = 0;
    rs_buf_wrap(&buf, (uint8_t *)data, len);
    /*
     * readLoc accumulates the inner gsmarts (pos += delta - 1). rs_map_parse_locs
     * stores each inner smart as an absolute position and then decrements it.
     * World.lowMemory (field default true) plus levelBuilt can skip addLoc before
     * collision when flags[0] lacks bit 2. This decoder still stamps collision.
     */
    for (;;) {
        int delta;
        int pos = 0;
        TRY(smarts(&buf, &delta));
        if (delta == 0) {
            return RS_OK;
        }
        id = (int)((uint32_t)id + (uint32_t)delta);
        for (;;) {
            int step;
            int info;
            int x;
            int z;
            int level;
            int plane;
            rs_land_loc *slot;
            TRY(smarts(&buf, &step));
            if (step == 0) {
                break;
            }
            pos = (int)((uint32_t)pos + (uint32_t)step - 1u);
            z = pos & 63;
            x = (pos >> 6) & 63;
            level = pos >> 12;
            TRY(g1(&buf, &info));
            if (*count >= cap) {
                return RS_ERR_RANGE;
            }
            slot = &out[*count];
            slot->id = id;
            slot->x = x;
            slot->z = z;
            slot->level = level;
            slot->shape = info >> 2;
            slot->rot = info & 3;
            plane = level;
            if (land && (land->flags[1][x][z] & 2) == 2) {
                plane = level - 1;
            }
            slot->plane = plane;
            (*count)++;
            if (land && types && id >= 0 && id < ntypes && x > 0 && z > 0 && x < 103 && z < 103 &&
                plane >= 0 && plane < RS_LAND_PLANES) {
                apply_loc_collision(land, plane, x, z, slot->shape, slot->rot, &types[id], 1);
            }
        }
    }
}

static int flo_hsl24to16(int hue, int sat, int light) {
    if (light > 179) {
        sat /= 2;
    }
    if (light > 192) {
        sat /= 2;
    }
    if (light > 217) {
        sat /= 2;
    }
    if (light > 243) {
        sat /= 2;
    }
    return ((hue / 4) << 10) + ((sat / 32) << 7) + light / 2;
}

/* FloType.setColour. Random jitter on the hsl channels is left at 0. */
static void flo_set_colour(rs_flo_type *t, int rgb) {
    double r = (double)((rgb >> 16) & 255) / 256.0;
    double g = (double)((rgb >> 8) & 255) / 256.0;
    double b = (double)(rgb & 255) / 256.0;
    double mn = r;
    double mx = r;
    double hue = 0.0;
    double sat = 0.0;
    double light;
    double h6;

    if (g < mn) {
        mn = g;
    }
    if (b < mn) {
        mn = b;
    }
    if (g > mx) {
        mx = g;
    }
    if (b > mx) {
        mx = b;
    }
    light = (mn + mx) / 2.0;
    if (mn != mx) {
        if (light < 0.5) {
            sat = (mx - mn) / (mx + mn);
        }
        if (light >= 0.5) {
            sat = (mx - mn) / (2.0 - mx - mn);
        }
        if (r == mx) {
            hue = (g - b) / (mx - mn);
        } else if (g == mx) {
            hue = 2.0 + (b - r) / (mx - mn);
        } else if (b == mx) {
            hue = 4.0 + (r - g) / (mx - mn);
        }
    }
    h6 = hue / 6.0;
    t->rgb = rgb;
    t->hue = (int)(h6 * 256.0);
    t->saturation = (int)(sat * 256.0);
    t->lightness = (int)(light * 256.0);
    if (t->saturation < 0) {
        t->saturation = 0;
    } else if (t->saturation > 255) {
        t->saturation = 255;
    }
    if (t->lightness < 0) {
        t->lightness = 0;
    } else if (t->lightness > 255) {
        t->lightness = 255;
    }
    if (light > 0.5) {
        t->luminance = (int)((1.0 - light) * sat * 512.0);
    } else {
        t->luminance = (int)(light * sat * 512.0);
    }
    if (t->luminance < 1) {
        t->luminance = 1;
    }
    t->chroma = (int)(h6 * (double)t->luminance);
    t->hsl = flo_hsl24to16(t->hue, t->saturation, t->lightness);
}

static rs_err flo_one(rs_buf *b, rs_flo_type *t, int opcode7) {
    memset(t, 0, sizeof(*t));
    t->texture = -1;
    t->occlude = 1;
    for (;;) {
        int op;
        TRY(g1(b, &op));
        if (op == 0) {
            return RS_OK;
        }
        if (op == 1) {
            int rgb;
            TRY(g3(b, &rgb));
            flo_set_colour(t, rgb);
        } else if (op == 2) {
            int tex;
            TRY(g1(b, &tex));
            t->texture = tex;
        } else if (op == 3) {
            t->overlay = 1;
        } else if (op == 5) {
            t->occlude = 0;
        } else if (op == 6) {
            char name[256];
            TRY(rs_gjstr(b, name, sizeof name));
        } else if (op == 7) {
            int rgb;
            int hue = t->hue;
            int sat = t->saturation;
            int light = t->lightness;
            int chroma = t->chroma;
            if (!opcode7) {
                return RS_ERR_FORMAT;
            }
            TRY(g3(b, &rgb));
            flo_set_colour(t, rgb);
            t->hue = hue;
            t->saturation = sat;
            t->lightness = light;
            t->chroma = chroma;
            t->luminance = chroma;
        } else {
            return RS_ERR_FORMAT;
        }
    }
}

/* 254 and 282 FloType.decodeClient stop at opcode 6. 317, 350, and 377 read opcode 7. */
static int flo_opcode7(int revision) {
    return revision == 317 || revision == 350 || revision == 377;
}

static int flo_rev(int revision) {
    return revision == 254 || revision == 282 || flo_opcode7(revision);
}

rs_err rs_flo_decode(int revision, const uint8_t *data, size_t len, rs_flo_type *out, int cap, int *count) {
    rs_buf buf;
    int n;
    int i;
    if (!data || !count || cap < 0 || (cap > 0 && !out)) {
        return RS_ERR_ARG;
    }
    if (!flo_rev(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    if (len < 2) {
        return RS_ERR_FORMAT;
    }
    rs_buf_wrap(&buf, (uint8_t *)data, len);
    TRY(g2(&buf, &n));
    if (n < 0 || n > cap) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < n; i++) {
        TRY(flo_one(&buf, &out[i], flo_opcode7(revision)));
    }
    *count = n;
    return RS_OK;
}

/*
 * Underlay opcode 1 is g3. 443 ByteArrayNodeSecondary.opByteArrayNodeSecondary9
 * and 468 WorldInfo.method291 have no texture opcode. 474
 * Js5QuickChatCommandDecoder.opJs5QuickChatCommandDecoder23 adds opcode 2 g2
 * (65535 = -1). 481 and later opcode 3 discards a g2 scale. 634
 * C4fbb1a43.opkind11211 opcode 5 clears occlude; 578 and 614 leave it.
 * Post setColour of the colour field (default 0): 443 opByteArrayNodeSecondary12,
 * 468 WorldInfo.method292, 474 opJs5QuickChatCommandDecoder27. Later clients
 * convert inside opcode 1 (742 FloorUnderlayType.opFloorUnderlayType14,
 * 910 FloorUnderlayType.computeColour).
 *
 * Overlay opcode 2 is g1. 443 HierKlickenUmFortzufahren.opkind1271 and 468
 * CursorType.opCursorType6 have no opcode 3. 474 CursorManager.call3aa88bf2
 * opcode 3 is g2 texture. 481 HardReferenceNode.opHardReferenceNode16 opcode 9
 * discards g2. 530 Packet.opPacket57 discards through opcode 14. 578
 * C3c9f52c8.opkind10221 and 614 Unused9.opUnused93 add opcode 15 g2 and 16 g1.
 * 634 Cc8c4eca9.opkind14023 and 667 ScriptFrame.opScriptFrame7 keep 16 and do
 * not read opcode 15. 742 FloorOverlayType.opFloorOverlayType2 and 910
 * FloorOverlayType.decode add opcodes 20 g2, 21 g1, and 22 g2.
 * Scale, water, tint, and priority are consumed at those widths and not stored.
 * An unknown opcode reads nothing and the loop continues.
 */
enum {
    LAY_NONE = 0,
    LAY_U443,
    LAY_U474,
    LAY_U481,
    LAY_U578,
    LAY_U634,
    LAY_O443,
    LAY_O474,
    LAY_O481,
    LAY_O530,
    LAY_O614,
    LAY_O634,
    LAY_O742
};

static int flo_js5_layout(int revision, int kind) {
    if (kind == RS_FLO_UNDERLAY) {
        if (revision == 443 || revision == 468) {
            return LAY_U443;
        }
        if (revision == 474) {
            return LAY_U474;
        }
        if (revision == 481 || revision == 508 || revision == 525 || revision == 530) {
            return LAY_U481;
        }
        if (revision == 578 || revision == 614) {
            return LAY_U578;
        }
        if (revision == 634 || revision == 667 || revision == 742 || revision == 745 || revision == 790 ||
            revision == 876 || revision == 887 || revision == 902 || revision == 910) {
            return LAY_U634;
        }
    } else if (kind == RS_FLO_OVERLAY) {
        if (revision == 443 || revision == 468) {
            return LAY_O443;
        }
        if (revision == 474) {
            return LAY_O474;
        }
        if (revision == 481) {
            return LAY_O481;
        }
        if (revision == 508 || revision == 525 || revision == 530) {
            return LAY_O530;
        }
        if (revision == 578 || revision == 614) {
            return LAY_O614;
        }
        if (revision == 634 || revision == 667) {
            return LAY_O634;
        }
        if (revision == 742 || revision == 745 || revision == 790 || revision == 876 || revision == 887 ||
            revision == 902 || revision == 910) {
            return LAY_O742;
        }
    }
    return LAY_NONE;
}

static int flo_js5_post(int lay) {
    return lay == LAY_U443 || lay == LAY_U474 || lay == LAY_O443 || lay == LAY_O474;
}

static rs_err flo_discard(rs_buf *b, int n) {
    int v;
    if (n == 1) {
        return g1(b, &v);
    }
    if (n == 2) {
        return g2(b, &v);
    }
    return g3(b, &v);
}

static rs_err flo_tex1(rs_buf *b, rs_flo_type *t) {
    int v;
    TRY(g1(b, &v));
    t->texture = v;
    return RS_OK;
}

static rs_err flo_tex2(rs_buf *b, rs_flo_type *t) {
    int v;
    TRY(g2(b, &v));
    t->texture = m1_u16(v);
    return RS_OK;
}

static int flo_over_late(int lay) {
    return lay == LAY_O530 || lay == LAY_O614 || lay == LAY_O634 || lay == LAY_O742;
}

static rs_err flo_js5_op(rs_buf *b, rs_flo_type *t, int lay, int op, int *saw_colour) {
    int over = lay >= LAY_O443;
    if (op == 1) {
        TRY(g3(b, &t->rgb));
        *saw_colour = 1;
        return RS_OK;
    }
    if (op == 2) {
        if (over) {
            return flo_tex1(b, t);
        }
        if (lay != LAY_U443) {
            return flo_tex2(b, t);
        }
        return RS_OK;
    }
    if (op == 3) {
        if (!over && (lay == LAY_U481 || lay == LAY_U578 || lay == LAY_U634)) {
            return flo_discard(b, 2);
        }
        if (over && lay != LAY_O443) {
            return flo_tex2(b, t);
        }
        return RS_OK;
    }
    if (op == 5) {
        if (lay == LAY_U634 || over) {
            t->occlude = 0;
        }
        return RS_OK;
    }
    if (op == 7 && over) {
        return flo_discard(b, 3);
    }
    if (op == 9 && over && lay != LAY_O443 && lay != LAY_O474) {
        return flo_discard(b, 2);
    }
    if (op == 11 && flo_over_late(lay)) {
        return flo_discard(b, 1);
    }
    if (op == 13 && flo_over_late(lay)) {
        return flo_discard(b, 3);
    }
    if (op == 14 && flo_over_late(lay)) {
        return flo_discard(b, 1);
    }
    if (op == 15 && lay == LAY_O614) {
        return flo_discard(b, 2);
    }
    if (op == 16 && (lay == LAY_O614 || lay == LAY_O634 || lay == LAY_O742)) {
        return flo_discard(b, 1);
    }
    if ((op == 20 || op == 22) && lay == LAY_O742) {
        return flo_discard(b, 2);
    }
    if (op == 21 && lay == LAY_O742) {
        return flo_discard(b, 1);
    }
    return RS_OK;
}

rs_err rs_flo_js5_decode(int revision, int kind, const uint8_t *data, size_t len, rs_flo_type *out) {
    rs_buf buf;
    int lay;
    int saw = 0;
    if (!data || !out || (kind != RS_FLO_UNDERLAY && kind != RS_FLO_OVERLAY)) {
        return RS_ERR_ARG;
    }
    lay = flo_js5_layout(revision, kind);
    if (lay == LAY_NONE) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(out, 0, sizeof(*out));
    out->texture = -1;
    out->occlude = 1;
    rs_buf_wrap(&buf, (uint8_t *)data, len);
    for (;;) {
        int op;
        TRY(g1(&buf, &op));
        if (op == 0) {
            break;
        }
        TRY(flo_js5_op(&buf, out, lay, op, &saw));
    }
    if (flo_js5_post(lay) || saw) {
        flo_set_colour(out, out->rgb);
    }
    return RS_OK;
}
