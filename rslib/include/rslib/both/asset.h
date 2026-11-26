#ifndef RSLIB_ASSET_H
#define RSLIB_ASSET_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 18-byte model tail (317 Model.unpack + Model(int,int)).
 * 254 Model.unpack / Model(int,boolean) and 377 Model.opModel3 /
 * Model(int,int) use the same bytes. 474 C88f97d27.calla50c1260 and
 * 530 QuickChatPhraseType.opQuickChatPhraseType45 do too, unless the
 * file ends FF FF (opkind2065 / opQuickChatPhraseType55), which does
 * not fit this struct. Any other revision returns RS_ERR_UNSUPPORTED.
 * These are the inflated bytes; idx1 stores them gzip-wrapped.
 *
 * face_colour is the face HSL. When face_info != NULL and
 * (face_info[i] & 3) is 2 or 3, the client uses face_colour[i] as the
 * texture id and face_info[i] >> 2 as the index into textured_a/b/c.
 * There is no separate texture-id array and no palette in the file.
 * When face_priority is NULL, every face uses `priority`.
 */
typedef struct rs_model {
    int vertex_count;
    int face_count;
    int textured_count;
    int priority;
    int *vertex_x;
    int *vertex_y;
    int *vertex_z;
    int *vertex_label;
    int *face_a;
    int *face_b;
    int *face_c;
    int *face_colour;
    int *face_info;
    int *face_priority;
    int *face_alpha;
    int *face_label;
    int *textured_a;
    int *textured_b;
    int *textured_c;
} rs_model;

rs_err rs_model_decode(int revision, const uint8_t *data, size_t len, rs_model **out);
void rs_model_free(rs_model *m);
int rs_model_vertex_count(const rs_model *m);
int rs_model_face_count(const rs_model *m);

enum {
    RS_COMPONENT_ACTIONS = 5,
    RS_COMPONENT_INV_GFX = 20
};

/*
 * One component, the fields Component.unpack sets from the data file.
 * Strings are owned by the component (NULL if that reader branch did not
 * set them, or an inventory/action string was empty). Sprite fields keep
 * the "name,index" text; pixels are not in this file. font is -1 when the
 * type has no font id. layer is -1 until an archive record assigns a parent.
 * script_code is the concatenation of each script; script i starts after
 * the sum of script_len[0..i).
 */
typedef struct rs_component {
    int id;
    int layer;
    int type;
    int button_type;
    int client_code;
    int width;
    int height;
    int alpha;
    int overlayer;
    int scroll;
    int hide;
    int fill;
    int center;
    int shadowed;
    int font;
    int colour;
    int active_colour;
    int over_colour;
    int active_over_colour;
    int type1_word;
    int type1_flag;
    int draggable;
    int interactable;
    int usable;
    int swappable;
    int margin_x;
    int margin_y;
    int model_type;
    int model;
    int active_model_type;
    int active_model;
    int anim;
    int active_anim;
    int zoom;
    int xan;
    int yan;
    int target_mask;
    char *text;
    char *active_text;
    char *option;
    char *target_verb;
    char *target_text;
    char *graphic;
    char *active_graphic;
    char *action[RS_COMPONENT_ACTIONS];
    int inv_x[RS_COMPONENT_INV_GFX];
    int inv_y[RS_COMPONENT_INV_GFX];
    int inv_sprite[RS_COMPONENT_INV_GFX];
    char *inv_graphic[RS_COMPONENT_INV_GFX];
    int script_n;
    int *script_comparator;
    int *script_operand;
    int script_count;
    int *script_len;
    int *script_code;
    int child_count;
    int *child_id;
    int *child_x;
    int *child_y;
} rs_component;

/* Body only, positioned at the type byte. Does not read the archive id. */
rs_err rs_component_decode(int revision, rs_buf *src, rs_component *out);
/* Frees strings and child/script arrays. Does not free `c` itself. */
void rs_component_free(rs_component *c);

/*
 * Interface jag member "data" (already unpacked, not the .jag).
 * 317 and 254 Component.unpack share this layout: u16 index length,
 * then records until the end of the buffer. A record is u16 id, or
 * 65535, u16 layer, u16 id, then the component body. Layer sticks until
 * the next 65535 marker. Ids must be < the u16 index length.
 * *count is how many components were present, not the index length.
 * On error the caller buffer is cleared and *count is 0.
 * 377 adds type 8 and one file per interface, so it stays unsupported.
 */
rs_err rs_component_archive_decode(int revision, const uint8_t *data, size_t len,
                                   rs_component *out, int cap, int *count);

rs_err rs_interface_decode(int revision, const uint8_t *data, size_t len,
                           rs_component **out, int *count);
void rs_interface_free(rs_component *list, int count);

#ifdef __cplusplus
}
#endif

#endif
