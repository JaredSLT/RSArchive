#ifndef RSLIB_PROTO_OPS_H
#define RSLIB_PROTO_OPS_H

#include "internal.h"

extern const rs_proto_ops rs_ops_rsc;
extern const rs_proto_ops rs_ops_rs2;
extern const rs_proto_ops rs_ops_js5;

rs_err rs_frame_read(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload, int use_c2s);
rs_err rs_frame_write(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload, size_t n,
                      int use_s2c);

rs_err rs_rsc_read(rs_buf *src, int *opcode, rs_buf *payload);
rs_err rs_rsc_write(rs_buf *dst, int opcode, const uint8_t *payload, size_t n);

rs_err rs_rsa_block_parse(rs_buf *block, rs_login_request *out);
rs_err rs_rsa_block_parse_ex(rs_buf *block, rs_login_request *out, int user37);
rs_err rs_rs2_login_parse(const rs_build_info *build, rs_buf *src, const rs_rsa_key *rsa,
                          rs_login_request *out);
rs_err rs_rs2_login_write(const rs_build_info *build, rs_buf *dst, const rs_login_request *req,
                          const rs_rsa_key *rsa);


rs_err rs2_handshake_parse(const rs_build_info *hint, const uint8_t *data, size_t len, rs_handshake *out);
rs_err rs2_handshake_write(rs_session *s, rs_buf *dst, rs_link link, int name_hash);
rs_err rs2_handshake_reply(rs_session *s, rs_buf *dst, const rs_handshake *hs, uint64_t server_seed);
rs_err rs2_handshake_reply_parse(rs_session *s, rs_buf *src, rs_link link, int *status, uint64_t *server_seed);
rs_err rs2_login_parse(rs_session *s, rs_buf *src, const rs_rsa_key *rsa, rs_login_request *out);
rs_err rs2_login_write(rs_session *s, rs_buf *dst, const rs_login_request *req, const rs_rsa_key *rsa);
rs_err rs2_login_success(rs_session *s, rs_buf *dst, int rights, int flagged);
rs_err rs2_login_success_parse(rs_session *s, rs_buf *src, int *status, int *rights, int *flagged);
rs_err rs2_read_game(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload);
rs_err rs2_write_game(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload, size_t n);
rs_err rs2_update_parse(rs_session *s, const uint8_t *data, size_t len, rs_update_req *out, size_t *consumed);
rs_err rs2_update_write(rs_session *s, rs_buf *dst, const rs_update_req *req);
rs_err rs2_update_reply(rs_session *s, rs_buf *dst, const rs_update_req *req, const uint8_t *data, size_t len);

rs_err js5_handshake_parse(const rs_build_info *hint, const uint8_t *data, size_t len, rs_handshake *out);
rs_err js5_handshake_write(rs_session *s, rs_buf *dst, rs_link link, int name_hash);
rs_err js5_handshake_reply(rs_session *s, rs_buf *dst, const rs_handshake *hs, uint64_t server_seed);
rs_err js5_handshake_reply_parse(rs_session *s, rs_buf *src, rs_link link, int *status, uint64_t *server_seed);
rs_err js5_login_parse(rs_session *s, rs_buf *src, const rs_rsa_key *rsa, rs_login_request *out);
rs_err js5_login_write(rs_session *s, rs_buf *dst, const rs_login_request *req, const rs_rsa_key *rsa);
rs_err js5_login_success(rs_session *s, rs_buf *dst, int rights, int flagged);
rs_err js5_login_success_parse(rs_session *s, rs_buf *src, int *status, int *rights, int *flagged);
rs_err js5_read_game(rs_session *s, rs_buf *src, int *opcode, rs_buf *payload);
rs_err js5_write_game(rs_session *s, rs_buf *dst, int opcode, const uint8_t *payload, size_t n);
rs_err js5_update_parse(rs_session *s, const uint8_t *data, size_t len, rs_update_req *out, size_t *consumed);
rs_err js5_update_write(rs_session *s, rs_buf *dst, const rs_update_req *req);
rs_err js5_update_reply(rs_session *s, rs_buf *dst, const rs_update_req *req, const uint8_t *data, size_t len);

#endif
