#include "rslib/both/pack.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRY(expr)                         \
    do {                                  \
        rs_err _rs_try_err = (expr);      \
        if (_rs_try_err) {                \
            return _rs_try_err;           \
        }                                 \
    } while (0)

enum {
    CAP_STRINGS = 8192,
    CAP_STRING_LEN = 240,
    CAP_NPCS = 8192,
    CAP_ITEMS = 16384,
    CAP_DROPS = 65536,
    CAP_SHOPS = 1024,
    CAP_SPAWNS = 16384,
    CAP_AREAS = 2048,
    CAP_OPTIONS = 16384,
    CAP_RECIPES = 8192,
    CAP_XP = 127,
    CAP_DIALOGUES = 4096,
    CAP_QUESTS = 512
};

#define ENSURE(arr, cap, count, max)                                                       \
    do {                                                                                   \
        if ((count) >= (max)) {                                                            \
            return RS_ERR_RANGE;                                                           \
        }                                                                                  \
        if ((count) >= (cap)) {                                                            \
            int rs_ncap = (cap) > 0 ? (cap) * 2 : 8;                                       \
            void *rs_np;                                                                   \
            if (rs_ncap > (max) || rs_ncap < (cap)) {                                      \
                rs_ncap = (max);                                                           \
            }                                                                              \
            if (rs_ncap < (count) + 1) {                                                   \
                return RS_ERR_NOMEM;                                                       \
            }                                                                              \
            rs_np = realloc((arr), (size_t)rs_ncap * sizeof(*(arr)));                      \
            if (!rs_np) {                                                                  \
                return RS_ERR_NOMEM;                                                       \
            }                                                                              \
            (arr) = rs_np;                                                                 \
            (cap) = rs_ncap;                                                               \
        }                                                                                  \
    } while (0)

typedef struct rs_pack_str {
    uint32_t id;
    uint16_t n;
    char *text;
} rs_pack_str;

struct rs_pack {
    uint16_t revision;
    int has_world;
    rs_pack_world world;
    rs_pack_str *strings;
    int nstrings;
    int cap_strings;
    rs_pack_npc *npcs;
    int nnpcs;
    int cap_npcs;
    rs_pack_item *items;
    int nitems;
    int cap_items;
    rs_pack_drop *drops;
    int ndrops;
    int cap_drops;
    rs_pack_shop *shops;
    int nshops;
    int cap_shops;
    rs_pack_spawn *spawns;
    int nspawns;
    int cap_spawns;
    rs_pack_area *areas;
    int nareas;
    int cap_areas;
    rs_pack_option *options;
    int noptions;
    int cap_options;
    rs_pack_recipe *recipes;
    int nrecipes;
    int cap_recipes;
    uint32_t *xp;
    int nxp;
    rs_pack_dialogue *dialogues;
    int ndialogues;
    int cap_dialogues;
    rs_pack_quest *quests;
    int nquests;
    int cap_quests;
};

static const char *const ACT_NAMES[] = {
    "give_item",        "take_item",      "set_var",           "add_xp",
    "set_quest_stage",  "open_dialogue",  "close_dialogue",    "teleport",
    "spawn_npc",        "remove_npc",     "message",           "play_anim",
    "play_sound",       "open_interface", "set_interface_text", "set_loc",
    "apply_effect"
};

static const char *const COND_NAMES[] = {
    "quest_stage_eq", "quest_stage_ge", "skill_ge", "item_ge", "item_equipped",
    "var_eq",         "var_ge",          "loc_eq",   "in_area"
};

static const char *const INTENT_NAMES[] = {
    "walk", "follow", "talk", "attack", "take", "use_item", "equip", "push_button", "trade", "say"
};

static const char *const EFFECT_NAMES[] = {
    "damage", "heal", "cure", "teleport", "alchemy", "enchant", "bind", "poison", "stun", "drain"
};

/* Stats.java names, including farming and runecraft. */
static const char *const SKILL_NAMES[] = {
    "attack",    "defence",  "strength",    "hitpoints", "ranged",    "prayer",   "magic",
    "cooking",   "woodcutting", "fletching", "fishing",  "firemaking", "crafting", "smithing",
    "mining",    "herblore", "agility",     "thieving",  "slayer",    "farming",  "runecraft",
    "-unused-",  "-unused-", "-unused-",    "-unused-"
};

enum { N_ACT = 17, N_COND = 9, N_INTENT = 10, N_EFFECT = 10, N_SKILL = 25 };

static int name_index(const char *const *names, int n, const char *name) {
    int i;
    if (!name) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        if (strcmp(names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

int rs_pack_act_from_name(const char *name) {
    int i = name_index(ACT_NAMES, N_ACT, name);
    return i < 0 ? -1 : i + 1;
}

const char *rs_pack_act_name(int id) {
    if (id < 1 || id > N_ACT) {
        return NULL;
    }
    return ACT_NAMES[id - 1];
}

int rs_pack_cond_from_name(const char *name) {
    int i = name_index(COND_NAMES, N_COND, name);
    return i < 0 ? -1 : i + 1;
}

const char *rs_pack_cond_name(int id) {
    if (id < 1 || id > N_COND) {
        return NULL;
    }
    return COND_NAMES[id - 1];
}

int rs_pack_intent_from_name(const char *name) {
    int i = name_index(INTENT_NAMES, N_INTENT, name);
    return i < 0 ? -1 : i + 1;
}

const char *rs_pack_intent_name(int id) {
    if (id < 1 || id > N_INTENT) {
        return NULL;
    }
    return INTENT_NAMES[id - 1];
}

int rs_pack_effect_from_name(const char *name) {
    int i = name_index(EFFECT_NAMES, N_EFFECT, name);
    return i < 0 ? -1 : i + 1;
}

const char *rs_pack_effect_name(int id) {
    if (id < 1 || id > N_EFFECT) {
        return NULL;
    }
    return EFFECT_NAMES[id - 1];
}

int rs_pack_skill_from_name(const char *name) {
    return name_index(SKILL_NAMES, N_SKILL, name);
}

const char *rs_pack_skill_name(int id) {
    if (id < 0 || id >= N_SKILL) {
        return NULL;
    }
    return SKILL_NAMES[id];
}

rs_err rs_pack_xp_317(uint32_t out[RS_PACK_XP_317]) {
    int var0 = 0;
    int var1;
    if (!out) {
        return RS_ERR_ARG;
    }
    /*
     * Client.java casts the pow() term with (int), which truncates toward zero.
     * libm pow matches those four published values, so the cast is unadjusted.
     */
    for (var1 = 0; var1 < RS_PACK_XP_317; var1++) {
        int var2 = var1 + 1;
        double raw = (double)var2 + 300.0 * pow(2.0, (double)var2 / 7.0);
        int var3 = (int)raw;
        var0 += var3;
        out[var1] = (uint32_t)(var0 / 4);
    }
    return RS_OK;
}

/* 317 Client.java CHARSET. U+00A3 is the single byte 0xA3, not UTF-8. */
static int charset_ok(uint8_t c) {
    static uint8_t ok[256];
    static int ready;
    if (!ready) {
        static const char lit[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
            "!\"$%^&*()-_=+[{]};:'@#~,<.>/?\\| ";
        size_t i;
        for (i = 0; i < sizeof(lit) - 1; i++) {
            ok[(unsigned char)lit[i]] = 1;
        }
        ok[0xA3] = 1;
        ready = 1;
    }
    return ok[c];
}

static int str_index(const rs_pack *p, uint32_t id) {
    int i;
    for (i = 0; i < p->nstrings; i++) {
        if (p->strings[i].id == id) {
            return i;
        }
    }
    return -1;
}

rs_pack *rs_pack_new(uint16_t revision) {
    rs_pack *p = calloc(1, sizeof(*p));
    if (!p) {
        return NULL;
    }
    p->revision = revision;
    return p;
}

void rs_pack_free(rs_pack *p) {
    int i;
    if (!p) {
        return;
    }
    for (i = 0; i < p->nstrings; i++) {
        free(p->strings[i].text);
    }
    free(p->strings);
    free(p->npcs);
    free(p->items);
    free(p->drops);
    free(p->shops);
    free(p->spawns);
    free(p->areas);
    free(p->options);
    free(p->recipes);
    free(p->xp);
    free(p->dialogues);
    free(p->quests);
    free(p);
}

uint16_t rs_pack_revision(const rs_pack *p) {
    return p ? p->revision : 0;
}

const rs_pack_world *rs_pack_get_world(const rs_pack *p) {
    if (!p || !p->has_world) {
        return NULL;
    }
    return &p->world;
}

rs_err rs_pack_set_world(rs_pack *p, const rs_pack_world *w) {
    if (!p || !w) {
        return RS_ERR_ARG;
    }
    p->world = *w;
    p->has_world = 1;
    return RS_OK;
}

rs_err rs_pack_add_string(rs_pack *p, uint32_t id, const uint8_t *bytes, uint16_t n) {
    char *text;
    uint16_t i;
    if (!p || (n > 0 && !bytes)) {
        return RS_ERR_ARG;
    }
    if (n > CAP_STRING_LEN) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < n; i++) {
        if (!charset_ok(bytes[i])) {
            return RS_ERR_FORMAT;
        }
    }
    if (str_index(p, id) >= 0) {
        return RS_ERR_FORMAT;
    }
    if (p->nstrings >= CAP_STRINGS) {
        return RS_ERR_RANGE;
    }
    text = malloc((size_t)n + 1u);
    if (!text) {
        return RS_ERR_NOMEM;
    }
    if (n) {
        memcpy(text, bytes, n);
    }
    text[n] = 0;
    if (p->nstrings >= p->cap_strings) {
        int ncap = p->cap_strings > 0 ? p->cap_strings * 2 : 8;
        rs_pack_str *grown;
        if (ncap > CAP_STRINGS || ncap < p->cap_strings) {
            ncap = CAP_STRINGS;
        }
        grown = realloc(p->strings, (size_t)ncap * sizeof(*grown));
        if (!grown) {
            free(text);
            return RS_ERR_NOMEM;
        }
        p->strings = grown;
        p->cap_strings = ncap;
    }
    p->strings[p->nstrings].id = id;
    p->strings[p->nstrings].n = n;
    p->strings[p->nstrings].text = text;
    p->nstrings++;
    return RS_OK;
}

const char *rs_pack_string(const rs_pack *p, uint32_t id) {
    int i;
    if (!p) {
        return NULL;
    }
    i = str_index(p, id);
    if (i < 0) {
        return NULL;
    }
    return p->strings[i].text;
}

int rs_pack_string_count(const rs_pack *p) {
    return p ? p->nstrings : 0;
}

static rs_err check_cond_id(uint8_t id) {
    if (id > RS_PACK_COND_IN_AREA) {
        return RS_ERR_RANGE;
    }
    return RS_OK;
}

static rs_err check_act_id(uint8_t id) {
    if (id > RS_PACK_ACT_APPLY_EFFECT) {
        return RS_ERR_RANGE;
    }
    return RS_OK;
}

static rs_err check_reply(const rs_pack_reply *r) {
    int i;
    if (r->ncond > RS_PACK_COND_MAX || r->nact > RS_PACK_ACT_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < r->ncond; i++) {
        TRY(check_cond_id(r->cond[i].id));
    }
    for (i = 0; i < r->nact; i++) {
        TRY(check_act_id(r->act[i].id));
    }
    return RS_OK;
}

static rs_err check_stage(const rs_pack_stage *s) {
    int i;
    if (s->ncond > RS_PACK_COND_MAX || s->nact > RS_PACK_ACT_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < s->ncond; i++) {
        TRY(check_cond_id(s->cond[i].id));
    }
    for (i = 0; i < s->nact; i++) {
        TRY(check_act_id(s->act[i].id));
    }
    return RS_OK;
}

rs_err rs_pack_add_npc(rs_pack *p, const rs_pack_npc *row) {
    int i;
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    for (i = 0; i < p->nnpcs; i++) {
        if (p->npcs[i].id == row->id) {
            return RS_ERR_FORMAT;
        }
    }
    ENSURE(p->npcs, p->cap_npcs, p->nnpcs, CAP_NPCS);
    p->npcs[p->nnpcs++] = *row;
    return RS_OK;
}

rs_err rs_pack_add_item(rs_pack *p, const rs_pack_item *row) {
    int i;
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    for (i = 0; i < p->nitems; i++) {
        if (p->items[i].id == row->id) {
            return RS_ERR_FORMAT;
        }
    }
    ENSURE(p->items, p->cap_items, p->nitems, CAP_ITEMS);
    p->items[p->nitems++] = *row;
    return RS_OK;
}

rs_err rs_pack_add_drop(rs_pack *p, const rs_pack_drop *row) {
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    ENSURE(p->drops, p->cap_drops, p->ndrops, CAP_DROPS);
    p->drops[p->ndrops++] = *row;
    return RS_OK;
}

rs_err rs_pack_add_shop(rs_pack *p, const rs_pack_shop *row) {
    int i;
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    if (row->nstock > RS_PACK_STOCK_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < p->nshops; i++) {
        if (p->shops[i].id == row->id) {
            return RS_ERR_FORMAT;
        }
    }
    ENSURE(p->shops, p->cap_shops, p->nshops, CAP_SHOPS);
    p->shops[p->nshops++] = *row;
    return RS_OK;
}

rs_err rs_pack_add_spawn(rs_pack *p, const rs_pack_spawn *row) {
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    ENSURE(p->spawns, p->cap_spawns, p->nspawns, CAP_SPAWNS);
    p->spawns[p->nspawns++] = *row;
    return RS_OK;
}

rs_err rs_pack_add_area(rs_pack *p, const rs_pack_area *row) {
    int i;
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    for (i = 0; i < p->nareas; i++) {
        if (p->areas[i].id == row->id) {
            return RS_ERR_FORMAT;
        }
    }
    ENSURE(p->areas, p->cap_areas, p->nareas, CAP_AREAS);
    p->areas[p->nareas++] = *row;
    return RS_OK;
}

rs_err rs_pack_add_option(rs_pack *p, const rs_pack_option *row) {
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    if (row->intent > RS_PACK_INTENT_SAY) {
        return RS_ERR_RANGE;
    }
    ENSURE(p->options, p->cap_options, p->noptions, CAP_OPTIONS);
    p->options[p->noptions++] = *row;
    return RS_OK;
}

rs_err rs_pack_add_recipe(rs_pack *p, const rs_pack_recipe *row) {
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    if (row->nin > RS_PACK_RECIPE_IN_MAX || row->effect > RS_PACK_EFFECT_DRAIN) {
        return RS_ERR_RANGE;
    }
    ENSURE(p->recipes, p->cap_recipes, p->nrecipes, CAP_RECIPES);
    p->recipes[p->nrecipes++] = *row;
    return RS_OK;
}

rs_err rs_pack_set_xp(rs_pack *p, const uint32_t *xp, int n) {
    uint32_t *copy;
    if (!p || n < 0 || (n > 0 && !xp)) {
        return RS_ERR_ARG;
    }
    if (n > CAP_XP) {
        return RS_ERR_RANGE;
    }
    if (n == 0) {
        free(p->xp);
        p->xp = NULL;
        p->nxp = 0;
        return RS_OK;
    }
    copy = malloc((size_t)n * sizeof(uint32_t));
    if (!copy) {
        return RS_ERR_NOMEM;
    }
    memcpy(copy, xp, (size_t)n * sizeof(uint32_t));
    free(p->xp);
    p->xp = copy;
    p->nxp = n;
    return RS_OK;
}

rs_err rs_pack_add_dialogue(rs_pack *p, const rs_pack_dialogue *row) {
    int i;
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    if (row->nreplies > RS_PACK_REPLY_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < row->nreplies; i++) {
        TRY(check_reply(&row->replies[i]));
    }
    for (i = 0; i < p->ndialogues; i++) {
        if (p->dialogues[i].id == row->id) {
            return RS_ERR_FORMAT;
        }
    }
    ENSURE(p->dialogues, p->cap_dialogues, p->ndialogues, CAP_DIALOGUES);
    p->dialogues[p->ndialogues++] = *row;
    return RS_OK;
}

rs_err rs_pack_add_quest(rs_pack *p, const rs_pack_quest *row) {
    int i;
    if (!p || !row) {
        return RS_ERR_ARG;
    }
    if (row->nstages > RS_PACK_STAGE_MAX) {
        return RS_ERR_RANGE;
    }
    for (i = 0; i < row->nstages; i++) {
        TRY(check_stage(&row->stages[i]));
    }
    for (i = 0; i < p->nquests; i++) {
        if (p->quests[i].id == row->id) {
            return RS_ERR_FORMAT;
        }
    }
    ENSURE(p->quests, p->cap_quests, p->nquests, CAP_QUESTS);
    p->quests[p->nquests++] = *row;
    return RS_OK;
}

#define AT(arr, n, index) \
    ((!p || (index) < 0 || (index) >= (n)) ? NULL : &(arr)[(index)])

int rs_pack_npc_count(const rs_pack *p) { return p ? p->nnpcs : 0; }
const rs_pack_npc *rs_pack_npc_at(const rs_pack *p, int index) { return AT(p->npcs, p->nnpcs, index); }
const rs_pack_npc *rs_pack_npc_by_id(const rs_pack *p, uint16_t id) {
    int i;
    if (!p) return NULL;
    for (i = 0; i < p->nnpcs; i++) if (p->npcs[i].id == id) return &p->npcs[i];
    return NULL;
}

int rs_pack_item_count(const rs_pack *p) { return p ? p->nitems : 0; }
const rs_pack_item *rs_pack_item_at(const rs_pack *p, int index) { return AT(p->items, p->nitems, index); }
const rs_pack_item *rs_pack_item_by_id(const rs_pack *p, uint16_t id) {
    int i;
    if (!p) return NULL;
    for (i = 0; i < p->nitems; i++) if (p->items[i].id == id) return &p->items[i];
    return NULL;
}

int rs_pack_drop_count(const rs_pack *p) { return p ? p->ndrops : 0; }
const rs_pack_drop *rs_pack_drop_at(const rs_pack *p, int index) { return AT(p->drops, p->ndrops, index); }

int rs_pack_shop_count(const rs_pack *p) { return p ? p->nshops : 0; }
const rs_pack_shop *rs_pack_shop_at(const rs_pack *p, int index) { return AT(p->shops, p->nshops, index); }
const rs_pack_shop *rs_pack_shop_by_id(const rs_pack *p, uint16_t id) {
    int i;
    if (!p) return NULL;
    for (i = 0; i < p->nshops; i++) if (p->shops[i].id == id) return &p->shops[i];
    return NULL;
}

int rs_pack_spawn_count(const rs_pack *p) { return p ? p->nspawns : 0; }
const rs_pack_spawn *rs_pack_spawn_at(const rs_pack *p, int index) { return AT(p->spawns, p->nspawns, index); }

int rs_pack_area_count(const rs_pack *p) { return p ? p->nareas : 0; }
const rs_pack_area *rs_pack_area_at(const rs_pack *p, int index) { return AT(p->areas, p->nareas, index); }
const rs_pack_area *rs_pack_area_by_id(const rs_pack *p, uint16_t id) {
    int i;
    if (!p) return NULL;
    for (i = 0; i < p->nareas; i++) if (p->areas[i].id == id) return &p->areas[i];
    return NULL;
}

int rs_pack_option_count(const rs_pack *p) { return p ? p->noptions : 0; }
const rs_pack_option *rs_pack_option_at(const rs_pack *p, int index) { return AT(p->options, p->noptions, index); }

int rs_pack_recipe_count(const rs_pack *p) { return p ? p->nrecipes : 0; }
const rs_pack_recipe *rs_pack_recipe_at(const rs_pack *p, int index) { return AT(p->recipes, p->nrecipes, index); }
const rs_pack_recipe *rs_pack_recipe_by_id(const rs_pack *p, uint16_t id) {
    int i;
    if (!p) return NULL;
    for (i = 0; i < p->nrecipes; i++) if (p->recipes[i].id == id) return &p->recipes[i];
    return NULL;
}

int rs_pack_xp_count(const rs_pack *p) { return p ? p->nxp : 0; }
const uint32_t *rs_pack_xp(const rs_pack *p) {
    if (!p || p->nxp <= 0) return NULL;
    return p->xp;
}

int rs_pack_dialogue_count(const rs_pack *p) { return p ? p->ndialogues : 0; }
const rs_pack_dialogue *rs_pack_dialogue_at(const rs_pack *p, int index) {
    return AT(p->dialogues, p->ndialogues, index);
}
const rs_pack_dialogue *rs_pack_dialogue_by_id(const rs_pack *p, uint16_t id) {
    int i;
    if (!p) return NULL;
    for (i = 0; i < p->ndialogues; i++) if (p->dialogues[i].id == id) return &p->dialogues[i];
    return NULL;
}

int rs_pack_quest_count(const rs_pack *p) { return p ? p->nquests : 0; }
const rs_pack_quest *rs_pack_quest_at(const rs_pack *p, int index) { return AT(p->quests, p->nquests, index); }
const rs_pack_quest *rs_pack_quest_by_id(const rs_pack *p, uint16_t id) {
    int i;
    if (!p) return NULL;
    for (i = 0; i < p->nquests; i++) if (p->quests[i].id == id) return &p->quests[i];
    return NULL;
}

static rs_err placeholders(const char *s) {
    size_t i;
    if (!s) {
        return RS_ERR_FORMAT;
    }
    for (i = 0; s[i]; i++) {
        if (s[i] == '%') {
            if (s[i + 1] != '1' && s[i + 1] != '2') {
                return RS_ERR_FORMAT;
            }
            i++;
        }
    }
    return RS_OK;
}

static rs_err need_str(const rs_pack *p, uint32_t id) {
    if (str_index(p, id) < 0) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err check_act_strings(const rs_pack *p, const rs_pack_act *a) {
    if (a->id == RS_PACK_ACT_MESSAGE) {
        return need_str(p, (uint32_t)a->p[0]);
    }
    if (a->id == RS_PACK_ACT_SET_INTERFACE_TEXT) {
        return need_str(p, (uint32_t)a->p[1]);
    }
    return RS_OK;
}

rs_err rs_pack_validate(const rs_pack *p) {
    int i, j, k;
    if (!p) {
        return RS_ERR_ARG;
    }
    for (i = 0; i < p->nstrings; i++) {
        TRY(placeholders(p->strings[i].text));
    }
    if (p->has_world) {
        TRY(need_str(p, p->world.welcome_string_id));
    }
    for (i = 0; i < p->nshops; i++) {
        TRY(need_str(p, p->shops[i].name_string_id));
    }
    for (i = 0; i < p->ndialogues; i++) {
        const rs_pack_dialogue *d = &p->dialogues[i];
        TRY(need_str(p, d->text_id));
        for (j = 0; j < d->nreplies; j++) {
            const rs_pack_reply *r = &d->replies[j];
            TRY(need_str(p, r->text_id));
            for (k = 0; k < r->nact; k++) {
                TRY(check_act_strings(p, &r->act[k]));
            }
        }
    }
    for (i = 0; i < p->nquests; i++) {
        const rs_pack_quest *q = &p->quests[i];
        TRY(need_str(p, q->name_string_id));
        for (j = 0; j < q->nstages; j++) {
            const rs_pack_stage *s = &q->stages[j];
            TRY(need_str(p, s->journal_id));
            for (k = 0; k < s->nact; k++) {
                TRY(check_act_strings(p, &s->act[k]));
            }
        }
    }
    return RS_OK;
}

rs_err rs_pack_string_subst(const rs_pack *p, uint32_t id, const char *a, const char *b, char *dst,
                            size_t dst_n) {
    const char *s;
    const char *src;
    size_t need;
    size_t used;
    if (!p || !dst || dst_n == 0) {
        return RS_ERR_ARG;
    }
    src = rs_pack_string(p, id);
    if (!src) {
        return RS_ERR_NOT_FOUND;
    }
    if (!a) {
        a = "";
    }
    if (!b) {
        b = "";
    }
    need = 1;
    for (s = src; *s; s++) {
        size_t add;
        if (*s == '%') {
            const char *rep;
            if (s[1] == '1') {
                rep = a;
            } else if (s[1] == '2') {
                rep = b;
            } else {
                return RS_ERR_FORMAT;
            }
            s++;
            add = strlen(rep);
        } else {
            add = 1;
        }
        if (need > (size_t)-1 - add) {
            return RS_ERR_RANGE;
        }
        need += add;
    }
    if (need > dst_n) {
        return RS_ERR_RANGE;
    }
    used = 0;
    for (s = src; *s; s++) {
        if (*s == '%') {
            const char *rep = s[1] == '1' ? a : b;
            size_t n = strlen(rep);
            memcpy(dst + used, rep, n);
            used += n;
            s++;
        } else {
            dst[used++] = *s;
        }
    }
    dst[used] = 0;
    return RS_OK;
}

static rs_err wbytes(FILE *f, const void *buf, size_t n) {
    if (n && fwrite(buf, 1, n, f) != n) {
        return RS_ERR_IO;
    }
    return RS_OK;
}

static rs_err wu8(FILE *f, uint8_t v) { return wbytes(f, &v, 1); }

static rs_err wu16(FILE *f, uint16_t v) {
    uint8_t b[2];
    b[0] = (uint8_t)v;
    b[1] = (uint8_t)(v >> 8);
    return wbytes(f, b, 2);
}

static rs_err wu32(FILE *f, uint32_t v) {
    uint8_t b[4];
    b[0] = (uint8_t)v;
    b[1] = (uint8_t)(v >> 8);
    b[2] = (uint8_t)(v >> 16);
    b[3] = (uint8_t)(v >> 24);
    return wbytes(f, b, 4);
}

static rs_err wi16(FILE *f, int16_t v) { return wu16(f, (uint16_t)v); }
static rs_err wi32(FILE *f, int32_t v) { return wu32(f, (uint32_t)v); }

static rs_err write_cond(FILE *f, const rs_pack_cond *c) {
    TRY(wu8(f, c->id));
    TRY(wi32(f, c->a));
    TRY(wi32(f, c->b));
    TRY(wi32(f, c->c));
    return RS_OK;
}

static rs_err write_act(FILE *f, const rs_pack_act *a) {
    int i;
    TRY(wu8(f, a->id));
    for (i = 0; i < RS_PACK_ACT_PARAMS; i++) {
        TRY(wi32(f, a->p[i]));
    }
    return RS_OK;
}

static rs_err write_pack(const rs_pack *p, FILE *f) {
    int i, j, k;
    uint8_t magic[4] = {'R', 'S', 'G', 'P'};
    TRY(wbytes(f, magic, 4));
    TRY(wu16(f, RS_PACK_VERSION));
    TRY(wu16(f, p->revision));

    TRY(wu32(f, (uint32_t)p->nstrings));
    for (i = 0; i < p->nstrings; i++) {
        TRY(wu32(f, p->strings[i].id));
        TRY(wu16(f, p->strings[i].n));
        TRY(wbytes(f, p->strings[i].text, p->strings[i].n));
    }

    TRY(wu32(f, p->has_world ? 1u : 0u));
    if (p->has_world) {
        const rs_pack_world *w = &p->world;
        TRY(wu16(f, w->xp_num));
        TRY(wu16(f, w->xp_den));
        TRY(wu8(f, w->members));
        TRY(wu8(f, w->spawn_plane));
        TRY(wu16(f, w->spawn_x));
        TRY(wu16(f, w->spawn_z));
        TRY(wu16(f, w->max_players));
        TRY(wu32(f, w->welcome_string_id));
        TRY(wu32(f, w->pbkdf2_iters));
        TRY(wu16(f, w->port));
        TRY(wu16(f, w->tick_ms));
        TRY(wu16(f, w->shop_restock_ticks));
        TRY(wu16(f, w->autosave_ticks));
        TRY(wu16(f, w->idle_timeout_ticks));
        TRY(wu16(f, w->max_packets_per_tick));
    }

    TRY(wu32(f, (uint32_t)p->nnpcs));
    for (i = 0; i < p->nnpcs; i++) {
        const rs_pack_npc *n = &p->npcs[i];
        TRY(wu16(f, n->id));
        TRY(wu16(f, n->combat));
        TRY(wu16(f, n->hitpoints));
        TRY(wu16(f, n->attack));
        TRY(wu16(f, n->strength));
        TRY(wu16(f, n->defence));
        TRY(wu16(f, n->ranged));
        TRY(wu16(f, n->magic));
        TRY(wu8(f, n->style));
        TRY(wu8(f, n->aggro));
        TRY(wu16(f, n->respawn));
        TRY(wu16(f, n->drop_table));
        TRY(wu8(f, (uint8_t)n->wander));
    }

    TRY(wu32(f, (uint32_t)p->nitems));
    for (i = 0; i < p->nitems; i++) {
        const rs_pack_item *it = &p->items[i];
        TRY(wu16(f, it->id));
        TRY(wu8(f, it->equip_slot));
        TRY(wu8(f, it->weapon_ticks));
        for (j = 0; j < RS_PACK_BONUS; j++) {
            TRY(wi16(f, it->bonus[j]));
        }
        TRY(wi32(f, it->shop_price));
        TRY(wu8(f, it->flags));
        TRY(wu16(f, it->note_link));
    }

    TRY(wu32(f, (uint32_t)p->ndrops));
    for (i = 0; i < p->ndrops; i++) {
        const rs_pack_drop *d = &p->drops[i];
        TRY(wu16(f, d->table_id));
        TRY(wu16(f, d->item_id));
        TRY(wu16(f, d->min_qty));
        TRY(wu16(f, d->max_qty));
        TRY(wu16(f, d->num));
        TRY(wu16(f, d->den));
    }

    TRY(wu32(f, (uint32_t)p->nshops));
    for (i = 0; i < p->nshops; i++) {
        const rs_pack_shop *s = &p->shops[i];
        if (s->nstock > RS_PACK_STOCK_MAX) {
            return RS_ERR_RANGE;
        }
        TRY(wu16(f, s->id));
        TRY(wu32(f, s->name_string_id));
        TRY(wu16(f, s->nstock));
        for (j = 0; j < s->nstock; j++) {
            TRY(wu16(f, s->item[j]));
            TRY(wu16(f, s->amount[j]));
            TRY(wu16(f, s->restock[j]));
        }
    }

    TRY(wu32(f, (uint32_t)p->nspawns));
    for (i = 0; i < p->nspawns; i++) {
        const rs_pack_spawn *s = &p->spawns[i];
        TRY(wu8(f, s->kind));
        TRY(wu16(f, s->id));
        TRY(wu8(f, s->plane));
        TRY(wu16(f, s->x));
        TRY(wu16(f, s->z));
        TRY(wu16(f, s->extra));
        TRY(wu16(f, s->respawn_ticks));
    }

    TRY(wu32(f, (uint32_t)p->nareas));
    for (i = 0; i < p->nareas; i++) {
        const rs_pack_area *a = &p->areas[i];
        TRY(wu16(f, a->id));
        TRY(wu8(f, a->plane));
        TRY(wu16(f, a->x));
        TRY(wu16(f, a->z));
        TRY(wu16(f, a->w));
        TRY(wu16(f, a->h));
        TRY(wu16(f, a->flags));
        TRY(wu16(f, a->music));
    }

    TRY(wu32(f, (uint32_t)p->noptions));
    for (i = 0; i < p->noptions; i++) {
        const rs_pack_option *o = &p->options[i];
        if (o->intent > RS_PACK_INTENT_SAY) {
            return RS_ERR_RANGE;
        }
        TRY(wu8(f, o->kind));
        TRY(wu16(f, o->id));
        TRY(wu8(f, o->index));
        TRY(wu8(f, o->intent));
    }

    TRY(wu32(f, (uint32_t)p->nrecipes));
    for (i = 0; i < p->nrecipes; i++) {
        const rs_pack_recipe *r = &p->recipes[i];
        if (r->nin > RS_PACK_RECIPE_IN_MAX || r->effect > RS_PACK_EFFECT_DRAIN) {
            return RS_ERR_RANGE;
        }
        TRY(wu16(f, r->id));
        TRY(wu8(f, r->skill));
        TRY(wu8(f, r->level));
        TRY(wu32(f, r->xp));
        TRY(wu8(f, r->nin));
        for (j = 0; j < r->nin; j++) {
            TRY(wu16(f, r->in_item[j]));
            TRY(wu16(f, r->in_qty[j]));
        }
        TRY(wu16(f, r->out_item));
        TRY(wu16(f, r->out_qty));
        TRY(wu8(f, r->effect));
        TRY(wi32(f, r->effect_mag));
        TRY(wi32(f, r->effect_dur));
    }

    TRY(wu32(f, (uint32_t)p->nxp));
    for (i = 0; i < p->nxp; i++) {
        TRY(wu32(f, p->xp[i]));
    }

    TRY(wu32(f, (uint32_t)p->ndialogues));
    for (i = 0; i < p->ndialogues; i++) {
        const rs_pack_dialogue *d = &p->dialogues[i];
        if (d->nreplies > RS_PACK_REPLY_MAX) {
            return RS_ERR_RANGE;
        }
        TRY(wu16(f, d->id));
        TRY(wu16(f, d->speaker));
        TRY(wu32(f, d->text_id));
        TRY(wu8(f, d->nreplies));
        for (j = 0; j < d->nreplies; j++) {
            const rs_pack_reply *r = &d->replies[j];
            if (r->ncond > RS_PACK_COND_MAX || r->nact > RS_PACK_ACT_MAX) {
                return RS_ERR_RANGE;
            }
            TRY(wu32(f, r->text_id));
            TRY(wu16(f, r->next_id));
            TRY(wu8(f, r->ncond));
            for (k = 0; k < r->ncond; k++) {
                TRY(write_cond(f, &r->cond[k]));
            }
            TRY(wu8(f, r->nact));
            for (k = 0; k < r->nact; k++) {
                TRY(write_act(f, &r->act[k]));
            }
        }
    }

    TRY(wu32(f, (uint32_t)p->nquests));
    for (i = 0; i < p->nquests; i++) {
        const rs_pack_quest *q = &p->quests[i];
        if (q->nstages > RS_PACK_STAGE_MAX) {
            return RS_ERR_RANGE;
        }
        TRY(wu16(f, q->id));
        TRY(wu32(f, q->name_string_id));
        TRY(wu16(f, q->varp_id));
        TRY(wu16(f, q->varbit_id));
        TRY(wu8(f, q->nstages));
        for (j = 0; j < q->nstages; j++) {
            const rs_pack_stage *s = &q->stages[j];
            if (s->ncond > RS_PACK_COND_MAX || s->nact > RS_PACK_ACT_MAX) {
                return RS_ERR_RANGE;
            }
            TRY(wu8(f, s->index));
            TRY(wu32(f, s->journal_id));
            TRY(wu8(f, s->ncond));
            for (k = 0; k < s->ncond; k++) {
                TRY(write_cond(f, &s->cond[k]));
            }
            TRY(wu8(f, s->nact));
            for (k = 0; k < s->nact; k++) {
                TRY(write_act(f, &s->act[k]));
            }
        }
    }
    return RS_OK;
}

rs_err rs_pack_write(const rs_pack *p, const char *path) {
    FILE *f;
    rs_err e;
    if (!p || !path) {
        return RS_ERR_ARG;
    }
    f = fopen(path, "wb");
    if (!f) {
        return RS_ERR_IO;
    }
    e = write_pack(p, f);
    if (fclose(f) != 0 && e == RS_OK) {
        e = RS_ERR_IO;
    }
    return e;
}

typedef struct rd {
    const uint8_t *p;
    size_t n;
    size_t i;
} rd;

static rs_err need(const rd *r, size_t n) {
    if (r->i > r->n || n > r->n - r->i) {
        return RS_ERR_FORMAT;
    }
    return RS_OK;
}

static rs_err ru8(rd *r, uint8_t *o) {
    TRY(need(r, 1));
    *o = r->p[r->i++];
    return RS_OK;
}

static rs_err ru16(rd *r, uint16_t *o) {
    TRY(need(r, 2));
    *o = (uint16_t)(r->p[r->i] | ((uint16_t)r->p[r->i + 1] << 8));
    r->i += 2;
    return RS_OK;
}

static rs_err ru32(rd *r, uint32_t *o) {
    TRY(need(r, 4));
    *o = (uint32_t)r->p[r->i] | ((uint32_t)r->p[r->i + 1] << 8) |
         ((uint32_t)r->p[r->i + 2] << 16) | ((uint32_t)r->p[r->i + 3] << 24);
    r->i += 4;
    return RS_OK;
}

static rs_err ri16(rd *r, int16_t *o) {
    uint16_t u;
    TRY(ru16(r, &u));
    *o = (int16_t)u;
    return RS_OK;
}

static rs_err ri32(rd *r, int32_t *o) {
    uint32_t u;
    TRY(ru32(r, &u));
    *o = (int32_t)u;
    return RS_OK;
}

static rs_err read_count(rd *r, uint32_t *c, uint32_t cap) {
    TRY(ru32(r, c));
    if (*c > cap) {
        return RS_ERR_RANGE;
    }
    return RS_OK;
}

static rs_err read_cond(rd *r, rs_pack_cond *c) {
    uint8_t id;
    TRY(ru8(r, &id));
    c->id = id;
    TRY(ri32(r, &c->a));
    TRY(ri32(r, &c->b));
    TRY(ri32(r, &c->c));
    return RS_OK;
}

static rs_err read_act(rd *r, rs_pack_act *a) {
    uint8_t id;
    int i;
    TRY(ru8(r, &id));
    a->id = id;
    for (i = 0; i < RS_PACK_ACT_PARAMS; i++) {
        TRY(ri32(r, &a->p[i]));
    }
    return RS_OK;
}

static rs_err read_pack(rs_pack *p, const uint8_t *bytes, size_t n) {
    rd r;
    uint32_t count;
    uint32_t k;
    uint16_t ver;

    r.p = bytes;
    r.n = n;
    r.i = 0;
    if (!bytes && n) {
        return RS_ERR_ARG;
    }
    TRY(need(&r, 4));
    if (r.p[0] != 'R' || r.p[1] != 'S' || r.p[2] != 'G' || r.p[3] != 'P') {
        return RS_ERR_FORMAT;
    }
    r.i = 4;
    TRY(ru16(&r, &ver));
    if (ver != RS_PACK_VERSION) {
        return RS_ERR_VERSION;
    }
    TRY(ru16(&r, &p->revision));

    TRY(read_count(&r, &count, CAP_STRINGS));
    for (k = 0; k < count; k++) {
        uint32_t id;
        uint16_t nbytes;
        rs_err e;
        TRY(ru32(&r, &id));
        TRY(ru16(&r, &nbytes));
        if (nbytes > CAP_STRING_LEN) {
            return RS_ERR_RANGE;
        }
        TRY(need(&r, nbytes));
        e = rs_pack_add_string(p, id, r.p + r.i, nbytes);
        if (e) {
            return e;
        }
        r.i += nbytes;
    }

    TRY(ru32(&r, &count));
    if (count > 1) {
        return RS_ERR_FORMAT;
    }
    if (count == 1) {
        rs_pack_world w;
        memset(&w, 0, sizeof(w));
        TRY(ru16(&r, &w.xp_num));
        TRY(ru16(&r, &w.xp_den));
        TRY(ru8(&r, &w.members));
        TRY(ru8(&r, &w.spawn_plane));
        TRY(ru16(&r, &w.spawn_x));
        TRY(ru16(&r, &w.spawn_z));
        TRY(ru16(&r, &w.max_players));
        TRY(ru32(&r, &w.welcome_string_id));
        TRY(ru32(&r, &w.pbkdf2_iters));
        TRY(ru16(&r, &w.port));
        TRY(ru16(&r, &w.tick_ms));
        TRY(ru16(&r, &w.shop_restock_ticks));
        TRY(ru16(&r, &w.autosave_ticks));
        TRY(ru16(&r, &w.idle_timeout_ticks));
        TRY(ru16(&r, &w.max_packets_per_tick));
        TRY(rs_pack_set_world(p, &w));
    }

    TRY(read_count(&r, &count, CAP_NPCS));
    for (k = 0; k < count; k++) {
        rs_pack_npc row;
        uint8_t u;
        memset(&row, 0, sizeof(row));
        TRY(ru16(&r, &row.id));
        TRY(ru16(&r, &row.combat));
        TRY(ru16(&r, &row.hitpoints));
        TRY(ru16(&r, &row.attack));
        TRY(ru16(&r, &row.strength));
        TRY(ru16(&r, &row.defence));
        TRY(ru16(&r, &row.ranged));
        TRY(ru16(&r, &row.magic));
        TRY(ru8(&r, &row.style));
        TRY(ru8(&r, &row.aggro));
        TRY(ru16(&r, &row.respawn));
        TRY(ru16(&r, &row.drop_table));
        TRY(ru8(&r, &u));
        row.wander = (int8_t)u;
        TRY(rs_pack_add_npc(p, &row));
    }

    TRY(read_count(&r, &count, CAP_ITEMS));
    for (k = 0; k < count; k++) {
        rs_pack_item row;
        int b;
        memset(&row, 0, sizeof(row));
        TRY(ru16(&r, &row.id));
        TRY(ru8(&r, &row.equip_slot));
        TRY(ru8(&r, &row.weapon_ticks));
        for (b = 0; b < RS_PACK_BONUS; b++) {
            TRY(ri16(&r, &row.bonus[b]));
        }
        TRY(ri32(&r, &row.shop_price));
        TRY(ru8(&r, &row.flags));
        TRY(ru16(&r, &row.note_link));
        TRY(rs_pack_add_item(p, &row));
    }

    TRY(read_count(&r, &count, CAP_DROPS));
    for (k = 0; k < count; k++) {
        rs_pack_drop row;
        memset(&row, 0, sizeof(row));
        TRY(ru16(&r, &row.table_id));
        TRY(ru16(&r, &row.item_id));
        TRY(ru16(&r, &row.min_qty));
        TRY(ru16(&r, &row.max_qty));
        TRY(ru16(&r, &row.num));
        TRY(ru16(&r, &row.den));
        TRY(rs_pack_add_drop(p, &row));
    }

    TRY(read_count(&r, &count, CAP_SHOPS));
    for (k = 0; k < count; k++) {
        rs_pack_shop row;
        uint16_t s;
        memset(&row, 0, sizeof(row));
        TRY(ru16(&r, &row.id));
        TRY(ru32(&r, &row.name_string_id));
        TRY(ru16(&r, &row.nstock));
        if (row.nstock > RS_PACK_STOCK_MAX) {
            return RS_ERR_RANGE;
        }
        for (s = 0; s < row.nstock; s++) {
            TRY(ru16(&r, &row.item[s]));
            TRY(ru16(&r, &row.amount[s]));
            TRY(ru16(&r, &row.restock[s]));
        }
        TRY(rs_pack_add_shop(p, &row));
    }

    TRY(read_count(&r, &count, CAP_SPAWNS));
    for (k = 0; k < count; k++) {
        rs_pack_spawn row;
        memset(&row, 0, sizeof(row));
        TRY(ru8(&r, &row.kind));
        TRY(ru16(&r, &row.id));
        TRY(ru8(&r, &row.plane));
        TRY(ru16(&r, &row.x));
        TRY(ru16(&r, &row.z));
        TRY(ru16(&r, &row.extra));
        TRY(ru16(&r, &row.respawn_ticks));
        TRY(rs_pack_add_spawn(p, &row));
    }

    TRY(read_count(&r, &count, CAP_AREAS));
    for (k = 0; k < count; k++) {
        rs_pack_area row;
        memset(&row, 0, sizeof(row));
        TRY(ru16(&r, &row.id));
        TRY(ru8(&r, &row.plane));
        TRY(ru16(&r, &row.x));
        TRY(ru16(&r, &row.z));
        TRY(ru16(&r, &row.w));
        TRY(ru16(&r, &row.h));
        TRY(ru16(&r, &row.flags));
        TRY(ru16(&r, &row.music));
        TRY(rs_pack_add_area(p, &row));
    }

    TRY(read_count(&r, &count, CAP_OPTIONS));
    for (k = 0; k < count; k++) {
        rs_pack_option row;
        memset(&row, 0, sizeof(row));
        TRY(ru8(&r, &row.kind));
        TRY(ru16(&r, &row.id));
        TRY(ru8(&r, &row.index));
        TRY(ru8(&r, &row.intent));
        TRY(rs_pack_add_option(p, &row));
    }

    TRY(read_count(&r, &count, CAP_RECIPES));
    for (k = 0; k < count; k++) {
        rs_pack_recipe row;
        uint8_t in;
        memset(&row, 0, sizeof(row));
        TRY(ru16(&r, &row.id));
        TRY(ru8(&r, &row.skill));
        TRY(ru8(&r, &row.level));
        TRY(ru32(&r, &row.xp));
        TRY(ru8(&r, &row.nin));
        if (row.nin > RS_PACK_RECIPE_IN_MAX) {
            return RS_ERR_RANGE;
        }
        for (in = 0; in < row.nin; in++) {
            TRY(ru16(&r, &row.in_item[in]));
            TRY(ru16(&r, &row.in_qty[in]));
        }
        TRY(ru16(&r, &row.out_item));
        TRY(ru16(&r, &row.out_qty));
        TRY(ru8(&r, &row.effect));
        TRY(ri32(&r, &row.effect_mag));
        TRY(ri32(&r, &row.effect_dur));
        TRY(rs_pack_add_recipe(p, &row));
    }

    TRY(read_count(&r, &count, CAP_XP));
    if (count == 0) {
        TRY(rs_pack_set_xp(p, NULL, 0));
    } else {
        uint32_t xp[CAP_XP];
        for (k = 0; k < count; k++) {
            TRY(ru32(&r, &xp[k]));
        }
        TRY(rs_pack_set_xp(p, xp, (int)count));
    }

    TRY(read_count(&r, &count, CAP_DIALOGUES));
    for (k = 0; k < count; k++) {
        rs_pack_dialogue row;
        uint8_t nr;
        memset(&row, 0, sizeof(row));
        TRY(ru16(&r, &row.id));
        TRY(ru16(&r, &row.speaker));
        TRY(ru32(&r, &row.text_id));
        TRY(ru8(&r, &nr));
        if (nr > RS_PACK_REPLY_MAX) {
            return RS_ERR_RANGE;
        }
        row.nreplies = nr;
        for (nr = 0; nr < row.nreplies; nr++) {
            rs_pack_reply *rep = &row.replies[nr];
            uint8_t nc;
            TRY(ru32(&r, &rep->text_id));
            TRY(ru16(&r, &rep->next_id));
            TRY(ru8(&r, &nc));
            if (nc > RS_PACK_COND_MAX) {
                return RS_ERR_RANGE;
            }
            rep->ncond = nc;
            for (nc = 0; nc < rep->ncond; nc++) {
                TRY(read_cond(&r, &rep->cond[nc]));
            }
            TRY(ru8(&r, &nc));
            if (nc > RS_PACK_ACT_MAX) {
                return RS_ERR_RANGE;
            }
            rep->nact = nc;
            for (nc = 0; nc < rep->nact; nc++) {
                TRY(read_act(&r, &rep->act[nc]));
            }
        }
        TRY(rs_pack_add_dialogue(p, &row));
    }

    TRY(read_count(&r, &count, CAP_QUESTS));
    for (k = 0; k < count; k++) {
        rs_pack_quest row;
        uint8_t ns;
        memset(&row, 0, sizeof(row));
        TRY(ru16(&r, &row.id));
        TRY(ru32(&r, &row.name_string_id));
        TRY(ru16(&r, &row.varp_id));
        TRY(ru16(&r, &row.varbit_id));
        TRY(ru8(&r, &ns));
        if (ns > RS_PACK_STAGE_MAX) {
            return RS_ERR_RANGE;
        }
        row.nstages = ns;
        for (ns = 0; ns < row.nstages; ns++) {
            rs_pack_stage *st = &row.stages[ns];
            uint8_t nc;
            TRY(ru8(&r, &st->index));
            TRY(ru32(&r, &st->journal_id));
            TRY(ru8(&r, &nc));
            if (nc > RS_PACK_COND_MAX) {
                return RS_ERR_RANGE;
            }
            st->ncond = nc;
            for (nc = 0; nc < st->ncond; nc++) {
                TRY(read_cond(&r, &st->cond[nc]));
            }
            TRY(ru8(&r, &nc));
            if (nc > RS_PACK_ACT_MAX) {
                return RS_ERR_RANGE;
            }
            st->nact = nc;
            for (nc = 0; nc < st->nact; nc++) {
                TRY(read_act(&r, &st->act[nc]));
            }
        }
        TRY(rs_pack_add_quest(p, &row));
    }
    return RS_OK;
}

rs_err rs_pack_load_mem(const uint8_t *bytes, size_t n, rs_pack **out) {
    rs_pack *p;
    rs_err e;
    if (!out) {
        return RS_ERR_ARG;
    }
    *out = NULL;
    if (!bytes && n) {
        return RS_ERR_ARG;
    }
    p = rs_pack_new(0);
    if (!p) {
        return RS_ERR_NOMEM;
    }
    e = read_pack(p, bytes, n);
    if (e) {
        rs_pack_free(p);
        return e;
    }
    *out = p;
    return RS_OK;
}

rs_err rs_pack_load(const char *path, rs_pack **out) {
    FILE *f;
    long sz;
    uint8_t *buf;
    size_t nread;
    rs_err e;
    if (!out) {
        return RS_ERR_ARG;
    }
    *out = NULL;
    if (!path) {
        return RS_ERR_ARG;
    }
    f = fopen(path, "rb");
    if (!f) {
        return RS_ERR_IO;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return RS_ERR_IO;
    }
    sz = ftell(f);
    if (sz < 0 || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return RS_ERR_IO;
    }
    buf = malloc(sz ? (size_t)sz : 1);
    if (!buf) {
        fclose(f);
        return RS_ERR_NOMEM;
    }
    nread = sz ? fread(buf, 1, (size_t)sz, f) : 0;
    fclose(f);
    if (nread != (size_t)sz) {
        free(buf);
        return RS_ERR_IO;
    }
    e = rs_pack_load_mem(buf, (size_t)sz, out);
    free(buf);
    return e;
}
