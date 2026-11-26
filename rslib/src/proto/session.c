#include "proto/ops.h"

void rs_session_init_role(rs_session *s, const rs_build_info *build, rs_role role) {
    memset(s, 0, sizeof(*s));
    s->build = build;
    s->role = role;
    s->ops = rs_proto_for(build);
}

void rs_session_init(rs_session *s, const rs_build_info *build) {
    rs_session_init_role(s, build, RS_ROLE_SERVER);
}

rs_err rs_session_set_role(rs_session *s, rs_role role) {
    if (!s) {
        return RS_ERR_ARG;
    }
    s->role = role;
    s->isaac_ready = 0;
    return RS_OK;
}

rs_err rs_session_set_build(rs_session *s, const rs_build_info *build) {
    if (!s || !build) {
        return RS_ERR_ARG;
    }
    s->build = build;
    s->ops = rs_proto_for(build);
    s->isaac_ready = 0;
    s->link = RS_LINK_UNKNOWN;
    return RS_OK;
}

rs_err rs_session_set_build_revision(rs_session *s, int revision) {
    const rs_build_info *b = rs_build_lookup(revision);
    if (!b) {
        return RS_ERR_VERSION;
    }
    return rs_session_set_build(s, b);
}

rs_err rs_session_set_build_id(rs_session *s, const char *id_or_rev) {
    const rs_build_info *b = rs_build_lookup_id(id_or_rev);
    if (!b) {
        return RS_ERR_VERSION;
    }
    return rs_session_set_build(s, b);
}

void rs_session_seed_isaac(rs_session *s, const uint32_t client_keys[4]) {
    uint32_t plus[4];
    int i;
    if (!s || !client_keys) {
        return;
    }
    for (i = 0; i < 4; i++) {
        plus[i] = client_keys[i] + 50;
    }
    /* Server decodes with the raw seed and encodes with seed+50.
     * Client encodes with the raw seed and decodes with seed+50. */
    if (s->role == RS_ROLE_CLIENT) {
        rs_isaac_init(&s->isaac_in, plus);
        rs_isaac_init(&s->isaac_out, client_keys);
    } else {
        rs_isaac_init(&s->isaac_in, client_keys);
        rs_isaac_init(&s->isaac_out, plus);
    }
    s->isaac_ready = 1;
    s->encrypt_out = 1;
}

int rs_session_size_c2s(const rs_session *s, int opcode) {
    const int8_t *sizes;
    if (!s || !s->build || opcode < 0 || opcode > 255) {
        return -3;
    }
    sizes = s->build->sizes_c2s ? s->build->sizes_c2s : rs_move_sizes_c2s(s->build->revision);
    if (!sizes) {
        return -3;
    }
    return sizes[opcode];
}

int rs_session_size_s2c(const rs_session *s, int opcode) {
    const int8_t *sizes;
    if (!s || !s->build || opcode < 0 || opcode > 255) {
        return -3;
    }
    sizes = s->build->sizes_s2c ? s->build->sizes_s2c : rs_move_sizes_s2c(s->build->revision);
    if (!sizes) {
        return -3;
    }
    return sizes[opcode];
}

int rs_session_size_in(const rs_session *s, int opcode) {
    if (!s) {
        return -3;
    }
    return s->role == RS_ROLE_CLIENT ? rs_session_size_s2c(s, opcode) : rs_session_size_c2s(s, opcode);
}

int rs_session_size_out(const rs_session *s, int opcode) {
    if (!s) {
        return -3;
    }
    return s->role == RS_ROLE_CLIENT ? rs_session_size_c2s(s, opcode) : rs_session_size_s2c(s, opcode);
}

int rs_session_decode_opcode(rs_session *s, int raw) {
    uint32_t v = (uint32_t)raw & 0xff;
    if (s && s->isaac_ready && s->build && s->build->isaac) {
        v = (v - rs_isaac_next(&s->isaac_in)) & 0xff;
    }
    return (int)v;
}

int rs_session_encode_opcode(rs_session *s, int opcode) {
    uint32_t v = (uint32_t)opcode & 0xff;
    if (s && s->isaac_ready && s->build && s->build->isaac) {
        v = (v + rs_isaac_next(&s->isaac_out)) & 0xff;
    }
    return (int)v;
}
