#include "rslib/both/crypto.h"
#include "rslib/both/pack.h"
#include "rslib/both/path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fail;

static void expect(int cond, const char *msg) {
    if (!cond) {
        fprintf(stderr, "fail: %s\n", msg);
        g_fail = 1;
    }
}

static void expect_err(rs_err got, rs_err want, const char *msg) {
    if (got != want) {
        fprintf(stderr, "fail: %s (got %d want %d)\n", msg, (int)got, (int)want);
        g_fail = 1;
    }
}

static int hex_eq(const uint8_t *got, const char *hex) {
    size_t i;
    size_t n = strlen(hex);
    if (n != 64) {
        return 0;
    }
    for (i = 0; i < 32; i++) {
        unsigned v;
        if (sscanf(hex + i * 2, "%2x", &v) != 1) {
            return 0;
        }
        if (got[i] != (uint8_t)v) {
            return 0;
        }
    }
    return 1;
}

static void test_sha(void) {
    uint8_t out[32];
    static const char abc[] = "abc";
    static const char long56[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    rs_sha256("", 0, out);
    expect(hex_eq(out, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"),
           "sha256 empty");
    rs_sha256(abc, 3, out);
    expect(hex_eq(out, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"),
           "sha256 abc");
    rs_sha256(long56, 56, out);
    expect(hex_eq(out, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"),
           "sha256 56");
}

static void test_pbkdf2(void) {
    uint8_t out[32];
    rs_pbkdf2_hmac_sha256("password", 8, "salt", 4, 1, out, 32);
    expect(hex_eq(out, "120fb6cffcf8b32c43e7225256c4f837a86548c92ccc35480805987cb70be17b"),
           "pbkdf2 c=1");
    rs_pbkdf2_hmac_sha256("password", 8, "salt", 4, 2, out, 32);
    expect(hex_eq(out, "ae4d0c95af6b46d32d0adff928f06dd02a303f8ef3c251dfd6e2d85a95474c43"),
           "pbkdf2 c=2");
}

static void test_xp(void) {
    uint32_t xp[RS_PACK_XP_317];
    expect_err(rs_pack_xp_317(xp), RS_OK, "xp 317");
    expect(xp[0] == 83 && xp[1] == 174 && xp[2] == 276 && xp[3] == 388, "xp first four");
    expect(xp[97] == 13034431u && xp[98] == 14391160u, "xp level 99/100");
    expect_err(rs_pack_xp_317(NULL), RS_ERR_ARG, "xp null");
}

static void test_names(void) {
    static const char *acts[] = {
        "give_item", "take_item", "set_var", "add_xp", "set_quest_stage", "open_dialogue",
        "close_dialogue", "teleport", "spawn_npc", "remove_npc", "message", "play_anim",
        "play_sound", "open_interface", "set_interface_text", "set_loc", "apply_effect"
    };
    static const char *conds[] = {
        "quest_stage_eq", "quest_stage_ge", "skill_ge", "item_ge", "item_equipped",
        "var_eq", "var_ge", "loc_eq", "in_area"
    };
    static const char *intents[] = {
        "walk", "follow", "talk", "attack", "take", "use_item", "equip", "push_button", "trade", "say"
    };
    static const char *effects[] = {
        "damage", "heal", "cure", "teleport", "alchemy", "enchant", "bind", "poison", "stun", "drain"
    };
    int i;
    for (i = 0; i < 17; i++) {
        if (rs_pack_act_from_name(acts[i]) != i + 1 || strcmp(rs_pack_act_name(i + 1), acts[i]) != 0) {
            expect(0, "act name");
        }
    }
    for (i = 0; i < 9; i++) {
        if (rs_pack_cond_from_name(conds[i]) != i + 1 || strcmp(rs_pack_cond_name(i + 1), conds[i]) != 0) {
            expect(0, "cond name");
        }
    }
    for (i = 0; i < 10; i++) {
        if (rs_pack_intent_from_name(intents[i]) != i + 1 ||
            strcmp(rs_pack_intent_name(i + 1), intents[i]) != 0) {
            expect(0, "intent name");
        }
    }
    for (i = 0; i < 10; i++) {
        if (rs_pack_effect_from_name(effects[i]) != i + 1 ||
            strcmp(rs_pack_effect_name(i + 1), effects[i]) != 0) {
            expect(0, "effect name");
        }
    }
    expect(rs_pack_act_from_name("none") == -1 && rs_pack_act_name(0) == NULL, "act none");
    expect(rs_pack_act_from_name(NULL) == -1 && rs_pack_skill_from_name(NULL) == -1, "null name");
    expect(rs_pack_skill_from_name("attack") == 0, "skill attack");
    expect(rs_pack_skill_from_name("farming") == 19, "skill farming");
    expect(rs_pack_skill_from_name("runecraft") == 20, "skill runecraft");
    expect(rs_pack_skill_name(19) && strcmp(rs_pack_skill_name(19), "farming") == 0, "skill name 19");
    expect(rs_pack_skill_name(20) && strcmp(rs_pack_skill_name(20), "runecraft") == 0, "skill name 20");
    expect(rs_pack_skill_name(21) && strcmp(rs_pack_skill_name(21), "-unused-") == 0, "skill unused");
    expect(rs_pack_skill_name(24) && strcmp(rs_pack_skill_name(24), "-unused-") == 0, "skill last");
    expect(rs_pack_skill_name(25) == NULL && rs_pack_skill_from_name("nope") == -1, "skill missing");
}

static int add_str(rs_pack *p, uint32_t id, const char *s) {
    return rs_pack_add_string(p, id, (const uint8_t *)s, (uint16_t)strlen(s)) == RS_OK;
}

static void test_roundtrip(void) {
    rs_pack *p;
    rs_pack *q = NULL;
    rs_pack_world world;
    rs_pack_npc npc;
    rs_pack_item item;
    rs_pack_drop drop;
    rs_pack_shop shop;
    rs_pack_spawn spawn;
    rs_pack_area area;
    rs_pack_option opt;
    rs_pack_recipe recipe;
    rs_pack_dialogue dlg;
    rs_pack_quest quest;
    uint32_t xp[RS_PACK_XP_317];
    uint8_t pound[3] = {'G', 'P', 0xA3};
    const char *path = "/tmp/rsgp_test_pack.bin";
    char subst[64];
    const rs_pack_npc *ln;
    const rs_pack_item *li;
    const rs_pack_shop *ls;
    const rs_pack_dialogue *ld;
    const rs_pack_quest *lq;
    const rs_pack_world *lw;
    const uint8_t *sp;
    int i;

    p = rs_pack_new(317);
    expect(p != NULL, "pack new");
    if (!p) {
        return;
    }
    memset(&world, 0, sizeof(world));
    world.xp_num = 1;
    world.xp_den = 1;
    world.members = 1;
    world.spawn_plane = 2;
    world.spawn_x = 3200;
    world.spawn_z = 3200;
    world.max_players = 2000;
    world.welcome_string_id = 1;
    world.pbkdf2_iters = 10000;
    world.port = 43594;
    world.tick_ms = 600;
    world.shop_restock_ticks = 100;
    world.autosave_ticks = 200;
    world.idle_timeout_ticks = 300;
    world.max_packets_per_tick = 50;
    expect_err(rs_pack_set_world(p, &world), RS_OK, "set world");

    expect(add_str(p, 1, "Welcome"), "str welcome");
    expect(add_str(p, 2, "General Store"), "str shop");
    expect(add_str(p, 3, "Hello %1"), "str dlg");
    expect(add_str(p, 4, "Bye %2"), "str reply");
    expect(add_str(p, 5, "Cook's Assistant"), "str quest");
    expect(add_str(p, 6, "Journal"), "str journal");
    expect(add_str(p, 7, "You hear a sound"), "str msg");
    expect(add_str(p, 8, "Interface"), "str iface");
    expect_err(rs_pack_add_string(p, 9, pound, 3), RS_OK, "str pound");
    expect_err(rs_pack_add_string(p, 1, (const uint8_t *)"x", 1), RS_ERR_FORMAT, "dup string");
    expect_err(rs_pack_add_string(p, 10, (const uint8_t *)"a\nb", 3), RS_ERR_FORMAT, "reject lf");
    expect_err(rs_pack_add_string(p, 10, (const uint8_t *)"a\0b", 3), RS_ERR_FORMAT, "reject nul");
    {
        uint8_t big[241];
        memset(big, 'A', sizeof(big));
        expect_err(rs_pack_add_string(p, 10, big, 241), RS_ERR_RANGE, "string cap");
    }
    expect(rs_pack_string(p, 1) && strcmp(rs_pack_string(p, 1), "Welcome") == 0, "string ptr");
    expect(rs_pack_string(p, 99) == NULL, "string missing");
    sp = (const uint8_t *)rs_pack_string(p, 9);
    expect(sp && sp[0] == 'G' && sp[1] == 'P' && sp[2] == 0xA3 && sp[3] == 0, "pound bytes");

    memset(&npc, 0, sizeof(npc));
    npc.id = 7;
    npc.combat = 2;
    npc.hitpoints = 10;
    npc.attack = 1;
    npc.strength = 1;
    npc.defence = 1;
    npc.ranged = 1;
    npc.magic = 1;
    npc.style = 3;
    npc.aggro = 1;
    npc.respawn = 50;
    npc.drop_table = 4;
    npc.wander = -3;
    expect_err(rs_pack_add_npc(p, &npc), RS_OK, "add npc");
    expect_err(rs_pack_add_npc(p, &npc), RS_ERR_FORMAT, "dup npc");

    memset(&item, 0, sizeof(item));
    item.id = 995;
    item.equip_slot = RS_PACK_EQUIP_NONE;
    item.weapon_ticks = 4;
    for (i = 0; i < RS_PACK_BONUS; i++) {
        item.bonus[i] = (int16_t)(i - 1);
    }
    item.shop_price = -10;
    item.flags = (uint8_t)(RS_PACK_ITEM_STACK | RS_PACK_ITEM_NOTED);
    item.note_link = 996;
    expect_err(rs_pack_add_item(p, &item), RS_OK, "add item");
    expect_err(rs_pack_add_item(p, &item), RS_ERR_FORMAT, "dup item");

    memset(&drop, 0, sizeof(drop));
    drop.table_id = 4;
    drop.item_id = 995;
    drop.min_qty = 1;
    drop.max_qty = 5;
    drop.num = 1;
    drop.den = 10;
    expect_err(rs_pack_add_drop(p, &drop), RS_OK, "add drop");
    expect_err(rs_pack_add_drop(p, &drop), RS_OK, "drop dup ok");

    memset(&shop, 0, sizeof(shop));
    shop.id = 1;
    shop.name_string_id = 2;
    shop.nstock = 2;
    shop.item[0] = 995;
    shop.amount[0] = 10;
    shop.restock[0] = 100;
    shop.item[1] = 526;
    shop.amount[1] = 5;
    shop.restock[1] = 50;
    expect_err(rs_pack_add_shop(p, &shop), RS_OK, "add shop");
    expect_err(rs_pack_add_shop(p, &shop), RS_ERR_FORMAT, "dup shop");
    shop.nstock = 41;
    shop.id = 2;
    expect_err(rs_pack_add_shop(p, &shop), RS_ERR_RANGE, "shop stock");
    shop.nstock = 2;
    shop.id = 1;

    memset(&spawn, 0, sizeof(spawn));
    spawn.kind = RS_PACK_SPAWN_NPC;
    spawn.id = 7;
    spawn.plane = 0;
    spawn.x = 3200;
    spawn.z = 3201;
    spawn.extra = 0;
    spawn.respawn_ticks = 100;
    expect_err(rs_pack_add_spawn(p, &spawn), RS_OK, "add spawn");

    memset(&area, 0, sizeof(area));
    area.id = 3;
    area.plane = 0;
    area.x = 3200;
    area.z = 3200;
    area.w = 10;
    area.h = 10;
    area.flags = (uint16_t)(RS_PACK_AREA_MULTI | RS_PACK_AREA_WILDERNESS | RS_PACK_AREA_MEMBERS);
    area.music = 12;
    expect_err(rs_pack_add_area(p, &area), RS_OK, "add area");
    expect_err(rs_pack_add_area(p, &area), RS_ERR_FORMAT, "dup area");

    memset(&opt, 0, sizeof(opt));
    opt.kind = RS_PACK_OPT_NPC;
    opt.id = 7;
    opt.index = 0;
    opt.intent = RS_PACK_INTENT_TALK;
    expect_err(rs_pack_add_option(p, &opt), RS_OK, "add option");
    opt.intent = 99;
    opt.id = 8;
    expect_err(rs_pack_add_option(p, &opt), RS_ERR_RANGE, "bad intent");

    memset(&recipe, 0, sizeof(recipe));
    recipe.id = 9;
    recipe.skill = 7;
    recipe.level = 10;
    recipe.xp = 40;
    recipe.nin = 2;
    recipe.in_item[0] = 995;
    recipe.in_qty[0] = 1;
    recipe.in_item[1] = 526;
    recipe.in_qty[1] = 2;
    recipe.out_item = 315;
    recipe.out_qty = 1;
    recipe.effect = RS_PACK_EFFECT_HEAL;
    recipe.effect_mag = 5;
    recipe.effect_dur = -1;
    expect_err(rs_pack_add_recipe(p, &recipe), RS_OK, "add recipe");
    recipe.effect = 50;
    recipe.id = 10;
    expect_err(rs_pack_add_recipe(p, &recipe), RS_ERR_RANGE, "bad effect");

    expect_err(rs_pack_xp_317(xp), RS_OK, "xp fill");
    expect_err(rs_pack_set_xp(p, xp, RS_PACK_XP_317), RS_OK, "set xp");
    expect_err(rs_pack_set_xp(p, xp, 128), RS_ERR_RANGE, "xp cap");

    memset(&dlg, 0, sizeof(dlg));
    dlg.id = 11;
    dlg.speaker = 0;
    dlg.text_id = 3;
    dlg.nreplies = 1;
    dlg.replies[0].text_id = 4;
    dlg.replies[0].next_id = 0;
    dlg.replies[0].ncond = 1;
    dlg.replies[0].cond[0].id = RS_PACK_COND_SKILL_GE;
    dlg.replies[0].cond[0].a = 7;
    dlg.replies[0].cond[0].b = 10;
    dlg.replies[0].cond[0].c = -2;
    dlg.replies[0].nact = 2;
    dlg.replies[0].act[0].id = RS_PACK_ACT_MESSAGE;
    dlg.replies[0].act[0].p[0] = 7;
    dlg.replies[0].act[1].id = RS_PACK_ACT_GIVE_ITEM;
    dlg.replies[0].act[1].p[0] = 995;
    dlg.replies[0].act[1].p[1] = 1;
    expect_err(rs_pack_add_dialogue(p, &dlg), RS_OK, "add dialogue");
    expect_err(rs_pack_add_dialogue(p, &dlg), RS_ERR_FORMAT, "dup dialogue");
    dlg.id = 12;
    dlg.replies[0].act[0].id = 99;
    expect_err(rs_pack_add_dialogue(p, &dlg), RS_ERR_RANGE, "unknown action");
    dlg.replies[0].act[0].id = RS_PACK_ACT_MESSAGE;
    dlg.replies[0].cond[0].id = 40;
    expect_err(rs_pack_add_dialogue(p, &dlg), RS_ERR_RANGE, "unknown cond");

    memset(&quest, 0, sizeof(quest));
    quest.id = 15;
    quest.name_string_id = 5;
    quest.varp_id = RS_PACK_VARP_UNREAD;
    quest.varbit_id = RS_PACK_VARP_UNREAD;
    quest.nstages = 1;
    quest.stages[0].index = 0;
    quest.stages[0].journal_id = 6;
    quest.stages[0].ncond = 1;
    quest.stages[0].cond[0].id = RS_PACK_COND_QUEST_STAGE_EQ;
    quest.stages[0].cond[0].a = 15;
    quest.stages[0].cond[0].b = 0;
    quest.stages[0].nact = 1;
    quest.stages[0].act[0].id = RS_PACK_ACT_SET_INTERFACE_TEXT;
    quest.stages[0].act[0].p[0] = 100;
    quest.stages[0].act[0].p[1] = 8;
    expect_err(rs_pack_add_quest(p, &quest), RS_OK, "add quest");
    expect_err(rs_pack_add_quest(p, &quest), RS_ERR_FORMAT, "dup quest");
    expect_err(rs_pack_validate(p), RS_OK, "validate full");
    expect_err(rs_pack_string_subst(p, 3, "bob", NULL, subst, sizeof(subst)), RS_OK, "subst");
    expect(strcmp(subst, "Hello bob") == 0, "subst text");
    expect_err(rs_pack_string_subst(p, 4, NULL, "sam", subst, sizeof(subst)), RS_OK, "subst 2");
    expect(strcmp(subst, "Bye sam") == 0, "subst text 2");

    expect_err(rs_pack_write(p, path), RS_OK, "write");
    expect_err(rs_pack_load(path, &q), RS_OK, "load");
    expect(q != NULL, "loaded");
    if (!q) {
        rs_pack_free(p);
        remove(path);
        return;
    }
    expect(rs_pack_revision(q) == 317, "revision");
    expect(rs_pack_string_count(q) == 9, "string count");
    expect(rs_pack_string(q, 5) && strcmp(rs_pack_string(q, 5), "Cook's Assistant") == 0, "quest name");
    sp = (const uint8_t *)rs_pack_string(q, 9);
    expect(sp && sp[2] == 0xA3, "loaded pound");
    lw = rs_pack_get_world(q);
    expect(lw && lw->port == 43594 && lw->welcome_string_id == 1 && lw->members == 1, "world");
    expect(lw && lw->spawn_x == 3200 && lw->max_packets_per_tick == 50 && lw->pbkdf2_iters == 10000,
           "world rest");
    expect(rs_pack_npc_count(q) == 1, "npc count");
    ln = rs_pack_npc_by_id(q, 7);
    expect(ln && ln->wander == -3 && ln->hitpoints == 10 && ln->drop_table == 4 && ln->style == 3,
           "npc fields");
    expect(rs_pack_npc_at(q, 1) == NULL && rs_pack_npc_by_id(q, 8) == NULL, "npc miss");
    li = rs_pack_item_by_id(q, 995);
    expect(li && li->equip_slot == 255 && li->shop_price == -10 && li->bonus[0] == -1, "item");
    expect(li && li->bonus[11] == 10 && li->flags == 3 && li->note_link == 996 && li->weapon_ticks == 4,
           "item rest");
    expect(rs_pack_drop_count(q) == 2, "drops");
    expect(rs_pack_drop_at(q, 0) && rs_pack_drop_at(q, 0)->den == 10 &&
               rs_pack_drop_at(q, 0)->max_qty == 5,
           "drop fields");
    ls = rs_pack_shop_by_id(q, 1);
    expect(ls && ls->nstock == 2 && ls->name_string_id == 2 && ls->item[1] == 526 && ls->restock[0] == 100,
           "shop");
    expect(rs_pack_spawn_count(q) == 1 && rs_pack_spawn_at(q, 0)->z == 3201 &&
               rs_pack_spawn_at(q, 0)->kind == RS_PACK_SPAWN_NPC,
           "spawn");
    expect(rs_pack_area_by_id(q, 3) && rs_pack_area_by_id(q, 3)->flags == 7 &&
               rs_pack_area_by_id(q, 3)->music == 12 && rs_pack_area_by_id(q, 3)->w == 10,
           "area");
    expect(rs_pack_option_at(q, 0) && rs_pack_option_at(q, 0)->intent == RS_PACK_INTENT_TALK, "option");
    expect(rs_pack_recipe_by_id(q, 9) && rs_pack_recipe_by_id(q, 9)->nin == 2 &&
               rs_pack_recipe_by_id(q, 9)->effect_dur == -1 &&
               rs_pack_recipe_by_id(q, 9)->in_qty[1] == 2 &&
               rs_pack_recipe_by_id(q, 9)->out_item == 315,
           "recipe");
    expect(rs_pack_xp_count(q) == 99 && rs_pack_xp(q) && rs_pack_xp(q)[0] == 83 &&
               rs_pack_xp(q)[3] == 388,
           "xp loaded");
    ld = rs_pack_dialogue_by_id(q, 11);
    expect(ld && ld->text_id == 3 && ld->nreplies == 1 && ld->replies[0].nact == 2, "dialogue");
    expect(ld && ld->replies[0].cond[0].id == RS_PACK_COND_SKILL_GE && ld->replies[0].cond[0].c == -2 &&
               ld->replies[0].act[0].p[0] == 7 && ld->replies[0].act[1].id == RS_PACK_ACT_GIVE_ITEM,
           "dialogue acts");
    lq = rs_pack_quest_by_id(q, 15);
    expect(lq && lq->varp_id == 0xFFFF && lq->name_string_id == 5 && lq->nstages == 1, "quest");
    expect(lq && lq->stages[0].journal_id == 6 &&
               lq->stages[0].act[0].id == RS_PACK_ACT_SET_INTERFACE_TEXT &&
               lq->stages[0].act[0].p[1] == 8,
           "quest stage");
    expect_err(rs_pack_validate(q), RS_OK, "validate loaded");
    rs_pack_free(q);
    rs_pack_free(p);
    remove(path);
}

static uint16_t rdu16(const uint8_t *p) {
    return (uint16_t)(p[0] | (p[1] << 8));
}

static uint32_t rdu32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void test_layout_and_reject(void) {
    rs_pack *p;
    rs_pack *q = NULL;
    rs_pack_npc npc;
    rs_pack_item item;
    const char *path = "/tmp/rsgp_test_layout.bin";
    uint8_t *buf = NULL;
    FILE *f;
    long sz;
    uint8_t bad[16];
    size_t nread;

    p = rs_pack_new(7);
    expect(p != NULL, "layout new");
    if (!p) {
        return;
    }
    expect_err(rs_pack_add_string(p, 5, (const uint8_t *)"Ab", 2), RS_OK, "layout str");
    memset(&npc, 0, sizeof(npc));
    npc.id = 9;
    npc.combat = 3;
    npc.wander = -2;
    expect_err(rs_pack_add_npc(p, &npc), RS_OK, "layout npc");
    memset(&item, 0, sizeof(item));
    item.id = 4;
    item.equip_slot = 255;
    item.bonus[0] = -1;
    item.shop_price = -5;
    expect_err(rs_pack_add_item(p, &item), RS_OK, "layout item");
    expect_err(rs_pack_write(p, path), RS_OK, "layout write");
    f = fopen(path, "rb");
    expect(f != NULL, "layout open");
    if (!f) {
        rs_pack_free(p);
        return;
    }
    expect(fseek(f, 0, SEEK_END) == 0, "layout seek");
    sz = ftell(f);
    expect(sz > 60, "layout size");
    rewind(f);
    buf = malloc((size_t)sz);
    expect(buf != NULL, "layout buf");
    if (!buf) {
        fclose(f);
        rs_pack_free(p);
        remove(path);
        return;
    }
    nread = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    expect(nread == (size_t)sz, "layout read");
    expect(buf[0] == 'R' && buf[1] == 'S' && buf[2] == 'G' && buf[3] == 'P', "magic");
    expect(rdu16(buf + 4) == 1, "version byte");
    expect(rdu16(buf + 6) == 7, "revision byte");
    expect(rdu32(buf + 8) == 1, "string count byte");
    expect(rdu32(buf + 12) == 5 && rdu16(buf + 16) == 2 && buf[18] == 'A' && buf[19] == 'b',
           "string record");
    expect(rdu32(buf + 20) == 0, "world count 0");
    expect(rdu32(buf + 24) == 1 && rdu16(buf + 28) == 9 && rdu16(buf + 30) == 3, "npc record");
    expect(buf[28 + 22] == 0xFE, "wander byte");
    expect(rdu32(buf + 51) == 1 && rdu16(buf + 55) == 4, "item count");
    expect(buf[55 + 4] == 0xFF && buf[55 + 5] == 0xFF, "bonus le");

    expect_err(rs_pack_load_mem(buf, (size_t)sz / 2, &q), RS_ERR_FORMAT, "truncated mem");
    expect(q == NULL, "truncated out");
    f = fopen(path, "wb");
    expect(f != NULL, "trunc open");
    if (f) {
        fwrite(buf, 1, (size_t)sz / 2, f);
        fclose(f);
        expect_err(rs_pack_load(path, &q), RS_ERR_FORMAT, "truncated file");
        expect(q == NULL, "truncated file out");
    }

    memset(bad, 0, sizeof(bad));
    bad[0] = 'X';
    bad[1] = 'S';
    bad[2] = 'G';
    bad[3] = 'P';
    bad[4] = 1;
    expect_err(rs_pack_load_mem(bad, sizeof(bad), &q), RS_ERR_FORMAT, "bad magic");

    memcpy(bad, buf, 8);
    bad[4] = 2;
    bad[5] = 0;
    expect_err(rs_pack_load_mem(bad, 8, &q), RS_ERR_VERSION, "version 2");

    memcpy(bad, buf, 12);
    bad[4] = 1;
    bad[5] = 0;
    bad[8] = 0xff;
    bad[9] = 0xff;
    bad[10] = 0xff;
    bad[11] = 0xff;
    expect_err(rs_pack_load_mem(bad, 12, &q), RS_ERR_RANGE, "huge count");

    bad[8] = 1;
    bad[9] = 0;
    bad[10] = 0;
    bad[11] = 0;
    expect_err(rs_pack_load_mem(bad, 12, &q), RS_ERR_FORMAT, "count past end");

    free(buf);
    rs_pack_free(p);
    remove(path);
}

static void test_validate(void) {
    rs_pack *p;
    rs_pack_quest quest;
    rs_pack_dialogue dlg;
    p = rs_pack_new(1);
    expect(p != NULL, "val new");
    if (!p) {
        return;
    }
    expect_err(rs_pack_add_string(p, 1, (const uint8_t *)"Name", 4), RS_OK, "val name");
    memset(&quest, 0, sizeof(quest));
    quest.id = 1;
    quest.name_string_id = 1;
    quest.varp_id = RS_PACK_VARP_UNREAD;
    quest.varbit_id = RS_PACK_VARP_UNREAD;
    quest.nstages = 1;
    quest.stages[0].journal_id = 2;
    expect_err(rs_pack_add_quest(p, &quest), RS_OK, "val quest");
    expect_err(rs_pack_validate(p), RS_ERR_FORMAT, "missing journal");
    expect_err(rs_pack_add_string(p, 2, (const uint8_t *)"Notes", 5), RS_OK, "val journal");
    expect_err(rs_pack_validate(p), RS_OK, "journal ok");
    rs_pack_free(p);

    p = rs_pack_new(1);
    expect(p != NULL, "pct new");
    if (!p) {
        return;
    }
    expect_err(rs_pack_add_string(p, 1, (const uint8_t *)"cost %3", 7), RS_OK, "add %3");
    expect_err(rs_pack_validate(p), RS_ERR_FORMAT, "%3 rejected");
    rs_pack_free(p);

    p = rs_pack_new(1);
    expect(p != NULL, "pct1 new");
    if (!p) {
        return;
    }
    expect_err(rs_pack_add_string(p, 1, (const uint8_t *)"Hello %1", 8), RS_OK, "add %1");
    expect_err(rs_pack_validate(p), RS_OK, "%1 accepted");
    memset(&dlg, 0, sizeof(dlg));
    dlg.id = 1;
    dlg.nreplies = 1;
    dlg.replies[0].nact = 1;
    dlg.replies[0].act[0].id = 18;
    expect_err(rs_pack_add_dialogue(p, &dlg), RS_ERR_RANGE, "action 18");
    rs_pack_free(p);
}

static int step_ok(int x0, int z0, int x1, int z1) {
    return x1 >= x0 - 1 && x1 <= x0 + 1 && z1 >= z0 - 1 && z1 <= z0 + 1 && !(x1 == x0 && z1 == z0);
}

static void test_path(void) {
    int flags[25];
    int xs[16];
    int zs[16];
    int n;
    int i;
    int x;
    int z;
    int blocked[5][5];

    memset(flags, 0, sizeof(flags));
    memset(xs, 0x7e, sizeof(xs));
    memset(zs, 0x7e, sizeof(zs));
    n = rs_path_find(flags, 5, 5, 5, 0, 0, 4, 4, xs, zs, 16);
    expect(n == 4, "open 5x5 length");
    expect(n == 4 && xs[3] == 4 && zs[3] == 4, "open 5x5 corner");
    if (n == 4) {
        x = 0;
        z = 0;
        for (i = 0; i < n; i++) {
            expect(step_ok(x, z, xs[i], zs[i]), "open step");
            x = xs[i];
            z = zs[i];
        }
    }
    n = rs_path_find(flags, 5, 5, 5, 2, 2, 2, 2, xs, zs, 0);
    expect(n == 0, "start is goal");
    n = rs_path_find(NULL, 5, 5, 5, 0, 0, 1, 1, xs, zs, 4);
    expect(n == -1, "null flags");
    n = rs_path_find(flags, 5, 0, 5, 0, 0, 0, 0, xs, zs, 4);
    expect(n == -1, "non positive");
    n = rs_path_find(flags, 5, 5, 5, 0, 0, 4, 4, xs, zs, 3);
    expect(n == -1, "cap too small");

    expect(rs_path_step_blocked(0, 0, 1, 0) == 0, "east open");
    expect(rs_path_step_blocked(8, 0, 1, 0) == 1, "east wall source");
    expect(rs_path_step_blocked(0, 128, 1, 0) == 1, "east wall dest");
    expect(rs_path_step_blocked(0, 512, 1, 0) == 0, "projectile ignored");
    {
        int row[2];
        row[0] = 8;
        row[1] = 0;
        n = rs_path_find(row, 2, 2, 1, 0, 0, 1, 0, xs, zs, 4);
        expect(n == -1, "wall blocks east");
        row[0] = 0;
        n = rs_path_find(row, 2, 2, 1, 0, 0, 1, 0, xs, zs, 4);
        expect(n == 1 && xs[0] == 1 && zs[0] == 0, "east clear");
    }

    expect(rs_path_step_blocked(0, 0x200000, 1, 0) == 1, "full block step");
    expect(rs_path_step_blocked(0, 0x100, 0, 1) == 1, "loc block step");
    memset(flags, 0, sizeof(flags));
    flags[2 * 5 + 2] = 0x200000;
    n = rs_path_find(flags, 5, 5, 5, 0, 0, 4, 4, xs, zs, 16);
    expect(n > 0 && xs[n - 1] == 4 && zs[n - 1] == 4, "around full block");
    if (n > 0) {
        for (i = 0; i < n; i++) {
            expect(!(xs[i] == 2 && zs[i] == 2), "did not enter block");
        }
    }
    n = rs_path_find(flags, 5, 5, 5, 0, 0, 2, 2, xs, zs, 16);
    expect(n == -1, "cannot enter occupancy");

    memset(blocked, 0, sizeof(blocked));
    blocked[0][1] = 0x200000;
    blocked[1][0] = 0x200000;
    n = rs_path_find(&blocked[0][0], 5, 2, 2, 0, 0, 1, 1, xs, zs, 4);
    expect(n == -1, "no corner cut");
    memset(blocked, 0, sizeof(blocked));
    n = rs_path_find(&blocked[0][0], 5, 2, 2, 0, 0, 1, 1, xs, zs, 4);
    expect(n == 1 && xs[0] == 1 && zs[0] == 1, "diagonal open");
    n = rs_path_find(flags, 5, 5, 5, -1, 0, 0, 0, xs, zs, 4);
    expect(n == -1, "out of bounds");
}

int main(void) {
    test_sha();
    test_pbkdf2();
    test_xp();
    test_names();
    test_roundtrip();
    test_layout_and_reject();
    test_validate();
    test_path();
    if (g_fail) {
        fprintf(stderr, "failed\n");
        return 1;
    }
    printf("ok\n");
    return 0;
}
