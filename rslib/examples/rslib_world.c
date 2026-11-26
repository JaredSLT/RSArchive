#include "rslib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <signal.h>
#endif

static rs_server *g_srv;

static void on_signal(int sig) {
    (void)sig;
    if (g_srv) {
        rs_server_stop(g_srv);
    }
}

static void send_welcome(rs_conn *c) {
    rs_region region;
    rs_player_index pi;
    rs_player_update pu;
    rs_message msg;
    rs_err e;

    memset(&region, 0, sizeof(region));
    region.zone_x = 402; /* Lumbridge 3222,3222 */
    region.zone_z = 402;
    region.local_x = 54;
    region.local_z = 54;
    region.cache = rs_server_cache(rs_conn_server(c), rs_conn_build(c));
    e = rs_conn_send_region(c, &region);
    if (e != RS_OK) {
        fprintf(stderr, "region: %s\n", rs_strerror(e));
    }

    memset(&pi, 0, sizeof(pi));
    pi.members = 1;
    pi.index = 1;
    (void)rs_conn_send_player_index(c, &pi);

    memset(&pu, 0, sizeof(pu));
    pu.teleport = 1;
    pu.discard_walk = 1;
    pu.local_x = 54;
    pu.local_z = 54;
    pu.mask = RS_PLAYER_APPEARANCE;
    pu.appearance.styles[1] = 10;
    pu.appearance.styles[2] = 18;
    pu.appearance.styles[3] = 26;
    pu.appearance.styles[4] = 33;
    pu.appearance.styles[5] = 36;
    pu.appearance.styles[6] = 42;
    e = rs_conn_send_player_update(c, &pu);
    if (e != RS_OK) {
        fprintf(stderr, "player update: %s\n", rs_strerror(e));
    }
    e = rs_conn_send_npc_empty(c);
    if (e != RS_OK) {
        fprintf(stderr, "npc update: %s\n", rs_strerror(e));
    }

    memset(&msg, 0, sizeof(msg));
    strncpy(msg.text, "Welcome to rslib-world.", sizeof(msg.text) - 1);
    e = rs_conn_send_message(c, &msg);
    if (e != RS_OK) {
        fprintf(stderr, "message: %s\n", rs_strerror(e));
    }
}

static void on_login(rs_conn *c, const rs_login_request *login) {
    printf("login %s from %s\n", login->username[0] ? login->username : "(empty)", rs_conn_addr(c));
    if (rs_conn_accept_login(c, 0, 0) != RS_OK) {
        fprintf(stderr, "accept login failed\n");
        rs_conn_close(c);
        return;
    }
    send_welcome(c);
}

static void echo_text(rs_conn *c, const char *text) {
    rs_message msg;
    const rs_login_request *login = rs_conn_login(c);
    const char *name = (login && login->username[0]) ? login->username : "player";

    memset(&msg, 0, sizeof(msg));
    snprintf(msg.text, sizeof(msg.text), "%s: %s", name, text && text[0] ? text : "...");
    (void)rs_conn_send_message(c, &msg);
}

static void on_packet(rs_conn *c, rs_pkt id, int opcode, rs_buf *payload) {
    rs_session *s = rs_conn_session(c);

    if (id == RS_C2S_WALK || id == RS_C2S_WALK_MINIMAP || id == RS_C2S_WALK_ANTICHEAT) {
        rs_walk w;
        if (rs_codec_walk_decode(s, payload, &w) == RS_OK) {
            printf("walk %s -> %d,%d\n", rs_conn_login(c)->username, w.dest_x, w.dest_z);
        }
        return;
    }

    if (id == RS_C2S_CHAT) {
        rs_chat_in chat;
        if (rs_codec_chat_in_decode(s, payload, &chat) == RS_OK) {
            printf("chat %s: %s\n", rs_conn_login(c)->username, chat.text);
            echo_text(c, chat.text);
        }
        return;
    }

    if (id == RS_C2S_COMMAND) {
        rs_command command;
        if (rs_codec_command_decode(s, payload, &command) == RS_OK) {
            printf("chat %s: %s\n", rs_conn_login(c)->username, command.text);
            echo_text(c, command.text);
        }
        return;
    }

    if (id == RS_C2S_PRIVATE_MESSAGE) {
        rs_private_out private_out;
        if (rs_codec_private_out_decode(s, payload, &private_out) == RS_OK) {
            printf("chat %s: %s\n", rs_conn_login(c)->username, private_out.text);
            echo_text(c, private_out.text);
        }
        return;
    }

    {
        const rs_build_info *build = rs_conn_build(c);
        int rev;
        rs_err ae = RS_ERR_UNSUPPORTED;

        if (!build || rs_pkt_opcode(build, id) != opcode) {
            return;
        }
        rev = build->revision;

        if (id >= RS_C2S_BUTTON && id <= RS_C2S_BUTTON5) {
            rs_action_button button;
            int which = (int)(id - RS_C2S_BUTTON) + 1;
            ae = rs_action_button_decode(rev, which, payload, &button);
            if (ae == RS_OK) {
                printf("button %d %d\n", which, button.interface_id);
            }
        } else if (id >= RS_C2S_NPC1 && id <= RS_C2S_NPC5) {
            rs_action_npc npc;
            int which = (int)(id - RS_C2S_NPC1) + 1;
            ae = rs_action_npc_decode(rev, which, payload, &npc);
            if (ae == RS_OK) {
                printf("npc %d %d\n", which, npc.index);
            }
        } else if (id >= RS_C2S_OBJECT1 && id <= RS_C2S_OBJECT5) {
            rs_action_object object;
            int which = (int)(id - RS_C2S_OBJECT1) + 1;
            ae = rs_action_object_decode(rev, which, payload, &object);
            if (ae == RS_OK) {
                printf("object %d %d %d,%d\n", which, object.id, object.x, object.z);
            }
        } else if (id == RS_C2S_GROUND_ITEM) {
            rs_action_ground ground;
            ae = rs_action_ground_decode(rev, payload, &ground);
            if (ae == RS_OK) {
                printf("ground item %d %d,%d\n", ground.id, ground.x, ground.z);
            }
        } else if (id == RS_C2S_TRADE) {
            rs_action_trade trade;
            ae = rs_action_trade_decode(rev, payload, &trade);
            if (ae == RS_OK) {
                printf("trade %d\n", trade.index);
            }
        } else if (id == RS_C2S_FOLLOW) {
            rs_action_follow follow;
            ae = rs_action_follow_decode(rev, payload, &follow);
            if (ae == RS_OK) {
                printf("follow %d\n", follow.index);
            }
        } else if (id == RS_C2S_ATTACK_PLAYER) {
            rs_action_attack_player attack;
            ae = rs_action_attack_player_decode(rev, payload, &attack);
            if (ae == RS_OK) {
                printf("attack %d\n", attack.index);
            }
        } else if (id == RS_C2S_ATTACK_NPC) {
            rs_action_attack_npc attack;
            ae = rs_action_attack_npc_decode(rev, payload, &attack);
            if (ae == RS_OK) {
                printf("attack %d\n", attack.index);
            }
        }
        if (ae != RS_OK && ae != RS_ERR_UNSUPPORTED && ae != RS_ERR_FORMAT) {
            fprintf(stderr, "action: %s\n", rs_strerror(ae));
        }
    }
}

static void on_update(rs_conn *c, const rs_update_req *req) {
    rs_err e = rs_conn_serve_update(c, req);
    if (e != RS_OK && e != RS_ERR_NOT_FOUND) {
        fprintf(stderr, "update %d:%d: %s\n", req->archive, req->id, rs_strerror(e));
    }
}

static void usage(void) {
    fprintf(stderr,
            "usage: rslib-world [--build <rev|id>] [--archive <builds/>] [--host <addr>] [--port N]\n"
            "  317 speaks on-demand; 474 speaks JS5 rebuild (Lumbridge 3222,3222). Echo chat.\n");
}

int main(int argc, char **argv) {
    const char *build_id = "317";
    const char *archive = "builds";
    const char *host = "0.0.0.0";
    int port = 43594;
    int i;
    rs_ctx *ctx;
    rs_server_hooks hooks;
    rs_server *srv;
    const rs_build_info *b;
    rs_err e;

    for (i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "--build") == 0 || strcmp(argv[i], "-b") == 0) && i + 1 < argc) {
            build_id = argv[++i];
        } else if (strcmp(argv[i], "--archive") == 0 && i + 1 < argc) {
            archive = argv[++i];
        } else if (strcmp(argv[i], "--host") == 0 && i + 1 < argc) {
            host = argv[++i];
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage();
            return 0;
        } else {
            usage();
            return 2;
        }
    }

    ctx = rs_ctx_create();
    if (!ctx || rs_ctx_set_build_id(ctx, build_id) != RS_OK) {
        fprintf(stderr, "unknown build '%s'\n", build_id);
        rs_ctx_destroy(ctx);
        return 1;
    }
    b = rs_ctx_build(ctx);

    memset(&hooks, 0, sizeof(hooks));
    hooks.on_login = on_login;
    hooks.on_packet = on_packet;
    hooks.on_update = on_update;
    srv = rs_server_create(ctx, &hooks);
    if (!srv) {
        fprintf(stderr, "rs_server_create failed\n");
        rs_ctx_destroy(ctx);
        return 1;
    }

    e = rs_server_open_archive(srv, archive);
    if (e != RS_OK) {
        fprintf(stderr, "cache at %s/%s: %s (update serving disabled)\n", archive,
                b->id, rs_strerror(e));
    }

    e = rs_server_listen(srv, host, port);
    if (e != RS_OK) {
        fprintf(stderr, "listen %s:%d: %s\n", host, port, rs_strerror(e));
        rs_server_destroy(srv);
        rs_ctx_destroy(ctx);
        return 1;
    }

    g_srv = srv;
#ifndef _WIN32
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);
    signal(SIGPIPE, SIG_IGN);
#endif

    printf("rslib-world %s rev %d %s on %s:%d\n", b->id, b->revision,
           b->js5 ? "js5" : (b->ondemand ? "ondemand" : "game"), host, rs_server_port(srv));
    e = rs_server_run(srv);
    if (e != RS_OK) {
        fprintf(stderr, "run: %s\n", rs_strerror(e));
    }
    rs_server_destroy(srv);
    rs_ctx_destroy(ctx);
    return e == RS_OK ? 0 : 1;
}
