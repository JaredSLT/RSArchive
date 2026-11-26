#include "proto/ops.h"
#include "rslib/both/login_ext.h"

/*
 * RS3 lobby and game login, from 910 LoginManager step 84 (non-social).
 *
 * The RSA block carries an explicit length so both sides can frame it
 * without Jagex's modulus. The tail after that block is XTEA'd with the
 * four isaac seeds, which is what 910's tinyenc does. Opcode 16 is the
 * game login. Opcode 19 is the lobby login (req->lobby).
 *
 * createPreferencesBlock allocates 59 bytes and writes 58 (version 38,
 * then the fields below, including two hardcoded 0 bytes). The length
 * prefix is that written size. Hardware has no length prefix.
 * pushUID192 is 24 bytes; a missing random.dat is 0xFF in the client,
 * and uid192 is written as stored (zeros if unset).
 */

#define W(expr)                 \
    do {                        \
        rs_err _e_ = (expr);    \
        if (_e_) {              \
            return _e_;         \
        }                       \
    } while (0)

enum { RS_PREFS_BYTES = 58, RS_LOGIN_TAIL_MAX = 4096 };

static rs_err lobby_rsa_write(rs_buf *blk, const rs_login_request *req) {
    rs_err e;
    int i;
    e = rs_p1(blk, 10);
    if (e) {
        return e;
    }
    for (i = 0; i < 4; i++) {
        e = rs_p4(blk, (int)req->isaac_seed[i]);
        if (e) {
            return e;
        }
    }
    e = rs_p8(blk, 0);
    if (e) {
        return e;
    }
    e = rs_p1(blk, 2); /* AUTH_NOT_FOUND */
    if (e) {
        return e;
    }
    e = rs_p4(blk, 0);
    if (e) {
        return e;
    }
    e = rs_p1(blk, 0);
    if (e) {
        return e;
    }
    e = rs_pjstrnull(blk, req->password);
    if (e) {
        return e;
    }
    e = rs_p8(blk, 0);
    if (e) {
        return e;
    }
    return rs_p8(blk, 0);
}

static rs_err lobby_rsa_parse(rs_buf *blk, rs_login_request *out) {
    int i, magic;
    magic = rs_g1(blk);
    if (magic != 10) {
        return RS_ERR_PROTO;
    }
    for (i = 0; i < 4; i++) {
        out->isaac_seed[i] = (uint32_t)rs_g4(blk);
    }
    (void)rs_g8(blk);
    (void)rs_g1(blk);
    (void)rs_g4(blk);
    (void)rs_g1(blk);
    if (rs_gjstrnull(blk, out->password, sizeof(out->password)) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    (void)rs_g8(blk);
    (void)rs_g8(blk);
    return RS_OK;
}

static rs_err p_jstr2(rs_buf *b, const char *s) {
    rs_err e = rs_p1(b, 0);
    if (e) {
        return e;
    }
    return rs_pjstrnull(b, s ? s : "");
}

static rs_err p_jstr2n(rs_buf *b, const char *s, size_t maxch) {
    char tmp[160];
    size_t n = 0;
    if (!s) {
        s = "";
    }
    if (maxch >= sizeof(tmp)) {
        maxch = sizeof(tmp) - 1;
    }
    while (s[n] && n < maxch) {
        n++;
    }
    memcpy(tmp, s, n);
    tmp[n] = 0;
    return p_jstr2(b, tmp);
}

static rs_err g_jstr2(rs_buf *b, char *dst, size_t cap) {
    if (rs_buf_remaining(b) < 2 || rs_g1(b) != 0) {
        return RS_ERR_FORMAT;
    }
    return rs_gjstrnull(b, dst, cap);
}

static int crc_n(const rs_build_info *build) {
    return rs_login_crc_count(build);
}

static const rs_login_ext *ext_of(const rs_login_request *req, rs_login_ext *scratch) {
    if (req->ext) {
        return req->ext;
    }
    memset(scratch, 0, sizeof(*scratch));
    return scratch;
}

static rs_err tail_user(rs_buf *t, const rs_login_request *req, const rs_login_ext *ext) {
    if (ext->social_key_set) {
        W(rs_p1(t, 0));
        return rs_p8(t, ext->social_key);
    }
    W(rs_p1(t, 1));
    return rs_pjstrnull(t, req->username);
}

static rs_err tail_window(rs_buf *t, const rs_login_request *req, const rs_login_ext *ext) {
    W(rs_p1(t, req->window_mode));
    W(rs_p2(t, req->canvas_w));
    W(rs_p2(t, req->canvas_h));
    W(rs_p1(t, ext->antialias));
    W(rs_pdata(t, ext->uid192, 24));
    return rs_pjstrnull(t, req->settings);
}

static rs_err prefs_write(rs_buf *t, const rs_login_prefs *p) {
    size_t mark = t->pos;
    size_t start;
    W(rs_p1(t, 0));
    start = t->pos;
    W(rs_p1(t, 38));
    W(rs_p1(t, p->anim_detail));
    W(rs_p1(t, p->anti_aliasing));
    W(rs_p1(t, p->unused));
    W(rs_p1(t, p->bloom));
    W(rs_p1(t, p->brightness));
    W(rs_p1(t, p->build_area));
    W(rs_p1(t, p->draw_distance));
    W(rs_p1(t, p->flickering));
    W(rs_p1(t, p->fog));
    W(rs_p1(t, p->ground_blending));
    W(rs_p1(t, p->ground_decoration));
    W(rs_p1(t, p->idle_animations));
    W(rs_p1(t, p->lighting_detail));
    W(rs_p1(t, p->scenery_shadows));
    W(rs_p1(t, p->shadow_quality));
    W(rs_p1(t, 0));
    W(rs_p1(t, p->orthographic));
    W(rs_p1(t, p->particles));
    W(rs_p1(t, p->remove_roofs));
    W(rs_p1(t, p->screen_size));
    W(rs_p1(t, p->skyboxes));
    W(rs_p1(t, p->character_shadows));
    W(rs_p1(t, p->textures));
    W(rs_p1(t, p->toolkit));
    W(rs_p1(t, 0));
    W(rs_p1(t, p->water_detail));
    W(rs_p1(t, p->window_mode));
    W(rs_p1(t, p->unused1));
    W(rs_p1(t, p->unused2));
    W(rs_p1(t, p->unused3));
    W(rs_p1(t, p->unused4));
    W(rs_p1(t, p->unused5));
    W(rs_p1(t, p->unused6));
    W(rs_p1(t, p->unused7));
    W(rs_p1(t, p->unused8));
    W(rs_p2(t, p->unused9));
    W(rs_p2(t, p->unused10));
    W(rs_p2(t, p->unused11));
    W(rs_p2(t, p->unused12));
    W(rs_p1(t, p->custom_cursors));
    W(rs_p1(t, p->preset));
    W(rs_p1(t, p->cpu_usage));
    W(rs_p1(t, p->loading_screen));
    W(rs_p1(t, p->safe_mode));
    W(rs_p1(t, p->unknown7));
    W(rs_p1(t, p->unused13));
    W(rs_p1(t, p->console_key_press));
    W(rs_p1(t, p->sound_volume));
    W(rs_p1(t, p->background_sound));
    W(rs_p1(t, p->speech_volume));
    W(rs_p1(t, p->unknown_volume1));
    W(rs_p1(t, p->unknown_volume2));
    W(rs_p1(t, p->stereo));
    if (t->pos - start != RS_PREFS_BYTES) {
        return RS_ERR_PROTO;
    }
    t->data[mark] = (uint8_t)RS_PREFS_BYTES;
    return RS_OK;
}

static rs_err hw_write(rs_buf *t, const rs_login_hw *h) {
    int i;
    W(rs_p1(t, 8));
    W(rs_p1(t, h->operating_system));
    W(rs_p1(t, h->os_arch64 ? 1 : 0));
    W(rs_p2(t, h->os_version));
    W(rs_p1(t, h->java_vendor));
    W(rs_p1(t, h->java_major));
    W(rs_p1(t, h->java_minor));
    W(rs_p1(t, h->java_patch));
    W(rs_p1(t, h->unused ? 1 : 0));
    W(rs_p2(t, h->max_memory));
    W(rs_p1(t, h->processors));
    W(rs_p3(t, h->cpu_ram));
    W(rs_p2(t, h->cpu_speed));
    W(p_jstr2n(t, h->gpu, 40));
    W(p_jstr2n(t, h->unused1, 40));
    W(p_jstr2n(t, h->directx, 10));
    W(p_jstr2n(t, h->unused2, 10));
    W(rs_p1(t, h->dx_month));
    W(rs_p2(t, h->dx_year));
    W(p_jstr2n(t, h->cpu_vendor, sizeof(h->cpu_vendor) - 1));
    W(p_jstr2n(t, h->cpu_desc, sizeof(h->cpu_desc) - 1));
    W(rs_p1(t, h->cpu_processors));
    W(rs_p1(t, h->cpu_processors2));
    for (i = 0; i < 3; i++) {
        W(rs_p4(t, h->cpu_unknown[i]));
    }
    W(rs_p4(t, h->cpu_unknown1));
    return p_jstr2n(t, h->unused3, 120);
}

static rs_err crcs_write(rs_buf *t, const rs_build_info *build, const rs_login_request *req) {
    int i, n = crc_n(build);
    for (i = 0; i < n; i++) {
        W(rs_p4(t, (int)req->archive_crc[i]));
    }
    return RS_OK;
}

static rs_err lobby_tail_write(rs_buf *tail, const rs_build_info *build, const rs_login_request *req,
                               int lobby) {
    rs_login_ext scratch;
    const rs_login_ext *ext = ext_of(req, &scratch);
    W(tail_user(tail, req, ext));
    if (lobby) {
        W(rs_p1(tail, ext->modegame));
        W(rs_p1(tail, ext->language));
    }
    W(tail_window(tail, req, ext));
    if (!lobby) {
        W(rs_p4(tail, req->affiliate));
    }
    W(prefs_write(tail, &ext->prefs));
    W(hw_write(tail, &ext->hw));
    W(rs_p4(tail, req->verify_id));
    if (!lobby) {
        W(rs_p4(tail, ext->user_flow2));
        W(rs_p4(tail, ext->user_flow1));
        W(rs_p4(tail, ext->tint3));
        W(rs_p4(tail, ext->tint));
    }
    W(rs_pjstrnull(tail, ext->string2));
    if (lobby) {
        W(rs_p4(tail, req->affiliate));
        W(rs_p4(tail, ext->tint4));
        W(rs_pjstrnull(tail, ext->gamepack));
        W(rs_p1(tail, ext->client_type & 1));
        W(rs_p1(tail, 0)); /* pbool(false) */
    } else {
        if (ext->additional_info[0]) {
            W(rs_p1(tail, 1));
            W(rs_pjstrnull(tail, ext->additional_info));
        } else {
            W(rs_p1(tail, 0));
        }
        W(rs_p1(tail, ext->javascript ? 1 : 0));
        W(rs_p1(tail, ext->chrome ? 1 : 0));
        W(rs_p1(tail, ext->client_type & 1));
        W(rs_p4(tail, ext->tint4));
        W(rs_pjstrnull(tail, ext->gamepack));
        W(rs_p1(tail, ext->world_mismatch ? 1 : 0));
        W(rs_p2(tail, ext->lobby_node));
    }
    return crcs_write(tail, build, req);
}

static rs_err need(rs_buf *b, size_t n) {
    if (rs_buf_remaining(b) < n) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err prefs_read(rs_buf *b, rs_login_prefs *p) {
    int n;
    size_t body;
    if (need(b, 1) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    n = rs_g1(b);
    if (need(b, (size_t)n) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    body = b->pos;
    if (n != RS_PREFS_BYTES) {
        b->pos += (size_t)n;
        return RS_OK;
    }
    (void)rs_g1(b); /* version 38 */
    p->anim_detail = rs_g1(b);
    p->anti_aliasing = rs_g1(b);
    p->unused = rs_g1(b);
    p->bloom = rs_g1(b);
    p->brightness = rs_g1(b);
    p->build_area = rs_g1(b);
    p->draw_distance = rs_g1(b);
    p->flickering = rs_g1(b);
    p->fog = rs_g1(b);
    p->ground_blending = rs_g1(b);
    p->ground_decoration = rs_g1(b);
    p->idle_animations = rs_g1(b);
    p->lighting_detail = rs_g1(b);
    p->scenery_shadows = rs_g1(b);
    p->shadow_quality = rs_g1(b);
    (void)rs_g1(b);
    p->orthographic = rs_g1(b);
    p->particles = rs_g1(b);
    p->remove_roofs = rs_g1(b);
    p->screen_size = rs_g1(b);
    p->skyboxes = rs_g1(b);
    p->character_shadows = rs_g1(b);
    p->textures = rs_g1(b);
    p->toolkit = rs_g1(b);
    (void)rs_g1(b);
    p->water_detail = rs_g1(b);
    p->window_mode = rs_g1(b);
    p->unused1 = rs_g1(b);
    p->unused2 = rs_g1(b);
    p->unused3 = rs_g1(b);
    p->unused4 = rs_g1(b);
    p->unused5 = rs_g1(b);
    p->unused6 = rs_g1(b);
    p->unused7 = rs_g1(b);
    p->unused8 = rs_g1(b);
    p->unused9 = rs_g2(b);
    p->unused10 = rs_g2(b);
    p->unused11 = rs_g2(b);
    p->unused12 = rs_g2(b);
    p->custom_cursors = rs_g1(b);
    p->preset = rs_g1(b);
    p->cpu_usage = rs_g1(b);
    p->loading_screen = rs_g1(b);
    p->safe_mode = rs_g1(b);
    p->unknown7 = rs_g1(b);
    p->unused13 = rs_g1(b);
    p->console_key_press = rs_g1(b);
    p->sound_volume = rs_g1(b);
    p->background_sound = rs_g1(b);
    p->speech_volume = rs_g1(b);
    p->unknown_volume1 = rs_g1(b);
    p->unknown_volume2 = rs_g1(b);
    p->stereo = rs_g1(b);
    if (b->pos - body != RS_PREFS_BYTES) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err hw_read(rs_buf *b, rs_login_hw *h) {
    int i, kind;
    if (need(b, 18) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    kind = rs_g1(b);
    if (kind != 8) {
        return RS_ERR_PROTO;
    }
    h->operating_system = rs_g1(b);
    h->os_arch64 = rs_g1(b);
    h->os_version = rs_g2(b);
    h->java_vendor = rs_g1(b);
    h->java_major = rs_g1(b);
    h->java_minor = rs_g1(b);
    h->java_patch = rs_g1(b);
    h->unused = rs_g1(b);
    h->max_memory = rs_g2(b);
    h->processors = rs_g1(b);
    h->cpu_ram = rs_g3(b);
    h->cpu_speed = rs_g2(b);
    if (g_jstr2(b, h->gpu, sizeof(h->gpu)) != RS_OK ||
        g_jstr2(b, h->unused1, sizeof(h->unused1)) != RS_OK ||
        g_jstr2(b, h->directx, sizeof(h->directx)) != RS_OK ||
        g_jstr2(b, h->unused2, sizeof(h->unused2)) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    if (need(b, 3) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    h->dx_month = rs_g1(b);
    h->dx_year = rs_g2(b);
    if (g_jstr2(b, h->cpu_vendor, sizeof(h->cpu_vendor)) != RS_OK ||
        g_jstr2(b, h->cpu_desc, sizeof(h->cpu_desc)) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    if (need(b, 2 + 16) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    h->cpu_processors = rs_g1(b);
    h->cpu_processors2 = rs_g1(b);
    for (i = 0; i < 3; i++) {
        h->cpu_unknown[i] = rs_g4s(b);
    }
    h->cpu_unknown1 = rs_g4s(b);
    if (g_jstr2(b, h->unused3, sizeof(h->unused3)) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err tail_user_read(rs_buf *b, rs_login_request *out, rs_login_ext *ext) {
    int flag;
    if (need(b, 1) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    flag = rs_g1(b);
    if (flag == 1) {
        ext->social_key_set = 0;
        if (rs_gjstrnull(b, out->username, sizeof(out->username)) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        return RS_OK;
    }
    if (flag != 0 || need(b, 8) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    ext->social_key_set = 1;
    ext->social_key = rs_g8(b);
    return RS_OK;
}

static rs_err tail_window_read(rs_buf *b, rs_login_request *out, rs_login_ext *ext) {
    if (need(b, 1 + 2 + 2 + 1 + 24) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    out->window_mode = rs_g1(b);
    out->canvas_w = rs_g2(b);
    out->canvas_h = rs_g2(b);
    ext->antialias = rs_g1(b);
    if (rs_gdata(b, ext->uid192, 24) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    if (rs_gjstrnull(b, out->settings, sizeof(out->settings)) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err crcs_read(rs_buf *b, const rs_build_info *build, rs_login_request *out) {
    int i, n = crc_n(build);
    out->ncrc = n;
    for (i = 0; i < n; i++) {
        if (need(b, 4) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        out->archive_crc[i] = (uint32_t)rs_g4(b);
    }
    return RS_OK;
}

static rs_err lobby_tail_parse(rs_buf *tail, const rs_build_info *build, rs_login_request *out,
                               int lobby) {
    rs_login_ext scratch;
    rs_login_ext *ext = out->ext ? out->ext : &scratch;
    rs_err e;
    memset(ext, 0, sizeof(*ext));
    e = tail_user_read(tail, out, ext);
    if (e) {
        return e;
    }
    if (lobby) {
        if (need(tail, 2) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        ext->modegame = rs_g1(tail);
        ext->language = rs_g1(tail);
    }
    e = tail_window_read(tail, out, ext);
    if (e) {
        return e;
    }
    if (!lobby) {
        if (need(tail, 4) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        out->affiliate = rs_g4s(tail);
    }
    e = prefs_read(tail, &ext->prefs);
    if (e) {
        return e;
    }
    e = hw_read(tail, &ext->hw);
    if (e) {
        return e;
    }
    if (need(tail, 4) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    out->verify_id = rs_g4s(tail);
    if (!lobby) {
        if (need(tail, 16) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        ext->user_flow2 = rs_g4s(tail);
        ext->user_flow1 = rs_g4s(tail);
        ext->tint3 = rs_g4s(tail);
        ext->tint = rs_g4s(tail);
    }
    if (rs_gjstrnull(tail, ext->string2, sizeof(ext->string2)) != RS_OK) {
        return RS_ERR_FORMAT;
    }
    if (lobby) {
        if (need(tail, 8) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        out->affiliate = rs_g4s(tail);
        ext->tint4 = rs_g4s(tail);
        if (rs_gjstrnull(tail, ext->gamepack, sizeof(ext->gamepack)) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        if (need(tail, 2) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        ext->client_type = rs_g1(tail);
        (void)rs_g1(tail); /* pbool(false) */
    } else {
        int extra;
        if (need(tail, 1) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        extra = rs_g1(tail);
        if (extra) {
            if (rs_gjstrnull(tail, ext->additional_info, sizeof(ext->additional_info)) != RS_OK) {
                return RS_ERR_FORMAT;
            }
        }
        if (need(tail, 3) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        ext->javascript = rs_g1(tail);
        ext->chrome = rs_g1(tail);
        ext->client_type = rs_g1(tail);
        if (need(tail, 4) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        ext->tint4 = rs_g4s(tail);
        if (rs_gjstrnull(tail, ext->gamepack, sizeof(ext->gamepack)) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        if (need(tail, 3) != RS_OK) {
            return RS_ERR_FORMAT;
        }
        ext->world_mismatch = rs_g1(tail);
        ext->lobby_node = rs_g2(tail);
    }
    e = crcs_read(tail, build, out);
    if (e) {
        return e;
    }
    if (rs_buf_remaining(tail) != 0) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

#undef W

rs_err js5_lobby_login_write(rs_session *s, rs_buf *dst, const rs_login_request *req,
                             const rs_rsa_key *rsa) {
    const rs_build_info *build = s ? s->build : NULL;
    uint8_t block[256], enc[RS_RSA_MAX_BYTES], tailbuf[RS_LOGIN_TAIL_MAX];
    rs_buf blk, tail;
    size_t size_at, enc_len = 0;
    int lobby, rev;
    uint32_t key[4];
    rs_err e;
    int i;

    if (!dst || !req) {
        return RS_ERR_ARG;
    }
    lobby = req->lobby ? 1 : 0;
    e = rs_p1(dst, lobby ? 19 : 16);
    if (e) {
        return e;
    }
    size_at = dst->pos;
    e = rs_p2(dst, 0);
    if (e) {
        return e;
    }
    rev = req->revision ? req->revision : (build ? build->revision : 0);
    e = rs_p4(dst, rev);
    if (e) {
        return e;
    }
    e = rs_p4(dst, 1);
    if (e) {
        return e;
    }
    if (!lobby) {
        e = rs_p1(dst, req->low_memory);
        if (e) {
            return e;
        }
    }
    rs_buf_wrap(&blk, block, sizeof(block));
    e = lobby_rsa_write(&blk, req);
    if (e) {
        return e;
    }
    if (rsa && rsa->elen > 0) {
        e = rs_rsa_modpow(rsa, block, blk.pos, enc, &enc_len);
        if (e) {
            return e;
        }
        e = rs_p1(dst, (int)enc_len);
        if (e) {
            return e;
        }
        e = rs_pdata(dst, enc, enc_len);
    } else {
        e = rs_p1(dst, (int)blk.pos);
        if (e) {
            return e;
        }
        e = rs_pdata(dst, block, blk.pos);
    }
    if (e) {
        return e;
    }
    rs_buf_wrap(&tail, tailbuf, sizeof(tailbuf));
    e = lobby_tail_write(&tail, build, req, lobby);
    if (e) {
        return e;
    }
    for (i = 0; i < 4; i++) {
        key[i] = req->isaac_seed[i];
    }
    rs_xtea_encrypt(key, tailbuf, tail.pos);
    e = rs_pdata(dst, tailbuf, tail.pos);
    if (e) {
        return e;
    }
    return rs_psize2(dst, size_at);
}

rs_err js5_lobby_login_parse(rs_session *s, rs_buf *src, const rs_rsa_key *rsa, rs_login_request *out) {
    const rs_build_info *build = s ? s->build : NULL;
    int opcode, size, lobby, rsa_len;
    size_t start, payload_end;
    rs_buf rsa_buf, tail;
    uint8_t rsa_in[RS_RSA_MAX_BYTES], rsa_out[RS_RSA_MAX_BYTES], tailbuf[RS_LOGIN_TAIL_MAX];
    size_t rsa_out_len = 0;
    uint32_t key[4];
    rs_err e;
    int i;
    rs_login_ext *keep;

    if (!src || !out) {
        return RS_ERR_ARG;
    }
    keep = out->ext;
    memset(out, 0, sizeof(*out));
    out->ext = keep;
    if (rs_buf_remaining(src) < 3) {
        return RS_ERR_NEED_MORE;
    }
    start = src->pos;
    opcode = rs_g1(src);
    if (opcode != 16 && opcode != 19) {
        src->pos = start;
        return RS_ERR_PROTO;
    }
    lobby = opcode == 19;
    out->lobby = lobby;
    size = rs_g2(src);
    if (size < 0 || rs_buf_remaining(src) < (size_t)size) {
        src->pos = start;
        return RS_ERR_NEED_MORE;
    }
    payload_end = src->pos + (size_t)size;
    out->revision = rs_g4s(src);
    (void)rs_g4(src); /* subrevision */
    if (!lobby) {
        out->low_memory = rs_g1(src);
    }
    if (src->pos >= payload_end) {
        src->pos = payload_end;
        return RS_ERR_PROTO;
    }
    rsa_len = rs_g1(src);
    if (rsa_len < 1 || src->pos + (size_t)rsa_len > payload_end) {
        src->pos = payload_end;
        return RS_ERR_PROTO;
    }
    if (rsa && rsa->dlen > 0) {
        if (rsa_len > RS_RSA_MAX_BYTES) {
            src->pos = payload_end;
            return RS_ERR_PROTO;
        }
        memcpy(rsa_in, src->data + src->pos, (size_t)rsa_len);
        src->pos += (size_t)rsa_len;
        e = rs_rsa_modpow_key(rsa, 1, rsa_in, (size_t)rsa_len, rsa_out, &rsa_out_len);
        if (e != RS_OK) {
            src->pos = payload_end;
            return e;
        }
        rs_buf_wrap(&rsa_buf, rsa_out, rsa_out_len);
    } else {
        rs_buf_wrap(&rsa_buf, src->data + src->pos, (size_t)rsa_len);
        src->pos += (size_t)rsa_len;
    }
    e = lobby_rsa_parse(&rsa_buf, out);
    if (e != RS_OK) {
        src->pos = payload_end;
        return e;
    }
    if (payload_end < src->pos || payload_end - src->pos > sizeof(tailbuf)) {
        src->pos = payload_end;
        return RS_ERR_FORMAT;
    }
    memcpy(tailbuf, src->data + src->pos, payload_end - src->pos);
    for (i = 0; i < 4; i++) {
        key[i] = out->isaac_seed[i];
    }
    rs_xtea_decrypt(key, tailbuf, payload_end - src->pos);
    rs_buf_wrap(&tail, tailbuf, payload_end - src->pos);
    e = lobby_tail_parse(&tail, build, out, lobby);
    src->pos = payload_end;
    return e;
}
