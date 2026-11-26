#ifndef RSLIB_ACTION_H
#define RSLIB_ACTION_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"
#include "rslib/both/build.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Click and resume payloads. The caller writes the opcode and frame length.
 * interface_id is the width the writer uses: u16 on 317/254/377, the whole
 * packed widget on 742/910. A revision with no transcribed writer returns
 * RS_ERR_UNSUPPORTED. which is 1..5; anything else is RS_ERR_ARG. */

enum { RS_ACTION_NAME = 256 };
enum { RS_ACTION_SCRIPT_ARGS = 32 };

typedef struct rs_action_button {
    int interface_id; /* 742/910: packed widget, not split */
    int child;        /* 742 u16 beside the widget; 910 component id */
    int slot;
    int item;
} rs_action_button;

/* 317 component iop and obj iop[0]. interface_id is menuParamC, slot is
 * menuParamB, item is menuParamA (the obj type, not the stored slot id). */
typedef struct rs_action_if_option {
    int interface_id;
    int slot;
    int item;
} rs_action_if_option;

typedef struct rs_action_npc {
    int index;
    int x;
    int z;
    int ctrl;
} rs_action_npc;

typedef struct rs_action_attack_npc {
    int index;
    int x;
    int z;
    int ctrl;
} rs_action_attack_npc;

typedef struct rs_action_object {
    int id;
    int x;
    int z;
    int ctrl;
} rs_action_object;

typedef struct rs_action_ground {
    int id;
    int x;
    int z;
    int ctrl;
} rs_action_ground;

typedef struct rs_action_item_on_item {
    int slot;
    int item;
    int interface_id; /* 742: target component hash; 910: target parent layer */
    int selected_slot;
    int selected_item;
    int selected_interface; /* 742/910: selected parent layer, u32 */
} rs_action_item_on_item;

typedef struct rs_action_item_on_npc {
    int index;
    int item;
    int interface_id;
    int slot;
    int ctrl;
    int component;
} rs_action_item_on_npc;

typedef struct rs_action_item_on_object {
    int id;
    int x;
    int z;
    int item;
    int interface_id;
    int slot;
    int ctrl;
    int component;
} rs_action_item_on_object;

typedef struct rs_action_item_on_player {
    int index;
    int item;
    int interface_id;
    int slot;
    int ctrl;
    int component;
} rs_action_item_on_player;

typedef struct rs_action_item_on_ground {
    int id;
    int x;
    int z;
    int item;
    int interface_id;
    int slot;
    int ctrl;
    int component;
} rs_action_item_on_ground;

typedef struct rs_action_equip {
    int item;
    int slot;
    int interface_id;
} rs_action_equip;

typedef struct rs_action_drop {
    int item;
    int slot;
    int interface_id;
} rs_action_drop;

typedef struct rs_action_move_item {
    int interface_id;
    int from_slot;
    int to_slot;
    int insert_mode;
} rs_action_move_item;

typedef struct rs_action_magic_on_npc {
    int index;
    int component;
    int item;
    int interface_id;
    int ctrl;
} rs_action_magic_on_npc;

typedef struct rs_action_magic_on_player {
    int index;
    int component;
    int item;
    int interface_id;
    int ctrl;
} rs_action_magic_on_player;

typedef struct rs_action_magic_on_item {
    int slot;
    int item;
    int interface_id;
    int component;
    int selected_slot;
    int selected_item;
    int selected_interface;
} rs_action_magic_on_item;

typedef struct rs_action_magic_on_ground {
    int id;
    int x;
    int z;
    int component;
    int item;
    int interface_id;
    int ctrl;
} rs_action_magic_on_ground;

typedef struct rs_action_trade {
    int index;
    int ctrl;
} rs_action_trade;

typedef struct rs_action_follow {
    int index;
    int ctrl;
} rs_action_follow;

typedef struct rs_action_attack_player {
    int index;
    int ctrl;
} rs_action_attack_player;

typedef struct rs_action_dialogue {
    int interface_id;
    int child;
} rs_action_dialogue;

typedef struct rs_action_enter_amount {
    int amount;
} rs_action_enter_amount;

typedef struct rs_action_enter_name {
    uint64_t name37;
    char name[RS_ACTION_NAME];
} rs_action_enter_name;

typedef struct rs_action_add_friend {
    uint64_t name37;
    char name[RS_ACTION_NAME];
} rs_action_add_friend;

typedef struct rs_action_del_friend {
    uint64_t name37;
    char name[RS_ACTION_NAME];
} rs_action_del_friend;

typedef struct rs_action_add_ignore {
    uint64_t name37;
    char name[RS_ACTION_NAME];
    int temporary; /* 742/910 ignore-add flag */
} rs_action_add_ignore;

typedef struct rs_action_del_ignore {
    uint64_t name37;
    char name[RS_ACTION_NAME];
} rs_action_del_ignore;

typedef struct rs_action_close {
    int unused;
} rs_action_close;

typedef struct rs_action_script_arg {
    int i;
    char s[RS_ACTION_NAME];
} rs_action_script_arg;

/* types[i] == 's' is a string. Any other char is a signed int.
 * Args are stored in type-string order. 530 component is the leading u16. */
typedef struct rs_action_script {
    int id;
    int component;
    int nargs;
    char types[RS_ACTION_SCRIPT_ARGS + 1];
    rs_action_script_arg arg[RS_ACTION_SCRIPT_ARGS];
} rs_action_script;

typedef struct rs_action_varbit {
    int id;
    int value;
} rs_action_varbit;

rs_err rs_action_button_encode(int revision, int which, rs_buf *dst, const rs_action_button *v);
rs_err rs_action_button_decode(int revision, int which, rs_buf *src, rs_action_button *v);

/* which is 1..5: Client.java actions 632, 78, 867, 431, 53. */
rs_err rs_action_if_option_encode(int revision, int which, rs_buf *dst, const rs_action_if_option *v);
rs_err rs_action_if_option_decode(int revision, int which, rs_buf *src, rs_action_if_option *v);

/* Client.java action 74, opcode 122. */
rs_err rs_action_opheld_encode(int revision, rs_buf *dst, const rs_action_if_option *v);
rs_err rs_action_opheld_decode(int revision, rs_buf *src, rs_action_if_option *v);

rs_err rs_action_npc_encode(int revision, int which, rs_buf *dst, const rs_action_npc *v);
rs_err rs_action_npc_decode(int revision, int which, rs_buf *src, rs_action_npc *v);

rs_err rs_action_attack_npc_encode(int revision, rs_buf *dst, const rs_action_attack_npc *v);
rs_err rs_action_attack_npc_decode(int revision, rs_buf *src, rs_action_attack_npc *v);

rs_err rs_action_object_encode(int revision, int which, rs_buf *dst, const rs_action_object *v);
rs_err rs_action_object_decode(int revision, int which, rs_buf *src, rs_action_object *v);

rs_err rs_action_ground_encode(int revision, rs_buf *dst, const rs_action_ground *v);
rs_err rs_action_ground_decode(int revision, rs_buf *src, rs_action_ground *v);

rs_err rs_action_item_on_item_encode(int revision, rs_buf *dst, const rs_action_item_on_item *v);
rs_err rs_action_item_on_item_decode(int revision, rs_buf *src, rs_action_item_on_item *v);

rs_err rs_action_item_on_npc_encode(int revision, rs_buf *dst, const rs_action_item_on_npc *v);
rs_err rs_action_item_on_npc_decode(int revision, rs_buf *src, rs_action_item_on_npc *v);

rs_err rs_action_item_on_object_encode(int revision, rs_buf *dst, const rs_action_item_on_object *v);
rs_err rs_action_item_on_object_decode(int revision, rs_buf *src, rs_action_item_on_object *v);

rs_err rs_action_item_on_player_encode(int revision, rs_buf *dst, const rs_action_item_on_player *v);
rs_err rs_action_item_on_player_decode(int revision, rs_buf *src, rs_action_item_on_player *v);

rs_err rs_action_item_on_ground_encode(int revision, rs_buf *dst, const rs_action_item_on_ground *v);
rs_err rs_action_item_on_ground_decode(int revision, rs_buf *src, rs_action_item_on_ground *v);

rs_err rs_action_equip_encode(int revision, rs_buf *dst, const rs_action_equip *v);
rs_err rs_action_equip_decode(int revision, rs_buf *src, rs_action_equip *v);

rs_err rs_action_drop_encode(int revision, rs_buf *dst, const rs_action_drop *v);
rs_err rs_action_drop_decode(int revision, rs_buf *src, rs_action_drop *v);

rs_err rs_action_move_item_encode(int revision, rs_buf *dst, const rs_action_move_item *v);
rs_err rs_action_move_item_decode(int revision, rs_buf *src, rs_action_move_item *v);

rs_err rs_action_magic_on_npc_encode(int revision, rs_buf *dst, const rs_action_magic_on_npc *v);
rs_err rs_action_magic_on_npc_decode(int revision, rs_buf *src, rs_action_magic_on_npc *v);

rs_err rs_action_magic_on_player_encode(int revision, rs_buf *dst, const rs_action_magic_on_player *v);
rs_err rs_action_magic_on_player_decode(int revision, rs_buf *src, rs_action_magic_on_player *v);

rs_err rs_action_magic_on_item_encode(int revision, rs_buf *dst, const rs_action_magic_on_item *v);
rs_err rs_action_magic_on_item_decode(int revision, rs_buf *src, rs_action_magic_on_item *v);

rs_err rs_action_magic_on_ground_encode(int revision, rs_buf *dst, const rs_action_magic_on_ground *v);
rs_err rs_action_magic_on_ground_decode(int revision, rs_buf *src, rs_action_magic_on_ground *v);

rs_err rs_action_trade_encode(int revision, rs_buf *dst, const rs_action_trade *v);
rs_err rs_action_trade_decode(int revision, rs_buf *src, rs_action_trade *v);

rs_err rs_action_follow_encode(int revision, rs_buf *dst, const rs_action_follow *v);
rs_err rs_action_follow_decode(int revision, rs_buf *src, rs_action_follow *v);

rs_err rs_action_attack_player_encode(int revision, rs_buf *dst, const rs_action_attack_player *v);
rs_err rs_action_attack_player_decode(int revision, rs_buf *src, rs_action_attack_player *v);

rs_err rs_action_dialogue_encode(int revision, rs_buf *dst, const rs_action_dialogue *v);
rs_err rs_action_dialogue_decode(int revision, rs_buf *src, rs_action_dialogue *v);

rs_err rs_action_enter_amount_encode(int revision, rs_buf *dst, const rs_action_enter_amount *v);
rs_err rs_action_enter_amount_decode(int revision, rs_buf *src, rs_action_enter_amount *v);

rs_err rs_action_enter_name_encode(int revision, rs_buf *dst, const rs_action_enter_name *v);
rs_err rs_action_enter_name_decode(int revision, rs_buf *src, rs_action_enter_name *v);

rs_err rs_action_add_friend_encode(int revision, rs_buf *dst, const rs_action_add_friend *v);
rs_err rs_action_add_friend_decode(int revision, rs_buf *src, rs_action_add_friend *v);

rs_err rs_action_del_friend_encode(int revision, rs_buf *dst, const rs_action_del_friend *v);
rs_err rs_action_del_friend_decode(int revision, rs_buf *src, rs_action_del_friend *v);

rs_err rs_action_add_ignore_encode(int revision, rs_buf *dst, const rs_action_add_ignore *v);
rs_err rs_action_add_ignore_decode(int revision, rs_buf *src, rs_action_add_ignore *v);

rs_err rs_action_del_ignore_encode(int revision, rs_buf *dst, const rs_action_del_ignore *v);
rs_err rs_action_del_ignore_decode(int revision, rs_buf *src, rs_action_del_ignore *v);

rs_err rs_action_close_encode(int revision, rs_buf *dst, const rs_action_close *v);
rs_err rs_action_close_decode(int revision, rs_buf *src, rs_action_close *v);

rs_err rs_action_script_encode(int revision, rs_buf *dst, const rs_action_script *v);
rs_err rs_action_script_decode(int revision, rs_buf *src, rs_action_script *v);

rs_err rs_action_varbit_small_encode(int revision, rs_buf *dst, const rs_action_varbit *v);
rs_err rs_action_varbit_small_decode(int revision, rs_buf *src, rs_action_varbit *v);

rs_err rs_action_varbit_large_encode(int revision, rs_buf *dst, const rs_action_varbit *v);
rs_err rs_action_varbit_large_decode(int revision, rs_buf *src, rs_action_varbit *v);

#ifdef __cplusplus
}
#endif

#endif
