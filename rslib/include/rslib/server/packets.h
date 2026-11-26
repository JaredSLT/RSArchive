#ifndef RSLIB_SERVER_PACKETS_H
#define RSLIB_SERVER_PACKETS_H

#include "rslib/both/codec.h"
#include "rslib/both/err.h"
#include "rslib/both/packets.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rs_conn;

rs_err rs_conn_send_pkt(struct rs_conn *c, rs_pkt id, const void *payload, size_t n);

rs_err rs_conn_send_player_update(struct rs_conn *c, const rs_player_update *v);
rs_err rs_conn_send_region(struct rs_conn *c, const rs_region *v);
rs_err rs_conn_send_region_instanced(struct rs_conn *c, const rs_region_instanced *v);
rs_err rs_conn_send_message(struct rs_conn *c, const rs_message *v);
rs_err rs_conn_send_skill(struct rs_conn *c, const rs_skill *v);
rs_err rs_conn_send_energy(struct rs_conn *c, const rs_energy *v);
rs_err rs_conn_send_weight(struct rs_conn *c, const rs_weight *v);
rs_err rs_conn_send_if_open(struct rs_conn *c, const rs_if_open *v);
rs_err rs_conn_send_if_text(struct rs_conn *c, const rs_if_text *v);
rs_err rs_conn_send_if_top(struct rs_conn *c, const rs_if_top *v);
rs_err rs_conn_send_if_sub(struct rs_conn *c, const rs_if_sub *v);
rs_err rs_conn_send_inventory(struct rs_conn *c, const rs_inventory *v);
rs_err rs_conn_send_inv_update(struct rs_conn *c, const rs_inv_update *v);
rs_err rs_conn_send_inv_clear(struct rs_conn *c, const rs_inv_clear *v);
rs_err rs_conn_send_system_update(struct rs_conn *c, const rs_system_update *v);
rs_err rs_conn_send_minimap(struct rs_conn *c, const rs_minimap *v);
rs_err rs_conn_send_hint(struct rs_conn *c, const rs_hint *v);
rs_err rs_conn_send_camera_move(struct rs_conn *c, const rs_camera *v);
rs_err rs_conn_send_cam_lookat(struct rs_conn *c, const rs_camera *v);
rs_err rs_conn_send_cam_shake(struct rs_conn *c, const rs_cam_shake *v);
rs_err rs_conn_send_player_option(struct rs_conn *c, const rs_player_option *v);
rs_err rs_conn_send_chat_settings(struct rs_conn *c, const rs_chat_settings *v);
rs_err rs_conn_send_friend_status(struct rs_conn *c, const rs_friend_status *v);
rs_err rs_conn_send_friend_entry(struct rs_conn *c, const rs_friend_entry *v);
rs_err rs_conn_send_ignore_list(struct rs_conn *c, const rs_ignore_list *v);
rs_err rs_conn_send_private_message(struct rs_conn *c, const rs_private_message *v);
rs_err rs_conn_send_projectile(struct rs_conn *c, const rs_projectile *v);
rs_err rs_conn_send_song(struct rs_conn *c, const rs_song *v);
rs_err rs_conn_send_jingle(struct rs_conn *c, const rs_jingle *v);
rs_err rs_conn_send_sound(struct rs_conn *c, const rs_sound *v);
rs_err rs_conn_send_sound_area(struct rs_conn *c, const rs_sound_area *v);
rs_err rs_conn_send_player_index(struct rs_conn *c, const rs_player_index *v);
rs_err rs_conn_send_walkable(struct rs_conn *c, const rs_walkable *v);
rs_err rs_conn_send_multizone(struct rs_conn *c, const rs_multizone *v);
rs_err rs_conn_send_obj_add(struct rs_conn *c, const rs_scene_obj *v);
rs_err rs_conn_send_obj_del(struct rs_conn *c, const rs_scene_obj *v);
rs_err rs_conn_send_loc_add(struct rs_conn *c, const rs_scene_loc *v);
rs_err rs_conn_send_loc_del(struct rs_conn *c, const rs_scene_loc *v);
rs_err rs_conn_send_loc_anim(struct rs_conn *c, const rs_loc_anim *v);
rs_err rs_conn_send_zone_full(struct rs_conn *c, const rs_zone_full *v);
rs_err rs_conn_send_zone_partial(struct rs_conn *c, const rs_zone_partial *v);
rs_err rs_conn_send_zone_base(struct rs_conn *c, const rs_zone_base *v);
rs_err rs_conn_send_if_anim(struct rs_conn *c, const rs_if_anim *v);
rs_err rs_conn_send_if_hide(struct rs_conn *c, const rs_if_hide *v);
rs_err rs_conn_send_if_object(struct rs_conn *c, const rs_if_object *v);
rs_err rs_conn_send_if_npc_head(struct rs_conn *c, const rs_if_npc_head *v);
rs_err rs_conn_send_if_player_head(struct rs_conn *c, const rs_if_player_head *v);
rs_err rs_conn_send_if_model(struct rs_conn *c, const rs_if_model *v);
rs_err rs_conn_send_if_colour(struct rs_conn *c, const rs_if_colour *v);
rs_err rs_conn_send_if_pos(struct rs_conn *c, const rs_if_pos *v);
rs_err rs_conn_send_if_scroll(struct rs_conn *c, const rs_if_scroll *v);
rs_err rs_conn_send_if_angle(struct rs_conn *c, const rs_if_angle *v);
rs_err rs_conn_send_spotanim(struct rs_conn *c, const rs_spotanim *v);
rs_err rs_conn_send_sidebar(struct rs_conn *c, const rs_sidebar *v);
rs_err rs_conn_send_tab(struct rs_conn *c, const rs_tab *v);
rs_err rs_conn_send_last_login(struct rs_conn *c, const rs_last_login *v);

rs_err rs_conn_send_npc_empty(struct rs_conn *c);
rs_err rs_conn_send_varp(struct rs_conn *c, const rs_varp *v);
rs_err rs_conn_send_logout(struct rs_conn *c);
rs_err rs_conn_send_camera_reset(struct rs_conn *c);
rs_err rs_conn_send_if_close(struct rs_conn *c);
rs_err rs_conn_send_reset_anims(struct rs_conn *c);

#ifdef __cplusplus
}
#endif

#endif
