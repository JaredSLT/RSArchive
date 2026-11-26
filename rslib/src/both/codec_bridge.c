#include "both/codec_impl.h"

#include "rslib/both/era_body.h"
#include "rslib/both/sync.h"

/* era loc/obj records start with the opcode. The codec payload is the body;
   the frame writes the opcode separately. */

enum {
    RS_PLAYER_KNOWN = RS_PLAYER_APPEARANCE | RS_PLAYER_ANIM | RS_PLAYER_CHAT | RS_PLAYER_HIT |
                      RS_PLAYER_SPOTANIM | RS_PLAYER_FACE_ENTITY | RS_PLAYER_FACE_TILE |
                      RS_PLAYER_FORCE_MOVE | RS_PLAYER_PUBLIC | RS_PLAYER_HIT2
};

enum { ZONE_LOC_ADD = 1, ZONE_LOC_DEL, ZONE_OBJ_ADD, ZONE_OBJ_DEL };

static rs_err bridge_revision(const rs_session *s, int *rev) {
    if (!s || !s->build || s->build->revision == 0) {
        return RS_ERR_ARG;
    }
    *rev = s->build->revision;
    return RS_OK;
}

static rs_err bridge_in(const rs_session *s, const void *buf, const void *v, int *rev) {
    if (!buf || !v) {
        return RS_ERR_ARG;
    }
    return bridge_revision(s, rev);
}

static void copy_text(char *dst, size_t n, const char *src) {
    size_t i;
    if (n == 0) {
        return;
    }
    for (i = 0; i + 1 < n && src[i]; i++) {
        dst[i] = src[i];
    }
    dst[i] = 0;
}

static int varp_small(int value) {
    return value >= -128 && value <= 127;
}

static int bounded(int n, int cap) {
    if (n < 0) {
        return 0;
    }
    if (n > cap) {
        return cap;
    }
    return n;
}

rs_err rs_bridge_skill_encode(const rs_session *s, rs_buf *dst, const rs_skill *v) {
    rs_era_skill era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    memset(&era, 0, sizeof(era));
    era.id = v->id;
    era.level = v->level;
    era.xp = v->xp;
    return rs_era_skill_encode(rev, dst, &era);
}

rs_err rs_bridge_skill_decode(const rs_session *s, rs_buf *src, rs_skill *v) {
    rs_era_skill era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = rs_era_skill_decode(rev, src, &era);
    if (e != RS_OK) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    v->id = era.id;
    v->level = era.level;
    v->xp = era.xp;
    return RS_OK;
}

rs_err rs_bridge_varp_encode(const rs_session *s, rs_buf *dst, const rs_varp *v) {
    rs_era_varp era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    memset(&era, 0, sizeof(era));
    era.id = v->id;
    era.value = v->value;
    /* One slot. Width matches rs_conn_send_varp. */
    if (varp_small(v->value)) {
        return rs_era_varp_small_encode(rev, dst, &era);
    }
    return rs_era_varp_large_encode(rev, dst, &era);
}

rs_err rs_bridge_varp_decode(const rs_session *s, rs_buf *src, rs_varp *v) {
    rs_era_varp era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    /* Every implemented small body is 3 bytes and every large body is 6. */
    if (rs_buf_remaining(src) <= 3) {
        e = rs_era_varp_small_decode(rev, src, &era);
    } else {
        e = rs_era_varp_large_decode(rev, src, &era);
    }
    if (e != RS_OK) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    v->id = era.id;
    v->value = era.value;
    return RS_OK;
}

static void inventory_to_era(rs_era_inventory *era, const rs_inventory *v) {
    int i;
    int n;
    memset(era, 0, sizeof(*era));
    era->component = v->component;
    era->flags = v->flags;
    era->nslots = v->nslots;
    n = bounded(v->nslots, RS_ERA_INV_SLOTS);
    for (i = 0; i < n; i++) {
        era->slots[i].id = v->slots[i].id;
        era->slots[i].amount = v->slots[i].amount;
    }
}

static void inventory_from_era(rs_inventory *v, const rs_era_inventory *era) {
    int i;
    int n;
    memset(v, 0, sizeof(*v));
    v->component = era->component;
    v->flags = era->flags;
    v->nslots = era->nslots;
    n = bounded(era->nslots, RS_INV_SLOTS);
    for (i = 0; i < n; i++) {
        v->slots[i].id = era->slots[i].id;
        v->slots[i].amount = era->slots[i].amount;
    }
}

rs_err rs_bridge_inventory_encode(const rs_session *s, rs_buf *dst, const rs_inventory *v) {
    rs_era_inventory era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    inventory_to_era(&era, v);
    return rs_era_inventory_encode(rev, dst, &era);
}

rs_err rs_bridge_inventory_decode(const rs_session *s, rs_buf *src, rs_inventory *v) {
    rs_era_inventory era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = rs_era_inventory_decode(rev, src, &era);
    if (e != RS_OK) {
        return e;
    }
    inventory_from_era(v, &era);
    return RS_OK;
}

static void changes_to_era(rs_era_inv_update *era, const rs_inv_update *v) {
    int i;
    int n;
    memset(era, 0, sizeof(*era));
    era->component = v->component;
    era->flags = v->flags;
    era->nchanges = v->nchanges;
    n = bounded(v->nchanges, RS_ERA_INV_SLOTS);
    for (i = 0; i < n; i++) {
        era->changes[i].slot = v->changes[i].slot;
        era->changes[i].id = v->changes[i].id;
        era->changes[i].amount = v->changes[i].amount;
    }
}

static void changes_from_era(rs_inv_update *v, const rs_era_inv_update *era) {
    int i;
    int n;
    memset(v, 0, sizeof(*v));
    v->component = era->component;
    v->flags = era->flags;
    v->nchanges = era->nchanges;
    n = bounded(era->nchanges, RS_INV_SLOTS);
    for (i = 0; i < n; i++) {
        v->changes[i].slot = era->changes[i].slot;
        v->changes[i].id = era->changes[i].id;
        v->changes[i].amount = era->changes[i].amount;
    }
}

rs_err rs_bridge_inv_update_encode(const rs_session *s, rs_buf *dst, const rs_inv_update *v) {
    rs_era_inv_update era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    changes_to_era(&era, v);
    return rs_era_inv_update_encode(rev, dst, &era);
}

rs_err rs_bridge_inv_update_decode(const rs_session *s, rs_buf *src, rs_inv_update *v) {
    rs_era_inv_update era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = rs_era_inv_update_decode(rev, src, &era);
    if (e != RS_OK) {
        return e;
    }
    changes_from_era(v, &era);
    return RS_OK;
}

rs_err rs_bridge_if_open_encode(const rs_session *s, rs_buf *dst, const rs_if_open *v) {
    rs_era_if_open era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    memset(&era, 0, sizeof(era));
    era.id = v->id;
    return rs_era_if_open_encode(rev, dst, &era);
}

rs_err rs_bridge_if_open_decode(const rs_session *s, rs_buf *src, rs_if_open *v) {
    rs_era_if_open era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = rs_era_if_open_decode(rev, src, &era);
    if (e != RS_OK) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    v->id = era.id;
    return RS_OK;
}

rs_err rs_bridge_if_text_encode(const rs_session *s, rs_buf *dst, const rs_if_text *v) {
    rs_era_if_text era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    memset(&era, 0, sizeof(era));
    era.component = v->component;
    copy_text(era.text, sizeof(era.text), v->text);
    return rs_era_if_text_encode(rev, dst, &era);
}

rs_err rs_bridge_if_text_decode(const rs_session *s, rs_buf *src, rs_if_text *v) {
    rs_era_if_text era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = rs_era_if_text_decode(rev, src, &era);
    if (e != RS_OK) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    v->component = era.component;
    copy_text(v->text, sizeof(v->text), era.text);
    return RS_OK;
}

static void side_to_era(rs_era_if_side *era, const rs_if_sub *v) {
    int i;
    memset(era, 0, sizeof(*era));
    era->id = v->id;
    era->parent = v->parent;
    era->type = v->type;
    for (i = 0; i < 4; i++) {
        era->key[i] = v->key[i];
    }
}

static void side_from_era(rs_if_sub *v, const rs_era_if_side *era) {
    int i;
    memset(v, 0, sizeof(*v));
    v->id = era->id;
    v->parent = era->parent;
    v->type = era->type;
    for (i = 0; i < 4; i++) {
        v->key[i] = era->key[i];
    }
}

rs_err rs_bridge_if_sub_encode(const rs_session *s, rs_buf *dst, const rs_if_sub *v) {
    rs_era_if_side era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    side_to_era(&era, v);
    return rs_era_if_side_encode(rev, dst, &era);
}

rs_err rs_bridge_if_sub_decode(const rs_session *s, rs_buf *src, rs_if_sub *v) {
    rs_era_if_side era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = rs_era_if_side_decode(rev, src, &era);
    if (e != RS_OK) {
        return e;
    }
    side_from_era(v, &era);
    return RS_OK;
}

static void top_to_era(rs_era_if_top *era, const rs_if_top *v) {
    int i;
    memset(era, 0, sizeof(*era));
    era->viewport = v->viewport;
    era->sidebar = v->sidebar;
    era->type = v->type;
    for (i = 0; i < 4; i++) {
        era->key[i] = v->key[i];
    }
}

static void top_from_era(rs_if_top *v, const rs_era_if_top *era) {
    int i;
    memset(v, 0, sizeof(*v));
    v->viewport = era->viewport;
    v->sidebar = era->sidebar;
    v->type = era->type;
    for (i = 0; i < 4; i++) {
        v->key[i] = era->key[i];
    }
}

rs_err rs_bridge_if_top_encode(const rs_session *s, rs_buf *dst, const rs_if_top *v) {
    rs_era_if_top era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    top_to_era(&era, v);
    return rs_era_if_top_encode(rev, dst, &era);
}

rs_err rs_bridge_if_top_decode(const rs_session *s, rs_buf *src, rs_if_top *v) {
    rs_era_if_top era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = rs_era_if_top_decode(rev, src, &era);
    if (e != RS_OK) {
        return e;
    }
    top_from_era(v, &era);
    return RS_OK;
}

static rs_err zone_strip(rs_buf *dst, const rs_buf *rec) {
    if (rec->pos < 1) {
        return RS_ERR_FORMAT;
    }
    return rs_pdata(dst, rec->data + 1, rec->pos - 1);
}

static rs_err zone_probe(int revision, int kind, int *opcode) {
    uint8_t raw[32];
    rs_buf probe;
    rs_era_loc loc;
    rs_era_obj obj;
    rs_err e;
    memset(&loc, 0, sizeof(loc));
    memset(&obj, 0, sizeof(obj));
    rs_buf_wrap(&probe, raw, sizeof(raw));
    if (kind == ZONE_LOC_ADD) {
        e = rs_era_loc_add_encode(revision, &probe, &loc);
    } else if (kind == ZONE_LOC_DEL) {
        e = rs_era_loc_del_encode(revision, &probe, &loc);
    } else if (kind == ZONE_OBJ_ADD) {
        e = rs_era_obj_add_encode(revision, &probe, &obj);
    } else {
        e = rs_era_obj_del_encode(revision, &probe, &obj);
    }
    if (e != RS_OK) {
        return e;
    }
    if (probe.pos < 1) {
        return RS_ERR_FORMAT;
    }
    *opcode = raw[0] & 0xff;
    return RS_OK;
}

static rs_err zone_decode(int revision, rs_buf *src, int kind, void *era_out) {
    uint8_t storage[64];
    rs_buf full;
    size_t n;
    int opcode;
    rs_err e = zone_probe(revision, kind, &opcode);
    if (e != RS_OK) {
        return e;
    }
    n = rs_buf_remaining(src);
    if (n > sizeof(storage) - 1) {
        n = sizeof(storage) - 1;
    }
    storage[0] = (uint8_t)opcode;
    if (n > 0) {
        memcpy(storage + 1, src->data + src->pos, n);
    }
    rs_buf_wrap(&full, storage, n + 1);
    if (kind == ZONE_LOC_ADD) {
        e = rs_era_loc_add_decode(revision, &full, era_out);
    } else if (kind == ZONE_LOC_DEL) {
        e = rs_era_loc_del_decode(revision, &full, era_out);
    } else if (kind == ZONE_OBJ_ADD) {
        e = rs_era_obj_add_decode(revision, &full, era_out);
    } else {
        e = rs_era_obj_del_decode(revision, &full, era_out);
    }
    if (e == RS_OK && full.pos > 0) {
        src->pos += full.pos - 1;
    }
    return e;
}

static void loc_to_era(rs_era_loc *era, const rs_scene_loc *v) {
    memset(era, 0, sizeof(*era));
    era->x = v->x;
    era->z = v->z;
    era->id = v->id;
    era->shape = v->shape;
    era->rot = v->rot;
}

static void loc_from_era(rs_scene_loc *v, const rs_era_loc *era) {
    memset(v, 0, sizeof(*v));
    v->x = era->x;
    v->z = era->z;
    v->id = era->id;
    v->shape = era->shape;
    v->rot = era->rot;
}

static void obj_to_era(rs_era_obj *era, const rs_scene_obj *v) {
    memset(era, 0, sizeof(*era));
    era->x = v->x;
    era->z = v->z;
    era->id = v->id;
    era->amount = v->amount;
}

static void obj_from_era(rs_scene_obj *v, const rs_era_obj *era) {
    memset(v, 0, sizeof(*v));
    v->x = era->x;
    v->z = era->z;
    v->id = era->id;
    v->amount = era->amount;
}

rs_err rs_bridge_loc_add_encode(const rs_session *s, rs_buf *dst, const rs_scene_loc *v) {
    uint8_t raw[32];
    rs_buf rec;
    rs_era_loc era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    loc_to_era(&era, v);
    rs_buf_wrap(&rec, raw, sizeof(raw));
    e = rs_era_loc_add_encode(rev, &rec, &era);
    if (e != RS_OK) {
        return e;
    }
    return zone_strip(dst, &rec);
}

rs_err rs_bridge_loc_add_decode(const rs_session *s, rs_buf *src, rs_scene_loc *v) {
    rs_era_loc era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = zone_decode(rev, src, ZONE_LOC_ADD, &era);
    if (e != RS_OK) {
        return e;
    }
    loc_from_era(v, &era);
    return RS_OK;
}

rs_err rs_bridge_loc_del_encode(const rs_session *s, rs_buf *dst, const rs_scene_loc *v) {
    uint8_t raw[32];
    rs_buf rec;
    rs_era_loc era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    loc_to_era(&era, v);
    rs_buf_wrap(&rec, raw, sizeof(raw));
    e = rs_era_loc_del_encode(rev, &rec, &era);
    if (e != RS_OK) {
        return e;
    }
    return zone_strip(dst, &rec);
}

rs_err rs_bridge_loc_del_decode(const rs_session *s, rs_buf *src, rs_scene_loc *v) {
    rs_era_loc era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = zone_decode(rev, src, ZONE_LOC_DEL, &era);
    if (e != RS_OK) {
        return e;
    }
    loc_from_era(v, &era);
    return RS_OK;
}

rs_err rs_bridge_obj_add_encode(const rs_session *s, rs_buf *dst, const rs_scene_obj *v) {
    uint8_t raw[32];
    rs_buf rec;
    rs_era_obj era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    obj_to_era(&era, v);
    rs_buf_wrap(&rec, raw, sizeof(raw));
    e = rs_era_obj_add_encode(rev, &rec, &era);
    if (e != RS_OK) {
        return e;
    }
    return zone_strip(dst, &rec);
}

rs_err rs_bridge_obj_add_decode(const rs_session *s, rs_buf *src, rs_scene_obj *v) {
    rs_era_obj era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = zone_decode(rev, src, ZONE_OBJ_ADD, &era);
    if (e != RS_OK) {
        return e;
    }
    obj_from_era(v, &era);
    return RS_OK;
}

rs_err rs_bridge_obj_del_encode(const rs_session *s, rs_buf *dst, const rs_scene_obj *v) {
    uint8_t raw[32];
    rs_buf rec;
    rs_era_obj era;
    int rev;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    obj_to_era(&era, v);
    rs_buf_wrap(&rec, raw, sizeof(raw));
    e = rs_era_obj_del_encode(rev, &rec, &era);
    if (e != RS_OK) {
        return e;
    }
    return zone_strip(dst, &rec);
}

rs_err rs_bridge_obj_del_decode(const rs_session *s, rs_buf *src, rs_scene_obj *v) {
    rs_era_obj era;
    int rev;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = zone_decode(rev, src, ZONE_OBJ_DEL, &era);
    if (e != RS_OK) {
        return e;
    }
    obj_from_era(v, &era);
    return RS_OK;
}

int rs_player_wire_mask(int revision, int logical) {
    int m = 0;
    if (revision == 254) {
        if (logical & RS_PLAYER_APPEARANCE) {
            m |= RS_SYNC_P254_APPEARANCE;
        }
        if (logical & RS_PLAYER_ANIM) {
            m |= RS_SYNC_P254_ANIM;
        }
        if (logical & RS_PLAYER_FACE_ENTITY) {
            m |= RS_SYNC_P254_FACE_ENTITY;
        }
        if (logical & RS_PLAYER_CHAT) {
            m |= RS_SYNC_P254_CHAT;
        }
        if (logical & RS_PLAYER_HIT) {
            m |= RS_SYNC_P254_HIT;
        }
        if (logical & RS_PLAYER_FACE_TILE) {
            m |= RS_SYNC_P254_FACE_TILE;
        }
        if (logical & RS_PLAYER_PUBLIC) {
            m |= RS_SYNC_P254_PUBLIC;
        }
        if (logical & RS_PLAYER_SPOTANIM) {
            m |= RS_SYNC_P254_SPOTANIM;
        }
        if (logical & RS_PLAYER_FORCE_MOVE) {
            m |= RS_SYNC_P254_FORCE;
        }
        if (logical & RS_PLAYER_HIT2) {
            m |= RS_SYNC_P254_HIT2;
        }
        return m;
    }
    if (revision == 317) {
        if (logical & RS_PLAYER_FACE_ENTITY) {
            m |= RS_SYNC_P317_FACE_ENTITY;
        }
        if (logical & RS_PLAYER_FACE_TILE) {
            m |= RS_SYNC_P317_FACE_TILE;
        }
        if (logical & RS_PLAYER_CHAT) {
            m |= RS_SYNC_P317_CHAT;
        }
        if (logical & RS_PLAYER_ANIM) {
            m |= RS_SYNC_P317_ANIM;
        }
        if (logical & RS_PLAYER_APPEARANCE) {
            m |= RS_SYNC_P317_APPEARANCE;
        }
        if (logical & RS_PLAYER_HIT) {
            m |= RS_SYNC_P317_HIT;
        }
        if (logical & RS_PLAYER_PUBLIC) {
            m |= RS_SYNC_P317_PUBLIC;
        }
        if (logical & RS_PLAYER_SPOTANIM) {
            m |= RS_SYNC_P317_SPOTANIM;
        }
        if (logical & RS_PLAYER_HIT2) {
            m |= RS_SYNC_P317_HIT2;
        }
        if (logical & RS_PLAYER_FORCE_MOVE) {
            m |= RS_SYNC_P317_FORCE;
        }
        return m;
    }
    if (revision == 350) {
        if (logical & RS_PLAYER_ANIM) {
            m |= RS_SYNC_P350_ANIM;
        }
        if (logical & RS_PLAYER_FACE_TILE) {
            m |= RS_SYNC_P350_FACE_TILE;
        }
        if (logical & RS_PLAYER_CHAT) {
            m |= RS_SYNC_P350_CHAT;
        }
        if (logical & RS_PLAYER_HIT2) {
            m |= RS_SYNC_P350_HIT2;
        }
        if (logical & RS_PLAYER_APPEARANCE) {
            m |= RS_SYNC_P350_APPEARANCE;
        }
        if (logical & RS_PLAYER_PUBLIC) {
            m |= RS_SYNC_P350_PUBLIC;
        }
        if (logical & RS_PLAYER_FACE_ENTITY) {
            m |= RS_SYNC_P350_FACE_ENTITY;
        }
        if (logical & RS_PLAYER_SPOTANIM) {
            m |= RS_SYNC_P350_SPOTANIM;
        }
        if (logical & RS_PLAYER_HIT) {
            m |= RS_SYNC_P350_HIT;
        }
        if (logical & RS_PLAYER_FORCE_MOVE) {
            m |= RS_SYNC_P350_FORCE;
        }
        return m;
    }
    if (revision == 377) {
        if (logical & RS_PLAYER_FACE_ENTITY) {
            m |= RS_SYNC_P377_FACE_ENTITY;
        }
        if (logical & RS_PLAYER_FACE_TILE) {
            m |= RS_SYNC_P377_FACE_TILE;
        }
        if (logical & RS_PLAYER_APPEARANCE) {
            m |= RS_SYNC_P377_APPEARANCE;
        }
        if (logical & RS_PLAYER_ANIM) {
            m |= RS_SYNC_P377_ANIM;
        }
        if (logical & RS_PLAYER_CHAT) {
            m |= RS_SYNC_P377_CHAT;
        }
        if (logical & RS_PLAYER_PUBLIC) {
            m |= RS_SYNC_P377_PUBLIC;
        }
        if (logical & RS_PLAYER_HIT2) {
            m |= RS_SYNC_P377_HIT2;
        }
        if (logical & RS_PLAYER_FORCE_MOVE) {
            m |= RS_SYNC_P377_FORCE;
        }
        if (logical & RS_PLAYER_SPOTANIM) {
            m |= RS_SYNC_P377_SPOTANIM;
        }
        if (logical & RS_PLAYER_HIT) {
            m |= RS_SYNC_P377_HIT;
        }
        return m;
    }
    return 0;
}

static int logical_player_mask(int revision, int wire) {
    int m = 0;
    if (revision == 254) {
        if (wire & RS_SYNC_P254_APPEARANCE) {
            m |= RS_PLAYER_APPEARANCE;
        }
        if (wire & RS_SYNC_P254_ANIM) {
            m |= RS_PLAYER_ANIM;
        }
        if (wire & RS_SYNC_P254_FACE_ENTITY) {
            m |= RS_PLAYER_FACE_ENTITY;
        }
        if (wire & RS_SYNC_P254_CHAT) {
            m |= RS_PLAYER_CHAT;
        }
        if (wire & RS_SYNC_P254_HIT) {
            m |= RS_PLAYER_HIT;
        }
        if (wire & RS_SYNC_P254_FACE_TILE) {
            m |= RS_PLAYER_FACE_TILE;
        }
        if (wire & RS_SYNC_P254_PUBLIC) {
            m |= RS_PLAYER_PUBLIC;
        }
        if (wire & RS_SYNC_P254_SPOTANIM) {
            m |= RS_PLAYER_SPOTANIM;
        }
        if (wire & RS_SYNC_P254_FORCE) {
            m |= RS_PLAYER_FORCE_MOVE;
        }
        if (wire & RS_SYNC_P254_HIT2) {
            m |= RS_PLAYER_HIT2;
        }
        return m;
    }
    if (revision == 317) {
        if (wire & RS_SYNC_P317_FACE_ENTITY) {
            m |= RS_PLAYER_FACE_ENTITY;
        }
        if (wire & RS_SYNC_P317_FACE_TILE) {
            m |= RS_PLAYER_FACE_TILE;
        }
        if (wire & RS_SYNC_P317_CHAT) {
            m |= RS_PLAYER_CHAT;
        }
        if (wire & RS_SYNC_P317_ANIM) {
            m |= RS_PLAYER_ANIM;
        }
        if (wire & RS_SYNC_P317_APPEARANCE) {
            m |= RS_PLAYER_APPEARANCE;
        }
        if (wire & RS_SYNC_P317_HIT) {
            m |= RS_PLAYER_HIT;
        }
        if (wire & RS_SYNC_P317_PUBLIC) {
            m |= RS_PLAYER_PUBLIC;
        }
        if (wire & RS_SYNC_P317_SPOTANIM) {
            m |= RS_PLAYER_SPOTANIM;
        }
        if (wire & RS_SYNC_P317_HIT2) {
            m |= RS_PLAYER_HIT2;
        }
        if (wire & RS_SYNC_P317_FORCE) {
            m |= RS_PLAYER_FORCE_MOVE;
        }
        return m;
    }
    if (revision == 350) {
        if (wire & RS_SYNC_P350_ANIM) {
            m |= RS_PLAYER_ANIM;
        }
        if (wire & RS_SYNC_P350_FACE_TILE) {
            m |= RS_PLAYER_FACE_TILE;
        }
        if (wire & RS_SYNC_P350_CHAT) {
            m |= RS_PLAYER_CHAT;
        }
        if (wire & RS_SYNC_P350_HIT2) {
            m |= RS_PLAYER_HIT2;
        }
        if (wire & RS_SYNC_P350_APPEARANCE) {
            m |= RS_PLAYER_APPEARANCE;
        }
        if (wire & RS_SYNC_P350_PUBLIC) {
            m |= RS_PLAYER_PUBLIC;
        }
        if (wire & RS_SYNC_P350_FACE_ENTITY) {
            m |= RS_PLAYER_FACE_ENTITY;
        }
        if (wire & RS_SYNC_P350_SPOTANIM) {
            m |= RS_PLAYER_SPOTANIM;
        }
        if (wire & RS_SYNC_P350_HIT) {
            m |= RS_PLAYER_HIT;
        }
        if (wire & RS_SYNC_P350_FORCE) {
            m |= RS_PLAYER_FORCE_MOVE;
        }
        return m;
    }
    if (revision == 377) {
        if (wire & RS_SYNC_P377_FACE_ENTITY) {
            m |= RS_PLAYER_FACE_ENTITY;
        }
        if (wire & RS_SYNC_P377_FACE_TILE) {
            m |= RS_PLAYER_FACE_TILE;
        }
        if (wire & RS_SYNC_P377_APPEARANCE) {
            m |= RS_PLAYER_APPEARANCE;
        }
        if (wire & RS_SYNC_P377_ANIM) {
            m |= RS_PLAYER_ANIM;
        }
        if (wire & RS_SYNC_P377_CHAT) {
            m |= RS_PLAYER_CHAT;
        }
        if (wire & RS_SYNC_P377_PUBLIC) {
            m |= RS_PLAYER_PUBLIC;
        }
        if (wire & RS_SYNC_P377_HIT2) {
            m |= RS_PLAYER_HIT2;
        }
        if (wire & RS_SYNC_P377_FORCE) {
            m |= RS_PLAYER_FORCE_MOVE;
        }
        if (wire & RS_SYNC_P377_SPOTANIM) {
            m |= RS_PLAYER_SPOTANIM;
        }
        if (wire & RS_SYNC_P377_HIT) {
            m |= RS_PLAYER_HIT;
        }
        return m;
    }
    return 0;
}

static void appearance_to_player(rs_sync_player *p, const rs_appearance *a) {
    int i;
    p->gender = a->gender;
    p->headicon = a->headicon;
    p->npc_id = a->npc_id;
    p->combat_level = a->combat_level;
    p->skill_level = a->skill_level;
    for (i = 0; i < 5; i++) {
        p->colours[i] = a->colours[i];
    }
    for (i = 0; i < 7; i++) {
        p->styles[i] = a->styles[i];
        p->anims[i] = a->anims[i];
    }
    copy_text(p->name, sizeof(p->name), a->name);
}

static void player_to_appearance(rs_appearance *a, const rs_sync_player *p) {
    int i;
    memset(a, 0, sizeof(*a));
    a->gender = p->gender;
    a->headicon = p->headicon;
    a->npc_id = p->npc_id;
    a->combat_level = p->combat_level;
    a->skill_level = p->skill_level;
    for (i = 0; i < 5; i++) {
        a->colours[i] = p->colours[i];
    }
    for (i = 0; i < 7; i++) {
        a->styles[i] = p->styles[i];
        a->anims[i] = p->anims[i];
    }
    copy_text(a->name, sizeof(a->name), p->name);
}

static rs_err open_view(int revision, rs_sync_view **out) {
    rs_sync_view *view = rs_sync_view_create(revision);
    rs_err e;
    if (!view) {
        return RS_ERR_NOMEM;
    }
    e = rs_sync_set_local(view, 0);
    if (e != RS_OK) {
        rs_sync_view_destroy(view);
        return e;
    }
    *out = view;
    return RS_OK;
}

rs_err rs_bridge_player_encode(const rs_session *s, rs_buf *dst, const rs_player_update *v) {
    rs_sync_view *view = NULL;
    rs_sync_player player;
    int rev;
    int logical;
    rs_err e = bridge_in(s, dst, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    logical = v->mask;
    if (v->include_appearance) {
        logical |= RS_PLAYER_APPEARANCE;
    }
    if (logical & ~RS_PLAYER_KNOWN) {
        return RS_ERR_UNSUPPORTED;
    }
    e = open_view(rev, &view);
    if (e != RS_OK) {
        return e;
    }
    memset(&player, 0, sizeof(player));
    player.active = 1;
    player.x = v->local_x;
    player.z = v->local_z;
    player.plane = v->plane;
    player.warp = v->teleport ? 1 : 0;
    player.dir1 = -1;
    player.dir2 = -1;
    player.mask = rs_player_wire_mask(rev, logical);
    appearance_to_player(&player, &v->appearance);
    e = rs_sync_player_set(view, 0, &player);
    if (e == RS_OK) {
        e = rs_sync_player_encode(view, dst);
    }
    rs_sync_view_destroy(view);
    return e;
}

rs_err rs_bridge_player_decode(const rs_session *s, rs_buf *src, rs_player_update *v) {
    rs_sync_view *view = NULL;
    rs_sync_player player;
    int rev;
    int tele = 0;
    rs_err e = bridge_in(s, src, v, &rev);
    if (e != RS_OK) {
        return e;
    }
    /* 317, 350, and 377 all put the update flag and move type in the first three bits. */
    if (rs_buf_remaining(src) > 0) {
        int b0 = src->data[src->pos] & 0xff;
        if ((b0 & 0x80) && ((b0 >> 5) & 3) == 3) {
            tele = 1;
        }
    }
    e = open_view(rev, &view);
    if (e != RS_OK) {
        return e;
    }
    e = rs_sync_player_decode(view, src);
    if (e == RS_OK) {
        e = rs_sync_player_get(view, 0, &player);
    }
    rs_sync_view_destroy(view);
    if (e != RS_OK) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    v->teleport = tele;
    v->plane = player.plane;
    v->local_x = player.x;
    v->local_z = player.z;
    v->mask = logical_player_mask(rev, player.mask);
    if (v->mask & RS_PLAYER_APPEARANCE) {
        v->include_appearance = 1;
        player_to_appearance(&v->appearance, &player);
    }
    return RS_OK;
}

rs_err rs_bridge_npc_empty(const rs_session *s, rs_buf *dst) {
    rs_sync_view *view = NULL;
    rs_sync_player player;
    int rev;
    rs_err e;
    if (!dst) {
        return RS_ERR_ARG;
    }
    e = bridge_revision(s, &rev);
    if (e != RS_OK) {
        return e;
    }
    e = open_view(rev, &view);
    if (e != RS_OK) {
        return e;
    }
    memset(&player, 0, sizeof(player));
    player.active = 1;
    player.dir1 = -1;
    player.dir2 = -1;
    e = rs_sync_player_set(view, 0, &player);
    if (e == RS_OK) {
        e = rs_sync_npc_encode(view, dst);
    }
    rs_sync_view_destroy(view);
    return e;
}
