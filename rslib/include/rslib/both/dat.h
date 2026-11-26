#ifndef RSLIB_DAT_H
#define RSLIB_DAT_H

#include "rslib/both/buf.h"
#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 317 sprite and font buffers are the bytes the constructor reads after the
 * dat u16 has seeked index.dat. Sprite: sheet, palette, crop, pixel order,
 * then indices. Font: the 4 skipped index bytes, the glyph-count byte, the
 * 3*(count-1) skip, then each glyph header followed by its pixels.
 * PixFont's quill flag is not in the file; space advance copies glyph 105.
 * 910 varbit DEBUGNAME is a known key and the switch does not read a string.
 */

typedef struct rs_dat_param {
    int key;
    int is_string;
    int int_value;
    char *str_value;
} rs_dat_param;

typedef struct rs_varbit {
    int basevar;
    int startbit;
    int endbit;
    int op2;
    int op3;
    int op4;
    int domain;
    int domain_set;
    char *name;
} rs_varbit;

typedef struct rs_enum_entry {
    int key;
    int is_string;
    int int_value;
    char *str_value;
} rs_enum_entry;

typedef struct rs_enum {
    int input_char;
    int input_char_set;
    int output_char;
    int output_char_set;
    int input_type;
    int input_type_set;
    int output_type;
    int output_type_set;
    char *default_str;
    int default_int;
    int count;
    int capacity;
    int sparse;
    rs_enum_entry *entries;
} rs_enum;

typedef struct rs_seq {
    int frame_count;
    int *frames;
    int *iframes;
    int *delays;
    int *walkmerge;
    int walkmerge_count;
    uint8_t *walk_flags;
    int replayoff;
    int stretches;
    int priority;
    int righthand;
    int lefthand;
    int replaycount;
    int preanim_move;
    int postanim_move;
    int replacemode;
    int op12;
    int *frames2;
    int frames2_count;
    int **sounds;
    int *sound_len;
    int sound_count;
    int flag14;
    int flag15;
    int flag18;
    int *sound_bytes;
    int *sound_a;
    int *sound_b;
    int op22;
    int length;
    int tint;
    int tint2;
    rs_dat_param *params;
    int param_count;
} rs_seq;

typedef struct rs_spot_type {
    int model;
    int anim;
    int resizeh;
    int resizev;
    int angle;
    int ambient;
    int contrast;
    int hillskew_mode;
    int hillskew_value;
    int tboolean;
    int recol_count;
    int *recol_s;
    int *recol_d;
    int retex_count;
    int *retex_s;
    int *retex_d;
    int8_t *recol_idx;
    int recol_idx_len;
    int8_t *retex_idx;
    int retex_idx_len;
} rs_spot_type;

typedef struct rs_identkit {
    int type;
    int disable;
    int model_count;
    int *models;
    int recol_count;
    int *recol_s;
    int *recol_d;
    int retex_count;
    int *retex_s;
    int *retex_d;
    int8_t *recol_idx;
    int recol_idx_len;
    int8_t *retex_idx;
    int retex_idx_len;
    int heads[5];
} rs_identkit;

typedef struct rs_sprite_frame {
    int offset_x;
    int offset_y;
    int width;
    int height;
    int crop_x;
    int crop_y;
    int sheet_w;
    int sheet_h;
    int flags;
    int pixel_count;
    int *pixels;
    int8_t *indices;
    int8_t *alpha;
} rs_sprite_frame;

typedef struct rs_sprite {
    int count;
    int full;
    int sheet_w;
    int sheet_h;
    int palette_count;
    int *palette;
    rs_sprite_frame *frames;
} rs_sprite;

typedef struct rs_font_glyph {
    int offset_x;
    int offset_y;
    int width;
    int height;
    int advance;
    int8_t *pixels;
} rs_font_glyph;

typedef struct rs_font {
    int height;
    int space_advance;
    int has_glyphs;
    rs_font_glyph glyph[256];
    uint8_t advance[256];
    int8_t *kerning;
    int ascent;
    int descent;
    int8_t top[256];
    int8_t bearing[256];
    int8_t left[256];
    int16_t box[256][4];
    int pad_a;
    int pad_b;
    int metric_a;
    int metric_b;
    int metric_c;
    int metric_d;
    int scale;
    int has_metrics;
} rs_font;

typedef struct rs_script_case {
    int key;
    int value;
} rs_script_case;

typedef struct rs_script_switch {
    int count;
    rs_script_case *cases;
} rs_script_switch;

typedef struct rs_script_op {
    int opcode;
    int int_operand;
    int int_set;
    int domain;
    int var_id;
    char *str_operand;
    uint64_t long_operand;
    int long_set;
} rs_script_op;

typedef struct rs_clientscript {
    char *name;
    int name_set;
    int int_local;
    int obj_local;
    int long_local;
    int int_arg;
    int obj_arg;
    int long_arg;
    int op_count;
    rs_script_op *ops;
    int switch_count;
    rs_script_switch *switches;
} rs_clientscript;

rs_err rs_varbit_decode(int revision, rs_buf *src, rs_varbit *out);
void rs_varbit_free(rs_varbit *out);

rs_err rs_enum_decode(int revision, rs_buf *src, rs_enum *out);
void rs_enum_free(rs_enum *out);

rs_err rs_seq_decode(int revision, rs_buf *src, rs_seq *out);
void rs_seq_free(rs_seq *out);

rs_err rs_spot_type_decode(int revision, rs_buf *src, rs_spot_type *out);
void rs_spot_type_free(rs_spot_type *out);

rs_err rs_identkit_decode(int revision, rs_buf *src, rs_identkit *out);
void rs_identkit_free(rs_identkit *out);

rs_err rs_sprite_decode(int revision, rs_buf *src, rs_sprite *out);
void rs_sprite_free(rs_sprite *out);

rs_err rs_font_decode(int revision, rs_buf *src, rs_font *out);
void rs_font_free(rs_font *out);

rs_err rs_clientscript_decode(int revision, rs_buf *src, rs_clientscript *out);
void rs_clientscript_free(rs_clientscript *out);

#ifdef __cplusplus
}
#endif

#endif
