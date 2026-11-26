#ifndef RSLIB_PKT_H
#define RSLIB_PKT_H

#include "rslib/both/build.h"
#include "rslib/both/buf.h"
#include "rslib/both/err.h"
#include "rslib/proto.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Logical packets shared across revisions. Opcode numbers change; these names
 * do not. Bind extra opcodes at runtime with rs_pkt_bind().
 */

typedef enum rs_pkt_dir {
    RS_DIR_C2S = 0,
    RS_DIR_S2C = 1
} rs_pkt_dir;

typedef enum rs_pkt {
    RS_PKT_NONE = 0,

    /* client -> server */
    RS_C2S_IDLE,
    RS_C2S_NO_TIMEOUT,
    RS_C2S_FOCUS,
    RS_C2S_WALK,
    RS_C2S_WALK_MINIMAP,
    RS_C2S_WALK_ANTICHEAT,
    RS_C2S_CHAT,
    RS_C2S_COMMAND,
    RS_C2S_PRIVATE_MESSAGE,
    RS_C2S_BUTTON,
    RS_C2S_BUTTON2,
    RS_C2S_BUTTON3,
    RS_C2S_BUTTON4,
    RS_C2S_BUTTON5,
    RS_C2S_CLOSE_INTERFACE,
    RS_C2S_DIALOGUE,
    RS_C2S_ENTER_AMOUNT,
    RS_C2S_ENTER_NAME,
    RS_C2S_CAMERA,
    RS_C2S_CLICK,
    RS_C2S_APPEARANCE,
    RS_C2S_DROP,
    RS_C2S_EQUIP,
    RS_C2S_MOVE_ITEM,
    RS_C2S_ITEM_ON_ITEM,
    RS_C2S_ITEM_ON_NPC,
    RS_C2S_ITEM_ON_OBJECT,
    RS_C2S_ITEM_ON_PLAYER,
    RS_C2S_ITEM_ON_GROUND,
    RS_C2S_MAGIC_ON_NPC,
    RS_C2S_MAGIC_ON_PLAYER,
    RS_C2S_MAGIC_ON_ITEM,
    RS_C2S_NPC1,
    RS_C2S_NPC2,
    RS_C2S_NPC3,
    RS_C2S_NPC4,
    RS_C2S_NPC5,
    RS_C2S_ATTACK_NPC,
    RS_C2S_OBJECT1,
    RS_C2S_OBJECT2,
    RS_C2S_OBJECT3,
    RS_C2S_OBJECT4,
    RS_C2S_OBJECT5,
    RS_C2S_GROUND_ITEM,
    RS_C2S_ATTACK_PLAYER,
    RS_C2S_FOLLOW,
    RS_C2S_TRADE,
    RS_C2S_ADD_FRIEND,
    RS_C2S_DEL_FRIEND,
    RS_C2S_ADD_IGNORE,
    RS_C2S_DEL_IGNORE,
    RS_C2S_CHAT_MODE,
    RS_C2S_IDLE_LOGOUT,
    RS_C2S_WINDOW_STATUS,
    RS_C2S_CLAN_JOIN,
    RS_C2S_CLAN_KICK,
    RS_C2S_FRIEND_RANK,
    RS_C2S_RESUME_STRING,
    RS_C2S_MAGIC_ON_GROUND,
    RS_C2S_OPHELD,

    /* server -> client */
    RS_S2C_PLAYER_UPDATE,
    RS_S2C_NPC_UPDATE,
    RS_S2C_NPC_UPDATE_LARGE,
    RS_S2C_REGION,
    RS_S2C_REGION_INSTANCED,
    RS_S2C_LOGOUT,
    RS_S2C_MESSAGE,
    RS_S2C_SKILL,
    RS_S2C_ENERGY,
    RS_S2C_WEIGHT,
    RS_S2C_VARP_SMALL,
    RS_S2C_VARP_LARGE,
    RS_S2C_TAB,
    RS_S2C_SIDEBAR,
    RS_S2C_IF_OPEN,
    RS_S2C_IF_CLOSE,
    RS_S2C_IF_TEXT,
    RS_S2C_IF_OPENTOP,
    RS_S2C_IF_OPENSUB,
    RS_S2C_IF_CLOSESUB,
    RS_S2C_INVENTORY,
    RS_S2C_INVENTORY_UPDATE,
    RS_S2C_SYSTEM_UPDATE,
    RS_S2C_MINIMAP,
    RS_S2C_HINT,
    RS_S2C_CAMERA_RESET,
    RS_S2C_CAMERA_MOVE,
    RS_S2C_PLAYER_OPTION,
    RS_S2C_CHAT_SETTINGS,
    RS_S2C_FRIEND_STATUS,
    RS_S2C_FRIEND_LIST,
    RS_S2C_IGNORE_LIST,
    RS_S2C_PRIVATE_MESSAGE,
    RS_S2C_RESET_ANIMS,
    RS_S2C_PROJECTILE,
    RS_S2C_SONG,
    RS_S2C_SOUND,
    RS_S2C_PLAYER_INDEX,
    RS_S2C_WALKABLE_INTERFACE,
    RS_S2C_MULTIZONE,
    RS_S2C_NO_TIMEOUT,
    RS_S2C_SCRIPT,
    RS_S2C_OBJ_ADD,
    RS_S2C_OBJ_DEL,
    RS_S2C_LOC_ADD,
    RS_S2C_LOC_DEL,
    RS_S2C_ZONE_FULL,
    RS_S2C_ZONE_PARTIAL,
    RS_S2C_ZONE_BASE,
    RS_S2C_IF_SETANIM,
    RS_S2C_IF_SETHIDE,
    RS_S2C_IF_SETOBJECT,
    RS_S2C_IF_SETNPCHEAD,
    RS_S2C_IF_SETPLAYERHEAD,
    RS_S2C_IF_SETMODEL,
    RS_S2C_IF_SETCOLOUR,
    RS_S2C_IF_SETPOS,
    RS_S2C_IF_SETSCROLL,
    RS_S2C_IF_SETANGLE,
    RS_S2C_VARBIT_SMALL,
    RS_S2C_VARBIT_LARGE,
    RS_S2C_MIDI_JINGLE,
    RS_S2C_CAM_SHAKE,
    RS_S2C_CAM_LOOKAT,
    RS_S2C_INV_CLEAR,
    RS_S2C_SPOTANIM,
    RS_S2C_LOC_ANIM,
    RS_S2C_SOUND_AREA,
    RS_S2C_UPDATE_UID,
    RS_S2C_LAST_LOGIN,
    RS_S2C_CLAN,
    RS_S2C_FRIEND_LOADED,
    RS_S2C_GE_OFFER,

    /* 317 component iop. Client.java useMenuOption actions 632, 78, 867, 431, 53. */
    RS_C2S_IF_OPTION1,
    RS_C2S_IF_OPTION2,
    RS_C2S_IF_OPTION3,
    RS_C2S_IF_OPTION4,
    RS_C2S_IF_OPTION5,
    /* 317 packet 27. Empty. Client.java sets chatbackInputOpen and draws "Enter amount:". */
    RS_S2C_AMOUNT_PROMPT,

    RS_PKT_MAX
} rs_pkt;

const char *rs_pkt_name(rs_pkt id);
rs_pkt rs_pkt_lookup(const char *name);
rs_pkt_dir rs_pkt_direction(rs_pkt id);

/* Opcode for this build, or -1 if unbound. Prefer the session helpers. */
int rs_pkt_opcode(const rs_build_info *build, rs_pkt id);
rs_pkt rs_pkt_from_opcode(const rs_build_info *build, rs_pkt_dir dir, int opcode);
int rs_session_pkt_opcode(const rs_session *s, rs_pkt id);
rs_pkt rs_session_pkt_id(const rs_session *s, int opcode);

/* Override or fill in a later revision. */
rs_err rs_pkt_bind(int revision, rs_pkt id, int opcode);
/* Copy every bound opcode from `src_revision` onto `dst_revision`. */
rs_err rs_pkt_copy(int dst_revision, int src_revision);

rs_err rs_session_write_pkt(rs_session *s, rs_buf *dst, rs_pkt id, const uint8_t *payload,
                            size_t n);
rs_err rs_session_read_pkt(rs_session *s, rs_buf *src, rs_pkt *id, int *opcode, rs_buf *payload);

#ifdef __cplusplus
}
#endif

#endif
