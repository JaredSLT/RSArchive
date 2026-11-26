#include "both/packets_impl.h"

static const rs_pkt_bind_row k_s2c[] = {
    {RS_S2C_MESSAGE, 8},
    {RS_S2C_LOGOUT, 9},
    {RS_S2C_PLAYER_UPDATE, 191},
    {RS_S2C_NPC_UPDATE, 79},
    {RS_S2C_INVENTORY, 53},
    {RS_S2C_REGION, 25},
};

void rs_packets_server_177(void) {
    rs_pkt_table_load(177, k_s2c, RS_NELEM(k_s2c), RS_DIR_S2C);
}
