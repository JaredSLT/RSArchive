#include "both/cache_impl.h"

static rs_err cfg_from_dat(const uint8_t *dat, size_t dat_len, const uint8_t *idx, size_t idx_len,
                           int id, rs_cfg *out) {
    rs_buf d, x;
    int count, off, next, opcode, i;
    if (!dat || !idx || !out || id < 0) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    rs_buf_wrap(&x, (uint8_t *)idx, idx_len);
    if (rs_buf_remaining(&x) < 2) {
        return RS_ERR_FORMAT;
    }
    /* ObjType/NpcType/LocType.unpack: count, then u16 sizes. Offsets start at 2. */
    count = rs_g2(&x);
    if (id >= count) {
        return RS_ERR_RANGE;
    }
    if (rs_buf_remaining(&x) < (size_t)(id + 1) * 2u) {
        return RS_ERR_FORMAT;
    }
    off = 2;
    for (i = 0; i < id; i++) {
        int sz = rs_g2(&x);
        if (off < 0 || (size_t)off > dat_len || (size_t)sz > dat_len - (size_t)off) {
            return RS_ERR_FORMAT;
        }
        off += sz;
    }
    {
        int sz = rs_g2(&x);
        if (off < 0 || (size_t)off > dat_len || (size_t)sz > dat_len - (size_t)off) {
            return RS_ERR_FORMAT;
        }
        next = off + sz;
    }
    rs_buf_wrap(&d, (uint8_t *)dat + off, (size_t)(next - off));
    out->id = id;
    out->size = 1;
    for (;;) {
        opcode = rs_g1(&d);
        if (opcode == 0) {
            break;
        }
        if (opcode == 1) {
            out->model = rs_g2(&d);
        } else if (opcode == 2) {
            rs_gjstr(&d, out->name, sizeof(out->name));
        } else if (opcode == 3) {
            rs_gjstr(&d, out->desc, sizeof(out->desc));
        } else if (opcode == 12) {
            out->size = rs_g1(&d);
        } else if (opcode == 11) {
            out->stackable = 1;
        } else if (opcode == 12 + 4) { /* 16 members on some types */
            out->members = 1;
        } else if (opcode == 16) {
            out->members = 1;
        } else if (opcode >= 30 && opcode < 35) {
            int slot = opcode - 30;
            if (slot < RS_CFG_ACTIONS) {
                rs_gjstr(&d, out->actions[slot], sizeof(out->actions[slot]));
                out->actions_n++;
            }
        } else if (opcode >= 150 && opcode < 155) {
            int slot = opcode - 150;
            if (slot < RS_CFG_ACTIONS) {
                rs_gjstr(&d, out->actions[slot], sizeof(out->actions[slot]));
                out->actions_n++;
            }
        } else if (opcode == 24 || opcode == 78 || opcode == 79) {
            (void)rs_g2(&d);
        } else if (opcode == 97 || opcode == 98) {
            (void)rs_g2(&d);
        } else if (opcode == 4 || opcode == 5 || opcode == 6 || opcode == 7 || opcode == 8) {
            (void)rs_g2(&d);
        } else if (opcode == 10) {
            (void)rs_g2(&d);
        } else if (opcode == 23 || opcode == 25) {
            (void)rs_g2(&d);
        } else if (opcode == 40) {
            int n = rs_g1(&d), i;
            for (i = 0; i < n; i++) {
                (void)rs_g2(&d);
                (void)rs_g2(&d);
            }
        } else {
            /* skip unknown by consuming remaining if we cannot size it */
            break;
        }
        if (d.pos >= d.cap) {
            break;
        }
    }
    return RS_OK;
}

rs_err rs_cfg_decode(const uint8_t *data, size_t len, rs_cfg *out) {
    rs_buf b;
    int opcode;
    if (!data || !out) {
        return RS_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    rs_buf_wrap(&b, (uint8_t *)data, len);
    out->size = 1;
    while (rs_buf_remaining(&b) > 0) {
        opcode = rs_g1(&b);
        if (opcode == 0) {
            break;
        }
        if (opcode == 2) {
            rs_gjstr(&b, out->name, sizeof(out->name));
        } else if (opcode == 1) {
            out->model = rs_g2(&b);
        } else if (opcode == 12) {
            out->size = rs_g1(&b);
        } else {
            break;
        }
    }
    return RS_OK;
}

static rs_err cache_cfg_pair(rs_cache *c, const char *dat, const char *idx, int id, rs_cfg *out) {
    rs_blob d, i;
    rs_err e;
    e = rs_cache_read_named(c, dat, &d);
    if (e != RS_OK) {
        return e;
    }
    e = rs_cache_read_named(c, idx, &i);
    if (e != RS_OK) {
        rs_blob_free(&d);
        return e;
    }
    e = cfg_from_dat(d.data, d.size, i.data, i.size, id, out);
    rs_blob_free(&d);
    rs_blob_free(&i);
    return e;
}

rs_err rs_cache_cfg_obj(rs_cache *c, int id, rs_cfg *out) {
    rs_blob file;
    if (!c) {
        return RS_ERR_ARG;
    }
    if (c->build && c->build->cache_kind == RS_CACHE_JS5) {
        if (rs_cache_read_file(c, 2, 10, id, &file) == RS_OK ||
            rs_cache_get(c, 2, 10, id, &file) == RS_OK) {
            rs_err e = rs_cfg_decode(file.data, file.size, out);
            if (out) {
                out->id = id;
            }
            rs_blob_free(&file);
            return e;
        }
    }
    return cache_cfg_pair(c, "obj.dat", "obj.idx", id, out);
}

rs_err rs_cache_cfg_npc(rs_cache *c, int id, rs_cfg *out) {
    return cache_cfg_pair(c, "npc.dat", "npc.idx", id, out);
}

rs_err rs_cache_cfg_loc(rs_cache *c, int id, rs_cfg *out) {
    return cache_cfg_pair(c, "loc.dat", "loc.idx", id, out);
}

static int map_file_id(int rx, int rz) {
    return (rx * 256) + rz;
}

/* OnDemand map_index is 7 bytes per row on 254, 282, 317, 350, and 377. */
static int rs2_map_index(const rs_cache *c) {
    int rev;
    if (!c || !c->build) {
        return 0;
    }
    rev = c->build->revision;
    return rev == 254 || rev == 282 || rev == 317 || rev == 350 || rev == 377;
}

/*
 * OnDemand.getMapFile: key is (map_x << 8) + map_z. Land is the first file id,
 * loc the second. request(3, id) reads fileStreams[4], which is idx4.
 * poll() gunzips the member and leaves the trailing bytes.
 */
static rs_err map_from_index(rs_cache *c, int region_x, int region_z, int which, rs_blob *out) {
    rs_blob index, raw;
    rs_buf b;
    int key, n, i, file;
    rs_err e;
    if (region_x < 0 || region_z < 0 || region_x > 255 || region_z > 255) {
        return RS_ERR_RANGE;
    }
    memset(&index, 0, sizeof index);
    e = rs_cache_read_named(c, "map_index", &index);
    if (e != RS_OK) {
        return e;
    }
    if (index.size == 0 || index.size % 7 != 0) {
        rs_blob_free(&index);
        return RS_ERR_FORMAT;
    }
    key = (region_x << 8) + region_z;
    file = -1;
    rs_buf_wrap(&b, index.data, index.size);
    n = (int)(index.size / 7);
    for (i = 0; i < n; i++) {
        int got = rs_g2(&b);
        int land = rs_g2(&b);
        int loc = rs_g2(&b);
        (void)rs_g1(&b);
        if (got == key) {
            file = which == 0 ? land : loc;
            break;
        }
    }
    rs_blob_free(&index);
    if (file < 0) {
        return RS_ERR_NOT_FOUND;
    }
    memset(&raw, 0, sizeof raw);
    e = rs_cache_get(c, 4, file, -1, &raw);
    if (e != RS_OK) {
        return e;
    }
    if (raw.size >= 2 && raw.data && raw.data[0] == 0x1f && raw.data[1] == 0x8b) {
        e = rs_gzip_member(raw.data, raw.size, out);
        rs_blob_free(&raw);
        return e;
    }
    *out = raw;
    return RS_OK;
}

rs_err rs_cache_map_land(rs_cache *c, int region_x, int region_z, rs_blob *out) {
    char name[32];
    rs_err e;
    if (!c || !out) {
        return RS_ERR_ARG;
    }
    if (c->build && c->build->cache_kind == RS_CACHE_JS5) {
        return rs_cache_get(c, 5, map_file_id(region_x, region_z), 0, out);
    }
    snprintf(name, sizeof(name), "m%d_%d", region_x, region_z);
    e = rs_cache_read_named(c, name, out);
    /* Named members win. The 7-byte map_index is the RS2 OnDemand fallback. */
    if (e == RS_OK || !rs2_map_index(c)) {
        return e;
    }
    return map_from_index(c, region_x, region_z, 0, out);
}

rs_err rs_cache_map_loc(rs_cache *c, int region_x, int region_z, rs_blob *out) {
    char name[32];
    rs_err e;
    if (!c || !out) {
        return RS_ERR_ARG;
    }
    if (c->build && c->build->cache_kind == RS_CACHE_JS5) {
        return rs_cache_get(c, 5, map_file_id(region_x, region_z), 1, out);
    }
    snprintf(name, sizeof(name), "l%d_%d", region_x, region_z);
    e = rs_cache_read_named(c, name, out);
    if (e == RS_OK || !rs2_map_index(c)) {
        return e;
    }
    return map_from_index(c, region_x, region_z, 1, out);
}

rs_err rs_map_parse_locs(const uint8_t *data, size_t len, rs_map_loc *out, int cap, int *n) {
    rs_buf b;
    int id = -1, count = 0;
    if (!data || !out || cap <= 0) {
        return RS_ERR_ARG;
    }
    rs_buf_wrap(&b, (uint8_t *)data, len);
    for (;;) {
        int delta = rs_gsmarts(&b);
        if (delta == 0) {
            break;
        }
        id += delta;
        for (;;) {
            int pos = rs_gsmarts(&b);
            int info;
            if (pos == 0) {
                break;
            }
            pos--;
            info = rs_g1(&b);
            if (count < cap) {
                out[count].id = id;
                out[count].level = pos >> 12;
                out[count].x = (pos >> 6) & 63;
                out[count].z = pos & 63;
                out[count].shape = info >> 2;
                out[count].rot = info & 3;
            }
            count++;
            if (b.pos >= b.cap) {
                break;
            }
        }
        if (b.pos >= b.cap) {
            break;
        }
    }
    if (n) {
        *n = count;
    }
    return RS_OK;
}
