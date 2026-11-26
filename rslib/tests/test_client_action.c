#include "rslib/client/client.h"

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

/* Build is taken at create. No connect: fd stays -1. */
static rs_client *open_rev(rs_ctx **ctx_out, int revision) {
    rs_ctx *ctx = rs_ctx_create();
    *ctx_out = ctx;
    if (!ctx) {
        return NULL;
    }
    if (revision > 0 && rs_ctx_set_build_revision(ctx, revision) != RS_OK) {
        return NULL;
    }
    return rs_client_create(ctx, NULL);
}

static void close_client(rs_client *c, rs_ctx *ctx) {
    rs_client_destroy(c);
    rs_ctx_destroy(ctx);
}

int main(void) {
    rs_ctx *ctx317 = NULL;
    rs_ctx *ctx468 = NULL;
    rs_ctx *ctx742 = NULL;
    rs_ctx *ctx831 = NULL;
    rs_ctx *ctx_none = NULL;
    rs_client *c317;
    rs_client *c468;
    rs_client *c742;
    rs_client *c831;
    rs_client *cnone;
    rs_action_button button;
    rs_action_item_on_item ioi;
    uint8_t storage[64];
    rs_buf dst;

    c317 = open_rev(&ctx317, 317);
    c468 = open_rev(&ctx468, 468);
    c742 = open_rev(&ctx742, 742);
    c831 = open_rev(&ctx831, 831);
    cnone = open_rev(&ctx_none, 0);
    expect("clients", c317 && c468 && c742 && c831 && cnone);
    if (!c317 || !c468 || !c742 || !c831 || !cnone) {
        close_client(c317, ctx317);
        close_client(c468, ctx468);
        close_client(c742, ctx742);
        close_client(c831, ctx831);
        close_client(cnone, ctx_none);
        return 1;
    }

    memset(&button, 0, sizeof(button));
    button.interface_id = 20000;
    rs_buf_wrap(&dst, storage, sizeof(storage));
    expect_err("317 button encode", rs_action_button_encode(317, 1, &dst, &button), RS_OK);

    expect_err("null client", rs_client_send_button(NULL, 1, &button), RS_ERR_ARG);
    expect_err("null value", rs_client_send_button(c317, 1, NULL), RS_ERR_ARG);
    expect_err("no build", rs_client_send_button(cnone, 1, &button), RS_ERR_PROTO);
    expect_err("which 0", rs_client_send_button(c317, 0, &button), RS_ERR_ARG);
    expect_err("which 6", rs_client_send_button(c317, 6, &button), RS_ERR_ARG);

    /* BUTTON2 is not bound on 317, so the opcode check fails before send. */
    expect_err("317 button2 unbound", rs_client_send_button(c317, 2, &button), RS_ERR_UNSUPPORTED);
    /* Encode succeeded above. fd < 0 makes rs_client_send return RS_ERR_ARG. */
    expect_err("317 button send", rs_client_send_button(c317, 1, &button), RS_ERR_ARG);
    expect("317 no socket", rs_client_fd(c317) < 0);

    /* 468 binds the button opcode; action.c has no 468 writer. */
    expect_err("468 button", rs_client_send_button(c468, 1, &button), RS_ERR_UNSUPPORTED);
    expect("468 no socket", rs_client_fd(c468) < 0);

    /* 831 has no packet table. Unsupported before encode or a socket write. */
    expect_err("831 button", rs_client_send_button(c831, 1, &button), RS_ERR_UNSUPPORTED);
    expect("831 no socket", rs_client_fd(c831) < 0);

    /* 742 encodes item-on-item, but that rs_pkt id is not bound. */
    memset(&ioi, 0, sizeof(ioi));
    dst.pos = 0;
    expect_err("742 item-on-item encode", rs_action_item_on_item_encode(742, &dst, &ioi), RS_OK);
    expect_err("742 item-on-item unbound", rs_client_send_item_on_item(c742, &ioi), RS_ERR_UNSUPPORTED);
    expect("742 no socket", rs_client_fd(c742) < 0);

    close_client(c317, ctx317);
    close_client(c468, ctx468);
    close_client(c742, ctx742);
    close_client(c831, ctx831);
    close_client(cnone, ctx_none);
    if (fails) {
        fprintf(stderr, "%d failed\n", fails);
        return 1;
    }
    return 0;
}
