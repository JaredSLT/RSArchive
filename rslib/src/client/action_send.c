#include "client/client_impl.h"

/* which outside 1..5 has no packet id; the action encoder returns RS_ERR_ARG. */
static rs_pkt which_id(int which, const rs_pkt ids[5]) {
    if (which < 1 || which > 5) {
        return RS_PKT_NONE;
    }
    return ids[which - 1];
}

#define RS_ACTION_SEND(fn, pkt, enc, ty) \
rs_err fn(rs_client *c, const ty *v) { \
    uint8_t raw[4096]; \
    rs_buf b; \
    rs_err e; \
    const rs_build_info *build; \
    if (!c || !v) return RS_ERR_ARG; \
    build = rs_client_build(c); \
    if (!build) return RS_ERR_PROTO; \
    if (rs_pkt_opcode(build, pkt) < 0) return RS_ERR_UNSUPPORTED; \
    rs_buf_wrap(&b, raw, sizeof(raw)); \
    e = enc(build->revision, &b, v); \
    if (e != RS_OK) return e; \
    return rs_client_send_pkt(c, pkt, b.data, b.pos); \
}

#define RS_ACTION_WHICH(fn, enc, ty, i1, i2, i3, i4, i5) \
rs_err fn(rs_client *c, int which, const ty *v) { \
    uint8_t raw[4096]; \
    rs_buf b; \
    rs_err e; \
    const rs_build_info *build; \
    rs_pkt id; \
    const rs_pkt ids[5] = {i1, i2, i3, i4, i5}; \
    if (!c || !v) return RS_ERR_ARG; \
    build = rs_client_build(c); \
    if (!build) return RS_ERR_PROTO; \
    id = which_id(which, ids); \
    if (id == RS_PKT_NONE) { \
        rs_buf_wrap(&b, raw, sizeof(raw)); \
        return enc(build->revision, which, &b, v); \
    } \
    if (rs_pkt_opcode(build, id) < 0) return RS_ERR_UNSUPPORTED; \
    rs_buf_wrap(&b, raw, sizeof(raw)); \
    e = enc(build->revision, which, &b, v); \
    if (e != RS_OK) return e; \
    return rs_client_send_pkt(c, id, b.data, b.pos); \
}

RS_ACTION_WHICH(rs_client_send_button, rs_action_button_encode, rs_action_button,
                RS_C2S_BUTTON, RS_C2S_BUTTON2, RS_C2S_BUTTON3, RS_C2S_BUTTON4, RS_C2S_BUTTON5)
RS_ACTION_WHICH(rs_client_send_if_option, rs_action_if_option_encode, rs_action_if_option,
                RS_C2S_IF_OPTION1, RS_C2S_IF_OPTION2, RS_C2S_IF_OPTION3, RS_C2S_IF_OPTION4,
                RS_C2S_IF_OPTION5)
RS_ACTION_SEND(rs_client_send_opheld, RS_C2S_OPHELD, rs_action_opheld_encode, rs_action_if_option)
RS_ACTION_WHICH(rs_client_send_npc, rs_action_npc_encode, rs_action_npc,
                RS_C2S_NPC1, RS_C2S_NPC2, RS_C2S_NPC3, RS_C2S_NPC4, RS_C2S_NPC5)
RS_ACTION_SEND(rs_client_send_attack_npc, RS_C2S_ATTACK_NPC, rs_action_attack_npc_encode, rs_action_attack_npc)
RS_ACTION_WHICH(rs_client_send_object, rs_action_object_encode, rs_action_object,
                RS_C2S_OBJECT1, RS_C2S_OBJECT2, RS_C2S_OBJECT3, RS_C2S_OBJECT4, RS_C2S_OBJECT5)
RS_ACTION_SEND(rs_client_send_ground, RS_C2S_GROUND_ITEM, rs_action_ground_encode, rs_action_ground)
RS_ACTION_SEND(rs_client_send_item_on_item, RS_C2S_ITEM_ON_ITEM, rs_action_item_on_item_encode, rs_action_item_on_item)
RS_ACTION_SEND(rs_client_send_item_on_npc, RS_C2S_ITEM_ON_NPC, rs_action_item_on_npc_encode, rs_action_item_on_npc)
RS_ACTION_SEND(rs_client_send_item_on_object, RS_C2S_ITEM_ON_OBJECT, rs_action_item_on_object_encode, rs_action_item_on_object)
RS_ACTION_SEND(rs_client_send_item_on_player, RS_C2S_ITEM_ON_PLAYER, rs_action_item_on_player_encode, rs_action_item_on_player)
RS_ACTION_SEND(rs_client_send_item_on_ground, RS_C2S_ITEM_ON_GROUND, rs_action_item_on_ground_encode, rs_action_item_on_ground)
RS_ACTION_SEND(rs_client_send_equip, RS_C2S_EQUIP, rs_action_equip_encode, rs_action_equip)
RS_ACTION_SEND(rs_client_send_drop, RS_C2S_DROP, rs_action_drop_encode, rs_action_drop)
RS_ACTION_SEND(rs_client_send_move_item, RS_C2S_MOVE_ITEM, rs_action_move_item_encode, rs_action_move_item)
RS_ACTION_SEND(rs_client_send_magic_on_npc, RS_C2S_MAGIC_ON_NPC, rs_action_magic_on_npc_encode, rs_action_magic_on_npc)
RS_ACTION_SEND(rs_client_send_magic_on_player, RS_C2S_MAGIC_ON_PLAYER, rs_action_magic_on_player_encode, rs_action_magic_on_player)
RS_ACTION_SEND(rs_client_send_magic_on_item, RS_C2S_MAGIC_ON_ITEM, rs_action_magic_on_item_encode, rs_action_magic_on_item)
RS_ACTION_SEND(rs_client_send_magic_on_ground, RS_C2S_MAGIC_ON_GROUND, rs_action_magic_on_ground_encode, rs_action_magic_on_ground)
RS_ACTION_SEND(rs_client_send_trade, RS_C2S_TRADE, rs_action_trade_encode, rs_action_trade)
RS_ACTION_SEND(rs_client_send_follow, RS_C2S_FOLLOW, rs_action_follow_encode, rs_action_follow)
RS_ACTION_SEND(rs_client_send_attack_player, RS_C2S_ATTACK_PLAYER, rs_action_attack_player_encode, rs_action_attack_player)
RS_ACTION_SEND(rs_client_send_dialogue, RS_C2S_DIALOGUE, rs_action_dialogue_encode, rs_action_dialogue)
RS_ACTION_SEND(rs_client_send_enter_amount, RS_C2S_ENTER_AMOUNT, rs_action_enter_amount_encode, rs_action_enter_amount)
RS_ACTION_SEND(rs_client_send_enter_name, RS_C2S_ENTER_NAME, rs_action_enter_name_encode, rs_action_enter_name)
RS_ACTION_SEND(rs_client_send_add_friend, RS_C2S_ADD_FRIEND, rs_action_add_friend_encode, rs_action_add_friend)
RS_ACTION_SEND(rs_client_send_del_friend, RS_C2S_DEL_FRIEND, rs_action_del_friend_encode, rs_action_del_friend)
RS_ACTION_SEND(rs_client_send_add_ignore, RS_C2S_ADD_IGNORE, rs_action_add_ignore_encode, rs_action_add_ignore)
RS_ACTION_SEND(rs_client_send_del_ignore, RS_C2S_DEL_IGNORE, rs_action_del_ignore_encode, rs_action_del_ignore)
RS_ACTION_SEND(rs_client_send_close, RS_C2S_CLOSE_INTERFACE, rs_action_close_encode, rs_action_close)

#undef RS_ACTION_SEND
#undef RS_ACTION_WHICH
