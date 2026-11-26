#include "both/codec_impl.h"
#include "rslib/both/move.h"

rs_err rs_walk_encode_317(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    rs_err e;
    int i;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    e = rs_p2_le_add128(dst, w->dest_x);
    if (e) {
        return e;
    }
    for (i = 0; i < w->nsteps && i < RS_WALK_STEPS; i++) {
        e = rs_p1(dst, w->dx[i]);
        if (e) {
            return e;
        }
        e = rs_p1(dst, w->dz[i]);
        if (e) {
            return e;
        }
    }
    e = rs_p2_le(dst, w->dest_z);
    if (e) {
        return e;
    }
    return rs_p1_neg(dst, w->running ? 1 : 0);
}

rs_err rs_walk_encode_late(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    rs_err e;
    int i;
    (void)s;
    if (!dst || !w) {
        return RS_ERR_ARG;
    }
    e = rs_p1(dst, w->running ? 1 : 0);
    if (e) {
        return e;
    }
    e = rs_p2(dst, w->dest_x);
    if (e) {
        return e;
    }
    e = rs_p2(dst, w->dest_z);
    if (e) {
        return e;
    }
    for (i = 0; i < w->nsteps && i < RS_WALK_STEPS; i++) {
        e = rs_p1(dst, w->dx[i]);
        if (e) {
            return e;
        }
        e = rs_p1(dst, w->dz[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_codec_walk_encode(const rs_session *s, rs_buf *dst, const rs_walk *w) {
    const rs_move_layout *mv = rs_move_for(s && s->build ? s->build->revision : 0);
    const rs_codec_ops *ops;
    if (mv && mv->walk_encode) {
        return mv->walk_encode(s, dst, w);
    }
    ops = rs_codec_use(s);
    if (!ops || !ops->walk_encode) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->walk_encode(s, dst, w);
}

rs_err rs_walk_decode_317(const rs_session *s, rs_buf *src, rs_walk *w) {
    int left;
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    memset(w, 0, sizeof(*w));
    w->dest_x = rs_g2_le_add128(src);
    left = (int)rs_buf_remaining(src) - 3;
    while (left >= 2 && w->nsteps < RS_WALK_STEPS) {
        w->dx[w->nsteps] = (int8_t)rs_g1(src);
        w->dz[w->nsteps] = (int8_t)rs_g1(src);
        w->nsteps++;
        left -= 2;
    }
    w->dest_z = rs_g2_le(src);
    w->running = rs_g1_neg(src) != 0;
    return RS_OK;
}

rs_err rs_walk_decode_late(const rs_session *s, rs_buf *src, rs_walk *w) {
    (void)s;
    if (!src || !w) {
        return RS_ERR_ARG;
    }
    memset(w, 0, sizeof(*w));
    w->running = rs_g1(src) != 0;
    w->dest_x = rs_g2(src);
    w->dest_z = rs_g2(src);
    while (rs_buf_remaining(src) >= 2 && w->nsteps < RS_WALK_STEPS) {
        w->dx[w->nsteps] = (int8_t)rs_g1(src);
        w->dz[w->nsteps] = (int8_t)rs_g1(src);
        w->nsteps++;
    }
    return RS_OK;
}

rs_err rs_codec_walk_decode(const rs_session *s, rs_buf *src, rs_walk *w) {
    const rs_move_layout *mv = rs_move_for(s && s->build ? s->build->revision : 0);
    const rs_codec_ops *ops;
    if (mv && mv->walk_decode) {
        return mv->walk_decode(s, src, w);
    }
    ops = rs_codec_use(s);
    if (!ops || !ops->walk_decode) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->walk_decode(s, src, w);
}

/* 317 PlayerEntity.read body anims: stand, turn, walk, turn180, turn90cw, turn90ccw, run. */
static const int k_body_anim_317[7] = {0x328, 0x337, 0x333, 0x334, 0x335, 0x336, 0x338};

rs_err rs_appearance_encode_317(const rs_session *s, rs_buf *dst, const rs_appearance *a) {
    int i;
    rs_err e;
    (void)s;
    if (!dst || !a) {
        return RS_ERR_ARG;
    }
    e = rs_p1(dst, a->gender);
    if (e) {
        return e;
    }
    e = rs_p1(dst, a->headicon);
    if (e) {
        return e;
    }
    if (a->npc_id > 0) {
        e = rs_p1(dst, 255);
        if (e) {
            return e;
        }
        e = rs_p1(dst, 255);
        if (e) {
            return e;
        }
        e = rs_p2(dst, a->npc_id);
        if (e) {
            return e;
        }
    } else {
        for (i = 0; i < 12; i++) {
            e = (i < 7) ? rs_p2(dst, 0x100 + a->styles[i]) : rs_p1(dst, 0);
            if (e) {
                return e;
            }
        }
    }
    for (i = 0; i < 5; i++) {
        e = rs_p1(dst, a->colours[i]);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < 7; i++) {
        int anim = a->anims[i];
        if (anim == 0) {
            anim = k_body_anim_317[i];
        } else if (anim < 0) {
            anim = 65535;
        }
        e = rs_p2(dst, anim);
        if (e) {
            return e;
        }
    }
    e = rs_p8(dst, rs_base37_encode(a->name));
    if (e) {
        return e;
    }
    e = rs_p1(dst, a->combat_level);
    if (e) {
        return e;
    }
    return rs_p2(dst, a->skill_level);
}

rs_err rs_appearance_decode_317(const rs_session *s, rs_buf *src, rs_appearance *a) {
    int i;
    uint64_t name37;
    (void)s;
    if (!src || !a) {
        return RS_ERR_ARG;
    }
    memset(a, 0, sizeof(*a));
    a->gender = rs_g1(src);
    a->headicon = rs_g1(src);
    for (i = 0; i < 12; i++) {
        int hi = rs_g1(src);
        int v;
        if (hi == 0) {
            continue;
        }
        v = (hi << 8) | rs_g1(src);
        if (i == 0 && v == 65535) {
            a->npc_id = rs_g2(src);
            break;
        }
        if (i < 7) {
            a->styles[i] = v - 0x100;
        }
    }
    for (i = 0; i < 5; i++) {
        a->colours[i] = rs_g1(src);
    }
    for (i = 0; i < 7; i++) {
        int anim = rs_g2(src);
        a->anims[i] = anim == 65535 ? -1 : anim;
    }
    name37 = rs_g8(src);
    if (name37 == 0) {
        a->name[0] = 0;
    } else {
        rs_base37_decode(name37, a->name);
    }
    a->combat_level = rs_g1(src);
    a->skill_level = rs_g2(src);
    return RS_OK;
}

/* 317 packet 134: g1 id, g4_alt2 xp, g1 level. */
rs_err rs_skill_encode_317(const rs_session *s, rs_buf *dst, const rs_skill *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p1(dst, v->id);
    if (e) {
        return e;
    }
    e = rs_p4_alt2(dst, v->xp);
    if (e) {
        return e;
    }
    return rs_p1(dst, v->level);
}

rs_err rs_skill_decode_317(const rs_session *s, rs_buf *src, rs_skill *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->id = rs_g1(src);
    v->xp = rs_g4_alt2(src);
    v->level = rs_g1(src);
    return RS_OK;
}

/* 317 packet 36 is g2_alt1 id + g1b value. Packet 87 is g2_alt1 id + g4_alt2 value. */
rs_err rs_varp_encode_317(const rs_session *s, rs_buf *dst, const rs_varp *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p2_le(dst, v->id);
    if (e) {
        return e;
    }
    if (v->value >= -128 && v->value <= 127) {
        return rs_p1(dst, v->value);
    }
    return rs_p4_alt2(dst, v->value);
}

rs_err rs_varp_decode_317(const rs_session *s, rs_buf *src, rs_varp *v) {
    size_t n;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    n = rs_buf_remaining(src);
    v->id = rs_g2_le(src);
    if (n <= 3) {
        v->value = rs_g1b(src);
    } else {
        v->value = rs_g4_alt2(src);
    }
    return RS_OK;
}

rs_err rs_if_open_encode_317(const rs_session *s, rs_buf *dst, const rs_if_open *v) {
    (void)s;
    return dst && v ? rs_p2(dst, v->id) : RS_ERR_ARG;
}

rs_err rs_if_open_decode_317(const rs_session *s, rs_buf *src, rs_if_open *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->id = rs_g2(src);
    return RS_OK;
}

/* 317 packet 126: string, then g2_alt2 component. */
rs_err rs_if_text_encode_317(const rs_session *s, rs_buf *dst, const rs_if_text *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_pjstr(dst, v->text);
    if (e) {
        return e;
    }
    return rs_p2_add128(dst, v->component);
}

rs_err rs_if_text_decode_317(const rs_session *s, rs_buf *src, rs_if_text *v) {
    rs_err e;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    e = rs_gjstr(src, v->text, sizeof(v->text));
    if (e) {
        return e;
    }
    v->component = rs_g2_add128(src);
    return RS_OK;
}

rs_err rs_message_encode_317(const rs_session *s, rs_buf *dst, const rs_message *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_pjstr(dst, v->text);
}

rs_err rs_message_decode_317(const rs_session *s, rs_buf *src, rs_message *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    return rs_gjstr(src, v->text, sizeof(v->text));
}

/* 317 packet 53: g2 component, g2 count, then amount (g1, or 255 + g4_alt3) and g2_alt3 id. */
rs_err rs_inventory_encode_317(const rs_session *s, rs_buf *dst, const rs_inventory *v) {
    int i;
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p2(dst, v->component);
    if (e) {
        return e;
    }
    e = rs_p2(dst, v->nslots);
    if (e) {
        return e;
    }
    for (i = 0; i < v->nslots && i < RS_INV_SLOTS; i++) {
        int id = v->slots[i].id;
        int amt = v->slots[i].amount;
        if (id < 0) {
            id = 0;
        }
        if (amt < 0) {
            amt = 0;
        }
        if (amt >= 255) {
            e = rs_p1(dst, 255);
            if (e) {
                return e;
            }
            e = rs_p4_alt3(dst, amt);
        } else {
            e = rs_p1(dst, amt);
        }
        if (e) {
            return e;
        }
        e = rs_p2_le_add128(dst, id);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_inventory_decode_317(const rs_session *s, rs_buf *src, rs_inventory *v) {
    int i;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->component = rs_g2(src);
    v->nslots = rs_g2(src);
    if (v->nslots > RS_INV_SLOTS) {
        v->nslots = RS_INV_SLOTS;
    }
    for (i = 0; i < v->nslots && rs_buf_remaining(src) >= 3; i++) {
        int amt = rs_g1(src);
        if (amt == 255) {
            amt = rs_g4_alt3(src);
        }
        v->slots[i].amount = amt;
        v->slots[i].id = rs_g2_le_add128(src);
    }
    return RS_OK;
}

static int region_mapsquares(int zone_x, int zone_z, int *out, int cap) {
    int mx0 = (zone_x - 6) / 8;
    int mx1 = (zone_x + 6) / 8;
    int mz0 = (zone_z - 6) / 8;
    int mz1 = (zone_z + 6) / 8;
    int skip = ((zone_x / 8 == 48 || zone_x / 8 == 49) && zone_z / 8 == 48) ||
               (zone_x / 8 == 48 && zone_z / 8 == 148);
    int n = 0;
    int mx, mz;
    for (mx = mx0; mx <= mx1; mx++) {
        for (mz = mz0; mz <= mz1; mz++) {
            if (skip && (mz == 49 || mz == 149 || mz == 147 || mx == 50 ||
                         (mx == 49 && mz == 47))) {
                continue;
            }
            if (n < cap) {
                out[n++] = (mx << 8) | mz;
            }
        }
    }
    return n;
}

static rs_err region_keys(rs_buf *dst, const rs_region *v, int squares[], int n, int alt3) {
    int i, k;
    rs_err e;
    for (i = 0; i < n; i++) {
        const rs_xtea_key *key = rs_cache_find_key_mapsquare(v->cache, squares[i]);
        for (k = 0; k < 4; k++) {
            int word = key ? (int)key->key[k] : 0;
            e = alt3 ? rs_p4_alt3(dst, word) : rs_ip4(dst, word);
            if (e) {
                return e;
            }
        }
    }
    return RS_OK;
}

static int session_rev(const rs_session *s) {
    if (!s || !s->build) {
        return 0;
    }
    return s->build->revision;
}

/* 578/614/667 ids 0–3. 742 BuildAreaSize.forId adds id 4 = 72. */
static int area_tiles(int rev, int id) {
    if (id == 0) {
        return 104;
    }
    if (id == 1) {
        return 120;
    }
    if (id == 2) {
        return 136;
    }
    if (id == 3) {
        return 168;
    }
    if (rev == 742 && id == 4) {
        return 72;
    }
    return -1;
}

/* Full (zone ± rad) / 8 window, x outer, z inner. No tutorial-island skip. */
static rs_err span_squares(int zone_x, int zone_z, int rad, int *out, int cap, int *n_out) {
    int mx0 = (zone_x - rad) / 8;
    int mx1 = (zone_x + rad) / 8;
    int mz0 = (zone_z - rad) / 8;
    int mz1 = (zone_z + rad) / 8;
    int n = 0;
    int mx, mz;
    for (mx = mx0; mx <= mx1; mx++) {
        for (mz = mz0; mz <= mz1; mz++) {
            if (n >= cap) {
                return RS_ERR_RANGE;
            }
            out[n++] = (mx << 8) | mz;
        }
    }
    *n_out = n;
    return RS_OK;
}

static rs_err write_keys(rs_buf *dst, const rs_region *v, const int *squares, int n, int alt2) {
    int i, k;
    rs_err e;
    for (i = 0; i < n; i++) {
        const rs_xtea_key *key = rs_cache_find_key_mapsquare(v->cache, squares[i]);
        for (k = 0; k < 4; k++) {
            int word = key ? (int)key->key[k] : 0;
            e = alt2 ? rs_p4_alt2(dst, word) : rs_p4(dst, word);
            if (e) {
                return e;
            }
        }
    }
    return RS_OK;
}

static void skip_keys(rs_buf *src, int n, int alt2) {
    int i, k;
    for (i = 0; i < n; i++) {
        for (k = 0; k < 4; k++) {
            if (alt2) {
                (void)rs_g4_alt2(src);
            } else {
                (void)rs_g4(src);
            }
        }
    }
}

static int key_groups(const rs_buf *src, int trailer) {
    int rem = (int)rs_buf_remaining(src);
    if (rem < trailer) {
        return 0;
    }
    return (rem - trailer) / 16;
}

static void local_tiles(const rs_region *v, int *lx, int *lz) {
    *lx = v->local_x ? v->local_x : 52;
    *lz = v->local_z ? v->local_z : 52;
}

rs_err rs_region_encode_zones(const rs_session *s, rs_buf *dst, const rs_region *v) {
    int rev, squares[64], n, lx, lz, tiles;
    rs_err e;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    rev = session_rev(s);
    /* 443 Timer.opTimer4(false): g2_le zone_x, g2_le local_z, g2_le_add128 local_x, g1_add128 plane, alt2 keys, g2_add128 zone_z. */
    if (rev == 443) {
        local_tiles(v, &lx, &lz);
        n = region_mapsquares(v->zone_x, v->zone_z, squares, 64);
        e = rs_p2_le(dst, v->zone_x);
        if (e) {
            return e;
        }
        e = rs_p2_le(dst, lz);
        if (e) {
            return e;
        }
        e = rs_p2_le_add128(dst, lx);
        if (e) {
            return e;
        }
        e = rs_p1_add128(dst, v->plane & 3);
        if (e) {
            return e;
        }
        e = write_keys(dst, v, squares, n, 1);
        if (e) {
            return e;
        }
        return rs_p2_add128(dst, v->zone_z);
    }
    /* 525 C8be0107e.opkind3609(false): BE keys, g2_le zone_x, g2_le_add128 zone_z, g2_add128 local_x, g1 plane, g2 local_z. */
    if (rev == 525) {
        local_tiles(v, &lx, &lz);
        e = span_squares(v->zone_x, v->zone_z, 6, squares, 64, &n);
        if (e) {
            return e;
        }
        e = write_keys(dst, v, squares, n, 0);
        if (e) {
            return e;
        }
        e = rs_p2_le(dst, v->zone_x);
        if (e) {
            return e;
        }
        e = rs_p2_le_add128(dst, v->zone_z);
        if (e) {
            return e;
        }
        e = rs_p2_add128(dst, lx);
        if (e) {
            return e;
        }
        e = rs_p1(dst, v->plane & 3);
        if (e) {
            return e;
        }
        return rs_p2(dst, lz);
    }
    /* 578 Cc4cace49.opkind9583(false): g2_le zone_x, g2 zone_z, g1 build area, g1_neg force, BE keys. */
    if (rev == 578) {
        tiles = area_tiles(rev, v->build_area);
        if (tiles < 0) {
            return RS_ERR_RANGE;
        }
        e = span_squares(v->zone_x, v->zone_z, tiles >> 4, squares, 64, &n);
        if (e) {
            return e;
        }
        e = rs_p2_le(dst, v->zone_x);
        if (e) {
            return e;
        }
        e = rs_p2(dst, v->zone_z);
        if (e) {
            return e;
        }
        e = rs_p1(dst, v->build_area);
        if (e) {
            return e;
        }
        e = rs_p1_neg(dst, v->force_reload ? 1 : 0);
        if (e) {
            return e;
        }
        return write_keys(dst, v, squares, n, 0);
    }
    /* 614 C0bd9f077.opkind11111(false): g2_le_add128 zone_z, g1_add128 build area, g1 force, g2_le zone_x, BE keys. */
    if (rev == 614) {
        tiles = area_tiles(rev, v->build_area);
        if (tiles < 0) {
            return RS_ERR_RANGE;
        }
        e = span_squares(v->zone_x, v->zone_z, tiles >> 4, squares, 64, &n);
        if (e) {
            return e;
        }
        e = rs_p2_le_add128(dst, v->zone_z);
        if (e) {
            return e;
        }
        e = rs_p1_add128(dst, v->build_area);
        if (e) {
            return e;
        }
        e = rs_p1(dst, v->force_reload ? 1 : 0);
        if (e) {
            return e;
        }
        e = rs_p2_le(dst, v->zone_x);
        if (e) {
            return e;
        }
        return write_keys(dst, v, squares, n, 0);
    }
    /* 667 C7a6fda97.opkind20206: g1_neg build area, g1 force, g2_le zone_x, g2 zone_z, BE keys. */
    if (rev == 667) {
        tiles = area_tiles(rev, v->build_area);
        if (tiles < 0) {
            return RS_ERR_RANGE;
        }
        e = span_squares(v->zone_x, v->zone_z, tiles >> 4, squares, 64, &n);
        if (e) {
            return e;
        }
        e = rs_p1_neg(dst, v->build_area);
        if (e) {
            return e;
        }
        e = rs_p1(dst, v->force_reload ? 1 : 0);
        if (e) {
            return e;
        }
        e = rs_p2_le(dst, v->zone_x);
        if (e) {
            return e;
        }
        e = rs_p2(dst, v->zone_z);
        if (e) {
            return e;
        }
        return write_keys(dst, v, squares, n, 0);
    }
    /* 742 World.rebuildNormalMap: g1_add128 build area, g2 zone_x, g1_add128 force, g2_add128 zone_z, BE keys. */
    if (rev == 742) {
        tiles = area_tiles(rev, v->build_area);
        if (tiles < 0) {
            return RS_ERR_RANGE;
        }
        e = span_squares(v->zone_x, v->zone_z, tiles >> 4, squares, 64, &n);
        if (e) {
            return e;
        }
        e = rs_p1_add128(dst, v->build_area);
        if (e) {
            return e;
        }
        e = rs_p2(dst, v->zone_x);
        if (e) {
            return e;
        }
        e = rs_p1_add128(dst, v->force_reload ? 1 : 0);
        if (e) {
            return e;
        }
        e = rs_p2_add128(dst, v->zone_z);
        if (e) {
            return e;
        }
        return write_keys(dst, v, squares, n, 0);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_region_encode_rs2(const rs_session *s, rs_buf *dst, const rs_region *v) {
    rs_err e;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    if (s && s->build && s->build->revision == 317) {
        e = rs_p2_add128(dst, v->zone_x);
        if (e) {
            return e;
        }
        return rs_p2(dst, v->zone_z);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_region_encode_508(const rs_session *s, rs_buf *dst, const rs_region *v) {
    rs_err e;
    int local_x, local_z, squares[16], n, i, k;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    local_x = v->local_x ? v->local_x : 52;
    local_z = v->local_z ? v->local_z : 52;
    n = region_mapsquares(v->zone_x, v->zone_z, squares, 16);
    e = rs_p2_add128(dst, v->zone_x);
    if (e) {
        return e;
    }
    e = rs_p2_le_add128(dst, local_x);
    if (e) {
        return e;
    }
    e = rs_p2_add128(dst, local_z);
    if (e) {
        return e;
    }
    for (i = 0; i < n; i++) {
        const rs_xtea_key *key = rs_cache_find_key_mapsquare(v->cache, squares[i]);
        for (k = 0; k < 4; k++) {
            e = rs_p4(dst, key ? (int)key->key[k] : 0);
            if (e) {
                return e;
            }
        }
    }
    e = rs_p1_neg(dst, v->plane & 3);
    if (e) {
        return e;
    }
    return rs_p2(dst, v->zone_z);
}

/* 474 rebuild-normal. 468–507 share it. Keys are little-endian. */
rs_err rs_region_encode_474(const rs_session *s, rs_buf *dst, const rs_region *v) {
    rs_err e;
    int local_x, local_z, squares[16], n;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    local_x = v->local_x ? v->local_x : 52;
    local_z = v->local_z ? v->local_z : 52;
    n = region_mapsquares(v->zone_x, v->zone_z, squares, 16);
    e = rs_p2_le_add128(dst, local_x);
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->plane & 3);
    if (e) {
        return e;
    }
    e = rs_p2(dst, v->zone_x);
    if (e) {
        return e;
    }
    e = region_keys(dst, v, squares, n, 0);
    if (e) {
        return e;
    }
    e = rs_p2_add128(dst, local_z);
    if (e) {
        return e;
    }
    return rs_p2(dst, v->zone_z);
}

/* 530 rebuild-normal (C633b3b0d.opkind5884(false)): local x, alt3 keys, plane, zones. */
rs_err rs_region_encode_530(const rs_session *s, rs_buf *dst, const rs_region *v) {
    rs_err e;
    int local_x, local_z, squares[16], n;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    local_x = v->local_x ? v->local_x : 52;
    local_z = v->local_z ? v->local_z : 52;
    n = region_mapsquares(v->zone_x, v->zone_z, squares, 16);
    e = rs_p2_add128(dst, local_x);
    if (e) {
        return e;
    }
    e = region_keys(dst, v, squares, n, 1);
    if (e) {
        return e;
    }
    e = rs_p1_sub128(dst, v->plane & 3);
    if (e) {
        return e;
    }
    e = rs_p2(dst, v->zone_x);
    if (e) {
        return e;
    }
    e = rs_p2_add128(dst, v->zone_z);
    if (e) {
        return e;
    }
    return rs_p2_add128(dst, local_z);
}

/* 910 World.rebuildNormalMap. No XTEA in the packet. Earlier RS3 rows are not this layout. */
rs_err rs_region_encode_rs3(const rs_session *s, rs_buf *dst, const rs_region *v) {
    rs_err e;
    int squares[16], n;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    if (session_rev(s) != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    n = region_mapsquares(v->zone_x, v->zone_z, squares, 16);
    e = rs_p2_add128(dst, v->zone_x);
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->npc_view ? v->npc_view : 14);
    if (e) {
        return e;
    }
    e = rs_p1_sub128(dst, n);
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->build_area);
    if (e) {
        return e;
    }
    e = rs_p2_add128(dst, v->zone_z);
    if (e) {
        return e;
    }
    return rs_p1_sub128(dst, v->force_reload ? 1 : 0);
}

rs_err rs_region_decode_zones(const rs_session *s, rs_buf *src, rs_region *v) {
    int rev, n;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    rev = session_rev(s);
    memset(v, 0, sizeof(*v));
    /* 443 Timer.opTimer4(false): g2_le zone_x, g2_le local_z, g2_le_add128 local_x, g1_add128 plane, alt2 keys, g2_add128 zone_z. */
    if (rev == 443) {
        v->zone_x = rs_g2_le(src);
        v->local_z = rs_g2_le(src);
        v->local_x = rs_g2_le_add128(src);
        v->plane = rs_g1_add128(src);
        n = key_groups(src, 2);
        skip_keys(src, n, 1);
        v->zone_z = rs_g2_add128(src);
        return RS_OK;
    }
    /* 525 C8be0107e.opkind3609(false): BE keys, g2_le zone_x, g2_le_add128 zone_z, g2_add128 local_x, g1 plane, g2 local_z. */
    if (rev == 525) {
        n = key_groups(src, 9);
        skip_keys(src, n, 0);
        v->zone_x = rs_g2_le(src);
        v->zone_z = rs_g2_le_add128(src);
        v->local_x = rs_g2_add128(src);
        v->plane = rs_g1(src);
        v->local_z = rs_g2(src);
        return RS_OK;
    }
    /* 578 Cc4cace49.opkind9583(false): g2_le zone_x, g2 zone_z, g1 build area, g1_neg force, BE keys. */
    if (rev == 578) {
        v->zone_x = rs_g2_le(src);
        v->zone_z = rs_g2(src);
        v->build_area = rs_g1(src);
        v->force_reload = rs_g1_neg(src) == 1;
        if (area_tiles(rev, v->build_area) < 0) {
            return RS_ERR_RANGE;
        }
        n = key_groups(src, 0);
        skip_keys(src, n, 0);
        return RS_OK;
    }
    /* 614 C0bd9f077.opkind11111(false): g2_le_add128 zone_z, g1_add128 build area, g1 force, g2_le zone_x, BE keys. */
    if (rev == 614) {
        v->zone_z = rs_g2_le_add128(src);
        v->build_area = rs_g1_add128(src);
        v->force_reload = rs_g1(src) == 1;
        v->zone_x = rs_g2_le(src);
        if (area_tiles(rev, v->build_area) < 0) {
            return RS_ERR_RANGE;
        }
        n = key_groups(src, 0);
        skip_keys(src, n, 0);
        return RS_OK;
    }
    /* 667 C7a6fda97.opkind20206: g1_neg build area, g1 force, g2_le zone_x, g2 zone_z, BE keys. */
    if (rev == 667) {
        v->build_area = rs_g1_neg(src);
        v->force_reload = rs_g1(src) == 1;
        v->zone_x = rs_g2_le(src);
        v->zone_z = rs_g2(src);
        if (area_tiles(rev, v->build_area) < 0) {
            return RS_ERR_RANGE;
        }
        n = key_groups(src, 0);
        skip_keys(src, n, 0);
        return RS_OK;
    }
    /* 742 World.rebuildNormalMap: g1_add128 build area, g2 zone_x, g1_add128 force, g2_add128 zone_z, BE keys. */
    if (rev == 742) {
        v->build_area = rs_g1_add128(src);
        v->zone_x = rs_g2(src);
        v->force_reload = rs_g1_add128(src) == 1;
        v->zone_z = rs_g2_add128(src);
        if (area_tiles(rev, v->build_area) < 0) {
            return RS_ERR_RANGE;
        }
        n = key_groups(src, 0);
        skip_keys(src, n, 0);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_region_decode_rs2(const rs_session *s, rs_buf *src, rs_region *v) {
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    if (s && s->build && s->build->revision == 317) {
        v->zone_x = rs_g2_add128(src);
        v->zone_z = rs_g2(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_region_decode_508(const rs_session *s, rs_buf *src, rs_region *v) {
    int n, i, k;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->zone_x = rs_g2_add128(src);
    v->local_x = rs_g2_le_add128(src);
    v->local_z = rs_g2_add128(src);
    n = ((int)rs_buf_remaining(src) - 3) / 16;
    for (i = 0; i < n; i++) {
        for (k = 0; k < 4; k++) {
            (void)rs_g4(src);
        }
    }
    v->plane = rs_g1_neg(src);
    v->zone_z = rs_g2(src);
    return RS_OK;
}

rs_err rs_region_decode_474(const rs_session *s, rs_buf *src, rs_region *v) {
    int n, i, k;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->local_x = rs_g2_le_add128(src);
    v->plane = rs_g1(src);
    v->zone_x = rs_g2(src);
    n = ((int)rs_buf_remaining(src) - 4) / 16;
    for (i = 0; i < n; i++) {
        for (k = 0; k < 4; k++) {
            (void)rs_ig4(src);
        }
    }
    v->local_z = rs_g2_add128(src);
    v->zone_z = rs_g2(src);
    return RS_OK;
}

rs_err rs_region_decode_530(const rs_session *s, rs_buf *src, rs_region *v) {
    int n, i, k;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->local_x = rs_g2_add128(src);
    n = ((int)rs_buf_remaining(src) - 7) / 16;
    for (i = 0; i < n; i++) {
        for (k = 0; k < 4; k++) {
            (void)rs_g4_alt3(src);
        }
    }
    v->plane = rs_g1_sub128(src);
    v->zone_x = rs_g2(src);
    v->zone_z = rs_g2_add128(src);
    v->local_z = rs_g2_add128(src);
    return RS_OK;
}

rs_err rs_region_decode_rs3(const rs_session *s, rs_buf *src, rs_region *v) {
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    if (session_rev(s) != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    v->zone_x = rs_g2_add128(src);
    v->npc_view = rs_g1(src);
    (void)rs_g1_sub128(src);
    v->build_area = rs_g1(src);
    v->zone_z = rs_g2_add128(src);
    v->force_reload = rs_g1_sub128(src) == 1;
    return RS_OK;
}

rs_err rs_codec_region_encode(const rs_session *s, rs_buf *dst, const rs_region *v) {
    const rs_move_layout *mv = rs_move_for(s && s->build ? s->build->revision : 0);
    const rs_codec_ops *ops;
    if (mv && mv->region_encode) {
        return mv->region_encode(s, dst, v);
    }
    ops = rs_codec_use(s);
    if (!ops || !ops->region_encode) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->region_encode(s, dst, v);
}

rs_err rs_codec_region_decode(const rs_session *s, rs_buf *src, rs_region *v) {
    const rs_move_layout *mv = rs_move_for(s && s->build ? s->build->revision : 0);
    const rs_codec_ops *ops;
    if (mv && mv->region_decode) {
        return mv->region_decode(s, src, v);
    }
    ops = rs_codec_use(s);
    if (!ops || !ops->region_decode) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->region_decode(s, src, v);
}

rs_err rs_player_encode_317(const rs_session *s, rs_buf *dst, const rs_player_update *v) {
    rs_buf app;
    uint8_t tmp[128];
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    rs_access_bits(dst);
    rs_pbit(dst, 1, 1);
    rs_pbit(dst, 2, 3);
    rs_pbit(dst, 2, v->plane & 3);
    rs_pbit(dst, 1, v->discard_walk ? 1 : 0);
    rs_pbit(dst, 1, v->include_appearance ? 1 : 0);
    rs_pbit(dst, 7, v->local_z & 127);
    rs_pbit(dst, 7, v->local_x & 127);
    rs_pbit(dst, 8, 0);
    rs_pbit(dst, 11, 2047);
    rs_access_bytes(dst);
    if (v->include_appearance) {
        rs_buf_wrap(&app, tmp, sizeof(tmp));
        rs_codec_appearance_encode(s, &app, &v->appearance);
        rs_p1(dst, 0x10);
        rs_p1(dst, (int)app.pos);
        rs_pdata(dst, tmp, app.pos);
    }
    return RS_OK;
}

rs_err rs_player_decode_317(const rs_session *s, rs_buf *src, rs_player_update *v) {
    int upd, type;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    rs_access_bits(src);
    upd = rs_gbit(src, 1);
    if (upd) {
        type = rs_gbit(src, 2);
        if (type == 0) {
            v->include_appearance = 1;
        } else if (type == 1) {
            (void)rs_gbit(src, 3);
            v->include_appearance = rs_gbit(src, 1);
        } else if (type == 2) {
            (void)rs_gbit(src, 3);
            (void)rs_gbit(src, 3);
            v->include_appearance = rs_gbit(src, 1);
        } else if (type == 3) {
            v->teleport = 1;
            v->plane = rs_gbit(src, 2);
            v->discard_walk = rs_gbit(src, 1);
            v->include_appearance = rs_gbit(src, 1);
            v->local_z = rs_gbit(src, 7);
            v->local_x = rs_gbit(src, 7);
        }
    }
    (void)rs_gbit(src, 8);
    (void)rs_gbit(src, 11);
    rs_access_bytes(src);
    if (v->include_appearance && rs_buf_remaining(src) >= 2) {
        int mask = rs_g1(src);
        int n = rs_g1(src);
        (void)mask;
        if (rs_buf_remaining(src) >= (size_t)n) {
            rs_buf app;
            rs_buf_wrap(&app, src->data + src->pos, (size_t)n);
            rs_codec_appearance_decode(s, &app, &v->appearance);
            src->pos += (size_t)n;
        }
    }
    return RS_OK;
}

rs_err rs_npc_empty_317(const rs_session *s, rs_buf *dst) {
    (void)s;
    if (!dst) {
        return RS_ERR_ARG;
    }
    rs_access_bits(dst);
    rs_pbit(dst, 8, 0);
    rs_pbit(dst, 14, 16383);
    rs_access_bytes(dst);
    return RS_OK;
}

rs_err rs_codec_player_update_encode(const rs_session *s, rs_buf *dst, const rs_player_update *v) {
    const rs_codec_ops *ops = rs_codec_use(s);
    if (!ops || !ops->player_encode) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->player_encode(s, dst, v);
}

rs_err rs_codec_player_update_decode(const rs_session *s, rs_buf *src, rs_player_update *v) {
    const rs_codec_ops *ops = rs_codec_use(s);
    if (!ops || !ops->player_decode) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->player_decode(s, src, v);
}

rs_err rs_codec_npc_update_empty(const rs_session *s, rs_buf *dst) {
    const rs_codec_ops *ops = rs_codec_use(s);
    if (!ops || !ops->npc_empty) {
        return RS_ERR_UNSUPPORTED;
    }
    return ops->npc_empty(s, dst);
}

static int zone_off(int x, int z) {
    return ((x & 7) << 4) | (z & 7);
}

rs_err rs_zone_base_encode_317(const rs_session *s, rs_buf *dst, const rs_zone_base *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p1_neg(dst, v->z);
    if (e) {
        return e;
    }
    return rs_p1_neg(dst, v->x);
}

rs_err rs_zone_base_decode_317(const rs_session *s, rs_buf *src, rs_zone_base *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->z = rs_g1_neg(src);
    v->x = rs_g1_neg(src);
    return RS_OK;
}

rs_err rs_loc_add_encode_317(const rs_session *s, rs_buf *dst, const rs_scene_loc *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p1_add128(dst, zone_off(v->x, v->z));
    if (e) {
        return e;
    }
    e = rs_p2_le(dst, v->id);
    if (e) {
        return e;
    }
    return rs_p1_sub128(dst, ((v->shape & 31) << 2) | (v->rot & 3));
}

rs_err rs_loc_add_decode_317(const rs_session *s, rs_buf *src, rs_scene_loc *v) {
    int off, shape;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    off = rs_g1_add128(src);
    v->x = (off >> 4) & 7;
    v->z = off & 7;
    v->id = rs_g2_le(src);
    shape = rs_g1_sub128(src);
    v->shape = shape >> 2;
    v->rot = shape & 3;
    return RS_OK;
}

rs_err rs_loc_del_encode_317(const rs_session *s, rs_buf *dst, const rs_scene_loc *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p1_neg(dst, ((v->shape & 31) << 2) | (v->rot & 3));
    if (e) {
        return e;
    }
    return rs_p1(dst, zone_off(v->x, v->z));
}

rs_err rs_loc_del_decode_317(const rs_session *s, rs_buf *src, rs_scene_loc *v) {
    int shape, off;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    shape = rs_g1_neg(src);
    v->shape = shape >> 2;
    v->rot = shape & 3;
    off = rs_g1(src);
    v->x = (off >> 4) & 7;
    v->z = off & 7;
    return RS_OK;
}

rs_err rs_obj_add_encode_317(const rs_session *s, rs_buf *dst, const rs_scene_obj *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p2_le_add128(dst, v->id);
    if (e) {
        return e;
    }
    e = rs_p2(dst, v->amount);
    if (e) {
        return e;
    }
    return rs_p1(dst, zone_off(v->x, v->z));
}

rs_err rs_obj_add_decode_317(const rs_session *s, rs_buf *src, rs_scene_obj *v) {
    int off;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->id = rs_g2_le_add128(src);
    v->amount = rs_g2(src);
    off = rs_g1(src);
    v->x = (off >> 4) & 7;
    v->z = off & 7;
    return RS_OK;
}

rs_err rs_obj_del_encode_317(const rs_session *s, rs_buf *dst, const rs_scene_obj *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p1_add128(dst, zone_off(v->x, v->z));
    if (e) {
        return e;
    }
    return rs_p2(dst, v->id);
}

rs_err rs_obj_del_decode_317(const rs_session *s, rs_buf *src, rs_scene_obj *v) {
    int off;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    off = rs_g1_add128(src);
    v->x = (off >> 4) & 7;
    v->z = off & 7;
    v->id = rs_g2(src);
    return RS_OK;
}

#define RS_CODEC_DISPATCH(slot, ...)                \
    do {                                            \
        const rs_codec_ops *ops_ = rs_codec_use(s); \
        if (!ops_ || !ops_->slot) {                 \
            return RS_ERR_UNSUPPORTED;              \
        }                                           \
        return ops_->slot(__VA_ARGS__);            \
    } while (0)

rs_err rs_codec_appearance_encode(const rs_session *s, rs_buf *dst, const rs_appearance *a) {
    RS_CODEC_DISPATCH(appearance_encode, s, dst, a);
}
rs_err rs_codec_appearance_decode(const rs_session *s, rs_buf *src, rs_appearance *a) {
    RS_CODEC_DISPATCH(appearance_decode, s, src, a);
}
rs_err rs_codec_skill_encode(const rs_session *s, rs_buf *dst, const rs_skill *v) {
    RS_CODEC_DISPATCH(skill_encode, s, dst, v);
}
rs_err rs_codec_skill_decode(const rs_session *s, rs_buf *src, rs_skill *v) {
    RS_CODEC_DISPATCH(skill_decode, s, src, v);
}
rs_err rs_codec_varp_encode(const rs_session *s, rs_buf *dst, const rs_varp *v) {
    RS_CODEC_DISPATCH(varp_encode, s, dst, v);
}
rs_err rs_codec_varp_decode(const rs_session *s, rs_buf *src, rs_varp *v) {
    RS_CODEC_DISPATCH(varp_decode, s, src, v);
}
rs_err rs_codec_if_open_encode(const rs_session *s, rs_buf *dst, const rs_if_open *v) {
    RS_CODEC_DISPATCH(if_open_encode, s, dst, v);
}
rs_err rs_codec_if_open_decode(const rs_session *s, rs_buf *src, rs_if_open *v) {
    RS_CODEC_DISPATCH(if_open_decode, s, src, v);
}
rs_err rs_codec_if_text_encode(const rs_session *s, rs_buf *dst, const rs_if_text *v) {
    RS_CODEC_DISPATCH(if_text_encode, s, dst, v);
}
rs_err rs_codec_if_text_decode(const rs_session *s, rs_buf *src, rs_if_text *v) {
    RS_CODEC_DISPATCH(if_text_decode, s, src, v);
}
rs_err rs_codec_message_encode(const rs_session *s, rs_buf *dst, const rs_message *v) {
    RS_CODEC_DISPATCH(message_encode, s, dst, v);
}
rs_err rs_codec_message_decode(const rs_session *s, rs_buf *src, rs_message *v) {
    RS_CODEC_DISPATCH(message_decode, s, src, v);
}
rs_err rs_codec_inventory_encode(const rs_session *s, rs_buf *dst, const rs_inventory *v) {
    RS_CODEC_DISPATCH(inventory_encode, s, dst, v);
}
rs_err rs_codec_inventory_decode(const rs_session *s, rs_buf *src, rs_inventory *v) {
    RS_CODEC_DISPATCH(inventory_decode, s, src, v);
}
rs_err rs_codec_zone_base_encode(const rs_session *s, rs_buf *dst, const rs_zone_base *v) {
    RS_CODEC_DISPATCH(zone_base_encode, s, dst, v);
}
rs_err rs_codec_zone_base_decode(const rs_session *s, rs_buf *src, rs_zone_base *v) {
    RS_CODEC_DISPATCH(zone_base_decode, s, src, v);
}
rs_err rs_codec_loc_add_encode(const rs_session *s, rs_buf *dst, const rs_scene_loc *v) {
    RS_CODEC_DISPATCH(loc_add_encode, s, dst, v);
}
rs_err rs_codec_loc_add_decode(const rs_session *s, rs_buf *src, rs_scene_loc *v) {
    RS_CODEC_DISPATCH(loc_add_decode, s, src, v);
}
rs_err rs_codec_loc_del_encode(const rs_session *s, rs_buf *dst, const rs_scene_loc *v) {
    RS_CODEC_DISPATCH(loc_del_encode, s, dst, v);
}
rs_err rs_codec_loc_del_decode(const rs_session *s, rs_buf *src, rs_scene_loc *v) {
    RS_CODEC_DISPATCH(loc_del_decode, s, src, v);
}
rs_err rs_codec_obj_add_encode(const rs_session *s, rs_buf *dst, const rs_scene_obj *v) {
    RS_CODEC_DISPATCH(obj_add_encode, s, dst, v);
}
rs_err rs_codec_obj_add_decode(const rs_session *s, rs_buf *src, rs_scene_obj *v) {
    RS_CODEC_DISPATCH(obj_add_decode, s, src, v);
}
rs_err rs_codec_obj_del_encode(const rs_session *s, rs_buf *dst, const rs_scene_obj *v) {
    RS_CODEC_DISPATCH(obj_del_encode, s, dst, v);
}
rs_err rs_codec_obj_del_decode(const rs_session *s, rs_buf *src, rs_scene_obj *v) {
    RS_CODEC_DISPATCH(obj_del_decode, s, src, v);
}
