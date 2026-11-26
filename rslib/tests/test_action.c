#include "rslib/both/action.h"

#include <stdio.h>
#include <string.h>

static int fails;

static void expect(const char *name, int cond) {
    if (!cond) {
        fprintf(stderr, "fail %s\n", name);
        fails++;
    }
}

static void expect_err(const char *name, rs_err got, rs_err want) {
    if (got != want) {
        fprintf(stderr, "fail %s err %d want %d\n", name, (int)got, (int)want);
        fails++;
    }
}

static int roundtrip(const char *name, rs_err e, const void *a, const void *b, size_t n, rs_buf *dst) {
    if (e != RS_OK) {
        fprintf(stderr, "fail %s encode %d\n", name, (int)e);
        fails++;
        return 0;
    }
    if (memcmp(a, b, n) != 0) {
        fprintf(stderr, "fail %s bytes\n", name);
        fails++;
        return 0;
    }
    if (rs_buf_remaining(dst) != 0) {
        fprintf(stderr, "fail %s leftover\n", name);
        fails++;
        return 0;
    }
    return 1;
}

static void bytes_eq(const char *name, const rs_buf *b, const uint8_t *want, size_t n) {
    if (b->pos != n || memcmp(b->data, want, n) != 0) {
        size_t i;
        fprintf(stderr, "fail %s wire", name);
        for (i = 0; i < b->pos; i++) {
            fprintf(stderr, " %02x", b->data[i]);
        }
        fprintf(stderr, "\n");
        fails++;
    }
}

static void test_317(void) {
    uint8_t storage[256];
    rs_buf dst, src;
    int which;

    rs_action_button button, button2;
    rs_action_npc npc, npc2;
    rs_action_attack_npc anpc, anpc2;
    rs_action_object obj, obj2;
    rs_action_ground ground, ground2;
    rs_action_item_on_item ioi, ioi2;
    rs_action_item_on_npc ion, ion2;
    rs_action_item_on_object ioo, ioo2;
    rs_action_item_on_player iop, iop2;
    rs_action_item_on_ground iog, iog2;
    rs_action_equip equip, equip2;
    rs_action_drop drop, drop2;
    rs_action_move_item move, move2;
    rs_action_magic_on_npc mon, mon2;
    rs_action_magic_on_player mop, mop2;
    rs_action_magic_on_item moi, moi2;
    rs_action_magic_on_ground mog, mog2;
    rs_action_trade trade, trade2;
    rs_action_follow follow, follow2;
    rs_action_attack_player atk, atk2;
    rs_action_dialogue dlg, dlg2;
    rs_action_enter_amount amt, amt2;
    rs_action_enter_name en, en2;
    rs_action_add_friend af, af2;
    rs_action_del_friend df, df2;
    rs_action_add_ignore ai, ai2;
    rs_action_del_ignore di, di2;
    rs_action_close close_v, close2;
    const uint64_t name37 = 0x0123456789abcdefULL;

    rs_buf_wrap(&dst, storage, sizeof(storage));

    memset(&button, 0, sizeof(button));
    button.interface_id = 20000;
    dst.pos = 0;
    expect_err("button", rs_action_button_encode(317, 1, &dst, &button), RS_OK);
    {
        const uint8_t want[] = {0x4e, 0x20};
        bytes_eq("button wire", &dst, want, sizeof(want));
    }
    rs_buf_wrap(&src, storage, dst.pos);
    memset(&button2, 0, sizeof(button2));
    expect_err("button dec", rs_action_button_decode(317, 1, &src, &button2), RS_OK);
    roundtrip("button", RS_OK, &button, &button2, sizeof(button), &src);
    expect_err("button2", rs_action_button_encode(317, 2, &dst, &button), RS_ERR_UNSUPPORTED);
    expect_err("button0", rs_action_button_encode(317, 0, &dst, &button), RS_ERR_ARG);

    for (which = 1; which <= 5; which++) {
        char name[32];
        memset(&npc, 0, sizeof(npc));
        npc.index = 20000;
        dst.pos = 0;
        snprintf(name, sizeof(name), "npc%d", which);
        expect_err(name, rs_action_npc_encode(317, which, &dst, &npc), RS_OK);
        if (which == 1) {
            const uint8_t want[] = {0x20, 0x4e};
            bytes_eq("npc1 wire", &dst, want, sizeof(want));
        }
        if (which == 2) {
            const uint8_t want[] = {0xa0, 0x4e};
            bytes_eq("npc2 wire", &dst, want, sizeof(want));
        }
        rs_buf_wrap(&src, storage, dst.pos);
        expect_err(name, rs_action_npc_decode(317, which, &src, &npc2), RS_OK);
        roundtrip(name, RS_OK, &npc, &npc2, sizeof(npc), &src);
    }

    memset(&anpc, 0, sizeof(anpc));
    anpc.index = 20000;
    dst.pos = 0;
    expect_err("attack npc", rs_action_attack_npc_encode(317, &dst, &anpc), RS_OK);
    {
        const uint8_t want[] = {0x4e, 0xa0};
        bytes_eq("attack npc wire", &dst, want, sizeof(want));
    }
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("attack npc dec", rs_action_attack_npc_decode(317, &src, &anpc2), RS_OK);
    roundtrip("attack npc", RS_OK, &anpc, &anpc2, sizeof(anpc), &src);

    for (which = 1; which <= 5; which++) {
        char name[32];
        memset(&obj, 0, sizeof(obj));
        obj.id = 400;
        obj.x = 1000;
        obj.z = 20000;
        dst.pos = 0;
        snprintf(name, sizeof(name), "obj%d", which);
        expect_err(name, rs_action_object_encode(317, which, &dst, &obj), RS_OK);
        if (which == 1) {
            const uint8_t want[] = {0x68, 0x03, 0x01, 0x90, 0x4e, 0xa0};
            bytes_eq("obj1 wire", &dst, want, sizeof(want));
        }
        rs_buf_wrap(&src, storage, dst.pos);
        expect_err(name, rs_action_object_decode(317, which, &src, &obj2), RS_OK);
        roundtrip(name, RS_OK, &obj, &obj2, sizeof(obj), &src);
    }

    memset(&ground, 0, sizeof(ground));
    ground.id = 400;
    ground.x = 1000;
    ground.z = 20000;
    dst.pos = 0;
    expect_err("ground", rs_action_ground_encode(317, &dst, &ground), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("ground dec", rs_action_ground_decode(317, &src, &ground2), RS_OK);
    roundtrip("ground", RS_OK, &ground, &ground2, sizeof(ground), &src);

    memset(&ioi, 0, sizeof(ioi));
    ioi.slot = 300;
    ioi.item = 400;
    ioi.interface_id = 1000;
    ioi.selected_slot = 20000;
    ioi.selected_item = 301;
    ioi.selected_interface = 1001;
    dst.pos = 0;
    expect_err("ioi", rs_action_item_on_item_encode(317, &dst, &ioi), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("ioi dec", rs_action_item_on_item_decode(317, &src, &ioi2), RS_OK);
    roundtrip("ioi", RS_OK, &ioi, &ioi2, sizeof(ioi), &src);

    memset(&ion, 0, sizeof(ion));
    ion.index = 20000;
    ion.item = 400;
    ion.interface_id = 1000;
    ion.slot = 300;
    dst.pos = 0;
    expect_err("ion", rs_action_item_on_npc_encode(317, &dst, &ion), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("ion dec", rs_action_item_on_npc_decode(317, &src, &ion2), RS_OK);
    roundtrip("ion", RS_OK, &ion, &ion2, sizeof(ion), &src);

    memset(&ioo, 0, sizeof(ioo));
    ioo.id = 400;
    ioo.x = 1000;
    ioo.z = 20000;
    ioo.item = 301;
    ioo.interface_id = 1001;
    ioo.slot = 300;
    dst.pos = 0;
    expect_err("ioo", rs_action_item_on_object_encode(317, &dst, &ioo), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("ioo dec", rs_action_item_on_object_decode(317, &src, &ioo2), RS_OK);
    roundtrip("ioo", RS_OK, &ioo, &ioo2, sizeof(ioo), &src);

    memset(&iop, 0, sizeof(iop));
    iop.index = 20000;
    iop.item = 400;
    iop.interface_id = 1000;
    iop.slot = 300;
    dst.pos = 0;
    expect_err("iop", rs_action_item_on_player_encode(317, &dst, &iop), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("iop dec", rs_action_item_on_player_decode(317, &src, &iop2), RS_OK);
    roundtrip("iop", RS_OK, &iop, &iop2, sizeof(iop), &src);

    memset(&iog, 0, sizeof(iog));
    iog.id = 400;
    iog.x = 1000;
    iog.z = 20000;
    iog.item = 301;
    iog.interface_id = 1001;
    iog.slot = 300;
    dst.pos = 0;
    expect_err("iog", rs_action_item_on_ground_encode(317, &dst, &iog), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("iog dec", rs_action_item_on_ground_decode(317, &src, &iog2), RS_OK);
    roundtrip("iog", RS_OK, &iog, &iog2, sizeof(iog), &src);

    memset(&equip, 0, sizeof(equip));
    equip.item = 400;
    equip.slot = 300;
    equip.interface_id = 20000;
    dst.pos = 0;
    expect_err("equip", rs_action_equip_encode(317, &dst, &equip), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("equip dec", rs_action_equip_decode(317, &src, &equip2), RS_OK);
    roundtrip("equip", RS_OK, &equip, &equip2, sizeof(equip), &src);

    memset(&drop, 0, sizeof(drop));
    drop.item = 400;
    drop.slot = 20000;
    drop.interface_id = 1000;
    dst.pos = 0;
    expect_err("drop", rs_action_drop_encode(317, &dst, &drop), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("drop dec", rs_action_drop_decode(317, &src, &drop2), RS_OK);
    roundtrip("drop", RS_OK, &drop, &drop2, sizeof(drop), &src);

    memset(&move, 0, sizeof(move));
    move.interface_id = 20000;
    move.from_slot = 300;
    move.to_slot = 1000;
    move.insert_mode = 1;
    dst.pos = 0;
    expect_err("move", rs_action_move_item_encode(317, &dst, &move), RS_OK);
    {
        /* p2_alt3(20000), p1_alt2(1), p2_alt3(300), p2_alt1(1000) */
        const uint8_t want[] = {0xa0, 0x4e, 0xff, 0xac, 0x01, 0xe8, 0x03};
        bytes_eq("move wire", &dst, want, sizeof(want));
    }
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("move dec", rs_action_move_item_decode(317, &src, &move2), RS_OK);
    roundtrip("move", RS_OK, &move, &move2, sizeof(move), &src);

    memset(&mon, 0, sizeof(mon));
    mon.index = 20000;
    mon.component = 1000;
    dst.pos = 0;
    expect_err("mon", rs_action_magic_on_npc_encode(317, &dst, &mon), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("mon dec", rs_action_magic_on_npc_decode(317, &src, &mon2), RS_OK);
    roundtrip("mon", RS_OK, &mon, &mon2, sizeof(mon), &src);

    memset(&mop, 0, sizeof(mop));
    mop.index = 20000;
    mop.component = 1000;
    dst.pos = 0;
    expect_err("mop", rs_action_magic_on_player_encode(317, &dst, &mop), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("mop dec", rs_action_magic_on_player_decode(317, &src, &mop2), RS_OK);
    roundtrip("mop", RS_OK, &mop, &mop2, sizeof(mop), &src);

    memset(&moi, 0, sizeof(moi));
    moi.slot = 300;
    moi.item = 400;
    moi.interface_id = 1000;
    moi.component = 20000;
    dst.pos = 0;
    expect_err("moi", rs_action_magic_on_item_encode(317, &dst, &moi), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("moi dec", rs_action_magic_on_item_decode(317, &src, &moi2), RS_OK);
    roundtrip("moi", RS_OK, &moi, &moi2, sizeof(moi), &src);

    memset(&mog, 0, sizeof(mog));
    mog.id = 400;
    mog.x = 1000;
    mog.z = 20000;
    mog.component = 301;
    dst.pos = 0;
    expect_err("mog", rs_action_magic_on_ground_encode(317, &dst, &mog), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("mog dec", rs_action_magic_on_ground_decode(317, &src, &mog2), RS_OK);
    roundtrip("mog", RS_OK, &mog, &mog2, sizeof(mog), &src);

    memset(&trade, 0, sizeof(trade));
    trade.index = 20000;
    dst.pos = 0;
    expect_err("trade", rs_action_trade_encode(317, &dst, &trade), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("trade dec", rs_action_trade_decode(317, &src, &trade2), RS_OK);
    roundtrip("trade", RS_OK, &trade, &trade2, sizeof(trade), &src);

    memset(&follow, 0, sizeof(follow));
    follow.index = 1000;
    dst.pos = 0;
    expect_err("follow", rs_action_follow_encode(317, &dst, &follow), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("follow dec", rs_action_follow_decode(317, &src, &follow2), RS_OK);
    roundtrip("follow", RS_OK, &follow, &follow2, sizeof(follow), &src);

    memset(&atk, 0, sizeof(atk));
    atk.index = 20000;
    dst.pos = 0;
    expect_err("atk", rs_action_attack_player_encode(317, &dst, &atk), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("atk dec", rs_action_attack_player_decode(317, &src, &atk2), RS_OK);
    roundtrip("atk", RS_OK, &atk, &atk2, sizeof(atk), &src);

    memset(&dlg, 0, sizeof(dlg));
    dlg.interface_id = 20000;
    dst.pos = 0;
    expect_err("dlg", rs_action_dialogue_encode(317, &dst, &dlg), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("dlg dec", rs_action_dialogue_decode(317, &src, &dlg2), RS_OK);
    roundtrip("dlg", RS_OK, &dlg, &dlg2, sizeof(dlg), &src);

    memset(&amt, 0, sizeof(amt));
    amt.amount = 2000000001;
    dst.pos = 0;
    expect_err("amt", rs_action_enter_amount_encode(317, &dst, &amt), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("amt dec", rs_action_enter_amount_decode(317, &src, &amt2), RS_OK);
    roundtrip("amt", RS_OK, &amt, &amt2, sizeof(amt), &src);

    memset(&en, 0, sizeof(en));
    en.name37 = name37;
    dst.pos = 0;
    expect_err("name", rs_action_enter_name_encode(317, &dst, &en), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("name dec", rs_action_enter_name_decode(317, &src, &en2), RS_OK);
    roundtrip("name", RS_OK, &en, &en2, sizeof(en), &src);

    memset(&af, 0, sizeof(af));
    af.name37 = name37;
    dst.pos = 0;
    expect_err("addf", rs_action_add_friend_encode(317, &dst, &af), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("addf dec", rs_action_add_friend_decode(317, &src, &af2), RS_OK);
    roundtrip("addf", RS_OK, &af, &af2, sizeof(af), &src);

    memset(&df, 0, sizeof(df));
    df.name37 = name37;
    dst.pos = 0;
    expect_err("delf", rs_action_del_friend_encode(317, &dst, &df), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("delf dec", rs_action_del_friend_decode(317, &src, &df2), RS_OK);
    roundtrip("delf", RS_OK, &df, &df2, sizeof(df), &src);

    memset(&ai, 0, sizeof(ai));
    ai.name37 = name37;
    dst.pos = 0;
    expect_err("addi", rs_action_add_ignore_encode(317, &dst, &ai), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("addi dec", rs_action_add_ignore_decode(317, &src, &ai2), RS_OK);
    roundtrip("addi", RS_OK, &ai, &ai2, sizeof(ai), &src);

    memset(&di, 0, sizeof(di));
    di.name37 = name37;
    dst.pos = 0;
    expect_err("deli", rs_action_del_ignore_encode(317, &dst, &di), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("deli dec", rs_action_del_ignore_decode(317, &src, &di2), RS_OK);
    roundtrip("deli", RS_OK, &di, &di2, sizeof(di), &src);

    memset(&close_v, 0, sizeof(close_v));
    dst.pos = 0;
    expect_err("close", rs_action_close_encode(317, &dst, &close_v), RS_OK);
    expect("close empty", close_v.unused == 0 && dst.pos == 0);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("close dec", rs_action_close_decode(317, &src, &close2), RS_OK);
    roundtrip("close", RS_OK, &close_v, &close2, sizeof(close_v), &src);

    expect_err("317 script", rs_action_script_encode(317, &dst, &(rs_action_script){0}), RS_ERR_UNSUPPORTED);
    expect_err("317 varbit", rs_action_varbit_small_encode(317, &dst, &(rs_action_varbit){0}), RS_ERR_UNSUPPORTED);

    {
        rs_action_if_option opt, opt2;
        int w;
        memset(&opt, 0, sizeof opt);
        opt.interface_id = 1;
        opt.slot = 2;
        opt.item = 3;
        dst.pos = 0;
        expect_err("if1", rs_action_if_option_encode(317, 1, &dst, &opt), RS_OK);
        {
            const uint8_t want[] = {0x00, 0x81, 0x00, 0x82, 0x00, 0x83};
            bytes_eq("if1 wire", &dst, want, sizeof want);
        }
        for (w = 1; w <= 5; w++) {
            char name[32];
            memset(&opt, 0, sizeof opt);
            opt.interface_id = 5382;
            opt.slot = 4;
            opt.item = 995;
            dst.pos = 0;
            snprintf(name, sizeof name, "if%d", w);
            expect_err(name, rs_action_if_option_encode(317, w, &dst, &opt), RS_OK);
            expect("if len", dst.pos == 6);
            rs_buf_wrap(&src, storage, dst.pos);
            memset(&opt2, 0, sizeof opt2);
            expect_err(name, rs_action_if_option_decode(317, w, &src, &opt2), RS_OK);
            expect("if fields", opt2.interface_id == 5382 && opt2.slot == 4 && opt2.item == 995);
            expect("if done", rs_buf_remaining(&src) == 0);
        }
        expect_err("if0", rs_action_if_option_encode(317, 0, &dst, &opt), RS_ERR_ARG);
        expect_err("if254", rs_action_if_option_encode(254, 1, &dst, &opt), RS_ERR_UNSUPPORTED);
        memset(&opt, 0, sizeof opt);
        opt.interface_id = 3214;
        opt.slot = 2;
        opt.item = 995;
        dst.pos = 0;
        expect_err("opheld", rs_action_opheld_encode(317, &dst, &opt), RS_OK);
        expect("opheld len", dst.pos == 6);
        rs_buf_wrap(&src, storage, dst.pos);
        memset(&opt2, 0, sizeof opt2);
        expect_err("opheld dec", rs_action_opheld_decode(317, &src, &opt2), RS_OK);
        expect("opheld fields", opt2.interface_id == 3214 && opt2.slot == 2 && opt2.item == 995);
        expect_err("opheld254", rs_action_opheld_encode(254, &dst, &opt), RS_ERR_UNSUPPORTED);
    }
}

static void test_later(void) {
    uint8_t storage[512];
    rs_buf dst, src;
    rs_action_button button, button2;
    rs_action_move_item move, move2;
    rs_action_npc npc, npc2;
    rs_action_object obj, obj2;
    rs_action_ground ground, ground2;
    rs_action_script script, script2;
    rs_action_varbit vb, vb2;
    rs_action_enter_name en, en2;
    rs_action_add_ignore ai, ai2;
    rs_action_dialogue dlg, dlg2;
    rs_action_item_on_item ioi, ioi2;
    int i;

    rs_buf_wrap(&dst, storage, sizeof(storage));

    memset(&button, 0, sizeof(button));
    button.interface_id = 0x01020304;
    button.child = 400;
    button.item = 300;
    dst.pos = 0;
    expect_err("742 button", rs_action_button_encode(742, 3, &dst, &button), RS_OK);
    {
        const uint8_t want[] = {0xac, 0x01, 0x10, 0x01, 0x02, 0x01, 0x04, 0x03};
        bytes_eq("742 button wire", &dst, want, sizeof(want));
    }
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("742 button dec", rs_action_button_decode(742, 3, &src, &button2), RS_OK);
    roundtrip("742 button", RS_OK, &button, &button2, sizeof(button), &src);

    dst.pos = 0;
    expect_err("910 button", rs_action_button_encode(910, 5, &dst, &button), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 button dec", rs_action_button_decode(910, 5, &src, &button2), RS_OK);
    roundtrip("910 button", RS_OK, &button, &button2, sizeof(button), &src);

    memset(&move, 0, sizeof(move));
    move.interface_id = 1000;
    move.from_slot = 300;
    move.to_slot = 400;
    move.insert_mode = 1;
    dst.pos = 0;
    expect_err("254 move", rs_action_move_item_encode(254, &dst, &move), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("254 move dec", rs_action_move_item_decode(254, &src, &move2), RS_OK);
    roundtrip("254 move", RS_OK, &move, &move2, sizeof(move), &src);

    dst.pos = 0;
    expect_err("377 move", rs_action_move_item_encode(377, &dst, &move), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("377 move dec", rs_action_move_item_decode(377, &src, &move2), RS_OK);
    roundtrip("377 move", RS_OK, &move, &move2, sizeof(move), &src);

    memset(&npc, 0, sizeof(npc));
    npc.index = 20000;
    npc.ctrl = 1;
    dst.pos = 0;
    expect_err("742 npc", rs_action_npc_encode(742, 4, &dst, &npc), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("742 npc dec", rs_action_npc_decode(742, 4, &src, &npc2), RS_OK);
    roundtrip("742 npc", RS_OK, &npc, &npc2, sizeof(npc), &src);
    dst.pos = 0;
    expect_err("910 npc", rs_action_npc_encode(910, 1, &dst, &npc), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 npc dec", rs_action_npc_decode(910, 1, &src, &npc2), RS_OK);
    roundtrip("910 npc", RS_OK, &npc, &npc2, sizeof(npc), &src);
    expect_err("377 npc3", rs_action_npc_encode(377, 3, &dst, &npc), RS_ERR_UNSUPPORTED);
    expect_err("742 atknpc", rs_action_attack_npc_encode(742, &dst, &(rs_action_attack_npc){0}), RS_ERR_UNSUPPORTED);

    memset(&obj, 0, sizeof(obj));
    obj.id = 0x01020304;
    obj.x = 1000;
    obj.z = 20000;
    obj.ctrl = 1;
    dst.pos = 0;
    expect_err("742 obj", rs_action_object_encode(742, 2, &dst, &obj), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("742 obj dec", rs_action_object_decode(742, 2, &src, &obj2), RS_OK);
    roundtrip("742 obj", RS_OK, &obj, &obj2, sizeof(obj), &src);
    dst.pos = 0;
    expect_err("910 obj", rs_action_object_encode(910, 5, &dst, &obj), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 obj dec", rs_action_object_decode(910, 5, &src, &obj2), RS_OK);
    roundtrip("910 obj", RS_OK, &obj, &obj2, sizeof(obj), &src);

    memset(&ground, 0, sizeof(ground));
    ground.id = 400;
    ground.x = 1000;
    ground.z = 20000;
    ground.ctrl = 3;
    dst.pos = 0;
    expect_err("910 ground", rs_action_ground_encode(910, &dst, &ground), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 ground dec", rs_action_ground_decode(910, &src, &ground2), RS_OK);
    roundtrip("910 ground", RS_OK, &ground, &ground2, sizeof(ground), &src);

    memset(&script, 0, sizeof(script));
    script.id = 99;
    script.component = 1000;
    script.nargs = 2;
    memcpy(script.types, "is", 3);
    script.arg[0].i = 42;
    memcpy(script.arg[1].s, "hi", 3);
    dst.pos = 0;
    expect_err("530 script", rs_action_script_encode(530, &dst, &script), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("530 script dec", rs_action_script_decode(530, &src, &script2), RS_OK);
    expect("530 script fields", script2.id == 99 && script2.component == 1000 && script2.nargs == 2);
    expect("530 script args", strcmp(script2.types, "is") == 0 && script2.arg[0].i == 42 && strcmp(script2.arg[1].s, "hi") == 0);
    expect("530 script leftover", rs_buf_remaining(&src) == 0);

    script.component = 0;
    dst.pos = 0;
    expect_err("742 script", rs_action_script_encode(742, &dst, &script), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("742 script dec", rs_action_script_decode(742, &src, &script2), RS_OK);
    expect("742 script fields", script2.id == 99 && script2.component == 0 && strcmp(script2.arg[1].s, "hi") == 0);

    dst.pos = 0;
    expect_err("910 script", rs_action_script_encode(910, &dst, &script), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 script dec", rs_action_script_decode(910, &src, &script2), RS_OK);
    expect("910 script fields", script2.arg[0].i == 42 && script2.id == 99);

    memset(&vb, 0, sizeof(vb));
    vb.id = 300;
    vb.value = 40;
    dst.pos = 0;
    expect_err("910 vbs", rs_action_varbit_small_encode(910, &dst, &vb), RS_OK);
    {
        const uint8_t want[] = {0x58, 0x01, 0xac};
        bytes_eq("910 vbs wire", &dst, want, sizeof(want));
    }
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 vbs dec", rs_action_varbit_small_decode(910, &src, &vb2), RS_OK);
    roundtrip("910 vbs", RS_OK, &vb, &vb2, sizeof(vb), &src);

    dst.pos = 0;
    expect_err("742 vbs", rs_action_varbit_small_encode(742, &dst, &vb), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("742 vbs dec", rs_action_varbit_small_decode(742, &src, &vb2), RS_OK);
    roundtrip("742 vbs", RS_OK, &vb, &vb2, sizeof(vb), &src);

    vb.id = 1000;
    vb.value = 0x01020304;
    dst.pos = 0;
    expect_err("742 vbl", rs_action_varbit_large_encode(742, &dst, &vb), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("742 vbl dec", rs_action_varbit_large_decode(742, &src, &vb2), RS_OK);
    roundtrip("742 vbl", RS_OK, &vb, &vb2, sizeof(vb), &src);
    dst.pos = 0;
    expect_err("910 vbl", rs_action_varbit_large_encode(910, &dst, &vb), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 vbl dec", rs_action_varbit_large_decode(910, &src, &vb2), RS_OK);
    roundtrip("910 vbl", RS_OK, &vb, &vb2, sizeof(vb), &src);
    expect_err("530 vbl", rs_action_varbit_large_encode(530, &dst, &vb), RS_ERR_UNSUPPORTED);

    memset(&en, 0, sizeof(en));
    memcpy(en.name, "Zezima", 7);
    dst.pos = 0;
    expect_err("742 name", rs_action_enter_name_encode(742, &dst, &en), RS_OK);
    expect("742 name len", dst.pos == 7 && storage[6] == 0);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("742 name dec", rs_action_enter_name_decode(742, &src, &en2), RS_OK);
    expect("742 name text", strcmp(en2.name, "Zezima") == 0);

    memset(&ai, 0, sizeof(ai));
    memcpy(ai.name, "Mod", 4);
    ai.temporary = 1;
    dst.pos = 0;
    expect_err("910 ign", rs_action_add_ignore_encode(910, &dst, &ai), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 ign dec", rs_action_add_ignore_decode(910, &src, &ai2), RS_OK);
    expect("910 ign fields", strcmp(ai2.name, "Mod") == 0 && ai2.temporary == 1);

    memset(&dlg, 0, sizeof(dlg));
    dlg.interface_id = 0x01020304;
    dlg.child = 300;
    dst.pos = 0;
    expect_err("910 dlg", rs_action_dialogue_encode(910, &dst, &dlg), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 dlg dec", rs_action_dialogue_decode(910, &src, &dlg2), RS_OK);
    roundtrip("910 dlg", RS_OK, &dlg, &dlg2, sizeof(dlg), &src);
    expect_err("742 dlg", rs_action_dialogue_encode(742, &dst, &dlg), RS_ERR_UNSUPPORTED);

    memset(&ioi, 0, sizeof(ioi));
    ioi.slot = 300;
    ioi.item = 400;
    ioi.interface_id = 0x01020304;
    ioi.selected_slot = 1000;
    ioi.selected_item = 301;
    ioi.selected_interface = 0x05060708;
    dst.pos = 0;
    expect_err("910 ioi", rs_action_item_on_item_encode(910, &dst, &ioi), RS_OK);
    rs_buf_wrap(&src, storage, dst.pos);
    expect_err("910 ioi dec", rs_action_item_on_item_decode(910, &src, &ioi2), RS_OK);
    roundtrip("910 ioi", RS_OK, &ioi, &ioi2, sizeof(ioi), &src);

    expect_err("468 button", rs_action_button_encode(468, 1, &dst, &button), RS_ERR_UNSUPPORTED);
    expect_err("474 npc", rs_action_npc_encode(474, 1, &dst, &npc), RS_ERR_UNSUPPORTED);
    expect_err("508 obj", rs_action_object_encode(508, 1, &dst, &obj), RS_ERR_UNSUPPORTED);
    expect_err("530 button", rs_action_button_encode(530, 1, &dst, &button), RS_ERR_UNSUPPORTED);
    (void)i;
}

static void test_831(void) {
    uint8_t storage[32];
    rs_buf dst;
    rs_action_button button;
    rs_action_npc npc;
    rs_action_close close_v;
    rs_action_script script;
    rs_action_varbit vb;
    rs_action_enter_amount amt;

    rs_buf_wrap(&dst, storage, sizeof(storage));
    memset(&button, 0, sizeof(button));
    memset(&npc, 0, sizeof(npc));
    memset(&close_v, 0, sizeof(close_v));
    memset(&script, 0, sizeof(script));
    memset(&vb, 0, sizeof(vb));
    memset(&amt, 0, sizeof(amt));
    expect_err("831 button", rs_action_button_encode(831, 1, &dst, &button), RS_ERR_UNSUPPORTED);
    expect_err("831 button dec", rs_action_button_decode(831, 1, &dst, &button), RS_ERR_UNSUPPORTED);
    expect_err("831 npc", rs_action_npc_encode(831, 1, &dst, &npc), RS_ERR_UNSUPPORTED);
    expect_err("831 close", rs_action_close_encode(831, &dst, &close_v), RS_ERR_UNSUPPORTED);
    expect_err("831 script", rs_action_script_encode(831, &dst, &script), RS_ERR_UNSUPPORTED);
    expect_err("831 varbit", rs_action_varbit_small_encode(831, &dst, &vb), RS_ERR_UNSUPPORTED);
    expect_err("831 varbit l", rs_action_varbit_large_decode(831, &dst, &vb), RS_ERR_UNSUPPORTED);
    expect_err("831 amount", rs_action_enter_amount_encode(831, &dst, &amt), RS_ERR_UNSUPPORTED);
    expect_err("860 trade", rs_action_trade_encode(860, &dst, &(rs_action_trade){0}), RS_ERR_UNSUPPORTED);
    expect_err("null", rs_action_button_encode(317, 1, NULL, &button), RS_ERR_ARG);
}

int main(void) {
    test_317();
    test_later();
    test_831();
    if (fails) {
        fprintf(stderr, "%d failures\n", fails);
        return 1;
    }
    return 0;
}
