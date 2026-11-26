#include "both/packets_impl.h"

#include <stdint.h>

typedef struct {
    rs_pkt id;
    const char *name;
    rs_pkt_dir dir;
} rs_pkt_meta;

static const rs_pkt_meta k_meta[] = {
    {RS_C2S_IDLE, "idle", RS_DIR_C2S},
    {RS_C2S_NO_TIMEOUT, "no_timeout", RS_DIR_C2S},
    {RS_C2S_FOCUS, "focus", RS_DIR_C2S},
    {RS_C2S_WALK, "walk", RS_DIR_C2S},
    {RS_C2S_WALK_MINIMAP, "walk_minimap", RS_DIR_C2S},
    {RS_C2S_WALK_ANTICHEAT, "walk_anticheat", RS_DIR_C2S},
    {RS_C2S_CHAT, "chat", RS_DIR_C2S},
    {RS_C2S_COMMAND, "command", RS_DIR_C2S},
    {RS_C2S_PRIVATE_MESSAGE, "private_message", RS_DIR_C2S},
    {RS_C2S_BUTTON, "button", RS_DIR_C2S},
    {RS_C2S_BUTTON2, "button2", RS_DIR_C2S},
    {RS_C2S_BUTTON3, "button3", RS_DIR_C2S},
    {RS_C2S_BUTTON4, "button4", RS_DIR_C2S},
    {RS_C2S_BUTTON5, "button5", RS_DIR_C2S},
    {RS_C2S_CLOSE_INTERFACE, "close_interface", RS_DIR_C2S},
    {RS_C2S_DIALOGUE, "dialogue", RS_DIR_C2S},
    {RS_C2S_ENTER_AMOUNT, "enter_amount", RS_DIR_C2S},
    {RS_C2S_ENTER_NAME, "enter_name", RS_DIR_C2S},
    {RS_C2S_CAMERA, "camera", RS_DIR_C2S},
    {RS_C2S_CLICK, "click", RS_DIR_C2S},
    {RS_C2S_APPEARANCE, "appearance", RS_DIR_C2S},
    {RS_C2S_DROP, "drop", RS_DIR_C2S},
    {RS_C2S_EQUIP, "equip", RS_DIR_C2S},
    {RS_C2S_MOVE_ITEM, "move_item", RS_DIR_C2S},
    {RS_C2S_ITEM_ON_ITEM, "item_on_item", RS_DIR_C2S},
    {RS_C2S_ITEM_ON_NPC, "item_on_npc", RS_DIR_C2S},
    {RS_C2S_ITEM_ON_OBJECT, "item_on_object", RS_DIR_C2S},
    {RS_C2S_ITEM_ON_PLAYER, "item_on_player", RS_DIR_C2S},
    {RS_C2S_ITEM_ON_GROUND, "item_on_ground", RS_DIR_C2S},
    {RS_C2S_MAGIC_ON_NPC, "magic_on_npc", RS_DIR_C2S},
    {RS_C2S_MAGIC_ON_PLAYER, "magic_on_player", RS_DIR_C2S},
    {RS_C2S_MAGIC_ON_ITEM, "magic_on_item", RS_DIR_C2S},
    {RS_C2S_NPC1, "npc1", RS_DIR_C2S},
    {RS_C2S_NPC2, "npc2", RS_DIR_C2S},
    {RS_C2S_NPC3, "npc3", RS_DIR_C2S},
    {RS_C2S_NPC4, "npc4", RS_DIR_C2S},
    {RS_C2S_NPC5, "npc5", RS_DIR_C2S},
    {RS_C2S_ATTACK_NPC, "attack_npc", RS_DIR_C2S},
    {RS_C2S_OBJECT1, "object1", RS_DIR_C2S},
    {RS_C2S_OBJECT2, "object2", RS_DIR_C2S},
    {RS_C2S_OBJECT3, "object3", RS_DIR_C2S},
    {RS_C2S_OBJECT4, "object4", RS_DIR_C2S},
    {RS_C2S_OBJECT5, "object5", RS_DIR_C2S},
    {RS_C2S_GROUND_ITEM, "ground_item", RS_DIR_C2S},
    {RS_C2S_ATTACK_PLAYER, "attack_player", RS_DIR_C2S},
    {RS_C2S_FOLLOW, "follow", RS_DIR_C2S},
    {RS_C2S_TRADE, "trade", RS_DIR_C2S},
    {RS_C2S_ADD_FRIEND, "add_friend", RS_DIR_C2S},
    {RS_C2S_DEL_FRIEND, "del_friend", RS_DIR_C2S},
    {RS_C2S_ADD_IGNORE, "add_ignore", RS_DIR_C2S},
    {RS_C2S_DEL_IGNORE, "del_ignore", RS_DIR_C2S},
    {RS_C2S_CHAT_MODE, "chat_mode", RS_DIR_C2S},
    {RS_C2S_IDLE_LOGOUT, "idle_logout", RS_DIR_C2S},
    {RS_C2S_WINDOW_STATUS, "window_status", RS_DIR_C2S},
    {RS_C2S_CLAN_JOIN, "clan_join", RS_DIR_C2S},
    {RS_C2S_CLAN_KICK, "clan_kick", RS_DIR_C2S},
    {RS_C2S_FRIEND_RANK, "friend_rank", RS_DIR_C2S},
    {RS_C2S_RESUME_STRING, "resume_string", RS_DIR_C2S},
    {RS_C2S_MAGIC_ON_GROUND, "magic_on_ground", RS_DIR_C2S},
    {RS_C2S_OPHELD, "opheld", RS_DIR_C2S},
    {RS_S2C_PLAYER_UPDATE, "player_update", RS_DIR_S2C},
    {RS_S2C_NPC_UPDATE, "npc_update", RS_DIR_S2C},
    {RS_S2C_NPC_UPDATE_LARGE, "npc_update_large", RS_DIR_S2C},
    {RS_S2C_REGION, "region", RS_DIR_S2C},
    {RS_S2C_REGION_INSTANCED, "region_instanced", RS_DIR_S2C},
    {RS_S2C_LOGOUT, "logout", RS_DIR_S2C},
    {RS_S2C_MESSAGE, "message", RS_DIR_S2C},
    {RS_S2C_SKILL, "skill", RS_DIR_S2C},
    {RS_S2C_ENERGY, "energy", RS_DIR_S2C},
    {RS_S2C_WEIGHT, "weight", RS_DIR_S2C},
    {RS_S2C_VARP_SMALL, "varp_small", RS_DIR_S2C},
    {RS_S2C_VARP_LARGE, "varp_large", RS_DIR_S2C},
    {RS_S2C_TAB, "tab", RS_DIR_S2C},
    {RS_S2C_SIDEBAR, "sidebar", RS_DIR_S2C},
    {RS_S2C_IF_OPEN, "if_open", RS_DIR_S2C},
    {RS_S2C_IF_CLOSE, "if_close", RS_DIR_S2C},
    {RS_S2C_IF_TEXT, "if_text", RS_DIR_S2C},
    {RS_S2C_IF_OPENTOP, "if_opentop", RS_DIR_S2C},
    {RS_S2C_IF_OPENSUB, "if_opensub", RS_DIR_S2C},
    {RS_S2C_IF_CLOSESUB, "if_closesub", RS_DIR_S2C},
    {RS_S2C_INVENTORY, "inventory", RS_DIR_S2C},
    {RS_S2C_INVENTORY_UPDATE, "inventory_update", RS_DIR_S2C},
    {RS_S2C_SYSTEM_UPDATE, "system_update", RS_DIR_S2C},
    {RS_S2C_MINIMAP, "minimap", RS_DIR_S2C},
    {RS_S2C_HINT, "hint", RS_DIR_S2C},
    {RS_S2C_CAMERA_RESET, "camera_reset", RS_DIR_S2C},
    {RS_S2C_CAMERA_MOVE, "camera_move", RS_DIR_S2C},
    {RS_S2C_PLAYER_OPTION, "player_option", RS_DIR_S2C},
    {RS_S2C_CHAT_SETTINGS, "chat_settings", RS_DIR_S2C},
    {RS_S2C_FRIEND_STATUS, "friend_status", RS_DIR_S2C},
    {RS_S2C_FRIEND_LIST, "friend_list", RS_DIR_S2C},
    {RS_S2C_IGNORE_LIST, "ignore_list", RS_DIR_S2C},
    {RS_S2C_PRIVATE_MESSAGE, "pm_in", RS_DIR_S2C},
    {RS_S2C_RESET_ANIMS, "reset_anims", RS_DIR_S2C},
    {RS_S2C_PROJECTILE, "projectile", RS_DIR_S2C},
    {RS_S2C_SONG, "song", RS_DIR_S2C},
    {RS_S2C_SOUND, "sound", RS_DIR_S2C},
    {RS_S2C_PLAYER_INDEX, "player_index", RS_DIR_S2C},
    {RS_S2C_WALKABLE_INTERFACE, "walkable_interface", RS_DIR_S2C},
    {RS_S2C_MULTIZONE, "multizone", RS_DIR_S2C},
    {RS_S2C_NO_TIMEOUT, "s2c_no_timeout", RS_DIR_S2C},
    {RS_S2C_SCRIPT, "script", RS_DIR_S2C},
    {RS_S2C_OBJ_ADD, "obj_add", RS_DIR_S2C},
    {RS_S2C_OBJ_DEL, "obj_del", RS_DIR_S2C},
    {RS_S2C_LOC_ADD, "loc_add", RS_DIR_S2C},
    {RS_S2C_LOC_DEL, "loc_del", RS_DIR_S2C},
    {RS_S2C_ZONE_FULL, "zone_full", RS_DIR_S2C},
    {RS_S2C_ZONE_PARTIAL, "zone_partial", RS_DIR_S2C},
    {RS_S2C_ZONE_BASE, "zone_base", RS_DIR_S2C},
    {RS_S2C_IF_SETANIM, "if_setanim", RS_DIR_S2C},
    {RS_S2C_IF_SETHIDE, "if_sethide", RS_DIR_S2C},
    {RS_S2C_IF_SETOBJECT, "if_setobject", RS_DIR_S2C},
    {RS_S2C_IF_SETNPCHEAD, "if_setnpchead", RS_DIR_S2C},
    {RS_S2C_IF_SETPLAYERHEAD, "if_setplayerhead", RS_DIR_S2C},
    {RS_S2C_IF_SETMODEL, "if_setmodel", RS_DIR_S2C},
    {RS_S2C_IF_SETCOLOUR, "if_setcolour", RS_DIR_S2C},
    {RS_S2C_IF_SETPOS, "if_setpos", RS_DIR_S2C},
    {RS_S2C_IF_SETSCROLL, "if_setscroll", RS_DIR_S2C},
    {RS_S2C_IF_SETANGLE, "if_setangle", RS_DIR_S2C},
    {RS_S2C_VARBIT_SMALL, "varbit_small", RS_DIR_S2C},
    {RS_S2C_VARBIT_LARGE, "varbit_large", RS_DIR_S2C},
    {RS_S2C_MIDI_JINGLE, "midi_jingle", RS_DIR_S2C},
    {RS_S2C_CAM_SHAKE, "cam_shake", RS_DIR_S2C},
    {RS_S2C_CAM_LOOKAT, "cam_lookat", RS_DIR_S2C},
    {RS_S2C_INV_CLEAR, "inv_clear", RS_DIR_S2C},
    {RS_S2C_SPOTANIM, "spotanim", RS_DIR_S2C},
    {RS_S2C_LOC_ANIM, "loc_anim", RS_DIR_S2C},
    {RS_S2C_SOUND_AREA, "sound_area", RS_DIR_S2C},
    {RS_S2C_UPDATE_UID, "update_uid", RS_DIR_S2C},
    {RS_S2C_LAST_LOGIN, "last_login", RS_DIR_S2C},
    {RS_S2C_CLAN, "clan", RS_DIR_S2C},
    {RS_S2C_FRIEND_LOADED, "friend_loaded", RS_DIR_S2C},
    {RS_S2C_GE_OFFER, "ge_offer", RS_DIR_S2C},
    {RS_C2S_IF_OPTION1, "if_option1", RS_DIR_C2S},
    {RS_C2S_IF_OPTION2, "if_option2", RS_DIR_C2S},
    {RS_C2S_IF_OPTION3, "if_option3", RS_DIR_C2S},
    {RS_C2S_IF_OPTION4, "if_option4", RS_DIR_C2S},
    {RS_C2S_IF_OPTION5, "if_option5", RS_DIR_C2S},
    {RS_S2C_AMOUNT_PROMPT, "amount_prompt", RS_DIR_S2C},
};

const char *rs_pkt_name(rs_pkt id) {
    size_t i;
    if (id <= 0 || id >= RS_PKT_MAX) {
        return "none";
    }
    for (i = 0; i < sizeof(k_meta) / sizeof(k_meta[0]); i++) {
        if (k_meta[i].id == id) {
            return k_meta[i].name;
        }
    }
    return "unknown";
}

rs_pkt rs_pkt_lookup(const char *name) {
    size_t i;
    if (!name) {
        return RS_PKT_NONE;
    }
    for (i = 0; i < sizeof(k_meta) / sizeof(k_meta[0]); i++) {
        if (strcmp(k_meta[i].name, name) == 0) {
            return k_meta[i].id;
        }
    }
    return RS_PKT_NONE;
}

rs_pkt_dir rs_pkt_direction(rs_pkt id) {
    size_t i;
    for (i = 0; i < sizeof(k_meta) / sizeof(k_meta[0]); i++) {
        if (k_meta[i].id == id) {
            return k_meta[i].dir;
        }
    }
    return RS_DIR_C2S;
}
