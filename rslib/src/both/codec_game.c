#include "both/codec_impl.h"

#define NEED(expr)                          \
    do {                                    \
        rs_err _e = (expr);                 \
        if (_e) {                           \
            return _e;                      \
        }                                   \
    } while (0)

static int zone_off(int x, int z) {
    return ((x & 7) << 4) | (z & 7);
}

static int id16(int id) {
    return id < 0 ? 65535 : id;
}

static int id16_in(int v) {
    return v == 65535 ? -1 : v;
}

static void name_in(uint64_t v, char out[13]) {
    if (v == 0) {
        out[0] = 0;
    } else {
        rs_base37_decode(v, out);
    }
}

/* 317 packet 8: g2_alt3 component, g2 model. */
rs_err rs_if_model_encode_317(const rs_session *s, rs_buf *dst, const rs_if_model *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2_le_add128(dst, v->component));
    return rs_p2(dst, v->model);
}
rs_err rs_if_model_decode_317(const rs_session *s, rs_buf *src, rs_if_model *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->component = rs_g2_le_add128(src);
    v->model = rs_g2(src);
    return RS_OK;
}

/* 317 packet 75: g2_alt3 npc, g2_alt3 component. */
rs_err rs_if_npc_head_encode_317(const rs_session *s, rs_buf *dst, const rs_if_npc_head *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2_le_add128(dst, v->npc));
    return rs_p2_le_add128(dst, v->component);
}
rs_err rs_if_npc_head_decode_317(const rs_session *s, rs_buf *src, rs_if_npc_head *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->npc = rs_g2_le_add128(src);
    v->component = rs_g2_le_add128(src);
    return RS_OK;
}

/* 317 packet 185. */
rs_err rs_if_player_head_encode_317(const rs_session *s, rs_buf *dst, const rs_if_player_head *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p2_le_add128(dst, v->component);
}
rs_err rs_if_player_head_decode_317(const rs_session *s, rs_buf *src, rs_if_player_head *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->component = rs_g2_le_add128(src);
    return RS_OK;
}

/* 317 packet 122. colour is the 15-bit wire value. */
rs_err rs_if_colour_encode_317(const rs_session *s, rs_buf *dst, const rs_if_colour *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2_le_add128(dst, v->component));
    return rs_p2_le_add128(dst, v->colour);
}
rs_err rs_if_colour_decode_317(const rs_session *s, rs_buf *src, rs_if_colour *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->component = rs_g2_le_add128(src);
    v->colour = rs_g2_le_add128(src);
    return RS_OK;
}

/* 317 packet 171: g1 hidden, g2 component. */
rs_err rs_if_hide_encode_317(const rs_session *s, rs_buf *dst, const rs_if_hide *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1(dst, v->hidden ? 1 : 0));
    return rs_p2(dst, v->component);
}
rs_err rs_if_hide_decode_317(const rs_session *s, rs_buf *src, rs_if_hide *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->hidden = rs_g1(src) == 1;
    v->component = rs_g2(src);
    return RS_OK;
}

/* 317 packet 246. obj 65535 clears the model. */
rs_err rs_if_object_encode_317(const rs_session *s, rs_buf *dst, const rs_if_object *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2_le(dst, v->component));
    NEED(rs_p2(dst, v->zoom));
    return rs_p2(dst, id16(v->obj));
}
rs_err rs_if_object_decode_317(const rs_session *s, rs_buf *src, rs_if_object *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->component = rs_g2_le(src);
    v->zoom = rs_g2(src);
    v->obj = id16_in(rs_g2(src));
    return RS_OK;
}

/* 317 packet 200: g2 component, g2b anim. */
rs_err rs_if_anim_encode_317(const rs_session *s, rs_buf *dst, const rs_if_anim *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2(dst, v->component));
    return rs_p2(dst, v->anim);
}
rs_err rs_if_anim_decode_317(const rs_session *s, rs_buf *src, rs_if_anim *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->component = rs_g2(src);
    v->anim = rs_g2b(src);
    return RS_OK;
}

/* 317 packet 230: g2_alt2 zoom, g2 component, g2 xan, g2_alt3 yan. */
rs_err rs_if_angle_encode_317(const rs_session *s, rs_buf *dst, const rs_if_angle *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2_add128(dst, v->zoom));
    NEED(rs_p2(dst, v->component));
    NEED(rs_p2(dst, v->xan));
    return rs_p2_le_add128(dst, v->yan);
}
rs_err rs_if_angle_decode_317(const rs_session *s, rs_buf *src, rs_if_angle *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->zoom = rs_g2_add128(src);
    v->component = rs_g2(src);
    v->xan = rs_g2(src);
    v->yan = rs_g2_le_add128(src);
    return RS_OK;
}

/* 317 packet 70: g2b x, g2b_alt1 y, g2_alt1 component. */
rs_err rs_if_pos_encode_317(const rs_session *s, rs_buf *dst, const rs_if_pos *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2(dst, v->x));
    NEED(rs_p2_le(dst, v->y));
    return rs_p2_le(dst, v->component);
}
rs_err rs_if_pos_decode_317(const rs_session *s, rs_buf *src, rs_if_pos *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->x = rs_g2b(src);
    v->y = rs_g2b_le(src);
    v->component = rs_g2_le(src);
    return RS_OK;
}

/* 317 packet 79. */
rs_err rs_if_scroll_encode_317(const rs_session *s, rs_buf *dst, const rs_if_scroll *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2_le(dst, v->component));
    return rs_p2_add128(dst, v->position);
}
rs_err rs_if_scroll_decode_317(const rs_session *s, rs_buf *src, rs_if_scroll *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->component = rs_g2_le(src);
    v->position = rs_g2_add128(src);
    return RS_OK;
}

/* 317 packet 142. */
rs_err rs_if_sub_encode_317(const rs_session *s, rs_buf *dst, const rs_if_sub *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p2_le(dst, v->id);
}
rs_err rs_if_sub_decode_317(const rs_session *s, rs_buf *src, rs_if_sub *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->id = rs_g2_le(src);
    return RS_OK;
}

/* 317 packet 248: g2_alt2 viewport, g2 sidebar. */
rs_err rs_if_top_encode_317(const rs_session *s, rs_buf *dst, const rs_if_top *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2_add128(dst, v->viewport));
    return rs_p2(dst, v->sidebar);
}
rs_err rs_if_top_decode_317(const rs_session *s, rs_buf *src, rs_if_top *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->viewport = rs_g2_add128(src);
    v->sidebar = rs_g2(src);
    return RS_OK;
}

/* 317 packet 72. */
rs_err rs_inv_clear_encode_317(const rs_session *s, rs_buf *dst, const rs_inv_clear *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p2_le(dst, v->component);
}
rs_err rs_inv_clear_decode_317(const rs_session *s, rs_buf *src, rs_inv_clear *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->component = rs_g2_le(src);
    return RS_OK;
}

/* 317 packet 34: g2 component, then gsmarts slot, g2 id, g1 or 255+g4 amount. */
rs_err rs_inv_update_encode_317(const rs_session *s, rs_buf *dst, const rs_inv_update *v) {
    int i;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2(dst, v->component));
    for (i = 0; i < v->nchanges && i < RS_INV_SLOTS; i++) {
        int amt = v->changes[i].amount;
        if (v->changes[i].slot < 0) {
            return RS_ERR_ARG;
        }
        if (amt < 0) {
            amt = 0;
        }
        NEED(rs_psmarts(dst, v->changes[i].slot));
        NEED(rs_p2(dst, v->changes[i].id));
        if (amt >= 255) {
            NEED(rs_p1(dst, 255));
            NEED(rs_p4(dst, amt));
        } else {
            NEED(rs_p1(dst, amt));
        }
    }
    return RS_OK;
}
rs_err rs_inv_update_decode_317(const rs_session *s, rs_buf *src, rs_inv_update *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->component = rs_g2(src);
    while (v->nchanges < RS_INV_SLOTS && rs_buf_remaining(src) > 0) {
        int wide = (src->data[src->pos] & 0xff) >= 128;
        size_t smart = wide ? 2u : 1u;
        int amt;
        if (rs_buf_remaining(src) < smart + 3) {
            break;
        }
        if ((src->data[src->pos + smart + 2] & 0xff) == 255 &&
            rs_buf_remaining(src) < smart + 7) {
            break;
        }
        v->changes[v->nchanges].slot = rs_gsmarts(src);
        v->changes[v->nchanges].id = rs_g2(src);
        amt = rs_g1(src);
        if (amt == 255) {
            amt = rs_g4(src);
        }
        v->changes[v->nchanges].amount = amt;
        v->nchanges++;
    }
    return RS_OK;
}

/* 317 packet 74. id < 0 writes 65535. */
rs_err rs_song_encode_317(const rs_session *s, rs_buf *dst, const rs_song *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p2_le(dst, id16(v->id));
}
rs_err rs_song_decode_317(const rs_session *s, rs_buf *src, rs_song *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->id = id16_in(rs_g2_le(src));
    return RS_OK;
}

/* 317 packet 121: g2_alt3 id, g2_alt2 delay. */
rs_err rs_jingle_encode_317(const rs_session *s, rs_buf *dst, const rs_jingle *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2_le_add128(dst, v->id));
    return rs_p2_add128(dst, v->delay);
}
rs_err rs_jingle_decode_317(const rs_session *s, rs_buf *src, rs_jingle *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->id = rs_g2_le_add128(src);
    v->delay = rs_g2_add128(src);
    return RS_OK;
}

/* 317 packet 105: offset, id, (radius << 4) | loops. */
rs_err rs_sound_area_encode_317(const rs_session *s, rs_buf *dst, const rs_sound_area *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1(dst, zone_off(v->x, v->z)));
    NEED(rs_p2(dst, v->id));
    return rs_p1(dst, ((v->radius & 15) << 4) | (v->loops & 7));
}
rs_err rs_sound_area_decode_317(const rs_session *s, rs_buf *src, rs_sound_area *v) {
    int off, packed;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    off = rs_g1(src);
    v->x = (off >> 4) & 7;
    v->z = off & 7;
    v->id = rs_g2(src);
    packed = rs_g1(src);
    v->radius = (packed >> 4) & 15;
    v->loops = packed & 7;
    return RS_OK;
}

/* 317 packet 160: g1_alt3 offset, g1_alt3 (shape << 2) | rot, g2_alt2 seq. */
rs_err rs_loc_anim_encode_317(const rs_session *s, rs_buf *dst, const rs_loc_anim *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1_sub128(dst, zone_off(v->x, v->z)));
    NEED(rs_p1_sub128(dst, ((v->shape & 31) << 2) | (v->rot & 3)));
    return rs_p2_add128(dst, v->seq);
}
rs_err rs_loc_anim_decode_317(const rs_session *s, rs_buf *src, rs_loc_anim *v) {
    int off, packed;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    off = rs_g1_sub128(src);
    v->x = (off >> 4) & 7;
    v->z = off & 7;
    packed = rs_g1_sub128(src);
    v->shape = (packed >> 2) & 31;
    v->rot = packed & 3;
    v->seq = rs_g2_add128(src);
    return RS_OK;
}

/* 317 packet 35. */
rs_err rs_cam_shake_encode_317(const rs_session *s, rs_buf *dst, const rs_cam_shake *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1(dst, v->index));
    NEED(rs_p1(dst, v->jitter));
    NEED(rs_p1(dst, v->wobble));
    return rs_p1(dst, v->speed);
}
rs_err rs_cam_shake_decode_317(const rs_session *s, rs_buf *src, rs_cam_shake *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->index = rs_g1(src);
    v->jitter = rs_g1(src);
    v->wobble = rs_g1(src);
    v->speed = rs_g1(src);
    return RS_OK;
}

static rs_err camera_encode(rs_buf *dst, const rs_camera *v) {
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1(dst, v->x));
    NEED(rs_p1(dst, v->z));
    NEED(rs_p2(dst, v->height));
    NEED(rs_p1(dst, v->speed));
    return rs_p1(dst, v->accel);
}
static rs_err camera_decode(rs_buf *src, rs_camera *v) {
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->x = rs_g1(src);
    v->z = rs_g1(src);
    v->height = rs_g2(src);
    v->speed = rs_g1(src);
    v->accel = rs_g1(src);
    return RS_OK;
}

/* 317 packet 166. */
rs_err rs_camera_move_encode_317(const rs_session *s, rs_buf *dst, const rs_camera *v) {
    (void)s;
    return camera_encode(dst, v);
}
rs_err rs_camera_move_decode_317(const rs_session *s, rs_buf *src, rs_camera *v) {
    (void)s;
    return camera_decode(src, v);
}

/* 317 packet 177. Same bytes as camera move, different meaning. */
rs_err rs_cam_lookat_encode_317(const rs_session *s, rs_buf *dst, const rs_camera *v) {
    (void)s;
    return camera_encode(dst, v);
}
rs_err rs_cam_lookat_decode_317(const rs_session *s, rs_buf *src, rs_camera *v) {
    (void)s;
    return camera_decode(src, v);
}

rs_err rs_minimap_encode_317(const rs_session *s, rs_buf *dst, const rs_minimap *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p1(dst, v->state);
}
rs_err rs_minimap_decode_317(const rs_session *s, rs_buf *src, rs_minimap *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->state = rs_g1(src);
    return RS_OK;
}

rs_err rs_multizone_encode_317(const rs_session *s, rs_buf *dst, const rs_multizone *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p1(dst, v->state);
}
rs_err rs_multizone_decode_317(const rs_session *s, rs_buf *src, rs_multizone *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->state = rs_g1(src);
    return RS_OK;
}

/* 317 packet 64: g1_alt2 x, g1_alt3 z. */
rs_err rs_zone_full_encode_317(const rs_session *s, rs_buf *dst, const rs_zone_full *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1_neg(dst, v->x));
    return rs_p1_sub128(dst, v->z);
}
rs_err rs_zone_full_decode_317(const rs_session *s, rs_buf *src, rs_zone_full *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->x = rs_g1_neg(src);
    v->z = rs_g1_sub128(src);
    return RS_OK;
}

/* 317 packet 60 header. The tail is more zone packets and stays in the buffer. */
rs_err rs_zone_partial_encode_317(const rs_session *s, rs_buf *dst, const rs_zone_partial *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1(dst, v->z));
    return rs_p1_neg(dst, v->x);
}
rs_err rs_zone_partial_decode_317(const rs_session *s, rs_buf *src, rs_zone_partial *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->z = rs_g1(src);
    v->x = rs_g1_neg(src);
    return RS_OK;
}

/* 317 packet 114. timer is the wire short; the client multiplies it by 30. */
rs_err rs_system_update_encode_317(const rs_session *s, rs_buf *dst, const rs_system_update *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p2_le(dst, v->timer);
}
rs_err rs_system_update_decode_317(const rs_session *s, rs_buf *src, rs_system_update *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->timer = rs_g2_le(src);
    return RS_OK;
}

/* 317 packet 206. */
rs_err rs_chat_settings_encode_317(const rs_session *s, rs_buf *dst, const rs_chat_settings *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1(dst, v->public_chat));
    NEED(rs_p1(dst, v->private_chat));
    return rs_p1(dst, v->trade);
}
rs_err rs_chat_settings_decode_317(const rs_session *s, rs_buf *src, rs_chat_settings *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->public_chat = rs_g1(src);
    v->private_chat = rs_g1(src);
    v->trade = rs_g1(src);
    return RS_OK;
}

/* 317 packet 71. interface_id < 0 writes 65535. */
rs_err rs_sidebar_encode_317(const rs_session *s, rs_buf *dst, const rs_sidebar *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2(dst, id16(v->interface_id)));
    return rs_p1_add128(dst, v->tab);
}
rs_err rs_sidebar_decode_317(const rs_session *s, rs_buf *src, rs_sidebar *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->interface_id = id16_in(rs_g2(src));
    v->tab = rs_g1_add128(src);
    return RS_OK;
}

/* 317 packet 106. */
rs_err rs_tab_encode_317(const rs_session *s, rs_buf *dst, const rs_tab *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p1_neg(dst, v->tab);
}
rs_err rs_tab_decode_317(const rs_session *s, rs_buf *src, rs_tab *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->tab = rs_g1_neg(src);
    return RS_OK;
}

/* 317 packet 208. Signed. */
rs_err rs_walkable_encode_317(const rs_session *s, rs_buf *dst, const rs_walkable *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p2_le(dst, v->id);
}
rs_err rs_walkable_decode_317(const rs_session *s, rs_buf *src, rs_walkable *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->id = rs_g2b_le(src);
    return RS_OK;
}

/* 317 packet 176. */
rs_err rs_last_login_encode_317(const rs_session *s, rs_buf *dst, const rs_last_login *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1_neg(dst, v->days_since_recovery));
    NEED(rs_p2_add128(dst, v->unread));
    NEED(rs_p1(dst, v->members_warning));
    NEED(rs_p4_alt3(dst, v->address));
    return rs_p2(dst, v->days_since_login);
}
rs_err rs_last_login_decode_317(const rs_session *s, rs_buf *src, rs_last_login *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->days_since_recovery = rs_g1_neg(src);
    v->unread = rs_g2_add128(src);
    v->members_warning = rs_g1(src);
    v->address = rs_g4_alt3(src);
    v->days_since_login = rs_g2(src);
    return RS_OK;
}

/* 317 packet 50: one friend, g8 name, g1 world. */
rs_err rs_friend_entry_encode_317(const rs_session *s, rs_buf *dst, const rs_friend_entry *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p8(dst, rs_base37_encode(v->name)));
    return rs_p1(dst, v->world);
}
rs_err rs_friend_entry_decode_317(const rs_session *s, rs_buf *src, rs_friend_entry *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    name_in(rs_g8(src), v->name);
    v->world = rs_g1(src);
    return RS_OK;
}

/* 317 packet 214: packet length / 8 names, each g8. */
rs_err rs_ignore_list_encode_317(const rs_session *s, rs_buf *dst, const rs_ignore_list *v) {
    int i, n;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    n = v->count;
    if (n > RS_IGNORE_MAX) {
        n = RS_IGNORE_MAX;
    }
    for (i = 0; i < n; i++) {
        NEED(rs_p8(dst, rs_base37_encode(v->names[i])));
    }
    return RS_OK;
}
rs_err rs_ignore_list_decode_317(const rs_session *s, rs_buf *src, rs_ignore_list *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    while (v->count < RS_IGNORE_MAX && rs_buf_remaining(src) >= 8) {
        name_in(rs_g8(src), v->names[v->count]);
        v->count++;
    }
    return RS_OK;
}

/* 317 packet 196: g8 name, g4 id, g1 rights, wordpack. */
rs_err rs_private_message_encode_317(const rs_session *s, rs_buf *dst, const rs_private_message *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p8(dst, rs_base37_encode(v->name)));
    NEED(rs_p4(dst, v->message_id));
    NEED(rs_p1(dst, v->rights));
    return rs_wordpack_encode(v->text, dst);
}
rs_err rs_private_message_decode_317(const rs_session *s, rs_buf *src, rs_private_message *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    name_in(rs_g8(src), v->name);
    v->message_id = rs_g4(src);
    v->rights = rs_g1(src);
    return rs_wordpack_decode(src, (int)rs_buf_remaining(src), v->text, sizeof(v->text));
}

/* 317 packet 254. type 2..6 keep the wire type; the client derives the offset. */
rs_err rs_hint_encode_317(const rs_session *s, rs_buf *dst, const rs_hint *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1(dst, v->type));
    if (v->type == 1) {
        return rs_p2(dst, v->npc);
    }
    if (v->type >= 2 && v->type <= 6) {
        NEED(rs_p2(dst, v->tile_x));
        NEED(rs_p2(dst, v->tile_z));
        return rs_p1(dst, v->height);
    }
    if (v->type == 10) {
        return rs_p2(dst, v->player);
    }
    return RS_OK;
}
rs_err rs_hint_decode_317(const rs_session *s, rs_buf *src, rs_hint *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->type = rs_g1(src);
    if (v->type == 1) {
        v->npc = rs_g2(src);
    } else if (v->type >= 2 && v->type <= 6) {
        v->tile_x = rs_g2(src);
        v->tile_z = rs_g2(src);
        v->height = rs_g1(src);
    } else if (v->type == 10) {
        v->player = rs_g2(src);
    }
    return RS_OK;
}

/*
 * 317 packet 241. chunks[plane][z][x] < 0 is absent.
 * Wire order is g2_alt2 zone_z, 4*13*13 presence bits, then g2 zone_x.
 */
rs_err rs_region_instanced_encode_317(const rs_session *s, rs_buf *dst, const rs_region_instanced *v) {
    int p, z, x;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p2_add128(dst, v->zone_z));
    rs_access_bits(dst);
    for (p = 0; p < RS_INSTANCED_PLANES; p++) {
        for (z = 0; z < RS_INSTANCED_SIZE; z++) {
            for (x = 0; x < RS_INSTANCED_SIZE; x++) {
                int chunk = v->chunks[p][z][x];
                if (chunk < 0) {
                    NEED(rs_pbit(dst, 1, 0));
                } else {
                    NEED(rs_pbit(dst, 1, 1));
                    NEED(rs_pbit(dst, 26, chunk));
                }
            }
        }
    }
    rs_access_bytes(dst);
    return rs_p2(dst, v->zone_x);
}
rs_err rs_region_instanced_decode_317(const rs_session *s, rs_buf *src, rs_region_instanced *v) {
    int p, z, x;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->zone_z = rs_g2_add128(src);
    rs_access_bits(src);
    for (p = 0; p < RS_INSTANCED_PLANES; p++) {
        for (z = 0; z < RS_INSTANCED_SIZE; z++) {
            for (x = 0; x < RS_INSTANCED_SIZE; x++) {
                if (rs_gbit(src, 1)) {
                    v->chunks[p][z][x] = rs_gbit(src, 26);
                } else {
                    v->chunks[p][z][x] = -1;
                }
            }
        }
    }
    rs_access_bytes(src);
    v->zone_x = rs_g2(src);
    return RS_OK;
}

/* 317 c2s command, opcode 103: jagex string. */
rs_err rs_command_encode_317(const rs_session *s, rs_buf *dst, const rs_command *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_pjstr(dst, v->text);
}
rs_err rs_command_decode_317(const rs_session *s, rs_buf *src, rs_command *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    return rs_gjstr(src, v->text, sizeof(v->text));
}

/* 317 c2s private message, opcode 126: g8 name, wordpack. */
rs_err rs_private_out_encode_317(const rs_session *s, rs_buf *dst, const rs_private_out *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p8(dst, rs_base37_encode(v->name)));
    return rs_wordpack_encode(v->text, dst);
}
rs_err rs_private_out_decode_317(const rs_session *s, rs_buf *src, rs_private_out *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    name_in(rs_g8(src), v->name);
    return rs_wordpack_decode(src, (int)rs_buf_remaining(src), v->text, sizeof(v->text));
}

/* 317 c2s chat mode, opcode 95. */
rs_err rs_chat_mode_encode_317(const rs_session *s, rs_buf *dst, const rs_chat_mode *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1(dst, v->public_chat));
    NEED(rs_p1(dst, v->private_chat));
    return rs_p1(dst, v->trade);
}
rs_err rs_chat_mode_decode_317(const rs_session *s, rs_buf *src, rs_chat_mode *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->public_chat = rs_g1(src);
    v->private_chat = rs_g1(src);
    v->trade = rs_g1(src);
    return RS_OK;
}

/* 317 c2s appearance, opcode 101. */
rs_err rs_design_encode_317(const rs_session *s, rs_buf *dst, const rs_design *v) {
    int i;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1(dst, v->gender));
    for (i = 0; i < 7; i++) {
        NEED(rs_p1(dst, v->styles[i]));
    }
    for (i = 0; i < 5; i++) {
        NEED(rs_p1(dst, v->colours[i]));
    }
    return RS_OK;
}
rs_err rs_design_decode_317(const rs_session *s, rs_buf *src, rs_design *v) {
    int i;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->gender = rs_g1(src);
    for (i = 0; i < 7; i++) {
        v->styles[i] = rs_g1(src);
    }
    for (i = 0; i < 5; i++) {
        v->colours[i] = rs_g1(src);
    }
    return RS_OK;
}

/* 317 c2s chat, opcode 4: g1_alt3 colour, g1_alt3 effect, wordpack. */
rs_err rs_chat_in_encode_317(const rs_session *s, rs_buf *dst, const rs_chat_in *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    NEED(rs_p1_sub128(dst, v->colour));
    NEED(rs_p1_sub128(dst, v->effect));
    return rs_wordpack_encode(v->text, dst);
}
rs_err rs_chat_in_decode_317(const rs_session *s, rs_buf *src, rs_chat_in *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->colour = rs_g1_sub128(src);
    v->effect = rs_g1_sub128(src);
    return rs_wordpack_decode(src, (int)rs_buf_remaining(src), v->text, sizeof(v->text));
}

rs_err rs_chat_in_encode_plain(const rs_session *s, rs_buf *dst, const rs_chat_in *v) {
    if (s && s->build && s->build->era == RS_ERA_RSC) {
        return RS_ERR_UNSUPPORTED;
    }
    return rs_chat_in_encode_317(s, dst, v);
}
rs_err rs_chat_in_decode_plain(const rs_session *s, rs_buf *src, rs_chat_in *v) {
    if (s && s->build && s->build->era == RS_ERA_RSC) {
        return RS_ERR_UNSUPPORTED;
    }
    return rs_chat_in_decode_317(s, src, v);
}

/* JS5 public chat body. The 317 colour/effect header is not on this packet. */
rs_err rs_chat_in_encode_js5(const rs_session *s, rs_buf *dst, const rs_chat_in *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_wordpack_encode(v->text, dst);
}
rs_err rs_chat_in_decode_js5(const rs_session *s, rs_buf *src, rs_chat_in *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    return rs_wordpack_decode(src, (int)rs_buf_remaining(src), v->text, sizeof(v->text));
}

#define RS_CODEC_DISPATCH(slot, ...)                \
    do {                                            \
        const rs_codec_ops *ops_ = rs_codec_use(s); \
        if (!ops_ || !ops_->slot) {                 \
            return RS_ERR_UNSUPPORTED;              \
        }                                           \
        return ops_->slot(__VA_ARGS__);            \
    } while (0)

#define PUB(enc, dec, slot_e, slot_d, ty)                                           \
    rs_err enc(const rs_session *s, rs_buf *dst, const ty *v) {                     \
        RS_CODEC_DISPATCH(slot_e, s, dst, v);                                       \
    }                                                                               \
    rs_err dec(const rs_session *s, rs_buf *src, ty *v) {                           \
        RS_CODEC_DISPATCH(slot_d, s, src, v);                                       \
    }

PUB(rs_codec_if_model_encode, rs_codec_if_model_decode, if_model_encode, if_model_decode, rs_if_model)
PUB(rs_codec_if_npc_head_encode, rs_codec_if_npc_head_decode, if_npc_head_encode, if_npc_head_decode, rs_if_npc_head)
PUB(rs_codec_if_player_head_encode, rs_codec_if_player_head_decode, if_player_head_encode, if_player_head_decode, rs_if_player_head)
PUB(rs_codec_if_colour_encode, rs_codec_if_colour_decode, if_colour_encode, if_colour_decode, rs_if_colour)
PUB(rs_codec_if_hide_encode, rs_codec_if_hide_decode, if_hide_encode, if_hide_decode, rs_if_hide)
PUB(rs_codec_if_object_encode, rs_codec_if_object_decode, if_object_encode, if_object_decode, rs_if_object)
PUB(rs_codec_if_anim_encode, rs_codec_if_anim_decode, if_anim_encode, if_anim_decode, rs_if_anim)
PUB(rs_codec_if_angle_encode, rs_codec_if_angle_decode, if_angle_encode, if_angle_decode, rs_if_angle)
PUB(rs_codec_if_pos_encode, rs_codec_if_pos_decode, if_pos_encode, if_pos_decode, rs_if_pos)
PUB(rs_codec_if_scroll_encode, rs_codec_if_scroll_decode, if_scroll_encode, if_scroll_decode, rs_if_scroll)
PUB(rs_codec_if_sub_encode, rs_codec_if_sub_decode, if_sub_encode, if_sub_decode, rs_if_sub)
PUB(rs_codec_if_top_encode, rs_codec_if_top_decode, if_top_encode, if_top_decode, rs_if_top)
PUB(rs_codec_inv_clear_encode, rs_codec_inv_clear_decode, inv_clear_encode, inv_clear_decode, rs_inv_clear)
PUB(rs_codec_inv_update_encode, rs_codec_inv_update_decode, inv_update_encode, inv_update_decode, rs_inv_update)
PUB(rs_codec_song_encode, rs_codec_song_decode, song_encode, song_decode, rs_song)
PUB(rs_codec_jingle_encode, rs_codec_jingle_decode, jingle_encode, jingle_decode, rs_jingle)
PUB(rs_codec_sound_area_encode, rs_codec_sound_area_decode, sound_area_encode, sound_area_decode, rs_sound_area)
PUB(rs_codec_loc_anim_encode, rs_codec_loc_anim_decode, loc_anim_encode, loc_anim_decode, rs_loc_anim)
PUB(rs_codec_cam_shake_encode, rs_codec_cam_shake_decode, cam_shake_encode, cam_shake_decode, rs_cam_shake)
PUB(rs_codec_camera_move_encode, rs_codec_camera_move_decode, camera_move_encode, camera_move_decode, rs_camera)
PUB(rs_codec_cam_lookat_encode, rs_codec_cam_lookat_decode, cam_lookat_encode, cam_lookat_decode, rs_camera)
PUB(rs_codec_minimap_encode, rs_codec_minimap_decode, minimap_encode, minimap_decode, rs_minimap)
PUB(rs_codec_multizone_encode, rs_codec_multizone_decode, multizone_encode, multizone_decode, rs_multizone)
PUB(rs_codec_zone_full_encode, rs_codec_zone_full_decode, zone_full_encode, zone_full_decode, rs_zone_full)
PUB(rs_codec_zone_partial_encode, rs_codec_zone_partial_decode, zone_partial_encode, zone_partial_decode, rs_zone_partial)
PUB(rs_codec_system_update_encode, rs_codec_system_update_decode, system_update_encode, system_update_decode, rs_system_update)
PUB(rs_codec_chat_settings_encode, rs_codec_chat_settings_decode, chat_settings_encode, chat_settings_decode, rs_chat_settings)
PUB(rs_codec_sidebar_encode, rs_codec_sidebar_decode, sidebar_encode, sidebar_decode, rs_sidebar)
PUB(rs_codec_tab_encode, rs_codec_tab_decode, tab_encode, tab_decode, rs_tab)
PUB(rs_codec_walkable_encode, rs_codec_walkable_decode, walkable_encode, walkable_decode, rs_walkable)
PUB(rs_codec_last_login_encode, rs_codec_last_login_decode, last_login_encode, last_login_decode, rs_last_login)
PUB(rs_codec_friend_entry_encode, rs_codec_friend_entry_decode, friend_entry_encode, friend_entry_decode, rs_friend_entry)
PUB(rs_codec_ignore_list_encode, rs_codec_ignore_list_decode, ignore_list_encode, ignore_list_decode, rs_ignore_list)
PUB(rs_codec_private_message_encode, rs_codec_private_message_decode, private_message_encode, private_message_decode, rs_private_message)
PUB(rs_codec_hint_encode, rs_codec_hint_decode, hint_encode, hint_decode, rs_hint)
PUB(rs_codec_region_instanced_encode, rs_codec_region_instanced_decode, region_instanced_encode, region_instanced_decode, rs_region_instanced)
PUB(rs_codec_command_encode, rs_codec_command_decode, command_encode, command_decode, rs_command)
PUB(rs_codec_private_out_encode, rs_codec_private_out_decode, private_out_encode, private_out_decode, rs_private_out)
PUB(rs_codec_chat_mode_encode, rs_codec_chat_mode_decode, chat_mode_encode, chat_mode_decode, rs_chat_mode)
PUB(rs_codec_design_encode, rs_codec_design_decode, design_encode, design_decode, rs_design)
PUB(rs_codec_chat_in_encode, rs_codec_chat_in_decode, chat_in_encode, chat_in_decode, rs_chat_in)

#undef PUB
#undef RS_CODEC_DISPATCH
#undef NEED
