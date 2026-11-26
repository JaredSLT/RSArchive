#ifndef RSLIB_CONTENT_H
#define RSLIB_CONTENT_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 317 Client.java named integer assignments.
 * Inventory, chat, sidebar, viewport, and report-abuse component ids are
 * packet-assigned. Their fields are initialised to -1. Energy has no named
 * interface id. Sidebar click handlers assign selectedTab 0..13; those indices
 * are not named inventory or stats in the source.
 */
#define RS_317_SIDEBAR_DEFAULT 3          /* selectedTab = 3 */
#define RS_317_SIDEBAR_FLASH_ALT 1        /* flashingTab == 3 -> selectedTab = 1 */
#define RS_317_SIDEBAR_TAB_COUNT 15       /* tabInterfaceId length */
#define RS_317_SIDEBAR_ICON_COUNT 13      /* imageSideicons length */
#define RS_317_CHAT_SCROLL_HEIGHT 78      /* chatScrollHeight = 78 */
#define RS_317_CHAT_MAX 50                /* MAX_CHATS = 50 */
#define RS_317_IF_NONE (-1)
#define RS_317_IF_CHAT_INIT (-1)          /* chatInterfaceId */
#define RS_317_IF_SIDEBAR_INIT (-1)       /* sidebarInterfaceId */
#define RS_317_IF_STICKY_CHAT_INIT (-1)   /* stickyChatInterfaceId */
#define RS_317_IF_VIEWPORT_INIT (-1)      /* viewportInterfaceId */
#define RS_317_IF_VIEWPORT_OVERLAY_INIT (-1)
#define RS_317_IF_REPORT_ABUSE_INIT (-1)  /* reportAbuseInterfaceId */

enum {
    RS_CONTENT_NAME = 64,
    RS_CONTENT_DESC = 256,
    RS_CONTENT_OP = 5,
    RS_CONTENT_STR = 64,
    RS_CONTENT_RECOL = 40,
    RS_CONTENT_INDEX = 16,
    RS_CONTENT_COUNT = 10,
    RS_CONTENT_QUEST = 64,
    RS_CONTENT_CURSOR = 6,
    RS_CONTENT_PARAM = 64,
    RS_CONTENT_MODEL = 64,
    RS_CONTENT_GROUP = 32,
    RS_CONTENT_GROUP_MODEL = 8,
    RS_CONTENT_MULTI = 64,
    RS_CONTENT_HEAD = 32,
    RS_CONTENT_SOUND = 32,
    RS_CONTENT_HEADICON = 16,
    RS_CONTENT_ANIM = 16
};

typedef struct rs_param {
    int key;
    int is_string;
    int int_value;
    char str_value[RS_CONTENT_STR];
} rs_param;

/*
 * id is not in the config buffer. Decode zeroes it.
 * String slots with *_set == 0 are the client's null.
 * stackable is 0/1, or 2 when a revision's opcode sets 2.
 * contrast is stored after the revision's multiplier (*5 or not).
 */
typedef struct rs_obj_type {
    int id;
    int model;
    char name[RS_CONTENT_NAME];
    int name_set;
    char desc[RS_CONTENT_DESC];
    int desc_set;
    int zoom2d;
    int xan2d;
    int yan2d;
    int zan2d;
    int xof2d;
    int yof2d;
    int stackable;
    int cost;
    int members;
    int tradeable;
    int cert_not_tradeable;
    int placeholder;
    int stockmarket;
    int category;
    int team;
    int ambient;
    int contrast;
    int resizex;
    int resizey;
    int resizez;
    int wearpos;
    int wearpos2;
    int wearpos3;
    int manwear;
    int manwear2;
    int manwear3;
    int manwear_y; /* 317/474 opcode 23 extra byte. 474 stores it unsigned. */
    int womanwear;
    int womanwear2;
    int womanwear3;
    int womanwear_y;
    int manwearxoff;
    int manwearyoff;
    int manwearzoff;
    int womanwearxoff;
    int womanwearyoff;
    int womanwearzoff;
    int manhead;
    int manhead2;
    int womanhead;
    int womanhead2;
    int certlink;
    int certtemplate;
    int lentlink;
    int lenttemplate;
    int shardlink;
    int shardtemplate;
    int shardcount;
    char shardname[RS_CONTENT_NAME];
    int shardname_set;
    int boughtlink;
    int boughttemplate;
    int dummy10; /* 317 opcode 10 */
    int dummyitem; /* opcode 96 */
    int aux18; /* 742 opcode 18 */
    int op127_a; /* 530 opcode 127 stores these; 742/910 only consume them */
    int op127_b;
    int op128_a;
    int op128_b;
    int picksizeshift;
    int minimenu_colour;
    int minimenu_colour_set;
    int tboolean;
    char op[RS_CONTENT_OP][RS_CONTENT_STR];
    int op_set[RS_CONTENT_OP];
    char iop[RS_CONTENT_OP][RS_CONTENT_STR];
    int iop_set[RS_CONTENT_OP];
    int recol_count;
    int recol_s[RS_CONTENT_RECOL];
    int recol_d[RS_CONTENT_RECOL];
    int retex_count;
    int retex_s[RS_CONTENT_RECOL];
    int retex_d[RS_CONTENT_RECOL];
    int palette_count;
    int palette[RS_CONTENT_RECOL];
    int recolindex_count;
    int recolindex[RS_CONTENT_INDEX];
    int retexindex_count;
    int retexindex[RS_CONTENT_INDEX];
    int countobj[RS_CONTENT_COUNT];
    int countco[RS_CONTENT_COUNT];
    int quest_count;
    int quests[RS_CONTENT_QUEST];
    int cursor_set;
    int cursor[RS_CONTENT_CURSOR];
    int icursor_set;
    int icursor[RS_CONTENT_OP];
    int param_count;
    rs_param params[RS_CONTENT_PARAM];
} rs_obj_type;

typedef struct rs_npc_type {
    int id;
    char name[RS_CONTENT_NAME];
    int name_set;
    char desc[RS_CONTENT_DESC];
    int desc_set;
    int size;
    int model_count;
    int models[RS_CONTENT_MODEL];
    int head_count;
    int heads[RS_CONTENT_HEAD];
    int readyanim;
    int walkanim;
    int walkanim_b;
    int walkanim_r;
    int walkanim_l;
    int anim15; /* 474 opcode 15 */
    int anim16; /* 474 opcode 16 */
    int vislevel;
    int resizeh;
    int resizev;
    int minimap;
    int active;
    int alwaysontop;
    int ambient;
    int contrast;
    int headicon;
    int headicon_count;
    int headicon_group[RS_CONTENT_HEADICON];
    int headicon_id[RS_CONTENT_HEADICON];
    int turnspeed;
    int walksmoothing;
    int spotshadow;
    int spotshadowcolour_1;
    int spotshadowcolour_2;
    int spotshadowtrans_1;
    int spotshadowtrans_2;
    int spotshadowtexture;
    int spotshadowtexture_alpha;
    int walkflags;
    int bas;
    int overlayheight;
    int respawndir;
    int covermarker;
    int attackcursor;
    int bgsound;
    int bgsound_crawl;
    int bgsound_walk;
    int bgsound_run;
    int bgsound_range;
    int bgsound_size;
    int bgsound_volume;
    int bgsound_minrate;
    int bgsound_maxrate;
    int follower;
    int drawbelow;
    int mapelement;
    int oppriority;
    int picksize;
    int picksizeshift;
    int antimacro;
    int fade_in;
    int transmog;
    int flag162;
    int field90; /* 317 opcode 90 */
    int field91;
    int field92;
    int aux122; /* 530 opcode 122 */
    int aux126; /* 530 opcode 126 */
    int aux135_a;
    int aux135_b;
    int aux136_a;
    int aux136_b;
    int model_off_count;
    int model_off_index[RS_CONTENT_MODEL];
    int model_off[RS_CONTENT_MODEL][3];
    int tint_hue;
    int tint_saturation;
    int tint_luminence;
    int tint_weight;
    int clickbox_set;
    int clickbox[6];
    char op[RS_CONTENT_OP][RS_CONTENT_STR];
    int op_set[RS_CONTENT_OP];
    int recol_count;
    int recol_s[RS_CONTENT_RECOL];
    int recol_d[RS_CONTENT_RECOL];
    int retex_count;
    int retex_s[RS_CONTENT_RECOL];
    int retex_d[RS_CONTENT_RECOL];
    int palette_count;
    int palette[RS_CONTENT_RECOL];
    int recolindex_count;
    int recolindex[RS_CONTENT_INDEX];
    int retexindex_count;
    int retexindex[RS_CONTENT_INDEX];
    int multivarbit;
    int multivarp;
    int multi_count;
    int multi[RS_CONTENT_MULTI];
    int multi_default;
    int quest_count;
    int quests[RS_CONTENT_QUEST];
    int cursor_set;
    int cursor[RS_CONTENT_CURSOR];
    int param_count;
    rs_param params[RS_CONTENT_PARAM];
} rs_npc_type;

/*
 * 317/474/530 opcode 1 is parallel models[i] + shapes[i].
 * Opcode 5 clears shapes (shapes_present = 0).
 * 742/910 opcode 1 is groups: group_shape[g] and group_model[g][0..group_n).
 * models[] is the same models flattened, with shapes[] repeating the group shape.
 * blockwalk is 0/1 on 317 and the client's block mode (default 2) on later revs.
 * 317 LocType.lowMemory is left false (Java default / setHighMemory). A second
 * model opcode is skipped when models are already set. Client.setLowMemory sets
 * the flag true, which would keep the later list instead.
 */
typedef struct rs_loc_type {
    int id;
    char name[RS_CONTENT_NAME];
    int name_set;
    char desc[RS_CONTENT_DESC];
    int desc_set;
    int model_count;
    int models[RS_CONTENT_MODEL];
    int shapes_present;
    int shapes[RS_CONTENT_MODEL];
    int group_count;
    int group_shape[RS_CONTENT_GROUP];
    int group_n[RS_CONTENT_GROUP];
    int group_model[RS_CONTENT_GROUP][RS_CONTENT_GROUP_MODEL];
    int width;
    int length;
    int blockwalk;
    int blockrange;
    int active;
    int hillskew;
    int hillchange;
    int hillchange_value;
    int sharelight;
    int occlude;
    int walloff;
    int ambient;
    int contrast;
    int mapfunction;
    int mapscene;
    int mirror;
    int shadow;
    int hardshadow;
    int randomanim;
    int istexture;
    int members;
    int resizex;
    int resizey;
    int resizez;
    int forceapproach;
    int offsetx;
    int offsety;
    int offsetz;
    int post_xoff;
    int post_yoff;
    int post_zoff;
    int forcedecor;
    int breakroutefinding;
    int raiseobject;
    int mapscene_rotate;
    int mapscene_mirror;
    int tboolean2;
    int tboolean3;
    int antimacro;
    int flag168;
    int flag169;
    int tint;
    int tint2;
    int tint3;
    int tint4;
    int tint_hue;
    int tint_saturation;
    int tint_luminence;
    int tint_weight;
    int aux90; /* 530 opcode 90 */
    int aux96;
    int aux97; /* 530 opcode 97. 742/910 opcode 97 is mapscene_rotate. */
    int aux98; /* 530 opcode 98. 742/910 opcode 98 is tboolean2. */
    int tboolean; /* 910 opcode 200 */
    int aux99_a;
    int aux99_b;
    int aux100_a;
    int aux100_b;
    int enum186;
    int enum196;
    int enum197;
    int clickbox_set;
    int clickbox[6];
    int anim;
    int anim_count;
    int anim_ids[RS_CONTENT_ANIM];
    int anim_weight[RS_CONTENT_ANIM];
    int bgsound;
    int bgsound_range;
    int bgsound_size;
    int bgsound_volume;
    int bgsound_mindelay;
    int bgsound_maxdelay;
    int bgsound_minrate;
    int bgsound_maxrate;
    int bgsound_random_count;
    int bgsound_random[RS_CONTENT_SOUND];
    int mapelement;
    int multivarbit;
    int multivarp;
    int multi_count;
    int multi[RS_CONTENT_MULTI];
    int multi_default;
    char op[RS_CONTENT_OP][RS_CONTENT_STR];
    int op_set[RS_CONTENT_OP];
    int recol_count;
    int recol_s[RS_CONTENT_RECOL];
    int recol_d[RS_CONTENT_RECOL];
    int retex_count;
    int retex_s[RS_CONTENT_RECOL];
    int retex_d[RS_CONTENT_RECOL];
    int palette_count;
    int palette[RS_CONTENT_RECOL];
    int recolindex_count;
    int recolindex[RS_CONTENT_INDEX];
    int retexindex_count;
    int retexindex[RS_CONTENT_INDEX];
    int quest_count;
    int quests[RS_CONTENT_QUEST];
    int cursor_set;
    int cursor[RS_CONTENT_CURSOR];
    int param_count;
    rs_param params[RS_CONTENT_PARAM];
} rs_loc_type;

enum { RS_LAND_PLANES = 4, RS_LAND_SIZE = 64 };

/* One 64x64 map square, 4 planes. Indices are [level][x][z], matching readLand. */
typedef struct rs_land {
    int height[RS_LAND_PLANES][RS_LAND_SIZE][RS_LAND_SIZE];
    int underlay[RS_LAND_PLANES][RS_LAND_SIZE][RS_LAND_SIZE];
    int overlay[RS_LAND_PLANES][RS_LAND_SIZE][RS_LAND_SIZE];
    int overlay_shape[RS_LAND_PLANES][RS_LAND_SIZE][RS_LAND_SIZE];
    int overlay_rot[RS_LAND_PLANES][RS_LAND_SIZE][RS_LAND_SIZE];
    int flags[RS_LAND_PLANES][RS_LAND_SIZE][RS_LAND_SIZE];
    int collision[RS_LAND_PLANES][RS_LAND_SIZE][RS_LAND_SIZE];
} rs_land;

typedef struct rs_land_loc {
    int id;
    int x;
    int z;
    int level; /* level stored in the file */
    int shape;
    int rot;
    int plane; /* collision plane; level-1 when flags[1][x][z] has bit 2 */
} rs_land_loc;

/* Optional clip, indexed by loc id. NULL skips loc collision stamps. */
typedef struct rs_land_loc_type {
    int width;
    int length;
    int blockwalk;
    int blockrange;
    int active;
} rs_land_loc_type;

/* Full decoders: 317, 474, 530, 742, 910. Other revisions, including 831 and 860, return RS_ERR_UNSUPPORTED. */
rs_err rs_obj_type_decode(int revision, rs_buf *src, rs_obj_type *out);
rs_err rs_npc_type_decode(int revision, rs_buf *src, rs_npc_type *out);
rs_err rs_loc_type_decode(int revision, rs_buf *src, rs_loc_type *out);

/*
 * One 64x64 square, rotation 0. Opcode 0 heights use perlin.
 * rs_land_decode places the square at world 0,0.
 * rs_land_decode_at uses origin_x/origin_z as the world tile of square (0,0),
 * matching World.readLandInner(perlin(932731 + x + baseX, 556238 + z + baseZ)).
 * Opcode 1 heights ignore the origin.
 * Classic loop, height * 8 and upper step 240: 254, 282, 317, 350, 377, 443,
 * 468, 474, 481, 508, 525, 530, 578.
 * Same loop, height * 32 and upper step 960: 614, 634, 667, 742.
 * One-byte header (MapLoader.readLandscape): 745, 790, 876, 887, 902, 910.
 * Flag-bit collision is stamped where that pass was read. 790 and later
 * bitfield rows store the flags and do not stamp. 695, 831, and 860 stay unread.
 */
rs_err rs_land_decode(int revision, const uint8_t *data, size_t len, rs_land *out);
rs_err rs_land_decode_at(int revision, const uint8_t *data, size_t len, int origin_x, int origin_z,
                         rs_land *out);

/*
 * FloType.unpack / decodeClient / setColour for 254, 282, 317, 350, and 377.
 * hue, saturation, lightness, chroma, and luminance are the unjittered fields
 * the underlay blender reads. hsl is hsl24to16 of those three channels.
 * The client adds Math.random jitter to the hsl channels only. That jitter is 0 here.
 * Opcode 7 is 317, 350, and 377. 254 and 282 reject it.
 * Other revisions return RS_ERR_UNSUPPORTED. JS5 floors are rs_flo_js5_decode.
 */
typedef struct rs_flo_type {
    int rgb;
    int texture;
    int overlay;
    int occlude;
    int hue;
    int saturation;
    int lightness;
    int chroma;
    int luminance;
    int hsl;
} rs_flo_type;

rs_err rs_flo_decode(int revision, const uint8_t *data, size_t len, rs_flo_type *out, int cap, int *count);

/* One config file, one type. Underlay ids and overlay ids are separate. */
enum { RS_FLO_UNDERLAY = 0, RS_FLO_OVERLAY = 1 };

/*
 * Config archive 2, group 1 (underlay) and group 4 (overlay).
 * rgb is the raw g3. Hue, saturation, lightness, chroma, and luminance come
 * from FloType.setColour of that g3. 16711935 is left in rgb.
 * 695, 831, and 860 return RS_ERR_UNSUPPORTED.
 */
rs_err rs_flo_js5_decode(int revision, int kind, const uint8_t *data, size_t len, rs_flo_type *out);

/*
 * 317 readLoc. Not the same as rs_map_parse_locs: the inner gsmarts values
 * accumulate (pos += delta - 1). LocType width/length/blockwalk/blockrange/active
 * are taken from types[id] when types is non-NULL, and CollisionMap.addWall,
 * addLoc, or setBlocked is applied on land. The 104-map reset mask is not stamped.
 */
rs_err rs_land_locs_decode(int revision, const uint8_t *data, size_t len, rs_land *land,
                           const rs_land_loc_type *types, int ntypes, rs_land_loc *out, int cap,
                           int *count);

/*
 * 317 CollisionMap add and remove for one loc already on a square.
 * x and z are square-local. A border tile (x <= 0 or z <= 0) is left unchanged,
 * matching Client.addLoc. Other revisions return RS_ERR_UNSUPPORTED.
 */
rs_err rs_land_collision_add(rs_land *land, int revision, int plane, int x, int z, int shape, int rot,
                             const rs_land_loc_type *ty);
rs_err rs_land_collision_remove(rs_land *land, int revision, int plane, int x, int z, int shape, int rot,
                                const rs_land_loc_type *ty);

#ifdef __cplusplus
}
#endif

#endif
