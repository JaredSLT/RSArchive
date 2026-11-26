#include "rslib/both/rsc_game.h"

#include <stdio.h>
#include <string.h>

static int g_failed;

static void expect(int cond, const char *msg) {
    if (!cond) {
        fprintf(stderr, "fail: %s\n", msg);
        g_failed = 1;
    }
}

static void expect_err(rs_err e, rs_err want, const char *msg) {
    if (e != want) {
        fprintf(stderr, "fail: %s (got %d want %d)\n", msg, (int)e, (int)want);
        g_failed = 1;
    }
}

static void test_message(void) {
    static const char text[] = "Welcome to RuneScape";
    rs_buf raw;
    rs_buf view;
    char dst[64];

    expect_err(rs_buf_alloc(&raw, 64), RS_OK, "message alloc");
    expect_err(rs_rsc_message_encode(0, &raw, text), RS_ERR_UNSUPPORTED, "message encode 0");
    expect_err(rs_rsc_message_decode(0, &raw, dst, sizeof(dst)), RS_ERR_UNSUPPORTED, "message decode 0");
    expect_err(rs_rsc_message_encode(177, &raw, text), RS_OK, "message encode 177");
    expect(raw.pos == sizeof(text) - 1, "message length");
    expect(raw.pos > 0 && raw.data[raw.pos - 1] != 10, "message has no newline");
    expect(memcmp(raw.data, text, sizeof(text) - 1) == 0, "message bytes");

    rs_buf_wrap(&view, raw.data, raw.pos);
    memset(dst, 0x7f, sizeof(dst));
    expect_err(rs_rsc_message_decode(177, &view, dst, sizeof(dst)), RS_OK, "message decode 177");
    expect(strcmp(dst, text) == 0, "message text");
    rs_buf_free(&raw);
}

static void test_position(void) {
    static rs_rsc_position in;
    static rs_rsc_position out;
    rs_buf raw;
    rs_buf view;

    memset(&in, 0, sizeof(in));
    in.region_x = 400;
    in.region_y = 500;
    in.dir = 2;

    expect_err(rs_buf_alloc(&raw, 32), RS_OK, "position alloc");
    expect_err(rs_rsc_position_encode(0, &raw, &in), RS_ERR_UNSUPPORTED, "position encode 0");
    expect_err(rs_rsc_position_decode(0, &raw, &out), RS_ERR_UNSUPPORTED, "position decode 0");
    expect_err(rs_rsc_position_encode(177, &raw, &in), RS_OK, "position encode 177");
    expect(raw.pos > 0, "position wrote bytes");

    rs_buf_wrap(&view, raw.data, raw.pos);
    expect_err(rs_rsc_position_decode(177, &view, &out), RS_OK, "position decode 177");
    expect(out.region_x == 400 && out.region_y == 500 && out.dir == 2, "position fields");
    expect(out.known_count == 0 && out.added_count == 0, "position lists empty");
    rs_buf_free(&raw);
}

static rs_buf view_of(rs_buf *raw) {
    rs_buf view;
    rs_buf_wrap(&view, raw->data, raw->pos);
    return view;
}

typedef rs_err (*empty_fn)(int, rs_buf *);

static void round_empty(empty_fn enc, empty_fn dec, int revision, const char *name) {
    rs_buf raw;
    rs_buf view;
    expect_err(rs_buf_alloc(&raw, 4), RS_OK, name);
    expect_err(enc(0, &raw), RS_ERR_UNSUPPORTED, name);
    expect_err(dec(0, &raw), RS_ERR_UNSUPPORTED, name);
    expect_err(enc(revision, &raw), RS_OK, name);
    expect(raw.pos == 0, name);
    view = view_of(&raw);
    expect_err(dec(revision, &view), RS_OK, name);
    rs_buf_free(&raw);
}

static void test_empty_packets(void) {
    rs_buf raw;
    round_empty(rs_rsc_close_socket_encode, rs_rsc_close_socket_decode, 115, "close socket");
    round_empty(rs_rsc_logout_encode, rs_rsc_logout_decode, 177, "logout");
    round_empty(rs_rsc_sleep_encode, rs_rsc_sleep_decode, 159, "sleep");
    round_empty(rs_rsc_trade_close_encode, rs_rsc_trade_close_decode, 177, "trade close");
    round_empty(rs_rsc_shop_close_encode, rs_rsc_shop_close_decode, 177, "shop close");
    round_empty(rs_rsc_bank_close_encode, rs_rsc_bank_close_decode, 177, "bank close");
    round_empty(rs_rsc_duel_close_encode, rs_rsc_duel_close_decode, 177, "duel close");
    round_empty(rs_rsc_timer_encode, rs_rsc_timer_decode, 177, "timer");
    round_empty(rs_rsc_strings_close_encode, rs_rsc_strings_close_decode, 177, "strings close");
    round_empty(rs_rsc_recovery_encode, rs_rsc_recovery_decode, 115, "recovery");
    round_empty(rs_rsc_sleep_wrong_encode, rs_rsc_sleep_wrong_decode, 177, "sleep wrong");
    round_empty(rs_rsc_ui_close_encode, rs_rsc_ui_close_decode, 177, "ui close");
    round_empty(rs_rsc_contact_encode, rs_rsc_contact_decode, 159, "contact");
    expect_err(rs_buf_alloc(&raw, 4), RS_OK, "empty limits");
    expect_err(rs_rsc_sleep_wrong_encode(115, &raw), RS_ERR_UNSUPPORTED, "sleep wrong 115");
    expect_err(rs_rsc_sleep_wrong_encode(159, &raw), RS_ERR_UNSUPPORTED, "sleep wrong 159");
    expect_err(rs_rsc_ui_close_decode(159, &raw), RS_ERR_UNSUPPORTED, "ui close 159");
    expect_err(rs_rsc_contact_encode(115, &raw), RS_ERR_UNSUPPORTED, "contact 115");
    rs_buf_free(&raw);
}

static void round_text(rs_err (*enc)(int, rs_buf *, const char *),
                       rs_err (*dec)(int, rs_buf *, char *, size_t), int revision, const char *text,
                       const char *name) {
    rs_buf raw;
    rs_buf view;
    char dst[64];
    expect_err(rs_buf_alloc(&raw, 64), RS_OK, name);
    expect_err(enc(revision, &raw, text), RS_OK, name);
    expect(raw.pos == strlen(text) && raw.data[raw.pos - 1] != 10, name);
    view = view_of(&raw);
    expect_err(dec(revision, &view, dst, sizeof(dst)), RS_OK, name);
    expect(strcmp(dst, text) == 0, name);
    rs_buf_free(&raw);
}

static void test_text_packets(void) {
    rs_buf raw;
    char dst[8];
    round_text(rs_rsc_sound_encode, rs_rsc_sound_decode, 177, "combat1", "sound");
    round_text(rs_rsc_sound_encode, rs_rsc_sound_decode, 159, "advance", "sound 159");
    round_text(rs_rsc_ui_string_encode, rs_rsc_ui_string_decode, 159, "Wait here", "ui string");
    expect_err(rs_buf_alloc(&raw, 8), RS_OK, "text limits");
    expect_err(rs_rsc_sound_encode(115, &raw, "a"), RS_ERR_UNSUPPORTED, "sound 115");
    expect_err(rs_rsc_ui_string_decode(115, &raw, dst, sizeof(dst)), RS_ERR_UNSUPPORTED, "ui string 115");
    expect_err(rs_rsc_sound_encode(0, &raw, "a"), RS_ERR_UNSUPPORTED, "sound 0");
    rs_buf_free(&raw);
}

static void test_shorts_and_effect(void) {
    rs_buf raw;
    rs_buf view;
    int value = 0;
    int player = 0;
    int accepted = 0;
    rs_rsc_effect effect;
    rs_rsc_effect out;
    rs_rsc_login_box box;
    rs_rsc_login_box box_out;
    rs_rsc_duel_options opt;
    rs_rsc_duel_options opt_out;

    expect_err(rs_buf_alloc(&raw, 32), RS_OK, "short alloc");
    expect_err(rs_rsc_ui_value_encode(177, &raw, 448), RS_OK, "ui value");
    expect(raw.pos == 2, "ui value length");
    view = view_of(&raw);
    expect_err(rs_rsc_ui_value_decode(177, &view, &value), RS_OK, "ui value dec");
    expect(value == 448, "ui value field");
    rs_buf_reset(&raw);
    expect_err(rs_rsc_ui_value_encode(159, &raw, 1), RS_ERR_UNSUPPORTED, "ui value 159");
    expect_err(rs_rsc_ui_value2_encode(159, &raw, 12), RS_OK, "ui value2");
    view = view_of(&raw);
    expect_err(rs_rsc_ui_value2_decode(177, &view, &value), RS_OK, "ui value2 dec");
    expect(value == 12, "ui value2 field");
    rs_buf_reset(&raw);
    expect_err(rs_rsc_ui_value2_encode(115, &raw, 1), RS_ERR_UNSUPPORTED, "ui value2 115");

    expect_err(rs_rsc_trade_open_encode(115, &raw, 40), RS_OK, "trade open");
    view = view_of(&raw);
    expect_err(rs_rsc_trade_open_decode(177, &view, &player), RS_OK, "trade open dec");
    expect(player == 40, "trade player");
    rs_buf_reset(&raw);
    expect_err(rs_rsc_duel_open_encode(177, &raw, 7), RS_OK, "duel open");
    view = view_of(&raw);
    expect_err(rs_rsc_duel_open_decode(159, &view, &player), RS_OK, "duel open dec");
    expect(player == 7, "duel player");

    rs_buf_reset(&raw);
    expect_err(rs_rsc_trade_other_encode(177, &raw, 1), RS_OK, "trade other");
    view = view_of(&raw);
    expect_err(rs_rsc_trade_other_decode(115, &view, &accepted), RS_OK, "trade other dec");
    expect(accepted == 1 && raw.pos == 1, "trade other bit");
    rs_buf_reset(&raw);
    expect_err(rs_rsc_trade_accept_encode(177, &raw, 0), RS_OK, "trade accept");
    view = view_of(&raw);
    expect_err(rs_rsc_trade_accept_decode(177, &view, &accepted), RS_OK, "trade accept dec");
    expect(accepted == 0, "trade accept bit");
    rs_buf_reset(&raw);
    expect_err(rs_rsc_duel_other_encode(159, &raw, 1), RS_OK, "duel other");
    view = view_of(&raw);
    expect_err(rs_rsc_duel_other_decode(177, &view, &accepted), RS_OK, "duel other dec");
    expect(accepted == 1, "duel other bit");
    rs_buf_reset(&raw);
    expect_err(rs_rsc_duel_accept_encode(115, &raw, 1), RS_OK, "duel accept");
    view = view_of(&raw);
    expect_err(rs_rsc_duel_accept_decode(115, &view, &accepted), RS_OK, "duel accept dec");
    expect(accepted == 1, "duel accept bit");

    memset(&effect, 0, sizeof(effect));
    effect.id = 3;
    effect.x = -1;
    effect.y = 4;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_effect_encode(115, &raw, &effect), RS_ERR_UNSUPPORTED, "effect 115");
    expect_err(rs_rsc_effect_encode(159, &raw, &effect), RS_OK, "effect");
    expect(raw.pos == 3 && raw.data[1] == 255 && raw.data[2] == 4, "effect bytes");
    view = view_of(&raw);
    expect_err(rs_rsc_effect_decode(177, &view, &out), RS_OK, "effect dec");
    expect(out.id == 3 && out.x == -1 && out.y == 4, "effect fields");

    memset(&box, 0, sizeof(box));
    box.days_ago = 2;
    box.recovery_days = 14;
    box.host = 0x7f000001u;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_login_box_encode(115, &raw, &box), RS_ERR_UNSUPPORTED, "login 115");
    expect_err(rs_rsc_login_box_encode(177, &raw, &box), RS_OK, "login");
    expect(raw.pos == 12, "login length");
    view = view_of(&raw);
    expect_err(rs_rsc_login_box_decode(159, &view, &box_out), RS_OK, "login dec");
    expect(box_out.days_ago == 2 && box_out.recovery_days == 14 && box_out.host == 0x7f000001u, "login fields");

    memset(&opt, 0, sizeof(opt));
    opt.retreat = 1;
    opt.weapons = 1;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_duel_options_encode(115, &raw, &opt), RS_OK, "duel options");
    expect(raw.pos == 4, "duel options length");
    view = view_of(&raw);
    expect_err(rs_rsc_duel_options_decode(177, &view, &opt_out), RS_OK, "duel options dec");
    expect(opt_out.retreat == 1 && opt_out.magic == 0 && opt_out.prayer == 0 && opt_out.weapons == 1,
           "duel options fields");
    rs_buf_free(&raw);
}

static void test_social(void) {
    static rs_rsc_friends friends;
    static rs_rsc_friends friends_out;
    rs_rsc_friend one;
    rs_rsc_friend one_out;
    rs_rsc_ignores ignores;
    rs_rsc_ignores ignores_out;
    rs_rsc_privacy privacy;
    rs_rsc_privacy privacy_out;
    rs_rsc_private_message pm;
    rs_rsc_private_message pm_out;
    uint8_t text[4] = {1, 2, 3, 4};
    uint8_t text_out[4];
    rs_buf raw;
    rs_buf view;

    memset(&friends, 0, sizeof(friends));
    friends.count = 2;
    friends.friend[0].name37 = 0x1234;
    friends.friend[0].world = 2;
    friends.friend[1].name37 = 99;
    friends.friend[1].world = 0;
    expect_err(rs_buf_alloc(&raw, 64), RS_OK, "social alloc");
    expect_err(rs_rsc_friends_encode(0, &raw, &friends), RS_ERR_UNSUPPORTED, "friends 0");
    expect_err(rs_rsc_friends_encode(177, &raw, &friends), RS_OK, "friends");
    expect(raw.pos == 1 + 18, "friends length");
    view = view_of(&raw);
    expect_err(rs_rsc_friends_decode(115, &view, &friends_out), RS_OK, "friends dec");
    expect(friends_out.count == 2 && friends_out.friend[0].name37 == 0x1234 && friends_out.friend[0].world == 2 &&
               friends_out.friend[1].world == 0,
           "friends fields");

    memset(&one, 0, sizeof(one));
    one.name37 = 55;
    one.world = 16;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_friend_update_encode(159, &raw, &one), RS_OK, "friend update");
    expect(raw.pos == 9, "friend update length");
    view = view_of(&raw);
    expect_err(rs_rsc_friend_update_decode(177, &view, &one_out), RS_OK, "friend update dec");
    expect(one_out.name37 == 55 && one_out.world == 16, "friend update fields");

    memset(&ignores, 0, sizeof(ignores));
    ignores.count = 1;
    ignores.name37[0] = 77;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_ignores_encode(115, &raw, &ignores), RS_OK, "ignores");
    view = view_of(&raw);
    expect_err(rs_rsc_ignores_decode(177, &view, &ignores_out), RS_OK, "ignores dec");
    expect(ignores_out.count == 1 && ignores_out.name37[0] == 77, "ignores fields");

    memset(&privacy, 0, sizeof(privacy));
    privacy.chat = 0;
    privacy.priv = 1;
    privacy.trade = 2;
    privacy.duel = 0;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_privacy_encode(177, &raw, &privacy), RS_OK, "privacy");
    expect(raw.pos == 4, "privacy length");
    view = view_of(&raw);
    expect_err(rs_rsc_privacy_decode(115, &view, &privacy_out), RS_OK, "privacy dec");
    expect(privacy_out.chat == 0 && privacy_out.priv == 1 && privacy_out.trade == 2 && privacy_out.duel == 0,
           "privacy fields");

    memset(&pm, 0, sizeof(pm));
    pm.from37 = 8;
    pm.len = 4;
    pm.text = text;
    memset(&pm_out, 0, sizeof(pm_out));
    pm_out.cap = 4;
    pm_out.text = text_out;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_private_message_encode(159, &raw, &pm), RS_OK, "pm");
    expect(raw.pos == 12, "pm length");
    view = view_of(&raw);
    expect_err(rs_rsc_private_message_decode(177, &view, &pm_out), RS_OK, "pm dec");
    expect(pm_out.from37 == 8 && pm_out.len == 4 && memcmp(text_out, text, 4) == 0, "pm fields");
    rs_buf_free(&raw);
}

static void test_stats_family(void) {
    static rs_rsc_stats stats;
    static rs_rsc_stats stats_out;
    static rs_rsc_quests quests;
    static rs_rsc_quests quests_out;
    rs_rsc_stat stat;
    rs_rsc_stat stat_out;
    rs_rsc_stat_xp xp;
    rs_rsc_stat_xp xp_out;
    rs_rsc_bonuses bonuses;
    rs_rsc_bonuses bonuses_out;
    rs_rsc_prayers prayers;
    rs_rsc_prayers prayers_out;
    rs_rsc_settings settings;
    rs_rsc_settings settings_out;
    rs_buf raw;
    rs_buf view;

    memset(&stats, 0, sizeof(stats));
    stats.nskills = 18;
    stats.cur[3] = 10;
    stats.base[3] = 12;
    stats.xp[3] = 1154;
    stats.quest_points = 4;
    expect_err(rs_buf_alloc(&raw, 128), RS_OK, "stats alloc");
    expect_err(rs_rsc_stats_encode(0, &raw, &stats), RS_ERR_UNSUPPORTED, "stats 0");
    expect_err(rs_rsc_stats_encode(115, &raw, &stats), RS_ERR_RANGE, "stats 115 count");
    stats.nskills = 16;
    expect_err(rs_rsc_stats_encode(115, &raw, &stats), RS_OK, "stats 115");
    expect(raw.pos == 97, "stats 115 length");
    view = view_of(&raw);
    expect_err(rs_rsc_stats_decode(115, &view, &stats_out), RS_OK, "stats 115 dec");
    expect(stats_out.nskills == 16 && stats_out.cur[3] == 10 && stats_out.base[3] == 12 && stats_out.xp[3] == 1154 &&
               stats_out.quest_points == 4,
           "stats 115 fields");
    stats.nskills = 18;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_stats_encode(177, &raw, &stats), RS_OK, "stats 177");
    expect(raw.pos == 109, "stats 177 length");
    view = view_of(&raw);
    expect_err(rs_rsc_stats_decode(159, &view, &stats_out), RS_OK, "stats 177 dec");
    expect(stats_out.nskills == 18 && stats_out.xp[3] == 1154 && stats_out.quest_points == 4, "stats 177 fields");

    memset(&stat, 0, sizeof(stat));
    stat.index = 17;
    stat.cur = 1;
    stat.base = 1;
    stat.xp = 83;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_stat_encode(115, &raw, &stat), RS_ERR_RANGE, "stat 115 index");
    expect_err(rs_rsc_stat_encode(177, &raw, &stat), RS_OK, "stat");
    view = view_of(&raw);
    expect_err(rs_rsc_stat_decode(159, &view, &stat_out), RS_OK, "stat dec");
    expect(stat_out.index == 17 && stat_out.cur == 1 && stat_out.base == 1 && stat_out.xp == 83, "stat fields");

    memset(&xp, 0, sizeof(xp));
    xp.index = 0;
    xp.xp = 99;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_stat_xp_encode(115, &raw, &xp), RS_OK, "stat xp");
    expect(raw.pos == 5, "stat xp length");
    view = view_of(&raw);
    expect_err(rs_rsc_stat_xp_decode(177, &view, &xp_out), RS_OK, "stat xp dec");
    expect(xp_out.index == 0 && xp_out.xp == 99, "stat xp fields");

    memset(&bonuses, 0, sizeof(bonuses));
    bonuses.value[0] = 4;
    bonuses.value[4] = 2;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_bonuses_encode(159, &raw, &bonuses), RS_OK, "bonuses");
    expect(raw.pos == 5, "bonuses length");
    view = view_of(&raw);
    expect_err(rs_rsc_bonuses_decode(115, &view, &bonuses_out), RS_OK, "bonuses dec");
    expect(bonuses_out.value[0] == 4 && bonuses_out.value[4] == 2, "bonuses fields");

    memset(&prayers, 0, sizeof(prayers));
    prayers.count = 3;
    prayers.on[0] = 1;
    prayers.on[2] = 1;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_prayers_encode(177, &raw, &prayers), RS_OK, "prayers");
    expect(raw.pos == 3 && raw.data[0] == 1 && raw.data[1] == 0 && raw.data[2] == 1, "prayers bytes");
    view = view_of(&raw);
    expect_err(rs_rsc_prayers_decode(115, &view, &prayers_out), RS_OK, "prayers dec");
    expect(prayers_out.count == 3 && prayers_out.on[0] == 1 && prayers_out.on[1] == 0 && prayers_out.on[2] == 1,
           "prayers fields");

    memset(&quests, 0, sizeof(quests));
    quests.count = 17;
    quests.done[0] = 1;
    quests.done[16] = 1;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_quests_encode(115, &raw, &quests), RS_OK, "quests 115");
    expect(raw.pos == 17, "quests 115 length");
    view = view_of(&raw);
    expect_err(rs_rsc_quests_decode(115, &view, &quests_out), RS_OK, "quests 115 dec");
    expect(quests_out.count == 17 && quests_out.done[0] == 1 && quests_out.done[16] == 1, "quests 115 fields");
    quests.count = 50;
    quests.done[49] = 1;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_quests_encode(159, &raw, &quests), RS_ERR_RANGE, "quests 159 count");
    expect_err(rs_rsc_quests_encode(177, &raw, &quests), RS_OK, "quests 177");
    expect(raw.pos == 50, "quests 177 length");
    view = view_of(&raw);
    expect_err(rs_rsc_quests_decode(177, &view, &quests_out), RS_OK, "quests 177 dec");
    expect(quests_out.count == 50 && quests_out.done[49] == 1, "quests 177 fields");

    memset(&settings, 0, sizeof(settings));
    settings.camera = 1;
    settings.mouse = 0;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_settings_encode(115, &raw, &settings), RS_OK, "settings 115");
    expect(raw.pos == 2, "settings 115 length");
    view = view_of(&raw);
    expect_err(rs_rsc_settings_decode(115, &view, &settings_out), RS_OK, "settings 115 dec");
    expect(settings_out.camera == 1 && settings_out.mouse == 0 && settings_out.sound == 0, "settings 115 fields");
    settings.sound = 1;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_settings_encode(115, &raw, &settings), RS_ERR_RANGE, "settings 115 sound");
    expect_err(rs_rsc_settings_encode(177, &raw, &settings), RS_OK, "settings 177");
    expect(raw.pos == 3, "settings 177 length");
    view = view_of(&raw);
    expect_err(rs_rsc_settings_decode(159, &view, &settings_out), RS_OK, "settings 177 dec");
    expect(settings_out.camera == 1 && settings_out.sound == 1, "settings 177 fields");
    rs_buf_free(&raw);
}

static void test_npc_aux(void) {
    rs_rsc_npc_aux row[3];
    rs_rsc_npc_aux out_row[3];
    rs_rsc_npc_aux_list in;
    rs_rsc_npc_aux_list out;
    rs_buf raw;
    rs_buf view;

    memset(row, 0, sizeof(row));
    row[0].index = 1;
    row[0].type = 0;
    row[1].index = 2;
    row[1].type = RS_RSC_NPC_AUX_CHAT;
    row[1].player = 9;
    row[1].chat_len = 2;
    row[1].chat[0] = 0x0a;
    row[1].chat[1] = 0x0b;
    row[2].index = 3;
    row[2].type = RS_RSC_NPC_AUX_HITS;
    row[2].damage = 4;
    row[2].current = 5;
    row[2].max = 6;
    memset(&in, 0, sizeof(in));
    in.count = 3;
    in.row = row;
    memset(&out, 0, sizeof(out));
    out.cap = 3;
    out.row = out_row;
    expect_err(rs_buf_alloc(&raw, 64), RS_OK, "npc aux alloc");
    expect_err(rs_rsc_npc_aux_encode(0, &raw, &in), RS_ERR_UNSUPPORTED, "npc aux 0");
    expect_err(rs_rsc_npc_aux_encode(115, &raw, &in), RS_OK, "npc aux");
    view = view_of(&raw);
    expect_err(rs_rsc_npc_aux_decode(177, &view, &out), RS_OK, "npc aux dec");
    expect(out.count == 3 && out_row[0].type == 0 && out_row[1].player == 9 && out_row[1].chat_len == 2 &&
               out_row[1].chat[1] == 0x0b && out_row[2].damage == 4 && out_row[2].current == 5 && out_row[2].max == 6,
           "npc aux fields");
    rs_buf_free(&raw);
}

static void test_trade_duel_shop_bank(void) {
    static rs_rsc_shop shop;
    static rs_rsc_shop shop_out;
    static rs_rsc_bank bank;
    static rs_rsc_bank bank_out;
    rs_rsc_trade_offer trade;
    rs_rsc_trade_offer trade_out;
    rs_rsc_duel_offer duel;
    rs_rsc_duel_offer duel_out;
    rs_rsc_trade_confirm tconfirm;
    rs_rsc_trade_confirm tconfirm_out;
    rs_rsc_duel_confirm dconfirm;
    rs_rsc_duel_confirm dconfirm_out;
    rs_rsc_bank_slot slot;
    rs_rsc_bank_slot slot_out;
    rs_rsc_strings strings;
    rs_rsc_strings strings_out;
    rs_buf raw;
    rs_buf view;

    memset(&trade, 0, sizeof(trade));
    trade.count = 1;
    trade.item[0].id = 10;
    trade.item[0].amount = 1000;
    expect_err(rs_buf_alloc(&raw, 64), RS_OK, "offer alloc");
    expect_err(rs_rsc_trade_offer_encode(177, &raw, &trade), RS_OK, "trade offer");
    expect(raw.pos == 7, "trade offer length");
    view = view_of(&raw);
    expect_err(rs_rsc_trade_offer_decode(115, &view, &trade_out), RS_OK, "trade offer dec");
    expect(trade_out.count == 1 && trade_out.item[0].id == 10 && trade_out.item[0].amount == 1000, "trade offer fields");

    memset(&duel, 0, sizeof(duel));
    duel.count = 1;
    duel.item[0].id = 11;
    duel.item[0].amount = 2;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_duel_offer_encode(159, &raw, &duel), RS_OK, "duel offer");
    view = view_of(&raw);
    expect_err(rs_rsc_duel_offer_decode(177, &view, &duel_out), RS_OK, "duel offer dec");
    expect(duel_out.count == 1 && duel_out.item[0].id == 11 && duel_out.item[0].amount == 2, "duel offer fields");

    memset(&tconfirm, 0, sizeof(tconfirm));
    tconfirm.name37 = 42;
    tconfirm.receive_count = 1;
    tconfirm.receive[0].id = 5;
    tconfirm.receive[0].amount = 9;
    tconfirm.give_count = 1;
    tconfirm.give[0].id = 6;
    tconfirm.give[0].amount = 8;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_trade_confirm_encode(115, &raw, &tconfirm), RS_OK, "trade confirm");
    view = view_of(&raw);
    expect_err(rs_rsc_trade_confirm_decode(177, &view, &tconfirm_out), RS_OK, "trade confirm dec");
    expect(tconfirm_out.name37 == 42 && tconfirm_out.receive_count == 1 && tconfirm_out.receive[0].id == 5 &&
               tconfirm_out.receive[0].amount == 9 && tconfirm_out.give[0].id == 6 && tconfirm_out.give[0].amount == 8,
           "trade confirm fields");

    memset(&dconfirm, 0, sizeof(dconfirm));
    dconfirm.name37 = 43;
    dconfirm.opponent_count = 1;
    dconfirm.opponent[0].id = 1;
    dconfirm.opponent[0].amount = 2;
    dconfirm.self_count = 1;
    dconfirm.self[0].id = 3;
    dconfirm.self[0].amount = 4;
    dconfirm.retreat = 1;
    dconfirm.magic = 0;
    dconfirm.prayer = 1;
    dconfirm.weapons = 0;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_duel_confirm_encode(177, &raw, &dconfirm), RS_OK, "duel confirm");
    view = view_of(&raw);
    expect_err(rs_rsc_duel_confirm_decode(115, &view, &dconfirm_out), RS_OK, "duel confirm dec");
    expect(dconfirm_out.opponent[0].id == 1 && dconfirm_out.self[0].amount == 4 && dconfirm_out.retreat == 1 &&
               dconfirm_out.prayer == 1 && dconfirm_out.weapons == 0,
           "duel confirm fields");

    memset(&shop, 0, sizeof(shop));
    shop.count = 1;
    shop.general = 1;
    shop.sell_mod = 40;
    shop.buy_mod = 30;
    shop.item[0].id = 10;
    shop.item[0].stock = 12;
    shop.item[0].price_mod = -5;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_shop_encode(159, &raw, &shop), RS_OK, "shop");
    expect(raw.pos == 9 && raw.data[8] == 251, "shop bytes");
    view = view_of(&raw);
    expect_err(rs_rsc_shop_decode(115, &view, &shop_out), RS_OK, "shop dec");
    expect(shop_out.count == 1 && shop_out.general == 1 && shop_out.sell_mod == 40 && shop_out.buy_mod == 30 &&
               shop_out.item[0].stock == 12 && shop_out.item[0].price_mod == -5,
           "shop fields");

    memset(&bank, 0, sizeof(bank));
    bank.count = 1;
    bank.item[0].id = 2;
    bank.item[0].amount = 3;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_bank_encode(115, &raw, &bank), RS_OK, "bank 115");
    expect(raw.pos == 4, "bank 115 length");
    view = view_of(&raw);
    expect_err(rs_rsc_bank_decode(115, &view, &bank_out), RS_OK, "bank 115 dec");
    expect(bank_out.count == 1 && bank_out.max_items == 0 && bank_out.item[0].id == 2 && bank_out.item[0].amount == 3,
           "bank 115 fields");
    bank.max_items = 48;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_bank_encode(115, &raw, &bank), RS_ERR_RANGE, "bank 115 max");
    expect_err(rs_rsc_bank_encode(177, &raw, &bank), RS_OK, "bank 177");
    expect(raw.pos == 5 && raw.data[1] == 48, "bank 177 length");
    view = view_of(&raw);
    expect_err(rs_rsc_bank_decode(159, &view, &bank_out), RS_OK, "bank 177 dec");
    expect(bank_out.max_items == 48 && bank_out.item[0].amount == 3, "bank 177 fields");

    memset(&slot, 0, sizeof(slot));
    slot.slot = 4;
    slot.id = 20;
    slot.amount = 200;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_bank_slot_encode(177, &raw, &slot), RS_OK, "bank slot");
    expect(raw.pos == 7, "bank slot gsmart");
    view = view_of(&raw);
    expect_err(rs_rsc_bank_slot_decode(115, &view, &slot_out), RS_OK, "bank slot dec");
    expect(slot_out.slot == 4 && slot_out.id == 20 && slot_out.amount == 200, "bank slot fields");

    memset(&strings, 0, sizeof(strings));
    strings.count = 1;
    strings.row[0].len = 5;
    memcpy(strings.row[0].text, "Hello", 6);
    rs_buf_reset(&raw);
    expect_err(rs_rsc_strings_encode(177, &raw, &strings), RS_OK, "strings");
    view = view_of(&raw);
    expect_err(rs_rsc_strings_decode(115, &view, &strings_out), RS_OK, "strings dec");
    expect(strings_out.count == 1 && strings_out.row[0].len == 5 && strcmp(strings_out.row[0].text, "Hello") == 0,
           "strings fields");
    rs_buf_free(&raw);
}

static void test_sleep_image(void) {
    static rs_rsc_sleep_image in;
    static rs_rsc_sleep_image out;
    rs_buf raw;
    rs_buf view;
    int i;

    memset(&in, 0, sizeof(in));
    expect_err(rs_buf_alloc(&raw, 64), RS_OK, "sleep alloc");
    expect_err(rs_rsc_sleep_image_encode(115, &raw, &in), RS_ERR_UNSUPPORTED, "sleep image 115");
    expect_err(rs_rsc_sleep_image_encode(159, &raw, &in), RS_ERR_UNSUPPORTED, "sleep image 159");
    expect_err(rs_rsc_sleep_image_encode(177, &raw, &in), RS_OK, "sleep image");
    expect(raw.pos == 40 && raw.data[0] == 255 && raw.data[39] == 255, "sleep image black");
    view = view_of(&raw);
    expect_err(rs_rsc_sleep_image_decode(177, &view, &out), RS_OK, "sleep image dec");
    expect(out.pixel[0] == 0 && out.pixel[RS_RSC_SLEEP_W * 39] == 0, "sleep image pixels");

    memset(&in, 0, sizeof(in));
    for (i = 10; i < RS_RSC_SLEEP_W; i++) {
        in.pixel[i] = 1;
    }
    in.pixel[RS_RSC_SLEEP_W + 3] = 1;
    rs_buf_reset(&raw);
    expect_err(rs_rsc_sleep_image_encode(177, &raw, &in), RS_OK, "sleep image pattern");
    view = view_of(&raw);
    expect_err(rs_rsc_sleep_image_decode(177, &view, &out), RS_OK, "sleep image pattern dec");
    expect(memcmp(out.pixel, in.pixel, sizeof(in.pixel)) == 0, "sleep image round trip");
    rs_buf_free(&raw);
}

int main(void) {
    test_message();
    test_position();
    test_empty_packets();
    test_text_packets();
    test_shorts_and_effect();
    test_social();
    test_stats_family();
    test_npc_aux();
    test_trade_duel_shop_bank();
    test_sleep_image();
    return g_failed ? 1 : 0;
}
