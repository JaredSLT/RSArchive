#include "internal.h"

#include "rslib/both/action.h"

/* Payload only. Field order is the client writer's order after the opcode. */

#define W(expr)                         \
    do {                                \
        rs_err _e = (expr);             \
        if (_e) {                       \
            return _e;                  \
        }                               \
    } while (0)

static rs_err args2(const void *a, const void *b) {
    if (!a || !b) {
        return RS_ERR_ARG;
    }
    return RS_OK;
}

static rs_err which_ok(int which) {
    if (which < 1 || which > 5) {
        return RS_ERR_ARG;
    }
    return RS_OK;
}

static int early_name(int revision) {
    return revision == 317 || revision == 254 || revision == 377;
}

static int late_name(int revision) {
    return revision == 742 || revision == 910;
}

static rs_err put_name37(rs_buf *dst, uint64_t name37) {
    return rs_p8(dst, name37);
}

static rs_err get_name37(rs_buf *src, uint64_t *name37) {
    *name37 = rs_g8(src);
    return RS_OK;
}

static rs_err put_pjstr(rs_buf *dst, const char *name) {
    return rs_pjstrnull(dst, name);
}

static rs_err get_pjstr(rs_buf *src, char *name) {
    return rs_gjstrnull(src, name, RS_ACTION_NAME);
}

/* ---- button ---- */

rs_err rs_action_button_encode(int revision, int which, rs_buf *dst, const rs_action_button *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    e = which_ok(which);
    if (e) {
        return e;
    }
    if (revision == 317 || revision == 254 || revision == 377) {
        /* 317 actions 315/646/169, 254 actions 231/225/435, 377 actions 352/518/890: p2(component). */
        if (which != 1) {
            return RS_ERR_UNSUPPORTED;
        }
        return rs_p2(dst, v->interface_id);
    }
    if (revision == 742) {
        /* opClient319: p2_alt3(item), p2_alt3(arg2), p4_alt3(packed widget). */
        W(rs_p2_le_add128(dst, v->item));
        W(rs_p2_le_add128(dst, v->child));
        return rs_p4_alt3(dst, v->interface_id);
    }
    if (revision == 910) {
        /* doClient: p2_alt3(invobject), p2_alt2(component id), p4(parentlayer). */
        W(rs_p2_le_add128(dst, v->item));
        W(rs_p2_add128(dst, v->child));
        return rs_p4(dst, v->interface_id);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_button_decode(int revision, int which, rs_buf *src, rs_action_button *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    e = which_ok(which);
    if (e) {
        return e;
    }
    if (revision == 317 || revision == 254 || revision == 377) {
        if (which != 1) {
            return RS_ERR_UNSUPPORTED;
        }
        memset(v, 0, sizeof(*v));
        v->interface_id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 742) {
        memset(v, 0, sizeof(*v));
        v->item = rs_g2_le_add128(src);
        v->child = rs_g2_le_add128(src);
        v->interface_id = rs_g4_alt3(src);
        return RS_OK;
    }
    if (revision == 910) {
        memset(v, 0, sizeof(*v));
        v->item = rs_g2_le_add128(src);
        v->child = rs_g2_add128(src);
        v->interface_id = rs_g4(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

/* ---- component iop ---- */

/*
 * Client.java useMenuOption. var2 is the slot, var3 is the interface, var5 is the item.
 * which 1 action 632 opcode 145: p2_alt2(interface), p2_alt2(slot), p2_alt2(item).
 * which 2 action 78  opcode 117: p2_alt3(interface), p2_alt3(item), p2_alt1(slot).
 * which 3 action 867 opcode 43:  p2_alt1(interface), p2_alt2(item), p2_alt2(slot).
 * which 4 action 431 opcode 129: p2_alt2(slot), p2(interface), p2_alt2(item).
 * which 5 action 53  opcode 135: p2_alt1(slot), p2_alt2(interface), p2_alt1(item).
 * p2_alt1 is little-endian. p2_alt2 adds 128 to the low byte of a big-endian word.
 * p2_alt3 adds 128 to the low byte of a little-endian word.
 */
rs_err rs_action_if_option_encode(int revision, int which, rs_buf *dst, const rs_action_if_option *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    e = which_ok(which);
    if (e) {
        return e;
    }
    if (revision != 317) {
        return RS_ERR_UNSUPPORTED;
    }
    if (which == 1) {
        W(rs_p2_add128(dst, v->interface_id));
        W(rs_p2_add128(dst, v->slot));
        return rs_p2_add128(dst, v->item);
    }
    if (which == 2) {
        W(rs_p2_le_add128(dst, v->interface_id));
        W(rs_p2_le_add128(dst, v->item));
        return rs_p2_le(dst, v->slot);
    }
    if (which == 3) {
        W(rs_p2_le(dst, v->interface_id));
        W(rs_p2_add128(dst, v->item));
        return rs_p2_add128(dst, v->slot);
    }
    if (which == 4) {
        W(rs_p2_add128(dst, v->slot));
        W(rs_p2(dst, v->interface_id));
        return rs_p2_add128(dst, v->item);
    }
    W(rs_p2_le(dst, v->slot));
    W(rs_p2_add128(dst, v->interface_id));
    return rs_p2_le(dst, v->item);
}

rs_err rs_action_if_option_decode(int revision, int which, rs_buf *src, rs_action_if_option *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    e = which_ok(which);
    if (e) {
        return e;
    }
    if (revision != 317) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (which == 1) {
        v->interface_id = rs_g2_add128(src);
        v->slot = rs_g2_add128(src);
        v->item = rs_g2_add128(src);
        return RS_OK;
    }
    if (which == 2) {
        v->interface_id = rs_g2_le_add128(src);
        v->item = rs_g2_le_add128(src);
        v->slot = rs_g2_le(src);
        return RS_OK;
    }
    if (which == 3) {
        v->interface_id = rs_g2_le(src);
        v->item = rs_g2_add128(src);
        v->slot = rs_g2_add128(src);
        return RS_OK;
    }
    if (which == 4) {
        v->slot = rs_g2_add128(src);
        v->interface_id = rs_g2(src);
        v->item = rs_g2_add128(src);
        return RS_OK;
    }
    v->slot = rs_g2_le(src);
    v->interface_id = rs_g2_add128(src);
    v->item = rs_g2_le(src);
    return RS_OK;
}

/* Client.java action 74 opcode 122: p2_alt3(interface), p2_alt2(slot), p2_alt1(item). */
rs_err rs_action_opheld_encode(int revision, rs_buf *dst, const rs_action_if_option *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision != 317) {
        return RS_ERR_UNSUPPORTED;
    }
    W(rs_p2_le_add128(dst, v->interface_id));
    W(rs_p2_add128(dst, v->slot));
    return rs_p2_le(dst, v->item);
}

rs_err rs_action_opheld_decode(int revision, rs_buf *src, rs_action_if_option *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    v->interface_id = rs_g2_le_add128(src);
    v->slot = rs_g2_add128(src);
    v->item = rs_g2_le(src);
    return RS_OK;
}

/* ---- npc ---- */

rs_err rs_action_npc_encode(int revision, int which, rs_buf *dst, const rs_action_npc *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    e = which_ok(which);
    if (e) {
        return e;
    }
    if (revision == 317) {
        /* which 5 is action 779: a player index on the NPC5 opcode. */
        if (which == 1 || which == 4 || which == 5) {
            return rs_p2_le(dst, v->index);
        }
        if (which == 2) {
            return rs_p2_le_add128(dst, v->index);
        }
        return rs_p2(dst, v->index);
    }
    if (revision == 377) {
        if (which == 1) {
            return rs_p2_add128(dst, v->index);
        }
        if (which == 2) {
            return rs_p2_le(dst, v->index);
        }
        return RS_ERR_UNSUPPORTED;
    }
    if (revision == 742) {
        W(rs_p2_le(dst, v->index));
        return rs_p1(dst, v->ctrl);
    }
    if (revision == 910) {
        W(rs_p1_sub128(dst, v->ctrl));
        return rs_p2_add128(dst, v->index);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_npc_decode(int revision, int which, rs_buf *src, rs_action_npc *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    e = which_ok(which);
    if (e) {
        return e;
    }
    if (revision == 317) {
        memset(v, 0, sizeof(*v));
        if (which == 1 || which == 4 || which == 5) {
            v->index = rs_g2_le(src);
        } else if (which == 2) {
            v->index = rs_g2_le_add128(src);
        } else {
            v->index = rs_g2(src);
        }
        return RS_OK;
    }
    if (revision == 377) {
        if (which > 2) {
            return RS_ERR_UNSUPPORTED;
        }
        memset(v, 0, sizeof(*v));
        v->index = (which == 1) ? rs_g2_add128(src) : rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 742) {
        memset(v, 0, sizeof(*v));
        v->index = rs_g2_le(src);
        v->ctrl = rs_g1(src);
        return RS_OK;
    }
    if (revision == 910) {
        memset(v, 0, sizeof(*v));
        v->ctrl = rs_g1_sub128(src);
        v->index = rs_g2_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_attack_npc_encode(int revision, rs_buf *dst, const rs_action_attack_npc *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        return rs_p2_add128(dst, v->index);
    }
    if (revision == 377) {
        return rs_p2_le_add128(dst, v->index);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_attack_npc_decode(int revision, rs_buf *src, rs_action_attack_npc *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        memset(v, 0, sizeof(*v));
        v->index = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        memset(v, 0, sizeof(*v));
        v->index = rs_g2_le_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

/* ---- object / ground ---- */

rs_err rs_action_object_encode(int revision, int which, rs_buf *dst, const rs_action_object *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    e = which_ok(which);
    if (e) {
        return e;
    }
    if (revision == 317) {
        if (which == 1) {
            W(rs_p2_le_add128(dst, v->x));
            W(rs_p2(dst, v->id));
            return rs_p2_add128(dst, v->z);
        }
        if (which == 2) {
            W(rs_p2_le_add128(dst, v->id));
            W(rs_p2_le(dst, v->z));
            return rs_p2_add128(dst, v->x);
        }
        if (which == 3) {
            W(rs_p2_le(dst, v->x));
            W(rs_p2(dst, v->z));
            return rs_p2_le_add128(dst, v->id);
        }
        if (which == 4) {
            W(rs_p2_le_add128(dst, v->x));
            W(rs_p2_add128(dst, v->id));
            return rs_p2_le_add128(dst, v->z);
        }
        W(rs_p2_add128(dst, v->id));
        W(rs_p2_add128(dst, v->z));
        return rs_p2(dst, v->x);
    }
    if (revision == 377) {
        if (which == 1) {
            W(rs_p2(dst, v->x));
            W(rs_p2_le(dst, v->z));
            return rs_p2(dst, v->id);
        }
        if (which == 2) {
            W(rs_p2_le(dst, v->id));
            W(rs_p2_le(dst, v->z));
            return rs_p2(dst, v->x);
        }
        if (which == 3) {
            W(rs_p2_add128(dst, v->x));
            W(rs_p2_le(dst, v->z));
            return rs_p2_le(dst, v->id);
        }
        if (which == 4) {
            W(rs_p2_add128(dst, v->z));
            W(rs_p2_le(dst, v->id));
            return rs_p2_le_add128(dst, v->x);
        }
        W(rs_p2(dst, v->id));
        W(rs_p2(dst, v->x));
        return rs_p2_add128(dst, v->z);
    }
    if (revision == 742) {
        W(rs_p1_neg(dst, v->ctrl));
        W(rs_p2_le_add128(dst, v->z));
        W(rs_ip4(dst, v->id));
        return rs_p2_le_add128(dst, v->x);
    }
    if (revision == 910) {
        W(rs_p1_neg(dst, v->ctrl));
        W(rs_p2(dst, v->z));
        W(rs_p4(dst, v->id));
        return rs_p2_le_add128(dst, v->x);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_object_decode(int revision, int which, rs_buf *src, rs_action_object *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    e = which_ok(which);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        if (which == 1) {
            v->x = rs_g2_le_add128(src);
            v->id = rs_g2(src);
            v->z = rs_g2_add128(src);
        } else if (which == 2) {
            v->id = rs_g2_le_add128(src);
            v->z = rs_g2_le(src);
            v->x = rs_g2_add128(src);
        } else if (which == 3) {
            v->x = rs_g2_le(src);
            v->z = rs_g2(src);
            v->id = rs_g2_le_add128(src);
        } else if (which == 4) {
            v->x = rs_g2_le_add128(src);
            v->id = rs_g2_add128(src);
            v->z = rs_g2_le_add128(src);
        } else {
            v->id = rs_g2_add128(src);
            v->z = rs_g2_add128(src);
            v->x = rs_g2(src);
        }
        return RS_OK;
    }
    if (revision == 377) {
        if (which == 1) {
            v->x = rs_g2(src);
            v->z = rs_g2_le(src);
            v->id = rs_g2(src);
        } else if (which == 2) {
            v->id = rs_g2_le(src);
            v->z = rs_g2_le(src);
            v->x = rs_g2(src);
        } else if (which == 3) {
            v->x = rs_g2_add128(src);
            v->z = rs_g2_le(src);
            v->id = rs_g2_le(src);
        } else if (which == 4) {
            v->z = rs_g2_add128(src);
            v->id = rs_g2_le(src);
            v->x = rs_g2_le_add128(src);
        } else {
            v->id = rs_g2(src);
            v->x = rs_g2(src);
            v->z = rs_g2_add128(src);
        }
        return RS_OK;
    }
    if (revision == 742) {
        v->ctrl = rs_g1_neg(src);
        v->z = rs_g2_le_add128(src);
        v->id = rs_ig4(src);
        v->x = rs_g2_le_add128(src);
        return RS_OK;
    }
    v->ctrl = rs_g1_neg(src);
    v->z = rs_g2(src);
    v->id = rs_g4(src);
    v->x = rs_g2_le_add128(src);
    return RS_OK;
}

rs_err rs_action_ground_encode(int revision, rs_buf *dst, const rs_action_ground *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_le(dst, v->z));
        W(rs_p2(dst, v->id));
        return rs_p2_le(dst, v->x);
    }
    if (revision == 377) {
        W(rs_p2_le_add128(dst, v->id));
        W(rs_p2_le_add128(dst, v->x));
        return rs_p2_add128(dst, v->z);
    }
    if (revision == 742) {
        W(rs_p1(dst, v->ctrl));
        W(rs_p2_le(dst, v->z));
        W(rs_p2(dst, v->x));
        return rs_p2(dst, v->id);
    }
    if (revision == 910) {
        W(rs_p2_le(dst, v->id));
        W(rs_p2_le(dst, v->x));
        W(rs_p2(dst, v->z));
        return rs_p1_sub128(dst, v->ctrl);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_ground_decode(int revision, rs_buf *src, rs_action_ground *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->z = rs_g2_le(src);
        v->id = rs_g2(src);
        v->x = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->id = rs_g2_le_add128(src);
        v->x = rs_g2_le_add128(src);
        v->z = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->ctrl = rs_g1(src);
        v->z = rs_g2_le(src);
        v->x = rs_g2(src);
        v->id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->id = rs_g2_le(src);
        v->x = rs_g2_le(src);
        v->z = rs_g2(src);
        v->ctrl = rs_g1_sub128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

/* ---- item on ---- */

static rs_err item_on_item_late(int revision, rs_buf *dst, const rs_action_item_on_item *v) {
    if (revision == 742) {
        /* IF_BUTTONT. */
        W(rs_p2_le(dst, v->selected_item));
        W(rs_ip4(dst, v->interface_id));
        W(rs_ip4(dst, v->selected_interface));
        W(rs_p2_add128(dst, v->selected_slot));
        W(rs_p2(dst, v->slot));
        return rs_p2_le_add128(dst, v->item);
    }
    /* 910 IF_BUTTONT. */
    W(rs_ip4(dst, v->interface_id));
    W(rs_p2(dst, v->item));
    W(rs_p2_add128(dst, v->selected_item));
    W(rs_p2_add128(dst, v->slot));
    W(rs_p2_add128(dst, v->selected_slot));
    return rs_ip4(dst, v->selected_interface);
}

rs_err rs_action_item_on_item_encode(int revision, rs_buf *dst, const rs_action_item_on_item *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2(dst, v->slot));
        W(rs_p2_add128(dst, v->selected_slot));
        W(rs_p2_le_add128(dst, v->item));
        W(rs_p2(dst, v->selected_interface));
        W(rs_p2_le(dst, v->selected_item));
        return rs_p2(dst, v->interface_id);
    }
    if (revision == 377) {
        /* action 903, use-with on an inventory slot. */
        W(rs_p2(dst, v->item));
        W(rs_p2_le(dst, v->selected_slot));
        W(rs_p2_le(dst, v->selected_item));
        W(rs_p2_le_add128(dst, v->selected_interface));
        W(rs_p2_add128(dst, v->slot));
        return rs_p2_add128(dst, v->interface_id);
    }
    if (revision == 742 || revision == 910) {
        return item_on_item_late(revision, dst, v);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_item_on_item_decode(int revision, rs_buf *src, rs_action_item_on_item *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->slot = rs_g2(src);
        v->selected_slot = rs_g2_add128(src);
        v->item = rs_g2_le_add128(src);
        v->selected_interface = rs_g2(src);
        v->selected_item = rs_g2_le(src);
        v->interface_id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->item = rs_g2(src);
        v->selected_slot = rs_g2_le(src);
        v->selected_item = rs_g2_le(src);
        v->selected_interface = rs_g2_le_add128(src);
        v->slot = rs_g2_add128(src);
        v->interface_id = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->selected_item = rs_g2_le(src);
        v->interface_id = rs_ig4(src);
        v->selected_interface = rs_ig4(src);
        v->selected_slot = rs_g2_add128(src);
        v->slot = rs_g2(src);
        v->item = rs_g2_le_add128(src);
        return RS_OK;
    }
    v->interface_id = rs_ig4(src);
    v->item = rs_g2(src);
    v->selected_item = rs_g2_add128(src);
    v->slot = rs_g2_add128(src);
    v->selected_slot = rs_g2_add128(src);
    v->selected_interface = rs_ig4(src);
    return RS_OK;
}

rs_err rs_action_item_on_npc_encode(int revision, rs_buf *dst, const rs_action_item_on_npc *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_add128(dst, v->item));
        W(rs_p2_add128(dst, v->index));
        W(rs_p2_le(dst, v->slot));
        return rs_p2_add128(dst, v->interface_id);
    }
    if (revision == 377) {
        W(rs_p2(dst, v->index));
        W(rs_p2_le(dst, v->item));
        W(rs_p2_le_add128(dst, v->interface_id));
        return rs_p2(dst, v->slot);
    }
    if (revision == 742) {
        W(rs_p2_le_add128(dst, v->index));
        W(rs_p4_alt2(dst, v->interface_id));
        W(rs_p2_add128(dst, v->item));
        W(rs_p2(dst, v->component));
        return rs_p1_sub128(dst, v->ctrl);
    }
    if (revision == 910) {
        W(rs_p4(dst, v->interface_id));
        W(rs_p2(dst, v->index));
        W(rs_p1_add128(dst, v->ctrl));
        W(rs_p2_le(dst, v->item));
        return rs_p2(dst, v->component);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_item_on_npc_decode(int revision, rs_buf *src, rs_action_item_on_npc *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->item = rs_g2_add128(src);
        v->index = rs_g2_add128(src);
        v->slot = rs_g2_le(src);
        v->interface_id = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->index = rs_g2(src);
        v->item = rs_g2_le(src);
        v->interface_id = rs_g2_le_add128(src);
        v->slot = rs_g2(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->index = rs_g2_le_add128(src);
        v->interface_id = rs_g4_alt2(src);
        v->item = rs_g2_add128(src);
        v->component = rs_g2(src);
        v->ctrl = rs_g1_sub128(src);
        return RS_OK;
    }
    v->interface_id = rs_g4(src);
    v->index = rs_g2(src);
    v->ctrl = rs_g1_add128(src);
    v->item = rs_g2_le(src);
    v->component = rs_g2(src);
    return RS_OK;
}

rs_err rs_action_item_on_object_encode(int revision, rs_buf *dst, const rs_action_item_on_object *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2(dst, v->interface_id));
        W(rs_p2_le(dst, v->id));
        W(rs_p2_le_add128(dst, v->z));
        W(rs_p2_le(dst, v->slot));
        W(rs_p2_le_add128(dst, v->x));
        return rs_p2(dst, v->item);
    }
    if (revision == 377) {
        /* action 467. Opcode 210 is a spell on a loc, not this writer. */
        W(rs_p2_le(dst, v->id));
        W(rs_p2_le(dst, v->interface_id));
        W(rs_p2_le(dst, v->item));
        W(rs_p2_le(dst, v->z));
        W(rs_p2(dst, v->slot));
        return rs_p2_le_add128(dst, v->x);
    }
    if (revision == 742) {
        W(rs_p2_add128(dst, v->z));
        W(rs_p2_le_add128(dst, v->x));
        W(rs_p2(dst, v->item));
        W(rs_p1_neg(dst, v->ctrl));
        W(rs_p4(dst, v->id));
        W(rs_ip4(dst, v->interface_id));
        return rs_p2_le_add128(dst, v->component);
    }
    if (revision == 910) {
        W(rs_p1_add128(dst, v->ctrl));
        W(rs_p2_le(dst, v->x));
        W(rs_p2_le(dst, v->item));
        W(rs_p2_le_add128(dst, v->z));
        W(rs_ip4(dst, v->interface_id));
        W(rs_p4_alt2(dst, v->id));
        return rs_p2(dst, v->component);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_item_on_object_decode(int revision, rs_buf *src, rs_action_item_on_object *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->interface_id = rs_g2(src);
        v->id = rs_g2_le(src);
        v->z = rs_g2_le_add128(src);
        v->slot = rs_g2_le(src);
        v->x = rs_g2_le_add128(src);
        v->item = rs_g2(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->id = rs_g2_le(src);
        v->interface_id = rs_g2_le(src);
        v->item = rs_g2_le(src);
        v->z = rs_g2_le(src);
        v->slot = rs_g2(src);
        v->x = rs_g2_le_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->z = rs_g2_add128(src);
        v->x = rs_g2_le_add128(src);
        v->item = rs_g2(src);
        v->ctrl = rs_g1_neg(src);
        v->id = rs_g4(src);
        v->interface_id = rs_ig4(src);
        v->component = rs_g2_le_add128(src);
        return RS_OK;
    }
    v->ctrl = rs_g1_add128(src);
    v->x = rs_g2_le(src);
    v->item = rs_g2_le(src);
    v->z = rs_g2_le_add128(src);
    v->interface_id = rs_ig4(src);
    v->id = rs_g4_alt2(src);
    v->component = rs_g2(src);
    return RS_OK;
}

rs_err rs_action_item_on_player_encode(int revision, rs_buf *dst, const rs_action_item_on_player *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_add128(dst, v->interface_id));
        W(rs_p2(dst, v->index));
        W(rs_p2(dst, v->item));
        return rs_p2_le(dst, v->slot);
    }
    if (revision == 377) {
        /* action 596. */
        W(rs_p2_le(dst, v->item));
        W(rs_p2_le_add128(dst, v->slot));
        W(rs_p2(dst, v->interface_id));
        return rs_p2_add128(dst, v->index);
    }
    if (revision == 742) {
        W(rs_p2_add128(dst, v->item));
        W(rs_p1(dst, v->ctrl));
        W(rs_p4(dst, v->interface_id));
        W(rs_p2(dst, v->component));
        return rs_p2_le(dst, v->index);
    }
    if (revision == 910) {
        W(rs_p2(dst, v->component));
        W(rs_p1_add128(dst, v->ctrl));
        W(rs_p2_le(dst, v->item));
        W(rs_p2_le_add128(dst, v->index));
        return rs_p4_alt2(dst, v->interface_id);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_item_on_player_decode(int revision, rs_buf *src, rs_action_item_on_player *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->interface_id = rs_g2_add128(src);
        v->index = rs_g2(src);
        v->item = rs_g2(src);
        v->slot = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->item = rs_g2_le(src);
        v->slot = rs_g2_le_add128(src);
        v->interface_id = rs_g2(src);
        v->index = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->item = rs_g2_add128(src);
        v->ctrl = rs_g1(src);
        v->interface_id = rs_g4(src);
        v->component = rs_g2(src);
        v->index = rs_g2_le(src);
        return RS_OK;
    }
    v->component = rs_g2(src);
    v->ctrl = rs_g1_add128(src);
    v->item = rs_g2_le(src);
    v->index = rs_g2_le_add128(src);
    v->interface_id = rs_g4_alt2(src);
    return RS_OK;
}

rs_err rs_action_item_on_ground_encode(int revision, rs_buf *dst, const rs_action_item_on_ground *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_le(dst, v->interface_id));
        W(rs_p2_add128(dst, v->item));
        W(rs_p2(dst, v->id));
        W(rs_p2_add128(dst, v->z));
        W(rs_p2_le_add128(dst, v->slot));
        return rs_p2(dst, v->x);
    }
    if (revision == 377) {
        W(rs_p2_le_add128(dst, v->slot));
        W(rs_p2_add128(dst, v->item));
        W(rs_p2_le_add128(dst, v->z));
        W(rs_p2_le_add128(dst, v->x));
        W(rs_p2_le(dst, v->interface_id));
        return rs_p2_le(dst, v->id);
    }
    if (revision == 742) {
        W(rs_p2_le_add128(dst, v->z));
        W(rs_p4(dst, v->interface_id));
        W(rs_p2_add128(dst, v->component));
        W(rs_p2_le_add128(dst, v->id));
        W(rs_p2_add128(dst, v->item));
        W(rs_p2(dst, v->x));
        return rs_p1_sub128(dst, v->ctrl);
    }
    if (revision == 910) {
        W(rs_p2_le(dst, v->id));
        W(rs_p1_add128(dst, v->ctrl));
        W(rs_p2_le(dst, v->item));
        W(rs_p2_le(dst, v->z));
        W(rs_p2_le(dst, v->x));
        W(rs_p4_alt2(dst, v->interface_id));
        return rs_p2_le_add128(dst, v->component);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_item_on_ground_decode(int revision, rs_buf *src, rs_action_item_on_ground *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->interface_id = rs_g2_le(src);
        v->item = rs_g2_add128(src);
        v->id = rs_g2(src);
        v->z = rs_g2_add128(src);
        v->slot = rs_g2_le_add128(src);
        v->x = rs_g2(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->slot = rs_g2_le_add128(src);
        v->item = rs_g2_add128(src);
        v->z = rs_g2_le_add128(src);
        v->x = rs_g2_le_add128(src);
        v->interface_id = rs_g2_le(src);
        v->id = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->z = rs_g2_le_add128(src);
        v->interface_id = rs_g4(src);
        v->component = rs_g2_add128(src);
        v->id = rs_g2_le_add128(src);
        v->item = rs_g2_add128(src);
        v->x = rs_g2(src);
        v->ctrl = rs_g1_sub128(src);
        return RS_OK;
    }
    v->id = rs_g2_le(src);
    v->ctrl = rs_g1_add128(src);
    v->item = rs_g2_le(src);
    v->z = rs_g2_le(src);
    v->x = rs_g2_le(src);
    v->interface_id = rs_g4_alt2(src);
    v->component = rs_g2_le_add128(src);
    return RS_OK;
}

rs_err rs_action_equip_encode(int revision, rs_buf *dst, const rs_action_equip *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2(dst, v->item));
        W(rs_p2_add128(dst, v->slot));
        return rs_p2_add128(dst, v->interface_id);
    }
    if (revision == 377) {
        W(rs_p2_add128(dst, v->item));
        W(rs_p2(dst, v->interface_id));
        return rs_p2(dst, v->slot);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_equip_decode(int revision, rs_buf *src, rs_action_equip *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->item = rs_g2(src);
        v->slot = rs_g2_add128(src);
        v->interface_id = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->item = rs_g2_add128(src);
        v->interface_id = rs_g2(src);
        v->slot = rs_g2(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_drop_encode(int revision, rs_buf *dst, const rs_action_drop *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_add128(dst, v->item));
        W(rs_p2(dst, v->interface_id));
        return rs_p2_add128(dst, v->slot);
    }
    if (revision == 377) {
        W(rs_p2_add128(dst, v->interface_id));
        W(rs_p2_le(dst, v->slot));
        return rs_p2_le(dst, v->item);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_drop_decode(int revision, rs_buf *src, rs_action_drop *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->item = rs_g2_add128(src);
        v->interface_id = rs_g2(src);
        v->slot = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->interface_id = rs_g2_add128(src);
        v->slot = rs_g2_le(src);
        v->item = rs_g2_le(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_move_item_encode(int revision, rs_buf *dst, const rs_action_move_item *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_le_add128(dst, v->interface_id));
        W(rs_p1_neg(dst, v->insert_mode));
        W(rs_p2_le_add128(dst, v->from_slot));
        return rs_p2_le(dst, v->to_slot);
    }
    if (revision == 254) {
        W(rs_p2(dst, v->interface_id));
        W(rs_p2(dst, v->from_slot));
        W(rs_p2(dst, v->to_slot));
        return rs_p1(dst, v->insert_mode);
    }
    if (revision == 377) {
        W(rs_p2_le_add128(dst, v->to_slot));
        W(rs_p1_add128(dst, v->insert_mode));
        W(rs_p2_add128(dst, v->interface_id));
        return rs_p2_le(dst, v->from_slot);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_move_item_decode(int revision, rs_buf *src, rs_action_move_item *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->interface_id = rs_g2_le_add128(src);
        v->insert_mode = rs_g1_neg(src);
        v->from_slot = rs_g2_le_add128(src);
        v->to_slot = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 254) {
        v->interface_id = rs_g2(src);
        v->from_slot = rs_g2(src);
        v->to_slot = rs_g2(src);
        v->insert_mode = rs_g1(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->to_slot = rs_g2_le_add128(src);
        v->insert_mode = rs_g1_add128(src);
        v->interface_id = rs_g2_add128(src);
        v->from_slot = rs_g2_le(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

/* ---- magic on ---- */

rs_err rs_action_magic_on_npc_encode(int revision, rs_buf *dst, const rs_action_magic_on_npc *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_le_add128(dst, v->index));
        return rs_p2_add128(dst, v->component);
    }
    if (revision == 377) {
        /* action 67 casts on an npc. */
        W(rs_p2_add128(dst, v->component));
        return rs_p2_le(dst, v->index);
    }
    if (revision == 742) {
        W(rs_p2_le_add128(dst, v->index));
        W(rs_p4_alt2(dst, v->interface_id));
        W(rs_p2_add128(dst, v->item));
        W(rs_p2(dst, v->component));
        return rs_p1_sub128(dst, v->ctrl);
    }
    if (revision == 910) {
        W(rs_p4(dst, v->interface_id));
        W(rs_p2(dst, v->index));
        W(rs_p1_add128(dst, v->ctrl));
        W(rs_p2_le(dst, v->item));
        return rs_p2(dst, v->component);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_magic_on_npc_decode(int revision, rs_buf *src, rs_action_magic_on_npc *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->index = rs_g2_le_add128(src);
        v->component = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->component = rs_g2_add128(src);
        v->index = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->index = rs_g2_le_add128(src);
        v->interface_id = rs_g4_alt2(src);
        v->item = rs_g2_add128(src);
        v->component = rs_g2(src);
        v->ctrl = rs_g1_sub128(src);
        return RS_OK;
    }
    v->interface_id = rs_g4(src);
    v->index = rs_g2(src);
    v->ctrl = rs_g1_add128(src);
    v->item = rs_g2_le(src);
    v->component = rs_g2(src);
    return RS_OK;
}

rs_err rs_action_magic_on_player_encode(int revision, rs_buf *dst, const rs_action_magic_on_player *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_add128(dst, v->index));
        return rs_p2_le(dst, v->component);
    }
    if (revision == 377) {
        /* action 918 casts on a player. */
        W(rs_p2(dst, v->index));
        return rs_p2_le(dst, v->component);
    }
    if (revision == 742) {
        W(rs_p2_add128(dst, v->item));
        W(rs_p1(dst, v->ctrl));
        W(rs_p4(dst, v->interface_id));
        W(rs_p2(dst, v->component));
        return rs_p2_le(dst, v->index);
    }
    if (revision == 910) {
        W(rs_p2(dst, v->component));
        W(rs_p1_add128(dst, v->ctrl));
        W(rs_p2_le(dst, v->item));
        W(rs_p2_le_add128(dst, v->index));
        return rs_p4_alt2(dst, v->interface_id);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_magic_on_player_decode(int revision, rs_buf *src, rs_action_magic_on_player *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->index = rs_g2_add128(src);
        v->component = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->index = rs_g2(src);
        v->component = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->item = rs_g2_add128(src);
        v->ctrl = rs_g1(src);
        v->interface_id = rs_g4(src);
        v->component = rs_g2(src);
        v->index = rs_g2_le(src);
        return RS_OK;
    }
    v->component = rs_g2(src);
    v->ctrl = rs_g1_add128(src);
    v->item = rs_g2_le(src);
    v->index = rs_g2_le_add128(src);
    v->interface_id = rs_g4_alt2(src);
    return RS_OK;
}

rs_err rs_action_magic_on_item_encode(int revision, rs_buf *dst, const rs_action_magic_on_item *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2(dst, v->slot));
        W(rs_p2_add128(dst, v->item));
        W(rs_p2(dst, v->interface_id));
        return rs_p2_add128(dst, v->component);
    }
    if (revision == 377) {
        W(rs_p2(dst, v->component));
        W(rs_p2_add128(dst, v->interface_id));
        W(rs_p2_add128(dst, v->slot));
        return rs_p2_add128(dst, v->item);
    }
    if (revision == 742) {
        W(rs_p2_le(dst, v->selected_item));
        W(rs_ip4(dst, v->interface_id));
        W(rs_ip4(dst, v->selected_interface));
        W(rs_p2_add128(dst, v->selected_slot));
        W(rs_p2(dst, v->slot));
        return rs_p2_le_add128(dst, v->item);
    }
    if (revision == 910) {
        /* IF_BUTTONT. selected_slot is activeComponentId. */
        W(rs_ip4(dst, v->interface_id));
        W(rs_p2(dst, v->item));
        W(rs_p2_add128(dst, v->selected_item));
        W(rs_p2_add128(dst, v->slot));
        W(rs_p2_add128(dst, v->selected_slot));
        return rs_ip4(dst, v->selected_interface);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_magic_on_item_decode(int revision, rs_buf *src, rs_action_magic_on_item *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->slot = rs_g2(src);
        v->item = rs_g2_add128(src);
        v->interface_id = rs_g2(src);
        v->component = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->component = rs_g2(src);
        v->interface_id = rs_g2_add128(src);
        v->slot = rs_g2_add128(src);
        v->item = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->selected_item = rs_g2_le(src);
        v->interface_id = rs_ig4(src);
        v->selected_interface = rs_ig4(src);
        v->selected_slot = rs_g2_add128(src);
        v->slot = rs_g2(src);
        v->item = rs_g2_le_add128(src);
        return RS_OK;
    }
    v->interface_id = rs_ig4(src);
    v->item = rs_g2(src);
    v->selected_item = rs_g2_add128(src);
    v->slot = rs_g2_add128(src);
    v->selected_slot = rs_g2_add128(src);
    v->selected_interface = rs_ig4(src);
    return RS_OK;
}

rs_err rs_action_magic_on_ground_encode(int revision, rs_buf *dst, const rs_action_magic_on_ground *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        W(rs_p2_le(dst, v->z));
        W(rs_p2(dst, v->id));
        W(rs_p2_le(dst, v->x));
        return rs_p2_add128(dst, v->component);
    }
    if (revision == 377) {
        W(rs_p2_le(dst, v->id));
        W(rs_p2(dst, v->z));
        W(rs_p2_le(dst, v->component));
        return rs_p2_le_add128(dst, v->x);
    }
    if (revision == 742) {
        W(rs_p2_le_add128(dst, v->z));
        W(rs_p4(dst, v->interface_id));
        W(rs_p2_add128(dst, v->component));
        W(rs_p2_le_add128(dst, v->id));
        W(rs_p2_add128(dst, v->item));
        W(rs_p2(dst, v->x));
        return rs_p1_sub128(dst, v->ctrl);
    }
    if (revision == 910) {
        W(rs_p2_le(dst, v->id));
        W(rs_p1_add128(dst, v->ctrl));
        W(rs_p2_le(dst, v->item));
        W(rs_p2_le(dst, v->z));
        W(rs_p2_le(dst, v->x));
        W(rs_p4_alt2(dst, v->interface_id));
        return rs_p2_le_add128(dst, v->component);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_magic_on_ground_decode(int revision, rs_buf *src, rs_action_magic_on_ground *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision != 317 && revision != 377 && revision != 742 && revision != 910) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->z = rs_g2_le(src);
        v->id = rs_g2(src);
        v->x = rs_g2_le(src);
        v->component = rs_g2_add128(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->id = rs_g2_le(src);
        v->z = rs_g2(src);
        v->component = rs_g2_le(src);
        v->x = rs_g2_le_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->z = rs_g2_le_add128(src);
        v->interface_id = rs_g4(src);
        v->component = rs_g2_add128(src);
        v->id = rs_g2_le_add128(src);
        v->item = rs_g2_add128(src);
        v->x = rs_g2(src);
        v->ctrl = rs_g1_sub128(src);
        return RS_OK;
    }
    v->id = rs_g2_le(src);
    v->ctrl = rs_g1_add128(src);
    v->item = rs_g2_le(src);
    v->z = rs_g2_le(src);
    v->x = rs_g2_le(src);
    v->interface_id = rs_g4_alt2(src);
    v->component = rs_g2_le_add128(src);
    return RS_OK;
}

/* ---- players ---- */

rs_err rs_action_trade_encode(int revision, rs_buf *dst, const rs_action_trade *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        return rs_p2_le(dst, v->index);
    }
    if (revision == 742) {
        W(rs_p2_le_add128(dst, v->index));
        return rs_p1(dst, v->ctrl);
    }
    if (revision == 910) {
        W(rs_p2(dst, v->index));
        return rs_p1_add128(dst, v->ctrl);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_trade_decode(int revision, rs_buf *src, rs_action_trade *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->index = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->index = rs_g2_le_add128(src);
        v->ctrl = rs_g1(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->index = rs_g2(src);
        v->ctrl = rs_g1_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_follow_encode(int revision, rs_buf *dst, const rs_action_follow *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317 || revision == 377) {
        return rs_p2_le(dst, v->index);
    }
    if (revision == 742) {
        W(rs_p2_le_add128(dst, v->index));
        return rs_p1(dst, v->ctrl);
    }
    if (revision == 910) {
        W(rs_p2(dst, v->index));
        return rs_p1_add128(dst, v->ctrl);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_follow_decode(int revision, rs_buf *src, rs_action_follow *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317 || revision == 377) {
        v->index = rs_g2_le(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->index = rs_g2_le_add128(src);
        v->ctrl = rs_g1(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->index = rs_g2(src);
        v->ctrl = rs_g1_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_attack_player_encode(int revision, rs_buf *dst, const rs_action_attack_player *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317) {
        return rs_p2(dst, v->index);
    }
    if (revision == 377) {
        return rs_p2_le_add128(dst, v->index);
    }
    if (revision == 742) {
        W(rs_p2_le_add128(dst, v->index));
        return rs_p1(dst, v->ctrl);
    }
    if (revision == 910) {
        W(rs_p2(dst, v->index));
        return rs_p1_add128(dst, v->ctrl);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_attack_player_decode(int revision, rs_buf *src, rs_action_attack_player *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317) {
        v->index = rs_g2(src);
        return RS_OK;
    }
    if (revision == 377) {
        v->index = rs_g2_le_add128(src);
        return RS_OK;
    }
    if (revision == 742) {
        v->index = rs_g2_le_add128(src);
        v->ctrl = rs_g1(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->index = rs_g2(src);
        v->ctrl = rs_g1_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

/* ---- dialogue, resume, social, close ---- */

rs_err rs_action_dialogue_encode(int revision, rs_buf *dst, const rs_action_dialogue *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317 || revision == 254 || revision == 377) {
        return rs_p2(dst, v->interface_id);
    }
    if (revision == 910) {
        W(rs_p4_alt3(dst, v->interface_id));
        return rs_p2_add128(dst, v->child);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_dialogue_decode(int revision, rs_buf *src, rs_action_dialogue *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317 || revision == 254 || revision == 377) {
        v->interface_id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->interface_id = rs_g4_alt3(src);
        v->child = rs_g2_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_enter_amount_encode(int revision, rs_buf *dst, const rs_action_enter_amount *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317 || revision == 254 || revision == 377 || revision == 742 || revision == 910) {
        return rs_p4(dst, v->amount);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_enter_amount_decode(int revision, rs_buf *src, rs_action_enter_amount *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision == 317 || revision == 254 || revision == 377 || revision == 742 || revision == 910) {
        memset(v, 0, sizeof(*v));
        v->amount = rs_g4(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_enter_name_encode(int revision, rs_buf *dst, const rs_action_enter_name *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 317 || revision == 377) {
        return put_name37(dst, v->name37);
    }
    if (revision == 742 || revision == 910) {
        /* p1(len+1) is the frame length. Payload is the NUL string. */
        return put_pjstr(dst, v->name);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_enter_name_decode(int revision, rs_buf *src, rs_action_enter_name *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 317 || revision == 377) {
        return get_name37(src, &v->name37);
    }
    if (revision == 742 || revision == 910) {
        return get_pjstr(src, v->name);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_add_friend_encode(int revision, rs_buf *dst, const rs_action_add_friend *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (early_name(revision)) {
        return put_name37(dst, v->name37);
    }
    if (late_name(revision)) {
        return put_pjstr(dst, v->name);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_add_friend_decode(int revision, rs_buf *src, rs_action_add_friend *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (early_name(revision)) {
        return get_name37(src, &v->name37);
    }
    if (late_name(revision)) {
        return get_pjstr(src, v->name);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_del_friend_encode(int revision, rs_buf *dst, const rs_action_del_friend *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (early_name(revision)) {
        return put_name37(dst, v->name37);
    }
    if (late_name(revision)) {
        return put_pjstr(dst, v->name);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_del_friend_decode(int revision, rs_buf *src, rs_action_del_friend *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (early_name(revision)) {
        return get_name37(src, &v->name37);
    }
    if (late_name(revision)) {
        return get_pjstr(src, v->name);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_add_ignore_encode(int revision, rs_buf *dst, const rs_action_add_ignore *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (early_name(revision)) {
        return put_name37(dst, v->name37);
    }
    if (late_name(revision)) {
        W(put_pjstr(dst, v->name));
        return rs_p1(dst, v->temporary ? 1 : 0);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_add_ignore_decode(int revision, rs_buf *src, rs_action_add_ignore *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (early_name(revision)) {
        return get_name37(src, &v->name37);
    }
    if (late_name(revision)) {
        e = get_pjstr(src, v->name);
        if (e) {
            return e;
        }
        v->temporary = rs_g1(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_del_ignore_encode(int revision, rs_buf *dst, const rs_action_del_ignore *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (early_name(revision)) {
        return put_name37(dst, v->name37);
    }
    if (late_name(revision)) {
        return put_pjstr(dst, v->name);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_del_ignore_decode(int revision, rs_buf *src, rs_action_del_ignore *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (early_name(revision)) {
        return get_name37(src, &v->name37);
    }
    if (late_name(revision)) {
        return get_pjstr(src, v->name);
    }
    return RS_ERR_UNSUPPORTED;
}

static int close_rev(int revision) {
    return revision == 317 || revision == 254 || revision == 377 || revision == 742 || revision == 910;
}

rs_err rs_action_close_encode(int revision, rs_buf *dst, const rs_action_close *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (!close_rev(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    return RS_OK;
}

rs_err rs_action_close_decode(int revision, rs_buf *src, rs_action_close *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (!close_rev(revision)) {
        return RS_ERR_UNSUPPORTED;
    }
    memset(v, 0, sizeof(*v));
    return RS_OK;
}

/* ---- script / varbit (server to client) ---- */

static rs_err script_put_args(rs_buf *dst, const rs_action_script *v) {
    int i;
    int n = (int)strlen(v->types);
    if (n != v->nargs || n > RS_ACTION_SCRIPT_ARGS) {
        return RS_ERR_RANGE;
    }
    W(rs_pjstrnull(dst, v->types));
    for (i = n - 1; i >= 0; i--) {
        if (v->types[i] == 's') {
            W(rs_pjstrnull(dst, v->arg[i].s));
        } else {
            W(rs_p4(dst, v->arg[i].i));
        }
    }
    return rs_p4(dst, v->id);
}

static rs_err script_get_args(rs_buf *src, rs_action_script *v, int leading_component) {
    int i;
    rs_err e;
    memset(v, 0, sizeof(*v));
    if (leading_component) {
        v->component = rs_g2(src);
    }
    e = rs_gjstrnull(src, v->types, sizeof(v->types));
    if (e) {
        return e;
    }
    v->nargs = (int)strlen(v->types);
    if (v->nargs > RS_ACTION_SCRIPT_ARGS) {
        return RS_ERR_RANGE;
    }
    for (i = v->nargs - 1; i >= 0; i--) {
        if (v->types[i] == 's') {
            e = rs_gjstrnull(src, v->arg[i].s, RS_ACTION_NAME);
            if (e) {
                return e;
            }
        } else {
            v->arg[i].i = rs_g4s(src);
        }
    }
    v->id = rs_g4s(src);
    return RS_OK;
}

rs_err rs_action_script_encode(int revision, rs_buf *dst, const rs_action_script *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 530) {
        W(rs_p2(dst, v->component));
        return script_put_args(dst, v);
    }
    if (revision == 742 || revision == 910) {
        return script_put_args(dst, v);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_script_decode(int revision, rs_buf *src, rs_action_script *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    if (revision == 530) {
        return script_get_args(src, v, 1);
    }
    if (revision == 742 || revision == 910) {
        return script_get_args(src, v, 0);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_varbit_small_encode(int revision, rs_buf *dst, const rs_action_varbit *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 742) {
        W(rs_p2_le_add128(dst, v->id));
        return rs_p1_neg(dst, v->value);
    }
    if (revision == 910) {
        W(rs_p1_sub128(dst, v->value));
        return rs_p2_add128(dst, v->id);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_varbit_small_decode(int revision, rs_buf *src, rs_action_varbit *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 742) {
        v->id = rs_g2_le_add128(src);
        v->value = rs_g1_neg(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->value = rs_g1_sub128(src);
        v->id = rs_g2_add128(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_varbit_large_encode(int revision, rs_buf *dst, const rs_action_varbit *v) {
    rs_err e = args2(dst, v);
    if (e) {
        return e;
    }
    if (revision == 742) {
        W(rs_ip4(dst, v->value));
        return rs_p2(dst, v->id);
    }
    if (revision == 910) {
        W(rs_p2_add128(dst, v->id));
        return rs_ip4(dst, v->value);
    }
    return RS_ERR_UNSUPPORTED;
}

rs_err rs_action_varbit_large_decode(int revision, rs_buf *src, rs_action_varbit *v) {
    rs_err e = args2(src, v);
    if (e) {
        return e;
    }
    memset(v, 0, sizeof(*v));
    if (revision == 742) {
        v->value = (int)(int32_t)rs_ig4(src);
        v->id = rs_g2(src);
        return RS_OK;
    }
    if (revision == 910) {
        v->id = rs_g2_add128(src);
        v->value = (int)(int32_t)rs_ig4(src);
        return RS_OK;
    }
    return RS_ERR_UNSUPPORTED;
}
