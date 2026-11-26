#ifndef RSLIB_PACKETS_IMPL_H
#define RSLIB_PACKETS_IMPL_H

#include "internal.h"

typedef struct {
    rs_pkt pkt;
    int op;
} rs_pkt_bind_row;

void rs_pkt_table_load(int revision, const rs_pkt_bind_row *rows, int n, rs_pkt_dir dir);
void rs_packets_client_register(void);
void rs_packets_server_register(void);

void rs_packets_client_115(void);
void rs_packets_client_159(void);
void rs_packets_client_177(void);
void rs_packets_client_254(void);
void rs_packets_client_317(void);
void rs_packets_client_377(void);
void rs_packets_client_468(void);
void rs_packets_client_474(void);
void rs_packets_client_508(void);
void rs_packets_client_530(void);
void rs_packets_client_614(void);
void rs_packets_client_667(void);
void rs_packets_client_742(void);
void rs_packets_client_745(void);
void rs_packets_client_790(void);
void rs_packets_client_876(void);
void rs_packets_client_887(void);
void rs_packets_client_902(void);
void rs_packets_client_910(void);

void rs_packets_server_115(void);
void rs_packets_server_159(void);
void rs_packets_server_177(void);
void rs_packets_server_254(void);
void rs_packets_server_282(void);
void rs_packets_server_317(void);
void rs_packets_server_350(void);
void rs_packets_server_377(void);
void rs_packets_server_443(void);
void rs_packets_server_468(void);
void rs_packets_server_474(void);
void rs_packets_server_481(void);
void rs_packets_server_508(void);
void rs_packets_server_525(void);
void rs_packets_server_530(void);
void rs_packets_server_614(void);
void rs_packets_server_667(void);
void rs_packets_server_742(void);
void rs_packets_server_745(void);
void rs_packets_server_790(void);
void rs_packets_server_876(void);
void rs_packets_server_887(void);
void rs_packets_server_902(void);
void rs_packets_server_910(void);

#endif
