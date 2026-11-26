#include "internal.h"

#define RS_CUSTOM_MAX 32

/* version = folder group (rsc/rs2/rs3/osrs); release = full date. Together
 * with revision they name the on-disk folder: <version>/<revision>-<release>.
 *
 * Milestone rows (115, 159, 177, 254, 317, 377, 468, 474, 508, 530, 614, 667,
 * 742) keep their original protocol numbers. Year-end rows added later:
 * s2c sizes come from the client when it has a table; c2s is NULL unless the
 * client defines that direction. 578 and 634 keep NULL here; their move
 * layouts hold the constructor size tables. 695 has no size table in the
 * gamepack. 831 and 860 have no sources. login_success_extra is the fixed
 * trailer after rights and flagged (the player-index u16 is included).
 * 443–578 were counted from each login-ok reader. From 614 the login-ok is a
 * length-prefixed record, so that trailer is 0. 695 was not re-read.
 * 481 leaves login_user37 at 0. 695 store/CRC stay the 667 carry.
 * 831 carries 790. 860 carries the old 876 CRC of 43; it has no sources.
 * 530 and 742 CRC fields stay 29 and 40; the writer override emits 28 and 37.
 * Constructor-era CRC counts are the archive-enum loop (910 skips LOADING_SPRITES).
 * RS3 rows keep JS5 framing and packet sizes. Their login writer is the lobby
 * delegate (rs_ops_js5_lobby). Codec pointers name the region/walk layout;
 * NULL would fall through rs_codec_for(). */
static const rs_build_info k_builds[] = {
    {115, 2001, "2001-115", "RSC 115", "rsc", "2001-12-24", RS_ERA_RSC, RS_CACHE_JAG, 43594, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, &rs_codec_plain},
    {159, 2002, "2002-159", "RSC 159", "rsc", "2002-11-25", RS_ERA_RSC, RS_CACHE_JAG, 43594, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, &rs_codec_plain},
    {177, 2003, "2003-177", "RSC 177", "rsc", "2003-10-31", RS_ERA_RSC, RS_CACHE_JAG, 43594, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, &rs_codec_plain},
    {254, 2004, "2004-254", "RS2 254", "rs2", "2004-09-07", RS_ERA_RS2, RS_CACHE_RS2, 43594, 5, 1, 0, 1, 0,
     14, 16, 18, 15, 0, 0, 0, 2, 9, 0, 0, 0, rs_sizes_s2c_254, rs_sizes_c2s_254, NULL, &rs_codec_rs2},
    {282, 2004, "2004-282", "RS2 282", "rs2", "2004-12-21", RS_ERA_RS2, RS_CACHE_RS2, 43594, 5, 1, 0, 1, 0,
     14, 16, 18, 15, 0, 0, 0, 2, 9, 0, 0, 0, rs_sizes_s2c_282, NULL, NULL, &rs_codec_rs2},
    {317, 2005, "2005-317", "RS2 317", "rs2", "2005-06-13", RS_ERA_RS2, RS_CACHE_RS2, 43594, 5, 1, 0, 1, 0,
     14, 16, 18, 15, 0, 0, 0, 2, 9, 0, 0, 0, rs_sizes_s2c_317, rs_sizes_c2s_317, NULL, &rs_codec_rs2},
    {350, 2005, "2005-350", "RS2 350", "rs2", "2005-12-30", RS_ERA_RS2, RS_CACHE_RS2, 43594, 5, 1, 0, 1, 0,
     14, 16, 18, 15, 0, 0, 0, 2, 9, 0, 0, 0, rs_sizes_s2c_350, NULL, NULL, &rs_codec_rs2},
    {377, 2006, "2006-377", "RS2 377", "rs2", "2006-05-02", RS_ERA_RS2, RS_CACHE_RS2, 43594, 5, 1, 0, 1, 0,
     14, 16, 18, 15, 0, 0, 0, 2, 9, 0, 0, 0, rs_sizes_s2c_377, rs_sizes_c2s_377, NULL, &rs_codec_rs2},
    {443, 2006, "2006-443", "RS2 443", "rs2", "2006-12-11", RS_ERA_JS5, RS_CACHE_JS5, 43594, 14, 1, 1, 0, 1,
     14, 16, 18, 15, 5, 0, 1, 4, 14, 0, 1, 3, rs_sizes_s2c_443, NULL, NULL, &rs_codec_plain},
    {468, 2007, "2007-468", "RS2 468", "rs2", "2007-08-08", RS_ERA_JS5, RS_CACHE_JS5, 43594, 16, 1, 1, 0, 1,
     14, 16, 18, 15, 5, 0, 1, 4, 16, 0, 1, 3, rs_sizes_s2c_468, rs_sizes_c2s_468, NULL, &rs_codec_js5_474},
    {474, 2007, "2007-474", "RS2 474", "rs2", "2007-10-19", RS_ERA_JS5, RS_CACHE_JS5, 43594, 16, 1, 1, 0, 1,
     14, 16, 18, 15, 5, 0, 1, 4, 16, 0, 1, 3, rs_sizes_s2c_474, rs_sizes_c2s_474, NULL, &rs_codec_js5_474},
    {481, 2007, "2007-481", "RS2 481", "rs2", "2007-12-11", RS_ERA_JS5, RS_CACHE_JS5, 43594, 16, 1, 1, 0, 1,
     14, 16, 18, 15, 6, 0, 1, 4, 16, 0, 1, 4, rs_sizes_s2c_481, NULL, NULL, &rs_codec_js5_474},
    {508, 2008, "2008-508", "RS2 508", "rs2", "2008-08-05", RS_ERA_JS5, RS_CACHE_JS5, 43594, 29, 1, 1, 0, 1,
     14, 16, 18, 15, 6, 0, 1, 4, 29, 1, 1, 6, rs_sizes_s2c_508, rs_sizes_c2s_508, NULL, &rs_codec_js5_508},
    {525, 2008, "2008-525", "RS2 525", "rs2", "2008-12-15", RS_ERA_JS5, RS_CACHE_JS5, 43594, 28, 1, 1, 0, 1,
     14, 16, 18, 15, 6, 0, 1, 4, 28, 1, 1, 9, rs_sizes_s2c_525, NULL, NULL, &rs_codec_plain},
    {530, 2009, "2009-530", "RS2 530", "rs2", "2009-01-28", RS_ERA_JS5, RS_CACHE_JS5, 43594, 29, 1, 1, 0, 1,
     14, 16, 18, 15, 6, 0, 1, 4, 29, 1, 1, 9, rs_sizes_s2c_530, rs_sizes_c2s_530, NULL, &rs_codec_js5_530},
    {578, 2009, "2009-578", "RS2 578", "rs2", "2009-12-16", RS_ERA_JS5, RS_CACHE_JS5, 43594, 29, 1, 1, 0, 1,
     14, 16, 18, 15, 6, 0, 1, 4, 29, 1, 1, 8, NULL, NULL, NULL, &rs_codec_plain},
    {614, 2010, "2010-614", "RS2 614", "rs2", "2010-07-13", RS_ERA_JS5, RS_CACHE_JS5, 43594, 36, 1, 1, 0, 1,
     14, 16, 18, 15, 6, 1, 1, 4, 34, 1, 0, 0, rs_sizes_s2c_614, rs_sizes_c2s_614, NULL, &rs_codec_plain},
    {634, 2010, "2010-634", "RS2 634", "rs2", "2010-12-10", RS_ERA_JS5, RS_CACHE_JS5, 43594, 37, 1, 1, 0, 1,
     14, 16, 18, 15, 6, 1, 1, 4, 35, 1, 0, 0, NULL, NULL, NULL, &rs_codec_plain},
    {667, 2011, "2011-667", "RS2 667", "rs2", "2011-09-29", RS_ERA_JS5, RS_CACHE_JS5, 43594, 37, 1, 1, 0, 1,
     14, 16, 18, 15, 6, 1, 1, 4, 36, 1, 0, 0, rs_sizes_s2c_667, rs_sizes_c2s_667, NULL, &rs_codec_plain},
    {695, 2011, "2011-695", "RS2 695", "rs2", "2011-12-22", RS_ERA_JS5, RS_CACHE_JS5, 43594, 37, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 37, 1, 1, 12, NULL, NULL, NULL, &rs_codec_plain},
    {742, 2012, "2012-742", "RS2 742", "rs2", "2012-11-07", RS_ERA_JS5, RS_CACHE_JS5, 43594, 40, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 40, 1, 0, 0, rs_sizes_s2c_742, rs_sizes_c2s_742, NULL, &rs_codec_plain},
    {745, 2012, "2012-745", "RS2 745", "rs2", "2012-12-07", RS_ERA_JS5, RS_CACHE_JS5, 43594, 38, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 36, 1, 0, 0, rs_sizes_s2c_745, rs_sizes_c2s_745, NULL, &rs_codec_plain},
    {790, 2013, "2013-790", "RS3 790", "rs3", "2013-12-16", RS_ERA_JS5, RS_CACHE_JS5, 43594, 41, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 36, 1, 0, 0, rs_sizes_s2c_790, rs_sizes_c2s_790, &rs_ops_js5_lobby, &rs_codec_rs3},
    {831, 2014, "2014-831", "RS3 831", "rs3", "2014-12-04", RS_ERA_JS5, RS_CACHE_JS5, 43594, 41, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 37, 1, 1, 12, NULL, NULL, &rs_ops_js5_lobby, &rs_codec_rs3},
    {860, 2015, "2015-860", "RS3 860", "rs3", "2015-12-01", RS_ERA_JS5, RS_CACHE_JS5, 43594, 52, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 43, 1, 1, 12, NULL, NULL, &rs_ops_js5_lobby, &rs_codec_rs3},
    {876, 2016, "2016-876", "RS3 876", "rs3", "2016-11-28", RS_ERA_JS5, RS_CACHE_JS5, 43594, 52, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 42, 1, 0, 0, rs_sizes_s2c_876, rs_sizes_c2s_876, &rs_ops_js5_lobby, &rs_codec_rs3},
    {887, 2017, "2017-887", "RS3 887", "rs3", "2017-11-01", RS_ERA_JS5, RS_CACHE_JS5, 43594, 52, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 42, 1, 0, 0, rs_sizes_s2c_887, rs_sizes_c2s_887, &rs_ops_js5_lobby, &rs_codec_rs3},
    {902, 2018, "2018-902", "RS3 902", "rs3", "2018-12-14", RS_ERA_JS5, RS_CACHE_JS5, 43594, 57, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 41, 1, 0, 0, rs_sizes_s2c_902, rs_sizes_c2s_902, &rs_ops_js5_lobby, &rs_codec_rs3},
    {910, 2019, "2019-910", "RS3 910", "rs3", "2019-10-16", RS_ERA_JS5, RS_CACHE_JS5, 43594, 57, 1, 1, 0, 1,
     14, 16, 18, 15, 7, 1, 1, 4, 41, 1, 0, 0, rs_sizes_s2c_910, rs_sizes_c2s_910, &rs_ops_js5_lobby, &rs_codec_rs3},
};

static rs_build_info g_custom[RS_CUSTOM_MAX];
static int g_ncustom;

struct rs_ctx {
    const rs_build_info *build;
    rs_build_info copy;
    int has_copy;
};

static int same_id(const char *a, const char *b) {
    if (!a || !b) {
        return 0;
    }
    while (*a && *b) {
        char ca = *a++;
        char cb = *b++;
        if (ca >= 'A' && ca <= 'Z') {
            ca = (char)(ca - 'A' + 'a');
        }
        if (cb >= 'A' && cb <= 'Z') {
            cb = (char)(cb - 'A' + 'a');
        }
        if (ca != cb) {
            return 0;
        }
    }
    return *a == *b;
}

const rs_build_info *rs_build_known(size_t *count) {
    if (count) {
        *count = (size_t)RS_NELEM(k_builds);
    }
    return k_builds;
}

static const rs_build_info *lookup_rev(int revision) {
    int i;
    for (i = 0; i < g_ncustom; i++) {
        if (g_custom[i].revision == revision) {
            return &g_custom[i];
        }
    }
    for (i = 0; i < RS_NELEM(k_builds); i++) {
        if (k_builds[i].revision == revision) {
            return &k_builds[i];
        }
    }
    return NULL;
}

const rs_build_info *rs_build_lookup(int revision) {
    return lookup_rev(revision);
}

const rs_build_info *rs_build_lookup_id(const char *id_or_rev) {
    int i;
    char *end;
    long rev;

    if (!id_or_rev || !id_or_rev[0]) {
        return NULL;
    }
    for (i = 0; i < g_ncustom; i++) {
        if (same_id(g_custom[i].id, id_or_rev) || same_id(g_custom[i].name, id_or_rev)) {
            return &g_custom[i];
        }
    }
    for (i = 0; i < RS_NELEM(k_builds); i++) {
        if (same_id(k_builds[i].id, id_or_rev) || same_id(k_builds[i].name, id_or_rev)) {
            return &k_builds[i];
        }
    }
    /* "317" or trailing "-317" */
    rev = strtol(id_or_rev, &end, 10);
    if (end != id_or_rev && *end == '\0') {
        return lookup_rev((int)rev);
    }
    {
        const char *dash = strrchr(id_or_rev, '-');
        if (dash && dash[1]) {
            rev = strtol(dash + 1, &end, 10);
            if (end != dash + 1 && *end == '\0') {
                return lookup_rev((int)rev);
            }
        }
    }
    return NULL;
}

rs_err rs_build_register(const rs_build_info *info) {
    int i;
    if (!info || info->revision <= 0 || !info->id) {
        return RS_ERR_ARG;
    }
    for (i = 0; i < g_ncustom; i++) {
        if (g_custom[i].revision == info->revision) {
            g_custom[i] = *info;
            return RS_OK;
        }
    }
    if (g_ncustom >= RS_CUSTOM_MAX) {
        return RS_ERR_RANGE;
    }
    g_custom[g_ncustom++] = *info;
    return RS_OK;
}

rs_ctx *rs_ctx_create(void) {
    return rs_calloc(1, sizeof(rs_ctx));
}

void rs_ctx_destroy(rs_ctx *ctx) {
    free(ctx);
}

rs_err rs_ctx_set_build(rs_ctx *ctx, const rs_build_info *info) {
    if (!ctx || !info) {
        return RS_ERR_ARG;
    }
    ctx->build = info;
    ctx->has_copy = 0;
    return RS_OK;
}

rs_err rs_ctx_set_build_revision(rs_ctx *ctx, int revision) {
    const rs_build_info *info = rs_build_lookup(revision);
    if (!info) {
        return RS_ERR_VERSION;
    }
    return rs_ctx_set_build(ctx, info);
}

rs_err rs_ctx_set_build_id(rs_ctx *ctx, const char *id_or_rev) {
    const rs_build_info *info = rs_build_lookup_id(id_or_rev);
    if (!info) {
        return RS_ERR_VERSION;
    }
    return rs_ctx_set_build(ctx, info);
}

const rs_build_info *rs_ctx_build(const rs_ctx *ctx) {
    return ctx ? ctx->build : NULL;
}

rs_err rs_ctx_set_build_copy(rs_ctx *ctx, const rs_build_info *info) {
    if (!ctx || !info) {
        return RS_ERR_ARG;
    }
    ctx->copy = *info;
    ctx->has_copy = 1;
    ctx->build = &ctx->copy;
    return RS_OK;
}

int rs_build_archive_relpath(const rs_build_info *info, char *out, unsigned long n) {
    if (!info || !out || n == 0) {
        return -1;
    }
    int w;
    if (info->version && info->release) {
        w = snprintf(out, n, "%s/%d-%s", info->version, info->revision, info->release);
    } else {
        /* Registered-at-runtime build with no folder layout: fall back to id. */
        w = snprintf(out, n, "%s", info->id ? info->id : "");
    }
    return (w < 0 || (unsigned long)w >= n) ? -1 : 0;
}
