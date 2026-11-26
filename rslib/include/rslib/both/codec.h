#ifndef RSLIB_CODEC_H
#define RSLIB_CODEC_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"
#include "rslib/proto.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { RS_WALK_STEPS = 25 };

typedef struct rs_walk {
    int dest_x, dest_z;
    int running;
    int nsteps;
    int8_t dx[RS_WALK_STEPS];
    int8_t dz[RS_WALK_STEPS];
} rs_walk;

typedef struct rs_appearance {
    int gender;
    int headicon;
    int colours[5];
    int styles[7];
    char name[13];
    int combat_level;
    int skill_level;
    /* 0 writes the 317 body-anim defaults. -1 writes 65535. */
    int anims[7];
    int npc_id; /* >0 replaces the equipment slots with an npc transmog */
} rs_appearance;

typedef struct rs_skill {
    int id;
    int level;
    int xp;
} rs_skill;

typedef struct rs_varp {
    int id;
    int value;
} rs_varp;

typedef struct rs_if_open {
    int id;
} rs_if_open;

typedef struct rs_if_text {
    int component;
    char text[256];
} rs_if_text;

typedef struct rs_message {
    int type;
    char text[256];
} rs_message;

/* 317 bank widget 5382 is 8 by 38. 254 still rejects a count above 255. */
enum { RS_INV_SLOTS = 304 };

typedef struct rs_inv_slot {
    int id;
    int amount;
} rs_inv_slot;

typedef struct rs_inventory {
    int component;
    /* flags, then nslots. 0 is the early body. 742 and 910 write this byte.
       474 stores the u16 container key. 910 bit 1 is unsupported. */
    int flags;
    int nslots;
    rs_inv_slot slots[RS_INV_SLOTS];
} rs_inventory;

typedef struct rs_region {
    int zone_x; /* player x >> 3 */
    int zone_z; /* player z >> 3 */
    int plane;
    int local_x; /* 0..103 scene tile; 0 means 52 */
    int local_z;
    const struct rs_cache *cache; /* JS5 XTEA keys; NULL writes zeros */
    int npc_view;                 /* RS3 rebuild; 0 encodes as 14 */
    int build_area;               /* RS3 build-area id; 0 is the standard 104 */
    int force_reload;             /* RS3 rebuild trailing flag */
} rs_region;

/* Logical player-block bits. Not the revision's wire mask. */
enum {
    RS_PLAYER_APPEARANCE = 1,
    RS_PLAYER_ANIM = 2,
    RS_PLAYER_CHAT = 4,
    RS_PLAYER_HIT = 8,
    RS_PLAYER_SPOTANIM = 16,
    RS_PLAYER_FACE_ENTITY = 32,
    RS_PLAYER_FACE_TILE = 64,
    RS_PLAYER_FORCE_MOVE = 128,
    RS_PLAYER_PUBLIC = 256,
    RS_PLAYER_HIT2 = 512
};

/* Wire bit for a logical RS_PLAYER_* flag. 0 when that revision has no player block. */
int rs_player_wire_mask(int revision, int logical);

typedef struct rs_player_update {
    int teleport;
    int plane;
    int local_x;
    int local_z;
    int discard_walk;
    int include_appearance; /* same block as RS_PLAYER_APPEARANCE */
    int mask;               /* RS_PLAYER_* */
    rs_appearance appearance;
} rs_player_update;

typedef struct rs_zone_base {
    int x, z; /* 0..103 scene tile of the 8x8 zone origin */
} rs_zone_base;

typedef struct rs_scene_loc {
    int x, z; /* 0..7 within the current zone */
    int id;
    int shape, rot;
} rs_scene_loc;

typedef struct rs_scene_obj {
    int x, z;
    int id;
    int amount;
} rs_scene_obj;

/* 317 packet 4. height is the wire byte; the client subtracts it from the tile. */
typedef struct rs_spotanim {
    int x, z;
    int id;
    int height;
    int delay;
} rs_spotanim;

/*
 * 317 packet 117. start_height and end_height are the wire bytes; the client
 * multiplies each by 4. dx, dz, and target are signed.
 */
typedef struct rs_projectile {
    int x, z;
    int dx, dz;
    int target;
    int graphic;
    int start_height;
    int end_height;
    int start_delay;
    int end_delay;
    int angle;
    int steepness;
} rs_projectile;

/* 317 packet 174. */
typedef struct rs_sound {
    int id;
    int loops;
    int delay;
} rs_sound;

/* 317 packet 249. members is the wire byte (g1_alt1). */
typedef struct rs_player_index {
    int members;
    int index;
} rs_player_index;

/* 317 packet 104. slot is 1..5. push_down is the wire byte; 0 means push down. */
typedef struct rs_player_option {
    int slot;
    int push_down;
    char text[64];
} rs_player_option;

/* 317 packet 221. */
typedef struct rs_friend_status {
    int status;
} rs_friend_status;

/* 317 packet 110. */
typedef struct rs_energy {
    int energy;
} rs_energy;

/* 317 packet 240. weight is signed. */
typedef struct rs_weight {
    int weight;
} rs_weight;

typedef struct rs_if_model {
    int component;
    int model;
} rs_if_model;

typedef struct rs_if_npc_head {
    int component;
    int npc;
} rs_if_npc_head;

typedef struct rs_if_player_head {
    int component;
} rs_if_player_head;

/* colour is the 15-bit wire value, not the expanded RGB. */
typedef struct rs_if_colour {
    int component;
    int colour;
} rs_if_colour;

typedef struct rs_if_hide {
    int component;
    int hidden;
} rs_if_hide;

/* obj < 0 writes 65535, which clears the model. */
typedef struct rs_if_object {
    int component;
    int zoom;
    int obj;
} rs_if_object;

/* anim < 0 writes 65535. */
typedef struct rs_if_anim {
    int component;
    int anim;
} rs_if_anim;

typedef struct rs_if_angle {
    int component;
    int xan;
    int yan;
    int zoom;
} rs_if_angle;

typedef struct rs_if_pos {
    int component;
    int x;
    int y;
} rs_if_pos;

typedef struct rs_if_scroll {
    int component;
    int position;
} rs_if_scroll;

typedef struct rs_if_sub {
    int id;
    /* Zero parent, type, and key is the early layout (id alone).
       Later eras follow rs_era_if_side: parent, type, key[0..3]
       in the order the client builds its int[]. */
    int parent;
    int type;
    int key[4];
} rs_if_sub;

typedef struct rs_if_top {
    int viewport;
    int sidebar;
    /* Zero type and key is the early layout (viewport, sidebar).
       Later eras follow rs_era_if_top: type, then key[0..3].
       742 key[] is open order. 910 key[] is wire order. */
    int type;
    int key[4];
} rs_if_top;

typedef struct rs_inv_clear {
    int component;
} rs_inv_clear;

typedef struct rs_inv_change {
    int slot;
    int id;
    int amount;
} rs_inv_change;

typedef struct rs_inv_update {
    int component;
    /* flags, then the changes. 0 is the early body. 742 and 910 write this byte.
       474 stores the u16 container key ahead of the changes. */
    int flags;
    int nchanges;
    rs_inv_change changes[RS_INV_SLOTS];
} rs_inv_update;

/* id < 0 writes 65535. */
typedef struct rs_song {
    int id;
} rs_song;

typedef struct rs_jingle {
    int id;
    int delay;
} rs_jingle;

typedef struct rs_sound_area {
    int x, z;
    int id;
    int radius;
    int loops;
} rs_sound_area;

typedef struct rs_loc_anim {
    int x, z;
    int shape, rot;
    int seq;
} rs_loc_anim;

typedef struct rs_cam_shake {
    int index;
    int jitter;
    int wobble;
    int speed;
} rs_cam_shake;

/* Shared by camera move (166) and camera look-at (177). */
typedef struct rs_camera {
    int x, z;
    int height;
    int speed;
    int accel;
} rs_camera;

typedef struct rs_minimap {
    int state;
} rs_minimap;

typedef struct rs_multizone {
    int state;
} rs_multizone;

typedef struct rs_zone_full {
    int x, z;
} rs_zone_full;

/* Header only. Bytes after it are nested zone packets. */
typedef struct rs_zone_partial {
    int x, z;
} rs_zone_partial;

/* timer is the wire short. The 317 client multiplies it by 30. */
typedef struct rs_system_update {
    int timer;
} rs_system_update;

typedef struct rs_chat_settings {
    int public_chat;
    int private_chat;
    int trade;
} rs_chat_settings;

/* interface_id < 0 writes 65535. */
typedef struct rs_sidebar {
    int interface_id;
    int tab;
} rs_sidebar;

typedef struct rs_tab {
    int tab;
} rs_tab;

typedef struct rs_walkable {
    int id;
} rs_walkable;

typedef struct rs_last_login {
    int days_since_recovery;
    int unread;
    int members_warning;
    int address;
    int days_since_login;
} rs_last_login;

typedef struct rs_friend_entry {
    char name[13];
    int world;
} rs_friend_entry;

enum { RS_IGNORE_MAX = 100 };

typedef struct rs_ignore_list {
    int count;
    char names[RS_IGNORE_MAX][13];
} rs_ignore_list;

typedef struct rs_private_message {
    char name[13];
    int message_id;
    int rights;
    char text[256];
} rs_private_message;

/* type 2..6 is the wire type. The client turns those into type 2 plus an offset. */
typedef struct rs_hint {
    int type;
    int npc;
    int player;
    int tile_x;
    int tile_z;
    int height;
} rs_hint;

enum { RS_INSTANCED_PLANES = 4, RS_INSTANCED_SIZE = 13 };

/* chunks[plane][z][x] < 0 means that square is not set. */
typedef struct rs_region_instanced {
    int zone_x;
    int zone_z;
    int chunks[RS_INSTANCED_PLANES][RS_INSTANCED_SIZE][RS_INSTANCED_SIZE];
} rs_region_instanced;

/* 317 c2s chat: colour and effect are the g1_alt3 bytes. JS5 leaves them 0. */
typedef struct rs_chat_in {
    int colour;
    int effect;
    char text[256];
} rs_chat_in;

typedef struct rs_command {
    char text[256];
} rs_command;

typedef struct rs_private_out {
    char name[13];
    char text[256];
} rs_private_out;

typedef struct rs_chat_mode {
    int public_chat;
    int private_chat;
    int trade;
} rs_chat_mode;

typedef struct rs_design {
    int gender;
    int styles[7];
    int colours[5];
} rs_design;

rs_err rs_codec_walk_encode(const rs_session *s, rs_buf *dst, const rs_walk *w);
rs_err rs_codec_walk_decode(const rs_session *s, rs_buf *src, rs_walk *w);

rs_err rs_codec_appearance_encode(const rs_session *s, rs_buf *dst, const rs_appearance *a);
rs_err rs_codec_appearance_decode(const rs_session *s, rs_buf *src, rs_appearance *a);

rs_err rs_codec_skill_encode(const rs_session *s, rs_buf *dst, const rs_skill *v);
rs_err rs_codec_skill_decode(const rs_session *s, rs_buf *src, rs_skill *v);

rs_err rs_codec_varp_encode(const rs_session *s, rs_buf *dst, const rs_varp *v);
rs_err rs_codec_varp_decode(const rs_session *s, rs_buf *src, rs_varp *v);

rs_err rs_codec_if_open_encode(const rs_session *s, rs_buf *dst, const rs_if_open *v);
rs_err rs_codec_if_open_decode(const rs_session *s, rs_buf *src, rs_if_open *v);

rs_err rs_codec_if_text_encode(const rs_session *s, rs_buf *dst, const rs_if_text *v);
rs_err rs_codec_if_text_decode(const rs_session *s, rs_buf *src, rs_if_text *v);

rs_err rs_codec_message_encode(const rs_session *s, rs_buf *dst, const rs_message *v);
rs_err rs_codec_message_decode(const rs_session *s, rs_buf *src, rs_message *v);

rs_err rs_codec_inventory_encode(const rs_session *s, rs_buf *dst, const rs_inventory *v);
rs_err rs_codec_inventory_decode(const rs_session *s, rs_buf *src, rs_inventory *v);

rs_err rs_codec_region_encode(const rs_session *s, rs_buf *dst, const rs_region *v);
rs_err rs_codec_region_decode(const rs_session *s, rs_buf *src, rs_region *v);

rs_err rs_codec_player_update_encode(const rs_session *s, rs_buf *dst, const rs_player_update *v);
rs_err rs_codec_player_update_decode(const rs_session *s, rs_buf *src, rs_player_update *v);

rs_err rs_codec_npc_update_empty(const rs_session *s, rs_buf *dst);

rs_err rs_codec_zone_base_encode(const rs_session *s, rs_buf *dst, const rs_zone_base *v);
rs_err rs_codec_zone_base_decode(const rs_session *s, rs_buf *src, rs_zone_base *v);

rs_err rs_codec_loc_add_encode(const rs_session *s, rs_buf *dst, const rs_scene_loc *v);
rs_err rs_codec_loc_add_decode(const rs_session *s, rs_buf *src, rs_scene_loc *v);
rs_err rs_codec_loc_del_encode(const rs_session *s, rs_buf *dst, const rs_scene_loc *v);
rs_err rs_codec_loc_del_decode(const rs_session *s, rs_buf *src, rs_scene_loc *v);

rs_err rs_codec_obj_add_encode(const rs_session *s, rs_buf *dst, const rs_scene_obj *v);
rs_err rs_codec_obj_add_decode(const rs_session *s, rs_buf *src, rs_scene_obj *v);
rs_err rs_codec_obj_del_encode(const rs_session *s, rs_buf *dst, const rs_scene_obj *v);
rs_err rs_codec_obj_del_decode(const rs_session *s, rs_buf *src, rs_scene_obj *v);

rs_err rs_codec_spotanim_encode(const rs_session *s, rs_buf *dst, const rs_spotanim *v);
rs_err rs_codec_spotanim_decode(const rs_session *s, rs_buf *src, rs_spotanim *v);

rs_err rs_codec_projectile_encode(const rs_session *s, rs_buf *dst, const rs_projectile *v);
rs_err rs_codec_projectile_decode(const rs_session *s, rs_buf *src, rs_projectile *v);

rs_err rs_codec_sound_encode(const rs_session *s, rs_buf *dst, const rs_sound *v);
rs_err rs_codec_sound_decode(const rs_session *s, rs_buf *src, rs_sound *v);

rs_err rs_codec_player_index_encode(const rs_session *s, rs_buf *dst, const rs_player_index *v);
rs_err rs_codec_player_index_decode(const rs_session *s, rs_buf *src, rs_player_index *v);

rs_err rs_codec_player_option_encode(const rs_session *s, rs_buf *dst, const rs_player_option *v);
rs_err rs_codec_player_option_decode(const rs_session *s, rs_buf *src, rs_player_option *v);

rs_err rs_codec_friend_status_encode(const rs_session *s, rs_buf *dst, const rs_friend_status *v);
rs_err rs_codec_friend_status_decode(const rs_session *s, rs_buf *src, rs_friend_status *v);

rs_err rs_codec_energy_encode(const rs_session *s, rs_buf *dst, const rs_energy *v);
rs_err rs_codec_energy_decode(const rs_session *s, rs_buf *src, rs_energy *v);

rs_err rs_codec_weight_encode(const rs_session *s, rs_buf *dst, const rs_weight *v);
rs_err rs_codec_weight_decode(const rs_session *s, rs_buf *src, rs_weight *v);

rs_err rs_codec_if_model_encode(const rs_session *s, rs_buf *dst, const rs_if_model *v);
rs_err rs_codec_if_model_decode(const rs_session *s, rs_buf *src, rs_if_model *v);
rs_err rs_codec_if_npc_head_encode(const rs_session *s, rs_buf *dst, const rs_if_npc_head *v);
rs_err rs_codec_if_npc_head_decode(const rs_session *s, rs_buf *src, rs_if_npc_head *v);
rs_err rs_codec_if_player_head_encode(const rs_session *s, rs_buf *dst, const rs_if_player_head *v);
rs_err rs_codec_if_player_head_decode(const rs_session *s, rs_buf *src, rs_if_player_head *v);
rs_err rs_codec_if_colour_encode(const rs_session *s, rs_buf *dst, const rs_if_colour *v);
rs_err rs_codec_if_colour_decode(const rs_session *s, rs_buf *src, rs_if_colour *v);
rs_err rs_codec_if_hide_encode(const rs_session *s, rs_buf *dst, const rs_if_hide *v);
rs_err rs_codec_if_hide_decode(const rs_session *s, rs_buf *src, rs_if_hide *v);
rs_err rs_codec_if_object_encode(const rs_session *s, rs_buf *dst, const rs_if_object *v);
rs_err rs_codec_if_object_decode(const rs_session *s, rs_buf *src, rs_if_object *v);
rs_err rs_codec_if_anim_encode(const rs_session *s, rs_buf *dst, const rs_if_anim *v);
rs_err rs_codec_if_anim_decode(const rs_session *s, rs_buf *src, rs_if_anim *v);
rs_err rs_codec_if_angle_encode(const rs_session *s, rs_buf *dst, const rs_if_angle *v);
rs_err rs_codec_if_angle_decode(const rs_session *s, rs_buf *src, rs_if_angle *v);
rs_err rs_codec_if_pos_encode(const rs_session *s, rs_buf *dst, const rs_if_pos *v);
rs_err rs_codec_if_pos_decode(const rs_session *s, rs_buf *src, rs_if_pos *v);
rs_err rs_codec_if_scroll_encode(const rs_session *s, rs_buf *dst, const rs_if_scroll *v);
rs_err rs_codec_if_scroll_decode(const rs_session *s, rs_buf *src, rs_if_scroll *v);
rs_err rs_codec_if_sub_encode(const rs_session *s, rs_buf *dst, const rs_if_sub *v);
rs_err rs_codec_if_sub_decode(const rs_session *s, rs_buf *src, rs_if_sub *v);
rs_err rs_codec_if_top_encode(const rs_session *s, rs_buf *dst, const rs_if_top *v);
rs_err rs_codec_if_top_decode(const rs_session *s, rs_buf *src, rs_if_top *v);
rs_err rs_codec_inv_clear_encode(const rs_session *s, rs_buf *dst, const rs_inv_clear *v);
rs_err rs_codec_inv_clear_decode(const rs_session *s, rs_buf *src, rs_inv_clear *v);
rs_err rs_codec_inv_update_encode(const rs_session *s, rs_buf *dst, const rs_inv_update *v);
rs_err rs_codec_inv_update_decode(const rs_session *s, rs_buf *src, rs_inv_update *v);
rs_err rs_codec_song_encode(const rs_session *s, rs_buf *dst, const rs_song *v);
rs_err rs_codec_song_decode(const rs_session *s, rs_buf *src, rs_song *v);
rs_err rs_codec_jingle_encode(const rs_session *s, rs_buf *dst, const rs_jingle *v);
rs_err rs_codec_jingle_decode(const rs_session *s, rs_buf *src, rs_jingle *v);
rs_err rs_codec_sound_area_encode(const rs_session *s, rs_buf *dst, const rs_sound_area *v);
rs_err rs_codec_sound_area_decode(const rs_session *s, rs_buf *src, rs_sound_area *v);
rs_err rs_codec_loc_anim_encode(const rs_session *s, rs_buf *dst, const rs_loc_anim *v);
rs_err rs_codec_loc_anim_decode(const rs_session *s, rs_buf *src, rs_loc_anim *v);
rs_err rs_codec_cam_shake_encode(const rs_session *s, rs_buf *dst, const rs_cam_shake *v);
rs_err rs_codec_cam_shake_decode(const rs_session *s, rs_buf *src, rs_cam_shake *v);
rs_err rs_codec_camera_move_encode(const rs_session *s, rs_buf *dst, const rs_camera *v);
rs_err rs_codec_camera_move_decode(const rs_session *s, rs_buf *src, rs_camera *v);
rs_err rs_codec_cam_lookat_encode(const rs_session *s, rs_buf *dst, const rs_camera *v);
rs_err rs_codec_cam_lookat_decode(const rs_session *s, rs_buf *src, rs_camera *v);
rs_err rs_codec_minimap_encode(const rs_session *s, rs_buf *dst, const rs_minimap *v);
rs_err rs_codec_minimap_decode(const rs_session *s, rs_buf *src, rs_minimap *v);
rs_err rs_codec_multizone_encode(const rs_session *s, rs_buf *dst, const rs_multizone *v);
rs_err rs_codec_multizone_decode(const rs_session *s, rs_buf *src, rs_multizone *v);
rs_err rs_codec_zone_full_encode(const rs_session *s, rs_buf *dst, const rs_zone_full *v);
rs_err rs_codec_zone_full_decode(const rs_session *s, rs_buf *src, rs_zone_full *v);
rs_err rs_codec_zone_partial_encode(const rs_session *s, rs_buf *dst, const rs_zone_partial *v);
rs_err rs_codec_zone_partial_decode(const rs_session *s, rs_buf *src, rs_zone_partial *v);
rs_err rs_codec_system_update_encode(const rs_session *s, rs_buf *dst, const rs_system_update *v);
rs_err rs_codec_system_update_decode(const rs_session *s, rs_buf *src, rs_system_update *v);
rs_err rs_codec_chat_settings_encode(const rs_session *s, rs_buf *dst, const rs_chat_settings *v);
rs_err rs_codec_chat_settings_decode(const rs_session *s, rs_buf *src, rs_chat_settings *v);
rs_err rs_codec_sidebar_encode(const rs_session *s, rs_buf *dst, const rs_sidebar *v);
rs_err rs_codec_sidebar_decode(const rs_session *s, rs_buf *src, rs_sidebar *v);
rs_err rs_codec_tab_encode(const rs_session *s, rs_buf *dst, const rs_tab *v);
rs_err rs_codec_tab_decode(const rs_session *s, rs_buf *src, rs_tab *v);
rs_err rs_codec_walkable_encode(const rs_session *s, rs_buf *dst, const rs_walkable *v);
rs_err rs_codec_walkable_decode(const rs_session *s, rs_buf *src, rs_walkable *v);
rs_err rs_codec_last_login_encode(const rs_session *s, rs_buf *dst, const rs_last_login *v);
rs_err rs_codec_last_login_decode(const rs_session *s, rs_buf *src, rs_last_login *v);
rs_err rs_codec_friend_entry_encode(const rs_session *s, rs_buf *dst, const rs_friend_entry *v);
rs_err rs_codec_friend_entry_decode(const rs_session *s, rs_buf *src, rs_friend_entry *v);
rs_err rs_codec_ignore_list_encode(const rs_session *s, rs_buf *dst, const rs_ignore_list *v);
rs_err rs_codec_ignore_list_decode(const rs_session *s, rs_buf *src, rs_ignore_list *v);
rs_err rs_codec_private_message_encode(const rs_session *s, rs_buf *dst, const rs_private_message *v);
rs_err rs_codec_private_message_decode(const rs_session *s, rs_buf *src, rs_private_message *v);
rs_err rs_codec_hint_encode(const rs_session *s, rs_buf *dst, const rs_hint *v);
rs_err rs_codec_hint_decode(const rs_session *s, rs_buf *src, rs_hint *v);
rs_err rs_codec_region_instanced_encode(const rs_session *s, rs_buf *dst, const rs_region_instanced *v);
rs_err rs_codec_region_instanced_decode(const rs_session *s, rs_buf *src, rs_region_instanced *v);
rs_err rs_codec_command_encode(const rs_session *s, rs_buf *dst, const rs_command *v);
rs_err rs_codec_command_decode(const rs_session *s, rs_buf *src, rs_command *v);
rs_err rs_codec_private_out_encode(const rs_session *s, rs_buf *dst, const rs_private_out *v);
rs_err rs_codec_private_out_decode(const rs_session *s, rs_buf *src, rs_private_out *v);
rs_err rs_codec_chat_mode_encode(const rs_session *s, rs_buf *dst, const rs_chat_mode *v);
rs_err rs_codec_chat_mode_decode(const rs_session *s, rs_buf *src, rs_chat_mode *v);
rs_err rs_codec_design_encode(const rs_session *s, rs_buf *dst, const rs_design *v);
rs_err rs_codec_design_decode(const rs_session *s, rs_buf *src, rs_design *v);
rs_err rs_codec_chat_in_encode(const rs_session *s, rs_buf *dst, const rs_chat_in *v);
rs_err rs_codec_chat_in_decode(const rs_session *s, rs_buf *src, rs_chat_in *v);

/*
 * Per-build codec delegate. Every rs_codec_* call goes through the build's
 * codec pointer (or rs_codec_for() when that pointer is NULL). Walk and
 * region differ by delegate. Skill, varp, inventory, interfaces, locs, and
 * objs call era_body. Player and NPC call rs_sync. Chat input differs for JS5.
 */
typedef struct rs_codec_ops {
    const char *name;
    rs_err (*walk_encode)(const rs_session *s, rs_buf *dst, const rs_walk *w);
    rs_err (*walk_decode)(const rs_session *s, rs_buf *src, rs_walk *w);
    rs_err (*region_encode)(const rs_session *s, rs_buf *dst, const rs_region *v);
    rs_err (*region_decode)(const rs_session *s, rs_buf *src, rs_region *v);
    rs_err (*player_encode)(const rs_session *s, rs_buf *dst, const rs_player_update *v);
    rs_err (*player_decode)(const rs_session *s, rs_buf *src, rs_player_update *v);
    rs_err (*npc_empty)(const rs_session *s, rs_buf *dst);
    rs_err (*appearance_encode)(const rs_session *s, rs_buf *dst, const rs_appearance *a);
    rs_err (*appearance_decode)(const rs_session *s, rs_buf *src, rs_appearance *a);
    rs_err (*skill_encode)(const rs_session *s, rs_buf *dst, const rs_skill *v);
    rs_err (*skill_decode)(const rs_session *s, rs_buf *src, rs_skill *v);
    rs_err (*varp_encode)(const rs_session *s, rs_buf *dst, const rs_varp *v);
    rs_err (*varp_decode)(const rs_session *s, rs_buf *src, rs_varp *v);
    rs_err (*if_open_encode)(const rs_session *s, rs_buf *dst, const rs_if_open *v);
    rs_err (*if_open_decode)(const rs_session *s, rs_buf *src, rs_if_open *v);
    rs_err (*if_text_encode)(const rs_session *s, rs_buf *dst, const rs_if_text *v);
    rs_err (*if_text_decode)(const rs_session *s, rs_buf *src, rs_if_text *v);
    rs_err (*message_encode)(const rs_session *s, rs_buf *dst, const rs_message *v);
    rs_err (*message_decode)(const rs_session *s, rs_buf *src, rs_message *v);
    rs_err (*inventory_encode)(const rs_session *s, rs_buf *dst, const rs_inventory *v);
    rs_err (*inventory_decode)(const rs_session *s, rs_buf *src, rs_inventory *v);
    rs_err (*zone_base_encode)(const rs_session *s, rs_buf *dst, const rs_zone_base *v);
    rs_err (*zone_base_decode)(const rs_session *s, rs_buf *src, rs_zone_base *v);
    rs_err (*loc_add_encode)(const rs_session *s, rs_buf *dst, const rs_scene_loc *v);
    rs_err (*loc_add_decode)(const rs_session *s, rs_buf *src, rs_scene_loc *v);
    rs_err (*loc_del_encode)(const rs_session *s, rs_buf *dst, const rs_scene_loc *v);
    rs_err (*loc_del_decode)(const rs_session *s, rs_buf *src, rs_scene_loc *v);
    rs_err (*obj_add_encode)(const rs_session *s, rs_buf *dst, const rs_scene_obj *v);
    rs_err (*obj_add_decode)(const rs_session *s, rs_buf *src, rs_scene_obj *v);
    rs_err (*obj_del_encode)(const rs_session *s, rs_buf *dst, const rs_scene_obj *v);
    rs_err (*obj_del_decode)(const rs_session *s, rs_buf *src, rs_scene_obj *v);
    rs_err (*spotanim_encode)(const rs_session *s, rs_buf *dst, const rs_spotanim *v);
    rs_err (*spotanim_decode)(const rs_session *s, rs_buf *src, rs_spotanim *v);
    rs_err (*projectile_encode)(const rs_session *s, rs_buf *dst, const rs_projectile *v);
    rs_err (*projectile_decode)(const rs_session *s, rs_buf *src, rs_projectile *v);
    rs_err (*sound_encode)(const rs_session *s, rs_buf *dst, const rs_sound *v);
    rs_err (*sound_decode)(const rs_session *s, rs_buf *src, rs_sound *v);
    rs_err (*player_index_encode)(const rs_session *s, rs_buf *dst, const rs_player_index *v);
    rs_err (*player_index_decode)(const rs_session *s, rs_buf *src, rs_player_index *v);
    rs_err (*player_option_encode)(const rs_session *s, rs_buf *dst, const rs_player_option *v);
    rs_err (*player_option_decode)(const rs_session *s, rs_buf *src, rs_player_option *v);
    rs_err (*friend_status_encode)(const rs_session *s, rs_buf *dst, const rs_friend_status *v);
    rs_err (*friend_status_decode)(const rs_session *s, rs_buf *src, rs_friend_status *v);
    rs_err (*energy_encode)(const rs_session *s, rs_buf *dst, const rs_energy *v);
    rs_err (*energy_decode)(const rs_session *s, rs_buf *src, rs_energy *v);
    rs_err (*weight_encode)(const rs_session *s, rs_buf *dst, const rs_weight *v);
    rs_err (*weight_decode)(const rs_session *s, rs_buf *src, rs_weight *v);
    rs_err (*if_model_encode)(const rs_session *s, rs_buf *dst, const rs_if_model *v);
    rs_err (*if_model_decode)(const rs_session *s, rs_buf *src, rs_if_model *v);
    rs_err (*if_npc_head_encode)(const rs_session *s, rs_buf *dst, const rs_if_npc_head *v);
    rs_err (*if_npc_head_decode)(const rs_session *s, rs_buf *src, rs_if_npc_head *v);
    rs_err (*if_player_head_encode)(const rs_session *s, rs_buf *dst, const rs_if_player_head *v);
    rs_err (*if_player_head_decode)(const rs_session *s, rs_buf *src, rs_if_player_head *v);
    rs_err (*if_colour_encode)(const rs_session *s, rs_buf *dst, const rs_if_colour *v);
    rs_err (*if_colour_decode)(const rs_session *s, rs_buf *src, rs_if_colour *v);
    rs_err (*if_hide_encode)(const rs_session *s, rs_buf *dst, const rs_if_hide *v);
    rs_err (*if_hide_decode)(const rs_session *s, rs_buf *src, rs_if_hide *v);
    rs_err (*if_object_encode)(const rs_session *s, rs_buf *dst, const rs_if_object *v);
    rs_err (*if_object_decode)(const rs_session *s, rs_buf *src, rs_if_object *v);
    rs_err (*if_anim_encode)(const rs_session *s, rs_buf *dst, const rs_if_anim *v);
    rs_err (*if_anim_decode)(const rs_session *s, rs_buf *src, rs_if_anim *v);
    rs_err (*if_angle_encode)(const rs_session *s, rs_buf *dst, const rs_if_angle *v);
    rs_err (*if_angle_decode)(const rs_session *s, rs_buf *src, rs_if_angle *v);
    rs_err (*if_pos_encode)(const rs_session *s, rs_buf *dst, const rs_if_pos *v);
    rs_err (*if_pos_decode)(const rs_session *s, rs_buf *src, rs_if_pos *v);
    rs_err (*if_scroll_encode)(const rs_session *s, rs_buf *dst, const rs_if_scroll *v);
    rs_err (*if_scroll_decode)(const rs_session *s, rs_buf *src, rs_if_scroll *v);
    rs_err (*if_sub_encode)(const rs_session *s, rs_buf *dst, const rs_if_sub *v);
    rs_err (*if_sub_decode)(const rs_session *s, rs_buf *src, rs_if_sub *v);
    rs_err (*if_top_encode)(const rs_session *s, rs_buf *dst, const rs_if_top *v);
    rs_err (*if_top_decode)(const rs_session *s, rs_buf *src, rs_if_top *v);
    rs_err (*inv_clear_encode)(const rs_session *s, rs_buf *dst, const rs_inv_clear *v);
    rs_err (*inv_clear_decode)(const rs_session *s, rs_buf *src, rs_inv_clear *v);
    rs_err (*inv_update_encode)(const rs_session *s, rs_buf *dst, const rs_inv_update *v);
    rs_err (*inv_update_decode)(const rs_session *s, rs_buf *src, rs_inv_update *v);
    rs_err (*song_encode)(const rs_session *s, rs_buf *dst, const rs_song *v);
    rs_err (*song_decode)(const rs_session *s, rs_buf *src, rs_song *v);
    rs_err (*jingle_encode)(const rs_session *s, rs_buf *dst, const rs_jingle *v);
    rs_err (*jingle_decode)(const rs_session *s, rs_buf *src, rs_jingle *v);
    rs_err (*sound_area_encode)(const rs_session *s, rs_buf *dst, const rs_sound_area *v);
    rs_err (*sound_area_decode)(const rs_session *s, rs_buf *src, rs_sound_area *v);
    rs_err (*loc_anim_encode)(const rs_session *s, rs_buf *dst, const rs_loc_anim *v);
    rs_err (*loc_anim_decode)(const rs_session *s, rs_buf *src, rs_loc_anim *v);
    rs_err (*cam_shake_encode)(const rs_session *s, rs_buf *dst, const rs_cam_shake *v);
    rs_err (*cam_shake_decode)(const rs_session *s, rs_buf *src, rs_cam_shake *v);
    rs_err (*camera_move_encode)(const rs_session *s, rs_buf *dst, const rs_camera *v);
    rs_err (*camera_move_decode)(const rs_session *s, rs_buf *src, rs_camera *v);
    rs_err (*cam_lookat_encode)(const rs_session *s, rs_buf *dst, const rs_camera *v);
    rs_err (*cam_lookat_decode)(const rs_session *s, rs_buf *src, rs_camera *v);
    rs_err (*minimap_encode)(const rs_session *s, rs_buf *dst, const rs_minimap *v);
    rs_err (*minimap_decode)(const rs_session *s, rs_buf *src, rs_minimap *v);
    rs_err (*multizone_encode)(const rs_session *s, rs_buf *dst, const rs_multizone *v);
    rs_err (*multizone_decode)(const rs_session *s, rs_buf *src, rs_multizone *v);
    rs_err (*zone_full_encode)(const rs_session *s, rs_buf *dst, const rs_zone_full *v);
    rs_err (*zone_full_decode)(const rs_session *s, rs_buf *src, rs_zone_full *v);
    rs_err (*zone_partial_encode)(const rs_session *s, rs_buf *dst, const rs_zone_partial *v);
    rs_err (*zone_partial_decode)(const rs_session *s, rs_buf *src, rs_zone_partial *v);
    rs_err (*system_update_encode)(const rs_session *s, rs_buf *dst, const rs_system_update *v);
    rs_err (*system_update_decode)(const rs_session *s, rs_buf *src, rs_system_update *v);
    rs_err (*chat_settings_encode)(const rs_session *s, rs_buf *dst, const rs_chat_settings *v);
    rs_err (*chat_settings_decode)(const rs_session *s, rs_buf *src, rs_chat_settings *v);
    rs_err (*sidebar_encode)(const rs_session *s, rs_buf *dst, const rs_sidebar *v);
    rs_err (*sidebar_decode)(const rs_session *s, rs_buf *src, rs_sidebar *v);
    rs_err (*tab_encode)(const rs_session *s, rs_buf *dst, const rs_tab *v);
    rs_err (*tab_decode)(const rs_session *s, rs_buf *src, rs_tab *v);
    rs_err (*walkable_encode)(const rs_session *s, rs_buf *dst, const rs_walkable *v);
    rs_err (*walkable_decode)(const rs_session *s, rs_buf *src, rs_walkable *v);
    rs_err (*last_login_encode)(const rs_session *s, rs_buf *dst, const rs_last_login *v);
    rs_err (*last_login_decode)(const rs_session *s, rs_buf *src, rs_last_login *v);
    rs_err (*friend_entry_encode)(const rs_session *s, rs_buf *dst, const rs_friend_entry *v);
    rs_err (*friend_entry_decode)(const rs_session *s, rs_buf *src, rs_friend_entry *v);
    rs_err (*ignore_list_encode)(const rs_session *s, rs_buf *dst, const rs_ignore_list *v);
    rs_err (*ignore_list_decode)(const rs_session *s, rs_buf *src, rs_ignore_list *v);
    rs_err (*private_message_encode)(const rs_session *s, rs_buf *dst, const rs_private_message *v);
    rs_err (*private_message_decode)(const rs_session *s, rs_buf *src, rs_private_message *v);
    rs_err (*hint_encode)(const rs_session *s, rs_buf *dst, const rs_hint *v);
    rs_err (*hint_decode)(const rs_session *s, rs_buf *src, rs_hint *v);
    rs_err (*region_instanced_encode)(const rs_session *s, rs_buf *dst, const rs_region_instanced *v);
    rs_err (*region_instanced_decode)(const rs_session *s, rs_buf *src, rs_region_instanced *v);
    rs_err (*command_encode)(const rs_session *s, rs_buf *dst, const rs_command *v);
    rs_err (*command_decode)(const rs_session *s, rs_buf *src, rs_command *v);
    rs_err (*private_out_encode)(const rs_session *s, rs_buf *dst, const rs_private_out *v);
    rs_err (*private_out_decode)(const rs_session *s, rs_buf *src, rs_private_out *v);
    rs_err (*chat_mode_encode)(const rs_session *s, rs_buf *dst, const rs_chat_mode *v);
    rs_err (*chat_mode_decode)(const rs_session *s, rs_buf *src, rs_chat_mode *v);
    rs_err (*design_encode)(const rs_session *s, rs_buf *dst, const rs_design *v);
    rs_err (*design_decode)(const rs_session *s, rs_buf *src, rs_design *v);
    rs_err (*chat_in_encode)(const rs_session *s, rs_buf *dst, const rs_chat_in *v);
    rs_err (*chat_in_decode)(const rs_session *s, rs_buf *src, rs_chat_in *v);
} rs_codec_ops;

const rs_codec_ops *rs_codec_for(const rs_build_info *build);

extern const rs_codec_ops rs_codec_plain;
extern const rs_codec_ops rs_codec_rs2;
extern const rs_codec_ops rs_codec_js5_474;
extern const rs_codec_ops rs_codec_js5_508;
extern const rs_codec_ops rs_codec_js5_530;
extern const rs_codec_ops rs_codec_rs3;

#ifdef __cplusplus
}
#endif

#endif
