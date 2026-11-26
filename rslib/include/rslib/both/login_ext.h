#ifndef RSLIB_LOGIN_EXT_H
#define RSLIB_LOGIN_EXT_H

#include "rslib/both/err.h"
#include "rslib/proto.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 910 ClientOptions.createPreferencesBlock. Version byte 38 and the two
 * hardcoded 0 bytes are written by the encoder, not stored here. */
typedef struct rs_login_prefs {
    int anim_detail;
    int anti_aliasing;
    int unused;
    int bloom;
    int brightness;
    int build_area;
    int draw_distance;
    int flickering;
    int fog;
    int ground_blending;
    int ground_decoration;
    int idle_animations;
    int lighting_detail;
    int scenery_shadows;
    int shadow_quality;
    int orthographic;
    int particles;
    int remove_roofs;
    int screen_size;
    int skyboxes;
    int character_shadows;
    int textures;
    int toolkit;
    int water_detail;
    int window_mode;
    int unused1;
    int unused2;
    int unused3;
    int unused4;
    int unused5;
    int unused6;
    int unused7;
    int unused8;
    int unused9; /* u16 */
    int unused10;
    int unused11;
    int unused12;
    int custom_cursors;
    int preset;
    int cpu_usage;
    int loading_screen;
    int safe_mode;
    int unknown7;
    int unused13;
    int console_key_press;
    int sound_volume;
    int background_sound;
    int speech_volume;
    int unknown_volume1;
    int unknown_volume2;
    int stereo;
} rs_login_prefs;

/* 910 HardwarePlatform.createHardwareBlock. Strings are pjstr2. */
typedef struct rs_login_hw {
    int operating_system;
    int os_arch64;
    int os_version;
    int java_vendor;
    int java_major;
    int java_minor;
    int java_patch;
    int unused;
    int max_memory;
    int processors;
    int cpu_ram; /* u24 */
    int cpu_speed;
    char gpu[41];
    char unused1[41];
    char directx[11];
    char unused2[11];
    int dx_month;
    int dx_year;
    char cpu_vendor[128];
    char cpu_desc[128];
    int cpu_processors;
    int cpu_processors2;
    int cpu_unknown[3];
    int cpu_unknown1;
    char unused3[121];
} rs_login_hw;

/* Fields from 910 LoginManager step 84 that rs_login_request does not hold.
 * A zeroed ext matches socialKey == -1 (pjstr username) and empty prefs. */
typedef struct rs_login_ext {
    int social_key_set;
    uint64_t social_key;
    int modegame;
    int language;
    int antialias;
    uint8_t uid192[24];
    rs_login_prefs prefs;
    rs_login_hw hw;
    int user_flow1;
    int user_flow2;
    int tint;
    int tint3;
    int tint4;
    char string2[128];
    char additional_info[128]; /* empty = the null branch, no pjstr */
    int javascript;
    int chrome;
    int client_type; /* written as client_type & 1 */
    char gamepack[128];
    int world_mismatch;
    int lobby_node;
} rs_login_ext;

/* CRC count, success_extra, and login_user37 counted from that revision's
 * client login method. RS_ERR_NOT_FOUND if this tree has no source or the
 * count is not a visible literal. success_extra is the plaintext after the
 * status byte and before the ISAAC opcode, minus rights and flagged.
 * 742 and 910 login-ok bodies are length-prefixed variable records, so
 * success_extra is 0 (no fixed trailer). */
rs_err rs_login_facts(int revision, int *crc_count, int *success_extra, int *user37);

#ifdef __cplusplus
}
#endif

#endif
