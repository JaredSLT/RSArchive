#ifndef RSLIB_MOVE_H
#define RSLIB_MOVE_H

#include "rslib/both/codec.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Login-and-walk layouts read from one archived client.
 * A NULL function keeps the codec that build already uses.
 * A size table is 256 entries: fixed length, -1 variable byte, -2 variable
 * short. Copy the client's array. Do not fill a table the client does not have.
 *
 * Direction 0..7 is NW, N, NE, W, E, SW, S, SE.
 * dx: -1, 0, 1, -1, 1, -1, 0, 1
 * dz:  1, 1, 1,  0, 0, -1,-1,-1
 * teleport 1 writes scene local_x/local_z. teleport 0 applies dir1 then dir2
 * to the previous scene tile. Return RS_ERR_UNSUPPORTED when that form was
 * not read. spawn_x and spawn_z of 0 mean world tile 3222,3222.
 */
typedef struct rs_move_step {
    int teleport;
    int plane;
    int local_x;
    int local_z;
    int dir1;
    int dir2;
    int appearance;
    rs_appearance look;
} rs_move_step;

typedef struct rs_move_layout {
    int revision;
    const int8_t *sizes_c2s;
    const int8_t *sizes_s2c;
    int opcode_walk;
    int opcode_walk_minimap;
    int opcode_region;
    int opcode_player;
    int opcode_player_index;
    int spawn_x;
    int spawn_z;
    rs_err (*walk_encode)(const rs_session *s, rs_buf *dst, const rs_walk *w);
    rs_err (*walk_decode)(const rs_session *s, rs_buf *src, rs_walk *w);
    rs_err (*region_encode)(const rs_session *s, rs_buf *dst, const rs_region *v);
    rs_err (*region_decode)(const rs_session *s, rs_buf *src, rs_region *v);
    rs_err (*player_encode)(const rs_session *s, rs_buf *dst, const rs_move_step *step);
    rs_err (*player_decode)(const rs_session *s, rs_buf *src, rs_move_step *step);
} rs_move_layout;

void rs_move_register(const rs_move_layout *layout);
const rs_move_layout *rs_move_for(int revision);
const int8_t *rs_move_sizes_c2s(int revision);
const int8_t *rs_move_sizes_s2c(int revision);

/* 1 when a session can frame, rebuild a region, place the local player, and encode a walk. */
int rs_move_playable(const rs_build_info *info);

#ifdef __cplusplus
}
#endif

#endif
