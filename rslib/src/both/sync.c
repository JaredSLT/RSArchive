#include "internal.h"
#include "rslib/both/sync.h"

#include <string.h>

#define SYNC_TRY(expr)                                                         \
    do {                                                                       \
        rs_err _sync_e = (expr);                                               \
        if (_sync_e != RS_OK) {                                                \
            return _sync_e;                                                    \
        }                                                                      \
    } while (0)

enum { KIND_317 = 1, KIND_350 = 2, KIND_377 = 3, KIND_254 = 4 };
enum { MV_NONE = 0, MV_STILL, MV_WALK, MV_RUN, MV_REMOVE, MV_ADD, MV_TELE };
enum { NPC_CAP = 1024, LIST_CAP = 255, NPC_INDEX_MAX = 16383 };

static const int8_t k_dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
static const int8_t k_dz[8] = {1, 1, 1, 0, 0, -1, -1, -1};
static const int k_anim317[7] = {0x328, 0x337, 0x333, 0x334, 0x335, 0x336, 0x338};

typedef struct sync_mv {
    int index;
    int op;
    int dir1, dir2;
    int x, z, plane;
    int jump;
    int mask;
    int upd;
    int dx, dz;
    int type;
} sync_mv;

typedef struct sync_npc_node {
    int index;
    int in_view;
    int sent_x, sent_z, sent_type;
    rs_sync_npc npc;
} sync_npc_node;

struct rs_sync_view {
    int revision;
    int kind;
    int local;
    rs_sync_player *players;
    uint8_t pin[2048];
    int psx[2048];
    int psz[2048];
    int psp[2048];
    int pids[LIST_CAP];
    int pn;
    int16_t *nmap;
    sync_npc_node *nodes;
    int ncount;
    int ncap;
    int nids[LIST_CAP];
    int nn;
};

static int kind_of(int revision) {
    /* 254 Client.getPlayerLocal / readPlayerUpdates / getNpcPosNewVis / readNpcUpdates. */
    if (revision == 254) {
        return KIND_254;
    }
    if (revision == 317) {
        return KIND_317;
    }
    if (revision == 350) {
        return KIND_350;
    }
    if (revision == 377) {
        return KIND_377;
    }
    return 0;
}

static rs_err sync_refuse(int revision) {
    if (revision == 282) {
        return RS_ERR_UNSUPPORTED;
    }
    if (revision == 443 || revision == 468 || revision == 474 || revision == 481 ||
        revision == 508 || revision == 525 || revision == 530 || revision == 578 ||
        revision == 614 || revision == 634 || revision == 667 || revision == 695 ||
        revision == 742 || revision == 745 || revision == 790) {
        /* No readable readPlayerInfo; 2047 hits in these trees are camera yaw. */
        return RS_ERR_UNSUPPORTED;
    }
    if (revision == 831 || revision == 860) {
        /* No client sources in this archive. */
        return RS_ERR_UNSUPPORTED;
    }
    if (revision == 876 || revision == 887 || revision == 902 || revision == 910) {
        /* 910 npc ids are 15-bit, terminator 32767, with a crawl step. */
        return RS_ERR_UNSUPPORTED;
    }
    /* No transcribed player/npc reader for this revision. */
    return RS_ERR_UNSUPPORTED;
}

static rs_err sync_use(const rs_sync_view *v) {
    if (!v) {
        return RS_ERR_ARG;
    }
    if (v->kind == 0) {
        return sync_refuse(v->revision);
    }
    return RS_OK;
}

static int player_known(int kind) {
    if (kind == KIND_254) {
        return 0x77F;
    }
    if (kind == KIND_317) {
        return 0x7BF;
    }
    if (kind == KIND_350) {
        return 0x7FB;
    }
    return 0x7DF;
}

static int player_signal(int kind) {
    if (kind == KIND_254) {
        return 0x80;
    }
    if (kind == KIND_317) {
        return 0x40;
    }
    if (kind == KIND_350) {
        return 0x04;
    }
    return 0x20;
}

static int npc_type_bit(int kind) {
    if (kind == KIND_254) {
        return RS_SYNC_N254_TYPE;
    }
    if (kind == KIND_317) {
        return RS_SYNC_N317_TYPE;
    }
    if (kind == KIND_350) {
        return RS_SYNC_N350_TYPE;
    }
    return RS_SYNC_N377_TYPE;
}

static int npc_type_width(int kind) {
    if (kind == KIND_254) {
        return 11;
    }
    return kind == KIND_317 ? 12 : 13;
}

static void player_blank(rs_sync_player *p) {
    memset(p, 0, sizeof(*p));
    p->dir1 = -1;
    p->dir2 = -1;
}

static void apply_step(int *x, int *z, int dir) {
    *x += k_dx[dir & 7];
    *z += k_dz[dir & 7];
}

static int find_dir(int dx, int dz) {
    int d;
    for (d = 0; d < 8; d++) {
        if (k_dx[d] == dx && k_dz[d] == dz) {
            return d;
        }
    }
    return -1;
}

static int find_run(int dx, int dz, int *d1, int *d2) {
    int a, b;
    for (a = 0; a < 8; a++) {
        for (b = 0; b < 8; b++) {
            if (k_dx[a] + k_dx[b] == dx && k_dz[a] + k_dz[b] == dz) {
                *d1 = a;
                *d2 = b;
                return 1;
            }
        }
    }
    return 0;
}

static int sign5(int v) {
    return v > 15 ? v - 32 : v;
}

static rs_err gbits(rs_buf *b, int n, int *out) {
    if (!b->data || n < 0 || b->bit_pos + (size_t)n > b->cap * 8) {
        return RS_ERR_NEED_MORE;
    }
    *out = rs_gbit(b, n);
    return RS_OK;
}

static rs_err need(const rs_buf *b, size_t n) {
    if (rs_buf_remaining(b) < n) {
        return RS_ERR_NEED_MORE;
    }
    return RS_OK;
}

static void finish_bits(rs_buf *b) {
    size_t bit = b->bit_pos;
    unsigned rem = (unsigned)(bit & 7);
    rs_access_bytes(b);
    if (rem != 0) {
        size_t off = bit >> 3;
        if (off < b->cap) {
            b->data[off] &= (uint8_t)(0xffu << (8u - rem));
        }
    }
}

static rs_err put_u8(rs_buf *b, int v) {
    if (v < 0 || v > 255) {
        return RS_ERR_RANGE;
    }
    return rs_p1(b, v);
}

static rs_err put_u16(rs_buf *b, int v, int how) {
    if (v < 0 || v > 65535) {
        return RS_ERR_RANGE;
    }
    if (how == 1) {
        return rs_p2_le(b, v);
    }
    if (how == 2) {
        return rs_p2_add128(b, v);
    }
    if (how == 3) {
        return rs_p2_le_add128(b, v);
    }
    return rs_p2(b, v);
}

static rs_err put_seq(rs_buf *b, int v, int how) {
    if (v < 0) {
        v = 65535;
    }
    return put_u16(b, v, how);
}

static rs_err put_chat(rs_buf *b, const char *s, size_t cap) {
    size_t n = 0;
    if (!s) {
        return RS_ERR_ARG;
    }
    while (n < cap && s[n] != 0) {
        if (s[n] == '\n') {
            return RS_ERR_RANGE;
        }
        n++;
    }
    if (n == cap) {
        return RS_ERR_RANGE;
    }
    return rs_pjstr(b, s);
}

static rs_err put_rev(rs_buf *b, const uint8_t *p, int n) {
    int i;
    for (i = n - 1; i >= 0; i--) {
        SYNC_TRY(rs_p1(b, p[i]));
    }
    return RS_OK;
}

static rs_err put_rev128(rs_buf *b, const uint8_t *p, int n) {
    int i;
    for (i = n - 1; i >= 0; i--) {
        SYNC_TRY(rs_p1(b, p[i] + 128));
    }
    return RS_OK;
}

static rs_err put_fwd128(rs_buf *b, const uint8_t *p, int n) {
    int i;
    for (i = 0; i < n; i++) {
        SYNC_TRY(rs_p1(b, p[i] + 128));
    }
    return RS_OK;
}

static rs_err get_rev(rs_buf *b, uint8_t *p, int n) {
    int i;
    SYNC_TRY(need(b, (size_t)n));
    for (i = n - 1; i >= 0; i--) {
        p[i] = (uint8_t)rs_g1(b);
    }
    return RS_OK;
}

static rs_err get_rev128(rs_buf *b, uint8_t *p, int n) {
    int i;
    SYNC_TRY(need(b, (size_t)n));
    for (i = n - 1; i >= 0; i--) {
        p[i] = (uint8_t)((rs_g1(b) - 128) & 0xff);
    }
    return RS_OK;
}

static rs_err get_fwd128(rs_buf *b, uint8_t *p, int n) {
    int i;
    SYNC_TRY(need(b, (size_t)n));
    for (i = 0; i < n; i++) {
        p[i] = (uint8_t)((rs_g1(b) - 128) & 0xff);
    }
    return RS_OK;
}

static void split_spot(int packed, int *height, int *delay) {
    *height = (int)(((unsigned)packed >> 16) & 0xffffu);
    *delay = (int)((unsigned)packed & 0xffffu);
}

static rs_err put_spot(rs_buf *b, int id, int height, int delay, int id_how, int alt2) {
    int packed;
    if (height < 0 || height > 65535 || delay < 0 || delay > 65535) {
        return RS_ERR_RANGE;
    }
    SYNC_TRY(put_seq(b, id, id_how));
    packed = (height << 16) | delay;
    return alt2 ? rs_p4_alt2(b, packed) : rs_p4(b, packed);
}

static rs_err get_spot(rs_buf *b, int id_how, int alt2, int *id, int *height, int *delay) {
    int raw, packed;
    SYNC_TRY(need(b, 6));
    if (id_how == 1) {
        raw = rs_g2_le(b);
    } else if (id_how == 2) {
        raw = rs_g2_add128(b);
    } else if (id_how == 3) {
        raw = rs_g2_le_add128(b);
    } else {
        raw = rs_g2(b);
    }
    packed = alt2 ? rs_g4_alt2(b) : rs_g4(b);
    *id = raw == 65535 ? -1 : raw;
    split_spot(packed, height, delay);
    return RS_OK;
}

static rs_err check_packed(const uint8_t *p, int n) {
    (void)p;
    if (n < 0 || n > 255) {
        return RS_ERR_RANGE;
    }
    return RS_OK;
}

static rs_err appearance_write(int kind, const rs_sync_player *p, rs_buf *dst) {
    int i;
    if (p->gender < 0 || p->gender > 255) {
        return RS_ERR_RANGE;
    }
    SYNC_TRY(rs_p1(dst, p->gender));
    if (kind == KIND_317 || kind == KIND_254) {
        SYNC_TRY(put_u8(dst, p->headicon));
    } else {
        if (p->headicon < -128 || p->headicon > 127 || p->headicon2 < -128 || p->headicon2 > 127) {
            return RS_ERR_RANGE;
        }
        SYNC_TRY(rs_p1(dst, p->headicon));
        SYNC_TRY(rs_p1(dst, p->headicon2));
    }
    if (p->npc_id > 65535) {
        return RS_ERR_RANGE;
    }
    if (p->npc_id > 0) {
        SYNC_TRY(rs_p1(dst, 255));
        SYNC_TRY(rs_p1(dst, 255));
        SYNC_TRY(rs_p2(dst, p->npc_id));
    } else if (p->npc_id < 0) {
        return RS_ERR_RANGE;
    } else {
        for (i = 0; i < 12; i++) {
            int id = p->styles[i];
            if (id < 0 || (id > 255 && id < 512) || id > 65535) {
                return RS_ERR_RANGE;
            }
            if (i >= 7 && id == 0) {
                SYNC_TRY(rs_p1(dst, 0));
            } else if (id < 256) {
                SYNC_TRY(rs_p2(dst, 0x100 + id));
            } else {
                SYNC_TRY(rs_p2(dst, id));
            }
        }
    }
    for (i = 0; i < 5; i++) {
        SYNC_TRY(put_u8(dst, p->colours[i]));
    }
    for (i = 0; i < 7; i++) {
        int anim = p->anims[i];
        if (kind == KIND_317 && anim == 0) {
            anim = k_anim317[i];
        }
        if (anim < 0) {
            anim = 65535;
        }
        if (anim > 65535) {
            return RS_ERR_RANGE;
        }
        SYNC_TRY(rs_p2(dst, anim));
    }
    SYNC_TRY(rs_p8(dst, rs_base37_encode(p->name)));
    SYNC_TRY(put_u8(dst, p->combat_level));
    if (kind == KIND_254) {
        return RS_OK;
    }
    return put_u16(dst, p->skill_level, 0);
}

static rs_err appearance_read(int kind, rs_buf *src, rs_sync_player *p) {
    int i;
    uint64_t name;
    SYNC_TRY(need(src, (kind == KIND_317 || kind == KIND_254) ? 2 : 3));
    p->gender = rs_g1(src);
    if (kind == KIND_317 || kind == KIND_254) {
        p->headicon = rs_g1(src);
        p->headicon2 = 0;
    } else {
        p->headicon = rs_g1b(src);
        p->headicon2 = rs_g1b(src);
    }
    p->npc_id = 0;
    for (i = 0; i < 12; i++) {
        p->styles[i] = 0;
    }
    for (i = 0; i < 12; i++) {
        int hi, word;
        SYNC_TRY(need(src, 1));
        hi = rs_g1(src);
        if (hi == 0) {
            continue;
        }
        SYNC_TRY(need(src, 1));
        word = (hi << 8) | rs_g1(src);
        if (i == 0 && word == 65535) {
            SYNC_TRY(need(src, 2));
            p->npc_id = rs_g2(src);
            break;
        }
        p->styles[i] = word < 512 ? word - 256 : word;
    }
    if (kind == KIND_254) {
        SYNC_TRY(need(src, 5 + 14 + 8 + 1));
        for (i = 0; i < 5; i++) {
            p->colours[i] = rs_g1(src);
        }
        for (i = 0; i < 7; i++) {
            int anim = rs_g2(src);
            p->anims[i] = anim == 65535 ? -1 : anim;
        }
        name = rs_g8(src);
        if (name == 0) {
            p->name[0] = 0;
        } else {
            rs_base37_decode(name, p->name);
        }
        p->combat_level = rs_g1(src);
        p->skill_level = 0;
        return RS_OK;
    }
    SYNC_TRY(need(src, 5 + 14 + 8 + 1 + 2));
    for (i = 0; i < 5; i++) {
        p->colours[i] = rs_g1(src);
    }
    for (i = 0; i < 7; i++) {
        int anim = rs_g2(src);
        p->anims[i] = anim == 65535 ? -1 : anim;
    }
    name = rs_g8(src);
    if (name == 0) {
        p->name[0] = 0;
    } else {
        rs_base37_decode(name, p->name);
    }
    p->combat_level = rs_g1(src);
    p->skill_level = rs_g2(src);
    return RS_OK;
}

static rs_err put_appearance(int kind, rs_buf *dst, const rs_sync_player *p) {
    uint8_t tmp[128];
    rs_buf body;
    int n, i;
    rs_buf_wrap(&body, tmp, sizeof(tmp));
    SYNC_TRY(appearance_write(kind, p, &body));
    n = (int)body.pos;
    if (n < 0 || n > 255) {
        return RS_ERR_RANGE;
    }
    if (kind == KIND_317) {
        SYNC_TRY(rs_p1_neg(dst, n));
        return rs_pdata(dst, tmp, (size_t)n);
    }
    if (kind == KIND_350) {
        SYNC_TRY(rs_p1_add128(dst, n));
        return rs_pdata(dst, tmp, (size_t)n);
    }
    if (kind == KIND_254) {
        SYNC_TRY(rs_p1(dst, n));
        return rs_pdata(dst, tmp, (size_t)n);
    }
    SYNC_TRY(rs_p1(dst, n));
    for (i = n - 1; i >= 0; i--) {
        SYNC_TRY(rs_p1(dst, tmp[i]));
    }
    return RS_OK;
}

static rs_err get_appearance(int kind, rs_buf *src, rs_sync_player *p) {
    uint8_t tmp[256];
    rs_buf body;
    int n, i;
    SYNC_TRY(need(src, 1));
    if (kind == KIND_317) {
        n = rs_g1_neg(src);
    } else if (kind == KIND_350) {
        n = rs_g1_add128(src);
    } else {
        n = rs_g1(src);
    }
    if (n < 0 || n > 255) {
        return RS_ERR_FORMAT;
    }
    SYNC_TRY(need(src, (size_t)n));
    if (kind == KIND_377) {
        for (i = 0; i < n; i++) {
            tmp[n - 1 - i] = (uint8_t)rs_g1(src);
        }
    } else {
        memcpy(tmp, src->data + src->pos, (size_t)n);
        src->pos += (size_t)n;
    }
    rs_buf_wrap(&body, tmp, (size_t)n);
    return appearance_read(kind, &body, p);
}

static rs_err put_mask_hdr(rs_buf *dst, int mask, int signal) {
    int low = mask & 0xff;
    int high = (mask >> 8) & 0xff;
    if (high != 0) {
        SYNC_TRY(rs_p1(dst, low | signal));
        return rs_p1(dst, high);
    }
    return rs_p1(dst, low);
}

static rs_err get_mask_hdr(rs_buf *src, int signal, int known, int *mask) {
    int m;
    SYNC_TRY(need(src, 1));
    m = rs_g1(src);
    if (m & signal) {
        SYNC_TRY(need(src, 1));
        m += rs_g1(src) << 8;
    }
    m &= ~signal;
    if ((m & ~known) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    *mask = m;
    return RS_OK;
}

static rs_err put_public(int kind, rs_buf *dst, int colour, int effect, int rights, const uint8_t *packed, int n) {
    SYNC_TRY(check_packed(packed, n));
    if (colour < 0 || colour > 255 || effect < 0 || effect > 255 || rights < 0 || rights > 255) {
        return RS_ERR_RANGE;
    }
    if (kind == KIND_317) {
        SYNC_TRY(rs_p2_le(dst, (colour << 8) | effect));
        SYNC_TRY(rs_p1(dst, rights));
        SYNC_TRY(rs_p1_neg(dst, n));
        return put_rev(dst, packed, n);
    }
    if (kind == KIND_350) {
        SYNC_TRY(rs_p2_le_add128(dst, (colour << 8) | effect));
        SYNC_TRY(rs_p1(dst, rights));
        SYNC_TRY(rs_p1_sub128(dst, n));
        return put_rev128(dst, packed, n);
    }
    if (kind == KIND_254) {
        SYNC_TRY(rs_p2(dst, (colour << 8) | effect));
        SYNC_TRY(rs_p1(dst, rights));
        SYNC_TRY(rs_p1(dst, n));
        return rs_pdata(dst, packed, (size_t)n);
    }
    SYNC_TRY(rs_p2(dst, (colour << 8) | effect));
    SYNC_TRY(rs_p1_neg(dst, rights));
    SYNC_TRY(rs_p1_add128(dst, n));
    return put_fwd128(dst, packed, n);
}

static rs_err get_public(int kind, rs_buf *src, rs_sync_player *p) {
    int colour, rights, n;
    SYNC_TRY(need(src, 4));
    if (kind == KIND_317) {
        colour = rs_g2_le(src);
        rights = rs_g1(src);
        n = rs_g1_neg(src);
        p->chat_colour = (colour >> 8) & 0xff;
        p->chat_effect = colour & 0xff;
        p->chat_rights = rights;
        p->packed_len = n;
        return get_rev(src, p->packed, n);
    }
    if (kind == KIND_350) {
        colour = rs_g2_le_add128(src);
        rights = rs_g1(src);
        n = rs_g1_sub128(src);
        p->chat_colour = (colour >> 8) & 0xff;
        p->chat_effect = colour & 0xff;
        p->chat_rights = rights;
        p->packed_len = n;
        return get_rev128(src, p->packed, n);
    }
    if (kind == KIND_254) {
        colour = rs_g2(src);
        rights = rs_g1(src);
        n = rs_g1(src);
        p->chat_colour = (colour >> 8) & 0xff;
        p->chat_effect = colour & 0xff;
        p->chat_rights = rights;
        p->packed_len = n;
        SYNC_TRY(need(src, (size_t)n));
        if (n > 0) {
            memcpy(p->packed, src->data + src->pos, (size_t)n);
            src->pos += (size_t)n;
        }
        return RS_OK;
    }
    colour = rs_g2(src);
    rights = rs_g1_neg(src);
    n = rs_g1_add128(src);
    p->chat_colour = (colour >> 8) & 0xff;
    p->chat_effect = colour & 0xff;
    p->chat_rights = rights;
    p->packed_len = n;
    return get_fwd128(src, p->packed, n);
}

static int byte_in(int v) {
    return v >= 0 && v <= 255;
}

static rs_err player_mask_write(int kind, rs_buf *dst, int mask, const rs_sync_player *p) {
    if ((mask & ~player_known(kind)) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    SYNC_TRY(put_mask_hdr(dst, mask, player_signal(kind)));
    if (kind == KIND_317) {
        if (mask & RS_SYNC_P317_FORCE) {
            if (!byte_in(p->force_sx) || !byte_in(p->force_sz) || !byte_in(p->force_ex) ||
                !byte_in(p->force_ez) || !byte_in(p->force_dir)) {
                return RS_ERR_RANGE;
            }
            SYNC_TRY(rs_p1_sub128(dst, p->force_sx));
            SYNC_TRY(rs_p1_sub128(dst, p->force_sz));
            SYNC_TRY(rs_p1_sub128(dst, p->force_ex));
            SYNC_TRY(rs_p1_sub128(dst, p->force_ez));
            SYNC_TRY(put_u16(dst, p->force_end, 3));
            SYNC_TRY(put_u16(dst, p->force_start, 2));
            SYNC_TRY(rs_p1_sub128(dst, p->force_dir));
        }
        if (mask & RS_SYNC_P317_SPOTANIM) {
            SYNC_TRY(put_spot(dst, p->spot_id, p->spot_height, p->spot_delay, 1, 0));
        }
        if (mask & RS_SYNC_P317_ANIM) {
            SYNC_TRY(put_seq(dst, p->seq, 1));
            SYNC_TRY(rs_p1_neg(dst, p->seq_delay));
        }
        if (mask & RS_SYNC_P317_CHAT) {
            SYNC_TRY(put_chat(dst, p->chat, sizeof(p->chat)));
        }
        if (mask & RS_SYNC_P317_PUBLIC) {
            SYNC_TRY(put_public(kind, dst, p->chat_colour, p->chat_effect, p->chat_rights, p->packed, p->packed_len));
        }
        if (mask & RS_SYNC_P317_FACE_ENTITY) {
            SYNC_TRY(put_seq(dst, p->face_entity, 1));
        }
        if (mask & RS_SYNC_P317_APPEARANCE) {
            SYNC_TRY(put_appearance(kind, dst, p));
        }
        if (mask & RS_SYNC_P317_FACE_TILE) {
            SYNC_TRY(put_u16(dst, p->face_x, 3));
            SYNC_TRY(put_u16(dst, p->face_z, 1));
        }
        if (mask & RS_SYNC_P317_HIT) {
            SYNC_TRY(put_u8(dst, p->hit_damage));
            SYNC_TRY(rs_p1_add128(dst, p->hit_type));
            SYNC_TRY(rs_p1_neg(dst, p->hit_hp));
            SYNC_TRY(put_u8(dst, p->hit_max));
        }
        if (mask & RS_SYNC_P317_HIT2) {
            SYNC_TRY(put_u8(dst, p->hit2_damage));
            SYNC_TRY(rs_p1_sub128(dst, p->hit2_type));
            SYNC_TRY(put_u8(dst, p->hit2_hp));
            SYNC_TRY(rs_p1_neg(dst, p->hit2_max));
        }
        return RS_OK;
    }
    if (kind == KIND_350) {
        if (mask & RS_SYNC_P350_APPEARANCE) {
            SYNC_TRY(put_appearance(kind, dst, p));
        }
        if (mask & RS_SYNC_P350_SPOTANIM) {
            SYNC_TRY(put_spot(dst, p->spot_id, p->spot_height, p->spot_delay, 3, 1));
        }
        if (mask & RS_SYNC_P350_ANIM) {
            SYNC_TRY(put_seq(dst, p->seq, 1));
            SYNC_TRY(rs_p1_add128(dst, p->seq_delay));
        }
        if (mask & RS_SYNC_P350_FACE_TILE) {
            SYNC_TRY(put_u16(dst, p->face_x, 0));
            SYNC_TRY(put_u16(dst, p->face_z, 1));
        }
        if (mask & RS_SYNC_P350_FORCE) {
            if (!byte_in(p->force_sx) || !byte_in(p->force_sz) || !byte_in(p->force_ex) ||
                !byte_in(p->force_ez) || !byte_in(p->force_dir)) {
                return RS_ERR_RANGE;
            }
            SYNC_TRY(rs_p1_neg(dst, p->force_sx));
            SYNC_TRY(rs_p1_sub128(dst, p->force_sz));
            SYNC_TRY(rs_p1_add128(dst, p->force_ex));
            SYNC_TRY(rs_p1_add128(dst, p->force_ez));
            SYNC_TRY(put_u16(dst, p->force_start, 1));
            SYNC_TRY(put_u16(dst, p->force_end, 2));
            SYNC_TRY(put_u8(dst, p->force_dir));
        }
        if (mask & RS_SYNC_P350_CHAT) {
            SYNC_TRY(put_chat(dst, p->chat, sizeof(p->chat)));
        }
        if (mask & RS_SYNC_P350_FACE_ENTITY) {
            SYNC_TRY(put_seq(dst, p->face_entity, 0));
        }
        if (mask & RS_SYNC_P350_HIT) {
            SYNC_TRY(put_u8(dst, p->hit_damage));
            SYNC_TRY(rs_p1_sub128(dst, p->hit_type));
            SYNC_TRY(rs_p1_neg(dst, p->hit_hp));
            SYNC_TRY(rs_p1_sub128(dst, p->hit_max));
        }
        if (mask & RS_SYNC_P350_PUBLIC) {
            SYNC_TRY(put_public(kind, dst, p->chat_colour, p->chat_effect, p->chat_rights, p->packed, p->packed_len));
        }
        if (mask & RS_SYNC_P350_HIT2) {
            SYNC_TRY(rs_p1_sub128(dst, p->hit2_damage));
            SYNC_TRY(rs_p1_add128(dst, p->hit2_type));
            SYNC_TRY(put_u8(dst, p->hit2_hp));
            SYNC_TRY(rs_p1_sub128(dst, p->hit2_max));
        }
        return RS_OK;
    }
    if (kind == KIND_254) {
        if (mask & RS_SYNC_P254_APPEARANCE) {
            SYNC_TRY(put_appearance(kind, dst, p));
        }
        if (mask & RS_SYNC_P254_ANIM) {
            SYNC_TRY(put_seq(dst, p->seq, 0));
            SYNC_TRY(put_u8(dst, p->seq_delay));
        }
        if (mask & RS_SYNC_P254_FACE_ENTITY) {
            SYNC_TRY(put_seq(dst, p->face_entity, 0));
        }
        if (mask & RS_SYNC_P254_CHAT) {
            SYNC_TRY(put_chat(dst, p->chat, sizeof(p->chat)));
        }
        if (mask & RS_SYNC_P254_HIT) {
            SYNC_TRY(put_u8(dst, p->hit_damage));
            SYNC_TRY(put_u8(dst, p->hit_type));
            SYNC_TRY(put_u8(dst, p->hit_hp));
            SYNC_TRY(put_u8(dst, p->hit_max));
        }
        if (mask & RS_SYNC_P254_FACE_TILE) {
            SYNC_TRY(put_u16(dst, p->face_x, 0));
            SYNC_TRY(put_u16(dst, p->face_z, 0));
        }
        if (mask & RS_SYNC_P254_PUBLIC) {
            SYNC_TRY(put_public(kind, dst, p->chat_colour, p->chat_effect, p->chat_rights, p->packed, p->packed_len));
        }
        if (mask & RS_SYNC_P254_SPOTANIM) {
            SYNC_TRY(put_spot(dst, p->spot_id, p->spot_height, p->spot_delay, 0, 0));
        }
        if (mask & RS_SYNC_P254_FORCE) {
            if (!byte_in(p->force_sx) || !byte_in(p->force_sz) || !byte_in(p->force_ex) ||
                !byte_in(p->force_ez) || !byte_in(p->force_dir)) {
                return RS_ERR_RANGE;
            }
            SYNC_TRY(put_u8(dst, p->force_sx));
            SYNC_TRY(put_u8(dst, p->force_sz));
            SYNC_TRY(put_u8(dst, p->force_ex));
            SYNC_TRY(put_u8(dst, p->force_ez));
            SYNC_TRY(put_u16(dst, p->force_end, 0));
            SYNC_TRY(put_u16(dst, p->force_start, 0));
            SYNC_TRY(put_u8(dst, p->force_dir));
        }
        if (mask & RS_SYNC_P254_HIT2) {
            SYNC_TRY(put_u8(dst, p->hit2_damage));
            SYNC_TRY(put_u8(dst, p->hit2_type));
            SYNC_TRY(put_u8(dst, p->hit2_hp));
            SYNC_TRY(put_u8(dst, p->hit2_max));
        }
        return RS_OK;
    }
    if (mask & RS_SYNC_P377_ANIM) {
        SYNC_TRY(put_seq(dst, p->seq, 0));
        SYNC_TRY(rs_p1_sub128(dst, p->seq_delay));
    }
    if (mask & RS_SYNC_P377_CHAT) {
        SYNC_TRY(put_chat(dst, p->chat, sizeof(p->chat)));
    }
    if (mask & RS_SYNC_P377_FORCE) {
        if (!byte_in(p->force_sx) || !byte_in(p->force_sz) || !byte_in(p->force_ex) ||
            !byte_in(p->force_ez) || !byte_in(p->force_dir)) {
            return RS_ERR_RANGE;
        }
        SYNC_TRY(rs_p1_add128(dst, p->force_sx));
        SYNC_TRY(rs_p1_neg(dst, p->force_sz));
        SYNC_TRY(rs_p1_sub128(dst, p->force_ex));
        SYNC_TRY(put_u8(dst, p->force_ez));
        SYNC_TRY(put_u16(dst, p->force_start, 0));
        SYNC_TRY(put_u16(dst, p->force_end, 2));
        SYNC_TRY(put_u8(dst, p->force_dir));
    }
    if (mask & RS_SYNC_P377_FACE_ENTITY) {
        SYNC_TRY(put_seq(dst, p->face_entity, 2));
    }
    if (mask & RS_SYNC_P377_FACE_TILE) {
        SYNC_TRY(put_u16(dst, p->face_x, 0));
        SYNC_TRY(put_u16(dst, p->face_z, 0));
    }
    if (mask & RS_SYNC_P377_SPOTANIM) {
        SYNC_TRY(put_spot(dst, p->spot_id, p->spot_height, p->spot_delay, 2, 1));
    }
    if (mask & RS_SYNC_P377_APPEARANCE) {
        SYNC_TRY(put_appearance(kind, dst, p));
    }
    if (mask & RS_SYNC_P377_HIT) {
        SYNC_TRY(rs_p1_add128(dst, p->hit_damage));
        SYNC_TRY(rs_p1_sub128(dst, p->hit_type));
        SYNC_TRY(rs_p1_neg(dst, p->hit_hp));
        SYNC_TRY(put_u8(dst, p->hit_max));
    }
    if (mask & RS_SYNC_P377_PUBLIC) {
        SYNC_TRY(put_public(kind, dst, p->chat_colour, p->chat_effect, p->chat_rights, p->packed, p->packed_len));
    }
    if (mask & RS_SYNC_P377_HIT2) {
        SYNC_TRY(rs_p1_sub128(dst, p->hit2_damage));
        SYNC_TRY(rs_p1_neg(dst, p->hit2_type));
        SYNC_TRY(rs_p1_sub128(dst, p->hit2_hp));
        SYNC_TRY(put_u8(dst, p->hit2_max));
    }
    return RS_OK;
}

static rs_err player_mask_read(int kind, rs_buf *src, int mask, rs_sync_player *p) {
    p->mask = mask;
    if (kind == KIND_317) {
        if (mask & RS_SYNC_P317_FORCE) {
            SYNC_TRY(need(src, 4 + 2 + 2 + 1));
            p->force_sx = rs_g1_sub128(src);
            p->force_sz = rs_g1_sub128(src);
            p->force_ex = rs_g1_sub128(src);
            p->force_ez = rs_g1_sub128(src);
            p->force_end = rs_g2_le_add128(src);
            p->force_start = rs_g2_add128(src);
            p->force_dir = rs_g1_sub128(src);
        }
        if (mask & RS_SYNC_P317_SPOTANIM) {
            SYNC_TRY(get_spot(src, 1, 0, &p->spot_id, &p->spot_height, &p->spot_delay));
        }
        if (mask & RS_SYNC_P317_ANIM) {
            SYNC_TRY(need(src, 3));
            p->seq = rs_g2_le(src);
            if (p->seq == 65535) {
                p->seq = -1;
            }
            p->seq_delay = rs_g1_neg(src);
        }
        if (mask & RS_SYNC_P317_CHAT) {
            SYNC_TRY(rs_gjstr(src, p->chat, sizeof(p->chat)));
        }
        if (mask & RS_SYNC_P317_PUBLIC) {
            SYNC_TRY(get_public(kind, src, p));
        }
        if (mask & RS_SYNC_P317_FACE_ENTITY) {
            SYNC_TRY(need(src, 2));
            p->face_entity = rs_g2_le(src);
            if (p->face_entity == 65535) {
                p->face_entity = -1;
            }
        }
        if (mask & RS_SYNC_P317_APPEARANCE) {
            SYNC_TRY(get_appearance(kind, src, p));
        }
        if (mask & RS_SYNC_P317_FACE_TILE) {
            SYNC_TRY(need(src, 4));
            p->face_x = rs_g2_le_add128(src);
            p->face_z = rs_g2_le(src);
        }
        if (mask & RS_SYNC_P317_HIT) {
            SYNC_TRY(need(src, 4));
            p->hit_damage = rs_g1(src);
            p->hit_type = rs_g1_add128(src);
            p->hit_hp = rs_g1_neg(src);
            p->hit_max = rs_g1(src);
        }
        if (mask & RS_SYNC_P317_HIT2) {
            SYNC_TRY(need(src, 4));
            p->hit2_damage = rs_g1(src);
            p->hit2_type = rs_g1_sub128(src);
            p->hit2_hp = rs_g1(src);
            p->hit2_max = rs_g1_neg(src);
        }
        return RS_OK;
    }
    if (kind == KIND_350) {
        if (mask & RS_SYNC_P350_APPEARANCE) {
            SYNC_TRY(get_appearance(kind, src, p));
        }
        if (mask & RS_SYNC_P350_SPOTANIM) {
            SYNC_TRY(get_spot(src, 3, 1, &p->spot_id, &p->spot_height, &p->spot_delay));
        }
        if (mask & RS_SYNC_P350_ANIM) {
            SYNC_TRY(need(src, 3));
            p->seq = rs_g2_le(src);
            if (p->seq == 65535) {
                p->seq = -1;
            }
            p->seq_delay = rs_g1_add128(src);
        }
        if (mask & RS_SYNC_P350_FACE_TILE) {
            SYNC_TRY(need(src, 4));
            p->face_x = rs_g2(src);
            p->face_z = rs_g2_le(src);
        }
        if (mask & RS_SYNC_P350_FORCE) {
            SYNC_TRY(need(src, 4 + 2 + 2 + 1));
            p->force_sx = rs_g1_neg(src);
            p->force_sz = rs_g1_sub128(src);
            p->force_ex = rs_g1_add128(src);
            p->force_ez = rs_g1_add128(src);
            p->force_start = rs_g2_le(src);
            p->force_end = rs_g2_add128(src);
            p->force_dir = rs_g1(src);
        }
        if (mask & RS_SYNC_P350_CHAT) {
            SYNC_TRY(rs_gjstr(src, p->chat, sizeof(p->chat)));
        }
        if (mask & RS_SYNC_P350_FACE_ENTITY) {
            SYNC_TRY(need(src, 2));
            p->face_entity = rs_g2(src);
            if (p->face_entity == 65535) {
                p->face_entity = -1;
            }
        }
        if (mask & RS_SYNC_P350_HIT) {
            SYNC_TRY(need(src, 4));
            p->hit_damage = rs_g1(src);
            p->hit_type = rs_g1_sub128(src);
            p->hit_hp = rs_g1_neg(src);
            p->hit_max = rs_g1_sub128(src);
        }
        if (mask & RS_SYNC_P350_PUBLIC) {
            SYNC_TRY(get_public(kind, src, p));
        }
        if (mask & RS_SYNC_P350_HIT2) {
            SYNC_TRY(need(src, 4));
            p->hit2_damage = rs_g1_sub128(src);
            p->hit2_type = rs_g1_add128(src);
            p->hit2_hp = rs_g1(src);
            p->hit2_max = rs_g1_sub128(src);
        }
        return RS_OK;
    }
    if (kind == KIND_254) {
        if (mask & RS_SYNC_P254_APPEARANCE) {
            SYNC_TRY(get_appearance(kind, src, p));
        }
        if (mask & RS_SYNC_P254_ANIM) {
            SYNC_TRY(need(src, 3));
            p->seq = rs_g2(src);
            if (p->seq == 65535) {
                p->seq = -1;
            }
            p->seq_delay = rs_g1(src);
        }
        if (mask & RS_SYNC_P254_FACE_ENTITY) {
            SYNC_TRY(need(src, 2));
            p->face_entity = rs_g2(src);
            if (p->face_entity == 65535) {
                p->face_entity = -1;
            }
        }
        if (mask & RS_SYNC_P254_CHAT) {
            SYNC_TRY(rs_gjstr(src, p->chat, sizeof(p->chat)));
        }
        if (mask & RS_SYNC_P254_HIT) {
            SYNC_TRY(need(src, 4));
            p->hit_damage = rs_g1(src);
            p->hit_type = rs_g1(src);
            p->hit_hp = rs_g1(src);
            p->hit_max = rs_g1(src);
        }
        if (mask & RS_SYNC_P254_FACE_TILE) {
            SYNC_TRY(need(src, 4));
            p->face_x = rs_g2(src);
            p->face_z = rs_g2(src);
        }
        if (mask & RS_SYNC_P254_PUBLIC) {
            SYNC_TRY(get_public(kind, src, p));
        }
        if (mask & RS_SYNC_P254_SPOTANIM) {
            SYNC_TRY(get_spot(src, 0, 0, &p->spot_id, &p->spot_height, &p->spot_delay));
        }
        if (mask & RS_SYNC_P254_FORCE) {
            SYNC_TRY(need(src, 4 + 2 + 2 + 1));
            p->force_sx = rs_g1(src);
            p->force_sz = rs_g1(src);
            p->force_ex = rs_g1(src);
            p->force_ez = rs_g1(src);
            p->force_end = rs_g2(src);
            p->force_start = rs_g2(src);
            p->force_dir = rs_g1(src);
        }
        if (mask & RS_SYNC_P254_HIT2) {
            SYNC_TRY(need(src, 4));
            p->hit2_damage = rs_g1(src);
            p->hit2_type = rs_g1(src);
            p->hit2_hp = rs_g1(src);
            p->hit2_max = rs_g1(src);
        }
        return RS_OK;
    }
    if (mask & RS_SYNC_P377_ANIM) {
        SYNC_TRY(need(src, 3));
        p->seq = rs_g2(src);
        if (p->seq == 65535) {
            p->seq = -1;
        }
        p->seq_delay = rs_g1_sub128(src);
    }
    if (mask & RS_SYNC_P377_CHAT) {
        SYNC_TRY(rs_gjstr(src, p->chat, sizeof(p->chat)));
    }
    if (mask & RS_SYNC_P377_FORCE) {
        SYNC_TRY(need(src, 4 + 2 + 2 + 1));
        p->force_sx = rs_g1_add128(src);
        p->force_sz = rs_g1_neg(src);
        p->force_ex = rs_g1_sub128(src);
        p->force_ez = rs_g1(src);
        p->force_start = rs_g2(src);
        p->force_end = rs_g2_add128(src);
        p->force_dir = rs_g1(src);
    }
    if (mask & RS_SYNC_P377_FACE_ENTITY) {
        SYNC_TRY(need(src, 2));
        p->face_entity = rs_g2_add128(src);
        if (p->face_entity == 65535) {
            p->face_entity = -1;
        }
    }
    if (mask & RS_SYNC_P377_FACE_TILE) {
        SYNC_TRY(need(src, 4));
        p->face_x = rs_g2(src);
        p->face_z = rs_g2(src);
    }
    if (mask & RS_SYNC_P377_SPOTANIM) {
        SYNC_TRY(get_spot(src, 2, 1, &p->spot_id, &p->spot_height, &p->spot_delay));
    }
    if (mask & RS_SYNC_P377_APPEARANCE) {
        SYNC_TRY(get_appearance(kind, src, p));
    }
    if (mask & RS_SYNC_P377_HIT) {
        SYNC_TRY(need(src, 4));
        p->hit_damage = rs_g1_add128(src);
        p->hit_type = rs_g1_sub128(src);
        p->hit_hp = rs_g1_neg(src);
        p->hit_max = rs_g1(src);
    }
    if (mask & RS_SYNC_P377_PUBLIC) {
        SYNC_TRY(get_public(kind, src, p));
    }
    if (mask & RS_SYNC_P377_HIT2) {
        SYNC_TRY(need(src, 4));
        p->hit2_damage = rs_g1_sub128(src);
        p->hit2_type = rs_g1_neg(src);
        p->hit2_hp = rs_g1_sub128(src);
        p->hit2_max = rs_g1(src);
    }
    return RS_OK;
}

static rs_err npc_mask_write(int kind, rs_buf *dst, int mask, const rs_sync_npc *n) {
    if ((mask & ~0xff) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    SYNC_TRY(rs_p1(dst, mask & 0xff));
    if (kind == KIND_317) {
        if (mask & RS_SYNC_N317_ANIM) {
            SYNC_TRY(put_seq(dst, n->seq, 1));
            SYNC_TRY(put_u8(dst, n->seq_delay));
        }
        if (mask & RS_SYNC_N317_HIT) {
            SYNC_TRY(rs_p1_add128(dst, n->hit_damage));
            SYNC_TRY(rs_p1_neg(dst, n->hit_type));
            SYNC_TRY(rs_p1_add128(dst, n->hit_hp));
            SYNC_TRY(put_u8(dst, n->hit_max));
        }
        if (mask & RS_SYNC_N317_SPOTANIM) {
            SYNC_TRY(put_spot(dst, n->spot_id, n->spot_height, n->spot_delay, 0, 0));
        }
        if (mask & RS_SYNC_N317_FACE_ENTITY) {
            SYNC_TRY(put_seq(dst, n->face_entity, 0));
        }
        if (mask & RS_SYNC_N317_CHAT) {
            SYNC_TRY(put_chat(dst, n->chat, sizeof(n->chat)));
        }
        if (mask & RS_SYNC_N317_HIT2) {
            SYNC_TRY(rs_p1_neg(dst, n->hit2_damage));
            SYNC_TRY(rs_p1_sub128(dst, n->hit2_type));
            SYNC_TRY(rs_p1_sub128(dst, n->hit2_hp));
            SYNC_TRY(rs_p1_neg(dst, n->hit2_max));
        }
        if (mask & RS_SYNC_N317_TYPE) {
            SYNC_TRY(put_u16(dst, n->type, 3));
        }
        if (mask & RS_SYNC_N317_FACE_TILE) {
            SYNC_TRY(put_u16(dst, n->face_x, 1));
            SYNC_TRY(put_u16(dst, n->face_z, 1));
        }
        return RS_OK;
    }
    if (kind == KIND_350) {
        if (mask & RS_SYNC_N350_CHAT) {
            SYNC_TRY(put_chat(dst, n->chat, sizeof(n->chat)));
        }
        if (mask & RS_SYNC_N350_FACE_TILE) {
            SYNC_TRY(put_u16(dst, n->face_x, 1));
            SYNC_TRY(put_u16(dst, n->face_z, 0));
        }
        if (mask & RS_SYNC_N350_HIT) {
            SYNC_TRY(rs_p1_neg(dst, n->hit_damage));
            SYNC_TRY(rs_p1_neg(dst, n->hit_type));
            SYNC_TRY(rs_p1_neg(dst, n->hit_hp));
            SYNC_TRY(rs_p1_add128(dst, n->hit_max));
        }
        if (mask & RS_SYNC_N350_HIT2) {
            SYNC_TRY(rs_p1_neg(dst, n->hit2_damage));
            SYNC_TRY(rs_p1_add128(dst, n->hit2_type));
            SYNC_TRY(rs_p1_sub128(dst, n->hit2_hp));
            SYNC_TRY(put_u8(dst, n->hit2_max));
        }
        if (mask & RS_SYNC_N350_SPOTANIM) {
            SYNC_TRY(put_spot(dst, n->spot_id, n->spot_height, n->spot_delay, 2, 1));
        }
        if (mask & RS_SYNC_N350_TYPE) {
            SYNC_TRY(put_u16(dst, n->type, 3));
        }
        if (mask & RS_SYNC_N350_FACE_ENTITY) {
            SYNC_TRY(put_seq(dst, n->face_entity, 2));
        }
        if (mask & RS_SYNC_N350_ANIM) {
            SYNC_TRY(put_seq(dst, n->seq, 1));
            SYNC_TRY(rs_p1_neg(dst, n->seq_delay));
        }
        return RS_OK;
    }
    if (kind == KIND_254) {
        if (mask & RS_SYNC_N254_HIT) {
            SYNC_TRY(put_u8(dst, n->hit_damage));
            SYNC_TRY(put_u8(dst, n->hit_type));
            SYNC_TRY(put_u8(dst, n->hit_hp));
            SYNC_TRY(put_u8(dst, n->hit_max));
        }
        if (mask & RS_SYNC_N254_ANIM) {
            SYNC_TRY(put_seq(dst, n->seq, 0));
            SYNC_TRY(put_u8(dst, n->seq_delay));
        }
        if (mask & RS_SYNC_N254_FACE_ENTITY) {
            SYNC_TRY(put_seq(dst, n->face_entity, 0));
        }
        if (mask & RS_SYNC_N254_CHAT) {
            SYNC_TRY(put_chat(dst, n->chat, sizeof(n->chat)));
        }
        if (mask & RS_SYNC_N254_HIT2) {
            SYNC_TRY(put_u8(dst, n->hit2_damage));
            SYNC_TRY(put_u8(dst, n->hit2_type));
            SYNC_TRY(put_u8(dst, n->hit2_hp));
            SYNC_TRY(put_u8(dst, n->hit2_max));
        }
        if (mask & RS_SYNC_N254_TYPE) {
            SYNC_TRY(put_u16(dst, n->type, 0));
        }
        if (mask & RS_SYNC_N254_SPOTANIM) {
            SYNC_TRY(put_spot(dst, n->spot_id, n->spot_height, n->spot_delay, 0, 0));
        }
        if (mask & RS_SYNC_N254_FACE_TILE) {
            SYNC_TRY(put_u16(dst, n->face_x, 0));
            SYNC_TRY(put_u16(dst, n->face_z, 0));
        }
        return RS_OK;
    }
    if (mask & RS_SYNC_N377_TYPE) {
        SYNC_TRY(put_u16(dst, n->type, 2));
    }
    if (mask & RS_SYNC_N377_FACE_ENTITY) {
        SYNC_TRY(put_seq(dst, n->face_entity, 1));
    }
    if (mask & RS_SYNC_N377_HIT) {
        SYNC_TRY(rs_p1_add128(dst, n->hit_damage));
        SYNC_TRY(rs_p1_add128(dst, n->hit_type));
        SYNC_TRY(put_u8(dst, n->hit_hp));
        SYNC_TRY(rs_p1_sub128(dst, n->hit_max));
    }
    if (mask & RS_SYNC_N377_SPOTANIM) {
        SYNC_TRY(put_spot(dst, n->spot_id, n->spot_height, n->spot_delay, 0, 1));
    }
    if (mask & RS_SYNC_N377_CHAT) {
        SYNC_TRY(put_chat(dst, n->chat, sizeof(n->chat)));
    }
    if (mask & RS_SYNC_N377_FACE_TILE) {
        SYNC_TRY(put_u16(dst, n->face_x, 3));
        SYNC_TRY(put_u16(dst, n->face_z, 1));
    }
    if (mask & RS_SYNC_N377_ANIM) {
        SYNC_TRY(put_seq(dst, n->seq, 0));
        SYNC_TRY(rs_p1_sub128(dst, n->seq_delay));
    }
    if (mask & RS_SYNC_N377_HIT2) {
        SYNC_TRY(rs_p1_sub128(dst, n->hit2_damage));
        SYNC_TRY(rs_p1_sub128(dst, n->hit2_type));
        SYNC_TRY(put_u8(dst, n->hit2_hp));
        SYNC_TRY(rs_p1_neg(dst, n->hit2_max));
    }
    return RS_OK;
}

static rs_err npc_mask_read(int kind, rs_buf *src, int mask, rs_sync_npc *n) {
    n->mask = mask;
    if (kind == KIND_317) {
        if (mask & RS_SYNC_N317_ANIM) {
            SYNC_TRY(need(src, 3));
            n->seq = rs_g2_le(src);
            if (n->seq == 65535) {
                n->seq = -1;
            }
            n->seq_delay = rs_g1(src);
        }
        if (mask & RS_SYNC_N317_HIT) {
            SYNC_TRY(need(src, 4));
            n->hit_damage = rs_g1_add128(src);
            n->hit_type = rs_g1_neg(src);
            n->hit_hp = rs_g1_add128(src);
            n->hit_max = rs_g1(src);
        }
        if (mask & RS_SYNC_N317_SPOTANIM) {
            SYNC_TRY(get_spot(src, 0, 0, &n->spot_id, &n->spot_height, &n->spot_delay));
        }
        if (mask & RS_SYNC_N317_FACE_ENTITY) {
            SYNC_TRY(need(src, 2));
            n->face_entity = rs_g2(src);
            if (n->face_entity == 65535) {
                n->face_entity = -1;
            }
        }
        if (mask & RS_SYNC_N317_CHAT) {
            SYNC_TRY(rs_gjstr(src, n->chat, sizeof(n->chat)));
        }
        if (mask & RS_SYNC_N317_HIT2) {
            SYNC_TRY(need(src, 4));
            n->hit2_damage = rs_g1_neg(src);
            n->hit2_type = rs_g1_sub128(src);
            n->hit2_hp = rs_g1_sub128(src);
            n->hit2_max = rs_g1_neg(src);
        }
        if (mask & RS_SYNC_N317_TYPE) {
            SYNC_TRY(need(src, 2));
            n->type = rs_g2_le_add128(src);
        }
        if (mask & RS_SYNC_N317_FACE_TILE) {
            SYNC_TRY(need(src, 4));
            n->face_x = rs_g2_le(src);
            n->face_z = rs_g2_le(src);
        }
        return RS_OK;
    }
    if (kind == KIND_350) {
        if (mask & RS_SYNC_N350_CHAT) {
            SYNC_TRY(rs_gjstr(src, n->chat, sizeof(n->chat)));
        }
        if (mask & RS_SYNC_N350_FACE_TILE) {
            SYNC_TRY(need(src, 4));
            n->face_x = rs_g2_le(src);
            n->face_z = rs_g2(src);
        }
        if (mask & RS_SYNC_N350_HIT) {
            SYNC_TRY(need(src, 4));
            n->hit_damage = rs_g1_neg(src);
            n->hit_type = rs_g1_neg(src);
            n->hit_hp = rs_g1_neg(src);
            n->hit_max = rs_g1_add128(src);
        }
        if (mask & RS_SYNC_N350_HIT2) {
            SYNC_TRY(need(src, 4));
            n->hit2_damage = rs_g1_neg(src);
            n->hit2_type = rs_g1_add128(src);
            n->hit2_hp = rs_g1_sub128(src);
            n->hit2_max = rs_g1(src);
        }
        if (mask & RS_SYNC_N350_SPOTANIM) {
            SYNC_TRY(get_spot(src, 2, 1, &n->spot_id, &n->spot_height, &n->spot_delay));
        }
        if (mask & RS_SYNC_N350_TYPE) {
            SYNC_TRY(need(src, 2));
            n->type = rs_g2_le_add128(src);
        }
        if (mask & RS_SYNC_N350_FACE_ENTITY) {
            SYNC_TRY(need(src, 2));
            n->face_entity = rs_g2_add128(src);
            if (n->face_entity == 65535) {
                n->face_entity = -1;
            }
        }
        if (mask & RS_SYNC_N350_ANIM) {
            SYNC_TRY(need(src, 3));
            n->seq = rs_g2_le(src);
            if (n->seq == 65535) {
                n->seq = -1;
            }
            n->seq_delay = rs_g1_neg(src);
        }
        return RS_OK;
    }
    if (kind == KIND_254) {
        if (mask & RS_SYNC_N254_HIT) {
            SYNC_TRY(need(src, 4));
            n->hit_damage = rs_g1(src);
            n->hit_type = rs_g1(src);
            n->hit_hp = rs_g1(src);
            n->hit_max = rs_g1(src);
        }
        if (mask & RS_SYNC_N254_ANIM) {
            SYNC_TRY(need(src, 3));
            n->seq = rs_g2(src);
            if (n->seq == 65535) {
                n->seq = -1;
            }
            n->seq_delay = rs_g1(src);
        }
        if (mask & RS_SYNC_N254_FACE_ENTITY) {
            SYNC_TRY(need(src, 2));
            n->face_entity = rs_g2(src);
            if (n->face_entity == 65535) {
                n->face_entity = -1;
            }
        }
        if (mask & RS_SYNC_N254_CHAT) {
            SYNC_TRY(rs_gjstr(src, n->chat, sizeof(n->chat)));
        }
        if (mask & RS_SYNC_N254_HIT2) {
            SYNC_TRY(need(src, 4));
            n->hit2_damage = rs_g1(src);
            n->hit2_type = rs_g1(src);
            n->hit2_hp = rs_g1(src);
            n->hit2_max = rs_g1(src);
        }
        if (mask & RS_SYNC_N254_TYPE) {
            SYNC_TRY(need(src, 2));
            n->type = rs_g2(src);
        }
        if (mask & RS_SYNC_N254_SPOTANIM) {
            SYNC_TRY(get_spot(src, 0, 0, &n->spot_id, &n->spot_height, &n->spot_delay));
        }
        if (mask & RS_SYNC_N254_FACE_TILE) {
            SYNC_TRY(need(src, 4));
            n->face_x = rs_g2(src);
            n->face_z = rs_g2(src);
        }
        return RS_OK;
    }
    if (mask & RS_SYNC_N377_TYPE) {
        SYNC_TRY(need(src, 2));
        n->type = rs_g2_add128(src);
    }
    if (mask & RS_SYNC_N377_FACE_ENTITY) {
        SYNC_TRY(need(src, 2));
        n->face_entity = rs_g2_le(src);
        if (n->face_entity == 65535) {
            n->face_entity = -1;
        }
    }
    if (mask & RS_SYNC_N377_HIT) {
        SYNC_TRY(need(src, 4));
        n->hit_damage = rs_g1_add128(src);
        n->hit_type = rs_g1_add128(src);
        n->hit_hp = rs_g1(src);
        n->hit_max = rs_g1_sub128(src);
    }
    if (mask & RS_SYNC_N377_SPOTANIM) {
        SYNC_TRY(get_spot(src, 0, 1, &n->spot_id, &n->spot_height, &n->spot_delay));
    }
    if (mask & RS_SYNC_N377_CHAT) {
        SYNC_TRY(rs_gjstr(src, n->chat, sizeof(n->chat)));
    }
    if (mask & RS_SYNC_N377_FACE_TILE) {
        SYNC_TRY(need(src, 4));
        n->face_x = rs_g2_le_add128(src);
        n->face_z = rs_g2_le(src);
    }
    if (mask & RS_SYNC_N377_ANIM) {
        SYNC_TRY(need(src, 3));
        n->seq = rs_g2(src);
        if (n->seq == 65535) {
            n->seq = -1;
        }
        n->seq_delay = rs_g1_sub128(src);
    }
    if (mask & RS_SYNC_N377_HIT2) {
        SYNC_TRY(need(src, 4));
        n->hit2_damage = rs_g1_sub128(src);
        n->hit2_type = rs_g1_sub128(src);
        n->hit2_hp = rs_g1(src);
        n->hit2_max = rs_g1_neg(src);
    }
    return RS_OK;
}

static rs_err take_dirs(int dir1, int dir2, int run, int *d1, int *d2, int *is_run) {
    *d1 = -1;
    *d2 = -1;
    *is_run = 0;
    if (dir1 < 0 && dir2 < 0) {
        if (run) {
            return RS_ERR_ARG;
        }
        return RS_OK;
    }
    if (dir1 < 0 || dir1 > 7 || dir2 > 7) {
        return RS_ERR_ARG;
    }
    *d1 = dir1;
    if (run || dir2 >= 0) {
        if (dir2 < 0) {
            return RS_ERR_ARG;
        }
        *d2 = dir2;
        *is_run = 1;
    }
    return RS_OK;
}

/* allow_tele: local player. Otherwise a jump comes back as MV_REMOVE. */
static rs_err plan_move(int sentx, int sentz, int x, int z, int warp, int run, int dir1, int dir2, int allow_tele, sync_mv *m) {
    int d1, d2, is_run, dx, dz;
    SYNC_TRY(take_dirs(dir1, dir2, run, &d1, &d2, &is_run));
    m->dir1 = -1;
    m->dir2 = -1;
    m->x = x;
    m->z = z;
    m->jump = 1;
    if (warp) {
        m->op = allow_tele ? MV_TELE : MV_REMOVE;
        return RS_OK;
    }
    if (d1 >= 0) {
        int sx = sentx;
        int sz = sentz;
        apply_step(&sx, &sz, d1);
        m->dir1 = d1;
        if (is_run) {
            apply_step(&sx, &sz, d2);
            m->dir2 = d2;
            m->op = MV_RUN;
        } else {
            m->op = MV_WALK;
        }
        m->x = sx;
        m->z = sz;
        return RS_OK;
    }
    dx = x - sentx;
    dz = z - sentz;
    if (dx == 0 && dz == 0) {
        m->op = MV_NONE;
        return RS_OK;
    }
    d1 = find_dir(dx, dz);
    if (d1 >= 0 && !run) {
        m->dir1 = d1;
        m->op = MV_WALK;
        return RS_OK;
    }
    if (find_run(dx, dz, &d1, &d2)) {
        m->dir1 = d1;
        m->dir2 = d2;
        m->op = MV_RUN;
        return RS_OK;
    }
    m->op = allow_tele ? MV_TELE : MV_REMOVE;
    return RS_OK;
}

static void note_upd(sync_mv *m) {
    if (m->op == MV_NONE || m->op == MV_REMOVE) {
        m->upd = 0;
        return;
    }
    if (m->op == MV_STILL) {
        m->upd = 1;
        return;
    }
    m->upd = m->mask != 0;
}

static rs_err write_local(rs_buf *dst, int kind, const sync_mv *m) {
    if (m->op == MV_NONE) {
        return rs_pbit(dst, 1, 0);
    }
    SYNC_TRY(rs_pbit(dst, 1, 1));
    if (m->op == MV_STILL) {
        return rs_pbit(dst, 2, 0);
    }
    if (m->op == MV_WALK) {
        SYNC_TRY(rs_pbit(dst, 2, 1));
        SYNC_TRY(rs_pbit(dst, 3, m->dir1));
        return rs_pbit(dst, 1, m->upd);
    }
    if (m->op == MV_RUN) {
        SYNC_TRY(rs_pbit(dst, 2, 2));
        SYNC_TRY(rs_pbit(dst, 3, m->dir1));
        SYNC_TRY(rs_pbit(dst, 3, m->dir2));
        return rs_pbit(dst, 1, m->upd);
    }
    SYNC_TRY(rs_pbit(dst, 2, 3));
    if (kind == KIND_254) {
        SYNC_TRY(rs_pbit(dst, 2, m->plane));
        SYNC_TRY(rs_pbit(dst, 7, m->x));
        SYNC_TRY(rs_pbit(dst, 7, m->z));
        SYNC_TRY(rs_pbit(dst, 1, m->jump));
        return rs_pbit(dst, 1, m->upd);
    }
    if (kind == KIND_317) {
        SYNC_TRY(rs_pbit(dst, 2, m->plane));
        SYNC_TRY(rs_pbit(dst, 1, m->jump));
        SYNC_TRY(rs_pbit(dst, 1, m->upd));
        SYNC_TRY(rs_pbit(dst, 7, m->z));
        return rs_pbit(dst, 7, m->x);
    }
    if (kind == KIND_377) {
        SYNC_TRY(rs_pbit(dst, 1, m->jump));
        SYNC_TRY(rs_pbit(dst, 2, m->plane));
        SYNC_TRY(rs_pbit(dst, 7, m->z));
        SYNC_TRY(rs_pbit(dst, 7, m->x));
        return rs_pbit(dst, 1, m->upd);
    }
    SYNC_TRY(rs_pbit(dst, 7, m->x));
    SYNC_TRY(rs_pbit(dst, 7, m->z));
    SYNC_TRY(rs_pbit(dst, 2, m->plane));
    SYNC_TRY(rs_pbit(dst, 1, m->jump));
    return rs_pbit(dst, 1, m->upd);
}

static rs_err write_existing(rs_buf *dst, const sync_mv *list, int n) {
    int i;
    SYNC_TRY(rs_pbit(dst, 8, n));
    for (i = 0; i < n; i++) {
        const sync_mv *m = &list[i];
        if (m->op == MV_NONE) {
            SYNC_TRY(rs_pbit(dst, 1, 0));
            continue;
        }
        SYNC_TRY(rs_pbit(dst, 1, 1));
        if (m->op == MV_STILL) {
            SYNC_TRY(rs_pbit(dst, 2, 0));
        } else if (m->op == MV_WALK) {
            SYNC_TRY(rs_pbit(dst, 2, 1));
            SYNC_TRY(rs_pbit(dst, 3, m->dir1));
            SYNC_TRY(rs_pbit(dst, 1, m->upd));
        } else if (m->op == MV_RUN) {
            SYNC_TRY(rs_pbit(dst, 2, 2));
            SYNC_TRY(rs_pbit(dst, 3, m->dir1));
            SYNC_TRY(rs_pbit(dst, 3, m->dir2));
            SYNC_TRY(rs_pbit(dst, 1, m->upd));
        } else {
            SYNC_TRY(rs_pbit(dst, 2, 3));
        }
    }
    return RS_OK;
}

static rs_err write_new_player(rs_buf *dst, int kind, const sync_mv *m) {
    int dx, dz;
    if (m->dx < -16 || m->dx > 15 || m->dz < -16 || m->dz > 15) {
        return RS_ERR_RANGE;
    }
    dx = m->dx & 31;
    dz = m->dz & 31;
    SYNC_TRY(rs_pbit(dst, 11, m->index));
    if (kind == KIND_254) {
        SYNC_TRY(rs_pbit(dst, 5, dx));
        SYNC_TRY(rs_pbit(dst, 5, dz));
        SYNC_TRY(rs_pbit(dst, 1, m->jump));
        return rs_pbit(dst, 1, m->upd);
    }
    if (kind == KIND_317) {
        SYNC_TRY(rs_pbit(dst, 1, m->upd));
        SYNC_TRY(rs_pbit(dst, 1, m->jump));
        SYNC_TRY(rs_pbit(dst, 5, dz));
        return rs_pbit(dst, 5, dx);
    }
    if (kind == KIND_377) {
        SYNC_TRY(rs_pbit(dst, 5, dx));
        SYNC_TRY(rs_pbit(dst, 1, m->upd));
        SYNC_TRY(rs_pbit(dst, 1, m->jump));
        return rs_pbit(dst, 5, dz);
    }
    SYNC_TRY(rs_pbit(dst, 5, dz));
    SYNC_TRY(rs_pbit(dst, 1, m->upd));
    SYNC_TRY(rs_pbit(dst, 5, dx));
    return rs_pbit(dst, 1, m->jump);
}

static rs_err write_new_npc(rs_buf *dst, int kind, const sync_mv *m) {
    int dx, dz, width;
    if (m->dx < -16 || m->dx > 15 || m->dz < -16 || m->dz > 15) {
        return RS_ERR_RANGE;
    }
    width = npc_type_width(kind);
    if (m->type < 0 || m->type >= (1 << width)) {
        return RS_ERR_RANGE;
    }
    dx = m->dx & 31;
    dz = m->dz & 31;
    SYNC_TRY(rs_pbit(dst, 14, m->index));
    if (kind == KIND_254) {
        SYNC_TRY(rs_pbit(dst, 11, m->type));
        SYNC_TRY(rs_pbit(dst, 5, dx));
        SYNC_TRY(rs_pbit(dst, 5, dz));
        return rs_pbit(dst, 1, m->upd);
    }
    if (kind == KIND_317) {
        SYNC_TRY(rs_pbit(dst, 5, dz));
        SYNC_TRY(rs_pbit(dst, 5, dx));
        SYNC_TRY(rs_pbit(dst, 1, m->jump));
        SYNC_TRY(rs_pbit(dst, 12, m->type));
        return rs_pbit(dst, 1, m->upd);
    }
    if (kind == KIND_377) {
        SYNC_TRY(rs_pbit(dst, 1, m->upd));
        SYNC_TRY(rs_pbit(dst, 5, dz));
        SYNC_TRY(rs_pbit(dst, 5, dx));
        SYNC_TRY(rs_pbit(dst, 1, m->jump));
        return rs_pbit(dst, 13, m->type);
    }
    SYNC_TRY(rs_pbit(dst, 1, m->jump));
    SYNC_TRY(rs_pbit(dst, 13, m->type));
    SYNC_TRY(rs_pbit(dst, 1, m->upd));
    SYNC_TRY(rs_pbit(dst, 5, dz));
    return rs_pbit(dst, 5, dx);
}

static rs_err read_local_bits(rs_buf *src, int kind, int *op, int *d1, int *d2, int *upd, int *x, int *z, int *plane, int *jump) {
    int flag, type;
    *op = MV_NONE;
    *d1 = -1;
    *d2 = -1;
    *upd = 0;
    *jump = 0;
    SYNC_TRY(gbits(src, 1, &flag));
    if (flag == 0) {
        return RS_OK;
    }
    SYNC_TRY(gbits(src, 2, &type));
    if (type == 0) {
        *op = MV_STILL;
        *upd = 1;
        return RS_OK;
    }
    if (type == 1) {
        *op = MV_WALK;
        SYNC_TRY(gbits(src, 3, d1));
        SYNC_TRY(gbits(src, 1, upd));
        return RS_OK;
    }
    if (type == 2) {
        *op = MV_RUN;
        SYNC_TRY(gbits(src, 3, d1));
        SYNC_TRY(gbits(src, 3, d2));
        SYNC_TRY(gbits(src, 1, upd));
        return RS_OK;
    }
    *op = MV_TELE;
    if (kind == KIND_254) {
        SYNC_TRY(gbits(src, 2, plane));
        SYNC_TRY(gbits(src, 7, x));
        SYNC_TRY(gbits(src, 7, z));
        SYNC_TRY(gbits(src, 1, jump));
        return gbits(src, 1, upd);
    }
    if (kind == KIND_317) {
        SYNC_TRY(gbits(src, 2, plane));
        SYNC_TRY(gbits(src, 1, jump));
        SYNC_TRY(gbits(src, 1, upd));
        SYNC_TRY(gbits(src, 7, z));
        return gbits(src, 7, x);
    }
    if (kind == KIND_377) {
        SYNC_TRY(gbits(src, 1, jump));
        SYNC_TRY(gbits(src, 2, plane));
        SYNC_TRY(gbits(src, 7, z));
        SYNC_TRY(gbits(src, 7, x));
        return gbits(src, 1, upd);
    }
    SYNC_TRY(gbits(src, 7, x));
    SYNC_TRY(gbits(src, 7, z));
    SYNC_TRY(gbits(src, 2, plane));
    SYNC_TRY(gbits(src, 1, jump));
    return gbits(src, 1, upd);
}

static rs_err read_existing_one(rs_buf *src, int *op, int *d1, int *d2, int *upd) {
    int flag, type;
    *op = MV_NONE;
    *d1 = -1;
    *d2 = -1;
    *upd = 0;
    SYNC_TRY(gbits(src, 1, &flag));
    if (flag == 0) {
        return RS_OK;
    }
    SYNC_TRY(gbits(src, 2, &type));
    if (type == 0) {
        *op = MV_STILL;
        *upd = 1;
        return RS_OK;
    }
    if (type == 1) {
        *op = MV_WALK;
        SYNC_TRY(gbits(src, 3, d1));
        return gbits(src, 1, upd);
    }
    if (type == 2) {
        *op = MV_RUN;
        SYNC_TRY(gbits(src, 3, d1));
        SYNC_TRY(gbits(src, 3, d2));
        return gbits(src, 1, upd);
    }
    *op = MV_REMOVE;
    return RS_OK;
}

static rs_err read_new_player_bits(rs_buf *src, int kind, int *upd, int *jump, int *dx, int *dz) {
    int x5, z5;
    if (kind == KIND_254) {
        SYNC_TRY(gbits(src, 5, &x5));
        SYNC_TRY(gbits(src, 5, &z5));
        SYNC_TRY(gbits(src, 1, jump));
        SYNC_TRY(gbits(src, 1, upd));
    } else if (kind == KIND_317) {
        SYNC_TRY(gbits(src, 1, upd));
        SYNC_TRY(gbits(src, 1, jump));
        SYNC_TRY(gbits(src, 5, &z5));
        SYNC_TRY(gbits(src, 5, &x5));
    } else if (kind == KIND_377) {
        SYNC_TRY(gbits(src, 5, &x5));
        SYNC_TRY(gbits(src, 1, upd));
        SYNC_TRY(gbits(src, 1, jump));
        SYNC_TRY(gbits(src, 5, &z5));
    } else {
        SYNC_TRY(gbits(src, 5, &z5));
        SYNC_TRY(gbits(src, 1, upd));
        SYNC_TRY(gbits(src, 5, &x5));
        SYNC_TRY(gbits(src, 1, jump));
    }
    *dx = sign5(x5);
    *dz = sign5(z5);
    return RS_OK;
}

static rs_err read_new_npc_bits(rs_buf *src, int kind, int *upd, int *jump, int *dx, int *dz, int *type) {
    int x5, z5;
    if (kind == KIND_254) {
        *jump = 0;
        SYNC_TRY(gbits(src, 11, type));
        SYNC_TRY(gbits(src, 5, &x5));
        SYNC_TRY(gbits(src, 5, &z5));
        SYNC_TRY(gbits(src, 1, upd));
    } else if (kind == KIND_317) {
        SYNC_TRY(gbits(src, 5, &z5));
        SYNC_TRY(gbits(src, 5, &x5));
        SYNC_TRY(gbits(src, 1, jump));
        SYNC_TRY(gbits(src, 12, type));
        SYNC_TRY(gbits(src, 1, upd));
    } else if (kind == KIND_377) {
        SYNC_TRY(gbits(src, 1, upd));
        SYNC_TRY(gbits(src, 5, &z5));
        SYNC_TRY(gbits(src, 5, &x5));
        SYNC_TRY(gbits(src, 1, jump));
        SYNC_TRY(gbits(src, 13, type));
    } else {
        SYNC_TRY(gbits(src, 1, jump));
        SYNC_TRY(gbits(src, 13, type));
        SYNC_TRY(gbits(src, 1, upd));
        SYNC_TRY(gbits(src, 5, &z5));
        SYNC_TRY(gbits(src, 5, &x5));
    }
    *dx = sign5(x5);
    *dz = sign5(z5);
    return RS_OK;
}

static rs_err npc_ensure(rs_sync_view *v, int index, int create, sync_npc_node **out) {
    int slot;
    sync_npc_node *grown;
    int ncap;
    if (index < 0 || index >= NPC_INDEX_MAX) {
        return RS_ERR_ARG;
    }
    slot = v->nmap[index];
    if (slot >= 0) {
        *out = &v->nodes[slot];
        return RS_OK;
    }
    if (!create) {
        return RS_ERR_NOT_FOUND;
    }
    if (v->ncount == v->ncap) {
        ncap = v->ncap == 0 ? 16 : v->ncap * 2;
        if (ncap > NPC_CAP) {
            ncap = NPC_CAP;
        }
        if (v->ncount >= ncap) {
            return RS_ERR_RANGE;
        }
        grown = rs_realloc(v->nodes, (size_t)ncap * sizeof(*grown));
        if (!grown) {
            return RS_ERR_NOMEM;
        }
        memset(grown + v->ncap, 0, (size_t)(ncap - v->ncap) * sizeof(*grown));
        v->nodes = grown;
        v->ncap = ncap;
    }
    slot = v->ncount++;
    v->nmap[index] = (int16_t)slot;
    memset(&v->nodes[slot], 0, sizeof(v->nodes[slot]));
    v->nodes[slot].index = index;
    v->nodes[slot].sent_type = -1;
    v->nodes[slot].npc.dir1 = -1;
    v->nodes[slot].npc.dir2 = -1;
    *out = &v->nodes[slot];
    return RS_OK;
}

static rs_err delta5(int x, int z, int ox, int oz, int *dx, int *dz) {
    *dx = x - ox;
    *dz = z - oz;
    if (*dx < -16 || *dx > 15 || *dz < -16 || *dz > 15) {
        return RS_ERR_RANGE;
    }
    return RS_OK;
}

static void consume_player(rs_sync_player *p, const sync_mv *m) {
    p->x = m->x;
    p->z = m->z;
    p->plane = m->plane;
    p->warp = 0;
    p->run = 0;
    p->dir1 = -1;
    p->dir2 = -1;
    p->mask = 0;
    p->active = 1;
}

static void consume_npc(sync_npc_node *node, const sync_mv *m) {
    node->npc.x = m->x;
    node->npc.z = m->z;
    node->npc.plane = m->plane;
    node->npc.type = m->type;
    node->npc.warp = 0;
    node->npc.run = 0;
    node->npc.dir1 = -1;
    node->npc.dir2 = -1;
    node->npc.mask = 0;
    node->npc.active = 1;
    node->in_view = 1;
    node->sent_x = m->x;
    node->sent_z = m->z;
    node->sent_type = m->type;
}

rs_sync_view *rs_sync_view_create(int revision) {
    rs_sync_view *v;
    int i;
    v = rs_calloc(1, sizeof(*v));
    if (!v) {
        return NULL;
    }
    v->revision = revision;
    v->kind = kind_of(revision);
    v->local = -1;
    v->players = rs_calloc(2048, sizeof(*v->players));
    v->nmap = rs_malloc((size_t)16384 * sizeof(int16_t));
    if (!v->players || !v->nmap) {
        rs_sync_view_destroy(v);
        return NULL;
    }
    for (i = 0; i < 2048; i++) {
        v->players[i].dir1 = -1;
        v->players[i].dir2 = -1;
    }
    for (i = 0; i < 16384; i++) {
        v->nmap[i] = -1;
    }
    return v;
}

void rs_sync_view_destroy(rs_sync_view *v) {
    if (!v) {
        return;
    }
    free(v->players);
    free(v->nmap);
    free(v->nodes);
    free(v);
}

rs_err rs_sync_set_local(rs_sync_view *v, int player_index) {
    if (!v || player_index < 0 || player_index > 2047) {
        return RS_ERR_ARG;
    }
    v->local = player_index;
    return RS_OK;
}

rs_err rs_sync_player_set(rs_sync_view *v, int index, const rs_sync_player *p) {
    if (!v || !p || index < 0 || index > 2047) {
        return RS_ERR_ARG;
    }
    if (index == 2047 && v->local != 2047) {
        return RS_ERR_ARG;
    }
    v->players[index] = *p;
    return RS_OK;
}

rs_err rs_sync_player_get(const rs_sync_view *v, int index, rs_sync_player *out) {
    if (!v || !out || index < 0 || index > 2047) {
        return RS_ERR_ARG;
    }
    *out = v->players[index];
    return RS_OK;
}

rs_err rs_sync_player_clear(rs_sync_view *v, int index) {
    if (!v || index < 0 || index > 2047) {
        return RS_ERR_ARG;
    }
    v->players[index].active = 0;
    return RS_OK;
}

rs_err rs_sync_npc_set(rs_sync_view *v, int index, int type, int x, int z, int plane, int warp) {
    sync_npc_node *node;
    rs_err e;
    if (!v) {
        return RS_ERR_ARG;
    }
    e = npc_ensure(v, index, 1, &node);
    if (e != RS_OK) {
        return e;
    }
    node->npc.active = 1;
    node->npc.type = type;
    node->npc.x = x;
    node->npc.z = z;
    node->npc.plane = plane;
    node->npc.warp = warp;
    node->npc.run = 0;
    node->npc.dir1 = -1;
    node->npc.dir2 = -1;
    return RS_OK;
}

rs_err rs_sync_npc_put(rs_sync_view *v, int index, const rs_sync_npc *n) {
    sync_npc_node *node;
    rs_err e;
    int in_view, sx, sz, st;
    if (!v || !n) {
        return RS_ERR_ARG;
    }
    e = npc_ensure(v, index, 1, &node);
    if (e != RS_OK) {
        return e;
    }
    in_view = node->in_view;
    sx = node->sent_x;
    sz = node->sent_z;
    st = node->sent_type;
    node->npc = *n;
    node->npc.active = 1;
    node->index = index;
    node->in_view = in_view;
    node->sent_x = sx;
    node->sent_z = sz;
    node->sent_type = st;
    return RS_OK;
}

rs_err rs_sync_npc_get(const rs_sync_view *v, int index, rs_sync_npc *out) {
    int slot;
    if (!v || !out || index < 0 || index >= NPC_INDEX_MAX) {
        return RS_ERR_ARG;
    }
    slot = v->nmap[index];
    if (slot < 0 || !v->nodes[slot].npc.active) {
        return RS_ERR_NOT_FOUND;
    }
    *out = v->nodes[slot].npc;
    return RS_OK;
}

rs_err rs_sync_npc_clear(rs_sync_view *v, int index) {
    int slot;
    if (!v || index < 0 || index >= NPC_INDEX_MAX) {
        return RS_ERR_ARG;
    }
    slot = v->nmap[index];
    if (slot >= 0) {
        v->nodes[slot].npc.active = 0;
    }
    return RS_OK;
}

static rs_err check_tile7(int x, int z, int plane) {
    if (plane < 0 || plane > 3 || x < 0 || x > 127 || z < 0 || z > 127) {
        return RS_ERR_RANGE;
    }
    return RS_OK;
}

rs_err rs_sync_player_encode(rs_sync_view *v, rs_buf *dst) {
    sync_mv local;
    sync_mv oldv[LIST_CAP];
    sync_mv addv[LIST_CAP];
    int q[1 + LIST_CAP + LIST_CAP];
    uint8_t stay[2048];
    uint8_t keep[2048];
    int addn, qn, i, lx, lz, lp;
    rs_sync_player *lp_player;
    rs_err e;

    e = sync_use(v);
    if (e != RS_OK) {
        return e;
    }
    if (!dst || v->local < 0) {
        return RS_ERR_ARG;
    }
    lp_player = &v->players[v->local];
    if (!lp_player->active) {
        return RS_ERR_ARG;
    }
    memset(&local, 0, sizeof(local));
    memset(stay, 0, sizeof(stay));
    local.index = v->local;
    local.mask = lp_player->mask;
    local.plane = lp_player->plane;
    local.jump = 1;
    if (!v->pin[v->local] || lp_player->warp || lp_player->plane != v->psp[v->local]) {
        e = check_tile7(lp_player->x, lp_player->z, lp_player->plane);
        if (e != RS_OK) {
            return e;
        }
        local.op = MV_TELE;
        local.x = lp_player->x;
        local.z = lp_player->z;
    } else {
        e = plan_move(v->psx[v->local], v->psz[v->local], lp_player->x, lp_player->z, 0, lp_player->run,
                      lp_player->dir1, lp_player->dir2, 1, &local);
        if (e != RS_OK) {
            return e;
        }
        local.mask = lp_player->mask;
        local.plane = lp_player->plane;
        if (local.op == MV_TELE) {
            e = check_tile7(local.x, local.z, local.plane);
            if (e != RS_OK) {
                return e;
            }
            local.jump = 1;
        }
    }
    if ((local.mask & ~player_known(v->kind)) != 0) {
        return RS_ERR_UNSUPPORTED;
    }
    if (local.op == MV_NONE && local.mask != 0) {
        local.op = MV_STILL;
    }
    note_upd(&local);
    lx = local.x;
    lz = local.z;
    lp = local.plane;

    for (i = 0; i < v->pn; i++) {
        int id = v->pids[i];
        rs_sync_player *p = &v->players[id];
        sync_mv *m = &oldv[i];
        memset(m, 0, sizeof(*m));
        m->index = id;
        m->dir1 = -1;
        m->dir2 = -1;
        if (id == v->local || !p->active) {
            m->op = MV_REMOVE;
            continue;
        }
        if (p->plane != lp) {
            return RS_ERR_RANGE;
        }
        if ((p->mask & ~player_known(v->kind)) != 0) {
            return RS_ERR_UNSUPPORTED;
        }
        e = plan_move(v->psx[id], v->psz[id], p->x, p->z, p->warp, p->run, p->dir1, p->dir2, 0, m);
        if (e != RS_OK) {
            return e;
        }
        m->index = id;
        m->mask = p->mask;
        m->plane = lp;
        if (m->op == MV_REMOVE) {
            continue;
        }
        if (m->op == MV_NONE && m->mask != 0) {
            m->op = MV_STILL;
        }
        note_upd(m);
        stay[id] = 1;
    }

    addn = 0;
    for (i = 0; i < 2048; i++) {
        rs_sync_player *p;
        sync_mv *m;
        if (i == v->local || stay[i] || !v->players[i].active) {
            continue;
        }
        if (i == 2047) {
            return RS_ERR_ARG;
        }
        if (addn >= LIST_CAP) {
            return RS_ERR_RANGE;
        }
        p = &v->players[i];
        if (p->plane != lp) {
            return RS_ERR_RANGE;
        }
        if ((p->mask & ~player_known(v->kind)) != 0) {
            return RS_ERR_UNSUPPORTED;
        }
        m = &addv[addn];
        memset(m, 0, sizeof(*m));
        m->index = i;
        m->op = MV_ADD;
        m->x = p->x;
        m->z = p->z;
        m->plane = lp;
        m->mask = p->mask;
        m->jump = p->warp ? 1 : 0;
        m->dir1 = -1;
        m->dir2 = -1;
        e = delta5(p->x, p->z, lx, lz, &m->dx, &m->dz);
        if (e != RS_OK) {
            return e;
        }
        note_upd(m);
        addn++;
    }
    {
        int stays = 0;
        for (i = 0; i < v->pn; i++) {
            if (oldv[i].op != MV_REMOVE) {
                stays++;
            }
        }
        if (stays + addn > LIST_CAP) {
            return RS_ERR_RANGE;
        }
    }

    rs_access_bits(dst);
    SYNC_TRY(write_local(dst, v->kind, &local));
    SYNC_TRY(write_existing(dst, oldv, v->pn));
    for (i = 0; i < addn; i++) {
        SYNC_TRY(write_new_player(dst, v->kind, &addv[i]));
    }
    SYNC_TRY(rs_pbit(dst, 11, 2047));
    finish_bits(dst);

    qn = 0;
    if (local.upd) {
        q[qn++] = v->local;
    }
    for (i = 0; i < v->pn; i++) {
        if (oldv[i].upd) {
            q[qn++] = oldv[i].index;
        }
    }
    for (i = 0; i < addn; i++) {
        if (addv[i].upd) {
            q[qn++] = addv[i].index;
        }
    }
    for (i = 0; i < qn; i++) {
        rs_sync_player *p = &v->players[q[i]];
        SYNC_TRY(player_mask_write(v->kind, dst, p->mask, p));
    }

    {
        int prev = v->pn;
        int np = 0;
        int new_ids[LIST_CAP];
        memset(keep, 0, sizeof(keep));
        consume_player(lp_player, &local);
        v->pin[v->local] = 1;
        v->psx[v->local] = local.x;
        v->psz[v->local] = local.z;
        v->psp[v->local] = local.plane;
        keep[v->local] = 1;
        for (i = 0; i < prev; i++) {
            if (oldv[i].op == MV_REMOVE) {
                continue;
            }
            new_ids[np++] = oldv[i].index;
            consume_player(&v->players[oldv[i].index], &oldv[i]);
            v->psx[oldv[i].index] = oldv[i].x;
            v->psz[oldv[i].index] = oldv[i].z;
            v->psp[oldv[i].index] = oldv[i].plane;
            v->pin[oldv[i].index] = 1;
            keep[oldv[i].index] = 1;
        }
        for (i = 0; i < addn; i++) {
            new_ids[np++] = addv[i].index;
            consume_player(&v->players[addv[i].index], &addv[i]);
            v->psx[addv[i].index] = addv[i].x;
            v->psz[addv[i].index] = addv[i].z;
            v->psp[addv[i].index] = addv[i].plane;
            v->pin[addv[i].index] = 1;
            keep[addv[i].index] = 1;
        }
        for (i = 0; i < 2048; i++) {
            if (v->pin[i] && !keep[i]) {
                v->pin[i] = 0;
                if (i != v->local) {
                    player_blank(&v->players[i]);
                }
            }
        }
        for (i = 0; i < np; i++) {
            v->pids[i] = new_ids[i];
        }
        v->pn = np;
    }
    return RS_OK;
}

static int mv_live(const sync_mv *list, int n, int index) {
    int i;
    for (i = 0; i < n; i++) {
        if (list[i].index == index && list[i].op != MV_REMOVE) {
            return 1;
        }
    }
    return 0;
}

static void sort_adds(sync_mv *a, int n) {
    int i, j;
    for (i = 1; i < n; i++) {
        sync_mv key = a[i];
        j = i - 1;
        while (j >= 0 && a[j].index > key.index) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

static void settle_player(rs_sync_view *v, int index, int x, int z, int plane, int keep_mask) {
    rs_sync_player *p = &v->players[index];
    p->x = x;
    p->z = z;
    p->plane = plane;
    p->warp = 0;
    p->run = 0;
    p->dir1 = -1;
    p->dir2 = -1;
    p->active = 1;
    if (!keep_mask) {
        p->mask = 0;
    }
    v->pin[index] = 1;
    v->psx[index] = x;
    v->psz[index] = z;
    v->psp[index] = plane;
}

static void settle_npc(sync_npc_node *node, int x, int z, int plane, int keep_mask) {
    node->npc.x = x;
    node->npc.z = z;
    node->npc.plane = plane;
    node->npc.warp = 0;
    node->npc.run = 0;
    node->npc.dir1 = -1;
    node->npc.dir2 = -1;
    node->npc.active = 1;
    if (!keep_mask) {
        node->npc.mask = 0;
    }
    node->in_view = 1;
    node->sent_x = x;
    node->sent_z = z;
    node->sent_type = node->npc.type;
}

static void deactivate_npc(sync_npc_node *node) {
    node->in_view = 0;
    node->npc.active = 0;
    node->npc.mask = 0;
    node->npc.warp = 0;
    node->npc.run = 0;
    node->npc.dir1 = -1;
    node->npc.dir2 = -1;
}

static sync_npc_node *npc_slot(rs_sync_view *v, int index) {
    int slot;
    if (index < 0 || index >= NPC_INDEX_MAX) {
        return NULL;
    }
    slot = v->nmap[index];
    if (slot < 0 || slot >= v->ncount) {
        return NULL;
    }
    return &v->nodes[slot];
}

rs_err rs_sync_npc_encode(rs_sync_view *v, rs_buf *dst) {
    sync_mv oldv[LIST_CAP];
    sync_mv addv[LIST_CAP];
    int addn, i, any, lx, lz, lp, prev, stays;
    rs_err e;

    e = sync_use(v);
    if (e != RS_OK) {
        return e;
    }
    if (!dst || v->local < 0 || !v->players[v->local].active) {
        return RS_ERR_ARG;
    }
    lx = v->players[v->local].x;
    lz = v->players[v->local].z;
    lp = v->players[v->local].plane;
    prev = v->nn;
    for (i = 0; i < prev; i++) {
        int id = v->nids[i];
        sync_npc_node *node = npc_slot(v, id);
        sync_mv *m = &oldv[i];
        memset(m, 0, sizeof(*m));
        m->index = id;
        m->dir1 = -1;
        m->dir2 = -1;
        if (!node || !node->npc.active) {
            m->op = MV_REMOVE;
            continue;
        }
        if (node->npc.plane != lp) {
            return RS_ERR_RANGE;
        }
        if ((node->npc.mask & ~0xff) != 0) {
            return RS_ERR_UNSUPPORTED;
        }
        e = plan_move(node->sent_x, node->sent_z, node->npc.x, node->npc.z, node->npc.warp, node->npc.run,
                      node->npc.dir1, node->npc.dir2, 0, m);
        if (e != RS_OK) {
            return e;
        }
        m->index = id;
        m->mask = node->npc.mask;
        m->plane = lp;
        m->type = node->npc.type;
        if (node->in_view && m->op != MV_REMOVE && node->npc.type != node->sent_type) {
            m->mask |= npc_type_bit(v->kind);
        }
        if (m->op == MV_NONE && m->mask != 0) {
            m->op = MV_STILL;
        }
        if (m->op != MV_REMOVE) {
            note_upd(m);
        }
    }

    addn = 0;
    for (i = 0; i < v->ncount; i++) {
        sync_npc_node *node = &v->nodes[i];
        sync_mv *m;
        int width;
        if (!node->npc.active || mv_live(oldv, prev, node->index)) {
            continue;
        }
        if (node->npc.plane != lp) {
            return RS_ERR_RANGE;
        }
        if ((node->npc.mask & ~0xff) != 0) {
            return RS_ERR_UNSUPPORTED;
        }
        width = npc_type_width(v->kind);
        if (node->npc.type < 0 || node->npc.type >= (1 << width)) {
            return RS_ERR_RANGE;
        }
        if (addn >= LIST_CAP) {
            return RS_ERR_RANGE;
        }
        m = &addv[addn];
        memset(m, 0, sizeof(*m));
        m->index = node->index;
        m->op = MV_ADD;
        m->x = node->npc.x;
        m->z = node->npc.z;
        m->plane = lp;
        m->type = node->npc.type;
        m->mask = node->npc.mask;
        m->jump = node->npc.warp ? 1 : 0;
        m->dir1 = -1;
        m->dir2 = -1;
        e = delta5(node->npc.x, node->npc.z, lx, lz, &m->dx, &m->dz);
        if (e != RS_OK) {
            return e;
        }
        note_upd(m);
        addn++;
    }
    sort_adds(addv, addn);
    stays = 0;
    for (i = 0; i < prev; i++) {
        if (oldv[i].op != MV_REMOVE) {
            stays++;
        }
    }
    if (stays + addn > LIST_CAP) {
        return RS_ERR_RANGE;
    }

    any = 0;
    for (i = 0; i < prev; i++) {
        if (oldv[i].upd) {
            any = 1;
        }
    }
    for (i = 0; i < addn; i++) {
        if (addv[i].upd) {
            any = 1;
        }
    }

    rs_access_bits(dst);
    SYNC_TRY(write_existing(dst, oldv, prev));
    for (i = 0; i < addn; i++) {
        SYNC_TRY(write_new_npc(dst, v->kind, &addv[i]));
    }
    /* 16383 only when a mask byte follows; a mask-less block never satisfies +21. */
    if (any) {
        SYNC_TRY(rs_pbit(dst, 14, 16383));
    }
    finish_bits(dst);
    for (i = 0; i < prev; i++) {
        sync_npc_node *node;
        if (!oldv[i].upd) {
            continue;
        }
        node = npc_slot(v, oldv[i].index);
        if (!node) {
            return RS_ERR_FORMAT;
        }
        SYNC_TRY(npc_mask_write(v->kind, dst, oldv[i].mask, &node->npc));
    }
    for (i = 0; i < addn; i++) {
        sync_npc_node *node;
        if (!addv[i].upd) {
            continue;
        }
        node = npc_slot(v, addv[i].index);
        if (!node) {
            return RS_ERR_FORMAT;
        }
        SYNC_TRY(npc_mask_write(v->kind, dst, addv[i].mask, &node->npc));
    }

    {
        int np = 0;
        int new_ids[LIST_CAP];
        for (i = 0; i < prev; i++) {
            sync_npc_node *node;
            if (oldv[i].op == MV_REMOVE) {
                continue;
            }
            node = npc_slot(v, oldv[i].index);
            if (!node) {
                return RS_ERR_FORMAT;
            }
            oldv[i].type = node->npc.type;
            new_ids[np++] = oldv[i].index;
            consume_npc(node, &oldv[i]);
        }
        for (i = 0; i < addn; i++) {
            sync_npc_node *node = npc_slot(v, addv[i].index);
            if (!node) {
                return RS_ERR_FORMAT;
            }
            addv[i].type = node->npc.type;
            new_ids[np++] = addv[i].index;
            consume_npc(node, &addv[i]);
        }
        for (i = 0; i < prev; i++) {
            sync_npc_node *node;
            if (oldv[i].op != MV_REMOVE || mv_live(addv, addn, oldv[i].index)) {
                continue;
            }
            node = npc_slot(v, oldv[i].index);
            if (node) {
                deactivate_npc(node);
            }
        }
        for (i = 0; i < np; i++) {
            v->nids[i] = new_ids[i];
        }
        v->nn = np;
    }
    return RS_OK;
}

rs_err rs_sync_player_decode(rs_sync_view *v, rs_buf *src) {
    sync_mv local;
    sync_mv oldv[LIST_CAP];
    sync_mv addv[LIST_CAP];
    uint8_t keep[2048];
    int new_ids[LIST_CAP];
    int count, addn, i, np, op, d1, d2, upd, tx, tz, tplane, jump;
    int lx, lz, lp;
    rs_sync_player *self;
    rs_err e;

    e = sync_use(v);
    if (e != RS_OK) {
        return e;
    }
    if (!src || !src->data || v->local < 0) {
        return RS_ERR_ARG;
    }
    self = &v->players[v->local];
    memset(&local, 0, sizeof(local));
    tx = tz = tplane = jump = 0;
    rs_access_bits(src);
    SYNC_TRY(read_local_bits(src, v->kind, &op, &d1, &d2, &upd, &tx, &tz, &tplane, &jump));
    (void)jump;
    local.op = op;
    local.upd = upd;
    local.x = self->x;
    local.z = self->z;
    local.plane = self->plane;
    local.index = v->local;
    if (op == MV_TELE) {
        local.x = tx;
        local.z = tz;
        local.plane = tplane;
    } else if (op == MV_WALK) {
        apply_step(&local.x, &local.z, d1);
    } else if (op == MV_RUN) {
        apply_step(&local.x, &local.z, d1);
        apply_step(&local.x, &local.z, d2);
    }
    lx = local.x;
    lz = local.z;
    lp = local.plane;

    SYNC_TRY(gbits(src, 8, &count));
    if (count > v->pn) {
        return RS_ERR_FORMAT;
    }
    for (i = 0; i < count; i++) {
        int id = v->pids[i];
        sync_mv *m = &oldv[i];
        memset(m, 0, sizeof(*m));
        if (id < 0 || id > 2047 || id == v->local) {
            return RS_ERR_FORMAT;
        }
        SYNC_TRY(read_existing_one(src, &op, &d1, &d2, &upd));
        m->index = id;
        m->op = op;
        m->upd = upd;
        m->x = v->players[id].x;
        m->z = v->players[id].z;
        m->plane = lp;
        if (op == MV_WALK) {
            apply_step(&m->x, &m->z, d1);
        } else if (op == MV_RUN) {
            apply_step(&m->x, &m->z, d1);
            apply_step(&m->x, &m->z, d2);
        }
    }
    for (i = count; i < v->pn; i++) {
        memset(&oldv[i], 0, sizeof(oldv[i]));
        oldv[i].index = v->pids[i];
        oldv[i].op = MV_REMOVE;
    }

    addn = 0;
    while (src->bit_pos + 10 < src->cap * 8) {
        int index, dx, dz, aupd, ajump;
        sync_mv *m;
        SYNC_TRY(gbits(src, 11, &index));
        if (index == 2047) {
            break;
        }
        if (index == v->local || mv_live(oldv, v->pn, index) || mv_live(addv, addn, index)) {
            return RS_ERR_FORMAT;
        }
        if (addn >= LIST_CAP) {
            return RS_ERR_RANGE;
        }
        aupd = ajump = 0;
        SYNC_TRY(read_new_player_bits(src, v->kind, &aupd, &ajump, &dx, &dz));
        m = &addv[addn];
        memset(m, 0, sizeof(*m));
        m->index = index;
        m->op = MV_ADD;
        m->upd = aupd;
        m->x = lx + dx;
        m->z = lz + dz;
        m->plane = lp;
        addn++;
    }
    {
        int live = 0;
        for (i = 0; i < v->pn; i++) {
            if (oldv[i].op != MV_REMOVE) {
                live++;
            }
        }
        if (live + addn > LIST_CAP) {
            return RS_ERR_RANGE;
        }
    }
    rs_access_bytes(src);

    if (local.upd) {
        int mask;
        SYNC_TRY(get_mask_hdr(src, player_signal(v->kind), player_known(v->kind), &mask));
        SYNC_TRY(player_mask_read(v->kind, src, mask, self));
    }
    for (i = 0; i < v->pn; i++) {
        int mask;
        if (!oldv[i].upd) {
            continue;
        }
        SYNC_TRY(get_mask_hdr(src, player_signal(v->kind), player_known(v->kind), &mask));
        SYNC_TRY(player_mask_read(v->kind, src, mask, &v->players[oldv[i].index]));
    }
    for (i = 0; i < addn; i++) {
        int mask;
        if (!addv[i].upd) {
            continue;
        }
        SYNC_TRY(get_mask_hdr(src, player_signal(v->kind), player_known(v->kind), &mask));
        SYNC_TRY(player_mask_read(v->kind, src, mask, &v->players[addv[i].index]));
    }
    if (src->pos != src->cap) {
        return RS_ERR_FORMAT;
    }

    memset(keep, 0, sizeof(keep));
    np = 0;
    keep[v->local] = 1;
    for (i = 0; i < v->pn; i++) {
        if (oldv[i].op == MV_REMOVE) {
            continue;
        }
        new_ids[np++] = oldv[i].index;
        keep[oldv[i].index] = 1;
    }
    for (i = 0; i < addn; i++) {
        new_ids[np++] = addv[i].index;
        keep[addv[i].index] = 1;
    }
    for (i = 0; i < 2048; i++) {
        if (v->pin[i] && !keep[i]) {
            v->pin[i] = 0;
            if (i != v->local) {
                player_blank(&v->players[i]);
            }
        }
    }
    settle_player(v, v->local, local.x, local.z, local.plane, local.upd);
    for (i = 0; i < v->pn; i++) {
        if (oldv[i].op == MV_REMOVE) {
            continue;
        }
        settle_player(v, oldv[i].index, oldv[i].x, oldv[i].z, lp, oldv[i].upd);
    }
    for (i = 0; i < addn; i++) {
        settle_player(v, addv[i].index, addv[i].x, addv[i].z, lp, addv[i].upd);
    }
    for (i = 0; i < np; i++) {
        v->pids[i] = new_ids[i];
    }
    v->pn = np;
    return RS_OK;
}

rs_err rs_sync_npc_decode(rs_sync_view *v, rs_buf *src) {
    sync_mv oldv[LIST_CAP];
    sync_mv addv[LIST_CAP];
    int new_ids[LIST_CAP];
    int count, addn, i, np, op, d1, d2, upd, prev;
    int lx, lz, lp;
    rs_err e;

    e = sync_use(v);
    if (e != RS_OK) {
        return e;
    }
    if (!src || !src->data || v->local < 0 || !v->players[v->local].active) {
        return RS_ERR_ARG;
    }
    lx = v->players[v->local].x;
    lz = v->players[v->local].z;
    lp = v->players[v->local].plane;
    prev = v->nn;
    rs_access_bits(src);
    SYNC_TRY(gbits(src, 8, &count));
    if (count > prev) {
        return RS_ERR_FORMAT;
    }
    for (i = 0; i < count; i++) {
        int id = v->nids[i];
        sync_npc_node *node = npc_slot(v, id);
        sync_mv *m = &oldv[i];
        memset(m, 0, sizeof(*m));
        if (!node) {
            return RS_ERR_FORMAT;
        }
        SYNC_TRY(read_existing_one(src, &op, &d1, &d2, &upd));
        m->index = id;
        m->op = op;
        m->upd = upd;
        m->x = node->npc.x;
        m->z = node->npc.z;
        m->plane = lp;
        m->type = node->npc.type;
        if (op == MV_WALK) {
            apply_step(&m->x, &m->z, d1);
        } else if (op == MV_RUN) {
            apply_step(&m->x, &m->z, d1);
            apply_step(&m->x, &m->z, d2);
        }
    }
    for (i = count; i < prev; i++) {
        memset(&oldv[i], 0, sizeof(oldv[i]));
        oldv[i].index = v->nids[i];
        oldv[i].op = MV_REMOVE;
    }

    addn = 0;
    while (src->bit_pos + 21 < src->cap * 8) {
        int index, dx, dz, atype, aupd, ajump;
        sync_mv *m;
        SYNC_TRY(gbits(src, 14, &index));
        if (index == 16383) {
            break;
        }
        if (mv_live(oldv, prev, index) || mv_live(addv, addn, index)) {
            return RS_ERR_FORMAT;
        }
        if (addn >= LIST_CAP) {
            return RS_ERR_RANGE;
        }
        atype = aupd = ajump = 0;
        SYNC_TRY(read_new_npc_bits(src, v->kind, &aupd, &ajump, &dx, &dz, &atype));
        m = &addv[addn];
        memset(m, 0, sizeof(*m));
        m->index = index;
        m->op = MV_ADD;
        m->upd = aupd;
        m->type = atype;
        m->x = lx + dx;
        m->z = lz + dz;
        m->plane = lp;
        addn++;
    }
    {
        int live = 0;
        for (i = 0; i < prev; i++) {
            if (oldv[i].op != MV_REMOVE) {
                live++;
            }
        }
        if (live + addn > LIST_CAP) {
            return RS_ERR_RANGE;
        }
    }
    rs_access_bytes(src);

    for (i = 0; i < prev; i++) {
        sync_npc_node *node;
        int mask;
        if (!oldv[i].upd) {
            continue;
        }
        node = npc_slot(v, oldv[i].index);
        if (!node) {
            return RS_ERR_FORMAT;
        }
        SYNC_TRY(need(src, 1));
        mask = rs_g1(src);
        SYNC_TRY(npc_mask_read(v->kind, src, mask, &node->npc));
    }
    for (i = 0; i < addn; i++) {
        sync_npc_node *node;
        int mask;
        e = npc_ensure(v, addv[i].index, 1, &node);
        if (e != RS_OK) {
            return e;
        }
        node->npc.type = addv[i].type;
        if (!addv[i].upd) {
            continue;
        }
        SYNC_TRY(need(src, 1));
        mask = rs_g1(src);
        SYNC_TRY(npc_mask_read(v->kind, src, mask, &node->npc));
    }
    if (src->pos != src->cap) {
        return RS_ERR_FORMAT;
    }

    np = 0;
    for (i = 0; i < prev; i++) {
        if (oldv[i].op == MV_REMOVE) {
            continue;
        }
        new_ids[np++] = oldv[i].index;
    }
    for (i = 0; i < addn; i++) {
        new_ids[np++] = addv[i].index;
    }
    for (i = 0; i < prev; i++) {
        sync_npc_node *node;
        if (oldv[i].op != MV_REMOVE || mv_live(addv, addn, oldv[i].index)) {
            continue;
        }
        node = npc_slot(v, oldv[i].index);
        if (node) {
            deactivate_npc(node);
        }
    }
    for (i = 0; i < prev; i++) {
        sync_npc_node *node;
        if (oldv[i].op == MV_REMOVE) {
            continue;
        }
        node = npc_slot(v, oldv[i].index);
        if (!node) {
            return RS_ERR_FORMAT;
        }
        settle_npc(node, oldv[i].x, oldv[i].z, lp, oldv[i].upd);
    }
    for (i = 0; i < addn; i++) {
        sync_npc_node *node = npc_slot(v, addv[i].index);
        if (!node) {
            return RS_ERR_FORMAT;
        }
        settle_npc(node, addv[i].x, addv[i].z, lp, addv[i].upd);
    }
    for (i = 0; i < np; i++) {
        v->nids[i] = new_ids[i];
    }
    v->nn = np;
    return RS_OK;
}
