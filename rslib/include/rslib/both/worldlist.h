#ifndef RSLIB_WORLDLIST_H
#define RSLIB_WORLDLIST_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { RS_WORLD_MAX = 256, RS_COUNTRY_MAX = 64 };

typedef struct rs_world_country {
    int flag;
    char name[64];
} rs_world_country;

typedef struct rs_world {
    int id;
    int country;
    uint32_t flags;
    int population;
    char activity[64];
    char hostname[128];
} rs_world;

typedef struct rs_worldlist {
    int ncountries;
    rs_world_country countries[RS_COUNTRY_MAX];
    int min_id;
    int max_id;
    int nworlds;
    rs_world worlds[RS_WORLD_MAX];
    uint32_t checksum;
} rs_worldlist;

/* Client request: opcode 255 + checksum (0 to fetch). */
rs_err rs_worldlist_write_request(rs_buf *dst, uint32_t checksum);
rs_err rs_worldlist_parse_request(const uint8_t *data, size_t len, uint32_t *checksum,
                                  size_t *consumed);

/* Server: status byte, then u16 length, then payload. */
rs_err rs_worldlist_encode(rs_buf *dst, const rs_worldlist *list);
rs_err rs_worldlist_decode(rs_buf *src, rs_worldlist *list);
rs_err rs_worldlist_write_reply(rs_buf *dst, const rs_worldlist *list);

#ifdef __cplusplus
}
#endif

#endif
