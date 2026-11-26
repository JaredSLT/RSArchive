#ifndef RSLIB_ERA_BODY_H
#define RSLIB_ERA_BODY_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* S2C bodies copied from each client's reader. revision selects the layout.
   A revision whose reader was not followed returns RS_ERR_UNSUPPORTED.
   Zone records are the opcode byte plus the body. The zone-partial header
   is not written. */

enum { RS_ERA_TEXT = 256 };
/* Same cap as RS_INV_SLOTS. 317 bank widget 5382 is 8 by 38. */
enum { RS_ERA_INV_SLOTS = 304 };

typedef struct rs_era_skill {
    int id;
    int level;
    int xp;
} rs_era_skill;

typedef struct rs_era_varp {
    int id;
    int value;
} rs_era_varp;

typedef struct rs_era_inv_slot {
    int id;
    int amount;
} rs_era_inv_slot;

typedef struct rs_era_inventory {
    int component;
    int flags; /* 742/910 byte. 474 container-key u16. Else 0. 910 bit 1 unsupported. */
    int nslots;
    rs_era_inv_slot slots[RS_ERA_INV_SLOTS];
} rs_era_inventory;

typedef struct rs_era_inv_change {
    int slot;
    int id;
    int amount;
} rs_era_inv_change;

typedef struct rs_era_inv_update {
    int component;
    int flags;
    int nchanges;
    rs_era_inv_change changes[RS_ERA_INV_SLOTS];
} rs_era_inv_update;

typedef struct rs_era_if_text {
    int component;
    char text[RS_ERA_TEXT];
} rs_era_if_text;

typedef struct rs_era_if_open {
    int id;
} rs_era_if_open;

/* Early revisions use id alone. 742 and 910 IF_OPENSUB also carry parent,
   type, and four key ints in the order the client builds its int[]. */
typedef struct rs_era_if_side {
    int id;
    int parent;
    int type;
    int key[4];
} rs_era_if_side;

/* Early revisions use viewport and sidebar. 742 stores key[] in the order
   passed to the open. 910 stores key[] in wire order; the client array is
   {key[2], key[3], key[1], key[0]}. type is 742's pane type, or the 910
   byte the reader discards. */
typedef struct rs_era_if_top {
    int viewport;
    int sidebar;
    int type;
    int key[4];
} rs_era_if_top;

typedef struct rs_era_if_close {
    int component; /* 742 and 910 packed id. Unused when the body is empty. */
} rs_era_if_close;

typedef struct rs_era_loc {
    int x;
    int z;
    int id;
    int shape;
    int rot;
} rs_era_loc;

typedef struct rs_era_obj {
    int x;
    int z;
    int id;
    int amount;
} rs_era_obj;

rs_err rs_era_skill_encode(int revision, rs_buf *dst, const rs_era_skill *v);
rs_err rs_era_skill_decode(int revision, rs_buf *src, rs_era_skill *v);

rs_err rs_era_varp_small_encode(int revision, rs_buf *dst, const rs_era_varp *v);
rs_err rs_era_varp_small_decode(int revision, rs_buf *src, rs_era_varp *v);
rs_err rs_era_varp_large_encode(int revision, rs_buf *dst, const rs_era_varp *v);
rs_err rs_era_varp_large_decode(int revision, rs_buf *src, rs_era_varp *v);

rs_err rs_era_inventory_encode(int revision, rs_buf *dst, const rs_era_inventory *v);
rs_err rs_era_inventory_decode(int revision, rs_buf *src, rs_era_inventory *v);
rs_err rs_era_inv_update_encode(int revision, rs_buf *dst, const rs_era_inv_update *v);
rs_err rs_era_inv_update_decode(int revision, rs_buf *src, rs_era_inv_update *v);

rs_err rs_era_if_text_encode(int revision, rs_buf *dst, const rs_era_if_text *v);
rs_err rs_era_if_text_decode(int revision, rs_buf *src, rs_era_if_text *v);
rs_err rs_era_if_open_encode(int revision, rs_buf *dst, const rs_era_if_open *v);
rs_err rs_era_if_open_decode(int revision, rs_buf *src, rs_era_if_open *v);
rs_err rs_era_if_side_encode(int revision, rs_buf *dst, const rs_era_if_side *v);
rs_err rs_era_if_side_decode(int revision, rs_buf *src, rs_era_if_side *v);
rs_err rs_era_if_top_encode(int revision, rs_buf *dst, const rs_era_if_top *v);
rs_err rs_era_if_top_decode(int revision, rs_buf *src, rs_era_if_top *v);
rs_err rs_era_if_close_encode(int revision, rs_buf *dst, const rs_era_if_close *v);
rs_err rs_era_if_close_decode(int revision, rs_buf *src, rs_era_if_close *v);

rs_err rs_era_loc_add_encode(int revision, rs_buf *dst, const rs_era_loc *v);
rs_err rs_era_loc_add_decode(int revision, rs_buf *src, rs_era_loc *v);
rs_err rs_era_loc_del_encode(int revision, rs_buf *dst, const rs_era_loc *v);
rs_err rs_era_loc_del_decode(int revision, rs_buf *src, rs_era_loc *v);
rs_err rs_era_obj_add_encode(int revision, rs_buf *dst, const rs_era_obj *v);
rs_err rs_era_obj_add_decode(int revision, rs_buf *src, rs_era_obj *v);
rs_err rs_era_obj_del_encode(int revision, rs_buf *dst, const rs_era_obj *v);
rs_err rs_era_obj_del_decode(int revision, rs_buf *src, rs_era_obj *v);

#ifdef __cplusplus
}
#endif

#endif
