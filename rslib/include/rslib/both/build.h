#ifndef RSLIB_BUILD_H
#define RSLIB_BUILD_H

#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Runtime build identity.
 *
 * A process holds one default target on rs_ctx and each connection may override
 * it on rs_session. That lets one server binary serve every client in the
 * archive (and later revisions registered at runtime).
 */

typedef enum rs_era {
    RS_ERA_UNKNOWN = 0,
    RS_ERA_RSC = 1, /* Classic jag archives, 2001–2003 */
    RS_ERA_RS2 = 2, /* main_file_cache.dat + on-demand, 2004–2006 */
    RS_ERA_JS5 = 3  /* main_file_cache.dat2 + JS5, 2007+ */
} rs_era;

typedef enum rs_cache_kind {
    RS_CACHE_UNKNOWN = 0,
    RS_CACHE_JAG = 1,
    RS_CACHE_RS2 = 2,
    RS_CACHE_JS5 = 3
} rs_cache_kind;

struct rs_codec_ops;

typedef struct rs_build_info {
    int revision;                 /* client revision, e.g. 317 */
    int year;                     /* snapshot year */
    const char *id;               /* archive id, e.g. "2005-317" */
    const char *name;             /* short label */
    const char *version;          /* folder group: rsc / rs2 / rs3 / osrs */
    const char *release;          /* full release date, e.g. "2005-06-13" */
    rs_era era;
    rs_cache_kind cache_kind;
    int default_port;             /* game port used by that era's clients */
    int store_archives;           /* idx0..N-1 (JS5 excludes 255) */
    int isaac;                    /* game stream ISAAC after login */
    int xtea_maps;                /* landscape groups XTEA-encrypted */
    int ondemand;                 /* classic 4-byte on-demand protocol */
    int js5;                      /* JS5 update protocol */
    int login_handshake;          /* first-byte opcode, usually 14 */
    int login_new;                /* usually 16 */
    int login_reconnect;          /* usually 18 */
    int update_opcode;            /* 15 for on-demand and JS5 */
    int js5_index_format;         /* 5/6/7; 0 = not JS5 */
    int large_file_ids;           /* 10-byte sector headers when id > 65535 */
    int login_size_wide;          /* 1 = u16 login length (JS5); 0 = u8 (RS2) */
    int login_rev_bytes;          /* 2 (RS2) or 4 (JS5) */
    int login_crc_count;          /* archive CRCs in the login block */
    int login_user37;             /* 1 = username as base37 u64 inside RSA */
    int login_player_index;       /* 1 = u16 player index on login-ok */
    int login_success_extra;      /* extra bytes after rights+flagged */
    const int8_t *sizes_s2c;      /* 256 server->client sizes; NULL if unknown */
    const int8_t *sizes_c2s;      /* 256 client->server sizes; NULL if unknown */
    const struct rs_proto_ops *ops; /* NULL = pick from era (RSC / RS2 / JS5) */
    const struct rs_codec_ops *codec; /* NULL = rs_codec_for() */
} rs_build_info;

/* What this archive can actually do for one registered client. */
typedef struct rs_build_caps {
    int frame_c2s;      /* 1 if c2s game frames can be sized (size table present, or RSC length-prefixed frames) */
    int frame_s2c;
    int named;          /* 1 if at least one logical rs_pkt is bound for this revision */
    int region;         /* 1 if the region rebuild layout was read from that client */
    int player_sync;    /* 1 if rs_sync_player_encode supports this revision */
    int cache_on_disk;  /* 1 if this archive snapshot is known to ship a cache directory */
    int login_checked;  /* 1 if login CRC count and login-ok extras were read from that client, not copied from a neighbor */
} rs_build_caps;

void rs_build_capabilities(const rs_build_info *info, rs_build_caps *out);

typedef struct rs_ctx rs_ctx;

const rs_build_info *rs_build_known(size_t *count);
const rs_build_info *rs_build_lookup(int revision);
const rs_build_info *rs_build_lookup_id(const char *id_or_rev);
rs_err rs_build_register(const rs_build_info *info);

rs_ctx *rs_ctx_create(void);
void rs_ctx_destroy(rs_ctx *ctx);

/* Empty ctx has no target. Set before opening a cache, or per session. */
rs_err rs_ctx_set_build(rs_ctx *ctx, const rs_build_info *info);
rs_err rs_ctx_set_build_revision(rs_ctx *ctx, int revision);
rs_err rs_ctx_set_build_id(rs_ctx *ctx, const char *id_or_rev);
const rs_build_info *rs_ctx_build(const rs_ctx *ctx);

/* Copy-and-patch a build (opcode tables, port, later-era flags). */
rs_err rs_ctx_set_build_copy(rs_ctx *ctx, const rs_build_info *info);

/*
 * On-disk location of a build's cache, relative to the builds/ root:
 * "<version>/<revision>-<release>". Returns 0 on success, -1 if it does not fit.
 * This is the single source of truth for where a build lives on disk.
 */
int rs_build_archive_relpath(const rs_build_info *info, char *out, unsigned long n);

#ifdef __cplusplus
}
#endif

#endif
