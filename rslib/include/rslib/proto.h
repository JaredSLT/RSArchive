#ifndef RSLIB_PROTO_H
#define RSLIB_PROTO_H

#include "rslib/both/buf.h"
#include "rslib/both/build.h"
#include "rslib/both/crypto.h"
#include "rslib/both/err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Build-neutral protocol. Pick a build (and optionally a role) once; every
 * handshake, login, game frame, and update call goes through the same
 * functions. RSC, RS2 on-demand, and JS5 plug in behind rs_proto_ops.
 */

extern const int8_t rs_sizes_s2c_254[256];
extern const int8_t rs_sizes_c2s_254[256];
extern const int8_t rs_sizes_s2c_317[256];
extern const int8_t rs_sizes_c2s_317[256];
extern const int8_t rs_sizes_s2c_377[256];
extern const int8_t rs_sizes_c2s_377[256];
extern const int8_t rs_sizes_s2c_468[256];
extern const int8_t rs_sizes_c2s_468[256];
extern const int8_t rs_sizes_s2c_474[256];
extern const int8_t rs_sizes_c2s_474[256];
extern const int8_t rs_sizes_s2c_508[256];
extern const int8_t rs_sizes_c2s_508[256];
extern const int8_t rs_sizes_s2c_530[256];
extern const int8_t rs_sizes_c2s_530[256];
extern const int8_t rs_sizes_s2c_614[256];
extern const int8_t rs_sizes_c2s_614[256];
extern const int8_t rs_sizes_s2c_667[256];
extern const int8_t rs_sizes_c2s_667[256];
extern const int8_t rs_sizes_s2c_282[256];
extern const int8_t rs_sizes_s2c_350[256];
extern const int8_t rs_sizes_s2c_443[256];
extern const int8_t rs_sizes_s2c_481[256];
extern const int8_t rs_sizes_s2c_525[256];
extern const int8_t rs_sizes_c2s_742[256];
extern const int8_t rs_sizes_s2c_742[256];
extern const int8_t rs_sizes_c2s_745[256];
extern const int8_t rs_sizes_s2c_745[256];
extern const int8_t rs_sizes_c2s_790[256];
extern const int8_t rs_sizes_s2c_790[256];
extern const int8_t rs_sizes_c2s_876[256];
extern const int8_t rs_sizes_s2c_876[256];
extern const int8_t rs_sizes_c2s_887[256];
extern const int8_t rs_sizes_s2c_887[256];
extern const int8_t rs_sizes_c2s_902[256];
extern const int8_t rs_sizes_s2c_902[256];
extern const int8_t rs_sizes_c2s_910[256];
extern const int8_t rs_sizes_s2c_910[256];

typedef enum rs_role {
    RS_ROLE_SERVER = 0,
    RS_ROLE_CLIENT = 1
} rs_role;

typedef enum rs_link {
    RS_LINK_UNKNOWN = 0,
    RS_LINK_LOGIN = 1,
    RS_LINK_ONDEMAND = 2,
    RS_LINK_JS5 = 3,
    RS_LINK_GAME = 4
} rs_link;

typedef struct rs_handshake {
    rs_link link;
    int opcode;
    int name_hash;     /* login 14 payload */
    int revision;      /* JS5 or later login */
    uint32_t js5_crc;  /* optional world-list / JS5 checksum */
    size_t consumed;
} rs_handshake;

typedef struct rs_update_req {
    int opcode;   /* 0 prefetch, 1 urgent, or JS5 control */
    int archive;
    int id;       /* on-demand file or JS5 group */
    int priority; /* on-demand priority byte */
    int key;      /* JS5 opcode 4 xor byte */
} rs_update_req;

enum {
    RS_LOGIN_OK = 2,
    RS_LOGIN_RETRY = 1,
    RS_LOGIN_INVALID = 3,
    RS_LOGIN_BANNED = 4,
    RS_LOGIN_ALREADY = 5,
    RS_LOGIN_UPDATED = 6,
    RS_LOGIN_FULL = 7,
    RS_LOGIN_LOGINSERVER = 8,
    RS_LOGIN_TOO_MANY = 9,
    RS_LOGIN_BAD_SESSION = 10,
    RS_LOGIN_REJECTED = 11,
    RS_LOGIN_MEMBERS = 12,
    RS_LOGIN_INCOMPLETE = 13,
    RS_LOGIN_SERVER_UPDATE = 14,
    RS_LOGIN_RECONNECT_OK = 15,
    RS_LOGIN_TOO_MANY_LOGINS = 16,
    RS_LOGIN_IN_MEMBERS = 17,
    RS_LOGIN_LOCKED = 18,
    RS_LOGIN_CLOSED_BETA = 19,
    RS_LOGIN_INVALID_LOGINSERVER = 20
};

enum { RS_LOGIN_CRC_MAX = 64 };

struct rs_login_ext;

typedef struct rs_login_request {
    int reconnect;
    int revision;
    int low_memory;
    int ncrc;
    uint32_t archive_crc[RS_LOGIN_CRC_MAX];
    uint32_t isaac_seed[4];
    int uid;
    uint64_t username37;
    char username[13];
    char password[33];
    int window_mode;
    int canvas_w;
    int canvas_h;
    int affiliate;
    int verify_id;
    char settings[128];
    int lobby; /* 1 = RS3 opcode 19 lobby login; 0 = game login */
    struct rs_login_ext *ext; /* optional 910 tail; not owned. NULL = zeros */
} rs_login_request;

typedef struct rs_session rs_session;
typedef struct rs_proto_ops rs_proto_ops;

struct rs_session {
    const rs_build_info *build;
    const rs_proto_ops *ops;
    rs_role role;
    rs_link link;
    rs_isaac isaac_in;
    rs_isaac isaac_out;
    int isaac_ready;
    int encrypt_out; /* encode outgoing opcode with isaac_out */
    int js5_encrypt;
    uint8_t js5_key;
    int player_index;
    uint64_t server_seed;
};

struct rs_proto_ops {
    const char *name;
    rs_err (*handshake_parse)(const rs_build_info *hint, const uint8_t *data, size_t len,
                              rs_handshake *out);
    rs_err (*handshake_write)(rs_session *s, rs_buf *dst, rs_link link, int name_hash);
    rs_err (*handshake_reply)(rs_session *s, rs_buf *dst, const rs_handshake *hs,
                              uint64_t server_seed);
    rs_err (*handshake_reply_parse)(rs_session *s, rs_buf *src, rs_link link, int *status,
                                    uint64_t *server_seed);
    rs_err (*login_parse)(rs_session *s, rs_buf *src, const rs_rsa_key *rsa, rs_login_request *out);
    rs_err (*login_write)(rs_session *s, rs_buf *dst, const rs_login_request *req,
                          const rs_rsa_key *rsa);
    rs_err (*login_success)(rs_session *s, rs_buf *dst, int rights, int flagged);
    rs_err (*login_success_parse)(rs_session *s, rs_buf *src, int *status, int *rights,
                                  int *flagged);
    rs_err (*read_game)(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload);
    rs_err (*write_game)(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload, size_t n);
    rs_err (*update_parse)(rs_session *s, const uint8_t *data, size_t len, rs_update_req *out,
                           size_t *consumed);
    rs_err (*update_write)(rs_session *s, rs_buf *dst, const rs_update_req *req);
    rs_err (*update_reply)(rs_session *s, rs_buf *dst, const rs_update_req *req, const uint8_t *data,
                           size_t len);
};

const rs_proto_ops *rs_proto_for(const rs_build_info *build);
const rs_proto_ops *rs_proto_rsc(void);
const rs_proto_ops *rs_proto_rs2(void);
const rs_proto_ops *rs_proto_js5(void);
/* RS3 lobby/game login. Handshake, frames, and JS5 update match rs_proto_js5. */
const rs_proto_ops *rs_proto_js5_lobby(void);
extern const rs_proto_ops rs_ops_js5_lobby;

void rs_session_init(rs_session *s, const rs_build_info *build);
void rs_session_init_role(rs_session *s, const rs_build_info *build, rs_role role);
rs_err rs_session_set_build(rs_session *s, const rs_build_info *build);
rs_err rs_session_set_build_revision(rs_session *s, int revision);
rs_err rs_session_set_build_id(rs_session *s, const char *id_or_rev);
rs_err rs_session_set_role(rs_session *s, rs_role role);

void rs_session_seed_isaac(rs_session *s, const uint32_t client_keys[4]);

int rs_session_size_c2s(const rs_session *s, int opcode);
int rs_session_size_s2c(const rs_session *s, int opcode);
int rs_session_size_in(const rs_session *s, int opcode);
int rs_session_size_out(const rs_session *s, int opcode);

int rs_session_decode_opcode(rs_session *s, int raw);
int rs_session_encode_opcode(rs_session *s, int opcode);

/*
 * Role-aware game frames. A server reads c2s and writes s2c; a client does
 * the opposite. Same call for every build.
 */
rs_err rs_session_read(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload);
rs_err rs_session_write(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload, size_t n);

/* Server-oriented names kept for existing call sites. */
rs_err rs_session_read_c2s(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload);
rs_err rs_session_write_s2c(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload,
                            size_t n);

rs_err rs_handshake_parse(const rs_build_info *hint, const uint8_t *data, size_t len,
                          rs_handshake *out);

rs_err rs_session_handshake_parse(rs_session *s, const uint8_t *data, size_t len, rs_handshake *out);
rs_err rs_session_handshake_write(rs_session *s, rs_buf *dst, rs_link link, int name_hash);
rs_err rs_session_handshake_reply(rs_session *s, rs_buf *dst, const rs_handshake *hs,
                                  uint64_t server_seed);
rs_err rs_session_handshake_reply_parse(rs_session *s, rs_buf *src, rs_link link, int *status,
                                        uint64_t *server_seed);

rs_err rs_session_login_parse(rs_session *s, rs_buf *src, const rs_rsa_key *rsa,
                              rs_login_request *out);
rs_err rs_session_login_write(rs_session *s, rs_buf *dst, const rs_login_request *req,
                              const rs_rsa_key *rsa);
rs_err rs_session_login_success(rs_session *s, rs_buf *dst, int rights, int flagged);
rs_err rs_session_login_success_parse(rs_session *s, rs_buf *src, int *status, int *rights,
                                      int *flagged);

rs_err rs_session_update_parse(rs_session *s, const uint8_t *data, size_t len, rs_update_req *out,
                               size_t *consumed);
rs_err rs_session_update_write(rs_session *s, rs_buf *dst, const rs_update_req *req);
rs_err rs_session_update_reply(rs_session *s, rs_buf *dst, const rs_update_req *req,
                               const uint8_t *data, size_t len);

/* After opcode 14 + name hash, server writes 8 dummy bytes then status 0. */
rs_err rs_login_write_handshake_ok(rs_buf *dst, uint64_t server_seed);

rs_err rs_login_parse(const rs_build_info *build, rs_buf *src, const rs_rsa_key *rsa,
                      rs_login_request *out);

/* CRC count the login writer emits. k_builds is unchanged. 530 and 742 are
 * the counts read from those clients' login writers. */
int rs_login_crc_count(const rs_build_info *b);

rs_err rs_login_write_success(rs_buf *dst, int rights, int flagged);
/* 474+: extra player index. `player_index` is ignored on RS2 (254–377). */
rs_err rs_login_write_success_ex(rs_buf *dst, const rs_build_info *build, int rights, int flagged,
                                 int player_index);

typedef struct rs_ondemand_req {
    int archive;
    int file;
    int priority; /* 0 prefetch, 1 extra, 2 urgent */
} rs_ondemand_req;

rs_err rs_ondemand_parse(const uint8_t *data, size_t len, rs_ondemand_req *out);
rs_err rs_ondemand_write_chunk(rs_buf *dst, int archive, int file, const uint8_t *data, size_t size,
                               int chunk);
rs_err rs_ondemand_write_reject(rs_buf *dst, int archive, int file);

typedef enum rs_js5_op {
    RS_JS5_PREFETCH = 0,
    RS_JS5_URGENT = 1,
    RS_JS5_LOGGED_IN = 2,
    RS_JS5_LOGGED_OUT = 3,
    RS_JS5_ENCRYPT = 4,
    RS_JS5_CONNECTED = 6,
    RS_JS5_DISCONNECT = 7
} rs_js5_op;

typedef struct rs_js5_req {
    int opcode;
    int archive;
    int group;
} rs_js5_req;

rs_err rs_js5_write_status(rs_buf *dst, int status);
rs_err rs_js5_parse_request(const rs_build_info *build, const uint8_t *data, size_t len,
                            rs_js5_req *out, size_t *consumed);
rs_err rs_js5_write_group(const rs_build_info *build, rs_buf *dst, int archive, int group,
                          const uint8_t *container, size_t len, int urgent);

/* One JS5 group reply, inverse of rs_js5_write_group's framer.
 * revision < 508 is an 8-byte header; 508+ is 10 bytes. A 0xFF is stripped
 * before every data byte once 512 data bytes have been taken in the group.
 * st->encrypt XORs each new wire byte with st->key (0xFF bytes included).
 * The callback container is only valid for that call. Partial input stays
 * in st. Zero-initialise st; rs_js5_reply_free releases st->buf. */
typedef void (*rs_js5_reply_fn)(void *user, int archive, int group, const uint8_t *container,
                                size_t n);

typedef struct rs_js5_reply {
    uint8_t *buf;
    size_t len;
    size_t cap;
    size_t block; /* data bytes since the last 0xFF, 0..512 */
    int have_hdr;
    size_t need; /* unframed header + payload for the current group */
    int encrypt;
    uint8_t key;
} rs_js5_reply;

rs_err rs_js5_reply_feed(rs_js5_reply *st, int revision, const uint8_t *data, size_t len,
                         rs_js5_reply_fn fn, void *user);
void rs_js5_reply_free(rs_js5_reply *st);

#ifdef __cplusplus
}
#endif

#endif
