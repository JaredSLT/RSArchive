/* Every archived client folder is a known build, and every known build has
 * that folder. Caches are opened when the snapshot has one. */
#include "rslib.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef RSARCHIVE_ROOT
#error "RSARCHIVE_ROOT must be defined by the build"
#endif

static int fails;
static int folders;

static int is_dir(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int read_file(const char *path, char *buf, size_t n) {
    FILE *f = fopen(path, "rb");
    size_t got;
    if (!f || n < 2) {
        if (f) {
            fclose(f);
        }
        return 0;
    }
    got = fread(buf, 1, n - 1, f);
    fclose(f);
    buf[got] = '\0';
    return got > 0;
}

static int json_int(const char *text, const char *key, int *out) {
    char pat[64];
    const char *p;
    snprintf(pat, sizeof pat, "\"%s\"", key);
    p = strstr(text, pat);
    if (!p) {
        return 0;
    }
    p += strlen(pat);
    while (*p == ' ' || *p == '\t' || *p == ':') {
        p++;
    }
    if (*p < '0' || *p > '9') {
        return 0;
    }
    *out = atoi(p);
    return 1;
}

static int json_str(const char *text, const char *key, char *out, size_t n) {
    char pat[64];
    const char *p;
    size_t i = 0;
    snprintf(pat, sizeof pat, "\"%s\"", key);
    p = strstr(text, pat);
    if (!p) {
        return 0;
    }
    p = strchr(p + strlen(pat), '"');
    if (!p) {
        return 0;
    }
    p++;
    while (*p && *p != '"' && i + 1 < n) {
        out[i++] = *p++;
    }
    out[i] = '\0';
    return *p == '"';
}

static void check_open(const char *id) {
    char root[512];
    rs_ctx *ctx = rs_ctx_create();
    rs_cache *c;
    int count;
    if (snprintf(root, sizeof root, "%s/builds", RSARCHIVE_ROOT) >= (int)sizeof root) {
        fails++;
        rs_ctx_destroy(ctx);
        return;
    }
    if (rs_ctx_set_build_id(ctx, id) != RS_OK) {
        fprintf(stderr, "FAIL set_build %s\n", id);
        fails++;
        rs_ctx_destroy(ctx);
        return;
    }
    c = rs_cache_open_archive(ctx, root);
    if (!c) {
        fprintf(stderr, "FAIL open_archive %s\n", id);
        fails++;
        rs_ctx_destroy(ctx);
        return;
    }
    count = rs_cache_archive_count(c);
    printf("OK  %-12s cache archives=%d\n", id, count);
    if (count < 0) {
        fails++;
    }
    rs_cache_close(c);
    rs_ctx_destroy(ctx);
}

static void scan_family(const char *family) {
    char dirpath[512];
    DIR *d;
    struct dirent *ent;

    snprintf(dirpath, sizeof dirpath, "%s/builds/%s", RSARCHIVE_ROOT, family);
    d = opendir(dirpath);
    if (!d) {
        fprintf(stderr, "FAIL opendir %s\n", dirpath);
        fails++;
        return;
    }
    while ((ent = readdir(d)) != NULL) {
        int rev, year, month, day;
        char folder[64];
        char rel[160];
        char id[32];
        char meta_path[512];
        char cache_path[512];
        char meta[8192];
        char date[32];
        char fam[16];
        char engine[16];
        int meta_rev = 0;
        const rs_build_info *b;

        if (sscanf(ent->d_name, "%d-%d-%d-%d", &rev, &year, &month, &day) != 4) {
            continue;
        }
        snprintf(folder, sizeof folder, "%d-%04d-%02d-%02d", rev, year, month, day);
        if (strcmp(folder, ent->d_name) != 0) {
            continue;
        }
        folders++;
        snprintf(id, sizeof id, "%d", rev);
        b = rs_build_lookup(rev);
        if (!b) {
            fprintf(stderr, "FAIL unregistered %s/%s\n", family, ent->d_name);
            fails++;
            continue;
        }
        if (strcmp(b->version, family) != 0) {
            fprintf(stderr, "FAIL %s version %s\n", id, b->version);
            fails++;
        }
        if (b->year != year) {
            fprintf(stderr, "FAIL %s year %d\n", id, b->year);
            fails++;
        }
        snprintf(date, sizeof date, "%04d-%02d-%02d", year, month, day);
        if (!b->release || strcmp(b->release, date) != 0) {
            fprintf(stderr, "FAIL %s release %s\n", id, b->release ? b->release : "");
            fails++;
        }
        if (rs_build_archive_relpath(b, rel, sizeof rel) != 0) {
            fprintf(stderr, "FAIL %s relpath\n", id);
            fails++;
            continue;
        }
        snprintf(folder, sizeof folder, "%s/%s", family, ent->d_name);
        if (strcmp(rel, folder) != 0) {
            fprintf(stderr, "FAIL %s relpath %s\n", id, rel);
            fails++;
        }
        snprintf(meta_path, sizeof meta_path, "%s/builds/%s/meta.json", RSARCHIVE_ROOT, rel);
        if (!read_file(meta_path, meta, sizeof meta)) {
            fprintf(stderr, "FAIL %s meta.json\n", id);
            fails++;
        } else {
            if (!json_int(meta, "rev", &meta_rev) || meta_rev != rev) {
                fprintf(stderr, "FAIL %s meta rev\n", id);
                fails++;
            }
            if (!json_str(meta, "date", date, sizeof date) || strcmp(date, b->release) != 0) {
                fprintf(stderr, "FAIL %s meta date\n", id);
                fails++;
            }
            if (json_str(meta, "family", fam, sizeof fam)) {
                if (strcmp(fam, family) != 0) {
                    fprintf(stderr, "FAIL %s meta family\n", id);
                    fails++;
                }
            } else if (!json_str(meta, "version", fam, sizeof fam) || strcmp(fam, family) != 0) {
                fprintf(stderr, "FAIL %s meta family\n", id);
                fails++;
            }
            if (json_str(meta, "engine", engine, sizeof engine)) {
                if (strcmp(engine, "js5") == 0 && b->era != RS_ERA_JS5) {
                    fprintf(stderr, "FAIL %s era not js5\n", id);
                    fails++;
                }
                if (strcmp(engine, "old") == 0 && b->era != RS_ERA_RS2) {
                    fprintf(stderr, "FAIL %s era not rs2\n", id);
                    fails++;
                }
            }
        }
        snprintf(cache_path, sizeof cache_path, "%s/builds/%s/cache", RSARCHIVE_ROOT, rel);
        if (is_dir(cache_path)) {
            check_open(id);
        } else {
            rs_ctx *ctx = rs_ctx_create();
            if (rs_ctx_set_build_id(ctx, id) != RS_OK) {
                fprintf(stderr, "FAIL set_build %s\n", id);
                fails++;
            } else {
                printf("OK  %-12s no cache\n", id);
            }
            rs_ctx_destroy(ctx);
        }
    }
    closedir(d);
}

int main(void) {
    size_t n = 0, i;
    const rs_build_info *all;
    static const char *families[] = {"rsc", "rs2", "rs3", "osrs"};
    int fi;

    for (fi = 0; fi < 4; fi++) {
        scan_family(families[fi]);
    }
    if (folders != 29) {
        fprintf(stderr, "FAIL folder count %d\n", folders);
        fails++;
    }
    all = rs_build_known(&n);
    if (n != 29) {
        fprintf(stderr, "FAIL known count %zu\n", n);
        fails++;
    }
    for (i = 0; i < n; i++) {
        char rel[160];
        char path[512];
        if (rs_build_archive_relpath(&all[i], rel, sizeof rel) != 0) {
            fprintf(stderr, "FAIL relpath %d\n", all[i].revision);
            fails++;
            continue;
        }
        snprintf(path, sizeof path, "%s/builds/%s", RSARCHIVE_ROOT, rel);
        if (!is_dir(path)) {
            fprintf(stderr, "FAIL missing folder %s\n", rel);
            fails++;
        }
    }
    printf("%s (%d failed, %d folders)\n", fails ? "FAILED" : "ALL PASSED", fails, folders);
    return fails ? 1 : 0;
}
