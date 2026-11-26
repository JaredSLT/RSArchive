#ifndef RSLIB_PACK_H
#define RSLIB_PACK_H

#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * RSGP version 1. Little-endian. One writer (rs_pack_write) and one reader
 * (rs_pack_load). rspack calls the builder. The game only loads.
 *
 * File:
 *   4 bytes magic 'R''S''G''P'
 *   u16 version (1)
 *   u16 revision
 *   then 13 tables, each u32 count followed by that many records:
 *   strings, world, npcs, items, drops, shops, spawns, areas, options,
 *   recipes, xp, dialogues, quests.
 *
 * strings: u32 id, u16 nbytes, nbytes of raw text (no NUL).
 * world (count 0 or 1): u16 xp_num, u16 xp_den, u8 members, u8 spawn_plane,
 *   u16 spawn_x, u16 spawn_z, u16 max_players, u32 welcome_string_id,
 *   u32 pbkdf2_iters, u16 port, u16 tick_ms, u16 shop_restock_ticks,
 *   u16 autosave_ticks, u16 idle_timeout_ticks, u16 max_packets_per_tick.
 * npcs: u16 id, combat, hitpoints, attack, strength, defence, ranged, magic,
 *   u8 style, u8 aggro, u16 respawn, u16 drop_table, i8 wander.
 * items: u16 id, u8 equip_slot (255 = not wearable), u8 weapon_ticks,
 *   i16 bonus[12], i32 shop_price, u8 flags, u16 note_link.
 *   flags bit0 stack, bit1 noted.
 *   bonus order: stab, slash, crush, magic, ranged, stab_def, slash_def,
 *   crush_def, magic_def, ranged_def, strength, prayer.
 * drops: u16 table_id, item_id, min_qty, max_qty, num, den.
 * shops: u16 id, u32 name_string_id, u16 nstock,
 *   nstock times (u16 item, u16 amount, u16 restock).
 * spawns: u8 kind, u16 id, u8 plane, u16 x, u16 z, u16 extra, u16 respawn_ticks.
 * areas: u16 id, u8 plane, u16 x, z, w, h, flags, music.
 *   flags bit0 multi, bit1 wilderness, bit2 members.
 * options: u8 kind, u16 id, u8 index, u8 intent.
 * recipes: u16 id, u8 skill, u8 level, u32 xp, u8 nin,
 *   nin times (u16 item, u16 qty), u16 out_item, u16 out_qty,
 *   u8 effect, i32 effect_mag, i32 effect_dur.
 * xp: u32 count, then count u32 values. For 317, count is 99 and entry i is
 *   Client.levelExperience[i] from the static block in
 *   builds/rs2/317-2005-06-13 Client.java (xp to reach level i+2).
 * dialogues: u16 id, u16 speaker (0 = player), u32 text,
 *   u8 nreplies, then replies.
 *   reply: u32 text, u16 next (0 = none), u8 ncond, conds, u8 nact, acts.
 *   cond: u8 id, i32 a, i32 b, i32 c.
 *   act: u8 id, i32 p0, p1, p2, p3, p4.
 * quests: u16 id, u32 name_string, u16 varp_id (0xFFFF = unread),
 *   u16 varbit_id (0xFFFF = none), u8 nstages, then stages.
 *   stage: u8 index, u32 journal, u8 ncond, conds, u8 nact, acts.
 *
 * A count that overruns the file is RS_ERR_FORMAT. Version other than 1 is
 * RS_ERR_VERSION. Unknown action, condition, intent, or effect ids are
 * RS_ERR_RANGE. rs_pack_validate fails when a referenced string id is missing,
 * or when a string contains a '%' that is not the placeholder %1 or %2.
 *
 * Stored text is bytes the 317 client can draw (Client.java CHARSET). U+00A3
 * is stored as 0xA3. Bytes 0x00 and 0x0A are rejected.
 *
 * 317 ObjType.java in this tree has no wear-position field. equip_slot 255
 * means the slot has not been read. Do not guess one.
 *
 * Action parameters:
 *   give_item / take_item: item, qty
 *   set_var: var, value
 *   add_xp: skill, xp
 *   set_quest_stage: quest, stage
 *   open_dialogue: dialogue
 *   teleport: x, z, plane
 *   spawn_npc: npc, x, z, plane
 *   remove_npc: npc
 *   message: string id
 *   play_anim: anim
 *   play_sound: sound (runtime no-op until the mixer)
 *   open_interface: interface
 *   set_interface_text: component, string id
 *   set_loc: loc, x, z, plane, state (0 removes)
 *   apply_effect: effect, magnitude, duration ticks, target (0 self, 1 other)
 */

enum {
    RS_PACK_VERSION = 1,
    RS_PACK_VARP_UNREAD = 0xFFFF,
    RS_PACK_EQUIP_NONE = 255,
    RS_PACK_STOCK_MAX = 40,
    RS_PACK_REPLY_MAX = 6,
    RS_PACK_COND_MAX = 6,
    RS_PACK_ACT_MAX = 8,
    RS_PACK_ACT_PARAMS = 5,
    RS_PACK_RECIPE_IN_MAX = 8,
    RS_PACK_STAGE_MAX = 16,
    RS_PACK_BONUS = 12,
    RS_PACK_XP_317 = 99
};

enum {
    RS_PACK_ACT_NONE = 0,
    RS_PACK_ACT_GIVE_ITEM = 1,
    RS_PACK_ACT_TAKE_ITEM = 2,
    RS_PACK_ACT_SET_VAR = 3,
    RS_PACK_ACT_ADD_XP = 4,
    RS_PACK_ACT_SET_QUEST_STAGE = 5,
    RS_PACK_ACT_OPEN_DIALOGUE = 6,
    RS_PACK_ACT_CLOSE_DIALOGUE = 7,
    RS_PACK_ACT_TELEPORT = 8,
    RS_PACK_ACT_SPAWN_NPC = 9,
    RS_PACK_ACT_REMOVE_NPC = 10,
    RS_PACK_ACT_MESSAGE = 11,
    RS_PACK_ACT_PLAY_ANIM = 12,
    RS_PACK_ACT_PLAY_SOUND = 13,
    RS_PACK_ACT_OPEN_INTERFACE = 14,
    RS_PACK_ACT_SET_INTERFACE_TEXT = 15,
    RS_PACK_ACT_SET_LOC = 16,
    RS_PACK_ACT_APPLY_EFFECT = 17
};

enum {
    RS_PACK_COND_NONE = 0,
    RS_PACK_COND_QUEST_STAGE_EQ = 1,
    RS_PACK_COND_QUEST_STAGE_GE = 2,
    RS_PACK_COND_SKILL_GE = 3,
    RS_PACK_COND_ITEM_GE = 4,
    RS_PACK_COND_ITEM_EQUIPPED = 5,
    RS_PACK_COND_VAR_EQ = 6,
    RS_PACK_COND_VAR_GE = 7,
    RS_PACK_COND_LOC_EQ = 8,
    RS_PACK_COND_IN_AREA = 9
};

enum {
    RS_PACK_INTENT_NONE = 0,
    RS_PACK_INTENT_WALK = 1,
    RS_PACK_INTENT_FOLLOW = 2,
    RS_PACK_INTENT_TALK = 3,
    RS_PACK_INTENT_ATTACK = 4,
    RS_PACK_INTENT_TAKE = 5,
    RS_PACK_INTENT_USE_ITEM = 6,
    RS_PACK_INTENT_EQUIP = 7,
    RS_PACK_INTENT_PUSH_BUTTON = 8,
    RS_PACK_INTENT_TRADE = 9,
    RS_PACK_INTENT_SAY = 10
};

enum {
    RS_PACK_EFFECT_NONE = 0,
    RS_PACK_EFFECT_DAMAGE = 1,
    RS_PACK_EFFECT_HEAL = 2,
    RS_PACK_EFFECT_CURE = 3,
    RS_PACK_EFFECT_TELEPORT = 4,
    RS_PACK_EFFECT_ALCHEMY = 5,
    RS_PACK_EFFECT_ENCHANT = 6,
    RS_PACK_EFFECT_BIND = 7,
    RS_PACK_EFFECT_POISON = 8,
    RS_PACK_EFFECT_STUN = 9,
    RS_PACK_EFFECT_DRAIN = 10
};

enum {
    RS_PACK_OPT_NPC = 1,
    RS_PACK_OPT_LOC = 2,
    RS_PACK_OPT_OBJ = 3,
    RS_PACK_SPAWN_NPC = 1,
    RS_PACK_SPAWN_ITEM = 2
};

enum {
    RS_PACK_AREA_MULTI = 1,
    RS_PACK_AREA_WILDERNESS = 2,
    RS_PACK_AREA_MEMBERS = 4,
    RS_PACK_ITEM_STACK = 1,
    RS_PACK_ITEM_NOTED = 2
};

typedef struct rs_pack rs_pack;

typedef struct rs_pack_world {
    uint16_t xp_num;
    uint16_t xp_den;
    uint8_t members;
    uint8_t spawn_plane;
    uint16_t spawn_x;
    uint16_t spawn_z;
    uint16_t max_players;
    uint32_t welcome_string_id;
    uint32_t pbkdf2_iters;
    uint16_t port;
    uint16_t tick_ms;
    uint16_t shop_restock_ticks;
    uint16_t autosave_ticks;
    uint16_t idle_timeout_ticks;
    uint16_t max_packets_per_tick;
} rs_pack_world;

typedef struct rs_pack_npc {
    uint16_t id;
    uint16_t combat;
    uint16_t hitpoints;
    uint16_t attack;
    uint16_t strength;
    uint16_t defence;
    uint16_t ranged;
    uint16_t magic;
    uint8_t style;
    uint8_t aggro;
    uint16_t respawn;
    uint16_t drop_table;
    int8_t wander;
} rs_pack_npc;

typedef struct rs_pack_item {
    uint16_t id;
    uint8_t equip_slot;
    uint8_t weapon_ticks;
    int16_t bonus[RS_PACK_BONUS];
    int32_t shop_price;
    uint8_t flags;
    uint16_t note_link;
} rs_pack_item;

typedef struct rs_pack_drop {
    uint16_t table_id;
    uint16_t item_id;
    uint16_t min_qty;
    uint16_t max_qty;
    uint16_t num;
    uint16_t den;
} rs_pack_drop;

typedef struct rs_pack_shop {
    uint16_t id;
    uint32_t name_string_id;
    uint16_t nstock;
    uint16_t item[RS_PACK_STOCK_MAX];
    uint16_t amount[RS_PACK_STOCK_MAX];
    uint16_t restock[RS_PACK_STOCK_MAX];
} rs_pack_shop;

typedef struct rs_pack_spawn {
    uint8_t kind;
    uint16_t id;
    uint8_t plane;
    uint16_t x;
    uint16_t z;
    uint16_t extra;
    uint16_t respawn_ticks;
} rs_pack_spawn;

typedef struct rs_pack_area {
    uint16_t id;
    uint8_t plane;
    uint16_t x, z, w, h;
    uint16_t flags;
    uint16_t music;
} rs_pack_area;

typedef struct rs_pack_option {
    uint8_t kind;
    uint16_t id;
    uint8_t index;
    uint8_t intent;
} rs_pack_option;

typedef struct rs_pack_recipe {
    uint16_t id;
    uint8_t skill;
    uint8_t level;
    uint32_t xp;
    uint8_t nin;
    uint16_t in_item[RS_PACK_RECIPE_IN_MAX];
    uint16_t in_qty[RS_PACK_RECIPE_IN_MAX];
    uint16_t out_item;
    uint16_t out_qty;
    uint8_t effect;
    int32_t effect_mag;
    int32_t effect_dur;
} rs_pack_recipe;

typedef struct rs_pack_cond {
    uint8_t id;
    int32_t a, b, c;
} rs_pack_cond;

typedef struct rs_pack_act {
    uint8_t id;
    int32_t p[RS_PACK_ACT_PARAMS];
} rs_pack_act;

typedef struct rs_pack_reply {
    uint32_t text_id;
    uint16_t next_id;
    uint8_t ncond;
    uint8_t nact;
    rs_pack_cond cond[RS_PACK_COND_MAX];
    rs_pack_act act[RS_PACK_ACT_MAX];
} rs_pack_reply;

typedef struct rs_pack_dialogue {
    uint16_t id;
    uint16_t speaker;
    uint32_t text_id;
    uint8_t nreplies;
    rs_pack_reply replies[RS_PACK_REPLY_MAX];
} rs_pack_dialogue;

typedef struct rs_pack_stage {
    uint8_t index;
    uint32_t journal_id;
    uint8_t ncond;
    uint8_t nact;
    rs_pack_cond cond[RS_PACK_COND_MAX];
    rs_pack_act act[RS_PACK_ACT_MAX];
} rs_pack_stage;

typedef struct rs_pack_quest {
    uint16_t id;
    uint32_t name_string_id;
    uint16_t varp_id;
    uint16_t varbit_id;
    uint8_t nstages;
    rs_pack_stage stages[RS_PACK_STAGE_MAX];
} rs_pack_quest;

rs_pack *rs_pack_new(uint16_t revision);
void rs_pack_free(rs_pack *p);

rs_err rs_pack_set_world(rs_pack *p, const rs_pack_world *w);
rs_err rs_pack_add_string(rs_pack *p, uint32_t id, const uint8_t *bytes, uint16_t n);
rs_err rs_pack_add_npc(rs_pack *p, const rs_pack_npc *row);
rs_err rs_pack_add_item(rs_pack *p, const rs_pack_item *row);
rs_err rs_pack_add_drop(rs_pack *p, const rs_pack_drop *row);
rs_err rs_pack_add_shop(rs_pack *p, const rs_pack_shop *row);
rs_err rs_pack_add_spawn(rs_pack *p, const rs_pack_spawn *row);
rs_err rs_pack_add_area(rs_pack *p, const rs_pack_area *row);
rs_err rs_pack_add_option(rs_pack *p, const rs_pack_option *row);
rs_err rs_pack_add_recipe(rs_pack *p, const rs_pack_recipe *row);
rs_err rs_pack_set_xp(rs_pack *p, const uint32_t *xp, int n);
rs_err rs_pack_add_dialogue(rs_pack *p, const rs_pack_dialogue *row);
rs_err rs_pack_add_quest(rs_pack *p, const rs_pack_quest *row);

rs_err rs_pack_write(const rs_pack *p, const char *path);
rs_err rs_pack_load(const char *path, rs_pack **out);
rs_err rs_pack_load_mem(const uint8_t *bytes, size_t n, rs_pack **out);
rs_err rs_pack_validate(const rs_pack *p);

uint16_t rs_pack_revision(const rs_pack *p);
const rs_pack_world *rs_pack_get_world(const rs_pack *p);
const char *rs_pack_string(const rs_pack *p, uint32_t id);
int rs_pack_string_count(const rs_pack *p);

int rs_pack_npc_count(const rs_pack *p);
const rs_pack_npc *rs_pack_npc_at(const rs_pack *p, int index);
const rs_pack_npc *rs_pack_npc_by_id(const rs_pack *p, uint16_t id);

int rs_pack_item_count(const rs_pack *p);
const rs_pack_item *rs_pack_item_at(const rs_pack *p, int index);
const rs_pack_item *rs_pack_item_by_id(const rs_pack *p, uint16_t id);

int rs_pack_drop_count(const rs_pack *p);
const rs_pack_drop *rs_pack_drop_at(const rs_pack *p, int index);

int rs_pack_shop_count(const rs_pack *p);
const rs_pack_shop *rs_pack_shop_at(const rs_pack *p, int index);
const rs_pack_shop *rs_pack_shop_by_id(const rs_pack *p, uint16_t id);

int rs_pack_spawn_count(const rs_pack *p);
const rs_pack_spawn *rs_pack_spawn_at(const rs_pack *p, int index);

int rs_pack_area_count(const rs_pack *p);
const rs_pack_area *rs_pack_area_at(const rs_pack *p, int index);
const rs_pack_area *rs_pack_area_by_id(const rs_pack *p, uint16_t id);

int rs_pack_option_count(const rs_pack *p);
const rs_pack_option *rs_pack_option_at(const rs_pack *p, int index);

int rs_pack_recipe_count(const rs_pack *p);
const rs_pack_recipe *rs_pack_recipe_at(const rs_pack *p, int index);
const rs_pack_recipe *rs_pack_recipe_by_id(const rs_pack *p, uint16_t id);

int rs_pack_xp_count(const rs_pack *p);
const uint32_t *rs_pack_xp(const rs_pack *p);

int rs_pack_dialogue_count(const rs_pack *p);
const rs_pack_dialogue *rs_pack_dialogue_at(const rs_pack *p, int index);
const rs_pack_dialogue *rs_pack_dialogue_by_id(const rs_pack *p, uint16_t id);

int rs_pack_quest_count(const rs_pack *p);
const rs_pack_quest *rs_pack_quest_at(const rs_pack *p, int index);
const rs_pack_quest *rs_pack_quest_by_id(const rs_pack *p, uint16_t id);

/* 0 / NULL / -1 when the name is not in the closed list. */
int rs_pack_act_from_name(const char *name);
const char *rs_pack_act_name(int id);
int rs_pack_cond_from_name(const char *name);
const char *rs_pack_cond_name(int id);
int rs_pack_intent_from_name(const char *name);
const char *rs_pack_intent_name(int id);
int rs_pack_effect_from_name(const char *name);
const char *rs_pack_effect_name(int id);

/* Stats.java names, index 0..24. -1 if the name is not in that array. */
int rs_pack_skill_from_name(const char *name);
const char *rs_pack_skill_name(int id);

/* Client.java static block. out[0] == 83, out[1] == 174, out[2] == 276, out[3] == 388. */
rs_err rs_pack_xp_317(uint32_t out[RS_PACK_XP_317]);

/* Replace %1 and %2. Other '%' forms return RS_ERR_FORMAT. */
rs_err rs_pack_string_subst(const rs_pack *p, uint32_t id, const char *a, const char *b,
                            char *dst, size_t dst_n);

#ifdef __cplusplus
}
#endif

#endif
