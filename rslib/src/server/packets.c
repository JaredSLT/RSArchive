#include "both/packets_impl.h"

void rs_packets_server_register(void) {
    rs_packets_server_115();
    rs_packets_server_159();
    rs_packets_server_177();
    rs_packets_server_254();
    rs_packets_server_282();
    rs_packets_server_317();
    rs_packets_server_350();
    rs_packets_server_377();
    rs_packets_server_443();
    rs_packets_server_468();
    rs_packets_server_474();
    rs_packets_server_481();
    rs_packets_server_508();
    rs_packets_server_525();
    rs_packets_server_530();
    rs_packets_server_614();
    rs_packets_server_667();
    rs_packets_server_742();
    rs_packets_server_745();
    rs_packets_server_790();
    rs_packets_server_876();
    rs_packets_server_887();
    rs_packets_server_902();
    rs_packets_server_910();
}

rs_err rs_conn_send_pkt(rs_conn *c, rs_pkt id, const void *payload, size_t n) {
    rs_buf tmp;
    rs_err e;
    rs_session *s;
    if (!c) {
        return RS_ERR_ARG;
    }
    s = rs_conn_session(c);
    e = rs_buf_alloc(&tmp, n + 8);
    if (e != RS_OK) {
        return e;
    }
    e = rs_session_write_pkt(s, &tmp, id, payload, n);
    if (e == RS_OK) {
        e = rs_conn_send(c, tmp.data, tmp.pos);
    }
    rs_buf_free(&tmp);
    return e;
}
