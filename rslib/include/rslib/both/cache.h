#ifndef RSLIB_CACHE_H
#define RSLIB_CACHE_H

#include "rslib/both/build.h"
#include "rslib/both/buf.h"
#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    RS_JS5_NONE = 0,
    RS_JS5_BZIP2 = 1,
    RS_JS5_GZIP = 2
};

typedef struct rs_jag_entry {
    int32_t hash;
    int unpacked;
    int packed;
    int offset;
} rs_jag_entry;

typedef struct rs_jag {
    uint8_t *data;
    size_t size;
    int unpacked_whole;
    int file_count;
    rs_jag_entry *files;
} rs_jag;

rs_err rs_jag_open_mem(rs_jag *jag, const uint8_t *bytes, size_t len);
rs_err rs_jag_open_file(rs_jag *jag, const char *path);
void rs_jag_close(rs_jag *jag);
rs_err rs_jag_read_hash(const rs_jag *jag, int32_t hash, rs_blob *out);
rs_err rs_jag_read_name(const rs_jag *jag, const char *name, rs_blob *out);

typedef struct rs_xtea_key {
    int archive;
    int group;
    int32_t name_hash;
    int mapsquare;
    uint32_t key[4];
} rs_xtea_key;

typedef struct rs_js5_group {
    int id;
    int32_t name_hash;
    uint32_t crc;
    int version;
    int nfiles;
    int capacity;
    int *file_ids;       /* nfiles, NULL if packed densely 0..nfiles-1 */
    int32_t *file_names; /* capacity entries or NULL */
    uint8_t whirlpool[64];
    int has_whirlpool;
} rs_js5_group;

typedef struct rs_js5_index {
    int format;
    int version;
    int flags;
    int ngroups;
    int capacity;
    rs_js5_group *groups;
    uint32_t crc;
} rs_js5_index;

rs_err rs_js5_container_unpack(const uint8_t *src, size_t len, rs_blob *out);
rs_err rs_js5_index_parse(const uint8_t *src, size_t len, rs_js5_index *idx);
void rs_js5_index_free(rs_js5_index *idx);
const rs_js5_group *rs_js5_index_group(const rs_js5_index *idx, int group_id);
rs_err rs_js5_group_file(const uint8_t *unpacked, size_t len, const rs_js5_group *g,
                         int file_id, rs_blob *out);

typedef struct rs_cache rs_cache;

rs_cache *rs_cache_open(rs_ctx *ctx, const char *dir);
void rs_cache_close(rs_cache *c);

const rs_build_info *rs_cache_build(const rs_cache *c);
int rs_cache_archive_count(const rs_cache *c);

/*
 * One read for every era. `child` < 0 means the whole object:
 *   RSC  — archive = .jag index, id = file inside it (child ignored)
 *   RS2  — archive = idx, id = file; child >= 0 is a jag member of idx0
 *   JS5  — archive + id = group; child >= 0 is a file inside the group
 */
rs_err rs_cache_get(rs_cache *c, int archive, int id, int child, rs_blob *out);

/*
 * One gzip member, as OnDemand.poll does (Client OnDemand.java GZIPInputStream).
 * Bytes after the member are ignored. Not-gzip is RS_ERR_FORMAT.
 */
rs_err rs_gzip_member(const uint8_t *src, size_t len, rs_blob *out);

/* Raw store bytes (JS5 container, RS2 file, RSC jag member). */
rs_err rs_cache_read(rs_cache *c, int archive, int file, rs_blob *out);
rs_err rs_cache_write(rs_cache *c, int archive, int file, const uint8_t *data, size_t len);

/* RS2 idx0 archives and RSC .jag files. */
rs_err rs_cache_read_jag(rs_cache *c, int archive_or_index, rs_jag *out);

/* JS5: unpack container (and XTEA if a key is loaded for that group). */
rs_err rs_cache_read_group(rs_cache *c, int archive, int group, rs_blob *out);
rs_err rs_cache_read_file(rs_cache *c, int archive, int group, int file, rs_blob *out);
rs_err rs_cache_index(rs_cache *c, int archive, rs_js5_index *out);

rs_err rs_cache_load_keys(rs_cache *c, const char *keys_json_path);
const rs_xtea_key *rs_cache_find_key(const rs_cache *c, int archive, int group);
const rs_xtea_key *rs_cache_find_key_mapsquare(const rs_cache *c, int mapsquare);

/* Open `{archive_root}/{ctx-build-id}/cache`. Does not change the ctx build. */
rs_cache *rs_cache_open_archive(rs_ctx *ctx, const char *archive_root);
/* Like open_archive after rs_ctx_set_build_id(ctx, id_or_rev). */
rs_cache *rs_cache_open_build(rs_ctx *ctx, const char *archive_root, const char *id_or_rev);

/* RSC .jag by filename, or RS2 idx0 archive member by jag name (e.g. "obj.dat"). */
rs_err rs_cache_read_named(rs_cache *c, const char *name, rs_blob *out);

enum { RS_CFG_ACTIONS = 10 };

typedef struct rs_cfg {
    int id;
    char name[64];
    char desc[80];
    int model;
    int size;
    int stackable;
    int cost;
    int members;
    int actions_n;
    char actions[RS_CFG_ACTIONS][24];
} rs_cfg;

rs_err rs_cfg_decode(const uint8_t *data, size_t len, rs_cfg *out);
rs_err rs_cache_cfg_obj(rs_cache *c, int id, rs_cfg *out);
rs_err rs_cache_cfg_npc(rs_cache *c, int id, rs_cfg *out);
rs_err rs_cache_cfg_loc(rs_cache *c, int id, rs_cfg *out);

rs_err rs_cache_map_land(rs_cache *c, int region_x, int region_z, rs_blob *out);
rs_err rs_cache_map_loc(rs_cache *c, int region_x, int region_z, rs_blob *out);

/* One 317 mapsquare (50,50) holds 4505 locs. 4096 dropped that file. */
enum { RS_MAP_LOCS_MAX = 8192 };

typedef struct rs_map_loc {
    int id;
    int x, z, level;
    int shape, rot;
} rs_map_loc;

rs_err rs_map_parse_locs(const uint8_t *data, size_t len, rs_map_loc *out, int cap, int *n);

#ifdef __cplusplus
}
#endif

#endif
