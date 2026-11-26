#include "rslib/server/server.h"

#define RS_CONN_SEND(fn, pkt, enc, ty) \
rs_err fn(rs_conn *c, const ty *v) { \
    uint8_t raw[4096]; \
    rs_buf b; \
    rs_err e; \
    if (!c || !v) return RS_ERR_ARG; \
    rs_buf_wrap(&b, raw, sizeof(raw)); \
    e = enc(rs_conn_session(c), &b, v); \
    if (e != RS_OK) return e; \
    return rs_conn_send_pkt(c, pkt, b.data, b.pos); \
}

RS_CONN_SEND(rs_conn_send_player_update, RS_S2C_PLAYER_UPDATE, rs_codec_player_update_encode, rs_player_update)
RS_CONN_SEND(rs_conn_send_region, RS_S2C_REGION, rs_codec_region_encode, rs_region)
RS_CONN_SEND(rs_conn_send_region_instanced, RS_S2C_REGION_INSTANCED, rs_codec_region_instanced_encode, rs_region_instanced)
RS_CONN_SEND(rs_conn_send_message, RS_S2C_MESSAGE, rs_codec_message_encode, rs_message)
RS_CONN_SEND(rs_conn_send_skill, RS_S2C_SKILL, rs_codec_skill_encode, rs_skill)
RS_CONN_SEND(rs_conn_send_energy, RS_S2C_ENERGY, rs_codec_energy_encode, rs_energy)
RS_CONN_SEND(rs_conn_send_weight, RS_S2C_WEIGHT, rs_codec_weight_encode, rs_weight)
RS_CONN_SEND(rs_conn_send_if_open, RS_S2C_IF_OPEN, rs_codec_if_open_encode, rs_if_open)
RS_CONN_SEND(rs_conn_send_if_text, RS_S2C_IF_TEXT, rs_codec_if_text_encode, rs_if_text)
RS_CONN_SEND(rs_conn_send_if_top, RS_S2C_IF_OPENTOP, rs_codec_if_top_encode, rs_if_top)
RS_CONN_SEND(rs_conn_send_if_sub, RS_S2C_IF_OPENSUB, rs_codec_if_sub_encode, rs_if_sub)
RS_CONN_SEND(rs_conn_send_inventory, RS_S2C_INVENTORY, rs_codec_inventory_encode, rs_inventory)
RS_CONN_SEND(rs_conn_send_inv_update, RS_S2C_INVENTORY_UPDATE, rs_codec_inv_update_encode, rs_inv_update)
RS_CONN_SEND(rs_conn_send_inv_clear, RS_S2C_INV_CLEAR, rs_codec_inv_clear_encode, rs_inv_clear)
RS_CONN_SEND(rs_conn_send_system_update, RS_S2C_SYSTEM_UPDATE, rs_codec_system_update_encode, rs_system_update)
RS_CONN_SEND(rs_conn_send_minimap, RS_S2C_MINIMAP, rs_codec_minimap_encode, rs_minimap)
RS_CONN_SEND(rs_conn_send_hint, RS_S2C_HINT, rs_codec_hint_encode, rs_hint)
RS_CONN_SEND(rs_conn_send_camera_move, RS_S2C_CAMERA_MOVE, rs_codec_camera_move_encode, rs_camera)
RS_CONN_SEND(rs_conn_send_cam_lookat, RS_S2C_CAM_LOOKAT, rs_codec_cam_lookat_encode, rs_camera)
RS_CONN_SEND(rs_conn_send_cam_shake, RS_S2C_CAM_SHAKE, rs_codec_cam_shake_encode, rs_cam_shake)
RS_CONN_SEND(rs_conn_send_player_option, RS_S2C_PLAYER_OPTION, rs_codec_player_option_encode, rs_player_option)
RS_CONN_SEND(rs_conn_send_chat_settings, RS_S2C_CHAT_SETTINGS, rs_codec_chat_settings_encode, rs_chat_settings)
RS_CONN_SEND(rs_conn_send_friend_status, RS_S2C_FRIEND_STATUS, rs_codec_friend_status_encode, rs_friend_status)
RS_CONN_SEND(rs_conn_send_friend_entry, RS_S2C_FRIEND_LIST, rs_codec_friend_entry_encode, rs_friend_entry)
RS_CONN_SEND(rs_conn_send_ignore_list, RS_S2C_IGNORE_LIST, rs_codec_ignore_list_encode, rs_ignore_list)
RS_CONN_SEND(rs_conn_send_private_message, RS_S2C_PRIVATE_MESSAGE, rs_codec_private_message_encode, rs_private_message)
RS_CONN_SEND(rs_conn_send_projectile, RS_S2C_PROJECTILE, rs_codec_projectile_encode, rs_projectile)
RS_CONN_SEND(rs_conn_send_song, RS_S2C_SONG, rs_codec_song_encode, rs_song)
RS_CONN_SEND(rs_conn_send_jingle, RS_S2C_MIDI_JINGLE, rs_codec_jingle_encode, rs_jingle)
RS_CONN_SEND(rs_conn_send_sound, RS_S2C_SOUND, rs_codec_sound_encode, rs_sound)
RS_CONN_SEND(rs_conn_send_sound_area, RS_S2C_SOUND_AREA, rs_codec_sound_area_encode, rs_sound_area)
RS_CONN_SEND(rs_conn_send_player_index, RS_S2C_PLAYER_INDEX, rs_codec_player_index_encode, rs_player_index)
RS_CONN_SEND(rs_conn_send_walkable, RS_S2C_WALKABLE_INTERFACE, rs_codec_walkable_encode, rs_walkable)
RS_CONN_SEND(rs_conn_send_multizone, RS_S2C_MULTIZONE, rs_codec_multizone_encode, rs_multizone)
RS_CONN_SEND(rs_conn_send_obj_add, RS_S2C_OBJ_ADD, rs_codec_obj_add_encode, rs_scene_obj)
RS_CONN_SEND(rs_conn_send_obj_del, RS_S2C_OBJ_DEL, rs_codec_obj_del_encode, rs_scene_obj)
RS_CONN_SEND(rs_conn_send_loc_add, RS_S2C_LOC_ADD, rs_codec_loc_add_encode, rs_scene_loc)
RS_CONN_SEND(rs_conn_send_loc_del, RS_S2C_LOC_DEL, rs_codec_loc_del_encode, rs_scene_loc)
RS_CONN_SEND(rs_conn_send_loc_anim, RS_S2C_LOC_ANIM, rs_codec_loc_anim_encode, rs_loc_anim)
RS_CONN_SEND(rs_conn_send_zone_full, RS_S2C_ZONE_FULL, rs_codec_zone_full_encode, rs_zone_full)
RS_CONN_SEND(rs_conn_send_zone_partial, RS_S2C_ZONE_PARTIAL, rs_codec_zone_partial_encode, rs_zone_partial)
RS_CONN_SEND(rs_conn_send_zone_base, RS_S2C_ZONE_BASE, rs_codec_zone_base_encode, rs_zone_base)
RS_CONN_SEND(rs_conn_send_if_anim, RS_S2C_IF_SETANIM, rs_codec_if_anim_encode, rs_if_anim)
RS_CONN_SEND(rs_conn_send_if_hide, RS_S2C_IF_SETHIDE, rs_codec_if_hide_encode, rs_if_hide)
RS_CONN_SEND(rs_conn_send_if_object, RS_S2C_IF_SETOBJECT, rs_codec_if_object_encode, rs_if_object)
RS_CONN_SEND(rs_conn_send_if_npc_head, RS_S2C_IF_SETNPCHEAD, rs_codec_if_npc_head_encode, rs_if_npc_head)
RS_CONN_SEND(rs_conn_send_if_player_head, RS_S2C_IF_SETPLAYERHEAD, rs_codec_if_player_head_encode, rs_if_player_head)
RS_CONN_SEND(rs_conn_send_if_model, RS_S2C_IF_SETMODEL, rs_codec_if_model_encode, rs_if_model)
RS_CONN_SEND(rs_conn_send_if_colour, RS_S2C_IF_SETCOLOUR, rs_codec_if_colour_encode, rs_if_colour)
RS_CONN_SEND(rs_conn_send_if_pos, RS_S2C_IF_SETPOS, rs_codec_if_pos_encode, rs_if_pos)
RS_CONN_SEND(rs_conn_send_if_scroll, RS_S2C_IF_SETSCROLL, rs_codec_if_scroll_encode, rs_if_scroll)
RS_CONN_SEND(rs_conn_send_if_angle, RS_S2C_IF_SETANGLE, rs_codec_if_angle_encode, rs_if_angle)
RS_CONN_SEND(rs_conn_send_spotanim, RS_S2C_SPOTANIM, rs_codec_spotanim_encode, rs_spotanim)
RS_CONN_SEND(rs_conn_send_sidebar, RS_S2C_SIDEBAR, rs_codec_sidebar_encode, rs_sidebar)
RS_CONN_SEND(rs_conn_send_tab, RS_S2C_TAB, rs_codec_tab_encode, rs_tab)
RS_CONN_SEND(rs_conn_send_last_login, RS_S2C_LAST_LOGIN, rs_codec_last_login_encode, rs_last_login)

#undef RS_CONN_SEND

rs_err rs_conn_send_npc_empty(rs_conn *c) {
    uint8_t raw[64];
    rs_buf b;
    rs_err e;
    if (!c) {
        return RS_ERR_ARG;
    }
    rs_buf_wrap(&b, raw, sizeof(raw));
    e = rs_codec_npc_update_empty(rs_conn_session(c), &b);
    if (e != RS_OK) {
        return e;
    }
    return rs_conn_send_pkt(c, RS_S2C_NPC_UPDATE, b.data, b.pos);
}

rs_err rs_conn_send_varp(rs_conn *c, const rs_varp *v) {
    uint8_t raw[4096];
    rs_buf b;
    rs_err e;
    rs_pkt id;
    if (!c || !v) {
        return RS_ERR_ARG;
    }
    /* Width picks the logical id. The codec only writes the bytes. */
    if (v->value >= -128 && v->value <= 127) {
        id = RS_S2C_VARP_SMALL;
    } else {
        id = RS_S2C_VARP_LARGE;
    }
    rs_buf_wrap(&b, raw, sizeof(raw));
    e = rs_codec_varp_encode(rs_conn_session(c), &b, v);
    if (e != RS_OK) {
        return e;
    }
    return rs_conn_send_pkt(c, id, b.data, b.pos);
}

rs_err rs_conn_send_logout(rs_conn *c) {
    if (!c) {
        return RS_ERR_ARG;
    }
    return rs_conn_send_pkt(c, RS_S2C_LOGOUT, NULL, 0);
}

rs_err rs_conn_send_camera_reset(rs_conn *c) {
    if (!c) {
        return RS_ERR_ARG;
    }
    return rs_conn_send_pkt(c, RS_S2C_CAMERA_RESET, NULL, 0);
}

rs_err rs_conn_send_if_close(rs_conn *c) {
    if (!c) {
        return RS_ERR_ARG;
    }
    return rs_conn_send_pkt(c, RS_S2C_IF_CLOSE, NULL, 0);
}

rs_err rs_conn_send_reset_anims(rs_conn *c) {
    if (!c) {
        return RS_ERR_ARG;
    }
    return rs_conn_send_pkt(c, RS_S2C_RESET_ANIMS, NULL, 0);
}
