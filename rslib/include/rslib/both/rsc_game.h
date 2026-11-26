#ifndef RSLIB_RSC_GAME_H
#define RSLIB_RSC_GAME_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Decrypted opcodes. Client.beenAddedFriendsList and MudClient.size compare
 * these after Stream.g1spooky. rs_rsc_read returns the on-wire byte, which is
 * still encrypted. The same numbers are used in revisions 115, 159, and 177
 * for every payload implemented below.
 *
 * Payloads are the bytes after that opcode. rsc.c already frames length.
 * 115, 159, and 177 share a layout unless a comment says otherwise.
 */
enum rs_rsc_op {
    RS_RSC_OP_MESSAGE = 8,          /* Client.beenAddedFriendsList -> bor */
    RS_RSC_OP_CLOSE_SOCKET = 9,     /* Client.beenAddedFriendsList -> callde55a5 */
    RS_RSC_OP_LOGOUT = 10,          /* Client.beenAddedFriendsList -> tryLogout */
    RS_RSC_OP_FRIEND_LIST = 23,     /* Client.beenAddedFriendsList */
    RS_RSC_OP_FRIEND_UPDATE = 24,   /* Client.beenAddedFriendsList */
    RS_RSC_OP_IGNORE_LIST = 26,     /* Client.beenAddedFriendsList */
    RS_RSC_OP_PRIVACY = 27,         /* Client.beenAddedFriendsList */
    RS_RSC_OP_PRIVATE_MESSAGE = 28, /* Client.beenAddedFriendsList */
    RS_RSC_OP_SLEEP_WRONG = 198,    /* MudClient.size */
    RS_RSC_OP_UI_CLOSE = 199,       /* MudClient.size */
    RS_RSC_OP_UI_VALUE = 200,       /* MudClient.size, one short */
    RS_RSC_OP_UI_OPEN = 201,        /* MudClient.size -> drawArea.call378 */
    RS_RSC_OP_UI_VALUE2 = 202,      /* MudClient.size, one short */
    RS_RSC_OP_203 = 203,            /* MudClient.size, raw string, flag97 set */
    RS_RSC_OP_204 = 204,            /* MudClient.size, raw string, flag97 clear */
    RS_RSC_OP_COORDS3 = 205,        /* MudClient.size, three ints */
    RS_RSC_OP_EFFECT = 206,         /* MudClient.size */
    RS_RSC_OP_SOUND = 207,          /* MudClient.size -> pcm */
    RS_RSC_OP_DUEL_CONFIRM = 208,   /* MudClient.size */
    RS_RSC_OP_DUEL_FLAG2 = 209,     /* MudClient.size */
    RS_RSC_OP_DUEL_FLAG1 = 210,     /* MudClient.size */
    RS_RSC_OP_STAT = 211,           /* MudClient.size */
    RS_RSC_OP_INV_REMOVE = 212,     /* MudClient.size */
    RS_RSC_OP_INV_SLOT = 213,       /* MudClient.size */
    RS_RSC_OP_BANK_SLOT = 214,      /* MudClient.size */
    RS_RSC_OP_DUEL_OPTIONS = 215,   /* MudClient.size */
    RS_RSC_OP_DUEL_OFFER = 216,     /* MudClient.size */
    RS_RSC_OP_217 = 217,            /* MudClient.size -> thereWAYReverseTrade */
    RS_RSC_OP_DUEL_CLOSE = 218,     /* MudClient.size */
    RS_RSC_OP_DUEL_OPEN = 219,      /* MudClient.size */
    RS_RSC_OP_STAT_XP = 220,        /* MudClient.size */
    RS_RSC_OP_BANK_CLOSE = 221,     /* MudClient.size */
    RS_RSC_OP_BANK_OPEN = 222,      /* MudClient.size */
    RS_RSC_OP_RECOVERY = 224,       /* MudClient.size */
    RS_RSC_OP_QUESTS = 226,         /* MudClient.size */
    RS_RSC_OP_PRAYERS = 227,        /* MudClient.size */
    RS_RSC_OP_SETTINGS = 228,       /* MudClient.size */
    RS_RSC_OP_TRADE_FLAG = 229,     /* MudClient.size */
    RS_RSC_OP_CONTACT = 232,        /* MudClient.size */
    RS_RSC_OP_SHOP_CLOSE = 233,     /* MudClient.size */
    RS_RSC_OP_SHOP = 234,           /* MudClient.size */
    RS_RSC_OP_TRADE_ACCEPT = 235,   /* MudClient.size */
    RS_RSC_OP_TRADE_OFFER = 236,    /* MudClient.size */
    RS_RSC_OP_TRADE_CLOSE = 237,    /* MudClient.size */
    RS_RSC_OP_TRADE_OPEN = 238,     /* MudClient.size */
    RS_RSC_OP_SLEEP = 239,          /* MudClient.size */
    RS_RSC_OP_SECTORS = 240,        /* MudClient.size */
    RS_RSC_OP_241 = 241,            /* MudClient.size, sets a timer, no payload read */
    RS_RSC_OP_BONUSES = 242,        /* MudClient.size, five bytes */
    RS_RSC_OP_STATS = 243,          /* MudClient.size. 115 reads 16 skills, 159 and 177 read 18 */
    RS_RSC_OP_SELF = 244,           /* MudClient.size */
    RS_RSC_OP_245 = 245,            /* MudClient.size, clears the string list opened by 246 */
    RS_RSC_OP_246 = 246,            /* MudClient.size, count plus length-prefixed strings */
    RS_RSC_OP_NPC_AUX = 247,        /* MudClient.size */
    RS_RSC_OP_NPCS = 248,           /* MudClient.size */
    RS_RSC_OP_WALLS = 249,          /* MudClient.size */
    RS_RSC_OP_PLAYERS_AUX = 250,    /* MudClient.size */
    RS_RSC_OP_INVENTORY = 252,      /* MudClient.size */
    RS_RSC_OP_OBJECTS = 253,        /* MudClient.size */
    RS_RSC_OP_GROUND_ITEMS = 254,   /* MudClient.size */
    RS_RSC_OP_POSITION = 255        /* MudClient.size */
};

enum { RS_RSC_MESSAGE_MAX = 255 };

/* Opcode 8. Raw bytes, not a newline jagex string. Same on 115, 159, and 177. */
rs_err rs_rsc_message_decode(int revision, rs_buf *src, char *dst, size_t cap);
rs_err rs_rsc_message_encode(int revision, rs_buf *dst, const char *text);

/*
 * Opcode 244. Five big-endian shorts, same on 115, 159, and 177.
 * index is the local server index opcode 255 passes to call98fe3b.
 * The client then does area_y -= mul_a * mul_b. These fields are the wire values.
 */
typedef struct rs_rsc_self {
    int index;
    int area_x;
    int area_y;
    int mul_a;
    int mul_b;
} rs_rsc_self;

rs_err rs_rsc_self_decode(int revision, rs_buf *src, rs_rsc_self *out);
rs_err rs_rsc_self_encode(int revision, rs_buf *dst, const rs_rsc_self *in);

/*
 * Opcode 255. One step record per other player the client already had.
 * MudClient applies known[i] to the previous player list at index i+1
 * (index 0 is the local player). A 3-bit walk moves one tile:
 *   0 y-1, 1 x+1 y-1, 2 x+1, 3 x+1 y+1, 4 y+1, 5 x-1 y+1, 6 x-1, 7 x-1 y-1
 * in 128-pixel steps. FACE stores a 4-bit facing (0..11) and does not step.
 * Values 12..15 are the remove encoding and are not a facing.
 *
 * region_x/region_y are the raw bitfields. 115 is 10 and 12 bits.
 * 159 and 177 are 11 and 13 bits. The client subtracts the loaded-area origin
 * (not in this packet) and stores pixel = (region - origin) * 128 + 64.
 * added[].known is the raw bit. 0 makes the client queue an appearance request.
 */
enum {
    RS_RSC_STEP_KEEP = 0,
    RS_RSC_STEP_WALK = 1,
    RS_RSC_STEP_REMOVE = 2,
    RS_RSC_STEP_FACE = 3,
    RS_RSC_KNOWN_MAX = 255,
    RS_RSC_ADDED_MAX = 255
};

typedef struct rs_rsc_step {
    int kind;
    int dir;
} rs_rsc_step;

typedef struct rs_rsc_added_player {
    int index;
    int dx;
    int dy;
    int dir;
    int known;
} rs_rsc_added_player;

typedef struct rs_rsc_position {
    int region_x;
    int region_y;
    int dir;
    int known_count;
    rs_rsc_step known[RS_RSC_KNOWN_MAX];
    int added_count;
    rs_rsc_added_player added[RS_RSC_ADDED_MAX];
} rs_rsc_position;

rs_err rs_rsc_position_decode(int revision, rs_buf *src, rs_rsc_position *out);
rs_err rs_rsc_position_encode(int revision, rs_buf *dst, const rs_rsc_position *in);

/*
 * Opcode 250. Same record layout on 115, 159, and 177.
 * APPEARANCE sprites are the 12 worn ids the client stores (unread slots become 0).
 * hair/top/leg/skin are the colour indexes passed to the sprite drawers.
 * skull is number317; the client draws the skull when it is 1.
 * CHAT and CHAT_OWN are the WordPack bytes, not the displayed string.
 * CHAT is filtered by WordFilter; CHAT_OWN is not.
 * COMBAT is the damage byte plus current and max hits (skill slot 3 on the local player).
 * PROJ_NPC / PROJ_PLAYER are a projectile sprite id and a target index.
 * HITS is the hitsplat value (number301).
 *
 * The client skip used when the server index is unknown is 2 bytes short of this
 * appearance record. This codec writes the record the client stores when the
 * player exists.
 */
enum {
    RS_RSC_AUX_HITS = 0,
    RS_RSC_AUX_CHAT = 1,
    RS_RSC_AUX_COMBAT = 2,
    RS_RSC_AUX_PROJ_NPC = 3,
    RS_RSC_AUX_PROJ_PLAYER = 4,
    RS_RSC_AUX_APPEARANCE = 5,
    RS_RSC_AUX_CHAT_OWN = 6,
    RS_RSC_SPRITES = 12,
    RS_RSC_CHAT_MAX = 127
};

typedef struct rs_rsc_player_aux {
    int index;
    int type;
    int a;
    int b;
    int c;
    int appearance_id;
    uint64_t name37;
    char name[13];
    int nsprites;
    uint8_t sprites[RS_RSC_SPRITES];
    int hair_colour;
    int top_colour;
    int leg_colour;
    int skin_colour;
    int combat_level;
    int skull;
    int chat_len;
    uint8_t chat[RS_RSC_CHAT_MAX];
} rs_rsc_player_aux;

typedef struct rs_rsc_player_aux_list {
    int count;
    int cap;
    rs_rsc_player_aux *row;
} rs_rsc_player_aux_list;

rs_err rs_rsc_player_aux_decode(int revision, rs_buf *src, rs_rsc_player_aux_list *out);
rs_err rs_rsc_player_aux_encode(int revision, rs_buf *dst, const rs_rsc_player_aux_list *in);

/*
 * Opcode 252, and opcode 213 for one slot. The client array is 35 slots
 * (value30 is 30). Same on 115, 159, and 177.
 * has_amount is nonzero when the config byte tested in MudClient.size is 0
 * (177 ClientConfig.objStackable, 159 numberArray483, 115 numberArray440).
 * Only then is a gsmart amount on the wire. Otherwise the client stores 1.
 * Decode needs the callback because the id is not known until it is read.
 * Encode uses each item's has_amount field.
 */
enum { RS_RSC_INV_MAX = 35 };

typedef int (*rs_rsc_has_amount_fn)(int item_id, void *user);

typedef struct rs_rsc_inv_item {
    int id;
    int equipped;
    int amount;
    int has_amount;
} rs_rsc_inv_item;

typedef struct rs_rsc_inventory {
    int count;
    rs_rsc_inv_item item[RS_RSC_INV_MAX];
} rs_rsc_inventory;

rs_err rs_rsc_inventory_decode(int revision, rs_buf *src, rs_rsc_inventory *out,
                               rs_rsc_has_amount_fn has_amount, void *user);
rs_err rs_rsc_inventory_encode(int revision, rs_buf *dst, const rs_rsc_inventory *in);

rs_err rs_rsc_inv_slot_decode(int revision, rs_buf *src, int *slot, rs_rsc_inv_item *out,
                              rs_rsc_has_amount_fn has_amount, void *user);
rs_err rs_rsc_inv_slot_encode(int revision, rs_buf *dst, int slot, const rs_rsc_inv_item *in);

/* Opcode 212. One byte, the slot shifted out. Same on 115, 159, and 177. */
rs_err rs_rsc_inv_remove_decode(int revision, rs_buf *src, int *slot);
rs_err rs_rsc_inv_remove_encode(int revision, rs_buf *dst, int slot);

/*
 * Opcode 248. known[i] updates the previous NPC list at index i (no local skip).
 * 115 new NPCs are 11-bit index, 8-bit type, and the client loops while
 * 31 bits remain. 159 and 177 are 12-bit index, 10-bit type, and 34 bits.
 * dx/dy are signed 5-bit offsets from the player's region. The client stores
 * pixel = (region + delta) * 128 + 64. It rewrites type to 24 when type >= npcCount.
 * That config value is not in the packet, so type is left raw.
 */
enum { RS_RSC_NPC_ADDED_MAX = 255 };

typedef struct rs_rsc_added_npc {
    int index;
    int dx;
    int dy;
    int dir;
    int type;
} rs_rsc_added_npc;

typedef struct rs_rsc_npcs {
    int known_count;
    rs_rsc_step known[RS_RSC_KNOWN_MAX];
    int added_count;
    rs_rsc_added_npc added[RS_RSC_NPC_ADDED_MAX];
} rs_rsc_npcs;

rs_err rs_rsc_npcs_decode(int revision, rs_buf *src, rs_rsc_npcs *out);
rs_err rs_rsc_npcs_encode(int revision, rs_buf *dst, const rs_rsc_npcs *in);

/*
 * Opcode 254 ground items, 253 scenery, 249 walls. Same on 115, 159, and 177.
 * x/y are signed byte offsets from the player's region base. CLEAR's x/y are
 * the two bytes after 0xFF; the client drops entities in that 8-tile sector.
 * Item REMOVE is the id with bit 0x8000 set. Ids 32512..32767 cannot be removed
 * this way: that short starts with 0xFF and the client treats it as CLEAR.
 * Object REMOVE is id 60000, which deletes the loc at x,y and does not add one.
 * Object facing is not in the packet. The client reads it from the landscape.
 * Item elevation is not in the packet. The client copies it from a loc on the tile.
 * Wall id 65535 is in the source as "do not add", but that short is 0xFFFF, which
 * the 0xFF test already consumed as CLEAR. It is not a wire value.
 */
enum {
    RS_RSC_LOC_ADD = 0,
    RS_RSC_LOC_REMOVE = 1,
    RS_RSC_LOC_CLEAR = 2,
    RS_RSC_LOC_MAX = 512
};

typedef struct rs_rsc_loc {
    int kind;
    int id;
    int x;
    int y;
    int dir;
} rs_rsc_loc;

typedef struct rs_rsc_locs {
    int count;
    rs_rsc_loc loc[RS_RSC_LOC_MAX];
} rs_rsc_locs;

rs_err rs_rsc_ground_items_decode(int revision, rs_buf *src, rs_rsc_locs *out);
rs_err rs_rsc_ground_items_encode(int revision, rs_buf *dst, const rs_rsc_locs *in);
rs_err rs_rsc_objects_decode(int revision, rs_buf *src, rs_rsc_locs *out);
rs_err rs_rsc_objects_encode(int revision, rs_buf *dst, const rs_rsc_locs *in);
rs_err rs_rsc_walls_decode(int revision, rs_buf *src, rs_rsc_locs *out);
rs_err rs_rsc_walls_encode(int revision, rs_buf *dst, const rs_rsc_locs *in);

/*
 * Opcode 240. Pairs of signed shorts. The client adds each pair to the region
 * base and drops ground items, scenery, and walls in that sector (>> 3).
 * A trailing 1..3 bytes are ignored, matching (length - 1) / 4. Same on 115, 159, and 177.
 */
enum { RS_RSC_SECTOR_MAX = 256 };

typedef struct rs_rsc_sector {
    int dx;
    int dy;
} rs_rsc_sector;

typedef struct rs_rsc_sectors {
    int count;
    rs_rsc_sector sector[RS_RSC_SECTOR_MAX];
} rs_rsc_sectors;

rs_err rs_rsc_sectors_decode(int revision, rs_buf *src, rs_rsc_sectors *out);
rs_err rs_rsc_sectors_encode(int revision, rs_buf *dst, const rs_rsc_sectors *in);

/*
 * Opcodes with no payload. 198, 199, and 201 exist on 177 only.
 * 202-207 and 232 exist on 159 and 177. The others are on 115, 159, and 177.
 * Opcode 241 assigns the constant 250 and reads no bytes.
 * Opcode 224 opens the recovery questions and reads no bytes.
 * Opcode 245 clears the opcode 246 list.
 */
rs_err rs_rsc_close_socket_decode(int revision, rs_buf *src);
rs_err rs_rsc_close_socket_encode(int revision, rs_buf *dst);
rs_err rs_rsc_logout_decode(int revision, rs_buf *src);
rs_err rs_rsc_logout_encode(int revision, rs_buf *dst);
rs_err rs_rsc_sleep_wrong_decode(int revision, rs_buf *src);
rs_err rs_rsc_sleep_wrong_encode(int revision, rs_buf *dst);
rs_err rs_rsc_ui_close_decode(int revision, rs_buf *src);
rs_err rs_rsc_ui_close_encode(int revision, rs_buf *dst);
rs_err rs_rsc_sleep_decode(int revision, rs_buf *src);
rs_err rs_rsc_sleep_encode(int revision, rs_buf *dst);
rs_err rs_rsc_trade_close_decode(int revision, rs_buf *src);
rs_err rs_rsc_trade_close_encode(int revision, rs_buf *dst);
rs_err rs_rsc_shop_close_decode(int revision, rs_buf *src);
rs_err rs_rsc_shop_close_encode(int revision, rs_buf *dst);
rs_err rs_rsc_bank_close_decode(int revision, rs_buf *src);
rs_err rs_rsc_bank_close_encode(int revision, rs_buf *dst);
rs_err rs_rsc_duel_close_decode(int revision, rs_buf *src);
rs_err rs_rsc_duel_close_encode(int revision, rs_buf *dst);
rs_err rs_rsc_timer_decode(int revision, rs_buf *src);
rs_err rs_rsc_timer_encode(int revision, rs_buf *dst);
rs_err rs_rsc_strings_close_decode(int revision, rs_buf *src);
rs_err rs_rsc_strings_close_encode(int revision, rs_buf *dst);
rs_err rs_rsc_recovery_decode(int revision, rs_buf *src);
rs_err rs_rsc_recovery_encode(int revision, rs_buf *dst);
rs_err rs_rsc_contact_decode(int revision, rs_buf *src);
rs_err rs_rsc_contact_encode(int revision, rs_buf *dst);

/* Opcode 207. Raw bytes, same shape as opcode 8. 159 and 177. */
rs_err rs_rsc_sound_decode(int revision, rs_buf *src, char *dst, size_t cap);
rs_err rs_rsc_sound_encode(int revision, rs_buf *dst, const char *text);

/*
 * Opcodes 203 and 204. Same raw string. 203 sets flag97 and 204 clears it.
 * That flag is which opcode arrived, not a payload byte. 159 and 177.
 */
rs_err rs_rsc_ui_string_decode(int revision, rs_buf *src, char *dst, size_t cap);
rs_err rs_rsc_ui_string_encode(int revision, rs_buf *dst, const char *text);

/* Opcode 200. One short, number448. 177 only. */
rs_err rs_rsc_ui_value_decode(int revision, rs_buf *src, int *value);
rs_err rs_rsc_ui_value_encode(int revision, rs_buf *dst, int value);

/* Opcode 202. One short, number447 on 177 and number417 on 159. */
rs_err rs_rsc_ui_value2_decode(int revision, rs_buf *src, int *value);
rs_err rs_rsc_ui_value2_encode(int revision, rs_buf *dst, int value);

/*
 * Opcode 201. PixMap.call378, 177 only. 255 by 40.
 * pixel 0 is client colour 0 and pixel 1 is 0xFFFFFF.
 * A run that would cross a row is RS_ERR_FORMAT.
 */
enum { RS_RSC_SLEEP_W = 255, RS_RSC_SLEEP_H = 40 };

typedef struct rs_rsc_sleep_image {
    uint8_t pixel[RS_RSC_SLEEP_W * RS_RSC_SLEEP_H];
} rs_rsc_sleep_image;

rs_err rs_rsc_sleep_image_decode(int revision, rs_buf *src, rs_rsc_sleep_image *out);
rs_err rs_rsc_sleep_image_encode(int revision, rs_buf *dst, const rs_rsc_sleep_image *in);

/*
 * Opcode 205. Three big-endian ints, 159 and 177.
 * 177 stores days since login, the recovery-question timer, then the host
 * int passed to getHostName. The random 0..5 beside them is not on the wire.
 */
typedef struct rs_rsc_login_box {
    int days_ago;
    int recovery_days;
    uint32_t host;
} rs_rsc_login_box;

rs_err rs_rsc_login_box_decode(int revision, rs_buf *src, rs_rsc_login_box *out);
rs_err rs_rsc_login_box_encode(int revision, rs_buf *dst, const rs_rsc_login_box *in);

/*
 * Opcode 206. Effect id and signed offsets from the player's region. 159 and 177.
 * The client drops the effect when 50 are already showing. That cap is not on the wire.
 */
typedef struct rs_rsc_effect {
    int id;
    int x;
    int y;
} rs_rsc_effect;

rs_err rs_rsc_effect_decode(int revision, rs_buf *src, rs_rsc_effect *out);
rs_err rs_rsc_effect_encode(int revision, rs_buf *dst, const rs_rsc_effect *in);

/* Opcode 23. Count, then name37 and world. The client array is 100. */
enum { RS_RSC_FRIEND_MAX = 100 };

typedef struct rs_rsc_friend {
    uint64_t name37;
    int world;
} rs_rsc_friend;

typedef struct rs_rsc_friends {
    int count;
    rs_rsc_friend friend[RS_RSC_FRIEND_MAX];
} rs_rsc_friends;

rs_err rs_rsc_friends_decode(int revision, rs_buf *src, rs_rsc_friends *out);
rs_err rs_rsc_friends_encode(int revision, rs_buf *dst, const rs_rsc_friends *in);

/* Opcode 24. One friend. World is the unsigned byte at offset 9. */
rs_err rs_rsc_friend_update_decode(int revision, rs_buf *src, rs_rsc_friend *out);
rs_err rs_rsc_friend_update_encode(int revision, rs_buf *dst, const rs_rsc_friend *in);

/* Opcode 26. Count, then name37. The client array is 50. */
enum { RS_RSC_IGNORE_MAX = 50 };

typedef struct rs_rsc_ignores {
    int count;
    uint64_t name37[RS_RSC_IGNORE_MAX];
} rs_rsc_ignores;

rs_err rs_rsc_ignores_decode(int revision, rs_buf *src, rs_rsc_ignores *out);
rs_err rs_rsc_ignores_encode(int revision, rs_buf *dst, const rs_rsc_ignores *in);

/*
 * Opcode 27. Four mode bytes: chat, private, trade, duel.
 * The client assigns the signed byte. Values 0..127 match that store.
 */
typedef struct rs_rsc_privacy {
    int chat;
    int priv;
    int trade;
    int duel;
} rs_rsc_privacy;

rs_err rs_rsc_privacy_decode(int revision, rs_buf *src, rs_rsc_privacy *out);
rs_err rs_rsc_privacy_encode(int revision, rs_buf *dst, const rs_rsc_privacy *in);

/*
 * Opcode 28. name37, then the remaining bytes as WordPack.
 * text is caller storage. len is set to the byte count.
 */
typedef struct rs_rsc_private_message {
    uint64_t from37;
    int len;
    int cap;
    uint8_t *text;
} rs_rsc_private_message;

rs_err rs_rsc_private_message_decode(int revision, rs_buf *src, rs_rsc_private_message *out);
rs_err rs_rsc_private_message_encode(int revision, rs_buf *dst, const rs_rsc_private_message *in);

/*
 * Opcode 243. Current, base, then experience, then one quest-point byte.
 * 115 reads 16 skills. 159 and 177 read 18.
 */
enum { RS_RSC_SKILL_MAX = 18 };

typedef struct rs_rsc_stats {
    int nskills;
    int cur[RS_RSC_SKILL_MAX];
    int base[RS_RSC_SKILL_MAX];
    uint32_t xp[RS_RSC_SKILL_MAX];
    int quest_points;
} rs_rsc_stats;

rs_err rs_rsc_stats_decode(int revision, rs_buf *src, rs_rsc_stats *out);
rs_err rs_rsc_stats_encode(int revision, rs_buf *dst, const rs_rsc_stats *in);

/* Opcode 211. One skill. index is checked against that revision's skill count. */
typedef struct rs_rsc_stat {
    int index;
    int cur;
    int base;
    uint32_t xp;
} rs_rsc_stat;

rs_err rs_rsc_stat_decode(int revision, rs_buf *src, rs_rsc_stat *out);
rs_err rs_rsc_stat_encode(int revision, rs_buf *dst, const rs_rsc_stat *in);

/* Opcode 220. Experience only. */
typedef struct rs_rsc_stat_xp {
    int index;
    uint32_t xp;
} rs_rsc_stat_xp;

rs_err rs_rsc_stat_xp_decode(int revision, rs_buf *src, rs_rsc_stat_xp *out);
rs_err rs_rsc_stat_xp_encode(int revision, rs_buf *dst, const rs_rsc_stat_xp *in);

/* Opcode 242. Armour, weapon aim, weapon power, magic, prayer. Unsigned bytes. */
enum { RS_RSC_BONUS_MAX = 5 };

typedef struct rs_rsc_bonuses {
    int value[RS_RSC_BONUS_MAX];
} rs_rsc_bonuses;

rs_err rs_rsc_bonuses_decode(int revision, rs_buf *src, rs_rsc_bonuses *out);
rs_err rs_rsc_bonuses_encode(int revision, rs_buf *dst, const rs_rsc_bonuses *in);

/*
 * Opcode 227. One byte per prayer, 1 means on. The count is the payload length.
 * The client array is 50 on 115, 159, and 177.
 */
enum { RS_RSC_PRAYER_MAX = 50 };

typedef struct rs_rsc_prayers {
    int count;
    uint8_t on[RS_RSC_PRAYER_MAX];
} rs_rsc_prayers;

rs_err rs_rsc_prayers_decode(int revision, rs_buf *src, rs_rsc_prayers *out);
rs_err rs_rsc_prayers_encode(int revision, rs_buf *dst, const rs_rsc_prayers *in);

/*
 * Opcode 226. One byte per quest, 1 means complete.
 * 115 reads 17, 159 reads 39, 177 reads 50.
 */
enum { RS_RSC_QUEST_MAX = 50 };

typedef struct rs_rsc_quests {
    int count;
    uint8_t done[RS_RSC_QUEST_MAX];
} rs_rsc_quests;

rs_err rs_rsc_quests_decode(int revision, rs_buf *src, rs_rsc_quests *out);
rs_err rs_rsc_quests_encode(int revision, rs_buf *dst, const rs_rsc_quests *in);

/*
 * Opcode 228. A byte of 1 sets the option.
 * 115 reads camera and mouse. 159 and 177 also read sound.
 */
typedef struct rs_rsc_settings {
    int camera;
    int mouse;
    int sound;
} rs_rsc_settings;

rs_err rs_rsc_settings_decode(int revision, rs_buf *src, rs_rsc_settings *out);
rs_err rs_rsc_settings_encode(int revision, rs_buf *dst, const rs_rsc_settings *in);

/*
 * Opcode 247. Same on 115, 159, and 177.
 * Type 1: a player server index (compared with the local player, not stored on
 * the npc), a signed length 0..127, and WordPack bytes.
 * Type 2: damage, current hits, max hits.
 * Any other type has no further bytes.
 */
enum { RS_RSC_NPC_AUX_CHAT = 1, RS_RSC_NPC_AUX_HITS = 2 };

typedef struct rs_rsc_npc_aux {
    int index;
    int type;
    int player;
    int chat_len;
    uint8_t chat[RS_RSC_CHAT_MAX];
    int damage;
    int current;
    int max;
} rs_rsc_npc_aux;

typedef struct rs_rsc_npc_aux_list {
    int count;
    int cap;
    rs_rsc_npc_aux *row;
} rs_rsc_npc_aux_list;

rs_err rs_rsc_npc_aux_decode(int revision, rs_buf *src, rs_rsc_npc_aux_list *out);
rs_err rs_rsc_npc_aux_encode(int revision, rs_buf *dst, const rs_rsc_npc_aux_list *in);

/* Shared id + amount. Amount is the unsigned int Tools.g4 reads. */
typedef struct rs_rsc_stack {
    int id;
    uint32_t amount;
} rs_rsc_stack;

enum { RS_RSC_TRADE_MAX = 14, RS_RSC_DUEL_MAX = 8 };

/* Opcode 236. The other player's trade offer. At most 14. */
typedef struct rs_rsc_trade_offer {
    int count;
    rs_rsc_stack item[RS_RSC_TRADE_MAX];
} rs_rsc_trade_offer;

rs_err rs_rsc_trade_offer_decode(int revision, rs_buf *src, rs_rsc_trade_offer *out);
rs_err rs_rsc_trade_offer_encode(int revision, rs_buf *dst, const rs_rsc_trade_offer *in);

/* Opcode 216. The other player's duel stake. At most 8. */
typedef struct rs_rsc_duel_offer {
    int count;
    rs_rsc_stack item[RS_RSC_DUEL_MAX];
} rs_rsc_duel_offer;

rs_err rs_rsc_duel_offer_decode(int revision, rs_buf *src, rs_rsc_duel_offer *out);
rs_err rs_rsc_duel_offer_encode(int revision, rs_buf *dst, const rs_rsc_duel_offer *in);

/* Opcode 238 and 219. One short, an entity-array index. */
rs_err rs_rsc_trade_open_decode(int revision, rs_buf *src, int *player);
rs_err rs_rsc_trade_open_encode(int revision, rs_buf *dst, int player);
rs_err rs_rsc_duel_open_decode(int revision, rs_buf *src, int *player);
rs_err rs_rsc_duel_open_encode(int revision, rs_buf *dst, int player);

/*
 * One byte compared with 1.
 * 235 is the other player accepting the trade. 229 is the local accept.
 * 210 is the other player accepting the duel. 209 is the local accept.
 */
rs_err rs_rsc_trade_other_decode(int revision, rs_buf *src, int *accepted);
rs_err rs_rsc_trade_other_encode(int revision, rs_buf *dst, int accepted);
rs_err rs_rsc_trade_accept_decode(int revision, rs_buf *src, int *accepted);
rs_err rs_rsc_trade_accept_encode(int revision, rs_buf *dst, int accepted);
rs_err rs_rsc_duel_other_decode(int revision, rs_buf *src, int *accepted);
rs_err rs_rsc_duel_other_encode(int revision, rs_buf *dst, int accepted);
rs_err rs_rsc_duel_accept_decode(int revision, rs_buf *src, int *accepted);
rs_err rs_rsc_duel_accept_encode(int revision, rs_buf *dst, int accepted);

/* Opcode 215. No retreating, no magic, no prayer, no weapons. */
typedef struct rs_rsc_duel_options {
    int retreat;
    int magic;
    int prayer;
    int weapons;
} rs_rsc_duel_options;

rs_err rs_rsc_duel_options_decode(int revision, rs_buf *src, rs_rsc_duel_options *out);
rs_err rs_rsc_duel_options_encode(int revision, rs_buf *dst, const rs_rsc_duel_options *in);

/*
 * Opcode 217. Name, then the items you receive, then the items you give.
 * Each list is at most 14.
 */
typedef struct rs_rsc_trade_confirm {
    uint64_t name37;
    int receive_count;
    rs_rsc_stack receive[RS_RSC_TRADE_MAX];
    int give_count;
    rs_rsc_stack give[RS_RSC_TRADE_MAX];
} rs_rsc_trade_confirm;

rs_err rs_rsc_trade_confirm_decode(int revision, rs_buf *src, rs_rsc_trade_confirm *out);
rs_err rs_rsc_trade_confirm_encode(int revision, rs_buf *dst, const rs_rsc_trade_confirm *in);

/*
 * Opcode 208. Name, opponent stake, your stake, then four rule bytes.
 * 0 means that action is allowed. Each stake is at most 8.
 */
typedef struct rs_rsc_duel_confirm {
    uint64_t name37;
    int opponent_count;
    rs_rsc_stack opponent[RS_RSC_DUEL_MAX];
    int self_count;
    rs_rsc_stack self[RS_RSC_DUEL_MAX];
    int retreat;
    int magic;
    int prayer;
    int weapons;
} rs_rsc_duel_confirm;

rs_err rs_rsc_duel_confirm_decode(int revision, rs_buf *src, rs_rsc_duel_confirm *out);
rs_err rs_rsc_duel_confirm_encode(int revision, rs_buf *dst, const rs_rsc_duel_confirm *in);

/*
 * Opcode 234. general is the byte after count. The client copies inventory
 * into empty slots only when that byte is 1. Those copies are not in the packet.
 * sell_mod and buy_mod are the next two bytes. price_mod is a signed byte
 * added to each of them. stock is the short drawn in green. The client array is 256.
 */
enum { RS_RSC_SHOP_MAX = 255 };

typedef struct rs_rsc_shop_item {
    int id;
    int stock;
    int price_mod;
} rs_rsc_shop_item;

typedef struct rs_rsc_shop {
    int count;
    int general;
    int sell_mod;
    int buy_mod;
    rs_rsc_shop_item item[RS_RSC_SHOP_MAX];
} rs_rsc_shop;

rs_err rs_rsc_shop_decode(int revision, rs_buf *src, rs_rsc_shop *out);
rs_err rs_rsc_shop_encode(int revision, rs_buf *dst, const rs_rsc_shop *in);

/*
 * Opcode 222. 115 is count, then id and a gsmart amount.
 * 159 and 177 read a max-size byte (number293 / number497) before the items.
 * The client array is 256.
 * Opcode 214 is one slot. Amount 0 removes that slot and shifts the tail down.
 * The codec stores slot, id, and amount.
 */
enum { RS_RSC_BANK_MAX = 255 };

typedef struct rs_rsc_bank_item {
    int id;
    int amount;
} rs_rsc_bank_item;

typedef struct rs_rsc_bank {
    int count;
    int max_items;
    rs_rsc_bank_item item[RS_RSC_BANK_MAX];
} rs_rsc_bank;

rs_err rs_rsc_bank_decode(int revision, rs_buf *src, rs_rsc_bank *out);
rs_err rs_rsc_bank_encode(int revision, rs_buf *dst, const rs_rsc_bank *in);

typedef struct rs_rsc_bank_slot {
    int slot;
    int id;
    int amount;
} rs_rsc_bank_slot;

rs_err rs_rsc_bank_slot_decode(int revision, rs_buf *src, rs_rsc_bank_slot *out);
rs_err rs_rsc_bank_slot_encode(int revision, rs_buf *dst, const rs_rsc_bank_slot *in);

/*
 * Opcode 246. Count, then an unsigned length and that many bytes. At most 5.
 * An interior 0 is RS_ERR_FORMAT, same as opcode 8.
 */
enum { RS_RSC_STRINGS_MAX = 5, RS_RSC_STRING_MAX = 255 };

typedef struct rs_rsc_text {
    int len;
    char text[RS_RSC_STRING_MAX + 1];
} rs_rsc_text;

typedef struct rs_rsc_strings {
    int count;
    rs_rsc_text row[RS_RSC_STRINGS_MAX];
} rs_rsc_strings;

rs_err rs_rsc_strings_decode(int revision, rs_buf *src, rs_rsc_strings *out);
rs_err rs_rsc_strings_encode(int revision, rs_buf *dst, const rs_rsc_strings *in);

#ifdef __cplusplus
}
#endif

#endif
