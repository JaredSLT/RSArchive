#include "rslib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(void) {
    fprintf(stderr,
            "usage:\n"
            "  rslib-inspect --list-builds\n"
            "  rslib-inspect --list-pkts [--build <rev|id>]\n"
            "  rslib-inspect --build <rev|id> --cache <dir> [--archive N] [--file N]\n");
}

static void list_pkts(const rs_build_info *b) {
    int id;
    printf("logical                  dir  opcode\n");
    for (id = 1; id < RS_PKT_MAX; id++) {
        int op = b ? rs_pkt_opcode(b, (rs_pkt)id) : -1;
        printf("%-24s %-4s %d\n", rs_pkt_name((rs_pkt)id),
               rs_pkt_direction((rs_pkt)id) == RS_DIR_C2S ? "c2s" : "s2c", op);
    }
}

static void list_builds(void) {
    size_t n = 0, i;
    const rs_build_info *b = rs_build_known(&n);
    printf("%-10s %-8s %-6s %-8s %-6s %s\n", "id", "rev", "era", "cache", "port", "features");
    for (i = 0; i < n; i++) {
        const char *era = b[i].era == RS_ERA_RSC ? "rsc" : b[i].era == RS_ERA_RS2 ? "rs2" : "js5";
        const char *ck = b[i].cache_kind == RS_CACHE_JAG
                             ? "jag"
                             : b[i].cache_kind == RS_CACHE_RS2 ? "dat/idx" : "dat2";
        printf("%-10s %-8d %-6s %-8s %-6d", b[i].id, b[i].revision, era, ck, b[i].default_port);
        if (b[i].isaac) {
            fputs(" isaac", stdout);
        }
        if (b[i].ondemand) {
            fputs(" ondemand", stdout);
        }
        if (b[i].js5) {
            fputs(" js5", stdout);
        }
        if (b[i].xtea_maps) {
            fputs(" xtea", stdout);
        }
        if (b[i].sizes_s2c) {
            fputs(" s2c-sizes", stdout);
        }
        if (b[i].sizes_c2s) {
            fputs(" c2s-sizes", stdout);
        }
        putchar('\n');
    }
}

int main(int argc, char **argv) {
    const char *build_id = NULL;
    const char *cache_dir = NULL;
    int archive = -1, file = -1;
    int i;
    rs_ctx *ctx;
    rs_cache *cache;
    const rs_build_info *b;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--list-builds") == 0) {
            list_builds();
            return 0;
        }
        if (strcmp(argv[i], "--list-pkts") == 0) {
            const char *id = NULL;
            int j;
            for (j = 1; j < argc - 1; j++) {
                if (strcmp(argv[j], "--build") == 0) {
                    id = argv[j + 1];
                }
            }
            list_pkts(id ? rs_build_lookup_id(id) : rs_build_lookup(317));
            return 0;
        }
        if (strcmp(argv[i], "--build") == 0 && i + 1 < argc) {
            build_id = argv[++i];
        } else if (strcmp(argv[i], "--cache") == 0 && i + 1 < argc) {
            cache_dir = argv[++i];
        } else if (strcmp(argv[i], "--archive") == 0 && i + 1 < argc) {
            archive = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            file = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage();
            return 0;
        } else {
            usage();
            return 2;
        }
    }
    if (!build_id || !cache_dir) {
        usage();
        return 2;
    }

    ctx = rs_ctx_create();
    if (rs_ctx_set_build_id(ctx, build_id) != RS_OK) {
        fprintf(stderr, "unknown build '%s'\n", build_id);
        rs_ctx_destroy(ctx);
        return 1;
    }
    b = rs_ctx_build(ctx);
    printf("build %s rev %d era %d cache %d port %d\n", b->id, b->revision, (int)b->era,
           (int)b->cache_kind, b->default_port);

    cache = rs_cache_open(ctx, cache_dir);
    if (!cache) {
        fprintf(stderr, "failed to open cache at %s\n", cache_dir);
        rs_ctx_destroy(ctx);
        return 1;
    }
    printf("archives %d\n", rs_cache_archive_count(cache));

    if (b->cache_kind == RS_CACHE_JS5 && archive >= 0 && file < 0) {
        rs_js5_index idx;
        rs_err e = rs_cache_index(cache, archive, &idx);
        if (e != RS_OK) {
            fprintf(stderr, "index: %s\n", rs_strerror(e));
        } else {
            printf("index archive %d format %d version %d groups %d capacity %d crc %08x\n",
                   archive, idx.format, idx.version, idx.ngroups, idx.capacity, idx.crc);
            rs_js5_index_free(&idx);
        }
    } else if (archive >= 0 && file >= 0) {
        rs_blob blob;
        rs_err e = rs_cache_read(cache, archive, file, &blob);
        if (e != RS_OK) {
            fprintf(stderr, "read: %s\n", rs_strerror(e));
        } else {
            printf("archive %d file %d size %zu crc %08x\n", archive, file, blob.size,
                   rs_crc32(blob.data, blob.size));
            rs_blob_free(&blob);
        }
    } else if (b->cache_kind == RS_CACHE_RS2) {
        int a;
        for (a = 1; a <= 8; a++) {
            rs_blob blob;
            if (rs_cache_read(cache, 0, a, &blob) == RS_OK) {
                printf("idx0 file %d size %zu crc %08x\n", a, blob.size,
                       rs_crc32(blob.data, blob.size));
                rs_blob_free(&blob);
            }
        }
    }

    rs_cache_close(cache);
    rs_ctx_destroy(ctx);
    return 0;
}
