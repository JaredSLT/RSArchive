#include "internal.h"
#include "rslib/both/move.h"

/* Milestone rows in src/both/build.c keep numbers read from that client.
 * Year-end rows copy login_success_extra from the old brackets, and 481,
 * 695, 831, and 860 copy a neighbor. Those stay login_checked = 0. */
static int login_was_read(int revision) {
    switch (revision) {
    case 115:
    case 159:
    case 177:
    case 254:
    case 317:
    case 377:
    case 468:
    case 474:
    case 508:
    case 530:
    case 614:
    case 667:
    case 742:
        return 1;
    default:
        return 0;
    }
}

/* Snapshots in this archive with no cache/ directory. */
static int cache_absent(int revision) {
    switch (revision) {
    case 695:
    case 745:
    case 790:
    case 831:
    case 860:
    case 876:
    case 887:
    case 902:
    case 910:
        return 1;
    default:
        return 0;
    }
}

/* Matches the region encoders that were read from that client. */
static int region_read(int revision) {
    switch (revision) {
    case 317:
    case 443:
    case 468:
    case 474:
    case 481:
    case 508:
    case 525:
    case 530:
    case 578:
    case 614:
    case 667:
    case 742:
    case 910:
        return 1;
    default:
        return 0;
    }
}

/* Matches rs_sync_player_encode after kind_of(). */
static int player_sync_read(int revision) {
    return revision == 254 || revision == 317 || revision == 350 || revision == 377;
}

void rs_build_capabilities(const rs_build_info *info, rs_build_caps *out) {
    if (!out) {
        return;
    }
    memset(out, 0, sizeof(*out));
    if (!info) {
        return;
    }
    if (info->era == RS_ERA_RSC) {
        out->frame_c2s = 1;
        out->frame_s2c = 1;
    } else {
        out->frame_c2s = info->sizes_c2s != NULL;
        out->frame_s2c = info->sizes_s2c != NULL;
    }
    out->named = rs_pkt_opcode(info, RS_C2S_WALK) >= 0 || rs_pkt_opcode(info, RS_S2C_REGION) >= 0 ||
                 rs_pkt_opcode(info, RS_S2C_MESSAGE) >= 0;
    out->region = region_read(info->revision);
    out->player_sync = player_sync_read(info->revision);
    {
        const rs_move_layout *mv = rs_move_for(info->revision);
        if (mv && mv->region_encode) {
            out->region = 1;
        }
        if (mv && mv->player_encode) {
            out->player_sync = 1;
        }
        if (info->era != RS_ERA_RSC) {
            if (mv && mv->sizes_c2s) {
                out->frame_c2s = 1;
            }
            if (mv && mv->sizes_s2c) {
                out->frame_s2c = 1;
            }
        }
    }
    out->cache_on_disk = !cache_absent(info->revision);
    out->login_checked = login_was_read(info->revision);
}
