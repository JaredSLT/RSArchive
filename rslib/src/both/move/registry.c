#include "rslib/both/move.h"
#include "rslib/both/packets.h"

#include <string.h>

enum { RS_MOVE_MAX = 40 };

static rs_move_layout g_move[RS_MOVE_MAX];
static int g_nmove;

static void bind_set(int revision, rs_pkt id, int opcode) {
    if (opcode < 0 || opcode > 255) {
        return;
    }
    /* A layout opcode replaces a 317-shaped bind that does not match this client. */
    (void)rs_pkt_bind(revision, id, opcode);
}

void rs_move_register(const rs_move_layout *layout) {
    int i;
    if (!layout || layout->revision <= 0) {
        return;
    }
    for (i = 0; i < g_nmove; i++) {
        if (g_move[i].revision == layout->revision) {
            return;
        }
    }
    if (g_nmove >= RS_MOVE_MAX) {
        return;
    }
    g_move[g_nmove] = *layout;
    bind_set(layout->revision, RS_C2S_WALK, layout->opcode_walk);
    bind_set(layout->revision, RS_C2S_WALK_MINIMAP, layout->opcode_walk_minimap);
    bind_set(layout->revision, RS_S2C_REGION, layout->opcode_region);
    bind_set(layout->revision, RS_S2C_PLAYER_UPDATE, layout->opcode_player);
    bind_set(layout->revision, RS_S2C_PLAYER_INDEX, layout->opcode_player_index);
    g_nmove++;
}

const rs_move_layout *rs_move_for(int revision) {
    int i;
    for (i = 0; i < g_nmove; i++) {
        if (g_move[i].revision == revision) {
            return &g_move[i];
        }
    }
    return NULL;
}

const int8_t *rs_move_sizes_c2s(int revision) {
    const rs_move_layout *m = rs_move_for(revision);
    return m ? m->sizes_c2s : NULL;
}

const int8_t *rs_move_sizes_s2c(int revision) {
    const rs_move_layout *m = rs_move_for(revision);
    return m ? m->sizes_s2c : NULL;
}

static int sync_player_rev(int revision) {
    return revision == 254 || revision == 317 || revision == 350 || revision == 377;
}

static int framed(const rs_build_info *info) {
    int rev;
    if (!info) {
        return 0;
    }
    if (info->era == RS_ERA_RSC) {
        return 1;
    }
    rev = info->revision;
    if (!info->sizes_c2s && !rs_move_sizes_c2s(rev)) {
        return 0;
    }
    if (!info->sizes_s2c && !rs_move_sizes_s2c(rev)) {
        return 0;
    }
    return 1;
}

int rs_move_playable(const rs_build_info *info) {
    rs_session session;
    rs_buf buf;
    rs_region region;
    rs_walk walk;
    rs_move_step step;
    rs_player_update up;
    const rs_move_layout *m;
    rs_err e;
    int rev;
    int world_x;
    int world_z;
    int zone_x;
    int zone_z;
    int local_x;
    int local_z;

    if (!framed(info)) {
        return 0;
    }
    rev = info->revision;
    m = rs_move_for(rev);
    world_x = 3222;
    world_z = 3222;
    if (m && (m->spawn_x != 0 || m->spawn_z != 0)) {
        world_x = m->spawn_x;
        world_z = m->spawn_z;
    }
    zone_x = world_x >> 3;
    zone_z = world_z >> 3;
    local_x = world_x - (zone_x - 6) * 8;
    local_z = world_z - (zone_z - 6) * 8;
    memset(&session, 0, sizeof session);
    rs_session_init_role(&session, info, RS_ROLE_SERVER);
    if (rs_buf_alloc(&buf, 4096) != RS_OK) {
        return 0;
    }
    memset(&region, 0, sizeof region);
    region.zone_x = zone_x;
    region.zone_z = zone_z;
    region.local_x = local_x;
    region.local_z = local_z;
    e = rs_codec_region_encode(&session, &buf, &region);
    if (e != RS_OK || buf.pos == 0) {
        rs_buf_free(&buf);
        return 0;
    }
    buf.pos = 0;
    e = RS_ERR_UNSUPPORTED;
    if (m && m->player_encode) {
        memset(&step, 0, sizeof step);
        step.teleport = 1;
        if (info->era == RS_ERA_RSC) {
            step.local_x = world_x;
            step.local_z = world_z;
        } else {
            step.local_x = local_x;
            step.local_z = local_z;
        }
        step.dir1 = -1;
        step.dir2 = -1;
        step.appearance = 1;
        e = m->player_encode(&session, &buf, &step);
    } else if (sync_player_rev(rev)) {
        memset(&up, 0, sizeof up);
        up.teleport = 1;
        up.local_x = local_x;
        up.local_z = local_z;
        up.include_appearance = 1;
        up.mask = RS_PLAYER_APPEARANCE;
        e = rs_codec_player_update_encode(&session, &buf, &up);
    }
    if (e != RS_OK || buf.pos == 0) {
        rs_buf_free(&buf);
        return 0;
    }
    buf.pos = 0;
    memset(&walk, 0, sizeof walk);
    walk.dest_x = world_x + 1;
    walk.dest_z = world_z;
    e = rs_codec_walk_encode(&session, &buf, &walk);
    {
        int ok = e == RS_OK && buf.pos > 0;
        rs_buf_free(&buf);
        return ok;
    }
}
