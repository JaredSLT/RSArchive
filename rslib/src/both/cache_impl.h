#ifndef RSLIB_CACHE_IMPL_H
#define RSLIB_CACHE_IMPL_H

#include "internal.h"

#include <dirent.h>
#include <sys/stat.h>

rs_err rs_store_open(void **out, const char *dat, const char *idx, int archive_id, int large_ids);
void rs_store_close(void *s);
rs_err rs_store_read(void *s, int file, rs_blob *out);

#define RS_MAX_STORES 256

struct rs_cache {
    const rs_build_info *build;
    char *dir;
    void *stores[RS_MAX_STORES];
    int nstores;
    void *master;
    rs_jag *jags;
    char **jag_names;
    int njags;
    rs_xtea_key *keys;
    int nkeys;
    rs_js5_index *indices;
    int *index_loaded;
};

int rs_cache_path_join(char *dst, size_t cap, const char *dir, const char *name);
int rs_cache_file_exists(const char *path);
rs_err rs_cache_load_jags(rs_cache *c);
rs_err rs_cache_parse_keys(rs_cache *c, const char *json, size_t len);

#endif
