#include "rslib/both/asset.h"

#include "internal.h"

static int *alloc_ints(int n) {
    if (n <= 0) {
        return NULL;
    }
    return calloc((size_t)n, sizeof(int));
}

static int take_u8(const uint8_t *data, size_t end, size_t *pos, int *ok) {
    if (!*ok || *pos >= end) {
        *ok = 0;
        return 0;
    }
    return data[(*pos)++] & 0xff;
}

static int take_u16(const uint8_t *data, size_t end, size_t *pos, int *ok) {
    int v;
    if (!*ok || *pos + 2 > end) {
        *ok = 0;
        return 0;
    }
    v = (data[*pos] << 8) | data[*pos + 1];
    *pos += 2;
    return v;
}

/* Packet.gsmart: byte < 128 ? g1()-64 : g2()-49152. */
static int take_smart(const uint8_t *data, size_t end, size_t *pos, int *ok) {
    unsigned v;
    if (!*ok || *pos >= end) {
        *ok = 0;
        return 0;
    }
    v = data[*pos] & 0xff;
    if (v < 128) {
        (*pos)++;
        return (int)v - 64;
    }
    if (*pos + 2 > end) {
        *ok = 0;
        return 0;
    }
    v = ((unsigned)data[*pos] << 8) | data[*pos + 1];
    *pos += 2;
    return (int)v - 49152;
}

void rs_model_free(rs_model *m) {
    if (!m) {
        return;
    }
    free(m->vertex_x);
    free(m->vertex_y);
    free(m->vertex_z);
    free(m->vertex_label);
    free(m->face_a);
    free(m->face_b);
    free(m->face_c);
    free(m->face_colour);
    free(m->face_info);
    free(m->face_priority);
    free(m->face_alpha);
    free(m->face_label);
    free(m->textured_a);
    free(m->textured_b);
    free(m->textured_c);
    free(m);
}

int rs_model_vertex_count(const rs_model *m) {
    return m ? m->vertex_count : 0;
}

int rs_model_face_count(const rs_model *m) {
    return m ? m->face_count : 0;
}

static rs_err model_317(const uint8_t *data, size_t len, rs_model **out) {
    const uint8_t *h;
    int vc, fc, tc, info_flag, pri_flag, alpha_flag, flabel_flag, vlabel_flag;
    int xlen, ylen, zlen, fv_len;
    int has_pri, has_flab, has_info, has_vlab, has_alpha;
    size_t cursor, off_ori, off_pri, off_flab, off_info, off_vlab, off_alpha;
    size_t off_fv, off_col, off_tex, off_x, off_y, off_z;
    rs_model *m;
    rs_err e;
    int i, ok, x, y, z, a, b, c, last;
    size_t xp, yp, zp, lp, op, fp, cp, pp, ip, ap, flp, tp;

    if (len < 18) {
        return RS_ERR_FORMAT;
    }
    h = data + (len - 18);
    vc = (h[0] << 8) | h[1];
    fc = (h[2] << 8) | h[3];
    tc = h[4];
    info_flag = h[5];
    pri_flag = h[6];
    alpha_flag = h[7];
    flabel_flag = h[8];
    vlabel_flag = h[9];
    xlen = (h[10] << 8) | h[11];
    ylen = (h[12] << 8) | h[13];
    zlen = (h[14] << 8) | h[15];
    fv_len = (h[16] << 8) | h[17];

    cursor = (size_t)vc;
    off_ori = cursor;
    cursor += (size_t)fc;
    has_pri = pri_flag == 255;
    off_pri = cursor;
    if (has_pri) {
        cursor += (size_t)fc;
    }
    has_flab = flabel_flag == 1;
    off_flab = cursor;
    if (has_flab) {
        cursor += (size_t)fc;
    }
    has_info = info_flag == 1;
    off_info = cursor;
    if (has_info) {
        cursor += (size_t)fc;
    }
    has_vlab = vlabel_flag == 1;
    off_vlab = cursor;
    if (has_vlab) {
        cursor += (size_t)vc;
    }
    has_alpha = alpha_flag == 1;
    off_alpha = cursor;
    if (has_alpha) {
        cursor += (size_t)fc;
    }
    off_fv = cursor;
    cursor += (size_t)fv_len;
    off_col = cursor;
    cursor += (size_t)fc * 2u;
    off_tex = cursor;
    cursor += (size_t)tc * 6u;
    off_x = cursor;
    cursor += (size_t)xlen;
    off_y = cursor;
    cursor += (size_t)ylen;
    off_z = cursor;
    cursor += (size_t)zlen;
    if (cursor + 18 != len) {
        return RS_ERR_FORMAT;
    }

    m = calloc(1, sizeof(*m));
    if (!m) {
        return RS_ERR_NOMEM;
    }
    m->vertex_count = vc;
    m->face_count = fc;
    m->textured_count = tc;
    m->priority = has_pri ? 0 : pri_flag;
    m->vertex_x = alloc_ints(vc);
    m->vertex_y = alloc_ints(vc);
    m->vertex_z = alloc_ints(vc);
    m->vertex_label = has_vlab ? alloc_ints(vc) : NULL;
    m->face_a = alloc_ints(fc);
    m->face_b = alloc_ints(fc);
    m->face_c = alloc_ints(fc);
    m->face_colour = alloc_ints(fc);
    m->face_info = has_info ? alloc_ints(fc) : NULL;
    m->face_priority = has_pri ? alloc_ints(fc) : NULL;
    m->face_alpha = has_alpha ? alloc_ints(fc) : NULL;
    m->face_label = has_flab ? alloc_ints(fc) : NULL;
    m->textured_a = alloc_ints(tc);
    m->textured_b = alloc_ints(tc);
    m->textured_c = alloc_ints(tc);
    if ((vc > 0 && (!m->vertex_x || !m->vertex_y || !m->vertex_z)) ||
        (has_vlab && vc > 0 && !m->vertex_label) ||
        (fc > 0 && (!m->face_a || !m->face_b || !m->face_c || !m->face_colour)) ||
        (has_info && fc > 0 && !m->face_info) ||
        (has_pri && fc > 0 && !m->face_priority) ||
        (has_alpha && fc > 0 && !m->face_alpha) ||
        (has_flab && fc > 0 && !m->face_label) ||
        (tc > 0 && (!m->textured_a || !m->textured_b || !m->textured_c))) {
        rs_model_free(m);
        return RS_ERR_NOMEM;
    }

    ok = 1;
    x = 0;
    y = 0;
    z = 0;
    xp = off_x;
    yp = off_y;
    zp = off_z;
    lp = off_vlab;
    for (i = 0; i < vc; i++) {
        int flag = data[i] & 0xff;
        int dx = 0, dy = 0, dz = 0;
        if (flag & 1) {
            dx = take_smart(data, off_x + (size_t)xlen, &xp, &ok);
        }
        if (flag & 2) {
            dy = take_smart(data, off_y + (size_t)ylen, &yp, &ok);
        }
        if (flag & 4) {
            dz = take_smart(data, off_z + (size_t)zlen, &zp, &ok);
        }
        if (!ok) {
            rs_model_free(m);
            return RS_ERR_FORMAT;
        }
        x += dx;
        y += dy;
        z += dz;
        m->vertex_x[i] = x;
        m->vertex_y[i] = y;
        m->vertex_z[i] = z;
        if (m->vertex_label) {
            m->vertex_label[i] = take_u8(data, off_vlab + (size_t)vc, &lp, &ok);
            if (!ok) {
                rs_model_free(m);
                return RS_ERR_FORMAT;
            }
        }
    }

    cp = off_col;
    pp = off_pri;
    ip = off_info;
    ap = off_alpha;
    flp = off_flab;
    for (i = 0; i < fc; i++) {
        m->face_colour[i] = take_u16(data, off_col + (size_t)fc * 2u, &cp, &ok);
        if (m->face_info) {
            m->face_info[i] = take_u8(data, off_info + (size_t)fc, &ip, &ok);
        }
        if (m->face_priority) {
            m->face_priority[i] = take_u8(data, off_pri + (size_t)fc, &pp, &ok);
        }
        if (m->face_alpha) {
            m->face_alpha[i] = take_u8(data, off_alpha + (size_t)fc, &ap, &ok);
        }
        if (m->face_label) {
            m->face_label[i] = take_u8(data, off_flab + (size_t)fc, &flp, &ok);
        }
    }
    if (!ok) {
        rs_model_free(m);
        return RS_ERR_FORMAT;
    }

    a = 0;
    b = 0;
    c = 0;
    last = 0;
    op = off_ori;
    fp = off_fv;
    for (i = 0; i < fc; i++) {
        int kind = take_u8(data, off_ori + (size_t)fc, &op, &ok);
        if (kind == 1) {
            a = take_smart(data, off_fv + (size_t)fv_len, &fp, &ok) + last;
            b = take_smart(data, off_fv + (size_t)fv_len, &fp, &ok) + a;
            c = take_smart(data, off_fv + (size_t)fv_len, &fp, &ok) + b;
            last = c;
            m->face_a[i] = a;
            m->face_b[i] = b;
            m->face_c[i] = c;
        } else if (kind == 2) {
            b = c;
            c = take_smart(data, off_fv + (size_t)fv_len, &fp, &ok) + last;
            last = c;
            m->face_a[i] = a;
            m->face_b[i] = b;
            m->face_c[i] = c;
        } else if (kind == 3) {
            a = c;
            c = take_smart(data, off_fv + (size_t)fv_len, &fp, &ok) + last;
            last = c;
            m->face_a[i] = a;
            m->face_b[i] = b;
            m->face_c[i] = c;
        } else if (kind == 4) {
            int tmp = a;
            a = b;
            b = tmp;
            c = take_smart(data, off_fv + (size_t)fv_len, &fp, &ok) + last;
            last = c;
            m->face_a[i] = a;
            m->face_b[i] = tmp;
            m->face_c[i] = c;
        }
        if (!ok) {
            rs_model_free(m);
            return RS_ERR_FORMAT;
        }
    }

    tp = off_tex;
    for (i = 0; i < tc; i++) {
        m->textured_a[i] = take_u16(data, off_tex + (size_t)tc * 6u, &tp, &ok);
        m->textured_b[i] = take_u16(data, off_tex + (size_t)tc * 6u, &tp, &ok);
        m->textured_c[i] = take_u16(data, off_tex + (size_t)tc * 6u, &tp, &ok);
    }
    if (!ok) {
        rs_model_free(m);
        return RS_ERR_FORMAT;
    }

    e = RS_OK;
    *out = m;
    return e;
}

static int model_tail_317(int revision) {
    return revision == 317 || revision == 254 || revision == 377 || revision == 474 ||
           revision == 530;
}

rs_err rs_model_decode(int revision, const uint8_t *data, size_t len, rs_model **out) {
    if (!model_tail_317(revision)) {
        if (out) {
            *out = NULL;
        }
        return RS_ERR_UNSUPPORTED;
    }
    /* C88f97d27(byte[]) and QuickChatPhraseType(byte[]): FF FF selects the 23-byte reader. */
    if ((revision == 474 || revision == 530) && data && len >= 2 && data[len - 1] == 0xff &&
        data[len - 2] == 0xff) {
        if (out) {
            *out = NULL;
        }
        return RS_ERR_UNSUPPORTED;
    }
    if (!data || !out) {
        return RS_ERR_ARG;
    }
    *out = NULL;
    return model_317(data, len, out);
}

static rs_err need(const rs_buf *b, size_t n) {
    if (!b || rs_buf_remaining(b) < n) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err read_jstr(rs_buf *b, char **out) {
    size_t start, n;
    char *s;
    rs_err e = need(b, 1);
    if (e != RS_OK) {
        return e;
    }
    start = b->pos;
    while (b->pos < b->cap && b->data[b->pos] != 10) {
        b->pos++;
    }
    if (b->pos >= b->cap) {
        return RS_ERR_FORMAT;
    }
    n = b->pos - start;
    b->pos++;
    s = malloc(n + 1);
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

static void clear_str(char **s) {
    free(*s);
    *s = NULL;
}

void rs_component_free(rs_component *c) {
    int i;
    if (!c) {
        return;
    }
    clear_str(&c->text);
    clear_str(&c->active_text);
    clear_str(&c->option);
    clear_str(&c->target_verb);
    clear_str(&c->target_text);
    clear_str(&c->graphic);
    clear_str(&c->active_graphic);
    for (i = 0; i < RS_COMPONENT_ACTIONS; i++) {
        clear_str(&c->action[i]);
    }
    for (i = 0; i < RS_COMPONENT_INV_GFX; i++) {
        clear_str(&c->inv_graphic[i]);
    }
    free(c->script_comparator);
    free(c->script_operand);
    free(c->script_len);
    free(c->script_code);
    free(c->child_id);
    free(c->child_x);
    free(c->child_y);
    c->script_comparator = NULL;
    c->script_operand = NULL;
    c->script_len = NULL;
    c->script_code = NULL;
    c->child_id = NULL;
    c->child_x = NULL;
    c->child_y = NULL;
    c->script_n = 0;
    c->script_count = 0;
    c->child_count = 0;
}

static rs_err read_actions(rs_buf *b, rs_component *c) {
    int i;
    for (i = 0; i < RS_COMPONENT_ACTIONS; i++) {
        rs_err e = read_jstr(b, &c->action[i]);
        if (e != RS_OK) {
            return e;
        }
        if (c->action[i][0] == '\0') {
            clear_str(&c->action[i]);
        }
    }
    return RS_OK;
}

static rs_err default_option(rs_component *c) {
    const char *d = NULL;
    if (c->option && c->option[0] != '\0') {
        return RS_OK;
    }
    if (c->button_type == 1) {
        d = "Ok";
    } else if (c->button_type == 4 || c->button_type == 5) {
        d = "Select";
    } else if (c->button_type == 6) {
        d = "Continue";
    }
    if (!d) {
        return RS_OK;
    }
    clear_str(&c->option);
    c->option = malloc(strlen(d) + 1);
    if (!c->option) {
        return RS_ERR_NOMEM;
    }
    memcpy(c->option, d, strlen(d) + 1);
    return RS_OK;
}

static rs_err component_body(rs_buf *b, rs_component *out) {
    rs_err e;
    int i, n, total, len, ov, v, nc;

    memset(out, 0, sizeof(*out));
    out->layer = -1;
    out->font = -1;
    out->overlayer = -1;

    if ((e = need(b, 10)) != RS_OK) {
        goto fail;
    }
    out->type = rs_g1(b);
    out->button_type = rs_g1(b);
    out->client_code = rs_g2(b);
    out->width = rs_g2(b);
    out->height = rs_g2(b);
    out->alpha = rs_g1(b);
    ov = rs_g1(b);
    if (ov != 0) {
        if ((e = need(b, 1)) != RS_OK) {
            goto fail;
        }
        out->overlayer = ((ov - 1) << 8) + rs_g1(b);
    }

    if ((e = need(b, 1)) != RS_OK) {
        goto fail;
    }
    n = rs_g1(b);
    out->script_n = n;
    if (n > 0) {
        if ((e = need(b, (size_t)n * 3u)) != RS_OK) {
            goto fail;
        }
        out->script_comparator = alloc_ints(n);
        out->script_operand = alloc_ints(n);
        if (!out->script_comparator || !out->script_operand) {
            e = RS_ERR_NOMEM;
            goto fail;
        }
        for (i = 0; i < n; i++) {
            out->script_comparator[i] = rs_g1(b);
            out->script_operand[i] = rs_g2(b);
        }
    }

    if ((e = need(b, 1)) != RS_OK) {
        goto fail;
    }
    n = rs_g1(b);
    out->script_count = n;
    if (n > 0) {
        out->script_len = alloc_ints(n);
        if (!out->script_len) {
            e = RS_ERR_NOMEM;
            goto fail;
        }
    }
    total = 0;
    for (i = 0; i < n; i++) {
        int k;
        if ((e = need(b, 2)) != RS_OK) {
            goto fail;
        }
        len = rs_g2(b);
        if (len < 0 || (size_t)len > (SIZE_MAX / 2u) ||
            rs_buf_remaining(b) < (size_t)len * 2u) {
            e = RS_ERR_FORMAT;
            goto fail;
        }
        if (total > (INT32_MAX - len)) {
            e = RS_ERR_FORMAT;
            goto fail;
        }
        out->script_len[i] = len;
        if (len > 0) {
            int *code = realloc(out->script_code, (size_t)(total + len) * sizeof(int));
            if (!code) {
                e = RS_ERR_NOMEM;
                goto fail;
            }
            out->script_code = code;
            for (k = 0; k < len; k++) {
                out->script_code[total++] = rs_g2(b);
            }
        }
    }

    if (out->type == 0) {
        if ((e = need(b, 5)) != RS_OK) {
            goto fail;
        }
        out->scroll = rs_g2(b);
        out->hide = rs_g1(b) == 1;
        nc = rs_g2(b);
        out->child_count = nc;
        if (nc > 0) {
            if ((e = need(b, (size_t)nc * 6u)) != RS_OK) {
                goto fail;
            }
            out->child_id = alloc_ints(nc);
            out->child_x = alloc_ints(nc);
            out->child_y = alloc_ints(nc);
            if (!out->child_id || !out->child_x || !out->child_y) {
                e = RS_ERR_NOMEM;
                goto fail;
            }
            for (i = 0; i < nc; i++) {
                out->child_id[i] = rs_g2(b);
                out->child_x[i] = rs_g2b(b);
                out->child_y[i] = rs_g2b(b);
            }
        }
    }

    if (out->type == 1) {
        if ((e = need(b, 3)) != RS_OK) {
            goto fail;
        }
        out->type1_word = rs_g2(b);
        out->type1_flag = rs_g1(b) == 1;
    }

    if (out->type == 2) {
        if ((e = need(b, 6)) != RS_OK) {
            goto fail;
        }
        out->draggable = rs_g1(b) == 1;
        out->interactable = rs_g1(b) == 1;
        out->usable = rs_g1(b) == 1;
        out->swappable = rs_g1(b) == 1;
        out->margin_x = rs_g1(b);
        out->margin_y = rs_g1(b);
        for (i = 0; i < RS_COMPONENT_INV_GFX; i++) {
            if ((e = need(b, 1)) != RS_OK) {
                goto fail;
            }
            v = rs_g1(b);
            if (v == 1) {
                if ((e = need(b, 4)) != RS_OK) {
                    goto fail;
                }
                out->inv_sprite[i] = 1;
                out->inv_x[i] = rs_g2b(b);
                out->inv_y[i] = rs_g2b(b);
                if ((e = read_jstr(b, &out->inv_graphic[i])) != RS_OK) {
                    goto fail;
                }
            }
        }
        if ((e = read_actions(b, out)) != RS_OK) {
            goto fail;
        }
    }

    if (out->type == 3) {
        if ((e = need(b, 1)) != RS_OK) {
            goto fail;
        }
        out->fill = rs_g1(b) == 1;
    }

    if (out->type == 4 || out->type == 1) {
        if ((e = need(b, 3)) != RS_OK) {
            goto fail;
        }
        out->center = rs_g1(b) == 1;
        out->font = rs_g1(b);
        out->shadowed = rs_g1(b) == 1;
    }

    if (out->type == 4) {
        if ((e = read_jstr(b, &out->text)) != RS_OK ||
            (e = read_jstr(b, &out->active_text)) != RS_OK) {
            goto fail;
        }
    }

    if (out->type == 1 || out->type == 3 || out->type == 4) {
        if ((e = need(b, 4)) != RS_OK) {
            goto fail;
        }
        out->colour = rs_g4(b);
    }

    if (out->type == 3 || out->type == 4) {
        if ((e = need(b, 12)) != RS_OK) {
            goto fail;
        }
        out->active_colour = rs_g4(b);
        out->over_colour = rs_g4(b);
        out->active_over_colour = rs_g4(b);
    }

    if (out->type == 5) {
        if ((e = read_jstr(b, &out->graphic)) != RS_OK ||
            (e = read_jstr(b, &out->active_graphic)) != RS_OK) {
            goto fail;
        }
    }

    if (out->type == 6) {
        if ((e = need(b, 1)) != RS_OK) {
            goto fail;
        }
        v = rs_g1(b);
        if (v != 0) {
            if ((e = need(b, 1)) != RS_OK) {
                goto fail;
            }
            out->model_type = 1;
            out->model = ((v - 1) << 8) + rs_g1(b);
        }
        if ((e = need(b, 1)) != RS_OK) {
            goto fail;
        }
        v = rs_g1(b);
        if (v != 0) {
            if ((e = need(b, 1)) != RS_OK) {
                goto fail;
            }
            out->active_model_type = 1;
            out->active_model = ((v - 1) << 8) + rs_g1(b);
        }
        if ((e = need(b, 1)) != RS_OK) {
            goto fail;
        }
        v = rs_g1(b);
        if (v != 0) {
            if ((e = need(b, 1)) != RS_OK) {
                goto fail;
            }
            out->anim = ((v - 1) << 8) + rs_g1(b);
        } else {
            out->anim = -1;
        }
        if ((e = need(b, 1)) != RS_OK) {
            goto fail;
        }
        v = rs_g1(b);
        if (v != 0) {
            if ((e = need(b, 1)) != RS_OK) {
                goto fail;
            }
            out->active_anim = ((v - 1) << 8) + rs_g1(b);
        } else {
            out->active_anim = -1;
        }
        if ((e = need(b, 6)) != RS_OK) {
            goto fail;
        }
        out->zoom = rs_g2(b);
        out->xan = rs_g2(b);
        out->yan = rs_g2(b);
    }

    if (out->type == 7) {
        if ((e = need(b, 1 + 1 + 1 + 4 + 2 + 2 + 1)) != RS_OK) {
            goto fail;
        }
        out->center = rs_g1(b) == 1;
        out->font = rs_g1(b);
        out->shadowed = rs_g1(b) == 1;
        out->colour = rs_g4(b);
        out->margin_x = rs_g2b(b);
        out->margin_y = rs_g2b(b);
        out->interactable = rs_g1(b) == 1;
        if ((e = read_actions(b, out)) != RS_OK) {
            goto fail;
        }
    }

    if (out->button_type == 2 || out->type == 2) {
        if ((e = read_jstr(b, &out->target_verb)) != RS_OK ||
            (e = read_jstr(b, &out->target_text)) != RS_OK) {
            goto fail;
        }
        if ((e = need(b, 2)) != RS_OK) {
            goto fail;
        }
        out->target_mask = rs_g2(b);
    }

    if (out->button_type == 1 || out->button_type == 4 || out->button_type == 5 ||
        out->button_type == 6) {
        if ((e = read_jstr(b, &out->option)) != RS_OK || (e = default_option(out)) != RS_OK) {
            goto fail;
        }
    }
    return RS_OK;

fail:
    rs_component_free(out);
    memset(out, 0, sizeof(*out));
    return e;
}

/* 254 Component.unpack reads the same body as 317. 377 call9e941ef0 adds type 8. */
static int component_layout_317(int revision) {
    return revision == 317 || revision == 254;
}

rs_err rs_component_decode(int revision, rs_buf *src, rs_component *out) {
    if (!component_layout_317(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    if (!src || !out) {
        return RS_ERR_ARG;
    }
    return component_body(src, out);
}

static rs_err read_record(rs_buf *b, int declared, int *layer, rs_component *out) {
    int id;
    rs_err e;
    memset(out, 0, sizeof(*out));
    if ((e = need(b, 2)) != RS_OK) {
        return e;
    }
    id = rs_g2(b);
    if (id == 65535) {
        if ((e = need(b, 4)) != RS_OK) {
            return e;
        }
        *layer = rs_g2(b);
        id = rs_g2(b);
    }
    if (id >= declared) {
        return RS_ERR_FORMAT;
    }
    e = component_body(b, out);
    if (e != RS_OK) {
        return e;
    }
    out->id = id;
    out->layer = *layer;
    return RS_OK;
}

static void release_n(rs_component *list, int n) {
    int i;
    for (i = 0; i < n; i++) {
        rs_component_free(&list[i]);
        memset(&list[i], 0, sizeof(list[i]));
    }
}

rs_err rs_component_archive_decode(int revision, const uint8_t *data, size_t len,
                                   rs_component *out, int cap, int *count) {
    rs_buf b;
    int declared, layer, n;
    rs_err e;
    if (count) {
        *count = 0;
    }
    if (!component_layout_317(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    if (!data || !count || cap < 0 || (cap > 0 && !out)) {
        return RS_ERR_ARG;
    }
    if (len < 2) {
        return RS_ERR_FORMAT;
    }
    rs_buf_wrap(&b, (uint8_t *)data, len);
    declared = rs_g2(&b);
    layer = -1;
    n = 0;
    while (rs_buf_remaining(&b) > 0) {
        if (n == cap) {
            rs_component extra;
            e = read_record(&b, declared, &layer, &extra);
            rs_component_free(&extra);
            release_n(out, n);
            return e == RS_OK ? RS_ERR_RANGE : e;
        }
        e = read_record(&b, declared, &layer, &out[n]);
        if (e != RS_OK) {
            release_n(out, n);
            return e;
        }
        n++;
    }
    *count = n;
    return RS_OK;
}

rs_err rs_interface_decode(int revision, const uint8_t *data, size_t len,
                           rs_component **out, int *count) {
    rs_buf b;
    rs_component *list;
    int declared, layer, n, cap;
    rs_err e;
    if (out) {
        *out = NULL;
    }
    if (count) {
        *count = 0;
    }
    if (!component_layout_317(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    if (!data || !out || !count) {
        return RS_ERR_ARG;
    }
    if (len < 2) {
        return RS_ERR_FORMAT;
    }
    rs_buf_wrap(&b, (uint8_t *)data, len);
    declared = rs_g2(&b);
    layer = -1;
    list = NULL;
    n = 0;
    cap = 0;
    while (rs_buf_remaining(&b) > 0) {
        if (n == cap) {
            int ncap = cap ? cap * 2 : 4;
            rs_component *grown = realloc(list, (size_t)ncap * sizeof(*list));
            if (!grown) {
                release_n(list, n);
                free(list);
                return RS_ERR_NOMEM;
            }
            memset(grown + cap, 0, (size_t)(ncap - cap) * sizeof(*grown));
            list = grown;
            cap = ncap;
        }
        e = read_record(&b, declared, &layer, &list[n]);
        if (e != RS_OK) {
            release_n(list, n);
            free(list);
            return e;
        }
        n++;
    }
    *out = list;
    *count = n;
    return RS_OK;
}

void rs_interface_free(rs_component *list, int count) {
    if (!list) {
        return;
    }
    if (count > 0) {
        release_n(list, count);
    }
    free(list);
}
