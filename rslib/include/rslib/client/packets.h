#ifndef RSLIB_CLIENT_PACKETS_H
#define RSLIB_CLIENT_PACKETS_H

#include "rslib/both/action.h"
#include "rslib/both/codec.h"
#include "rslib/both/err.h"
#include "rslib/both/packets.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rs_client;

rs_err rs_client_send_pkt(struct rs_client *c, rs_pkt id, const void *payload, size_t n);

rs_err rs_client_send_walk(struct rs_client *c, const rs_walk *v);
rs_err rs_client_send_chat(struct rs_client *c, const rs_chat_in *v);
rs_err rs_client_send_command(struct rs_client *c, const rs_command *v);
rs_err rs_client_send_private(struct rs_client *c, const rs_private_out *v);
rs_err rs_client_send_chat_mode(struct rs_client *c, const rs_chat_mode *v);
rs_err rs_client_send_design(struct rs_client *c, const rs_design *v);

rs_err rs_client_send_button(struct rs_client *c, int which, const rs_action_button *v);
rs_err rs_client_send_if_option(struct rs_client *c, int which, const rs_action_if_option *v);
rs_err rs_client_send_opheld(struct rs_client *c, const rs_action_if_option *v);
rs_err rs_client_send_npc(struct rs_client *c, int which, const rs_action_npc *v);
rs_err rs_client_send_attack_npc(struct rs_client *c, const rs_action_attack_npc *v);
rs_err rs_client_send_object(struct rs_client *c, int which, const rs_action_object *v);
rs_err rs_client_send_ground(struct rs_client *c, const rs_action_ground *v);
rs_err rs_client_send_item_on_item(struct rs_client *c, const rs_action_item_on_item *v);
rs_err rs_client_send_item_on_npc(struct rs_client *c, const rs_action_item_on_npc *v);
rs_err rs_client_send_item_on_object(struct rs_client *c, const rs_action_item_on_object *v);
rs_err rs_client_send_item_on_player(struct rs_client *c, const rs_action_item_on_player *v);
rs_err rs_client_send_item_on_ground(struct rs_client *c, const rs_action_item_on_ground *v);
rs_err rs_client_send_equip(struct rs_client *c, const rs_action_equip *v);
rs_err rs_client_send_drop(struct rs_client *c, const rs_action_drop *v);
rs_err rs_client_send_move_item(struct rs_client *c, const rs_action_move_item *v);
rs_err rs_client_send_magic_on_npc(struct rs_client *c, const rs_action_magic_on_npc *v);
rs_err rs_client_send_magic_on_player(struct rs_client *c, const rs_action_magic_on_player *v);
rs_err rs_client_send_magic_on_item(struct rs_client *c, const rs_action_magic_on_item *v);
rs_err rs_client_send_magic_on_ground(struct rs_client *c, const rs_action_magic_on_ground *v);
rs_err rs_client_send_trade(struct rs_client *c, const rs_action_trade *v);
rs_err rs_client_send_follow(struct rs_client *c, const rs_action_follow *v);
rs_err rs_client_send_attack_player(struct rs_client *c, const rs_action_attack_player *v);
rs_err rs_client_send_dialogue(struct rs_client *c, const rs_action_dialogue *v);
rs_err rs_client_send_enter_amount(struct rs_client *c, const rs_action_enter_amount *v);
rs_err rs_client_send_enter_name(struct rs_client *c, const rs_action_enter_name *v);
rs_err rs_client_send_add_friend(struct rs_client *c, const rs_action_add_friend *v);
rs_err rs_client_send_del_friend(struct rs_client *c, const rs_action_del_friend *v);
rs_err rs_client_send_add_ignore(struct rs_client *c, const rs_action_add_ignore *v);
rs_err rs_client_send_del_ignore(struct rs_client *c, const rs_action_del_ignore *v);
rs_err rs_client_send_close(struct rs_client *c, const rs_action_close *v);

#ifdef __cplusplus
}
#endif

#endif
