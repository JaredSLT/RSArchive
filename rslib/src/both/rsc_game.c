#include "internal.h"
#include "rslib/both/rsc_game.h"

/*
 * not implemented:
 * - Opcode 249 id 65535 is dead. The short is 0xFFFF, and MudClient.size treats a
 *   leading 0xFF as a 3-byte sector clear before it would read that short.
 * - Opcode 250 appearance, when entityArray has no player at that index, skips
 *   14 bytes plus a count byte plus count bytes (15+n). The stored record is
 *   2+8+1+n+6 = 17+n. The skip does not match. This file writes the stored record.
 * - Scenery facing comes from the landscape (world.call433 / call107), not the packet.
 * - Ground-item elevation is copied from a loc on the same tile, not the packet.
 * - NPC type >= ClientConfig.npcCount is replaced with 24. npcCount is not on the wire.
 * - Chat, private messages, and opcode 247 type 1 are WordPack bytes. call352
 *   (and WordFilter on player type 1) makes the string MudClient stores.
 * - Opcode 201 (PixMap.call378) rejects a run that crosses a row. The client
 *   writes those pixels past the row and keeps going.
 * - No jag/cache request opcode. Tools.read loads a file or URL, and Link.getjag
 *   returns bytes already in memory. A jag blob is rs_jag_*, not a game payload.
 * - 177 MudClient.loadingWait is a stub that returns false in this tree.
 * - 177 branches on ClientConfig.number92 are the obfuscator's other path.
 *   These codecs follow the path that matches 115 and 159.
 */

static rs_err rsc_revision(int revision) {
    if (revision == 115 || revision == 159 || revision == 177) {
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

static rs_err need_bits(const rs_buf *b, int n) {
    size_t end;
    if (!b || !b->data || n < 0) {
        return RS_ERR_ARG;
    }
    end = b->cap * 8;
    if (b->bit_pos > end || (size_t)n > end - b->bit_pos) {
        return RS_ERR_RANGE;
    }
    return RS_OK;
}

static rs_err get_bits(rs_buf *b, int n, int *out) {
    rs_err e = need_bits(b, n);
    if (e) {
        return e;
    }
    *out = rs_gbit(b, n);
    return RS_OK;
}

static rs_err rsc_pbit(rs_buf *b, int n, int v) {
    size_t off;
    size_t end;
    rs_err e;
    if (!b || n < 0) {
        return RS_ERR_ARG;
    }
    e = rs_buf_ensure(b, (size_t)((n + 7) / 8) + 2);
    if (e) {
        return e;
    }
    off = b->bit_pos >> 3;
    end = (b->bit_pos + (size_t)n + 7) >> 3;
    if ((b->bit_pos & 7) == 0) {
        if (end > off && end <= b->cap) {
            memset(b->data + off, 0, end - off);
        }
    } else if (end > off + 1 && end <= b->cap) {
        memset(b->data + off + 1, 0, end - (off + 1));
    }
    return rs_pbit(b, n, v);
}

static void bits_close(rs_buf *b) {
    unsigned used;
    rs_access_bytes(b);
    used = (unsigned)(b->bit_pos & 7u);
    if (used && b->pos > 0) {
        b->data[b->pos - 1] &= (uint8_t)(0xffu << (8u - used));
    }
}

static rs_err get_u8(rs_buf *b, int *out) {
    if (!b || rs_buf_remaining(b) < 1) {
        return RS_ERR_RANGE;
    }
    *out = rs_g1(b);
    return RS_OK;
}

static rs_err get_i8(rs_buf *b, int *out) {
    int v;
    rs_err e = get_u8(b, &v);
    if (e) {
        return e;
    }
    *out = v >= 128 ? v - 256 : v;
    return RS_OK;
}

static rs_err put_i8(rs_buf *b, int v) {
    if (v < -128 || v > 127) {
        return RS_ERR_RANGE;
    }
    return rs_p1(b, v & 0xff);
}

static rs_err get_u16(rs_buf *b, int *out) {
    if (!b || rs_buf_remaining(b) < 2) {
        return RS_ERR_RANGE;
    }
    *out = rs_g2(b);
    return RS_OK;
}

static rs_err put_u16_range(rs_buf *b, int v) {
    if (v < 0 || v > 65535) {
        return RS_ERR_RANGE;
    }
    return rs_p2(b, v);
}

static rs_err get_gsmart(rs_buf *b, int *out) {
    unsigned b0;
    uint32_t raw;
    int v;
    if (!b || rs_buf_remaining(b) < 1) {
        return RS_ERR_RANGE;
    }
    b0 = b->data[b->pos] & 0xffu;
    if (b0 < 128) {
        *out = rs_g1(b);
        return RS_OK;
    }
    if (rs_buf_remaining(b) < 4) {
        return RS_ERR_RANGE;
    }
    raw = (uint32_t)rs_g4(b);
    v = (int)(raw & 0x7fffffffu);
    /* MudClient.size advances 4 only when the value is >= 128, else 1. */
    if (v < 128) {
        b->pos -= 3;
    }
    *out = v;
    return RS_OK;
}

static rs_err put_gsmart(rs_buf *b, int v) {
    if (v < 0) {
        return RS_ERR_RANGE;
    }
    if (v < 128) {
        return rs_p1(b, v);
    }
    return rs_p4(b, (int)((uint32_t)v | 0x80000000u));
}

static rs_err put_signed_bits(rs_buf *b, int bits, int v) {
    int min = -(1 << (bits - 1));
    int max = (1 << (bits - 1)) - 1;
    if (v < min || v > max) {
        return RS_ERR_RANGE;
    }
    return rsc_pbit(b, bits, v & ((1 << bits) - 1));
}

static rs_err get_step(rs_buf *b, rs_rsc_step *s) {
    int bit;
    int bit2;
    int top;
    int low;
    rs_err e;
    memset(s, 0, sizeof(*s));
    e = get_bits(b, 1, &bit);
    if (e) {
        return e;
    }
    if (bit == 0) {
        s->kind = RS_RSC_STEP_KEEP;
        return RS_OK;
    }
    e = get_bits(b, 1, &bit2);
    if (e) {
        return e;
    }
    if (bit2 == 0) {
        e = get_bits(b, 3, &s->dir);
        if (e) {
            return e;
        }
        s->kind = RS_RSC_STEP_WALK;
        return RS_OK;
    }
    e = get_bits(b, 2, &top);
    if (e) {
        return e;
    }
    if (top == 3) {
        s->kind = RS_RSC_STEP_REMOVE;
        return RS_OK;
    }
    e = get_bits(b, 2, &low);
    if (e) {
        return e;
    }
    s->kind = RS_RSC_STEP_FACE;
    s->dir = (top << 2) | low;
    return RS_OK;
}

static rs_err put_step(rs_buf *b, const rs_rsc_step *s) {
    rs_err e;
    if (s->kind == RS_RSC_STEP_KEEP) {
        return rsc_pbit(b, 1, 0);
    }
    e = rsc_pbit(b, 1, 1);
    if (e) {
        return e;
    }
    if (s->kind == RS_RSC_STEP_WALK) {
        if (s->dir < 0 || s->dir > 7) {
            return RS_ERR_RANGE;
        }
        e = rsc_pbit(b, 1, 0);
        if (e) {
            return e;
        }
        return rsc_pbit(b, 3, s->dir);
    }
    if (s->kind == RS_RSC_STEP_REMOVE) {
        e = rsc_pbit(b, 1, 1);
        if (e) {
            return e;
        }
        return rsc_pbit(b, 2, 3);
    }
    if (s->kind == RS_RSC_STEP_FACE) {
        if (s->dir < 0 || s->dir > 11) {
            return RS_ERR_RANGE;
        }
        e = rsc_pbit(b, 1, 1);
        if (e) {
            return e;
        }
        return rsc_pbit(b, 4, s->dir);
    }
    return RS_ERR_FORMAT;
}

static int region_bits(int revision, int *xbits, int *ybits) {
    if (revision == 115) {
        *xbits = 10;
        *ybits = 12;
        return 0;
    }
    if (revision == 159 || revision == 177) {
        *xbits = 11;
        *ybits = 13;
        return 0;
    }
    return -1;
}

static int npc_layout(int revision, int *index_bits, int *type_bits, int *thresh) {
    if (revision == 115) {
        *index_bits = 11;
        *type_bits = 8;
        *thresh = 31;
        return 0;
    }
    if (revision == 159 || revision == 177) {
        *index_bits = 12;
        *type_bits = 10;
        *thresh = 34;
        return 0;
    }
    return -1;
}

rs_err rs_rsc_message_decode(int revision, rs_buf *src, char *dst, size_t cap) {
    size_t n;
    size_t i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !dst || cap == 0) {
        return RS_ERR_ARG;
    }
    n = rs_buf_remaining(src);
    if (n + 1 > cap) {
        return RS_ERR_RANGE;
    }
    if (n) {
        e = rs_gdata(src, dst, n);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < n; i++) {
        if (dst[i] == 0) {
            return RS_ERR_FORMAT;
        }
    }
    dst[n] = 0;
    return RS_OK;
}

rs_err rs_rsc_message_encode(int revision, rs_buf *dst, const char *text) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !text) {
        return RS_ERR_ARG;
    }
    /* Client.beenAddedFriendsList uses new String(in, 1, len-1): no 0x0a. */
    return rs_pdata(dst, text, strlen(text));
}

rs_err rs_rsc_self_decode(int revision, rs_buf *src, rs_rsc_self *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_u16(src, &out->index);
    if (e) {
        return e;
    }
    e = get_u16(src, &out->area_x);
    if (e) {
        return e;
    }
    e = get_u16(src, &out->area_y);
    if (e) {
        return e;
    }
    e = get_u16(src, &out->mul_a);
    if (e) {
        return e;
    }
    return get_u16(src, &out->mul_b);
}

rs_err rs_rsc_self_encode(int revision, rs_buf *dst, const rs_rsc_self *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    e = put_u16_range(dst, in->index);
    if (e) {
        return e;
    }
    e = put_u16_range(dst, in->area_x);
    if (e) {
        return e;
    }
    e = put_u16_range(dst, in->area_y);
    if (e) {
        return e;
    }
    e = put_u16_range(dst, in->mul_a);
    if (e) {
        return e;
    }
    return put_u16_range(dst, in->mul_b);
}

static rs_err read_added_player(rs_buf *b, rs_rsc_added_player *p) {
    int dx;
    int dy;
    rs_err e;
    memset(p, 0, sizeof(*p));
    e = get_bits(b, 11, &p->index);
    if (e) {
        return e;
    }
    e = get_bits(b, 5, &dx);
    if (e) {
        return e;
    }
    e = get_bits(b, 5, &dy);
    if (e) {
        return e;
    }
    p->dx = dx > 15 ? dx - 32 : dx;
    p->dy = dy > 15 ? dy - 32 : dy;
    e = get_bits(b, 4, &p->dir);
    if (e) {
        return e;
    }
    return get_bits(b, 1, &p->known);
}

rs_err rs_rsc_position_decode(int revision, rs_buf *src, rs_rsc_position *out) {
    int xbits;
    int ybits;
    int i;
    size_t lim;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out || region_bits(revision, &xbits, &ybits)) {
        return !src || !out ? RS_ERR_ARG : RS_ERR_UNSUPPORTED;
    }
    memset(out, 0, sizeof(*out));
    rs_access_bits(src);
    e = get_bits(src, xbits, &out->region_x);
    if (e) {
        return e;
    }
    e = get_bits(src, ybits, &out->region_y);
    if (e) {
        return e;
    }
    e = get_bits(src, 4, &out->dir);
    if (e) {
        return e;
    }
    e = get_bits(src, 8, &out->known_count);
    if (e) {
        return e;
    }
    for (i = 0; i < out->known_count; i++) {
        e = get_step(src, &out->known[i]);
        if (e) {
            return e;
        }
    }
    lim = src->cap * 8;
    /* MudClient.size: while (bit + 24 < length * 8), then a 26-bit record. */
    while (lim > src->bit_pos + 24) {
        if (out->added_count >= RS_RSC_ADDED_MAX) {
            return RS_ERR_RANGE;
        }
        e = read_added_player(src, &out->added[out->added_count]);
        if (e) {
            return e;
        }
        out->added_count++;
    }
    src->pos = src->cap;
    return RS_OK;
}

rs_err rs_rsc_position_encode(int revision, rs_buf *dst, const rs_rsc_position *in) {
    int xbits;
    int ybits;
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in || region_bits(revision, &xbits, &ybits)) {
        return !dst || !in ? RS_ERR_ARG : RS_ERR_UNSUPPORTED;
    }
    if (in->region_x < 0 || in->region_x >= (1 << xbits) || in->region_y < 0 ||
        in->region_y >= (1 << ybits) || in->dir < 0 || in->dir > 15) {
        return RS_ERR_RANGE;
    }
    if (in->known_count < 0 || in->known_count > RS_RSC_KNOWN_MAX || in->added_count < 0 ||
        in->added_count > RS_RSC_ADDED_MAX) {
        return RS_ERR_RANGE;
    }
    rs_access_bits(dst);
    e = rsc_pbit(dst, xbits, in->region_x);
    if (e) {
        return e;
    }
    e = rsc_pbit(dst, ybits, in->region_y);
    if (e) {
        return e;
    }
    e = rsc_pbit(dst, 4, in->dir);
    if (e) {
        return e;
    }
    e = rsc_pbit(dst, 8, in->known_count);
    if (e) {
        return e;
    }
    for (i = 0; i < in->known_count; i++) {
        e = put_step(dst, &in->known[i]);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < in->added_count; i++) {
        const rs_rsc_added_player *p = &in->added[i];
        if (p->index < 0 || p->index > 2047 || p->dir < 0 || p->dir > 15 || p->known < 0 ||
            p->known > 1) {
            return RS_ERR_RANGE;
        }
        e = rsc_pbit(dst, 11, p->index);
        if (e) {
            return e;
        }
        e = put_signed_bits(dst, 5, p->dx);
        if (e) {
            return e;
        }
        e = put_signed_bits(dst, 5, p->dy);
        if (e) {
            return e;
        }
        e = rsc_pbit(dst, 4, p->dir);
        if (e) {
            return e;
        }
        e = rsc_pbit(dst, 1, p->known);
        if (e) {
            return e;
        }
    }
    bits_close(dst);
    return RS_OK;
}

static rs_err read_aux(rs_buf *b, rs_rsc_player_aux *row) {
    int n;
    int i;
    rs_err e;
    memset(row, 0, sizeof(*row));
    e = get_u16(b, &row->index);
    if (e) {
        return e;
    }
    e = get_u8(b, &row->type);
    if (e) {
        return e;
    }
    if (row->type == RS_RSC_AUX_HITS) {
        return get_u16(b, &row->a);
    }
    if (row->type == RS_RSC_AUX_CHAT || row->type == RS_RSC_AUX_CHAT_OWN) {
        e = get_u8(b, &n);
        if (e) {
            return e;
        }
        if (n > RS_RSC_CHAT_MAX) {
            return RS_ERR_FORMAT;
        }
        if (n && rs_gdata(b, row->chat, (size_t)n) != RS_OK) {
            return RS_ERR_RANGE;
        }
        row->chat_len = n;
        return RS_OK;
    }
    if (row->type == RS_RSC_AUX_COMBAT) {
        e = get_u8(b, &row->a);
        if (e) {
            return e;
        }
        e = get_u8(b, &row->b);
        if (e) {
            return e;
        }
        return get_u8(b, &row->c);
    }
    if (row->type == RS_RSC_AUX_PROJ_NPC || row->type == RS_RSC_AUX_PROJ_PLAYER) {
        e = get_u16(b, &row->a);
        if (e) {
            return e;
        }
        return get_u16(b, &row->b);
    }
    if (row->type != RS_RSC_AUX_APPEARANCE) {
        return RS_ERR_FORMAT;
    }
    e = get_u16(b, &row->appearance_id);
    if (e) {
        return e;
    }
    if (rs_buf_remaining(b) < 8) {
        return RS_ERR_RANGE;
    }
    row->name37 = rs_g8(b);
    rs_base37_decode(row->name37, row->name);
    e = get_u8(b, &row->nsprites);
    if (e) {
        return e;
    }
    if (row->nsprites > RS_RSC_SPRITES) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < row->nsprites; i++) {
        int sprite;
        e = get_u8(b, &sprite);
        if (e) {
            return e;
        }
        row->sprites[i] = (uint8_t)sprite;
    }
    e = get_u8(b, &row->hair_colour);
    if (e) {
        return e;
    }
    e = get_u8(b, &row->top_colour);
    if (e) {
        return e;
    }
    e = get_u8(b, &row->leg_colour);
    if (e) {
        return e;
    }
    e = get_u8(b, &row->skin_colour);
    if (e) {
        return e;
    }
    e = get_u8(b, &row->combat_level);
    if (e) {
        return e;
    }
    return get_u8(b, &row->skull);
}

static rs_err write_colour(rs_buf *b, int v) {
    if (v < 0 || v > 255) {
        return RS_ERR_RANGE;
    }
    return rs_p1(b, v);
}

static rs_err write_aux(rs_buf *b, const rs_rsc_player_aux *row) {
    int i;
    rs_err e;
    uint64_t name;
    e = put_u16_range(b, row->index);
    if (e) {
        return e;
    }
    if (row->type < 0 || row->type > 255) {
        return RS_ERR_RANGE;
    }
    e = rs_p1(b, row->type);
    if (e) {
        return e;
    }
    if (row->type == RS_RSC_AUX_HITS) {
        return put_u16_range(b, row->a);
    }
    if (row->type == RS_RSC_AUX_CHAT || row->type == RS_RSC_AUX_CHAT_OWN) {
        if (row->chat_len < 0 || row->chat_len > RS_RSC_CHAT_MAX) {
            return RS_ERR_RANGE;
        }
        e = rs_p1(b, row->chat_len);
        if (e) {
            return e;
        }
        if (row->chat_len) {
            return rs_pdata(b, row->chat, (size_t)row->chat_len);
        }
        return RS_OK;
    }
    if (row->type == RS_RSC_AUX_COMBAT) {
        e = write_colour(b, row->a);
        if (e) {
            return e;
        }
        e = write_colour(b, row->b);
        if (e) {
            return e;
        }
        return write_colour(b, row->c);
    }
    if (row->type == RS_RSC_AUX_PROJ_NPC || row->type == RS_RSC_AUX_PROJ_PLAYER) {
        e = put_u16_range(b, row->a);
        if (e) {
            return e;
        }
        return put_u16_range(b, row->b);
    }
    if (row->type != RS_RSC_AUX_APPEARANCE) {
        return RS_ERR_FORMAT;
    }
    e = put_u16_range(b, row->appearance_id);
    if (e) {
        return e;
    }
    name = row->name37 ? row->name37 : rs_base37_encode(row->name);
    e = rs_p8(b, name);
    if (e) {
        return e;
    }
    if (row->nsprites < 0 || row->nsprites > RS_RSC_SPRITES) {
        return RS_ERR_RANGE;
    }
    e = rs_p1(b, row->nsprites);
    if (e) {
        return e;
    }
    for (i = 0; i < row->nsprites; i++) {
        e = rs_p1(b, row->sprites[i]);
        if (e) {
            return e;
        }
    }
    e = write_colour(b, row->hair_colour);
    if (e) {
        return e;
    }
    e = write_colour(b, row->top_colour);
    if (e) {
        return e;
    }
    e = write_colour(b, row->leg_colour);
    if (e) {
        return e;
    }
    e = write_colour(b, row->skin_colour);
    if (e) {
        return e;
    }
    e = write_colour(b, row->combat_level);
    if (e) {
        return e;
    }
    return write_colour(b, row->skull);
}

rs_err rs_rsc_player_aux_decode(int revision, rs_buf *src, rs_rsc_player_aux_list *out) {
    int count;
    int i;
    size_t start;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out || out->cap < 0) {
        return RS_ERR_ARG;
    }
    start = src->pos;
    e = get_u16(src, &count);
    if (e) {
        return e;
    }
    if (count > out->cap) {
        src->pos = start;
        return RS_ERR_RANGE;
    }
    if (count && !out->row) {
        src->pos = start;
        return RS_ERR_ARG;
    }
    for (i = 0; i < count; i++) {
        e = read_aux(src, &out->row[i]);
        if (e) {
            return e;
        }
    }
    out->count = count;
    return RS_OK;
}

rs_err rs_rsc_player_aux_encode(int revision, rs_buf *dst, const rs_rsc_player_aux_list *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in || in->count < 0 || (in->count && !in->row)) {
        return RS_ERR_ARG;
    }
    if (in->count > 65535) {
        return RS_ERR_RANGE;
    }
    e = rs_p2(dst, in->count);
    if (e) {
        return e;
    }
    for (i = 0; i < in->count; i++) {
        e = write_aux(dst, &in->row[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

static rs_err read_item(rs_buf *b, rs_rsc_inv_item *item, rs_rsc_has_amount_fn has_amount, void *user) {
    int word;
    rs_err e;
    memset(item, 0, sizeof(*item));
    e = get_u16(b, &word);
    if (e) {
        return e;
    }
    item->id = word & 32767;
    item->equipped = (word & 0x8000) ? 1 : 0;
    if (!has_amount) {
        return RS_ERR_ARG;
    }
    item->has_amount = has_amount(item->id, user) ? 1 : 0;
    if (!item->has_amount) {
        item->amount = 1;
        return RS_OK;
    }
    return get_gsmart(b, &item->amount);
}

static rs_err write_item(rs_buf *b, const rs_rsc_inv_item *item) {
    int word;
    rs_err e;
    if (!item || item->id < 0 || item->id > 32767 || (item->equipped != 0 && item->equipped != 1)) {
        return RS_ERR_RANGE;
    }
    word = item->id;
    if (item->equipped) {
        word |= 0x8000;
    }
    e = rs_p2(b, word);
    if (e) {
        return e;
    }
    if (!item->has_amount) {
        return RS_OK;
    }
    return put_gsmart(b, item->amount);
}

rs_err rs_rsc_inventory_decode(int revision, rs_buf *src, rs_rsc_inventory *out,
                               rs_rsc_has_amount_fn has_amount, void *user) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_u8(src, &out->count);
    if (e) {
        return e;
    }
    if (out->count > RS_RSC_INV_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < out->count; i++) {
        e = read_item(src, &out->item[i], has_amount, user);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_inventory_encode(int revision, rs_buf *dst, const rs_rsc_inventory *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in || in->count < 0 || in->count > RS_RSC_INV_MAX) {
        return !dst || !in ? RS_ERR_ARG : RS_ERR_RANGE;
    }
    e = rs_p1(dst, in->count);
    if (e) {
        return e;
    }
    for (i = 0; i < in->count; i++) {
        e = write_item(dst, &in->item[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_inv_slot_decode(int revision, rs_buf *src, int *slot, rs_rsc_inv_item *out,
                              rs_rsc_has_amount_fn has_amount, void *user) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !slot || !out) {
        return RS_ERR_ARG;
    }
    e = get_u8(src, slot);
    if (e) {
        return e;
    }
    return read_item(src, out, has_amount, user);
}

rs_err rs_rsc_inv_slot_encode(int revision, rs_buf *dst, int slot, const rs_rsc_inv_item *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (slot < 0 || slot > 255) {
        return RS_ERR_RANGE;
    }
    e = rs_p1(dst, slot);
    if (e) {
        return e;
    }
    return write_item(dst, in);
}

rs_err rs_rsc_inv_remove_decode(int revision, rs_buf *src, int *slot) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !slot) {
        return RS_ERR_ARG;
    }
    return get_u8(src, slot);
}

rs_err rs_rsc_inv_remove_encode(int revision, rs_buf *dst, int slot) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst) {
        return RS_ERR_ARG;
    }
    if (slot < 0 || slot > 255) {
        return RS_ERR_RANGE;
    }
    return rs_p1(dst, slot);
}

static rs_err read_added_npc(rs_buf *b, int index_bits, int type_bits, rs_rsc_added_npc *n) {
    int dx;
    int dy;
    rs_err e;
    memset(n, 0, sizeof(*n));
    e = get_bits(b, index_bits, &n->index);
    if (e) {
        return e;
    }
    e = get_bits(b, 5, &dx);
    if (e) {
        return e;
    }
    e = get_bits(b, 5, &dy);
    if (e) {
        return e;
    }
    n->dx = dx > 15 ? dx - 32 : dx;
    n->dy = dy > 15 ? dy - 32 : dy;
    e = get_bits(b, 4, &n->dir);
    if (e) {
        return e;
    }
    return get_bits(b, type_bits, &n->type);
}

rs_err rs_rsc_npcs_decode(int revision, rs_buf *src, rs_rsc_npcs *out) {
    int index_bits;
    int type_bits;
    int thresh;
    int i;
    size_t lim;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out || npc_layout(revision, &index_bits, &type_bits, &thresh)) {
        return !src || !out ? RS_ERR_ARG : RS_ERR_UNSUPPORTED;
    }
    memset(out, 0, sizeof(*out));
    rs_access_bits(src);
    e = get_bits(src, 8, &out->known_count);
    if (e) {
        return e;
    }
    for (i = 0; i < out->known_count; i++) {
        e = get_step(src, &out->known[i]);
        if (e) {
            return e;
        }
    }
    lim = src->cap * 8;
    while (lim > src->bit_pos + (size_t)thresh) {
        if (out->added_count >= RS_RSC_NPC_ADDED_MAX) {
            return RS_ERR_RANGE;
        }
        e = read_added_npc(src, index_bits, type_bits, &out->added[out->added_count]);
        if (e) {
            return e;
        }
        out->added_count++;
    }
    src->pos = src->cap;
    return RS_OK;
}

rs_err rs_rsc_npcs_encode(int revision, rs_buf *dst, const rs_rsc_npcs *in) {
    int index_bits;
    int type_bits;
    int thresh;
    int i;
    int index_max;
    int type_max;
    rs_err e = rsc_revision(revision);
    (void)thresh;
    if (e) {
        return e;
    }
    if (!dst || !in || npc_layout(revision, &index_bits, &type_bits, &thresh)) {
        return !dst || !in ? RS_ERR_ARG : RS_ERR_UNSUPPORTED;
    }
    if (in->known_count < 0 || in->known_count > RS_RSC_KNOWN_MAX || in->added_count < 0 ||
        in->added_count > RS_RSC_NPC_ADDED_MAX) {
        return RS_ERR_RANGE;
    }
    index_max = (1 << index_bits) - 1;
    type_max = (1 << type_bits) - 1;
    rs_access_bits(dst);
    e = rsc_pbit(dst, 8, in->known_count);
    if (e) {
        return e;
    }
    for (i = 0; i < in->known_count; i++) {
        e = put_step(dst, &in->known[i]);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < in->added_count; i++) {
        const rs_rsc_added_npc *n = &in->added[i];
        if (n->index < 0 || n->index > index_max || n->type < 0 || n->type > type_max || n->dir < 0 ||
            n->dir > 15) {
            return RS_ERR_RANGE;
        }
        e = rsc_pbit(dst, index_bits, n->index);
        if (e) {
            return e;
        }
        e = put_signed_bits(dst, 5, n->dx);
        if (e) {
            return e;
        }
        e = put_signed_bits(dst, 5, n->dy);
        if (e) {
            return e;
        }
        e = rsc_pbit(dst, 4, n->dir);
        if (e) {
            return e;
        }
        e = rsc_pbit(dst, type_bits, n->type);
        if (e) {
            return e;
        }
    }
    bits_close(dst);
    return RS_OK;
}

static rs_err loc_push(rs_rsc_locs *out, int kind, int id, int x, int y, int dir) {
    rs_rsc_loc *loc;
    if (out->count >= RS_RSC_LOC_MAX) {
        return RS_ERR_RANGE;
    }
    loc = &out->loc[out->count++];
    loc->kind = kind;
    loc->id = id;
    loc->x = x;
    loc->y = y;
    loc->dir = dir;
    return RS_OK;
}

static rs_err read_clear_or(rs_buf *src, int *marked) {
    int b0;
    if (rs_buf_remaining(src) < 1) {
        return RS_ERR_RANGE;
    }
    b0 = src->data[src->pos] & 0xff;
    *marked = b0 == 255;
    return RS_OK;
}

rs_err rs_rsc_ground_items_decode(int revision, rs_buf *src, rs_rsc_locs *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    while (rs_buf_remaining(src) > 0) {
        int marked;
        int word;
        int x;
        int y;
        e = read_clear_or(src, &marked);
        if (e) {
            return e;
        }
        if (marked) {
            if (rs_buf_remaining(src) < 3) {
                return RS_ERR_RANGE;
            }
            src->pos++;
            e = get_i8(src, &x);
            if (e) {
                return e;
            }
            e = get_i8(src, &y);
            if (e) {
                return e;
            }
            e = loc_push(out, RS_RSC_LOC_CLEAR, 0, x, y, 0);
            if (e) {
                return e;
            }
            continue;
        }
        e = get_u16(src, &word);
        if (e) {
            return e;
        }
        e = get_i8(src, &x);
        if (e) {
            return e;
        }
        e = get_i8(src, &y);
        if (e) {
            return e;
        }
        if (word & 0x8000) {
            e = loc_push(out, RS_RSC_LOC_REMOVE, word & 32767, x, y, 0);
        } else {
            e = loc_push(out, RS_RSC_LOC_ADD, word, x, y, 0);
        }
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_ground_items_encode(int revision, rs_buf *dst, const rs_rsc_locs *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in || in->count < 0 || in->count > RS_RSC_LOC_MAX) {
        return !dst || !in ? RS_ERR_ARG : RS_ERR_RANGE;
    }
    for (i = 0; i < in->count; i++) {
        const rs_rsc_loc *loc = &in->loc[i];
        if (loc->kind == RS_RSC_LOC_CLEAR) {
            e = rs_p1(dst, 255);
            if (e) {
                return e;
            }
            e = put_i8(dst, loc->x);
            if (e) {
                return e;
            }
            e = put_i8(dst, loc->y);
            if (e) {
                return e;
            }
            continue;
        }
        if (loc->kind == RS_RSC_LOC_ADD) {
            if (loc->id < 0 || loc->id > 32767) {
                return RS_ERR_RANGE;
            }
            e = rs_p2(dst, loc->id);
        } else if (loc->kind == RS_RSC_LOC_REMOVE) {
            /* id|0x8000 starts with 0xFF once id >= 32512, which is a clear. */
            if (loc->id < 0 || loc->id >= 32512) {
                return RS_ERR_RANGE;
            }
            e = rs_p2(dst, loc->id | 0x8000);
        } else {
            return RS_ERR_FORMAT;
        }
        if (e) {
            return e;
        }
        e = put_i8(dst, loc->x);
        if (e) {
            return e;
        }
        e = put_i8(dst, loc->y);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_objects_decode(int revision, rs_buf *src, rs_rsc_locs *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    while (rs_buf_remaining(src) > 0) {
        int marked;
        int id;
        int x;
        int y;
        e = read_clear_or(src, &marked);
        if (e) {
            return e;
        }
        if (marked) {
            if (rs_buf_remaining(src) < 3) {
                return RS_ERR_RANGE;
            }
            src->pos++;
            e = get_i8(src, &x);
            if (e) {
                return e;
            }
            e = get_i8(src, &y);
            if (e) {
                return e;
            }
            e = loc_push(out, RS_RSC_LOC_CLEAR, 0, x, y, 0);
            if (e) {
                return e;
            }
            continue;
        }
        e = get_u16(src, &id);
        if (e) {
            return e;
        }
        e = get_i8(src, &x);
        if (e) {
            return e;
        }
        e = get_i8(src, &y);
        if (e) {
            return e;
        }
        if (id == 60000) {
            e = loc_push(out, RS_RSC_LOC_REMOVE, 60000, x, y, 0);
        } else {
            e = loc_push(out, RS_RSC_LOC_ADD, id, x, y, 0);
        }
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_objects_encode(int revision, rs_buf *dst, const rs_rsc_locs *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in || in->count < 0 || in->count > RS_RSC_LOC_MAX) {
        return !dst || !in ? RS_ERR_ARG : RS_ERR_RANGE;
    }
    for (i = 0; i < in->count; i++) {
        const rs_rsc_loc *loc = &in->loc[i];
        int id;
        if (loc->kind == RS_RSC_LOC_CLEAR) {
            e = rs_p1(dst, 255);
            if (e) {
                return e;
            }
        } else if (loc->kind == RS_RSC_LOC_REMOVE) {
            e = rs_p2(dst, 60000);
        } else if (loc->kind == RS_RSC_LOC_ADD) {
            id = loc->id;
            if (id < 0 || id > 65535 || id == 60000 || (id >> 8) == 255) {
                return RS_ERR_RANGE;
            }
            e = rs_p2(dst, id);
        } else {
            return RS_ERR_FORMAT;
        }
        if (e) {
            return e;
        }
        e = put_i8(dst, loc->x);
        if (e) {
            return e;
        }
        e = put_i8(dst, loc->y);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_walls_decode(int revision, rs_buf *src, rs_rsc_locs *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    while (rs_buf_remaining(src) > 0) {
        int marked;
        int id;
        int x;
        int y;
        int dir;
        e = read_clear_or(src, &marked);
        if (e) {
            return e;
        }
        if (marked) {
            if (rs_buf_remaining(src) < 3) {
                return RS_ERR_RANGE;
            }
            src->pos++;
            e = get_i8(src, &x);
            if (e) {
                return e;
            }
            e = get_i8(src, &y);
            if (e) {
                return e;
            }
            e = loc_push(out, RS_RSC_LOC_CLEAR, 0, x, y, 0);
            if (e) {
                return e;
            }
            continue;
        }
        e = get_u16(src, &id);
        if (e) {
            return e;
        }
        e = get_i8(src, &x);
        if (e) {
            return e;
        }
        e = get_i8(src, &y);
        if (e) {
            return e;
        }
        e = get_i8(src, &dir);
        if (e) {
            return e;
        }
        e = loc_push(out, RS_RSC_LOC_ADD, id, x, y, dir);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_walls_encode(int revision, rs_buf *dst, const rs_rsc_locs *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in || in->count < 0 || in->count > RS_RSC_LOC_MAX) {
        return !dst || !in ? RS_ERR_ARG : RS_ERR_RANGE;
    }
    for (i = 0; i < in->count; i++) {
        const rs_rsc_loc *loc = &in->loc[i];
        if (loc->kind == RS_RSC_LOC_CLEAR) {
            e = rs_p1(dst, 255);
            if (e) {
                return e;
            }
            e = put_i8(dst, loc->x);
            if (e) {
                return e;
            }
            e = put_i8(dst, loc->y);
            if (e) {
                return e;
            }
            continue;
        }
        if (loc->kind != RS_RSC_LOC_ADD) {
            return RS_ERR_FORMAT;
        }
        if (loc->id < 0 || loc->id > 65535 || (loc->id >> 8) == 255) {
            return RS_ERR_RANGE;
        }
        e = rs_p2(dst, loc->id);
        if (e) {
            return e;
        }
        e = put_i8(dst, loc->x);
        if (e) {
            return e;
        }
        e = put_i8(dst, loc->y);
        if (e) {
            return e;
        }
        e = put_i8(dst, loc->dir);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_sectors_decode(int revision, rs_buf *src, rs_rsc_sectors *out) {
    int n;
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    n = (int)(rs_buf_remaining(src) / 4);
    if (n > RS_RSC_SECTOR_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < n; i++) {
        if (rs_buf_remaining(src) < 4) {
            return RS_ERR_RANGE;
        }
        out->sector[i].dx = rs_g2b(src);
        out->sector[i].dy = rs_g2b(src);
    }
    out->count = n;
    return RS_OK;
}

rs_err rs_rsc_sectors_encode(int revision, rs_buf *dst, const rs_rsc_sectors *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in || in->count < 0 || in->count > RS_RSC_SECTOR_MAX) {
        return !dst || !in ? RS_ERR_ARG : RS_ERR_RANGE;
    }
    for (i = 0; i < in->count; i++) {
        if (in->sector[i].dx < -32768 || in->sector[i].dx > 32767 || in->sector[i].dy < -32768 ||
            in->sector[i].dy > 32767) {
            return RS_ERR_RANGE;
        }
        e = rs_p2(dst, in->sector[i].dx & 0xffff);
        if (e) {
            return e;
        }
        e = rs_p2(dst, in->sector[i].dy & 0xffff);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

enum { RSC_115 = 1, RSC_159 = 2, RSC_177 = 4, RSC_ALL = 7, RSC_159_177 = 6 };

static rs_err rsc_revs(int revision, int mask) {
    if (revision == 115) {
        return (mask & RSC_115) ? RS_OK : RS_ERR_UNSUPPORTED;
    }
    if (revision == 159) {
        return (mask & RSC_159) ? RS_OK : RS_ERR_UNSUPPORTED;
    }
    if (revision == 177) {
        return (mask & RSC_177) ? RS_OK : RS_ERR_UNSUPPORTED;
    }
    return RS_ERR_UNSUPPORTED;
}

static rs_err empty_io(int revision, rs_buf *b, int mask) {
    rs_err e = rsc_revs(revision, mask);
    if (e) {
        return e;
    }
    if (!b) {
        return RS_ERR_ARG;
    }
    return RS_OK;
}

static int skill_n(int revision) {
    if (revision == 115) {
        return 16;
    }
    if (revision == 159 || revision == 177) {
        return 18;
    }
    return 0;
}

static int quest_n(int revision) {
    if (revision == 115) {
        return 17;
    }
    if (revision == 159) {
        return 39;
    }
    if (revision == 177) {
        return 50;
    }
    return 0;
}

static rs_err write_u8(rs_buf *b, int v) {
    if (v < 0 || v > 255) {
        return RS_ERR_RANGE;
    }
    return rs_p1(b, v);
}

static rs_err read_bool(rs_buf *b, int *out) {
    int v;
    rs_err e = get_u8(b, &v);
    if (e) {
        return e;
    }
    *out = v == 1;
    return RS_OK;
}

static rs_err write_bool(rs_buf *b, int v) {
    if (v != 0 && v != 1) {
        return RS_ERR_RANGE;
    }
    return rs_p1(b, v);
}

static rs_err get_u32(rs_buf *b, uint32_t *out) {
    if (!b || rs_buf_remaining(b) < 4) {
        return RS_ERR_RANGE;
    }
    *out = (uint32_t)rs_g4(b);
    return RS_OK;
}

static rs_err put_u32(rs_buf *b, uint32_t v) {
    return rs_p4(b, (int)v);
}

static rs_err get_name(rs_buf *b, uint64_t *out) {
    if (!b || rs_buf_remaining(b) < 8) {
        return RS_ERR_RANGE;
    }
    *out = rs_g8(b);
    return RS_OK;
}

static rs_err raw_text_decode(int revision, int mask, rs_buf *src, char *dst, size_t cap) {
    size_t n;
    size_t i;
    rs_err e = rsc_revs(revision, mask);
    if (e) {
        return e;
    }
    if (!src || !dst || cap == 0) {
        return RS_ERR_ARG;
    }
    n = rs_buf_remaining(src);
    if (n + 1 > cap) {
        return RS_ERR_RANGE;
    }
    if (n) {
        e = rs_gdata(src, dst, n);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < n; i++) {
        if (dst[i] == 0) {
            return RS_ERR_FORMAT;
        }
    }
    dst[n] = 0;
    return RS_OK;
}

static rs_err raw_text_encode(int revision, int mask, rs_buf *dst, const char *text) {
    rs_err e = rsc_revs(revision, mask);
    if (e) {
        return e;
    }
    if (!dst || !text) {
        return RS_ERR_ARG;
    }
    return rs_pdata(dst, text, strlen(text));
}

static rs_err read_short_rev(int revision, int mask, rs_buf *src, int *value) {
    rs_err e = rsc_revs(revision, mask);
    if (e) {
        return e;
    }
    if (!src || !value) {
        return RS_ERR_ARG;
    }
    return get_u16(src, value);
}

static rs_err write_short_rev(int revision, int mask, rs_buf *dst, int value) {
    rs_err e = rsc_revs(revision, mask);
    if (e) {
        return e;
    }
    if (!dst) {
        return RS_ERR_ARG;
    }
    return put_u16_range(dst, value);
}

static rs_err flag_decode(int revision, rs_buf *src, int *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    return read_bool(src, out);
}

static rs_err flag_encode(int revision, rs_buf *dst, int accepted) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst) {
        return RS_ERR_ARG;
    }
    return write_bool(dst, accepted);
}

static rs_err read_stacks(rs_buf *b, int n, rs_rsc_stack *item) {
    int i;
    rs_err e;
    for (i = 0; i < n; i++) {
        e = get_u16(b, &item[i].id);
        if (e) {
            return e;
        }
        e = get_u32(b, &item[i].amount);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

static rs_err write_stacks(rs_buf *b, int n, const rs_rsc_stack *item) {
    int i;
    rs_err e;
    for (i = 0; i < n; i++) {
        e = put_u16_range(b, item[i].id);
        if (e) {
            return e;
        }
        e = put_u32(b, item[i].amount);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

static rs_err read_count_stacks(rs_buf *b, int max, int *count, rs_rsc_stack *item) {
    int n;
    rs_err e = get_u8(b, &n);
    if (e) {
        return e;
    }
    if (n > max) {
        return RS_ERR_RANGE;
    }
    e = read_stacks(b, n, item);
    if (e) {
        return e;
    }
    *count = n;
    return RS_OK;
}

static rs_err write_count_stacks(rs_buf *b, int max, int count, const rs_rsc_stack *item) {
    rs_err e;
    if (count < 0 || count > max) {
        return RS_ERR_RANGE;
    }
    e = write_u8(b, count);
    if (e) {
        return e;
    }
    return write_stacks(b, count, item);
}

rs_err rs_rsc_close_socket_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_close_socket_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_logout_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_logout_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_sleep_wrong_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_177); }
rs_err rs_rsc_sleep_wrong_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_177); }
rs_err rs_rsc_ui_close_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_177); }
rs_err rs_rsc_ui_close_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_177); }
rs_err rs_rsc_sleep_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_sleep_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_trade_close_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_trade_close_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_shop_close_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_shop_close_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_bank_close_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_bank_close_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_duel_close_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_duel_close_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_timer_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_timer_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_strings_close_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_strings_close_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_recovery_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_ALL); }
rs_err rs_rsc_recovery_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_ALL); }
rs_err rs_rsc_contact_decode(int revision, rs_buf *src) { return empty_io(revision, src, RSC_159_177); }
rs_err rs_rsc_contact_encode(int revision, rs_buf *dst) { return empty_io(revision, dst, RSC_159_177); }

rs_err rs_rsc_sound_decode(int revision, rs_buf *src, char *dst, size_t cap) {
    return raw_text_decode(revision, RSC_159_177, src, dst, cap);
}

rs_err rs_rsc_sound_encode(int revision, rs_buf *dst, const char *text) {
    return raw_text_encode(revision, RSC_159_177, dst, text);
}

rs_err rs_rsc_ui_string_decode(int revision, rs_buf *src, char *dst, size_t cap) {
    return raw_text_decode(revision, RSC_159_177, src, dst, cap);
}

rs_err rs_rsc_ui_string_encode(int revision, rs_buf *dst, const char *text) {
    return raw_text_encode(revision, RSC_159_177, dst, text);
}

rs_err rs_rsc_ui_value_decode(int revision, rs_buf *src, int *value) {
    return read_short_rev(revision, RSC_177, src, value);
}

rs_err rs_rsc_ui_value_encode(int revision, rs_buf *dst, int value) {
    return write_short_rev(revision, RSC_177, dst, value);
}

rs_err rs_rsc_ui_value2_decode(int revision, rs_buf *src, int *value) {
    return read_short_rev(revision, RSC_159_177, src, value);
}

rs_err rs_rsc_ui_value2_encode(int revision, rs_buf *dst, int value) {
    return write_short_rev(revision, RSC_159_177, dst, value);
}

static rs_err sleep_bits_decode(rs_buf *src, rs_rsc_sleep_image *out) {
    int pos;
    int color;
    int n;
    int i;
    int row;
    int col;
    rs_err e;
    memset(out, 0, sizeof(*out));
    pos = 0;
    color = 0;
    while (pos < RS_RSC_SLEEP_W) {
        e = get_u8(src, &n);
        if (e) {
            return e;
        }
        if (pos + n > RS_RSC_SLEEP_W) {
            return RS_ERR_FORMAT;
        }
        for (i = 0; i < n; i++) {
            out->pixel[pos++] = (uint8_t)color;
        }
        color ^= 1;
    }
    for (row = 1; row < RS_RSC_SLEEP_H; row++) {
        col = 0;
        while (col < RS_RSC_SLEEP_W) {
            e = get_u8(src, &n);
            if (e) {
                return e;
            }
            if (col + n > RS_RSC_SLEEP_W) {
                return RS_ERR_FORMAT;
            }
            for (i = 0; i < n; i++) {
                out->pixel[row * RS_RSC_SLEEP_W + col] = out->pixel[(row - 1) * RS_RSC_SLEEP_W + col];
                col++;
            }
            if (col < RS_RSC_SLEEP_W) {
                uint8_t above = out->pixel[(row - 1) * RS_RSC_SLEEP_W + col];
                out->pixel[row * RS_RSC_SLEEP_W + col] = (uint8_t)(above ^ 1u);
                col++;
            }
        }
    }
    return RS_OK;
}

static rs_err sleep_bits_encode(rs_buf *dst, const rs_rsc_sleep_image *in) {
    int i;
    int pos;
    int color;
    int run;
    int row;
    int col;
    rs_err e;
    for (i = 0; i < RS_RSC_SLEEP_W * RS_RSC_SLEEP_H; i++) {
        if (in->pixel[i] > 1) {
            return RS_ERR_FORMAT;
        }
    }
    pos = 0;
    color = 0;
    while (pos < RS_RSC_SLEEP_W) {
        run = 0;
        while (pos + run < RS_RSC_SLEEP_W && in->pixel[pos + run] == (uint8_t)color && run < 255) {
            run++;
        }
        e = rs_p1(dst, run);
        if (e) {
            return e;
        }
        pos += run;
        color ^= 1;
    }
    for (row = 1; row < RS_RSC_SLEEP_H; row++) {
        col = 0;
        while (col < RS_RSC_SLEEP_W) {
            run = 0;
            while (col + run < RS_RSC_SLEEP_W &&
                   in->pixel[row * RS_RSC_SLEEP_W + col + run] ==
                       in->pixel[(row - 1) * RS_RSC_SLEEP_W + col + run] &&
                   run < 255) {
                run++;
            }
            e = rs_p1(dst, run);
            if (e) {
                return e;
            }
            col += run;
            if (col < RS_RSC_SLEEP_W) {
                uint8_t above = in->pixel[(row - 1) * RS_RSC_SLEEP_W + col];
                if (in->pixel[row * RS_RSC_SLEEP_W + col] != (uint8_t)(above ^ 1u)) {
                    return RS_ERR_FORMAT;
                }
                col++;
            }
        }
    }
    return RS_OK;
}

rs_err rs_rsc_sleep_image_decode(int revision, rs_buf *src, rs_rsc_sleep_image *out) {
    rs_err e = rsc_revs(revision, RSC_177);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    return sleep_bits_decode(src, out);
}

rs_err rs_rsc_sleep_image_encode(int revision, rs_buf *dst, const rs_rsc_sleep_image *in) {
    rs_err e = rsc_revs(revision, RSC_177);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    return sleep_bits_encode(dst, in);
}

rs_err rs_rsc_login_box_decode(int revision, rs_buf *src, rs_rsc_login_box *out) {
    rs_err e = rsc_revs(revision, RSC_159_177);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    if (rs_buf_remaining(src) < 12) {
        return RS_ERR_RANGE;
    }
    out->days_ago = rs_g4(src);
    out->recovery_days = rs_g4(src);
    out->host = (uint32_t)rs_g4(src);
    return RS_OK;
}

rs_err rs_rsc_login_box_encode(int revision, rs_buf *dst, const rs_rsc_login_box *in) {
    rs_err e = rsc_revs(revision, RSC_159_177);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    e = rs_p4(dst, in->days_ago);
    if (e) {
        return e;
    }
    e = rs_p4(dst, in->recovery_days);
    if (e) {
        return e;
    }
    return put_u32(dst, in->host);
}

rs_err rs_rsc_effect_decode(int revision, rs_buf *src, rs_rsc_effect *out) {
    rs_err e = rsc_revs(revision, RSC_159_177);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    e = get_u8(src, &out->id);
    if (e) {
        return e;
    }
    e = get_i8(src, &out->x);
    if (e) {
        return e;
    }
    return get_i8(src, &out->y);
}

rs_err rs_rsc_effect_encode(int revision, rs_buf *dst, const rs_rsc_effect *in) {
    rs_err e = rsc_revs(revision, RSC_159_177);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    e = write_u8(dst, in->id);
    if (e) {
        return e;
    }
    e = put_i8(dst, in->x);
    if (e) {
        return e;
    }
    return put_i8(dst, in->y);
}

static rs_err read_friend(rs_buf *b, rs_rsc_friend *f) {
    rs_err e = get_name(b, &f->name37);
    if (e) {
        return e;
    }
    return get_u8(b, &f->world);
}

static rs_err write_friend(rs_buf *b, const rs_rsc_friend *f) {
    rs_err e = rs_p8(b, f->name37);
    if (e) {
        return e;
    }
    return write_u8(b, f->world);
}

rs_err rs_rsc_friends_decode(int revision, rs_buf *src, rs_rsc_friends *out) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_u8(src, &out->count);
    if (e) {
        return e;
    }
    if (out->count > RS_RSC_FRIEND_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < out->count; i++) {
        e = read_friend(src, &out->friend[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_friends_encode(int revision, rs_buf *dst, const rs_rsc_friends *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (in->count < 0 || in->count > RS_RSC_FRIEND_MAX) {
        return RS_ERR_RANGE;
    }
    e = write_u8(dst, in->count);
    if (e) {
        return e;
    }
    for (i = 0; i < in->count; i++) {
        e = write_friend(dst, &in->friend[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_friend_update_decode(int revision, rs_buf *src, rs_rsc_friend *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    return read_friend(src, out);
}

rs_err rs_rsc_friend_update_encode(int revision, rs_buf *dst, const rs_rsc_friend *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    return write_friend(dst, in);
}

rs_err rs_rsc_ignores_decode(int revision, rs_buf *src, rs_rsc_ignores *out) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_u8(src, &out->count);
    if (e) {
        return e;
    }
    if (out->count > RS_RSC_IGNORE_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < out->count; i++) {
        e = get_name(src, &out->name37[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_ignores_encode(int revision, rs_buf *dst, const rs_rsc_ignores *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (in->count < 0 || in->count > RS_RSC_IGNORE_MAX) {
        return RS_ERR_RANGE;
    }
    e = write_u8(dst, in->count);
    if (e) {
        return e;
    }
    for (i = 0; i < in->count; i++) {
        e = rs_p8(dst, in->name37[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_privacy_decode(int revision, rs_buf *src, rs_rsc_privacy *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    e = get_u8(src, &out->chat);
    if (e) {
        return e;
    }
    e = get_u8(src, &out->priv);
    if (e) {
        return e;
    }
    e = get_u8(src, &out->trade);
    if (e) {
        return e;
    }
    return get_u8(src, &out->duel);
}

rs_err rs_rsc_privacy_encode(int revision, rs_buf *dst, const rs_rsc_privacy *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    e = write_u8(dst, in->chat);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->priv);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->trade);
    if (e) {
        return e;
    }
    return write_u8(dst, in->duel);
}

rs_err rs_rsc_private_message_decode(int revision, rs_buf *src, rs_rsc_private_message *out) {
    size_t n;
    size_t start;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out || out->cap < 0) {
        return RS_ERR_ARG;
    }
    start = src->pos;
    e = get_name(src, &out->from37);
    if (e) {
        return e;
    }
    n = rs_buf_remaining(src);
    if (n > (size_t)out->cap) {
        src->pos = start;
        return RS_ERR_RANGE;
    }
    if (n && !out->text) {
        src->pos = start;
        return RS_ERR_ARG;
    }
    if (n) {
        e = rs_gdata(src, out->text, n);
        if (e) {
            return e;
        }
    }
    out->len = (int)n;
    return RS_OK;
}

rs_err rs_rsc_private_message_encode(int revision, rs_buf *dst, const rs_rsc_private_message *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in || in->len < 0 || (in->len && !in->text)) {
        return RS_ERR_ARG;
    }
    e = rs_p8(dst, in->from37);
    if (e) {
        return e;
    }
    if (!in->len) {
        return RS_OK;
    }
    return rs_pdata(dst, in->text, (size_t)in->len);
}

static rs_err read_skill_byte(rs_buf *b, int *out) {
    return get_u8(b, out);
}

rs_err rs_rsc_stats_decode(int revision, rs_buf *src, rs_rsc_stats *out) {
    int n;
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    n = skill_n(revision);
    memset(out, 0, sizeof(*out));
    for (i = 0; i < n; i++) {
        e = read_skill_byte(src, &out->cur[i]);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < n; i++) {
        e = read_skill_byte(src, &out->base[i]);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < n; i++) {
        e = get_u32(src, &out->xp[i]);
        if (e) {
            return e;
        }
    }
    e = get_u8(src, &out->quest_points);
    if (e) {
        return e;
    }
    out->nskills = n;
    return RS_OK;
}

rs_err rs_rsc_stats_encode(int revision, rs_buf *dst, const rs_rsc_stats *in) {
    int n;
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    n = skill_n(revision);
    if (in->nskills != n) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < n; i++) {
        e = write_u8(dst, in->cur[i]);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < n; i++) {
        e = write_u8(dst, in->base[i]);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < n; i++) {
        e = put_u32(dst, in->xp[i]);
        if (e) {
            return e;
        }
    }
    return write_u8(dst, in->quest_points);
}

rs_err rs_rsc_stat_decode(int revision, rs_buf *src, rs_rsc_stat *out) {
    int n;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    n = skill_n(revision);
    memset(out, 0, sizeof(*out));
    e = get_u8(src, &out->index);
    if (e) {
        return e;
    }
    if (out->index >= n) {
        return RS_ERR_RANGE;
    }
    e = get_u8(src, &out->cur);
    if (e) {
        return e;
    }
    e = get_u8(src, &out->base);
    if (e) {
        return e;
    }
    return get_u32(src, &out->xp);
}

rs_err rs_rsc_stat_encode(int revision, rs_buf *dst, const rs_rsc_stat *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (in->index < 0 || in->index >= skill_n(revision)) {
        return RS_ERR_RANGE;
    }
    e = write_u8(dst, in->index);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->cur);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->base);
    if (e) {
        return e;
    }
    return put_u32(dst, in->xp);
}

rs_err rs_rsc_stat_xp_decode(int revision, rs_buf *src, rs_rsc_stat_xp *out) {
    int n;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    n = skill_n(revision);
    memset(out, 0, sizeof(*out));
    e = get_u8(src, &out->index);
    if (e) {
        return e;
    }
    if (out->index >= n) {
        return RS_ERR_RANGE;
    }
    return get_u32(src, &out->xp);
}

rs_err rs_rsc_stat_xp_encode(int revision, rs_buf *dst, const rs_rsc_stat_xp *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (in->index < 0 || in->index >= skill_n(revision)) {
        return RS_ERR_RANGE;
    }
    e = write_u8(dst, in->index);
    if (e) {
        return e;
    }
    return put_u32(dst, in->xp);
}

rs_err rs_rsc_bonuses_decode(int revision, rs_buf *src, rs_rsc_bonuses *out) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    for (i = 0; i < RS_RSC_BONUS_MAX; i++) {
        e = get_u8(src, &out->value[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_bonuses_encode(int revision, rs_buf *dst, const rs_rsc_bonuses *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    for (i = 0; i < RS_RSC_BONUS_MAX; i++) {
        e = write_u8(dst, in->value[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_prayers_decode(int revision, rs_buf *src, rs_rsc_prayers *out) {
    size_t n;
    size_t i;
    int v;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    n = rs_buf_remaining(src);
    if (n > RS_RSC_PRAYER_MAX) {
        return RS_ERR_RANGE;
    }
    memset(out, 0, sizeof(*out));
    for (i = 0; i < n; i++) {
        e = get_u8(src, &v);
        if (e) {
            return e;
        }
        out->on[i] = (uint8_t)(v == 1);
    }
    out->count = (int)n;
    return RS_OK;
}

rs_err rs_rsc_prayers_encode(int revision, rs_buf *dst, const rs_rsc_prayers *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (in->count < 0 || in->count > RS_RSC_PRAYER_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < in->count; i++) {
        e = write_bool(dst, in->on[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_quests_decode(int revision, rs_buf *src, rs_rsc_quests *out) {
    int n;
    int i;
    int v;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    n = quest_n(revision);
    if ((int)rs_buf_remaining(src) < n) {
        return RS_ERR_RANGE;
    }
    memset(out, 0, sizeof(*out));
    for (i = 0; i < n; i++) {
        e = get_u8(src, &v);
        if (e) {
            return e;
        }
        out->done[i] = (uint8_t)(v == 1);
    }
    out->count = n;
    return RS_OK;
}

rs_err rs_rsc_quests_encode(int revision, rs_buf *dst, const rs_rsc_quests *in) {
    int n;
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    n = quest_n(revision);
    if (in->count != n) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < n; i++) {
        e = write_bool(dst, in->done[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_settings_decode(int revision, rs_buf *src, rs_rsc_settings *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = read_bool(src, &out->camera);
    if (e) {
        return e;
    }
    e = read_bool(src, &out->mouse);
    if (e) {
        return e;
    }
    if (revision == 115) {
        return RS_OK;
    }
    return read_bool(src, &out->sound);
}

rs_err rs_rsc_settings_encode(int revision, rs_buf *dst, const rs_rsc_settings *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (revision == 115 && in->sound != 0) {
        return RS_ERR_RANGE;
    }
    e = write_bool(dst, in->camera);
    if (e) {
        return e;
    }
    e = write_bool(dst, in->mouse);
    if (e) {
        return e;
    }
    if (revision == 115) {
        return RS_OK;
    }
    return write_bool(dst, in->sound);
}

static rs_err read_npc_aux(rs_buf *b, rs_rsc_npc_aux *row) {
    int n;
    rs_err e;
    memset(row, 0, sizeof(*row));
    e = get_u16(b, &row->index);
    if (e) {
        return e;
    }
    e = get_u8(b, &row->type);
    if (e) {
        return e;
    }
    if (row->type == RS_RSC_NPC_AUX_CHAT) {
        e = get_u16(b, &row->player);
        if (e) {
            return e;
        }
        e = get_u8(b, &n);
        if (e) {
            return e;
        }
        if (n > RS_RSC_CHAT_MAX) {
            return RS_ERR_FORMAT;
        }
        row->chat_len = n;
        if (n) {
            return rs_gdata(b, row->chat, (size_t)n);
        }
        return RS_OK;
    }
    if (row->type == RS_RSC_NPC_AUX_HITS) {
        e = get_u8(b, &row->damage);
        if (e) {
            return e;
        }
        e = get_u8(b, &row->current);
        if (e) {
            return e;
        }
        return get_u8(b, &row->max);
    }
    return RS_OK;
}

static rs_err write_npc_aux(rs_buf *b, const rs_rsc_npc_aux *row) {
    rs_err e;
    if (row->index < 0 || row->index > 65535 || row->type < 0 || row->type > 255) {
        return RS_ERR_RANGE;
    }
    e = put_u16_range(b, row->index);
    if (e) {
        return e;
    }
    e = write_u8(b, row->type);
    if (e) {
        return e;
    }
    if (row->type == RS_RSC_NPC_AUX_CHAT) {
        if (row->player < 0 || row->player > 65535 || row->chat_len < 0 || row->chat_len > RS_RSC_CHAT_MAX) {
            return RS_ERR_RANGE;
        }
        e = put_u16_range(b, row->player);
        if (e) {
            return e;
        }
        e = write_u8(b, row->chat_len);
        if (e) {
            return e;
        }
        if (!row->chat_len) {
            return RS_OK;
        }
        return rs_pdata(b, row->chat, (size_t)row->chat_len);
    }
    if (row->type == RS_RSC_NPC_AUX_HITS) {
        e = write_u8(b, row->damage);
        if (e) {
            return e;
        }
        e = write_u8(b, row->current);
        if (e) {
            return e;
        }
        return write_u8(b, row->max);
    }
    return RS_OK;
}

rs_err rs_rsc_npc_aux_decode(int revision, rs_buf *src, rs_rsc_npc_aux_list *out) {
    int count;
    int i;
    size_t start;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out || out->cap < 0) {
        return RS_ERR_ARG;
    }
    start = src->pos;
    e = get_u16(src, &count);
    if (e) {
        return e;
    }
    if (count > out->cap) {
        src->pos = start;
        return RS_ERR_RANGE;
    }
    if (count && !out->row) {
        src->pos = start;
        return RS_ERR_ARG;
    }
    for (i = 0; i < count; i++) {
        e = read_npc_aux(src, &out->row[i]);
        if (e) {
            return e;
        }
    }
    out->count = count;
    return RS_OK;
}

rs_err rs_rsc_npc_aux_encode(int revision, rs_buf *dst, const rs_rsc_npc_aux_list *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in || in->count < 0 || (in->count && !in->row)) {
        return RS_ERR_ARG;
    }
    if (in->count > 65535) {
        return RS_ERR_RANGE;
    }
    e = rs_p2(dst, in->count);
    if (e) {
        return e;
    }
    for (i = 0; i < in->count; i++) {
        e = write_npc_aux(dst, &in->row[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_trade_offer_decode(int revision, rs_buf *src, rs_rsc_trade_offer *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    return read_count_stacks(src, RS_RSC_TRADE_MAX, &out->count, out->item);
}

rs_err rs_rsc_trade_offer_encode(int revision, rs_buf *dst, const rs_rsc_trade_offer *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    return write_count_stacks(dst, RS_RSC_TRADE_MAX, in->count, in->item);
}

rs_err rs_rsc_duel_offer_decode(int revision, rs_buf *src, rs_rsc_duel_offer *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    return read_count_stacks(src, RS_RSC_DUEL_MAX, &out->count, out->item);
}

rs_err rs_rsc_duel_offer_encode(int revision, rs_buf *dst, const rs_rsc_duel_offer *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    return write_count_stacks(dst, RS_RSC_DUEL_MAX, in->count, in->item);
}

rs_err rs_rsc_trade_open_decode(int revision, rs_buf *src, int *player) {
    return read_short_rev(revision, RSC_ALL, src, player);
}

rs_err rs_rsc_trade_open_encode(int revision, rs_buf *dst, int player) {
    return write_short_rev(revision, RSC_ALL, dst, player);
}

rs_err rs_rsc_duel_open_decode(int revision, rs_buf *src, int *player) {
    return read_short_rev(revision, RSC_ALL, src, player);
}

rs_err rs_rsc_duel_open_encode(int revision, rs_buf *dst, int player) {
    return write_short_rev(revision, RSC_ALL, dst, player);
}

rs_err rs_rsc_trade_other_decode(int revision, rs_buf *src, int *accepted) {
    return flag_decode(revision, src, accepted);
}

rs_err rs_rsc_trade_other_encode(int revision, rs_buf *dst, int accepted) {
    return flag_encode(revision, dst, accepted);
}

rs_err rs_rsc_trade_accept_decode(int revision, rs_buf *src, int *accepted) {
    return flag_decode(revision, src, accepted);
}

rs_err rs_rsc_trade_accept_encode(int revision, rs_buf *dst, int accepted) {
    return flag_encode(revision, dst, accepted);
}

rs_err rs_rsc_duel_other_decode(int revision, rs_buf *src, int *accepted) {
    return flag_decode(revision, src, accepted);
}

rs_err rs_rsc_duel_other_encode(int revision, rs_buf *dst, int accepted) {
    return flag_encode(revision, dst, accepted);
}

rs_err rs_rsc_duel_accept_decode(int revision, rs_buf *src, int *accepted) {
    return flag_decode(revision, src, accepted);
}

rs_err rs_rsc_duel_accept_encode(int revision, rs_buf *dst, int accepted) {
    return flag_encode(revision, dst, accepted);
}

rs_err rs_rsc_duel_options_decode(int revision, rs_buf *src, rs_rsc_duel_options *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    e = read_bool(src, &out->retreat);
    if (e) {
        return e;
    }
    e = read_bool(src, &out->magic);
    if (e) {
        return e;
    }
    e = read_bool(src, &out->prayer);
    if (e) {
        return e;
    }
    return read_bool(src, &out->weapons);
}

rs_err rs_rsc_duel_options_encode(int revision, rs_buf *dst, const rs_rsc_duel_options *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    e = write_bool(dst, in->retreat);
    if (e) {
        return e;
    }
    e = write_bool(dst, in->magic);
    if (e) {
        return e;
    }
    e = write_bool(dst, in->prayer);
    if (e) {
        return e;
    }
    return write_bool(dst, in->weapons);
}

rs_err rs_rsc_trade_confirm_decode(int revision, rs_buf *src, rs_rsc_trade_confirm *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_name(src, &out->name37);
    if (e) {
        return e;
    }
    e = read_count_stacks(src, RS_RSC_TRADE_MAX, &out->receive_count, out->receive);
    if (e) {
        return e;
    }
    return read_count_stacks(src, RS_RSC_TRADE_MAX, &out->give_count, out->give);
}

rs_err rs_rsc_trade_confirm_encode(int revision, rs_buf *dst, const rs_rsc_trade_confirm *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    e = rs_p8(dst, in->name37);
    if (e) {
        return e;
    }
    e = write_count_stacks(dst, RS_RSC_TRADE_MAX, in->receive_count, in->receive);
    if (e) {
        return e;
    }
    return write_count_stacks(dst, RS_RSC_TRADE_MAX, in->give_count, in->give);
}

rs_err rs_rsc_duel_confirm_decode(int revision, rs_buf *src, rs_rsc_duel_confirm *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_name(src, &out->name37);
    if (e) {
        return e;
    }
    e = read_count_stacks(src, RS_RSC_DUEL_MAX, &out->opponent_count, out->opponent);
    if (e) {
        return e;
    }
    e = read_count_stacks(src, RS_RSC_DUEL_MAX, &out->self_count, out->self);
    if (e) {
        return e;
    }
    e = get_u8(src, &out->retreat);
    if (e) {
        return e;
    }
    e = get_u8(src, &out->magic);
    if (e) {
        return e;
    }
    e = get_u8(src, &out->prayer);
    if (e) {
        return e;
    }
    return get_u8(src, &out->weapons);
}

rs_err rs_rsc_duel_confirm_encode(int revision, rs_buf *dst, const rs_rsc_duel_confirm *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    e = rs_p8(dst, in->name37);
    if (e) {
        return e;
    }
    e = write_count_stacks(dst, RS_RSC_DUEL_MAX, in->opponent_count, in->opponent);
    if (e) {
        return e;
    }
    e = write_count_stacks(dst, RS_RSC_DUEL_MAX, in->self_count, in->self);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->retreat);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->magic);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->prayer);
    if (e) {
        return e;
    }
    return write_u8(dst, in->weapons);
}

rs_err rs_rsc_shop_decode(int revision, rs_buf *src, rs_rsc_shop *out) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_u8(src, &out->count);
    if (e) {
        return e;
    }
    if (out->count > RS_RSC_SHOP_MAX) {
        return RS_ERR_RANGE;
    }
    e = get_u8(src, &out->general);
    if (e) {
        return e;
    }
    e = get_u8(src, &out->sell_mod);
    if (e) {
        return e;
    }
    e = get_u8(src, &out->buy_mod);
    if (e) {
        return e;
    }
    for (i = 0; i < out->count; i++) {
        e = get_u16(src, &out->item[i].id);
        if (e) {
            return e;
        }
        e = get_u16(src, &out->item[i].stock);
        if (e) {
            return e;
        }
        e = get_i8(src, &out->item[i].price_mod);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_shop_encode(int revision, rs_buf *dst, const rs_rsc_shop *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (in->count < 0 || in->count > RS_RSC_SHOP_MAX) {
        return RS_ERR_RANGE;
    }
    e = write_u8(dst, in->count);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->general);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->sell_mod);
    if (e) {
        return e;
    }
    e = write_u8(dst, in->buy_mod);
    if (e) {
        return e;
    }
    for (i = 0; i < in->count; i++) {
        e = put_u16_range(dst, in->item[i].id);
        if (e) {
            return e;
        }
        e = put_u16_range(dst, in->item[i].stock);
        if (e) {
            return e;
        }
        e = put_i8(dst, in->item[i].price_mod);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

static rs_err read_bank_item(rs_buf *b, rs_rsc_bank_item *item) {
    rs_err e = get_u16(b, &item->id);
    if (e) {
        return e;
    }
    return get_gsmart(b, &item->amount);
}

static rs_err write_bank_item(rs_buf *b, const rs_rsc_bank_item *item) {
    rs_err e = put_u16_range(b, item->id);
    if (e) {
        return e;
    }
    return put_gsmart(b, item->amount);
}

rs_err rs_rsc_bank_decode(int revision, rs_buf *src, rs_rsc_bank *out) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_u8(src, &out->count);
    if (e) {
        return e;
    }
    if (out->count > RS_RSC_BANK_MAX) {
        return RS_ERR_RANGE;
    }
    if (revision != 115) {
        e = get_u8(src, &out->max_items);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < out->count; i++) {
        e = read_bank_item(src, &out->item[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_bank_encode(int revision, rs_buf *dst, const rs_rsc_bank *in) {
    int i;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (in->count < 0 || in->count > RS_RSC_BANK_MAX) {
        return RS_ERR_RANGE;
    }
    if (revision == 115 && in->max_items != 0) {
        return RS_ERR_RANGE;
    }
    e = write_u8(dst, in->count);
    if (e) {
        return e;
    }
    if (revision != 115) {
        e = write_u8(dst, in->max_items);
        if (e) {
            return e;
        }
    }
    for (i = 0; i < in->count; i++) {
        e = write_bank_item(dst, &in->item[i]);
        if (e) {
            return e;
        }
    }
    return RS_OK;
}

rs_err rs_rsc_bank_slot_decode(int revision, rs_buf *src, rs_rsc_bank_slot *out) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_u8(src, &out->slot);
    if (e) {
        return e;
    }
    e = get_u16(src, &out->id);
    if (e) {
        return e;
    }
    return get_gsmart(src, &out->amount);
}

rs_err rs_rsc_bank_slot_encode(int revision, rs_buf *dst, const rs_rsc_bank_slot *in) {
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    e = write_u8(dst, in->slot);
    if (e) {
        return e;
    }
    e = put_u16_range(dst, in->id);
    if (e) {
        return e;
    }
    return put_gsmart(dst, in->amount);
}

rs_err rs_rsc_strings_decode(int revision, rs_buf *src, rs_rsc_strings *out) {
    int i;
    int n;
    int c;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    e = get_u8(src, &out->count);
    if (e) {
        return e;
    }
    if (out->count > RS_RSC_STRINGS_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < out->count; i++) {
        e = get_u8(src, &n);
        if (e) {
            return e;
        }
        if (n > RS_RSC_STRING_MAX) {
            return RS_ERR_RANGE;
        }
        if (n) {
            e = rs_gdata(src, out->row[i].text, (size_t)n);
            if (e) {
                return e;
            }
        }
        for (c = 0; c < n; c++) {
            if (out->row[i].text[c] == 0) {
                return RS_ERR_FORMAT;
            }
        }
        out->row[i].text[n] = 0;
        out->row[i].len = n;
    }
    return RS_OK;
}

rs_err rs_rsc_strings_encode(int revision, rs_buf *dst, const rs_rsc_strings *in) {
    int i;
    int c;
    rs_err e = rsc_revision(revision);
    if (e) {
        return e;
    }
    if (!dst || !in) {
        return RS_ERR_ARG;
    }
    if (in->count < 0 || in->count > RS_RSC_STRINGS_MAX) {
        return RS_ERR_RANGE;
    }
    e = write_u8(dst, in->count);
    if (e) {
        return e;
    }
    for (i = 0; i < in->count; i++) {
        if (in->row[i].len < 0 || in->row[i].len > RS_RSC_STRING_MAX || in->row[i].text[in->row[i].len] != 0) {
            return RS_ERR_RANGE;
        }
        for (c = 0; c < in->row[i].len; c++) {
            if (in->row[i].text[c] == 0) {
                return RS_ERR_FORMAT;
            }
        }
        e = write_u8(dst, in->row[i].len);
        if (e) {
            return e;
        }
        if (in->row[i].len) {
            e = rs_pdata(dst, in->row[i].text, (size_t)in->row[i].len);
            if (e) {
                return e;
            }
        }
    }
    return RS_OK;
}
