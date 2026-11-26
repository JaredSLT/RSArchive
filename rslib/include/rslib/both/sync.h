#ifndef RSLIB_SYNC_H
#define RSLIB_SYNC_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Viewport delta for one tick. x/z are scene tiles in the client's pathTileX/pathTileZ
 * space (the same numbers a 317 local teleport carries, 0..127). Other players and
 * NPCs are placed as a signed 5-bit delta from the local player's tile after this
 * packet's local move. Plane is on the wire only for that local teleport; everyone
 * else copies it. dir 0 is northwest, not "no step"; unused dirs are -1.
 *
 * mask bits are the body bits of that revision's reader. The extension signal
 * (254 players 0x80, 317 players 0x40, 350 players 0x04, 377 players 0x20)
 * is not a body bit. 254 NPCs have no signal; bit 0x80 is face-tile.
 */

typedef struct rs_sync_view rs_sync_view;

enum {
    RS_SYNC_P254_APPEARANCE = 1,
    RS_SYNC_P254_ANIM = 2,
    RS_SYNC_P254_FACE_ENTITY = 4,
    RS_SYNC_P254_CHAT = 8,
    RS_SYNC_P254_HIT = 16,
    RS_SYNC_P254_FACE_TILE = 32,
    RS_SYNC_P254_PUBLIC = 64,
    RS_SYNC_P254_SPOTANIM = 256,
    RS_SYNC_P254_FORCE = 512,
    RS_SYNC_P254_HIT2 = 1024
};

enum {
    RS_SYNC_P317_FACE_ENTITY = 1,
    RS_SYNC_P317_FACE_TILE = 2,
    RS_SYNC_P317_CHAT = 4,
    RS_SYNC_P317_ANIM = 8,
    RS_SYNC_P317_APPEARANCE = 16,
    RS_SYNC_P317_HIT = 32,
    RS_SYNC_P317_PUBLIC = 128,
    RS_SYNC_P317_SPOTANIM = 256,
    RS_SYNC_P317_HIT2 = 512,
    RS_SYNC_P317_FORCE = 1024
};

enum {
    RS_SYNC_P350_ANIM = 1,
    RS_SYNC_P350_FACE_TILE = 2,
    RS_SYNC_P350_CHAT = 8,
    RS_SYNC_P350_HIT2 = 16,
    RS_SYNC_P350_APPEARANCE = 32,
    RS_SYNC_P350_PUBLIC = 64,
    RS_SYNC_P350_FACE_ENTITY = 128,
    RS_SYNC_P350_SPOTANIM = 256,
    RS_SYNC_P350_HIT = 512,
    RS_SYNC_P350_FORCE = 1024
};

enum {
    RS_SYNC_P377_FACE_ENTITY = 1,
    RS_SYNC_P377_FACE_TILE = 2,
    RS_SYNC_P377_APPEARANCE = 4,
    RS_SYNC_P377_ANIM = 8,
    RS_SYNC_P377_CHAT = 16,
    RS_SYNC_P377_PUBLIC = 64,
    RS_SYNC_P377_HIT2 = 128,
    RS_SYNC_P377_FORCE = 256,
    RS_SYNC_P377_SPOTANIM = 512,
    RS_SYNC_P377_HIT = 1024
};

enum {
    RS_SYNC_N254_HIT = 1,
    RS_SYNC_N254_ANIM = 2,
    RS_SYNC_N254_FACE_ENTITY = 4,
    RS_SYNC_N254_CHAT = 8,
    RS_SYNC_N254_HIT2 = 16,
    RS_SYNC_N254_TYPE = 32,
    RS_SYNC_N254_SPOTANIM = 64,
    RS_SYNC_N254_FACE_TILE = 128
};

enum {
    RS_SYNC_N317_CHAT = 1,
    RS_SYNC_N317_TYPE = 2,
    RS_SYNC_N317_FACE_TILE = 4,
    RS_SYNC_N317_HIT = 8,
    RS_SYNC_N317_ANIM = 16,
    RS_SYNC_N317_FACE_ENTITY = 32,
    RS_SYNC_N317_HIT2 = 64,
    RS_SYNC_N317_SPOTANIM = 128
};

enum {
    RS_SYNC_N350_HIT = 1,
    RS_SYNC_N350_HIT2 = 2,
    RS_SYNC_N350_FACE_ENTITY = 4,
    RS_SYNC_N350_ANIM = 8,
    RS_SYNC_N350_TYPE = 16,
    RS_SYNC_N350_FACE_TILE = 32,
    RS_SYNC_N350_CHAT = 64,
    RS_SYNC_N350_SPOTANIM = 128
};

enum {
    RS_SYNC_N377_TYPE = 1,
    RS_SYNC_N377_ANIM = 2,
    RS_SYNC_N377_SPOTANIM = 4,
    RS_SYNC_N377_FACE_TILE = 8,
    RS_SYNC_N377_HIT2 = 16,
    RS_SYNC_N377_CHAT = 32,
    RS_SYNC_N377_FACE_ENTITY = 64,
    RS_SYNC_N377_HIT = 128
};

typedef struct rs_sync_player {
    int active;
    int x, z, plane;
    int run;
    int dir1, dir2;
    int warp;
    int mask;
    int gender, headicon, headicon2;
    int npc_id;
    int combat_level, skill_level;
    int styles[12];
    int colours[5];
    int anims[7];
    char name[13];
    char chat[160];
    int chat_colour, chat_effect, chat_rights;
    uint8_t packed[256];
    int packed_len;
    int hit_damage, hit_type, hit_hp, hit_max;
    int hit2_damage, hit2_type, hit2_hp, hit2_max;
    int seq, seq_delay;
    int face_entity;
    int face_x, face_z; /* first word, then second, in reader order */
    int spot_id, spot_height, spot_delay;
    int force_sx, force_sz, force_ex, force_ez;
    int force_start, force_end, force_dir; /* cycle fields are wire offsets */
} rs_sync_player;

typedef struct rs_sync_npc {
    int active;
    int type;
    int x, z, plane;
    int run;
    int dir1, dir2;
    int warp;
    int mask;
    char chat[160];
    int hit_damage, hit_type, hit_hp, hit_max;
    int hit2_damage, hit2_type, hit2_hp, hit2_max;
    int seq, seq_delay;
    int face_entity;
    int face_x, face_z;
    int spot_id, spot_height, spot_delay;
} rs_sync_npc;

rs_sync_view *rs_sync_view_create(int revision);
void rs_sync_view_destroy(rs_sync_view *v);
rs_err rs_sync_set_local(rs_sync_view *v, int player_index);
rs_err rs_sync_player_set(rs_sync_view *v, int index, const rs_sync_player *p);
rs_err rs_sync_player_get(const rs_sync_view *v, int index, rs_sync_player *out);
rs_err rs_sync_player_clear(rs_sync_view *v, int index);
rs_err rs_sync_npc_set(rs_sync_view *v, int index, int type, int x, int z, int plane, int warp);
rs_err rs_sync_npc_put(rs_sync_view *v, int index, const rs_sync_npc *n);
rs_err rs_sync_npc_get(const rs_sync_view *v, int index, rs_sync_npc *out);
rs_err rs_sync_npc_clear(rs_sync_view *v, int index);
rs_err rs_sync_player_encode(rs_sync_view *v, rs_buf *dst);
rs_err rs_sync_npc_encode(rs_sync_view *v, rs_buf *dst);
rs_err rs_sync_player_decode(rs_sync_view *v, rs_buf *src);
rs_err rs_sync_npc_decode(rs_sync_view *v, rs_buf *src);

#ifdef __cplusplus
}
#endif

#endif
