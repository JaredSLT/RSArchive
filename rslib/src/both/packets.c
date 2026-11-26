#include "both/packets_impl.h"

#include <stdint.h>

#define RS_PKT_TABLES 48

typedef struct {
    int revision;
    int used;
    int16_t to_op[RS_PKT_MAX];
    int16_t from_c2s[256];
    int16_t from_s2c[256];
} rs_pkt_table;

static rs_pkt_table g_tables[RS_PKT_TABLES];
static int g_ready;

static void table_clear(rs_pkt_table *t) {
    int i;
    for (i = 0; i < RS_PKT_MAX; i++) {
        t->to_op[i] = -1;
    }
    for (i = 0; i < 256; i++) {
        t->from_c2s[i] = -1;
        t->from_s2c[i] = -1;
    }
}

static rs_pkt_table *table_find(int revision, int create) {
    int i, empty = -1;
    for (i = 0; i < RS_PKT_TABLES; i++) {
        if (g_tables[i].used && g_tables[i].revision == revision) {
            return &g_tables[i];
        }
        if (!g_tables[i].used && empty < 0) {
            empty = i;
        }
    }
    if (!create || empty < 0) {
        return NULL;
    }
    table_clear(&g_tables[empty]);
    g_tables[empty].used = 1;
    g_tables[empty].revision = revision;
    return &g_tables[empty];
}

void rs_pkt_table_load(int revision, const rs_pkt_bind_row *rows, int n, rs_pkt_dir dir) {
    rs_pkt_table *t = table_find(revision, 1);
    int i;
    if (!t || !rows) {
        return;
    }
    for (i = 0; i < n; i++) {
        rs_pkt id = rows[i].pkt;
        int op = rows[i].op;
        if (id <= 0 || id >= RS_PKT_MAX || op < 0 || op > 255) {
            continue;
        }
        t->to_op[id] = (int16_t)op;
        if (dir == RS_DIR_C2S) {
            t->from_c2s[op] = (int16_t)id;
        } else {
            t->from_s2c[op] = (int16_t)id;
        }
    }
}

static void pkt_init(void) {
    if (g_ready) {
        return;
    }
    g_ready = 1;
    rs_packets_client_register();
    rs_packets_server_register();
}

int rs_session_pkt_opcode(const rs_session *s, rs_pkt id) {
    return rs_pkt_opcode(s ? s->build : NULL, id);
}

rs_pkt rs_session_pkt_id(const rs_session *s, int opcode) {
    rs_pkt_dir dir = (s && s->role == RS_ROLE_CLIENT) ? RS_DIR_S2C : RS_DIR_C2S;
    return rs_pkt_from_opcode(s ? s->build : NULL, dir, opcode);
}

int rs_pkt_opcode(const rs_build_info *build, rs_pkt id) {
    rs_pkt_table *t;
    pkt_init();
    if (!build || id <= 0 || id >= RS_PKT_MAX) {
        return -1;
    }
    t = table_find(build->revision, 0);
    if (!t) {
        return -1;
    }
    return t->to_op[id];
}

rs_pkt rs_pkt_from_opcode(const rs_build_info *build, rs_pkt_dir dir, int opcode) {
    rs_pkt_table *t;
    int v;
    pkt_init();
    if (!build || opcode < 0 || opcode > 255) {
        return RS_PKT_NONE;
    }
    t = table_find(build->revision, 0);
    if (!t) {
        return RS_PKT_NONE;
    }
    v = (dir == RS_DIR_C2S) ? t->from_c2s[opcode] : t->from_s2c[opcode];
    return v > 0 ? (rs_pkt)v : RS_PKT_NONE;
}

rs_err rs_pkt_bind(int revision, rs_pkt id, int opcode) {
    rs_pkt_table *t;
    rs_pkt_dir dir;
    pkt_init();
    if (revision <= 0 || id <= 0 || id >= RS_PKT_MAX || opcode < 0 || opcode > 255) {
        return RS_ERR_ARG;
    }
    t = table_find(revision, 1);
    if (!t) {
        return RS_ERR_RANGE;
    }
    dir = rs_pkt_direction(id);
    t->to_op[id] = (int16_t)opcode;
    if (dir == RS_DIR_C2S) {
        t->from_c2s[opcode] = (int16_t)id;
    } else {
        t->from_s2c[opcode] = (int16_t)id;
    }
    return RS_OK;
}

rs_err rs_pkt_copy(int dst_revision, int src_revision) {
    rs_pkt_table *src, *dst;
    int i;
    pkt_init();
    if (dst_revision <= 0 || src_revision <= 0) {
        return RS_ERR_ARG;
    }
    src = table_find(src_revision, 0);
    if (!src) {
        return RS_ERR_NOT_FOUND;
    }
    dst = table_find(dst_revision, 1);
    if (!dst) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < RS_PKT_MAX; i++) {
        dst->to_op[i] = src->to_op[i];
    }
    for (i = 0; i < 256; i++) {
        dst->from_c2s[i] = src->from_c2s[i];
        dst->from_s2c[i] = src->from_s2c[i];
    }
    return RS_OK;
}

rs_err rs_session_write_pkt(rs_session *s, rs_buf *dst, rs_pkt id, const uint8_t *payload,
                            size_t n) {
    int op;
    if (!s) {
        return RS_ERR_ARG;
    }
    op = rs_session_pkt_opcode(s, id);
    if (op < 0) {
        return RS_ERR_UNSUPPORTED;
    }
    return rs_session_write(s, dst, op, payload, n);
}

rs_err rs_session_read_pkt(rs_session *s, rs_buf *src, rs_pkt *id, int *opcode, rs_buf *payload) {
    int op;
    rs_err e = rs_session_read(s, src, &op, payload);
    if (e != RS_OK) {
        return e;
    }
    if (opcode) {
        *opcode = op;
    }
    if (id) {
        *id = rs_session_pkt_id(s, op);
    }
    return RS_OK;
}
