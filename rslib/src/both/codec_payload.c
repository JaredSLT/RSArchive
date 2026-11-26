#include "both/codec_impl.h"

static int zone_off(int x, int z) {
    return ((x & 7) << 4) | (z & 7);
}

#define RS_CODEC_DISPATCH(slot, ...)                \
    do {                                            \
        const rs_codec_ops *ops_ = rs_codec_use(s); \
        if (!ops_ || !ops_->slot) {                 \
            return RS_ERR_UNSUPPORTED;              \
        }                                           \
        return ops_->slot(__VA_ARGS__);            \
    } while (0)

/* 317 packet 4: g1 offset, g2 id, g1 height, g2 delay. */
rs_err rs_spotanim_encode_317(const rs_session *s, rs_buf *dst, const rs_spotanim *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p1(dst, zone_off(v->x, v->z));
    if (e) {
        return e;
    }
    e = rs_p2(dst, v->id);
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->height);
    if (e) {
        return e;
    }
    return rs_p2(dst, v->delay);
}

rs_err rs_spotanim_decode_317(const rs_session *s, rs_buf *src, rs_spotanim *v) {
    int off;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    off = rs_g1(src);
    v->x = (off >> 4) & 7;
    v->z = off & 7;
    v->id = rs_g2(src);
    v->height = rs_g1(src);
    v->delay = rs_g2(src);
    return RS_OK;
}

/*
 * 317 packet 117: offset, signed dx/dz, signed target, graphic, two height
 * bytes, two delays, angle, steepness.
 */
rs_err rs_projectile_encode_317(const rs_session *s, rs_buf *dst, const rs_projectile *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p1(dst, zone_off(v->x, v->z));
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->dx);
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->dz);
    if (e) {
        return e;
    }
    e = rs_p2(dst, v->target);
    if (e) {
        return e;
    }
    e = rs_p2(dst, v->graphic);
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->start_height);
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->end_height);
    if (e) {
        return e;
    }
    e = rs_p2(dst, v->start_delay);
    if (e) {
        return e;
    }
    e = rs_p2(dst, v->end_delay);
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->angle);
    if (e) {
        return e;
    }
    return rs_p1(dst, v->steepness);
}

rs_err rs_projectile_decode_317(const rs_session *s, rs_buf *src, rs_projectile *v) {
    int off;
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    off = rs_g1(src);
    v->x = (off >> 4) & 7;
    v->z = off & 7;
    v->dx = rs_g1b(src);
    v->dz = rs_g1b(src);
    v->target = rs_g2b(src);
    v->graphic = rs_g2(src);
    v->start_height = rs_g1(src);
    v->end_height = rs_g1(src);
    v->start_delay = rs_g2(src);
    v->end_delay = rs_g2(src);
    v->angle = rs_g1(src);
    v->steepness = rs_g1(src);
    return RS_OK;
}

/* 317 packet 174: g2 id, g1 loops, g2 delay. */
rs_err rs_sound_encode_317(const rs_session *s, rs_buf *dst, const rs_sound *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p2(dst, v->id);
    if (e) {
        return e;
    }
    e = rs_p1(dst, v->loops);
    if (e) {
        return e;
    }
    return rs_p2(dst, v->delay);
}

rs_err rs_sound_decode_317(const rs_session *s, rs_buf *src, rs_sound *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->id = rs_g2(src);
    v->loops = rs_g1(src);
    v->delay = rs_g2(src);
    return RS_OK;
}

/* 317 packet 249: g1_alt1 members, g2_alt3 pid. */
rs_err rs_player_index_encode_317(const rs_session *s, rs_buf *dst, const rs_player_index *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p1_add128(dst, v->members);
    if (e) {
        return e;
    }
    return rs_p2_le_add128(dst, v->index);
}

rs_err rs_player_index_decode_317(const rs_session *s, rs_buf *src, rs_player_index *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->members = rs_g1_add128(src);
    v->index = rs_g2_le_add128(src);
    return RS_OK;
}

/* 317 packet 104: g1_alt2 slot, g1_alt1 push flag, string. */
rs_err rs_player_option_encode_317(const rs_session *s, rs_buf *dst, const rs_player_option *v) {
    rs_err e;
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    e = rs_p1_neg(dst, v->slot);
    if (e) {
        return e;
    }
    e = rs_p1_add128(dst, v->push_down);
    if (e) {
        return e;
    }
    return rs_pjstr(dst, v->text);
}

rs_err rs_player_option_decode_317(const rs_session *s, rs_buf *src, rs_player_option *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    memset(v, 0, sizeof(*v));
    v->slot = rs_g1_neg(src);
    v->push_down = rs_g1_add128(src);
    return rs_gjstr(src, v->text, sizeof(v->text));
}

rs_err rs_friend_status_encode_317(const rs_session *s, rs_buf *dst, const rs_friend_status *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p1(dst, v->status);
}

rs_err rs_friend_status_decode_317(const rs_session *s, rs_buf *src, rs_friend_status *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->status = rs_g1(src);
    return RS_OK;
}

rs_err rs_energy_encode_317(const rs_session *s, rs_buf *dst, const rs_energy *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p1(dst, v->energy);
}

rs_err rs_energy_decode_317(const rs_session *s, rs_buf *src, rs_energy *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->energy = rs_g1(src);
    return RS_OK;
}

/* 317 packet 240: g2b weight. */
rs_err rs_weight_encode_317(const rs_session *s, rs_buf *dst, const rs_weight *v) {
    (void)s;
    if (!dst || !v) {
        return RS_ERR_ARG;
    }
    return rs_p2(dst, v->weight);
}

rs_err rs_weight_decode_317(const rs_session *s, rs_buf *src, rs_weight *v) {
    (void)s;
    if (!src || !v) {
        return RS_ERR_ARG;
    }
    v->weight = rs_g2b(src);
    return RS_OK;
}

rs_err rs_codec_spotanim_encode(const rs_session *s, rs_buf *dst, const rs_spotanim *v) {
    RS_CODEC_DISPATCH(spotanim_encode, s, dst, v);
}
rs_err rs_codec_spotanim_decode(const rs_session *s, rs_buf *src, rs_spotanim *v) {
    RS_CODEC_DISPATCH(spotanim_decode, s, src, v);
}
rs_err rs_codec_projectile_encode(const rs_session *s, rs_buf *dst, const rs_projectile *v) {
    RS_CODEC_DISPATCH(projectile_encode, s, dst, v);
}
rs_err rs_codec_projectile_decode(const rs_session *s, rs_buf *src, rs_projectile *v) {
    RS_CODEC_DISPATCH(projectile_decode, s, src, v);
}
rs_err rs_codec_sound_encode(const rs_session *s, rs_buf *dst, const rs_sound *v) {
    RS_CODEC_DISPATCH(sound_encode, s, dst, v);
}
rs_err rs_codec_sound_decode(const rs_session *s, rs_buf *src, rs_sound *v) {
    RS_CODEC_DISPATCH(sound_decode, s, src, v);
}
rs_err rs_codec_player_index_encode(const rs_session *s, rs_buf *dst, const rs_player_index *v) {
    RS_CODEC_DISPATCH(player_index_encode, s, dst, v);
}
rs_err rs_codec_player_index_decode(const rs_session *s, rs_buf *src, rs_player_index *v) {
    RS_CODEC_DISPATCH(player_index_decode, s, src, v);
}
rs_err rs_codec_player_option_encode(const rs_session *s, rs_buf *dst, const rs_player_option *v) {
    RS_CODEC_DISPATCH(player_option_encode, s, dst, v);
}
rs_err rs_codec_player_option_decode(const rs_session *s, rs_buf *src, rs_player_option *v) {
    RS_CODEC_DISPATCH(player_option_decode, s, src, v);
}
rs_err rs_codec_friend_status_encode(const rs_session *s, rs_buf *dst, const rs_friend_status *v) {
    RS_CODEC_DISPATCH(friend_status_encode, s, dst, v);
}
rs_err rs_codec_friend_status_decode(const rs_session *s, rs_buf *src, rs_friend_status *v) {
    RS_CODEC_DISPATCH(friend_status_decode, s, src, v);
}
rs_err rs_codec_energy_encode(const rs_session *s, rs_buf *dst, const rs_energy *v) {
    RS_CODEC_DISPATCH(energy_encode, s, dst, v);
}
rs_err rs_codec_energy_decode(const rs_session *s, rs_buf *src, rs_energy *v) {
    RS_CODEC_DISPATCH(energy_decode, s, src, v);
}
rs_err rs_codec_weight_encode(const rs_session *s, rs_buf *dst, const rs_weight *v) {
    RS_CODEC_DISPATCH(weight_encode, s, dst, v);
}
rs_err rs_codec_weight_decode(const rs_session *s, rs_buf *src, rs_weight *v) {
    RS_CODEC_DISPATCH(weight_decode, s, src, v);
}
