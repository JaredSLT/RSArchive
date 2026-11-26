#include "both/codec_impl.h"

/*
 * One delegate per encoding. Walk and region stay on their own functions.
 * Skill, varp, inventory, inv update, interfaces, locs, and objs call
 * era_body. Player and NPC call rs_sync. Chat input differs: plain uses
 * the 317 header except RSC (unsupported), rs2 uses the 317 header, and
 * the JS5 objects use wordpack of the text only.
 */
#define RS_CODEC_PAYLOAD                                                                         \
    rs_appearance_encode_317, rs_appearance_decode_317, rs_bridge_skill_encode,                  \
        rs_bridge_skill_decode, rs_bridge_varp_encode, rs_bridge_varp_decode,                    \
        rs_bridge_if_open_encode, rs_bridge_if_open_decode, rs_bridge_if_text_encode,            \
        rs_bridge_if_text_decode, rs_message_encode_317, rs_message_decode_317,                  \
        rs_bridge_inventory_encode, rs_bridge_inventory_decode, rs_zone_base_encode_317,         \
        rs_zone_base_decode_317, rs_bridge_loc_add_encode, rs_bridge_loc_add_decode,             \
        rs_bridge_loc_del_encode, rs_bridge_loc_del_decode, rs_bridge_obj_add_encode,            \
        rs_bridge_obj_add_decode, rs_bridge_obj_del_encode, rs_bridge_obj_del_decode,            \
        rs_spotanim_encode_317, rs_spotanim_decode_317, rs_projectile_encode_317,                 \
        rs_projectile_decode_317, rs_sound_encode_317, rs_sound_decode_317,                       \
        rs_player_index_encode_317, rs_player_index_decode_317, rs_player_option_encode_317,      \
        rs_player_option_decode_317, rs_friend_status_encode_317, rs_friend_status_decode_317,    \
        rs_energy_encode_317, rs_energy_decode_317, rs_weight_encode_317, rs_weight_decode_317

#define RS_CODEC_GAME                                                                            \
    rs_if_model_encode_317, rs_if_model_decode_317, rs_if_npc_head_encode_317,                   \
        rs_if_npc_head_decode_317, rs_if_player_head_encode_317, rs_if_player_head_decode_317,   \
        rs_if_colour_encode_317, rs_if_colour_decode_317, rs_if_hide_encode_317,                 \
        rs_if_hide_decode_317, rs_if_object_encode_317, rs_if_object_decode_317,                 \
        rs_if_anim_encode_317, rs_if_anim_decode_317, rs_if_angle_encode_317,                    \
        rs_if_angle_decode_317, rs_if_pos_encode_317, rs_if_pos_decode_317,                      \
        rs_if_scroll_encode_317, rs_if_scroll_decode_317, rs_bridge_if_sub_encode,              \
        rs_bridge_if_sub_decode, rs_bridge_if_top_encode, rs_bridge_if_top_decode,              \
        rs_inv_clear_encode_317, rs_inv_clear_decode_317, rs_bridge_inv_update_encode,           \
        rs_bridge_inv_update_decode, rs_song_encode_317, rs_song_decode_317,                     \
        rs_jingle_encode_317, rs_jingle_decode_317, rs_sound_area_encode_317,                    \
        rs_sound_area_decode_317, rs_loc_anim_encode_317, rs_loc_anim_decode_317,                \
        rs_cam_shake_encode_317, rs_cam_shake_decode_317, rs_camera_move_encode_317,             \
        rs_camera_move_decode_317, rs_cam_lookat_encode_317, rs_cam_lookat_decode_317,           \
        rs_minimap_encode_317, rs_minimap_decode_317, rs_multizone_encode_317,                   \
        rs_multizone_decode_317, rs_zone_full_encode_317, rs_zone_full_decode_317,               \
        rs_zone_partial_encode_317, rs_zone_partial_decode_317, rs_system_update_encode_317,     \
        rs_system_update_decode_317, rs_chat_settings_encode_317, rs_chat_settings_decode_317,   \
        rs_sidebar_encode_317, rs_sidebar_decode_317, rs_tab_encode_317, rs_tab_decode_317,      \
        rs_walkable_encode_317, rs_walkable_decode_317, rs_last_login_encode_317,                \
        rs_last_login_decode_317, rs_friend_entry_encode_317, rs_friend_entry_decode_317,        \
        rs_ignore_list_encode_317, rs_ignore_list_decode_317, rs_private_message_encode_317,     \
        rs_private_message_decode_317, rs_hint_encode_317, rs_hint_decode_317,                   \
        rs_region_instanced_encode_317, rs_region_instanced_decode_317, rs_command_encode_317,   \
        rs_command_decode_317, rs_private_out_encode_317, rs_private_out_decode_317,             \
        rs_chat_mode_encode_317, rs_chat_mode_decode_317, rs_design_encode_317,                  \
        rs_design_decode_317

const rs_codec_ops rs_codec_plain = {
    "plain",
    rs_walk_encode_late,
    rs_walk_decode_late,
    rs_region_encode_zones,
    rs_region_decode_zones,
    rs_bridge_player_encode,
    rs_bridge_player_decode,
    rs_bridge_npc_empty,
    RS_CODEC_PAYLOAD,
    RS_CODEC_GAME,
    rs_chat_in_encode_plain, rs_chat_in_decode_plain,
};

const rs_codec_ops rs_codec_rs2 = {
    "rs2",
    rs_walk_encode_317,
    rs_walk_decode_317,
    rs_region_encode_rs2,
    rs_region_decode_rs2,
    rs_bridge_player_encode,
    rs_bridge_player_decode,
    rs_bridge_npc_empty,
    RS_CODEC_PAYLOAD,
    RS_CODEC_GAME,
    rs_chat_in_encode_317, rs_chat_in_decode_317,
};

const rs_codec_ops rs_codec_js5_474 = {
    "js5-474",
    rs_walk_encode_late,
    rs_walk_decode_late,
    rs_region_encode_474,
    rs_region_decode_474,
    rs_bridge_player_encode,
    rs_bridge_player_decode,
    rs_bridge_npc_empty,
    RS_CODEC_PAYLOAD,
    RS_CODEC_GAME,
    rs_chat_in_encode_js5, rs_chat_in_decode_js5,
};

const rs_codec_ops rs_codec_js5_508 = {
    "js5-508",
    rs_walk_encode_late,
    rs_walk_decode_late,
    rs_region_encode_508,
    rs_region_decode_508,
    rs_bridge_player_encode,
    rs_bridge_player_decode,
    rs_bridge_npc_empty,
    RS_CODEC_PAYLOAD,
    RS_CODEC_GAME,
    rs_chat_in_encode_js5, rs_chat_in_decode_js5,
};

const rs_codec_ops rs_codec_js5_530 = {
    "js5-530",
    rs_walk_encode_late,
    rs_walk_decode_late,
    rs_region_encode_530,
    rs_region_decode_530,
    rs_bridge_player_encode,
    rs_bridge_player_decode,
    rs_bridge_npc_empty,
    RS_CODEC_PAYLOAD,
    RS_CODEC_GAME,
    rs_chat_in_encode_js5, rs_chat_in_decode_js5,
};

const rs_codec_ops rs_codec_rs3 = {
    "rs3",
    rs_walk_encode_late,
    rs_walk_decode_late,
    rs_region_encode_rs3,
    rs_region_decode_rs3,
    rs_bridge_player_encode,
    rs_bridge_player_decode,
    rs_bridge_npc_empty,
    RS_CODEC_PAYLOAD,
    RS_CODEC_GAME,
    rs_chat_in_encode_js5, rs_chat_in_decode_js5,
};

const rs_codec_ops *rs_codec_for(const rs_build_info *build) {
    int r;
    if (!build) {
        return &rs_codec_rs2;
    }
    if (build->codec) {
        return build->codec;
    }
    r = build->revision;
    if (build->era == RS_ERA_RSC) {
        return &rs_codec_plain;
    }
    if (r >= 790) {
        return &rs_codec_rs3;
    }
    if (r == 530) {
        return &rs_codec_js5_530;
    }
    if (r == 508) {
        return &rs_codec_js5_508;
    }
    if (r >= 468 && r < 508) {
        return &rs_codec_js5_474;
    }
    if (r >= 254 && r <= 377) {
        return &rs_codec_rs2;
    }
    return &rs_codec_plain;
}

const rs_codec_ops *rs_codec_use(const rs_session *s) {
    if (s && s->build && s->build->codec) {
        return s->build->codec;
    }
    return rs_codec_for(s ? s->build : NULL);
}
