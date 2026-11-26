#include "internal.h"

#include "rslib/both/era_body.h"

#define W(expr)                         \
    do {                                \
        rs_err _e = (expr);             \
        if (_e) {                       \
            return _e;                  \
        }                               \
    } while (0)

static rs_err args2(const void *a, const void *b) {
    if (!a || !b) {
        return RS_ERR_ARG;
    }
    return RS_OK;
}

static int zone_off(int x, int z) {
    return ((x & 7) << 4) | (z & 7);
}

static int split_off(int off, int *x, int *z) {
    *x = (off >> 4) & 7;
    *z = off & 7;
    return off;
}

static int g1b_add128(rs_buf *b) {
    int v = rs_g1_add128(b);
    if (v > 127) {
        v -= 256;
    }
    return v;
}

static int g1b_sub128(rs_buf *b) {
    int v = rs_g1_sub128(b);
    if (v > 127) {
        v -= 256;
    }
    return v;
}

static int pack_shape(int shape, int rot) {
    return ((shape & 31) << 2) | (rot & 3);
}

static void unpack_shape(int packed, int *shape, int *rot) {
    *shape = packed >> 2;
    *rot = packed & 3;
}

/* Plain g1, or 255 plus a 4-byte amount. kind 0 is g4, 1 is g4_alt3, 2 is LE. */
static rs_err put_amt(rs_buf *dst, int amt, int kind) {
    if (amt < 0) {
        return RS_ERR_ARG;
    }
    if (amt >= 255) {
        W(rs_p1(dst, 255));
        if (kind == 1) {
            return rs_p4_alt3(dst, amt);
        }
        if (kind == 2) {
            return rs_ip4(dst, amt);
        }
        return rs_p4(dst, amt);
    }
    return rs_p1(dst, amt);
}

static int get_amt(rs_buf *src, int kind) {
    int amt = rs_g1(src);
    if (amt != 255) {
        return amt;
    }
    if (kind == 1) {
        return rs_g4_alt3(src);
    }
    if (kind == 2) {
        return rs_ig4(src);
    }
    return rs_g4(src);
}

static int smart_len(const rs_buf *b) {
    if (rs_buf_remaining(b) < 1) {
        return 0;
    }
    return (b->data[b->pos] & 0xff) < 128 ? 1 : 2;
}

static int late_inv(int revision) {
    return revision == 742 || revision == 910;
}

/* 742 and 910 subtract one from the wire id before storing it. */
static int from_wire_id(int wire) {
    return wire - 1;
}

static rs_err flags_ok(int revision, int flags) {
    if (late_inv(revision)) {
        if (flags < 0 || flags > 255) {
            return RS_ERR_ARG;
        }
        /* 910 bit 1 selects a var-container the reader decodes next. */
        if (revision == 910 && (flags & 2)) {
            return RS_ERR_UNSUPPORTED;
        }
        return RS_OK;
    }
    /* JavaMouseWheel.call72b59b7f opcodes 119 and 187 store a container key. */
    if (revision == 474) {
        if (flags < 0 || flags > 65535) {
            return RS_ERR_ARG;
        }
        return RS_OK;
    }
    if (flags != 0) {
        return RS_ERR_ARG;
    }
    return RS_OK;
}

static rs_err slot_count_ok(int revision, int n) {
    int cap = revision == 254 ? 255 : 65535;
    if (n < 0 || n > RS_ERA_INV_SLOTS || n > cap) {
        return RS_ERR_ARG;
    }
    return RS_OK;
}

/* ---- skill ---- */

rs_err rs_era_skill_encode(int revision, rs_buf *dst, const rs_era_skill *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p1(dst, v->id));
        W(rs_p4_alt2(dst, v->xp));
        return rs_p1(dst, v->level);
    }
    if (revision == 254) {
        W(rs_p1(dst, v->id));
        W(rs_p4(dst, v->xp));
        return rs_p1(dst, v->level);
    }
    if (revision == 350) {
        W(rs_ip4(dst, v->xp));
        W(rs_p1_add128(dst, v->level));
        return rs_p1_sub128(dst, v->id);
    }
    if (revision == 377) {
        W(rs_p1_neg(dst, v->id));
        W(rs_p1(dst, v->level));
        return rs_p4(dst, v->xp);
    }
    if (revision == 508) {
        W(rs_p1_neg(dst, v->level));
        W(rs_p4_alt3(dst, v->xp));
        return rs_p1_neg(dst, v->id);
    }
    if (revision == 742) {
        W(rs_p1_add128(dst, v->id));
        W(rs_p1(dst, v->level));
        return rs_p4_alt3(dst, v->xp);
    }
    if (revision == 910) {
        W(rs_p1_neg(dst, v->level));
        W(rs_ip4(dst, v->xp));
        return rs_p1_sub128(dst, v->id);
    }
    /* JavaMouseWheel.call72b59b7f opcode 196: op67, op59, op67. */
    if (revision == 474) {
        W(rs_p1(dst, v->id));
        W(rs_p4_alt3(dst, v->xp));
        return rs_p1(dst, v->level);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_skill_decode(int revision, rs_buf *src, rs_era_skill *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->id = rs_g1(src);
        v->xp = rs_g4_alt2(src);
        v->level = rs_g1(src);
        return RS_OK;
    }
    if (revision == 254) {
        v->id = rs_g1(src);
        v->xp = rs_g4(src);
        v->level = rs_g1(src);
        return RS_OK;
    }
    if (revision == 350) {
        v->xp = rs_ig4(src);
        v->level = rs_g1_add128(src);
        v->id = rs_g1_sub128(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->id = rs_g1_neg(src);
        v->level = rs_g1(src);
        v->xp = rs_g4(src);
        return RS_OK;
    }
    if (revision == 508) {
        v->level = rs_g1_neg(src);
        v->xp = rs_g4_alt3(src);
        v->id = rs_g1_neg(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->id = rs_g1_add128(src);
        v->level = rs_g1(src);
        v->xp = rs_g4_alt3(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->level = rs_g1_neg(src);
        v->xp = rs_ig4(src);
        v->id = rs_g1_sub128(src);
        return RS_OK;
    }
    /* JavaMouseWheel.call72b59b7f opcode 196: op67, op59, op67. */
    if (revision == 474) {
        v->id = rs_g1(src);
        v->xp = rs_g4_alt3(src);
        v->level = rs_g1(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

/* ---- varp ---- */

rs_err rs_era_varp_small_encode(int revision, rs_buf *dst, const rs_era_varp *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_le(dst, v->id));
        return rs_p1(dst, v->value);
    }
    if (revision == 254) {
        W(rs_p2(dst, v->id));
        return rs_p1(dst, v->value);
    }
    if (revision == 350) {
        W(rs_p1(dst, v->value));
        return rs_p2(dst, v->id);
    }
    if (revision == 377) {
        W(rs_p2_add128(dst, v->id));
        return rs_p1_sub128(dst, v->value);
    }
    if (revision == 508) {
        W(rs_p2_add128(dst, v->id));
        return rs_p1_add128(dst, v->value);
    }
    if (revision == 742) {
        W(rs_p1_sub128(dst, v->value));
        return rs_p2_le(dst, v->id);
    }
    if (revision == 910) {
        W(rs_p1(dst, v->value));
        return rs_p2_add128(dst, v->id);
    }
    /* JavaMouseWheel.call72b59b7f opcode 253: op69 id, op31 signed (byte-128). */
    if (revision == 474) {
        W(rs_p2(dst, v->id));
        return rs_p1_add128(dst, v->value);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_varp_small_decode(int revision, rs_buf *src, rs_era_varp *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->id = rs_g2_le(src);
        v->value = rs_g1b(src);
        return RS_OK;
    }
    if (revision == 254) {
        v->id = rs_g2(src);
        v->value = rs_g1b(src);
        return RS_OK;
    }
    if (revision == 350) {
        v->value = rs_g1b(src);
        v->id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->id = rs_g2_add128(src);
        v->value = g1b_sub128(src);
        return RS_OK;
    }
    if (revision == 508) {
        v->id = rs_g2_add128(src);
        v->value = g1b_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->value = g1b_sub128(src);
        v->id = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->value = rs_g1b(src);
        v->id = rs_g2_add128(src);
        return RS_OK;
    }
    /* JavaMouseWheel.call72b59b7f opcode 253: op69 id, op31 signed (byte-128). */
    if (revision == 474) {
        v->id = rs_g2(src);
        v->value = g1b_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_varp_large_encode(int revision, rs_buf *dst, const rs_era_varp *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_le(dst, v->id));
        return rs_p4_alt2(dst, v->value);
    }
    if (revision == 254) {
        W(rs_p2(dst, v->id));
        return rs_p4(dst, v->value);
    }
    if (revision == 350) {
        W(rs_p2(dst, v->id));
        return rs_p4_alt3(dst, v->value);
    }
    if (revision == 377) {
        W(rs_p4_alt3(dst, v->value));
        return rs_p2_le(dst, v->id);
    }
    if (revision == 508) {
        W(rs_p2(dst, v->id));
        return rs_p4_alt2(dst, v->value);
    }
    if (revision == 742) {
        W(rs_ip4(dst, v->value));
        return rs_p2_add128(dst, v->id);
    }
    if (revision == 910) {
        W(rs_p2_le(dst, v->id));
        return rs_p4(dst, v->value);
    }
    /* JavaMouseWheel.call72b59b7f opcode 10: op68 value, op69 id. */
    if (revision == 474) {
        W(rs_ip4(dst, v->value));
        return rs_p2(dst, v->id);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_varp_large_decode(int revision, rs_buf *src, rs_era_varp *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->id = rs_g2_le(src);
        v->value = rs_g4_alt2(src);
        return RS_OK;
    }
    if (revision == 254) {
        v->id = rs_g2(src);
        v->value = rs_g4(src);
        return RS_OK;
    }
    if (revision == 350) {
        v->id = rs_g2(src);
        v->value = rs_g4_alt3(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->value = rs_g4_alt3(src);
        v->id = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 508) {
        v->id = rs_g2(src);
        v->value = rs_g4_alt2(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->value = rs_ig4(src);
        v->id = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->id = rs_g2_le(src);
        v->value = rs_g4s(src);
        return RS_OK;
    }
    /* JavaMouseWheel.call72b59b7f opcode 10: op68 value, op69 id. */
    if (revision == 474) {
        v->value = rs_ig4(src);
        v->id = rs_g2(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

/* ---- inventory ---- */

static rs_err put_early_slot(int revision, rs_buf *dst, int id, int amt) {
    if (revision == 317) {
        if (id < 0) {
            id = 0;
        }
        if (amt < 0) {
            amt = 0;
        }
        W(put_amt(dst, amt, 1));
        return rs_p2_le_add128(dst, id);
    }
    if (revision == 254) {
        if (id < 0 || amt < 0) {
            return RS_ERR_ARG;
        }
        W(rs_p2(dst, id));
        return put_amt(dst, amt, 0);
    }
    if (revision == 350) {
        if (id < 0 || amt < 0) {
            return RS_ERR_ARG;
        }
        W(rs_p2(dst, id));
        if (amt >= 255) {
            W(rs_p1_add128(dst, 255));
            return rs_p4(dst, amt);
        }
        return rs_p1_add128(dst, amt);
    }
    if (revision == 377) {
        if (id < 0 || amt < 0) {
            return RS_ERR_ARG;
        }
        W(rs_p2_le_add128(dst, id));
        if (amt >= 255) {
            W(rs_p1_neg(dst, 255));
            return rs_ip4(dst, amt);
        }
        return rs_p1_neg(dst, amt);
    }
    return RS_ERR_UNSUPPORTED;
}

static rs_err get_early_slot(int revision, rs_buf *src, int *id, int *amt) {
    if (rs_buf_remaining(src) < 3) {
        return RS_ERR_FORMAT;
    }
    if (revision == 317) {
        int wide = 0;
        if (rs_buf_remaining(src) >= 1 && (src->data[src->pos] & 0xff) == 255) {
            wide = 1;
        }
        if (wide && rs_buf_remaining(src) < 7) {
            return RS_ERR_FORMAT;
        }
        *amt = get_amt(src, 1);
        *id = rs_g2_le_add128(src);
        return RS_OK;
    }
    if (revision == 254) {
        *id = rs_g2(src);
        if (rs_buf_remaining(src) < 1) {
            return RS_ERR_FORMAT;
        }
        if ((src->data[src->pos] & 0xff) == 255 && rs_buf_remaining(src) < 5) {
            return RS_ERR_FORMAT;
        }
        *amt = get_amt(src, 0);
        return RS_OK;
    }
    if (revision == 350) {
        *id = rs_g2(src);
        if (rs_buf_remaining(src) < 1) {
            return RS_ERR_FORMAT;
        }
        *amt = rs_g1_add128(src);
        if (*amt == 255) {
            if (rs_buf_remaining(src) < 4) {
                return RS_ERR_FORMAT;
            }
            *amt = rs_g4(src);
        }
        return RS_OK;
    }
    if (revision == 377) {
        *id = rs_g2_le_add128(src);
        if (rs_buf_remaining(src) < 1) {
            return RS_ERR_FORMAT;
        }
        *amt = rs_g1_neg(src);
        if (*amt == 255) {
            if (rs_buf_remaining(src) < 4) {
                return RS_ERR_FORMAT;
            }
            *amt = rs_ig4(src);
        }
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

static rs_err put_late_full_slot(int revision, rs_buf *dst, int id, int amt) {
    int wire;
    if (id < -1 || amt < 0) {
        return RS_ERR_ARG;
    }
    wire = id + 1;
    if (revision == 742) {
        W(put_amt(dst, amt, 2));
        return rs_p2(dst, wire);
    }
    W(rs_p2(dst, wire));
    return put_amt(dst, amt, 0);
}

static rs_err get_late_full_slot(int revision, rs_buf *src, int *id, int *amt) {
    if (revision == 742) {
        if (rs_buf_remaining(src) < 3) {
            return RS_ERR_FORMAT;
        }
        if ((src->data[src->pos] & 0xff) == 255 && rs_buf_remaining(src) < 7) {
            return RS_ERR_FORMAT;
        }
        *amt = get_amt(src, 2);
        *id = from_wire_id(rs_g2(src));
        return RS_OK;
    }
    if (rs_buf_remaining(src) < 3) {
        return RS_ERR_FORMAT;
    }
    *id = from_wire_id(rs_g2(src));
    if (rs_buf_remaining(src) < 1) {
        return RS_ERR_FORMAT;
    }
    if ((src->data[src->pos] & 0xff) == 255 && rs_buf_remaining(src) < 5) {
        return RS_ERR_FORMAT;
    }
    *amt = get_amt(src, 0);
    return RS_OK;
}

/* Opcode 119 slot: op22 amount, or 255 then op68, then op46 id. Raw id. */
static rs_err put_474_full_slot(rs_buf *dst, int id, int amt) {
    if (id < 0 || amt < 0) {
        return RS_ERR_ARG;
    }
    if (amt >= 255) {
        W(rs_p1_neg(dst, 255));
        W(rs_ip4(dst, amt));
    } else {
        W(rs_p1_neg(dst, amt));
    }
    return rs_p2_le_add128(dst, id);
}

static rs_err get_474_full_slot(rs_buf *src, int *id, int *amt) {
    int marker;
    if (rs_buf_remaining(src) < 3) {
        return RS_ERR_FORMAT;
    }
    marker = rs_g1_neg(src);
    if (marker == 255) {
        if (rs_buf_remaining(src) < 6) {
            return RS_ERR_FORMAT;
        }
        *amt = rs_ig4(src);
    } else {
        *amt = marker;
    }
    *id = rs_g2_le_add128(src);
    return RS_OK;
}

rs_err rs_era_inventory_encode(int revision, rs_buf *dst, const rs_era_inventory *v) {
    int i;
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    e = flags_ok(revision, v->flags);
    if (e) {
        return e;
    }
    e = slot_count_ok(revision, v->nslots);
    if (e) {
        return e;
    }
    if (revision == 254 || revision == 317 || revision == 350 || revision == 377) {
        W(rs_p2(dst, v->component));
        if (revision == 254) {
            W(rs_p1(dst, v->nslots));
        } else {
            W(rs_p2(dst, v->nslots));
        }
        for (i = 0; i < v->nslots; i++) {
            W(put_early_slot(revision, dst, v->slots[i].id, v->slots[i].amount));
        }
        return RS_OK;
    }
    if (late_inv(revision)) {
        W(rs_p2(dst, v->component));
        W(rs_p1(dst, v->flags));
        W(rs_p2(dst, v->nslots));
        for (i = 0; i < v->nslots; i++) {
            W(put_late_full_slot(revision, dst, v->slots[i].id, v->slots[i].amount));
        }
        return RS_OK;
    }
    /* JavaMouseWheel.call72b59b7f opcode 119: op23, op69 key, op69 count. */
    if (revision == 474) {
        W(rs_p4(dst, v->component));
        W(rs_p2(dst, v->flags));
        W(rs_p2(dst, v->nslots));
        for (i = 0; i < v->nslots; i++) {
            W(put_474_full_slot(dst, v->slots[i].id, v->slots[i].amount));
        }
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_inventory_decode(int revision, rs_buf *src, rs_era_inventory *v) {
    int i;
    int n;
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 254 || revision == 317 || revision == 350 || revision == 377) {
        v->component = rs_g2(src);
        n = revision == 254 ? rs_g1(src) : rs_g2(src);
        if (n > RS_ERA_INV_SLOTS) {
            return RS_ERR_RANGE;
        }
        v->nslots = n;
        for (i = 0; i < n; i++) {
            e = get_early_slot(revision, src, &v->slots[i].id, &v->slots[i].amount);
            if (e) {
                return e;
            }
        }
        return RS_OK;
    }
    if (late_inv(revision)) {
        v->component = rs_g2(src);
        v->flags = rs_g1(src);
        if (revision == 910 && (v->flags & 2)) {
            return RS_ERR_UNSUPPORTED;
        }
        n = rs_g2(src);
        if (n > RS_ERA_INV_SLOTS) {
            return RS_ERR_RANGE;
        }
        v->nslots = n;
        for (i = 0; i < n; i++) {
            e = get_late_full_slot(revision, src, &v->slots[i].id, &v->slots[i].amount);
            if (e) {
                return e;
            }
        }
        return RS_OK;
    }
    /* JavaMouseWheel.call72b59b7f opcode 119: op23, op69 key, op69 count. */
    if (revision == 474) {
        if (rs_buf_remaining(src) < 6) {
            return RS_ERR_FORMAT;
        }
        v->component = rs_g4s(src);
        v->flags = rs_g2(src);
        n = rs_g2(src);
        if (n > RS_ERA_INV_SLOTS) {
            return RS_ERR_RANGE;
        }
        v->nslots = n;
        for (i = 0; i < n; i++) {
            e = get_474_full_slot(src, &v->slots[i].id, &v->slots[i].amount);
            if (e) {
                return e;
            }
        }
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

static rs_err put_update_change(int revision, rs_buf *dst, const rs_era_inv_change *c) {
    int amt = c->amount;
    int id = c->id;
    if (c->slot < 0) {
        return RS_ERR_ARG;
    }
    if (revision == 317) {
        if (amt < 0) {
            amt = 0;
        }
        W(rs_psmarts(dst, c->slot));
        W(rs_p2(dst, id));
        return put_amt(dst, amt, 0);
    }
    if (revision == 377 || revision == 350) {
        if (id < 0 || amt < 0) {
            return RS_ERR_ARG;
        }
        W(rs_psmarts(dst, c->slot));
        W(rs_p2(dst, id));
        return put_amt(dst, amt, 0);
    }
    if (revision == 254) {
        if (id < 0 || amt < 0 || c->slot > 255) {
            return RS_ERR_ARG;
        }
        W(rs_p1(dst, c->slot));
        W(rs_p2(dst, id));
        return put_amt(dst, amt, 0);
    }
    if (late_inv(revision)) {
        int wire = id < 0 ? 0 : id + 1;
        if (id < -1) {
            return RS_ERR_ARG;
        }
        W(rs_psmarts(dst, c->slot));
        W(rs_p2(dst, wire));
        if (wire == 0) {
            return RS_OK;
        }
        if (amt < 0) {
            return RS_ERR_ARG;
        }
        /* 742's wide amount is g4s. 910's is g4s as well. */
        return put_amt(dst, amt, 0);
    }
    /* Opcode 187: op51 slot, op69 id. Amount only when id != 0 (op67, or 255 then op23). */
    if (revision == 474) {
        if (id < 0 || c->slot < 0) {
            return RS_ERR_ARG;
        }
        W(rs_psmarts(dst, c->slot));
        W(rs_p2(dst, id));
        if (id == 0) {
            return RS_OK;
        }
        if (amt < 0) {
            return RS_ERR_ARG;
        }
        return put_amt(dst, amt, 0);
    }
    return RS_ERR_UNSUPPORTED;
}

static rs_err change_fits(int revision, const rs_buf *src) {
    size_t need;
    int wide;
    int id_at;
    if (revision == 254) {
        if (rs_buf_remaining(src) < 4) {
            return RS_ERR_FORMAT;
        }
        if ((src->data[src->pos + 3] & 0xff) == 255 && rs_buf_remaining(src) < 8) {
            return RS_ERR_FORMAT;
        }
        return RS_OK;
    }
    wide = smart_len(src);
    if (wide == 0) {
        return RS_ERR_FORMAT;
    }
    if (late_inv(revision)) {
        /* Wire id 0 is a delete: the smart slot and two zero id bytes, no amount. */
        if (rs_buf_remaining(src) < (size_t)wide + 2) {
            return RS_ERR_FORMAT;
        }
        id_at = wide;
        if (src->data[src->pos + id_at] == 0 && src->data[src->pos + id_at + 1] == 0) {
            return RS_OK;
        }
    }
    need = (size_t)wide + 3;
    if (rs_buf_remaining(src) < need) {
        return RS_ERR_FORMAT;
    }
    if ((src->data[src->pos + wide + 2] & 0xff) == 255 && rs_buf_remaining(src) < need + 4) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

/* Opcode 187 change: gsmarts, g2 id, then g1/g4 amount only when id != 0. */
static rs_err fits_474_change(const rs_buf *src) {
    int sl;
    int id;
    size_t rem;
    sl = smart_len(src);
    if (sl == 0) {
        return RS_ERR_FORMAT;
    }
    rem = rs_buf_remaining(src);
    if (rem < (size_t)sl + 2) {
        return RS_ERR_FORMAT;
    }
    id = ((src->data[src->pos + (size_t)sl] & 0xff) << 8) |
         (src->data[src->pos + (size_t)sl + 1] & 0xff);
    if (id == 0) {
        return RS_OK;
    }
    if (rem < (size_t)sl + 3) {
        return RS_ERR_FORMAT;
    }
    if ((src->data[src->pos + (size_t)sl + 2] & 0xff) == 255 && rem < (size_t)sl + 7) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err get_update_change(int revision, rs_buf *src, rs_era_inv_change *c) {
    rs_err e;
    int wire;
    if (revision == 474) {
        e = fits_474_change(src);
        if (e) {
            return e;
        }
        memset(c, 0, sizeof(*c));
        c->slot = rs_gsmarts(src);
        c->id = rs_g2(src);
        if (c->id == 0) {
            c->amount = 0;
            return RS_OK;
        }
        c->amount = get_amt(src, 0);
        return RS_OK;
    }
    e = change_fits(revision, src);
    if (e) {
        return e;
    }
    memset(c, 0, sizeof(*c));
    if (revision == 254) {
        c->slot = rs_g1(src);
        c->id = rs_g2(src);
        c->amount = get_amt(src, 0);
        return RS_OK;
    }
    c->slot = rs_gsmarts(src);
    if (late_inv(revision)) {
        wire = rs_g2(src);
        if (wire == 0) {
            c->id = -1;
            c->amount = 0;
            return RS_OK;
        }
        c->id = from_wire_id(wire);
        c->amount = get_amt(src, 0);
        return RS_OK;
    }
    c->id = rs_g2(src);
    c->amount = get_amt(src, 0);
    return RS_OK;
}

rs_err rs_era_inv_update_encode(int revision, rs_buf *dst, const rs_era_inv_update *v) {
    int i;
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    e = flags_ok(revision, v->flags);
    if (e) {
        return e;
    }
    e = slot_count_ok(revision == 254 ? 254 : 317, v->nchanges);
    if (e) {
        return e;
    }
    /* JavaMouseWheel.call72b59b7f opcode 187: op23 component, op69 key. */
    if (revision == 474) {
        W(rs_p4(dst, v->component));
        W(rs_p2(dst, v->flags));
        for (i = 0; i < v->nchanges; i++) {
            W(put_update_change(revision, dst, &v->changes[i]));
        }
        return RS_OK;
    }
    if (revision != 254 && revision != 317 && revision != 350 && revision != 377 &&
        !late_inv(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    W(rs_p2(dst, v->component));
    if (late_inv(revision)) {
        W(rs_p1(dst, v->flags));
    }
    for (i = 0; i < v->nchanges; i++) {
        W(put_update_change(revision, dst, &v->changes[i]));
    }
    return RS_OK;
}

rs_err rs_era_inv_update_decode(int revision, rs_buf *src, rs_era_inv_update *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    /* JavaMouseWheel.call72b59b7f opcode 187: op23 component, op69 key. */
    if (revision == 474) {
        if (rs_buf_remaining(src) < 6) {
            return RS_ERR_FORMAT;
        }
        v->component = rs_g4s(src);
        v->flags = rs_g2(src);
        while (rs_buf_remaining(src) > 0 && v->nchanges < RS_ERA_INV_SLOTS) {
            e = get_update_change(revision, src, &v->changes[v->nchanges]);
            if (e) {
                return e;
            }
            v->nchanges++;
        }
        if (rs_buf_remaining(src) > 0) {
            return RS_ERR_RANGE;
        }
        return RS_OK;
    }
    if (revision != 254 && revision != 317 && revision != 350 && revision != 377 &&
        !late_inv(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    v->component = rs_g2(src);
    if (late_inv(revision)) {
        v->flags = rs_g1(src);
        if (revision == 910 && (v->flags & 2)) {
            return RS_ERR_UNSUPPORTED;
        }
    }
    while (rs_buf_remaining(src) > 0 && v->nchanges < RS_ERA_INV_SLOTS) {
        e = get_update_change(revision, src, &v->changes[v->nchanges]);
        if (e) {
            return e;
        }
        v->nchanges++;
    }
    if (rs_buf_remaining(src) > 0) {
        return RS_ERR_RANGE;
    }
    return RS_OK;
}

/* ---- interfaces ---- */

static int text_nul(int revision) {
    return revision == 742 || revision == 910;
}

static rs_err put_text(int revision, rs_buf *dst, const char *text) {
    if (text_nul(revision)) {
        return rs_pjstrnull(dst, text);
    }
    return rs_pjstr(dst, text);
}

static rs_err get_text(int revision, rs_buf *src, char *text) {
    if (text_nul(revision)) {
        return rs_gjstrnull(src, text, RS_ERA_TEXT);
    }
    return rs_gjstr(src, text, RS_ERA_TEXT);
}

rs_err rs_era_if_text_encode(int revision, rs_buf *dst, const rs_era_if_text *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(put_text(revision, dst, v->text));
        return rs_p2_add128(dst, v->component);
    }
    if (revision == 254) {
        W(rs_p2(dst, v->component));
        return put_text(revision, dst, v->text);
    }
    if (revision == 350) {
        W(put_text(revision, dst, v->text));
        return rs_p2_le(dst, v->component);
    }
    if (revision == 377) {
        W(rs_p2_le_add128(dst, v->component));
        return put_text(revision, dst, v->text);
    }
    if (revision == 742) {
        W(put_text(revision, dst, v->text));
        return rs_p4_alt3(dst, v->component);
    }
    if (revision == 910) {
        W(rs_ip4(dst, v->component));
        return put_text(revision, dst, v->text);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_if_text_decode(int revision, rs_buf *src, rs_era_if_text *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        e = get_text(revision, src, v->text);
        if (e) {
            return e;
        }
        v->component = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 254) {
        v->component = rs_g2(src);
        return get_text(revision, src, v->text);
    }
    if (revision == 350) {
        e = get_text(revision, src, v->text);
        if (e) {
            return e;
        }
        v->component = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->component = rs_g2_le_add128(src);
        return get_text(revision, src, v->text);
    }
    if (revision == 742) {
        e = get_text(revision, src, v->text);
        if (e) {
            return e;
        }
        v->component = rs_g4_alt3(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->component = rs_ig4(src);
        return get_text(revision, src, v->text);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_if_open_encode(int revision, rs_buf *dst, const rs_era_if_open *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317 || revision == 254 || revision == 350) {
        return rs_p2(dst, v->id);
    }
    if (revision == 377) {
        return rs_p2_le_add128(dst, v->id);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_if_open_decode(int revision, rs_buf *src, rs_era_if_open *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317 || revision == 254 || revision == 350) {
        v->id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->id = rs_g2_le_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_if_side_encode(int revision, rs_buf *dst, const rs_era_if_side *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317 || revision == 350) {
        return rs_p2_le(dst, v->id);
    }
    if (revision == 254) {
        return rs_p2(dst, v->id);
    }
    if (revision == 377) {
        return rs_p2_le_add128(dst, v->id);
    }
    if (revision == 742) {
        W(rs_ip4(dst, v->key[0]));
        W(rs_ip4(dst, v->key[1]));
        W(rs_p4_alt2(dst, v->parent));
        W(rs_p1_sub128(dst, v->type));
        W(rs_p2_le_add128(dst, v->id));
        W(rs_p4_alt2(dst, v->key[2]));
        return rs_p4_alt2(dst, v->key[3]);
    }
    if (revision == 910) {
        W(rs_p4_alt2(dst, v->key[2]));
        W(rs_ip4(dst, v->parent));
        W(rs_p1_neg(dst, v->type));
        W(rs_p4(dst, v->key[3]));
        W(rs_p2(dst, v->id));
        W(rs_p4_alt2(dst, v->key[1]));
        return rs_p4_alt2(dst, v->key[0]);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_if_side_decode(int revision, rs_buf *src, rs_era_if_side *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317 || revision == 350) {
        v->id = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 254) {
        v->id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->id = rs_g2_le_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->key[0] = rs_ig4(src);
        v->key[1] = rs_ig4(src);
        v->parent = rs_g4_alt2(src);
        v->type = rs_g1_sub128(src);
        v->id = rs_g2_le_add128(src);
        v->key[2] = rs_g4_alt2(src);
        v->key[3] = rs_g4_alt2(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->key[2] = rs_g4_alt2(src);
        v->parent = rs_ig4(src);
        v->type = rs_g1_neg(src);
        v->key[3] = rs_g4s(src);
        v->id = rs_g2(src);
        v->key[1] = rs_g4_alt2(src);
        v->key[0] = rs_g4_alt2(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_if_top_encode(int revision, rs_buf *dst, const rs_era_if_top *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_add128(dst, v->viewport));
        return rs_p2(dst, v->sidebar);
    }
    if (revision == 254) {
        W(rs_p2(dst, v->viewport));
        return rs_p2(dst, v->sidebar);
    }
    if (revision == 350) {
        /* Packet 62 writes the sidebar id, then the viewport. */
        W(rs_p2(dst, v->sidebar));
        return rs_p2_add128(dst, v->viewport);
    }
    if (revision == 377) {
        /* Packet 253 writes valueClient1092, then valueClient1182. */
        W(rs_p2_le(dst, v->sidebar));
        return rs_p2_add128(dst, v->viewport);
    }
    if (revision == 742) {
        W(rs_ip4(dst, v->key[0]));
        W(rs_p2_le_add128(dst, v->viewport));
        W(rs_p4_alt2(dst, v->key[1]));
        W(rs_p1_sub128(dst, v->type));
        W(rs_p4(dst, v->key[3]));
        return rs_p4(dst, v->key[2]);
    }
    if (revision == 910) {
        W(rs_p4_alt2(dst, v->key[0]));
        W(rs_ip4(dst, v->key[1]));
        W(rs_p4_alt2(dst, v->key[2]));
        W(rs_p4(dst, v->key[3]));
        W(rs_p1(dst, v->type));
        return rs_p2_le_add128(dst, v->viewport);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_if_top_decode(int revision, rs_buf *src, rs_era_if_top *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->viewport = rs_g2_add128(src);
        v->sidebar = rs_g2(src);
        return RS_OK;
    }
    if (revision == 254) {
        v->viewport = rs_g2(src);
        v->sidebar = rs_g2(src);
        return RS_OK;
    }
    if (revision == 350) {
        v->sidebar = rs_g2(src);
        v->viewport = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->sidebar = rs_g2_le(src);
        v->viewport = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->key[0] = rs_ig4(src);
        v->viewport = rs_g2_le_add128(src);
        v->key[1] = rs_g4_alt2(src);
        v->type = rs_g1_sub128(src);
        v->key[3] = rs_g4s(src);
        v->key[2] = rs_g4s(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->key[0] = rs_g4_alt2(src);
        v->key[1] = rs_ig4(src);
        v->key[2] = rs_g4_alt2(src);
        v->key[3] = rs_g4s(src);
        v->type = rs_g1(src);
        v->viewport = rs_g2_le_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_if_close_encode(int revision, rs_buf *dst, const rs_era_if_close *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    /* 474 JavaMouseWheel.call72b59b7f opcode 241 reads no bytes, as 317 packet 219. */
    if (revision == 254 || revision == 317 || revision == 350 || revision == 377 ||
        revision == 474) {
        return RS_OK;
    }
    if (revision == 742) {
        return rs_ip4(dst, v->component);
    }
    if (revision == 910) {
        return rs_p4_alt2(dst, v->component);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_if_close_decode(int revision, rs_buf *src, rs_era_if_close *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    /* 474 JavaMouseWheel.call72b59b7f opcode 241 reads no bytes, as 317 packet 219. */
    if (revision == 254 || revision == 317 || revision == 350 || revision == 377 ||
        revision == 474) {
        return RS_OK;
    }
    if (revision == 742) {
        v->component = rs_ig4(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->component = rs_g4_alt2(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

/* ---- zone records ---- */

static int expect_op(rs_buf *src, int opcode) {
    int op = rs_g1(src);
    if (op != opcode) {
        return 0;
    }
    return 1;
}

/* 910 bit 7 of the loc flags byte selects a ScaleRotTrans tail. */
static int loc_scaled(int flags) {
    return (flags & 128) != 0;
}

rs_err rs_era_loc_add_encode(int revision, rs_buf *dst, const rs_era_loc *v) {
    int packed;
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    packed = pack_shape(v->shape, v->rot);
    if (revision == 317) {
        W(rs_p1(dst, 151));
        W(rs_p1_add128(dst, zone_off(v->x, v->z)));
        W(rs_p2_le(dst, v->id));
        return rs_p1_sub128(dst, packed);
    }
    if (revision == 254) {
        W(rs_p1(dst, 70));
        W(rs_p1(dst, zone_off(v->x, v->z)));
        W(rs_p1(dst, packed));
        return rs_p2(dst, v->id);
    }
    if (revision == 350) {
        W(rs_p1(dst, 198));
        W(rs_p1_sub128(dst, zone_off(v->x, v->z)));
        W(rs_p2(dst, v->id));
        return rs_p1_neg(dst, packed);
    }
    if (revision == 377) {
        W(rs_p1(dst, 152));
        W(rs_p1_neg(dst, packed));
        W(rs_p2_le_add128(dst, v->id));
        return rs_p1_add128(dst, zone_off(v->x, v->z));
    }
    if (revision == 742) {
        W(rs_p1(dst, 6));
        W(rs_p4_alt3(dst, v->id));
        W(rs_p1_sub128(dst, zone_off(v->x, v->z)));
        return rs_p1_add128(dst, packed);
    }
    if (revision == 910) {
        if (loc_scaled(packed)) {
            return RS_ERR_UNSUPPORTED;
        }
        W(rs_p1(dst, 10));
        W(rs_p1(dst, packed));
        W(rs_p4_alt3(dst, v->id));
        return rs_p1_neg(dst, zone_off(v->x, v->z));
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_loc_add_decode(int revision, rs_buf *src, rs_era_loc *v) {
    int off;
    int packed;
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        if (!expect_op(src, 151)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1_add128(src);
        split_off(off, &v->x, &v->z);
        v->id = rs_g2_le(src);
        unpack_shape(rs_g1_sub128(src), &v->shape, &v->rot);
        return RS_OK;
    }
    if (revision == 254) {
        if (!expect_op(src, 70)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1(src);
        split_off(off, &v->x, &v->z);
        unpack_shape(rs_g1(src), &v->shape, &v->rot);
        v->id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 350) {
        if (!expect_op(src, 198)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1_sub128(src);
        split_off(off, &v->x, &v->z);
        v->id = rs_g2(src);
        unpack_shape(rs_g1_neg(src), &v->shape, &v->rot);
        return RS_OK;
    }
    if (revision == 377) {
        if (!expect_op(src, 152)) {
            return RS_ERR_FORMAT;
        }
        unpack_shape(rs_g1_neg(src), &v->shape, &v->rot);
        v->id = rs_g2_le_add128(src);
        off = rs_g1_add128(src);
        split_off(off, &v->x, &v->z);
        return RS_OK;
    }
    if (revision == 742) {
        if (!expect_op(src, 6)) {
            return RS_ERR_FORMAT;
        }
        v->id = rs_g4_alt3(src);
        off = rs_g1_sub128(src);
        split_off(off, &v->x, &v->z);
        unpack_shape(rs_g1_add128(src), &v->shape, &v->rot);
        return RS_OK;
    }
    if (revision == 910) {
        if (!expect_op(src, 10)) {
            return RS_ERR_FORMAT;
        }
        packed = rs_g1(src);
        if (loc_scaled(packed)) {
            return RS_ERR_UNSUPPORTED;
        }
        unpack_shape(packed, &v->shape, &v->rot);
        v->id = rs_g4_alt3(src);
        off = rs_g1_neg(src);
        split_off(off, &v->x, &v->z);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_loc_del_encode(int revision, rs_buf *dst, const rs_era_loc *v) {
    int packed;
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    packed = pack_shape(v->shape, v->rot);
    if (revision == 317) {
        W(rs_p1(dst, 101));
        W(rs_p1_neg(dst, packed));
        return rs_p1(dst, zone_off(v->x, v->z));
    }
    if (revision == 254) {
        W(rs_p1(dst, 88));
        W(rs_p1(dst, zone_off(v->x, v->z)));
        return rs_p1(dst, packed);
    }
    if (revision == 350) {
        W(rs_p1(dst, 164));
        W(rs_p1_neg(dst, zone_off(v->x, v->z)));
        return rs_p1(dst, packed);
    }
    if (revision == 377) {
        W(rs_p1(dst, 88));
        W(rs_p1_sub128(dst, zone_off(v->x, v->z)));
        return rs_p1_sub128(dst, packed);
    }
    if (revision == 742) {
        W(rs_p1(dst, 1));
        W(rs_p1_neg(dst, zone_off(v->x, v->z)));
        return rs_p1_add128(dst, packed);
    }
    if (revision == 910) {
        if (loc_scaled(packed)) {
            return RS_ERR_UNSUPPORTED;
        }
        W(rs_p1(dst, 5));
        W(rs_p1(dst, packed));
        return rs_p1_sub128(dst, zone_off(v->x, v->z));
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_loc_del_decode(int revision, rs_buf *src, rs_era_loc *v) {
    int off;
    int packed;
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    v->id = -1;
    if (revision == 317) {
        if (!expect_op(src, 101)) {
            return RS_ERR_FORMAT;
        }
        unpack_shape(rs_g1_neg(src), &v->shape, &v->rot);
        off = rs_g1(src);
        split_off(off, &v->x, &v->z);
        return RS_OK;
    }
    if (revision == 254) {
        if (!expect_op(src, 88)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1(src);
        split_off(off, &v->x, &v->z);
        unpack_shape(rs_g1(src), &v->shape, &v->rot);
        return RS_OK;
    }
    if (revision == 350) {
        if (!expect_op(src, 164)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1_neg(src);
        split_off(off, &v->x, &v->z);
        unpack_shape(rs_g1(src), &v->shape, &v->rot);
        return RS_OK;
    }
    if (revision == 377) {
        if (!expect_op(src, 88)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1_sub128(src);
        split_off(off, &v->x, &v->z);
        unpack_shape(rs_g1_sub128(src), &v->shape, &v->rot);
        return RS_OK;
    }
    if (revision == 742) {
        if (!expect_op(src, 1)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1_neg(src);
        split_off(off, &v->x, &v->z);
        unpack_shape(rs_g1_add128(src), &v->shape, &v->rot);
        return RS_OK;
    }
    if (revision == 910) {
        if (!expect_op(src, 5)) {
            return RS_ERR_FORMAT;
        }
        packed = rs_g1(src);
        if (loc_scaled(packed)) {
            return RS_ERR_UNSUPPORTED;
        }
        unpack_shape(packed, &v->shape, &v->rot);
        off = rs_g1_sub128(src);
        split_off(off, &v->x, &v->z);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_obj_add_encode(int revision, rs_buf *dst, const rs_era_obj *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p1(dst, 44));
        W(rs_p2_le_add128(dst, v->id));
        W(rs_p2(dst, v->amount));
        return rs_p1(dst, zone_off(v->x, v->z));
    }
    if (revision == 254) {
        W(rs_p1(dst, 120));
        W(rs_p1(dst, zone_off(v->x, v->z)));
        W(rs_p2(dst, v->id));
        return rs_p2(dst, v->amount);
    }
    if (revision == 350) {
        W(rs_p1(dst, 250));
        W(rs_p1(dst, zone_off(v->x, v->z)));
        W(rs_p2_add128(dst, v->id));
        return rs_p2_le_add128(dst, v->amount);
    }
    if (revision == 377) {
        W(rs_p1(dst, 107));
        W(rs_p2(dst, v->id));
        W(rs_p1_neg(dst, zone_off(v->x, v->z)));
        return rs_p2_add128(dst, v->amount);
    }
    if (revision == 742) {
        W(rs_p1(dst, 2));
        W(rs_p2_le(dst, v->amount));
        W(rs_p2_le(dst, v->id));
        return rs_p1_neg(dst, zone_off(v->x, v->z));
    }
    if (revision == 910) {
        W(rs_p1(dst, 13));
        W(rs_p1_sub128(dst, zone_off(v->x, v->z)));
        W(rs_p2_le_add128(dst, v->id));
        return rs_p2_le(dst, v->amount);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_obj_add_decode(int revision, rs_buf *src, rs_era_obj *v) {
    int off;
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        if (!expect_op(src, 44)) {
            return RS_ERR_FORMAT;
        }
        v->id = rs_g2_le_add128(src);
        v->amount = rs_g2(src);
        off = rs_g1(src);
        split_off(off, &v->x, &v->z);
        return RS_OK;
    }
    if (revision == 254) {
        if (!expect_op(src, 120)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1(src);
        split_off(off, &v->x, &v->z);
        v->id = rs_g2(src);
        v->amount = rs_g2(src);
        return RS_OK;
    }
    if (revision == 350) {
        if (!expect_op(src, 250)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1(src);
        split_off(off, &v->x, &v->z);
        v->id = rs_g2_add128(src);
        v->amount = rs_g2_le_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        if (!expect_op(src, 107)) {
            return RS_ERR_FORMAT;
        }
        v->id = rs_g2(src);
        off = rs_g1_neg(src);
        split_off(off, &v->x, &v->z);
        v->amount = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        if (!expect_op(src, 2)) {
            return RS_ERR_FORMAT;
        }
        v->amount = rs_g2_le(src);
        v->id = rs_g2_le(src);
        off = rs_g1_neg(src);
        split_off(off, &v->x, &v->z);
        return RS_OK;
    }
    if (revision == 910) {
        if (!expect_op(src, 13)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1_sub128(src);
        split_off(off, &v->x, &v->z);
        v->id = rs_g2_le_add128(src);
        v->amount = rs_g2_le(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_obj_del_encode(int revision, rs_buf *dst, const rs_era_obj *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p1(dst, 156));
        W(rs_p1_add128(dst, zone_off(v->x, v->z)));
        return rs_p2(dst, v->id);
    }
    if (revision == 254) {
        W(rs_p1(dst, 115));
        W(rs_p1(dst, zone_off(v->x, v->z)));
        return rs_p2(dst, v->id);
    }
    if (revision == 350) {
        W(rs_p1(dst, 121));
        W(rs_p1_sub128(dst, zone_off(v->x, v->z)));
        return rs_p2_le_add128(dst, v->id);
    }
    if (revision == 377) {
        W(rs_p1(dst, 208));
        W(rs_p2_add128(dst, v->id));
        return rs_p1_add128(dst, zone_off(v->x, v->z));
    }
    if (revision == 742) {
        W(rs_p1(dst, 9));
        W(rs_p2_le(dst, v->id));
        return rs_p1_sub128(dst, zone_off(v->x, v->z));
    }
    if (revision == 910) {
        W(rs_p1(dst, 9));
        W(rs_p2(dst, v->id));
        return rs_p1_sub128(dst, zone_off(v->x, v->z));
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_era_obj_del_decode(int revision, rs_buf *src, rs_era_obj *v) {
    int off;
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        if (!expect_op(src, 156)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1_add128(src);
        split_off(off, &v->x, &v->z);
        v->id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 254) {
        if (!expect_op(src, 115)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1(src);
        split_off(off, &v->x, &v->z);
        v->id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 350) {
        if (!expect_op(src, 121)) {
            return RS_ERR_FORMAT;
        }
        off = rs_g1_sub128(src);
        split_off(off, &v->x, &v->z);
        v->id = rs_g2_le_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        if (!expect_op(src, 208)) {
            return RS_ERR_FORMAT;
        }
        v->id = rs_g2_add128(src);
        off = rs_g1_add128(src);
        split_off(off, &v->x, &v->z);
        return RS_OK;
    }
    if (revision == 742) {
        if (!expect_op(src, 9)) {
            return RS_ERR_FORMAT;
        }
        v->id = rs_g2_le(src);
        off = rs_g1_sub128(src);
        split_off(off, &v->x, &v->z);
        return RS_OK;
    }
    if (revision == 910) {
        if (!expect_op(src, 9)) {
            return RS_ERR_FORMAT;
        }
        v->id = rs_g2(src);
        off = rs_g1_sub128(src);
        split_off(off, &v->x, &v->z);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}
