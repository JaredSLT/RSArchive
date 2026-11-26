#include "both/packets_impl.h"

void rs_packets_client_register(void) {
    rs_packets_client_115();
    rs_packets_client_159();
    rs_packets_client_177();
    rs_packets_client_254();
    rs_packets_client_317();
    rs_packets_client_377();
    rs_packets_client_468();
    rs_packets_client_474();
    rs_packets_client_508();
    rs_packets_client_530();
    rs_packets_client_614();
    rs_packets_client_667();
    rs_packets_client_742();
    rs_packets_client_745();
    rs_packets_client_790();
    rs_packets_client_876();
    rs_packets_client_887();
    rs_packets_client_902();
    rs_packets_client_910();
}

rs_err rs_client_send_pkt(rs_client *c, rs_pkt id, const void *payload, size_t n) {
    rs_buf tmp;
    rs_err e;
    rs_session *s;
    if (!c) {
        return RS_ERR_ARG;
    }
    s = rs_client_session(c);
    e = rs_buf_alloc(&tmp, n + 8);
    if (e != RS_OK) {
        return e;
    }
    e = rs_session_write_pkt(s, &tmp, id, payload, n);
    if (e == RS_OK) {
        e = rs_client_send(c, tmp.data, tmp.pos);
    }
    rs_buf_free(&tmp);
    return e;
}
